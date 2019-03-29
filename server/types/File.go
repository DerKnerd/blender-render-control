package types

import (
	"os"
	"path/filepath"
	"strings"
	"time"
)

type File struct {
	Path         string    `json:"path"`
	Name         string    `json:"name"`
	Size         int64     `json:"size"`
	ModifiedDate time.Time `json:"modified_date"`
}

func NewFile(fileInfo os.FileInfo, fullPath string) *File {
	basename := fileInfo.Name()
	extension := filepath.Ext(basename)

	return &File{
		Path:         fullPath,
		Size:         fileInfo.Size(),
		Name:         strings.TrimSuffix(basename, extension),
		ModifiedDate: fileInfo.ModTime(),
	}
}
