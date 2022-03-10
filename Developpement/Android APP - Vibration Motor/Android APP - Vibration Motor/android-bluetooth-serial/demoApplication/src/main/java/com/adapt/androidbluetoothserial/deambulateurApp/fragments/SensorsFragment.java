package com.adapt.androidbluetoothserial.deambulateurApp.fragments;

import android.annotation.SuppressLint;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentActivity;
import androidx.lifecycle.ViewModelProvider;

import com.adapt.androidbluetoothserial.deambulateurApp.enums.MessageEnum;
import com.adapt.androidbluetoothserial.deambulateurApp.models.ProfileModel;
import com.adapt.androidbluetoothserial.deambulateurApp.models.SensorModel;
import com.adapt.androidbluetoothserial.deambulateurApp.viewModel.PagerViewModel;
import com.adapt.androidbluetoothserial.deambulateurApp.R;
import com.github.mikephil.charting.charts.CombinedChart;
import com.github.mikephil.charting.charts.ScatterChart;
import com.github.mikephil.charting.components.XAxis;
import com.github.mikephil.charting.components.YAxis;
import com.github.mikephil.charting.data.CombinedData;
import com.github.mikephil.charting.data.DataSet;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;
import com.github.mikephil.charting.data.ScatterData;
import com.github.mikephil.charting.data.ScatterDataSet;
import com.github.mikephil.charting.highlight.Highlight;
import com.github.mikephil.charting.listener.OnChartValueSelectedListener;
import com.github.mikephil.charting.utils.EntryXComparator;
import com.google.android.material.switchmaterial.SwitchMaterial;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;

public class SensorsFragment extends Fragment implements OnChartValueSelectedListener {

    private CombinedChart combinedChart;
    CombinedData combinedData;

    private TextView currentSelectedPoint, currentSelectedPointData;
    private EditText currentSensorAngle;
    private Button btnSetCurrentAngle;

    private ScatterDataSet scatterDataSetLeft, scatterDataSetRight, scatterDataSetReference;
    private LineDataSet lineDataSet;
    private ArrayList<LineDataSet> arrayLineDataSet;
    private int currentTrackedIndex;
    private boolean isTrackingEnabled = false;
    private boolean isRightSensors;
    private ArrayList scatterEntriesLeft, scatterEntriesRight, scatterEntriesReference, lineEntries;
    private ArrayList<ArrayList> starLineEntries;
    private ArrayList<Float> sensorsOffset;

    double[] angles;
    double[] displacement;


    // View model instance
    private PagerViewModel viewModel;
    private FragmentActivity fragmentActivity;

    // Switches and configurations
    private SwitchMaterial switchCubic, switchLine, switchScatter;

    @NonNull
    public static Fragment newInstance() {
        return new SensorsFragment();
    }

