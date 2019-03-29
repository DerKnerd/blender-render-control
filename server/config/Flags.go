package config

import (
	"flag"
)

type Flags struct {
	ConfigFile *string
	Address    *string
	LogFile    *string
	Port       *int
}

var flags = parse()

func parse() Flags {
	flags := Flags{}
	flags.ConfigFile = flag.String("config.file", "", "The configuration file to use")
	flags.Port = flag.Int("listen.port", 1337, "The port to listen on")
	flags.Address = flag.String("listen.address", "", "The address to listen on")
	flags.LogFile = flag.String("log.file", "", "The log file to write to")
	flag.Parse()

	return flags
}

func GetFlags() Flags {
	return flags
}
