package ui

import (
	"bufio"
	"image"
	"strings"

	"gioui.org/layout"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"gioui.org/x/richtext"
	"github.com/altid/client"
)

/**
 * TODO: Switch to using a channel insted of this weird locking v.bytes crap
 */
const vfb = `Loading...`

type View struct {
	notify   func(string)
	bytes    chan []byte
	list     *widget.List
	data     [][]richtext.SpanStyle
	th       *material.Theme
	ready    bool
	first    bool
	isFeed   bool
}

func NewView(list *widget.List, th *material.Theme, notify func(string)) *View {
	return &View{
		notify:   notify,
		list:     list,
		th:       th,
		bytes:    make(chan []byte),
		ready:    false,
		isFeed:   false,
		first:    true,
	}
}

func (v *View) Fallback() ([]byte, bool) { return []byte(vfb), true }
func (v *View) Status(ready bool) { v.ready = ready }
func (v *View) Style() richtext.SpanStyle {
	return richtext.SpanStyle{
		Size: unit.Sp(14),
	}
}

func (v *View) Clear() {
	v.data = nil
	close(v.bytes)
	v.bytes = make(chan []byte)
	v.list = &widget.List{
		Scrollbar: widget.Scrollbar{},
		List: layout.List{
			Axis: layout.Vertical,
		},
	}
	v.first = true
}

func (v *View) Fetch(c *client.Client) (b []byte, err error) {
	if v.first {
		go v.getData(c)
		return
	}
	select {
	case c := <-v.bytes:
		b = append(c, b...)
		return b, nil
	default:
		return b, nil
	}

}

func (v *View) Set(data interface{}) {
	v.list.ScrollToEnd = v.isFeed
	if d, ok := data.([]richtext.SpanStyle); ok {
		var last = 0
		for i, span := range d {
			if strings.ContainsAny(span.Content, "\n") {
				v.data = append(v.data, d[last:i])
				last = i
			}
		}
		if len(d) > last {
			v.data = append(v.data, d[last:])
		}
	}
}

// Main view - build up each line with formatting
func (v *View) Layout(gtx layout.Context) layout.Dimensions {
	var state richtext.InteractiveText
	if !v.ready {
		d := layout.Dimensions{
			Size: image.Point{
				X: gtx.Constraints.Max.X,
				Y: gtx.Constraints.Max.Y,
			},
		}
		return d
	}
	vi := material.List(v.th, v.list)
	return vi.Layout(gtx, len(v.data), func(gtx layout.Context, index int) layout.Dimensions {
		d := richtext.Text(&state, v.th.Shaper, v.data[index]...).Layout(gtx)
		d.Size.X = gtx.Constraints.Max.X
		return d
	})
}

func (v *View) getData(c *client.Client) {
	v.first = false
	v.isFeed = true
	d, _ := c.Document()
	if len(d) > 0 {
		v.isFeed = false
		v.notify("feed")
		v.bytes <- d
		return
	}
	dd, e := c.Feed()
	if e != nil {
		return
	}
	rd := bufio.NewScanner(dd)
	rb := make([]byte, client.MSIZE)
	rd.Buffer(rb, client.MSIZE)
	for rd.Scan() {
		b := rd.Bytes()
		v.notify("feed")
		v.bytes <- b
	}
}
