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
#include <HardwareSerial.h>

#include "HTTPClient.h"

//helper function to ignore the HTTP Result Header
FILE* skipHeader(FILE* stream);


HTTPClient::HTTPClient(char*host, uint8_t* ip, uint16_t port) :
  Client(ip, port)
{
  this->hostName=host;
}

FILE*
HTTPClient::getURI(char* uri)
{
  return getURI(uri,NULL);
}

FILE*
HTTPClient::getURI(char* uri,char* headers)
{
  FILE* result = openClientFile();
  //the request and the default headers
  fprintf_P(result,PSTR("GET %s HTTP/1.1\nHost %s\nAccept: */*\n"),uri,hostName);
  //is there an additional header?
  if (headers!=NULL) {
      fprintf(result,headers);
  }
  //ok finished
  fprintf_P(result,"\n");
  result = skipHeader(result);
  return result;
}

FILE*
HTTPClient::postURI(char* uri)
{
  FILE* result = openClientFile();
  fprintf_P(result,PSTR("POST %s HTTP/1.1\nHost %s\nAccept: */*\n\n"),uri,hostName);
  result = skipHeader(result);
  //TODO where and how to write the parameters
  return result;
}

FILE*
HTTPClient::openClientFile()
{
  FILE* result = fdevopen(clientWrite, clientRead);
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
  HTTPClient* client = (HTTPClient*) fdev_get_udata(stream);
  client->write(byte);
	Serial.print(byte);
  return 0;
}

int
HTTPClient::clientRead(FILE* stream)
{
  HTTPClient* client = (HTTPClient*) fdev_get_udata(stream);
  //block until we got a byte
  while (client->available()==0)
    {
      //do nothing
    };
  return client->read();
}

FILE* skipHeader(FILE* stream) {
  //skip over the header
    static int inByte = 0;
    static int lastByte = 0;
    while (!(inByte=='\n' && lastByte=='\n')) {
		//by that we ignore \r
		if (inByte!='\r') {
       lastByte=inByte;
		}
       inByte = fgetc(stream);
		if (inByte!='\n') {
		Serial.print((char)inByte);
			Serial.print(inByte);
		} else {
			Serial.print(inByte);
			Serial.println('.');
		}
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
