NetTempReader
=============
#### A  C++ Application to receive Temperature Information from an Arduino IoT Device

## Description

Receive Temperature and Hydro readings from a ESP8266 based board or RedBearLab Duo through WiFi on a Linux or Mac device (e.g. Raspberry Pi).

## Build

### Daemon
+ Using `cmake` on Linux or Mac: 
```
mkdir build && cd build  
cmake ../ && make
```

### Arduino
To build on Arduino, simply use the Sketch provided.
Make sure to add your AP Station Name and Password as well as the
Remote IP Address of your the device running the daemon.
   
Beware:   
The Arduino Sketch is developed to work with either the common [Adafruit Feather HUZZAH ESP8266](https://www.adafruit.com/product/2821) or the [Redbear Duo](https://github.com/redbear/Duo).
However all data is written into a regular buffer, which can be used with various other
Arduino IoT/WiFi SDKs. The code should be rather straight forward.

The temperature sensor is the commonly used [DHT22/AM2302] (https://www.adafruit.com/product/393).

## Run
`NetTempReader` provides a few command line options:   
`-v` for Verbose output (useful for diagnostics)   
`-d` for daemon mode (will fork into background)   
   
The last argument is the `path` to which the daemon shall write the data.
It creates and writes to `<deviceid>.txt` files, made up from the device id
specified in the Arduino Sketch.   
   
Example Output:
```
NetTempReader/build [ ./NetTempReader -v 
Device: 1 Temp: 19.5 Humidity: 53.6
Device: 1 Temp: 19.5 Humidity: 53.4
Device: 1 Temp: 19.5 Humidity: 53.4
^C
NetTempReader/build
```