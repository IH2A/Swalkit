package fr.insarennes.ih2a.swalkitandroid

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
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
import androidx.compose.ui.tooling.preview.Preview
import fr.insarennes.ih2a.swalkitandroid.ui.theme.SwalkitAndroidTheme

object Motors {
    private val frequencyList: List<Int> = mutableListOf(0) + (30..95 step 5).toList()
    private val frequencyMap = frequencyList.associateWith { SignalSetup.frequencyString(it) }

    private val pulseFrequencyList: List<Int> = (0..500 step 100).toList()
    private val pulseFrequencyMap = pulseFrequencyList.associateWith { SignalSetup.pulseString(it) }

    private val distanceList: List<Int> = (0..115 step 5).toList()
    private val distanceMap = distanceList.associateWith { SignalSetup.distanceString(it) }

    @Composable
    fun ResourceTextCell(id:Int, modifier:Modifier) {
        Text(
            modifier = modifier.wrapContentHeight(), text = stringResource(id = id)
        )
    }
    @Composable
    fun ComboCell(value:Int, values:Map<Int, String>, modifier:Modifier) {
        ComboText(
            value = value,
            values = values,
            modifier = modifier.wrapContentHeight()
        )
    }

    @Composable
    fun MotorsScreen(swalkitConfig: SwalkitConfig) {
        Row(modifier = Modifier.fillMaxSize()) {
            Column(
                modifier = Modifier
                    .weight(1f)
                    .wrapContentWidth()
            ) {
                ResourceTextCell(R.string.motors_motors, Modifier.weight(1f))
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
                ResourceTextCell(R.string.motors_frequency, Modifier.weight(1f))
                ComboCell(swalkitConfig.frontSignal.frequency, frequencyMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.dangerSignal.frequency, frequencyMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.nearSignal.frequency, frequencyMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.farSignal.frequency, frequencyMap, Modifier.weight(1f))
            }
            Column(
                modifier = Modifier
                    .weight(1f)
                    .wrapContentWidth()
            ) {
                ResourceTextCell(R.string.motors_pulse, Modifier.weight(1f))
                ComboCell(swalkitConfig.frontSignal.pulse, pulseFrequencyMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.dangerSignal.pulse, pulseFrequencyMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.nearSignal.pulse, pulseFrequencyMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.farSignal.pulse, pulseFrequencyMap, Modifier.weight(1f))
            }
            Column(
                modifier = Modifier
                    .weight(1f)
                    .wrapContentWidth()
            ) {
                ResourceTextCell(R.string.motors_distance, Modifier.weight(1f))
                ComboCell(swalkitConfig.frontSignal.distance, distanceMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.dangerSignal.distance, distanceMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.nearSignal.distance, distanceMap, Modifier.weight(1f))
                ComboCell(swalkitConfig.farSignal.distance, distanceMap, Modifier.weight(1f))
            }
        }
    }

    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun ComboText(value: Int, values: Map<Int, String>, modifier: Modifier) {
        var expanded by remember { mutableStateOf(false) }
        var selectedText by remember { mutableStateOf(values.getOrDefault(value, "")) }
        var selectedValue by remember { mutableIntStateOf(value) }

        ExposedDropdownMenuBox(
            modifier = modifier,
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

@Preview(showBackground = true)
@Composable
fun Preview() {
    SwalkitAndroidTheme {
        Motors.MotorsScreen(SwalkitConfig())
    }
}
