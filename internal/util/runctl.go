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

func RunClientCtl(cmd int, from string, args ...string) ([]byte, error) {
	var data string

	if len(args) != 1 {
		return nil, ErrBadArgs
	}

	switch cmd {
	case CmdBuffer:
		data = fmt.Sprintf("buffer %s %s\x00", from, args[0])
	case CmdOpen:
		data = fmt.Sprintf("open %s %s\x00", from, args[0])
	case CmdClose:
		data = fmt.Sprintf("close %s %s\x00", from, args[0])
	case CmdLink:
		data = fmt.Sprintf("link %s %s\x00", from, args[0])
	default:
		return nil, ErrBadArgs
	}

	return []byte(data), nil
}