# HTTPClient for Arduino (Enhanced)

## Not about this version:

This is an enhanced version of the HTTPClient library originally from Interactive-Matter
(https://github.com/interactive-matter/HTTPClient). Additionally changes from Salanki's 
fork (https://github.com/salanki/HTTPClient) were incorporated.

The main differences are:
* The used communication client instance can be specified with new constructors. With 
  this it can be used with nearly any kind of Client-inherited class (e.g. EthernetClient, 
  WifiClient, GSMCLient and many more). The old constructor methods without providing a 
  Client instance are still available and an EthernetClient instance is created then. So 
  this version should be a drop-in replacement for the original "HTTPClient"
* Additionally some new constructors were introduced that allow the connection to be 
  done by just providing the hostname and DHCP is used then. To prevent DNS lookup and 
  such simply use the original constructor versions where IP is provided additionally 
  to Hostname - then the IP is used for the connection without the need of a lookup.
* My optimizations that are in Pull-request #15 on original library are incorporated as 
  well:
  * make sure connection is correctly closed at end of communication so that reuse of 
    one Client instance is possible
  * check success of parsing for last HTTP returncode and only set the variable on success
  
### Open Todos
* add examples for new Client-Feature ...

## Overview

An Arduino HTTP Client that uses the Arduino Ethernet Library to make HTTP requests
and handle responses.

## Usage
  
### Creating a HTTP Client

HTTP Client works with the Arduino Ethernet Library. Before you can create
a HTTP client you must initialize your ethernet connection with something 
like:

```c++
Ethernet.begin(mac, ip);
```

For details on this see http://arduino.cc/en/Reference/ServerBegin

To create a client (in this example for pachube) you can simply call one of 
the constructors:

```c++
//  The address of the server you want to connect to (pachube.com):
byte server[] = { 173,203,98,29 }; 
HTTPClient client("api.pachube.com",server);
```

which is equivalent to

```c++
HTTPClient client("api.pachube.com",server,80);
```

Now you are ready to go.

### Making a request

HTTP client supports three types of requests:

1. `GET` requests to get some data from a URL
2. `POST` requests to transfer a larger amount of data to a server
3. `PUT` requests as sepcified by REST APIs

`DELETE` requests have not yet been implemented.

All request take a number of parameters (depending on the request type):

* The URI - a string (char*) containing the uri - which is normally everything 
  following the hostname of a URL for http://arduino.cc/en/Reference/HomePage
  it would be Reference/HomePage
* Optional parameters as key value pairs. Parameters are appended to a URL like
  http://myhost/the/uri?parameter-name=parameter-value&other=parameter
  parameters are values of the struct http_client_parameter. It is easiest to
  do this like:

```c++
http_client_parameter parameters[] = {
  { "key","afad32216dd2aa83c768ce51eef041d69a90a6737b2187dada3bb301e4c48841" }
  ,{ NULL,NULL }
};
```

* For POST and PUT request a string with additional data can be passed as a string. The data
  has to be in memory. Future Versions may have future features.
* For all requests additional headers can be specified. It works exactly the same was as uri
  parameters:

```c++
http_client_parameter pachube_api_header[] = {
  { "X-PachubeApiKey","afad32216dd2aa83c768ce51eef041d69a90a6737b2187dada3bb301e4c48841" }
  ,{ NULL,NULL }
};
```

Even though the HTTPClient supports HTTP 1.1 request no keep alive requests are supported currently.

### Handling a response

The result code of a HTTP request can be read with `getLastReturnCode()`. It returns a 
integer containing the return code. 200 indicates that everything was ok.
For further details refer to http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html

The result of a request is a stream (aka `FILE*`) by that you can read the data
without the need to keep the whole answer in memory - which would fail for most
HTML pages.

`FILE*` streams are a bit more unusual the normal Arduino streams. They have been 
choosen since you can use all the nice fprintff and fscanf routines of avr-libc.

After reading the response from the stream it has to be closed with the method:

```c++
closeStream(stream)
```

**DO NOT FORGET IT**. Each stream has some data attached and if you forget to close the 
stream you get a memory leak, slowly filling up the precious memory of the Arduino.

### Debug mode

The HTTPClient has also a debug mode which can be switched on and off by using `debug()`
with parameter 0 as no debug and anything else Ð e.g. -1 Ð as enabling debug output.
By default the debug code is disabled. If debug is enabled the complete request and 
response is printed out on the serial connection. Very useful if your request does
not work.

## Contributions

* Thanks to [colagrosso](http://github.com/colagrosso) for fixing the URL encoding
* Thanks to [hex705](http://github.com/hex705) for properly porting it to Arduino 1.0

## Copyright and license

HTTPClient is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

HTTPClient is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with HTTPClient.  If not, see http://www.gnu.org/licenses/.