package main

import (
	"bufio"
	"errors"
	"io"
	"os"
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
		rd:   bufio.NewReader(os.Stdin),
		err:  make(chan error),
		data: make(chan []byte),
		done: make(chan struct{}),
		c:    c,
	}

	return l, nil
}

func (l *listener) listen() {
	if emitFeedData(l) != nil && emitDocumentData(l) != nil {
		l.err <- errors.New("Unable to find feed or document for given buffer")
		return
	}

	for {
		line, err := l.rd.ReadString('\n')
		if err != nil && err != io.EOF {
			l.err <- err
			return
		}

		t := strings.Fields(line)
		if t[0] == "/quit" {
			close(l.done)
			return
		}

		handle(l, t)
	}
}

func handle(l *listener, args []string) {
	switch args[0] {
	case "/help":
		l.data <- listCommands(l.cmds)
	case "/buffer":
		sendCmd(l, l.c.Buffer, args[1])
	case "/open":
		sendCmd(l, l.c.Open, args[1])
	case "/close":
		sendCmd(l, l.c.Close, args[1])
	case "/link":
		sendCmd(l, l.c.Link, args[1])
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
		otherMsg(l, args)
	}
}

func otherMsg(l *listener, args []string) {
	if args[0][0] != '/' {
		line := strings.Join(args, " ")
		l.c.Input([]byte(line))
		return
	}

	for _, cmd := range l.cmds {
		if "/"+cmd.Name == args[0] {
			line := strings.Join(args[1:], " ")
			l.c.Send(cmd, []byte(line))
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

	time.Sleep(time.Millisecond * 200)
	if emitFeedData(l) != nil && emitDocumentData(l) != nil {
		l.err <- errors.New("Unable to find feed or document for given buffer")
	}
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
