package fr.insarennes.ih2a.swalkitandroid.data

import androidx.room.Dao
import androidx.room.Delete
import androidx.room.Insert
import androidx.room.OnConflictStrategy
import androidx.room.Query
import androidx.room.Upsert

@Dao
interface ProfileDAO {
    @Query("SELECT name FROM DataProfile")
    fun getProfiles(): List<String>

    @Query("SELECT * FROM DataProfile WHERE name LIKE :name")
    fun getProfile(name:String) : DataProfile

    @Upsert()
    suspend fun saveProfile(profile:DataProfile)

//    @Query("DELETE FROM DataProfile WHERE name = :name")
    @Delete
    suspend fun deleteProfile(name: String)

}
