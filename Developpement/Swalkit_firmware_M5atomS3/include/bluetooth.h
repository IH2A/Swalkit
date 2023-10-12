// ********** bluetooth.h ***********
#ifndef bluetooth_h
#define bluetooth_h
// inspiré de https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/UART/UART.ino
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#define SERVICE_UUID "07632d2a-6284-4cdf-82ee-f6a70b627c61"
#define REQUEST_CHARACTERISTIC_UUID "c9f2218b-a76a-4643-b567-296dc58bffd7"
#define CONFIGURATION_CHARACTERISTIC_UUID "7027e0e3-e02a-4346-9d4f-826bf6db7772"

class Bluetooth_serial {
public:
    Bluetooth_serial();
    ~Bluetooth_serial();
    void start();
    void stop();

    BLEServer *pServer;
    BLEService *pService;

    BLECharacteristic *pRequestCharacteristic;
    BLECharacteristic *pConfigurationCharacteristic;
    bool deviceConnected;
    bool oldDeviceConnected;
    std::string BLUETOOTH_DEVICE_NAME;

private:
    class ServerCallbacks : public BLEServerCallbacks {
    public:
        ServerCallbacks(Bluetooth_serial &_BT_serial);
        void onConnect(BLEServer *pServer);
        void onDisconnect(BLEServer *pServer);

    private:
        Bluetooth_serial &BT_serial;
    };

    class RequestCallbacks : public BLECharacteristicCallbacks {
    public:
        void onWrite(BLECharacteristic *pCharacteristic) override;
    };

    class ConfigurationCallbacks : public BLECharacteristicCallbacks {
    public:
        void onRead(BLECharacteristic *pCharacteristic) override;
        void onWrite(BLECharacteristic *pCharacteristic) override;
   };


    ServerCallbacks *pServerCB;
    RequestCallbacks *pRequestCB;
    ConfigurationCallbacks *pConfigurationCB;
};

#endif // bluetooth_h