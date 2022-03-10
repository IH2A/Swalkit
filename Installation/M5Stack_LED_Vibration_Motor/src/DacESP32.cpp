// 50 Hz - 200000Hz:     frequency error ~ 1%
// from 17 Hz - 250000   possible
 
// based on:
// https://github.com/krzychb/dac-cosine
 
// clk_8m_div and frequency_step are computed to give the best result for the wished frequency
 
/* DAC Cosine Generator Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "DacESP32.h"


DACControlESP32::DACControlESP32() {
    clk_8m_div = 1;      // RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
    frequency_step = 7;  // Frequency step for CW generator
    scale = 1;           // 50% of the full scale // scale 0 == fill scale = bad signal (no longer a sine wave)
    offset;              // leave it default / 0 = no any offset
    invert = 2;          // invert MSB to get sine waveform  // 3: invert 180°
    channel = DAC_CHANNEL_2;    // GPIO-25 
}

DACControlESP32::DACControlESP32(uint8_t _clk_8m_div, 
            uint8_t _frequency_step,
            uint8_t _scale,
            uint8_t _offset,
            uint8_t _invert,
            dac_channel_t _channel) {
    clk_8m_div = _clk_8m_div;
    frequency_step = _frequency_step;  
    scale = _scale;          
    offset = _offset;              
    invert = _invert;
    _channel == DAC_CHANNEL_1 || _channel == DAC_CHANNEL_2 ?          
    channel = _channel : channel= _channel, printf("The selected port is not a DAC");    
}

void DACControlESP32::dac_enable(uint8_t _channel) {
    dac_cosine_enable();
    switch (_channel)
    {
    case 1:
        channel = DAC_CHANNEL_1;
        dac_output_enable(DAC_CHANNEL_1);
        dac_output_disable(DAC_CHANNEL_2);
        break;

    case 2:
        channel = DAC_CHANNEL_2;
        dac_output_enable(DAC_CHANNEL_2);
        dac_output_disable(DAC_CHANNEL_1);
        break;

    case 0:
        channel = DAC_CHANNEL_MAX;
        dac_output_enable(DAC_CHANNEL_1);
        dac_output_enable(DAC_CHANNEL_2);
        break;
    
    default:
        dac_output_disable(DAC_CHANNEL_1);
        dac_output_disable(DAC_CHANNEL_2);
    }
}

/*
 * Enable cosine waveform generator on a DAC channel
 */
void DACControlESP32::dac_cosine_enable()
{
    // Enable tone generator common to both channels
    SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_SW_TONE_EN);
    switch(channel) {
        case DAC_CHANNEL_1:
            // Enable / connect tone tone generator on / to this channel
            SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
            // Invert MSB, otherwise part of waveform will have inverted
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV1, 2, SENS_DAC_INV1_S);
            break;
        case DAC_CHANNEL_2:
            SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV2, 2, SENS_DAC_INV2_S);
            break;
        default :
           printf("Channel %d\n", channel);
    }
}
 
 
/*
 * Set frequency of internal CW generator common to both DAC channels
 *
 * clk_8m_div: 0b000 - 0b111
 * frequency_step: range 0x0001 - 0xFFFF
 *
 */
void DACControlESP32::dac_frequency_set()
{
    REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, clk_8m_div);
    SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL1_REG, SENS_SW_FSTEP, frequency_step, SENS_SW_FSTEP_S);
}
 
 
/*
 * Scale output of a DAC channel using two bit pattern:
 *
 * - 00: no scale
 * - 01: scale to 1/2
 * - 10: scale to 1/4
 * - 11: scale to 1/8
 *
 */
void DACControlESP32::dac_scale_set()
{
    switch(channel) {
        case DAC_CHANNEL_1:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE1, scale, SENS_DAC_SCALE1_S);
            break;
        case DAC_CHANNEL_2:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE2, scale, SENS_DAC_SCALE2_S);
            break;
        default :
           printf("Channel %d\n", channel);
    }
}
 
 
/*
 * Offset output of a DAC channel
 *
 * Range 0x00 - 0xFF
 *
 */
