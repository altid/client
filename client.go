package client

import (
	"context
	"sync"

	"github.com/altid/client/driver"
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

// UI is a ui handle representing a pool of zero or more underlying connections.
// It's safe for concurrent use by multiple goroutines
type UI struct {
	ctx context.Context
	connector driver.Connector
	mu	sync.Mutex
}

// Open returns a handle to an uninitialized UI connector.
func Open(driverName string) (*UI, error) {
	return nil, nil
}

// WithSession will create a client session, using a given UI driver
// and an underlying session state. 
func WithSession(driverName string, session session.Session) (*UI, error) {
	return nil, nil
} 
