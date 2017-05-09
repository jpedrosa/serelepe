#include <string.h>
#include "jerryscript.h"
#include <stdio.h>

int
main (int argc, char *argv[])
{
  printf("Hey");
  const jerry_char_t *script = "print ('Hello, World!');";
  //const jerry_char_t script[] = "print ('Hello, Serelepe!');";
  size_t script_size = strlen ((const char *) script);
  printf("size %lu", script_size);

  bool ret_value = jerry_run_simple (script, script_size, JERRY_INIT_EMPTY);
  printf("return %d", ret_value);

  return (ret_value ? 0 : 1);
}
