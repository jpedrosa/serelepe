#!/bin/sh

#gcc -O2 -s -DNDEBUG main.c -I ../../../vendor/jerryscript/jerry-core/include/  -I ../../../vendor/jerryscript/jerry-port/default/include/ -L ../../../vendor/jerryscript/build/lib/ -ljerry-core -lm -ljerry-port-default

gcc main.c -I ../../../vendor/jerryscript/jerry-core/include/  -I ../../../vendor/jerryscript/jerry-port/default/include/ -L ../../../vendor/jerryscript/build/lib/ -ljerry-core -lm -ljerry-port-default
