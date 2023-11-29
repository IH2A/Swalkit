// ********** sensors.h ***********
#ifndef sensors_h
#define sensors_h

#include "runningAverage.h"
#include <Wire.h>
#include "TCA9548A.h"
//#include "Adafruit_VL53L0X.h"
#include "VL53L0X.h"

#define NUMBER_OF_SENSORS 8

class Sensors
{
public:
    Sensors();

    bool _debug;
    void begin(bool debug = false);
    void read();
    TCA9548A<TwoWire> TCA_HUB_1;
    TCA9548A<TwoWire> TCA_HUB_2;

    RunningAverage<int>* sensor_average[NUMBER_OF_SENSORS];
    VL53L0X* sensor[NUMBER_OF_SENSORS];

};

#endif // sensors_h