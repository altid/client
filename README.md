# ubqt
![What it looks like so far](https://pb.gehidore.net/GWfH.png)

** This is pre-alpha, please do not expect anything of this **

http://tiamat.tsotech.com/pao inspired playground, using filesystem-based program state representations to allow clients to draw content based on a suitable toolkit. 

## Usage
Ubqt is designed as a front end for programs that act as a file server - that is, programs that communicate state and input using raw files.

It expects a layout such as the following:

```
/dir
 | title
 | status
 | main
 | tabs
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

## Markdown
Ubqt is meant to translate markdown to backend-specific markup; examples of this will eventually follow. It's expected that the following characters will be escaped, `\\` 

Should be escaped | Don't need to be 
---------------------------|-------------------------
\\ backslash \`back tick | \(\) parentheses 
\# hash mark \* asterisk | \+ plus sign 
\[\] square brackets \_ underscore | \! exclamation 
\- minus sign \{\} curly braces | \. dot \[\]  \! exclamation

\`\`\` wraps a code block, but currently there's no github-esque 

## -YOUR BACKEND HERE-
include [src/ubqt.h](https://github.com/halfwit/ubqt/blob/master/src/ubqt.h), drop your functions in to plugin/foo, add a plugin/foo/foo.mk; and you should be able to build your target backend by modifying the appropriate flag in config.mk 
If you have any questions, insights, ideas; feel free to join #ubqt on freenode
