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
#include <stdio.h>
#include "Client.h"

class HTTPClient: private Client {
private:
  FILE* openClientFile();
  static int clientWrite(char byte, FILE* stream);
  static int clientRead(FILE* stream);
	uint8_t * host_ip
	char* host_name;
public:
  HTTPClient(char* host, uint16_t port);
  FILE* getURI(char* uri);
  FILE* postURI(char* uri, FILE* content);
  static void closeStream(FILE* stream);
};


#endif /* HTTPCLIENT_H_ */
