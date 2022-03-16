#define M5STACK_MPU6886
#include <Arduino.h>
#include <M5Stack.h>
#include "runningAverage.h"
#include "M5StackSprites.h"
#include <Wire.h>
#include <Preferences.h>
#include "deambulateurPrefs.h"
#include "DacESP32.h"
#include "PWMFrequencyControl.h"
#include "TCA9548A.h"
#include "Adafruit_VL53L0X.h"
#include "FastLED.h"
#include "BluetoothSerial.h"
#include "fileIODefinition.h"
#include "M5Timer.h"

/********************************************/
// CONSTANTS AND DEFINITIONS
/********************************************/

/************************************************************/
// Control Variables
/************************************************************/

bool secondHub = true; // needs to be changed to false if only one hub is used

bool LEDsEnabled = false;
bool VibrationEnabled = true;
bool BluetoothEnabled = false;
bool DebugEnabled = false;
bool IMUEnabled = true;
/************************************************************/
// SENSORS
/************************************************************/
const unsigned char RA_SIZE = 2;  //RunningAverage size for the sensors
const uint16_t SENSOR_MAX_VALUE = 80; //cm
uint8_t TCA9548A_ADDRESS = 0x70;  // Hardware defined
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
  TCA_CHANNEL_7
  };
const std::vector<unsigned char> HUB_SENSORS_ADDRS = {
   0x2A, 0x2B, 0x2C, 0x2D, 0x2F, 0x2E, 0x29, 0x30
};
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
  0x2A, 0x2B, 0x2C, 0x2D
};
const std::vector<unsigned char> HUB_2_SENSORS_ADDRS = {
  0x2F, 0x2E, 0x29, 0x30
};


std::vector<uint16_t> vector_sensorAngles = {
  11, 34, 56, 79, 101, 124, 146, 169
};
std::vector<int8_t> offsetSensors ={
  40, -40
};

std::vector<Adafruit_VL53L0X*> vectorVL53L0X; // Vector with Sensor devices

uint8_t currentI2CHubPort = 0;
int lastReadingValue = 0;

std::vector<RunningAverage<int>*> RA_vector; // Running average vector, hold the current reading values

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
const int IN1_PIN = 16;
const int IN2_PIN = 17;

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

ushort FRONTAL_DANGER_VALUE_LEVEL = 90;
ushort FRONTAL_PULSE_VALUE_LEVEL = 300;
ushort DISTANCE_FORWARD_DANGER = 75;// cm 

ushort STEP_LEVELS[3] = { 90,    // Frequencies
                          65,
                          0};
#define OFF 0
std::vector<uint16_t> DISTANCES_LEVELS = {35,  // Danger
                                          55,  // Near
                                          75}; // Far

ushort PULSE_LEVELS[3] = {0,0,0};

void setupWaveValuesFromSensors();

void waveLMA(ushort left, ushort right, ushort front, ushort pulse = 0);
void playWaveValues();


//Pulsing parameters
//In ms
unsigned long PULSATION_TIME = 0;
unsigned long LAST_PULSATION = 0;
bool PULSING = false;


/**************Bluetooth****************************/

int BluetoothTime = 500; // ms
M5Timer timer2;
void readBluetoothData();
void ISRBlueetoothWriteSensors();
bool SendBluetooth = false;

void IRAM_ATTR onTimer2() {
  SendBluetooth = true;
}

M5StackSprites m5StackSprites;
const char *BLUETOOTH_DEVICE_NAME = "Fauteuil IRISA";
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
String serialCommandData = ""; // Holds last bluetooth lecture
void readBluetoothData();
void serialCommandOptions(int serialCommand, String serialCommandData);

/************************************************************/
// PREFERENCES FILES
/************************************************************/

Preferences prefs;
Profile* profile;
uint8_t ProfileNr = 1;

void switchProfile();
void initProfile(); 

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
float accX = 0.0F;  // Define variables for storing inertial sensor data
float accY = 0.0F;
float accZ = 0.0F;

const unsigned char RA_IMU_SIZE = 10;
RunningAverage<float> RA_IMU(RA_IMU_SIZE);
float MOVEMENT_TRIGGER = 0.1; 
float IMU_Calibration_Data;
int no_Movement_Timer = 1000; // Vibration stops after 2s without movement
M5Timer timer3;
bool Movement = true;

void IRAM_ATTR TimerHandler3(){
  Movement = false;
}

/************************************************************/
// LEDs
/************************************************************/
#define LED_PORT    26
#define NUM_LEDS    5
#define COLOR_ORDER GRB
#define CHIPSET     WS2811
#define BRIGHTNESS  10
#define FRAMES_PER_SECOND 60

// RGB colors
#define COLOR_DANGER  0xFF0F08
#define COLOR_LEVEL_2 0XFF6F0F
#define COLOR_LEVEL_1 0xFFBF0F
#define COLOR_LEVEL_0 0xAFFF0F

