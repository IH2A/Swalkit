#include "LMA.h"
#include "fw.h"
#include "SwalkitDisplay.h"

void LMA::begin(uint16_t duty_max)
{
    dir = 1;

    tryHBridgeRecovery(driver_right, HBRIDGE_ADDR + 1);
    checkAndUpdateFirmware(driver_right);

    tryHBridgeRecovery(driver_left, HBRIDGE_ADDR);
    checkAndUpdateFirmware(driver_left);

    set_duty_max(duty_max);
}

uint16_t LMA::write(uint16_t right, uint16_t left)
{
    uint8_t lresult, rresult;
    dir = (dir >= 1) ? 1 : 0;
    lresult = driver_right.setDriverSpeed16Bits((right < duty_max) ? right : duty_max);
    rresult = driver_left.setDriverSpeed16Bits((left < duty_max) ? left : duty_max);
    // USBSerial.printf("dir %d val %d\n", dir, left);
    return (lresult << 8) | rresult;
}

void LMA::on_off(bool on)
{
    dir = (on) ? 1 : 0;
    driver_right.setDriverDirection(dir);
    driver_left.setDriverDirection(dir);
}

void LMA::tryHBridgeRecovery(UNIT_HBRIDGE &hbridge, uint8_t address)
{
    SwalkitDisplay &swalkitDisplay = SwalkitDisplay::GetInstance();

    if (!hbridge.begin(&Wire, address, 2, 1, 100000L))
    {
        USBSerial.println("HBridge Missing");
        hbridge.attemptRecoveryBootloader();
        swalkitDisplay.PressToContinue("Connecter H");

        swalkitDisplay.SetMessage("Attempting Recovery : ");

        if (hbridge.performRecovery(Hbridge_bin, sizeof(Hbridge_bin)))
        {
            hbridge.startApp();
            swalkitDisplay.PressToContinue("Recovery OK");
        }
        else
        {
            swalkitDisplay.PressToContinue("Recovery failed");
        }
    }
    else
    {
        USBSerial.println("HBridge Ok");
        swalkitDisplay.SetError("Restart avec H deconnecte");
        while (1)
            ;
    }
}

void LMA::recover()
{
    SwalkitDisplay &swalkitDisplay = SwalkitDisplay::GetInstance();
    swalkitDisplay.PressToContinue("H-recovery: debranchez tout");
    swalkitDisplay.PressToContinue("preparez H droite sans brancher");
    tryHBridgeRecovery(driver_right, HBRIDGE_ADDR+1);
    swalkitDisplay.PressToContinue("preparez H gauche sans brancher");
    tryHBridgeRecovery(driver_left, HBRIDGE_ADDR);
}

void LMA::checkAndUpdateFirmware(UNIT_HBRIDGE &hbridge)
{
    uint8_t version = hbridge.getFirmwareVersion();

    if (version == NORMAL_VERSION)
    {
        USBSerial.printf("Version %d != %d\n", version, SWALKIT_VERSION);

        USBSerial.printf("Jumping to BootLoader\n");
        hbridge.jumpBootloader();

        USBSerial.printf("Updating Firmware :");
        if (hbridge.updateFW(Hbridge_bin, sizeof(Hbridge_bin)))
        {
            USBSerial.printf("OK\n");
        }
        else
        {
            USBSerial.printf("NOK\n");
            USBSerial.printf("Error\n");
            while (1)
                ;
        }

        delay(1000);

        USBSerial.printf("Restarting HBridge\n");
        hbridge.startApp();

        delay(1000);
        version = 0xFF;
        version = hbridge.getFirmwareVersion();
        USBSerial.printf("New Version : %d\n", version);
        if (version != SWALKIT_VERSION)
        {
            USBSerial.printf("Version %d != %d\n", version, SWALKIT_VERSION);
            USBSerial.printf("Error\n");
            while (1)
                ;
        }
    }
}

void LMA::debug(UNIT_HBRIDGE driver, uint8_t address)
{
    SwalkitDisplay &swalkitDisplay = SwalkitDisplay::GetInstance();

    if (!driver.begin(&Wire, address, 2, 1, 100000L))
    {
        swalkitDisplay.PressToContinue("H not connected id " + String(address));
    }
    else
    {
        // check firmware
        uint8_t version = driver.getFirmwareVersion();
        swalkitDisplay.PressToContinue("H version: " + String(version));
        USBSerial.printf("Version %hhu ?= %hhu\n", version, SWALKIT_VERSION);

        if (version == NORMAL_VERSION)
        {
            swalkitDisplay.PressToContinue("Jumping to BootLoader");
            driver.jumpBootloader();

            swalkitDisplay.SetMessage("Updating Firmware : wait...");
            if (driver.updateFW(Hbridge_bin, sizeof(Hbridge_bin)))
            {
                swalkitDisplay.PressToContinue("OK");
            }
            else
            {
                USBSerial.printf("Error\n");
                swalkitDisplay.PressToContinue("Error");
                return;
            }

            swalkitDisplay.SetMessage("Restarting Hbridge: wait...");
            driver.startApp();

            delay(1000);
            version = 0xFF;
            version = driver.getFirmwareVersion();
            swalkitDisplay.PressToContinue("New version " + String(version) + " should be " + String(SWALKIT_VERSION));
        }
        else
        {
            driver.setDriverPWMFreq(40000);
            swalkitDisplay.PressToContinue("PWM freq=" + String(driver.getDriverPWMFreq()));
            swalkitDisplay.PressToContinue("Ready to test on H bridge");

            swalkitDisplay.SetMessage("Running normal mode");
            unsigned long t0 = millis();
            unsigned long now = millis();
            driver.setDriverSpeed16Bits(UINT16_MAX);
            do
            {
                driver.setDriverDirection(DIR_FORWARD);
                delay(14);
                driver.setDriverDirection(DIR_REVERSE);
                delay(14);
                now = millis();
            } while (now - t0 < 5000);
            driver.setDriverSpeed16Bits(0);

            swalkitDisplay.PressToContinue("Ready to test on H bridge");
            swalkitDisplay.SetMessage("Running Swalkit mode");
            t0 = millis();
            now = millis();
            driver.setDriverSpeed16Bits(UINT16_MAX);
            do
            {
                now = millis();
            } while (now - t0 < 5000);
            driver.setDriverSpeed16Bits(0);

            swalkitDisplay.PressToContinue("Tests on H bridge done");
        }
    }
}