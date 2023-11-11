package ui

import (
	"gioui.org/layout"
	"gioui.org/unit"
	"gioui.org/widget/material"
	"gioui.org/x/richtext"
	"github.com/altid/client"
)

const fb = `Altid - Please connect to a service to continue`

type Title struct {
	th    *material.Theme
	data  []richtext.SpanStyle
	ready bool
}

func NewTitle(th *material.Theme) *Title {
	return &Title{
		th: th,
		ready: false,
	}
}

func (t *Title) Fallback() ([]byte, bool)               { return []byte(fb), true }
func (t *Title) Fetch(c *client.Client) ([]byte, error) { return c.Title() }
func (t *Title) Clear()                                 { t.data = nil }
func (t *Title) Status(ready bool)                      { t.ready = ready }
func (t *Title) Style() richtext.SpanStyle {
	return richtext.SpanStyle{
		Size: unit.Sp(16),
	}
}

func (t *Title) Set(data interface{}) { 
	if d, ok := data.([]richtext.SpanStyle); ok {
		t.data = d
	}
}

func (t *Title) Layout(gtx layout.Context) layout.Dimensions {
	var state richtext.InteractiveText
	if !t.ready {
		return layout.Dimensions{}
	}
	return richtext.Text(&state, t.th.Shaper, t.data...).Layout(gtx)
}
