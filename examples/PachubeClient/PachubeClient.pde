/*
  Pachube sensor client
 
 This sketch connects an analog sensor to Pachube (http://www.pachube.com)
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 
 Circuit:
 * Analog sensor attached to analog in 0
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 15 March 2010
 updated 4 Sep 2010
 by Tom Igoe
 
 http://www.tigoe.net/pcomp/code/category/arduinowiring/873
 This code is in the public domain.
 
 */

#include "WiFly.h"
#include "Credentials.h"

#include <HTTPClient.h>
#include <stdio.h>
#include <Metro.h>
char buffer[40];

Metro sendingMetro = Metro(60000L);


#define SHARE_FEED_URI      "/v2/feeds/10564.csv" 


long lastConnectionTime = 0;        // last time you connected to the server, in milliseconds
boolean lastConnected = false;      // state of the connection last time through the main loop
const int postingInterval = 10000;  //delay between updates to Pachube.com

void setup() {
  Serial.begin(9600);
  Serial.println("Pachube Client");
  // start the ethernet connection and serial port:
  WiFly.begin();
  
  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }
  Serial.println("ready");
}

void loop() {
  // read the analog sensor:
  int sensorReading = analogRead(A0);   


  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(sendingMetro.check() && !client.connected()) {
    Serial.print("sending ");
    Serial.println(sensorReading);

    sprintf(buffer, "%d,%d", sensorReading, sensorReading);

    HTTPClient client("api.pachube.com");
    //client.debug(-1);
    http_client_parameter pachube_api_header[] = {
      { 
        "X-PachubeApiKey","afad32216dd2aa83c768ce51eef041d69a90a6737b2187dada3bb301e4c48841"      }
      ,{
        NULL,NULL      }
    };
    FILE* result = client.putURI(SHARE_FEED_URI,NULL,buffer, pachube_api_header);
    int returnCode = client.getLastReturnCode();
    if (result!=NULL) {
      client.closeStream(result);
    } 
    else {
      Serial.println("failed to connect");
    }
    if (returnCode==200) {
      Serial.println("data uploaded");
    } 
    else {
      Serial.print("ERROR: Server returned ");
      Serial.println(returnCode);
    }
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}






