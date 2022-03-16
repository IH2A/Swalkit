#include <Arduino.h>
#include <SPIFFS.h>
#include <M5Atom.h>
#include "runningAverage.h"
#include <Wire.h>
#include <Preferences.h>
#include "DacESP32.h"
#include "TCA9548A.h"
#include "Adafruit_VL53L0X.h"
#include "FastLED.h"
#include "BluetoothSerial.h"
#include "ArduinoJson.h"

#ifndef _m5atom_tfi_h_
#define _m5atom_tfi_h_

bool BluetoothEnable = false;
bool DisplayEnable = true;
bool VibrationEnabled = true;
bool IMUEnabled = true;
bool SecondHub = true;
bool HubBridge = true;

/************************************************************/
// PREFERENCES FILES
/************************************************************/

int addr = 0; // adress to read data
String filename = "/M5Stack/profile.txt";

std::vector<char> profile;
std::vector<char> name;
std::vector<char> surname;

void initProfile(); 

/*************************************************************/
// Bluetooth
/*************************************************************/

const char *BLUETOOTH_DEVICE_NAME = "Smart Walker IRISA";

bool BluetoothTimer = false;
int BluetoothInterval = 500;  // ms
String serialCommandData;

void serialCommandOptions(int serialCommand, String serialCommandData);

void readBluetoothData();
void ISRBlueetoothWriteSensors();

// timer for Bluetooth
hw_timer_t * timer1 = NULL;  // pointer to timer variable	
portMUX_TYPE timer1Mux = portMUX_INITIALIZER_UNLOCKED;
int prescaler = 80;           // results in timer frequency 1MHz; increment each microsecond


void IRAM_ATTR onTimer1() {   // timer interrupt for pulse
  portENTER_CRITICAL_ISR(&timer1Mux);
  BluetoothTimer = true;
  portEXIT_CRITICAL_ISR(&timer1Mux);
}

BluetoothSerial SerialBT;

/************************************************************************/
// Motor
/************************************************************************/
const int IN1_PIN = 19;
const int IN2_PIN = 23;
int VIN_PIN = 33;
int FAULT_PIN = 22;

bool RightON = false; // detection
bool LeftON = false;
bool motorLeft = false; // vibration
bool motorRight = false;
bool motor = true;    // pulsing


ushort FRONTAL_DANGER_VALUE_LEVEL = 90;
ushort FRONTAL_PULSE_VALUE_LEVEL = 300;
ushort DISTANCE_FORWARD_DANGER = 20;// cm 

std::vector<uint16_t> DISTANCES_LEVELS = {15,
                                          30,
                                          50};  // Far

std::vector<uint16_t> STEP_LEVELS =      {60,   // Danger
                                          70,
                                          80};  // Far
                                          
std::vector<uint16_t> PULSE_LEVELS =     {100,   // Danger
                                          0,
                                          0};  // Far


// timer for pulses
hw_timer_t * timer3 = NULL;
portMUX_TYPE timer3Mux = portMUX_INITIALIZER_UNLOCKED;
int pulseLength = 200;  // in ms
bool timer3running = false;


void IRAM_ATTR onTimer3() {   // timer interrupt for pulse
  portENTER_CRITICAL_ISR(&timer1Mux);
  motor = !motor;
  portEXIT_CRITICAL_ISR(&timer1Mux);
}

void changePulseTime(double ms){  // for changing pulse time while running
  if(pulseLength == ms) return;
    pulseLength = ms;
    timerAlarmWrite(timer3, ms * 1000, true);
    timerAlarmEnable(timer3);
}

// timer for vibration
hw_timer_t * timer2 = NULL;  // pointer to timer variable	
portMUX_TYPE timer2Mux = portMUX_INITIALIZER_UNLOCKED;

double currentVibration = 0;
bool lastLeft = true;

void IRAM_ATTR onTimer2() {
  portENTER_CRITICAL_ISR(&timer2Mux);
  digitalWrite(IN1_PIN, motorLeft && motor && RightON);
  digitalWrite(IN2_PIN, motorRight && motor && LeftON);
  motorLeft = !motorLeft;
  motorRight = !motorRight;
  portEXIT_CRITICAL_ISR(&timer2Mux);
}

void changeVibrationFrequency(double f){
  if(f == currentVibration) return;
  else{
    currentVibration= f;
    double us = 1/f * 1000000;     // f in T(us)
    timerAlarmWrite(timer2, us/2, true);  // cycle = T/2
    timerAlarmEnable(timer2);
  }
}

void WaveLMA(uint16_t right, uint16_t left, uint16_t front, uint16_t pulse);

/************************************************************************/
// SENSORS
/************************************************************************/
const unsigned char RA_SIZE = 2; //RunningAverage size for the sensors
const uint16_t SENSOR_MAX_VALUE = 80; //cm
const uint8_t NUMBER_OF_SENSORS = 8;
const uint8_t MAIN_TCA9548A_ADDRESS = 0x73; // A2, A1, A0 - L, L, L
const uint8_t SECOND_TCA9548A_ADDRESS = 0x70;
const uint8_t BRIDGE_TCA9548A_ADDRESS = 0x71;

const std::vector<unsigned char> HUB_I2C_SENSORS_CHANNELS = {
  TCA_CHANNEL_0, 
  TCA_CHANNEL_1, 
  TCA_CHANNEL_2, 
  TCA_CHANNEL_3,
  TCA_CHANNEL_4,
  TCA_CHANNEL_5,
  TCA_CHANNEL_6, 
  TCA_CHANNEL_7
  };
