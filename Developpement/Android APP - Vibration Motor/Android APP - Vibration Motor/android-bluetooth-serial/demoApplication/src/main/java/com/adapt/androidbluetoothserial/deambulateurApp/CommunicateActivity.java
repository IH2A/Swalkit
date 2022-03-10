package com.adapt.androidbluetoothserial.deambulateurApp;

import android.content.DialogInterface;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.MenuItem;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProviders;

import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.CommunicateViewModel;

import java.util.Comparator;

public class CommunicateActivity extends AppCompatActivity {

    // Interfaces to the elements of the UI
    private TextView connectionText, messagesView, profileFileView, profileUserView, profileSurnameView,
                    motor1View, motor2View, motor3View, motor4View,
                    sensor1View, sensor2View, sensor3View, sensor4View, sensor5View, sensor6View,
                    sensor7View, sensor8View, sensor9View, sensor10View, sensor11View, sensor12View;
    private EditText messageBox;
    private Button sendButton, connectButton, profileButton;
    private ImageButton saveButton, deleteButton;
    private Spinner profileListView;
    ArrayAdapter<String> profileListAdapter;

    // Value to check actions that are done only on the startup and can't be on onCreate
    private boolean startup = true;

    // View model instance
    private CommunicateViewModel viewModel;

    // Setup the view when the activity is called
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Setup our activity
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_communicate);
        // Enable the back button in the action bar if possible
        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        }

        // Setup our ViewModel
        viewModel = ViewModelProviders.of(this).get(CommunicateViewModel.class);

        // This method return false if there is an error, so if it does, we should close.
        if (!viewModel.setupViewModel(getIntent().getStringExtra("device_name"), getIntent().getStringExtra("device_mac"))) {
            finish();
            return;
        }

        // Setup our Views
        connectionText = findViewById(R.id.communicate_connection_text);
        messagesView = findViewById(R.id.communicate_messages);
        messageBox = findViewById(R.id.communicate_message);
        sendButton = findViewById(R.id.communicate_send);
        connectButton = findViewById(R.id.communicate_connect);
        saveButton = findViewById(R.id.saveData);
        deleteButton = findViewById(R.id.delete_data);
        profileButton = findViewById(R.id.select_profile);

        profileFileView = findViewById(R.id.profileInfo);
        profileUserView = findViewById(R.id.userInfo);
        profileSurnameView = findViewById(R.id.surnameInfo);
        profileListView = findViewById(R.id.spinner_profiles);

        motor1View = findViewById(R.id.motorInfo1);
        motor2View = findViewById(R.id.motorInfo2);
        motor3View = findViewById(R.id.motorInfo3);
        motor4View = findViewById(R.id.motorInfo4);
        sensor1View = findViewById(R.id.sensorInfo1);
        sensor2View = findViewById(R.id.sensorInfo2);
        sensor3View = findViewById(R.id.sensorInfo3);
        sensor4View = findViewById(R.id.sensorInfo4);
        sensor5View = findViewById(R.id.sensorInfo5);
        sensor6View = findViewById(R.id.sensorInfo6);
        sensor7View = findViewById(R.id.sensorInfo7);
        sensor8View = findViewById(R.id.sensorInfo8);
        sensor9View = findViewById(R.id.sensorInfo9);
        sensor10View = findViewById(R.id.sensorInfo10);
        sensor11View = findViewById(R.id.sensorInfo11);
        sensor12View = findViewById(R.id.sensorInfo12);


        // Start observing the data sent to us by the ViewModel
        viewModel.getConnectionStatus().observe(this, this::onConnectionStatus);
        viewModel.getDeviceName().observe(this, name -> setTitle(getString(R.string.device_name_format, name)));
        viewModel.getMessages().observe(this, message -> {
            if (TextUtils.isEmpty(message)) {
                message = getString(R.string.no_messages);
            }
            messagesView.setText(message);
        });
        viewModel.getMessage().observe(this, message -> {
            // Only update the message if the ViewModel is trying to reset it
            if (TextUtils.isEmpty(message)) {
                messageBox.setText(message);
            }
        });

        viewModel.getFile().observe(this, file -> {
            profileFileView.setText(file);
        });

        viewModel.getUser().observe(this, user -> {
            profileUserView.setText(user);
        });

        viewModel.getSurname().observe(this, surname -> {
            profileSurnameView.setText(surname);
        });

        // Dropdown list sorted in alphabetic order
        viewModel.getListProfiles().observe( this, list-> {
            profileListAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, list);
            profileListAdapter.sort(new Comparator<String>() {
                @Override
                public int compare(String o1, String o2) {
                    return o1.compareTo(o2);
                }
            });
            profileListView.setAdapter(profileListAdapter);
        });

        getMotors();

        getSensors();

        // Setup the send button click action
        sendButton.setOnClickListener(v -> viewModel.sendMessage(messageBox.getText().toString()));

        // Setup profile selection button
        profileButton.setOnClickListener(v -> viewModel.sendMessage("11"+profileListView.getSelectedItem().toString()));

        saveButton.setOnClickListener(v ->
                sendSaveAlert(getResources().getString(R.string.save_file_alert),
                        getResources().getString(R.string.confirm),
                        getResources().getString(R.string.reject)));

        deleteButton.setOnClickListener(v -> sendDeleteAlert(
                getResources().getString(R.string.delete_file_alert),
                getResources().getString(R.string.confirm),
                getResources().getString(R.string.reject)));
    }

    void sendSaveAlert(String message, String positive, String negative) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(message)
                .setPositiveButton(positive, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        onSaveButtonClick();
                    }
                })
                .setNegativeButton(negative, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        // Do nothing
                    }
                });
        // Create the AlertDialog object and return it
        builder.create();
        builder.show();
    }

    private void onSaveButtonClick() {
        viewModel.saveProfile(
                profileUserView.getText().toString(),
                profileSurnameView.getText().toString(),
                motor1View.getText().toString(),
                motor2View.getText().toString(),
                motor3View.getText().toString(),
                motor4View.getText().toString(),
                profileFileView.getText().toString());
        Toast.makeText(this, "File saved!", Toast.LENGTH_SHORT).show();
    }

    void sendDeleteAlert(String message, String positive, String negative) {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(message)
                .setPositiveButton(positive, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        onDeleteButtonClick();
                    }
                })
                .setNegativeButton(negative, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        // Do nothing
                    }
                });
        // Create the AlertDialog object and return it
        builder.create();
        builder.show();
    }

    private void onDeleteButtonClick() {
        viewModel.sendMessage("13" +
                "{\"file\":\"" + profileListView.getSelectedItem().toString() + "\"}");
        Toast.makeText(this, "File deleted!", Toast.LENGTH_SHORT).show();
        viewModel.sendMessage("10");
        profileUserView.setText("-");
        profileSurnameView.setText("-");
        motor1View.setText("-");
        motor2View.setText("-");
        motor3View.setText("-");
        motor4View.setText("-");
        profileFileView.setText("-");
    }

    private void getMotors() {
        viewModel.getMotor1().observe(this, motor -> {
            motor1View.setText(motor.toString());
        });
        viewModel.getMotor2().observe(this, motor -> {
            motor2View.setText(motor.toString());
        });
        viewModel.getMotor3().observe(this, motor -> {
            motor3View.setText(motor.toString());
        });
        viewModel.getMotor4().observe(this, motor -> {
            motor4View.setText(motor.toString());
        });
    }

    private void getSensors() {
        viewModel.getSensor1().observe(this, sensor -> {
            sensor1View.setText(sensor.toString());
        });
        viewModel.getSensor2().observe(this, sensor -> {
            sensor2View.setText(sensor.toString());
        });
        viewModel.getSensor3().observe(this, sensor -> {
            sensor3View.setText(sensor.toString());
        });
        viewModel.getSensor4().observe(this, sensor -> {
            sensor4View.setText(sensor.toString());
        });
        viewModel.getSensor5().observe(this, sensor -> {
            sensor5View.setText(sensor.toString());
        });
        viewModel.getSensor6().observe(this, sensor -> {
            sensor6View.setText(sensor.toString());
        });
        viewModel.getSensor7().observe(this, sensor -> {
            sensor7View.setText(sensor.toString());
        });
        viewModel.getSensor8().observe(this, sensor -> {
            sensor8View.setText(sensor.toString());
        });
        viewModel.getSensor9().observe(this, sensor -> {
            sensor9View.setText(sensor.toString());
        });
        viewModel.getSensor10().observe(this, sensor -> {
            sensor10View.setText(sensor.toString());
        });
        viewModel.getSensor11().observe(this, sensor -> {
            sensor11View.setText(sensor.toString());
        });
        viewModel.getSensor12().observe(this, sensor -> {
            sensor12View.setText(sensor.toString());
        });
    }

    // Called when the ViewModel updates us of our connectivity status
    private void onConnectionStatus(CommunicateViewModel.ConnectionStatus connectionStatus) {
        switch (connectionStatus) {
            case CONNECTED:
                connectionText.setText(R.string.status_connected);
                messageBox.setEnabled(true);
                sendButton.setEnabled(true);
                connectButton.setEnabled(true);
                connectButton.setText(R.string.disconnect);
                connectButton.setOnClickListener(v -> viewModel.disconnect());
                break;

            case CONNECTING:
                connectionText.setText(R.string.status_connecting);
                messageBox.setEnabled(false);
                sendButton.setEnabled(false);
                connectButton.setEnabled(false);
                connectButton.setText(R.string.connect);
                break;

            case DISCONNECTED:
                connectionText.setText(R.string.status_disconnected);
                messageBox.setEnabled(false);
                sendButton.setEnabled(false);
                connectButton.setEnabled(true);
                connectButton.setText(R.string.connect);
                connectButton.setOnClickListener(v -> viewModel.connect());
                if (startup) {
                    viewModel.connect();
                    startup = false;
                }
                break;
        }
    }

    // Called when a button in the action bar is pressed
    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId()) {
            case android.R.id.home:
                // If the back button was pressed, handle it the normal way
                onBackPressed();
                return true;

            default:
                return super.onOptionsItemSelected(item);
        }
    }

    // Called when the user presses the back button
    @Override
    public void onBackPressed() {
        // Close the activity
        finish();
    }
}
