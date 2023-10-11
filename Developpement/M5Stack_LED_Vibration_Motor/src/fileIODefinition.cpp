// #include <M5Stack.h>
// #include <Arduino.h>
// #include <ArduinoJson.h>
// #include <string>
// #include "fileIODefinition.h"
// #include <stdio.h>
// #include "BluetoothSerial.h"

// BluetoothSerial SerialBT;
// /*
// Generic function to write files
// Takes file path inside the SD card and the text that'll be into it
// It replaces the whole file
// */
// bool writeFile(fs::FS &fs, const char * path, const char * message) {
//     Serial.printf("Writing file: %s\n", path);
//     M5.Lcd.printf("Writing file: %s\n", path);

//     File file = fs.open(path, FILE_WRITE);
//     if(!file){
//         Serial.println("Failed to open file for writing");
//         M5.Lcd.println("Failed to open file for writing");
//         return false;
//     }
//     if(file.print(message)){
//         Serial.println("File written");
//         M5.Lcd.println("File written");
//     } else {
//         Serial.println("Write failed");
//         M5.Lcd.println("Write failed");
//         return false;
//     }
//     return true;
// }

// /*
// Generic function to read files
// Takes file path inside the SD card 
// Returns the whole file as a String
// */
// String readFile(fs::FS &fs, const char * path) {
//     Serial.printf("Reading file: %s\n", path);
//     //M5.Lcd.printf("Reading file: %s\n", path);
//     String file_information = "";
//     File file = fs.open(path);
//     if(!file){
//         Serial.println("Failed to open file for reading");
//         //M5.Lcd.println("Failed to open file for reading");
//         return file_information;
//     }
//     while(file.available()){
//         int ch = file.read();
//         char int_to_char = ch;
//         file_information = file_information + int_to_char;
//     }
//     return file_information;
// }

// /*
// Delete files from the SD card
// Returns the error function
//     True - Success (200 Ok)
//     False - Cannot locate file (404 File not found)
// */
// bool deleteFile(fs::FS &fs, const char * path) {
//     M5.Lcd.printf("Deleting file: %s\n", path);
//     String file_information = "";
//     File file = fs.open(path);
//     if(!file){
//         M5.Lcd.println("File does not exist");
//         return false;
//     } else {
//         fs.remove(path);
//     }
//     return 1;
// }

// /*
// Debug function to read the files and directories currently in the SD card
// */
// std::vector<String> listDir(fs::FS &fs, const char * dirname, uint8_t levels){

//     std::vector<String> filesInDirectory;
//     // Print blank line on screen
//     M5.Lcd.printf(" \n  ");
//     M5.Lcd.printf(" \n  ");
    
//     Serial.printf("Listing directory: %s\n", dirname);
//     SerialBT.printf("Listing directory: %s\n", dirname);

//     File root = fs.open(dirname);
//     if(!root){
//         Serial.println("Failed to open directory");
//         SerialBT.println("Failed to open directory");
//         return filesInDirectory;
//     }
//     if(!root.isDirectory()){
//         Serial.println("Not a directory");
//         SerialBT.println("Not a directory");
//         return filesInDirectory;
//     }

//     File file = root.openNextFile();
//     while(file){
//         if(file.isDirectory()){
//             Serial.print("  DIR : ");
//             SerialBT.print("  DIR : ");
//             Serial.println(file.name());
//             SerialBT.println(file.name());
//             if(levels){
//                 listDir(fs, file.name(), levels -1);
//             }
//         } else {
//             Serial.print("  FILE: ");
//             SerialBT.print("  FILE: ");
//             Serial.print(file.name());
//             SerialBT.print(file.name());
//             filesInDirectory.push_back(file.name());
//             Serial.print("  SIZE: ");
//             SerialBT.print("  SIZE: ");
//             Serial.println(file.size());
//             SerialBT.println(file.size());
//         }
//         file = root.openNextFile();
//     }
//     return filesInDirectory;
// }

// bool writeToJSON(fs::FS &fs, const char * path, StaticJsonDocument<500> doc) {
//   //doc["sensor"] = "gps";
//   //doc["time"] = 1351824120;
//   //JsonArray data = doc.createNestedArray("data");
//   //data.add(48.756080);
//   //data.add(2.302038);
//   File file = fs.open(path, FILE_WRITE);
//   if(!file){
//         Serial.println("Failed to open file for writing");
//         M5.Lcd.println("Failed to open file for writing");
//         return false;
//     }
//   serializeJsonPretty(doc, file);
//   return true;
// }

// DynamicJsonDocument readJSONFromFile(fs::FS &fs, const char * path, const char * file) {
//   String jsonFile = readFile(SD, path); 
//   DynamicJsonDocument doc(1500);
//   if (!jsonFile) {
//       Serial.println("Invalid file");
//   }
//   // Create the JSON object and convert to a inline information to send to the App
//   DeserializationError error = deserializeJson(doc, jsonFile);
//   doc["file"] = file;
//   if (error) {
//     Serial.println("Error in the deserialization of the JSON");
//   }
//   return doc;
// }

// // Get all the profiles currently in the device
// std::vector<String> listProfiles(fs::FS &fs){
//     std::vector<String> profileFiles;
//     const char * dirname = "/Profiles";
//     File root = fs.open(dirname);
//     if(!root){
//         Serial.println("Failed to read profiles");
//         return profileFiles;
//     }
//     if(!root.isDirectory()){
//         Serial.println("Can't find profiles");
//         return profileFiles;
//     }
//     File file = root.openNextFile();
//     while(file){
//         String fileName = file.name();
//         profileFiles.push_back(fileName.substring(10,fileName.length()));
//         file = root.openNextFile();
//     }
//     return profileFiles;
// }

// // The library doesn't support the transmission of String arrays, so it has to be send like one single line
// String sendListBT(std::vector<String> values, const char * field) {
//     String stringToSend = "";
    
//     DynamicJsonDocument doc(500);
//     JsonArray data = doc.createNestedArray(field);
//     for (String value : values){
//         if (value) {
//             data.add(value.substring(0,value.length()-5));   
//         }
//     }
//     serializeJson(doc, stringToSend);
//     return stringToSend;
// }

// String sendIntListBT(std::vector<int> values, const char * field) {
//     DynamicJsonDocument doc(500);
//     JsonArray data = doc.createNestedArray(field);
//     for (int value : values){
//         //if (value) {
//             data.add(value);   
//         //}
//     }
//     return JSONtoStringBT(doc);
// }

// String JSONtoStringBT(DynamicJsonDocument doc) {
//     if (!doc.isNull()) {
//         String serialInformation;
//         serializeJson(doc, serialInformation);
//         return serialInformation;
//     }
//     else return "";
// }