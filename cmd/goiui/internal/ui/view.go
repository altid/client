package ui

import (
	"image"

	"gioui.org/layout"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type view struct {
	svc *services.Service
	// Scroll offset
	th  *material.Theme
}

// Main view - build up each line with formatting
func (v *view) Layout(gtx layout.Context) layout.Dimensions {
	// TODO: scrollable panel
	if v.svc == nil || !v.svc.Ready {
		d := layout.Dimensions{}
		d.Size = image.Point{
			X: gtx.Constraints.Max.X,
			Y: gtx.Constraints.Max.Y,
		}
		return d
	}
	return layout.Flex{
		Axis:    layout.Vertical,
		Spacing: layout.SpaceStart,
	}.Layout(gtx,
		layout.Rigid(
			func(gtx layout.Context) layout.Dimensions {
				list := &widget.List{}
				//list.ScrollToEnd = true
				list.Axis = layout.Vertical
				vi := material.List(v.th, list)
				return vi.Layout(gtx, len(v.svc.Data), func(gtx layout.Context, index int) layout.Dimensions {
					return v.svc.Data[index](gtx)
				})
			},
		),
	)
}
