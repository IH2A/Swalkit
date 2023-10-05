package fr.insarennes.ih2a.swalkitandroid

import androidx.compose.runtime.Composable
import androidx.compose.ui.res.stringResource

interface SwalkitDestination {
//    val icon:ImageVector
    val route:String
    val textId:Int
    val screen:@Composable () -> Unit
}

object ProfilesDestination : SwalkitDestination {
    override val route = "profiles"
    override val textId = R.string.profiles_screen_title
    override val screen: @Composable () -> Unit =  { Profiles.ProfilesScreen() }
}

object MotorsDestination : SwalkitDestination {
    override val route = "motors"
    override val textId = R.string.motors_screen_title
    override val screen: @Composable () -> Unit =  { Motors.MotorsScreen(MotorsSetup.default) }
}

object SensorsDestination : SwalkitDestination {
    override val route = "sensors"
    override val textId = R.string.sensors_screen_title
    override val screen: @Composable () -> Unit =  { Sensors.SensorsScreen() }
}

val bottomBarDestinations = listOf(ProfilesDestination, MotorsDestination, SensorsDestination)