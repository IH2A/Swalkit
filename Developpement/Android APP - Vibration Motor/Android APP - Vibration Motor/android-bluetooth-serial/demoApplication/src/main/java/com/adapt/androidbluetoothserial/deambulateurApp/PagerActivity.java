package com.adapt.androidbluetoothserial.deambulateurApp;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.lifecycle.ViewModelProviders;
import androidx.viewpager.widget.PagerAdapter;
import androidx.viewpager.widget.ViewPager;

import com.adapt.androidbluetoothserial.deambulateurApp.enums.MessageEnum;
import com.adapt.androidbluetoothserial.deambulateurApp.fragments.MotorFragment;
import com.adapt.androidbluetoothserial.deambulateurApp.fragments.ProfileFragment;
import com.adapt.androidbluetoothserial.deambulateurApp.fragments.SensorsFragment;
import com.adapt.androidbluetoothserial.deambulateurApp.models.ProfileModel;
import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.PagerViewModel;
import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.ProfileViewModel;
import com.google.android.material.dialog.MaterialAlertDialogBuilder;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Objects;

public class PagerActivity extends AppCompatActivity {

    // Interfaces to the elements of the UI
    private TextView connectionText;
    private ConstraintLayout btnToolbarConnect, btnToolbarSave, btnToolbarDelete, btnToolbarProfile, loadingPage;
    private Fragment pager1fragment, pager2fragment, pager3fragment;
    private ImageView imgToolbarConnect;
    private ConstraintLayout pagerItem1, pagerItem2, pagerItem3;
    ViewPager pager;
    PageAdapter a;

    ArrayAdapter<String> profileListAdapter;
    ArrayList<String> profileList;

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater menuInflater = getMenuInflater();
        menuInflater.inflate(R.menu.bluetooth_menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    // Value to check actions that are done only on the startup and can't be on onCreate
    private boolean startup = true;

    // View model instance
    private PagerViewModel viewModel;
    private ProfileViewModel profileViewModel;
    private ProfileModel currentProfile;

    // View model

    // Setup the view when the activity is called
    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // Setup our activity
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_pager);
        // Enable the back button in the action bar if possible
        if (getSupportActionBar() != null) {
            getSupportActionBar().setDisplayHomeAsUpEnabled(true);
        }

        // Remove action bar shadow
        getSupportActionBar().setElevation(0);

        // Setup our ViewModel
        viewModel = ViewModelProviders.of(this).get(PagerViewModel.class);
        profileViewModel = ViewModelProviders.of(this).get(ProfileViewModel.class);

        // This method return false if there is an error, so if it does, we should close.
        if (!viewModel.setupViewModel(getIntent().getStringExtra("device_name"), getIntent().getStringExtra("device_mac"))) {
            finish();
            return;
        }

        pagerItem1 = findViewById(R.id.pager_item_1_box);
        pagerItem2 = findViewById(R.id.pager_item_2_box);
        pagerItem3 = findViewById(R.id.pager_item_3_box);
        loadingPage = findViewById(R.id.loading_page);

        pager1fragment = ProfileFragment.newInstance();
        pager2fragment = MotorFragment.newInstance();
        pager3fragment = SensorsFragment.newInstance();

        pager = findViewById(R.id.pager);
        pager.setOffscreenPageLimit(3);
        a = new PageAdapter(getSupportFragmentManager());
        pager.setAdapter(a);

        pager.addOnAdapterChangeListener(new ViewPager.OnAdapterChangeListener() {

            @Override
            public void onAdapterChanged(@NonNull ViewPager viewPager, @Nullable PagerAdapter oldAdapter, @Nullable PagerAdapter newAdapter) {

            }
        });

        pager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {
            }

            @Override
            public void onPageSelected(int position) {
                switch (position) {
                    case 0:
                        pagerItem1.setVisibility(View.VISIBLE);
                        pagerItem2.setVisibility(View.INVISIBLE);
                        pagerItem3.setVisibility(View.INVISIBLE);
                        break;
                    case 1:
                        pagerItem1.setVisibility(View.INVISIBLE);
                        pagerItem2.setVisibility(View.VISIBLE);
                        pagerItem3.setVisibility(View.INVISIBLE);
                        break;
                    case 2:
                        pagerItem1.setVisibility(View.INVISIBLE);
                        pagerItem2.setVisibility(View.INVISIBLE);
                        pagerItem3.setVisibility(View.VISIBLE);
                        break;
                }
            }

