#ifndef BEEPSPEAKER_H
#define BEEPSPEAKER_H

#include "arduino.h"

#define SPEAKER_PIN 25

#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523

class SimpleBeep {

    public:
        SimpleBeep();
        void init();
        void beep(uint8_t vol, uint16_t freq, uint16_t duration);

};

extern SimpleBeep sb;
#define SB sb;

#endif