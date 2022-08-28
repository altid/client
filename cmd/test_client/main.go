package main

import (
	"bufio"
	"flag"

	"log"
	"os"

	"github.com/altid/client"
)

var debug = flag.Bool("d", false, "enable debug output")
var addr = flag.String("s", "127.0.0.1", "address to connect to")
var port = flag.String("p", "564", "port to connect to")

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
		log.Fatalf("connect error: %v", e)
	}

	// Ideally we would call auth here, when it's properly supported
	if e := cl.Attach(); e != nil {
		log.Fatalf("attach error: %v", e)
	}

	l, err := newListener(cl)
	if err != nil {
		log.Fatalf("listener error %s", err)
	}

	reader := bufio.NewReader(os.Stdin)
	for {
		// read by one line (enter pressed)
		s, err := reader.ReadString('\n')
		// check for errors
		if err != nil {
			log.Println("Error in read string", err)
		}
		handle(l, s)
	}
}
