#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jerryscript.h"
#include "jerryscript-port.h"


int
main (int argc, char *argv[]) {

  double d = jerry_port_get_current_time();

  printf("jerry_port_get_current_time: %f", d);

  return 0;
}
