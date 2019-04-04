package nextcloud

import (
	"log"

	"github.com/gorilla/websocket"
)

type Connection struct {
	Connection *websocket.Conn
}

func (connection *Connection) SendMessage(message string) {
	err := connection.Connection.WriteMessage(websocket.TextMessage, []byte(message))
	if err != nil {
		log.Printf(err.Error())
	}
}

func (connection *Connection) SendError(err error) {
	if err != nil {
		connection.SendMessage(err.Error())
	}
}

func (connection *Connection) ReadDataToObject(v interface{}) error {
	return connection.Connection.ReadJSON(v)
}
