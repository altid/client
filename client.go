package client

import (
	"context"
	"io"

	"github.com/altid/client/internal/defaults"
	"github.com/altid/client/internal/mock"
	"github.com/altid/client/driver"
	"github.com/altid/libs/fs"
)

// MSIZE - maximum size for a message
const MSIZE = defaults.MSIZE

// Client represents a 9p client session
type Client struct {
	run runner
}

// Driver implementations are used to draw content to physical devices, terminals, screen readers, etc 
type Renderer struct {
	ui driver.Driver
}

type runner interface {
	Aside() ([]byte, error)
	Attach() error
	Auth() error
	Ctl() ([]byte, error)
	Cleanup()
	Command(*fs.Command) (int, error)
	Connect(int) error
	Document() ([]byte, error)
	Feed() (io.ReadCloser, error)
	GetCommands() ([]*fs.Command, error)
	Input([]byte) (int, error)
	Notifications() ([]byte, error)
	Send(*fs.Command, []string) (int, error)
	Status() ([]byte, error)
	Tabs() ([]byte, error)
	Title() ([]byte, error)
}

// NewClient returns a client ready to connect to addr:port
func NewClient(addr, port string) *Client {
	return &Client{
		run: defaults.NewClient(addr, port),
	}
}

// NewMockClient returns a client for testing
// Feed, if called, will be populated with data from google's GoFuzz every 100ms
func NewMockClient(addr string) *Client {
	return &Client{
		run: mock.NewClient(addr),
	}
}

// GetCommands initializes c.CmdList
func (c *Client) GetCommands() ([]*fs.Command, error) {
	return c.run.GetCommands()
}

// Document returns the contents of a document file on the host
// if it exists, or an error
func (c *Client) Document() ([]byte, error) {
	return c.run.Document()
}

// Cleanup closes the underlying connection
func (c *Client) Cleanup() {
	c.run.Cleanup()
}

// Connect performs the network dial for the connection
func (c *Client) Connect(debug int) (err error) {
	return c.run.Connect(debug)
}

// Command sends the named command to the service
// If command is invalid, it will return an error
func (c *Client) Command(cmd *fs.Command) (int, error) {
	return c.run.Command(cmd)
}

// Attach is called after optionally calling Auth
func (c *Client) Attach() (err error) {
	return c.run.Attach()
}

// Auth is optionally called after Connect to authenticate with the server
func (c *Client) Auth() error {
	return c.run.Auth()
}

// Buffer changes the active buffer to the named buffer, or returns an error
func (c *Client) Buffer(name string) (int, error) {
	cmd := &fs.Command{
		Heading: fs.DefaultGroup,
		Name: "buffer",
		Args: []string{name},
	}

	return c.run.Command(cmd)
}

// Open attempts to open the named buffer
func (c *Client) Open(name string) (int, error) {
	cmd := &fs.Command{
		Heading: fs.DefaultGroup,
		Name: "open",
		Args: []string{name},
	}

	return c.run.Command(cmd)
}

// Close attempts to close the named buffer
func (c *Client) Close(name string) (int, error) {
	cmd := &fs.Command{
		Heading: fs.DefaultGroup,
		Name: "close",
		Args: []string{name},
	}

	return c.run.Command(cmd)
}

// Link updates the current buffer to the named buffer, closing the former
func (c *Client) Link(name string) (int, error) {
	cmd := &fs.Command{
		Heading: fs.DefaultGroup,
		Name: "link",
		Args: []string{name},
	}

	return c.run.Command(cmd)
}

// Tabs returns the contents of the `tabs` file for the server
func (c *Client) Tabs() ([]byte, error) {
	return c.run.Tabs()
}

// Title returns the contents of the `title` file for a given buffer
func (c *Client) Title() ([]byte, error) {
	return c.run.Title()
}

// Status returns the contents of the `status` file for a given buffer
func (c *Client) Status() ([]byte, error) {
	return c.run.Status()
}

// Aside returns the contents of the `aside` file for a given buffer
func (c *Client) Aside() ([]byte, error) {
	return c.run.Aside()
}

// Ctl returns the contents of the `ctl` file for a given service
func (c *Client) Ctl() ([]byte, error) {
	return c.run.Ctl()
}

// Input appends the given data string to input
func (c *Client) Input(data []byte) (int, error) {
	return c.run.Input(data)
}

// Notifications returns and clears any pending notifications
func (c *Client) Notifications() ([]byte, error) {
	return c.run.Notifications()
}

// Feed returns a ReadCloser connected to `feed`. It's expected all reads
// will be read into a buffer with a size of MSIZE
// It is also expected for Feed to be called in its own thread
func (c *Client) Feed() (io.ReadCloser, error) {
	return c.run.Feed()
}

// Send a named command with optional args to the service
func (c *Client) Send(cmd *fs.Command, args []string) (int, error) {
	return c.run.Send(cmd, args)
}

// FeedIterator allows you to step through lines of feed with Next()
// Useful for gomobile, etc
type FeedIterator struct {
	rc io.ReadCloser
}

// FeedIterator returns a new FeedIterator ready to go
func (c *Client) FeedIterator() (*FeedIterator, error) {
	f, err := c.run.Feed()
	if err != nil {
		return nil, err
	}

	return &FeedIterator{f}, nil
}

// Next will return the next slice of bytes, or an error
// After an error, future calls to Next() will panic
func (f *FeedIterator) Next() ([]byte, error) {
	b := make([]byte, MSIZE)
	if _, err := f.rc.Read(b); err != nil {
		defer f.rc.Close()
		return nil, err
	}

	return b, nil
}

// Register a UI renderer for client
func (c *Client) Register(d *driver.Driver) {
	
}
