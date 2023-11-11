package ui

import (
	"image"
	"image/color"

	"gioui.org/io/system"
	"gioui.org/layout"
	"gioui.org/op"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
	"gioui.org/widget/material"
)

type Layout struct {
	Ops *op.Ops
	th *material.Theme
}

func NewLayout(th *material.Theme) *Layout {
	return &Layout{
		Ops: &op.Ops{},
		th: th,
	}
}

func (l *Layout) Update(e system.FrameEvent, t *Title, a *Aside, s *Status, v *View, i *Input, li *List) {
	gtx := layout.NewContext(l.Ops, e)
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
					layout.Rigid(t.Layout),
					layout.Flexed(1, v.Layout),
					layout.Rigid(s.Layout),
					layout.Rigid(i.Layout),
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
			d := li.Layout(gtx)
			d.Size = image.Point{
				X: 400,
				Y: gtx.Constraints.Max.Y,
			}
			return d
		}),
	)
	//s.menu.Layout(gtx)
}
