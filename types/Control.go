package types

type BlenderControl struct {
	Action int      `json:"action"`
	Files  []string `json:"files,omitempty"`
}
