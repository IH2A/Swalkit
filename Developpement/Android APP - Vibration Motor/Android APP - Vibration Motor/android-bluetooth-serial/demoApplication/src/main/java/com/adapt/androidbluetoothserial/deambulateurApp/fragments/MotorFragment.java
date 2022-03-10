package com.adapt.androidbluetoothserial.deambulateurApp.fragments;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.ViewModelProvider;

import com.adapt.androidbluetoothserial.deambulateurApp.MainActivity;
import com.adapt.androidbluetoothserial.deambulateurApp.enums.MessageEnum;
import com.adapt.androidbluetoothserial.deambulateurApp.models.HapticMotorModel;
import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.PagerViewModel;
import com.adapt.androidbluetoothserial.deambulateurApp.R;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.android.material.slider.Slider;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;

public class MotorFragment extends Fragment {

    // View model instance
    private PagerViewModel viewModel;
    private FragmentActivity fragmentActivity;
    private ArrayList<Button> arrayBtnMotors = new ArrayList<>();
    private ArrayList<Button> arrayBtnPulse = new ArrayList<>();
    private ArrayList<Button> arrayBtnDistance = new ArrayList<>();
    private ArrayList<TextView> arrayTxtMotors = new ArrayList<>();
    private ArrayList<TextView> arrayTxtPulse = new ArrayList<>();
    private ArrayList<TextView> arrayTxtDistance = new ArrayList<>();

    private ArrayList<String> waveList = new ArrayList<>();
    private ArrayList<Integer> waveValuesList = new ArrayList<>();
    private ArrayList<String> distanceList = new ArrayList<>();
    private ArrayList<Integer> distanceValuesList = new ArrayList<>();
    private ArrayList<String> pulseList = new ArrayList<>();
    private ArrayList<Integer> pulseValuesList = new ArrayList<>();

    private ArrayList<Integer> currentWaveValueConfiguration = new ArrayList<>();
    private ArrayList<Integer> currentPulseValueConfiguration = new ArrayList<>();
    private ArrayList<Integer> currentDistanceValueConfiguration = new ArrayList<>();

    @NonNull
    public static Fragment newInstance() {
        return new MotorFragment();
    }

