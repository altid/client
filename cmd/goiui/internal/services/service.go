package services

import (
	"bufio"
	"bytes"
	"fmt"
	"image/color"
	"io"
	"log"
	"sync"
	"strconv"

	"gioui.org/font"
	"gioui.org/font/gofont"
	"gioui.org/unit"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"gioui.org/x/richtext"
	"github.com/altid/client"
	"github.com/altid/libs/markup"
	"github.com/altid/libs/service/commander"
)

type Service struct {
	Client *client.Client
	// TODO: Some way to watch for new activity
	Name      string
	Ready     bool
	Notify    func(string)
	Data      [][]richtext.SpanStyle
	cmds      []*commander.Command
	tabs      map[string]*widget.Clickable
	addr      string
	port      int
	isRunning bool
	hasFeed   bool
	debug     bool
	sync.Mutex
}

type Tab struct {
	Name   string
	Unread int
	Click  *widget.Clickable
}

func (s *Service) HasFeed() bool {
	return s.hasFeed
}

func (s *Service) Connect() error {
	port := fmt.Sprintf("%d", s.port)
	s.Client = client.NewClient(s.addr, port)
	if e := s.Client.Connect(s.debug); e != nil {
		return e
	}
	if e := s.Client.Attach(); e != nil {
		return e
	}
	s.Ready = true
	// TODO: Handle commands to start Parse for other channels when we swap
	return nil
}

func (s *Service) Buffer(name string) {
	if _, e := s.Client.Buffer(name); e != nil {
		log.Println("attempting to go to buffer error: ", e)
	}
	s.Data = [][]richtext.SpanStyle{{{
		Content: "Loading",
		Color: color.NRGBA{A: 255},
		Size: unit.Sp(24),
	}}}
	s.Notify("buffer")
	s.Parse()
}

func (s *Service) Parse() {
	if !s.Ready {
		return
	}
	s.Lock()
	defer s.Unlock()
	s.Data = [][]richtext.SpanStyle{}
	th := material.NewTheme(gofont.Collection())
	d, _ := s.Client.Document()
	if len(d) > 0 {
		s.hasFeed = false
		fd := bytes.NewBuffer(d)
		s.toLines(fd, th)
		return
	}
	// Read in feed and handle it
	go func(s *Service) {
		fd, err := s.Client.Feed()
		if err != nil {
			s.Data = [][]richtext.SpanStyle{{{
				Content: "Not connected",
				Color: 	color.NRGBA{A: 255},
				Size: unit.Sp(24),
			}}}
			return
		}
		s.hasFeed = true
		s.toLines(fd, th)
	}(s)
}

func (s *Service) Tabs() []*Tab {
	list := []*Tab{}
	data, err := s.Client.Tabs()
	if err != nil {
		return list
	}
	// #foobar [123]
	for _, item := range bytes.Split(data, []byte("\n")) {
		opening := bytes.LastIndex(item, []byte("["))
		closing := bytes.LastIndex(item, []byte("]"))
		// If we have neither of these values, either we're malformed or it's a blank line
		if closing == -1 || opening == -1 {
			continue
		}
		unread, _ := strconv.Atoi(string(item[opening+1:closing]))
		tab := &Tab{
			Name:   string(item[:opening-1]),
			Unread: unread,
		}
		// Stash the clickable and look up later, we don't care about the rest
		if t, ok := s.tabs[tab.Name]; ok {
			tab.Click = t
		} else {
			tab.Click = &widget.Clickable{}
			s.tabs[tab.Name] = tab.Click
		}
		list = append(list, tab)
	}
	return list
}

func (s *Service) toLines(fd io.Reader, th *material.Theme) {
	c := make([]byte, client.MSIZE)
	font := gofont.Collection()[0]
	scanner := bufio.NewScanner(fd)
	scanner.Buffer(c, client.MSIZE)
	for scanner.Scan() {
		d := scanner.Bytes()
		s.addLine(th, d, font)
	}
}

func (s *Service) addLine(th *material.Theme, line []byte, font font.FontFace) {
	l := markup.NewLexer(line)
	var items []richtext.SpanStyle
	
	for {
		item := l.Next()
	LOOP:
		switch item.ItemType {
		case markup.EOF:
			s.Data = append(s.Data, items)
			s.Data = append(s.Data, []richtext.SpanStyle{{Content: "\n"}})
			s.Notify("feed")
			return
		case markup.NormalText:
			if len(item.Data) > 0 {
				d := richtext.SpanStyle{
					Color: color.NRGBA{A: 255},
					Content: string(item.Data),
					Size: unit.Sp(14),
					Font: font.Font,
				}
				items = append(items, d)
			}
		case markup.ColorText:
			d := richtext.SpanStyle{
				Content: string(item.Data),
				Size: unit.Sp(14),
				Font: font.Font,
			}
			item = l.Next()
			// If we don't have a colour code next, we're likely in a sub-format, catch those first
			if item.ItemType == markup.ColorCode {
				d.Color = colorFromBytes(item.Data)
				items = append(items, d)
			} else {
				goto LOOP
			}
		case markup.ErrorText:
			log.Printf("Error: %s", item.Data)
		default:
			log.Println("Markup TODO: ", string(item.Data))
		}
	}
}

func colorFromBytes(in []byte) color.NRGBA {
	switch string(in) {
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
