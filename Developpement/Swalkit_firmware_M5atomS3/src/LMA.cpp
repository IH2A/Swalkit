#include "LMA.h"

void LMA::begin(uint16_t duty_max)
{
    dir = 1;

    driver_right.begin(&Wire, HBRIDGE_ADDR, 2, 1, 1000000L);
    driver_right.setDriverPWMFreq(65534);
    driver_right.setDriverDirection(dir);
    
    driver_left.begin(&Wire, HBRIDGE_ADDR+1, 2, 1, 1000000L);
    driver_left.setDriverPWMFreq(65534);
    driver_left.setDriverDirection(dir);
}

void LMA::write(uint16_t right, uint16_t left){
    dir = (dir > 1) ? 1 : 0;
    driver_right.setDriverSpeed16Bits( (right < duty_max) ? right : duty_max );
    driver_left.setDriverSpeed16Bits( (left < duty_max) ? left : duty_max );
}

void LMA::on_off(bool on)
{
    dir = (on) ? 1 : 0;
    driver_right.setDriverDirection(dir);
    driver_left.setDriverDirection(dir);
}