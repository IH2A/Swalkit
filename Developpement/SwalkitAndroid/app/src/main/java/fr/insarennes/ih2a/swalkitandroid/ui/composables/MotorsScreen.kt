package fr.insarennes.ih2a.swalkitandroid.ui.composables

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.wrapContentHeight
import androidx.compose.foundation.layout.wrapContentWidth
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.ExposedDropdownMenuBox
import androidx.compose.material3.ExposedDropdownMenuDefaults
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import fr.insarennes.ih2a.swalkitandroid.R
import fr.insarennes.ih2a.swalkitandroid.SwalkitSignal
import fr.insarennes.ih2a.swalkitandroid.SwalkitViewModel

object Motors {
    private val frequencyList: List<Int> = mutableListOf(0) + (30..95 step 5).toList()
    private val frequencyMap = frequencyList.associateWith { SwalkitSignal.frequencyString(it) }

    private val pulseFrequencyList: List<Int> = (0..500 step 100).toList()
    private val pulseFrequencyMap = pulseFrequencyList.associateWith { SwalkitSignal.pulseString(it) }

    private val distanceList: List<Int> = (0..115 step 5).toList()
    private val distanceMap = distanceList.associateWith { SwalkitSignal.distanceString(it) }

    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun MotorsScreen(viewModel: SwalkitViewModel) {
        val currentProfileState by viewModel.currentProfile.collectAsStateWithLifecycle()

        Column {
            TextField(
                modifier = Modifier.fillMaxWidth(1f),
                value = currentProfileState.name,
                onValueChange = {
                    viewModel.updateCurrentProfile(currentProfileState.copy(name = it))
                })
            Row(modifier = Modifier.fillMaxSize()) {
                Column(
                    modifier = Modifier
                        .weight(1f)
                        .wrapContentWidth()
                ) {
                    ResourceTextCell(R.string.motors_motors, Modifier)
                    ResourceTextCell(R.string.motors_front, Modifier.weight(1f))
                    ResourceTextCell(R.string.motors_danger, Modifier.weight(1f))
                    ResourceTextCell(R.string.motors_near, Modifier.weight(1f))
                    ResourceTextCell(R.string.motors_far, Modifier.weight(1f))
                }
                Column(
                    modifier = Modifier
                        .weight(1f)
                        .wrapContentWidth()
                ) {
                    ResourceTextCell(R.string.motors_frequency, Modifier)
                    ComboCell(
                        currentProfileState.frontSignal.frequency,
                        frequencyMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(frontSignal = currentProfileState.frontSignal.copy(frequency = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.dangerSignal.frequency,
                        frequencyMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(dangerSignal = currentProfileState.dangerSignal.copy(frequency = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.nearSignal.frequency,
                        frequencyMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(nearSignal = currentProfileState.nearSignal.copy(frequency = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.farSignal.frequency,
                        frequencyMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(farSignal = currentProfileState.farSignal.copy(frequency = it) )
                        )
                    }
                }
                Column(
                    modifier = Modifier
                        .weight(1f)
                        .wrapContentWidth()
                ) {
                    ResourceTextCell(R.string.motors_pulse, Modifier)
                    ComboCell(
                        currentProfileState.frontSignal.pulse,
                        pulseFrequencyMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(frontSignal = currentProfileState.frontSignal.copy(pulse = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.dangerSignal.pulse,
                        pulseFrequencyMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(dangerSignal = currentProfileState.dangerSignal.copy(pulse = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.nearSignal.pulse,
                        pulseFrequencyMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(nearSignal = currentProfileState.nearSignal.copy(pulse = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.farSignal.pulse,
                        pulseFrequencyMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(farSignal = currentProfileState.farSignal.copy(pulse = it) )
                        )
                    }
                }
                Column(
                    modifier = Modifier
                        .weight(1f)
                        .wrapContentWidth()
                ) {
                    ResourceTextCell(R.string.motors_distance, Modifier)
                    ComboCell(
                        currentProfileState.frontSignal.distance,
                        distanceMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(frontSignal = currentProfileState.frontSignal.copy(distance = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.dangerSignal.distance,
                        distanceMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(dangerSignal = currentProfileState.dangerSignal.copy(distance = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.nearSignal.distance,
                        distanceMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(nearSignal = currentProfileState.nearSignal.copy(distance = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.farSignal.distance,
                        distanceMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(farSignal = currentProfileState.farSignal.copy(distance = it) )
                        )
                    }
                }
            }
        }
    }

    @Composable
    fun ResourceTextCell(id:Int, modifier:Modifier) {
        Text(
            modifier = modifier.wrapContentHeight(), text = stringResource(id = id)
        )
    }

    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun ComboCell(value: Int, values: Map<Int, String>, modifier: Modifier, valueChanged:(Int) -> Unit) {
        var expanded by remember { mutableStateOf(false) }
        var selectedText by remember { mutableStateOf(values.getOrDefault(value, "")) }
        var selectedValue by remember { mutableIntStateOf(value) }

        ExposedDropdownMenuBox(
            modifier = modifier.wrapContentHeight(),
            expanded = expanded,
            onExpandedChange = { expanded = !expanded }) {
            TextField(
                modifier = Modifier.menuAnchor(),
                readOnly = true,
                value = selectedText,
                onValueChange = {},
                colors = ExposedDropdownMenuDefaults.textFieldColors()
            )
            ExposedDropdownMenu(expanded = expanded,
                onDismissRequest = { expanded = false }) {
                values.forEach {
                    DropdownMenuItem(
                        text = { Text(it.value) },
                        onClick = {
                            valueChanged(it.key)
                            selectedValue = it.key
                            selectedText = it.value
                            expanded = false
                        },
                        contentPadding = ExposedDropdownMenuDefaults.ItemContentPadding
                    )
                }
            }
        }
    }
}

