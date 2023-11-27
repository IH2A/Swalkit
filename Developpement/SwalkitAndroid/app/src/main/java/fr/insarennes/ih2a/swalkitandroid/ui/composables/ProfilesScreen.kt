package fr.insarennes.ih2a.swalkitandroid.ui.composables

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import fr.insarennes.ih2a.swalkitandroid.SwalkitViewModel

object Profiles {
    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun ProfilesScreen(viewModel: SwalkitViewModel) {
        val currentProfileState by viewModel.currentProfile.collectAsStateWithLifecycle()
        val profilesList by viewModel.profilesList.collectAsStateWithLifecycle()

        Column(modifier = Modifier.fillMaxSize()) {
            Button(onClick = { viewModel.zeroProfile() }) {
                Text(text = "Zero configuration")
            }
            Text(text = currentProfileState.toString())
            Button(onClick = { viewModel.saveCurrentProfile() }) {
                Text(text = "Save current profile (${currentProfileState.name})")
            }
            LazyColumn {
                items(profilesList) { name ->
                    Row {
                        Text(text = name)
                        Button(onClick = { viewModel.loadProfile(name) }) {
                            Text(text = "Load")
                        }
                        Button(onClick = { viewModel.deleteProfile(name) }) {
                            Text(text = "Delete")
                        }
                    }
                }
            }
            Button(onClick = { viewModel.loadProfilesList() }) {
                Text(text = "Load profiles")
            }
        }
    }
/*
    TextField(value = request, onValueChange = {request = it}, label = { Text("Request")} )
    Button(onClick = { swBluetooth.writeCharacteristic(SwalkitCharacteristic.REQUEST, request) }) {
        Text(text = "Send request")
    }
    TextField(value = swBluetooth.configurationValue.value, onValueChange = {swBluetooth.configurationValue.value = it}, label = { Text("Configuration")} )
    Button(onClick = { swBluetooth.writeCharacteristic(SwalkitCharacteristic.CONFIGURATION, swBluetooth.configurationValue.value) }) {
        Text(text = "Send configuration")
    }
    Button(onClick = { swBluetooth.readCharacteristic(SwalkitCharacteristic.CONFIGURATION) }) {
        Text(text = "Receive configuration")
    }

    val c2 = SwalkitConfig()
    c2.name = "a"
    c2.frontSignal.distance = 9
    c2.fromByteArray(config.toByteArray())
    Text(text = config.toString())
    Text(text = c2.toString())

 */
}