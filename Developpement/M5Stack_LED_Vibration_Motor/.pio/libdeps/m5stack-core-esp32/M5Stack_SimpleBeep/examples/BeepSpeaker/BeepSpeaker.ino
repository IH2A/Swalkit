//------------------------------------------------------------
// 2020/08/10 hatatta27
// Simple Beep Library for M5Stack
//
// Sample sketch
// When you press a button, the corresponding beep sounds.
// For example, when ButtonA is pressed, C(262Hz) sounds.
//------------------------------------------------------------
#include <M5Stack.h>
#include "SimpleBeep.h"

void setup(void){

    M5.begin();
    sb.init();
}

void loop(void){

    M5.update();

    if(M5.BtnA.wasPressed()){
        sb.beep(5,C4,100);
    }
    if(M5.BtnB.wasPressed()){
        sb.beep(5,D4,100);
    }
    if(M5.BtnC.wasPressed()){
        sb.beep(5,E4,100);
    }
    delay(100);
}