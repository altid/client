package main

import (
	"bufio"
	"flag"

	"log"
	"os"

	"github.com/altid/client"
	"github.com/altid/client/slashcmd"
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
	if e := cl.Connect(*debug); e != nil {
		log.Fatalf("connect error: %v", e)
	}

	// Ideally we would call auth here, when it's properly supported
	if e := cl.Attach(); e != nil {
		log.Fatalf("attach error: %v", e)
	}

	writer := bufio.NewWriter(os.Stdout)
	l, err := slashcmd.NewListener(cl, writer)
	if err != nil {
		log.Fatalf("listener error %s", err)
	}

	reader := bufio.NewReader(os.Stdin)
	for {
		s, err := reader.ReadString('\n')
		if err != nil {
			log.Fatalf("reader error: %s", err)
		}
		l.Handle(s)
	}
}
