package blender

import (
	"bufio"
	"os"
	"os/exec"
	"path"
	"sort"
)

var (
	workingDir, _ = os.Getwd()
	processQueue  *ProcessQueue
)

type ProcessQueue struct {
	Files          []string
	process        bool
	processing     bool
	blenderCommand *exec.Cmd
}

func GetProcessQueue() *ProcessQueue {
	if processQueue == nil {
		processQueue = &ProcessQueue{}
	}

	return processQueue
}

func (processQueue *ProcessQueue) Process() {
	processQueue.process = true

	if !processQueue.processing {
		processQueue.processing = true

		for len(processQueue.Files) != 0 && processQueue.process {
			processQueue.ProcessNext()
		}

		processQueue.processing = false
	}
}

func (processQueue *ProcessQueue) StopProcessing(force bool) {
	processQueue.process = false
	websocketClient := GetClient()

	if force && processQueue.blenderCommand != nil {
		err := processQueue.blenderCommand.Process.Kill()
		websocketClient.SendError("", err)
	}
}

func (processQueue *ProcessQueue) ProcessNext() {
	file, leftFiles := processQueue.Files[0], processQueue.Files[1:]
	websocketClient := GetClient()

	processQueue.blenderCommand = exec.Command(
		"/usr/bin/blender",
		file,
		"-b",
		"-P",
		path.Join(workingDir, "render_all_cameras.py"),
		"--",
		"7680",
		"4320",
	)

	stdoutReader, err := processQueue.blenderCommand.StdoutPipe()
	websocketClient.SendError(file, err)

	if err == nil {
		scanner := bufio.NewScanner(stdoutReader)
		go ReportScanner(file, scanner)
	} else {
		websocketClient.SendError(file, err)
	}

	stderrReader, err := processQueue.blenderCommand.StderrPipe()
	websocketClient.SendError(file, err)
	if err == nil {
		scanner := bufio.NewScanner(stderrReader)
		go ReportScanner(file, scanner)
	} else {
		websocketClient.SendError(file, err)
	}

	err = processQueue.blenderCommand.Start()
	if err != nil {
		websocketClient.SendError(file, err)
	} else {
		defer processQueue.blenderCommand.Wait()
		processQueue.Files = leftFiles
	}
}

func (processQueue *ProcessQueue) AddFiles(files []string) {
	processQueue.Files = append(processQueue.Files, files...)
}

func (processQueue *ProcessQueue) RemoveFiles(files []string) {
	for _, file := range files {
		idx := sort.SearchStrings(processQueue.Files, file)
		afterIdx := processQueue.Files[idx+1:]
		beforeIdx := processQueue.Files[:idx]

		processQueue.Files = append(afterIdx, beforeIdx...)
	}
}
