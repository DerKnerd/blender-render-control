package blender

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
	currentBlenderClient *Client
)

type Client struct {
	connections []*Connection
}

func GetClient() *Client {
	if currentBlenderClient == nil {
		currentBlenderClient = &Client{connections: make([]*Connection, 0)}
	}

	return currentBlenderClient
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

func (client *Client) SendResponse(response Response) {
	for _, connection := range client.connections {
		connection.SendResponse(response)
	}
}

func (client *Client) SendError(file string, err error) {
	for _, connection := range client.connections {
		connection.SendError(file, err)
	}
}

func (client *Client) Send(v interface{}) {
	for _, connection := range client.connections {
		err := connection.Connection.WriteJSON(v)

		if err != nil {
			client.SendError("", err)
		}
	}
}
