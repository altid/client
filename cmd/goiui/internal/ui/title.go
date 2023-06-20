package ui

import (
	"image/color"

	"gioui.org/layout"
	"gioui.org/text"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type title struct {
	svc *services.Service
	th *material.Theme
}

func (t *title) Layout(gtx layout.Context) layout.Dimensions {
	tmsg := []byte("Altid - Please connect to a service to continue")
	if t.svc != nil && t.svc.Ready {
		tmsg, _ = t.svc.Client.Title()
	}
	tt := material.Subtitle1(t.th, string(tmsg))
	tt.Color = color.NRGBA{R: 102, G: 51, B: 153, A: 255}
	tt.Alignment = text.Middle
	return tt.Layout(gtx)
}
