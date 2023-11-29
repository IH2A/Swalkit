#include <Arduino.h> // for Serial
#include "SwalkitBLE.h"
#include <WiFi.h> // only to have unique id from mac address

SwalkitBLE::SwalkitBLE(SwalkitProfile &profile, Sensors &sensors):
                sensors(sensors),
                profileRef(profile),
                pServer(nullptr),
                pService(nullptr),
                pRequestCharacteristic(nullptr),
                pProfileCharacteristic(nullptr),
                deviceConnected(false),
                oldDeviceConnected(false),
                pServerCB(nullptr), 
                pRequestCB(nullptr),
                pProfileCB(nullptr)
{}

void SwalkitBLE::start() {
    if (pServer == nullptr) {
        BLUETOOTH_DEVICE_NAME = "Swalkit " + std::string(WiFi.macAddress().substring(0, 8).c_str());

        // Create the BLE Device
        BLEDevice::init(BLUETOOTH_DEVICE_NAME);

        // Create the BLE Server
        pServer = BLEDevice::createServer();
        pServerCB = new ServerCallbacks(*this);
        pServer->setCallbacks(pServerCB);

        // Create the BLE Service
        pService = pServer->createService(SERVICE_UUID);

        // Create BLE Characteristics
        pRequestCharacteristic = pService->createCharacteristic(REQUEST_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
        pRequestCB = new RequestCallbacks(sensors);
        pRequestCharacteristic->setCallbacks(pRequestCB);

        pProfileCharacteristic = pService->createCharacteristic(PROFILE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
        pProfileCB = new ProfileCallbacks(profileRef);
        pProfileCharacteristic->setCallbacks(pProfileCB);

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

void SwalkitBLE::stop() {
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


SwalkitBLE::~SwalkitBLE() {
    stop();
    delete pServer;
    delete pServerCB;
    delete pRequestCB;
    delete pProfileCB;
}


#pragma region Server Callbacks

SwalkitBLE::ServerCallbacks::ServerCallbacks(SwalkitBLE& _BT_serial) : BT_serial(_BT_serial){}

void SwalkitBLE::ServerCallbacks::onConnect(BLEServer *pServer) {
    USBSerial.println("Server connected");
    BT_serial.deviceConnected = true;
    BLEDevice::stopAdvertising();
}

void SwalkitBLE::ServerCallbacks::onDisconnect(BLEServer *pServer) {
    USBSerial.println("Server disconnected");
    BT_serial.deviceConnected = false;
}

#pragma endregion


#pragma region Request Callbacks

SwalkitBLE::RequestCallbacks::RequestCallbacks(Sensors &sensors) : sensors(sensors) {}

void SwalkitBLE::RequestCallbacks::onRead(BLECharacteristic *pCharacteristic) {
    uint8_t data[8];
    for( int i = 0 ; i < 8; i++) data[i] = (uint8_t)(sensors.sensor_average[i]->getAverage());
    pCharacteristic->setValue(data, 4);

    USBSerial.print("Sending Request value : ");
    for (int i = 0; i < 4; i++) {
        USBSerial.print(data[i]);
        USBSerial.print(" ");
    }
    USBSerial.println();
    USBSerial.println("*********");
}


#pragma endregion

#pragma region Profile Callbacks
SwalkitBLE::ProfileCallbacks::ProfileCallbacks(SwalkitProfile &profile) : profileRef(profile) {}

void SwalkitBLE::ProfileCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    size_t data_length = pCharacteristic->getLength();
    uint8_t *data = pCharacteristic->getData();
    if (data_length > 0) {
        profileRef.fromBytes(data, data_length);
        USBSerial.print("Received profile: ");
        for (int i = 0; i < data_length; i++) {
            USBSerial.print(data[i]);
            USBSerial.print(" ");
        }
        USBSerial.println();
        USBSerial.println(profileRef.toString().c_str());
        USBSerial.println("*********");
    }
}

void SwalkitBLE::ProfileCallbacks::onRead(BLECharacteristic *pCharacteristic) {
    uint8_t *data = nullptr;
    size_t data_length;
    profileRef.toBytes(data, data_length);
    pCharacteristic->setValue(data, data_length);
    USBSerial.print("Sending profile : ");
        for (int i = 0; i < data_length; i++)
            USBSerial.print(data[i]);
        USBSerial.println();
    USBSerial.println(profileRef.toString().c_str());
    USBSerial.println("*********");
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
