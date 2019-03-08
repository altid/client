# 9p Transport Plugin

This plugin allows direct connection to a running 9p-server instance. 

## Building
Add the following to config.mk

```
TRANSPORT=9p

```

Required https://github.com/0intro/libixp

## Runtime Caveats

Linux Client is generally started with one or more addresses to connect to.
At the time of this writing, only a single connection is supported at a time, though future work will be done at some point to allow multiple connections to a single client.
This transport is performant, but will suffer latency issues.

## Additional Reading

Securing your 9p session (coming soon!)
