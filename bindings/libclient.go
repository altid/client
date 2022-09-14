package main

import "C"
import (
	"fmt"
	"time"

	"github.com/altid/client"
	"github.com/altid/client/messages"
)

var cmds chan string
var msgs chan *messages.Message

//export Start
func Start(address, port *C.char) {
	cmds = make(chan string, 10)
	msgs = make(chan *messages.Message, 20)
	addressPtr := C.GoString(address)
	portPtr := C.GoString(port)

	cl := client.NewClient(addressPtr, portPtr)

	if e := cl.Connect(false); e != nil {
		fmt.Printf("%s\n", e)
		return
	}

	if e := cl.Attach(); e != nil {
		fmt.Printf("%s\n", e)
		return
	}

	l, err := messages.NewListener(cl, msgs)
	if err != nil {
		fmt.Printf("%s\n", err)
		return
	}

	go func(l *messages.Listener, cmds chan string) {
		for cmd := range cmds {
			l.Handle(cmd)
		}
	}(l, cmds)
}

//export Input
func Input(input *C.char)  {
	in := C.GoString(input)
	select {
	case cmds <- in:
	case <-time.After(time.Second * 5):
	}
}

//export Next
func Next() *C.char {
	msg := <- msgs
	return C.CString(msg.String())
}

func listen(cl *client.Client, l *messages.Listener) {

}

func main() {}
