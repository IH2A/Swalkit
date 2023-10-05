package fr.insarennes.ih2a.swalkitandroid

data class MotorsSetup(var motors:Int, var frequency:Int, var pulse:Int, var distance:Int) {
    companion object {
        fun frequencyString(value:Int) = "${value}hz"
        fun pulseString(value:Int) = if (value > 0) "${value}ms" else "OFF"
        fun distanceString(value:Int) = "${value}cm"

        val default: List<MotorsSetup> = listOf(
            MotorsSetup(R.string.motors_front, 55, 100, 20),
            MotorsSetup(R.string.motors_danger, 45, 200, 30),
            MotorsSetup(R.string.motors_near, 55, 0, 40),
            MotorsSetup(R.string.motors_far, 90, 0, 50),
        )
    }

    fun frequencyString() = Companion.frequencyString(frequency)
    fun pulseString() = Companion.pulseString(pulse)
    fun distanceString() = Companion.distanceString(distance)
}

