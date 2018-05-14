.PHONY:test docs

all:test docs bin/libernet bin/liberstuff

start:bin/libernet
	bin/libernet

tool:bin/liberstuff

-include bin/libernet.d
-include bin/liberstuff.d

# TODO: Add dependency on headers
bin/libernet:libernet.cpp
	@mkdir -p bin
	@clang++ $< -o $@ -I.. -lsqlite3 -lz -Wall -Weffc++ -Wextra -Wshadow -Wwrite-strings -MMD -MP

bin/liberstuff:liberstuff.cpp
	@mkdir -p bin
	@clang++ $< -o $@ -I.. -lz -Wall -Weffc++ -Wextra -Wshadow -Wwrite-strings -MMD -MP

documentation/index.html: Makefile
	@mkdir -p documentation
	@doxygen libernet.dox 2> bin/logs/doxygen.txt
	@if [ `cat bin/logs/doxygen.txt | wc -l` -ne "0" ]; then echo `cat bin/logs/doxygen.txt | wc -l` documentation messages; fi

docs:documentation/index.html

test:bin/test Makefile
	@bin/test $(OS_OPTIONS)

../os/tests/test.cpp:
	@git clone http://github.com/marcpage/os ../os
	@git clone http://github.com/marcpage/protocol ../protocol

 ../os/*.h:../os/tests/test.cpp
 ../protocol/*.h:../os/tests/test.cpp

bin/test:../os/tests/test.cpp ../protocol/*.h ../os/*.h *.h Makefile
	@mkdir -p bin
	@clang++ ../os/tests/test.cpp -o $@ -I.. -lsqlite3 -Wall -Weffc++ -Wextra -Wshadow -Wwrite-strings
