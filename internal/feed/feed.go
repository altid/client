package feed

import (
	"io"
)

type Feed struct {
	Data chan []byte
	Done chan struct{}
}

func (f *Feed) Read(b []byte) (n int, err error) {
	select {
	case in := <-f.Data:
		n = copy(b, in)
		return
	case <-f.Done:
		return 0, io.EOF
	}
}

func (f *Feed) Close() error {
	f.Done <- struct{}{}
	return nil
}
