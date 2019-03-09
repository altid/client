# Cairo Pango Drawing Plugin

This plugin leverages Pango and Cairo to draw a traditional user interface, complete with images, a menu control interface, and in future releases will use your current GTK theme. (This will require the plugin to undergo a moderate rewrite, but it should not break any features.)

## Building

Add the following to config.mk

```
DRAW=9p

```

### Required libraries

 - [xkb](https://www.x.org/wiki/XKB)
 - [xcb](https://xcb.freedesktop.org)
 - [cairo](https://www.cairographics.org)
 - [pango](https://www.pango.org)

## Menu Control Interface

The Cairo Pango drawing plugin uses a traditional gtk-style menu interface to access the underlying control, but additionaly supports leading slash-style commands, common from chat services such as IRC and Discord. 
