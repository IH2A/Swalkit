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


using namespace std;

// Configuration générale
bool imu_enable = true;
constexpr bool usb_serial_enable = true;
constexpr bool sensors_enabled = false;
constexpr bool motors_enabled = false;

// imu
float total_acceleration;
float seuil_mouvement;
float ax, ay, az;   // accelerations
constexpr size_t gyro_data_size = 10;
float gyro_data[gyro_data_size];
size_t gyro_index;
float gyro_calibration = 0;
float gyro_threshold = 80;
bool moving = true;
unsigned long watchdog_imu_move = 5000; // 5 secondes
unsigned long last_time_moved = 0;      // secondes
float acceleration_threshold = 0.02f;   // relative to calibration

// Configuration Bluetooth
SwalkitProfile swalkitProfile;
Sensors sensors;
SwalkitBLE swalkitBLE(swalkitProfile, sensors, usb_serial_enable);
SwalkitDisplay swalkitDisplay{};
bool bluetooth_enabled = false;

// Déclarations
LMA lma;

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

// gyro
void read_gyro_data();
float get_current_gyro();

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

    if (motors_enabled) {
        // Init. de la communication I2C avec les moteurs LMA
        if (usb_serial_enable)
            USBSerial.println("Lma begin...");
        lma.begin();
    }

    if (imu_enable)
    {
        // Init. de la centrale inertielle pour la detection de mouvement
        if (usb_serial_enable)
            USBSerial.println("IMU begin...");

        swalkitDisplay.SetSwalkitState(SwalkitDisplay::SwalkitState::Calibrating);

        M5.IMU.begin();
        seuil_mouvement = 0;
        for (int i = 0; i < 10; i++)
        {
            M5.IMU.getAccel(&ax, &ay, &az);
            total_acceleration = std::sqrt(ax * ax + ay * ay + az * az);
            seuil_mouvement += total_acceleration;
            read_gyro_data();
            delay(200);
        }
        seuil_mouvement *= 0.1f;
        gyro_calibration = get_current_gyro();
        if (usb_serial_enable) {
            USBSerial.print("seuil mouvement : ");
            USBSerial.print(seuil_mouvement);
            USBSerial.print("  calibration gyro : ");
            USBSerial.println(gyro_calibration);
        }
    }


    if (sensors_enabled) {
        // Init. de la communication I2C avec les capteurs
        if (usb_serial_enable)
            USBSerial.print("sensors begin...");
        sensors.begin(false);
        if (usb_serial_enable)
            USBSerial.println("done");
    }

    if (motors_enabled) {
        // Init. de la configuration les moteurs LMA
        lma.on_off(true);
        lma.set_duty_max(UINT16_MAX);
        // lma.write(UINT16_MAX,UINT16_MAX);
    }
    
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
    float gyro_value;

    while (1) {
        if (sensors_enabled) {
            sensors.read();
        }
        now = millis();
        leftIntensity = rightIntensity = 0;

        if (imu_enable)
        {
            M5.IMU.getAccel(&ax, &ay, &az);
            total_acceleration = std::sqrt(ax * ax + ay * ay + az * az) * 0.1f + total_acceleration * 0.9f;

            read_gyro_data();
            gyro_value = abs(get_current_gyro() - gyro_calibration);

            if (abs(total_acceleration - seuil_mouvement) > acceleration_threshold || gyro_value > gyro_threshold) {
                moving = true;
                swalkitDisplay.SetImuState(SwalkitDisplay::IMUState::Moving);
                last_time_moved = now;
            } else if (now - last_time_moved > watchdog_imu_move) {
                swalkitDisplay.SetMessage(nullptr);
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

        if (motors_enabled) {
            lma.write(rightIntensity * (UINT16_MAX / 100), leftIntensity * (UINT16_MAX / 100));
        }

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
            bluetooth_enabled = !bluetooth_enabled;

            if (bluetooth_enabled)
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

void read_gyro_data() {
    float gx, gy, gz;
    M5.IMU.getGyro(&gx, &gy, &gz);
    gyro_data[gyro_index++] = gx * gx + gy * gy + gz * gz;
    if (gyro_index == gyro_data_size) {
        gyro_index = 0;
    }
}

float get_current_gyro() {
    float value = 0;
    constexpr float inv_gyro_data_size = 1.0f / gyro_data_size;
    for (size_t i = 0; i < gyro_data_size; ++i) {
        value += gyro_data[i];
    }
    return value * inv_gyro_data_size;
}   
