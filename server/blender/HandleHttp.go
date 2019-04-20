package blender

import (
	"encoding/json"
	"net/http"

	"../queue"
)

func HandleHttp(w http.ResponseWriter, r *http.Request) {
	switch r.Method {
	case http.MethodGet:
		json.NewEncoder(w).Encode(queue.GetProcessQueue())
		w.WriteHeader(http.StatusOK)
		break
	case http.MethodPut:
		go queue.GetProcessQueue().Process()
		w.WriteHeader(http.StatusOK)
		break
	case http.MethodDelete:
		force := r.URL.Query().Get("force") == "true"
		go queue.GetProcessQueue().StopProcessing(force)
		w.WriteHeader(http.StatusOK)
		break
	default:
		w.WriteHeader(http.StatusMethodNotAllowed)
		break
	}
}
