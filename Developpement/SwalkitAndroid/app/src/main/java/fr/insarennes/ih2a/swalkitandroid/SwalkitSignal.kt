package fr.insarennes.ih2a.swalkitandroid

import fr.insarennes.ih2a.swalkitandroid.data.DataSignal

data class SwalkitSignal(var motors:SwalkitSignals, var intensity:Int, var pulse:Int, var distance:Int) {
    enum class SwalkitSignals {
        FRONT,
        DANGER,
        NEAR,
        FAR,
    }

    companion object {
        fun signalString(signal:SwalkitSignals):Int = when(signal) {
            SwalkitSignals.FRONT -> R.string.motors_front
            SwalkitSignals.DANGER -> R.string.motors_danger
            SwalkitSignals.NEAR -> R.string.motors_near
            SwalkitSignals.FAR -> R.string.motors_far
        }
        fun intensityString(value:Int) = "${value}%"
        fun pulseString(value:Int) = if (value > 0) "${value}ms" else "OFF"
        fun distanceString(value:Int) = "${value}cm"

        val default: List<SwalkitSignal> = listOf(
            SwalkitSignal(SwalkitSignals.FRONT, 55, 100, 20),
            SwalkitSignal(SwalkitSignals.DANGER, 45, 200, 30),
            SwalkitSignal(SwalkitSignals.NEAR, 55, 0, 40),
            SwalkitSignal(SwalkitSignals.FAR, 90, 0, 50),
        )
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
