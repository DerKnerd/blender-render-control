package server

import (
	"gopkg.in/yaml.v2"
	"io/ioutil"
)

type Nextcloud struct {
	Executable    string `yaml:"executable"`
	SyncDirectory string `yaml:"syncdir"`
	Server        string `yaml:"server"`
}

type Blender struct {
	Executable   string `yaml:"executable"`
	PythonScript string `yaml:"python_script"`
}

type Config struct {
	Nextcloud Nextcloud `yaml:"nextcloud"`
	Blender   Blender   `yaml:"blender"`
}

func (config *Config) Parse(configFile string) {
	data, err := ioutil.ReadFile(configFile);
	if err != nil {
		panic(err)
	}

	err = yaml.Unmarshal(data, config)
	if err != nil {
		panic(err)
	}
}

var (
	GlobalConfig Config
)
