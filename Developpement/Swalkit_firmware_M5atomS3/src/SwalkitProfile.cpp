#include "SwalkitProfile.h"
#include <cstring>
#include <sstream>

const char *SwalkitProfile::profile_namespace = "swalkit";
const char *SwalkitProfile::profile_data = "profile_data";

SwalkitProfile::SwalkitProfile() {
    // set default values
    name = "default";
    frontSignal.set(100, 100, 20);
    dangerSignal.set(100, 200, 30);
    nearSignal.set(100, 0, 40);
    farSignal.set(100, 0, 50);
}

void SwalkitProfile::fromBytes(uint8_t *data, size_t data_length) {
    size_t name_length = data[0];
    name = std::string(reinterpret_cast<char *>(data + 1), name_length);
    uint8_t *pData = data + name_length + 1;
    frontSignal.fromBytes(pData + 0);
    dangerSignal.fromBytes(pData + 3);
    nearSignal.fromBytes(pData + 6);
    farSignal.fromBytes(pData + 9);
}

void SwalkitProfile::toBytes(uint8_t *&data, size_t &data_length) {
    size_t name_length = name.size();
    data_length = name_length + 13;
    data = new uint8_t[data_length];
    data[0] = name_length;
    std::memcpy(data + 1, name.data(), name_length);
    uint8_t *pData = data + name_length + 1;
    frontSignal.toBytes(pData + 0);
    dangerSignal.toBytes(pData + 3);
    nearSignal.toBytes(pData + 6);
    farSignal.toBytes(pData + 9);
}

bool SwalkitProfile::load() {
    bool result = false;
    if (preferences.begin(profile_namespace, true)) {
        if (!preferences.isKey(profile_data)) {
            // if no profile exists : create it first and reopen the namespace
            USBSerial.println("No profile found");
            preferences.end();
            store();
            if (!preferences.begin(profile_namespace, true)) {
                USBSerial.println("Unable to reopen preferences namespace");
                return false;
            }
        }

        size_t data_length = preferences.getBytesLength(profile_data);
        if (data_length > 0) {
            uint8_t *data = new uint8_t[data_length];
            if (data_length == preferences.getBytes(profile_data, data, data_length)) {
                fromBytes(data, data_length);
                result = true;
            } else {
                USBSerial.println("Unable to get profile data");
            }
            delete[] data;
        } else {
            USBSerial.println("Unable to get profile data length");
        }
        preferences.end();
    } else {
        USBSerial.println("Unable to open preferences namespace");
    }
    return result;
}

bool SwalkitProfile::store() {
    bool result = false;
    uint8_t *data = nullptr;
    size_t data_length;

    USBSerial.print("Saving profile: ");

    toBytes(data, data_length);
    
    if (preferences.begin(profile_namespace, false)) {
        result = (data_length == preferences.putBytes(profile_data, data, data_length));
        preferences.end();
    }
    USBSerial.println(result ? "OK" : "NOK");
    return result;
}

std::string SwalkitProfile::toString() {
    std::ostringstream oss;
    oss << name << " Front : " << frontSignal.toString() << ", Danger : " << dangerSignal.toString() << ", Near : " << nearSignal.toString() << ", Far : " << farSignal.toString();
    return oss.str();
}
