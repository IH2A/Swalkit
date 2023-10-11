#define M5STACK_MPU6886
#include <Arduino.h>
#include <M5Stack.h>
#include "runningAverage.h"
// #include "M5StackSprites.h"
#include <Wire.h>
// #include <Preferences.h>
// #include "deambulateurPrefs.h"
#include "DacESP32.h"
// #include "PWMFrequencyControl.h"
#include "TCA9548A.h"
#include "Adafruit_VL53L0X.h"
#include "FastLED.h"
// #include "BluetoothSerial.h"
// #include "fileIODefinition.h"
#include "M5Timer.h"
#include <vector>

#define MIN(A, B) ((A > B) ? (B) : (A))
#define MAX(A, B) ((A < B) ? (B) : (A))
#define CLIP(A, AMIN, AMAX) (MAX(MIN((A), (AMAX)), (AMIN)))

/********************************************/
// CONSTANTS AND DEFINITIONS
/********************************************/

/************************************************************/
// Control Variables
/************************************************************/

bool secondHub = true; // needs to be changed to false if only one hub is used

bool LEDsEnabled = true;
bool VibrationEnabled = true;
bool BluetoothEnabled = false;
bool DebugEnabled = false;
bool IMUEnabled = false;
/************************************************************/
// SENSORS
/************************************************************/
const unsigned char RA_SIZE = 2;      // RunningAverage size for the sensors
const uint16_t SENSOR_MAX_VALUE = 80; // cm
uint8_t TCA9548A_ADDRESS = 0x70;      // Hardware defined
uint8_t TCA9548A_2_ADDRESS = 0x73;
uint8_t NUMBER_OF_SENSORS = 8;
// 1 Hub -> use all channels
std::vector<unsigned char> HUB_I2C_SENSORS_CHANNELS = {
    TCA_CHANNEL_0,
    TCA_CHANNEL_1,
    TCA_CHANNEL_2,
    TCA_CHANNEL_3,
    TCA_CHANNEL_4,
    TCA_CHANNEL_5,
    TCA_CHANNEL_6,
    TCA_CHANNEL_7};
const std::vector<unsigned char> HUB_SENSORS_ADDRS = {
    0x2A, 0x2B, 0x2C, 0x2D, 0x2F, 0x2E, 0x29, 0x30};
// 2 Hubs
std::vector<unsigned char> HUB_1_I2C_SENSORS_CHANNELS = {
    TCA_CHANNEL_0,
    TCA_CHANNEL_1,
    TCA_CHANNEL_2,
    TCA_CHANNEL_3,
};
std::vector<unsigned char> HUB_2_I2C_SENSORS_CHANNELS = {
    TCA_CHANNEL_3,
    TCA_CHANNEL_2,
    TCA_CHANNEL_1,
    TCA_CHANNEL_0,
};
const std::vector<unsigned char> HUB_1_SENSORS_ADDRS = {
    0x2A, 0x2B, 0x2C, 0x2D};
const std::vector<unsigned char> HUB_2_SENSORS_ADDRS = {
    0x2F, 0x2E, 0x29, 0x30};

std::vector<uint16_t> vector_sensorAngles = {
    11, 34, 56, 79, 101, 124, 146, 169};
std::vector<int8_t> offsetSensors = {
    40, -40};

std::vector<Adafruit_VL53L0X *> vectorVL53L0X; // Vector with Sensor devices

uint8_t currentI2CHubPort = 0;
int lastReadingValue = 0;

std::vector<RunningAverage<int> *> RA_vector; // Running average vector, hold the current reading values

int ChannelLeft = 0;
int ChannelRight = 1;

// void getHubAddress();
void initSensors(); // Initialises the moving average and sensor task related variables
// void countSensors();
void readSensors(); // Read each sensor port of the Hub I2C

/************************************************************/
// MOTORS
/************************************************************/
// Motor Pins

// 2 pins way
// const int IN1_PIN = 16;
// const int IN2_PIN = 17;

