
// #include "deambulateurPrefs.h"
// #include "Preferences.h"

// SensorConfiguration::SensorConfiguration() {
//                         id = 0;
//                         statusBoolValue = false;
//                         relativeAngleDeg = 0;
//                         location = 'r';
//                         offset = 23.5;
//                       }

// SensorConfiguration::SensorConfiguration(int8_t __id, bool __status,
//                       int16_t __angle, String __location, int8_t __offset) {
//                         id = __id;
//                         statusBoolValue = __status;
//                         relativeAngleDeg = __angle;
//                         location = __location;
//                         offset = __offset;
//                       }

// DynamicJsonDocument SensorConfiguration::getJSONObject() {
//     DynamicJsonDocument jsonSensorDocument(500);
//     jsonSensorDocument["id"] = id;
//     jsonSensorDocument["statusBoolValue"] = statusBoolValue;
//     jsonSensorDocument["irelativeAngleDegd"] = relativeAngleDeg;
//     jsonSensorDocument["location"] = location;
//     jsonSensorDocument["offset"] = offset;
//     return jsonSensorDocument;
// }

// // Generic constructor
// CurrentFile::CurrentFile() {
//     file = "Base Profile";
//     name = "User";
//     surname = "Surname";
// }

// CurrentFile::CurrentFile(String __file, String __name, String __surname) {
//     file = __file;
//     name = __name;
//     surname = __surname;
// }

// // Gets the complete name sotred in the profile
// String CurrentFile::fullName() {
//       surname.toUpperCase();
//       return name + " " + surname;
// }

// // Converts the file information to JSON
// DynamicJsonDocument CurrentFile::getJSONObject() {
//     DynamicJsonDocument jsonCurrentFileDocument(500);
//     jsonCurrentFileDocument["fileName"] = file;
//     jsonCurrentFileDocument["userName"] = name;
//     jsonCurrentFileDocument["userSurname"] = surname;
//     return jsonCurrentFileDocument;
// }

// // Loads the profile class using the selected preferences
// Profile::Profile(Preferences prefs, uint8_t profileNr) {
//     if(profileNr == 1)
//     {    currentFile = new CurrentFile(
//             prefs.getString("fileName"),
//             prefs.getString("userName"),
//             prefs.getString("userSurname")
//         );
//         numberOfSensors = prefs.getInt("numberOfSensors");
//         for (int i = 0; i < numberOfSensors; i++) {
//             sensorObjectVector.push_back(new SensorConfiguration(
//                 prefs.getInt(i + "sensorId"),
//                 prefs.getBool(i + "sensorStatus"),
//                 prefs.getInt(i + "sensorAngle"),
//                 prefs.getString(i + "sensorLocation"),
//                 prefs.getInt(i + "sensorOffset")
//             ));
//         }
//         std::vector<int> parserVector = {
//             prefs.getInt("waveForward"),
//             prefs.getInt("waveDanger"),
//             prefs.getInt("waveNear"),
//             prefs.getInt("waveFar")
//             };
//         leftMotor = new MotorConfiguration();
//         leftMotor->location = 'l';
//         leftMotor->vibrations.clear();
//         leftMotor->pulse.clear();
//         leftMotor->distances.clear();

//         rightMotor = new MotorConfiguration();
//         rightMotor->location = 'r';
//         rightMotor->vibrations.clear();
//         rightMotor->distances.clear();

//         for (int v : parserVector)
//         {
//             leftMotor->vibrations.push_back(v);
//             rightMotor->vibrations.push_back(v);
//         }

//         parserVector = {
//             prefs.getInt("pulseForward"),
//             prefs.getInt("pulseDanger"),
//             prefs.getInt("pulseNear"),
//             prefs.getInt("pulseFar")
//             };
        
//         for (int p : parserVector)
//         {
//             leftMotor->pulse.push_back(p);
//             rightMotor->pulse.push_back(p);
//         }
        
//         parserVector = {
//             prefs.getInt("distanceForward"),
//             prefs.getInt("distanceDanger"),
//             prefs.getInt("distanceNear"),
//             prefs.getInt("distanceFar")
//             };
        
//         for (int d : parserVector)
//         {
//             leftMotor->distances.push_back(d);
//             rightMotor->distances.push_back(d);
//         }
//     }

