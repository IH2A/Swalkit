package fr.insarennes.ih2a.swalkitandroid.ui.composables

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import fr.insarennes.ih2a.swalkitandroid.R
import fr.insarennes.ih2a.swalkitandroid.SWBluetoothLE
import fr.insarennes.ih2a.swalkitandroid.SwalkitCharacteristic
import fr.insarennes.ih2a.swalkitandroid.SwalkitProfile
import fr.insarennes.ih2a.swalkitandroid.SwalkitViewModel
import java.util.BitSet

object Sensors {
    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun SensorsScreen(viewModel: SwalkitViewModel) {
        val swBluetooth = SWBluetoothLE.getInstance()
        val sensorValuesState by viewModel.sensorValues.collectAsState()
        val currentProfileState by viewModel.currentProfile.collectAsState()
        Column(modifier = Modifier.fillMaxSize()) {
            Text(text = stringResource(id = R.string.sensors_value))
            sensorValuesState.forEach { Text(text = it.toString()) }
            Button(onClick = { viewModel.readSensorValues() } ) {
                Text(text = stringResource(id = R.string.read_sensors_value))
            }

            Text(text = viewModel.currentProfile.toString())
            Button(onClick = { viewModel.sendProfileToDevice(currentProfileState) }) {
                Text(text = "Send configuration")
            }
            Button(onClick = { viewModel.receiveProfileFromDevice() }) {
                Text(text = "Receive configuration")
            }
            Button(onClick = { viewModel.zeroProfile() }) {
                Text(text = "Zero configuration")
            }
        }
    }
}