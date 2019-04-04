package blender

const (
	Render          = 0
	StopRender      = 1
	ForceStopRender = 2
)

type Control struct {
	Action int      `json:"action"`
	Files  []string `json:"files,omitempty"`
}
