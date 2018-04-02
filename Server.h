#ifndef __Server_h__
#define __Server_h__

#include "os/Queue.h"
#include "os/Thread.h"
#include "os/SocketServer.h"
#include "os/AddressIPv4.h"
#include "os/AddressIPv6.h"
#include "os/BufferManaged.h"
#include "os/BufferString.h"
#include "libernet/Storage.h"
#include <vector>

namespace server {

	class HTTPHandler : public exec::Thread {
		public:
			HTTPHandler(store::Storage &store);
			virtual ~HTTPHandler();
			bool handle(net::Socket *connection);
		protected:
			virtual void *run();
		private:
			store::Storage				&_store;
			bool						_working;
			exec::Queue<net::Socket*>	_queue;
			http::Request &_readRequest(net::Socket &connection);
			std::string &_readLine(net::Socket &connection, std::string &buffer);

	};

	class HTTP : public exec::Thread {
		public:
			HTTP(int port, store::Storage &storage);
			virtual ~HTTP();
		protected:
			virtual void *run();
		private:
			typedef std::vector<HTTPHandler*>	_HandlerList;
			store::Storage	&_store;
			int				_port;
			_HandlerList	_handlers;

	};

	inline HTTPHandler::HTTPHandler(store::Storage &store):exec::Thread(KeepAroundAfterFinish),_store(store),_working(false),_queue() {
		start();
	}
	inline HTTPHandler::~HTTPHandler() {
	}
	inline bool HTTPHandler::handle(net::Socket *connection) {
		if (_queue.size() == 0) {
			_queue.enqueue(conection);
			return true;
		}
		return false;
	}
	inline void *HTTPHandler::run() {
		while (true) {
			_working= false;
			net::Socket		*next= _queue.dequeue();
			try {
				_working= true;
				http::Request		request= _readRequest(*next);
				http::Response		response;
				std::string			buffer;
				const std::string	hash= "/sha256/";
				std::string			responseData;

				responseData= std::string("<html><head><title>404 Path not found</title></head><body><h1>404 Path not found</h1></br><pre>") + request + "</pre></body></html>\n";
				response.info().code()= "404";
				response.info().message()= "Not Found";
				response.fields()["Content-Type"]= "text/html; charset=utf-8";
				if (request.info().path().find(hash) == 0) {
					std::string::size_type nextSlash= request.info().path().find('/', hash.length());
					std::string name= request.info().path().substr(hash.length(), nextSlash);

					if (_store.has(name)) {
						std::string content= _store.get(name);
						response.info().code()= "200";
						response.info().message()= "OK";
						responseData= content;
					} else {
						responseData= std::string("<html><head><title>504 Resource Not Local</title></head><body><h1>504 Resource Not Local</h1></br><pre>") + request + "</pre></body></html>\n";
						response.info().code()= "504";
						response.info().message()= "Gateway Timeout: Not Local: " + name;
					}
				}
				buffer= response;
				next->write(BufferString(buffer), buffer.size());
				next->write(BufferString(responseData), responseData.size());
				connection->close();
				delete connection;
			} catch(const std::exception &exception) {
				// TODO: log
			}
		}
	}
	inline http::Request &HTTPHandler::_readRequest(net::Socket &connection) {
		std::string headers, line;

		while ( (_readLine(connection, line) != "\r\n") && (line != "\n") ) {
			headers+= line;
		}
		return http::Request(headers);
	}
	inline std::string &HTTPHandler::_readLine(net::Socket &connection, std::string &buffer) {
			char			byte= '\0';
			BufferAddress	singleByte(&byte, 1);

			buffer.clear();
			while (byte != '\n') {
				connection.read(singleByte, 1);
				buffer += byte;
			}
			return buffer;
	}

	inline HTTP::HTTP(int port, store::Storage &storage):exec::Thread(KeepAroundAfterFinish),_store(store),_port(port),_handlers() {
		start();
	}
	inline HTTP::~HTTP() {
		for (_HandlerList::iterator i= _handlers.begin(); i != _handlers.end(); ++i) {
			delete *i;
		}
	}
	inline void *HTTP::run() {
		net::AddressIPv4	address(_port);
		net::SocketServer	server(address.family());

		server.resuseAddress();
		server.reusePort();
		server.bind(address);
		server.listen(10/* backlog */);
		while (true) {
			net::AddressIPv6	connectedTo;
			net::Socket			*connection= new net::Socket();
			HTTPHandler			*found= NULL;

			server.accept(connectedTo, *connection);
			for (_HandlerList::iterator i= _handlers.begin(); i != _handlers.end(); ++i) {
				if ( (*i)->handle(connection) ) {
					found= *i;
					break;
				}
			}
			if (NULL == found) {
				found= new HTTPHandler(_store);
				found->handle(connection);
				_handlers.push_back(found);
			}
		}
	}

}


#endif // __Server_h__
