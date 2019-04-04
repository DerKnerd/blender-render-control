package blender

import (
	"net/http"
)

func Handle(w http.ResponseWriter, r *http.Request) {
	connection, err := GetClient().ConvertHttpToWs(w, r)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		w.Write([]byte(err.Error()))
		return
	}

	GetClient().AddConnection(connection)
	defer Listen(connection)
}
