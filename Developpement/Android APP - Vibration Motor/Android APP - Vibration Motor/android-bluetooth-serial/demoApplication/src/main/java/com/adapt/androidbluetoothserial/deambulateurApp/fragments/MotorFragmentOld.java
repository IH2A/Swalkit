package com.adapt.androidbluetoothserial.deambulateurApp.fragments;

import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.ViewModelProvider;

import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.PagerViewModel;
import com.adapt.androidbluetoothserial.deambulateurApp.R;
import com.google.android.material.slider.Slider;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;

public class MotorFragmentOld extends Fragment {

    // View model instance
    private PagerViewModel viewModel;
    private FragmentActivity fragmentActivity;
    private ArrayList<Button> arrayBtnMotors = new ArrayList<>();
    private ArrayList<Slider> arraySliderMotors = new ArrayList<>();
    private Button btnTestAll;

    @NonNull
    public static Fragment newInstance() {
        return new MotorFragment();
    }

    public MotorFragmentOld() {super(R.layout.fragment_motors);}

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_motors, container, false);

        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_1));
        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_2));
        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_3));
        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_4));
        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_5));
        arrayBtnMotors.add(view.findViewById(R.id.btn_motor_6));
        btnTestAll = view.findViewById(R.id.btn_test_all);

        arraySliderMotors.add(view.findViewById(R.id.sliderMotor1));
        arraySliderMotors.add(view.findViewById(R.id.sliderMotor2));
        arraySliderMotors.add(view.findViewById(R.id.sliderMotor3));
        arraySliderMotors.add(view.findViewById(R.id.sliderMotor4));
        arraySliderMotors.add(view.findViewById(R.id.sliderMotor5));
        arraySliderMotors.add(view.findViewById(R.id.sliderMotor6));


        for (int i = 0; i < arrayBtnMotors.size(); i ++) {
            int finalI = i;
            arrayBtnMotors.get(i).setOnClickListener(v -> createTestOneMotorJSONMessage(finalI));
        }

        btnTestAll.setOnClickListener(v -> createTestAllJSONMessage());

        return view;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        //viewModel  = ViewModelProviders.of(this).get(ParserViewModel.class);
        fragmentActivity = getActivity();
        viewModel = new ViewModelProvider(fragmentActivity).get(PagerViewModel.class);
    }

    void createTestOneMotorJSONMessage(int motorIndex) {
        JSONObject jsonObject = new JSONObject();
        JSONArray jsonArray = new JSONArray();
        try {
            for (int i = 0; i < arrayBtnMotors.size(); i ++) {
                if (i == motorIndex) {
                    jsonArray.put((int) arraySliderMotors.get(i).getValue());
                } else {
                    jsonArray.put(0);
                }
            }
            jsonObject.put("motors", (Object) jsonArray);
            Log.i("MOTORS FRAGMENT", "Writing on Serial\n" + jsonObject.toString());
            viewModel.sendMessage("99"+jsonObject.toString());
        } catch (JSONException e) {
            Log.e("MOTORS FRAGMENT FAILED","Json parsing error: " + e.getMessage());
        }
    }

    void createTestAllJSONMessage() {
        JSONObject jsonObject = new JSONObject();
        JSONArray jsonArray = new JSONArray();
        try {
            for (int i = 0; i < arrayBtnMotors.size(); i ++) {
                jsonArray.put((int) arraySliderMotors.get(i).getValue());
            }
            jsonObject.put("motors", (Object) jsonArray);
            Log.i("MOTORS FRAGMENT", "Writing on Serial\n" + jsonObject.toString());
            viewModel.sendMessage("99"+jsonObject.toString());
        } catch (JSONException e) {
            Log.e("MOTORS FRAGMENT FAILED","Json parsing error: " + e.getMessage());
        }
    }
}
