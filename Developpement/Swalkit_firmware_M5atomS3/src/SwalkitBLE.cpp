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
    uint8_t data[NUMBER_OF_SENSORS];
    for( int i = 0 ; i < NUMBER_OF_SENSORS; i++) data[i] = (uint8_t)(sensors.sensor_average[i]->getAverage());
    pCharacteristic->setValue(data, NUMBER_OF_SENSORS);

    USBSerial.print("Sending Request value : ");
    for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
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
    if (data_length > 0) {
        uint8_t *data = pCharacteristic->getData();
        profileRef.fromBytes(data);
        USBSerial.print("Received profile: ");
        USBSerial.println(profileRef.toString().c_str());
        profileRef.store();
        USBSerial.println("->profileRef.store()");
    }
}

void SwalkitBLE::ProfileCallbacks::onRead(BLECharacteristic *pCharacteristic) {
    size_t data_length;
	const uint8_t *data_bytes = profileRef.toBytes(data_length);
    pCharacteristic->setValue(const_cast<uint8_t*>(data_bytes), data_length);
    USBSerial.print("Sending profile : ");
    USBSerial.println(profileRef.toString().c_str());
}
#pragma endregion