            @Override
            public void onPageScrollStateChanged(int state) {
            }
        });
        // Setup our Views
        connectionText = findViewById(R.id.text_connection);

        btnToolbarConnect = findViewById(R.id.btn_connection);
        btnToolbarProfile = findViewById(R.id.btn_profile);
        btnToolbarSave = findViewById(R.id.btn_save);
        btnToolbarDelete = findViewById(R.id.btn_delete);
        imgToolbarConnect = findViewById(R.id.img_connection);


        // Start observing the data sent to us by the ViewModel
        viewModel.getConnectionStatus().observe(this, this::onConnectionStatus);
        viewModel.getDeviceName().observe(this, name -> setTitle(getString(R.string.device_name_format, name)));
        viewModel.getShouldUpdateProfileSensors().observe(this, v -> {
                if (v) {
                    pager.setVisibility(View.VISIBLE);
                    loadingPage.setVisibility(View.GONE);
                }
                });

        // Dropdown list sorted in alphabetic order
        viewModel.getListProfiles().observe(this, list -> {
            profileList = list;
            profileListAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_item, list);
            profileListAdapter.sort(new Comparator<String>() {
                @Override
                public int compare(String o1, String o2) {
                    return o1.compareTo(o2);
                }
            });
            Toast.makeText(this, "Profiles updated", Toast.LENGTH_SHORT);
        });

        // Setup profile selection button
        btnToolbarProfile.setOnClickListener(v -> showDialogProfileList());

        btnToolbarSave.setOnClickListener(v ->
                sendSaveAlert(getResources().getString(R.string.save_file_alert),
                        getResources().getString(R.string.confirm),
                        getResources().getString(R.string.reject)));

        btnToolbarDelete.setOnClickListener(v -> sendDeleteAlert(
                getResources().getString(R.string.delete_file_alert),
                getResources().getString(R.string.confirm),
                getResources().getString(R.string.reject)));

        viewModel.getProfileModel().observe(this, profileModel -> {
            currentProfile = profileModel;
        });

    }

    private class PageAdapter extends FragmentPagerAdapter {

        PageAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int pos) {
            Fragment f = null;

            switch(pos) {
                case 0:
                    f = pager1fragment;
                    break;
                case 1:
                    f = pager2fragment;
                    break;
                case 2:
                    f = pager3fragment;
                    break;
            }

            return f;
        }

        @Override
        public int getCount() {
            return 3;
        }
    }

    public void showDialogProfileList() {
        final int[] itemSelected = {0};
        String[] optionsItems = new String[profileListAdapter.getCount()];
        for (int i = 0; i < profileListAdapter.getCount(); i ++){
            // Ignores the cache file that will be created during the execution of the app
            if (!Objects.equals(profileListAdapter.getItem(i), "hw_cached_resid.list")
                    && !Objects.requireNonNull(profileListAdapter.getItem(i)).isEmpty())
                optionsItems[i] = profileListAdapter.getItem(i);
        }
        new MaterialAlertDialogBuilder(this)
                .setTitle(getResources().getString(R.string.profile))
                .setSingleChoiceItems(optionsItems, itemSelected[0], new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialogInterface, int selectedIndex) {
                        itemSelected[0] = selectedIndex;
                    }
                })
                .setPositiveButton(getResources().getString(R.string.select_profile), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        //viewModel.sendMessage("11"+ profileListAdapter.getItem(itemSelected[0]));
                        Log.i("PagerActivity", profileViewModel.readFromFile(profileListAdapter.getItem(itemSelected[0]) + ".json", getApplicationContext()));
                        Log.d("PagerActivity", String.valueOf(itemSelected[0]));
                        //ProfileModel profileModelAux = ProfileModel(profileViewModel.readFromFile(profileListAdapter.getItem(itemSelected[0]) + ".json", getApplicationContext()));
                        //viewModel.setProfileModel();
                        viewModel.setProfile(profileViewModel.readFromFile(profileListAdapter.getItem(itemSelected[0]) + ".json", getBaseContext()));
                        dialog.dismiss();
                    }
                })
                .setNegativeButton(getResources().getString(R.string.cancel), new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        Log.d("PagerActivity", "Operation canceled");
                        dialog.dismiss();
                    }
                })
                .show();
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
        Log.i("PAGER ACTIVITY", "Sending profile\n" + currentProfile.setProfileAsJSONObject().toString());
        profileViewModel.writeToFile(currentProfile.setProfileAsJSONObject().toString(),
                currentProfile.getFileName() + ".json",
                this);

        ArrayList<String> filesInDirectory = profileViewModel.filesInTheDirectory(this);
        viewModel.setupProfileList(filesInDirectory);

        viewModel.sendMessage(String.valueOf(80) + currentProfile.setProfileAsCompactJSONObject().toString());
        Toast.makeText(this, "File activated!", Toast.LENGTH_SHORT).show();
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
        profileViewModel.deleteFile(viewModel.getProfileModel().getValue().getFileName(), this);
        // Refresh the profile list view
        ArrayList<String> filesInDirectory = profileViewModel.filesInTheDirectory(this);
        viewModel.setupProfileList(filesInDirectory);

        Toast.makeText(this, "File deleted!", Toast.LENGTH_SHORT).show();
        showDialogProfileList();
    }

    // Called when the ViewModel updates us of our connectivity status
    private void onConnectionStatus(PagerViewModel.ConnectionStatus connectionStatus) {
        switch (connectionStatus) {
            case CONNECTED:
                // Setup the connection toolbar icon and button
                connectionText.setText(R.string.status_connected);
                btnToolbarConnect.setEnabled(true);
                imgToolbarConnect.setImageDrawable(getResources().getDrawable(R.drawable.ic_baseline_bluetooth_24));
                btnToolbarConnect.setOnClickListener(v -> viewModel.disconnect());

                // Create a mock profile to assure the person will always have something as reference
                ProfileModel testModel = new ProfileModel(true, 2, 4);
                Log.i(MessageEnum.PAGER_ACT_MESSAGE_SUCCESS.getMessage(), "Created: " + testModel.getProfileJSON().toString());
                profileViewModel.writeToFile(testModel.getProfileJSON().toString(), "Base Profile.json", this);
                viewModel.setProfile(profileViewModel.readFromFile("Base Profile.json", this));
                viewModel.sendMessage("00"); // Asks for the current profile

                // Refresh the profile list view
                ArrayList<String> filesInDirectory = profileViewModel.filesInTheDirectory(this);
                Log.i(MessageEnum.PAGER_VM_LOG_MESSAGE_SUCCESS.getMessage(), String.valueOf(filesInDirectory));
                viewModel.setupProfileList(filesInDirectory);
                break;

            case CONNECTING:
                connectionText.setText(R.string.status_connecting);
                imgToolbarConnect.setImageDrawable(getResources().getDrawable(R.drawable.ic_baseline_bluetooth_audio_24));
                break;

            case DISCONNECTED:
                connectionText.setText(R.string.status_disconnected);
                btnToolbarConnect.setEnabled(true);
                imgToolbarConnect.setImageDrawable(getResources().getDrawable(R.drawable.ic_baseline_bluetooth_disabled_24));
                btnToolbarConnect.setOnClickListener(v -> viewModel.connect());
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
            case R.id.item1:
                Intent intent = new Intent(this, CommunicateActivity.class);
                intent.putExtra("device_name", getIntent().getStringExtra("device_name"));
                intent.putExtra("device_mac", getIntent().getStringExtra("device_mac"));
                startActivity(intent);
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    // Called when the user presses the back button
    @Override
    public void onBackPressed() {
        // Close the activity
        viewModel.disconnect();
        finish();
    }

    public PagerViewModel getViewModel() {
        return viewModel;
    }
}
