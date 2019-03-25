package server

import (
	"../types"
	"bufio"
	"log"
	"os/exec"
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
	cmd := exec.Command(
		GlobalConfig.Nextcloud.Executable,
		GlobalConfig.Nextcloud.SyncDirectory,
		GlobalConfig.Nextcloud.Server,
	)

	processes = append(processes, cmd)

	stdoutReader, err := cmd.StdoutPipe()
	sendError("", err)

	if err == nil {
		scanner := bufio.NewScanner(stdoutReader)
		go func() {
			for scanner.Scan() {
				log.Print(scanner.Text())
				sendResponse(types.Response{
					File:    "",
					Message: scanner.Text(),
				})
			}
		}()
	} else {
		sendError("", err)
	}

	stderrReader, err := cmd.StderrPipe()
	sendError("", err)
	if err == nil {
		scanner := bufio.NewScanner(stderrReader)
		go func() {
			for scanner.Scan() {
				log.Fatal(scanner.Text())
				sendResponse(types.Response{
					File:    "",
					Message: scanner.Text(),
				})
			}
		}()
	} else {
		sendError("", err)
	}

	sendResponse(types.Response{
		Message: "Start Nextcloud sync",
	})
	err = cmd.Start()
	if err != nil {
		sendError("", err)
	} else {
		cmd.Wait()
		sendResponse(types.Response{
			Message: "Finished Nextcloud sync",
		})
	}
}
