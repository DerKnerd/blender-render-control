package types

const (
	Render          = 0
	StopRender      = 1
	ForceStopRender = 2
	SyncFiles       = 3
	StopSyncFiles   = 4
	ListFiles       = 5
)

type BlenderControl struct {
	Action int      `json:"action"`
	Files  []string `json:"files,omitempty"`
}
