// // ********** PWMFrequencyControl.h ***********
// #ifndef PWMFrequencyControl_h
// #define PWMFrequencyControl_h

// #include <Arduino.h>

// class Motor {
//     public:
//         uint8_t channel;
//         uint16_t percentageValue;
//         uint16_t pin;
//         Motor(uint8_t _channel, uint16_t _pin, uint16_t percentageValue);
// };

// class MotorCommand {
//     public:
//         MotorCommand();
//         MotorCommand(uint16_t maxFreq, uint16_t minFreq);
//         uint8_t addMotor(uint16_t pin);
//         uint8_t addMotor(uint16_t pin, uint8_t channel);
//         std::vector<Motor> arrayMotors;
//         uint8_t checkNumberOfDuplicatesChannel(uint8_t channel);
//         void setupPWMChannel(uint8_t channel, uint16_t _res, uint16_t _freq);
//         void setupPWMAllChannels(uint16_t _res, uint16_t _freq);
//         uint16_t writeFrequency(uint8_t index);
//         void writeAllFrequencies();
//         TaskFunction_t taskMotorControl(void * args);
//     private:
//         uint16_t vectorWavesSize = 255;
//         uint16_t frequencyDoubleWaveRatio = 10;
//         uint16_t scaleDoubleWaveRatio = 9;
//         std::vector<int> sinusWaveVector;
//         std::vector<int> squareWaveVector;
//         std::vector<int> doubleFrequencyWaveVector;
//         std::vector<int> userDefinedWaveVector;
//         void initSinusWave();
//         void initSquareWave();
//         void initDoubleFrequencyWave();
//         uint16_t PWMFrequency;
//         uint16_t resolution;
//         uint16_t maxAvaliableDutyCicle;
//         uint16_t maxFrequency;
//         uint16_t minFrequency;
//         uint8_t checkNextNonDuplicate();
// };

// #endif