const std::vector<unsigned char> HUB_2_I2C_SENSORS_CHANNELS = {
  TCA_CHANNEL_3, 
  TCA_CHANNEL_2, 
  TCA_CHANNEL_1, 
  TCA_CHANNEL_0
  };
const std::vector<unsigned char> HUB_SENSORS_ADDRS = {
   0x2F, 0x2E, 0x29, 0x30, 0x2A, 0x2B, 0x2C, 0x2D
};
std::vector<uint16_t> vector_sensorAngles = {
  11, 34, 56, 79, 101, 124, 146, 169
};
std::vector<int8_t> offsetSensors ={
  40, 40, 40, 40, -40, -40, -40, -40
};

std::vector<Adafruit_VL53L0X*> vectorVL53L0X; // Sensor devices

int lastReadingValue = 0;
std::vector<RunningAverage<int>*> RA_vector; // Running average vector, hold the current reading values

void initSensors(); // Initialises the moving average and sensor task related variables
void readSensors(); // Read each sensor port of the Hub I2C

void setDisplayFromSensors();


// I2C HUB Configuration 
#ifdef SOFTWAREWIRE
  #include <SoftwareWIRE.h>
  SoftwareWire myWIRE(3, 2);
  TCA9548A<SoftwareWire> TCA_MAIN;
  TCA9548A<SoftwareWire> TCA_HUB_2;
  TCA9548A<SoftwareWire> TCA_HUB_BRIDGE;
  #define WIRE myWIRE
#else   
  TCA9548A<TwoWire> TCA_HUB;
  TCA9548A<TwoWire> TCA_HUB_2;
  TCA9548A<TwoWire> TCA_HUB_BRIDGE;
#endif

/************************************************************/
// IMU
/************************************************************/
float accX = 0.0F;  // Define variables for storing inertial sensor data
float accY = 0.0F;
float accZ = 0.0F;

const unsigned char RA_IMU_SIZE = 10;
RunningAverage<float> RA_IMU(RA_IMU_SIZE);
float MOVEMENT_TRIGGER = 0.01; 
float IMU_Calibration_Data;
int no_Movement_Timer = 3000;

std::vector<int> lastDistance;
int DistanceTrigger = 5;
bool distanceChanged = true;

bool Movement = true;
int lastMovement = 0;
int now = 0;

/************************************************************************/
// LCD
/************************************************************************/

#define BLUE    0x0000ff
#define GREEN   0x00ff00
#define RED     0xff0000
#define ORANGE  0xff5000
#define YELLOW  0xffff00

/************************************************************************/
// Functions
/************************************************************************/

void initSensors() {
  if(SecondHub){
    for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      RA_vector.push_back(new RunningAverage<int>(RA_SIZE));
      vectorVL53L0X.push_back(new Adafruit_VL53L0X());
      lastDistance.push_back(80);
      RA_vector[i]->fillValue(80, RA_SIZE);
    }
    
    for (int i = 0; i < NUMBER_OF_SENSORS/2; i ++) {
      TCA_HUB.closeAll();
      TCA_HUB_2.closeAll();
      TCA_HUB.openChannel(HUB_I2C_SENSORS_CHANNELS[i]);
    
      if (vectorVL53L0X[i]->begin(0x29)) {
        vectorVL53L0X[i]->setAddress(HUB_SENSORS_ADDRS[i]);
      } else {
        vectorVL53L0X[i]->begin(HUB_SENSORS_ADDRS[i]);
      }
    }

    for (int i = 0; i < NUMBER_OF_SENSORS/2; i ++) {
      TCA_HUB.closeAll();
      TCA_HUB_2.closeAll();
      TCA_HUB_2.openChannel(HUB_2_I2C_SENSORS_CHANNELS[i]);
    
      if (vectorVL53L0X[i + NUMBER_OF_SENSORS/2]->begin(0x29)) {
        vectorVL53L0X[i + NUMBER_OF_SENSORS/2]->setAddress(HUB_SENSORS_ADDRS[i + NUMBER_OF_SENSORS/2]);
      } else {
        vectorVL53L0X[i + NUMBER_OF_SENSORS/2]->begin(HUB_SENSORS_ADDRS[i + NUMBER_OF_SENSORS/2]);
      }
    }

    TCA_HUB.openAll();
    TCA_HUB_2.openAll();
  }

  else{  
    for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      RA_vector.push_back(new RunningAverage<int>(RA_SIZE));
      vectorVL53L0X.push_back(new Adafruit_VL53L0X()); 
      RA_vector[i]->fillValue(80, RA_SIZE);
    }
    
    for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
      TCA_HUB.closeAll();
      TCA_HUB.openChannel(HUB_I2C_SENSORS_CHANNELS[i]);
    
      if (vectorVL53L0X[i]->begin(0x29)) {
        vectorVL53L0X[i]->setAddress(HUB_SENSORS_ADDRS[i]);
      } else {
        vectorVL53L0X[i]->begin(HUB_SENSORS_ADDRS[i]);
      }
    }

    TCA_HUB.openAll();
  }

  // IMU
  RA_IMU.fillValue(1.5, RA_IMU_SIZE);
  Serial.println("\ncalibrating IMU...");
  for (int i = 0; i < RA_IMU_SIZE*5; i ++) {
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      RA_IMU.addValue(sqrt(abs(accX)*abs(accX)+abs(accY)*abs(accY)+abs(accZ)*abs(accZ)));
      delay(50);
    }
    IMU_Calibration_Data = RA_IMU.getAverage();
    Serial.printf("IMU Calibration: %f\n", IMU_Calibration_Data);
}

