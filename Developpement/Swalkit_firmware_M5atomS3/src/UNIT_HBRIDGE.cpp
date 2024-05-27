#include "UNIT_HBRIDGE.h"

uint8_t UNIT_HBRIDGE::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                              uint8_t length) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    for (int i = 0; i < length; i++) {
        _wire->write(*(buffer + i));
    }
    return _wire->endTransmission();
}

void UNIT_HBRIDGE::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                             uint8_t length) {
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    _wire->endTransmission();
    _wire->requestFrom(addr, length);
    for (int i = 0; i < length; i++) {
        buffer[index++] = _wire->read();
    }
}

bool UNIT_HBRIDGE::begin(TwoWire *wire, uint8_t addr, uint8_t sda, uint8_t scl,
                         uint32_t speed) {
    _wire  = wire;
    _addr  = addr;
    _sda   = sda;
    _scl   = scl;
    _speed = speed;
    _wire->begin(_sda, _scl, _speed);
    delay(10);
    _wire->beginTransmission(_addr);
    uint8_t error = _wire->endTransmission();
    if (error == 0) {
        return true;
    } else {
        return false;
    }
}

uint8_t UNIT_HBRIDGE::getDriverDirection(void) {
    uint8_t data[4];
    readBytes(_addr, DRIVER_CONFIG_REG, data, 1);
    return data[0];
}

uint8_t UNIT_HBRIDGE::getDriverSpeed8Bits(void) {
    uint8_t data[4];
    readBytes(_addr, DRIVER_CONFIG_REG + 1, data, 1);
    return data[0];
}

uint16_t UNIT_HBRIDGE::getDriverSpeed16Bits(void) {
    uint8_t data[4];
    readBytes(_addr, DRIVER_CONFIG_REG + 2, data, 2);
    return (data[0] | (data[1] << 8));
}

uint16_t UNIT_HBRIDGE::getDriverPWMFreq(void) {
    uint8_t data[4];
    readBytes(_addr, DRIVER_CONFIG_REG + 4, data, 2);
    return (data[0] | (data[1] << 8));
}

void UNIT_HBRIDGE::setDriverPWMFreq(uint16_t freq) {
    uint8_t data[4];
    data[0] = (freq & 0xff);
    data[1] = ((freq >> 8) & 0xff);
    writeBytes(_addr, DRIVER_CONFIG_REG + 4, data, 2);
}

void UNIT_HBRIDGE::setDriverDirection(uint8_t dir) {
    uint8_t data[4];
    data[0] = dir;
    writeBytes(_addr, DRIVER_CONFIG_REG, data, 1);
}

void UNIT_HBRIDGE::setDriverSpeed8Bits(uint8_t speed) {
    uint8_t data[4];
    data[0] = speed;
    writeBytes(_addr, DRIVER_CONFIG_REG + 1, data, 1);
}

uint8_t UNIT_HBRIDGE::setDriverSpeed16Bits(uint16_t speed) {
    uint8_t data[4];
    data[0] = speed;
    data[1] = (speed >> 8);
    return writeBytes(_addr, DRIVER_CONFIG_REG + 2, data, 2);
}

uint16_t UNIT_HBRIDGE::getAnalogInput(hbridge_anolog_read_mode_t bit) {
    if (bit == _8bit) {
        uint8_t data;
        readBytes(_addr, MOTOR_ADC_8BIT_REG, &data, 1);
        return data;
    } else {
        uint8_t data[2];
        readBytes(_addr, MOTOR_ADC_12BIT_REG, data, 2);
        return (data[0] | (data[1] << 8));
    }
}

// Only V1.1 can use this
float UNIT_HBRIDGE::getMotorCurrent(void) {
    uint8_t data[4];
    float c;
    uint8_t *p;

    readBytes(_addr, MOTOR_CURRENT_REG, data, 4);
    p = (uint8_t *)&c;
    memcpy(p, data, 4);

    return c;
}

uint8_t UNIT_HBRIDGE::getFirmwareVersion(void) {
    uint8_t data[4];
    readBytes(_addr, HBRIDGE_FW_VERSION_REG, data, 1);
    return data[0];
}

uint8_t UNIT_HBRIDGE::getI2CAddress(void) {
    uint8_t data[4];
    readBytes(_addr, HBRIDGE_I2C_ADDRESS_REG, data, 1);
    return data[0];
}

// Only V1.1 can use this
void UNIT_HBRIDGE::jumpBootloader(void) {
    uint8_t value = 1;
    writeBytes(_addr, JUMP_TO_BOOTLOADER_REG, (uint8_t *)&value, 1);
    _wire->end();
    pinMode(_sda, OUTPUT);
    pinMode(_scl, OUTPUT);
    digitalWrite(_sda, LOW);
    digitalWrite(_scl, LOW);
    delay(2000);
    _wire->begin(_sda, _scl, _speed);
    delay(10);
}

void UNIT_HBRIDGE::attemptRecoveryBootloader() {
    uint8_t value = 1;
    _wire->end();
    pinMode(_sda, OUTPUT);
    pinMode(_scl, OUTPUT);
    digitalWrite(_sda, LOW);
    digitalWrite(_scl, LOW);
}


bool UNIT_HBRIDGE::performRecovery(const uint8_t *fw, uint32_t length, uint32_t startAddr) {
    delay(2000);
    _wire->begin(_sda, _scl, 100000L);
    return updateFW(fw, length, startAddr);
}



void UNIT_HBRIDGE::startApp(void){
    writeBytes(0x54, 0x77, NULL, 0);
    delay(100);
}

bool UNIT_HBRIDGE::updateFW(const uint8_t *fw, uint32_t length, uint32_t startAddr){
    _wire->setBufferSize(1050);
    uint32_t maxPage = 1024;
    int nbPage = length / maxPage;
    if(length % maxPage != 0) nbPage++;
    uint32_t addr = startAddr;
    uint16_t sizeLeft = length;
    for(int j = 0 ; j < nbPage ; j++){
        int tryCnt = 0;
        uint8_t error = 0;
        uint16_t pageSize = maxPage;
        do{
            _wire->beginTransmission(0x54);
            _wire->write(0x06);
            _wire->write((addr >> 24) & 0xFF);
            _wire->write((addr >> 16) & 0xFF);
            _wire->write((addr >> 8) & 0xFF);
            _wire->write(addr & 0xFF);

            
            if(sizeLeft < maxPage){
                pageSize = sizeLeft;
            }

            _wire->write((pageSize >> 8) & 0xFF);
            _wire->write(pageSize & 0xFF); 
            _wire->write(0); 

            uint16_t sizeWritten = 0;      

            for (int i = 0; i < pageSize; i++) {
                sizeWritten += _wire->write(*(fw + i + 1024 * j));
            }
            error = _wire->endTransmission();
            tryCnt ++;
        }while(error && tryCnt < 3);

        if(error) return false;

        delay(300);
        addr += pageSize;
    }
    return true;
}
