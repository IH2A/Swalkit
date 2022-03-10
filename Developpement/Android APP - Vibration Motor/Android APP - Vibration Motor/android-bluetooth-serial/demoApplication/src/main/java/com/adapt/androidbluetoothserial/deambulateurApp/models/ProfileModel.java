package com.adapt.androidbluetoothserial.deambulateurApp.models;

import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.List;

public class ProfileModel {

    private String fileName;
    private String userName;
    private String userSurname;
    private ArrayList<MotorModel> arrayMotors;
    private HapticMotorModel hapticMotor;
    private ArrayList<SensorModel> arraySensors;
    private Date dateCreatedAt;
    private Date dateUpdatedAt;
    private JSONObject profileJSON;
    private JSONObject compactProfileJSON;

    public ProfileModel() {}


    public ProfileModel(String fileName, String userName, String userSurname, ArrayList<MotorModel> arrayMotors, ArrayList<SensorModel> arraySensors, HapticMotorModel hapticMotor, Date dateCreatedAt, Date dateUpdatedAt) {
        this.fileName = fileName;
        this.userName = userName;
        this.userSurname = userSurname;
        this.arrayMotors = arrayMotors;
        this.arraySensors = arraySensors;
        this.hapticMotor = hapticMotor;
        this.dateCreatedAt = dateCreatedAt;
        this.dateUpdatedAt = dateUpdatedAt;
        this.setProfileAsJSONObject();
    }

    public ProfileModel(boolean isBaseProfileFile, int numberOfMotors, int numberOfSensorsPerSide) {

        ArrayList<SensorModel> arraySensors = new ArrayList<>();
        for (int i = 0; i < numberOfSensorsPerSide; i++) {
            arraySensors.add(new SensorModel(i,true, (30+30*i), "r", -2));
        }

        for (int i = 0; i < numberOfSensorsPerSide; i++) {
            arraySensors.add(new SensorModel(i,true, (30+30*i), "l", 2));
        }
        ArrayList<MotorModel> arrayMotors = new ArrayList<>();
        for (int i = 0; i < numberOfMotors; i++) {
            arrayMotors.add(new MotorModel(i,true,100));
        }

        this.fileName = "Base Profile";
        this.userName = "Username";
        this.userSurname = "Surname";
        this.arrayMotors = arrayMotors;
        this.arraySensors = arraySensors;
        ArrayList<Integer> wave = new ArrayList<>();
        for (int i = 0; i < 4; i++)
        {
            wave.add(65);
        }
        ArrayList<Integer> pulse = new ArrayList<>();
        for (int i = 0; i < 4; i++)
        {
            pulse.add(0);
        }
        ArrayList<Integer> distance = new ArrayList<>();
        for (int i = 0; i < 4; i++)
        {
            distance.add(20+i*10);
        }
        this.hapticMotor = new HapticMotorModel(true, wave, pulse, distance);

        this.dateCreatedAt = new Date(System.currentTimeMillis());
        this.dateUpdatedAt = new Date(System.currentTimeMillis());
        this.setProfileAsJSONObject();
    }

    public ProfileModel(String profileJSONString) {
        try {
            JSONObject profileJSON = new JSONObject(profileJSONString);
            this.fileName = profileJSON.getString("fileName");
            this.userName = profileJSON.getString("userName");
            this.userSurname = profileJSON.getString("userSurname");
            this.hapticMotor = new HapticMotorModel();
            this.arraySensors = new ArrayList<>();


            JSONArray sensorsAux = profileJSON.getJSONArray("sensors");

            for (int i = 0; i < sensorsAux.length(); i++) {
                SensorModel sensorAux = new SensorModel(
                        sensorsAux.getJSONObject(i).getInt("id"),
                        sensorsAux.getJSONObject(i).getBoolean( "enableStatus"),
                        sensorsAux.getJSONObject(i).getInt("relativeAngleDeg"),
                        sensorsAux.getJSONObject(i).getString("location"),
                        sensorsAux.getJSONObject(i).getInt("offset")
                );
                this.arraySensors.add(sensorAux);
            }

            JSONObject hapticMotorsAux = profileJSON.getJSONObject("hapticMotor");

            JSONArray percValue = hapticMotorsAux.getJSONArray("percentageValue");
            ArrayList<Integer> wave = new ArrayList<>();
            for (int i = 0; i < percValue.length(); i++)
            {
                wave.add(percValue.getInt(i));
            }
            this.hapticMotor.setPercentageValue(wave);

            JSONArray pulseValue = hapticMotorsAux.getJSONArray("pulseValue");
            ArrayList<Integer> pulse = new ArrayList<>();
            for (int i = 0; i < pulseValue.length(); i++)
            {
                pulse.add(pulseValue.getInt(i));
            }
            this.hapticMotor.setPulseValue(pulse);

            JSONArray distanceValue = hapticMotorsAux.getJSONArray("distanceValue");
            ArrayList<Integer> distance = new ArrayList<>();
            for (int i = 0; i < distanceValue.length(); i++)
            {
                distance.add(distanceValue.getInt(i));
            }
            this.hapticMotor.setDistanceValue(distance);

            //this.dateCreatedAt = new Date(profileJSON.getString( "dateCreatedAt"));
            this.dateUpdatedAt = new Date(System.currentTimeMillis());

            this.setProfileAsJSONObject();

            Log.i("PROFILE MODEL", "Profile successfully updated");
        } catch (JSONException e) {
            Log.e("PROFILE MODEL FAILED", "Json parsing error: " + e.getMessage());
        }
    }

