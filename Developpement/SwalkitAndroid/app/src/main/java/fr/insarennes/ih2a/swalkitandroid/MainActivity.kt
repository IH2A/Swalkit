package fr.insarennes.ih2a.swalkitandroid

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.BottomAppBar
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import fr.insarennes.ih2a.swalkitandroid.data.SwalkitDatabase
import fr.insarennes.ih2a.swalkitandroid.ui.composables.BlueToothStatus
import fr.insarennes.ih2a.swalkitandroid.ui.theme.SwalkitAndroidTheme


const val LOG_TAG = "SWALKIT_APP"
class MainActivity : ComponentActivity() {
    private lateinit var swBluetooth:SWBluetoothLE
    private lateinit var swViewModel:SwalkitViewModel

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if (!this::swBluetooth.isInitialized) {
            swBluetooth = SWBluetoothLE(this)
        }
        if (!this::swViewModel.isInitialized) {
            swViewModel = SwalkitViewModel(SwalkitDatabase.getInstance(this))
            swViewModel.loadProfilesList()
        }
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
                        ProfilesDestination.screen(swViewModel)
                    }
                    composable(route = MotorsDestination.route) {
                        MotorsDestination.screen(swViewModel)
                    }
                    composable(route = SwalkitDestination.route) {
                        SwalkitDestination.screen(swViewModel)
                    }
                }
            }
        }
    }
}
