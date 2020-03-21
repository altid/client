package main

import (
	"errors"
	"flag"
	"log"
	"os"

	"github.com/altid/libs/client"
)

var debug = flag.Bool("d", false, "enable debug output")
var addr = flag.String("s", "127.0.0.1", "address to connect to")
var errBadArgs = errors.New("Incorrect arguments to command")

func main() {
	flag.Parse()

	if flag.Lookup("h") != nil {
		flag.Usage()
		os.Exit(0)
	}

	cl := client.NewClient(*addr)

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

	go l.listen()

	// Main loop
	for {
		select {
		case <-l.done:
			break
		case p := <-l.data:
			os.Stdout.Write(p)
		case e := <-l.err:
			log.Println(e)
		}
	}
}
