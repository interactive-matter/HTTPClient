/*
  Pachube sensor client using HTTPClient Library rom Interactive Matter
  
 http://interactive-matter.eu/how-to/arduino-http-client-library/
 
 
 This version for Arduino 1.0  // http://www.arduino.cc
 In addition to HTTPClient, this code uses metro library
 
 
 Original code from example by Tom Igoe 
 Modified by Steve Daniels (www.spinningtheweb.org), Jan 2012
 

 
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

#include <SPI.h>
#include <Ethernet.h>
#include <HTTPClient.h>

#include <Metro.h> // http://www.arduino.cc/playground/Code/Metro  (link checked Jan, 2012)


//  Ethernet Variables
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };  // MAC address for the ethernet controller.
byte server[] = { 173,203,98,29 };                     // address of the server you want to connect to (pachube.com):
EthernetClient client;


// Pachube Variables
#define PACHUBE_API_KEY     "YOUR_PACHUBE_API_KEY_GOES_HERE--KEEP_THE_QUOTES" 
#define FEED_URI            "/v2/feeds/PUT_YOUR_FEED_NUMBER_HERE.csv" // eg  "/v2/feeds/19321.csv"

// initialize the Metro library instance:
Metro sendingMetro = Metro(15000L);  
boolean lastConnected = false;       // state of the connection last time through the main loop

// buffer to store send string
char data[40];


void setup() {
 
  // start the ethernet connection and serial port:
  Ethernet.begin(mac);
  Serial.begin(9600);
  
  // give the ethernet module time to boot up:
  delay(1000);
  
  Serial.println("Pachube Push-Client using HTTPClient Library");
  Serial.println("ready");
}

void loop() {
  // read the analog sensor:
  int sensorReading = analogRead(A0);  

  // if you're not connected, and Metro has expired then connect again and send data:
  if(sendingMetro.check() && !client.connected() ) {

    Serial.print("sending ");
    Serial.println(sensorReading);
  
                        // Pachube API_v2 STREAMS are updated with the following pattern:
                        // <streamID>,<value>                                                             (for a single STREAM within a FEED) 
                        // <streamID>,<value> NEWLINE <streamID>,<value> ... NEWLINE <streamID>,<value>   (for multiple STREAM within a FEED)
                        
    // use this for a single STREAM
    sprintf(data, "%d,%d ", 0, sensorReading);  // updates 1 STREAM: Stream 0 set to sensorReading 
    
    // uncomment below to try updating two streams 
    // sprintf(data, "%d,%d \n %d,%d", 0, sensorReading, 1 , 333); // updates 2 STREAMs: STREAM 0 set to sensorReading, STREAM 1 set to 333
    
    Serial.println(data);
    
    // create HTTPClient
    HTTPClient client( "api.pachube.com", server );
    
    http_client_parameter pachube_APIHeader[] = {
     
          { "X-PachubeApiKey", PACHUBE_API_KEY  }
          ,
          { NULL, NULL }
      };
      
    // FILE is the return STREAM type of the HTTPClient
    FILE* result = client.putURI( FEED_URI, NULL, data, pachube_APIHeader );
  
    int returnCode = client.getLastReturnCode();
    
    if (result!=NULL) {
      client.closeStream(result);  // this is very important -- be sure to close the STREAM
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


