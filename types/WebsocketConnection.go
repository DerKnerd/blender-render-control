package types

import (
	"log"

	"github.com/gorilla/websocket"
)

type WebsocketConnection struct {
	Connection *websocket.Conn
}

func (connection *WebsocketConnection) SendResponse(response Response) {
	err := connection.Connection.WriteJSON(response)
	if err != nil {
		log.Printf(err.Error())
	}
}

func (connection *WebsocketConnection) SendError(file string, err error) {
	if err != nil {
		connection.SendResponse(Response{
			File:    file,
			Message: err.Error(),
		})
	}
}

func (connection *WebsocketConnection) ReadDataToObject(v interface{}) error {
	return connection.Connection.ReadJSON(v)
}
