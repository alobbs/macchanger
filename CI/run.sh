#!/bin/sh

set -e -x

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

macc() {
    $DIR/run-valgrind.py $DIR/../src/macchanger "$@"
}

# Basic Valgrind
macc --version
macc --show
macc --list=red

# Virtual Interface
if ! ifconfig -a | grep dummy0: 2>&1 >/dev/null ; then
    modprobe dummy
    ip li add dummy0 type dummy0
fi

ip addr show dummy0

# Set MACs
macc --ending dummy0
macc --random dummy0
macc --random --bia dummy0
macc --mac 11:22:33:44:55:66 dummy0

# Hurray!
echo "Finished"
echo
