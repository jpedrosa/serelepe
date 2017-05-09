#include <string.h>
#include <libc.h>
#include "jerryscript.h"

int
main (int argc, char *argv[])
{
  const jerry_char_t script[] = "print ('Hello, Serelepe!');";
  size_t script_size = strlen ((const char *) script);

  bool ret_value = jerry_run_simple (script, script_size, JERRY_INIT_EMPTY);

  return (ret_value ? 0 : 1);
}
