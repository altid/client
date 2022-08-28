package main

import (
	"fmt"
	"os"
	"strings"
	"time"

	"github.com/altid/client"
	"github.com/altid/libs/service/commander"
)

type listener struct {
	done chan struct{}
	c    *client.Client
	cmds []*commander.Command
}

func newListener(c *client.Client) (*listener, error) {
	cmds, err := c.GetCommands()
	if err != nil {
		return nil, err
	}

	cmds = append(cmds, additional...)

	l := &listener{
		cmds: cmds,
		done: make(chan struct{}),
		c:    c,
	}

	return l, nil
}

func (l *listener) fetch() {
	go emitDocumentData(l)
	go emitFeedData(l)
}

func handle(l *listener, args string) {
	name := strings.Fields(args)
	switch name[0] {
	case "/help":
		fmt.Printf("%s", listCommands(l.cmds))
	case "/buffer":
		l.c.Buffer(name[1])
		time.AfterFunc(time.Millisecond*1000, l.fetch)
	case "/open":
		l.c.Open(name[1])
		time.AfterFunc(time.Millisecond*1000, l.fetch)
	case "/close":
		l.c.Close(name[1])
		time.AfterFunc(time.Millisecond*1000, l.fetch)
	case "/link":
		l.c.Link(name[1])
		time.AfterFunc(time.Millisecond*1000, l.fetch)
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
	case "/quit":
		l.c.Cleanup()
		os.Exit(0)
	default:
		otherMsg(l, name[0], args)
	}
}

func otherMsg(l *listener, name, args string) {
	if name[0] != '/' {
		l.c.Input([]byte(args))
		return
	}

	for _, cmd := range l.cmds {
		if "/"+cmd.Name == name {
			l.c.Send(cmd, strings.Fields(args))
		}
	}
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
				fmt.Printf("%s\n", err)
				return
			}

			if len(b) > 0 {
				fmt.Printf("%s\n", b)
			}
		}
	}()

	return nil
}

func emitDocumentData(l *listener) error {
	f, err := l.c.Document()
	if err != nil {
		fmt.Printf("%s\n", err)
		return err
	}

	fmt.Printf("%s\n", f)
	return nil
}

func getData(l *listener, fn func() (b []byte, err error)) {
	t, err := fn()
	if err != nil {
		return
	}

	fmt.Printf("%s\n", t)
}