int freq = 10000;
int resolution = 10;
bool direction = true;

CRGB leds[NUM_LEDS];  // Array of leds
CFastLED LED_STRIP;

std::vector<uint8_t> SensorData;

void setLedValues();

void Buttons(void);

/********************************************
 * SET UP
 ********************************************/

void setup() {

  M5.begin(true, true, true, true); // (bool LCDEnable = true, bool SDEnable = true, bool SerialEnable = true, bool I2CEnable = false)
  M5.Power.begin();
  M5.Speaker.end();

  // Bluetooth Timer
  timer2.setInterval(BluetoothTime, onTimer2);
  timer2.disable(2);

  // Mouvement Timer
  timer3.setInterval(no_Movement_Timer, TimerHandler3);
  if(IMUEnabled)
    timer3.enable(3);
  else
    timer3.disable(3);

  M5.Lcd.setTextSize(2);

  M5.Lcd.println("Starting the system...");
  
  TCA_HUB.begin(Wire, TCA9548A_ADDRESS);
  TCA_HUB_2.begin(Wire, TCA9548A_2_ADDRESS);

  TCA_HUB.openAll();

  M5.Lcd.println("Configuring sensors");
  initSensors();

  // initialise LEDs and switch off
  FastLED.addLeds<WS2811, LED_PORT , COLOR_ORDER>(leds, NUM_LEDS);

  for(int i = 0; i < NUM_LEDS; i++){
    leds[i] = 0x000000;
  }
  FastLED.show();
  FastLED.setBrightness(BRIGHTNESS);

  // set up motors
  ledcSetup(ChannelLeft, freq, resolution);
  ledcSetup(ChannelRight, freq, resolution);
  ledcAttachPin(IN1_PIN, ChannelLeft);
  ledcAttachPin(IN2_PIN, ChannelRight);

  ledcWriteTone(ChannelRight, 0);
  ledcWriteTone(ChannelLeft, 0);

  if(IMUEnabled){
    M5.IMU.Init();
    M5.Lcd.printf("calibrating IMU, do not move!\n");
    RA_IMU.fillValue(1.5, RA_IMU_SIZE); // 1.5 As default 
    for (int i = 0; i < RA_IMU_SIZE; i ++) {
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      RA_IMU.addValue(abs(accX)+abs(accY)+abs(accZ));
      delay(50);
    }
    IMU_Calibration_Data = RA_IMU.getAverage();
  }

  delay(200);

  M5.Lcd.clear();

  initProfile();
  
  prefs.begin("user_profile", false);
  M5.Lcd.setCursor(0,0);
  M5.Lcd.println("Profile 1: " + prefs.getString("fileName"));
  prefs.end();

  M5.Lcd.setTextColor(BLUE , BLACK);
  M5.Lcd.setCursor(10, 210);
  M5.Lcd.print("Bluetooth");

  if(!LEDsEnabled)
    M5.Lcd.setTextColor(ORANGE , BLACK);
  else
    M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.setCursor(135, 210);
  M5.Lcd.print("LEDs");

  if(!VibrationEnabled)
    M5.Lcd.setTextColor(ORANGE , BLACK);
  else
    M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.setCursor(135, 190);
  M5.Lcd.print("Vibration");
  
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(220, 210);
  M5.Lcd.print("Profile");
}

/********************************************
 * LOOP
 ********************************************/

void loop() {
  timer3.run();
  timer2.run();

  // Read Sensors and print to display
  if(SendBluetooth && BluetoothEnabled){
    readBluetoothData();
    ISRBlueetoothWriteSensors();
  }

  TCA_HUB.openAll();
  TCA_HUB_2.openAll();

  readSensors();

  for(int i = 0; i < NUMBER_OF_SENSORS; i++)
  {
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(0, (i+1)*20);
    M5.Lcd.print("Sensor ");
    M5.Lcd.print(i+1);
    M5.Lcd.print(": ");
    M5.Lcd.setCursor(120, (i+1)*20);
    if(RA_vector[i]->getAverage() < 10)
      M5.Lcd.print("0");
    M5.Lcd.print(RA_vector[i]->getAverage());
  }
    
  TCA_HUB.closeAll();
  TCA_HUB_2.closeAll();

  // set LEDs
  if(LEDsEnabled)
    setLedValues(); 
  
  // set Vibrations
  if(VibrationEnabled){
    setupWaveValuesFromSensors();
  }

  Buttons();
  
}




