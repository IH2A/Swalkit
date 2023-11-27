#include <Arduino.h>
#include <M5AtomS3.h>
// #include <Preferences.h>
#include "SwalkitBle.h"
#include "SwalkitProfile.h"
// #include "sensors.h"
#include "LMA.h"

// Configuration générale
bool display_enable = true;
bool imu_enable = true;
bool bluetooth_enable = false;

// Configuration Bluetooth
SwalkitProfile swalkitProfile;
SwalkitBLE swalkitBLE(swalkitProfile);

// Déclarations
// Sensors sensors;
LMA lma;
float ax, ay, az;
void sense_and_drive_task(void *pvParameters);
void screen_update_task(void *pvParameters);
void set_display_from_sensors();

void setup()
{
    delay(100);
    // Initialisation du M5stack
    M5.begin(true, true, true, true);
    USBSerial.println("Starting...");

    // Init. de la centrale inertielle pour la detection de mouvement
    // if (imu_enable)
    // {
    //     M5.IMU.begin();
    // }

    // Init LCD
    if (display_enable)
    {
        M5.Lcd.fillScreen(YELLOW);
        M5.lcd.drawSpot(0,0,160,M5.Lcd.color16to24(random16()));
        M5.lcd.drawSpot(0,0,140,M5.Lcd.color16to24(random16()));
        M5.lcd.drawSpot(0,0,120,M5.Lcd.color16to24(random16()));
        M5.lcd.drawSpot(0,0,100,M5.Lcd.color16to24(random16()));
        M5.lcd.drawSpot(0,0,80,M5.Lcd.color16to24(random16()));
        M5.lcd.drawSpot(0,0,60,M5.Lcd.color16to24(random16()));
        M5.lcd.drawSpot(0,0,40,M5.Lcd.color16to24(random16()));
        M5.lcd.drawSpot(0,0,20,M5.Lcd.color16to24(random16()));
        xTaskCreatePinnedToCore(screen_update_task, "screen_update_task", 4096, NULL, 3, NULL, 0);
    }

    // Init. de la communication I2C
    // sensors.begin();
    lma.begin();
    xTaskCreatePinnedToCore(sense_and_drive_task, "sense_and_drive_task", 4096, NULL, 1, NULL, 0);
    // TODO check stack size with uxTaskGetStackHighWaterMark https://www.freertos.org/uxTaskGetStackHighWaterMark.html
}

void sense_and_drive_task(void *pvParameters)
{
    while (1)
    {
        delay(100);
        // sensors.read();
        lma.write();
        // M5.lcd.drawSpot(118,10,10,M5.Lcd.color16to24(random16()));
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
        delay(50);
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
            USBSerial.print("Bluetooth enabled\n");
            M5.Lcd.fillScreen(BLUE);
        }
        else
        {
            swalkitBLE.stop();
            USBSerial.print("Bluetooth disabled\n");
            M5.Lcd.fillScreen(YELLOW);
        }
    }


    motors.write();
    // delay(10);
    delay(100);
    // USBSerial.println(motors.driver.getAnalogInput(_12bit)/ 4095.0f * 3.3f / 0.09f);
}

void set_display_from_sensors()
{
    if(imu_enable)
    {
        M5.IMU.getAccel(&ax,&ay,&az);
        M5.Lcd.drawSpot((int)(-ay*128)+64,(int)(-ax*128)+64,5,M5.Lcd.color16to24(random16()));
    }
}