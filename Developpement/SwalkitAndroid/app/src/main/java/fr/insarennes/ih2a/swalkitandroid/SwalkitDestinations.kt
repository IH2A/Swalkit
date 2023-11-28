package fr.insarennes.ih2a.swalkitandroid

import androidx.compose.runtime.Composable
import fr.insarennes.ih2a.swalkitandroid.ui.composables.Motors
import fr.insarennes.ih2a.swalkitandroid.ui.composables.Profiles
import fr.insarennes.ih2a.swalkitandroid.ui.composables.Swalkit

interface IDestination {
//    val icon:ImageVector
    val route:String
    val textId:Int
    val screen:@Composable (SwalkitViewModel) -> Unit
}

object ProfilesDestination : IDestination {
    override val route = "profiles"
    override val textId = R.string.profiles_screen_title
    override val screen: @Composable (vm:SwalkitViewModel) -> Unit =  { Profiles.ProfilesScreen(it) }
}

object MotorsDestination : IDestination {
    override val route = "motors"
    override val textId = R.string.motors_screen_title
    override val screen: @Composable (vm:SwalkitViewModel) -> Unit = { Motors.MotorsScreen(it) }
}

object SwalkitDestination : IDestination {
    override val route = "swalkit"
    override val textId = R.string.swalkit_screen_title
    override val screen: @Composable (vm:SwalkitViewModel) -> Unit =  { Swalkit.SwalkitScreen(it) }
}

val bottomBarDestinations = listOf(ProfilesDestination, MotorsDestination, SwalkitDestination)