package driver

/* Package driver defines interfaces do be implemented by graphics library drivers as used by package client.

**This has been modelled, initially off of the SQL driver interface**

A driver allows connections open to various UI elements, writing payloads to them or streaming
data, until a connection is closed
Widgets enable additional data to be included in addition to the stateful elements used internally 
*/

type Driver interface {
	Open() (Conn, error)
}

// A conn is a connection to a UI driver. It is not used concurrently by multiple goroutines
// Conn is assumed to be stateful
type Conn interface {
	// Prepare returns a return widget, bound to this connection
	Prepare(target string) (Widget, error)

	// Close invalidates and potentially stops any current 
	// prepared payloads and transactions, marking this
	// connection as no longer in use.
	Close() error

	// Begin starts and returns a new Element
	Begin() (Element, error)
}

// A connector represents a driver in a fixed configuration and can
// create any number of equivalent Conns for use by multiple goroutines
// These are typically used to manage widgets 
type Connector interface {
	// Connect returns a connection to the graphics library driver
	// The connections returned are idempotent to one another	
	Connect(context.Context) (Conn, error)
}

type Element interface {
	Redraw()
	Clear()
}

type Widget interface {
	// Close will remove the widget from the UI
	Close() error

	// SetContent shows, and optionally updates the payload of a widget
	SetContent([]byte) error
}
