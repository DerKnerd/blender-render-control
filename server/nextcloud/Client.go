package nextcloud

import (
	"log"
	"net/http"

	"github.com/gorilla/websocket"
)

var (
	upgrader = websocket.Upgrader{
		ReadBufferSize:  1024,
		WriteBufferSize: 1024,
		Subprotocols:    []string{"binary", "text"},
	}
	currentNextcloudClient *Client
)

type Client struct {
	connections []*Connection
}

func GetClient() *Client {
	if currentNextcloudClient == nil {
		currentNextcloudClient = &Client{connections: make([]*Connection, 0)}
	}

	return currentNextcloudClient
}

func (client *Client) AddConnection(connection *Connection) {
	client.connections = append(client.connections, connection)

}

func (client *Client) ConvertHttpToWs(w http.ResponseWriter, r *http.Request) (*Connection, error) {
	conn, err := upgrader.Upgrade(w, r, nil)

	if err != nil {
		log.Print("Upgrade: ", err)
		return nil, err
	}

	websocketConnection := &Connection{
		Connection: conn,
	}

	return websocketConnection, nil
}

func (client *Client) SendMessage(message string) {
	for _, connection := range client.connections {
		connection.SendMessage(message)
	}
}

func (client *Client) SendError(err error) {
	for _, connection := range client.connections {
		connection.SendError(err)
	}
}
