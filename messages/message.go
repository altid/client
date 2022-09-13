package messages

import (
	"fmt"
)

type MsgType int
const (
	QuitMsg MsgType = iota
	NullMsg
	FeedMsg
	MainMsg
	StatusMsg
	ListMsg
	TabsMsg
	TitleMsg
	AsideMsg
	NotificationMsg
	BufferMsg
	OpenMsg
	LinkMsg
	CloseMsg
	SendMsg
)

const msgfmt = `%d:%s`
type Message struct {
	Data    []byte
	MsgType MsgType
}

func (m *Message) String() string {
	return fmt.Sprintf(msgfmt, m.MsgType, m.Data)
}
