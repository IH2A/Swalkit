package fr.insarennes.ih2a.swalkitandroid.data

import androidx.room.Dao
import androidx.room.Query
import androidx.room.Upsert
import kotlinx.coroutines.flow.Flow

@Dao
interface ProfileDAO {
    @Query("SELECT name FROM DataProfile")
    fun getProfiles(): Flow<List<String>>

    @Query("SELECT * FROM DataProfile WHERE name = :name")
    fun getProfile(name:String) : Flow<DataProfile>

    @Upsert()
    suspend fun saveProfile(profile:DataProfile)

    //    @Delete
    @Query("DELETE FROM DataProfile WHERE name = :name")
    suspend fun deleteProfile(name: String)

}
