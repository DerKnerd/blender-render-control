package nextcloud

import (
	"bufio"
	"os/exec"

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
	websocketClient := GetClient()

	cmd := exec.Command(
		config.GlobalConfig.Nextcloud.Executable,
		config.GlobalConfig.Nextcloud.SyncDirectory,
		config.GlobalConfig.Nextcloud.Server,
	)

	processes = append(processes, cmd)

	stdoutReader, err := cmd.StdoutPipe()
	websocketClient.SendError(err)

	if err == nil {
		scanner := bufio.NewScanner(stdoutReader)
		go ReportScanner(scanner)
	} else {
		websocketClient.SendError(err)
	}

	stderrReader, err := cmd.StderrPipe()
	websocketClient.SendError(err)
	if err == nil {
		scanner := bufio.NewScanner(stderrReader)
		go ReportScanner(scanner)
	} else {
		websocketClient.SendError(err)
	}

	websocketClient.SendMessage("Start Nextcloud sync")

	err = cmd.Start()
	if err != nil {
		websocketClient.SendError(err)
	} else {
		err = cmd.Wait()
		if err != nil {
			websocketClient.SendMessage("Finished Nextcloud sync")
		} else {
			websocketClient.SendError(err)
		}
	}
}
