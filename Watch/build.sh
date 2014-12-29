#!/bin/bash

SCRIPT_PATH=`pwd`/$0
cd `dirname $SCRIPT_PATH`

PEBBLE_PHONE=$1

ruby ../configure.rb
cat src/js/libs/* src/js/main.js > src/js/pebble-js-app.js

pebble clean
pebble build
pebble install --phone $PEBBLE_PHONE
pebble logs --phone $PEBBLE_PHONE

cp build/Watch.pbw ~/Dropbox/Projects/Pebble/
