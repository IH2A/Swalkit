package fr.insarennes.ih2a.swalkitandroid

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.BottomAppBar
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.lifecycleScope
import androidx.lifecycle.repeatOnLifecycle
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import fr.insarennes.ih2a.swalkitandroid.data.SwalkitDatabase
import fr.insarennes.ih2a.swalkitandroid.ui.composables.BlueToothStatus
import fr.insarennes.ih2a.swalkitandroid.ui.theme.SwalkitAndroidTheme
import kotlinx.coroutines.launch


const val LOG_TAG = "SWALKIT_APP"
class MainActivity : ComponentActivity() {
    private lateinit var swBluetooth:SWBluetoothLE

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if (!this::swBluetooth.isInitialized) {
            swBluetooth = SWBluetoothLE(this)
        }
        val viewModel:SwalkitViewModel = SwalkitViewModel(SwalkitDatabase.getInstance(this))
        lifecycleScope.launch { repeatOnLifecycle(Lifecycle.State.STARTED) {
            viewModel.currentProfile.collect {

            }
        } }
        lifecycleScope.launch { repeatOnLifecycle(Lifecycle.State.STARTED) {
            viewModel.sensorValues.collect {

            }
        } }
        setContent {
            MainComposable()
        }
    }

    override fun onStart() {
        super.onStart()
        swBluetooth.start()
    }

    override fun onStop() {
        swBluetooth.stop()
        super.onStop()
    }

    @OptIn(ExperimentalMaterial3Api::class)
    @Composable
    fun MainComposable() {
        SwalkitAndroidTheme {
            val navController = rememberNavController()
            Scaffold(
                topBar = { BlueToothStatus() },
                bottomBar = {
                    BottomAppBar(
                        actions = {
                            bottomBarDestinations.forEach{
                                Button(modifier = Modifier.weight(1f), onClick = {
                                    navController.navigate(it.route) { launchSingleTop = true }
                                }) {
                                    Text(text = getString(it.textId))
                                }
                            }
                        }
                    )
                }
            ) { innerPadding ->
                NavHost(navController = navController, startDestination = ProfilesDestination.route, modifier = Modifier.padding(innerPadding)) {
                    composable(route = ProfilesDestination.route) {
                        ProfilesDestination.screen(viewModel())
                    }
                    composable(route = MotorsDestination.route) {
                        MotorsDestination.screen(viewModel())
                    }
                    composable(route = SensorsDestination.route) {
                        SensorsDestination.screen(viewModel())
                    }
                }
            }
        }
    }
}
