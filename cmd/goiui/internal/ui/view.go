package ui

import (
	"image"

	"gioui.org/layout"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"gioui.org/x/richtext"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type view struct {
	position layout.Position
	svc *services.Service
	list *widget.List
	th  *material.Theme
}

// Main view - build up each line with formatting
func (v *view) Layout(gtx layout.Context) layout.Dimensions {
	if v.svc == nil || !v.svc.Ready {
		d := layout.Dimensions{
			Size: image.Point{
				X: gtx.Constraints.Max.X,
				Y: gtx.Constraints.Max.Y,
			},
		}
		return d
	}
	// Set up our scroll
	if v.svc.HasFeed() {
		v.list.ScrollToEnd = true
	} else {
		v.list.ScrollToEnd = false
	}
	var state richtext.InteractiveText
	vi := material.List(v.th, v.list)
	return vi.Layout(gtx, len(v.svc.Data), func(gtx layout.Context, index int) layout.Dimensions {
		return richtext.Text(&state, v.th.Shaper, v.svc.Data[index]...).Layout(gtx)
	})
}
