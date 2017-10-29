#include "Defines.h"
#include "pb.h"
#include "pb_encode.h"
#include "AirSensor.pb.h"
#include "DHT.H"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <RCSwitch.h>

/*--------------------*/
// WIFI SSID and PASSWD
/*--------------------*/
char ssid[] = "";
char password[] = "";

IPAddress remoteAddr(192,168,8,15); // Remote NetTemp Daemon

const int MESSAGE_SIZE = 128;
byte packetBuffer[ MESSAGE_SIZE]; //buffer to hold outgoing packets

static const int DHT_PIN = 3;
static const int DHT_TYPE = DHT22;
static const int deviceid = 1; // device ID in network packet

#define DEBUGLOG // Uncomment to enable Debug Console output
#define DATAPIN 4 // Data PIN on Board

RCSwitch mySwitch = RCSwitch();
char systemCode[32] = {0};
int unitCode = 0;
int command = -1;
String recvBuffer = "";
static unsigned long lastRefreshTime = 0;
static unsigned long lastMillis = 0;
  
DHT dht(DHT_PIN, DHT_TYPE);
WiFiUDP Udp;
void printWifiStatus();

WiFiUDP UdpRF;
static unsigned int kLocalUdpPort = 4210;
char incomingPacket[255];

void setup()
{
   pinMode(DATAPIN, OUTPUT);
  digitalWrite(DATAPIN, LOW);   // I do this first to avoid any reverse polarity

  mySwitch.enableTransmit(DATAPIN);
  mySwitch.setRepeatTransmit(5);
 
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  Serial.println(ssid);
 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  IPAddress localIP = WiFi.localIP();
  while (localIP[0] == 0)
  {
    localIP = WiFi.localIP();
    Serial.println("waiting for an IP address");
    delay(1000);
  }

  Serial.println("\nIP Address obtained");
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  Udp.begin(kCommsPort);
  UdpRF.begin(kLocalUdpPort);
  dht.begin();
}

void loop()
{
  sendTempUpdate();
  handleUDPIncoming();
}

void sendTempUpdate()
{
  static const unsigned long REFRESH_INTERVAL = 10000; // ms

 
  if(millis() - lastRefreshTime >= REFRESH_INTERVAL)
  {
    lastRefreshTime += REFRESH_INTERVAL;

    AirSensorMessage message = AirSensorMessage_init_zero;

    message.temperature = dht.readTemperature();
    message.humidity = dht.readHumidity();
    message.deviceid = deviceid;

    // temp or humidity is NaN, early return
    if (isnan(message.temperature) || isnan(message.humidity))
    {
      return;
    }
    sendMessage(remoteAddr, message);
  }
}
// Send Message to Daemon via UDP
void sendMessage(IPAddress& address, AirSensorMessage &message)
{
  memset(packetBuffer, 0, MESSAGE_SIZE);
  Udp.beginPacket(address, kCommsPort);
 
  pb_ostream_t output = pb_ostream_from_buffer(packetBuffer, MESSAGE_SIZE);
  if (!pb_encode(&output, AirSensorMessage_fields, &message))
  {
      Serial.println("Encoding failed.\n");
      Udp.endPacket();
      return;
  }
  Udp.write(packetBuffer, MESSAGE_SIZE);
  Udp.endPacket();
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void handleUDPIncoming() {
  int packetSize = UdpRF.parsePacket();
  if (packetSize)
  {
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, UdpRF.remoteIP().toString().c_str(), UdpRF.remotePort());
    int len = UdpRF.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);
    recvBuffer = incomingPacket;
    sendRFControl();
  }
}

void parseSerial() {
  getValue(recvBuffer, ' ', 0).toCharArray(systemCode, 32);
  unitCode = getValue(recvBuffer, ' ' , 1).toInt();
  command = getValue(recvBuffer, ' ', 2).toInt();
}

void sendRFControl() {
  parseSerial();

#ifdef DEBUGLOG
  if (command > -1) {
    Serial.print("System Code: ");
    Serial.println(systemCode);
    Serial.print("Unit Code: ");
    Serial.println(unitCode);
    Serial.print("Command: ");
    Serial.println(command);
  }
#endif

  switch (command) {
    case 0:
      mySwitch.switchOff(systemCode, unitCode);
      break;
    case 1:
      mySwitch.switchOn(systemCode, unitCode);
      break;
    default:
      command = -1;
      return;
  }
 
  command = -1;
  memset(systemCode, 0, sizeof(systemCode));
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void checkForRollover() {
  
  if (millis() < lastMillis) {
    lastMillis = 0;
    lastRefreshTime = 0;
  }
  lastMillis = millis();
}

