#include <Arduino.h>
#include <M5AtomS3.h>
#include "SwalkitBle.h"
#include "SwalkitProfile.h"
#include "SwalkitDisplay.h"
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
bool usb_serial_enable = true;

// imu
float total_acceleration;
float seuil_mouvement = 0;
bool moving = true;
unsigned long watchdog_imu_move = 5000; // 5 secondes
unsigned long last_time_moved = 0;      // secondes

// Configuration Bluetooth
SwalkitProfile swalkitProfile;
Sensors sensors;
SwalkitBLE swalkitBLE(swalkitProfile, sensors, usb_serial_enable);
SwalkitDisplay swalkitDisplay{};

// Déclarations
LMA lma;
float ax, ay, az;
bool last_left = false;

// button state
enum swalkit_btn_state
{
    on_short_pressed,
    on_long_press,
    on_long_press_released,
    idle
};
swalkit_btn_state btn_state = idle;

// tasks
void sense_and_drive_task(void *pvParameters);
void handle_button_state(void *pvParameters);

void setup()
{
    // Initialisation du M5stack
    M5.begin(true, usb_serial_enable, true, false);
    swalkitDisplay.Init();
    swalkitDisplay.SetSwalkitState(SwalkitDisplay::SwalkitState::Initialising);

    // default in M5atomS3 is Wire1 tu use MPU6886, but we are also using Wire on grove (pins 2 and 1)
    Wire.endTransmission();
    Wire.begin(2, 1, 100000L);

    // load stored profile
    if (!swalkitProfile.load())
    {
        swalkitDisplay.SetError("Error loading profile");
        delay(5000);
        swalkitDisplay.SetError(nullptr);
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
        swalkitDisplay.SetSwalkitState(SwalkitDisplay::SwalkitState::Calibrating);

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
    delay(200);
    swalkitDisplay.SetSwalkitState(SwalkitDisplay::SwalkitState::Ready);

    // Init. des tâches multiples paralleles
    xTaskCreatePinnedToCore(handle_button_state, "handle_button_state_task", 4096, NULL, 3, NULL, 0);
    xTaskCreatePinnedToCore(sense_and_drive_task, "sense_and_drive_task", 2048, NULL, 1, NULL, 0);
    // TODO check stack size with uxTaskGetStackHighWaterMark https://www.freertos.org/uxTaskGetStackHighWaterMark.html

    if (usb_serial_enable)
        USBSerial.println("Running...");

}

enum class SignalRange {
    Danger,
    Near,
    Far,
    Away
};

SignalRange getRangeFromDistance(int distance, int &intensity, int &pulse) {
    if (distance < swalkitProfile.dangerSignal.distance) {
        intensity = swalkitProfile.dangerSignal.intensity;
        pulse = swalkitProfile.dangerSignal.pulse;
        return SignalRange::Danger;
    }
    if (distance < swalkitProfile.nearSignal.distance) {
        intensity = swalkitProfile.nearSignal.intensity;
        pulse = swalkitProfile.nearSignal.pulse;
        return SignalRange::Near;
    }
    if (distance < swalkitProfile.farSignal.distance) {
        intensity = swalkitProfile.farSignal.intensity;
        pulse = swalkitProfile.farSignal.pulse;
        return SignalRange::Far;
    }
    intensity = 0;
    pulse = 0;
    return SignalRange::Away;
}

void sense_and_drive_task(void *pvParameters)
{
    // UBaseType_t uxHighWaterMark;
    // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
    // USBSerial.println(uxHighWaterMark);
    unsigned long now;
    int leftIntensity, rightIntensity, pulse;
    unsigned long last_pulsation = 0;
    bool pulsing = false;

    while (1) {
        sensors.read();
        now = millis();
        leftIntensity = rightIntensity = 0;

        if (imu_enable)
        {
            M5.IMU.getAccel(&ax, &ay, &az);
            total_acceleration = std::sqrt(ax * ax + ay * ay + az * az) * 0.1 + total_acceleration * 0.9;
            //USBSerial.println(total_acceleration);

            if (abs(total_acceleration - seuil_mouvement) > 0.03f) {
                moving = true;
                swalkitDisplay.SetImuState(SwalkitDisplay::IMUState::Moving);
                last_time_moved = now;
            } else if (now - last_time_moved > watchdog_imu_move) {
                moving = false;
                swalkitDisplay.SetImuState(SwalkitDisplay::IMUState::Stopped);
            }
        }

        // for 8 sensors

        if (moving) {
            int pulseId = 0;
            int rightId = 0;
            int leftId = 0;

            int pulseTab[5] = {0, swalkitProfile.farSignal.pulse, swalkitProfile.nearSignal.pulse, swalkitProfile.dangerSignal.pulse, swalkitProfile.frontSignal.pulse};
            int sideTab[4] = {0, swalkitProfile.farSignal.intensity, swalkitProfile.nearSignal.intensity, swalkitProfile.dangerSignal.intensity};


            int front_distance = MIN3(MAX(sensors.sensor_average[2]->getAverage(), sensors.sensor_average[5]->getAverage()), sensors.sensor_average[3]->getAverage(), sensors.sensor_average[4]->getAverage());
            int right_distance = MIN3(sensors.sensor_average[0]->getAverage(), sensors.sensor_average[1]->getAverage(), sensors.sensor_average[2]->getAverage());
            int left_distance = MIN3(sensors.sensor_average[5]->getAverage(), sensors.sensor_average[6]->getAverage(), sensors.sensor_average[7]->getAverage());

            // center
            if (front_distance < swalkitProfile.frontSignal.distance) {
                leftIntensity = rightIntensity = swalkitProfile.frontSignal.intensity;
                pulse = swalkitProfile.frontSignal.pulse;
            } else {
                int rightPulse;
                SignalRange leftRange = getRangeFromDistance(left_distance, leftIntensity, pulse);
                SignalRange rightRange = getRangeFromDistance(right_distance, rightIntensity, rightPulse);
                if (leftRange > rightRange) {   // keep only closest range if both are different
                    leftIntensity = 0;
                    pulse = rightPulse;
                } else if (rightRange > leftRange) {
                    rightIntensity = 0;
                }
            }

            if (pulse != 0) {   // manage pulse if set
                if (pulsing) {
                    leftIntensity = rightIntensity = 0;
                }
                if ((now - last_pulsation) > pulse) {
                    pulsing = !pulsing;
                    last_pulsation = now;
                }
            }
        }

        lma.write(rightIntensity * (UINT16_MAX / 100), leftIntensity * (UINT16_MAX / 100));

        // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        // USBSerial.println(uxHighWaterMark);
    }
}


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
}

void handle_button_state(void *pvParameters)
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
            }
            else
            {
                swalkitBLE.stop();
            }

            btn_state = idle;
            break;
        case on_long_press:
            swalkitDisplay.SetSwalkitState(SwalkitDisplay::SwalkitState::InLongPress);
            break;
        case on_long_press_released:
            swalkitDisplay.SetSwalkitState(SwalkitDisplay::SwalkitState::Ready);
            imu_enable = !imu_enable;

            if (!imu_enable)
            {
                moving = true;
                if (usb_serial_enable)
                    USBSerial.print("Imu disabled\n");
                swalkitDisplay.SetImuState(SwalkitDisplay::IMUState::Disabled);
            }
            else
            {
                if (usb_serial_enable)
                    USBSerial.print("Imu enabled\n");
                    
                swalkitDisplay.SetImuState(SwalkitDisplay::IMUState::Moving);
            }
            btn_state = idle;

            break;
        default:
            break;
        }
        // uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        // USBSerial.println(uxHighWaterMark);
        delay(100);
    }
}
