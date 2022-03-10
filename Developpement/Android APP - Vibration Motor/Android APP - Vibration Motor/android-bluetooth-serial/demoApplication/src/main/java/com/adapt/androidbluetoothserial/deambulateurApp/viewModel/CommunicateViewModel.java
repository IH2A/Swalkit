package com.adapt.androidbluetoothserial.deambulateurApp.viewModel;

import android.app.Application;
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

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.stream.IntStream;

import io.reactivex.android.schedulers.AndroidSchedulers;
import io.reactivex.disposables.CompositeDisposable;
import io.reactivex.schedulers.Schedulers;

public class CommunicateViewModel extends AndroidViewModel {

    // A CompositeDisposable that keeps track of all of our asynchronous tasks
    private CompositeDisposable compositeDisposable = new CompositeDisposable();

    // Our BluetoothManager!
    private BluetoothManager bluetoothManager;

    // Our Bluetooth Device! When disconnected it is null, so make sure we know that we need to deal with it potentially being null
    @Nullable
    private SimpleBluetoothDeviceInterface deviceInterface;

    // The messages feed that the activity sees
    private MutableLiveData<String> messagesData = new MutableLiveData<>();
    // The connection status that the activity sees
    private MutableLiveData<ConnectionStatus> connectionStatusData = new MutableLiveData<>();
    // The device name that the activity sees
    private MutableLiveData<String> deviceNameData = new MutableLiveData<>();
    // The message in the message box that the activity sees
    private MutableLiveData<String> messageData = new MutableLiveData<>();

    // The profile information that the activity sees
    private MutableLiveData<String> profileFile = new MutableLiveData<>();
    private MutableLiveData<String> profileUser = new MutableLiveData<>();
    private MutableLiveData<String> profileSurname = new MutableLiveData<>();

    private MutableLiveData<Integer> profileMotor1 = new MutableLiveData<>();
    private MutableLiveData<Integer> profileMotor2 = new MutableLiveData<>();
    private MutableLiveData<Integer> profileMotor3 = new MutableLiveData<>();
    private MutableLiveData<Integer> profileMotor4 = new MutableLiveData<>();

    private MutableLiveData<Double> profileSensor1 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor2 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor3 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor4 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor5 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor6 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor7 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor8 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor9 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor10 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor11 = new MutableLiveData<>();
    private MutableLiveData<Double> profileSensor12 = new MutableLiveData<>();

    private MutableLiveData<ArrayList<Double>> profileSensors = new MutableLiveData<>();

