package ui

import (
	"image/color"

	"gioui.org/layout"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/gioui/internal/services"
)

type Input struct {
	notify func(string)
	s  *services.Services
	th *material.Theme
	ed widget.Editor
}

func NewInput(s *services.Services, th *material.Theme, notify func(string)) *Input {
	return &Input{
		notify: notify,
		s: s,
		th: th,
		ed: widget.Editor{},
	}
}

func (i *Input) Layout(gtx layout.Context) layout.Dimensions {
	i.ed.Submit = true
	i.ed.SingleLine = true
	ed := material.Editor(i.th, &i.ed, "Enter your message here...")
	for _, ev := range ed.Editor.Events() {
		if e, ok := ev.(widget.SubmitEvent); ok {
			i.ed.SetText("")
			if i.s.Current() != nil {
				i.s.Current().Client.Input([]byte(e.Text))
				i.notify("input")
			}
		}
	}
	e := ed.Layout(gtx)
	e.Size.X = gtx.Constraints.Max.X
	return widget.Border{
		Color:        color.NRGBA{R: 204, G: 204, B: 204, A: 255},
		CornerRadius: unit.Dp(3),
		Width:        unit.Dp(2),
	}.Layout(gtx, func(gtx layout.Context) layout.Dimensions {
		return e
	})
}

func (i *Input) Handle() {
	for _, in := range i.ed.Events() {
		// If we have a submit, clear our input send input to Service
		if item, ok := in.(widget.SubmitEvent); ok {
			i.ed.SetText("")
			curr := i.s.Current()
			if curr != nil {
				curr.Client.Input([]byte(item.Text))
				curr.Notify("input")
			}
		}
	}
}
