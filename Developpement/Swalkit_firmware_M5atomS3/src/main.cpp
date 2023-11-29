#include <Arduino.h>
#include <M5AtomS3.h>
#include <Preferences.h>
#include "SwalkitBle.h"
#include "SwalkitProfile.h"
#include "sensors.h"
#include "LMA.h"

using namespace std;

// Configuration générale
bool display_enable = true;
bool imu_enable = true;
bool bluetooth_enable = false;
bool usb_serial_enable = true;

// Configuration Bluetooth
SwalkitProfile swalkitProfile;
Sensors sensors;
SwalkitBLE swalkitBLE(swalkitProfile, sensors);

// Déclarations
LMA lma;
float ax, ay, az;
unsigned long last_pulsation = 0;
bool pulsing = false;
bool last_left = false;

void sense_and_drive_task(void *pvParameters);
void screen_update_task(void *pvParameters);
void set_display_from_sensors();

void display_circles()
{
    if (display_enable)
    {
        M5.Lcd.fillScreen(YELLOW);
        delay(100);
        M5.lcd.drawSpot(0,0,160,M5.Lcd.color16to24(random16()));
        delay(100);
        M5.lcd.drawSpot(0,0,140,M5.Lcd.color16to24(random16()));
        delay(100);
        M5.lcd.drawSpot(0,0,120,M5.Lcd.color16to24(random16()));
        delay(100);
        M5.lcd.drawSpot(0,0,100,M5.Lcd.color16to24(random16()));
        delay(100);
        M5.lcd.drawSpot(0,0,80,M5.Lcd.color16to24(random16()));
        delay(100);
        M5.lcd.drawSpot(0,0,60,M5.Lcd.color16to24(random16()));
        delay(100);
        M5.lcd.drawSpot(0,0,40,M5.Lcd.color16to24(random16()));
        delay(100);
        M5.lcd.drawSpot(0,0,20,M5.Lcd.color16to24(random16()));
    }
}

