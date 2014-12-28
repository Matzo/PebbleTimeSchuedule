#!/bin/bash

SCRIPT_PATH=`pwd`/$0
cd `dirname $SCRIPT_PATH`

ruby ../configure.rb
cat src/js/libs/* src/js/_* > src/js/pebble-js-app.js

pebble clean
pebble build
pebble install
pebble logs
