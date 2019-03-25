package app

import (
	"flag"
)

type Flags struct {
	ConfigFile *string
	Mode       *string
}

var flags = parse()

func parse() Flags {
	flags := Flags{}
	flags.Mode = flag.String("mode", "client", "Start qt app or server, use server to start as server or client to start the qt app")
	flags.ConfigFile = flag.String("config.file", "", "The configuration file to use")
	flag.Parse()

	return flags
}

func GetFlags() Flags {
	return flags
}
