#include <socket.hpp>
#include <AirSensor.pb.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <sstream>
#include <unistd.h>
#define CHAR4(c0, c1, c2, c3) ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0)

bool isValidHeader(std::vector<uint8_t>& data)
{
  using namespace std;
  uint32_t headerVal = CHAR4(data[0], data[1], data[2], data[3]);
  if (headerVal != CHAR4('t', 'e', 'm', 'p'))
  {
    return false;
  }
  return true;  
}

int main (int argc, const char * argv[]) {

  ServiceSocket mSocket(2400);
  AirSensorMessage msg = AirSensorMessage_init_zero;
  while(true)
  {
    if (mSocket.hasReceived())
    {
      std::vector<uint8_t> data;
      std::uint32_t ip;
      std::uint32_t port;
      if (mSocket.receive(ip, port, data))
      {
        if (!isValidHeader(data))
        {
          break;
        }

        unsigned long d;
        d = CHAR4(data[4], data[5], data[6], data[7]);
        float bla = *reinterpret_cast<float*>(&d);
        std::cout << "Temp Received: " << bla << std::endl;
      }
    }
    sleep(0.1);
  }

  return 0;
}

