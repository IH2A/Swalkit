#include "motors.h"

void Motors::begin()
{
driver.begin(&Wire, HBRIDGE_ADDR, 2, 1, 1000000L);
    driver.setDriverPWMFreq(65534);
    dir = 1;
}

void Motors::write(){
    // dir = (dir > 1) ? 1 : 2;
    driver.setDriverDirection(dir);
    driver.setDriverSpeed8Bits(255);
    // driver.setDriverSpeed16Bits( uint16_t(65534*( sin(float(millis())/1000 * 2 *3.141592)*0.5+0.5) ));
}