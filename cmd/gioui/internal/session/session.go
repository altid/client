package session

import (
	"context"
	"errors"
	"log"
	"time"

	"gioui.org/app"
	"gioui.org/font/gofont"
	"gioui.org/io/system"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/gioui/internal/parser"
	"github.com/altid/client/cmd/gioui/internal/services"
	"github.com/altid/client/cmd/gioui/internal/ui"
)

type Session struct {
	win    *app.Window
	svcs   *services.Services
	unread *time.Ticker
	layout *ui.Layout
	view   *ui.View
	status *ui.Status
	title  *ui.Title
	aside  *ui.Aside
	list   *ui.List
	input  *ui.Input
}

func NewSession(w *app.Window, s *services.Services) *Session {
	th := material.NewTheme(gofont.Collection())
	list := &widget.List{}
	return &Session{
		win: w,
		svcs: s,
		layout: ui.NewLayout(th),
		unread: time.NewTicker(time.Second * 5),
		view: ui.NewView(list, th, s.Notify),
		status: ui.NewStatus(th),
		title:  ui.NewTitle(th),
		aside:  ui.NewAside(th),
		input: ui.NewInput(s, th),
		list: ui.NewList(s, th),
	}
}

func (s *Session) Run(ctx context.Context, debug bool) error {
	for {
		select {
		case <- s.unread.C:
			if debug {
				log.Println("timer")
			}
			parser.RebuildTabs(s.list, s.svcs)
		case e := <-s.win.Events():
			switch e := e.(type) {
			case system.DestroyEvent:
				return errors.New("Finished")
			case system.FrameEvent:
				s.layout.Update(e,	s.title, s.aside, s.status, s.view, s.input, s.list)
				e.Frame(s.layout.Ops)
			case system.StageEvent:
				//
			}
		case e := <-s.svcs.Event():
			//if debug {
				log.Println(e)
			//}
			curr := s.svcs.Current()
			switch e {
			case "list":
				parser.RebuildTabs(s.list, s.svcs)
			case "scan":
				s.list.AddService(s.svcs)
				parser.RebuildTabs(s.list, s.svcs)
			case "buffer":
				parser.RebuildData(s.title, curr)
				parser.RebuildData(s.aside, curr)
				parser.RebuildData(s.status, curr)
				parser.RebuildData(s.view, curr)
			case "input":
				//
			case "feed":
				parser.BuildData(s.view, curr)
			}
			s.win.Invalidate()
		}
	}
}