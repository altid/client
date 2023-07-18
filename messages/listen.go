package messages

import (
	"bytes"
	"io"
	"log"
	"os"
	"strings"

	"github.com/altid/client"
	"github.com/altid/client/internal/defaults"
	"github.com/altid/libs/service/commander"
)

type Listener struct {
	c    *client.Client
	msgs chan *Message
	cmds []*commander.Command
}

func NewListener(c *client.Client, msgs chan *Message) (*Listener, error) {
	cmds, err := c.GetCommands()
	if err != nil {
		return nil, err
	}

	cmds = append(cmds, defaults.Commands...)
	l := &Listener{
		cmds: cmds,
		msgs: msgs,
		c:    c,
	}

	return l, nil
}

// Handle will block until the resulting Msg is read from our msgs channel
func (l *Listener) Handle(args string) {
	name := strings.Fields(args)
	switch name[0] {
	case "/help":
		l.send(listCommands(l.cmds), ListMsg)
	case "/buffer":
		l.send([]byte(name[1]), BufferMsg)
		l.c.Buffer(name[1])
		go l.fetch()
	case "/switch":
		l.send([]byte(name[1]), SendMsg)
		l.c.Buffer(name[1])
	case "/open":
		l.send([]byte(name[1]), OpenMsg)
		l.c.Open(name[1])
		go l.fetch()
	case "/close":
		l.send([]byte(name[1]), CloseMsg)
		l.c.Close(name[1])
		go l.fetch()
	case "/link":
		l.send([]byte(name[1]), LinkMsg)
		l.c.Link(name[1])
		go l.fetch()
	case "/list":
		data, _ := l.c.Tabs()
		l.send(data, ListMsg)
	case "/tabs":
		data, _ := l.c.Tabs()
		l.send(data, TabsMsg)
	case "/title":
		data, _ := l.c.Title()
		l.send(data, TitleMsg)
	case "/status":
		data, _ := l.c.Status()
		l.send(data, StatusMsg)
	case "/aside":
		data, _ := l.c.Aside()
		l.send(data, AsideMsg)
	case "/notify":
		data, _ := l.c.Notifications()
		l.send(data, NotificationMsg)
	case "/quit":
		l.c.Cleanup()
		os.Exit(0)
	default:
		if args[0] != '/' {
			l.c.Input([]byte(args))
			return
		}
		for _, cmd := range l.cmds {
			if "/"+cmd.Name == name[0] {
				l.c.Send(cmd, strings.Fields(args))
			}
		}
		return
	}
}

func (l *Listener) fetch() {
	data, err := l.c.Document()
	if err == nil && data != nil {
		l.msgs <- &Message{
			Data:    data,
			MsgType: MainMsg,
		}
		return
	}

	f, err := l.c.Feed()
	if err != nil {
		return
	}

	go func(f io.ReadCloser) {
		defer f.Close()

		for {
			// Ensure your buffer is MSIZE
			b := make([]byte, client.MSIZE)

			_, err := f.Read(b)
			if err != nil && err != io.EOF {
				log.Printf("error: %s\n", err)
				return
			}

			if len(b) > 0 {
				l.msgs <- &Message{
					Data:    b,
					MsgType: FeedMsg,
				}
			}
		}
	}(f)
}

func listCommands(cmds []*commander.Command) []byte {
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

func (l *Listener) send(data []byte, t MsgType) {
	l.msgs <- &Message{
		Data:    data,
		MsgType: t,
	}
}
