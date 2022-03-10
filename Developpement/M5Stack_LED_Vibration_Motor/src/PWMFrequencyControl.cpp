#include "PWMFrequencyControl.h"

Motor::Motor(uint8_t _channel, uint16_t _pin, uint16_t _percentageValue) {
    channel = _channel;
    pin = _pin;
    percentageValue = _percentageValue;
}

MotorCommand::MotorCommand() {
    maxFrequency = 500;
    minFrequency = 10;
    initSinusWave();
    initSquareWave();
    initDoubleFrequencyWave();
}

MotorCommand::MotorCommand(uint16_t maxFreq, uint16_t minFreq) {
    maxFrequency = maxFreq;
    minFrequency = minFreq;
    initSinusWave();
    initSquareWave();
    initDoubleFrequencyWave();
}

uint8_t MotorCommand::addMotor(uint16_t pin) {
    uint8_t channel = checkNextNonDuplicate();
    arrayMotors.push_back(Motor(channel, pin, 0));
    ledcAttachPin(pin, channel);
}

uint8_t MotorCommand::addMotor(uint16_t pin, uint8_t channel) {
    arrayMotors.push_back(Motor(channel, pin, 0));
    ledcAttachPin(pin, channel);
}

uint8_t MotorCommand::checkNumberOfDuplicatesChannel(uint8_t channel) {
    uint8_t duplicates = 0;
    if (!arrayMotors.empty()) {
        for (Motor motor : arrayMotors) {
            if (motor.channel == channel)
                duplicates ++;        
        }
    }
    return duplicates;
}

uint8_t MotorCommand::checkNextNonDuplicate() {
    uint8_t channel = 0;
    while(checkNumberOfDuplicatesChannel(channel) > 0) {
        channel ++;
    }
    return channel;
}

uint16_t MotorCommand::writeFrequency(uint8_t index) {
    if (arrayMotors.size() > index) {
        Motor motor = arrayMotors[index];
        uint16_t realFrequency = motor.percentageValue*0.01*(maxFrequency-minFrequency)+minFrequency;
        ledcWriteTone(motor.channel, realFrequency);
        return realFrequency;
    } else {
        printf("INVALID INDEX ERROR\tCan't find motor data at %d\n", index);
    }
    return 0;
}

void MotorCommand::writeAllFrequencies() {
    for (Motor motor : arrayMotors) {
        //uint16_t realFrequency = motor.percentageValue*(maxFrequency-minFrequency)+minFrequency;
        //ledcWriteTone(motor.channel, realFrequency);
        if(motor.percentageValue != 0) {
            Serial.println("FOI");
            //printf("MOTOR AT CHANNEL 1 ENABLED\tCurrent Frequency %d", motor.channel);
        }
    }
}

TaskFunction_t MotorCommand::taskMotorControl(void * args) {
    while(1) {
        for (Motor motor : arrayMotors) {
            ledcWriteTone(motor.channel, motor.percentageValue);
            
        }
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}

void MotorCommand::setupPWMChannel(uint8_t channel, uint16_t _res, uint16_t _freq) {
    ledcSetup(channel, _freq, _res);
}

void MotorCommand::setupPWMAllChannels(uint16_t _res, uint16_t _freq) {
    for (Motor motor : arrayMotors) {
        ledcSetup(motor.channel, _freq, _res);
    }
}

void MotorCommand::initSinusWave() {
  sinusWaveVector.clear();
  for (int i = 0; i < vectorWavesSize; i ++) {
    unsigned int value = 0.5f * (1.0f + sin(   2.0f * PI * (i/(float)vectorWavesSize) - PI * 0.5f))*vectorWavesSize;
    sinusWaveVector.push_back(value > 0 ? value : 1);
  }
}

void MotorCommand::initSquareWave() {
  sinusWaveVector.clear();
  for (int i = 0; i < vectorWavesSize; i ++) {
    unsigned int value = 0.5f * (1.0f + sin(   2.0f * PI * (i/(float)vectorWavesSize) - PI * 0.5f))*vectorWavesSize;
    sinusWaveVector.push_back(value > 0 ? value : 1);
  }
}

void MotorCommand::initDoubleFrequencyWave() {
  sinusWaveVector.clear();
  for (int i = 0; i < vectorWavesSize; i ++) {
    unsigned int value = 0.5f * (1.0f + 
                         (scaleDoubleWaveRatio-1.0f)/scaleDoubleWaveRatio * sin(2.0f * PI * (i/(float)vectorWavesSize)  - PI * 0.5f) +
                         1.0f/scaleDoubleWaveRatio * sin((2.0f * PI * (i*frequencyDoubleWaveRatio/(float)vectorWavesSize)  - PI * 0.5f)));
    sinusWaveVector.push_back(value > 0 ? value*vectorWavesSize : 1);
  }
}