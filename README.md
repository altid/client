# Linux Client for Ubqt

** This is pre-alpha, please do not expect anything of this **

http://tiamat.tsotech.com/pao inspired playground, using filesystem-based program state representations to allow clients to draw content based on a suitable toolkit. 

## Usage

`linux-client <addr>[:port] [<addr>[:port]...]`

The default port is 564 for historical reasons.

### Control

Depending on the backend and implementation details, the control methodology will vary; refer to the plugin documentation for further details.
Each service may describe its own set of available controls, but the default are as follows

 - `close <buffer name>` ends the session with the named buffer, removing its entry from `tabs`, as well as relaying the message to the underlying service; which should result in the path being removed from its runtime directory.
 - `open <buffer name>` attempts to begin a session for the named resource. For example, 
   - `open ##ubqt` sent to an IRC service will join that IRC channel
   - `open /path/to/some/document.pdf` sent to docfs will open the document for viewing
 - `buffer <buffer name>` will attempt to switch the client session to the named buffer. This will not notify the underlying service.
 - `quit` sends a quit message to the underlying service

Additionally, when used with 9p-server an additional set of control messages are made available. (coming soon)
 - `9p quit` terminates the 9p-server, as well as the client connection
 - `9p reload` hot restarts the 9p-server, maintaining the client connection

## Backends

Linux Client allows compile-time pluggable back ends for the drawing library, the input, and the transport method (such as a direct 9p connection, or using local files)

### Drawing

 - [Cairo](https://github.com/ubqt-systems/linux-client/tree/master/plugins/cairo/README.md)
 - Ncurses coming soon!
 - Nuklear coming soon!

### Input

 - [Vi-mode](https://github.com/ubqt-systems/linux-client/tree/master/plugins/vi/README.md)
 - Leanback coming soon!
 - Touch coming soon!

### Transport

 - [9p](https://github.com/ubqt-systems/linux-client/tree/master/plugins/9p/README.md)
 - Ssh coming soon!

## Building

Modify config.mk to fit your setup, and set which plugins you wish to target, then simply

`make && make install`

### Dependencies

Refer to the documentation for each plugin for the libraries they require to build.
Linux Client itself requires `make` and `gcc`

## Screenshots & Other Media

Coming Soon!
