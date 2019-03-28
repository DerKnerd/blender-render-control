package processing

import (
	"bufio"
	"log"
	"os/exec"

	"../../types"
	"../config"
)

var (
	processes []*exec.Cmd
)

func StopSyncNextcloudFiles() {
	for _, process := range processes {
		_ = process.Process.Kill()
	}
}

func SyncNextcloudFiles() {
	websocketClient := types.GetWebsocketClient()

	cmd := exec.Command(
		config.GlobalConfig.Nextcloud.Executable,
		config.GlobalConfig.Nextcloud.SyncDirectory,
		config.GlobalConfig.Nextcloud.Server,
	)

	processes = append(processes, cmd)

	stdoutReader, err := cmd.StdoutPipe()
	websocketClient.SendError("", err)

	if err == nil {
		scanner := bufio.NewScanner(stdoutReader)
		go func() {
			for scanner.Scan() {
				log.Print(scanner.Text())
				websocketClient.SendResponse(types.Response{
					File:    "",
					Message: scanner.Text(),
				})
			}
		}()
	} else {
		websocketClient.SendError("", err)
	}

	stderrReader, err := cmd.StderrPipe()
	websocketClient.SendError("", err)
	if err == nil {
		scanner := bufio.NewScanner(stderrReader)
		go func() {
			for scanner.Scan() {
				log.Fatal(scanner.Text())
				websocketClient.SendResponse(types.Response{
					File:    "",
					Message: scanner.Text(),
				})
			}
		}()
	} else {
		websocketClient.SendError("", err)
	}

	websocketClient.SendResponse(types.Response{
		Message: "Start Nextcloud sync",
	})

	err = cmd.Start()
	if err != nil {
		websocketClient.SendError("", err)
	} else {
		err = cmd.Wait()
		if err != nil {
			websocketClient.SendResponse(types.Response{
				Message: "Finished Nextcloud sync",
			})
		} else {
			websocketClient.SendError("", err)
		}
	}
}
