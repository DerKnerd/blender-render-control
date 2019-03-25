package server

import (
	"../types"
	"bufio"
	"log"
	"os/exec"
	"path"
	"sort"
)

type ProcessQueue struct {
	Files          []string
	process        bool
	blenderCommand *exec.Cmd
}

func (processQueue *ProcessQueue) Process() {
	processQueue.process = true

	for len(processQueue.Files) != 0 && processQueue.process {
		processQueue.ProcessNext()
	}
}

func (processQueue *ProcessQueue) StopProcessing(force bool) {
	processQueue.process = false
	if force && processQueue.blenderCommand != nil {
		err := processQueue.blenderCommand.Process.Kill()
		sendError("", err)
	}
}

func (processQueue *ProcessQueue) ProcessNext() {
	file, leftFiles := processQueue.Files[0], processQueue.Files[1:]

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

	stderrReader, err := processQueue.blenderCommand.StderrPipe()
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

	err = processQueue.blenderCommand.Start()
	if err != nil {
		sendError("", err)
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