// 4 pins way with ref pins
const double freq_const = 40000;
const uint8_t resolution = 10;
const uint8_t pins[2] = {16, 17};   // used for pwm ledcwrite
const uint8_t ref_pins[2] = {2, 5}; // used as ref

// const int mask = (1 << 1) | (1 << 17) | (1 << 5) | (1 << 26) | (1 << 36) ;
int mask = 0;

uint16_t dmax = 1000;          // 450~2V
uint16_t duty[2] = {100, 100}; // percentage
uint16_t duty_max[2] = {dmax, dmax};

int nextWaveLevelLeft = 0;
int nextWaveLevelRight = 0;
int nextWaveLevelFront = 0;

uint16_t nextWaveValue = 0;
bool nextWaveDebug = false;
bool nextWaveRightMotor = false;
bool nextWaveLeftMotor = false;
bool nextWaveForward = false;
bool nextWaveLeftDebug = false;
bool nextWaveRightDebug = false;

int nextWaveRightLevel = 0;
int nextWaveLeftLevel = 0;
bool lastLeft = false;

// ushort FRONTAL_DANGER_VALUE_LEVEL = 90;
ushort FRONTAL_DANGER_VALUE_LEVEL = 100;
ushort FRONTAL_PULSE_VALUE_LEVEL = 50;
ushort DISTANCE_FORWARD_DANGER = 75; // cm

// ushort STEP_LEVELS[3] = {90, // Frequencies
//                          65,
//                          0};
ushort STEP_LEVELS[3] = {100, // % of duty
                         80,
                         65};
#define OFF 0
std::vector<uint16_t> DISTANCES_LEVELS = {25,  // Danger
                                          55,  // Near
                                          75}; // Far

ushort PULSE_LEVELS[3] = {0, 0, 0};

void setupWaveValuesFromSensors();

void waveLMA(ushort left, ushort right, ushort front, ushort pulse = 0);
void playWaveValues();

// Pulsing parameters
// In ms
double PULSATION_TIME = 0;
double LAST_PULSATION = 0;
bool PULSING = false;

/**************Bluetooth****************************/

// int BluetoothTime = 500; // ms
// M5Timer timer2;
// void readBluetoothData();
// void ISRBlueetoothWriteSensors();
// bool SendBluetooth = false;

// void IRAM_ATTR onTimer2() {
//   SendBluetooth = true;
// }
int base_freq = 70; // Hz
int base_ms = int(round(1 / float(base_freq) * 1000));
int base_signal = 1;

M5Timer timer2;
void run_pwm();
void IRAM_ATTR onTimer2()
{
  base_signal *= -1;
  run_pwm();
}

// M5StackSprites m5StackSprites;
// const char *BLUETOOTH_DEVICE_NAME = "Demo SWALKIT";
// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif
// String serialCommandData = ""; // Holds last bluetooth lecture
// // void readBluetoothData();
// void serialCommandOptions(int serialCommand, String serialCommandData);

/************************************************************/
// PREFERENCES FILES
/************************************************************/

// Preferences prefs;
// Profile *profile;
// uint8_t ProfileNr = 1;

// void switchProfile();
// void initProfile();

/************************************************************/
// I2C HUB Configuration
/************************************************************/
#ifdef SOFTWAREWIRE
#include <SoftwareWIRE.h>
SoftwareWire myWIRE(3, 2);
TCA9548A<SoftwareWire> TCA_HUB;
TCA9548A<SoftwareWire> TCA_HUB_2;
#define WIRE myWIRE
#else
TCA9548A<TwoWire> TCA_HUB;
TCA9548A<TwoWire> TCA_HUB_2;
#endif

/************************************************************/
// IMU
/************************************************************/
// float accX = 0.0F; // Define variables for storing inertial sensor data
// float accY = 0.0F;
// float accZ = 0.0F;

// const unsigned char RA_IMU_SIZE = 10;
// RunningAverage<float> RA_IMU(RA_IMU_SIZE);
// float MOVEMENT_TRIGGER = 0.1;
// float IMU_Calibration_Data;
// int no_Movement_Timer = 1000; // Vibration stops after 2s without movement
// M5Timer timer3;
// bool Movement = true;

