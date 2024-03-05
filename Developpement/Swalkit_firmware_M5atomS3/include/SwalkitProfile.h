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

    SwalkitProfile();
    void fromBytes(const uint8_t *data);
    const uint8_t* toBytes(size_t &data_length);

    bool load();
    bool store();

    std::string toString();

private:
    Preferences preferences;
    static const char *profile_namespace;
    static const char *profile_data;
    static constexpr std::size_t BufferSize = 32;
    uint8_t dataBuffer[BufferSize];
};

#endif // __SwalkitProfile_h__