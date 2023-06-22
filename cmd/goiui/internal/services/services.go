package services

import (
	"context"
	"fmt"
	"sync"

	"gioui.org/widget"
	"github.com/grandcat/zeroconf"
)

type Services struct {
	events    chan string
	srvlist   map[string]*Service
	current   string
	debug     bool
	sync.RWMutex
}

func NewServices(debug bool) *Services {
	return &Services{
		events: make(chan string, 64),
		srvlist: make(map[string]*Service),
		debug:   debug,
	}
}

// Don't bother leaking anything we don't need here
func (s *Services) List() []*Service {
	s.Lock()
	defer s.Unlock()
	list := []*Service{}
	for _, item := range s.srvlist {
		list = append(list, item)
	}
	return list
}

// Current returns the active service
func (s *Services) Current() *Service {
	if svc, ok := s.srvlist[s.current]; ok {
		return svc
	}
	return nil
}

func (s *Services) Notify(str string) {
	s.events <- str
}

func (s *Services) Event() chan string {
	return s.events
}

// Select will instantiate the named service if it has not been already, setting it as the current
// It will return any errors encountered
func (s *Services) Select(req string) error {
	svc, ok := s.srvlist[req]
	if svc == nil || !ok {
		return fmt.Errorf("no service available named %s", req)
	}
	if (s.current != req) {
		// Pop the current buffer to "none" so unreads populate correctly
		curr := s.Current()
		if curr != nil && curr.Ready {
			curr.Client.Buffer("none")
		}
		s.current = req
	}
	if svc.isRunning {
		return nil
	}
	svc.isRunning = true
	if e := svc.Connect(); e != nil {
		return e
	}
	if e := svc.Client.Attach(); e != nil {
		return e
	}
	// TODO: Build menu with this data too!
	cmds, err := svc.Client.GetCommands()
	if err != nil {
		return err
	}
	svc.cmds = cmds
	svc.Ready = true
	return nil
}

// the list returned from mDNS can grow prior to finishing, we call in a Go routine and update a local type
func (s *Services) Scan(ctx context.Context) error {
	resolver, err := zeroconf.NewResolver(nil)
	if err != nil {
		return err
	}
	entries := make(chan *zeroconf.ServiceEntry)
	go func(results <-chan *zeroconf.ServiceEntry) {
		for entry := range results {
			svc := &Service{
				Name:   entry.ServiceRecord.Instance,
				Notify: s.Notify,
				// We only list one IP per service
				addr:  entry.AddrIPv4[0].String(),
				tabs:  make(map[string]*widget.Clickable),
				port:  entry.Port,
				debug: s.debug,
			}
			svc.Parse()
			svc.Notify("scan")
			s.srvlist[svc.Name] = svc
		}
	}(entries)
	return resolver.Browse(ctx, "_altid._tcp", "local", entries)
}
