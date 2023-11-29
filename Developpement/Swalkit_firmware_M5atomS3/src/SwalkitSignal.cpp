#include "SwalkitSignal.h"
#include <sstream>

void SwalkitSignal::set(int frequency, int pulse, int distance) {
    this->frequency = frequency;
    this->pulse = pulse;
    this->distance = distance;
}

void SwalkitSignal::fromBytes(const uint8_t *data) {
    frequency = static_cast<int>(data[0]);
    pulse = static_cast<int>(data[1]) * 10;
    distance = static_cast<int>(data[2]);
}
    
void SwalkitSignal::toBytes(uint8_t *data) {
    data[0] = static_cast<uint8_t>(frequency);
    data[1] = static_cast<uint8_t>(pulse / 10);
    data[2] = static_cast<uint8_t>(distance);
}

std::string SwalkitSignal::toString() {
    std::ostringstream oss;
    oss << frequency << ", " << pulse << ", " << distance;
    return oss.str();
}
