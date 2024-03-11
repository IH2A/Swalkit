#include "SwalkitDisplay.h"
#include <M5AtomS3.h>

SwalkitDisplay *SwalkitDisplay::pInstance = nullptr;

SwalkitDisplay::SwalkitDisplay() {
    pInstance = this;
    currentBluetoothState = BluetoothState::Disabled;

    // TODO : check colors
    ColorBluetoothStateEnabled = TFT_BLUE;
    ColorBluetoothStateConnected = TFT_SKYBLUE;
    ColorBluetoothStateDisabled = TFT_TRANSPARENT;

    ColorIMUDisabled = TFT_DARKGREY;
    ColorIMUMoving = 0x2589;    // GREEN_SWALKIT
    ColorIMUStopped = 0xe8e4;   // RED_SWALKIT

    ColorStateInitialising = TFT_CYAN;
    ColorStateCalibrating = TFT_VIOLET;
    ColorStateReady = TFT_WHITE;
    ColorStateInLongPress = TFT_ORANGE;
}

void SwalkitDisplay::SetBluetoothState(BluetoothState bluetoothState) {
    switch(bluetoothState) {
        case BluetoothState::Enabled:
            SetBorderColor(ColorBluetoothStateEnabled);
            if (currentBluetoothState == BluetoothState::Disabled) {
                SetQRCode(R"(https://github.com/IH2A/Swalkit/raw/master/Installation/App/app-release.apk)");
            }
            break;
        case BluetoothState::Connected:
            SetBorderColor(ColorBluetoothStateConnected);
            SetQRCode(nullptr);
            break;
        case BluetoothState::Disabled:
            SetBorderColor(ColorBluetoothStateDisabled);
            SetQRCode(nullptr);
            break;
    }
    currentBluetoothState = bluetoothState;
}

void SwalkitDisplay::SetImuState(IMUState imuState) {
    switch(imuState) {
        case IMUState::Disabled:
            SetCenterColor(ColorIMUDisabled);
            break;
        case IMUState::Moving:
            SetCenterColor(ColorIMUMoving);
            break;
        case IMUState::Stopped:
            SetCenterColor(ColorIMUStopped);
            break;
    }
}

void SwalkitDisplay::SetSwalkitState(SwalkitState swalkitState) {
    switch(swalkitState) {
        case SwalkitState::Initialising:
            SetBackgroundColor(ColorStateInitialising);
            break;
        case SwalkitState::Calibrating:
            SetBackgroundColor(ColorStateCalibrating, false);
            SetMessage("Calibration NE PAS BOUGER");
            break;
        case SwalkitState::Ready:
            SetBackgroundColor(ColorStateReady);
            SetMessage(nullptr);
            break;
        case SwalkitState::InLongPress:
            SetBackgroundColor(ColorStateInLongPress);
            break;
    }
}