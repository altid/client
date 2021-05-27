package main

import (
	"bufio"
	"errors"
	"strings"
	"time"

	"github.com/altid/client"
	"github.com/altid/libs/fs"
)

type listener struct {
	err  chan error
	data chan []byte
	done chan struct{}
	rd   *bufio.Reader
	c    *client.Client
	cmds []*fs.Command
}

func newListener(c *client.Client) (*listener, error) {
	cmds, err := c.GetCommands()
	if err != nil {
		return nil, err
	}

	cmds = append(cmds, additional...)

	l := &listener{
		cmds: cmds,
		err:  make(chan error),
		data: make(chan []byte),
		done: make(chan struct{}),
		c:    c,
	}

	return l, nil
}

func (l *listener) fetch() {
	if emitFeedData(l) != nil && emitDocumentData(l) != nil {
		l.err <- errors.New("Unable to find feed or document for given buffer")
		return
	}
}

func handle(l *listener, name, args string) {
	switch name {
	case "/help":
		l.data <- listCommands(l.cmds)
	case "/buffer":
		sendCmd(l, l.c.Buffer, args)
	case "/open":
		sendCmd(l, l.c.Open, args)
	case "/close":
		sendCmd(l, l.c.Close, args)
	case "/link":
		sendCmd(l, l.c.Link, args)
	case "/tabs":
		getData(l, l.c.Tabs)
	case "/title":
		getData(l, l.c.Title)
	case "/status":
		getData(l, l.c.Status)
	case "/aside":
		getData(l, l.c.Aside)
	case "/notify":
		getData(l, l.c.Notifications)
	default:
		otherMsg(l, name, args)
	}
}

func otherMsg(l *listener, name, args string) {
	if name[0] != '/' {
		l.c.Input([]byte(name + " " + args))
		return
	}

	for _, cmd := range l.cmds {
		if "/"+cmd.Name == name {
			l.c.Send(cmd, strings.Fields(args))
		}
	}

	return
}

func emitDocumentData(l *listener) error {
	f, err := l.c.Document()
	if err != nil {
		return err
	}

	if len(f) > 0 {
		l.data <- f
	}

	return nil
}

func emitFeedData(l *listener) error {
	f, err := l.c.Feed()
	if err != nil {
		return err
	}

	go func() {
		defer f.Close()

		for {
			// Ensure your buffer is MSIZE
			b := make([]byte, client.MSIZE)

			_, err := f.Read(b)
			if err != nil {
				l.err <- err
				return
			}

			if len(b) > 0 {
				l.data <- b
			}
		}
	}()

	return nil
}

func sendCmd(l *listener, fn func(string) (int, error), args ...string) {
	if len(args) != 1 {
		l.err <- errBadArgs
		return
	}

	if _, err := fn(args[0]); err != nil {
		l.err <- err
		return
	}

	// Eventually, we should have a chan here we can block on
	time.AfterFunc(time.Millisecond * 500, l.fetch)
}

func getData(l *listener, fn func() ([]byte, error)) {
	t, err := fn()
	if err != nil {
		l.err <- err
		return
	}

	l.data <- t
	return
}
