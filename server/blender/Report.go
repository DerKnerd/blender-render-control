package blender

import (
	"bufio"
	"log"
)

func ReportScanner(file string, scanner *bufio.Scanner) {
	websocketClient := GetClient()

	for scanner.Scan() {
		log.Print(scanner.Text())
		websocketClient.SendResponse(Response{
			File:    file,
			Message: scanner.Text(),
		})
	}
}
