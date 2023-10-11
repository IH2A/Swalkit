#include "sensors.h"

Sensors::Sensors() {}

void Sensors::begin()
{
    // TCA_HUB_1.begin(Wire, TCA9548A_ADDRESS_1); // Start first Multiplexer
    // TCA_HUB_2.begin(Wire, TCA9548A_ADDRESS_2); // Start second Multiplexer
    
    // for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
    //     // sensor_average[i] = RunningAverage<int>(RA_SIZE);
    //     // sensor_average[i].fillValue(80,RA_SIZE);
    // }

    // for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
    //     // sensor[i] = Adafruit_VL53L0X();  
    // }
    
    // for (int i = 0; i < NUMBER_OF_SENSORS/2; i ++) {
    //   TCA_HUB_1.closeAll();
    //   TCA_HUB_2.closeAll();
    //   TCA_HUB_1.openChannel(HUB_1_I2C_SENSORS_CHANNELS[i]);
    
    // //   if (sensor[i].begin(0x29)) {
    //     // sensor[i].setAddress(HUB_1_SENSORS_ADDRS[i]);
    // //   } else {
    //     // sensor[i].begin(HUB_1_SENSORS_ADDRS[i]);
    // //   }
    // }

    // for (int i = 0; i < NUMBER_OF_SENSORS/2; i ++) {
    //   TCA_HUB_1.closeAll();
    //   TCA_HUB_2.closeAll();
    //   TCA_HUB_2.openChannel(HUB_2_I2C_SENSORS_CHANNELS[i]);
    
    // //   if (sensor[i + NUMBER_OF_SENSORS/2].begin(0x29)) {
    // //     sensor[i + NUMBER_OF_SENSORS/2].setAddress(HUB_2_SENSORS_ADDRS[i]);
    // //   } else {
    // //     sensor[i + NUMBER_OF_SENSORS/2].begin(HUB_2_SENSORS_ADDRS[i]);
    // //   }
    // }

    // TCA_HUB_1.openAll();
    // TCA_HUB_2.openAll();
}

void Sensors::read()
{
    // TCA_HUB_1.openAll();
    // TCA_HUB_2.openAll();

    // // read

    // TCA_HUB_1.closeAll();
    // TCA_HUB_2.closeAll();
}


// void readSensors() {
//   VL53L0X_RangingMeasurementData_t measure;
//     for(int i = 0; i < vectorVL53L0X.size(); i++) {
      
//       vectorVL53L0X[i]->getSingleRangingMeasurement(&measure, false);
//       if (measure.RangeStatus != 4 ) {  // phase failures have incorrect data
//         RA_vector[i]->addValue(measure.RangeMilliMeter<SENSOR_MAX_VALUE*10?
//                           measure.RangeMilliMeter/10 :
//                           SENSOR_MAX_VALUE);
//       } else {
//         RA_vector[i]->addValue(SENSOR_MAX_VALUE);
//       } 
//     } 

//     if(IMUEnabled){
//       M5.IMU.getAccelData(&accX, &accY, &accZ);
//       RA_IMU.addValue(abs(accX)+abs(accY)+abs(accZ));
//     }
// }
