#include <Arduino.h>
#include <M5AtomS3.h>
#include "SwalkitBle.h"
#include "SwalkitProfile.h"
#include "SwalkitDisplay.h"
#include "sensors.h"
#include "LMA.h"

#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MIN3(X, Y, Z) (MIN(X, MIN(Y, Z)))

using namespace std;

// Configuration générale
String swalkit_version{"Analyse Swalkit 1.2.0"};

// imu
float total_acceleration;
float seuil_mouvement;
float ax, ay, az; // accelerations
constexpr size_t gyro_data_size = 10;
float gyro_data[gyro_data_size];
size_t gyro_index;
float gyro_calibration = 0;
float gyro_threshold = 50;
bool moving = true;
unsigned long watchdog_imu_move = 5000; // 5 secondes
unsigned long last_time_moved = 0;      // secondes
float acceleration_threshold = 0.015f;  // relative to calibration

//I2C
void I2C_scan();

// Configuration Bluetooth
Sensors sensors;
SwalkitDisplay swalkitDisplay{};

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

// gyro
void read_gyro_data();
float get_current_gyro();


void setup()
{
    // Initialisation du M5stack
    M5.begin(true, true, true, false);

    swalkitDisplay.Init();
    swalkitDisplay.SetSwalkitState(SwalkitDisplay::SwalkitState::Initialising);
    USBSerial.println("Starting...");
    swalkitDisplay.SetMessage(swalkit_version);
        
    // default in M5atomS3 is Wire1 tu use MPU6886, but we are also using Wire on grove (pins 2 and 1)
    Wire.endTransmission();
    Wire.begin(2, 1, 100000L);

    // // Init. de la communication I2C avec les capteurs
    swalkitDisplay.PressToContinue("Set sensor address");
    swalkitDisplay.SetMessage("Wait...");    
    sensors.begin(true); //true = debug

    swalkitDisplay.PressToContinue("Scan des Bus i2C:");
    I2C_scan();

    // Init. de la communication I2C avec les moteurs LMA
    swalkitDisplay.PressToContinue("Tests Hbridge droite");
    lma.debug(lma.driver_right, HBRIDGE_ADDR+1);
    swalkitDisplay.PressToContinue("Tests Hbridge gauche");
    lma.debug(lma.driver_left, HBRIDGE_ADDR);
    
    swalkitDisplay.SetMessage("Done");
}

void I2C_scan()
{
    byte error;
    //{smart hub gauche, smart hub droite, 
    //      VLGauche->droite,
    //      LMA gauche, LMA droite,
    //      AccGyro
    byte address[13] = 
    {
        0x70,0x73,
        0x2A, 0x2B, 0x2C, 0x2D,0x2F, 0x2E, 0x29, 0x30,
        HBRIDGE_ADDR, HBRIDGE_ADDR+1,
        MPU6886_DEFAULT_ADDRESS
    };

    bool use_wire1[13] = 
    {
        0,0,
        0,0,0,0,0,0,0,0,
        0,0,
        1
    };

    String sub_msgs[13] =
    {
        "hubg","hubd",
        "c1","c2","c3","c4","c5","c6","c7","c8",
        "Hg","Hd",
        "gyro"
    };

    String messageOk("OK: ");
    String messageFail("FAIL: ");
    String messageErr("ERR: ");


    for (int address_index = 0; address_index < 13; address_index++)
    {
        TwoWire* bus = (use_wire1[address_index]) ? &Wire1 : &Wire;

        bus->beginTransmission(address[address_index]);
        error = bus->endTransmission();
        if (error == 0)
        {
            messageOk += sub_msgs[address_index] + " ";
        }
        else if (error == 2)
        {
            messageFail += sub_msgs[address_index] + " ";
        }
        else if (error != 2)
        {
            messageErr += sub_msgs[address_index] + String(error) +" ";
        }
    }

    swalkitDisplay.PressToContinue(messageOk);
    swalkitDisplay.PressToContinue(messageFail);
    swalkitDisplay.PressToContinue(messageErr);

}

void loop()
{
   
}