void DACControlESP32::dac_offset_set()
{
    switch(channel) {
        case DAC_CHANNEL_1:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC1, offset, SENS_DAC_DC1_S);
            break;
        case DAC_CHANNEL_2:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC2, offset, SENS_DAC_DC2_S);
            break;
        default :
           printf("Channel %d\n", channel);
    }
}
 
 
/*
 * Invert output pattern of a DAC channel
 *
 * - 00: does not invert any bits,
 * - 01: inverts all bits,
 * - 10: inverts MSB,
 * - 11: inverts all bits except for MSB
 *
 */
void DACControlESP32::dac_invert_set()
{
    switch(channel) {
        case DAC_CHANNEL_1:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV1, invert, SENS_DAC_INV1_S);
            break;
        case DAC_CHANNEL_2:
            SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV2, invert, SENS_DAC_INV2_S);
            break;
        default :
           printf("Channel %d\n", channel);
    }
}
 
/*
 * Main task that let you test CW parameters in action
 *
*/
//void dactask(void* arg)
void DACControlESP32::calculateStepFrequency(uint32_t frequecy)
{
double f, delta, delta_min=9999999.0;
int step_target=0, divi_target=0;
 
    // From experiments:  f=125.6*step/(div+1)
    // check all combinations of step iand divi to get the best guess:
    for (int step=1;step<2000; step++) {
      for (int divi=0; divi<8; divi++) {
        f=125.6*(double)step/(double)(divi+1);
        delta= abs((frequecy-f));
        if (delta  < delta_min) {delta_min=delta; step_target=step; divi_target=divi; }
      }
    }
    clk_8m_div=divi_target;
    frequency_step=step_target;
  printf("divi=%d step=%d\n",divi_target, step_target);
}

// Create a cossinus
// The volume is set by a 00 to 11 bit to change the register
// 0 is not a complete wave
void DACControlESP32::generateWave(uint32_t desiredfrequency, uint8_t volume, bool debug) {
    
    calculateStepFrequency(desiredfrequency);
    // frequency setting is common to both channels
    dac_frequency_set();
    /* Tune parameters of channel 2 only
    * to see and compare changes against channel 1
    */
    if (scale != volume) {
        scale = volume;
        dac_scale_set();
        dac_offset_set();
        dac_invert_set();
    }
    if (debug) {
        float frequency = RTC_FAST_CLK_FREQ_APPROX / (1 + clk_8m_div) * (float) frequency_step / 65536;
        printf("THEORIE:   clk_8m_div: %d, frequency step: %d, frequency: %.0f Hz\n", clk_8m_div, frequency_step, frequency);
        printf("PRACTICAL: frequency: %.0f Hz\n", 125.6*(float)frequency_step /(1 + (float)clk_8m_div) );

        printf("DAC2 scale: %d, offset %d, invert: %d\n", scale, offset, invert);

    }
}

void DACControlESP32::generateWaveDirect(uint8_t _clk_8m_div, uint8_t _frequency_step) {
    clk_8m_div = _clk_8m_div;
    frequency_step = _frequency_step;
    dac_frequency_set();
}

// Create a cossinus
// The volume is set by a 00 to 11 bit to change the register
// 0 is not a complete wave
void DACControlESP32::generateWaveTask(int desiredfrequency) {
    while(1) {
        //scale = volume;
        calculateStepFrequency(desiredfrequency);
        // frequency setting is common to both channels
        dac_frequency_set();
    
        /* Tune parameters of channel 2 only
         * to see and compare changes against channel 1
         */
        dac_scale_set();
        dac_offset_set();
        dac_invert_set();
 
        float frequency = RTC_FAST_CLK_FREQ_APPROX / (1 + clk_8m_div) * (float) frequency_step / 65536;
        printf("THEORIE:   clk_8m_div: %d, frequency step: %d, frequency: %.0f Hz\n", clk_8m_div, frequency_step, frequency);
        printf("PRACTICAL: frequency: %.0f Hz\n", 125.6*(float)frequency_step /(1 + (float)clk_8m_div) );

        printf("DAC2 scale: %d, offset %d, invert: %d\n", scale, offset, invert);
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}

/* EXEMPLE

DACControlESP32* dacControl;

void setup()
{
  dacControl = new DACControlESP32();
  dacControl->dac_enable();
}

void loop()
{
  dacControl->generateWave(500, 2000, 1)
} */