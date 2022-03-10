package com.adapt.androidbluetoothserial.deambulateurApp.viewModel;

import android.app.Application;
import android.hardware.Sensor;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import androidx.annotation.RequiresApi;
import androidx.annotation.StringRes;
import androidx.lifecycle.AndroidViewModel;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;

import com.adapt.androidbluetoothserial.BluetoothManager;
import com.adapt.androidbluetoothserial.SimpleBluetoothDeviceInterface;
import com.adapt.androidbluetoothserial.deambulateurApp.R;
import com.adapt.androidbluetoothserial.deambulateurApp.enums.MessageEnum;
import com.adapt.androidbluetoothserial.deambulateurApp.models.HapticMotorModel;
import com.adapt.androidbluetoothserial.deambulateurApp.models.MotorModel;
import com.adapt.androidbluetoothserial.deambulateurApp.models.ProfileModel;
import com.adapt.androidbluetoothserial.deambulateurApp.models.SensorModel;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.concurrent.ExecutionException;
import java.util.stream.IntStream;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.schedulers.Schedulers;

public class PagerViewModel extends AndroidViewModel {

    // A CompositeDisposable that keeps track of all of our asynchronous tasks
    private final CompositeDisposable compositeDisposable = new CompositeDisposable();

    // Our BluetoothManager!
    private BluetoothManager bluetoothManager;

    // Our Bluetooth Device! When disconnected it is null, so make sure we know that we need to deal with it potentially being null
    @Nullable
    private SimpleBluetoothDeviceInterface deviceInterface;

    // The messages feed that the activity sees
    private final MutableLiveData<String> messagesData = new MutableLiveData<>();
    // The connection status that the activity sees
    private final MutableLiveData<ConnectionStatus> connectionStatusData = new MutableLiveData<>();
    // The device name that the activity sees
    private final MutableLiveData<String> deviceNameData = new MutableLiveData<>();
    // The profile information that the activity sees
    private final MutableLiveData<ProfileModel> profileModelMutableLiveData = new MutableLiveData<>();
    private final MutableLiveData<ArrayList<String>> profileList = new MutableLiveData<>();
    private final MutableLiveData<Boolean> shouldUpdateProfile = new MutableLiveData<>();
    private final MutableLiveData<Boolean> shouldUpdateProfileSensors = new MutableLiveData<>();
    private final MutableLiveData<Boolean> isBootFinished = new MutableLiveData<>();

    // Variable to see if it's the first time booting up
    private final boolean onBoot = true;

    // Our modifiable record of the conversation
    private StringBuilder messages = new StringBuilder();

    // Our configuration
    private String deviceName;
    private String mac;

    // A variable to help us not double-connect
    private boolean connectionAttemptedOrMade = false;
    // A variable to help us not setup twice
    private boolean viewModelSetup = false;

    // Called by the system, this is just a constructor that matches AndroidViewModel.
    public PagerViewModel(@NotNull Application application) {
        super(application);
    }

    // Called in the activity's onCreate(). Checks if it has been called before, and if not, sets up the data.
    // Returns true if everything went okay, or false if there was an error and therefore the activity should finish.
    public boolean setupViewModel(String deviceName, String mac) {
        // Check we haven't already been called
        if (!viewModelSetup) {
            viewModelSetup = true;

            // Setup our BluetoothManager
            bluetoothManager = BluetoothManager.getInstance();
            if (bluetoothManager == null) {
                // Bluetooth unavailable on this device :( tell the user
                toast(R.string.bluetooth_unavailable);
                // Tell the activity there was an error and to close
                return false;
            }

            // Remember the configuration
            this.deviceName = deviceName;
            this.mac = mac;

            // Tell the activity the device name so it can set the title
            deviceNameData.postValue(deviceName);
            // Tell the activity we are disconnected.
            connectionStatusData.postValue(ConnectionStatus.DISCONNECTED);

            // Create the instance of the profile model
            profileModelMutableLiveData.setValue(new ProfileModel());
            shouldUpdateProfileSensors.setValue(false);
            shouldUpdateProfile.setValue(false);
            isBootFinished.setValue(false);
        }
        // If we got this far, nothing went wrong, so return true
        return true;
    }