    public JSONObject setProfileAsJSONObject() {
        JSONObject jsonObject = new JSONObject();
        try {
            jsonObject.put("fileName", this.fileName);
            jsonObject.put("userName", this.userName);
            jsonObject.put("userSurname", this.userSurname);

            JSONArray sensors = new JSONArray();
            for (SensorModel sensor : arraySensors) {
                JSONObject sensorObject = new JSONObject();
                sensorObject.put("id", sensor.getId());
                sensorObject.put("relativeAngleDeg", sensor.getRelativeAngleDeg());
                sensorObject.put("location", sensor.getLocation());
                sensorObject.put("enableStatus", sensor.isEnableStatus());
                sensorObject.put("offset", sensor.getOffset());
                sensors.put(sensorObject);
            }
            jsonObject.put("sensors", sensors);

            JSONObject hapticMotorJson = new JSONObject();
            JSONArray percentageValue = new JSONArray();
            JSONArray pulseValue = new JSONArray();
            JSONArray distanceValue = new JSONArray();

            for (Integer val : hapticMotor.getPercentageValue())
            {
                percentageValue.put(val);
            }
            for (Integer pulse : hapticMotor.getPulseValue())
            {
                pulseValue.put(pulse);
            }
            for (Integer distance : hapticMotor.getDistanceValue())
            {
                distanceValue.put(distance);
            }
            hapticMotorJson.put("percentageValue", percentageValue);
            hapticMotorJson.put("pulseValue", pulseValue);
            hapticMotorJson.put("distanceValue", distanceValue);

            jsonObject.put("hapticMotor", hapticMotorJson);

            jsonObject.put("dateCreatedAt", this.dateCreatedAt);
            jsonObject.put("dateUpdatedAt", this.dateUpdatedAt);
            this.profileJSON = jsonObject;
            Log.i("PROFILE MODEL", "Profile created successfully");
        } catch (Exception ex) {
            Log.e("PROFILE MODEL FAILED", "Cannot create the JSON Object of the profile");
        }
        return jsonObject;
    }

    public JSONObject setProfileAsCompactJSONObject() {
        JSONObject jsonObject = new JSONObject();
        try {
            jsonObject.put("fileName", this.fileName);
            jsonObject.put("userName", this.userName);
            jsonObject.put("userSurname", this.userSurname);


            JSONArray motors = new JSONArray();
            for (int i = 0; i < hapticMotor.getPercentageValue().size(); i++) {
                motors.put(hapticMotor.getPercentageValue().get(i));
            }
            for (int i = 0; i < hapticMotor.getPulseValue().size(); i++) {
                motors.put(hapticMotor.getPulseValue().get(i));
            }
            for (int i = 0; i < hapticMotor.getDistanceValue().size(); i++) {
                motors.put(hapticMotor.getDistanceValue().get(i));
            }

            jsonObject.put("hapticMotor", motors);

            this.compactProfileJSON = jsonObject;
            Log.i("PROFILE MODEL", "Compact profile created successfully\n"+jsonObject.toString());
        } catch (Exception ex) {
            Log.e("PROFILE MODEL FAILED", "Cannot create the JSON Object of the profile");
        }
        return jsonObject;
    }

    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    public String getUserSurname() {
        return userSurname;
    }

    public void setUserSurname(String userSurname) {
        this.userSurname = userSurname;
    }

    public ArrayList<MotorModel> getArrayMotors() {
        return arrayMotors;
    }

    public void setArrayMotors(ArrayList<MotorModel> arrayMotors) {
        this.arrayMotors = arrayMotors;
    }

    public ArrayList<SensorModel> getArraySensors() {
        return arraySensors;
    }

    public void setArraySensors(ArrayList<SensorModel> arraySensors) {
        this.arraySensors = arraySensors;
    }

    public Date getDateCreatedAt() {
        return dateCreatedAt;
    }

    public void setDateCreatedAt(Date dateCreatedAt) {
        this.dateCreatedAt = dateCreatedAt;
    }

    public Date getDateUpdatedAt() {
        return dateUpdatedAt;
    }

    public void setDateUpdatedAt(Date dateUpdatedAt) {
        this.dateUpdatedAt = dateUpdatedAt;
    }

    public HapticMotorModel getHapticMotor() {
        return hapticMotor;
    }

    public void setHapticMotor(HapticMotorModel hapticMotor) {
        this.hapticMotor = hapticMotor;
    }

    public JSONObject getProfileJSON() {
        this.setProfileAsJSONObject();
        return profileJSON;
    }

    public void setProfileJSON(JSONObject profileJSON) {
        this.profileJSON = profileJSON;
    }
}