void readSensors() {
  VL53L0X_RangingMeasurementData_t measure;
  for(int i = 0; i < vectorVL53L0X.size(); i++) {
    
    vectorVL53L0X[i]->getSingleRangingMeasurement(&measure, false);
    if (measure.RangeStatus != 4 ) {  // phase failures have incorrect data
      RA_vector[i]->addValue(measure.RangeMilliMeter<SENSOR_MAX_VALUE*10?
                         measure.RangeMilliMeter/10 :
                         SENSOR_MAX_VALUE);
    } else {
      RA_vector[i]->addValue(SENSOR_MAX_VALUE);
    } 
  } 

  // read IMU
  if(IMUEnabled){
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      RA_IMU.addValue(sqrt(abs(accX)*abs(accX)+abs(accY)*abs(accY)+abs(accZ)*abs(accZ)));
      float test = sqrt(abs(accX)*abs(accX)+abs(accY)*abs(accY)+abs(accZ)*abs(accZ));
      now = millis();
      // Serial.printf("IMU: %f, %f, %f| %f\n", accX, accY, accZ, abs(test - IMU_Calibration_Data));

      for(int i = 0; i < NUMBER_OF_SENSORS; i++){
        if((!RightON && !LeftON) && abs(lastDistance[i] - RA_vector[i]->getAverage()) > DistanceTrigger){
            distanceChanged = true;
            // Serial.printf("Distance changed %i: %i", i, abs(lastDistance[i] - RA_vector[i]->getAverage()));
        }
      }

      if(abs(test - IMU_Calibration_Data) > MOVEMENT_TRIGGER && distanceChanged){
        Movement = true;
        lastMovement = now;
        distanceChanged = false;
      }
      if(abs(now - lastMovement) > no_Movement_Timer) Movement = false;
  }
}

