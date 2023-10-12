package fr.insarennes.ih2a.swalkitandroid

import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Build
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.core.content.ContextCompat
import androidx.lifecycle.lifecycleScope
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import java.io.IOException
import java.util.UUID

enum class BluetoothClassicStatus {
    UNKNOWN,
    NO_BLUETOOTH,
    PERMISSION_REQUIRED,
    BLUETOOTH_DISABLED,
    NO_DEVICE_SELECTED,
    NOT_CONNECTED_TO_DEVICE,
    CONNECTED_TO_DEVICE
}

/* TODO :
check whether lifecyclescope is appropriate or should be replaced by a custom one
make sure we properly handle application suspend/resume or start/stop
 */

@SuppressLint("MissingPermission")
class SWBluetoothClassic(protected val activity: ComponentActivity) : BroadcastReceiver() {
    var status: MutableState<BluetoothStatus> = mutableStateOf(BluetoothStatus.UNKNOWN)

    // UUID for bluetooth serial port profile
    private val sppUUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")

    private val bluetoothManager: BluetoothManager = activity.getSystemService(BluetoothManager::class.java)
    private val bluetoothAdapter: BluetoothAdapter? = bluetoothManager.adapter
    private var bluetoothDevice: BluetoothDevice? = null
    private var bluetoothSocket: BluetoothSocket? = null

    private var bluetoothEnabled: Boolean = bluetoothAdapter?.isEnabled ?: false
    private val bluetoothPermission = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
        android.Manifest.permission.BLUETOOTH_CONNECT
    } else {
        android.Manifest.permission.BLUETOOTH
    }
    private var bluetoothPermissionGranted = ContextCompat.checkSelfPermission( activity, bluetoothPermission ) == PackageManager.PERMISSION_GRANTED
    private var bluetoothPermissionsAsked = false
    private val requestPermissionLauncher: ActivityResultLauncher<String>

    init {
        updateStatus()
        // register launcher for permission request
        requestPermissionLauncher = activity.registerForActivityResult(ActivityResultContracts.RequestPermission()) { isGranted ->
            bluetoothPermissionsAsked = true
            bluetoothPermissionGranted = isGranted
            if (isGranted) {
                enableBluetooth()
            }
            updateStatus()
        }
    }

    fun start() {
        // register receiver for bluetooth being enabled/disabled
        activity.registerReceiver(this, IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED))
        Log.e("SWAPP", "bluetooth state registered")
    }
    fun stop() {
        Log.e("SWAPP", "bluetooth state unregistered")
        activity.unregisterReceiver(this)
    }

    fun enableBluetooth() {
        if (!bluetoothEnabled) {
            when (ContextCompat.checkSelfPermission(activity, bluetoothPermission)) {
                PackageManager.PERMISSION_GRANTED -> activity.startActivity(Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE))
                PackageManager.PERMISSION_DENIED -> if (!bluetoothPermissionsAsked) {
                    requestPermissionLauncher.launch(bluetoothPermission)
                }
            }
        }
    }

    fun getBondedDevices(): MutableSet<BluetoothDevice>? {
        return bluetoothAdapter?.bondedDevices
    }

    fun connectToDevice(deviceMac: String) {
        if (deviceMac.isNotEmpty()) {
            bluetoothDevice = bluetoothAdapter?.getRemoteDevice(deviceMac)
            activity.lifecycleScope.launch(Dispatchers.IO) { connect() }
        }
        updateStatus()
    }

    override fun onReceive(p0: Context?, p1: Intent?) {
        when(p1?.action) {
            BluetoothAdapter.ACTION_STATE_CHANGED ->
                when (p1.extras?.getInt(BluetoothAdapter.EXTRA_STATE)) {
                    BluetoothAdapter.STATE_OFF -> { bluetoothEnabled = false; updateStatus(); Log.e("SWAPP", "bluetooth disabled") }
                    BluetoothAdapter.STATE_ON -> { bluetoothEnabled = true; updateStatus(); Log.e("SWAPP", "bluetooth enabled") }
                }
        }
    }

    private fun updateStatus() {
        status.value = when {
            bluetoothAdapter == null -> BluetoothStatus.NO_BLUETOOTH
            !bluetoothPermissionGranted -> BluetoothStatus.PERMISSION_REQUIRED
            !bluetoothEnabled -> BluetoothStatus.BLUETOOTH_DISABLED
            bluetoothDevice == null -> BluetoothStatus.NO_DEVICE_SELECTED
            bluetoothSocket == null -> BluetoothStatus.NOT_CONNECTED_TO_DEVICE
            bluetoothSocket != null -> BluetoothStatus.CONNECTED_TO_DEVICE
            else -> BluetoothStatus.UNKNOWN
        }
    }

    private suspend fun connect() {
        if (bluetoothSocket != null)
            disconnect()
        bluetoothSocket = bluetoothDevice?.createRfcommSocketToServiceRecord(sppUUID)
        val socket = bluetoothSocket ?: return
        bluetoothAdapter?.cancelDiscovery()
        try {
            socket.connect()
            updateStatus()
            var numBytes:Int
            val buffer = ByteArray(256)
            while (bluetoothSocket?.isConnected == true) {
                numBytes = socket.inputStream.read(buffer)
                Log.e("SWALKIT_APP", String.format("read %d bytes : %s", numBytes, buffer.take(numBytes).joinToString { it.toInt().toChar().toString() }))
            }
        } catch (e: IOException) {
            Log.e("SWALKIT_APP", "Unable to connect bluetooth socket")
            disconnect()
        }
        updateStatus()
    }

    private suspend fun disconnect() {
        if (bluetoothSocket != null) {
            try {
                bluetoothSocket?.close()
            } catch (e: IOException) {
                Log.e("SWALKIT_APP", "Unable to close bluetooth socket")
            }
            bluetoothSocket = null
            updateStatus()
        }
    }
}

