/*
 * HTTPClient
 * HTTPClient.c
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
#include <avr/pgmspace.h>

#include "HTTPClient.h"

//the prototypes for the different requests
prog_char GET_REQUEST[] = "GET %s HTTP/1.1\nAccept: */*\n";
//TODO is that correct ?
prog_char POST_REQUEST[] = "POST %s HTTP/1.1\nAccept: */*\n";

//helper function to ignore the HTTP Result Header
FILE* skipHeader(FILE* stream);


HTTPClient::HTTPClient(uint8_t* ip, uint16_t port) :
  Client(ip, port)
{
  //anyting else to do?
}

FILE*
HTTPClient::getURI(char* uri)
{
  FILE* result = openClientFile();
  fprintf_P(result,uri);
  result = skipHeader(result);
  //read for the big time
  return result;
}

FILE*
HTTPClient::openClientFile()
{
  FILE* result = fdevopen(&clientWrite, &clientRead);
  if (result==NULL) {
      return NULL;
  }
  fdev_set_udata(result,this);
  connect();
  return result;
}

int
HTTPClient::clientWrite(char byte, FILE* stream)
{
  HTTPClient* client = (HTTPClient*) stream->udata;
  client->write(byte);
  return 0;
}

int
HTTPClient::clientRead(FILE* stream)
{
  HTTPClient* client = (HTTPClient*) stream->udata;
  //block until we got a byte
  while (!client->available())
    {
      //do nothing
    };
  return client->read();
}

FILE* skipHeader(FILE* stream) {
  //skip over the header
    int inByte = 0;
    int lastByte = 0;
    while (!(inByte=='\n' && lastByte=='\n')) {
       lastByte=inByte;
       inByte = fgetc(stream);
       if (inByte == EOF) {
           //hmm, an error occured - lets just end this
           HTTPClient::closeStream(stream);
           return NULL;
       }
    }
    return stream;
}

void HTTPClient::closeStream(FILE* stream) {
  HTTPClient* client = (HTTPClient*) stream->udata;
  client->stop();
  fclose(stream);
}
