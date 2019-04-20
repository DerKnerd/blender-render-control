package socket

import (
	"log"

	"github.com/gorilla/websocket"
)

type Connection struct {
	Connection *websocket.Conn
}

func (connection *Connection) SendResponse(response Response) {
	err := connection.Connection.WriteJSON(response)
	if err != nil {
		log.Printf(err.Error())
	}
}

func (connection *Connection) SendError(file string, err error) {
	if err != nil {
		connection.SendResponse(Response{
			File:    file,
			Message: err.Error(),
		})
	}
}

func (connection *Connection) ReadDataToObject(v interface{}) error {
	return connection.Connection.ReadJSON(v)
}