    // Called when the user presses the connect button
    public void connect() {
        // Check we are not already connecting or connected
        if (!connectionAttemptedOrMade) {
            // Connect asynchronously
            compositeDisposable.add(bluetoothManager.openSerialDevice(mac)
                    .subscribeOn(Schedulers.io())
                    .observeOn(AndroidSchedulers.mainThread())
                    .subscribe(device -> onConnected(device.toSimpleDeviceInterface()), t -> {
                        toast(R.string.connection_failed);
                        connectionAttemptedOrMade = false;
                        connectionStatusData.postValue(ConnectionStatus.DISCONNECTED);
                    }));
            // Remember that we made a connection attempt.
            connectionAttemptedOrMade = true;
            // Tell the activity that we are connecting.
            connectionStatusData.postValue(ConnectionStatus.CONNECTING);
        }
    }

    // Called when the user presses the disconnect button
    public void disconnect() {
        // Check we were connected
        if (connectionAttemptedOrMade && deviceInterface != null) {
            connectionAttemptedOrMade = false;
            // Use the library to close the connection
            bluetoothManager.closeDevice(deviceInterface);
            // Set it to null so no one tries to use it
            deviceInterface = null;
            // Tell the activity we are disconnected
            connectionStatusData.postValue(ConnectionStatus.DISCONNECTED);
        }
    }

    // Called once the library connects a bluetooth device
    private void onConnected(SimpleBluetoothDeviceInterface deviceInterface) {
        this.deviceInterface = deviceInterface;
        if (this.deviceInterface != null) {
            // We have a device! Tell the activity we are connected.
            connectionStatusData.postValue(ConnectionStatus.CONNECTED);
            // Setup the listeners for the interface
            this.deviceInterface.setListeners(this::onMessageReceived, this::onMessageSent, t -> toast(R.string.message_send_error));
            // Tell the user we are connected.
            toast(R.string.connected);
            // Reset the conversation
            messages = new StringBuilder();
            messagesData.postValue(messages.toString());
            //sendMessage("10");
        } else {
            // deviceInterface was null, so the connection failed
            toast(R.string.connection_failed);
            connectionStatusData.postValue(ConnectionStatus.DISCONNECTED);
        }
    }


    // Adds a received message to the conversation
    @RequiresApi(api = Build.VERSION_CODES.N)
    private void onMessageReceived(String message) {
        // Init the serial response
        String serialResponseInfo = "";
        switch (message.substring(0,2)) {
            case "00":
                serialResponseInfo = message.substring(2);
                setupProfileModel(serialResponseInfo);
                break;
            case "01":
                serialResponseInfo = message.substring(2);
                setupSensorsModel(serialResponseInfo);
                break;
            case "02":
                serialResponseInfo = message.substring(2);
                setupArrayMotors(serialResponseInfo);
                break;
            case "11":
                serialResponseInfo = message.substring(2, message.length());
                setProfile(serialResponseInfo);
                break;
            case "20":
                serialResponseInfo = message.substring(2, message.length());
                readSensors(serialResponseInfo);
                break;
            default:
                messages.append(deviceName).append(": ").append(message).append('\n');
                messagesData.postValue(messages.toString());
        }
    }

    private void setupProfileModel(String serialResponseInfo) {
        try {
            // Setup the data for the Profile Fragment
            ProfileModel profileAux = profileModelMutableLiveData.getValue();
            JSONObject JSONResponse = new JSONObject(serialResponseInfo);
            profileAux.setFileName(JSONResponse.getString("fileName"));
            profileAux.setUserName(JSONResponse.getString("userName"));
            profileAux.setUserSurname(JSONResponse.getString("userSurname"));
            profileModelMutableLiveData.setValue(profileAux);
            shouldUpdateProfile.setValue(true);
            sendMessage("01");
            Log.i(MessageEnum.PAGER_VM_LOG_MESSAGE_SUCCESS.getMessage(), "Profile read " + JSONResponse.toString());
        } catch (final JSONException e) {
            Log.e(MessageEnum.PAGER_VM_LOG_MESSAGE_ERROR.getMessage(),
                    MessageEnum.JSON_PARSER_EXCEPTION.getMessage() + e.getMessage());
        }
    }

