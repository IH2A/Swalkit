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
String swalkit_version{"Hbridge Recovery 1.2.0"};

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

void setup()
{
    // Initialisation du M5stack
    M5.begin(true, true, true, false);

    swalkitDisplay.Init();
    swalkitDisplay.SetSwalkitState(SwalkitDisplay::SwalkitState::Initialising);
    swalkitDisplay.SetMessage(swalkit_version);
        
    // default in M5atomS3 is Wire1 tu use MPU6886, but we are also using Wire on grove (pins 2 and 1)
    Wire.endTransmission();
    Wire.begin(2, 1, 100000L);

    // Init. de la communication I2C avec les moteurs LMA
    lma.recover();
    
    swalkitDisplay.SetMessage("Done");
}

void loop()
{
   
}