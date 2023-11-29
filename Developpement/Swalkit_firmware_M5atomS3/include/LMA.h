// ********** LMA.h ***********
#ifndef LMA_h
#define LMA_h

#include "UNIT_HBRIDGE.h"

class LMA
{
public:
    LMA(){};
    void begin(uint16_t duty_max = 1024);
    UNIT_HBRIDGE driver_right;
    UNIT_HBRIDGE driver_left;
    void write(uint16_t right, uint16_t left);
    void on_off(bool on);
    inline void set_duty_max(uint16_t duty_max_){ duty_max = duty_max_; };
    inline bool is_on(){ return dir; };

private:
    uint16_t duty_max;
    uint8_t dir;
};

#endif // LMA_h