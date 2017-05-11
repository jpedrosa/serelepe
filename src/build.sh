#!/bin/sh

gcc main.c -o ser -I ../vendor/jerryscript/jerry-core/include/ -I ../vendor/jerryscript/jerry-port/default/include/ -L ../vendor/jerryscript/build/lib/ -ljerry-core -lm -ljerry-port-default