// void IRAM_ATTR TimerHandler3()
// {
//   Movement = false;
// }


void printScreenSensorsTask(void *pvParameters)
{
  while (1)
  {
    for (int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setCursor(0, (i + 1) * 20);
      M5.Lcd.print("Sensor ");
      M5.Lcd.print(i + 1);
      M5.Lcd.print(": ");
      M5.Lcd.setCursor(120, (i + 1) * 20);
      if (RA_vector[i]->getAverage() < 10)
        M5.Lcd.print("0");
      M5.Lcd.print(RA_vector[i]->getAverage());
    }
    delay(300);
  }
}

/************************************************************/
// LEDs
/************************************************************/
#define LED_PORT 26
#define NUM_LEDS 5
#define COLOR_ORDER GRB
#define CHIPSET WS2811
#define BRIGHTNESS 10
#define FRAMES_PER_SECOND 60

// RGB colors
#define COLOR_DANGER 0xFF0F08
#define COLOR_LEVEL_2 0XFF6F0F
#define COLOR_LEVEL_1 0xFFBF0F
#define COLOR_LEVEL_0 0xAFFF0F

int freq = 40000;
bool direction = true;

CRGB leds[NUM_LEDS]; // Array of leds
CFastLED LED_STRIP;

std::vector<uint8_t> SensorData;

void setLedValues();

void Buttons(void);

void readSensorsTask(void *pvParameters)
{
  while (1)
  {
    TCA_HUB.openAll();
    if (secondHub)
      TCA_HUB_2.openAll();

    readSensors();

    TCA_HUB.closeAll();
    TCA_HUB_2.closeAll();
    setupWaveValuesFromSensors();
    setLedValues();
    // delay(10);
  }
}
/********************************************
 * SET UP
 ********************************************/

void setup()
{

  M5.begin(true, false, true, true); // (bool LCDEnable = true, bool SDEnable = true, bool SerialEnable = true, bool I2CEnable = false)
  M5.Power.begin();
  M5.Power.setPowerVin(false);
  M5.Speaker.end();

  // // Bluetooth Timer
  // timer2.setInterval(BluetoothTime, onTimer2);
  // timer2.disable(2);
  timer2.setInterval(base_ms, onTimer2);
  timer2.enable(2);

  // Mouvement Timer
  // timer3.setInterval(no_Movement_Timer, TimerHandler3);
  // if (IMUEnabled)
  //   timer3.enable(3);
  // else
  //   timer3.disable(3);

  M5.Lcd.setTextSize(2);

  M5.Lcd.println("Starting the system...");

  TCA_HUB.begin(Wire, TCA9548A_ADDRESS);
  TCA_HUB.openAll();
  if (secondHub)
  {
    TCA_HUB_2.begin(Wire, TCA9548A_2_ADDRESS);
    TCA_HUB_2.openAll();
  }

  M5.Lcd.println("Configuring sensors");
  initSensors();

  // initialise LEDs and switch off
  pinMode(LED_PORT,OUTPUT);
  FastLED.addLeds<WS2811, LED_PORT, COLOR_ORDER>(leds, NUM_LEDS);

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = 0x000000;
  }
  FastLED.show();
  FastLED.setBrightness(BRIGHTNESS);

  // set up motors
  // old 2 pins mode
  // ledcSetup(ChannelLeft, freq, resolution);
  // ledcSetup(ChannelRight, freq, resolution);
  // ledcAttachPin(IN1_PIN, ChannelLeft);
  // ledcAttachPin(IN2_PIN, ChannelRight);

  // ledcWriteTone(ChannelRight, 0);
  // ledcWriteTone(ChannelLeft, 0);

  // using pins as ref
  gpio_config_t io_conf;
  io_conf.mode = GPIO_MODE_OUTPUT;
  for (size_t i = 0; i < 2; i++)
  {
    mask |= (1 << ref_pins[i]);
  }

  io_conf.pin_bit_mask = mask;

  gpio_config(&io_conf);

  for (int channel = 0; channel < 2; channel++)
  {
    ledcSetup(channel + 8, freq, resolution);  //+8 for high speed channels
    ledcAttachPin(pins[channel], channel + 8); //+8 for high speed channels
  }

  // if (IMUEnabled)
  // {
  //   M5.IMU.Init();
  //   M5.Lcd.printf("calibrating IMU, do not move!\n");
  //   RA_IMU.fillValue(1.5, RA_IMU_SIZE); // 1.5 As default
  //   for (int i = 0; i < RA_IMU_SIZE; i++)
  //   {
  //     M5.IMU.getAccelData(&accX, &accY, &accZ);
  //     RA_IMU.addValue(abs(accX) + abs(accY) + abs(accZ));
  //     delay(50);
  //   }
  //   IMU_Calibration_Data = RA_IMU.getAverage();
  // }

  delay(200);

  M5.Lcd.clear();

  // initProfile();

  // prefs.begin("user_profile", false);
  M5.Lcd.setCursor(0, 0);
  // M5.Lcd.println("Profile 1: " + prefs.getString("fileName"));
  // prefs.end();

  M5.Lcd.setTextColor(BLUE, BLACK);
  M5.Lcd.setCursor(10, 210);
  M5.Lcd.print("Bluetooth");

  if (!LEDsEnabled)
    M5.Lcd.setTextColor(ORANGE, BLACK);
  else
    M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setCursor(135, 210);
  M5.Lcd.print("LEDs");

  if (!VibrationEnabled)
    M5.Lcd.setTextColor(ORANGE, BLACK);
  else
    M5.Lcd.setTextColor(GREEN, BLACK);
  M5.Lcd.setCursor(135, 190);
  M5.Lcd.print("Vibration");

  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(220, 210);
  M5.Lcd.print("Profile");

  delay(100);
  xTaskCreatePinnedToCore(readSensorsTask, "readSensorsTask", 4096, NULL, 1, NULL, 0);
  // xTaskCreatePinnedToCore(printScreenSensorsTask, "printScreenSensorsTask", 4096, NULL, 2, NULL, 0);
}

