package types

import (
	"log"
	"net/http"

	"github.com/gorilla/websocket"
)

var (
	upgrader = websocket.Upgrader{
		ReadBufferSize:  1024,
		WriteBufferSize: 1024,
	}
	currentClient *WebsocketClient
)

type WebsocketClient struct {
	connections []*WebsocketConnection
}

func GetWebsocketClient() *WebsocketClient {
	if currentClient != nil {
		currentClient = &WebsocketClient{connections: make([]*WebsocketConnection, 0)}
	}

	return currentClient
}

func (client *WebsocketClient) AddConnection(connection *WebsocketConnection) {
	client.connections = append(client.connections, connection)

}

func (client *WebsocketClient) ConvertHttpToWs(r *http.Request, w http.ResponseWriter) (*WebsocketConnection, error) {
	conn, err := upgrader.Upgrade(w, r, nil)

	if err != nil {
		log.Print("Upgrade: ", err)
		return nil, err
	}

	websocketConnection := &WebsocketConnection{
		Connection: conn,
	}

	defer conn.Close()

	return websocketConnection, nil
}

func (client *WebsocketClient) SendResponse(response Response) {
	for _, connection := range client.connections {
		connection.SendResponse(response)
	}
}

func (client *WebsocketClient) SendError(file string, err error) {
	for _, connection := range client.connections {
		connection.SendError(file, err)
	}
}
