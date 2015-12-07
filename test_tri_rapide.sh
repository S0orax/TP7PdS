#!/usr/bin/sh

dd if=/dev/urandom of=/tmp/alea bs=1048576 count=128
./tri-rapide /tmp/alea 1