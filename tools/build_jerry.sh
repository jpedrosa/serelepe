#!/bin/sh
cd vendor/jerryscript

python tools/build.py --cpointer-32bit=on --mem-heap=2000000 --jerry-libc=off
