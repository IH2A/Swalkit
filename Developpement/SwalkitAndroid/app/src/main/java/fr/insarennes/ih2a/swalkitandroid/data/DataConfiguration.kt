package fr.insarennes.ih2a.swalkitandroid.data

import androidx.room.Entity
import androidx.room.PrimaryKey

@Entity
data class DataConfiguration(
    @PrimaryKey val name:String,
    var signalFront:DataSignal,
    var signalDanger:DataSignal,
    var signalNear:DataSignal,
    var signalFar: DataSignal
)
