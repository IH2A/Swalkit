package fr.insarennes.ih2a.swalkitandroid.data

import android.content.Context
import androidx.room.Database
import androidx.room.Room
import androidx.room.RoomDatabase

@Database(entities = [DataProfile::class], version = 1)
abstract class SwalkitDatabase : RoomDatabase() {
    abstract fun profileDAO():ProfileDAO

    companion object {
        @Volatile
        private var instance: SwalkitDatabase? = null

        fun getInstance(context: Context): SwalkitDatabase = instance ?: synchronized(this) {
            instance ?: buildDatabase(context).also { instance = it }
        }

        private fun buildDatabase(context: Context) =
            Room.databaseBuilder(
                context.applicationContext,
                SwalkitDatabase::class.java,
                "Swalkit.db"
                /*
            ).addCallback(object : Callback() {
                    override fun onCreate(db: SupportSQLiteDatabase) {
                        super.onCreate(db)
                        runOnIOThread {
                            getInstance(context).profileDAO().saveProfile(SwalkitProfile().toDataProfile())
                        }
                    }
            }
                 */
            ).build()
    }
}