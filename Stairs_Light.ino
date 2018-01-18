#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//const char* ssid = "ssid";
//const char* password = "password";

WiFiUDP Udp;
const unsigned int localUdpPort = 4210;  // local port to listen on
const unsigned int myseconds = 60000;//1 minute
char incomingPacket[255];  // buffer for incoming packets
//const char replyPacket[] = "Turn On";  // a reply string to send back
const int led = 4;//1 TX, 3 RX
const int pirpin = 14;
unsigned int mymillis = millis()+10000;//how long to turn it initially (manually using switch)? 10 seconds
bool pirFlag = false;
bool isOnFlag = false;

void setup()
{
  pinMode(led,OUTPUT);
  digitalWrite(led, HIGH);
  //Serial.begin(115200);
  
  //Serial.printf("Connecting to %s ", ssid);
  //if you want to connect it to the internet:
  //WiFi.begin(ssid, password);
  //to assign a static ip, if the router refuses to assign the ip (eg. already assigned to another device) the esp will keep waiting and may not connect
  //WiFi.config(IPAddress(192, 168, 31, 240), IPAddress(192, 168, 31, 1), IPAddress(255, 255, 255, 0));
 // while (WiFi.status() != WL_CONNECTED)
  //{
   // delay(500);
    //Serial.print(".");
 // }
 // Serial.println(" connected");

  //IPAddress Ip(192, 168, 1, 1);
  //IPAddress NMask(255, 255, 255, 0);
  //WiFi.softAPConfig(Ip, Ip, NMask);
  //Serial.println(WiFi.softAP("StaircaseLight", "password") ? "Ready" : "Failed!");
  WiFi.softAP("StaircaseLight", "password");
  Udp.begin(localUdpPort);
  //Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}

void loop()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    //Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
      incomingPacket[len] = 0;
    receivedPacket();
  }
  int pir = digitalRead(pirpin);
  int ldr = analogRead(A0);
  //Serial.println(ldr);
  //pirFlag is used to take into consideration the comparator time set on the physical pir sensor, it's useless in this scenario but kept for reference
  if(pir)
  {
    if((!isOnFlag && ldr<400) || isOnFlag)//500: always turning up //keep it checking for new movement while it's on// no ldr check when it's on
    {
      receivedPacket();
      pirFlag = true;
      isOnFlag = true;
    }
  }
  else
      pirFlag = false;
  delay(100);
  if(mymillis<millis())
  {
    digitalWrite(led,LOW);
    isOnFlag = false;
  }
}

void receivedPacket()
{
    //Serial.printf("UDP packet contents: %s\n", incomingPacket);
    // send back a reply, to the IP address and port we got the packet from
    //Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    
    mymillis = millis()+myseconds;//60 seconds from now
    IPAddress broadcastIP = IPAddress(192, 168, 4, 255);
    Udp.beginPacket(broadcastIP, localUdpPort);
    //Udp.write(replyPacket);
    char charBuf[50];
    String(myseconds).toCharArray(charBuf, 10);
    Udp.write(charBuf);
    Udp.endPacket();
    //if connected to the local network as well
    //IPAddress broadcastIP2 = WiFi.localIP();
    //broadcastIP2[3]=255;
    //Udp.beginPacket(broadcastIP2, localUdpPort);
    //Udp.write(replyPacket);
   // Udp.endPacket();
    digitalWrite(led,HIGH);
    Serial.printf("Now millis:%d\nNext off: %d\n", millis(),mymillis);
}

