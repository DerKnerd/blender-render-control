package socket

type Control struct {
	Action int      `json:"action"`
	Files  []string `json:"files,omitempty"`
}
