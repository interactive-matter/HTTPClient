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
 *      Author: marcus
 */

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include <inttypes.h>
#include "Client.h"

//a struct to store parameters
typedef struct {
  char* name;
  char* value;
} uri_parameter;
//by default we assume that a parameter struct with NULL as parameter ends the struct

class HTTPClient: private Client {
private:
  char* hostName;
  uint8_t hostIp;
  //opening the client stream
  FILE* openClientFile();
  //the rw routines
  static int clientWrite(char byte, FILE* stream);
  static int clientRead(FILE* stream);

public:
  HTTPClient(char* host, uint8_t* ip, uint16_t port);
  FILE* getURI(char* uri);
  FILE* getURI(char* uri, uri_parameter parameters[]);
  FILE* getURI(char* uri, uri_parameter parameters[], char* headers);
  FILE* postURI(char* uri, char* data);
  FILE* postURI(char* uri, uri_parameter parameters[], char* data);
  FILE* postURI(char* uri, uri_parameter parameters[], char* data, char* headers);
  FILE* putURI(char* uri, char* data);
  FILE* putURI(char* uri, uri_parameter parameters[],char* data);
  FILE* putURI(char* uri, uri_parameter parameters[],char* data, char* headers);

  static void setEncoding(FILE* stream, char encode, char encodeReserved);
  static void closeStream(FILE* stream);
};


#endif /* HTTPCLIENT_H_ */