//     if(profileNr == 2){
//             currentFile = new CurrentFile(
//             prefs.getString("fileName2"),
//             prefs.getString("userName2"),
//             prefs.getString("userSurname2")
//         );
//         numberOfSensors = prefs.getInt("numberOfSensors2");
//         for (int i = 0; i < numberOfSensors; i++) {
//             sensorObjectVector.push_back(new SensorConfiguration(
//                 prefs.getInt(i + "sensorId2"),
//                 prefs.getBool(i + "sensorStatus2"),
//                 prefs.getInt(i + "sensorAngle2"),
//                 prefs.getString(i + "sensorLocation2"),
//                 prefs.getInt(i + "sensorOffset2")
//             ));
//         }
//         std::vector<int> parserVector = {
//             prefs.getInt("waveForward2"),
//             prefs.getInt("waveDanger2"),
//             prefs.getInt("waveNear2"),
//             prefs.getInt("waveFar2")
//             };
//         leftMotor = new MotorConfiguration();
//         leftMotor->location = 'l';
//         leftMotor->vibrations.clear();
//         leftMotor->pulse.clear();
//         leftMotor->distances.clear();

//         rightMotor = new MotorConfiguration();
//         rightMotor->location = 'r';
//         rightMotor->vibrations.clear();
//         rightMotor->distances.clear();

//         for (int v : parserVector)
//         {
//             leftMotor->vibrations.push_back(v);
//             rightMotor->vibrations.push_back(v);
//         }

//         parserVector = {
//             prefs.getInt("pulseForward2"),
//             prefs.getInt("pulseDanger2"),
//             prefs.getInt("pulseNear2"),
//             prefs.getInt("pulseFar2")
//             };
        
//         for (int p : parserVector)
//         {
//             leftMotor->pulse.push_back(p);
//             rightMotor->pulse.push_back(p);
//         }
        
//         parserVector = {
//             prefs.getInt("distanceForward2"),
//             prefs.getInt("distanceDanger2"),
//             prefs.getInt("distanceNear2"),
//             prefs.getInt("distanceFar2")
//             };
        
//         for (int d : parserVector)
//         {
//             leftMotor->distances.push_back(d);
//             rightMotor->distances.push_back(d);
//         }
//     }
// }

// // Iterates all sensors values and creates a JSON that contains
// // Each sensor id value
// //      To simplify the data, if id is -1 is because the sensor is not enabled
// // Each sensor relative angles value
// // General sensor offset 
// DynamicJsonDocument Profile::getSensorsJSONObject() { 
//     DynamicJsonDocument jsonSensorsVector(1500);
//     JsonArray idArray = jsonSensorsVector.createNestedArray("id");
//     JsonArray angleArray = jsonSensorsVector.createNestedArray("angles");
//     for (int i = 0; i < numberOfSensors; i ++) {
//         idArray.add(sensorObjectVector[i]->statusBoolValue ? 
//                     sensorObjectVector[i]->id : -1);
//         angleArray.add(sensorObjectVector[i]->relativeAngleDeg);
//     } 
//     // By default the array is filed with right sensors first and left sensors after
//     // The two desired offsets can be found in the first and last sensors
//     JsonArray offsetArray = jsonSensorsVector.createNestedArray("offset");
//     offsetArray.add(sensorObjectVector[0]->offset);
//     offsetArray.add(sensorObjectVector.back()->offset);
//     return jsonSensorsVector;
// }

// MotorConfiguration::MotorConfiguration() {
//     location = 'r',
//     vibrations = {
//         60, 40, 45, 50
//     };
//     pulse = { 
//         60, 40, 45, 50
//     };
// }

// DynamicJsonDocument Profile::getMotorsJSONObject() { 
//     DynamicJsonDocument jsonVector(1500);
//     JsonArray vibrationsArray = jsonVector.createNestedArray("values");
//     JsonArray pulseArray = jsonVector.createNestedArray("pulses");
//     JsonArray distancesArray = jsonVector.createNestedArray("distances");
//     for (int i = 0; i < leftMotor->vibrations.size(); i++) {
//         vibrationsArray.add(leftMotor->vibrations[i]);
//         pulseArray.add(leftMotor->pulse[i]);
//         distancesArray.add(leftMotor->distances[i]);
//     }
//     /*for (int vibration : leftMotor->vibrations) {
//         vibrationsArray.add(vibration);
//         Serial.println(vibration);
//     } 
//     for (int vibration : rightMotor->vibrations) {
//         vibrationsArray.add(vibration);
//         Serial.println(vibration);
//     } */
    
//     return jsonVector;
// }