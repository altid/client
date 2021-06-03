package main

import (
	"errors"
	"flag"
	"log"
	"os"

	"github.com/altid/client"
	//"github.com/altid-client/drivers/tcell"
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

	//ui := cl.Register(tcell.Driver)
}
