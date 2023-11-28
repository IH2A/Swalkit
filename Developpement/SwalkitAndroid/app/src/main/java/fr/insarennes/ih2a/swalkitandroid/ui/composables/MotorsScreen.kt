package fr.insarennes.ih2a.swalkitandroid.ui.composables

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
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
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import fr.insarennes.ih2a.swalkitandroid.R
import fr.insarennes.ih2a.swalkitandroid.SwalkitSignal
import fr.insarennes.ih2a.swalkitandroid.SwalkitViewModel

object Motors {
    private val intensityList: List<Int> =  (0..100 step 5).toList()
    private val intensityMap = intensityList.associateWith { SwalkitSignal.intensityString(it) }

    private val pulseFrequencyList: List<Int> = (0..500 step 100).toList()
    private val pulseFrequencyMap = pulseFrequencyList.associateWith { SwalkitSignal.pulseString(it) }

    private val distanceList: List<Int> = (0..115 step 5).toList()
    private val distanceMap = distanceList.associateWith { SwalkitSignal.distanceString(it) }

    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun MotorsScreen(viewModel: SwalkitViewModel) {
        val currentProfileState by viewModel.currentProfile.collectAsStateWithLifecycle()

        Column(Modifier.padding(8.dp)) {
            Row(modifier = Modifier.fillMaxWidth(1f)) {
                Text(text = stringResource(id = R.string.profile), modifier = Modifier.align(Alignment.CenterVertically))
                TextField(
                    modifier = Modifier.align(Alignment.CenterVertically).fillMaxWidth(1f),
                    value = currentProfileState.name,
                    onValueChange = {
                        viewModel.updateCurrentProfile(currentProfileState.copy(name = it))
                    })
            }
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
                    ResourceTextCell(R.string.motors_intensity, Modifier)
                    ComboCell(
                        currentProfileState.frontSignal.intensity,
                        intensityMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(frontSignal = currentProfileState.frontSignal.copy(intensity = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.dangerSignal.intensity,
                        intensityMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(dangerSignal = currentProfileState.dangerSignal.copy(intensity = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.nearSignal.intensity,
                        intensityMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(nearSignal = currentProfileState.nearSignal.copy(intensity = it) )
                        )
                    }
                    ComboCell(
                        currentProfileState.farSignal.intensity,
                        intensityMap,
                        Modifier.weight(1f)
                    ) {
                        viewModel.updateCurrentProfile(
                            currentProfileState.copy(farSignal = currentProfileState.farSignal.copy(intensity = it) )
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