/********************************************
 * LOOP
 ********************************************/

void loop()
{
  // timer3.run();
  timer2.run();

  // // Read Sensors and print to display
  // if(SendBluetooth && BluetoothEnabled){
  //   readBluetoothData();
  //   ISRBlueetoothWriteSensors();
  // }

  // TCA_HUB.openAll();
  // if(secondHub)
  //   TCA_HUB_2.openAll();

  // readSensors();

  // for (int i = 0; i < NUMBER_OF_SENSORS; i++)
  // {
  //   M5.Lcd.setTextColor(WHITE, BLACK);
  //   M5.Lcd.setCursor(0, (i + 1) * 20);
  //   M5.Lcd.print("Sensor ");
  //   M5.Lcd.print(i + 1);
  //   M5.Lcd.print(": ");
  //   M5.Lcd.setCursor(120, (i + 1) * 20);
  //   if (RA_vector[i]->getAverage() < 10)
  //     M5.Lcd.print("0");
  //   M5.Lcd.print(RA_vector[i]->getAverage());
  // }

  // TCA_HUB.closeAll();
  // TCA_HUB_2.closeAll();

  // set LEDs
  // if (LEDsEnabled)
  //   setLedValues();

  // set Vibrations
  // if (VibrationEnabled)
  // {
  // setupWaveValuesFromSensors();
  // }
}

