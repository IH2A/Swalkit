package fr.insarennes.ih2a.swalkitandroid.tools

import java.util.concurrent.Executors

private val ioExecutor = Executors.newSingleThreadExecutor()

fun runOnIOThread(f : () -> Unit) {
    ioExecutor.execute(f)
}