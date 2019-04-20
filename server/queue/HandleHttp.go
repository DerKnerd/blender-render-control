package queue

import (
	"encoding/json"
	"net/http"
)

func HandleHttp(w http.ResponseWriter, r *http.Request) {
	responseEncoder := json.NewEncoder(w)

	switch r.Method {
	case http.MethodGet:
		queue := GetProcessQueue()
		w.WriteHeader(http.StatusOK)
		responseEncoder.Encode(queue.Items)
		break
	case http.MethodPost:
		var entries []QueueEntry
		err := json.NewDecoder(r.Body).Decode(&entries)

		if err == nil {
			w.WriteHeader(http.StatusNoContent)
			GetProcessQueue().AddFiles(entries)
		} else {
			responseEncoder.Encode(err)
			w.WriteHeader(http.StatusInternalServerError)
		}
		break
	case http.MethodPut:
		var files []string
		err := json.NewDecoder(r.Body).Decode(&files)

		if err == nil {
			w.WriteHeader(http.StatusNoContent)
			GetProcessQueue().RemoveFiles(files)
		} else {
			w.WriteHeader(http.StatusInternalServerError)
			responseEncoder.Encode(err)
		}
		break
	default:
		w.WriteHeader(http.StatusMethodNotAllowed)
		break
	}
}
