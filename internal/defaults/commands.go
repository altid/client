package defaults

import (
	"github.com/altid/libs/service/commander"
)

var Commands = []*commander.Command{
	{
		Name:        "title",
		Args:        []string{},
		Heading:     commander.DefaultGroup,
		Description: "print the title of the current buffer",
	},
	{
		Name:        "aside",
		Args:        []string{},
		Heading:     commander.DefaultGroup,
		Description: "print the aside data of the current buffer",
	},
	{
		Name:        "status",
		Args:        []string{},
		Heading:     commander.DefaultGroup,
		Description: "print the status of the current buffer",
	},
	{
		Name:        "tabs",
		Args:        []string{},
		Heading:     commander.DefaultGroup,
		Description: "print a list of all connected buffers",
	},
	{
		Name:        "notify",
		Args:        []string{},
		Heading:     commander.DefaultGroup,
		Description: "print and clear any pending notifications",
	},
}
