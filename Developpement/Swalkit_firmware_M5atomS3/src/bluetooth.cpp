#include <Arduino.h> // for Serial
#include "bluetooth.h"
#include <WiFi.h> // only to have unique id from mac address

Bluetooth_serial::Bluetooth_serial(): pServer(NULL),
                                       pTxCharacteristic(NULL),
                                       deviceConnected(false),
                                       oldDeviceConnected(false),
                                       txValue(0),
                                       last_rx_value("")
{}

void Bluetooth_serial::begin()
{
    BLUETOOTH_DEVICE_NAME = "Swalkit " + std::string(WiFi.macAddress().c_str());

    // Create the BLE Device
    BLEDevice::init(BLUETOOTH_DEVICE_NAME);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServerCallback = new MyServerCallbacks(*this);
    pServer->setCallbacks(pServerCallback);

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);

    pTxCharacteristic->addDescriptor(new BLE2902());

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);

    pCallback = new MyCallbacks(*this);
    pRxCharacteristic->setCallbacks(pCallback);

    // Start the service
    pService->start();

    // Start advertising
    pServer->getAdvertising()->start();
    USBSerial.println("BLE: Waiting a client connection to notify...");
}


Bluetooth_serial::~Bluetooth_serial()
{
    delete pServer;
    delete pServerCallback;
}
Bluetooth_serial::MyServerCallbacks::MyServerCallbacks(Bluetooth_serial& _BT_serial) : BT_serial(_BT_serial){}
Bluetooth_serial::MyCallbacks::MyCallbacks(Bluetooth_serial& _BT_serial) : BT_serial(_BT_serial){}

void Bluetooth_serial::task()
{
    if (deviceConnected)
    {
        pTxCharacteristic->setValue(&txValue, 1);
        pTxCharacteristic->notify();
        txValue++;
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
    else
    {
        USBSerial.println("No device connected");
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        USBSerial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

void Bluetooth_serial::MyServerCallbacks::onConnect(BLEServer *pServer)
{
    BT_serial.deviceConnected = true;
}

void Bluetooth_serial::MyServerCallbacks::onDisconnect(BLEServer *pServer)
{
    BT_serial.deviceConnected = false;
}

void Bluetooth_serial::MyCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    std::string rxValue = pCharacteristic->getValue();
    BT_serial.last_rx_value = rxValue;

    if (rxValue.length() > 0)
    {
        USBSerial.println("*********");
        USBSerial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
            USBSerial.print(rxValue[i]);

        USBSerial.println();
        USBSerial.println("*********");
    }
}

// void ISRBlueetoothWriteSensors() {
//   std::vector<int> values;
//   for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
//       values.push_back(RA_vector[i]->getAverage());
//   }
//   SerialBT.println("20"+sendIntListBT(values, "sensors"));
//   Serial.println("SEND\t20" + sendIntListBT(values, "sensors"));
// }

// void readBluetoothData() {
//   if (SerialBT.available()) {
//     Serial.println("\nreading BT");
//     // Read the whole command send via Bluetooth
//     serialCommandData = SerialBT.readStringUntil('\n');
//     if (!serialCommandData) {
//       Serial.println("Cannot read command");
//     } else {
//       Serial.println("got command: " + serialCommandData);
//       // Get wich type of command it represents
//       int serialCommand = serialCommandData.substring(0,2).toInt();
//       Serial.println("command: " + String(serialCommand));
//       String commandAtributes = serialCommandData.length() > 2 ?
//               serialCommandData.substring(2, serialCommandData.length()) :
//               "";
//       Serial.println("attributes: " + commandAtributes);
//       serialCommandOptions(serialCommand, commandAtributes);
//     }
//   }
// }
