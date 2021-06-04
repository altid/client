package client

import (
	"github.com/altid/client/internal/defaults"
	"github.com/altid/client/internal/mock"
	"github.com/altid/client/internal/session"
	"github.com/altid/client/driver"
	"github.com/altid/libs/fs"
)

var (
	driversMu sync.RWMutex
	drivers = make(map[string]driver.Driver)
)

// Register makes a ui driver available by the provider name
// If Register is called twice with the same name or if driver is nil
// it panics
func Register(name string, driver driver.Driver) {
	driversMu.Lock()
	defer driversMu.Unlock()
	if driver == nil {
		panic("ui: Register driver is nil")
	}
	if _, dup := drivers[name]; dup {
		panic("ui: Register called twice for driver " + name)
	}
	drivers[name] = driver
}

// NewSession returns a session.Session ready to connect to addr:port
func NewSession(addr, port string) *session.Session {
	return &Session{
		run: defaults.NewSession(addr, port),
	}
}

// NewMockSession returns a session for testing
// Feed, if called, will be populated with data from google's GoFuzz every 100ms
func NewMockSession(addr string) *session.Session {
	return &Session{
		run: mock.NewSession(addr),
	}
}
