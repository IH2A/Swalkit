package fr.insarennes.ih2a.swalkitandroid.ui.composables

import android.widget.Toast
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.currentCompositionLocalContext
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import fr.insarennes.ih2a.swalkitandroid.R
import fr.insarennes.ih2a.swalkitandroid.SWBluetoothLE
import fr.insarennes.ih2a.swalkitandroid.SwalkitCharacteristic
import fr.insarennes.ih2a.swalkitandroid.SwalkitViewModel

object Swalkit {
    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun SwalkitScreen(viewModel: SwalkitViewModel) {
        val context = LocalContext.current
        val sensorValuesState by viewModel.sensorValues.collectAsStateWithLifecycle()
        val currentProfileState by viewModel.currentProfile.collectAsStateWithLifecycle()
        Column(modifier = Modifier.padding(16.dp).fillMaxSize()) {
            Button(onClick = {
                    if (viewModel.sendProfileToDevice(currentProfileState)) {
                        Toast.makeText(context, context.getString(R.string.profile_sent_to_device), Toast.LENGTH_SHORT).show()
                    } else {
                        Toast.makeText(context, context.getString(R.string.profile_send_error), Toast.LENGTH_LONG).show()
                    }
                }, modifier = Modifier.fillMaxWidth()) {
                Text(text = stringResource(id=R.string.send_configuration))
            }
            Button(onClick = {
                    if (!viewModel.receiveProfileFromDevice{
                            Toast.makeText(context, context.getString(R.string.profile_received_from_device), Toast.LENGTH_SHORT).show()
                        }) {
                        Toast.makeText(context, context.getString(R.string.profile_receive_error), Toast.LENGTH_LONG).show()
                    }
                }, modifier = Modifier.fillMaxWidth()) {
                Text(text = stringResource(id=R.string.receive_configuration))
            }

            Text(text = stringResource(id = R.string.sensors_value))
            sensorValuesState.forEach { Text(text = it.toString()) }
            Button(onClick = { viewModel.readSensorValues() }, modifier = Modifier.fillMaxWidth()) {
                Text(text = stringResource(id = R.string.read_sensors_value))
            }
        }
    }
}