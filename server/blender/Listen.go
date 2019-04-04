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
			processQueue.AddFiles(control.Files)
			go processQueue.Process()
		case control.Action == StopRender:
			go processQueue.StopProcessing(false)
		case control.Action == ForceStopRender:
			go processQueue.StopProcessing(true)
		}
	}
}
