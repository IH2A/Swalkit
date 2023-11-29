#include "sensors.h"

const unsigned char RA_SIZE = 4;  //RunningAverage size for the sensors
const unsigned char SENSOR_MAX_VALUE =  80;

#define VL53L0X_ADDRESS_DEFAULT 0b0101001

// Configuration des HUB i2c intelligents
uint8_t TCA9548A_ADDRESS_1 = 0x73;
uint8_t TCA9548A_ADDRESS_2 = 0x70;

unsigned char HUB_1_I2C_SENSORS_CHANNELS [4]= {
    TCA_CHANNEL_0,
    TCA_CHANNEL_1,
    TCA_CHANNEL_2,
    TCA_CHANNEL_3,
};
unsigned char HUB_2_I2C_SENSORS_CHANNELS [4]= {
    TCA_CHANNEL_3,
    TCA_CHANNEL_2,
    TCA_CHANNEL_1,
    TCA_CHANNEL_0,
};
unsigned char HUB_1_SENSORS_ADDRS [4]= {
    0x2A, 0x2B, 0x2C, 0x2D};
unsigned char HUB_2_SENSORS_ADDRS [4]= {
    0x2F, 0x2E, 0x29, 0x30};

Sensors::Sensors() : _debug(false){
      for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
        sensor_average[i] = new RunningAverage<int>(RA_SIZE);
        sensor_average[i]->fillValue(80,RA_SIZE);
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
        sensor[i] = new VL53L0X();  
    }
}

void Sensors::begin(bool debug)
{
    _debug = debug;
    TCA_HUB_1.begin(Wire, TCA9548A_ADDRESS_1); // Start first Multiplexer
    TCA_HUB_1.closeAll();
    TCA_HUB_2.begin(Wire, TCA9548A_ADDRESS_2); // Start second Multiplexer
    TCA_HUB_2.closeAll();

    for (int i = 0; i < NUMBER_OF_SENSORS/2; i ++) {
      TCA_HUB_1.closeAll();
      TCA_HUB_1.openChannel(HUB_1_I2C_SENSORS_CHANNELS[i]);

      Wire.beginTransmission(VL53L0X_ADDRESS_DEFAULT);
      byte error = Wire.endTransmission();

      sensor[i]->setTimeout(500);
      if(error == 0)
      {
          sensor[i]->init();
          sensor[i]->setAddress(HUB_1_SENSORS_ADDRS[i]);
      }
      else //RESET of the atoms3 case, sensors already init with new address
      {
          sensor[i]->setAddressNoI2CSend(HUB_1_SENSORS_ADDRS[i]);
          sensor[i]->init();
      }
      if(_debug) USBSerial.println(sensor[i]->readRangeSingleMillimeters());
    }   
    TCA_HUB_1.closeAll();

    TCA_HUB_2.begin(Wire, TCA9548A_ADDRESS_2); // Start second Multiplexer
  for (int i = 0; i < NUMBER_OF_SENSORS/2; i ++) {
      TCA_HUB_2.closeAll();
      TCA_HUB_2.openChannel(HUB_2_I2C_SENSORS_CHANNELS[i]);

      Wire.beginTransmission(VL53L0X_ADDRESS_DEFAULT);
      byte error = Wire.endTransmission();
      sensor[i + NUMBER_OF_SENSORS/2]->setTimeout(500);
      if(error == 0)
      {
          sensor[i+ NUMBER_OF_SENSORS/2]->init();
          sensor[i+ NUMBER_OF_SENSORS/2]->setAddress(HUB_2_SENSORS_ADDRS[i]);
      }
      else //RESET of the atoms3 case, sensors already init with new address
      {
          sensor[i+ NUMBER_OF_SENSORS/2]->setAddressNoI2CSend(HUB_2_SENSORS_ADDRS[i]);
          sensor[i+ NUMBER_OF_SENSORS/2]->init();
      }
      if(_debug) USBSerial.println(sensor[i+NUMBER_OF_SENSORS/2]->readRangeSingleMillimeters());
    }      

    TCA_HUB_1.openAll();
    TCA_HUB_2.openAll();
}

void Sensors::read()
{
    uint16_t measure;
    for(int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
      measure = sensor[i]->readRangeSingleMillimeters();
      if(_debug) USBSerial.print(i);
      if(_debug) USBSerial.print("...");
      if(_debug) USBSerial.println(measure);

      if (sensor[i]->timeoutOccurred()) {  // phase failures have incorrect data
        sensor_average[i]->addValue(measure<SENSOR_MAX_VALUE*10?
                          measure/10 :
                          SENSOR_MAX_VALUE);
      } else {
        sensor_average[i]->addValue(SENSOR_MAX_VALUE);
      } 
    } 
}