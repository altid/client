# Dynamic Client for Altid

This is a compile-time pluggable client for connecting to Altid services. 

## Backends

At the moment, the current target backends will be microui for drawing, 9p for transport, simple X11 for rendering, and vimode for seat. More will come in time, and work has been done on cairo/pango, as well as ncurses.

## Building

### Dependencies

- [libtask](https://swtch.com/libtask)
- [libixp](https://github.com/0intro/libixp)

## Notes

- Most everything happens in draw at this point
- threading is clumsy at the moment, one thread is started per service
