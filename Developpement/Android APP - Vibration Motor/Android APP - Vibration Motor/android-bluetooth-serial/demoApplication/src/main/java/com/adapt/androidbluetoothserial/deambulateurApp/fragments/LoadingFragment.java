package com.adapt.androidbluetoothserial.deambulateurApp.fragments;

import android.os.Build;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.ViewModelProvider;

import com.adapt.androidbluetoothserial.deambulateurApp.R;
import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.PagerViewModel;

public class LoadingFragment extends Fragment {


    // View model instance
    private PagerViewModel viewModel;
    private FragmentActivity fragmentActivity;

    @NonNull
    public static Fragment newInstance() {
        return new LoadingFragment();
    }

    public LoadingFragment() {super(R.layout.fragment_loading);}

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_loading, container, false);

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

}
