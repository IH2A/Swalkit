package fr.insarennes.ih2a.swalkitandroid.data

import androidx.room.Embedded
import androidx.room.Entity
import androidx.room.PrimaryKey

data class DataSignal(
    val frequency:Int,
    val pulse:Int,
    val distance:Int
)

@Entity
data class DataProfile(
    @PrimaryKey val name:String,
    @Embedded val signalFront:DataSignal,
    @Embedded val signalDanger:DataSignal,
    @Embedded val signalNear:DataSignal,
    @Embedded val signalFar: DataSignal
)
