package types

type Response struct {
	Message string `json:"message,omitempty"`
	File    string `json:"file,omitempty"`
}