    public MotorFragment() {super(R.layout.fragment_motor2);}

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_motor2, container, false);

        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_1_1));
        arrayTxtMotors.add(view.findViewById(R.id.txt_motor_1_1));
        arrayBtnPulse.add(view.findViewById(R.id.btn_motor_1_2));
        arrayTxtPulse.add(view.findViewById(R.id.txt_motor_1_2));
        arrayBtnDistance.add(view.findViewById(R.id.btn_motor_1_3));
        arrayTxtDistance.add(view.findViewById(R.id.txt_motor_1_3));

        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_2_1));
        arrayTxtMotors.add(view.findViewById(R.id.txt_motor_2_1));
        arrayBtnPulse.add(view.findViewById(R.id.btn_motor_2_2));
        arrayTxtPulse.add(view.findViewById(R.id.txt_motor_2_2));
        arrayBtnDistance.add(view.findViewById(R.id.btn_motor_2_3));
        arrayTxtDistance.add(view.findViewById(R.id.txt_motor_2_3));

        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_3_1));
        arrayTxtMotors.add(view.findViewById(R.id.txt_motor_3_1));
        arrayBtnPulse.add(view.findViewById(R.id.btn_motor_3_2));
        arrayTxtPulse.add(view.findViewById(R.id.txt_motor_3_2));
        arrayBtnDistance.add(view.findViewById(R.id.btn_motor_3_3));
        arrayTxtDistance.add(view.findViewById(R.id.txt_motor_3_3));

        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_4_1));
        arrayTxtMotors.add(view.findViewById(R.id.txt_motor_4_1));
        arrayBtnPulse.add(view.findViewById(R.id.btn_motor_4_2));
        arrayTxtPulse.add(view.findViewById(R.id.txt_motor_4_2));
        arrayBtnDistance.add(view.findViewById(R.id.btn_motor_4_3));
        arrayTxtDistance.add(view.findViewById(R.id.txt_motor_4_3));

        List<String> auxiliarStringList = Arrays.asList(
                "Off",
                "30Hz",
                "35Hz",
                "40Hz",
                "45Hz",
                "50Hz",
                "55Hz",
                "60Hz",
                "65Hz",
                "70Hz",
                "75Hz",
                "80Hz",
                "85Hz",
                "90Hz",
                "95Hz"
        );


        List<Integer> auxiliarList = Arrays.asList(
                0,
                30,
                35,
                40,
                45,
                50,
                55,
                55,
                60,
                65,
                70,
                75,
                80,
                85,
                90,
                95
        );

        List<String> auxiliarPulseStringList = Arrays.asList(
                "Off",
                "100ms",
                "200ms",
                "300ms",
                "400ms",
                "500ms"
        );


        List<Integer> auxiliarPulseList = Arrays.asList(
                0,
                100,
                200,
                300,
                400,
                500
        );

        List<String> auxiliarDistanceStringList = Arrays.asList(
                "Off",
                "5cm",
                "10cm",
                "15cm",
                "20cm",
                "25cm",
                "30cm",
                "35cm",
                "40cm",
                "45cm",
                "50cm",
                "55cm",
                "60cm",
                "65cm",
                "70cm",
                "75cm",
                "80cm",
                "85cm",
                "90cm",
                "95cm",
                "100cm",
                "105cm",
                "110cm",
                "115cm"
        );


        List<Integer> auxiliarDistanceList = Arrays.asList(
                0,
                5,
                10,
                15,
                20,
                25,
                30,
                35,
                40,
                45,
                50,
                55,
                60,
                65,
                70,
                75,
                80,
                85,
                90,
                95,
                100,
                105,
                110,
                115
        );

        waveList.addAll(auxiliarStringList);
        pulseList.addAll(auxiliarPulseStringList);
        distanceList.addAll(auxiliarDistanceStringList);
        waveValuesList.addAll(auxiliarList);
        pulseValuesList.addAll(auxiliarPulseList);
        distanceValuesList.addAll(auxiliarDistanceList);

        for (int i = 0; i < arrayBtnMotors.size(); i ++) {
            int finalI = i;
            arrayBtnMotors.get(i).setOnClickListener(v -> showDialogSetMotor(finalI));
        }
        for (int i = 0; i < arrayBtnPulse.size(); i ++) {
            int finalI = i;
            arrayBtnPulse.get(i).setOnClickListener(v -> showDialogSetPulse(finalI));
        }
        for (int i = 0; i < arrayBtnDistance.size(); i ++) {
            int finalI = i;
            arrayBtnDistance.get(i).setOnClickListener(v -> showDialogSetDistance(finalI));
        }
        return view;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        //viewModel  = ViewModelProviders.of(this).get(ParserViewModel.class);
        fragmentActivity = getActivity();
        viewModel = new ViewModelProvider(fragmentActivity).get(PagerViewModel.class);

        HapticMotorModel hapticMotor = viewModel.getProfileModel().getValue().getHapticMotor();

        for(int val : hapticMotor.getPercentageValue())
        {
            currentWaveValueConfiguration.add(val);
        }

        for(int val : hapticMotor.getPulseValue())
        {
            currentPulseValueConfiguration.add(val);
        }

        for(int val : hapticMotor.getDistanceValue())
        {
            currentDistanceValueConfiguration.add(val);
        }

        for (int i = 0; i < currentWaveValueConfiguration.size(); i ++) {
            int index = waveValuesList.indexOf(currentWaveValueConfiguration.get(i));
            if (index != -1) {
                arrayBtnMotors.get(i).setText(waveList.get(index));
            }
        }

        for (int i = 0; i < currentPulseValueConfiguration.size(); i ++) {
            int index = pulseValuesList.indexOf(currentPulseValueConfiguration.get(i));
            if (index != -1)
            {
                arrayBtnPulse.get(i).setText(pulseList.get(index));
            }

        }

        for (int i = 0; i < currentDistanceValueConfiguration.size(); i ++) {
            int index = distanceValuesList.indexOf(currentDistanceValueConfiguration.get(i));
            if (index != -1) {
                arrayBtnDistance.get(i).setText(distanceList.get(index));
            }        }

        viewModel.getProfileModel().observe(fragmentActivity, profileModel -> {
            HapticMotorModel currentHm = profileModel.getHapticMotor();
            ArrayList<Integer> currentWave = new ArrayList<>();
            ArrayList<Integer> currentPulse = new ArrayList<>();
            ArrayList<Integer> currentDistance = new ArrayList<>();

            for(int val : currentHm.getPercentageValue())
            {
                currentWave.add(val);
            }

            for(int val : currentHm.getPulseValue())
            {
                currentPulse.add(val);
            }

            for(int val : currentHm.getDistanceValue())
            {
                currentDistance.add(val);
            }

            if (!currentWaveValueConfiguration.equals(currentWave)) {
                Log.i("MOTOR_FRAGMENT_DEBUG", "NO UPDATE OF WAVE");
                currentWaveValueConfiguration = currentWave;
                for (int i = 0; i < this.currentWaveValueConfiguration.size(); i ++) {
                    int index = waveValuesList.indexOf(currentWaveValueConfiguration.get(i));
                    if (index != -1)
                    {
                        arrayBtnMotors.get(i).setText(waveList.get(index));

                    }
                }
            }
            if (!currentPulseValueConfiguration.equals(currentPulse)) {
                Log.i("MOTOR_FRAGMENT_DEBUG", "NO UPDATE OF PULSE");
                currentPulseValueConfiguration = currentPulse;
                for (int i = 0; i < this.currentPulseValueConfiguration.size(); i ++) {
                    int index = pulseValuesList.indexOf(currentPulseValueConfiguration.get(i));
                    if (index != -1) {
                        //arrayTxtPulse.get(i).setText(pulseList.get(index));
                        arrayBtnPulse.get(i).setText(pulseList.get(index));

                    }                }
            }
            if (!currentDistanceValueConfiguration.equals(currentDistance)) {
                Log.i("MOTOR_FRAGMENT_DEBUG", "NO UPDATE OF DISTANCE");
                currentDistanceValueConfiguration = currentPulse;
                for (int i = 0; i < this.currentDistanceValueConfiguration.size(); i ++) {
                    int index = distanceValuesList.indexOf(currentDistanceValueConfiguration.get(i));
                    if (index != -1) {
                       // arrayTxtDistance.get(i).setText(distanceList.get(index));
                        arrayBtnDistance.get(i).setText(distanceList.get(index));

                    }                }
            }
        });

    }

    // Setup the dialog box to configure the motor
    void showDialogSetMotor(int finalI) {
        final int[] itemSelected = {1};
        // Convert the Wave List to a static array
        String[] optionsItems = waveList.toArray(new String[0]);

        // Setup default positive and negative buttons
        MaterialAlertDialogBuilder dialog = new MaterialAlertDialogBuilder(fragmentActivity)
                .setTitle(getResources().getString(R.string.options_scroll))
                .setSingleChoiceItems(optionsItems, itemSelected[0], new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int selectedIndex) {
                        itemSelected[0] = selectedIndex;
                    }
                })
                .setPositiveButton(getResources().getString(R.string.select_profile), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.i(MessageEnum.MOTOR_FRAGMENT_DEBUG.getMessage(), String.valueOf(itemSelected[0]));
                        //arrayTxtMotors.get(finalI).setText(waveList.get(itemSelected[0]));
                        arrayBtnMotors.get(finalI).setText(waveList.get(itemSelected[0]));

                        currentWaveValueConfiguration.set(finalI, Integer.valueOf(waveValuesList.get(itemSelected[0])));

                        HapticMotorModel hm = new HapticMotorModel();
                        hm.setPercentageValue(currentWaveValueConfiguration);
                        hm.setPulseValue(currentPulseValueConfiguration);
                        hm.setDistanceValue(currentDistanceValueConfiguration);

                        viewModel.getProfileModel().getValue().setHapticMotor(hm);
                        dialog.dismiss();
                    }
                })
                .setNegativeButton(getResources().getString(R.string.cancel), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.i(MessageEnum.MOTOR_FRAGMENT_DEBUG.getMessage(), "Operation canceled");
                        dialog.dismiss();
                    }
                })
                // Disable the listener to prevent the window from closing when the neutral button is clicked
                .setNeutralButton(getResources().getString(R.string.test_motor), null);

        // Create a new listener to the neutral button
        final androidx.appcompat.app.AlertDialog alertDialog = dialog.create();
        // Setup the neutral button
        alertDialog.setOnShowListener(new DialogInterface.OnShowListener() {
            @Override
            public void onShow(final DialogInterface dialog) {

                Button neutralButton = alertDialog.getButton(AlertDialog.BUTTON_NEUTRAL);
                neutralButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        if (finalI >= 0 && finalI <= 1) {
                            viewModel.sendMessage("41" + "0" + waveValuesList.get(itemSelected[0]));
                        } else if (finalI%2 == 0) {
                            viewModel.sendMessage("41" + "1" + waveValuesList.get(itemSelected[0]));
                        } else {
                            viewModel.sendMessage("41" + "2" + waveValuesList.get(itemSelected[0]));
                        }
                        //Toast.makeText(fragmentActivity, String.valueOf(waveValuesList.get(itemSelected[0])), Toast.LENGTH_SHORT).show();

                    }
                });
            }
        });

        alertDialog.show();
    }

    void showDialogSetPulse(int finalI) {
        final int[] itemSelected = {1};
        // Convert the Wave List to a static array
        String[] optionsItems = pulseList.toArray(new String[0]);

        // Setup default positive and negative buttons
        MaterialAlertDialogBuilder dialog = new MaterialAlertDialogBuilder(fragmentActivity)
                .setTitle(getResources().getString(R.string.options))
                .setSingleChoiceItems(optionsItems, itemSelected[0], new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int selectedIndex) {
                        itemSelected[0] = selectedIndex;
                    }
                })
                .setPositiveButton(getResources().getString(R.string.select_profile), new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        Log.i(MessageEnum.MOTOR_FRAGMENT_DEBUG.getMessage(), String.valueOf(itemSelected[0]));
                        //arrayTxtPulse.get(finalI).setText(pulseList.get(itemSelected[0]));
                        arrayBtnPulse.get(finalI).setText(pulseList.get(itemSelected[0]));
                        currentPulseValueConfiguration.set(finalI, Integer.valueOf(pulseValuesList.get(itemSelected[0])));

                        HapticMotorModel hm = new HapticMotorModel();
                        hm.setPercentageValue(currentWaveValueConfiguration);
                        hm.setPulseValue(currentPulseValueConfiguration);
                        hm.setDistanceValue(currentDistanceValueConfiguration);

                        viewModel.getProfileModel().getValue().setHapticMotor(hm);
                        dialog.dismiss();

                    }
                })
                .setNegativeButton(getResources().getString(R.string.cancel), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.i(MessageEnum.MOTOR_FRAGMENT_DEBUG.getMessage(), "Operation canceled");
                        dialog.dismiss();
                    }
                })
                // Disable the listener to prevent the window from closing when the neutral button is clicked
                .setNeutralButton(getResources().getString(R.string.test_motor), null);

        // Create a new listener to the neutral button
        final androidx.appcompat.app.AlertDialog alertDialog = dialog.create();
        // Setup the neutral button
        alertDialog.setOnShowListener(new DialogInterface.OnShowListener() {
            @Override
            public void onShow(final DialogInterface dialog) {

                Button neutralButton = alertDialog.getButton(AlertDialog.BUTTON_NEUTRAL);
                neutralButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        if (finalI >= 0 && finalI <= 1) {
                            viewModel.sendMessage("41" + "0" + pulseValuesList.get(itemSelected[0]));
                        } else if (finalI%2 == 0) {
                            viewModel.sendMessage("41" + "1" + pulseValuesList.get(itemSelected[0]));
                        } else {
                            viewModel.sendMessage("41" + "2" + pulseValuesList.get(itemSelected[0]));
                        }
                        //Toast.makeText(fragmentActivity, String.valueOf(waveValuesList.get(itemSelected[0])), Toast.LENGTH_SHORT).show();

                    }
                });
            }
        });

        alertDialog.show();
    }

    // Setup the dialog box to configure the motor
    void showDialogSetDistance(int finalI) {
        final int[] itemSelected = {1};
        // Convert the Wave List to a static array
        String[] optionsItems = distanceList.toArray(new String[0]);

        // Setup default positive and negative buttons
        MaterialAlertDialogBuilder dialog = new MaterialAlertDialogBuilder(fragmentActivity)
                .setTitle(getResources().getString(R.string.options_scroll))
                .setSingleChoiceItems(optionsItems, itemSelected[0], new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int selectedIndex) {
                        itemSelected[0] = selectedIndex;
                    }
                })

                .setPositiveButton(getResources().getString(R.string.select_profile), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.i(MessageEnum.MOTOR_FRAGMENT_DEBUG.getMessage(), String.valueOf(itemSelected[0]));
                        arrayBtnDistance.get(finalI).setText(distanceList.get(itemSelected[0]));
                        currentDistanceValueConfiguration.set(finalI, Integer.valueOf(distanceValuesList.get(itemSelected[0])));

                        HapticMotorModel hm = new HapticMotorModel();
                        hm.setPercentageValue(currentWaveValueConfiguration);
                        hm.setPulseValue(currentPulseValueConfiguration);
                        hm.setDistanceValue(currentDistanceValueConfiguration);

                        viewModel.getProfileModel().getValue().setHapticMotor(hm);
                        dialog.dismiss();
                    }
                })
                .setNegativeButton(getResources().getString(R.string.cancel), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.i(MessageEnum.MOTOR_FRAGMENT_DEBUG.getMessage(), "Operation canceled");
                        dialog.dismiss();
                    }
                })
                // Disable the listener to prevent the window from closing when the neutral button is clicked
                .setNeutralButton(getResources().getString(R.string.test_motor), null);

        // Create a new listener to the neutral button
        final androidx.appcompat.app.AlertDialog alertDialog = dialog.create();
        // Setup the neutral button
        alertDialog.setOnShowListener(new DialogInterface.OnShowListener() {
            @Override
            public void onShow(final DialogInterface dialog) {

                Button neutralButton = alertDialog.getButton(AlertDialog.BUTTON_NEUTRAL);
                neutralButton.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View v) {
                        if (finalI >= 0 && finalI <= 1) {
                            viewModel.sendMessage("41" + "0" + distanceValuesList.get(itemSelected[0]));
                        } else if (finalI%2 == 0) {
                            viewModel.sendMessage("41" + "1" + distanceValuesList.get(itemSelected[0]));
                        } else {
                            viewModel.sendMessage("41" + "2" + distanceValuesList.get(itemSelected[0]));
                        }
                    }
                });
            }
        });

        alertDialog.show();
    }
}