void setDisplayFromSensors(){
  if(BluetoothEnable){
    M5.dis.fillpix(BLUE);
    return;
  }
  if(!Movement){
    M5.dis.fillpix(GREEN);
    return;
  }
  // first row
    if ( RA_vector[0]->getAverage() < DISTANCE_FORWARD_DANGER || RA_vector[1]->getAverage() < DISTANCES_LEVELS[0] ){
      M5.dis.drawpix(0, RED);
      M5.dis.drawpix(5, RED);
      M5.dis.drawpix(10, RED);
      M5.dis.drawpix(15, RED);
      M5.dis.drawpix(20, RED);
    }
    else if ( RA_vector[0]->getAverage() < DISTANCES_LEVELS[0] || RA_vector[1]->getAverage() < DISTANCES_LEVELS[1] ){
      M5.dis.drawpix(0, YELLOW);
      M5.dis.drawpix(5, ORANGE);
      M5.dis.drawpix(10, RED);
      M5.dis.drawpix(15, RED);
      M5.dis.drawpix(20, RED);
    }
    else if ( RA_vector[0]->getAverage() < DISTANCES_LEVELS[1] || RA_vector[1]->getAverage() < DISTANCES_LEVELS[2] ){
      M5.dis.drawpix(0, GREEN);
      M5.dis.drawpix(5, YELLOW);
      M5.dis.drawpix(10, ORANGE);
      M5.dis.drawpix(15, RED);
      M5.dis.drawpix(20, RED);
    }
    else if ( RA_vector[0]->getAverage() < DISTANCES_LEVELS[2] || RA_vector[1]->getAverage() < DISTANCES_LEVELS[3] ){
      M5.dis.drawpix(0, GREEN);
      M5.dis.drawpix(5, GREEN);
      M5.dis.drawpix(10, YELLOW);
      M5.dis.drawpix(15, ORANGE);
      M5.dis.drawpix(20, RED);
    }
    else {
      M5.dis.drawpix(0, GREEN);
      M5.dis.drawpix(5, GREEN);
      M5.dis.drawpix(10, GREEN);
      M5.dis.drawpix(15, YELLOW);
      M5.dis.drawpix(20, ORANGE);
    }

  // second row
  if ( RA_vector[2]->getAverage() < DISTANCE_FORWARD_DANGER || RA_vector[3]->getAverage() < DISTANCES_LEVELS[0] ){
      M5.dis.drawpix(1, RED);
      M5.dis.drawpix(6, RED);
      M5.dis.drawpix(11, RED);
      M5.dis.drawpix(16, RED);
      M5.dis.drawpix(21, RED);
    }
    else if ( RA_vector[2]->getAverage() < DISTANCES_LEVELS[0] || RA_vector[3]->getAverage() < DISTANCES_LEVELS[1] ){
      M5.dis.drawpix(1, YELLOW);
      M5.dis.drawpix(6, ORANGE);
      M5.dis.drawpix(11, RED);
      M5.dis.drawpix(16, RED);
      M5.dis.drawpix(21, RED);
    }
    else if ( RA_vector[2]->getAverage() < DISTANCES_LEVELS[1] || RA_vector[3]->getAverage() < DISTANCES_LEVELS[2] ){
      M5.dis.drawpix(1, GREEN);
      M5.dis.drawpix(6, YELLOW);
      M5.dis.drawpix(11, ORANGE);
      M5.dis.drawpix(16, RED);
      M5.dis.drawpix(21, RED);
    }
    else if ( RA_vector[2]->getAverage() < DISTANCES_LEVELS[2] || RA_vector[3]->getAverage() < DISTANCES_LEVELS[3] ){
      M5.dis.drawpix(1, GREEN);
      M5.dis.drawpix(6, GREEN);
      M5.dis.drawpix(11, YELLOW);
      M5.dis.drawpix(16, ORANGE);
      M5.dis.drawpix(21, RED);
    }
    else {
      M5.dis.drawpix(1, GREEN);
      M5.dis.drawpix(6, GREEN);
      M5.dis.drawpix(11, GREEN);
      M5.dis.drawpix(16, YELLOW);
      M5.dis.drawpix(21, ORANGE);
    }

    //third row
    if ( RA_vector[3]->getAverage() < DISTANCE_FORWARD_DANGER || RA_vector[3]->getAverage() < DISTANCES_LEVELS[0] ){
      M5.dis.drawpix(2, RED);
      M5.dis.drawpix(7, RED);
      M5.dis.drawpix(12, RED);
      M5.dis.drawpix(17, RED);
      M5.dis.drawpix(22, RED);
    }
    else if ( RA_vector[3]->getAverage() < DISTANCES_LEVELS[0] || RA_vector[4]->getAverage() < DISTANCES_LEVELS[1] ){
      M5.dis.drawpix(2, YELLOW);
      M5.dis.drawpix(7, ORANGE);
      M5.dis.drawpix(12, RED);
      M5.dis.drawpix(17, RED);
      M5.dis.drawpix(22, RED);
    }
    else if ( RA_vector[3]->getAverage() < DISTANCES_LEVELS[1] || RA_vector[4]->getAverage() < DISTANCES_LEVELS[2] ){
      M5.dis.drawpix(2, GREEN);
      M5.dis.drawpix(7, YELLOW);
      M5.dis.drawpix(12, ORANGE);
      M5.dis.drawpix(17, RED);
      M5.dis.drawpix(22, RED);
    }
    else if ( RA_vector[3]->getAverage() < DISTANCES_LEVELS[2] || RA_vector[4]->getAverage() < DISTANCES_LEVELS[3] ){
      M5.dis.drawpix(2, GREEN);
      M5.dis.drawpix(7, GREEN);
      M5.dis.drawpix(12, YELLOW);
      M5.dis.drawpix(17, ORANGE);
      M5.dis.drawpix(22, RED);
    }
    else {
      M5.dis.drawpix(2, GREEN);
      M5.dis.drawpix(7, GREEN);
      M5.dis.drawpix(12, GREEN);
      M5.dis.drawpix(17, YELLOW);
      M5.dis.drawpix(22, ORANGE);
    }

  // forth row
    if ( RA_vector[4]->getAverage() < DISTANCE_FORWARD_DANGER || RA_vector[5]->getAverage() < DISTANCES_LEVELS[0] ){
      M5.dis.drawpix(3, RED);
      M5.dis.drawpix(8, RED);
      M5.dis.drawpix(13, RED);
      M5.dis.drawpix(18, RED);
      M5.dis.drawpix(23, RED);
    }
    else if ( RA_vector[3]->getAverage() < DISTANCES_LEVELS[0] || RA_vector[4]->getAverage() < DISTANCES_LEVELS[1] ){
      M5.dis.drawpix(3, YELLOW);
      M5.dis.drawpix(8, ORANGE);
      M5.dis.drawpix(13, RED);
      M5.dis.drawpix(18, RED);
      M5.dis.drawpix(23, RED);
    }
    else if ( RA_vector[3]->getAverage() < DISTANCES_LEVELS[1] || RA_vector[4]->getAverage() < DISTANCES_LEVELS[2] ){
      M5.dis.drawpix(3, GREEN);
      M5.dis.drawpix(8, YELLOW);
      M5.dis.drawpix(13, ORANGE);
      M5.dis.drawpix(18, RED);
      M5.dis.drawpix(23, RED);
    }
    else if ( RA_vector[3]->getAverage() < DISTANCES_LEVELS[2] || RA_vector[4]->getAverage() < DISTANCES_LEVELS[3] ){
      M5.dis.drawpix(3, GREEN);
      M5.dis.drawpix(8, GREEN);
      M5.dis.drawpix(13, YELLOW);
      M5.dis.drawpix(18, ORANGE);
      M5.dis.drawpix(23, RED);
    }
    else {
      M5.dis.drawpix(3, GREEN);
      M5.dis.drawpix(8, GREEN);
      M5.dis.drawpix(13, GREEN);
      M5.dis.drawpix(18, YELLOW);
      M5.dis.drawpix(23, ORANGE);
    }

  // fifth row
    if ( RA_vector[6]->getAverage() < DISTANCE_FORWARD_DANGER || RA_vector[7]->getAverage() < DISTANCES_LEVELS[0] ){
      M5.dis.drawpix(4, RED);
      M5.dis.drawpix(9, RED);
      M5.dis.drawpix(14, RED);
      M5.dis.drawpix(19, RED);
      M5.dis.drawpix(24, RED);
    }
    else if ( RA_vector[6]->getAverage() < DISTANCES_LEVELS[0] || RA_vector[7]->getAverage() < DISTANCES_LEVELS[1] ){
      M5.dis.drawpix(4, YELLOW);
      M5.dis.drawpix(9, ORANGE);
      M5.dis.drawpix(14, RED);
      M5.dis.drawpix(19, RED);
      M5.dis.drawpix(24, RED);
    }
    else if ( RA_vector[6]->getAverage() < DISTANCES_LEVELS[1] || RA_vector[7]->getAverage() < DISTANCES_LEVELS[2] ){
      M5.dis.drawpix(4, GREEN);
      M5.dis.drawpix(9, YELLOW);
      M5.dis.drawpix(14, ORANGE);
      M5.dis.drawpix(19, RED);
      M5.dis.drawpix(24, RED);
    }
    else if ( RA_vector[6]->getAverage() < DISTANCES_LEVELS[2] || RA_vector[7]->getAverage() < DISTANCES_LEVELS[3] ){
      M5.dis.drawpix(4, GREEN);
      M5.dis.drawpix(9, GREEN);
      M5.dis.drawpix(14, YELLOW);
      M5.dis.drawpix(19, ORANGE);
      M5.dis.drawpix(24, RED);
    }
    else {
      M5.dis.drawpix(4, GREEN);
      M5.dis.drawpix(9, GREEN);
      M5.dis.drawpix(14, GREEN);
      M5.dis.drawpix(19, YELLOW);
      M5.dis.drawpix(24, ORANGE);
    }
}

