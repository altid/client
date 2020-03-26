# client

This includes libraries for writing Altid clients, as well as the clients themselves. In cmd/, you will find examples using each library; expressed as fully working, end-user clients.

Additionally, there are simple shell example clients to showcase the ease with which a client can be implemented in this system!

[![GoDoc](https://godoc.org/github.com/golang/gddo?status.svg)](https://godoc.org/github.com/altid/client) ![Tests](https://github.com/altid/server/workflows/Tests/badge.svg) [![Go Report Card](https://goreportcard.com/badge/github.com/altid/server)](https://goreportcard.com/report/github.com/altid/client) [![License](http://img.shields.io/:license-mit-blue.svg)](http://doge.mit-license.org)

## Libs

Libs exist currently for clients written in several languages

### Go

`go get github.com/altid/client`

### Flutter

Add the following to your pubspec.yml

```
# pubspec.yml
dependancies:
  altid_mobile: ^0.0.1
```

And run

`flutter pub get`

### C

Work will be done to pull the useful bits out into an importable library.

## Clients

Example clients are listed below, and can be found in the cmd/ directory

### altid-cli

Example client which connects over 9p to a 9pd server

`go get github.com/altid/client/cmd/altid-cli`

For more information, see https://github.com/altid/client/blob/master/cmd/altid-cli/README.md

### mobile

Example iOS/Android client which connects over 9p to a 9pd server

```
cd cmd/mobile
flutter build <ios|android>
flutter install
```

#### iOS

On your mobile device, on first-run you'll be presented with an error. Navigate to general > device management, and "trust developer"

### rc, sh

Example shell clients which connect over 9p to a 9pd server
These require plan9port (or in the case of rc, to be running on plan9) and are very simple. They read from stdin, write to stdout.
