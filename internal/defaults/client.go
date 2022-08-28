package defaults

import (
	"errors"
	"fmt"
	//"io"
	"net"
	//"time"

	//"github.com/altid/client/internal/feed"
	"github.com/altid/libs/service/commander"
	"github.com/altid/libs/service/parser"
	"github.com/knieriem/g/go9p/user"
	"github.com/lionkov/go9p/p"
	"github.com/lionkov/go9p/p/clnt"
)

type Client struct {
	commands []*commander.Command
	afid     *clnt.Fid
	root     *clnt.Fid
	addr     string
	port     string
	buffer   string
	clnt     *clnt.Clnt
}

func NewClient(addr, port string) *Client {
	return &Client{
		addr: addr,
		port: port,
	}
}

func (c *Client) Connect(debug int) (err error) {
	dial := fmt.Sprintf("%s:%s", c.addr, c.port)

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

// Command will write a command, returning an error
// if the command is not supported by the service
// It will set cmd.From on your behalf
func (c *Client) Command(cmd *commander.Command) (int, error) {
	nfid := c.clnt.FidAlloc()
	_, err := c.clnt.Walk(c.root, nfid, []string{"ctrl"})
	if err != nil {
		return 0, err
	}

	cmd.From = c.buffer
	fmt.Printf("Found command: %s\n", cmd.Bytes())

	c.clnt.Open(nfid, p.OAPPEND)
	defer c.clnt.Clunk(nfid)

	for _, comm := range c.commands {
		if comm.Name == cmd.Name {
			return c.clnt.Write(nfid, cmd.Bytes(), 0)
		}
	}

	return 0, errors.New("found no such command")
}

func (c *Client) Send(cmd *commander.Command, data []string) (int, error) {
	nfid := c.clnt.FidAlloc()
	_, err := c.clnt.Walk(c.root, nfid, []string{"ctrl"})
	if err != nil {
		return 0, err
	}

	cmd.From = c.buffer

	c.clnt.Open(nfid, p.OAPPEND)
	defer c.clnt.Clunk(nfid)

	cmd.Args = data
	return c.clnt.Write(nfid, cmd.Bytes(), 0)
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
	return getNamedFile(c, "ctrl")
}

func (c *Client) Document() ([]byte, error) {
	return getNamedFile(c, "main")
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

// GetCommands initializes the internal cmdlist
func (c *Client) GetCommands() ([]*commander.Command, error) {
	b, err := getNamedFile(c, "ctrl")
	if err != nil {
		return nil, err
	}

	cmds, err := parser.ParseCtrlFile(b)
	if err != nil {
		return nil, err
	}

	c.commands = cmds
	return cmds, nil
}

/*
func (c *Client) Feed() (io.ReadCloser, error) {
	nfid := c.clnt.FidAlloc()

	_, err := c.clnt.Walk(c.root, nfid, []string{"stream"})
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
				time.Sleep(time.Millisecond * 300)
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
*/
func getNamedFile(c *Client, name string) ([]byte, error) {
	nfid := c.clnt.FidAlloc()
	_, err := c.clnt.Walk(c.root, nfid, []string{name})
	if err != nil {
		return nil, fmt.Errorf("bad walk to %s: %s", name, err)
	}
	defer c.clnt.Clunk(nfid)
	if e := c.clnt.Open(nfid, p.OREAD); e != nil {
		return nil, fmt.Errorf("issue in Open on %s: %s", name, e)
	}

	return c.clnt.Read(nfid, 0, p.MSIZE)
}
