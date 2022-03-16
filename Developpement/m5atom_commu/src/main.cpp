#include <Arduino.h>
#include <M5Atom.h>
#include "runningAverage.h"
#include <Wire.h>
#include <Preferences.h>
#include "DacESP32.h"
#include "TCA9548A.h"
#include "Adafruit_VL53L0X.h"
#include "AtomDisplay.h"
#include "BluetoothSerial.h"
#include "m5atom_tfi_commu.h"



void setup() {

  // M5.begin(true, true, true);
  M5.begin(true, true, true);

  M5.IMU.Init();
  M5.IMU.SetAccelFsr(M5.IMU.AFS_2G);  // set Accalerometer sensitivity

  initProfile();

  int prescaler = 80; // results in timer frequency 1MHz; increment each microsecond

  // Timer 1
  timer1 = timerBegin(1, prescaler, true);
  timerAttachInterrupt(timer1, &onTimer1, true); //attach timer interrupt
  timerAlarmWrite(timer1, 100000, true);
  timerAlarmDisable(timer1);

  // set up vibrators
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(VIN_PIN,INPUT);
  pinMode(FAULT_PIN,INPUT);

  // Timer 2 - Vibration
  timer2 = timerBegin(2, prescaler, true);
  timerAttachInterrupt(timer2, &onTimer2, true);
  timerAlarmWrite(timer2, 100000, true);
  timerAlarmDisable(timer2);

  // Timer 3 - Pulsing
  timer3 = timerBegin(3, prescaler, true);
  timerAttachInterrupt(timer3, &onTimer3, true);
  timerAlarmWrite(timer3, pulseLength * 1000, true);
  timerAlarmDisable(timer3);

  Wire.begin(26,32);

  // test LCD
  if(DisplayEnable)
    M5.dis.fillpix(ORANGE);

  // set up I2C
  if(HubBridge){
    TCA_HUB_BRIDGE.begin(Wire, BRIDGE_TCA9548A_ADDRESS);    // Start bridge Multiplexer
    TCA_HUB_BRIDGE.openAll();
  }
  TCA_HUB.begin(Wire, MAIN_TCA9548A_ADDRESS);           // Start first Multiplexer
  if(SecondHub)
    TCA_HUB_2.begin(Wire, SECOND_TCA9548A_ADDRESS);     // Start second Multiplexer 

  initSensors();

  delay(100);

  M5.dis.clear();
  
}

void loop() {

  if (M5.Btn.wasPressed()){
    BluetoothEnable = !BluetoothEnable;
    if(BluetoothEnable){
      SerialBT.begin(BLUETOOTH_DEVICE_NAME);
      Serial.print("Bluetooth enabled\n");
      timerAlarmEnable(timer1);
    }
    else{
      SerialBT.end();
      Serial.print("Bluetooth disabled\n");
      timerAlarmDisable(timer1);
    }
  }

  if(BluetoothTimer){
    readBluetoothData();
    ISRBlueetoothWriteSensors();
    BluetoothTimer = false;
  }

  TCA_HUB.openAll();
  TCA_HUB_2.openAll();

  readSensors();

  TCA_HUB.closeAll();
  TCA_HUB_2.closeAll();

  if(DisplayEnable){
    setDisplayFromSensors();
  }

  if(VibrationEnabled){
    setVibrationFromSensors();
  }
  
  M5.update();
}