void initSensors()
{
  if (secondHub)
  {
    for (int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
      RA_vector.push_back(new RunningAverage<int>(RA_SIZE));
      RA_vector[i]->fillValue(80, RA_SIZE);
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
      vectorVL53L0X.push_back(new Adafruit_VL53L0X());
    }

    for (int i = 0; i < NUMBER_OF_SENSORS / 2; i++)
    {
      TCA_HUB.closeAll();
      TCA_HUB_2.closeAll();
      TCA_HUB.openChannel(HUB_1_I2C_SENSORS_CHANNELS[i]);
      if (!vectorVL53L0X[i]->begin(HUB_1_SENSORS_ADDRS[i]))
      {
        Serial.println(F("Failed to boot VL53L0X"));
      }
    }

    for (int i = 0; i < NUMBER_OF_SENSORS / 2; i++)
    {
      TCA_HUB.closeAll();
      TCA_HUB_2.closeAll();
      TCA_HUB_2.openChannel(HUB_2_I2C_SENSORS_CHANNELS[i]);

      if (!vectorVL53L0X[i + NUMBER_OF_SENSORS / 2]->begin(HUB_2_SENSORS_ADDRS[i]))
      {
        Serial.println(F("Failed to boot VL53L0X"));
      }
    }

    TCA_HUB.openAll();
    TCA_HUB_2.openAll();
  }

  else
  {
    for (int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
      RA_vector.push_back(new RunningAverage<int>(RA_SIZE));
      RA_vector[i]->fillValue(80, RA_SIZE);
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
      vectorVL53L0X.push_back(new Adafruit_VL53L0X());
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i++)
    {
      TCA_HUB.closeAll();
      TCA_HUB.openChannel(HUB_I2C_SENSORS_CHANNELS[i]);
      delay(10);
      if (vectorVL53L0X[i]->begin(0x29))
      {

        vectorVL53L0X[i]->setAddress(HUB_SENSORS_ADDRS[i]);
      }
      else
      {

        vectorVL53L0X[i]->begin(HUB_SENSORS_ADDRS[i]);
      }
    }

    TCA_HUB.openAll();
  }
}

void readSensors()
{
  VL53L0X_RangingMeasurementData_t measure;
  for (int i = 0; i < vectorVL53L0X.size(); i++)
  {

    vectorVL53L0X[i]->getSingleRangingMeasurement(&measure, false);
    if (measure.RangeStatus != 4)
    { // phase failures have incorrect data
      RA_vector[i]->addValue(measure.RangeMilliMeter < SENSOR_MAX_VALUE * 10 ? measure.RangeMilliMeter / 10 : SENSOR_MAX_VALUE);
    }
    else
    {
      RA_vector[i]->addValue(SENSOR_MAX_VALUE);
    }
  }
}

void setLedValues()
{

  int s = NUMBER_OF_SENSORS % 2; // 1 if uneven number
  int ledLevel = 0;

  // Left
  for (int i = 0; i < NUMBER_OF_SENSORS / 2 - 1; i++)
  {
    if (RA_vector[i]->getAverage() < DISTANCES_LEVELS[0])
    {
      ledLevel = 3;
      leds[0] = COLOR_DANGER;
      leds[1] = COLOR_DANGER;
    }
    else if (RA_vector[i]->getAverage() < DISTANCES_LEVELS[1] && ledLevel < 2)
    {
      ledLevel = 2;
      leds[0] = COLOR_LEVEL_2;
      leds[1] = COLOR_LEVEL_2;
    }
    else if (RA_vector[i]->getAverage() < DISTANCES_LEVELS[2] && ledLevel < 1)
    {
      ledLevel = 1;
      leds[0] = COLOR_LEVEL_1;
      leds[1] = COLOR_LEVEL_1;
    }
    else if (ledLevel == 0)
    {
      leds[0] = COLOR_LEVEL_0;
      leds[1] = COLOR_LEVEL_0;
    }
  }

  // center
  if (RA_vector[NUMBER_OF_SENSORS / 2 - 1 + s]->getAverage() < DISTANCES_LEVELS[0] || RA_vector[NUMBER_OF_SENSORS / 2]->getAverage() < DISTANCES_LEVELS[0])
  {
    leds[2] = COLOR_DANGER;
  }
  else if ((RA_vector[NUMBER_OF_SENSORS / 2 - 1 + s]->getAverage() < DISTANCES_LEVELS[1] || RA_vector[NUMBER_OF_SENSORS / 2]->getAverage() < DISTANCES_LEVELS[1]) && ledLevel < 2)
  {
    leds[2] = COLOR_LEVEL_2;
  }
  else if ((RA_vector[NUMBER_OF_SENSORS / 2 - 1 + s]->getAverage() < DISTANCES_LEVELS[2] || RA_vector[NUMBER_OF_SENSORS / 2]->getAverage() < DISTANCES_LEVELS[2]) && ledLevel < 1)
  {
    leds[2] = COLOR_LEVEL_1;
  }
  else
  {
    leds[2] = COLOR_LEVEL_0;
  }

  ledLevel = 0;

  // right
  for (int i = NUMBER_OF_SENSORS / 2 + 1; i < NUMBER_OF_SENSORS; i++)
  {
    if (RA_vector[i]->getAverage() < DISTANCES_LEVELS[0])
    {
      ledLevel = 3;
      leds[3] = COLOR_DANGER;
      leds[4] = COLOR_DANGER;
    }
    else if (RA_vector[i]->getAverage() < DISTANCES_LEVELS[1] && ledLevel <= 2)
    {
      ledLevel = 2;
      leds[3] = COLOR_LEVEL_2;
      leds[4] = COLOR_LEVEL_2;
    }
    else if (RA_vector[i]->getAverage() < DISTANCES_LEVELS[2] && ledLevel <= 1)
    {
      ledLevel = 1;
      leds[3] = COLOR_LEVEL_1;
      leds[4] = COLOR_LEVEL_1;
    }
    else if (ledLevel == 0)
    {
      leds[3] = COLOR_LEVEL_0;
      leds[4] = COLOR_LEVEL_0;
    }
  }

  FastLED.show();
}

