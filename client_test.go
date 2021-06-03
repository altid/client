package client

import (
	"testing"
)

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
}
