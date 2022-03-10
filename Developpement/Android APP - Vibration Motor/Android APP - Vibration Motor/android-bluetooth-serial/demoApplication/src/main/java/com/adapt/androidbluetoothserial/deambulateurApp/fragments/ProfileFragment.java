package com.adapt.androidbluetoothserial.deambulateurApp.fragments;

import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.ViewModelProvider;

import com.adapt.androidbluetoothserial.deambulateurApp.PagerActivity;
import com.adapt.androidbluetoothserial.deambulateurApp.models.ProfileModel;
import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.ProfileViewModel;
import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.PagerViewModel;
import com.adapt.androidbluetoothserial.deambulateurApp.R;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import java.util.Objects;

public class ProfileFragment extends Fragment {

    // View model instance
    private PagerViewModel viewModel;
    private FragmentActivity fragmentActivity;

    // Interfaces to the elements of the UI
    private TextView profileFileView, profileUserView, profileSurnameView;

    private ProfileModel currentProfile;
    private boolean isFirstSetup;

    private Button btnApplyProfile;

    ArrayAdapter<String> profileListAdapter;

    @NonNull
    public static Fragment newInstance() {
        return new ProfileFragment();
    }

    public ProfileFragment() {super(R.layout.fragment_profile);}

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_profile, container, false);
        profileFileView = view.findViewById(R.id.profileInfo);
        profileUserView = view.findViewById(R.id.userInfo);
        profileSurnameView = view.findViewById(R.id.surnameInfo);

        btnApplyProfile = view.findViewById(R.id.BtnApply);
        btnApplyProfile.setOnClickListener(v -> actualise());

        return view;
    }


    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        currentProfile = new ProfileModel();

        fragmentActivity = getActivity();
        viewModel = new ViewModelProvider(fragmentActivity).get(PagerViewModel.class);
        isFirstSetup = true;

        // If there is a loaded profile and
        viewModel.getProfileModel().observe(fragmentActivity, profileModel -> {
                if (isFirstSetup && profileFileView.getText().toString().equals("-") && profileModel.getFileName() != null) {
                    currentProfile = profileModel;
                    profileFileView.setText(profileModel.getFileName());
                    profileUserView.setText(profileModel.getUserName());
                    profileSurnameView.setText(profileModel.getUserSurname());
                    isFirstSetup = false;
                    viewModel.setShouldUpdateProfile(false);
                    Log.i("PROFILE FRAGMENT", "Loaded base profile: " + profileModel.getFileName());
                }
                else if (profileModel.getFileName() != null) {
                    if (viewModel.getShouldUpdateProfile().getValue()) {
                        currentProfile = profileModel;
                        profileFileView.setText(profileModel.getFileName());
                        profileUserView.setText(profileModel.getUserName());
                        profileSurnameView.setText(profileModel.getUserSurname());
                        viewModel.setShouldUpdateProfile(false);
                        Log.i("PROFILE FRAGMENT", "Profile Updated to: " + profileModel.getFileName());
                    }
                }
        });

//        profileFileView.setOnFocusChangeListener(new View.OnFocusChangeListener() {
//            public void onFocusChange(View v, boolean hasFocus) {
//                currentProfile.setFileName(profileFileView.getText().toString());
//                viewModel.setProfileModel(currentProfile);
//            }
//        });
//
//        profileUserView.setOnFocusChangeListener(new View.OnFocusChangeListener() {
//            public void onFocusChange(View v, boolean hasFocus) {
//                currentProfile.setUserName(profileUserView.getText().toString());
//                viewModel.setProfileModel(currentProfile);
//
//            }
//        });
//
//        profileSurnameView.setOnFocusChangeListener(new View.OnFocusChangeListener() {
//            public void onFocusChange(View v, boolean hasFocus) {
//                currentProfile.setUserSurname(profileSurnameView.getText().toString());
//                viewModel.setProfileModel(currentProfile);
//            }
//        });
    }

    public void actualise(){
        currentProfile.setFileName(profileFileView.getText().toString());
        currentProfile.setUserName(profileUserView.getText().toString());
        currentProfile.setUserSurname(profileSurnameView.getText().toString());
        viewModel.setProfileModel(currentProfile);
        Toast.makeText(getContext(), R.string.applied, Toast.LENGTH_SHORT).show();
    }
}