#!/bin/sh

usage() {
	echo "usage: $0 path"
	exit 1
}

# Make sure we have ctrl and event at a bare minimum
if ! test -e "$1"/ctrl; then
	printf 'No %s/ctl, exiting\n' "$1"
	exit 1
fi
if ! test -e "$1"/event; then
	printf 'No %s/event, exiting\n' "$1"
	exit 1
fi

if test -e "$1"/feed; then
	tail -f "$1"/feed &
elif test -e "$1"/document; then
	cat "$1"/buffer
fi

# TODO: snarf the pid of tail here to kill in /buffer

# Main event loop read
while :; do
	read -r line
	case $line
	/buffer*) ;;
	*) echo $line >> "$1"/input
	esac
done
