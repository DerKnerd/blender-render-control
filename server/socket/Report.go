package socket

import (
	"bufio"
	"log"
)

func ReportScannerWithFile(file string, scanner *bufio.Scanner) {
	websocketClient := GetClient()

	for scanner.Scan() {
		log.Print(scanner.Text())
		websocketClient.SendResponse(Response{
			File:    file,
			Message: scanner.Text(),
		})
	}
}
