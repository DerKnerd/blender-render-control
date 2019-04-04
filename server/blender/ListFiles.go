package blender

import (
	"encoding/json"
	"net/http"
	"os"
	"path/filepath"

	"../config"
)

func ListFiles(w http.ResponseWriter, r *http.Request) {
	searchDir := config.GlobalConfig.Nextcloud.SyncDirectory

	fileList := make([]File, 0)
	err := filepath.Walk(searchDir, func(path string, fileInfo os.FileInfo, err error) error {
		if filepath.Ext(path) == ".blend" {
			fileList = append(fileList, *NewFile(fileInfo, path))
		}

		return nil
	})

	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		json.NewEncoder(w).Encode(err)
	} else {
		w.WriteHeader(http.StatusOK)
		err := json.NewEncoder(w).Encode(fileList)

		if err != nil {
			w.WriteHeader(http.StatusInternalServerError)
			json.NewEncoder(w).Encode(err)
		}
	}
}
