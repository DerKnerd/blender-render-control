package main

import (
	"fmt"
	"log"
	"net/http"
	"os"

	"./blender"
	"./config"
	socketLog "./log"
	"./nextcloud"
	"./queue"
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
	} else {
		log.SetOutput(os.Stdout)
	}

	http.HandleFunc("/log", socketLog.HandleSocket)
	http.HandleFunc("/queue", queue.HandleHttp)
	http.HandleFunc("/blender", blender.HandleHttp)
	http.HandleFunc("/nextcloud", nextcloud.HandleHttp)

	log.Printf("Listening on address and port %s:%d", *config.GetFlags().Address, *config.GetFlags().Port)
	err := http.ListenAndServe(fmt.Sprintf("%s:%d", *config.GetFlags().Address, *config.GetFlags().Port), nil)

	if err != nil {
		log.Fatal(err.Error())
	}
}
