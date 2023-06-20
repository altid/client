package ui

import (
	"image"
	"image/color"

	//"log"

	"gioui.org/layout"
	"gioui.org/op/clip"
	"gioui.org/op/paint"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type list struct {
	s      *services.Services
	items  []*listItem
	th     *material.Theme
	button *widget.Clickable
	hide   bool
}

type listItem struct {
	svc       *services.Service
	th        *material.Theme
	tabs      []*services.Tab
	clickable *widget.Clickable
	// Visibility for toggling item
	visible bool
}

// List of layout items, which are themselves nested
func (l *list) Layout(gtx layout.Context) layout.Dimensions {
	frame := layout.Flex{}
	frame.Axis = layout.Vertical
	frame.Spacing = layout.SpaceEnd
	bt := material.Button(l.th, l.button, "+")
	bt.Background = color.NRGBA{B: 99, G: 30, R: 0, A: 255}
	bt.Color = color.NRGBA{B: 255, G: 255, R: 255, A: 255}
	for range l.button.Clicks() {
		l.hide = !l.hide
	}

	// Draw our button, menu
	if l.hide {
		return frame.Layout(gtx,
			layout.Rigid(func(gtx layout.Context) layout.Dimensions {
				return bt.Layout(gtx)
			}),
		)
	}
	list := &widget.List{}
	list.Axis = layout.Vertical
	li := material.List(l.th, list)
	return frame.Layout(gtx,
		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			return bt.Layout(gtx)
		}),
		layout.Rigid(func(gtx layout.Context) layout.Dimensions {
			// Background colour
			menu := clip.Rect{
				Min: image.Pt(0, 4),
				Max: image.Pt(400, gtx.Constraints.Max.Y-50),
			}.Push(gtx.Ops)
			paint.ColorOp{
				Color: color.NRGBA{R: 220, G: 220, B: 220, A: 0xff},
			}.Add(gtx.Ops)
			paint.PaintOp{}.Add(gtx.Ops)
			menu.Pop()
			return li.Layout(gtx, len(l.items), l.layoutItem)
		}),
	)
}

func (l *list) layoutItem(gtx layout.Context, index int) layout.Dimensions {
	tt := material.Subtitle1(l.th, l.items[index].svc.Name)
	insets := layout.Inset{
		Top:    4,
		Bottom: 4,
		Left:   4,
		Right:  4,
	}
	for range l.items[index].clickable.Clicks() {
		if !l.items[index].svc.Ready {
			l.s.Select(l.items[index].svc.Name)
			l.items[index].visible = true
			return material.Loader(l.th).Layout(gtx)
		} else {
			// Toggle visibility of group
			l.items[index].visible = !l.items[index].visible
		}
	}
	if !l.items[index].svc.Ready {
		tt.Color = color.NRGBA{R: 010, G: 010, B: 010, A: 0xff}
		tt.Text = tt.Text + " [connect]"
		return insets.Layout(gtx, func(gtx layout.Context) layout.Dimensions {
			return l.items[index].clickable.Layout(gtx, tt.Layout)
		})
	}
	// Border as well
	l.items[index].tabs = l.items[index].svc.Tabs()
	return insets.Layout(gtx, func(gtx layout.Context) layout.Dimensions {
		d := layout.Flex{
			Axis: layout.Vertical,
			Spacing: layout.SpaceAround,
		}.Layout(gtx,
			layout.Rigid(func(gtx layout.Context) layout.Dimensions {
				tt.Color = color.NRGBA{R: 40, G: 40, B: 40, A: 255}
				if l.items[index].visible {
					tt.Text = tt.Text + " -"
				} else {
					tt.Text = tt.Text + " +"
				}
				return l.items[index].clickable.Layout(gtx, tt.Layout)
			}),
			layout.Flexed(1, func(gtx layout.Context) layout.Dimensions {
				list := &widget.List{}
				list.Axis = layout.Vertical
				ll := material.List(l.th, list)
				return ll.Layout(gtx, len(l.items[index].tabs), l.items[index].entry)
			}),
		)
		d.Size = image.Point{
			X: gtx.Constraints.Max.X,
			Y: gtx.Constraints.Max.Y,
		}
		return d
	})

}
func (li *listItem) entry(gtx layout.Context, index int) layout.Dimensions {
	if !li.visible {
		return layout.Dimensions{}
	}
	// #foobar [123], we switch to a flex row here
	st := material.Body1(li.th, li.tabs[index].Name)
	return st.Layout(gtx)
}
