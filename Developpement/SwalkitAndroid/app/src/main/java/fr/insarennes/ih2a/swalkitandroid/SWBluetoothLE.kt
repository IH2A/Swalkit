package fr.insarennes.ih2a.swalkitandroid

import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattService
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothProfile
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanFilter
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.os.ParcelUuid
import android.util.Log
import android.widget.Toast
import androidx.activity.ComponentActivity
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.annotation.RequiresPermission
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.mutableStateOf
import androidx.core.content.ContextCompat
import java.util.UUID

enum class BluetoothLEStatus {
    UNKNOWN,
    NO_BLUETOOTH,
    PERMISSION_REQUIRED,
    BLUETOOTH_DISABLED,
    NO_DEVICE_SELECTED,
    NOT_CONNECTED_TO_DEVICE,
    CONNECTED_TO_DEVICE
}

enum class SwalkitCharacteristic {
    REQUEST,
    CONFIGURATION,
}

/* TODO :
check whether lifecyclescope is appropriate or should be replaced by a custom one
make sure we properly handle application suspend/resume or start/stop
check on android S+
 */

class SWBluetoothLE(protected val activity: ComponentActivity) : BroadcastReceiver() {
    companion object {
        private lateinit var instance: SWBluetoothLE
        public fun getInstance() = instance
    }

    private val SWALKITSERVICEUUID = UUID.fromString("07632d2a-6284-4cdf-82ee-f6a70b627c61")
    private val SWALKITREQUESTCHARACTERISTICUUID = UUID.fromString("c9f2218b-a76a-4643-b567-296dc58bffd7")
    private val SWALKITCONFIGURATIONCHARACTERISTICUUID = UUID.fromString("7027e0e3-e02a-4346-9d4f-826bf6db7772")

