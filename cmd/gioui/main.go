package main

import (
	"context"
	"flag"
	"log"
	"os"
	"time"

	"gioui.org/app"
	"gioui.org/unit"
	"github.com/altid/client/cmd/gioui/internal/services"
	"github.com/altid/client/cmd/gioui/internal/session"
)

var debug = flag.Bool("d", false, "enable debug logging")

func main() {
	flag.Parse()
	if flag.Lookup("h") != nil {
		flag.Usage()
		os.Exit(0)
	}
	s := services.NewServices(*debug)
	ctx, cancel := context.WithTimeout(context.Background(), time.Second * 15)
	defer cancel()
	go s.Scan(ctx)
	go func() {
		w := app.NewWindow(
			app.Size(unit.Dp(800), unit.Dp(600)),
		)
		sess := session.NewSession(w, s)
		if e := sess.Run(ctx, *debug); e != nil {
			log.Fatal(e)
		}
		os.Exit(0)
	}()
	app.Main()
}
