package ui

import (
	"strings"

	"gioui.org/layout"
	"gioui.org/widget"
	"gioui.org/widget/material"
	"github.com/altid/client/cmd/goiui/internal/services"
)

type aside struct {
	svc *services.Service
	th *material.Theme
	items []string
	visible bool
}

// Fetch the list of items, build into list type
func (a *aside) Layout(gtx layout.Context) layout.Dimensions {
	if a.svc == nil || ! a.svc.Ready || !a.visible {
		return layout.Dimensions{}
	}
	abytes, _ := a.svc.Client.Aside()
	a.items = strings.Fields(string(abytes))
	if len(a.items) == 0 {
		return layout.Dimensions{}
	}
	list := &widget.List{}
	list.Axis = layout.Vertical
	as := material.List(a.th, list)
	return as.Layout(gtx, len(a.items), a.layoutItem)
}

func (a *aside) layoutItem(gtx layout.Context, index int) layout.Dimensions {
	st := material.Body1(a.th, a.items[index])
	return st.Layout(gtx)
}