// ********** LMA.h ***********
#ifndef LMA_h
#define LMA_h

// #include "UNIT_HBRIDGE.h"

class LMA
{
public:
    LMA(){};
    void begin(uint16_t frequency = 70, uint16_t duty_max = 1024);
    // UNIT_HBRIDGE driver;
    void write();
    void set_duty_max();
    void set_frequency();

private:
    uint16_t duty_max;
    uint16_t frequency;
};

#endif // LMA_h