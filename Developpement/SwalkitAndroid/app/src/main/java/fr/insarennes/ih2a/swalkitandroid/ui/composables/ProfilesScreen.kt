package fr.insarennes.ih2a.swalkitandroid.ui.composables

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.wrapContentWidth
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.modifier.modifierLocalConsumer
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import fr.insarennes.ih2a.swalkitandroid.R
import fr.insarennes.ih2a.swalkitandroid.SwalkitViewModel

object Profiles {
    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun ProfilesScreen(viewModel: SwalkitViewModel) {
        val currentProfileState by viewModel.currentProfile.collectAsStateWithLifecycle()
        val profilesList by viewModel.profilesList.collectAsStateWithLifecycle()
        val openAlertDialog = remember{mutableStateOf(false) }

        Column(modifier = Modifier
            .padding(16.dp)
            .fillMaxSize()) {
            Text(text = currentProfileState.toString())
            Button(onClick = { viewModel.saveCurrentProfile() }) {
                Text(text = String.format(stringResource(R.string.save_profile), currentProfileState.name))
            }
            LazyColumn {
                items(profilesList) { name ->
                    Row(modifier = Modifier.fillMaxWidth()) {
                        Text(text = name, modifier = Modifier.weight(1f))
                        Button(onClick = { viewModel.loadProfile(name) }, modifier = Modifier.wrapContentWidth()) {
                            Text(text = stringResource(id = R.string.load))
                        }
                        Button(onClick = { viewModel.deleteProfile(name) }, modifier = Modifier.wrapContentWidth()) {
                            Text(text = stringResource(id = R.string.delete))
                        }
                    }
                }
            }
            Button(onClick = { openAlertDialog.value = true }) {
                Text(text = stringResource(id = R.string.reset_configuration))
            }
        }

        when {
            openAlertDialog.value -> {
                ResetDialog(
                    title = stringResource(id = R.string.reset_configuration),
                    text = String.format(stringResource(R.string.reset_configuration_text), currentProfileState.name),
                    onDismiss = {
                        openAlertDialog.value = false
                    },
                    onConfirm = {
                        openAlertDialog.value = false
                        viewModel.resetProfile()
                    })
            }
        }
    }

    @Composable
    fun ResetDialog(title:String, text:String, onDismiss: () -> Unit, onConfirm: () -> Unit) : Unit {
        AlertDialog(
            onDismissRequest = {

            },
            title = { Text(text = title) },
            text = { Text(text = text) },
            confirmButton = {
            TextButton(onClick = onConfirm) {
                Text(text = stringResource(id = R.string.reset))
            }
        }, dismissButton = {
            TextButton(onClick = onDismiss) {
                Text(text = stringResource(id = R.string.cancel))
            }
        })
    }
}