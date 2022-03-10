package com.adapt.androidbluetoothserial.deambulateurApp.models;

import java.util.ArrayList;

public class HapticMotorModel {

    //private int id;
    private boolean enableStatus;
    private ArrayList<Integer> percentageValue;
    private ArrayList<Integer> pulseValue;
    private ArrayList<Integer> distanceValue;


    public HapticMotorModel() {}

    //public HapticMotorModel(int id, boolean enableStatus, int percentageValue, int pulseValue) {
    public HapticMotorModel(boolean enableStatus, ArrayList<Integer> percentageValue, ArrayList<Integer> pulseValue, ArrayList<Integer> distanceValue) {

            //this.id = id;
        this.enableStatus = enableStatus;
        this.percentageValue = percentageValue;
        this.pulseValue = pulseValue;
        this.distanceValue = distanceValue;

    }

   // public int getId() {
    //    return id;
  //  }

   // public void setId(int id) {
   //     this.id = id;
   // }

    public boolean isEnableStatus() {
        return enableStatus;
    }

    public void setEnableStatus(boolean enableStatus) {
        this.enableStatus = enableStatus;
    }

    public ArrayList<Integer> getPercentageValue() {
        return percentageValue;
    }

    public void setPercentageValue(ArrayList<Integer>  percentageValue) {
        this.percentageValue = percentageValue;
    }

    public ArrayList<Integer>  getPulseValue() {      return pulseValue;    }

    public void setPulseValue(ArrayList<Integer>  pulseValue) {        this.pulseValue = pulseValue;    }

    public ArrayList<Integer>  getDistanceValue() {      return distanceValue;    }

    public void setDistanceValue(ArrayList<Integer>  distanceValue) {        this.distanceValue = distanceValue;    }
}
