// ********** sw_profile.h ***********
#ifndef __SwalkitProfile_h__
#define __SwalkitProfile_h__

#include <string>
#include "SwalkitSignal.h"

class SwalkitProfile {
public:
    std::string name;
    SwalkitSignal frontSignal;
    SwalkitSignal dangerSignal;
    SwalkitSignal nearSignal;
    SwalkitSignal farSignal;

    void fromBytes(uint8_t *data, size_t data_length);
    void toBytes(uint8_t *&data, size_t &data_length);

    std::string toString();
};

#endif // __SwalkitProfile_h__