package fr.insarennes.ih2a.swalkitandroid

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier

object Profiles {
    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun ProfilesScreen() {
        val swBluetooth = SWBluetoothLE.getInstance()
        var request by remember { mutableStateOf("0") }
        Column(modifier = Modifier.fillMaxSize()) {
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
        }
    }
}