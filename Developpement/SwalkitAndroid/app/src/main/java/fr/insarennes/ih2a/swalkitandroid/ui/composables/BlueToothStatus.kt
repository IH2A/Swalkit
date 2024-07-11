package fr.insarennes.ih2a.swalkitandroid.ui.composables

import android.annotation.SuppressLint
import androidx.compose.foundation.text.ClickableText
import androidx.compose.material3.CenterAlignedTopAppBar
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExposedDropdownMenuDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.LocalContentColor
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.AnnotatedString
import androidx.compose.ui.text.TextStyle
import fr.insarennes.ih2a.swalkitandroid.BluetoothLEStatus
import fr.insarennes.ih2a.swalkitandroid.R
import fr.insarennes.ih2a.swalkitandroid.SWBluetoothLE

typealias BluetoothStatus = BluetoothLEStatus

@SuppressLint("MissingPermission")
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun BlueToothStatus() {
    // Use bluetooth status in State
    val swBluetooth = SWBluetoothLE.getInstance()
    val status by remember { swBluetooth.status }

    // Selected device
    var deviceMenuExpanded by remember { mutableStateOf(false) }
    var selectedDeviceName: String by remember { mutableStateOf("") }
    var selectedDeviceMAC: String by remember { mutableStateOf("") }

    // UI
    val title: String
    val iconId: Int
    var action: () -> Unit = {}

    // Bluetooth
    when(status) {
        BluetoothLEStatus.UNKNOWN -> {
            title = ""
            iconId = R.drawable.baseline_bluetooth_disabled_24
        }
        BluetoothLEStatus.NO_BLUETOOTH -> {
            title = stringResource(id = R.string.no_bluetooth_adapter)
            iconId = R.drawable.baseline_bluetooth_disabled_24
        }
        BluetoothLEStatus.PERMISSION_REQUIRED -> {
            title = stringResource(id = R.string.permission_required)
            iconId = R.drawable.baseline_bluetooth_disabled_24
            action = { swBluetooth.enableBluetooth() }
        }
        BluetoothLEStatus.LOCATION_REQUIRED -> {
            title = stringResource(id = R.string.location_disabled)
            iconId = R.drawable.baseline_bluetooth_disabled_24
        }
        BluetoothLEStatus.BLUETOOTH_DISABLED -> {
            title = stringResource(id = R.string.bluetooth_disabled)
            iconId = R.drawable.baseline_bluetooth_disabled_24
            action = { swBluetooth.enableBluetooth() }
        }
        BluetoothLEStatus.NO_DEVICE_SELECTED -> {
            title = stringResource(id = R.string.no_device_selected)
            iconId = R.drawable.baseline_bluetooth_24
            action = {
                swBluetooth.startScanningForDevices()
                deviceMenuExpanded = !deviceMenuExpanded
            }
        }
        BluetoothLEStatus.NOT_CONNECTED_TO_DEVICE -> {
            title = String.format(stringResource(R.string.device_not_found), selectedDeviceName)
            iconId = R.drawable.baseline_perm_device_information_24
            action = { deviceMenuExpanded = !deviceMenuExpanded }
        }
        BluetoothLEStatus.CONNECTED_TO_DEVICE -> {
            title = selectedDeviceName
            iconId = R.drawable.baseline_perm_device_information_24
        }
    }

    CenterAlignedTopAppBar(title = { ClickableText(text = AnnotatedString(title), onClick = { _ -> action()}, style = TextStyle.Default.copy(color = LocalContentColor.current)) },
        actions = {
            IconButton(onClick = action) {
                Icon(
                    imageVector = ImageVector.vectorResource(iconId),
                    contentDescription = title
                )
            }
            if (swBluetooth.bluetoothEnabled.value) {
                DropdownMenu(
                    expanded = deviceMenuExpanded,
                    onDismissRequest = { deviceMenuExpanded = false }) {
                    swBluetooth.bluetoothDevices.forEach {
                        DropdownMenuItem(
                            text = { Text(it.name) },
                            onClick = {
                                selectedDeviceMAC = it.device.address
                                selectedDeviceName = it.name
                                deviceMenuExpanded = false
                                swBluetooth.connectToDevice(it.device)
                            },
                            contentPadding = ExposedDropdownMenuDefaults.ItemContentPadding
                        )
                    }
                }
            }
        }
    )
}