/************************************************************************/
// profile
/************************************************************************/
void initProfile() {
  if(!SPIFFS.begin()){
    Serial.println("\nSPIFFS format start...");
    SPIFFS.format();
    Serial.println("SPIFFS format finish");
  }
  // for debugging only
      // dataFile = SPIFFS.open(filename, "w");
      // dataFile.printf("");

  if(SPIFFS.begin()){
    Serial.println("SPIFFS Begin.");
    File dataFile = SPIFFS.open(filename, "r");
    if(dataFile.size() > 0){
      Serial.println("profile found, initialising...\n");

      std::vector<char> DataBuffer;
      for(int i=0; i<dataFile.size(); i++){
        DataBuffer.push_back((char)dataFile.read());
      }
      for(int i=0; i<DataBuffer.size(); i++){
        Serial.print(DataBuffer[i]);
      }

      int pos1;
      for(pos1 = 0; DataBuffer[pos1] != ':'; pos1++){}
      pos1+=2;
      int pos2;
      profile.clear(); // empty vector
      for(pos2 = pos1; DataBuffer[pos2] != ';'; pos2++){
        profile.push_back(DataBuffer[pos2]);
      }
      
      name.clear();
      for(;DataBuffer[pos1] != ':'; pos1++){}
      pos1+=2;
      for(pos2 = pos1; DataBuffer[pos2] != ';'; pos2++){
        name.push_back(DataBuffer[pos2]);
      }

      surname.clear();
      for(;DataBuffer[pos1] != ':'; pos1++){}
      pos1+=2;
      for(pos2 = pos1; DataBuffer[pos2] != ';'; pos2++){
        surname.push_back(DataBuffer[pos2]);
      }

      // read settings
      bool searching = true;
      while(searching && DataBuffer[pos1] != '\0'){
          pos1++;
          if(DataBuffer[pos1] == 'F' && DataBuffer[pos1 + 1] == 'a' && DataBuffer[pos1 + 2] == 'r' && DataBuffer[pos1 + 3] == ':'){
            searching = false;
            pos1+=5;
          }
      }
      char buf[10];
      int b = 0;
      for(pos2 = pos1; DataBuffer[pos2] != ';'; pos2++){
        buf[b] = DataBuffer[pos2];
        b++;
      }
      buf[b] = '\0';
      STEP_LEVELS[2] = atoi(buf);
      Serial.printf("\ngot distance level far: %i", DISTANCES_LEVELS[2]);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      STEP_LEVELS[1] = atoi(buf);
      Serial.printf("\ngot distance level near: %i", DISTANCES_LEVELS[1]);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      STEP_LEVELS[0] = atoi(buf);
      Serial.printf("\ngot distance level danger: %i", DISTANCES_LEVELS[0]);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      STEP_LEVELS[2] = atoi(buf);
      Serial.printf("\ngot step level far: %i", STEP_LEVELS[2]);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      STEP_LEVELS[1] = atoi(buf);
      Serial.printf("\ngot step level near: %i", STEP_LEVELS[1]);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      STEP_LEVELS[0] = atoi(buf);
      Serial.printf("\ngot step level danger: %i", STEP_LEVELS[0]);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      PULSE_LEVELS[2] = atoi(buf);
      Serial.printf("\ngot pulse level far: %i", PULSE_LEVELS[2]);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      PULSE_LEVELS[1] = atoi(buf);
      Serial.printf("\ngot pulse level near: %i", PULSE_LEVELS[1]);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      PULSE_LEVELS[0] = atoi(buf);
      Serial.printf("\ngot pulse level danger: %i", PULSE_LEVELS[0]);      

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      FRONTAL_PULSE_VALUE_LEVEL = atoi(buf);
      Serial.printf("\ngot pulse level forward: %i", FRONTAL_PULSE_VALUE_LEVEL);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      FRONTAL_DANGER_VALUE_LEVEL = atoi(buf);
      Serial.printf("\ngot wave level forward: %i", FRONTAL_DANGER_VALUE_LEVEL);

      pos1 = pos2;
      while(DataBuffer[pos1] != ':'){ 
        pos1++;
      }
      pos1+=2;
      pos2 = pos1;
      while(DataBuffer[pos2] != ';'){
        pos2++;
      }
      for(int i = 0; i < 10; i++) buf[i] = '\0';
      b = 0;
      while(pos1 != pos2){
        buf[b] = DataBuffer[pos1];
        pos1++;
        b++;
      }
      buf[b] = '\0';
      DISTANCE_FORWARD_DANGER = atoi(buf);
      Serial.printf("\ngot distance level far: %i", DISTANCE_FORWARD_DANGER);

      dataFile.close();
    }
    else{
      Serial.println("no profile found, creating base profile");
      dataFile = SPIFFS.open(filename, "a");
      char stringBuffer[50];
      sprintf(stringBuffer, "filename: Default File; ");
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "name: TestName; ");
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "surname: TestSurname; ");
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "number_of_sensors: %i; ", NUMBER_OF_SENSORS);
      dataFile.printf(stringBuffer);

      for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
        sprintf(stringBuffer, "%i sensorID: %i; ", i, i);
        dataFile.printf(stringBuffer);
        sprintf(stringBuffer, "sensorAngle: %i; ", vector_sensorAngles[i]);
        dataFile.printf(stringBuffer);
        if(i < NUMBER_OF_SENSORS/2) sprintf(stringBuffer, "sensorLocation: r; ");
        else sprintf(stringBuffer, "sensorLocation: l; ");
        dataFile.printf(stringBuffer);
        sprintf(stringBuffer, "sensorOffset: %i; ", offsetSensors[i]);
        dataFile.printf(stringBuffer);
      }

      sprintf(stringBuffer, "Far: %i; ", DISTANCES_LEVELS[2]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "Near: %i; ", DISTANCES_LEVELS[1]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "Danger: %i; ", DISTANCES_LEVELS[0]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "waveFar: %i; ", STEP_LEVELS[2]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "waveNear: %i; ", STEP_LEVELS[1]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "waveDanger: %i; ", STEP_LEVELS[0]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "pulseFar: %i; ", PULSE_LEVELS[2]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "pulseNear: %i; ", PULSE_LEVELS[1]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "pulseDanger: %i; ", PULSE_LEVELS[0]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "pulseForward: %i; ", FRONTAL_PULSE_VALUE_LEVEL);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "waveForward: %i; ", FRONTAL_DANGER_VALUE_LEVEL);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "Forward: %i; ", DISTANCE_FORWARD_DANGER);
      dataFile.printf(stringBuffer);

      dataFile.close();
      
      Serial.printf("profile created\n");

      // debugging
      File dataFile = SPIFFS.open(filename, "r");   // Create a File object dafaFile to read information to file_name in the SPIFFS.
      for(int i=0; i<dataFile.size(); i++){ //Reads file contents and outputs file information through the serial port monitor.
        Serial.print((char)dataFile.read());
      }
    dataFile.close(); //Close the file after reading the file.
    }
  }
  else {
    Serial.println("SPIFFS Failed to Begin.\nYou need to Run SPIFFS_Add.ino first");
  }
}

