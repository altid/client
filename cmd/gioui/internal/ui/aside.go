package ui

import (
	"gioui.org/layout"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"gioui.org/x/richtext"
	"github.com/altid/client"
)

type Aside struct {
	th      *material.Theme
	data    [][]richtext.SpanStyle
	visible bool
	ready   bool
}

func NewAside(th *material.Theme) *Aside {
	return &Aside{
		th: th,
		ready: false,
	}
}

func (a *Aside) Fallback() ([]byte, bool)               { return nil, false }
func (a *Aside) Fetch(c *client.Client) ([]byte, error) { return c.Aside() }
func (a *Aside) Clear()                                 { a.data = nil }
func (a *Aside) Status(ready bool)                      { a.ready = ready }

// TODO: Build widget.list for scrollable like view.go
func (a *Aside) Style() richtext.SpanStyle {
	return richtext.SpanStyle{
		Size: unit.Sp(14),
	}
}

func (a *Aside) Set(data interface{}) {
	/* Test this */
	if d, ok := data.([]richtext.SpanStyle); ok {
		var last = 0
		for i, span := range d {
			if span.Content == "\n" {
				a.data = append(a.data, d[last:i])
				last = i
			}
		}
	}
}

// Fetch the list of items, build into list type
func (a *Aside) Layout(gtx layout.Context) layout.Dimensions {
	var state richtext.InteractiveText
	if !a.ready || !a.visible {
		return layout.Dimensions{}
	}
	list := &widget.List{}
	list.Axis = layout.Vertical
	as := material.List(a.th, list)
	return as.Layout(gtx, len(a.data), func(gtx layout.Context, index int) layout.Dimensions {
		return richtext.Text(&state, a.th.Shaper, a.data[index]...).Layout(gtx)
	})
}
