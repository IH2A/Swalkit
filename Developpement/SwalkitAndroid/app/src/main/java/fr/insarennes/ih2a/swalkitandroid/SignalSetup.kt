package fr.insarennes.ih2a.swalkitandroid

data class SignalSetup(var motors:SwalkitSignals, var frequency:Int, var pulse:Int, var distance:Int) {
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
        fun frequencyString(value:Int) = "${value}hz"
        fun pulseString(value:Int) = if (value > 0) "${value}ms" else "OFF"
        fun distanceString(value:Int) = "${value}cm"

        val default: List<SignalSetup> = listOf(
            SignalSetup(SwalkitSignals.FRONT, 55, 100, 20),
            SignalSetup(SwalkitSignals.DANGER, 45, 200, 30),
            SignalSetup(SwalkitSignals.NEAR, 55, 0, 40),
            SignalSetup(SwalkitSignals.FAR, 90, 0, 50),
        )
    }

    fun frequencyString() = Companion.frequencyString(frequency)
    fun pulseString() = Companion.pulseString(pulse)
    fun distanceString() = Companion.distanceString(distance)

    override fun toString(): String {
        return "${frequencyString()}, ${pulseString()}, ${distanceString()}"
    }
}
