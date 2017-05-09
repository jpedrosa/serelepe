#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jerryscript.h"

static void
print_value (const jerry_value_t value)
{
  if (jerry_value_is_undefined (value))
  {
    printf ("undefined");
  }
  else if (jerry_value_is_null (value))
  {
    printf ("null");
  }
  else if (jerry_value_is_boolean (value))
  {
    if (jerry_get_boolean_value (value))
    {
      printf ("true");
    }
    else
    {
      printf ("false");
    }
  }
  /* Float value */
  else if (jerry_value_is_number (value))
  {
    printf ("number");
  }
  /* String value */
  else if (jerry_value_is_string (value))
  {
    /* Determining required buffer size */
    jerry_size_t req_sz = jerry_get_string_size (value);
    jerry_char_t str_buf_p[req_sz];

    jerry_string_to_char_buffer (value, str_buf_p, req_sz);
    str_buf_p[req_sz] = '\0';

    printf ("string: \"%s\"", (const char *) str_buf_p);
  }
  /* Object reference */
  else if (jerry_value_is_object (value))
  {
    printf ("[JS object]");
  }

  printf ("\n");
}


int
main (int argc, char *argv[]) {
  // const jerry_char_t str[] = "3 + 4";
  // const jerry_char_t str[] = "{abc: [8, 4, 1]}";
  const jerry_char_t str[] = "\"Lobo\"";

  /* Initializing JavaScript environment */
  jerry_init (JERRY_INIT_EMPTY);

  jerry_value_t eval_ret = jerry_eval (str,
                                       strlen ((const char *) str),
                                       false);

  print_value(eval_ret);

  /* Free JavaScript value, returned by eval */
  jerry_release_value (eval_ret);

  /* Freeing engine */
  jerry_cleanup ();

  return 0;
}