    private val bluetoothPermission = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
        android.Manifest.permission.BLUETOOTH_CONNECT
    } else {
        android.Manifest.permission.BLUETOOTH
    }
    private val SCANMAXDURATION: Long = 10000

    private val bluetoothManager: BluetoothManager = activity.getSystemService(BluetoothManager::class.java)
    private val bluetoothAdapter: BluetoothAdapter? = bluetoothManager.adapter
    private val bluetoothLEScanner = bluetoothAdapter?.bluetoothLeScanner
    private var bluetoothDevice: BluetoothDevice? = null
    private var bluetoothGatt: BluetoothGatt? = null
    private var bluetoothGattService: BluetoothGattService? = null

    private var scanningForDevices = false
    private val stopScanHandler = Handler(Looper.getMainLooper())
    private val updateNameHandler = Handler(Looper.getMainLooper())

    private val requestPermissionLauncher: ActivityResultLauncher<String>

    data class BLEDevice(val name:String, val device:BluetoothDevice)

    var bluetoothEnabled = mutableStateOf(bluetoothAdapter?.isEnabled ?: false)
    var bluetoothDevices = mutableStateListOf<BLEDevice>()
    var bluetoothConnected = mutableStateOf(false)
    var status: MutableState<BluetoothLEStatus> = mutableStateOf(BluetoothLEStatus.UNKNOWN)
    var requestValue = mutableStateOf("")
    var configurationValue = mutableStateOf("")

    init {
        instance = this
        if (!activity.packageManager.hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Log.e(LOG_TAG, activity.getString(R.string.no_bluetoothle_feature))
            Toast.makeText(activity, activity.getString(R.string.no_bluetoothle_feature), Toast.LENGTH_LONG).show()
        }
        // register launcher for permission request
        requestPermissionLauncher = activity.registerForActivityResult(ActivityResultContracts.RequestPermission()) { isGranted ->
            Log.i(LOG_TAG, if (isGranted) "Permission granted" else "Permission denied")
        }
        updateStatus()
    }

    fun start() {
        // register receiver for bluetooth being enabled/disabled
        activity.registerReceiver(this, IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED))
        Log.i(LOG_TAG, "bluetooth state registered")
    }
    fun stop() {
        Log.i(LOG_TAG, "bluetooth state unregistered")
        activity.unregisterReceiver(this)
    }

    override fun onReceive(p0: Context?, p1: Intent?) {
        when(p1?.action) {
            BluetoothAdapter.ACTION_STATE_CHANGED ->
                when (p1.extras?.getInt(BluetoothAdapter.EXTRA_STATE)) {
                    BluetoothAdapter.STATE_OFF -> {
                        Log.i(LOG_TAG, "Bluetooth disabled")
                        bluetoothEnabled.value = false;
                        updateStatus()
                    }
                    BluetoothAdapter.STATE_ON -> {
                        Log.i(LOG_TAG, "Bluetooth enabled")
                        bluetoothEnabled.value = true;
                        updateStatus()
                    }
                }
        }
    }


    fun enableBluetooth() {
        if (bluetoothAdapter?.isEnabled == false) {
            checkedBTConnectPermission { activity.startActivity(Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)) }
        }
    }

    fun connectToDevice(deviceMac: String) {
        bluetoothAdapter?.getRemoteDevice(deviceMac)?.let {
            connectToDevice(it)
        }
    }
    fun connectToDevice(device: BluetoothDevice) {
        if (bluetoothGatt != null)
            disconnectFromDevice()
        bluetoothDevice = device
        val gattCallback = object : BluetoothGattCallback() {
            override fun onConnectionStateChange(gatt: BluetoothGatt?, status: Int, newState: Int) {
                super.onConnectionStateChange(gatt, status, newState)
                when(newState) {
                    BluetoothProfile.STATE_CONNECTED -> {
                        bluetoothConnected.value = true;
                        checkedBTConnectPermission { gatt?.discoverServices() }
                        updateStatus()
                    }
                    BluetoothProfile.STATE_DISCONNECTED -> { bluetoothConnected.value = false; updateStatus() }
                }
            }

            override fun onServicesDiscovered(gatt: BluetoothGatt?, status: Int) {
                super.onServicesDiscovered(gatt, status)
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    Log.i(LOG_TAG, "BLE Services discovered")
                    bluetoothGattService = gatt?.getService(SWALKITSERVICEUUID)
                } else {
                    Log.i(LOG_TAG, "BLE Services discovery failed")
                }
            }

            override fun onCharacteristicRead(
                gatt: BluetoothGatt?,
                characteristic: BluetoothGattCharacteristic?,
                status: Int
            ) {
                super.onCharacteristicRead(gatt, characteristic, status)
                characteristic?.let { characteristicRead(characteristic, characteristic.value) }
            }

            override fun onCharacteristicRead(gatt: BluetoothGatt, characteristic: BluetoothGattCharacteristic, value: ByteArray, status: Int ) {
                super.onCharacteristicRead(gatt, characteristic, value, status)
                characteristicRead(characteristic, value)
            }

            private fun characteristicRead(characteristic:BluetoothGattCharacteristic, value:ByteArray) {
                when(characteristic.uuid) {
                    SWALKITREQUESTCHARACTERISTICUUID -> requestValue.value = value.toString(Charsets.UTF_8)
                    SWALKITCONFIGURATIONCHARACTERISTICUUID -> configurationValue.value = value.toString(Charsets.UTF_8)
                }
            }
        }
        checkedBTConnectPermission { bluetoothGatt = bluetoothDevice?.connectGatt(activity, false, gattCallback) }
        updateStatus()
    }

    fun disconnectFromDevice() {
        checkedBTConnectPermission {
            bluetoothGatt?.let {
                it.disconnect()
                bluetoothGatt = null
                updateStatus()
            }
            bluetoothDevice = null
        }
    }

    fun writeCharacteristic(characteristicId:SwalkitCharacteristic, value:String) {
        bluetoothGatt?.let { gatt ->
            bluetoothGattService?.getCharacteristic(getCharacteristicUUID(characteristicId))
                ?.let { characteristic ->
                    val byteArrayValue = value.toByteArray()
                    checkedBTConnectPermission {
                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                            gatt.writeCharacteristic(
                                characteristic,
                                byteArrayValue,
                                BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE
                            )
                        } else {
                            characteristic.value = byteArrayValue
                            gatt.writeCharacteristic(characteristic)
                        }
                    }
                }
        }
    }

    fun readCharacteristic(characteristicId: SwalkitCharacteristic) {
        bluetoothGatt?.let { gatt ->
            bluetoothGattService?.getCharacteristic(getCharacteristicUUID(characteristicId))
                ?.let { characteristic ->
                    checkedBTConnectPermission {
                        gatt.readCharacteristic(characteristic)
                    }
                }
        }
    }

    private fun getCharacteristicUUID(characteristic:SwalkitCharacteristic): UUID {
        return when(characteristic) {
            SwalkitCharacteristic.REQUEST -> SWALKITREQUESTCHARACTERISTICUUID
            SwalkitCharacteristic.CONFIGURATION -> SWALKITCONFIGURATIONCHARACTERISTICUUID
        }
    }

    private fun updateStatus() {
        status.value = when {
            bluetoothAdapter == null -> BluetoothLEStatus.NO_BLUETOOTH
            !bluetoothEnabled.value -> BluetoothLEStatus.BLUETOOTH_DISABLED
            bluetoothDevice == null -> BluetoothLEStatus.NO_DEVICE_SELECTED
            !bluetoothConnected.value -> BluetoothLEStatus.NOT_CONNECTED_TO_DEVICE
            bluetoothConnected.value -> BluetoothLEStatus.CONNECTED_TO_DEVICE
            else -> BluetoothLEStatus.UNKNOWN
        }
    }


    fun startScanningForDevices() {
        if (!scanningForDevices) {
            bluetoothDevices.clear()
            scanningForDevices = true
            if (checkPermission(android.Manifest.permission.BLUETOOTH_SCAN) && checkPermission(android.Manifest.permission.ACCESS_FINE_LOCATION, Build.VERSION_CODES.LOLLIPOP)) {
                val serviceFilter = ScanFilter.Builder().setServiceUuid(ParcelUuid(SWALKITSERVICEUUID)).build()
                val scanSettings = ScanSettings.Builder().build()
                bluetoothLEScanner?.startScan(listOf(serviceFilter), scanSettings, scanForDevicesCallback)
            }
            stopScanHandler.postDelayed({ stopScanningForDevices() }, SCANMAXDURATION)
        }
    }
    fun stopScanningForDevices() {
        if (scanningForDevices) {
            bluetoothLEScanner?.let {
                if (checkPermission(android.Manifest.permission.BLUETOOTH_SCAN)) {
                    bluetoothLEScanner.flushPendingScanResults(scanForDevicesCallback)
                    bluetoothLEScanner.stopScan(scanForDevicesCallback)
                }
            }
            scanningForDevices = false
        }
    }

    private val scanForDevicesCallback: ScanCallback = object : ScanCallback() {
        override fun onScanResult(callbackType: Int, result: ScanResult?) {
            super.onScanResult(callbackType, result)
            checkedBTConnectPermission {
                result?.let {
                    if (!bluetoothDevices.any { bleDevice -> bleDevice.device == result.device }) {
                        bluetoothDevices.add(
                            BLEDevice(
                                result.device.name ?: result.scanRecord?.deviceName
                                ?: result.device.address, result.device
                            )
                        )
                    }
                }
            }
        }
        override fun onScanFailed(errorCode: Int) {
            Log.e(LOG_TAG, String.format("Failed scanning BLE : %d", errorCode))
            super.onScanFailed(errorCode)
        }
    }

    private fun checkedBTConnectPermission(action: () -> Unit) {
        if (checkPermission(android.Manifest.permission.BLUETOOTH_CONNECT)) {
            action()
        }
    }
    private fun checkPermission(permission:String, fromVersion:Int = Build.VERSION_CODES.S): Boolean {
        if (Build.VERSION.SDK_INT >= fromVersion) {
            if (ContextCompat.checkSelfPermission(activity, permission) == PackageManager.PERMISSION_DENIED) {
                requestPermissionLauncher.launch(permission)
                return false
            }
        }
        return true
    }
}
