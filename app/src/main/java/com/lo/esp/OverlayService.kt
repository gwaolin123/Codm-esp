package com.lo.esp

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.Service
import android.content.Context
import android.content.Intent
import android.graphics.PixelFormat
import android.os.Build
import android.os.IBinder
import android.view.Gravity
import android.view.WindowManager

class OverlayService : Service() {
    private lateinit var windowManager: WindowManager
    private lateinit var espView: ESPView

    override fun onCreate() {
        super.onCreate()
        windowManager = getSystemService(WINDOW_SERVICE) as WindowManager

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channel = NotificationChannel(
                "esp_channel",
                "CODM ESP",
                NotificationManager.IMPORTANCE_MIN
            )
            val nm = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            nm.createNotificationChannel(channel)

            val notification = Notification.Builder(this, "esp_channel")
                .setContentTitle("CODM ESP")
                .setContentText("ESP overlay active")
                .setSmallIcon(android.R.drawable.ic_menu_view)
                .build()

            startForeground(1, notification)
        }

        val params = WindowManager.LayoutParams(
            WindowManager.LayoutParams.MATCH_PARENT,
            WindowManager.LayoutParams.MATCH_PARENT,
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
                WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
            else
                WindowManager.LayoutParams.TYPE_PHONE,
            WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or
            WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE or
            WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS,
            PixelFormat.TRANSLUCENT
        ).apply {
            gravity = Gravity.TOP or Gravity.START
        }

        espView = ESPView(this)
        windowManager.addView(espView, params)

        val gamePid = findGameProcess("com.garena.game.codm")
        if (gamePid > 0) {
            NativeBridge.initESP(gamePid)
            espView.startRenderLoop()
        }
    }

    private fun findGameProcess(packageName: String): Int {
        return try {
            val runtime = Runtime.getRuntime()
            val process = runtime.exec("pidof $packageName")
            process.inputStream.bufferedReader().readLine()?.toIntOrNull() ?: -1
        } catch (e: Exception) {
            -1
        }
    }

    override fun onBind(intent: Intent?): IBinder? = null

    override fun onDestroy() {
        super.onDestroy()
        if (::espView.isInitialized) {
            windowManager.removeView(espView)
        }
    }
}
