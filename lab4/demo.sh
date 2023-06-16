#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer jason &
./reader 192.168.3.10 8888 /dev/mydev
