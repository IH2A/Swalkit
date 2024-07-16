#ifndef _SWALKIT_DISPLAY_H
#define _SWALKIT_DISPLAY_H

#include "BasicDisplay.h"

class SwalkitDisplay : public BasicDisplay {
public:
    enum class BluetoothState {
        Enabled,
        Connected,
        Disabled,
    };
    enum class IMUState {
        Disabled,
        Moving,
        Stopped
    };
    enum class SwalkitState {
        Initialising,
        Calibrating,
        Ready,
        InLongPress,
    };
    enum class MotorState {
        Stopped,
        Running,
    };

    SwalkitDisplay();

    static SwalkitDisplay& GetInstance() { return *pInstance; }

    void SetBluetoothState(BluetoothState bluetoothState);
    void SetImuState(IMUState imuState);
    void SetSwalkitState(SwalkitState swalkitState);
    void SetMotorsState(MotorState leftMotorState, MotorState rightMotorState);
    void PressToContinue(String message);

private:
    static SwalkitDisplay *pInstance;

    BluetoothState currentBluetoothState;

    uint32_t ColorBluetoothStateEnabled;
    uint32_t ColorBluetoothStateConnected;
    uint32_t ColorBluetoothStateDisabled;

    uint32_t ColorIMUDisabled;
    uint32_t ColorIMUMoving;
    uint32_t ColorIMUStopped;

    uint32_t ColorStateInitialising;
    uint32_t ColorStateCalibrating;
    uint32_t ColorStateReady;
    uint32_t ColorStateInLongPress;

    uint32_t ColorMotorStopped;
    uint32_t ColorMotorRunning;
};

#endif