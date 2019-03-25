package main

import (
	"./app"
	"./server"
)

func main() {
	if *app.GetFlags().Mode == "server" {
		server.Serve()
	} else {
	}
}