void setupWaveValuesFromSensors()
{

  // for 8 sensors

  short front = 0;
  short rightLevel = 0;
  short leftLevel = 0;
  short pulse = 0;

  // center
  if ((RA_vector[2]->getAverage() < DISTANCE_FORWARD_DANGER && RA_vector[5]->getAverage() < DISTANCE_FORWARD_DANGER) || RA_vector[3]->getAverage() < DISTANCE_FORWARD_DANGER || RA_vector[4]->getAverage() < DISTANCE_FORWARD_DANGER)
  {
    front = FRONTAL_DANGER_VALUE_LEVEL;
    pulse = 4;
  }

  // left side
  if (RA_vector[5]->getAverage() < DISTANCES_LEVELS[0] ||
      RA_vector[6]->getAverage() < DISTANCES_LEVELS[0] ||
      RA_vector[7]->getAverage() < DISTANCES_LEVELS[0])
  {
    leftLevel = STEP_LEVELS[0];
    pulse = pulse > 3 ? pulse : 3; // if pulse > 3 then pulse = pulse else pulse = 3
  }
  else if (RA_vector[5]->getAverage() < DISTANCES_LEVELS[1] ||
           RA_vector[6]->getAverage() < DISTANCES_LEVELS[1] ||
           RA_vector[7]->getAverage() < DISTANCES_LEVELS[1])
  {
    leftLevel = STEP_LEVELS[1];
    pulse = pulse > 2 ? pulse : 2;
  }
  else if (RA_vector[5]->getAverage() < DISTANCES_LEVELS[2] ||
           RA_vector[6]->getAverage() < DISTANCES_LEVELS[2] ||
           RA_vector[7]->getAverage() < DISTANCES_LEVELS[2])
  {
    leftLevel = STEP_LEVELS[2];
    pulse = pulse > 1 ? pulse : 1;
  }
  else
  {
    leftLevel = 0;
  }

  // right side
  if (RA_vector[0]->getAverage() < DISTANCES_LEVELS[0] ||
      RA_vector[1]->getAverage() < DISTANCES_LEVELS[0] ||
      RA_vector[2]->getAverage() < DISTANCES_LEVELS[0])
  {
    rightLevel = STEP_LEVELS[0];
    pulse = pulse > 3 ? pulse : 3;
  }
  else if (RA_vector[0]->getAverage() < DISTANCES_LEVELS[1] ||
           RA_vector[1]->getAverage() < DISTANCES_LEVELS[1] ||
           RA_vector[2]->getAverage() < DISTANCES_LEVELS[1])
  {
    rightLevel = STEP_LEVELS[1];
    pulse = pulse > 2 ? pulse : 2;
  }
  else if (RA_vector[0]->getAverage() < DISTANCES_LEVELS[2] ||
           RA_vector[1]->getAverage() < DISTANCES_LEVELS[2] ||
           RA_vector[2]->getAverage() < DISTANCES_LEVELS[2])
  {
    rightLevel = STEP_LEVELS[2];
    pulse = pulse > 1 ? pulse : 1;
  }
  else
  {
    rightLevel = 0;
  }

  switch (pulse)
  {
  case 4:
    pulse = FRONTAL_PULSE_VALUE_LEVEL;
    break;
  case 3:
    pulse = PULSE_LEVELS[0];
    break;
  case 2:
    pulse = PULSE_LEVELS[1];
    break;
  case 1:
    pulse = PULSE_LEVELS[2];
    break;
  default:
    pulse = 0;
    break;
  }

  waveLMA(leftLevel, rightLevel, front, pulse);
}

