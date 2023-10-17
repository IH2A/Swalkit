package fr.insarennes.ih2a.swalkitandroid

import androidx.compose.runtime.Composable

interface SwalkitDestination {
//    val icon:ImageVector
    val route:String
    val textId:Int
    val screen:@Composable (config:SwalkitConfig) -> Unit
}

object ProfilesDestination : SwalkitDestination {
    override val route = "profiles"
    override val textId = R.string.profiles_screen_title
    override val screen: @Composable (config:SwalkitConfig) -> Unit =  { Profiles.ProfilesScreen(it) }
}

object MotorsDestination : SwalkitDestination {
    override val route = "motors"
    override val textId = R.string.motors_screen_title
    override val screen: @Composable (config:SwalkitConfig) -> Unit = { Motors.MotorsScreen(it) }
}

object SensorsDestination : SwalkitDestination {
    override val route = "sensors"
    override val textId = R.string.sensors_screen_title
    override val screen: @Composable (config:SwalkitConfig) -> Unit =  { Sensors.SensorsScreen(it) }
}

val bottomBarDestinations = listOf(ProfilesDestination, MotorsDestination, SensorsDestination)