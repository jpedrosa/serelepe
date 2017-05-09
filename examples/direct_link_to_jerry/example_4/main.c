#include <string.h>
#include "jerryscript.h"

int
main (int argc, char *argv[]) {
  const jerry_char_t str[] = "Hello, World!";
  const jerry_char_t script[] = "print (s);";

  /* Initializing JavaScript environment */
  jerry_init (JERRY_INIT_EMPTY);

  /* Getting pointer to the Global object */
  jerry_value_t global_object = jerry_get_global_object ();

  /* Constructing strings */
  jerry_value_t prop_name = jerry_create_string ((const jerry_char_t *) "s");
  jerry_value_t prop_value = jerry_create_string (str);

  /* Setting the string value as a property of the Global object */
  jerry_set_property (global_object, prop_name, prop_value);

  /* Releasing string values, as it is no longer necessary outside of engine */
  jerry_release_value (prop_name);
  jerry_release_value (prop_value);

  /* Releasing the Global object */
  jerry_release_value (global_object);

  /* Now starting script that would output value of just initialized field */
  jerry_value_t eval_ret = jerry_eval (script,
                                       strlen ((const char *) script),
                                       false);

  /* Free JavaScript value, returned by eval */
  jerry_release_value (eval_ret);

  /* Freeing engine */
  jerry_cleanup ();

  return 0;
}
