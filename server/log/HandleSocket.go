package log

import (
	"encoding/json"
	"net/http"

	"../socket"
)

func HandleSocket(w http.ResponseWriter, r *http.Request) {
	connection, err := socket.GetClient().ConvertHttpToWs(w, r)
	if err == nil {
		socket.GetClient().AddConnection(connection)
		select {}
	} else {
		w.WriteHeader(http.StatusInternalServerError)
		json.NewEncoder(w).Encode(err)
	}
}
