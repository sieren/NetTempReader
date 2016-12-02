// Daemon to handle incoming Temperature and Humidity from Arduino Sketch
// Written by Matthias Frick, public domain

#include <socket.hpp>
#include <AirSensor.pb.h>
#include <pb_decode.h>
#include <Defines.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

void printHelp()
{
  std::string helpText;
  helpText = "Usage: NetTemReapder -vd [path] \n" \
    "-v Verbose Output \n" \
    "-d Daemon (Background) \n" \
    "path Path to write incoming Device Messages to. \n" \
    "     Messages will be in form <deviceid>.txt";
  std::cout << helpText << std::endl;
}

void forkSelf(int& pid, int& sid)
{
  
  pid = fork();
  if (pid < 0) {
    exit(EXIT_FAILURE);
  }
  /* If we got a good PID, then
   we can exit the parent process. */
  if (pid > 0) {
    exit(EXIT_SUCCESS);
  }
  
  /* Change the file mode mask */
  umask(0);
  
  /* Open any logs here */
  
  /* Create a new SID for the child process */
  sid = setsid();
  if (sid < 0) {
    /* Log the failure */
    exit(EXIT_FAILURE);
  }
}

void writeToFile(const std::string filePath, const AirSensorMessage& msg)
{
  std::filebuf fb;
  fb.open (filePath,std::ios::trunc | std::ios::out);
  std::ostream os(&fb);
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << msg.temperature;
  os << stream.str() << std::endl;
  stream.str("");
  stream << std::fixed << std::setprecision(2) << msg.humidity;
  os << stream.str() << std::endl;
  fb.close();
}

int main (int argc, const char * argv[])
{
  bool daemon = false;
  bool verbose = false;
  pid_t pid, sid;
  if (argc == 1)
  {
    printHelp();
    return 0;
  }
  while ((argc > 1) && (argv[1][0] == '-'))
  {
    switch (argv[1][1])
    {
      case 'd':
        daemon = true;
        break;
        
      case 'v':
        verbose = true;
        break;
    }
    ++argv;
    --argc;
  }


  std::string path;
  if (argv[1] != nullptr)
  {
    path = std::string(argv[1]);
  }

  if (daemon)
  {
    std::cout << "Forking into background..." << std::endl;
    verbose = daemon ? false : verbose; // override verbose if daemon mode
    forkSelf(pid, sid);
  }

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
          break;
        };
        if (!path.empty())
        {
          try
          {
            if (path.back() == '/')
            {
              path.pop_back();
            }
            std::string filePath = path + "/" + std::to_string(msg.deviceid) + ".txt";
            writeToFile(filePath, msg);
          }
          catch (std::exception& e)
          {
            std::cerr << "Error writing to file: " << e.what() << std::endl;
          }
        }
        if (verbose)
        {
          std::cout << "Device: " << msg.deviceid
          << " Temp: " << msg.temperature
          << " Humidity: " << msg.humidity << std::endl;
        }
      }
    }
    sleep(1);
  }

  return 0;
}

