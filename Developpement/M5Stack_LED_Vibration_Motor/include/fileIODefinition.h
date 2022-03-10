// ********** fileIODefinition.h ***********
#ifndef fileIODefinition_h
#define fileIODefinition_h

#include <ArduinoJson.h>
#include "BluetoothSerial.h"

extern BluetoothSerial SerialBT;
// ********** IO header file ***********
// Creates the IO interface to the Serial bluetooth data
// IO Interface to the MicroSD 
//  - Generic files
//  - Json files

// File I/O into the MicroSD
// Create a new file into the microSD in the desired path
bool writeFile(fs::FS &fs, const char * path, const char * message);
// Read file from bath and get the information as a String
String readFile(fs::FS &fs, const char * path);
// Delete file from the desired directory 
bool deleteFile(fs::FS &fs, const char * path);

// Get MicroSD current file information and displays on the screen
std::vector<String> listDir(fs::FS &fs, const char * dirname, uint8_t levels);

// JSON operations
// Write to a JSON file
bool writeToJSON(fs::FS &fs, const char * path, StaticJsonDocument<500> doc);
// Get the JSON object that's on a file
DynamicJsonDocument readJSONFromFile(fs::FS &fs, const char * path, const char * file);

// Program I/O files for the app manipulation
// Get all files from the profile director as a iterable vector
std::vector<String> listProfiles(fs::FS &fs);
// Creates a String to send via bluetooth, values in a JSON "data" field
String sendListBT(std::vector<String> values, const char * field);
String sendIntListBT(std::vector<int> values, const char * field);
String JSONtoStringBT(DynamicJsonDocument doc);

#endif