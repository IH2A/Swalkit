// ********** bluetooth.h ***********
#ifndef __SwalkitBLE_h__
#define __SwalkitBLE_h__
// inspiré de https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/UART/UART.ino
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include "SwalkitProfile.h"
#include "sensors.h"

#define SERVICE_UUID "07632d2a-6284-4cdf-82ee-f6a70b627c61"
#define REQUEST_CHARACTERISTIC_UUID "c9f2218b-a76a-4643-b567-296dc58bffd7"
#define PROFILE_CHARACTERISTIC_UUID "7027e0e3-e02a-4346-9d4f-826bf6db7772"

class SwalkitBLE {
public:
    SwalkitBLE(SwalkitProfile &profile, Sensors &sensors, bool serrrrrrrrrrrrrrrrrr);
    ~SwalkitBLE();
    void start();
    void stop();

private:
    class ServerCallbacks : public BLEServerCallbacks {
    public:
        ServerCallbacks(SwalkitBLE &_BT_serial, bool usb_serial_log);
        void onConnect(BLEServer *pServer);
        void onDisconnect(BLEServer *pServer);

    private:
        SwalkitBLE &BT_serial;
        bool usb_serial_log;
    };

    class RequestCallbacks : public BLECharacteristicCallbacks {
    public:
        RequestCallbacks(Sensors &sensors, bool usb_serial_log);
        void onRead(BLECharacteristic *pCharacteristic) override;
    private:
        Sensors &sensors;
        bool usb_serial_log;
    };

    class ProfileCallbacks : public BLECharacteristicCallbacks {
    public:
        ProfileCallbacks(SwalkitProfile &profile, bool usb_serial_log);
        void onRead(BLECharacteristic *pCharacteristic) override;
        void onWrite(BLECharacteristic *pCharacteristic) override;
    private:
        SwalkitProfile &profileRef;
        bool usb_serial_log;
   };

    SwalkitProfile &profileRef;
    Sensors &sensors;
    bool usb_serial_log;

    BLEServer *pServer;
    BLEService *pService;

    BLECharacteristic *pRequestCharacteristic;
    BLECharacteristic *pProfileCharacteristic;
    bool deviceConnected;
    bool oldDeviceConnected;
    std::string BLUETOOTH_DEVICE_NAME;

    ServerCallbacks *pServerCB;
    RequestCallbacks *pRequestCB;
    ProfileCallbacks *pProfileCB;
};

#endif // __SwalkitBLE_h__