/**
 * Function to command the vibration of the two LMA actuators.
 */
void waveLMA(ushort left, ushort right, ushort front, ushort pulse)
{
  // If pulsing is enabled
  if (pulse != 0)
  {

    PULSATION_TIME = pulse;

    if (PULSING)
    {
      left = 0;
      right = 0;
      front = 0;
    }

    double current = double(millis());
    if (abs(current - LAST_PULSATION) > PULSATION_TIME)
    {
      PULSING = !PULSING;
      LAST_PULSATION = current;
    }
  }

  // //Wave for frontal collision is prioritary
  if (front)
  {
    // ledcWriteTone(ChannelLeft,front);
    // ledcWriteTone(ChannelRight,front);
    duty[0] = front;
    duty[1] = front;
    return;
  }

  // Vibrate on the side closest to the obstacle
  if (left > right)
  {
    duty[0] = left;
    duty[1] = 0;
    // ledcWriteTone(ChannelLeft, left);
    // ledcWriteTone(ChannelRight,0);
    lastLeft = true;
  }
  else if (right > left)
  {
    // ledcWriteTone(ChannelLeft, 0);
    // ledcWriteTone(ChannelRight, right);
    duty[0] = 0;
    duty[1] = right;
    lastLeft = false;
  }
  else if (left == right && left > 0)
  {
    // If we are in an uncertain state (impropable but might happen), vibrate on the last side detected previously
    //  lastLeft ? ledcWriteTone(ChannelLeft, left) + ledcWriteTone(ChannelRight,0) : ledcWriteTone(ChannelRight, right) + ledcWriteTone(ChannelLeft, 0);
    if (lastLeft)
    {
      duty[0] = left;
      duty[1] = 0;
    }
    else
    {
      duty[0] = 0;
      duty[1] = right;
    }
  }
  else
  {

    duty[0] = 0;
    duty[1] = right;
  }
}

void run_pwm()
{
  int set_ref_pin = 0;
  int clear_ref_pin = 0;

  uint16_t unsigned_duty[2];
  int base_signal_multiplier = (duty[0] == 0 && duty[1] == 0) ? 0 : 1;

  for (size_t i = 0; i < 2; i++)
  {

    // reference pins
    if (base_signal * base_signal_multiplier >= 0)
    {
      unsigned_duty[i] = CLIP(uint16_t(duty[i] * 1023 / 100), 0, dmax);
      clear_ref_pin = clear_ref_pin | (1 << ref_pins[i]);
    }
    else
    {
      unsigned_duty[i] = CLIP(uint16_t(1023 - (duty[i] * 1023 / 100)), 1023 - dmax, 1023);
      set_ref_pin = set_ref_pin | (1 << ref_pins[i]);
    }
  }

  GPIO.out_w1tc = mask & clear_ref_pin; // clear
  GPIO.out_w1ts = mask & set_ref_pin;   // set
  for (int channel = 0; channel < 2; channel++)
  {
    ledcWrite(channel + 8, unsigned_duty[channel]);
  }
}