    public SensorsFragment() {super(R.layout.fragment_sensors);}

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_sensors, container, false);
        return view;
    }

    @RequiresApi(api = Build.VERSION_CODES.N)
    @Override
    public void onViewCreated(@NonNull View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        fragmentActivity = getActivity();
        viewModel = new ViewModelProvider(fragmentActivity).get(PagerViewModel.class);

        displacement = new double[]{0, 0};
        angles = new double[]{11, 34, 56, 79, 101, 124, 146, 169};

        currentSelectedPoint = view.findViewById(R.id.current_selected_point);
        currentSelectedPointData = view.findViewById(R.id.current_selected_point_data);
        currentSensorAngle = view.findViewById(R.id.edit_sensor_relative_angle);

        combinedChart = view.findViewById(R.id.scatter_chart);
        createGraph();
        getSensors();

        switchScatter = view.findViewById(R.id.switchScatter);
        switchScatter.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @SuppressLint("UseCompatLoadingForDrawables")
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    scatterDataSetLeft.setDrawIcons(true);
                    scatterDataSetRight.setDrawIcons(true);
                    scatterDataSetLeft.setColor(getResources().getColor(R.color.primary_blue2));
                    scatterDataSetRight.setColor(getResources().getColor(R.color.primary_blue));
                } else {
                    scatterDataSetLeft.setDrawIcons(false);
                    scatterDataSetRight.setDrawIcons(false);
                    scatterDataSetLeft.setColor(Color.TRANSPARENT);
                    scatterDataSetRight.setColor(Color.TRANSPARENT);
                }
                scatterDataSetLeft.notifyDataSetChanged();
                scatterDataSetRight.notifyDataSetChanged();
                for (LineDataSet set : arrayLineDataSet) {
                    set.notifyDataSetChanged();
                }
                combinedChart.invalidate();
            }
        });

        switchLine = view.findViewById(R.id.switchLine);
        switchLine.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @SuppressLint("UseCompatLoadingForDrawables")
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if (isChecked) {
                    lineDataSet.setDrawFilled(true);
                    lineDataSet.setColor(getResources().getColor(R.color.secondary_blue));
                } else {
                    lineDataSet.setDrawFilled(false);
                    lineDataSet.setColor(Color.TRANSPARENT);
                }
                lineDataSet.notifyDataSetChanged();
                combinedChart.invalidate();
            }
        });

        switchCubic = view.findViewById(R.id.switchCubic);
        switchCubic.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                lineDataSet.setMode(isChecked
                        ? LineDataSet.Mode.CUBIC_BEZIER
                        : LineDataSet.Mode.LINEAR);
                lineDataSet.notifyDataSetChanged();
                combinedChart.invalidate();
            }
        });

        btnSetCurrentAngle = view.findViewById(R.id.btn_set_angle);

        btnSetCurrentAngle.setOnClickListener(v ->
                onChangeAngleBtnCLicked());

    }

    private void createGraph() {

        combinedChart.setOnChartValueSelectedListener(this);
        combinedChart.getDescription().setEnabled(false);
        combinedData = new CombinedData();
        combinedData.setData(generateScatterData());
        LineData lineData = generateLineData();
        generateStarLineData();
        for (LineDataSet lineDataInfo : arrayLineDataSet)
            lineData.addDataSet(lineDataInfo);
        combinedData.setData(lineData);
        combinedChart.setData(combinedData);
        combinedChart.getLegend().setEnabled(false);
        combinedChart.setDrawOrder(new CombinedChart.DrawOrder[]{
                CombinedChart.DrawOrder.LINE, CombinedChart.DrawOrder.SCATTER
        });


        YAxis yAxis = combinedChart.getAxisLeft();
        yAxis.setAxisMaximum(110f);
        yAxis.setAxisMinimum(0f);
        yAxis.setDrawLabels(false);
        yAxis.setDrawAxisLine(false);
        yAxis.setDrawGridLines(true);
        XAxis xAxis = combinedChart.getXAxis();
        xAxis.setAxisMaximum(150f);
        xAxis.setAxisMinimum(-150f);
        xAxis.setDrawLabels(false);
        xAxis.setDrawAxisLine(true);
        xAxis.setDrawGridLines(true);
        combinedChart.getAxisRight().setDrawGridLines(false);
        combinedChart.getAxisRight().setDrawLabels(false);
        combinedChart.getAxisRight().setDrawAxisLine(false);

        combinedChart.invalidate();
    }

    private LineData generateLineData() {
        LineData lineData = new LineData();
        lineEntries = new ArrayList<>();
        lineDataSet = new LineDataSet(lineEntries, "Sensors");
        lineDataSet.setDrawValues(false);
        lineDataSet.setDrawCircles(false);
        lineDataSet.setDrawFilled(true);
        // drawables only supported on api level 18 and above
        Drawable drawable = getResources().getDrawable(R.drawable.fade_primary);
        lineDataSet.setFillDrawable(drawable);

        lineDataSet.setMode(LineDataSet.Mode.CUBIC_BEZIER);

        lineData.addDataSet(lineDataSet);

        return lineData;
    }

    private ArrayList<LineData> generateStarLineData() {
        ArrayList<LineData> arrayLineData = new ArrayList<>();
        starLineEntries = new ArrayList<>();
        arrayLineDataSet = new ArrayList<>();

        for (int i = 0; i < 8; i ++){
            starLineEntries.add(new ArrayList<>());
            starLineEntries.get(i).add(new Entry(sensorsOffset.get(i<4? 0 : 1), 3));
            arrayLineDataSet.add(new LineDataSet(starLineEntries.get(i), "Line ref"));
            arrayLineData.add(new LineData(arrayLineDataSet.get(i)));
            arrayLineDataSet.get(i).setDrawFilled(false);
            arrayLineDataSet.get(i).setDrawCircles(false);
            arrayLineDataSet.get(i).setDrawValues(false);
        }
        return arrayLineData;
    }

    private ScatterData generateScatterData() {
        ScatterData scatterData;

        sensorsOffset = new ArrayList<>();
        sensorsOffset.add(0, 23.5f);
        sensorsOffset.add(1, -23.5f);

        scatterEntriesLeft = new ArrayList<>();
        scatterEntriesRight = new ArrayList<>();
        scatterEntriesReference = new ArrayList<>();
        scatterEntriesReference.add(new Entry(sensorsOffset.get(0), 3));
        scatterEntriesReference.add(new Entry(sensorsOffset.get(1), 3));

        scatterDataSetLeft = new ScatterDataSet(scatterEntriesLeft, "Sensors Left");
        scatterDataSetRight = new ScatterDataSet(scatterEntriesRight, "Sensors Right");
        scatterDataSetReference = new ScatterDataSet(scatterEntriesReference, "Reference");
        scatterDataSetLeft.setScatterShape(ScatterChart.ScatterShape.CIRCLE);
        scatterDataSetRight.setScatterShape(ScatterChart.ScatterShape.CIRCLE);
        scatterDataSetReference.setScatterShape(ScatterChart.ScatterShape.CIRCLE);

        scatterDataSetReference.setColors(getResources().getColor(R.color.primary_red));
        scatterDataSetReference.setScatterShapeSize(25f);

        scatterDataSetLeft.setColors(getResources().getColor(R.color.primary_blue));
        scatterDataSetLeft.setScatterShapeSize(30f);

        scatterDataSetRight.setColors(getResources().getColor(R.color.secondary_blue));
        scatterDataSetRight.setScatterShapeSize(30f);

        scatterDataSetLeft.setDrawValues(false);
        scatterDataSetRight.setDrawValues(false);
        scatterDataSetReference.setDrawValues(false);

        scatterData = new ScatterData(scatterDataSetLeft);
        scatterData.addDataSet(scatterDataSetRight);
        scatterData.addDataSet(scatterDataSetReference);
        return scatterData;
    }


    @RequiresApi(api = Build.VERSION_CODES.N)
    private void getSensors() {
        viewModel.getProfileModel().observe(fragmentActivity, profileModel-> {

            ArrayList<SensorModel> sensors = profileModel.getArraySensors();
            lineEntries.clear();
            for (ArrayList entriesList : starLineEntries)
                entriesList.clear();
            scatterEntriesLeft.clear();
            scatterEntriesRight.clear();
            try{
                for (int i = 0; i< sensors.size(); i++) {

                    float xValue = (float) (Math.cos(Math.toRadians(sensors.get(i).getRelativeAngleDeg()))*sensors.get(i).getCurrentValue()+sensors.get(i).getOffset());
                    float yValue = (float) (Math.sin(Math.toRadians(sensors.get(i).getRelativeAngleDeg()))*(sensors.get(i).getCurrentValue()));
                    if (sensors.get(i).getLocation().equals("r")) {
                        scatterEntriesRight.add(new Entry(xValue, yValue));
                        starLineEntries.get(i).add(new Entry(xValue, yValue));
                        starLineEntries.get(i).add(new Entry(sensorsOffset.get(0), 3));
                        starLineEntries.get(i).sort(new EntryXComparator());
                    }
                    else{
                        scatterEntriesLeft.add(new Entry(xValue, yValue));
                        starLineEntries.get(i).add(new Entry(xValue, yValue));
                        starLineEntries.get(i).add(new Entry(sensorsOffset.get(1), 3));
                        starLineEntries.get(i).sort(new EntryXComparator());
                    }
                    lineEntries.add( new Entry(xValue, yValue));
                }
            } catch (Exception ex) {
                //...
            }
            lineEntries.sort(new EntryXComparator());
            lineDataSet.notifyDataSetChanged();

            for (LineDataSet lineDataSet : arrayLineDataSet)
                lineDataSet.notifyDataSetChanged();

            scatterEntriesLeft.sort(new EntryXComparator());
            scatterEntriesRight.sort(new EntryXComparator());
            scatterDataSetLeft.notifyDataSetChanged();
            scatterDataSetRight.notifyDataSetChanged();

            combinedChart.notifyDataSetChanged();
            combinedChart.invalidate();

            setSelectedPointText();
        });
    }

    @Override
    public void onValueSelected(Entry e, Highlight h) {
        Log.i(MessageEnum.SENSORS_FRAGMENT_DEBUG.getMessage(), "Entry selected:\t" + e.toString());
        //Log.i("High", h.toString());
        if (!scatterDataSetLeft.getEntriesForXValue(h.getX()).isEmpty()) {
            currentTrackedIndex = scatterDataSetLeft.getEntryIndex(h.getX(), h.getY(), DataSet.Rounding.CLOSEST);
            isRightSensors = false;
        } else {
            currentTrackedIndex = scatterDataSetRight.getEntryIndex(h.getX(), h.getY(), DataSet.Rounding.CLOSEST);
            isRightSensors = true;
        }
        Log.i(MessageEnum.SENSORS_FRAGMENT_DEBUG.getMessage(), "Index:\t" + String.valueOf(currentTrackedIndex));
        isTrackingEnabled = true;
        setSelectedPointText();
    }

    @Override
    public void onNothingSelected() {
        Log.i(MessageEnum.SENSORS_FRAGMENT_DEBUG.getMessage(), "Nothing selected.");
        isTrackingEnabled = false;
        currentSelectedPoint.setText("X: -- Y: --");
        currentSelectedPointData.setText("--");
        currentSensorAngle.setText("--");
    }

    private void setSelectedPointText() {
        if (isTrackingEnabled) {
            String newPoint = isRightSensors ?
                    "X: " + Math.round(scatterDataSetRight.getEntryForIndex(currentTrackedIndex).getX()) +
                    " Y: " + Math.round(scatterDataSetRight.getEntryForIndex(currentTrackedIndex).getY())
                    : "X: " + Math.round(scatterDataSetLeft.getEntryForIndex(currentTrackedIndex).getX()) +
                    " Y: " + Math.round(scatterDataSetLeft.getEntryForIndex(currentTrackedIndex).getY());
            currentSelectedPoint.setText(newPoint);
            currentSelectedPointData.setText(isRightSensors ?
                            getResources().getString(R.string.right_sensor_id_reference) + currentTrackedIndex:
                            getResources().getString(R.string.left_sensor_id_reference) + currentTrackedIndex
                    );
            if (!currentSensorAngle.hasFocus())
                currentSensorAngle.setText( isRightSensors ?
                            String.valueOf(angles[3-currentTrackedIndex]):
                            String.valueOf(angles[7-currentTrackedIndex])
                    );
        }
    }

    private void onChangeAngleBtnCLicked() {
        if (isTrackingEnabled) {
            double newAngleAux = Double.valueOf(currentSensorAngle.getText().toString());
            int newAngle = (int) newAngleAux;
            angles[isRightSensors ?
                    3 - currentTrackedIndex :
                    7 - currentTrackedIndex] = newAngle;
            ProfileModel profileModel = viewModel.getProfileModel().getValue();
            ArrayList<SensorModel> arrayListAux = profileModel.getArraySensors();
            arrayListAux.get(isRightSensors ?
                    3 - currentTrackedIndex :
                    7 - currentTrackedIndex).setRelativeAngleDeg(newAngle);
            profileModel.setArraySensors(arrayListAux);
            currentSensorAngle.clearFocus();
        }
    }
}