void Buttons(void){
  M5.update();
   // if a button was pressed
  if(M5.BtnA.wasReleased()){
    if(!BluetoothEnabled){
      m5StackSprites.drawImage('B');
      BluetoothEnabled = true;
      SerialBT.begin(BLUETOOTH_DEVICE_NAME);
      timer2.enable(2);

      M5.Lcd.setCursor(0,0);
      if(ProfileNr == 1){
        prefs.begin("user_profile", false);
        M5.Lcd.println("Profile 1: " + prefs.getString("fileName"));
        prefs.end();
      }
      else{
        prefs.begin("user_profile", false);
        M5.Lcd.println("Profile 2: " + prefs.getString("fileName2"));
        prefs.end();
      }

      M5.Lcd.setTextColor(BLUE , BLACK);
      M5.Lcd.setCursor(10, 210);
      M5.Lcd.print("Bluetooth");

      if(!LEDsEnabled)
        M5.Lcd.setTextColor(ORANGE , BLACK);
      else
        M5.Lcd.setTextColor(GREEN , BLACK);
      M5.Lcd.setCursor(135, 210);
      M5.Lcd.print("LEDs");

      if(!VibrationEnabled)
        M5.Lcd.setTextColor(ORANGE , BLACK);
      else
        M5.Lcd.setTextColor(GREEN , BLACK);
      M5.Lcd.setCursor(135, 190);
      M5.Lcd.print("Vibration");
      
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setCursor(220, 210);
      M5.Lcd.print("Profile");
    }
    else{
      timer2.disable(2);
      SerialBT.end();
      M5.Lcd.clear();
      BluetoothEnabled = false;

      M5.Lcd.setCursor(0,0);
      if(ProfileNr == 1){
        prefs.begin("user_profile", false);
        M5.Lcd.println("Profile 1: " + prefs.getString("fileName"));
        prefs.end();
      }
      else{
        prefs.begin("user_profile", false);
        M5.Lcd.println("Profile 2: " + prefs.getString("fileName2"));
        prefs.end();
      }

      M5.Lcd.setTextColor(BLUE , BLACK);
      M5.Lcd.setCursor(10, 210);
      M5.Lcd.print("Bluetooth");

      if(!LEDsEnabled)
        M5.Lcd.setTextColor(ORANGE , BLACK);
      else
        M5.Lcd.setTextColor(GREEN , BLACK);
      M5.Lcd.setCursor(135, 210);
      M5.Lcd.print("LEDs");

      if(!VibrationEnabled)
        M5.Lcd.setTextColor(ORANGE , BLACK);
      else
        M5.Lcd.setTextColor(GREEN , BLACK);
      M5.Lcd.setCursor(135, 190);
      M5.Lcd.print("Vibration");
      
      M5.Lcd.setTextColor(WHITE, BLACK);
      M5.Lcd.setCursor(220, 210);
      M5.Lcd.print("Profile");
    }
  }

  else if(M5.BtnB.wasReleased()){
    // nothing > LEDs > Vibration > both > nothing
    if(LEDsEnabled)
      VibrationEnabled = !VibrationEnabled;
    LEDsEnabled = !LEDsEnabled;

    ledcWriteTone(ChannelRight, 0);
    ledcWriteTone(ChannelLeft, 0);

    for(int i = 0; i < NUM_LEDS; i++){
      leds[i] = 0x000000;
    }
    FastLED.show();

    if(!LEDsEnabled)
      M5.Lcd.setTextColor(ORANGE , BLACK);
    else
      M5.Lcd.setTextColor(GREEN , BLACK);
    M5.Lcd.setCursor(135, 210);
    M5.Lcd.print("LEDs");

    if(!VibrationEnabled)
      M5.Lcd.setTextColor(ORANGE , BLACK);
    else
      M5.Lcd.setTextColor(GREEN , BLACK);
    M5.Lcd.setCursor(135, 190);
    M5.Lcd.print("Vibration");

    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setCursor(220, 210);
    M5.Lcd.print("Profile");
  }

  else if(M5.BtnC.wasReleased()){
    switchProfile();
    M5.Lcd.clear();
    if(BluetoothEnabled)
      m5StackSprites.drawImage('B');

    M5.Lcd.setCursor(0,0);
    M5.Lcd.setCursor(0,0);

    if(ProfileNr == 1){
      prefs.begin("user_profile", false);
      M5.Lcd.println("Profile 1: " + prefs.getString("fileName"));
      prefs.end();
    }
    else{
      prefs.begin("user_profile", false);
      M5.Lcd.println("Profile 2: " + prefs.getString("fileName2"));
      prefs.end();
    }

    M5.Lcd.setTextColor(BLUE , BLACK);
    M5.Lcd.setCursor(10, 210);
    M5.Lcd.print("Bluetooth");

    if(!LEDsEnabled)
      M5.Lcd.setTextColor(ORANGE , BLACK);
    else
      M5.Lcd.setTextColor(GREEN , BLACK);
    M5.Lcd.setCursor(135, 210);
    M5.Lcd.print("LEDs");

    if(!VibrationEnabled)
      M5.Lcd.setTextColor(ORANGE , BLACK);
    else
      M5.Lcd.setTextColor(GREEN , BLACK);
    M5.Lcd.setCursor(135, 190);
    M5.Lcd.print("Vibration");
  }
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setCursor(220, 210);
  M5.Lcd.print("Profile");
}


