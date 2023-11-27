// ********** sw_signal.h ***********
#ifndef __sw_signal_h__
#define __sw_signal_h__

#include <string>

class SwalkitSignal {
public:
    int frequency;
    int pulse;
    int distance;

    void fromBytes(const uint8_t *data);
    void toBytes(uint8_t *data);

    std::string toString();
};

#endif // __sw_signal_h__