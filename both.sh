#!/bin/bash

echo "pebble build"
pebble build

echo "pebble install --phone 192.168.1.16 build/evilface.pbw"
pebble install --phone 192.168.1.16 build/evilface.pbw