// Interrupt function to constantly read the bluetooth data
void readBluetoothData() {
  if (SerialBT.available()) {
    // Read the whole command send via Bluetooth
    serialCommandData = SerialBT.readString();
    if (!serialCommandData) {
      SerialBT.println("Cannot read command");
    } else {
      // Get wich type of command it represents
      int serialCommand = serialCommandData.substring(0,2).toInt();
      // Get the data that's appended to it that will be used as an atribute to the function
      //  The communication include \n at the end, so it's ignored with -2
      String commandAtributes = serialCommandData.length() > 2 ?
              serialCommandData.substring(2, serialCommandData.length()) :
              "";
      serialCommandOptions(serialCommand, commandAtributes);
    }   
  }
}


void serialCommandOptions(int serialCommand, String serialCommandData) {
  uint16_t waveValue = 0;
  DynamicJsonDocument doc(1500);
  
  switch (serialCommand)
  {
    case 00:  // send current profile
      SerialBT.println("00" + JSONtoStringBT(profile->currentFile->getJSONObject()));
      Serial.println("SERIAL BT\t00" + JSONtoStringBT(profile->currentFile->getJSONObject()));
      break;
    case 01:
      SerialBT.println("01" + JSONtoStringBT(profile->getSensorsJSONObject()));
      Serial.println("01" + JSONtoStringBT(profile->getSensorsJSONObject()));
      break;
    case 02:
      SerialBT.println("02" + JSONtoStringBT(profile->getMotorsJSONObject()));
      Serial.println("02" + JSONtoStringBT(profile->getMotorsJSONObject()));
      break;
    case 41:
      waveValue = serialCommandData.substring(1).toInt();
      if (serialCommandData.charAt(0) == '1') {
        nextWaveDebug = true;
        nextWaveLeftDebug = false;
        nextWaveRightDebug = true;
        nextWaveValue = waveValue;
      } else if (serialCommandData.charAt(0) == '2') {
        nextWaveDebug = true;
        nextWaveLeftDebug = true;
        nextWaveRightDebug = false;
        nextWaveValue = waveValue;
      } else if (serialCommandData.charAt(0) == '0') {
        nextWaveDebug = true;
        nextWaveLeftDebug = true;
        nextWaveRightDebug = true;
        nextWaveValue = waveValue;
      }
      break;
      
    case 80:
      Serial.println(serialCommandData);
      deserializeJson(doc, serialCommandData);

      doc["hapticMotor"].as<JsonArray>();

      if(ProfileNr == 1){  
        prefs.begin("user_profile", false);
        prefs.putString("fileName", doc["fileName"].as<String>());
        prefs.putString("userName", doc["userName"].as<String>());
        prefs.putString("userSurname", doc["userSurname"].as<String>());
        
        profile->currentFile->file = doc["fileName"].as<String>();
        profile->currentFile->name = doc["userName"].as<String>();
        profile->currentFile->surname = doc["userSurname"].as<String>();

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

        prefs.putInt("waveFar", STEP_LEVELS[2]);
        prefs.putInt("waveNear", STEP_LEVELS[1]);
        prefs.putInt("waveDanger", STEP_LEVELS[0]);
        prefs.putInt("pulseFar", PULSE_LEVELS[2]);
        prefs.putInt("pulseNear", PULSE_LEVELS[1]);
        prefs.putInt("pulseDanger", PULSE_LEVELS[0]);
        prefs.putInt("pulseForward", FRONTAL_PULSE_VALUE_LEVEL);
        prefs.putInt("waveForward", FRONTAL_DANGER_VALUE_LEVEL);
        prefs.putInt("distanceForward", DISTANCE_FORWARD_DANGER);
        prefs.putInt("distanceFar", DISTANCES_LEVELS[2]);
        prefs.putInt("distanceNear", DISTANCES_LEVELS[1]);
        prefs.putInt("distanceDanger", DISTANCES_LEVELS[0]);
      
        profile = new Profile(prefs, 1);
        Serial.println("Profile updated :" + JSONtoStringBT(profile->getMotorsJSONObject()));
        prefs.end();
        }

      if(ProfileNr == 2){  
        prefs.begin("user_profile", false);
        prefs.putString("fileName2", doc["fileName"].as<String>());
        prefs.putString("userName2", doc["userName"].as<String>());
        prefs.putString("userSurname2", doc["userSurname"].as<String>());
        
        profile->currentFile->file = doc["fileName"].as<String>();
        profile->currentFile->name = doc["userName"].as<String>();
        profile->currentFile->surname = doc["userSurname"].as<String>();

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

        prefs.putInt("waveFar2", STEP_LEVELS[2]);
        prefs.putInt("waveNear2", STEP_LEVELS[1]);
        prefs.putInt("waveDanger2", STEP_LEVELS[0]);
        prefs.putInt("pulseFar2", PULSE_LEVELS[2]);
        prefs.putInt("pulseNear2", PULSE_LEVELS[1]);
        prefs.putInt("pulseDanger2", PULSE_LEVELS[0]);
        prefs.putInt("pulseForward2", FRONTAL_PULSE_VALUE_LEVEL);
        prefs.putInt("waveForward2", FRONTAL_DANGER_VALUE_LEVEL);
        prefs.putInt("distanceForward2", DISTANCE_FORWARD_DANGER);
        prefs.putInt("distanceFar2", DISTANCES_LEVELS[2]);
        prefs.putInt("distanceNear2", DISTANCES_LEVELS[1]);
        prefs.putInt("distanceDanger2", DISTANCES_LEVELS[0]);
      
        profile = new Profile(prefs, 2);
        Serial.println("Profile 2 updated :" + JSONtoStringBT(profile->getMotorsJSONObject()));
        prefs.end();
        }
      break;
    default:
      break;
  }
}

