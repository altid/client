package mock

import (
	"io"
	"log"
	"os"
	"time"

	"github.com/altid/libs/client/internal/feed"
	"github.com/altid/libs/client/internal/util"
	"github.com/altid/libs/fs"
	fuzz "github.com/google/gofuzz"
)

type Client struct {
	afid  io.ReadWriteCloser
	addr  string
	debug func(int, ...interface{})
}

func NewClient(addr string) *Client {
	return &Client{
		addr:  addr,
		debug: func(int, ...interface{}) {},
	}
}

func (c *Client) Cleanup() {}

func (c *Client) Connect(debug int) error {
	if debug > 0 {
		c.debug = mockLogging
	}

	c.debug(util.CmdConnect, c.addr)
	return nil
}

// Test the afid here
func (c *Client) Attach() error {
	// Read on RPC for
	c.debug(util.CmdAttach, true)
	return nil
}

func (c *Client) Auth() error {
	// TODO(halfwit): We want to flag in factotum use and hand it an afid
	c.debug(util.CmdAuth, true)
	return nil
}

func (c *Client) Command(cmd *fs.Command) error {
	c.debug(util.CmdComm, cmd)
	return nil
}

func (c *Client) GetCommands() ([]*fs.Command, error) {
	d := []*fs.Command{
		{
			Name:        "open",
			Args:        []string{"<buffer>"},
			Heading:     fs.DefaultGroup,
			Description: "Open and change buffers to a given service",
		},
		{
			Name:        "close",
			Args:        []string{"<buffer>"},
			Heading:     fs.DefaultGroup,
			Description: "Close a buffer and return to the last opened previously",
		},
		{
			Name:        "buffer",
			Args:        []string{"<buffer>"},
			Heading:     fs.DefaultGroup,
			Description: "Change to the named buffer",
		},
		{
			Name:        "link",
			Args:        []string{"<to>", "<from>"},
			Heading:     fs.DefaultGroup,
			Description: "Overwrite the current <to> buffer with <from>, switching to from after. This destroys <to>",
		},
		{
			Name:        "quit",
			Args:        []string{},
			Heading:     fs.DefaultGroup,
			Description: "Exits the service",
		},
	}
	return d, nil
}

// We want to eventually create and track tabs internally to the library
func (c *Client) Ctl(cmd int, args ...string) (int, error) {
	data, err := util.RunClientCtl(cmd, args...)
	if err != nil {
		return 0, err
	}

	c.debug(cmd, data)
	return 0, nil
}

func (c *Client) Tabs() ([]byte, error) {
	//c.debug(util.CmdTabs, c.tablist)
	return nil, nil
}

func (c *Client) Title() ([]byte, error) {
	//c.debug(util.CmdTitle, c.current)
	//return c.current, nil
	return nil, nil
}

func (c *Client) Status() ([]byte, error) {
	c.debug(util.CmdStatus)
	return []byte("status"), nil
}

func (c *Client) Document() ([]byte, error) {
	b := make([]byte, 4096)
	fuzz := fuzz.New()

	fuzz.Fuzz(&b)
	c.debug(util.CmdDocument, b)
	return b, nil
}

func (c *Client) Aside() ([]byte, error) {
	c.debug(util.CmdAside)
	return []byte("aside"), nil
}

func (c *Client) Input(data []byte) (int, error) {
	c.debug(util.CmdInput, data)
	return len(data), nil
}

func (c *Client) Notifications() ([]byte, error) {
	c.debug(util.CmdNotify)
	return []byte("notifications"), nil
}

func (c *Client) Feed() (io.ReadCloser, error) {
	data := make(chan []byte)
	done := make(chan struct{})

	go func() {
		var b []byte
		fuzz := fuzz.New()
		defer close(data)

		for {
			select {
			case <-done:
				return
			default:
				fuzz.Fuzz(&b)
				c.debug(util.CmdFeed, b)
				data <- b
				time.Sleep(time.Millisecond * 100)
			}
		}
	}()

	f := &feed.Feed{
		Data: data,
		Done: done,
	}

	return f, nil
}

func mockLogging(cmd int, args ...interface{}) {
	l := log.New(os.Stdout, "client ", 0)

	switch cmd {
	case util.CmdConnect:
		l.Printf("connect addr=\"%s\"\n", args[0])
	case util.CmdAttach:
		l.Printf("attach success=%t\n", args[0])
	case util.CmdAuth:
		l.Printf("auth success=%t\n", args[0])
	case util.CmdBuffer, util.CmdOpen, util.CmdClose, util.CmdLink:
		l.Printf("cmd %s", args[0])
		l.Println()
	case util.CmdComm:
		m := args[0].(*fs.Command)
		l.Printf("%s from=\"%s\" args=\"%s\"\n", m.Name, m.From, m.Args)
	case util.CmdTabs:
		l.Printf("tabs list=\"%s\"\n", args[0])
	case util.CmdTitle:
		l.Printf("title data=\"%s\"\n", args[0])
	case util.CmdStatus:
		l.Println("status data=nil")
	case util.CmdAside:
		l.Println("aside data=nil")
	case util.CmdInput:
		l.Printf("input data=\"%s\"\n", args[0])
	case util.CmdNotify:
		l.Println("notification data=nil")
	case util.CmdFeed:
		l.Printf("feed data=\"%s\"\n", args[0])
	case util.CmdDocument:
		l.Printf("document data=\"%s\"\n", args[0])
	}
}
