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

const usage = `
Commands are entered simply by typing a leading slash
All other input is sent to the input channel of the current buffer.
Commands are:
/quit				# exit
/buffer <target>	# swap to named buffer, if it exists
/open <target>		# open and swap to named buffer
/close <target>		# close named buffer
/link <to> <from>	# close current buffer and replace with named buffer
/title		# print the title of the current buffer
/aside		# print the aside data for the current buffer
/status		# print the status of the current buffer
/tabs		# display a list of all connected buffers
/notify		# display any pending notifications and clear them
`

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

	l := newListener(cl)
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
