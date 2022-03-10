
#include "SimpleBeep.h"

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
volatile uint8_t amplitude;
volatile uint16_t toggleCount;

SimpleBeep::SimpleBeep(){

    amplitude = 0;
    toggleCount = 0;
}

void IRAM_ATTR beepSpeakerTimer(void){

    portENTER_CRITICAL_ISR(&timerMux);
    toggleCount++;
    if(toggleCount % 2 == 0){
        dacWrite(SPEAKER_PIN, 128 - amplitude);
    }else{
        dacWrite(SPEAKER_PIN, 128 + amplitude);
    }
    portEXIT_CRITICAL_ISR(&timerMux);
}

void SimpleBeep::init(void){

    pinMode(SPEAKER_PIN,OUTPUT);
    dacWrite(SPEAKER_PIN, 0);

    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &beepSpeakerTimer, true);
}

//volume 0-127
//frquency[Hz] 0-65535
//duration[ms] 0-65535
void SimpleBeep::beep(uint8_t vol, uint16_t freq, uint16_t duration){

    toggleCount = 0;
    uint16_t finishCount = (uint16_t)((float)duration*2*0.001*freq);
    amplitude = vol;
    timerAlarmWrite(timer, (uint16_t)((float)1/2/freq*1000000),true);
    timerAlarmEnable(timer);
    while(toggleCount < finishCount);
    timerAlarmDisable(timer);
    for(int8_t i = 127; i >0; i=i-16){
        dacWrite(SPEAKER_PIN, i);
        delay(1);
    }
}

SimpleBeep sb;
