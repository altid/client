package defaults

import (
	"errors"
	"fmt"
	"io"
	"net"

	"github.com/altid/libs/client/internal/feed"
	"github.com/altid/libs/fs"
	"github.com/knieriem/g/go9p/user"
	"github.com/lionkov/go9p/p"
	"github.com/lionkov/go9p/p/clnt"
)

type Client struct {
	commands []*fs.Command
	afid     *clnt.Fid
	root     *clnt.Fid
	addr     string
	buffer   string
	clnt     *clnt.Clnt
}

func NewClient(addr string) *Client {
	return &Client{
		addr: addr,
	}
}

func (c *Client) Connect(debug int) (err error) {
	dial := fmt.Sprintf("%s:564", c.addr)

	conn, err := net.Dial("tcp", dial)
	if err != nil {
		return err
	}

	c.clnt, err = clnt.Connect(conn, p.MSIZE, false)
	if err != nil {
		return err
	}

	c.clnt.Debuglevel = debug

	return
}

func (c *Client) Cleanup() {
	c.clnt.Unmount()
}

func (c *Client) Attach() error {
	root, err := c.clnt.Attach(c.afid, user.Current(), "/")
	if err != nil {
		return err
	}

	c.root = root

	return nil
}

func (c *Client) Auth() error {
	// TODO(halfwit): We want to flag in factotum use and hand it the afid
	afid, err := c.clnt.Auth(user.Current(), "/")
	if err != nil {
		return err
	}

	c.afid = afid
	return nil
}

// Command sends off a named command
func (c *Client) Command(cmd *fs.Command) (int, error) {
	nfid := c.clnt.FidAlloc()
	_, err := c.clnt.Walk(c.root, nfid, []string{"ctl"})
	if err != nil {
		return 0, err
	}

	cmd.Name = c.buffer

	c.clnt.Open(nfid, p.OAPPEND)
	defer c.clnt.Clunk(nfid)

	for _, comm := range c.commands {
		if comm.Name == cmd.Name {
			return c.clnt.Write(nfid, cmd.Bytes(), 0)
		}
	}
	return 0, errors.New("found no such command")
}

func (c *Client) Tabs() ([]byte, error) {
	return getNamedFile(c, "tabs")
}

func (c *Client) Title() ([]byte, error) {
	return getNamedFile(c, "title")
}

func (c *Client) Status() ([]byte, error) {
	return getNamedFile(c, "status")
}

func (c *Client) Aside() ([]byte, error) {
	return getNamedFile(c, "aside")
}

func (c *Client) Ctl() ([]byte, error) {
	return getNamedFile(c, "ctl")
}

func (c *Client) Document() ([]byte, error) {
	return getNamedFile(c, "document")
}

func (c *Client) Input(data []byte) (int, error) {
	nfid := c.clnt.FidAlloc()
	_, err := c.clnt.Walk(c.root, nfid, []string{"input"})
	if err != nil {
		return 0, err
	}

	c.clnt.Open(nfid, p.OAPPEND)
	defer c.clnt.Clunk(nfid)

	return c.clnt.Write(nfid, data, 0)
}

func (c *Client) Notifications() ([]byte, error) {
	nfid := c.clnt.FidAlloc()
	_, err := c.clnt.Walk(c.root, nfid, []string{"notification"})
	if err != nil {
		return nil, err
	}

	c.clnt.Open(nfid, p.OREAD)
	defer c.clnt.Remove(nfid)

	return c.clnt.Read(nfid, 0, p.MSIZE)
}

func (c *Client) GetCommands() ([]*fs.Command, error) {
	b, err := getNamedFile(c, "ctl")
	if err != nil {
		return nil, err
	}

	return fs.FindCommands(b)
}

func (c *Client) Feed() (io.ReadCloser, error) {
	nfid := c.clnt.FidAlloc()

	_, err := c.clnt.Walk(c.root, nfid, []string{"feed"})
	if err != nil {
		return nil, err
	}

	c.clnt.Open(nfid, p.OREAD)

	data := make(chan []byte)
	done := make(chan struct{})

	go func() {
		var off uint64
		defer c.clnt.Clunk(nfid)

		for {

			b, err := c.clnt.Read(nfid, off, p.MSIZE)
			if err != nil {
				return
			}

			if len(b) > 0 {
				data <- b
				off += uint64(len(b))
			}

			select {
			case <-done:
				return
			default:
				continue
			}
		}

	}()

	f := &feed.Feed{
		Data: data,
		Done: done,
	}

	return f, nil

}

func getNamedFile(c *Client, name string) ([]byte, error) {
	nfid := c.clnt.FidAlloc()
	_, err := c.clnt.Walk(c.root, nfid, []string{name})
	if err != nil {
		return nil, err
	}

	c.clnt.Open(nfid, p.OREAD)
	defer c.clnt.Clunk(nfid)

	return c.clnt.Read(nfid, 0, p.MSIZE)
}
