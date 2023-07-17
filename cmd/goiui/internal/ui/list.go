package ui

import (
	"fmt"
	"image"
	"image/color"
	"log"
	"sort"
	"strings"

	"gioui.org/layout"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type list struct {
	s      *services.Services
	items  items
	th     *material.Theme
	button *widget.Clickable
	hide   bool
}

type listItem struct {
	svc       *services.Service
	th        *material.Theme
	tabs      tabs
	clickable *widget.Clickable
	visible   bool
	set       func(req string) error
}

// Make it sortable
type tabs []*services.Tab

func (t tabs) Len() int           { return len(t) }
func (t tabs) Swap(i, j int)      { t[i], t[j] = t[j], t[i] }
func (t tabs) Less(i, j int) bool { return strings.Compare(t[i].Name, t[j].Name) <= 0 }

type items []*listItem

func (t items) Len() int           { return len(t) }
func (t items) Swap(i, j int)      { t[i], t[j] = t[j], t[i] }
func (t items) Less(i, j int) bool { return strings.Compare(t[i].svc.Name, t[j].svc.Name) <= 0 }

func (t items) update(s *services.Services, th *material.Theme) {
	for _, item := range s.List() {
		if t.contains(item.Name) {
			continue
		}
		li := &listItem{
			svc:       item,
			th:        th,
			clickable: &widget.Clickable{},
		}
		t = append(t, li)
	}
	sort.Sort(t)
}

func (t items) contains(item string) bool {
	for _, d := range t {
		if d.svc.Name == item {
			return true
		}
	}
	return false
}

// List of layout items, which are themselves nested
func (l *list) Layout(gtx layout.Context) layout.Dimensions {
	list := &widget.List{}
	list.Axis = layout.Vertical
	li := material.List(l.th, list)
	// TODO: Make sidebar scrollable
	return li.Layout(gtx, len(l.items), l.layoutItem)
}

func (l *list) layoutItem(gtx layout.Context, index int) layout.Dimensions {
	tt := material.Subtitle1(l.th, l.items[index].svc.Name)
	tt.TextSize = unit.Sp(16)
	insets := layout.UniformInset(4)
	for range l.items[index].clickable.Clicks() {
		if !l.items[index].svc.Ready {
			// At least log the error
			l.items[index].visible = true
			if e := l.s.Select(l.items[index].svc.Name); e != nil {
				l.items[index].visible = false
				log.Printf("Unable to switch/enable service %v", e)
			}
		} else {
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
	t := l.items[index].svc.Tabs()
	sort.Sort(tabs(t))
	l.items[index].tabs = t
	return insets.Layout(gtx, func(gtx layout.Context) layout.Dimensions {
		return layout.Flex{
			Axis:    layout.Vertical,
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
			layout.Rigid(func(gtx layout.Context) layout.Dimensions {
				if l.items[index].visible {
					list := &widget.List{}
					list.Axis = layout.Vertical
					ll := material.List(l.th, list)
					// back reference so we can set the right service
					l.items[index].set = l.s.Select
					d := ll.Layout(gtx, len(l.items[index].tabs), l.items[index].entry)
					d.Size = image.Point{
						X: 400,
						Y: int(unit.Sp(32)) * (len(l.items[index].tabs)),
					}
					return d
				}
				return layout.Dimensions{}
			}),
		)
	})

}
func (li *listItem) entry(gtx layout.Context, index int) layout.Dimensions {
	if !li.visible {
		return layout.Dimensions{}
	}
	for range li.tabs[index].Click.Clicks() {
		li.set(li.svc.Name)
		li.svc.Buffer(li.tabs[index].Name)
	}
	// We'd like to paint this differently in the future, but for now just draw it as a string
	data := fmt.Sprintf("%s [%d]", li.tabs[index].Name, li.tabs[index].Unread)
	tt := material.Body2(li.th, data)
	tt.MaxLines = 1
	return li.tabs[index].Click.Layout(gtx, tt.Layout)
}
