# Vi-mode Seat Plugin

This plugin allows vi's modal style input, and visual selection of ranges of text. For editable fields, it also allows vi's verb and action language to describe edits.

## Building
Add the following to config.mk

`SEAT=vi`

No additional libraries are required at this time.

## Control Input

The vi-mode seat allows control messags to be written to the server, by means of a prefixing a leading slash to your message. This is a common practice on services such as IRC, and Discord. 

```
Examples of commands:

/open myfile
/close myfile
/9p quit
/9p restart
```
