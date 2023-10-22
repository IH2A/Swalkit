package fr.insarennes.ih2a.swalkitandroid.ui.composables

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import fr.insarennes.ih2a.swalkitandroid.SwalkitProfile
import fr.insarennes.ih2a.swalkitandroid.SwalkitViewModel

object Profiles {
    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun ProfilesScreen(viewModel: SwalkitViewModel) {
        Column(modifier = Modifier.fillMaxSize()) {
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
    }
}