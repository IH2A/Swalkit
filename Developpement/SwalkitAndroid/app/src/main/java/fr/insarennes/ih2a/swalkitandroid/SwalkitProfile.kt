package fr.insarennes.ih2a.swalkitandroid

import fr.insarennes.ih2a.swalkitandroid.data.DataProfile


class SwalkitProfile() {
    var name:String = "default"
    var frontSignal:SwalkitSignal = SwalkitSignal(SwalkitSignal.SwalkitSignals.FRONT, 55, 100, 20)
    var dangerSignal:SwalkitSignal = SwalkitSignal(SwalkitSignal.SwalkitSignals.FRONT, 45, 200, 30)
    var nearSignal:SwalkitSignal = SwalkitSignal(SwalkitSignal.SwalkitSignals.FRONT, 55, 0, 40)
    var farSignal:SwalkitSignal = SwalkitSignal(SwalkitSignal.SwalkitSignals.FRONT, 90, 0, 50)

    override fun toString() : String {
        return "$name - Front : $frontSignal, Danger : $dangerSignal, Near : $nearSignal, Far : $farSignal"
    }

    fun copy(name:String = this.name,
             frontSignal:SwalkitSignal = this.frontSignal.copy(),
             dangerSignal:SwalkitSignal = this.dangerSignal.copy(),
             nearSignal:SwalkitSignal = this.nearSignal.copy(),
             farSignal:SwalkitSignal = this.farSignal.copy()
             ):SwalkitProfile {
        var result = SwalkitProfile()
        result.name = name
        result.frontSignal = frontSignal
        result.dangerSignal = dangerSignal
        result.nearSignal = nearSignal
        result.farSignal = farSignal
        return result
    }

    fun toDataProfile() : DataProfile = DataProfile(name, frontSignal.toDataSignal(), dangerSignal.toDataSignal(), nearSignal.toDataSignal(), farSignal.toDataSignal())
    fun fromDataProfile(dataProfile:DataProfile) {
        name = dataProfile.name
        frontSignal.fromDataSignal(dataProfile.signalFront)
        dangerSignal.fromDataSignal(dataProfile.signalDanger)
        nearSignal.fromDataSignal(dataProfile.signalNear)
        farSignal.fromDataSignal(dataProfile.signalFar)
    }

    fun toByteArray() : ByteArray {
        var nameArray = name.toByteArray(Charsets.UTF_8)
        var result = ByteArray(nameArray.size + 13)
        result[0] = nameArray.size.toByte()
        nameArray.copyInto(result, 1)
        var index:Int = nameArray.size + 1
        result[index++] = frontSignal.frequency.toByte()
        result[index++] = (frontSignal.pulse / 10).toByte()
        result[index++] = frontSignal.distance.toByte()
        result[index++] = dangerSignal.frequency.toByte()
        result[index++] = (dangerSignal.pulse / 10).toByte()
        result[index++] = dangerSignal.distance.toByte()
        result[index++] = nearSignal.frequency.toByte()
        result[index++] = (nearSignal.pulse / 10).toByte()
        result[index++] = nearSignal.distance.toByte()
        result[index++] = farSignal.frequency.toByte()
        result[index++] = (farSignal.pulse / 10).toByte()
        result[index++] = farSignal.distance.toByte()
        return result
    }

    fun fromByteArray(ba:ByteArray) {
        val nameLength = ba[0].toInt()
        name = ba.decodeToString(1, nameLength + 1)
        var index:Int = nameLength + 1
        frontSignal.frequency = ba[index++].toInt()
        frontSignal.pulse = ba[index++] * 10
        frontSignal.distance = ba[index++].toInt()
        dangerSignal.frequency = ba[index++].toInt()
        dangerSignal.pulse = ba[index++] * 10
        dangerSignal.distance = ba[index++].toInt()
        nearSignal.frequency = ba[index++].toInt()
        nearSignal.pulse = ba[index++] * 10
        nearSignal.distance = ba[index++].toInt()
        farSignal.frequency = ba[index++].toInt()
        farSignal.pulse = ba[index++] * 10
        farSignal.distance = ba[index++].toInt()
    }
}
