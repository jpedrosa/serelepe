#include <string.h>
#include <stdio.h>
#include "jerryscript.h"

static jerry_value_t
print_handler (const jerry_value_t func_obj_val __attribute__((unused)), /**< function object */
               const jerry_value_t this_p __attribute__((unused)), /**< this arg */
               const jerry_value_t args_p[], /**< function arguments */
               const jerry_length_t args_cnt) /**< number of function arguments */
{
  jerry_value_t ret_val = jerry_create_undefined ();

  for (jerry_length_t arg_index = 0;
       jerry_value_is_undefined (ret_val) && arg_index < args_cnt;
       arg_index++)
  {
    jerry_value_t str_val = jerry_value_to_string (args_p[arg_index]);

    if (!jerry_value_has_error_flag (str_val))
    {
      if (arg_index != 0)
      {
        printf (" ");
      }

      jerry_size_t substr_size;
      jerry_length_t substr_pos = 0;
      jerry_char_t substr_buf[256];

      while ((substr_size = jerry_substring_to_char_buffer (str_val,
                                                            substr_pos,
                                                            substr_pos + 256,
                                                            substr_buf,
                                                            256)) != 0)
      {
        for (jerry_size_t chr_index = 0; chr_index < substr_size; chr_index++)
        {
          char chr = (char) substr_buf[chr_index];
          if (chr == '\0')
          {
            printf ("\\u0000");
          }
          else
          {
            printf ("%c", chr);
          }
        }

        substr_pos += substr_size;
      }

      jerry_release_value (str_val);
    }
    else
    {
      ret_val = str_val;
    }
  }

  printf ("\n");

  return ret_val;
} /* print_handler */


static jerry_value_t
trip_handler (const jerry_value_t func_obj_val __attribute__((unused)), /**< function object */
               const jerry_value_t this_p __attribute__((unused)), /**< this arg */
               const jerry_value_t args_p[], /**< function arguments */
               const jerry_length_t args_cnt) /**< number of function arguments */
{
  jerry_size_t req_sz = jerry_get_string_size (args_p[0]);
  jerry_char_t str_buf_p[req_sz + 1];
  jerry_string_to_char_buffer (args_p[0], str_buf_p, req_sz);
  str_buf_p[req_sz] = '\0';
  printf("never %d (%s)\n", req_sz, str_buf_p);
  return args_p[0];
} /* trip_handler */


static void
register_js_function (const char *name_p, /**< name of the function */
                      jerry_external_handler_t handler_p) /**< function callback */
{
  jerry_value_t global_obj_val = jerry_get_global_object ();

  jerry_value_t function_val = jerry_create_external_function (handler_p);
  jerry_value_t function_name_val = jerry_create_string ((const jerry_char_t *) name_p);
  jerry_value_t result_val = jerry_set_property (global_obj_val, function_name_val, function_val);

  jerry_release_value (function_name_val);
  jerry_release_value (function_val);
  jerry_release_value (global_obj_val);

  if (jerry_value_has_error_flag (result_val))
  {
    // jerry_port_log (JERRY_LOG_LEVEL_WARNING, "Warning: failed to register '%s' method.", name_p);
    printf("Warning: failed to register '%s' method.", name_p);
    // print_unhandled_exception (result_val);
  }

  jerry_release_value (result_val);
} /* register_js_function */

int
main (int argc, char *argv[])
{
  const jerry_char_t script[] = "print('ok'); print ('Hello, Serelepe!'); print('2'); print('tripped: ' + trip('tripping'));";
  size_t script_size = strlen ((const char *) script);

  jerry_init(JERRY_INIT_EMPTY);

  register_js_function ("print", print_handler);
  register_js_function ("trip", trip_handler);

  bool ret_value = jerry_eval(script, script_size, false);

//  bool ret_value = jerry_run_simple (script, script_size, JERRY_INIT_EMPTY);
  // bool ret_value = jerry_run_simple (script, script_size, JERRY_INIT_SHOW_OPCODES);

  return (ret_value ? 0 : 1);
}
