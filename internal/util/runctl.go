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
var ErrBadArgs = errors.New("too few/incorrect arguments")

func RunClientCtl(cmd int, from string, arg string) ([]byte, error) {
	var data string

	switch cmd {
	case CmdBuffer:
		data = fmt.Sprintf("buffer %s\x00", arg)
	case CmdOpen:
		data = fmt.Sprintf("open %s\x00", arg)
	case CmdClose:
		data = fmt.Sprintf("close %s\x00", arg)
	case CmdLink:
		data = fmt.Sprintf("link %s\x00", arg)
	default:
		return nil, ErrBadArgs
	}

	return []byte(data), nil
}
