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
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <HardwareSerial.h>
#include "HTTPClient.h"

//some HTTP helpers
char sendUriAndHeaders(FILE* stream, char* hostName, char* uri, char* headers);
char sendContentPayload(FILE* stream, char* data);
char skipHeader(FILE* stream);
//uri encoding functions
FILE* createUriEncodeStream(FILE* stream);
FILE* getUriEncodedStream(FILE* stream);
int uriEncodedWrite(char byte, FILE* stream);
int uriEncodedRead(FILE* stream);
int uriEncodedStringLength(char* string);

//a struct to store the uriEncoder & the handle to the http client
typedef struct
{
  HTTPClient* client;
  FILE* encoderStream;
} http_stream_udata;

HTTPClient::HTTPClient(char*host, uint8_t* ip, uint16_t port) :
  Client(ip, port)
{
  this->hostName = host;
}

FILE*
HTTPClient::getURI(char* uri)
{
  return getURI(uri, NULL);
}

FILE*
HTTPClient::getURI(char* uri, char* headers)
{
  FILE* result = openClientFile();
  //the request and the default headers
  fprintf_P(result, PSTR("GET "));
  sendUriAndHeaders(result, this->hostName, uri, headers);
  //ok finished
  fprintf_P(result, PSTR("\n"));
  skipHeader(result);

  return result;
}

FILE*
HTTPClient::postURI(char* uri, char* data)
{
  return postURI(uri, data, NULL);
}

FILE*
HTTPClient::postURI(char* uri, char* data, char* headers)
{
  FILE* result = openClientFile();
  fprintf_P(result, PSTR("POST "));
  sendUriAndHeaders(result, this->hostName, uri, headers);
  sendContentPayload(result,data);
  skipHeader(result);
  return result;
}

FILE*
HTTPClient::openClientFile()
{
  FILE* result = fdevopen(clientWrite, clientRead);
  if (result == NULL)
    {
      return NULL;
    }
  http_stream_udata* udata = (http_stream_udata*) malloc(
      sizeof(http_stream_udata));
  udata->client = this;
  udata->encoderStream = createUriEncodeStream(result);
  fdev_set_udata(result,udata);
  connect();
  return result;
}

char sendUriAndHeaders(FILE* stream, char* hostName, char* uri, char* headers) {
  fprintf_P(getUriEncodedStream(stream), PSTR("%s"), uri);
  fprintf_P(stream,PSTR("HTTP/1.1\nHost: %s\nAccept: */*\n"),
      hostName);
  //is there an additional header?
  if (headers != NULL)
    {
      fprintf(stream, PSTR("%s"), headers);
    }
  return 0;
}

char sendContentPayload(FILE* stream, char* data) {
  //calculate the content length
   int content_length = 0;
    if (data != NULL)
    {
      content_length = uriEncodedStringLength(data);
    }
  fprintf_P(stream, PSTR("Content-Length: %i"), content_length);
  //ok finished header
  fprintf_P(stream, PSTR("\n"));
  //now sending data
  if (content_length > 0)
    {
      fprintf_P(getUriEncodedStream(stream), PSTR("%s"),data);
    }
}

int HTTPClient::clientWrite(char byte, FILE* stream)
{
  http_stream_udata* udata = (http_stream_udata*) fdev_get_udata(stream);
  HTTPClient* client = udata->client;
  client->write(byte);
  Serial.print(byte);
  return 0;
}

int HTTPClient::clientRead(FILE* stream)
{
  http_stream_udata* udata = (http_stream_udata*) fdev_get_udata(stream);
  HTTPClient* client = udata->client;
  //block until we got a byte
  while (client->available() == 0)
    {
      //do nothing
    };
  return client->read();
}

char
skipHeader(FILE* stream)
{
  //skip over the header
  static int inByte = 0;
  static int lastByte = 0;
  while (!(inByte == '\n' && lastByte == '\n'))
    {
      //by that we ignore \r
      if (inByte != '\r')
        {
          lastByte = inByte;
        }
      inByte = fgetc(stream);
      Serial.print((char) inByte);
      if (inByte == EOF)
        {
          //hmm, an error occured - lets just end this
          HTTPClient::closeStream(stream);
          return NULL;
        }
    }
  return 0;
}

void
HTTPClient::closeStream(FILE* stream)
{
  HTTPClient* client = (HTTPClient*) stream->udata;
  client->stop();
  fclose(stream);
}

//we need this more than once o it is defined here
#define URI_ALLOWED(byte) ((byte>='A' && byte<='Z') || (byte>='a' && byte<='z') || (byte>='0' && byte<='9') || byte == '-' || byte == '_' || byte == '.' || byte == '~')
//TODO - no close - so an memory leak - but how to close it??
FILE* createUriEncodeStream(FILE* stream)
{
  FILE* encodedStream = fdevopen(uriEncodedWrite, uriEncodedRead);
  fdev_set_udata(encodedStream,stream);
  return encodedStream;
}

int uriEncodedWrite(char byte, FILE* stream)
{
  FILE* origStream = (FILE*) fdev_get_udata(stream);
  //is is and allowed char?
  if URI_ALLOWED(byte)
    {
      return origStream->put(byte,origStream);
    }
  else
    {
      char encoded[4] =
        { 0,0,0};
      sprintf(encoded,"%%%02xc",byte);
      for (char i =0; i<4; i++)
        {
          int result = origStream->put(encoded[i],origStream);
          if (result==EOF)
            {
              return result;
            }
        }
    }
  return 0;
}

int uriEncodedRead(FILE* stream)
{
  FILE* origStream = (FILE*) fdev_get_udata(stream);
  int result = origStream->get(stream);
  if (result == EOF)
    {
      return EOF;
    }
  if (result != '%')
    {
      return result;
    }
  else
    {
      char return_value = 0;
      for (char i; i < 2; i++)
        {
          result = origStream->get(stream);
          if (result == EOF)
            {
              return EOF;
            }
          if (result >= 'A' && result <= 'Z')
            {
              return_value += (1 - i) * 16 * (result - 'A');
            }
          else if (result >= 'a' && result <= 'z')
            {
              return_value += (1 - i) * 16 * (result - 'a');
            }
          else if (result >= '0' && result <= '9')
            {
              return_value += (1 - i) * 16 * (result - '0');
            }
        }
      return return_value;
    }
}

int uriEncodedStringLength(char* string)
{
  if (string==NULL) {
      return 0;
  }
  int length = 0;
  char* current = string;
  while (*current!=0) {
      if URI_ALLOWED(*current) {
          //this will not be percentage encoded
          length++;
      } else {
          //percentage encoding takes 3 bytes
          length +=3;
      }
      current++;
  }
  return length;
}

FILE* getUriEncodedStream(FILE *stream) {
  http_stream_udata* udata = (http_stream_udata*) fdev_get_udata(stream);
  return udata->encoderStream;
}
