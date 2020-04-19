
Table of Contents
=================

* [Summary](#summary)
* [Definitions](#definitions)
* [Concepts](#concepts)
  * [Data Identity](#data-identity)
  * [Data Routing](#data-routing)
  * [Deleting Data](#deleting-data)
  * [Data Matching](#data-matching)
    * [Similar to results](#similar-to-results)
    * [Cost to match digits](#cost-to-match-digits)
* [Server](#server)
  * [HTTP server](#http-server)
  * [Data transfer](#data-transfer)
* [Data Types](#data-types)
  * [Small file](#small-file)
  * [Large file](#large-file)
  * [Bundle Description](#bundle-description)
  * [Address History](#address-history)
    * [Resolving web addresses](#resolving-web-addresses)
  * [Personal Key](#personal-key)
    * [Personal Information](#personal-information)
  * [Messages](#messages)
    * [Message Dictionary](#message-dictionary)
    * [Carrier Dictionary](#carrier-dictionary)
  * [Trust](#trust)
    * [Direct Trust](#direct-trust)
    * [Domain Trust](#domain-trust)
    * [Indirect Trust](#indirect-trust)
    * [Trust Document](#trust-document)
  * [Server Information](#server-information)
  * [Requests](#requests)
  * [Karma](#karma)
    * [Reach](#reach)
    * [Choosing a transaction block to add your transaction](#choosing-a-transaction-block-to-add-your-transaction)
    * [Transaction Block](#transaction-block)



# Summary

Libernet is a free, open, and secure platform for communicating and sharing data.
It is designed to provide a rich internet experience while allowing you to maintain control over your data.
All data is securely encrypted and distributed across the Libernet.
While data loss is possible, only data that no one is requesting would be lost.
Similar to Bittorrent, data that is more popular is distributed to more nodes, making data loss less likely and data transfer faster.

Every person and computer (node) on the Libernet has at least one unique identity.
This identity allows them to share data, receive messages, and participate in a network of trust.

The web, or traditional internet, experience acts like a giant wiki where anyone can contribute, add, remove, or modify anything.
However, all revisions are kept and only revisions that are validated by trusted people will be shown by default.
Websites can be full HTML5 apps, allowing powerful applications to be developed.

Messages are similar to email with three major differences.
The first difference being that the only thing that is publicly visible is the recipient and date sent.
The second difference is that senders can be verified as being a specific identity and, using the trust network, determine how likely an identity is valued.
The third difference is that there is a cost to each message sent.

Senders can spend CPU time as "postage" on messages to send a message first class, or spend less CPU time and send it bulk rate.
This makes it more costly to send bulk messages and makes it easier to identify them.
If it takes 30 seconds to prep a message for three people, that is not a big deal.
But for a company to send out a message to 3,000 people, it would take 8 hours of intense compute power.
Companies would screen their mailing lists to only those who would truly be interested in their message.
Sending bulk messages is no longer free.


# Definitions

* *Hash* - SHA256 is used.
* *Encrypt* - Typically refers to AES256 encryption using the contents Hash (SHA256) as the key.
* *Public/Private Key* - An RSA public/private key pair
* *Public-key encrypted* - RSA public key used to encrypt data so only the paired RSA private key can decrypt.
* *Signing* - RSA private encryption of SHA256 hashing of the data, stored as hex.
* *Compression* - zlib compression, level 9
* *Dates* - All dates and times are in GMT, even YYYY/MM/DD


# Concepts


## Data Identity

Data is identified by the hash of the contents.
Any data block can, therefore, be validated that the identity is correct by hashing the data block and verifying it matches the stated identity.


## Data Routing

When new data is created, it is pushed (at the least) to the connected node whose identity most closely matches the identity of the data.
When data is pushed to a node, it must either be stored, or pushed to another node.
A node may not delete data (except for the cases in [Deleting Data](#deleting-data)) without first passing the data on to another node.
When data is passed on before deleting, it is passed to the node that most closely matches the identity of the data.
When a node originates data, it may want to push the data to more than one node, to ensure the data is seeded properly, before deleting locally.


## Deleting Data

Certain data can be squelched (not passed on and deleted from local cache).
Each node keeps a list of identifiers that are squelched (deleted).
When data of a squelched identifier is pushed to the node, it can immediately delete it.
When responding to ["similar to" search](#data-matching), squelched data should be excluded.

Reasons for squelching data include:
* Old, fully Merged [Address History](#address-history) (assuming no history is lost)
* Old [Personal Information](#personal-information) (either through owner updates or through merging verifiers)
* Old [Trust](#trust) lists (determine by timestamp)
* [Merged Karma blocks](#choosing-a-transaction-block-to-add-your-transaction)


## Data Matching

Data can be matched to other data by adding random data until the first hex digits of the hashes match.
Data can be requested by exact hash, or by "similar to" search.
The more digits that match, the more time (in general) and expense and, therefore, the higher the priority or value of the match.
When searching "similar to" each node may drop lower priority items if the number of items being returned is "large".
Items are not dropped just because they are low priority, but because there are "too many" items to return, in which case lower priority items may be omitted.


### Similar to results

When "similar to" search is done, it returns the json below.
The json may be compressed if it reduces the size.
```
{
	{similar to identifier}: [{similar identiiers}]
}
```
Requests may be repeated as when a request is received, those requests are passed on to nodes likely to have that information.


### Cost to match digits

Below are the times to calculate a hash of sequential integer strings until it matches the hash of an empty string as well as a string of the letters a, b, c, and d.
Absolute times are somewhat irrelevant and more to give scale.
Different processors will drastically change the absolute times.
Times are provided to give relative speed of matching the number of digits.

Numbers in the table are not exact, but generally the magnitude.
Theoretical assumes that 2^(digits x 4) unique patterns will be required to ensure that many hex digits will match.
That is not necessarily the case (see hex digits = 7).

Hex Digits | Low Time   | High Time | Low Tries   | High Tries  | Category      | Theoretical Tries | Theoretical Time
---------- | ---------- | --------- | ----------- | ----------- | ------------- | ----------------- | ----------------
4          | 0.01       | 0.10      | 5,000       | 7,000       | bulk          | 65,536            | 0.01 seconds
5          | 0.31       | 3.32      | 200,688     | 2,148,794   | standard      | 1,048,576         | 0.18 seconds
6          | 10.9       | 24.0      | 7,156,184   | 15,579,804  | priority      | 16,777,220        | 2.8 seconds
7          | 11.8       | 1,449     | 70,179,856  | 938,827,609 | urgent        | 268,435,500       | 45 seconds
8          | 300        | 300       | 188,138,959 | 188,138,959 | urgent ...    | 4,294,967,000     | 12 minutes


# Server


## HTTP server

Data is transfered via an http server.
Some paths may only be available when connecting to localhost, others are available publicly.

Path           | Always Public | Description
------         | ------------- | -----------
/              | No            | Index app
/app           | No            | Configuration app. Chooses and configures apps, which are [bundles](#bundle-description) mapped to root paths.
/data          | Yes           | Data is stored here at the address /data/sha256/{hash} and bundles can be accessed via /data/sha256/{hash}/aes256/{key}/relative/path/file.html. PUT is supported on /data/sha256/{hash}
/data/like     | Yes           | ["similar to" search](#data-matching) /data/like/sha256/{hash} will return a list of hashes similar to {hash}
/data/requests | Yes           | [Requests](#requests) that this node has. Supports PUT for connecting node's requests.
/mail          | No            | [Messages](#messages) app
/server        | Yes           | Returns server information. Supports PUT for connecting node's information.
/web           | No            | See [Address History](#address-history). Appending the address with  an empty query (?) will return information about how the bundle was chosen and other options as well as gives you opportunities to download and change trust in identity.
/{app}         | No            | Apps installed by /app


## Data transfer

Servers communicate via http.
Server to server communication is done via /data, /data/like, /data/requests, and /server.


# Data Types

Type                                          | Encrypted | Contents | [Match](#data-matching)
--------------------------------------------- | --------- | -------- | -----------------------
[Small File](#small-file)                     | Yes       | binary   | None
[Large File](#large-file)                     | Yes       | json     | None
[Bundle](#bundle-description)                 | Yes       | json     | None
[Address History](#address-history)           | No        | json     | web:{lowercase/path/to/bundle}
[Personal Key](#personal-key)                 | No        | PEM      | None
[Personal Information](#personal-information) | No        | json     | info:{personal key identifier}
[Message Envelope](#message-dictionary)       | Yes       | json     | None
[Message Carrier](#carrier-dictionary)        | No        | json     | message:{YYYY:MM:DD(GMT):recipient personal key identifier}
[Trust Document](#trust-document)             | No        | json     | trust:{trust owner's personal key identifier}
[Similar to results](#similar-to-results)     | No        | json     | None
[Requests](#requests)                         | No        | json     | None

All data should be compressed, before encryption, if compression reduces the size of the original data.
Data should be no larger than 1 MiB after compression.
Data should be no larger than 1 MiB before compression unless there is not a mechanism to work around this limit.


## Small file

A small file is a file that is 1 MiB or smaller.
The data is hashed to generate the encryption/decryption key.
Then the data is compressed if compression can reduce the size.
The key previously generated from the hash of the contents is used to encrypt the data.
The data is then hashed to generate the identifier of the data.

When data is received, it is received with the [identifier](#data-identity).
If the recipient is intended to access the contents the key is also passed with the identifier.
Data can be requested by the exact identifier.
The data is then passed from node to node without being able to read the contents.

When data is received and we have the decryption key, we can get the original data.
The data is first verified that it matches the [identifier](#data-identity).
We then decrypt the data using the shared decryption key.
If the decryption key matches the hash of the decrypted contents, then we're done.
Otherwise, the data is decompressed.
If the data does not match the decrypted contents, then we cannot use the data is it was a bad key or there is a hash collision on the original data.


## Large file

Large files are any file that is larger than 1 MiB.
The file is broken up into up to 1 MiB chunks with each chunk being treated as a [Small file](#small-file).
Maximum number of chunks is at least 6,095 as large file description is also limited to 1 MiB in size.
This means the file limit is no lower than 6 GiB.
The file description is a json array of chunk identifiers.
The first element of the array describes the overall file contents.
```
	[
		{
			'size': data size
		},
		{
			'sha256': data identifier,
			'aes256': decryption key,
			'size': data size
		}
	]
```
This json is treated like a [small file](#small-file).


## Bundle Description

A bundle description describes a bundle of files.
Each entry is around 512 bytes, so limit of files in a bundle is roughly no less than 2,000 files.
A bundle description is a json dictionary of relative paths within the bundle to information about each file.
The *previous* field lists bundles this bundle is based on.
When merging bundles, place all bundle identifiers that were merged into this bundle in the *previous* list.
```
{
	"contents": {
		"index.html": {
			"sha256": identifier of Small or Large file,
			"aes256": key to decrypt Large file description or small file,
			"size": number of bytes in the final file,
			"Content-Type": mime type for the data},
		"images/logo.png": {
			"sha256": identifier of Small or Large file,
			"aes256": key to decrypt Large file description or small file,
			"size": number of bytes in the final file,
			"Content-Type": mime type for the data},
	},
	"previous": [list of bundle identifiers this bundle is based on]
}
```
The *size* fields can be used to help determine if the file is stored as a [small file](#small-file) or a [large file](#large-file).
When restoring, files larger than 1 MiB are assumed to be a [large file](#large-file), but if it doesn't result in [large file](#large-file) json, then try again treating it as a [small file](#small-file).
It may be that the file compressed down below 1 MiB.
If it is smaller tha 1 MiB, it should be a [small file](#small-file).


## Address History

Address history describes the most recent versions of a [bundle](#bundle-description), each of which link to previous [bundles](#bundle-description) for a web address.
Each entry is at least 330 bytes, histories can be up to 3,000 entries.
To get the size down to 1 MiB uncompressed, entries may be removed.
Care should be taken to have every signer represented in the *heads* list.
If every head cannot be represented (as it would increase the size of the history beyond 1 MiB), then older entries which show up in [bundle](#bundle-description) *previous* chains me be removed.
Address histories are not encrypted, but may be compressed if compression improves size.
The identifier of an address history is a hash of compressed contents (or uncompressed if uncompressed is smaller).
Unlike a [small file](#small-file), address history is not encrypted.

The *heads* list should be ordered from most recent change to oldest change.

Address history is requested for a particular address via [Data Matching](#data-matching).
The path to the contents referenced by the history is first converted to lowercase.
Then any backslashes (\\) are converted to slashes (/).
Any slash prefixes or suffixes are removed and then whitespace is removed from the ends.
The path is then prefixed with "web:".
```\Testing\the\Path\To Enlightenment -> web:testing/the/path/to enlightenment```
This string is then hashed.
This is the hash we will [match](#data-matching) when adding padding.

Address history is requested using ["similar to" search](#data-matching).
Many versions are likely to be returned.
All versions returned are evaluated.
A list of [bundles](#bundle-description) is built up and the address history that lists all recent [bundles](#bundle-description) is chosen.
If there is not one address history that contains all recent [bundles](#bundle-description), then a new address history is created and published containing all recent bundles.
Differences can also occur from divergent signed dictionaries.
Signed dictionaries can be merged adding all signers of each version bundle.
Any address histories that do not add any new head [bundles](#bundle-description) or signers may be [deleted](#deleting-data).

When choosing which head to use, your [trust](#trust) network is used to determine most trusted head.

```
{
	"heads": [
		{
			"sha256": identifier of bundle,
			"aes256": key to decrypt bundle,
			"timestamp": seconds since epoch,
			"signed": {identifier of public key: signature of bundle identifier},
		},
	],
	"padding": random number to get hash to fit the right pattern,
}
```


### Resolving web addresses

When a web address is received, it is really the address of a history address and a relative path in the bundle.
Since there is not a clear distinction of where the relative path starts, there will need to be some searches.
These searches start by looking for the bundle at the root (hash of "web:").
If the relative path does not exist in the trusted root bundle then add the first path element (ie web:apple.com, web:company, web:country, etc).
To prevent link-hijacking and cut down on requests needed to resolve paths, bundles should be stored in the first two path items.


## Personal Key

A personal key is a PEM encoded public key that is stored without encryption (similar to [Address History](#address-history)) but with possible compression if it reduces the size.
The [identity](#data-identity) of the personal key is the identifier for a person or node.


### Personal Information

Personal information about a personal key is stored in a json dictionary.
This is also stored without encryption but can be compressed to reduce size.
The padding is added to [match](#data-matching) the personal key [identity](#data-identity) with "info:personal key identifier".

Personal information is requested using ["similar to" search](#data-matching).
When multiple are found, the correctly signed one with the latest timestamp is used.
Older personal information should be [deleted](#deleting-data) if all or most verifiers have verified the new data and *next* field has not changed.
Multiple versions of the same timestamp can exist with different sets of verifiers.
The personal information that contains all verifiers of the latest personal information should be kept and others should be [deleted](#deleting-data).
If there is not a personal information that contains all verifiers, you may merge verifiers and republish the personal information.
If you do create a new personal information, you should [delete](#deleting-data) all other personal information.
You do not have to verify the person to merge verifiers.

Verifiers are people who can vouch for the personal information.
They may have had communication with this identity, or may be the next, or backup, identity.
They may even provide a services where they privately receive information (like scans of drivers licenses, passport, etc)
 from the identity and confirm they are who they say they are.

Personal information fields are mostly optional.
Nickname is required as it is the display name.
The *credentials* field may not be a good idea as the personal information may not be ideal to share.
The *credentials* and *verifiers* fields are two ways to help people know that this person is who they say they are.
Information that you may not want to share are email, apartment, street number (or even street or city), first name, last name.
Whenever personal information is updated, verifiers should be [messaged](#messages) to notify them of the change to give them an opportunity to verify the updated information.

Whenever personal information is created, a backup (*next*) personal key should be created.
The next field is a backup key.
In the event that your private key for the personal key is leaked, you can set the *valid* field to false.
Anything signed after the timestamp when the personal information is marked *valid=false* should be considered not signed.
[Trust](#trust) trees should be updated with this timestamp and anything after this timestamp should not be trusted and treated as malevolent.

If *valid=false* then the *next* value is considered an alias for valid signatures after timestamp.
The *next* fields should be generated and its private key should be put in cold storage and not used until *valid=false*.
The *next* should not be trusted in the same personal information in which *valid=false*.
Previous personal information should be sought out to see other personal keys for *next*.
The oldest *next* with personal information should be used.
When personal information is invalidated via *valid=false*, the preferred (oldest viable) *next* should be activated by generating personal information for that identity.


```
{
	"nickname": short name or handle for person,
	"image": identifier for a small file or large file jpg image,
	"first name": name,
	"last name": name,
	"domain": a domain name that http serves a file named libernet.trust at the root,
	"email": email,
	"website": address of a website,
	"twitter": twitter handle,
	"facebook": facebook url,
	"youtube": youtube url,
	"credentials": [
		{"sha256": identifier of image of some identification credentials,"aes256": key to decrypt credential image}
	],
	"country": country,
	"state": state,
	"province": province,
	"timestamp": seconds since the epoch used to determine latest profile,
	"city": city,
	"street": street,
	"street number": house number,
	"apartment": apartment number,
	"valid": true or false if false then this key cannot be used,
	"next": personal information identifier for a backup identity
}
```
The above dictionary is stored in the following wrapper
```
{
	"identity": above dictionary in a string,
	"signature": signature data of identity string from signer,
	"signer": hash of public key,
	"padding": random data to get hash of data to match digits with public key hash,
	"verifiers": {identifier of public key: signature of identity}
}
````


## Messages

Messages are json files that describe from, to, cc, subject, and [bundle](#bundle-description) of message body.
Messages are composed of two separate data blocks, the envelope and the carrier.
The envelope is treated as a [small file](#small-file) and contains sender's signature, routing and general message headers.
The carrier is stored unencrypted, but may be compressed.
Messages are sent by [matching](#data-matching) the carrier with the hash of "message:YYYY:MM:DD(GMT):public key identifier" in all lowercase.

Messages are requested using ["similar to" search](#data-matching).
They are requested for specific dates (in GMT).

The encryption key to decrypt the message is encrypted using the recipients public key.


### Message Dictionary
```
{
	"from": identifier of personal key of sender,
	"to": [list of identifiers of personal keys],
	"cc": [list of identifiers of personal keys],
	"subject": subject of the message,
	"date": YYYY/MMM/DD HH:MM:SS in GMT,
	"reply": identifier of the message being replied to,
	"sha256": the identifier of the body Bundle,
	"aes256": the key to decrypt the body Bundle,
}
```
The above dictionary is encoded into the following Envelope Dictionary
```
{
	"message": the above dictionary,
	"signature": signature of message from signer,
	"signer": hash of public key,
}
````


### Carrier Dictionary
```
{
	"sha256": the identifier of the message,
	"key": {"aes256": the public-key encrypted key to decrypt the message}
	"padding": random data to get hash of data to match digits with public key hash,
}
```


## Trust

Each person collects trust into a trust list.
Trust lists are shared by [matching](#data-matching) to: "trust:public key identifier".
Trust lists are compressed (if it makes it smaller) but are not encrypted.
The shared trust list does not represent the trust tree, just the first-order, direct trusts.
When using trust lists, the most recent trust list for each trusted identity is loaded.
This can be done recursively (loading more and more trust lists) depending on the trust of each identity.

If an identity is found in your first-order, direct trust list, that is the trust value assigned to an identity.
If an identity is not found in your first-order, direct trust list, then the domain trust is checked.
If the identity is not in the domain trust list, then second-order trust lists are used (then third-order, and so on).

Temporary trust can be given to "try out" something, to see if it is what is advertised and be able to mark as trusted.
For websites and apps (special websites) this would allow downloading the bundle and being able to manually inspect it before trusting it.

For web display, a node can specify whitelist, blacklist, or graylist mode.
Whitelist mode only allows trusted (positive score) identities.
Blacklist mode only disallows untrusted (negative score) identities.
Graylist mode allows whitelist and any other identity that is not blacklisted (unknowns).

For [messages](#messages), senders are evaluated via this trust method.
Trusted senders go to the inbox.
Graylist senders (unable to determine a trust score) to to the spam folder.
Blacklist senders go to the trash.


### Direct Trust
Is it in the top level trust (we trusted or distrusted the person explicitly):

Field      | Points Scaler
---------- | -------------
trusted    | x +10
mistaken   | x -1
disagree   | x -5
malevolent | x -100


### Domain Trust

If there is no first-order, direct trust information about an identity and there is a domain context available, we check for domain trust.
If the [address](#address-history) starts with a domain name, for example apple.com, check the domain for identities.
An http request on the root of the domain for /libernet.trust file.
The libernet.trust text file just contains a list of the [identity](#data-identity) of the [personal keys](#personal-key) that are trusted by the domain, with one identity per line.
For [messages](#messages), if the [personal information](#personal-information) contains a domain, this will be used.

Domain trust may be moved below second-order, indirect trust to handle websites that have objectionable content.


### Indirect Trust

If the identity is not found in the first-order, direct trust list and not found in domain trust, we use indirect trust.
We walk through all first level identities whose score is greater than zero and ask:
* Is it in their trust list?
* Add up the scores (some positive, some negative) from each second-order, trusted identity.

If no second-order, indirect trust can be established, we move on to third-order, then fourth-order, and so on.
For the first level identities whose score is greater than zero but does not list the identity, walk through those looking for scores greater than zero that list the identity.

We go through each level until we find a level that contains the identity for which we are seeking trust information.
If we don't find any trust information, we then look at the identity's trust list and see how much we agree with his trusted identities.


### Trust Document

The trust document is [matched](#data-matching) with "trust:trust owner's public key identifier"
```
{
	"timestamp": seconds since epoch,
	"trust": {
		public key identifier: {
			"trusted": count of times content has been marked as trusted,
			"mistaken": count of times content was marked mistaken (honest mistake suspected)
			"disagree": count of times content was marked as disagree (maybe not correct, or maybe correct, but dislike presenation or stance)
			"malevolent": count of times contents was marked as malevolent intent
			"timestamp": optional field that if set marks that trust information is only valid until this timestamp
		}
	}
}
```
The above dictionary is placed in a string in the following wrapper
```
{
	"trust": string of the above dictionary,
	"signature": signature message,
	"signer": hash of public key,
	"padding": random data to get hash of data to match,
}
```

## Server Information

When servers connect they request information about the node they are connecting to.
The information returned is the json below, compressed if it reduces the size.
The *identifier* field is used for determining best nodes to [route data](#data-routing) to.
```
{
	"identifier": hash identifier of the node,
	"name": the name of this node,
	"servers": {
		"identifier": {
			"address": address and port,
			"connections": number of times connected,
			"failed": number of failed connection attempts,
			"time": total time in seconds connected to the server,
			"input": total bytes received from this node,
			"output": total bytes sent to this node,
			"karma": karma received from this node,
		}
	},
}
```


## Requests

A node may ask for requests of another node.
What is returned is the json description below.
```
{
	"sha256": [list of data being searched for],
	"like": {"sha256": [list of data being searched for]}
}
```


## Karma

Karma is a method to keep track of value being added to the network.
Karma is created during the validation of Karma transactions.

Karma transaction blocks are limited to 1 MiB (before compression) or about 1,700 single sender/receiver transactions.
When creating a new transaction, it is either added to an existing block or a new block is created.
You can also merge existing blocks as part of a transaction.

The maximum number of Karma is 100 trillion (100,000,000,000,000).
Karma can also be divided into 100 trillion Kismet.
Each block is awarded 1 Kismet from each sender in the block, all the transaction fees, and the Karma created from the block.
The Karma created for each block is 20 Karma - index x 200 Kismet.
At that rate, the last 200 Kismet of the 100 trillion Karma will be created by the 10 trillionth block.

Each transaction has one or more *from* identities.
Each *from* identity must supply a verification signature for the block te be valid.
Any block missing a verification signature is considered a pending transaction.
Each transaction has one or more *to* identities.
There is no participation needed for *to* identities.
You can have *from* identities that have zero (0) amount added.
These are identities that must sign to make the transaction valid, for instance, escrow identities.

The *pending* transactions are transactions where one or more identities does not have sufficient funds or one or more identities have not signed yet.
To be a *pending* transaction, however, it must contain at least one signature.

Cancelation signatures in the *pending* section allow signatures to be withdrawn.
To cancel a signature, instead of signing the pending transaction, the hash of the pending transaction is signed.
The block that contains a cancellation will be the last block that contains cancellation as well as the signature that is being cancelled.
If all signatures for a pending transaction are cancelled, then the pending transaction is no longer listed in future transactions.


### Reach

All balances and pending transactions are captured in a certain number of recent blocks.
These blocks are referred to as the reach.
In order to find all pending transactions and the balance of any identity, if the reach value is correct, you only need to go back to the reach block.

In order to determine the reach block, start with the previous reach block.
Track every pending transaction and the identity of every balance and see if the show up in a more recent block in the chain (and the pending transactions have not been cancelled).
If they do, you may advance the reach block.

The balances section of each block should be half balances affected from transactions in the current block and half from balances carried forward to increase the reach block.
The goal is to advance the reach block at least one on each transaction.
While this will not always be the case, it does increase the value of the block.

A reach block is a list of transaction blocks in the block chain that contains every balance and every current pending transaction.
The *reach* field tells you the index at which a reach block would end for this transaction block.


### Choosing a transaction block to add your transaction

When creating a new block, or merging with a block, the *previous* block is chosen by ensuring they are validated and then ranked by quality.
When adding a new transaction, you may add a new block with the current *index* but missing a *previous* field.
If you add a block that is missing the *previous* field, your transactions will be merged with other transactions, your block deleted, and you will not be eligible for block ownership.

Failing validation for any reason other than missing *previous* field, should have an increase in [mistaken demerit](#direct-trust) in your trust list.
Shortcuts may be taken by using reach blocks (double, triple, etc), but use at your own [trust](#trust) risk.
Critical validations to perform include:
1. All prior transactions has been validated and all balances are correct.
1. All identity balances are captured in the reach.
1. All pending, uncanceled transactions are in the reach.
1. The block contains at least one transaction.
1. For every new balance from the transactions in this block there is no more than one balance from outside this block to help increment the reach.
1. There are no more pending transactions than there are transactions in this block.
1. The index is an unbroken series of monotonically increasing integers from the prime block (index=0).
1. The size of the block is as close to 1 MiB without going over.
1. The block is not padded (parsing the json, and embedded json, and then generating compact json should result in about the same size).
1. No transaction (completed or pending) has a memo field over 4k.
1. The next *previous* block is validated with the above criteria.

The following criteria should be used to evaluate which *previous* block, among the validated blocks, should be chosen.
When ranking by quality, the first criteria is the most important.
If there is a tie in the first criteria, move on to the second criteria, and so forth.
1. Fully validated. (correct)
1. The signing identity with the largest key size (up to 4096) and the best (reverse) [match](#data-matching) of the block identity. (diversity)
1. The block with the best [match](#data-matching) to the hash of "karma:{block index}". (cost)
1. Most trusted signer. (trust)
1. Closest to matching new balance changes with old balance changes and cancelations. (balance)
1. The reach distance is smallest. (clean)
1. Contains the most completed transactions. (breadth)
1. Fees are the highest. (value)
1. The signer with the highest balance. (stake)

The most important criteria is that the block is correct.
The diversity criteria helps ensure a diverse set of identities are chosen and prevents a few from dominating.
Note that for diversity criteria we use reverse matching to match the end of the identity with the end of the block identity to prevent matching a the known prefix from the index.
Best [match](#data-matching) is the next criteria because it shows dedication (or luck) in getting the block completed, and weeds out a lot of candidates.
Trust is the next criteria because it encourages correctness by punishing those found to have made mistakes.
Balance is to ensure we are not just filling the block with new transactions for the fees.
Reach is the next highest to keep the chain efficient to validate by ensuring that reach minimization is prioritized.
Breadth encourages adding as many transactions as possible to the block.
High fees show urgency and value to the senders.
All else being equal, we want the signers with the most at stake (highest balance).

There probably won't be much competition for best [match](#data-matching) hash until the uncompressed size of the block starts to reach 1 MiB.
As the potential size of the next block reaches 1 MiB, blocks will start to be merged, [deleted](#deleting-data), and submitted at an increased pace.
At that point the race to generate the optimal, chosen *previous* block will begin.
The race ends as the potential size of the next block starts to reach 1 MiB and the chances of being chosen as a previous block diminish.

The goal is to eventually be able to generate up to 15 indexes per second (assuming we have enough transactions per second).
This pace will prevent too much computation cost trying to generate a block to match the index hash.
It should also discourage wasted computation creating new keys to try and reverse match with the block.

You may [delete](#deleting-data) blocks when better (see above criteria) blocks are found and the block to be [deleted](#deleting-data) does not add anything outside of existing blocks of higher value (see above criteria).
You may also [delete](#deleting-data) blocks that are invalid (balances do not [match](#data-matching), missing balances or pending transactions in reach) if all of the block's valid transactions are captured in other blocks.
When validating from the prime block (index=0), you can [delete](#deleting-data) blocks as you see that they are not referenced in future index blocks (their branches die off).

You may take shortcuts in validating the entire history by only validating back two or three reach blocks.
For instance, go back to the index this block says has reach back to.
Take that block and go back to what it says its reach is.
This would be double-reach-back validation.
Validate all transactions since the double-reach-back block.
Triple would look at the double-reach-back block and go back to its reach-back and validate back to those transactions.
Care should be taken when using shortcuts to ensure there is consensus on the blocks near the head of the block chain to ensure someone doesn't fabricate an entire double- or triple-reach-back chain.

When taking shortcuts, note that you do risk your node getting its trust lowered (if you miss something), which could impact the likelihood of your blocks being added to the chain.


### Transaction Block

Transactions show the move of Karma from a set of identities to other identities.
The fee is an optional amount added to the transaction to encourage more people to add the transaction to the block they are validating.
Each transaction is about 400 bytes in size for single sender and single receiver.
```
{
	"from": {sender identity:amount},
	"to": {recipient identity:amount},
	"fee": {sender identity:amount},
	"memo": any comment but limited to 4k,
	"timestamp": seconds since the epoch,
}
```

Transactions are included in a transaction block description.
This description includes:
* The index of the transaction, starting at zero and monotonically increasing.
* The timestamp when the block was created
* The identifier of the previous index transaction
* List of balances which include not only everyone in the transactions but also any identities that are not in the reach.
* List of new, validated transactions.
* List of pending transactions which not only include new pending transactions but also pending transactions not in the reach.
* List of cancelation requests for pending transactions.

The transaction block description is about 200 bytes plus about 600 bytes per single sender/receiver transaction.
```
{
	"index": transaction index starting at zero for prime block,
	"timestamp": seconds since the epoch,
	"previous": identifier for previous index block this is based on,
	"reach": index in which all blocks since then have everyones balance,
	"balances": {
		identity: {
			"balance": balance after this transaction block,
			"last": index of last block with balance,
		}
	}
	"transactions": [
		{
			"transaction": text of transaction above,
			"verification": {identity of sender:signature of transaction}
		},
	],
	"pending": [
		{
			"transaction": text of transaction above,
			"verification": {identity of sender:signature of transaction},
			"cancelations": {identity of the sender: signature of sha256 of transaction}
		},
	]
}
```
Transaction blocks are wrapped in a signing block.
The signing block adds about 200 bytes.
```
{
	"block": text of the above transaction block,
	"padding": padding added to get the hash of the block to match,
	"signer": the identity that signs this block,
	"signature": The signature data of the block,
}
```
