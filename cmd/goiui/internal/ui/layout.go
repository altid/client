package ui

import (
	"errors"
	"image"
	"image/color"
	"sort"

	"gioui.org/app"
	"gioui.org/font/gofont"
	"gioui.org/io/event"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
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
	i := []*listItem{}
	for _, item := range s.List() {
		li := &listItem{
			svc:       item,
			th:        th,
			clickable: &widget.Clickable{},
		}
		i = append(i, li)
		sort.Sort(items(i))
	}
	// Only on feed, not document
	viewList := &widget.List{Scrollbar: widget.Scrollbar{}}
	viewList.Axis = layout.Vertical
	viewList.ScrollToEnd = true
	sess := &session{
		w: w,
		s: s,
		view: &view{
			position: layout.Position{},
			svc: s.Current(),
			list: viewList,
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
			s:      s,
			items:  i,
			button: &widget.Clickable{},
			th:     th,
			hide:   true,
		},
		input: &input{
			s:  s,
			th: th,
			ed: widget.Editor{},
		},
	}
	// Instead, fetch tabs each 5 seconds
	for {
		select {
		case e := <-w.Events():
			// NOTE: Only use reset if we have nothing as a macro
			// https://pkg.go.dev/gioui.org/op#MacroOp
			if err := sess.handle(e, &ops); err != nil {
				return err
			}
		// TODO: Store each ops on the type, build up and invalidate only when touched
		// TODO: We call tabs about 30 times a second when scrolling
		case e := <-s.Event():
			curr := s.Current()
			switch e {
			case "list":
				sess.list.items.update(s, th)
			case "scan":
				sess.list.items.update(s, th)
			case "buffer":
				sess.status.svc = curr
				sess.aside.svc = curr
				sess.title.svc = curr
			case "input":
				sess.view.svc = curr
			case "feed":
				sess.view.svc = curr
			}
			w.Invalidate()
		}
	}
}

func (s *session) handle(e event.Event, ops *op.Ops) error {
	switch e := e.(type) {
	case system.DestroyEvent:
		return errors.New("finished")
	case system.FrameEvent:
		gtx := layout.NewContext(ops, e)
		layout.Stack{}.Layout(gtx,
			layout.Stacked(func(gtx layout.Context) layout.Dimensions {
				return layout.Inset{
					Left: 204,
					Top: 4,
					Bottom: 4,
					Right: 4,
				}.Layout(gtx, func(gtx layout.Context) layout.Dimensions {
					return layout.Flex{
						Axis: layout.Vertical,
						Spacing: layout.SpaceEvenly,
					}.Layout(gtx,
						layout.Rigid(s.title.Layout),
						layout.Flexed(1, s.view.Layout),
						layout.Rigid(s.status.Layout),
						layout.Rigid(s.input.Layout),
					)
				})
			}),
			layout.Expanded(func(gtx layout.Context) layout.Dimensions {
				menu := clip.Rect{
					Min: image.Pt(0, 4),
					Max: image.Pt(400, gtx.Constraints.Max.Y-4),
				}
				menu.Push(gtx.Ops)
				paint.ColorOp{
					Color: color.NRGBA{R: 220, G: 220, B: 220, A: 0xff},
				}.Add(gtx.Ops)
				paint.PaintOp{}.Add(gtx.Ops)
				d := s.list.Layout(gtx)
				d.Size = image.Point{
					X: 400,
					Y: gtx.Constraints.Max.Y,
				}
				return d
			}),
		)

		//s.menu.Layout(gtx)
		e.Frame(ops)
	case system.StageEvent:
		//
	}
	return nil
}