void safeProfile(){
      File dataFile = SPIFFS.open(filename, "w");
      dataFile.printf("");  // empty file
      dataFile = SPIFFS.open(filename, "a");  // append

      char Buffer[20];
      char stringBuffer[40];

      for(int i = 0; i < profile.size(); i++){
        Buffer[i] = profile[i];
      }
      sprintf(stringBuffer, "filename: %s; ", Buffer);
      dataFile.printf(stringBuffer);

      for(int i = 0; i < name.size(); i++){
        Buffer[i] = name[i];
      }
      sprintf(stringBuffer, "name: %s; ", Buffer);
      dataFile.printf(stringBuffer);

      for(int i = 0; i < surname.size(); i++){
        Buffer[i] = surname[i];
      }
      sprintf(stringBuffer, "surname: %s; ", Buffer);
      dataFile.printf(stringBuffer);


      for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
        sprintf(stringBuffer, "%i sensorID: %i; ", i, i);
        dataFile.printf(stringBuffer);
        sprintf(stringBuffer, "sensorAngle: %i; ", vector_sensorAngles[i]);
        dataFile.printf(stringBuffer);
        if(i < NUMBER_OF_SENSORS/2) sprintf(stringBuffer, "sensorLocation: r; ");
        else sprintf(stringBuffer, "sensorLocation: l; ");
        dataFile.printf(stringBuffer);
        sprintf(stringBuffer, "sensorOffset: %i; ", offsetSensors[i]);
        dataFile.printf(stringBuffer);
      }

      sprintf(stringBuffer, "Far: %i; ", DISTANCES_LEVELS[2]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "Near: %i; ", DISTANCES_LEVELS[1]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "Danger: %i; ", DISTANCES_LEVELS[0]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "waveFar: %i; ", STEP_LEVELS[2]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "waveNear: %i; ", STEP_LEVELS[1]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "waveDanger: %i; ", STEP_LEVELS[0]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "pulseFar: %i; ", PULSE_LEVELS[2]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "pulseNear: %i; ", PULSE_LEVELS[1]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "pulseDanger: %i; ", PULSE_LEVELS[0]);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "pulseForward: %i; ", FRONTAL_PULSE_VALUE_LEVEL);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "waveForward: %i; ", FRONTAL_DANGER_VALUE_LEVEL);
      dataFile.printf(stringBuffer);
      sprintf(stringBuffer, "Forward: %i; ", DISTANCE_FORWARD_DANGER);
      dataFile.printf(stringBuffer);

      dataFile.close();
}
/************************************************************************/
// Bluetooth
/************************************************************************/
String JSONtoStringBT(DynamicJsonDocument doc) {
    if (!doc.isNull()) {
        String serialInformation;
        serializeJson(doc, serialInformation);
        return serialInformation;
    }
    else return "";
}

String sendIntListBT(std::vector<int> values, const char * field) {
    DynamicJsonDocument doc(500);
    JsonArray data = doc.createNestedArray(field);
    for (int value : values){
        //if (value) {
            data.add(value);   
        //}
    }
    return JSONtoStringBT(doc);
}

void ISRBlueetoothWriteSensors() {
  std::vector<int> values;
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      values.push_back(RA_vector[i]->getAverage());
  }
  SerialBT.println("20"+sendIntListBT(values, "sensors"));
  Serial.println("SEND\t20" + sendIntListBT(values, "sensors"));
}


