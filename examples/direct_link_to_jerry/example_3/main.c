#include <string.h>
#include "jerryscript.h"

int
main (int argc, char *argv[])
{
  const jerry_char_t script_1[] = "var s = 'Hello, World!';";
  const jerry_char_t script_2[] = "print (s);";

  /* Initialize engine */
  jerry_init (JERRY_INIT_EMPTY);

  jerry_value_t eval_ret;

  /* Evaluate script1 */
  eval_ret = jerry_eval (script_1,
                         strlen ((const char *) script_1),
                         false);

  /* Free JavaScript value, returned by eval */
  jerry_release_value (eval_ret);

  /* Evaluate script2 */
  eval_ret = jerry_eval (script_2,
                         strlen ((const char *) script_2),
                         false);

  /* Free JavaScript value, returned by eval */
  jerry_release_value (eval_ret);

  /* Cleanup engine */
  jerry_cleanup ();

  return 0;
}
