package main

import (
	"fmt"
	"log"
	"net/http"
	"os"

	"./blender"
	"./config"
	"./nextcloud"
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

	http.HandleFunc("/list-files", blender.ListFiles)
	http.HandleFunc("/blender-control", blender.Handle)
	http.HandleFunc("/nextcloud-control", nextcloud.Handle)

	log.Printf("Listening on address and port %s:%d", *config.GetFlags().Address, *config.GetFlags().Port)
	err := http.ListenAndServe(fmt.Sprintf("%s:%d", *config.GetFlags().Address, *config.GetFlags().Port), nil)

	if err != nil {
		log.Fatal(err.Error())
	}
}