void initProfile() {
  prefs.begin("user_profile", false); // Creates writable preferences
  
  // Check if it's the first time setup
  // For debug purposes
  // prefs.clear();

  bool file_exist = false;
  try
  {
    file_exist = !(prefs.getString("fileName") == "");
  }
  catch(const std::exception& e)
  {
    //if error, then file does not exist
    file_exist = false;
  }
  
  if(!file_exist) {
  // if(true) {
    printf("No profile found\nWriting base profile\n");
    prefs.putString("fileName","Default file");
    prefs.putString("userName", "User");
    prefs.putString("userSurname","Surname");
    prefs.putInt("numberOfSensors", NUMBER_OF_SENSORS);
    for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
      prefs.putInt(i + "sensorId", i),
      prefs.putBool(i + "sensorStatus", true),
      prefs.putInt(i + "sensorAngle", vector_sensorAngles[i]),
      prefs.putString(i + "sensorLocation", i < NUMBER_OF_SENSORS/2 ? "r" : "l"),
      prefs.putInt(i + "sensorOffset", offsetSensors[i]);
    }
    prefs.putInt("Far", DISTANCES_LEVELS[2]);
    prefs.putInt("Near", DISTANCES_LEVELS[1]);
    prefs.putInt("Danger", DISTANCES_LEVELS[0]);
    prefs.putInt("waveFar", STEP_LEVELS[2]);
    prefs.putInt("waveNear", STEP_LEVELS[1]);
    prefs.putInt("waveDanger", STEP_LEVELS[0]);
    prefs.putInt("pulseFar", PULSE_LEVELS[2]);
    prefs.putInt("pulseNear", PULSE_LEVELS[1]);
    prefs.putInt("pulseDanger", PULSE_LEVELS[0]);
    prefs.putInt("pulseForward", FRONTAL_PULSE_VALUE_LEVEL);
    prefs.putInt("waveForward", FRONTAL_DANGER_VALUE_LEVEL);
    prefs.putInt("Forward", DISTANCE_FORWARD_DANGER);
    
    profile = new Profile(prefs, 1);
  }
  else {
    Serial.println( "Profile found: " + prefs.getString("fileName"));

    profile = new Profile(prefs, 1);
    
    for (size_t i = 0; i < profile->leftMotor->vibrations.size() ; i++)
    {
      if(i == 0) FRONTAL_DANGER_VALUE_LEVEL = (ushort)(profile->leftMotor->vibrations[i]);
      else STEP_LEVELS[i-1] = (ushort)(profile->leftMotor->vibrations[i]);  
    }
    
    for (size_t i = 0; i < profile->leftMotor->pulse.size() ; i++)
    {
      if(i == 0) FRONTAL_PULSE_VALUE_LEVEL = (ushort)(profile->leftMotor->pulse[i]);
      else PULSE_LEVELS[i-1] = (ushort)(profile->leftMotor->pulse[i]);
    }

    for (size_t i = 0; i < profile->leftMotor->distances.size() ; i++)
    {
      if(i == 0) DISTANCE_FORWARD_DANGER = (ushort)(profile->leftMotor->distances[i]);
      else DISTANCES_LEVELS[i-1] = (ushort)(profile->leftMotor->distances[i]);
    }

  }
  prefs.end();

  printf("Currently loaded profile\n");
  Serial.println(JSONtoStringBT(profile->currentFile->getJSONObject()));
  printf("Current sensor configuration\n");
  Serial.println(JSONtoStringBT(profile->getSensorsJSONObject()));
  printf("Current motor configuration\n");
  Serial.println(JSONtoStringBT(profile->getMotorsJSONObject()));
}


