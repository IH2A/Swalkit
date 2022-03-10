package com.adapt.androidbluetoothserial.deambulateurApp.models;

public class MotorModel {

    private int id;
    private boolean enableStatus;
    private int percentageValue;

    public MotorModel() {}

    public MotorModel(int id, boolean enableStatus, int percentageValue) {
        this.id = id;
        this.enableStatus = enableStatus;
        this.percentageValue = percentageValue;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public boolean isEnableStatus() {
        return enableStatus;
    }

    public void setEnableStatus(boolean enableStatus) {
        this.enableStatus = enableStatus;
    }

    public int getPercentageValue() {
        return percentageValue;
    }

    public void setPercentageValue(int percentageValue) {
        this.percentageValue = percentageValue;
    }
}
