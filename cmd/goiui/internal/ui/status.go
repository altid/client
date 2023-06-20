package ui

import (
	"image/color"

	"gioui.org/layout"
	"gioui.org/text"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type status struct {
	svc *services.Service
	th *material.Theme
	visible bool
}

func (s *status) Layout(gtx layout.Context) layout.Dimensions {
	if s.svc == nil || !s.svc.Ready || !s.visible {
		return layout.Dimensions{}
	}
	sbytes, _ := s.svc.Client.Status()
	st := material.Subtitle2(s.th, string(sbytes))
	st.Color = color.NRGBA{R: 102, G: 51, B: 153, A: 255}
	st.Alignment = text.Start
	return st.Layout(gtx)
}
