#include <Arduino.h>
#include <M5AtomS3.h>
#include "SwalkitBle.h"
#include "SwalkitProfile.h"
#include "sensors.h"
#include "LMA.h"

#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define MIN3(X,Y,Z) (MIN(X,MIN(Y,Z)))
#define CLIP(X,A,B) (MAX(MIN(X,B),A))


using namespace std;

// Configuration générale
bool imu_enable = true;
bool bluetooth_enable = false;
bool usb_serial_enable = false;

// imu
float total_acceleration;
float seuil_mouvement = 0;
bool moving = true;
bool moving_changed = true;
unsigned long watchdog_imu_move = 5000; // 5 secondes
unsigned long last_time_moved = 0;      // secondes

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
void set_display_from_sensors(void *pvParameters);
void lcd_write_state();

void display_circles(int _delay)
{
    M5.Lcd.fillScreen(YELLOW);
    delay(_delay);
    M5.lcd.drawSpot(0, 0, 160, M5.Lcd.color16to24(random16()));
    delay(_delay);
    M5.lcd.drawSpot(0, 0, 140, M5.Lcd.color16to24(random16()));
    delay(_delay);
    M5.lcd.drawSpot(0, 0, 120, M5.Lcd.color16to24(random16()));
    delay(_delay);
    M5.lcd.drawSpot(0, 0, 100, M5.Lcd.color16to24(random16()));
    delay(_delay);
    M5.lcd.drawSpot(0, 0, 80, M5.Lcd.color16to24(random16()));
    delay(_delay);
    M5.lcd.drawSpot(0, 0, 60, M5.Lcd.color16to24(random16()));
    delay(_delay);
    M5.lcd.drawSpot(0, 0, 40, M5.Lcd.color16to24(random16()));
    delay(_delay);
    M5.lcd.drawSpot(0, 0, 20, M5.Lcd.color16to24(random16()));
}

