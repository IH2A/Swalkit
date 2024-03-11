#include <Arduino.h> // for Serial
#include "SwalkitBLE.h"
#include <WiFi.h> // only to have unique id from mac address
#include "SwalkitDisplay.h"

SwalkitBLE::SwalkitBLE(SwalkitProfile &profile, Sensors &sensors, bool usb_serial_log):
                sensors(sensors),
                profileRef(profile),
                usb_serial_log(usb_serial_log),
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
        pServerCB = new ServerCallbacks(*this, usb_serial_log);
        pServer->setCallbacks(pServerCB);

        // Create the BLE Service
        pService = pServer->createService(SERVICE_UUID);

        // Create BLE Characteristics
        pRequestCharacteristic = pService->createCharacteristic(REQUEST_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
        pRequestCB = new RequestCallbacks(sensors, usb_serial_log);
        pRequestCharacteristic->setCallbacks(pRequestCB);

        pProfileCharacteristic = pService->createCharacteristic(PROFILE_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
        pProfileCB = new ProfileCallbacks(profileRef, usb_serial_log);
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
    SwalkitDisplay::GetInstance().SetBluetoothState(SwalkitDisplay::BluetoothState::Enabled);

    if (usb_serial_log)
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
    SwalkitDisplay::GetInstance().SetBluetoothState(SwalkitDisplay::BluetoothState::Disabled);

    if (usb_serial_log)
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

SwalkitBLE::ServerCallbacks::ServerCallbacks(SwalkitBLE& _BT_serial, bool usb_serial_log) : BT_serial(_BT_serial), usb_serial_log() {}

void SwalkitBLE::ServerCallbacks::onConnect(BLEServer *pServer) {
    if (usb_serial_log) USBSerial.println("Server connected");
    BT_serial.deviceConnected = true;
    BLEDevice::stopAdvertising();
    SwalkitDisplay::GetInstance().SetBluetoothState(SwalkitDisplay::BluetoothState::Connected);
}

void SwalkitBLE::ServerCallbacks::onDisconnect(BLEServer *pServer) {
    if (usb_serial_log) USBSerial.println("Server disconnected");
    BT_serial.deviceConnected = false;
    BLEDevice::startAdvertising();
    SwalkitDisplay::GetInstance().SetBluetoothState(SwalkitDisplay::BluetoothState::Enabled);
}

#pragma endregion


#pragma region Request Callbacks

SwalkitBLE::RequestCallbacks::RequestCallbacks(Sensors &sensors, bool usb_serial_log) : sensors(sensors), usb_serial_log(usb_serial_log) {}

void SwalkitBLE::RequestCallbacks::onRead(BLECharacteristic *pCharacteristic) {
    uint8_t data[NUMBER_OF_SENSORS];
    for( int i = 0 ; i < NUMBER_OF_SENSORS; i++) data[i] = (uint8_t)(sensors.sensor_average[i]->getAverage());
    pCharacteristic->setValue(data, NUMBER_OF_SENSORS);

    if (usb_serial_log) {
        USBSerial.print("Sending Request value : ");
        for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
            USBSerial.print(data[i]);
            USBSerial.print(" ");
        }
        USBSerial.println();
        USBSerial.println("*********");
    }
}


#pragma endregion

#pragma region Profile Callbacks
SwalkitBLE::ProfileCallbacks::ProfileCallbacks(SwalkitProfile &profile, bool usb_serial_log) : profileRef(profile), usb_serial_log(usb_serial_log) {}

void SwalkitBLE::ProfileCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    size_t data_length = pCharacteristic->getLength();
    if (data_length > 0) {
        uint8_t *data = pCharacteristic->getData();
        profileRef.fromBytes(data);
        if (usb_serial_log) {
            USBSerial.print("Received profile: ");
            USBSerial.println(profileRef.toString().c_str());
        }
        profileRef.store();
    }
}

void SwalkitBLE::ProfileCallbacks::onRead(BLECharacteristic *pCharacteristic) {
    size_t data_length;
	const uint8_t *data_bytes = profileRef.toBytes(data_length);
    pCharacteristic->setValue(const_cast<uint8_t*>(data_bytes), data_length);
    if (usb_serial_log) {
        USBSerial.print("Sending profile : ");
        USBSerial.println(profileRef.toString().c_str());
    }
}
#pragma endregion

