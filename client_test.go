package client

import (
	"io"
	"testing"
	"time"
)

func TestFeed(t *testing.T) {
	mc := NewMockClient("none")
	mc.Connect(1)
	mc.Attach()
	mc.Auth()

	f, err := mc.Feed()
	if err != nil {
		t.Error(err)
		return
	}

	go func() {
		for {
			b := make([]byte, MSIZE)

			_, err := f.Read(b)
			if err != nil && err != io.EOF {
				t.Error(err)
				return
			}
		}
	}()

	time.Sleep(time.Second * 5)
	f.Close()
}

func TestCommands(t *testing.T) {
	mc := NewMockClient("none")
	defer mc.Cleanup()

	if e := mc.Connect(1); e != nil {
		t.Error(e)
	}

	if e := mc.Attach(); e != nil {
		t.Error(e)
	}

	if e := mc.Auth(); e != nil {
		t.Error(e)
	}

	if _, e := mc.Input([]byte("Some text")); e != nil {
		t.Error(e)
	}

	if _, e := mc.Open("chicken"); e != nil {
		t.Error(e)
	}

	if _, e := mc.Close("chicken"); e != nil {
		t.Error(e)
	}
}
