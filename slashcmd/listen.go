package slashcmd

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"strings"
	"time"

	"github.com/altid/client"
	"github.com/altid/client/internal/defaults"
	"github.com/altid/libs/service/commander"
)

type Listener struct {
	done chan struct{}
	c    *client.Client
	out  *bufio.Writer
	cmds []*commander.Command
}

func NewListener(c *client.Client, out *bufio.Writer) (*Listener, error) {
	cmds, err := c.GetCommands()
	if err != nil {
		return nil, err
	}

	cmds = append(cmds, defaults.Commands...)

	l := &Listener{
		cmds: cmds,
		out:  out,
		done: make(chan struct{}),
		c:    c,
	}

	return l, nil
}

func (l *Listener) fetch() {
	go emitDocumentData(l)
	go emitFeedData(l)
}

func (l *Listener) Handle(args string) {
	name := strings.Fields(args)
	defer l.out.Flush()
	switch name[0] {
	case "/help":
		l.out.Write(ListCommands(l.cmds))
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

func otherMsg(l *Listener, name, args string) {
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

func emitFeedData(l *Listener) error {
	f, err := l.c.Feed()
	if err != nil {
		return err
	}

	go func(f io.ReadCloser) {
		defer f.Close()

		for {
			// Ensure your buffer is MSIZE
			b := make([]byte, client.MSIZE)

			_, err := f.Read(b)
			if err != nil && err != io.EOF {
				l.out.WriteString(fmt.Sprintf("error: %s\n", err))
				f.Close()
				return
			}

			if len(b) > 0 {
				l.out.Write(b)
			}
		}
	}(f)

	return nil
}

func emitDocumentData(l *Listener) error {
	f, err := l.c.Document()
	if err != nil {
		l.out.WriteString(fmt.Sprintf("error: %s\n", err))
		return err
	}

	l.out.Write(f)
	l.out.WriteRune('\n')
	return nil
}

func getData(l *Listener, fn func() (b []byte, err error)) {
	t, err := fn()
	if err != nil {
		return
	}

	l.out.Write(t)
	l.out.WriteRune('\n')
}
