package services

import (
	"bytes"
	"fmt"
	"strconv"

	"gioui.org/font/gofont"
	"gioui.org/layout"
	"gioui.org/widget/material"
	"github.com/altid/client"
)

type Service struct {
	Client *client.Client
	// TODO: Some way to watch for new activity
	Name      string
	Ready     bool
	Data      []layout.Widget
	addr      string
	port      int
	isRunning bool
	debug     bool
}

type Tab struct {
	Name   string
	Unread int
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
	// TODO: Start a goroutine to watch
	// TODO: Handle commands to start Parse for other channels when we swap
	return nil
}

func (s *Service) Parse() {
	th := material.NewTheme(gofont.Collection())

	s.Data = []layout.Widget{
		material.Body1(th, "test").Layout,
		material.Body1(th, "this").Layout,
		material.Body1(th, "chortle").Layout,
		material.Subtitle2(th, "Chickin").Layout,
	}
}

func (s *Service) Tabs() []*Tab {
	list := []*Tab{}
	data, err := s.Client.Tabs()
	if err != nil {
		return list
	}
	// #foobar [123]
	for _, item := range bytes.Split(data, []byte("\n")) {
		opening := bytes.LastIndex(item, []byte("["))
		closing := bytes.LastIndex(item, []byte("]"))
		// If we have neither of these values, either we're malformed or it's a blank line
		if closing == -1 || opening == -1 {
			continue
		}
		unread, _ := strconv.Atoi(string(item[opening+1 : closing-1]))
		tab := &Tab{
			Name:   string(item[:opening-1]),
			Unread: unread,
		}
		list = append(list, tab)
	}
	return list
}
