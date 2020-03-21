package main

import (
	"bytes"

	"github.com/altid/libs/fs"
)

var additional = []*fs.Command{
	{
		Name:        "title",
		Args:        []string{},
		Heading:     fs.DefaultGroup,
		Description: "print the title of the current buffer",
	},
	{
		Name:        "aside",
		Args:        []string{},
		Heading:     fs.DefaultGroup,
		Description: "print the aside data of the current buffer",
	},
	{
		Name:        "status",
		Args:        []string{},
		Heading:     fs.DefaultGroup,
		Description: "print the status of the current buffer",
	},
	{
		Name:        "tabs",
		Args:        []string{},
		Heading:     fs.DefaultGroup,
		Description: "print a list of all connected buffers",
	},
	{
		Name:        "notify",
		Args:        []string{},
		Heading:     fs.DefaultGroup,
		Description: "print and clear any pending notifications",
	},
}

func listCommands(cmds []*fs.Command) []byte {
	var b bytes.Buffer

	for _, cmd := range cmds {
		b.WriteString("/" + cmd.Name + "\t#" + cmd.Description)
	}

	return b.Bytes()
}
