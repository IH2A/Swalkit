
#ifndef _AtomDisplay_h_
#define _AtomDisplay_h_

#include <Arduino.h>
#include <M5Atom.h>

const uint32_t BLUE = 0x0000ff;
const uint32_t GREEN = 0x00ff00;
const uint32_t RED = 0xff0000;

void numberToLCD(int number, int c){

  M5.dis.clear();
  
  if(number == 0){
      unsigned char AtomDisplay[25] =  {0, 1, 1, 1, 0,
                                        0, 1, 0, 1, 0,
                                        0, 1, 0, 1, 0,
                                        0, 1, 0, 1, 0,
                                        0, 1, 1, 1, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

  if(number == 1){
      unsigned char AtomDisplay[25] =  {0, 0, 0, 1, 0,
                                        0, 0, 1, 1, 0,
                                        0, 1, 0, 1, 0,
                                        0, 0, 0, 1, 0,
                                        0, 0, 0, 1, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

  if(number == 2){
      unsigned char AtomDisplay[25] =  {0, 1, 1, 0, 0,
                                        0, 0, 0, 1, 0,
                                        0, 0, 1, 0, 0,
                                        0, 1, 0, 0, 0,
                                        0, 1, 1, 1, 0 };
  
      M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

if(number == 3){
      unsigned char AtomDisplay[25] =  {0, 1, 1, 1, 0,
                                        0, 0, 0, 1, 0,
                                        0, 0, 1, 0, 0,
                                        0, 0, 0, 1, 0,
                                        0, 1, 1, 0, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

  if(number == 4){
      unsigned char AtomDisplay[25] =  {0, 1, 0, 1, 0,
                                        0, 1, 0, 1, 0,
                                        0, 1, 1, 1, 0,
                                        0, 0, 0, 1, 0,
                                        0, 0, 0, 1, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

  if(number == 5){
      unsigned char AtomDisplay[25] =  {0, 1, 1, 1, 0,
                                        0, 1, 0, 0, 0,
                                        0, 1, 1, 0, 0,
                                        0, 0, 0, 1, 0,
                                        0, 1, 1, 0, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

  if(number == 6){
      unsigned char AtomDisplay[25] =  {0, 0, 1, 1, 0,
                                        0, 1, 0, 0, 0,
                                        0, 1, 1, 0, 0,
                                        0, 1, 0, 1, 0,
                                        0, 1, 1, 0, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

  if(number == 7){
      unsigned char AtomDisplay[25] =  {0, 1, 1, 1, 0,
                                        0, 0, 0, 1, 0,
                                        0, 0, 1, 0, 0,
                                        0, 0, 1, 0, 0,
                                        0, 1, 0, 0, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

  if(number == 8){
      unsigned char AtomDisplay[25] =  {0, 0, 1, 0, 0,
                                        0, 1, 0, 1, 0,
                                        0, 0, 1, 0, 0,
                                        0, 1, 0, 1, 0,
                                        0, 0, 1, 0, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

  if(number == 9){
      unsigned char AtomDisplay[25] =  {0, 0, 1, 0, 0,
                                        0, 1, 0, 1, 0,
                                        0, 0, 1, 9, 0,
                                        0, 0, 0, 1, 0,
                                        0, 0, 1, 0, 0 };
  
    M5.dis.animation((uint8_t *)AtomDisplay, 200, LED_DisPlay::kStatic, 25);

  for(int i=0; i<25; i++){
    if(AtomDisplay[i]){
      M5.dis.drawpix(i, c);
    }
  }
    return;
  }

}

#endif /* _ATOMDISPLAY_H_ */