    
/*
 * The MIT License (MIT)
 * 
 * Authors: Hongtai Liu (lht856@foxmail.com)
 *          Gustavo Dutra
 * Copyright (C) 2019-2021  Seeed Technology Co.,Ltd. 
 * 
 */

#include "TCA9548A.h"

#ifdef SOFTWAREWIRE
  #include <SoftwareWire.h>
#else   
  #include <Wire.h>
#endif

template<class T>
TCA9548A<T>::TCA9548A(void)
{
   
}

template<class T>
TCA9548A<T>::~TCA9548A(void)
{
   
}

template<class T>
void TCA9548A<T>::begin(T &wire, uint8_t address)
{
     this->_address = address;
     this->_wire = &wire;
     this->_channels = 0x00;
     this->_wire->begin();

     this->closeAll();  // defalut close all channel;
}

template<class T>
void TCA9548A<T>::beginByInputs(T &wire, bool A0, bool A1, bool A2)
{
     int A0_value = A0 ? 1 : 0;
     int A1_value = A1 ? 2 : 0;
     int A2_value = A2 ? 4 : 0;
     this->_address = 112 + A0_value + A1_value + A2_value;
     this->_wire = &wire;
     this->_channels = 0x00;
     this->_wire->begin();

     this->closeAll();  // defalut close all channel;
}

template<class T>
void TCA9548A<T>::openChannel(uint8_t channel)
{
    this->_channels |= channel;
    write(this->_channels);
}

template<class T>
void TCA9548A<T>::closeChannel(uint8_t channel)
{  
    this->_channels ^= channel;

    write(this->_channels);
}

template<class T>
void TCA9548A<T>::openAll()
{
    this->_channels = 0xFF;
    write(this->_channels);
}

template<class T>
void TCA9548A<T>::closeAll()
{
    this->_channels = 0x00;
    write(this->_channels);
}


template<class T>
void TCA9548A<T>::write(uint8_t value)
{
    this->_wire->beginTransmission(this->_address);
    this->_wire->write(value);
    this->_wire->endTransmission(true);
}

template<class T>
uint8_t TCA9548A<T>::read()
{
    uint8_t status = 0;
    this->_wire->requestFrom((uint16_t)this->_address, (uint8_t)1, (uint8_t)1);

    if(!this->_wire->available())
        return 255;

    status = this->_wire->read();
    
    this->_channels = status;
    return status;
}

#ifdef SOFTWAREWIRE
  template class TCA9548A<SoftwareWire>;
#else
  template class TCA9548A<TwoWire>;
#endif