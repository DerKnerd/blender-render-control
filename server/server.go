package server

import (
	"../app"
	"../types"
	"github.com/gorilla/websocket"
	"log"
	"net/http"
	"os"
)

const (
	Render          = 0
	StopRender      = 1
	ForceStopRender = 2
	SyncFiles       = 3
	StopSyncFiles   = 4
)

var (
	connection    *websocket.Conn
	workingDir, _ = os.Getwd()
	processQueue  = ProcessQueue{}
	upgrader      = websocket.Upgrader{
		ReadBufferSize:  1024,
		WriteBufferSize: 1024,
	}
)

func sendResponse(response types.Response) {
	err := connection.WriteJSON(response)
	if err != nil {
		log.Printf(err.Error())
	}
}

func sendError(file string, err error) {
	if err != nil {
		sendResponse(types.Response{
			File:    file,
			Message: err.Error(),
		})
	}
}

func Serve() {
	GlobalConfig.Parse(*app.GetFlags().ConfigFile)

	http.HandleFunc("/blender-control", func(w http.ResponseWriter, r *http.Request) {
		conn, err := upgrader.Upgrade(w, r, nil)
		connection = conn
		if err != nil {
			log.Print("Upgrade:", err)
			return
		}

		blenderControl := types.BlenderControl{}

		defer connection.Close()
		for {
			err := conn.ReadJSON(&blenderControl)
			sendError("", err)

			switch {
			case blenderControl.Action == Render:
				processQueue.AddFiles(blenderControl.Files)
				go processQueue.Process()
			case blenderControl.Action == StopRender:
				go processQueue.StopProcessing(false)
			case blenderControl.Action == ForceStopRender:
				go processQueue.StopProcessing(true)
			case blenderControl.Action == SyncFiles:
				go SyncNextcloudFiles()
			case blenderControl.Action == StopSyncFiles:
				go StopSyncNextcloudFiles()
			}
		}
	})

	err := http.ListenAndServe(":1337", nil)
	if err != nil {
		log.Fatal(err.Error())
	}
}
