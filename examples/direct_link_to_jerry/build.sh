#!/bin/sh

gcc main.c -I ../../vendor/jerryscript/jerry-core/include/ -L ../../vendor/jerryscript/build/lib/ -ljerry-core -lm -ljerry-port-default

