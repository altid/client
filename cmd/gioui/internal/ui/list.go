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
	"gioui.org/x/richtext"
	"github.com/altid/client"
	"github.com/altid/client/cmd/gioui/internal/services"
)

type List struct {
	s      *services.Services
	th     *material.Theme
	button *widget.Clickable
	items  items
	ready  bool
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

func NewList(s *services.Services, th *material.Theme) *List {
	return &List{
		s: s,
		th: th,
		button: &widget.Clickable{},
	}
}

func (l *List) Fallback() ([]byte, bool) { return nil, false }
func (l *List) Clear() { }
func (l *List) Status(ready bool) { l.ready = ready }
func (l *List) Fetch(c *client.Client) ([]byte, error) { return c.Tabs() }
func (l *List) Style() richtext.SpanStyle {
	return richtext.SpanStyle{
		Size: unit.Sp(14),
	}
}

func (l *List) AddService(s *services.Services) {
	for _, service := range s.List() {
		if l.items.contains(service.Name) {
			continue
		}
		li := &listItem{
			svc: service,
			th: l.th,
			clickable: &widget.Clickable{},
			set: s.Select,
		}
		l.items = append(l.items, li)
	}
}

func (l *List) Set(data interface{}) {
	if d, ok := data.([]*services.Tab); ok {
		for _, item := range l.items {
			if item.svc.Name == d[0].S.Name {
				item.tabs = tabs(d)
				return
			}
		}
	}
}

// List of layout items, which are themselves nested
func (l *List) Layout(gtx layout.Context) layout.Dimensions {
	list := &widget.List{}
	list.Axis = layout.Vertical
	li := material.List(l.th, list)
	// TODO: Make sidebar scrollable
	return li.Layout(gtx, len(l.items), l.layoutItem)
}

func (l *List) layoutItem(gtx layout.Context, index int) layout.Dimensions {
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
			l.s.Notify("list")
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
	sort.Sort(l.items)
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
	sort.Sort(li.tabs)
	for range li.tabs[index].Click.Clicks() {
		li.set(li.svc.Name)
		li.svc.Buffer(li.tabs[index].Name)
	}
	//TODO: Process into an array of richtext.SpanStyle
	data := fmt.Sprintf("%s [%d]", li.tabs[index].Name, li.tabs[index].Unread)
	tt := material.Body2(li.th, data)
	tt.MaxLines = 1
	return li.tabs[index].Click.Layout(gtx, tt.Layout)
}

func (t items) contains(item string) bool {
	for _, d := range t {
		if d.svc.Name == item {
			return true
		}
	}
	return false
}
