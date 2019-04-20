package nextcloud

import (
	"bufio"
	"log"

	"../socket"
)

func ReportScanner(scanner *bufio.Scanner) {
	websocketClient := socket.GetClient()

	for scanner.Scan() {
		log.Print(scanner.Text())
		websocketClient.Send(scanner.Text())
	}
}
