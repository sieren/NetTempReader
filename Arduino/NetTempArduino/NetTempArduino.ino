#include "Defines.h"
#include "pb.h"
#include "pb_encode.h"
#include "AirSensor.pb.h"
#include "DHT.H"
#if defined(ARDUINO) 
SYSTEM_MODE(SEMI_AUTOMATIC); 
#endif

/*--------------------*/
// WIFI SSID and PASSWD
/*--------------------*/
char ssid[] = "";
char password[] = "";

IPAddress remoteAddr(192,168,8,124); // Remote NetTemp Daemon

const int MESSAGE_SIZE = 128;
byte packetBuffer[ MESSAGE_SIZE]; //buffer to hold outgoing packets

static const int DHT_PIN = 3;
static const int DHT_TYPE = DHT22;
static const int deviceid = 1; // device ID in network packet

DHT dht(DHT_PIN, DHT_TYPE);
UDP Udp;
void printWifiStatus();

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  Serial.println(ssid);
 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.on();
  WiFi.setCredentials(ssid, password);
  WiFi.connect();
  
  while (WiFi.connecting())
  {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
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
  dht.begin();
}

void loop()
{
  delay(1000);
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

