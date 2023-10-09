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
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import fr.insarennes.ih2a.swalkitandroid.ui.theme.SwalkitAndroidTheme


class MainActivity : ComponentActivity() {
    private lateinit var swBluetooth:SWBluetooth

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        swBluetooth = SWBluetooth(this)
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
                topBar = { BlueToothStatus(swBluetooth) },
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
                        ProfilesDestination.screen()
                    }
                    composable(route = MotorsDestination.route) {
                        MotorsDestination.screen()
                    }
                    composable(route = SensorsDestination.route) {
                        SensorsDestination.screen()
                    }
                }
            }
        }
    }
}