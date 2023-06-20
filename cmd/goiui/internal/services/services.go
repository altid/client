package services

import (
	"context"
	"fmt"
	"sync"

	"github.com/grandcat/zeroconf"
)

type Services struct {
	srvlist map[string]*Service
	current string
	debug   bool
	sync.Mutex
}

func NewServices(debug bool) *Services {
	return &Services{
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

// Select will instantiate the named service if it has not been already, setting it as the current
// It will return any errors encountered
func (s *Services) Select(req string) error {
	svc := s.Current()
	if svc == nil {
		return fmt.Errorf("no service available named %s", req)
	}	
	s.current = req	
	if svc.isRunning {
		return nil
	}
	if e := svc.Connect(); e != nil {
		return e
	}
	if e := svc.Client.Attach(); e != nil {
		return e
	}
	svc.Parse()
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
				Name: entry.ServiceRecord.Instance,
				// We only list one IP per service
				addr:  entry.AddrIPv4[0].String(),
				port:  entry.Port,
				debug: s.debug,
			}
			s.Lock()
			svc.Parse()
			s.srvlist[svc.Name] = svc
			s.Unlock()
		}
	}(entries)
	return resolver.Browse(ctx, "_altid._tcp", "local", entries)
}
