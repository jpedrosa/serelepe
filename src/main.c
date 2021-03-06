
#include <string.h>

#include "jerryscript.h"
#include "jerryscript-port.h"

/**
 * Maximum size of source code / snapshots buffer
 */
#define JERRY_BUFFER_SIZE (1048576)

/**
 * Standalone Jerry exit codes
 */
#define JERRY_STANDALONE_EXIT_CODE_OK   (0)
#define JERRY_STANDALONE_EXIT_CODE_FAIL (1)

static uint8_t buffer[ JERRY_BUFFER_SIZE ];

static const uint8_t *
read_file (const char *file_name,
           size_t *out_size_p)
{
  FILE *file = fopen (file_name, "r");
  if (file == NULL)
  {
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "Error: failed to open file: %s\n", file_name);
    return NULL;
  }

  size_t bytes_read = fread (buffer, 1u, sizeof (buffer), file);
  if (!bytes_read)
  {
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "Error: failed to read file: %s\n", file_name);
    fclose (file);
    return NULL;
  }

  fclose (file);

  *out_size_p = bytes_read;
  return (const uint8_t *) buffer;
} /* read_file */

static void
print_help (char *name)
{
  printf ("Usage: %s [OPTION]... [FILE]...\n"
          "\n"
          "Options:\n"
          "  -h, --help\n"
          "\n",
          name);
} /* print_help */

int
main (int argc,
      char **argv)
{
  if (argc <= 1 || (argc == 2 && (!strcmp ("-h", argv[1]) || !strcmp ("--help", argv[1]))))
  {
    print_help (argv[0]);
    return JERRY_STANDALONE_EXIT_CODE_OK;
  }

  jerry_init (JERRY_INIT_EMPTY);
  jerry_value_t ret_value = jerry_create_undefined ();

  for (int i = 1; i < argc; i++)
  {
    const char *file_name = argv[i];
    size_t source_size;

    const jerry_char_t *source_p = read_file (file_name, &source_size);

    if (source_p == NULL)
    {
      ret_value = jerry_create_error (JERRY_ERROR_COMMON, (jerry_char_t *) "");
      break;
    }
    else
    {
      ret_value = jerry_parse (source_p, source_size, false);

      if (!jerry_value_has_error_flag (ret_value))
      {
        jerry_value_t func_val = ret_value;
        ret_value = jerry_run (func_val);
        jerry_release_value (func_val);
      }
    }

    if (jerry_value_has_error_flag (ret_value))
    {
      break;
    }

    jerry_release_value (ret_value);
    ret_value = jerry_create_undefined ();
  }

  int ret_code = JERRY_STANDALONE_EXIT_CODE_OK;

  if (jerry_value_has_error_flag (ret_value))
  {
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "Unhandled exception: Script Error!\n");
    ret_code = JERRY_STANDALONE_EXIT_CODE_FAIL;
  }

  jerry_release_value (ret_value);
  jerry_cleanup ();

  return ret_code;
} /* main */
