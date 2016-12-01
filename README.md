NetTempReader
=============
#### A  C++ Application to receive Temperature Information from an Arduino IoT Device

## Description

Receive Temperature and Hydro readings from a RedBearLab Duo through WiFi on a Linux or Mac device (e.g. Raspberry Pi).

## Build & Run

### Daemon
+ Using `cmake`: 
```
mkdir build && cd build
cmake ../ && make
```

### Arduino
To build on Arduino, simply use the Sketch provided.
   
Beware:   
The Arduino Sketch is developed on a [Redbear Duo](https://github.com/redbear/Duo).
However all data is written into a regular buffer, which can be used with various other
Arduino IoT/WiFi SDKs. The code should be rather straight forward.
