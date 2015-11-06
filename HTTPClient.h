/*
 * HTTPClient
 * HTTPClient.h
 *
 *  http://interactive-matter.org/
 *
 *  This file is part of HTTPClient.
 *
 *  HTTPClient is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  HTTPClient is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with HTTPClient.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Created on: 18.09.2010
 *      Author: Marcus Nowotny of Interactive Matter
 */

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <inttypes.h>
#include <Client.h>

/* With this we try a delay of 20 ms before checking again if content is available
 * and we try this up to 50 times.
 * So max: 20x50ms=1000ms=1s
 */
#define HTTPCLIENT_HEADER_READ_DELAY_ONCE 20
#define HTTPCLIENT_HEADER_READ_DELAY_MAXCOUNT 50

/* This struct is used to pass parameters as URI paramters and additional HTTP headers.
 * normally you pass this as a array. The last entry must have the NULL-Pointer as name.
 */
typedef struct
{
  char* name;
  char* value;
} http_client_parameter;

/*
 * The HTTP client is basically a Ethernet client with some additional functions. The functions
 * of the Ethernet client are not accessible.
 * To construct a HTTP client you have to provide the IP AND the name of the server Ð else
 * the virtual host management of most internet servers will fail. Sorry for the inconvenience.
 */
class HTTPClient
{
public:
  /*
   * create a HTTP client that connects to the default port 80 using hostname (and dhcp lookup).
   * An internal EthernetClient instance is used for the communication.
   */
  HTTPClient(char* host);
  /*
   * create a HTTP client that connects to another port using hostname (and dhcp lookup). HTTPS is not supported.
   * An internal EthernetClient instance is used for the communication.
   */
  HTTPClient(char* host, uint16_t port);
  /*
   * create a HTTP Ethernet client that connects to the default port 80 using provided ip. hostname is used in HTTP message.
   * An internal EthernetClient instance is used for the communication.
   */
  HTTPClient(char* host, uint8_t* ip );
  /*
   * create a HTTP Ethernet client that connects to another port using provided ip. hostname is used in HTTP message. HTTPS is not supported.
   * An internal EthernetClient instance is used for the communication.
   */
  HTTPClient(char* host, uint8_t* ip, uint16_t port);

  /*
   * create a HTTP client that connects to the default port 80 using hostname (and dhcp lookup).
   * The given instance of a Client (e.g. EthernetClient, WifiClient or such) is used for communication.
   */
  HTTPClient(Client& client, char* host);
  /*
   * create a HTTP client that connects to another port using hostname (and dhcp lookup). HTTPS is not supported.
   * The given instance of a Client (e.g. EthernetClient, WifiClient or such) is used for communication.
   */
  HTTPClient(Client& client, char* host, uint16_t port);
  /*
   * create a HTTP client that connects to the default port 80  using provided ip. hostname is used in HTTP message..
   * The given instance of a Client (e.g. EthernetClient, WifiClient or such) is used for communication.
   */
  HTTPClient(Client& client, char* host, uint8_t* ip );
  /*
   * create a HTTP client that connects to another port using provided ip. hostname is used in HTTP message. HTTPS is not supported.
   * The given instance of a Client (e.g. EthernetClient, WifiClient or such) is used for communication.
   */
  HTTPClient(Client& client, char* host, uint8_t* ip, uint16_t port);

  /*
   * the destructor
   */
  ~HTTPClient();

