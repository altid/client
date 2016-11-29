# ubqt

http://tiamat.tsotech.com/pao inspired playground, using filesystem-based program state representations to allow clients to draw content based on a suitable toolkit. 

## Usage
Ubqt is designed as a front end for programs that act as a file server - that is, programs that communicate state and input using raw files.

It expects a layout such as the following:

```
/dir
 | title
 | status
 | main
 | tabbar
 | slideout
 | input
 | ctl
 | cmpl
```

Any file present will be drawn, with the exception of ctl and cmpl. The file server will display or not display these based on settings through the ctl file.

Once you have your file-server-program up and running, the general case would be to simply call `ubqt /path/to/my/dir`.


## Cairo backend
Eventually we'll have runtime configurations; but currently all colors in markup are hardcoded.  The files read are expected to be in markdown; and this will not change in the forseeable future.
I'm going to be trying out [libclipboard](https://github.com/jtanx/libclipboard) in this, on top of xcb

## Ncurses backend
To be implemented

## Nuklear backend
Possibly going to be implemented

## Android backend
This will end up as a sister project, if we get through the first few.

## Input schemes
I plan on a full vi-like stack where possible, including visual selection (save for images - more on that later), and a url highlighting function.
A leanback-friendly (gamepad, optional keyboard) will likely accompany the Nuklear backend

## -YOUR BACKEND HERE-
include [src/ubqt.h](https://github.com/halfwit/ubqt/blob/master/src/ubqt.h), drop your functions in to plugin/foo, add a plugin/foo/foo.mk; and you should be able to build your target backend with a simple `make foo` 