    private void setupSensorsModel(String serialResponseInfo) {
        try {
            // Setup the data for the Sensors Fragment
            ProfileModel profileAux = profileModelMutableLiveData.getValue();
            JSONObject JSONResponse = new JSONObject(serialResponseInfo);
            JSONArray anglesIdArray = JSONResponse.getJSONArray("id");
            JSONArray anglesValuesArray = JSONResponse.getJSONArray("angles");
            JSONArray offsetArray = JSONResponse.getJSONArray("offset");
            assert profileAux != null;
            ArrayList<SensorModel> sensorsAux = profileAux.getArraySensors();
            for (int i = 0; i < sensorsAux.size(); i ++) {
                sensorsAux.set(i, new SensorModel(
                        anglesIdArray.getInt(i),
                        true,
                        anglesValuesArray.getInt(i),
                        i < sensorsAux.size()/2 ? "r" : "l",
                        i < sensorsAux.size()/2 ? offsetArray.getInt(0) : offsetArray.getInt(1)
                ));
            }
            profileAux.setArraySensors(sensorsAux);
            profileModelMutableLiveData.setValue(profileAux);
            Log.i(MessageEnum.PAGER_VM_LOG_MESSAGE_SUCCESS.getMessage(), "Sensor angles configured: " + JSONResponse.toString());
            sendMessage("02");
        } catch (final JSONException e) {
            Log.e(MessageEnum.PAGER_VM_LOG_MESSAGE_ERROR.getMessage(),
                    MessageEnum.JSON_PARSER_EXCEPTION.getMessage()+ e.getMessage());
        }
    }

    public void setupArrayMotors(String serialResponseInfo) {
        try {
            // Setup the data for the Sensors Fragment
            ProfileModel profileAux = profileModelMutableLiveData.getValue();
            JSONObject JSONResponse = new JSONObject(serialResponseInfo);
            //JSONArray idArray = JSONResponse.getJSONArray("id");
            JSONArray valuesArray = JSONResponse.getJSONArray("values");
            JSONArray pulsesArray = JSONResponse.getJSONArray("pulses");
            assert profileAux != null;

            HapticMotorModel motorAux = profileAux.getHapticMotor();

            ArrayList<Integer> wave = new ArrayList<>();
            for (int i = 0; i < valuesArray.length(); i++)
            {
                wave.add(valuesArray.getInt(i));
            }
            ArrayList<Integer> pulse = new ArrayList<>();
            for (int i = 0; i < 4; i++)
            {
                pulse.add(pulsesArray.getInt(i));
            }
            motorAux.setPercentageValue(wave);
            motorAux.setPulseValue(pulse);

            profileAux.setHapticMotor(motorAux);

            profileModelMutableLiveData.setValue(profileAux);
            Log.i(MessageEnum.PAGER_VM_LOG_MESSAGE_SUCCESS.getMessage(), "Motors values configured: " + JSONResponse.toString());
            shouldUpdateProfileSensors.setValue(true); // Closes the loading fragment
        } catch (final JSONException e) {
            Log.e(MessageEnum.PAGER_VM_LOG_MESSAGE_ERROR.getMessage(),
                    MessageEnum.JSON_PARSER_EXCEPTION.getMessage()+ e.getMessage());
        }
    }

    public void setProfile(String serialResponseInfo) {
        try {
            Log.i("PagerView", "Reading file " + serialResponseInfo);
            ProfileModel profileModelAux = new ProfileModel(serialResponseInfo);
            shouldUpdateProfile.setValue(true);
            shouldUpdateProfileSensors.setValue(false);
            profileModelMutableLiveData.setValue(profileModelAux);
        } catch (final Exception e) {
            Log.e(MessageEnum.PAGER_VM_LOG_MESSAGE_ERROR.getMessage(), "Cannot load profile: " + e.getMessage());
        }
    }

