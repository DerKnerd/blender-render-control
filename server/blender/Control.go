package blender

const (
	Render          = 0
	StopRender      = 1
	ForceStopRender = 2
	AddToQueue      = 3
	RemoveFromQueue = 4
)

type Control struct {
	Action int      `json:"action"`
	Files  []string `json:"files,omitempty"`
}
