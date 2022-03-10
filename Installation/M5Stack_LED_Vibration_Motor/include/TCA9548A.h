/*
 * The MIT License (MIT)
 * 
 * Author: Hongtai Liu (lht856@foxmail.com)
 *         Gustavo Souza Vieira Dutra
 * Copyright (C) 2019-2021 
 */


#ifndef SEEED_TCA9548A_H
#define SEEED_TCA9548A_H

//#define SOFTWAREWIRE

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifdef SOFTWAREWIRE
  #include <SoftwareWire.h>
#else   
  #include <Wire.h>
#endif

/**************************************************************************
    I2C ADDRESS/BITS
**************************************************************************/
#define TCA9548_DEFAULT_ADDRESS 0x70

/**************************************************************************
          INPUT PINS STATUS
INPUTS    ┆   I2C BUS SLAVE ADDRESS
A2 A1 A0  ┆ 
L  L  L   ┆ 112 (decimal), 70 (hexadecimal)
L  L  H   ┆ 113 (decimal), 71 (hexadecimal)
L  H  L   ┆ 114 (decimal), 72 (hexadecimal)
L  H  H   ┆ 115 (decimal), 73 (hexadecimal)
H  L  L   ┆ 116 (decimal), 74 (hexadecimal)
H  L  H   ┆ 117 (decimal), 75 (hexadecimal)
H  H  L   ┆ 118 (decimal), 76 (hexadecimal)
H  H  H   ┆ 119 (decimal), 77 (hexadecimal)
**************************************************************************/

/**************************************************************************
          CHANNEL STATUS
  -------------------------------------------------------
 ┆  B7  ┆  B6  ┆  B5  ┆  B4  ┆  B3  ┆  B2  ┆  B1  ┆  B0  ┆                                                                |
 ┆-------------------------------------------------------┆
 ┆  ch7 ┆  ch6 ┆ ch5  ┆  ch4 ┆  ch3 ┆  ch2 ┆  ch1 ┆  ch0 ┆ 
 ˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉˉ  
**************************************************************************/
#define TCA_CHANNEL_0 0x1
#define TCA_CHANNEL_1 0x2
#define TCA_CHANNEL_2 0x4
#define TCA_CHANNEL_3 0x8
#define TCA_CHANNEL_4 0x10
#define TCA_CHANNEL_5 0x20
#define TCA_CHANNEL_6 0x40
#define TCA_CHANNEL_7 0x80

template <class T>
class TCA9548A
{
    public:
        TCA9548A();
        ~TCA9548A();

        void begin(T &wire = Wire, uint8_t address = TCA9548_DEFAULT_ADDRESS);
        void beginByInputs(T &wire = Wire, bool A0 = false, bool A1 = false, bool A2 = false);
        void openChannel(uint8_t channel);
        void closeChannel(uint8_t channel);
        inline uint8_t readStatus(){return read();};
 
        void closeAll();
        void openAll();

    private:

        T * _wire;
        uint8_t _address; 
        uint8_t _channels; // status of channel

        void write(uint8_t val);
        uint8_t read();

    
};

#endif /*SEEED_TCA9548A_H*/