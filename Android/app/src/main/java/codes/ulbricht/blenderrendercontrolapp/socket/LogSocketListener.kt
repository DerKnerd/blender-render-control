package codes.ulbricht.blenderrendercontrolapp.socket

import okhttp3.WebSocket
import okhttp3.WebSocketListener


class LogSocketListener(val onMessage: (text: String) -> Unit) : WebSocketListener() {

    override fun onMessage(webSocket: WebSocket, text: String) {
        onMessage(text)
    }

    override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
        webSocket.close(NORMAL_CLOSURE_STATUS, null)
    }

    companion object {
        private val NORMAL_CLOSURE_STATUS = 1000
    }
}