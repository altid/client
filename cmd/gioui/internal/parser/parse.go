package parser

import (
	"bytes"
	"image/color"
	"log"
	"strconv"

	"gioui.org/font"
	"gioui.org/font/gofont"
	"gioui.org/widget"
	"gioui.org/x/richtext"
	"github.com/altid/client"
	"github.com/altid/client/cmd/gioui/internal/services"
	"github.com/altid/libs/markup"
)

var ff = gofont.Collection()[0]

type Parser interface {
	Fallback() ([]byte, bool)
	Fetch(*client.Client) ([]byte, error)
	Clear()
	Set(interface{})
	Status(bool)
	Style() richtext.SpanStyle
}

func RebuildData(p Parser, svc *services.Service) {
	p.Clear()
	BuildData(p, svc)
}

func BuildData(p Parser, svc *services.Service) {
	var items []richtext.SpanStyle
	p.Status(false)
	fb, use := p.Fallback()
	if svc == nil || !svc.Ready {
		if !use {
			return
		}
	}
	gb, err := p.Fetch(svc.Client)
	if err != nil {
		if len(fb) == 0 {
			return
		}
		gb = fb
	}
	if(len(gb) < 1) {
		return
	}
	l := markup.NewLexer(gb)
	for {
		item := l.Next()
		d := p.Style()
		d.Font = ff.Font
		d.Color = color.NRGBA{A: 255}
		d.Content = string(item.Data)
		switch item.ItemType {
		case markup.EOF:
			items = append(items, d)
			p.Set(items)
			p.Status(true)
			return
		//case markup.ColorText:
		case markup.ColorTextBold:
			d.Font.Weight = font.Bold
		case markup.ColorTextEmphasis:
			d.Font.Style = font.Italic
		//case markup.ColorTextStrike:
		case markup.ColorTextStrong:
			d.Font.Weight = font.Bold
			d.Font.Style = font.Italic
		case markup.ColorCode:
			items[len(items)-1].Color = colorFromBytes(item.Data)
			continue
		case markup.BoldText:
			d.Font.Weight = font.Bold
		case markup.EmphasisText:
			d.Font.Style = font.Italic
		//case markup.NormalText:
		//case markup.StrikeText:
		case markup.StrongText:
			d.Font.Weight = font.Bold
			d.Font.Style = font.Italic
		case markup.ErrorText:
			log.Printf("Errored out: %s\n", item.Data)
			return
		}
		items = append(items, d)
	}
}

func RebuildTabs(p Parser, svcs *services.Services) {
	// Build and set each list of tabs
	for _, s := range svcs.List() {
		var list []*services.Tab
		if !s.Ready {
			continue
		}
		gb, err := p.Fetch(s.Client)
		if err != nil {
			log.Println(err)
			continue
		}
		// #foobar [123]
		for _, item := range bytes.Split(gb, []byte("\n")) {
			opening := bytes.LastIndex(item, []byte("["))
			closing := bytes.LastIndex(item, []byte("]"))
			// If we have neither of these values, either we're malformed or it's a blank line
			if closing == -1 || opening == -1 {
				continue
			}
			unread, _ := strconv.Atoi(string(item[opening+1:closing]))
			tab := &services.Tab{
				Name:   string(item[:opening-1]),
				Unread: unread,
				S: s,
			}
			// Stash the clickable and look up later, we don't care about the rest
			if t, ok := s.Tabs[tab.Name]; ok {
				tab.Click = t
			} else {
				tab.Click = &widget.Clickable{}
				s.Tabs[tab.Name] = tab.Click
			}
			list = append(list, tab)
		}
		p.Set(list)
	}
}

func colorFromBytes(in []byte) color.NRGBA {
	switch string(in) {
	case "red":
		return color.NRGBA{R:255, G: 0, B: 0, A: 255}
	case "green":
		return color.NRGBA{R: 255, G: 0, B: 120, A: 255}
	case "blue":
		return color.NRGBA{B: 255, R: 80, G: 100, A: 255}
	case "orange":
		return color.NRGBA{B: 0, R: 255, G: 165, A: 255}
	case "grey":
		return color.NRGBA{R: 120, G: 120, B: 120, A: 255}
	}
	return color.NRGBA{B: 0, R: 0, G: 0, A: 255}
}
