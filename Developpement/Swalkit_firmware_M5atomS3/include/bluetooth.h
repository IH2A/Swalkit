// ********** bluetooth.h ***********
#ifndef bluetooth_h
#define bluetooth_h
// inspiré de https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/UART/UART.ino
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class Bluetooth_serial
{
public:
    Bluetooth_serial();
    ~Bluetooth_serial();
    void begin();
    void task();

    BLEServer *pServer;
    BLECharacteristic *pTxCharacteristic;
    bool deviceConnected;
    bool oldDeviceConnected;
    uint8_t txValue;
    std::string last_rx_value;
    std::string BLUETOOTH_DEVICE_NAME;

private:
    class MyServerCallbacks : public BLEServerCallbacks
    {
    public:
        MyServerCallbacks(Bluetooth_serial &_BT_serial);
        void onConnect(BLEServer *pServer);
        void onDisconnect(BLEServer *pServer);

    private:
        Bluetooth_serial &BT_serial;
    };

    class MyCallbacks : public BLECharacteristicCallbacks
    {
    public:
        MyCallbacks(Bluetooth_serial &_BT_serial);
        void onWrite(BLECharacteristic *pCharacteristic);

    private:
        Bluetooth_serial &BT_serial;
    };

    MyServerCallbacks *pServerCallback;
    MyCallbacks *pCallback;
};

#endif // bluetooth_h