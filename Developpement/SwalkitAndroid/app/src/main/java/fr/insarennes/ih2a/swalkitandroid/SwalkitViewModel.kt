package fr.insarennes.ih2a.swalkitandroid

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import fr.insarennes.ih2a.swalkitandroid.data.SwalkitDatabase
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch

class SwalkitViewModel(private val database: SwalkitDatabase) : ViewModel() {
    private val _currentProfile = MutableStateFlow(SwalkitProfile())
    val currentProfile = _currentProfile.asStateFlow()

    private val _sensorValues = MutableStateFlow(ByteArray(0))
    val sensorValues = _sensorValues.asStateFlow()

    private val _profilesList = MutableStateFlow<List<String>>(listOf())
    val profilesList = _profilesList.asStateFlow()

    fun readSensorValues() {
        SWBluetoothLE.getInstance()
            .readCharacteristic(SwalkitCharacteristic.SENSOR_VALUES) { data ->
                _sensorValues.value = data
            }
    }

    fun sendProfileToDevice(profile:SwalkitProfile): Boolean {
        return SWBluetoothLE.getInstance().writeCharacteristic(SwalkitCharacteristic.PROFILE, profile.toByteArray())
    }

    fun receiveProfileFromDevice(callback: () -> Unit): Boolean {
        return SWBluetoothLE.getInstance().readCharacteristic(SwalkitCharacteristic.PROFILE) { data ->
            _currentProfile.value.fromByteArray(data)
            callback.invoke()
        }
    }

    fun zeroProfile() {
        val profile = SwalkitProfile()
        profile.name = "-"
        profile.frontSignal.intensity = 0
        profile.frontSignal.pulse = 0
        profile.frontSignal.distance = 0
        profile.dangerSignal.intensity = 0
        profile.dangerSignal.pulse = 0
        profile.dangerSignal.distance = 0
        profile.nearSignal.intensity = 0
        profile.nearSignal.pulse = 0
        profile.nearSignal.distance = 0
        profile.farSignal.intensity = 0
        profile.farSignal.pulse = 0
        profile.farSignal.distance = 0
        _currentProfile.value = profile
    }

    fun resetProfile() {
        val profile = SwalkitProfile()
        profile.name = _currentProfile.value.name
        _currentProfile.value = profile
    }

    fun updateCurrentProfile(profile:SwalkitProfile) {
        _currentProfile.value = profile
    }

    fun loadProfilesList() {
        viewModelScope.launch {
            _profilesList.value = database.profileDAO().getProfiles().first()
        }
    }

    fun saveCurrentProfile(callback:() -> Unit) {
        viewModelScope.launch {
            database.profileDAO().saveProfile(currentProfile.value.toDataProfile())
            loadProfilesList()
            callback.invoke()
        }
    }

    fun loadProfile(name:String) {
        viewModelScope.launch {
            val profile = SwalkitProfile()
            profile.fromDataProfile(database.profileDAO().getProfile(name).first())
            _currentProfile.value = profile
        }
    }

    fun deleteProfile(name:String) {
        viewModelScope.launch {
            database.profileDAO().deleteProfile(name)
            loadProfilesList()
        }
    }
}