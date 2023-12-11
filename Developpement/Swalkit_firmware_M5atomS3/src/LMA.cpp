#include "LMA.h"
#include "fw.h"

void LMA::begin(uint16_t duty_max)
{
    dir = 1;
    uint8_t version = 0xFF;

    if(!driver_right.begin(&Wire, HBRIDGE_ADDR, 2, 1, 1000000L))
    {
        USBSerial.println("HBridge Missing");
    }
    else
    {
        driver_right.setDriverPWMFreq(65534);
        driver_right.setDriverDirection(dir);
        version = driver_right.getFirmwareVersion();
        USBSerial.printf("Version right driver: %d\n", version);
        if(version != SWALKIT_VERSION){
            USBSerial.printf("Version %d != %d\n", version, SWALKIT_VERSION);
            USBSerial.printf("Jumping to BootLoader\n");
            driver_right.jumpBootloader();
            USBSerial.printf("Updating Firmware :");
            if(driver_right.updateFW(Hbridge_bin, sizeof(Hbridge_bin))){
            USBSerial.printf("OK\n");
            }else{
            USBSerial.printf("NOK\n");
            USBSerial.printf("Error\n");
            while(1);
            }
            delay(1000);
            USBSerial.printf("Restarting HBridge\n");
            driver_right.startApp();
            delay(1000);
            version = 0xFF;
            version = driver_right.getFirmwareVersion();
            USBSerial.printf("New Version : %d\n", version);
            if(version != SWALKIT_VERSION){
            USBSerial.printf("Version %d != %d\n", version, SWALKIT_VERSION);
            USBSerial.printf("Error\n");
            while(1);
            }
        }

    }

    if(!driver_left.begin(&Wire, HBRIDGE_ADDR+1, 2, 1, 1000000L))
    {
        USBSerial.println("HBridge Missing");
    }
    else
    {
        driver_left.setDriverPWMFreq(65534);
        driver_left.setDriverDirection(dir);
        version = driver_left.getFirmwareVersion();
        USBSerial.printf("Version left driver: %d\n", version);
        if(version != SWALKIT_VERSION){
            USBSerial.printf("Version %d != %d\n", version, SWALKIT_VERSION);
            USBSerial.printf("Jumping to BootLoader\n");
            driver_left.jumpBootloader();
            USBSerial.printf("Updating Firmware :");
            if(driver_left.updateFW(Hbridge_bin, sizeof(Hbridge_bin))){
            USBSerial.printf("OK\n");
            }else{
            USBSerial.printf("NOK\n");
            USBSerial.printf("Error\n");
            while(1);
            }
            delay(1000);
            USBSerial.printf("Restarting HBridge\n");
            driver_left.startApp();
            delay(1000);
            version = 0xFF;
            version = driver_left.getFirmwareVersion();
            USBSerial.printf("New Version : %d\n", version);
            if(version != SWALKIT_VERSION){
            USBSerial.printf("Version %d != %d\n", version, SWALKIT_VERSION);
            USBSerial.printf("Error\n");
            while(1);
            }
        }
    }
}

void LMA::write(uint16_t right, uint16_t left){
    dir = (dir > 1) ? 1 : 0;
    driver_right.setDriverSpeed16Bits( (right < duty_max) ? right : duty_max );
    driver_left.setDriverSpeed16Bits( (left < duty_max) ? left : duty_max );
}

void LMA::on_off(bool on)
{
    dir = (on) ? 1 : 0;
    driver_right.setDriverDirection(dir);
    driver_left.setDriverDirection(dir);
}