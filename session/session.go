package session 

import (
	"github.com/altid/client/internal/defaults"
	"github.com/altid/client/internal/mock"
	"github.com/altid/libs/fs"
)

// Session represents a 9p client session
type Session struct {
	run runner
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

// GetCommands initializes c.CmdList
func (c *Session) GetCommands() ([]*fs.Command, error) {
	return c.run.GetCommands()
}

// Cleanup closes the underlying connection
func (c *Session) Cleanup() {
	c.run.Cleanup()
}

// Connect performs the network dial for the connection
func (c *Session) Connect(debug int) (err error) {
	return c.run.Connect(debug)
}

// Command sends the named command to the service
// If command is invalid, it will return an error
func (c *Session) Command(cmd *fs.Command) (int, error) {
	return c.run.Command(cmd)
}

// Attach is called after optionally calling Auth
func (c *Session) Attach() (err error) {
	return c.run.Attach()
}

// Auth is optionally called after Connect to authenticate with the server
func (c *Session) Auth() error {
	return c.run.Auth()
}

// Input appends the given data string to input
func (c *Session) Input(data []byte) (int, error) {
	return c.run.Input(data)
}
