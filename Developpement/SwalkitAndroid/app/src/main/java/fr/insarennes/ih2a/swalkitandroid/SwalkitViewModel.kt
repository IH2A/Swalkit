package fr.insarennes.ih2a.swalkitandroid

import androidx.compose.runtime.currentCompositionLocalContext
import androidx.lifecycle.AndroidViewModel
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import fr.insarennes.ih2a.swalkitandroid.data.SwalkitDatabase
import fr.insarennes.ih2a.swalkitandroid.tools.runOnIOThread
import kotlinx.coroutines.currentCoroutineContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch

class SwalkitViewModel(val database: SwalkitDatabase) : ViewModel() {
    private val _currentProfile = MutableStateFlow(SwalkitProfile())
    val currentProfile = _currentProfile.asStateFlow()

    private val _sensorValues = MutableStateFlow(ByteArray(0))
    val sensorValues = _sensorValues.asStateFlow()

    private val _profilesList = MutableStateFlow<List<String>>(listOf())
    val profilesList = _profilesList.asStateFlow()

    fun readSensorValues() {
        SWBluetoothLE.getInstance()
            .readCharacteristic(SwalkitCharacteristic.SENSOR_VALUES) { data ->
                runOnIOThread {
                    _sensorValues.value = data
                }
            }
    }

    fun sendProfileToDevice(profile:SwalkitProfile) {
        SWBluetoothLE.getInstance().writeCharacteristic(SwalkitCharacteristic.PROFILE, profile.toByteArray())
    }

    fun receiveProfileFromDevice() {
        SWBluetoothLE.getInstance().readCharacteristic(SwalkitCharacteristic.PROFILE) { data ->
            _currentProfile.value.fromByteArray(data)
        }
    }

    fun zeroProfile() {
        _currentProfile.value.name = "-"
       _currentProfile.value.frontSignal.frequency = 0
       _currentProfile.value.frontSignal.pulse = 0
       _currentProfile.value.frontSignal.distance = 0
       _currentProfile.value.dangerSignal.frequency = 0
       _currentProfile.value.dangerSignal.pulse = 0
       _currentProfile.value.dangerSignal.distance = 0
       _currentProfile.value.nearSignal.frequency = 0
       _currentProfile.value.nearSignal.pulse = 0
       _currentProfile.value.nearSignal.distance = 0
       _currentProfile.value.farSignal.frequency = 0
       _currentProfile.value.farSignal.pulse = 0
       _currentProfile.value.farSignal.distance = 0
    }

    fun loadProfilesList() {
        viewModelScope.launch { database.profileDAO().getProfiles() }
    }

    fun saveCurrentProfile() {
        viewModelScope.launch { database.profileDAO().saveProfile(currentProfile.value.toDataProfile()) }
    }

    fun loadProfile(name:String) {
        viewModelScope.launch { database.profileDAO().getProfile(name) }
    }

    fun deleteProfile(name:String) {
        viewModelScope.launch { database.profileDAO().deleteProfile(name) }
    }
}