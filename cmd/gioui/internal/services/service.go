package services

import (
	"fmt"
	"log"
	"sync"

	"gioui.org/widget"
	"github.com/altid/client"
	"github.com/altid/libs/service/commander"
)

type Service struct {
	Client *client.Client
	Name   string
	Ready  bool
	Notify func(string)
	Tabs      map[string]*widget.Clickable
	cmds      []*commander.Command
	addr      string
	port      int
	isRunning bool
	hasFeed   bool
	debug     bool
	sync.Mutex
}

type Tab struct {
	Name   string
	Unread int
	Click  *widget.Clickable
	S      *Service
}

func (s *Service) HasFeed() bool {
	return s.hasFeed
}

func (s *Service) Connect() error {
	port := fmt.Sprintf("%d", s.port)
	s.Client = client.NewClient(s.addr, port)
	if e := s.Client.Connect(s.debug); e != nil {
		return e
	}
	if e := s.Client.Attach(); e != nil {
		return e
	}
	s.Ready = true
	// TODO: Handle commands to start Parse for other channels when we swap
	return nil
}

func (s *Service) Buffer(name string) {
	if _, e := s.Client.Buffer(name); e != nil {
		log.Println("attempting to go to buffer error: ", e)
	}
	s.Notify("buffer")
}
