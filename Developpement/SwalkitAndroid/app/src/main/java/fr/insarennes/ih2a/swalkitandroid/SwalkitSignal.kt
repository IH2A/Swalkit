package fr.insarennes.ih2a.swalkitandroid

import fr.insarennes.ih2a.swalkitandroid.data.DataSignal

data class SwalkitSignal(var intensity:Int, var pulse:Int, var distance:Int) {
    companion object {
        fun intensityString(value:Int) = "${value}%"
        fun pulseString(value:Int) = if (value > 0) "${value}ms" else "OFF"
        fun distanceString(value:Int) = "${value}cm"
    }

    fun intensityString() = Companion.intensityString(intensity)
    fun pulseString() = Companion.pulseString(pulse)
    fun distanceString() = Companion.distanceString(distance)

    override fun toString(): String {
        return "${intensityString()}, ${pulseString()}, ${distanceString()}"
    }

    fun toDataSignal() : DataSignal = DataSignal(intensity, pulse, distance)
    fun fromDataSignal(dataSignal:DataSignal) {
        intensity = dataSignal.intensity
        pulse = dataSignal.pulse
        distance = dataSignal.distance
    }
}