  /*
   * Post a GET request to the server.
   * The result is a file handle or null is an error occured.
   */
  FILE*
  getURI(char* uri);
  /*
   * Post a GET request to the server and give additional URI parameters like ?X=Y&...
   * The URI Parameters should be given as array with {NULL,NULL} as marker for the
   * last element.
   * The result is a file handle or null is an error occured.
   */
  FILE*
  getURI(char* uri, http_client_parameter parameters[]);
  /*
   * Post a GET request to the server, give additional URI parameters ?X=Y&...
   * and give additional HTTP header parameters (e.g. for some API key).
   * The URI Parameters should be given as array with {NULL,NULL} as marker for the
   * last element.
   * The result is a file handle or null is an error occured.
   */
  FILE*
  getURI(char* uri, http_client_parameter parameters[],
      http_client_parameter headers[]);
  FILE*
  /*
   * Post a POST request to the server.
   * The data is directly streamed to the server. It must be kept completely in
   * memory since we must know how much data to send.
   * The result is a file handle or null is an error occured.
   */
  postURI(char* uri, char* data);
  /*
   * Post a POST request to the server and give additional URI parameters like ?X=Y&...
   * The URI Parameters should be given as array with {NULL,NULL} as marker for the
   * last element.
   * The data is directly streamed to the server. It must be kept completely in
   * memory since we must know how much data to send.
   * The result is a file handle or null is an error occured.
   */
  FILE*
  postURI(char* uri, http_client_parameter parameters[], char* data);
  /*
   * Post a POST request to the server, give additional URI parameters ?X=Y&...
   * and give additional HTTP header parameters (e.g. for some API key).
   * The URI Parameters should be given as array with {NULL,NULL} as marker for the
   * last element.
   * The data is directly streamed to the server. It must be kept completely in
   * memory since we must know how much data to send.
   * The result is a file handle or null is an error occured.
   */
  FILE*
  postURI(char* uri, http_client_parameter parameters[], char* data,
      http_client_parameter headers[]);
  /*
   * Post a REST PUT request to the server.
   * The data is directly streamed to the server. It must be kept completely in
   * memory since we must know how much data to send.
   * The result is a file handle or null is an error occured.
   */
  FILE*
  putURI(char* uri, char* data);
  /*
   * Post a REST PUT request to the server and give additional URI parameters like ?X=Y&...
   * The URI Parameters should be given as array with {NULL,NULL} as marker for the
   * last element.
   * The data is directly streamed to the server. It must be kept completely in
   * memory since we must know how much data to send.
   * The result is a file handle or null is an error occured.
   */
  FILE*
  putURI(char* uri, http_client_parameter parameters[], char* data);
  /*
   * Post a REST PUT request to the server, give additional URI parameters ?X=Y&...
   * and give additional HTTP header parameters (e.g. for some API key).
   * The URI Parameters should be given as array with {NULL,NULL} as marker for the
   * last element.
   * The data is directly streamed to the server. It must be kept completely in
   * memory since we must know how much data to send.
   * The result is a file handle or null is an error occured.
   */
  FILE*
  putURI(char* uri, http_client_parameter parameters[], char* data,
      http_client_parameter headers[]);

  /*
   * Retrieve the HTTP return code of the last request. 200 indicates that everything was ok.
   * For further details refer to http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
   */
  int
  getLastReturnCode(void);
  /*
   * enable or disable the debug code.
   * By default the debug code is disabled. If debug is enabled the complete request and response
   * is printed out on the serial connection. Very useful if your request do not work.
   */
  void
  debug(char debugOn);

  static void
  closeStream(FILE* stream);
/*
 * And that is the internal stuff of the HTTP client
 */
private:
  //client-instance to use
  Client* _theClient;
  //flag if Client was self-generated by compatibility constructors and needs to be removed
  bool _selfInitializedTheClient=false;

  //the name of the host we are talking to
  char* hostName;
  //the ip of the host
  uint8_t* ip;
  //use ip instead of hostname
  bool _useIp;
  //the port we are talking to
  uint16_t port;  
  //the HTTP return code of the last request
  int lastReturnCode;
  //print to serial?
  char debugCommunication;

  //opening the client stream
  FILE*
  openClientFile();
  //the rw routines
  static int
  clientWrite(char byte, FILE* stream);
  static int
  clientRead(FILE* stream);
  //a helper to throw away the HTTP client
  int
  skipHeader(FILE* stream);
  //a way to specify if and which URI encoding to use - encodeReserved also encodes characters
  // like / or ? for URI parameters & stuff
  static void
  setEncoding(FILE* stream, char encode, char encodeReserved);
  //some HTTP helpers
  char
  sendUriAndHeaders(FILE* stream, char* hostName, const char* requestType, char* uri,
      http_client_parameter parameters[], http_client_parameter headers[]);
  char
  sendContentPayload(FILE* stream, char* data);
};

#endif /* HTTPCLIENT_H_ */
