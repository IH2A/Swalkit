#include "SwalkitSignal.h"
#include <sstream>


void SwalkitSignal::fromBytes(const uint8_t *data) {
    intensity = static_cast<int>(data[0]);
    pulse = static_cast<int>(data[1]) * 10;
    distance = static_cast<int>(data[2]);
}
    
void SwalkitSignal::toBytes(uint8_t *data) {
    data[0] = static_cast<uint8_t>(intensity);
    data[1] = static_cast<uint8_t>(pulse / 10);
    data[2] = static_cast<uint8_t>(distance);
}

std::string SwalkitSignal::toString() {
    std::ostringstream oss;
    oss << intensity << ", " << pulse << ", " << distance;
    return oss.str();
}
