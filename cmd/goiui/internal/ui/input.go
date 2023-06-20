package ui

import (
	"image/color"

	"gioui.org/layout"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type input struct {
	s  *services.Services
	th *material.Theme
	ed widget.Editor
}

func (i *input) Layout(gtx layout.Context) layout.Dimensions {
	i.ed.Submit = true
	i.ed.SingleLine = true
	ed := material.Editor(i.th, &i.ed, "")
	for _, ev := range ed.Editor.Events() {
		if e, ok := ev.(widget.SubmitEvent); ok {
			i.ed.SetText("")
			if i.s.Current() != nil {
				i.s.Current().Client.Input([]byte(e.Text))
			}
		}
	}
	border := widget.Border{
		Color:        color.NRGBA{R: 204, G: 204, B: 204, A: 255},
		CornerRadius: unit.Dp(3),
		Width:        unit.Dp(2),
	}
	return border.Layout(gtx, ed.Layout)
}

func (i *input) Handle() {
	for _, in := range i.ed.Events() {
		// If we have a submit, clear our input
		// Send input to Service
		if item, ok := in.(widget.SubmitEvent); ok {
			i.ed.SetText("")
			curr := i.s.Current()
			if curr != nil {
				curr.Client.Input([]byte(item.Text))
			}
		}
	}
}