void readBluetoothData() {
  if (SerialBT.available()) {
    Serial.println("\nreading BT");
    // Read the whole command send via Bluetooth
    serialCommandData = SerialBT.readStringUntil('\n');
    if (!serialCommandData) {
      Serial.println("Cannot read command");
    } else {
      Serial.println("got command: " + serialCommandData);
      // Get wich type of command it represents
      int serialCommand = serialCommandData.substring(0,2).toInt();
      Serial.println("command: " + String(serialCommand));
      String commandAtributes = serialCommandData.length() > 2 ?
              serialCommandData.substring(2, serialCommandData.length()) :
              "";
      Serial.println("attributes: " + commandAtributes);
      serialCommandOptions(serialCommand, commandAtributes);
    }   
  }
}

/***************JSON*****************/
DynamicJsonDocument getJSONObject() {
    DynamicJsonDocument jsonCurrentFileDocument(500);
    char buffer[50];
    for(int i = 0; i < 50; i++) buffer[i] = '\0';
    for( int i = 0; i < profile.size(); i++)
      buffer[i] = profile[i];
    String buf(buffer);
    jsonCurrentFileDocument["fileName"] = buf;

    for(int i = 0; i < 50; i++) buffer[i] = '\0';
    for( int i = 0; i < name.size(); i++)
      buffer[i] = name[i];
    buf = buffer;
    jsonCurrentFileDocument["userName"] = buf;

    for(int i = 0; i < 50; i++) buffer[i] = '\0';
    for( int i = 0; i < surname.size(); i++)
      buffer[i] = surname[i];
    buf = buffer;
    jsonCurrentFileDocument["userSurname"] = buf;
    
    return jsonCurrentFileDocument;
}

DynamicJsonDocument getSensorsJSONObject() { 
    DynamicJsonDocument jsonSensorsVector(1500);
    JsonArray idArray = jsonSensorsVector.createNestedArray("id");
    JsonArray angleArray = jsonSensorsVector.createNestedArray("angles");
    for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
        idArray.add(i+1);
        angleArray.add(vector_sensorAngles[i]);
    } 
    // By default the array is filed with right sensors first and left sensors after
    // The two desired offsets can be found in the first and last sensors
    JsonArray offsetArray = jsonSensorsVector.createNestedArray("offset");
    offsetArray.add(offsetSensors[0]);
    offsetArray.add(offsetSensors[1]);
    return jsonSensorsVector;
}

DynamicJsonDocument getMotorsJSONObject() { 

  std::vector<int> vibrations;
  std::vector<int> pulse;
  std::vector<int> distances;

  std::vector<int> parserVector = {
    FRONTAL_DANGER_VALUE_LEVEL,
      STEP_LEVELS[0],
      STEP_LEVELS[1],
      STEP_LEVELS[2]
    };

  for (int v : parserVector)
  {
      vibrations.push_back(v);
  }

  parserVector = {
    FRONTAL_PULSE_VALUE_LEVEL,
      PULSE_LEVELS[0],
      PULSE_LEVELS[1],
      PULSE_LEVELS[2]
    };
        
  for (int p : parserVector)
  {
      pulse.push_back(p);
  }

  parserVector = {
    DISTANCE_FORWARD_DANGER,
      DISTANCES_LEVELS[0],
      DISTANCES_LEVELS[1],
      DISTANCES_LEVELS[2]
    };
        
  for (int d : parserVector)
  {
      distances.push_back(d);
  }

    DynamicJsonDocument jsonVector(1500);
    JsonArray vibrationsArray = jsonVector.createNestedArray("values");
    JsonArray pulseArray = jsonVector.createNestedArray("pulses");
    JsonArray distancesArray = jsonVector.createNestedArray("distances");
    for(int i = 0; i < 4; i++){
      vibrationsArray.add(vibrations[i]);
      pulseArray.add(pulse[i]);
      distancesArray.add(distances[i]);
    }
    return jsonVector;
}
/***************END JSON*****************/

void serialCommandOptions(int serialCommand, String serialCommandData) {
  DynamicJsonDocument doc(1500);
  
  switch (serialCommand)
  {
    case 00:  // send current profile
      SerialBT.println("00" + JSONtoStringBT(getJSONObject()));
      Serial.println("SERIAL BT\t00" + JSONtoStringBT(getJSONObject()));
      break;
    case 01:
      SerialBT.println("01" + JSONtoStringBT(getSensorsJSONObject()));
      Serial.println("01" + JSONtoStringBT(getSensorsJSONObject()));
      break;
    case 02:
      SerialBT.println("02" + JSONtoStringBT(getMotorsJSONObject()));
      Serial.println("02" + JSONtoStringBT(getMotorsJSONObject()));
      break;
    
    case 80:
      Serial.println(serialCommandData);
      deserializeJson(doc, serialCommandData);

      doc["hapticMotor"].as<JsonArray>();

        char Buffer1[20];
        char Buffer3[20];
        char Buffer2[20];
        doc["fileName"].as<String>().toCharArray(Buffer1, 20);
        profile.clear();
        for(int i = 0; i < 20; i++){
          profile.push_back(Buffer1[i]);
        }
        doc["userName"].as<String>().toCharArray(Buffer2, 20);
        name.clear();
        for(int i = 0; i < 20; i++){
          name.push_back(Buffer2[i]);
        }
        doc["userSurname"].as<String>().toCharArray(Buffer3, 20);
        surname.clear();
        for(int i = 0; i < 20; i++){
          surname.push_back(Buffer3[i]);
        }

        FRONTAL_DANGER_VALUE_LEVEL = doc["hapticMotor"].getElement(0).as<int>();
        STEP_LEVELS[0] = doc["hapticMotor"].getElement(1).as<int>();
        STEP_LEVELS[1] = doc["hapticMotor"].getElement(2).as<int>();
        STEP_LEVELS[2] = doc["hapticMotor"].getElement(3).as<int>();
        FRONTAL_PULSE_VALUE_LEVEL = doc["hapticMotor"].getElement(4).as<int>();
        PULSE_LEVELS[0] = doc["hapticMotor"].getElement(5).as<int>();
        PULSE_LEVELS[1] = doc["hapticMotor"].getElement(6).as<int>();
        PULSE_LEVELS[2] = doc["hapticMotor"].getElement(7).as<int>();
        DISTANCE_FORWARD_DANGER = doc["hapticMotor"].getElement(8).as<int>();
        DISTANCES_LEVELS[0] = doc["hapticMotor"].getElement(9).as<int>();
        DISTANCES_LEVELS[1] = doc["hapticMotor"].getElement(10).as<int>();
        DISTANCES_LEVELS[2] = doc["hapticMotor"].getElement(11).as<int>();

        safeProfile();
      
        Serial.println("Profile updated");
        
      break;

    default:
      break;
  }
}

