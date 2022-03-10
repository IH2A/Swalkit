package com.adapt.androidbluetoothserial.deambulateurApp.models;

import java.util.Comparator;
import java.util.Map;

public class SensorModel implements Comparable<SensorModel>{
    private int id;
    private boolean enableStatus;
    private int relativeAngleDeg;
    private String location;
    private int offset;
    private int currentValue;


    public SensorModel() { }

    public SensorModel(int id, boolean enableStatus, int relativeAngleDeg, String location, int offset) {
        this.id = id;
        this.enableStatus = enableStatus;
        this.relativeAngleDeg = relativeAngleDeg;
        this.location = location;
        this.offset = offset;
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

    public int getRelativeAngleDeg() {
        return relativeAngleDeg;
    }

    public void setRelativeAngleDeg(int relativeAngleDeg) {
        this.relativeAngleDeg = relativeAngleDeg;
    }

    public String getLocation() {
        return location;
    }

    public void setLocation(String location) { this.location = location;}

    public int getOffset() {
        return offset;
    }

    public void setLocation(int offset) { this.offset = offset;}

    public int getCurrentValue() {
        return currentValue;
    }

    public void setCurrentValue(int currentValue) { this.currentValue = currentValue; }

    @Override
    public int compareTo(SensorModel o) {
        return this.getCurrentValue() - o.getCurrentValue();
    }
}
