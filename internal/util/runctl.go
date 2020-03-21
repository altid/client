package util

import (
	"errors"
	"fmt"
)

const (
	CmdBuffer int = iota
	CmdOpen
	CmdClose
	CmdLink
	CmdRefresh
	CmdConnect
	CmdAttach
	CmdAuth
	CmdTabs
	CmdTitle
	CmdStatus
	CmdAside
	CmdNotify
	CmdInput
	CmdFeed
	CmdDocument
	CmdComm
)

// ErrBadArgs is returned from Ctl when incorrect arguments are provided
var ErrBadArgs = errors.New("Too few/incorrect arguments")

func RunClientCtl(cmd int, args ...string) ([]byte, error) {
	var data string
	switch cmd {
	case CmdBuffer:
		if len(args) != 1 {
			return nil, ErrBadArgs
		}

		data = fmt.Sprintf("buffer %s\x00", args[0])
	case CmdOpen:
		if len(args) != 1 {
			return nil, ErrBadArgs
		}

		data = fmt.Sprintf("open %s\x00", args[0])
	case CmdClose:
		if len(args) != 1 {
			return nil, ErrBadArgs
		}

		data = fmt.Sprintf("close %s\x00", args[0])
	case CmdLink:
		if len(args) != 2 {
			return nil, ErrBadArgs
		}

		data = fmt.Sprintf("link %s %s\x00", args[0], args[1])
	default:
		return nil, ErrBadArgs
	}

	return []byte(data), nil
}
