package fr.insarennes.ih2a.swalkitandroid

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import java.nio.ByteBuffer
import java.util.BitSet

object Sensors {
    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun SensorsScreen(config:SwalkitConfig) {
        val swBluetooth = SWBluetoothLE.getInstance()
        var request by remember { mutableIntStateOf(1234) }
        Column(modifier = Modifier.fillMaxSize()) {
            TextField(value = request.toString(), onValueChange = { try { request = it.toInt() } catch(_:Exception) {} }, label = { Text("Request")} )
            Button(onClick = {
                val ba:ByteArray = ByteArray(4)
                ba[0] = (request and 0xff).toByte()
                ba[1] = (request shr 8 and 0xff).toByte()
                ba[2] = (request shr 16 and 0xff).toByte()
                ba[3] = (request shr 24 and 0xff).toByte()
                swBluetooth.writeCharacteristic(SwalkitCharacteristic.REQUEST, ba)
            }) {
                Text(text = "Send request")
            }
            Button(onClick = { swBluetooth.readCharacteristic(SwalkitCharacteristic.REQUEST) { data ->
                when {
                    data[3] > 0x7f -> request = data[0].toInt() or (data[1].toInt() and 0xff) shl 8 or (data[2].toInt() and 0xff) shl 16 or (data[3].toInt() and 0xff) shl 24
                    else -> {
                        val bs:BitSet = BitSet.valueOf(data)
                        bs.flip(0, 32)
                        var index:Int = 0
                        do {
                            bs.flip(index)
                        } while (!bs[index++])
                        val bytes = bs.toByteArray()
                        request = data[0].toInt() or (data[1].toInt() and 0xff) shl 8 or (data[2].toInt() and 0xff) shl 16 or (data[3].toInt() and 0xff) shl 24
                    }
                }
            }
            }) {
                Text(text = "Receive request")
            }

            Text(text = config.toString())
            Button(onClick = { swBluetooth.writeCharacteristic(SwalkitCharacteristic.PROFILE, config.toByteArray()) }) {
                Text(text = "Send configuration")
            }
            Button(onClick = { swBluetooth.readCharacteristic(SwalkitCharacteristic.PROFILE) { data ->
                config.fromByteArray(
                    data
                )
            }
            }) {
                Text(text = "Receive configuration")
            }
            Button(onClick = {
                    config.name = "-"
                    config.frontSignal.frequency = 0
                    config.frontSignal.pulse = 0
                    config.frontSignal.distance = 0
                config.dangerSignal.frequency = 0
                config.dangerSignal.pulse = 0
                config.dangerSignal.distance = 0
                config.nearSignal.frequency = 0
                config.nearSignal.pulse = 0
                config.nearSignal.distance = 0
                config.farSignal.frequency = 0
                config.farSignal.pulse = 0
                config.farSignal.distance = 0
                }) {
                Text(text = "Zero configuration")
            }
        }
    }
}