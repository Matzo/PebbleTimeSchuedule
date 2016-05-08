#!/bin/bash

SCRIPT_PATH=`pwd`/$0
cd `dirname $SCRIPT_PATH`

PEBBLE_PHONE=$1

ruby ../configure.rb

pebble clean
pebble build
pebble install --phone $PEBBLE_PHONE
pebble logs --phone $PEBBLE_PHONE

cp build/Watch.pbw ~/Dropbox/Projects/Pebble/
