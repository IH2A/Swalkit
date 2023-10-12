#include <Arduino.h> // for Serial
#include "bluetooth.h"
#include <WiFi.h> // only to have unique id from mac address

Bluetooth_serial::Bluetooth_serial(): pServer(nullptr),
                pService(nullptr),
                pRequestCharacteristic(nullptr),
                pConfigurationCharacteristic(nullptr),
                deviceConnected(false),
                oldDeviceConnected(false),
                pServerCB(nullptr), 
                pRequestCB(nullptr),
                pConfigurationCB(nullptr)
{}

void Bluetooth_serial::start() {
    if (pServer == nullptr) {
        BLUETOOTH_DEVICE_NAME = "Swalkit " + std::string(WiFi.macAddress().c_str());

        // Create the BLE Device
        BLEDevice::init(BLUETOOTH_DEVICE_NAME);

        // Create the BLE Server
        pServer = BLEDevice::createServer();
        pServerCB = new ServerCallbacks(*this);
        pServer->setCallbacks(pServerCB);

        // Create the BLE Service
        pService = pServer->createService(SERVICE_UUID);

        // Create BLE Characteristics
        pRequestCharacteristic = pService->createCharacteristic(REQUEST_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
        pRequestCB = new RequestCallbacks();
        pRequestCharacteristic->setCallbacks(pRequestCB);

        pConfigurationCharacteristic = pService->createCharacteristic(CONFIGURATION_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
        pConfigurationCB = new ConfigurationCallbacks();
        pConfigurationCharacteristic->setCallbacks(pConfigurationCB);

        // Add advertising
        BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x06);
        pAdvertising->setMaxPreferred(0x12);
    }

    // Start the service
    pService->start();
    // Start advertising
    BLEDevice::startAdvertising();

    USBSerial.println("BLE: Server started");
}

void Bluetooth_serial::stop() {
    // don't delete everything here, the library is not meant to be used that way. just stop what's running.
    if (pService != nullptr) {
        BLEDevice::stopAdvertising();

        if (pServer != nullptr) {
            auto peerDevices = pServer->getPeerDevices(true);
            for (auto it : peerDevices) {
                pServer->disconnect(it.first);
            }
        }
        pService->stop();
    }

    USBSerial.println("BLE : Server stopped");
}


Bluetooth_serial::~Bluetooth_serial() {
    stop();
    delete pServer;
    delete pServerCB;
    delete pRequestCB;
    delete pConfigurationCB;
}


#pragma region Server Callbacks

Bluetooth_serial::ServerCallbacks::ServerCallbacks(Bluetooth_serial& _BT_serial) : BT_serial(_BT_serial){}

void Bluetooth_serial::ServerCallbacks::onConnect(BLEServer *pServer) {
    USBSerial.println("Server connected");
    BT_serial.deviceConnected = true;
    BLEDevice::stopAdvertising();
}

void Bluetooth_serial::ServerCallbacks::onDisconnect(BLEServer *pServer) {
    USBSerial.println("Server disconnected");
    BT_serial.deviceConnected = false;
}

#pragma endregion


#pragma region Request Callbacks
void Bluetooth_serial::RequestCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
        USBSerial.print("Received Value: ");
        USBSerial.println(value.c_str());
        USBSerial.println("*********");
    }
}
#pragma endregion

#pragma region Request Callbacks
void Bluetooth_serial::ConfigurationCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    if (value.length() > 0) {
        USBSerial.print("Received Value: ");
        for (int i = 0; i < value.length(); i++)
            USBSerial.print(value[i]);

        USBSerial.println();
        USBSerial.println("*********");
    }
}

void Bluetooth_serial::ConfigurationCallbacks::onRead(BLECharacteristic *pCharacteristic) {
    std::string value = pCharacteristic->getValue();
    for(auto it = value.begin(); it != value.end(); ++it) {
        ++*it;
    }
    USBSerial.print("Sending value : ");
        USBSerial.println(value.c_str());
    USBSerial.println("*********");
    pCharacteristic->setValue(value);
}
#pragma endregion

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
