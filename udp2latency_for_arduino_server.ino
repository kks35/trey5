//this is the arduino server file use with udp2latency.cpp client program to test latency of the link
// KKS Ethernet UDP connection
#include <SPI.h> // SPI library
#include <Ethernet2.h>
#include <EthernetUdp2.h>         // UDP library 
#include <Wire.h> // for i2c
#include <stdio.h>

/////////////////////////////////////////////////////////////// i2c reading address/////////////////
int timberconAddress = 0x51; // device address
int X0, X1, X_out;
#define rssi_Register_DATA_ADDR 0x68 // Hexadecima address 
uint32_t i=0;
char line[64];
/////////////////////////////////////////////////////////////// i2C reading address/////////////////

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // assign the MAC address
IPAddress ip(192, 168, 1, 177); // assign the ip address
unsigned int localPort = 8888;      // local port to talkover
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  //buffer to hold data packet
String datReq; //String for our data
int packetSize;// Size of the packet
EthernetUDP Udp; // Create UDP Object
bool xout_received = false;

char  ReplyBuffer[] = "acknowledged";       // a string to send back

void setup() {
  // start the Ethernet and UDP:
  Serial.begin(115200); //Initialize Serial Port
  Ethernet.begin(mac, ip); //Initialize Ethernet
  Udp.begin(localPort); //Initialize UDP
  delay(100); //Delay

  ///////////For i2C
  Wire.begin();
  Serial1.begin(115200);
  delay(100);
   
}

void loop() {
  if (!xout_received) {
    int packetSize = Udp.parsePacket();// if there's data available, read a packet
    if (packetSize>0) {     //if packet size>0 , some one is asking data
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE); // Read data request
      String datReq(packetBuffer); // convert char array into a string we called -- dataRequest
      if(datReq == "X_out"){ // do the following i2c
        xout_received = true;
      }
    }
  }
    
////////////////////////i2C reading from TimberCon//////////////
  Wire.beginTransmission(timberconAddress);
  Wire.write(rssi_Register_DATA_ADDR);
  Wire.endTransmission(); 
  Wire.requestFrom(timberconAddress, 2);    // request 2 bytes from timbercon #2

  if (Wire.available()<=2) { // 
    X0 = Wire.read();
    X1 = Wire.read();
    X_out = X0*256+X1;

    if (X_out <800) {
      X_out = -X_out;
    }
    
    // send a reply to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(line, snprintf(line, 64, "%lu %d\n", millis(), X_out));
    Udp.endPacket();

    
  }
}


