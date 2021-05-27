package main

import (
	"errors"
	"flag"
	"fmt"
	"log"
	"os"

	"github.com/altid/client"
	"github.com/gdamore/tcell/v2"
)

var debug = flag.Bool("d", false, "enable debug output")
var addr = flag.String("s", "127.0.0.1", "address to connect to")
var port = flag.String("p", "564", "port to connect to")
var errBadArgs = errors.New("Incorrect arguments to command")

func main() {
	flag.Parse()

	if flag.Lookup("h") != nil {
		flag.Usage()
		os.Exit(0)
	}

	cl := client.NewClient(*addr, *port)

	withDebug := 0
	if *debug {
		withDebug = 1
	}

	if e := cl.Connect(withDebug); e != nil {
		log.Fatal(e)
	}

	// Ideally we would call auth here, when it's properly supported

	if e := cl.Attach(); e != nil {
		log.Fatal(e)
	}

	l, err := newListener(cl)
	if err != nil {
 		log.Fatal(err)
 	}	


	tcell.SetEncodingFallback(tcell.EncodingFallbackASCII)
	s, e := tcell.NewScreen()
 	if e != nil {
		fmt.Fprintf(os.Stderr, "%v\n", e)
		os.Exit(1)
	}

	if e = s.Init(); e != nil {
		fmt.Fprintf(os.Stderr, "%v\n", e)
		os.Exit(1)
	}

	s.SetStyle(tcell.StyleDefault.Foreground(tcell.ColorWhite).Background(tcell.ColorBlack))
	s.Clear()

	go func() {
		for {
			ev := s.PollEvent()
			switch ev := ev.(type) {
			case *tcell.EventKey:
				switch ev.Key() {
				case tcell.KeyEscape, tcell.KeyEnter:
					close(l.done)
					return
				case tcell.KeyCtrlL:
					s.Sync()
				}
			case *tcell.EventResize:
				s.Sync()
			}
		}
	}()


	for {
		select {
		case <-l.done:
			s.Fini()
			os.Exit(0)
		case p := <-l.data:
			draw(s, p)
		case e := <-l.err:
			log.Println(e)
		}
	}
}
