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
    private val frequencyMap = frequencyList.associateWith { MotorsSetup.frequencyString(it) }

    private val pulseFrequencyList: List<Int> = (0..500 step 100).toList()
    private val pulseFrequencyMap = pulseFrequencyList.associateWith { MotorsSetup.pulseString(it) }

    private val distanceList: List<Int> = (0..115 step 5).toList()
    private val distanceMap = distanceList.associateWith { MotorsSetup.distanceString(it) }

    @Composable
    fun MotorsScreen(motors: List<MotorsSetup>) {
        Row(modifier = Modifier.fillMaxSize()) {
            Column(
                modifier = Modifier
                    .weight(1f)
                    .wrapContentWidth()
            ) {
                Text(
                    modifier = Modifier
                        .weight(1f)
                        .wrapContentHeight(), text = stringResource(id = R.string.motors_motors)
                )
                for (m in motors) {
                    Text(
                        modifier = Modifier
                            .weight(1f)
                            .wrapContentHeight(), text = stringResource(id = m.motors)
                    )
                }
            }
            Column(
                modifier = Modifier
                    .weight(1f)
                    .wrapContentWidth()
            ) {
                Text(
                    modifier = Modifier
                        .weight(1f)
                        .wrapContentHeight(), text = stringResource(id = R.string.motors_frequency)
                )
                for (m in motors) {
                    ComboText(
                        value = m.frequency,
                        values = frequencyMap,
                        modifier = Modifier
                            .weight(1f)
                            .wrapContentHeight()
                    )
                }
            }
            Column(
                modifier = Modifier
                    .weight(1f)
                    .wrapContentWidth()
            ) {
                Text(
                    modifier = Modifier
                        .weight(1f)
                        .wrapContentHeight(), text = stringResource(id = R.string.motors_pulse)
                )
                for (m in motors) {
                    ComboText(
                        value = m.pulse,
                        values = pulseFrequencyMap,
                        modifier = Modifier
                            .weight(1f)
                            .wrapContentHeight()
                    )
                }
            }
            Column(
                modifier = Modifier
                    .weight(1f)
                    .wrapContentWidth()
            ) {
                Text(
                    modifier = Modifier
                        .weight(1f)
                        .wrapContentHeight(), text = stringResource(id = R.string.motors_distance)
                )
                for (m in motors) {
                    ComboText(
                        value = m.distance,
                        values = distanceMap,
                        modifier = Modifier
                            .weight(1f)
                            .wrapContentHeight()
                    )
                }
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
        Motors.MotorsScreen(MotorsSetup.default)
    }
}