void setVibrationFromSensors() {
  uint16_t right, left, front, pulse;

  if(!Movement){
    WaveLMA(0, 0, 0, 0);
    return;
  }

  // Front
  if (   RA_vector[2]->getAverage() < DISTANCE_FORWARD_DANGER     // Danger
      || RA_vector[3]->getAverage() < DISTANCE_FORWARD_DANGER 
      || RA_vector[4]->getAverage() < DISTANCE_FORWARD_DANGER 
      || RA_vector[5]->getAverage() < DISTANCE_FORWARD_DANGER ){
        front = 1;
        pulse = FRONTAL_PULSE_VALUE_LEVEL;
      }
  else{
      front = 0;
      pulse = 0;
      }

  // Right
  if (   RA_vector[0]->getAverage() < DISTANCES_LEVELS[0]     // Danger
      || RA_vector[1]->getAverage() < DISTANCES_LEVELS[0]
      || RA_vector[2]->getAverage() < DISTANCES_LEVELS[0]) {
        right = 3;
        pulse = PULSE_LEVELS[0];
  } else if (  RA_vector[0]->getAverage() < DISTANCES_LEVELS[1]
            || RA_vector[1]->getAverage() < DISTANCES_LEVELS[1]
            || RA_vector[2]->getAverage() < DISTANCES_LEVELS[1]) {
        right = 2;
        pulse = PULSE_LEVELS[1];
  } else if (  RA_vector[0]->getAverage() < DISTANCES_LEVELS[2]
            || RA_vector[1]->getAverage() < DISTANCES_LEVELS[2]
            || RA_vector[2]->getAverage() < DISTANCES_LEVELS[2]) {
        right = 1;
        pulse = PULSE_LEVELS[2];
  }else {
        right = 0;
        pulse = 0;
  }
  // Left
  if (   RA_vector[5]->getAverage() < DISTANCES_LEVELS[0]
      || RA_vector[6]->getAverage() < DISTANCES_LEVELS[0]
      || RA_vector[6]->getAverage() < DISTANCES_LEVELS[0]) {
        left = 3;
        pulse = PULSE_LEVELS[0];
  } else if (  RA_vector[5]->getAverage() < DISTANCES_LEVELS[1]
            || RA_vector[6]->getAverage() < DISTANCES_LEVELS[1]
            || RA_vector[7]->getAverage() < DISTANCES_LEVELS[1]) {
        left = 3;
        pulse = PULSE_LEVELS[1];
  } else if (  RA_vector[5]->getAverage() < DISTANCES_LEVELS[2]
            || RA_vector[6]->getAverage() < DISTANCES_LEVELS[2]
            || RA_vector[7]->getAverage() < DISTANCES_LEVELS[2]) {
        left = 3;
        pulse = PULSE_LEVELS[2];
  } else {
        left = 0;
        pulse = 0;
  }

  WaveLMA(right, left, front, pulse);
}

void WaveLMA(uint16_t right, uint16_t left, uint16_t front, uint16_t pulse){
  if(pulse){
    if(!timer3running){
      timerAlarmEnable(timer3);
      timer3running = true;
    }
    changePulseTime(pulse);
  }
  else{
    timerAlarmDisable(timer3);
    motor = true;
    timer3running = false;
  }
  
  // front is prioritary
  if(front){
    changeVibrationFrequency(FRONTAL_DANGER_VALUE_LEVEL);
    RightON = true;
    LeftON = true;
  }

  // closer side
  else if(left && left > right){
    changeVibrationFrequency(STEP_LEVELS[3-left]);
    RightON = false;
    LeftON= true;
  }
  else if(right && right > left){
    changeVibrationFrequency(STEP_LEVELS[3-right]);
    RightON = true;
    LeftON= false;
  }
  // if same distance last side
  else if(left && lastLeft){
    changeVibrationFrequency(STEP_LEVELS[3-left]);
    RightON = false;
    LeftON= true;
  }
  else if(right && !lastLeft){
    changeVibrationFrequency(STEP_LEVELS[3-right]);
    RightON = true;
    LeftON= false;
  }
  // off
  else{
    RightON = false;
    LeftON= false;
  }
}

#endif // _m5atom_tfi_h_