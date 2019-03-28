package utils

import (
	"bufio"
	"log"

	"../../types"
)

func ReportScanner(scanner *bufio.Scanner) {
	websocketClient := types.GetWebsocketClient()

	for scanner.Scan() {
		log.Print(scanner.Text())
		websocketClient.SendResponse(types.Response{
			File:    "",
			Message: scanner.Text(),
		})
	}
}