    private MutableLiveData<ArrayList<String>> profileList = new MutableLiveData<>();

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
    public CommunicateViewModel(@NotNull Application application) {
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
            sendMessage("10");
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
            case "10":
                serialResponseInfo = message.substring(2, message.length());
                // Read string as a JSON
                try {
                    ArrayList<String> stringsHolder = new ArrayList<>();
                    JSONObject stringObject = new JSONObject(serialResponseInfo);
                    JSONArray arrayObject = stringObject.getJSONArray("data");
                    for (int iterator = 0; iterator < arrayObject.length(); iterator++){
                        message = arrayObject.getString(iterator);
                        stringsHolder.add(arrayObject.getString(iterator));
                        messages.append(deviceName).append(": ").append(message).append('\n');
                        messagesData.postValue(messages.toString());
                    }
                    profileList.setValue(stringsHolder);

                } catch (final JSONException e) {
                    Log.e("FAILED", "Json parsing error: " + e.getMessage());
                }
                break;
            case "11":
                serialResponseInfo = message.substring(2, message.length());
                messages.append(deviceName).append(": ").append(message).append('\n');
                messagesData.postValue(messages.toString());
                try {
                    JSONObject stringObject = new JSONObject(serialResponseInfo);
                    try {
                        profileFile.setValue(stringObject.getString("file"));
                        profileUser.setValue(stringObject.getString("name"));
                        profileSurname.setValue(stringObject.getString("surname"));
                    } catch (final JSONException e) {
                        Log.e("FAILED", "Json parsing error: " + e.getMessage());
                    }
                    try {
                        JSONArray arrayObject = stringObject.getJSONArray("sensors");
                        ArrayList<Double> sensorDoubleArray = new ArrayList<>();
                        IntStream.range(0, 11).forEachOrdered(n -> {
                            try {
                                sensorDoubleArray.add(arrayObject.getDouble(n));
                            } catch (JSONException e) {
                                e.printStackTrace();
                            }
                        });
                        profileSensors.setValue(sensorDoubleArray);
                        profileSensor1.setValue(arrayObject.getDouble(0));
                        profileSensor2.setValue(arrayObject.getDouble(1));
                        profileSensor3.setValue(arrayObject.getDouble(2));
                        profileSensor4.setValue(arrayObject.getDouble(3));
                        profileSensor5.setValue(arrayObject.getDouble(4));
                        profileSensor6.setValue(arrayObject.getDouble(5));
                        profileSensor7.setValue(arrayObject.getDouble(6));
                        profileSensor8.setValue(arrayObject.getDouble(7));
                        profileSensor9.setValue(arrayObject.getDouble(8));
                        profileSensor10.setValue(arrayObject.getDouble(9));
                        profileSensor11.setValue(arrayObject.getDouble(10));
                        profileSensor12.setValue(arrayObject.getDouble(11));
                    } catch (final JSONException e) {
                        Log.e("FAILED", "Json parsing error: " + e.getMessage());
                    }
                    try {
                        JSONArray arrayObjectMotor = stringObject.getJSONArray("motors");
                        profileMotor1.setValue((int) arrayObjectMotor.getDouble(0));
                        profileMotor2.setValue((int) arrayObjectMotor.getDouble(1));
                        profileMotor3.setValue((int) arrayObjectMotor.getDouble(2));
                        profileMotor4.setValue((int) arrayObjectMotor.getDouble(3));
                    } catch (final JSONException e) {
                        Log.e("FAILED", "Json parsing error: " + e.getMessage());
                    }
                } catch (final JSONException e) {
                    Log.e("FAILED", "Json parsing error: " + e.getMessage());
                }
                break;
            default:
                messages.append(deviceName).append(": ").append(message).append('\n');
                messagesData.postValue(messages.toString());
        }
    }

    // Adds a sent message to the conversation
    private void onMessageSent(String message) {
        // Add it to the conversation
        messages.append(getApplication().getString(R.string.you_sent)).append(": ").append(message).append('\n');
        messagesData.postValue(messages.toString());
        // Reset the message box
        messageData.postValue("");
    }

    // Send a message
    public void sendMessage(String message) {
        // Check we have a connected device and the message is not empty, then send the message
        if (deviceInterface != null && !TextUtils.isEmpty(message)) {
            deviceInterface.sendMessage(message);
        }
    }

    // Save profile data
    public void saveProfile(String name, String surname, String motor1, String motor2, String motor3, String motor4, String file) {
        // Check we have a connected device and the message is not empty, then send the message
        try {
            JSONObject objectToSend = new JSONObject();
            objectToSend.put("file", file);
            objectToSend.put("name" , name);
            objectToSend.put("surname", surname);
            JSONArray arrayMotors = new JSONArray();
            // A field may not be filled
            try {
                arrayMotors.put(Double.parseDouble(motor1));
                arrayMotors.put(Double.parseDouble(motor2));
                arrayMotors.put(Double.parseDouble(motor3));
                arrayMotors.put(Double.parseDouble(motor4));
            } catch (final Exception e) {
                Log.e("FAILED", "Motor configuration error: " + e.getMessage());
            }
            objectToSend.put("motors", arrayMotors);
            sendMessage("12"+objectToSend.toString());
        } catch (final JSONException e) {
                Log.e("FAILED", "Json parsing error: " + e.getMessage());
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

    // Getter method for the activity to use.
    public LiveData<String> getMessage() { return messageData; }

    // Send the list of profiles
    public MutableLiveData<ArrayList<String>> getListProfiles() { return profileList; }

    // Getter method for the activity to use.
    public LiveData<String> getFile() { return profileFile; }
    public LiveData<String> getUser() { return profileUser; }
    public LiveData<String> getSurname() { return profileSurname; }

    // Getters to the motors
    public LiveData<Integer> getMotor1() {return profileMotor1; }
    public LiveData<Integer> getMotor2() {return profileMotor2; }
    public LiveData<Integer> getMotor3() {return profileMotor3; }
    public LiveData<Integer> getMotor4() {return profileMotor4; }

    // Getters to the sensors
    public LiveData<Double> getSensor1() {return profileSensor1; }
    public LiveData<Double> getSensor2() {return profileSensor2; }
    public LiveData<Double> getSensor3() {return profileSensor3; }
    public LiveData<Double> getSensor4() {return profileSensor4; }
    public LiveData<Double> getSensor5() {return profileSensor5; }
    public LiveData<Double> getSensor6() {return profileSensor6; }
    public LiveData<Double> getSensor7() {return profileSensor7; }
    public LiveData<Double> getSensor8() {return profileSensor8; }
    public LiveData<Double> getSensor9() {return profileSensor9; }
    public LiveData<Double> getSensor10() {return profileSensor10; }
    public LiveData<Double> getSensor11() {return profileSensor11; }
    public LiveData<Double> getSensor12() {return profileSensor12; }

    public LiveData<ArrayList<Double>> getSensors() { return profileSensors; }

    // An enum that is passed to the activity to indicate the current connection status
    public enum ConnectionStatus {
        DISCONNECTED,
        CONNECTING,
        CONNECTED
    }
}
