/*
 Name:		CM_SS_client.ino
 Created:	10/2/2016 1:32:00 AM
 Author:	Pranav
*/


#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <EEPROM.h>
#define DEBUG 1
#define NUM_MAX_CLIENT 4 
#define CLIENT_PORT 4444	  // port to send UDP packets to clients
#define SERVER_PORT 8888      // local port to listen for UDP packets
#define SEPARATORbeg "."
#define SEPARATORend ">"
#define MYNAME ".channel1>" 

/*
format for chat string
.name>message
*/
const char* serverSSID = "myServer";			//  your network SSID (name)
const char* password = "passphrase";       // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
IPAddress serverIP;
unsigned long int TIME=0;
byte unconnected=1;

void printWifiStatus() {
	// print the SSID of the network you're attached to:
	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("\n\nIP Address: ");
	Serial.println(ip);
	Serial.print("gateway IP Address: ");
	Serial.println(WiFi.gatewayIP());
}

void sendMessage(String data, IPAddress sendIP) {
	Udp.beginPacket(sendIP, SERVER_PORT);
	Udp.write(data.c_str());
	Udp.endPacket();
}

#define SIGN 40
void debuggerInit(){
  
  //error checking and self diagnosis code do not alter
  EEPROM.begin(64);
  if (EEPROM.read(SIGN) != 0x22){
    EEPROM.write(SIGN, 0x22);
    EEPROM.write(SIGN + 1, 40);
    EEPROM.commit();
    EEPROM.end();
  }
  else{
    byte status = 0;
    status = EEPROM.read(SIGN + 1);
    EEPROM.write(SIGN + 1, status - 1);
    EEPROM.commit();
    EEPROM.end();
    if (status == 0){ Serial.println("\nError Code 1"); while (1); }
  }
}

void setup(){

	Serial.setDebugOutput(true);
	Serial.begin(115200);
 debuggerInit();
	Serial.print("Udp server started at port ");
	Serial.println(CLIENT_PORT);
	Serial.print("begin =");
	Serial.println(Udp.begin(CLIENT_PORT));


	// Open serial communications and wait for port to open:
	
	WiFi.mode(WIFI_STA);
	WiFi.begin(serverSSID, password);
 int tries=0;
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
    Serial.print(".");
    tries++;
    if (tries > 30){
      break;
    }
  }
  printWifiStatus();
  serverIP=WiFi.gatewayIP();
TIME=millis();
}
void checkConnection(){
  WiFi.begin(serverSSID, password);
 int tries=0;
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
    Serial.print(".");
    tries++;
    if (tries > 30){
      break;
    }
  }
  if(WiFi.status() != WL_CONNECTED){unconnected=1;}
  else{unconnected=0;}
  printWifiStatus();
  serverIP=WiFi.gatewayIP();
  TIME=millis();
  }

void loop(){
  if(WiFi.status() != WL_CONNECTED){unconnected=1;}
  if(millis()>TIME+10000 && unconnected){
    TIME=millis();
    checkConnection();
    }
	char inBuffer[128] = {}; //buffer to hold incoming and outgoing packets
	String outBuffer = MYNAME;
	if (Serial.available()){
		//parse the serial input to send
		outBuffer = outBuffer + Serial.readString();
		sendMessage(outBuffer, serverIP);
	}


	int packetSize = Udp.parsePacket();
	if (packetSize) {
		int len = Udp.read(inBuffer,packetSize);
		if (len > 0) { inBuffer[len] = 0; }
		Serial.println(inBuffer);
	}

}

