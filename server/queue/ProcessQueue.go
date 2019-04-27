package queue

import (
	"../socket"
	"bufio"
	"os"
	"os/exec"
	"path"
	"strconv"
)

var (
	workingDir, _ = os.Getwd()
	processQueue  *ProcessQueue
)

type ProcessQueue struct {
	Items          []QueueEntry `json:"items"`
	Processing     bool         `json:"processing"`
	process        bool
	blenderCommand *exec.Cmd
}

func GetProcessQueue() *ProcessQueue {
	if processQueue == nil {
		processQueue = &ProcessQueue{
			Items: make([]QueueEntry, 0),
		}
	}

	return processQueue
}

func (processQueue *ProcessQueue) Process() {
	processQueue.process = true

	if !processQueue.Processing {
		processQueue.Processing = true

		for len(processQueue.Items) != 0 && processQueue.process {
			processQueue.ProcessNext()
		}

		processQueue.Processing = false
	}
}

func (processQueue *ProcessQueue) StopProcessing(force bool) {
	processQueue.process = false
	websocketClient := socket.GetClient()

	if force && processQueue.blenderCommand != nil {
		err := processQueue.blenderCommand.Process.Kill()
		websocketClient.SendError("", err)
	}
}

func (processQueue *ProcessQueue) ProcessNext() {
	queueEntry, leftFiles := processQueue.Items[0], processQueue.Items[1:]
	file := queueEntry.Path
	websocketClient := socket.GetClient()

	processQueue.blenderCommand = exec.Command(
		"/usr/bin/blender",
		file,
		"-b",
		"-P",
		path.Join(workingDir, "render_all_cameras.py"),
		"--",
		strconv.Itoa(queueEntry.Width),
		strconv.Itoa(queueEntry.Height),
	)

	stdoutReader, err := processQueue.blenderCommand.StdoutPipe()
	websocketClient.SendError(file, err)

	if err == nil {
		scanner := bufio.NewScanner(stdoutReader)
		go socket.ReportScannerWithFile(file, scanner)
	} else {
		websocketClient.SendError(file, err)
	}

	stderrReader, err := processQueue.blenderCommand.StderrPipe()
	websocketClient.SendError(file, err)
	if err == nil {
		scanner := bufio.NewScanner(stderrReader)
		go socket.ReportScannerWithFile(file, scanner)
	} else {
		websocketClient.SendError(file, err)
	}

	err = processQueue.blenderCommand.Start()
	if err != nil {
		websocketClient.SendError(file, err)
	} else {
		defer processQueue.blenderCommand.Wait()
		processQueue.Items = leftFiles
	}
}

func (processQueue *ProcessQueue) AddFiles(entries []QueueEntry) {
	processQueue.Items = append(processQueue.Items, entries...)

	socket.GetClient().Send("Added entries to queue")
}

func (processQueue *ProcessQueue) RemoveFiles(files []string) {
	for _, file := range files {
		var idx int
		var item QueueEntry
		for idx, item = range processQueue.Items {
			if item.Path == file {
				break
			}
		}
		afterIdx := processQueue.Items[idx+1:]
		beforeIdx := processQueue.Items[:idx]

		processQueue.Items = append(afterIdx, beforeIdx...)
	}

	socket.GetClient().Send("Removed files from queue")
}
