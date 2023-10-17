package fr.insarennes.ih2a.swalkitandroid.data

import androidx.room.Entity

@Entity
data class DataSignal(
    var frequency:Int,
    var pulse:Int,
    var distance:Int
)
