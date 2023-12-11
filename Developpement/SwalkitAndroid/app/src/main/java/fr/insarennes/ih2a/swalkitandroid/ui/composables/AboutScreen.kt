package fr.insarennes.ih2a.swalkitandroid.ui.composables

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import fr.insarennes.ih2a.swalkitandroid.R

object About {
    @Composable
    fun AboutScreen() {
        Column(modifier = Modifier.padding(16.dp).fillMaxSize()) {
            val imageModifier = Modifier.size(160.dp).align(Alignment.CenterHorizontally).weight(1.0f)
            Text(text = stringResource(id = R.string.swalkit_screen_title), modifier = Modifier.align(Alignment.CenterHorizontally))
            Image(painter = painterResource(id = R.drawable.dornell),
                contentDescription = stringResource(id = R.string.logo_dornell),
                modifier = imageModifier)
            Image(painter = painterResource(id = R.drawable.ih2a),
                contentDescription = stringResource(id = R.string.logo_ih2a),
                modifier = imageModifier)
            Image(painter = painterResource(id = R.drawable.inria),
                contentDescription = stringResource(id = R.string.logo_inria),
                modifier = imageModifier)
            Image(painter = painterResource(id = R.drawable.insa),
                contentDescription = stringResource(id = R.string.logo_insa),
                modifier = imageModifier)
            Image(painter = painterResource(id = R.drawable.sopra),
                contentDescription = stringResource(id = R.string.logo_sopra),
                modifier = imageModifier)
        }
    }
}