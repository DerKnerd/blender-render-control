package nextcloud

import (
	"encoding/json"
	"net/http"
	"os"
	"path/filepath"

	"../config"
	"../queue"
)

func HandleHttp(w http.ResponseWriter, r *http.Request) {
	responseEncoder := json.NewEncoder(w)

	switch r.Method {
	case http.MethodGet:
		searchDir := config.GlobalConfig.Nextcloud.SyncDirectory

		fileList := make([]queue.File, 0)
		err := filepath.Walk(searchDir, func(path string, fileInfo os.FileInfo, err error) error {
			if filepath.Ext(path) == ".blend" {
				fileList = append(fileList, *queue.NewFile(fileInfo, path))
			}

			return nil
		})

		if err == nil {
			err := responseEncoder.Encode(fileList)

			if err == nil {
				w.WriteHeader(http.StatusOK)
			} else {
				w.WriteHeader(http.StatusInternalServerError)
				responseEncoder.Encode(err)
			}
		} else {
			w.WriteHeader(http.StatusInternalServerError)
			responseEncoder.Encode(err)
		}
		break
	case http.MethodPost:
		err := SyncNextcloudFiles()
		if err == nil {
			w.WriteHeader(http.StatusNoContent)
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
