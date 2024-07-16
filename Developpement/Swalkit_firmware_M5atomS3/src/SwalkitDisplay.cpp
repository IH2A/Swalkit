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

    ColorMotorStopped = TFT_TRANSPARENT;
    ColorMotorRunning = TFT_NAVY;
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
            SetQRCode(String{});
            break;
        case BluetoothState::Disabled:
            SetBorderColor(ColorBluetoothStateDisabled);
            SetQRCode(String{});
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
            SetMessage("Calibra-\ntion\n\nNE PAS BOUGER");
            break;
        case SwalkitState::Ready:
            SetBackgroundColor(ColorStateReady);
            SetMessage(String{});
            break;
        case SwalkitState::InLongPress:
            SetBackgroundColor(ColorStateInLongPress);
            break;
    }
}

void SwalkitDisplay::SetMotorsState(MotorState leftMotorState, MotorState rightMotorState) {
    SetFrontColors(
        (leftMotorState == MotorState::Running) ? ColorMotorRunning : ColorMotorStopped, 
        (rightMotorState == MotorState::Running) ? ColorMotorRunning : ColorMotorStopped
    );
}


void SwalkitDisplay::PressToContinue(String message)
{
    SetMessage(message+" press button");
    do { M5.update(); } while (!M5.Btn.wasReleased());
}