#include "LMA.h"
#include "fw.h"
#include "SwalkitDisplay.h"

void LMA::begin(uint16_t duty_max)
{
    dir = 1;

    tryHBridgeRecovery(driver_right, HBRIDGE_ADDR+1);
    checkAndUpdateFirmware(driver_right);

    tryHBridgeRecovery(driver_left, HBRIDGE_ADDR);
    checkAndUpdateFirmware(driver_left);

    set_duty_max(duty_max);
}

uint16_t LMA::write(uint16_t right, uint16_t left){
    uint8_t lresult, rresult;
    dir = (dir >= 1) ? 1 : 0;
    lresult = driver_right.setDriverSpeed16Bits( (right < duty_max) ? right : duty_max );
    rresult = driver_left.setDriverSpeed16Bits( (left < duty_max) ? left : duty_max );
    //USBSerial.printf("dir %d val %d\n", dir, left);
    return (lresult << 8) | rresult;
}

void LMA::on_off(bool on)
{
    dir = (on) ? 1 : 0;
    driver_right.setDriverDirection(dir);
    driver_left.setDriverDirection(dir);
}

void LMA::tryHBridgeRecovery(UNIT_HBRIDGE &hbridge, uint8_t address) {
    SwalkitDisplay &swalkitDisplay = SwalkitDisplay::GetInstance();

    if(!hbridge.begin(&Wire, address, 2, 1, 100000L))
    {
        USBSerial.println("HBridge Missing");
        hbridge.attemptRecoveryBootloader();
        swalkitDisplay.SetMessage("Connect HBridge and Press Screen ...");

        do{
            M5.update();
        } while(!M5.Btn.wasReleased());

        swalkitDisplay.SetMessage("Attempting Recovery : ");

        if(hbridge.performRecovery(Hbridge_bin, sizeof(Hbridge_bin)))
        {
            hbridge.startApp();
            swalkitDisplay.SetMessage("Recovery OK");
        } 
        else 
        {
            swalkitDisplay.SetError("Recovery failed");
            while(1);
        }
    } 
    else 
    {
        USBSerial.println("HBridge Ok");
    }
}

void LMA::checkAndUpdateFirmware(UNIT_HBRIDGE &hbridge) {
    uint8_t version = hbridge.getFirmwareVersion();

    if(version == NORMAL_VERSION)
    {
        USBSerial.printf("Version %d != %d\n", version, SWALKIT_VERSION);
        
        USBSerial.printf("Jumping to BootLoader\n");
        hbridge.jumpBootloader();
        
        USBSerial.printf("Updating Firmware :");
        if(hbridge.updateFW(Hbridge_bin, sizeof(Hbridge_bin))){
        USBSerial.printf("OK\n");
        }else{
        USBSerial.printf("NOK\n");
        USBSerial.printf("Error\n");
        while(1);
        }
        
        delay(1000);
        
        USBSerial.printf("Restarting HBridge\n");
        hbridge.startApp();
        
        delay(1000);        
        version = 0xFF;
        version = hbridge.getFirmwareVersion();
        USBSerial.printf("New Version : %d\n", version);
        if(version != SWALKIT_VERSION){
        USBSerial.printf("Version %d != %d\n", version, SWALKIT_VERSION);
        USBSerial.printf("Error\n");
        while(1);
        }
    }
}
