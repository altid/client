package ui

import (
	"image"
	"time"

	"gioui.org/app"
	"gioui.org/font/gofont"
	"gioui.org/io/event"
	"gioui.org/widget"

	"gioui.org/io/system"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type session struct {
	w      *app.Window
	s      *services.Services
	view   *view
	status *status
	title  *title
	aside  *aside
	list   *list
	input  *input
}

func Run(w *app.Window, s *services.Services, debug bool) error {
	th := material.NewTheme(gofont.Collection())
	var ops op.Ops
	// We share the theme with all of our widgets, the current service will be updated
	// Any time we click a different service buffer or main window
	// Create a list of listItems, update later on
	items := []*listItem{}
	for _, item := range s.List() {
		li := &listItem{
			svc: item,
			th: th,
			clickable: &widget.Clickable{},
		}
		items = append(items, li)
	}
	sess := &session{
		w: w,
		s: s,
		view: &view{
			svc: s.Current(),
			th:  th,
		},
		status: &status{
			svc: s.Current(),
			th:  th,
		},
		title: &title{
			svc: s.Current(),
			th:  th,
		},
		aside: &aside{
			svc: s.Current(),
			th:  th,
		},
		list: &list{
			items: items,
			button: &widget.Clickable{},
			th:      th,
			hide:    true,
		},
		input: &input{
			s:  s,
			th: th,
			ed: widget.Editor{},
		},
	}

	for {
		select {
		case e := <-w.Events():
			// NOTE: Only use reset if we have nothing as a macro
			// https://pkg.go.dev/gioui.org/op#MacroOp
			ops.Reset()
			if err := sess.handle(e, &ops); err != nil {
				return err
			}
		//case e := <-s.Events():
		// If the stack is inactive for a few seconds, check for new data from the tabs list
		case <-time.After(time.Second * 2):
			w.Invalidate()
		}
	}
}

func (s *session) handle(e event.Event, ops *op.Ops) error {
	switch e := e.(type) {
	case system.DestroyEvent:
		return e.Err
	case system.FrameEvent:
		gtx := layout.NewContext(ops, e)
		layout.Inset{
			Top:    4,
			Bottom: 4,
			Left:   4,
			Right:  4,
		}.Layout(gtx, func(gtx layout.Context) layout.Dimensions {
			return layout.Stack{}.Layout(gtx,
				layout.Expanded(func(gtx layout.Context) layout.Dimensions {
					return layout.Flex{
						Axis: layout.Vertical,
					}.Layout(gtx,
						layout.Rigid(s.title.Layout),
						layout.Flexed(1, s.view.Layout),
						layout.Rigid(s.status.Layout),
						layout.Rigid(s.input.Layout),
					)
				}),
				layout.Stacked(func(gtx layout.Context) layout.Dimensions {
					d := s.list.Layout(gtx)
					d.Size = image.Point{
						X: gtx.Constraints.Max.X,
						Y: gtx.Constraints.Max.Y,
					}
					return d
				}),
			)
		})
		//s.aside.Layout(gtx
		//s.menu.Layout(gtx)
		e.Frame(ops)
	}
	return nil
}
