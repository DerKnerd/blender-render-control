package nextcloud

import (
	"bufio"
	"log"
)

func ReportScanner(scanner *bufio.Scanner) {
	websocketClient := GetClient()

	for scanner.Scan() {
		log.Print(scanner.Text())
		websocketClient.SendMessage(scanner.Text())
	}
}
