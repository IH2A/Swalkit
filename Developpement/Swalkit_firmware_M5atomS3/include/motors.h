// ********** motors.h ***********
#ifndef motors_h
#define motors_h

#include "UNIT_HBRIDGE.h"

class Motors
{
public:
    Motors(){};
    void begin();

    UNIT_HBRIDGE driver;
    uint8_t dir;
    void write();

};

#endif // motors_h