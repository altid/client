# Shell Client

There are two clients, one meant for use on POSIX systems and one for use on Plan9 (or with plan9port)

## Usage

`altid-rc ip` || `altid-sh ip`

Several commands are available while in a session

  - `/tabs` display all connected buffers
  - `/buffer [buffer name]`switch to named buffer
  - `/close [buffer name]` close named buffer
  - `/open [buffer name]` attempt to open/join named buffer
  - `/quit` disconnect and quit
  
