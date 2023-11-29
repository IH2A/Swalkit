#include <Arduino.h>
#include <M5AtomS3.h>
#include <Preferences.h>
#include "SwalkitBle.h"
#include "SwalkitProfile.h"
#include "sensors.h"
#include "LMA.h"

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
    sensors.begin(true);
    if(usb_serial_enable) USBSerial.println("done");

    // Init. de la configuration les moteurs LMA
    lma.on_off(false);
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
        
        uint16_t left = UINT16_MAX * ( sin(float(millis())/1000 * 2 *3.141592 * 0.25)*0.1+0.9) ;
        uint16_t right = ( (millis() / 1000)%2 > 0 ) ? UINT16_MAX : 0; 
        lma.write(right, left); 
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