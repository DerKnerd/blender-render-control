package main

import (
	"log"
	"net/http"

	"./config"
	"./processing"
	"./types"
)

var (
	websocketClient = types.GetWebsocketClient()
)

func main() {
	config.GlobalConfig.Parse(*config.GetFlags().ConfigFile)

	http.HandleFunc("/blender-control", func(w http.ResponseWriter, r *http.Request) {
		connection, err := websocketClient.ConvertHttpToWs(r, w)
		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			w.Write([]byte(err.Error()))
			return
		}

		websocketClient.AddConnection(connection)
		defer processing.Listen(connection)
	})

	err := http.ListenAndServe(":1337", nil)
	if err != nil {
		log.Fatal(err.Error())
	}
}
