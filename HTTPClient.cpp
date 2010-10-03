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
#include <string.h>
#include <avr/pgmspace.h>
#include <HardwareSerial.h>
#include "HTTPClient.h"

//some HTTP helpers
char
sendUriAndHeaders(FILE* stream, char* hostName, char* uri, uri_parameter parameters[], char* headers);
char
sendContentPayload(FILE* stream, char* data);
char
skipHeader(FILE* stream);

//a struct to store the uriEncoder & the handle to the http client
typedef struct
{
  HTTPClient* client;
  uint8_t encode;
#define URI_ENCODE _BV(0)
#define URI_ENCODE_RESERVED _BV(1)
} http_stream_udata;

//we need this probably more than once o it is defined here
#define URI_ALLOWED(byte) ((byte>='A' && byte<='Z') || (byte>='a' && byte<='z') || (byte>='0' && byte<='9') || byte == '-' || byte == '_' || byte == '.' || byte == '~')
#define URI_RESERVED(byte) (byte == '!' || byte == '*' || byte == '\'' || byte == '(' || byte == ')' || byte == ';' || byte == ':' || byte == '&' || byte == '=' || byte == '+' || byte == '$' || byte == ',' || byte == '/' || byte == '?' || byte == '#' || byte == '[' || byte == ']')

HTTPClient::HTTPClient(char*host, uint8_t* ip, uint16_t port) :
  Client(ip, port)
{
  this->hostName = host;
}

FILE*
HTTPClient::getURI(char* uri)
{
  return getURI(uri, NULL, NULL);
}

FILE*
HTTPClient::getURI(char* uri, uri_parameter parameters[])
{
  return getURI(uri, parameters, NULL);
}

FILE*
HTTPClient::getURI(char* uri, uri_parameter parameters[], char* headers)
{
  FILE* result = openClientFile();
  //the request and the default headers
  fprintf_P(result, PSTR("GET "));
  sendUriAndHeaders(result, this->hostName, uri, parameters, headers);
  //ok finished
  fprintf_P(result, PSTR("\n"));
  skipHeader(result);

  return result;
}

FILE*
HTTPClient::postURI(char* uri, char* data)
{
  return postURI(uri, NULL, data, NULL);
}
FILE*
HTTPClient::postURI(char* uri, uri_parameter parameters[], char* data)
{
  return postURI(uri, parameters, data, NULL);
}

FILE*
HTTPClient::postURI(char* uri, uri_parameter parameters[], char* data, char* headers)
{
  FILE* result = openClientFile();
  fprintf_P(result, PSTR("POST "));
  sendUriAndHeaders(result, this->hostName, uri, parameters, headers);
  sendContentPayload(result, data);
  skipHeader(result);
  return result;
}

void HTTPClient::setEncoding(FILE* stream, char encode, char encodeReserved) {
  http_stream_udata* udata = (http_stream_udata*) fdev_get_udata(stream);
  if (encode==0) {
      udata->encode=0;
  } else {
      if (encodeReserved) {
          udata->encode = URI_ENCODE | URI_ENCODE_RESERVED;
      } else {
          udata->encode = URI_ENCODE;
      }
  }
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
  fdev_set_udata(result,udata);
  udata->client = this;
  udata->encode = 0;
  connect();
  return result;
}

char
sendUriAndHeaders(FILE* stream, char* hostName, char* uri, uri_parameter parameters[], char* headers)
{
  //encode but use reserved characters
  HTTPClient::setEncoding(stream, 1, 0);
  fprintf_P(stream, PSTR("%s"), uri);
  if (parameters!=NULL) {
      fprintf_P(stream, PSTR("?"));
      char parameter_number = 0;
      uri_parameter* parameter = &parameters[0];
      while (parameter->name!=NULL) {
          if (parameter_number>0) {
              fprintf_P(stream, PSTR("&"));
          }
          HTTPClient::setEncoding(stream, 1, 1);
          fprintf_P(stream, PSTR("%s"), parameter->name);
          HTTPClient::setEncoding(stream, 1, 0);
          fprintf_P(stream, PSTR("="));
          HTTPClient::setEncoding(stream, 1, 1);
          if (parameter->value!=NULL) {
              fprintf_P(stream, PSTR("%s"), parameter->value);
          }
          HTTPClient::setEncoding(stream, 1, 0);
          parameter_number++;
          parameter = &parameters[parameter_number];
      }
  }
  HTTPClient::setEncoding(stream, 0, 0);
  fprintf_P(stream, PSTR(" HTTP/1.1\nHost: %s\nAccept: */*\n"), hostName);
  //is there an additional header?
  if (headers != NULL)
    {
      fprintf(stream, PSTR("%s"), headers);
    }
  return 0;
}

char
sendContentPayload(FILE* stream, char* data)
{
  //calculate the content length
  int content_length = 0;
  if (data != NULL)
    {
      content_length = strlen(data);
    }
  fprintf_P(stream, PSTR("Content-Length: %i"), content_length);
  //ok finished header
  fprintf_P(stream, PSTR("\n"));
  //now sending data
  if (content_length > 0)
    {
      fprintf_P(stream, PSTR("%s"), data);
    }
  return 0;
}

int
HTTPClient::clientWrite(char byte, FILE* stream)
{
  http_stream_udata* udata = (http_stream_udata*) fdev_get_udata(stream);
  HTTPClient* client = udata->client;
  if (udata->encode==0) {
      client->write(byte);
      Serial.print(byte);
  } else {
      if (URI_ALLOWED(byte) || ((URI_RESERVED(byte) && (udata->encode & URI_ENCODE_RESERVED)==0)))
        {
          client->write(byte);
          Serial.print(byte);
        }
      else
        {
          char encoded[4] =
            { 0, 0, 0 };
          sprintf(encoded, "%%%2x", byte);
          for (char i = 0; i < 4; i++)
            {
              client->write(encoded[i]);
              Serial.print(encoded[i]);
            }
        }
  }
  return 0;
}

int
HTTPClient::clientRead(FILE* stream)
{
  http_stream_udata* udata = (http_stream_udata*) fdev_get_udata(stream);
  HTTPClient* client = udata->client;
  //block until we got a byte
  while (client->available() == 0)
    {
      //do nothing
    };
  int result = client->read();
  if (result == EOF)
    {
      return EOF;
    }
  //as long as we do not read encoded or it is no % everything is ok
  else if (udata->encode==0 || result != '%') {
      return result;
  } else {
          char return_value = 0;
          for (char i = 0; i < 2; i++)
            {
              result = client->read();
              if (result == EOF)
                {
                  return EOF;
                }
              else if (result >= 'A' && result <= 'Z')
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
  http_stream_udata* udata = (http_stream_udata*) fdev_get_udata(stream);
  udata->client->stop();
  free(udata);
  fclose(stream);
}
