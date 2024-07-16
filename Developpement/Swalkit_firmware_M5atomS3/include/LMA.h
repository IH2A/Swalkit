// ********** LMA.h ***********
#ifndef LMA_h
#define LMA_h

#include "UNIT_HBRIDGE.h"

#define SWALKIT_VERSION      21
#define NORMAL_VERSION       11

class LMA
{
public:
    LMA(){};
    void begin(uint16_t duty_max = UINT16_MAX);
    UNIT_HBRIDGE driver_right;
    UNIT_HBRIDGE driver_left;
    uint16_t write(uint16_t right, uint16_t left);
    void on_off(bool on);
    inline void set_duty_max(uint16_t duty_max_){ duty_max = duty_max_; };
    inline bool is_on(){ return dir; };

    void debug(UNIT_HBRIDGE driver, uint8_t address);

    void recover();

private:
    void tryHBridgeRecovery(UNIT_HBRIDGE &hbridge, uint8_t address);
    void checkAndUpdateFirmware(UNIT_HBRIDGE &hbridge);

    uint16_t duty_max;
    uint8_t dir;
};

#endif // LMA_h