    public void readSensors(String serialResponseInfo) {
        try {
            //Log.d(MessageEnum.PAGER_VM_LOG_MESSAGE_SUCCESS.getMessage(), "Reading :" + serialResponseInfo);
            ProfileModel profileModelAux = profileModelMutableLiveData.getValue();
            JSONObject stringObject = new JSONObject(serialResponseInfo);
            try {
                JSONArray arrayObject = stringObject.getJSONArray("sensors");
                ArrayList<SensorModel> profileSensorAux = new ArrayList<>();
                // Fill up the sensors array
                for (int i = 0; i < profileModelAux.getArraySensors().size(); i++) {
                    SensorModel SensorAux = profileModelAux.getArraySensors().get(i);
                    SensorAux.setCurrentValue(arrayObject.getInt(SensorAux.getId()));
                    profileSensorAux.add(SensorAux);
                }
                Collections.sort(profileSensorAux);
                profileModelAux.setArraySensors(profileSensorAux);
                profileModelMutableLiveData.setValue(profileModelAux);
                //Log.d(MessageEnum.PAGER_VM_LOG_MESSAGE_SUCCESS.getMessage(), "Sensors read");
            } catch (final JSONException e) {
                Log.e(MessageEnum.PAGER_VM_LOG_MESSAGE_ERROR.getMessage(),
                        MessageEnum.JSON_PARSER_EXCEPTION.getMessage()+ e.getMessage());
            } catch (Exception ex) {
                Log.e(MessageEnum.PAGER_VM_LOG_MESSAGE_ERROR.getMessage(), ex.getMessage());
            }
        } catch (final JSONException e) {
            Log.e("FAILED", MessageEnum.JSON_PARSER_EXCEPTION.getMessage()+ e.getMessage());
        }
    }

    public void setupProfileList(ArrayList<String> files) {
        try {
            ArrayList<String> stringsHolder = new ArrayList<>();
            for (String file : files) {
                if (!file.isEmpty() && file != null && file.endsWith(".json")) {
                    stringsHolder.add(file.replace(".json",""));
                }
            }
            profileList.setValue(stringsHolder);
        } catch (Exception e) {
            Log.e(MessageEnum.PAGER_VM_LOG_MESSAGE_ERROR.getMessage(), "Cannot append local profiles to list\t" + e.getMessage());
        }
    }

    // Adds a sent message to the conversation
    private void onMessageSent(String message) {
        // Add it to the conversation
        Log.i(MessageEnum.PAGER_VM_LOG_MESSAGE_SUCCESS.getMessage(), "Send\t" + message);
    }

    // Send a message
    public void sendMessage(String message) {
        // Check we have a connected device and the message is not empty, then send the message
        if (deviceInterface != null && !TextUtils.isEmpty(message)) {
            deviceInterface.sendMessage(message);
        }
    }

    // Called when the activity finishes - clear up after ourselves.
    @Override
    protected void onCleared() {
        // Dispose any asynchronous operations that are running
        compositeDisposable.dispose();
        // Shutdown bluetooth connections
        bluetoothManager.close();
    }

    // Helper method to create toast messages.
    private void toast(@StringRes int messageResource) { Toast.makeText(getApplication(), messageResource, Toast.LENGTH_LONG).show(); }

    // Getter method for the activity to use.
    public LiveData<String> getMessages() { return messagesData; }

    // Getter method for the activity to use.
    public LiveData<ConnectionStatus> getConnectionStatus() { return connectionStatusData; }

    // Getter method for the activity to use.
    public LiveData<String> getDeviceName() { return deviceNameData; }

    // Send the list of profiles
    public MutableLiveData<ArrayList<String>> getListProfiles() { return profileList; }
    public MutableLiveData<Boolean> getShouldUpdateProfile() { return shouldUpdateProfile; }
    public MutableLiveData<Boolean> getShouldUpdateProfileSensors() { return shouldUpdateProfileSensors; }
    public MutableLiveData<Boolean> getIsBootFinished() { return isBootFinished; }
    public void setShouldUpdateProfile(boolean status) { shouldUpdateProfile.setValue(status);}
    public void setShouldUpdateProfileSensors(boolean status) { shouldUpdateProfileSensors.setValue(status);}
    public void setIsBootFinished(boolean status) { isBootFinished.setValue(status);}

    public LiveData<ProfileModel> getProfileModel() {
        //profileModelMutableLiveData.setValue(profileModel);
        return profileModelMutableLiveData;
    }

    public void setProfileModel(ProfileModel profileModel) {profileModelMutableLiveData.setValue(profileModel); }

    // An enum that is passed to the activity to indicate the current connection status
    public enum ConnectionStatus {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    }
}
