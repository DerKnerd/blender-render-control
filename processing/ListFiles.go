package processing

import (
	"os"
	"path/filepath"

	"../config"
	"../types"
)

func ListFiles() {
	websocketClient := types.GetWebsocketClient()
	searchDir := config.GlobalConfig.Nextcloud.SyncDirectory

	fileList := make([]types.File, 0)
	err := filepath.Walk(searchDir, func(path string, fileInfo os.FileInfo, err error) error {
		if filepath.Ext(path) == ".blend" {
			fileList = append(fileList, *types.NewFile(fileInfo, path))
		}

		return nil
	})

	if err != nil {
		websocketClient.SendError("", err)
	} else {
		websocketClient.Send(fileList)
	}
}