void setup()
{
    // Initialisation du M5stack
    M5.begin(true, usb_serial_enable, true, false);
    M5.Lcd.fillScreen(CYAN);

    // default in M5atomS3 is Wire1 tu use MPU6886, but we are also using Wire on grove (pins 2 and 1)
    Wire.endTransmission();
    Wire.begin(2, 1, 100000L);

    // load stored profile
    if (!swalkitProfile.load())
    {
        M5.Lcd.fillScreen(ORANGE);
    }

    delay(2000);
    if (usb_serial_enable)
        USBSerial.println("Starting...");

    // Init. de la communication I2C avec les moteurs LMA
    if (usb_serial_enable)
        USBSerial.println("Lma begin...");
    lma.begin();
    // Init. de la centrale inertielle pour la detection de mouvement
    if (imu_enable & usb_serial_enable)
        USBSerial.println("IMU begin...");

    if (imu_enable)
    {

        M5.Lcd.fillScreen(PINK);
        M5.Lcd.setCursor(10, 10);
        M5.Lcd.print("CALIBRATION\nNE PAS BOUGER");

        M5.IMU.begin();
        for (int i = 0; i < 10; i++)
        {
            M5.IMU.getAccel(&ax, &ay, &az);
            total_acceleration = std::sqrt(ax * ax + ay * ay + az * az);
            seuil_mouvement += total_acceleration;
            delay(200);
            USBSerial.print("ax ay az: ");
            USBSerial.print(ax);
            USBSerial.print(" ");
            USBSerial.print(ay);
            USBSerial.print(" ");
            USBSerial.println(az);
        }
        seuil_mouvement *= 0.1;
        USBSerial.println(seuil_mouvement);
    }


    // Init. de la communication I2C avec les capteurs
    if (usb_serial_enable)
        USBSerial.print("sensors begin...");
    sensors.begin(false);
    if (usb_serial_enable)
        USBSerial.println("done");

    // Init. de la configuration les moteurs LMA
    lma.on_off(true);
    lma.set_duty_max(UINT16_MAX);
    // lma.write(UINT16_MAX,UINT16_MAX);
    
    //init lcd
    display_circles(100);
    delay(200);
    M5.lcd.fillScreen(WHITE);

    // Init. des tâches multiples paralleles
    xTaskCreatePinnedToCore(set_display_from_sensors, "set_display_from_sensors_task", 4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(sense_and_drive_task, "sense_and_drive_task", 2048, NULL, 1, NULL, 0);
    // TODO check stack size with uxTaskGetStackHighWaterMark https://www.freertos.org/uxTaskGetStackHighWaterMark.html

    if (usb_serial_enable)
        USBSerial.println("Running...");

}

void sense_and_drive_task(void *pvParameters)
{
    // UBaseType_t uxHighWaterMark;
    // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    // USBSerial.println(uxHighWaterMark);
    uint32_t now = millis();
    while (1)
    {
        sensors.read();

        if (imu_enable)
        {
            M5.IMU.getAccel(&ax, &ay, &az);
            total_acceleration = std::sqrt(ax * ax + ay * ay + az * az) * 0.1 + total_acceleration * 0.9;
            //USBSerial.println(total_acceleration);
            if (abs(total_acceleration - seuil_mouvement) > 0.003f)
            {
                if (!moving)
                    moving_changed = true;
                moving = true;
                last_time_moved = millis();
            }
            if (millis() - last_time_moved > watchdog_imu_move)
            {
                if (moving)
                    moving_changed = true;
                moving = false;
            }
        }

        // for 8 sensors
        uint16_t front = 0;
        uint16_t right = 0;
        uint16_t left = 0;
        uint16_t pulse = 0;
        int pulseId = 0;
        int rightId = 0;
        int leftId = 0;

        int pulseTab[5] = {0, swalkitProfile.farSignal.pulse, swalkitProfile.nearSignal.pulse, swalkitProfile.dangerSignal.pulse, swalkitProfile.frontSignal.pulse};
        int sideTab[4] = {0, swalkitProfile.farSignal.intensity, swalkitProfile.nearSignal.intensity, swalkitProfile.dangerSignal.intensity};


        int front_distance = MIN3(MAX(sensors.sensor_average[2]->getAverage(), sensors.sensor_average[5]->getAverage()), sensors.sensor_average[3]->getAverage(), sensors.sensor_average[4]->getAverage());
        int right_distance = MIN3(sensors.sensor_average[0]->getAverage(), sensors.sensor_average[1]->getAverage(), sensors.sensor_average[2]->getAverage());
        int left_distance = MIN3(sensors.sensor_average[5]->getAverage(), sensors.sensor_average[6]->getAverage(), sensors.sensor_average[7]->getAverage());

            
        // center
        if (front_distance < swalkitProfile.frontSignal.distance)
        {
            front = swalkitProfile.frontSignal.intensity;
            pulseId = 4;
        }

        // left side
        if (left_distance < swalkitProfile.dangerSignal.distance)
        {
            leftId = 3;
            pulseId = MAX(pulseId, 3);
        } 
        else 
        {
            if (left_distance < swalkitProfile.nearSignal.distance)
            {
                leftId =  2;
                pulseId = MAX(pulseId, 2);
            } 
            else 
            {
                if (left_distance < swalkitProfile.farSignal.distance)
                {
                    leftId = 1;
                    pulseId = MAX(pulseId, 1);
                } 
                else
                {
                    leftId = 0;
                }
            }
        }


        // right side
        if (right_distance < swalkitProfile.dangerSignal.distance)
        {
            rightId = 3;
            pulseId = MAX(pulseId, 3);
        } 
        else 
        {
            if (right_distance < swalkitProfile.nearSignal.distance)
            {
                rightId = 2;
                pulseId = MAX(pulseId, 2);
            } 
            else 
            {
                if (right_distance < swalkitProfile.farSignal.distance)
                {
                    rightId = 1;
                    pulseId = MAX(pulseId, 1);
                } 
                else
                {
                    rightId = 0;
                }
            }
        }

        pulseId = CLIP(pulseId, 0, 4);
        leftId = CLIP(leftId, 0, 3);
        rightId = CLIP(rightId, 0, 3);


        pulse = pulseTab[pulseId];
        left = sideTab[leftId];
        right = sideTab[rightId];

        if (pulse != 0)
        {
            if (pulsing)
            {
                left = 0;
                right = 0;
                front = 0;
            }

            unsigned long current = millis();

            if ((current - last_pulsation) > pulse)
            {
                pulsing = !pulsing;
                last_pulsation = current;
            }
        }

        if (!moving)
        {
            left = 0;
            right = 0;
            front = 0;
        }


        if (front){
            right = front;
            left = front;
        } else {
            if(rightId < leftId) //left closer
            {
                right = 0;
            } 
            else 
            {
                if(rightId > leftId)
                {
                    left = 0;
                }
            }
        }

        lma.write(right * (UINT16_MAX / 100), left * (UINT16_MAX / 100));

        // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        // USBSerial.println(uxHighWaterMark);
    }
}


enum swalkit_btn_state
{
    on_short_pressed,
    on_long_press,
    on_long_press_released,
    idle
};

swalkit_btn_state btn_state = idle;

void loop()
{
    M5.update(); // Btn read

    if (M5.Btn.wasReleased())
    {
        if (btn_state == on_long_press)
            btn_state = on_long_press_released;
        else
            btn_state = on_short_pressed;
    }

    if (M5.Btn.pressedFor(500))
    {
        btn_state = on_long_press;
    }

    if(swalkitProfile.store_flag){
        swalkitProfile.store_flag = false;
        swalkitProfile.store();
    }
}

#define GREEN_SWALKIT 0x2589
#define RED_SWALKIT 0xe8e4

void lcd_write_state()
{
    M5.Lcd.setCursor(0, 50);
    if (bluetooth_enable)
        M5.Lcd.print("Bluetooth ON\n");
    else
        M5.Lcd.print("Bluetooth OFF\n");
    if (imu_enable)
        M5.Lcd.print("Swalkit Move ON\n");
    else
        M5.Lcd.print("Swalkit Move OFF\n");
}

void set_display_from_sensors(void *pvParameters)
{
    // UBaseType_t uxHighWaterMark;
    // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    // USBSerial.println(uxHighWaterMark);

    while (1)
    {
        switch (btn_state)
        {
        case on_short_pressed:
            bluetooth_enable = !bluetooth_enable;

            if (bluetooth_enable)
            {
                swalkitBLE.start();
                if (usb_serial_enable)
                    USBSerial.print("Bluetooth enabled\n");
                M5.Lcd.fillScreen(BLUE);
            }
            else
            {
                swalkitBLE.stop();
                if (usb_serial_enable)
                    USBSerial.print("Bluetooth disabled\n");
                M5.Lcd.fillScreen(WHITE);
            }

            if (imu_enable & !moving) M5.lcd.drawSpot(64, 64, 40, RED_SWALKIT);
            else M5.lcd.drawSpot(64, 64, 60, GREEN_SWALKIT);
            
            btn_state = idle;
            lcd_write_state();
            break;
        case on_long_press:
            M5.Lcd.fillScreen(ORANGE);
            break;
        case on_long_press_released:
            imu_enable = !imu_enable;

            if (!imu_enable)
            {
                moving = true;
                if (usb_serial_enable)
                    USBSerial.print("Imu disabled\n");
                M5.lcd.drawSpot(64, 64, 60, GREEN_SWALKIT);
            }
            else
            {
                if (usb_serial_enable)
                    USBSerial.print("Imu enabled\n");
                M5.Lcd.fillScreen(WHITE);
                M5.lcd.drawSpot(64, 64, 40, RED_SWALKIT);
            }
            btn_state = idle;
            lcd_write_state();

            break;
        default:
            if (imu_enable)
            {
                // M5.Lcd.drawSpot((int)(-az * 128) + 64, (int)(-ax * 128) + 64, 5, M5.Lcd.color16to24(random16()));
                if (moving_changed)
                {
                    moving_changed = false;
                    if (moving)
                    {
                        M5.lcd.drawSpot(64, 64, 60, GREEN_SWALKIT);
                    }
                    else
                    {
                        M5.lcd.drawSpot(64, 64, 60, WHITE);
                        M5.lcd.drawSpot(64, 64, 40, RED_SWALKIT);
                    }

                    lcd_write_state();
                }
            }
            break;
        }
        // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        // USBSerial.println(uxHighWaterMark);
        delay(100);
    }
}