void initProfile2() {
  prefs.begin("user_profile", false); // Creates writable preferences
  
  // Check if it's the first time setup
  // For debug purposes
  // prefs.clear();

  bool file_exist = false;
  try
  {
    file_exist = !(prefs.getString("fileName2") == "");
  }
  catch(const std::exception& e)
  {
    //if error, then file does not exist
    file_exist = false;
  }
  
  if(!file_exist) {
  // if(true) {
    printf("No second profile found\nWriting base profile\n");
    prefs.putString("fileName2","Default file 2");
    prefs.putString("userName2", "User");
    prefs.putString("userSurname2","Surname");
    prefs.putInt("numberOfSensors2", NUMBER_OF_SENSORS);
    for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
      prefs.putInt(i + "sensorId2", i),
      prefs.putBool(i + "sensorStatus2", true),
      prefs.putInt(i + "sensorAngle2", vector_sensorAngles[i]),
      prefs.putString(i + "sensorLocation2", i < NUMBER_OF_SENSORS/2 ? "r" : "l"),
      prefs.putInt(i + "sensorOffset2", offsetSensors[i]);
    }
    prefs.putInt("Far2", DISTANCES_LEVELS[2]);
    prefs.putInt("Near2", DISTANCES_LEVELS[1]);
    prefs.putInt("Danger2", DISTANCES_LEVELS[0]);
    prefs.putInt("waveFar2", STEP_LEVELS[2]);
    prefs.putInt("waveNear2", STEP_LEVELS[1]);
    prefs.putInt("waveDanger2", STEP_LEVELS[0]);
    prefs.putInt("pulseFar2", PULSE_LEVELS[2]);
    prefs.putInt("pulseNear2", PULSE_LEVELS[1]);
    prefs.putInt("pulseDanger2", PULSE_LEVELS[0]);
    prefs.putInt("pulseForward2", FRONTAL_PULSE_VALUE_LEVEL);
    prefs.putInt("waveForward2", FRONTAL_DANGER_VALUE_LEVEL);
    prefs.putInt("Forward2", DISTANCE_FORWARD_DANGER);
    
    profile = new Profile(prefs, 2);
  }
  else {
    Serial.println( "Second profile found: " + prefs.getString("fileName2"));

    profile = new Profile(prefs, 2);
    
    for (size_t i = 0; i < profile->leftMotor->vibrations.size() ; i++)
    {
      if(i == 0) FRONTAL_DANGER_VALUE_LEVEL = (ushort)(profile->leftMotor->vibrations[i]);
      else STEP_LEVELS[i-1] = (ushort)(profile->leftMotor->vibrations[i]);  
    }
    
    for (size_t i = 0; i < profile->leftMotor->pulse.size() ; i++)
    {
      if(i == 0) FRONTAL_PULSE_VALUE_LEVEL = (ushort)(profile->leftMotor->pulse[i]);
      else PULSE_LEVELS[i-1] = (ushort)(profile->leftMotor->pulse[i]);
    }

    for (size_t i = 0; i < profile->leftMotor->distances.size() ; i++)
    {
      if(i == 0) DISTANCE_FORWARD_DANGER = (ushort)(profile->leftMotor->distances[i]);
      else DISTANCES_LEVELS[i-1] = (ushort)(profile->leftMotor->distances[i]);
    }

  }
  prefs.end();

  printf("Currently loaded profile\n");
  Serial.println(JSONtoStringBT(profile->currentFile->getJSONObject()));
  printf("Current sensor configuration\n");
  Serial.println(JSONtoStringBT(profile->getSensorsJSONObject()));
  printf("Current motor configuration\n");
  Serial.println(JSONtoStringBT(profile->getMotorsJSONObject()));
}


void switchProfile(){
  if(ProfileNr == 1) {
    ProfileNr = 2;
    initProfile2();

    return;
  }
  ProfileNr = 1;
  initProfile();
}


void ISRBlueetoothWriteSensors() {
  std::vector<int> values;
  for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      values.push_back(RA_vector[i]->getAverage());
  }
  SerialBT.println("20"+sendIntListBT(values, "sensors"));
  if (DebugEnabled)
    Serial.println("SEND\t20" + sendIntListBT(values, "sensors"));
}


