package fr.insarennes.ih2a.swalkitandroid

import android.Manifest
import android.app.Activity
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Build
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.contract.ActivityResultContracts
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
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.AnnotatedString
import androidx.core.content.ContextCompat

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun BlueToothStatus() {
    val context = LocalContext.current

    // Bluetooth status
    val bluetoothManager: BluetoothManager = LocalContext.current.getSystemService(BluetoothManager::class.java)
    var bluetoothEnabled: Boolean by remember { mutableStateOf(bluetoothManager.adapter.isEnabled) }
    val enableBluetoothLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.StartActivityForResult(),
        onResult = { result -> bluetoothEnabled = result.resultCode == Activity.RESULT_OK
        })
    // register receiver for bluetooth being enabled/disabled
    DisposableEffect(context) {
        val broadcastReceiver = object:BroadcastReceiver() {
            override fun onReceive(p0: Context?, p1: Intent?) {
                when (p1?.extras?.getInt(BluetoothAdapter.EXTRA_STATE)) {
                    BluetoothAdapter.STATE_OFF -> bluetoothEnabled = false
                    BluetoothAdapter.STATE_ON -> bluetoothEnabled = true
                }
            }
        }
        context.registerReceiver(
            broadcastReceiver,
            IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED)
        )
        onDispose { context.unregisterReceiver(broadcastReceiver)}
    }

    // Bluetooth Permission
    val bluetoothPermission: String = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) Manifest.permission.BLUETOOTH_CONNECT else Manifest.permission.BLUETOOTH
    var bluetoothPermissionGranted by remember {
        mutableStateOf( ContextCompat.checkSelfPermission( context, bluetoothPermission ) == PackageManager.PERMISSION_GRANTED )
    }
    val requestPermissionLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.RequestPermission(),
        onResult = { result ->
            bluetoothPermissionGranted = result
        })

    // Selected device
    var deviceMenuExpanded by remember { mutableStateOf(false) }
    var selectedDeviceName: String by remember { mutableStateOf("") }
    var selectedDeviceMAC: String by remember { mutableStateOf("") }


    // UI
    var title: String
    var iconId: Int
    var action: () -> Unit = {}

    // Bluetooth
    if (bluetoothManager.adapter == null) {
        title = stringResource(id = R.string.no_bluetooth_adapter)
        iconId = R.drawable.baseline_bluetooth_disabled_24
    } else if (!bluetoothEnabled) {
        title = stringResource(id = R.string.bluetooth_disabled)
        iconId = R.drawable.baseline_bluetooth_disabled_24
        action = { enableBluetoothLauncher.launch(Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)) }
    } else if (!bluetoothPermissionGranted) {
        title = stringResource(id = R.string.permission_required)
        iconId = R.drawable.baseline_bluetooth_disabled_24
        action = { requestPermissionLauncher.launch(bluetoothPermission) }
    } else if (selectedDeviceName.isEmpty()) {
        title = stringResource(id = R.string.no_device_selected)
        iconId = R.drawable.baseline_bluetooth_24
        action = { deviceMenuExpanded = !deviceMenuExpanded }
    } else {
        title = selectedDeviceName
        iconId = R.drawable.baseline_perm_device_information_24
    }

    CenterAlignedTopAppBar(title = { ClickableText(text = AnnotatedString(title), onClick = { _ -> action()}) },
        actions = {
            IconButton(onClick = action) {
                Icon(
                    imageVector = ImageVector.vectorResource(iconId),
                    contentDescription = title
                )
            }
            if (bluetoothEnabled && bluetoothPermissionGranted) {
                DropdownMenu(
                    expanded = deviceMenuExpanded,
                    onDismissRequest = { deviceMenuExpanded = false }) {
                    bluetoothManager.adapter.bondedDevices.forEach {
                        DropdownMenuItem(
                            text = { Text(it.name) },
                            onClick = {
                                selectedDeviceMAC = it.address
                                selectedDeviceName = it.name
                                deviceMenuExpanded = false
                            },
                            contentPadding = ExposedDropdownMenuDefaults.ItemContentPadding
                        )
                    }
                }
            }
        }
    )
}
