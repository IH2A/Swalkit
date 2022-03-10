package com.adapt.androidbluetoothserial.deambulateurApp.viewModel;

import android.app.Application;
import android.content.Context;
import android.content.DialogInterface;
import android.util.Log;

import androidx.lifecycle.AndroidViewModel;

import org.jetbrains.annotations.NotNull;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;

public class ProfileViewModel extends AndroidViewModel {

    // Called by the system, this is just a constructor that matches AndroidViewModel.
    public ProfileViewModel(@NotNull Application application) {
        super(application);
    }

    // Write files to the root directory
    public void writeToFile(String data, String fileName, Context context) {
        try {
            OutputStreamWriter outputStreamWriter = new OutputStreamWriter(context.openFileOutput(fileName, Context.MODE_PRIVATE));
            outputStreamWriter.write(data);
            outputStreamWriter.close();
        }
        catch (IOException e) {
            Log.e("PROFILE VIEW MODEL", "Could not write on file: " + e.toString());
        }
    }

    // Delete files to the root directory
    public void deleteFile(String fileName, Context context) {
        try {
            Log.i("PROFILE VIEW MODEL", "Trying to delete: " + fileName);
            File dir = context.getFilesDir();
            File file = new File(dir, fileName + ".json");
            boolean deleted = file.delete();
        }
        catch (Exception ex) {
            Log.e("PROFILE VIEW MODEL", "Could not delete file: " + ex.toString());
        }
    }

    public String readFromFile(String fileName, Context context) {

        String ret = "";

        try {
            InputStream inputStream = context.openFileInput(fileName);


            if ( inputStream != null ) {
                InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
                BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
                String receiveString = "";
                StringBuilder stringBuilder = new StringBuilder();

                while ( (receiveString = bufferedReader.readLine()) != null ) {
                    stringBuilder.append("\n").append(receiveString);
                }

                inputStream.close();
                ret = stringBuilder.toString();
            }
        }
        catch (FileNotFoundException e) {
            Log.e("PROFILE VIEW MODEL", "File not found: " + e.toString());
        } catch (IOException e) {
            Log.e("PROFILE VIEW MODEL", "Cannot read file: " + e.toString());
        }

        return ret;
    }

    public ArrayList<String> filesInTheDirectory(Context context){

        String[] files = context.fileList();
        ArrayList<String> filesArray = new ArrayList<>();
        for (String file : files)
        {
            filesArray.add(file);
            Log.d("Files", "FileName:" + file);
        }
        return filesArray;
    }




}
