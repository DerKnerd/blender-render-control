package codes.ulbricht.blenderrendercontrolapp

import android.app.*
import android.content.Context
import android.content.Intent
import android.os.IBinder
import codes.ulbricht.blenderrendercontrolapp.blender.BlenderActivity
import codes.ulbricht.blenderrendercontrolapp.socket.LogSocketListener
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.WebSocket
import org.jetbrains.anko.AnkoLogger
import org.jetbrains.anko.error
import org.jetbrains.anko.intentFor
import org.json.JSONObject
import java.io.File

class BlenderRenderProgressService : Service(), AnkoLogger {
    private lateinit var notificationManager: NotificationManager
    private var notificationId = 0

    override fun onCreate() {
        super.onCreate()
        val name = getString(R.string.blender_render_control_app_status_channel_name)
        val descriptionText = getString(R.string.blender_render_control_app_status_channel_description)
        val importance = NotificationManager.IMPORTANCE_DEFAULT
        val channel = NotificationChannel(CHANNEL_ID, name, importance).apply {
            description = descriptionText
        }
        // Register the channel with the system
        notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
        notificationManager.createNotificationChannel(channel)
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }

    private lateinit var logSocket: WebSocket

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        val preferences = getSharedPreferences(getString(R.string.preferences_file_key), Context.MODE_PRIVATE)

        val basePath = "ws://${preferences.getString(
            getString(R.string.preferences_server_host),
            "localhost"
        )}:${preferences.getString(
            getString(R.string.preferences_server_port),
            "1337"
        )}/log"

        val logSocketListener = LogSocketListener {
            val data = JSONObject(it)
            if (data.getString("message").toLowerCase() == "blender quit") {
                try {
                    if (data.has("file")) {
                        val file = data.getString("file")
                        val filenameWithoutExtension = File(file).nameWithoutExtension
                        val notification = Notification
                            .Builder(applicationContext, CHANNEL_ID)
                            .setContentTitle(getString(R.string.blender_render_progress_service_title))
                            .setContentText(
                                getString(R.string.blender_render_progress_service_text).format(
                                    filenameWithoutExtension
                                )
                            )
                            .setColorized(true)
                            .setColor(getColor(R.color.colorBlender))
                            .setSmallIcon(R.drawable.ic_small_icon)
                            .setContentIntent(
                                PendingIntent.getActivity(applicationContext, 0, intentFor<BlenderActivity>(), 0)
                            )
                            .build()

                        notificationManager.notify(
                            ++notificationId, notification
                        )
                    }
                } catch (e: Exception) {
                    error(e.localizedMessage, e)
                }
            }
        }
        val client = OkHttpClient()
        val request = Request.Builder().url(basePath).build()
        logSocket = client.newWebSocket(request, logSocketListener)

        return START_STICKY
    }

    companion object {
        const val CHANNEL_ID = "blender_render_control_app_status"
    }
}