void initSensors() {
  if(secondHub){
    for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      RA_vector.push_back(new RunningAverage<int>(RA_SIZE));
      RA_vector[i]->fillValue(80, RA_SIZE);
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
      vectorVL53L0X.push_back(new Adafruit_VL53L0X());  
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
        vectorVL53L0X[i + NUMBER_OF_SENSORS/2]->setAddress(HUB_2_SENSORS_ADDRS[i]);
      } else {
        vectorVL53L0X[i + NUMBER_OF_SENSORS/2]->begin(HUB_2_SENSORS_ADDRS[i]);
      }
    }

    TCA_HUB.openAll();
    TCA_HUB_2.openAll();
  }

  else{  
    for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
      RA_vector.push_back(new RunningAverage<int>(RA_SIZE));
      RA_vector[i]->fillValue(80, RA_SIZE);
    }

    for (int i = 0; i < NUMBER_OF_SENSORS; i ++) {
      vectorVL53L0X.push_back(new Adafruit_VL53L0X());  
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

    if(IMUEnabled){
      M5.IMU.getAccelData(&accX, &accY, &accZ);
      RA_IMU.addValue(abs(accX)+abs(accY)+abs(accZ));
    }
}


void setLedValues(){

int s = NUMBER_OF_SENSORS % 2;  // 1 if uneven number
int ledLevel = 0;

// Left
for(int i = 0; i < NUMBER_OF_SENSORS/2 - 1; i++){
  if(RA_vector[i]->getAverage() < DISTANCES_LEVELS[0])
    {
      ledLevel = 3;
      leds[0] = COLOR_DANGER;
      leds[1] = COLOR_DANGER;
    }
  else if(RA_vector[i]->getAverage() < DISTANCES_LEVELS[1] && ledLevel < 2)
    {
      ledLevel = 2;
      leds[0] = COLOR_LEVEL_2;
      leds[1] = COLOR_LEVEL_2;}
  else if(RA_vector[i]->getAverage() < DISTANCES_LEVELS[2] && ledLevel < 1)
    {
      ledLevel = 1;
      leds[0] = COLOR_LEVEL_1;
      leds[1] = COLOR_LEVEL_1;}
  else if(ledLevel == 0){
      leds[0] = COLOR_LEVEL_0;
      leds[1] = COLOR_LEVEL_0;
  }
}

// center
  if(RA_vector[NUMBER_OF_SENSORS/2 - 1 + s]->getAverage() < DISTANCES_LEVELS[0] || RA_vector[NUMBER_OF_SENSORS/2]->getAverage() < DISTANCES_LEVELS[0])
    {
      leds[2] = COLOR_DANGER;
    }
  else if((RA_vector[NUMBER_OF_SENSORS/2 - 1 + s]->getAverage() < DISTANCES_LEVELS[1] || RA_vector[NUMBER_OF_SENSORS/2]->getAverage() < DISTANCES_LEVELS[1]) && ledLevel < 2)
    {
      leds[2] = COLOR_LEVEL_2;
    }
  else if((RA_vector[NUMBER_OF_SENSORS/2 - 1 + s]->getAverage() < DISTANCES_LEVELS[2] || RA_vector[NUMBER_OF_SENSORS/2]->getAverage() < DISTANCES_LEVELS[2]) && ledLevel < 1)
    {
      leds[2] = COLOR_LEVEL_1;
    }
  else{
      leds[2] = COLOR_LEVEL_0;
    }


ledLevel = 0;

// right
for(int i = NUMBER_OF_SENSORS/2 + 1; i < NUMBER_OF_SENSORS; i++){
  if(RA_vector[i]->getAverage() < DISTANCES_LEVELS[0])
    {
      ledLevel = 3;
      leds[3] = COLOR_DANGER;
      leds[4] = COLOR_DANGER;
    }
  else if(RA_vector[i]->getAverage() < DISTANCES_LEVELS[1] && ledLevel <= 2)
    {
      ledLevel = 2;
      leds[3] = COLOR_LEVEL_2;
      leds[4] = COLOR_LEVEL_2;}
  else if(RA_vector[i]->getAverage() < DISTANCES_LEVELS[2] && ledLevel <= 1)
    {
      ledLevel = 1;
      leds[3] = COLOR_LEVEL_1;
      leds[4] = COLOR_LEVEL_1;}
  else if(ledLevel == 0){
      leds[3] = COLOR_LEVEL_0;
      leds[4] = COLOR_LEVEL_0;
  }
}
      
  FastLED.show();

}


