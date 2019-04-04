package nextcloud

import (
	"github.com/gorilla/websocket"
)

const (
	SyncFilesBinary     = 0
	SyncFilesText       = 48 // Binary representation of the number 0
	StopSyncFilesBinary = 1
	StopSyncFilesText   = 49 // Binary representation of the number 1
)

func Listen(connection *Connection) {
	defer connection.Connection.Close()

	for {
		messageType, data, err := connection.Connection.ReadMessage()

		if err != nil {
			connection.Connection.Close()
			break
		}

		if messageType == websocket.BinaryMessage && data[0] == SyncFilesBinary ||
			messageType == websocket.TextMessage && data[0] == SyncFilesText {
			go SyncNextcloudFiles()
		} else if messageType == websocket.BinaryMessage && data[0] == StopSyncFilesBinary ||
			messageType == websocket.TextMessage && data[0] == StopSyncFilesText {
			go StopSyncNextcloudFiles()
		}
	}
}