void setup()
{
    // Initialisation du M5stack
    M5.begin(display_enable, usb_serial_enable, true, false); 
    //default in M5atomS3 is Wire1, but we are also using Wire
    Wire.begin();
    // Init LCD
    display_circles(); 

    delay(1000);
    if(usb_serial_enable) USBSerial.println("Starting...");

    // Init. de la communication I2C avec les moteurs LMA
    if(usb_serial_enable) USBSerial.println("Lma begin...");
    lma.begin();
    // Init. de la centrale inertielle pour la detection de mouvement
    if(imu_enable & usb_serial_enable) USBSerial.println("IMU begin...");

    if (imu_enable) M5.IMU.begin();
    
    // Init. de la communication I2C avec les capteurs
    if(usb_serial_enable) USBSerial.print("sensors begin...");
    sensors.begin(false);
    if(usb_serial_enable) USBSerial.println("done");

    // Init. de la configuration les moteurs LMA
    // lma.on_off(false);
    lma.set_duty_max(UINT16_MAX);
    
    // Init. des tâches multiples paralleles
    if(display_enable) xTaskCreatePinnedToCore(screen_update_task, "screen_update_task", 4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(sense_and_drive_task, "sense_and_drive_task", 4096, NULL, 1, NULL, 0);
    // TODO check stack size with uxTaskGetStackHighWaterMark https://www.freertos.org/uxTaskGetStackHighWaterMark.html

    if(usb_serial_enable) USBSerial.println("Running...");
}

void sense_and_drive_task(void *pvParameters)
{
    while (1)
    {
        sensors.read();
        // uint16_t test_left = UINT16_MAX * ( sin(float(millis())/1000 * 2 *3.141592 * 0.25)*0.1+0.9) ;
        // uint16_t test_right = ( (millis() / 1000)%2 > 0 ) ? UINT16_MAX : 0; 

        if(imu_enable){
    //     if(abs(RA_IMU.getAverage() - IMU_Calibration_Data) > MOVEMENT_TRIGGER){
    //     if(!timer2.isEnabled(2))
    //     timer2.enable(2);
    //   M5.Lcd.setCursor(0, 200);
    //   M5.Lcd.println("               ");
    //   M5.Lcd.setCursor(0, 200);
    //   M5.Lcd.printf("moving\n");
    //   timer2.restartTimer(2);
    //   Movement = true;
    // }
    // else if(!Movement){
    //   if(timer2.isEnabled(2))
    //     timer2.disable(2);
    //   ledcWriteTone(ChannelLeft, 0);
    //   ledcWriteTone(ChannelRight, 0);
    //   M5.Lcd.setCursor(0, 200);
    //   M5.Lcd.println("               ");
    //   M5.Lcd.setCursor(0, 200);
    //   M5.Lcd.println("not moving");
    //   return;
    // }
        }

    // for 8 sensors
    
        uint16_t front = 0;
        uint16_t right = 0;
        uint16_t left = 0;
        uint16_t pulse = 0;

        // center
        if ( (sensors.sensor_average[2]->getAverage() < swalkitProfile.frontSignal.distance && sensors.sensor_average[5]->getAverage() < swalkitProfile.frontSignal.distance)
        || sensors.sensor_average[3]->getAverage() < swalkitProfile.frontSignal.distance
        || sensors.sensor_average[4]->getAverage() < swalkitProfile.frontSignal.distance) {
        front = swalkitProfile.frontSignal.intensity;
        pulse = 4;
        }
        
        // left side
        if (sensors.sensor_average[5]->getAverage() < swalkitProfile.dangerSignal.distance  ||
        sensors.sensor_average[6]->getAverage() < swalkitProfile.dangerSignal.distance  ||
        sensors.sensor_average[7]->getAverage() < swalkitProfile.dangerSignal.distance) {
        left = swalkitProfile.dangerSignal.intensity;
        pulse = pulse > 3 ? pulse : 3;  // if pulse > 3 then pulse = pulse else pulse = 3
        }
        else if (sensors.sensor_average[5]->getAverage() < swalkitProfile.nearSignal.distance  ||
        sensors.sensor_average[6]->getAverage() < swalkitProfile.nearSignal.distance  ||
        sensors.sensor_average[7]->getAverage() < swalkitProfile.nearSignal.distance) {
        left = swalkitProfile.nearSignal.intensity;
        pulse = pulse > 2 ? pulse : 2;
        }
        else if (sensors.sensor_average[5]->getAverage() < swalkitProfile.farSignal.distance  ||
        sensors.sensor_average[6]->getAverage() < swalkitProfile.farSignal.distance  ||
        sensors.sensor_average[7]->getAverage() < swalkitProfile.farSignal.distance) {
        left = swalkitProfile.farSignal.intensity;
        pulse = pulse > 1 ? pulse : 1;
        } 
        else {
        left = 0;
        }

        // right side    
        if (sensors.sensor_average[0]->getAverage() < swalkitProfile.dangerSignal.distance  ||
        sensors.sensor_average[1]->getAverage() < swalkitProfile.dangerSignal.distance  ||
        sensors.sensor_average[2]->getAverage() < swalkitProfile.dangerSignal.distance) {
        right = swalkitProfile.dangerSignal.intensity;
        pulse = pulse > 3 ? pulse : 3;
        }
        else if (sensors.sensor_average[0]->getAverage() < swalkitProfile.nearSignal.distance  ||
        sensors.sensor_average[1]->getAverage() < swalkitProfile.nearSignal.distance  ||
        sensors.sensor_average[2]->getAverage() < swalkitProfile.nearSignal.distance) {
        right = swalkitProfile.nearSignal.intensity;
        pulse = pulse > 2 ? pulse : 2;
        }
        else if (sensors.sensor_average[0]->getAverage() < swalkitProfile.farSignal.distance  ||
        sensors.sensor_average[1]->getAverage() < swalkitProfile.farSignal.distance  ||
        sensors.sensor_average[2]->getAverage() < swalkitProfile.farSignal.distance) {
        right = swalkitProfile.farSignal.intensity;
        pulse = pulse > 1 ? pulse : 1;

        } else {
        right = 0;
        }

        switch (pulse)
        {
        case 4:
        pulse = swalkitProfile.frontSignal.pulse;
        break;
        case 3:
        pulse = swalkitProfile.dangerSignal.pulse;
        break;
        case 2:
        pulse = swalkitProfile.nearSignal.pulse;
        break;
        case 1:
        pulse = swalkitProfile.farSignal.pulse;
        break;
        default:
        pulse = 0;
        break;
        }

        // USBSerial.print(right);
        // USBSerial.print("...");
        // USBSerial.print(left);
        // USBSerial.print("...");
        // USBSerial.println(front);
    
        if(pulse != 0){
            if(pulsing){
                left = 0;
                right = 0;
                front = 0;
            }

            unsigned long current = millis();

            if((current - last_pulsation) > pulse){
                pulsing = !pulsing;
                last_pulsation = current;
            }
        }

        
        if(front){ lma.write(front*(UINT16_MAX/100), front*(UINT16_MAX/100)); }
        else
        {
            //Vibrate on the side closest to the obstacle
            if(left > right){
                lma.write(0, left*(UINT16_MAX/100));
                last_left = true;
            }
            else if (right > left){
                lma.write(right*(UINT16_MAX/100),0);
                last_left = false;
            }
            else if (left == right && left > 0){
            //If we are in an uncertain state (impropable but might happen), vibrate on the last side detected previously
                last_left ? lma.write(0, left*(UINT16_MAX/100)) : lma.write(right*(UINT16_MAX/100),0);
                
            }
            else{ lma.write(0,0); }    
        }
    }
}

void screen_update_task(void *pvParameters)
{
    while (1)
    {
        if (display_enable)
        {
            set_display_from_sensors();
        }
        delay(100);
    }
}


void loop()
{
    M5.update(); //Btn read
    if (M5.Btn.wasPressed())
    {
        bluetooth_enable = !bluetooth_enable;
        if (bluetooth_enable)
        {
            swalkitBLE.start();
            if(usb_serial_enable) USBSerial.print("Bluetooth enabled\n");
            M5.Lcd.fillScreen(BLUE);
        }
        else
        {
            swalkitBLE.stop();
            if(usb_serial_enable) USBSerial.print("Bluetooth disabled\n");
            display_circles();
        }
    }
    delay(100);
}

void set_display_from_sensors()
{
    if(imu_enable)
    {
        M5.IMU.getAccel(&ax,&ay,&az);
        M5.Lcd.drawSpot((int)(-ay*128)+64,(int)(-ax*128)+64,5,M5.Lcd.color16to24(random16()));
    }
}