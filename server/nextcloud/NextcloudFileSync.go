package nextcloud

import (
	"bufio"
	"os/exec"

	"../config"
	"../socket"
)

var (
	processes []*exec.Cmd
)

func SyncNextcloudFiles() error {
	websocketClient := socket.GetClient()

	cmd := exec.Command(
		config.GlobalConfig.Nextcloud.Executable,
		config.GlobalConfig.Nextcloud.SyncDirectory,
		config.GlobalConfig.Nextcloud.Server,
	)

	processes = append(processes, cmd)

	stdoutReader, err := cmd.StdoutPipe()
	websocketClient.Send(err)

	if err == nil {
		scanner := bufio.NewScanner(stdoutReader)
		go ReportScanner(scanner)
	} else {
		websocketClient.Send(err)
		return err
	}

	stderrReader, err := cmd.StderrPipe()
	websocketClient.Send(err)
	if err == nil {
		scanner := bufio.NewScanner(stderrReader)
		go ReportScanner(scanner)
	} else {
		websocketClient.Send(err)
		return err
	}

	websocketClient.Send("Start Nextcloud sync")

	err = cmd.Start()
	if err == nil {
		go cmd.Wait()
	} else {
		websocketClient.Send(err)
		return err
	}

	return nil
}
