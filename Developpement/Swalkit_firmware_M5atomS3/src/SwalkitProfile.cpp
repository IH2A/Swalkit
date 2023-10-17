#include "SwalkitProfile.h"
#include <cstring>
#include <sstream>

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

std::string SwalkitProfile::toString() {
    std::ostringstream oss;
    oss << name << " Front : " << frontSignal.toString() << ", Danger : " << dangerSignal.toString() << ", Near : " << nearSignal.toString() << ", Far : " << farSignal.toString();
    return oss.str();
}
