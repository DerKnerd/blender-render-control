package main

import (
	"log"
	"net/http"

	"./config"
	"./processing"
	"./types"
)

const (
	Render          = 0
	StopRender      = 1
	ForceStopRender = 2
	SyncFiles       = 3
	StopSyncFiles   = 4
	ListFiles       = 5
)

var (
	websocketClient = types.GetWebsocketClient()
	processQueue    = processing.ProcessQueue{}
)

func main() {
	config.GlobalConfig.Parse(*config.GetFlags().ConfigFile)

	http.HandleFunc("/blender-control", func(w http.ResponseWriter, r *http.Request) {
		connection := websocketClient.ConvertHttpToWs(r, w)
		blenderControl := types.BlenderControl{}

		for {
			err := connection.ReadDataToObject(blenderControl)
			connection.SendError("", err)

			switch {
			case blenderControl.Action == Render:
				processQueue.AddFiles(blenderControl.Files)
				go processQueue.Process()
			case blenderControl.Action == StopRender:
				go processQueue.StopProcessing(false)
			case blenderControl.Action == ForceStopRender:
				go processQueue.StopProcessing(true)
			case blenderControl.Action == SyncFiles:
				go processing.SyncNextcloudFiles()
			case blenderControl.Action == StopSyncFiles:
				go processing.StopSyncNextcloudFiles()
			}
		}
	})

	err := http.ListenAndServe(":1337", nil)
	if err != nil {
		log.Fatal(err.Error())
	}
}
