package main

import (
	"fmt"
	"log"
	"net/http"
	"os"

	"./config"
	"./processing"
	"./types"
)

var (
	websocketClient = types.GetWebsocketClient()
)

func main() {
	config.GlobalConfig.Parse(*config.GetFlags().ConfigFile)
	if *config.GetFlags().LogFile != "" {
		logFile, err := os.OpenFile(*config.GetFlags().LogFile, os.O_RDWR|os.O_CREATE|os.O_APPEND, 0666)

		if err != nil {
			log.Fatal(err)
		}
		defer logFile.Close()

		log.SetOutput(logFile)
	}

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

	err := http.ListenAndServe(fmt.Sprintf("%s:%d", *config.GetFlags().Address, *config.GetFlags().Port), nil)
	log.Printf("Listening on address and port %s:%d", *config.GetFlags().Address, *config.GetFlags().Port)

	if err != nil {
		log.Fatal(err.Error())
	}
}
