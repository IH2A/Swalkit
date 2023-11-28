package fr.insarennes.ih2a.swalkitandroid.data

import androidx.room.Embedded
import androidx.room.Entity
import androidx.room.PrimaryKey

data class DataSignal(
    val intensity:Int,
    val pulse:Int,
    val distance:Int
)

@Entity
data class DataProfile(
    @PrimaryKey val name:String,
    @Embedded(prefix = "front_") val signalFront:DataSignal,
    @Embedded(prefix = "danger_") val signalDanger:DataSignal,
    @Embedded(prefix = "near_") val signalNear:DataSignal,
    @Embedded(prefix = "far_") val signalFar: DataSignal
)
