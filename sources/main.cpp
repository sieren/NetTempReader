#include <socket.hpp>
#include <AirSensor.pb.h>
#include <pb_decode.h>
#include <Defines.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <sstream>
#include <unistd.h>


int main (int argc, const char * argv[]) {

  ServiceSocket mSocket(kCommsPort);
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
        pb_istream_t input = pb_istream_from_buffer(&data[0], data.size());
        if (!pb_decode(&input, AirSensorMessage_fields, &msg))
        {
          fprintf(stderr, "Decode failed: %s\n", PB_GET_ERROR(&input));
          return false;
        };
      }
    }
    std::cout << "Device: " << msg.deviceid << " Temp: " << msg.temperature << " Humidity: " << msg.humidity << std::endl;
    sleep(1);
  }

  return 0;
}

