package ui

import (
	"gioui.org/layout"
	"gioui.org/unit"
	"gioui.org/widget/material"
	"gioui.org/x/richtext"
	"github.com/altid/client"
)

type Status struct {
	th      *material.Theme
	data    []richtext.SpanStyle
	ready   bool
	visible bool
}

func NewStatus(th *material.Theme) *Status {
	return &Status{
		th: th,
		ready: false,
	}
}

func (s *Status) Fallback() ([]byte, bool)               { return nil, false }
func (s *Status) Fetch(c *client.Client) ([]byte, error) { return c.Status() }
func (s *Status) Clear()                                 { s.data = nil }
func (s *Status) Status(ready bool)                      { s.ready = ready }
func (s *Status) Style() richtext.SpanStyle {
	return richtext.SpanStyle{
		Size: unit.Sp(14),
	}
}

func (s *Status) Set(data interface{}) {
	if d, ok := data.([]richtext.SpanStyle); ok {
		s.data = d
	}
}

func (s *Status) Layout(gtx layout.Context) layout.Dimensions {
	var state richtext.InteractiveText
	if !s.ready || !s.visible {
		return layout.Dimensions{}
	}

	return richtext.Text(&state, s.th.Shaper, s.data...).Layout(gtx)
}
