// ********** DacESP32.h ***********
#ifndef DacESP32_h
#define DacESP32_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
 
#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
 
#include "driver/dac.h"

/* Declare global sine waveform parameters
 * so they may be then accessed and changed from debugger
 * over an JTAG interface
 */
 
// Esp32 @ 240 MHz:
 
// f=125.6*step/(div+1)            [div=0..7] [calc: (div+1)]
 
// step0:
// 0=15.61Hz 1=--
 
// step 1:
// 0 = 125.6 Hz 1 = 62.50 Hz 2 = 41.67Hz 3=31.35Hz 4=25.00Hz 5=20.92Hz 6=17.86Hz 7=15.63Hz
 
// step 2:125,6
 
// 0= 250.0 Hz 1=125Hz 2=83.68 Hz 3=62.5 Hz 4=50.1 Hz 5=41.74Hz  6=35.74 Hz  7=31.35Hz
 
 // step 10
 // 0 = 1.248 kHz 1 = 625.4 Hz 2 = 417.4 Hz (noisy) 3 = 31.33 Hz 4 = 250.6  5 = 208.3 Hz  6=178.9 7=156.3
 
 // step 100:
 // 0: 12.53 kHz  1 = 6.266 kHz 2=4.173kHz 3= 3.127 kHz 4=2.505 kHz 6=1.789 kHz 7=1.565KHz
 
 
// step 500:
// 0: 62.54 kHz (noisy) 1 = 31.33 kHz 2 = 20.86 kHz  5 = 10.5 kHz   6 =20.88 KhZ  7=
 
// step 1000:
// 0 = 125 kHz, 1 = 62.11 kHz, 5 = 20.73 kHz    ==>>> 10 = 42.02 kHz 100
 
// step 2000
// 0: 249.4 kHz(unten verzerrt)  1 = 125.8 kHz (noisy)  5 = 41.5 kHz (noisy)
 

class DACControlESP32 {
    private:
        uint8_t scale = 1;           // 1 == 50% of the full scale // scale 0 == fill scale = bad signal (no longer a sine wave)
        uint8_t offset;              // leave it default / 0 = no any offset
        uint8_t invert = 2;          // invert MSB to get sine waveform  // 3: invert 180°
    public:
        uint8_t clk_8m_div = 1;      // RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
        uint8_t frequency_step = 7;  // Frequency step for CW generator
        DACControlESP32();
        DACControlESP32(uint8_t _clk_8m_div, 
            uint8_t _frequency_step,
            uint8_t _scale,
            uint8_t _offset,
            uint8_t _invert,
            dac_channel_t _channel);

        /**
         * Switch the status of the DAC output channel based on a Channel Flag
         * 0 - Both channel
         * 1 - Right side (GPIO 26)
         * 2 - Left side (GPIO 25)
         * -1 - Disable all channels
         * @param _channel Channel flag based on the configuration values
         */
        void dac_enable(uint8_t _channel);

        /**
         * Creates a wave based on its parameters 
         * This function calculates the DAC values and creates the output
         * 
         * @param frequency Fequency in Hz of the wave
         * @param volume    Volume divisor in bits: 00 (max), 01, 10, 11 (min)
         * @param debug     Print wave information during execution
         */
        void generateWave(uint32_t frequency, uint8_t volume, bool debug);
        
        /**
         * Generates a task with the wave frequency
         * Deprecated
         * 
         * @param frequency Fequency in Hz of the wave
         */
        void generateWaveTask(int desiredFrequency);

        /**
         * Set up the wave directly from the clock and the step
         * This function is faster than the default function
         * It launches the wave directly
         * See the documentation in the beginning of this file for further details
         * Use function calculateStepFrequency to obtain the values
         * 
         * @param _clk_8m_div       Clock divisions necessary to create the wave
         * @param _frequency_step   Step divitions necessary to create the wave
         */
        void generateWaveDirect(uint8_t _clk_8m_div, uint8_t _frequency_step);
        
        /**
         * Function to calculate the Clock and Frequency step division
         * 
         * @param frequency Desired frequency in Hz
         */
        void calculateStepFrequency(uint32_t frequecy);
    private:
        dac_channel_t channel;
        double f_target = 10;
        void dac_cosine_enable();
        void dac_frequency_set();
        void dac_scale_set();
        void dac_offset_set();
        void dac_invert_set();
};

#endif