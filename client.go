package client

import (
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
	client *Client
}

type runner interface {
	Attach() error
	Auth() error
	Cleanup()
	Command(*fs.Command) (int, error)
	Connect(int) error
	GetCommands() ([]*fs.Command, error)
	Input([]byte) (int, error)
	Notifications() ([]byte, error)
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

// Input appends the given data string to input
func (c *Client) Input(data []byte) (int, error) {
	return c.run.Input(data)
}

// Register a UI renderer for client
func (c *Client) Register(d *driver.Driver) *Renderer {
	return nil	
}
