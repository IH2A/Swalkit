// ********** deambulateurPrefs.h ***********
#ifndef deambulateurPrefs_h
#define deambulateurPrefs_h

#include <Arduino.h>
#include <Preferences.h>
#include <ArduinoJson.h>

class SensorConfiguration {
  public:
    int8_t id;
    bool statusBoolValue;
    int16_t relativeAngleDeg;
    String location;
    int8_t offset;
    SensorConfiguration();
    SensorConfiguration(int8_t __id, bool __status,
                      int16_t __angle, String __location, int8_t __offset);
    DynamicJsonDocument getJSONObject();
};

class MotorConfiguration {
  public:
    char location;
    std::vector<uint16_t> vibrations;
    std::vector<uint16_t> pulse;
    std::vector<uint16_t> distances;

    MotorConfiguration();
    /**
     * Configures a motor element
     * Array of vibrations contains 
     * 0 ~ 2 - Vibrations values to right objects
     * 3 ~ 5 - Vibrations values to left objects
     * 6 ~ 8 - Vibrations values to forward objects
     * @param _vibrations Vibrations values to 
     */
    MotorConfiguration(std::vector<int> _vibrations, std::vector<int> _pulse, std::vector<uint16_t> _distances, char _location);
};

class CurrentFile {
  public:
    String file;
    String name;
    String surname;
    CurrentFile();
    CurrentFile(String _file, String _name, String _surname);
    String fullName(void);
    DynamicJsonDocument getJSONObject();
};

class Profile {
  public:
    CurrentFile* currentFile;
    uint8_t numberOfSensors;
    std::vector<SensorConfiguration*> sensorObjectVector;
    MotorConfiguration* rightMotor;
    MotorConfiguration* leftMotor;
    
    Profile(Preferences prefs, uint8_t profileNr);
    DynamicJsonDocument getSensorsJSONObject();
    DynamicJsonDocument getMotorsJSONObject();
};

#endif