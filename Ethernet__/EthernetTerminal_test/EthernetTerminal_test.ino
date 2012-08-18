#include <SPI.h>
#include <EEPROM.h>
#include "Ethernet.h"

#include "Monitor.h"

Monitor mon(Serial);
EthernetClient client;
boolean clientIsConnected;

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(1234);

char buf[128];
char * bufp;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(19200);
  mon << "Ethernet terminal test @ port 1234. " << endl;
  //   while (!Serial) {
  //    ; // wait for serial port to connect. Needed for Leonardo only
  //  }

  SPI.begin();
  // start the Ethernet connection and the server:
  init_Ethernet();
  //
  server.begin();
  
  bufp = buf;
  *bufp = 0;

  mon << "server is at " << Ethernet.localIP() << endl;
  clientIsConnected = false;
}


void loop() {
  // listen for incoming clients
  char c;

  if ( !client ) {
    if ( client = server.available() ) {
      clientIsConnected = true;
      mon << "new client opened." << endl;
    }
  } 
  else if ( client ) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    if (client.connected()) {
      if (client.available()) {
        Monitor cmon(client);
        if ( cmon.readLine(bufp) ) {
          if ( strcmp("MAC", buf) == 0 ) {
            cmon << ">> " << "MAC ";
            cmon.printHex(mac, 6);
            cmon << endl;
            mon << "MAC command. " << endl;
            //
            bufp = buf;
            *bufp = 0;
          } 
          else 
            if ( strcmp("TIME", buf) == 0 ) {
            cmon << ">> " << "TIME " << millis() << endl;
            mon << "TIME command. " << endl;
            //
            bufp = buf;
            *bufp = 0;
          }
          else 
            if ( strcmp("QUIT", buf) == 0 ) {
            cmon << ">> " << "bye bye." << endl;
            delay(5);
            client.stop();
            mon << "disonnected client." << endl;
             //
            bufp = buf;
            *bufp = 0;
          } else {
            cmon << ">>" << buf << endl;
             //
            bufp = buf;
            *bufp = 0;
          }
        }
      }
    }
    // close the connection:
  }
}

void init_Ethernet() {
  if ( EEPROM.read(0) == 0x9e && EEPROM.read(1) ) {
    for(int i = 0; i < 6; i++) {
      mac[i] = EEPROM.read(2+i);
    }
  }
  IPAddress ip(192,168,1, 177);
  IPAddress gateway(192,168,1, 1);
  IPAddress subnet(255, 255, 0, 0);
  Ethernet.begin(mac, ip, gateway, subnet);
}





