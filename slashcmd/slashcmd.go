package slashcmd

import (
	"bytes"

	"github.com/altid/libs/service/commander"
)

func ListCommands(cmds []*commander.Command) []byte {
	var b bytes.Buffer

	for _, cmd := range cmds {
		b.WriteString("/" + cmd.Name)
		for _, arg := range cmd.Args {
			b.WriteString(" <" + arg + ">")
		}

		b.WriteString("\t# " + cmd.Description)
		b.WriteRune('\n')
	}

	return b.Bytes()
}
