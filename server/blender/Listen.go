package blender

func Listen(connection *Connection) {
	control := Control{}
	processQueue := GetProcessQueue()

	defer connection.Connection.Close()

	for {
		err := connection.ReadDataToObject(&control)
		if err != nil {
			connection.Connection.Close()
			break
		}

		switch {
		case control.Action == Render:
			go processQueue.Process()
		case control.Action == StopRender:
			go processQueue.StopProcessing(false)
		case control.Action == ForceStopRender:
			go processQueue.StopProcessing(true)
		case control.Action == AddToQueue:
			processQueue.AddFiles(control.Files)
		case control.Action == RemoveFromQueue:
			processQueue.RemoveFiles(control.Files)
		}
	}
}
