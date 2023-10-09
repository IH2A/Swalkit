package fr.insarennes.ih2a.swalkitandroid

import android.annotation.SuppressLint
import androidx.compose.foundation.text.ClickableText
import androidx.compose.material3.CenterAlignedTopAppBar
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExposedDropdownMenuDefaults
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
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
import java.text.Format

@SuppressLint("MissingPermission")
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun BlueToothStatus(swBluetooth: SWBluetooth) {
    // Use bluetooth status in State
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
        BluetoothStatus.UNKNOWN -> {
            title = ""
            iconId = R.drawable.baseline_bluetooth_disabled_24
        }
        BluetoothStatus.NO_BLUETOOTH -> {
            title = stringResource(id = R.string.no_bluetooth_adapter)
            iconId = R.drawable.baseline_bluetooth_disabled_24
        }
        BluetoothStatus.PERMISSION_REQUIRED -> {
            title = stringResource(id = R.string.permission_required)
            iconId = R.drawable.baseline_bluetooth_disabled_24
            action = { swBluetooth.enableBluetooth() }
        }
        BluetoothStatus.BLUETOOTH_DISABLED -> {
            title = stringResource(id = R.string.bluetooth_disabled)
            iconId = R.drawable.baseline_bluetooth_disabled_24
            action = { swBluetooth.enableBluetooth() }
        }
        BluetoothStatus.NO_DEVICE_SELECTED -> {
            title = stringResource(id = R.string.no_device_selected)
            iconId = R.drawable.baseline_bluetooth_24
            action = { deviceMenuExpanded = !deviceMenuExpanded }
        }
        BluetoothStatus.NOT_CONNECTED_TO_DEVICE -> {
            title = String.format(stringResource(R.string.device_not_found), selectedDeviceName)
            iconId = R.drawable.baseline_perm_device_information_24
            action = { deviceMenuExpanded = !deviceMenuExpanded }
        }
        BluetoothStatus.CONNECTED_TO_DEVICE -> {
            title = selectedDeviceName
            iconId = R.drawable.baseline_perm_device_information_24
        }
        else -> {
            title = ""
            iconId = R.drawable.baseline_bluetooth_disabled_24
        }
    }

    CenterAlignedTopAppBar(title = { ClickableText(text = AnnotatedString(title), onClick = { _ -> action()}) },
        actions = {
            IconButton(onClick = action) {
                Icon(
                    imageVector = ImageVector.vectorResource(iconId),
                    contentDescription = title
                )
            }
            val bondedDevices = swBluetooth.getBondedDevices()
            if (bondedDevices != null) {
                DropdownMenu(
                    expanded = deviceMenuExpanded,
                    onDismissRequest = { deviceMenuExpanded = false }) {
                    bondedDevices.forEach {
                        DropdownMenuItem(
                            text = { Text(it.name) },
                            onClick = {
                                selectedDeviceMAC = it.address
                                selectedDeviceName = it.name
                                deviceMenuExpanded = false
                                swBluetooth.connectToDevice(selectedDeviceMAC)
                            },
                            contentPadding = ExposedDropdownMenuDefaults.ItemContentPadding
                        )
                    }
                }
            }
        }
    )
}