void setupWaveValuesFromSensors() {

  if(IMUEnabled){
    if(abs(RA_IMU.getAverage() - IMU_Calibration_Data) > MOVEMENT_TRIGGER){
      if(!timer2.isEnabled(2))
        timer2.enable(2);
      M5.Lcd.setCursor(0, 200);
      M5.Lcd.println("               ");
      M5.Lcd.setCursor(0, 200);
      M5.Lcd.printf("moving\n");
      timer2.restartTimer(2);
      Movement = true;
    }
    else if(!Movement){
      if(timer2.isEnabled(2))
        timer2.disable(2);
      ledcWriteTone(ChannelLeft, 0);
      ledcWriteTone(ChannelRight, 0);
      M5.Lcd.setCursor(0, 200);
      M5.Lcd.println("               ");
      M5.Lcd.setCursor(0, 200);
      M5.Lcd.println("not moving");
      return;
    }
  }

  // for 8 sensors
  
  short front = 0;
  short rightLevel = 0;
  short leftLevel = 0;
  short pulse = 0;

    // center
    if ( (RA_vector[2]->getAverage() < DISTANCE_FORWARD_DANGER && RA_vector[5]->getAverage() < DISTANCE_FORWARD_DANGER)
       || RA_vector[3]->getAverage() < DISTANCE_FORWARD_DANGER
       || RA_vector[4]->getAverage() < DISTANCE_FORWARD_DANGER) {
      front = FRONTAL_DANGER_VALUE_LEVEL;
      pulse = 4;
    }
    
    // left side
    if (RA_vector[5]->getAverage() < DISTANCES_LEVELS[0]  ||
      RA_vector[6]->getAverage() < DISTANCES_LEVELS[0]  ||
      RA_vector[7]->getAverage() < DISTANCES_LEVELS[0]) {
      leftLevel = STEP_LEVELS[0];
      pulse = pulse > 3 ? pulse : 3;  // if pulse > 3 then pulse = pulse else pulse = 3
    }
    else if (RA_vector[5]->getAverage() < DISTANCES_LEVELS[1]  ||
      RA_vector[6]->getAverage() < DISTANCES_LEVELS[1]  ||
      RA_vector[7]->getAverage() < DISTANCES_LEVELS[1]) {
      leftLevel = STEP_LEVELS[1];
      pulse = pulse > 2 ? pulse : 2;
    }
    else if (RA_vector[5]->getAverage() < DISTANCES_LEVELS[2]  ||
      RA_vector[6]->getAverage() < DISTANCES_LEVELS[2]  ||
      RA_vector[7]->getAverage() < DISTANCES_LEVELS[2]) {
      leftLevel = STEP_LEVELS[2];
      pulse = pulse > 1 ? pulse : 1;
    } 
    else {
      leftLevel = 0;
    }

    // right side    
    if (RA_vector[0]->getAverage() < DISTANCES_LEVELS[0]  ||
      RA_vector[1]->getAverage() < DISTANCES_LEVELS[0]  ||
      RA_vector[2]->getAverage() < DISTANCES_LEVELS[0]) {
      rightLevel = STEP_LEVELS[0];
      pulse = pulse > 3 ? pulse : 3;
    }
    else if (RA_vector[0]->getAverage() < DISTANCES_LEVELS[1]  ||
      RA_vector[1]->getAverage() < DISTANCES_LEVELS[1]  ||
      RA_vector[2]->getAverage() < DISTANCES_LEVELS[1]) {
      rightLevel = STEP_LEVELS[1];
      pulse = pulse > 2 ? pulse : 2;
    }
    else if (RA_vector[0]->getAverage() < DISTANCES_LEVELS[2]  ||
      RA_vector[1]->getAverage() < DISTANCES_LEVELS[2]  ||
      RA_vector[2]->getAverage() < DISTANCES_LEVELS[2]) {
      rightLevel = STEP_LEVELS[2];
      pulse = pulse > 1 ? pulse : 1;

    } else {
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
void waveLMA(ushort left, ushort right, ushort front, ushort pulse){
  //If pulsing is enabled
  if(pulse != 0){

    PULSATION_TIME = pulse;
  
    if(PULSING){
      left = 0;
      right = 0;
      front = 0;
    }

    unsigned long current = millis();
    if(abs(current - LAST_PULSATION) > PULSATION_TIME){
      PULSING = !PULSING;
      LAST_PULSATION = current;
    }
  }

    //Wave for frontal collision is prioritary
    if(front){
      ledcWriteTone(ChannelLeft,front);
      ledcWriteTone(ChannelRight,front);
      return;
    }

    //Vibrate on the side closest to the obstacle
    if(left > right){
      ledcWriteTone(ChannelLeft, left);
      ledcWriteTone(ChannelRight,0);
      lastLeft = true;
    }else if (right > left){
      ledcWriteTone(ChannelLeft, 0);
      ledcWriteTone(ChannelRight, right);
      lastLeft = false;
    }else if (left == right && left > 0){
      //If we are in an uncertain state (impropable but might happen), vibrate on the last side detected previously
      lastLeft ? ledcWriteTone(ChannelLeft, left) + ledcWriteTone(ChannelRight,0) : ledcWriteTone(ChannelRight, right) + ledcWriteTone(ChannelLeft, 0);
    }
    else{
      ledcWriteTone(ChannelLeft, 0);
      ledcWriteTone(ChannelRight, 0);
    }
}
