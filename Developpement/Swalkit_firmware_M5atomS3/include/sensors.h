// ********** sensors.h ***********
#ifndef sensors_h
#define sensors_h

// #include "runningAverage.h"
#include <Wire.h>
#include "TCA9548A.h"
// #include "Adafruit_VL53L0X.h"

const unsigned char NUMBER_OF_SENSORS =  8;
const unsigned char RA_SIZE = 4;  //RunningAverage size for the sensors

// Configuration des HUB i2c intelligents
// uint8_t TCA9548A_ADDRESS_1 = 0x73;
// uint8_t TCA9548A_ADDRESS_2 = 0x70;

// unsigned char HUB_1_I2C_SENSORS_CHANNELS [4]= {
//     TCA_CHANNEL_0,
//     TCA_CHANNEL_1,
//     TCA_CHANNEL_2,
//     TCA_CHANNEL_3,
// };
// unsigned char HUB_2_I2C_SENSORS_CHANNELS [4]= {
//     TCA_CHANNEL_3,
//     TCA_CHANNEL_2,
//     TCA_CHANNEL_1,
//     TCA_CHANNEL_0,
// };
// unsigned char HUB_1_SENSORS_ADDRS [4]= {
//     0x2A, 0x2B, 0x2C, 0x2D};
// unsigned char HUB_2_SENSORS_ADDRS [4]= {
//     0x2F, 0x2E, 0x29, 0x30};


class Sensors
{
public:
    Sensors();

    void begin();
    void read();
    TCA9548A<TwoWire> TCA_HUB_1;
    // TCA9548A<TwoWire> TCA_HUB_2;

    // RunningAverage<int> sensor_average[8];
    // Adafruit_VL53L0X sensor[8];

};

#endif // sensors_h