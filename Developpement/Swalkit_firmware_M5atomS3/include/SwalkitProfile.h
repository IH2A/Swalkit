// ********** sw_profile.h ***********
#ifndef __SwalkitProfile_h__
#define __SwalkitProfile_h__

#include <string>
#include "SwalkitSignal.h"
#include <Preferences.h>

class SwalkitProfile {
public:
    std::string name;
    SwalkitSignal frontSignal;
    SwalkitSignal dangerSignal;
    SwalkitSignal nearSignal;
    SwalkitSignal farSignal;
    uint8_t dataBuffer[32];
    bool store_flag;

    SwalkitProfile();
    void fromBytes(uint8_t *data, size_t data_length);
    void toBytes(size_t &data_length);

    bool load();
    bool store();

    std::string toString();

private:
    Preferences preferences;
    static const char *profile_namespace;
    static const char *profile_data;
};

#endif // __SwalkitProfile_h__