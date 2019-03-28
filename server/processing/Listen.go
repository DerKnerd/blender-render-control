package processing

import (
	"../../types"
)

func Listen(connection *types.WebsocketConnection) {
	blenderControl := types.BlenderControl{}
	processQueue := GetProcessQueue()

	defer connection.Connection.Close()

	for {
		err := connection.ReadDataToObject(&blenderControl)
		if err != nil {
			connection.SendError("", err)
			continue
		}

		switch {
		case blenderControl.Action == types.Render:
			processQueue.AddFiles(blenderControl.Files)
			go processQueue.Process()
		case blenderControl.Action == types.StopRender:
			go processQueue.StopProcessing(false)
		case blenderControl.Action == types.ForceStopRender:
			go processQueue.StopProcessing(true)
		case blenderControl.Action == types.SyncFiles:
			go SyncNextcloudFiles()
		case blenderControl.Action == types.StopSyncFiles:
			go StopSyncNextcloudFiles()
		case blenderControl.Action == types.ListFiles:
			go ListFiles()
		}
	}
}
