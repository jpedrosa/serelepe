/* Copyright JS Foundation and other contributors, http://js.foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jerryscript.h"
#include "jerryscript-port.h"
#include "jerryscript-port-default.h"

/**
 * Maximum command line arguments number
 */
#define JERRY_MAX_COMMAND_LINE_ARGS (64)

/**
 * Maximum size of source code
 */
#define JERRY_BUFFER_SIZE (1048576)

/**
 * Maximum size of snapshots buffer
 */
#define JERRY_SNAPSHOT_BUFFER_SIZE (JERRY_BUFFER_SIZE / sizeof (uint32_t))

/**
 * Standalone Jerry exit codes
 */
#define JERRY_STANDALONE_EXIT_CODE_OK   (0)
#define JERRY_STANDALONE_EXIT_CODE_FAIL (1)

/**
 * Context size of the SYNTAX_ERROR
 */
#define SYNTAX_ERROR_CONTEXT_SIZE 2

static uint8_t buffer[ JERRY_BUFFER_SIZE ];

static const uint32_t *
read_file (const char *file_name,
           size_t *out_size_p)
{
  FILE *file;
  if (!strcmp ("-", file_name))
  {
    file = stdin;
  }
  else
  {
    file = fopen (file_name, "r");
    if (file == NULL)
    {
      jerry_port_log (JERRY_LOG_LEVEL_ERROR, "Error: failed to open file: %s\n", file_name);
      return NULL;
    }
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
  return (const uint32_t *) buffer;
} /* read_file */

/**
 * Provide the 'assert' implementation for the engine.
 *
 * @return true - if only one argument was passed and the argument is a boolean true.
 */
static jerry_value_t
assert_handler (const jerry_value_t func_obj_val __attribute__((unused)), /**< function object */
                const jerry_value_t this_p __attribute__((unused)), /**< this arg */
                const jerry_value_t args_p[], /**< function arguments */
                const jerry_length_t args_cnt) /**< number of function arguments */
{
  if (args_cnt == 1
      && jerry_value_is_boolean (args_p[0])
      && jerry_get_boolean_value (args_p[0]))
  {
    return jerry_create_boolean (true);
  }
  else
  {
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "Script Error: assertion failed\n");
    exit (JERRY_STANDALONE_EXIT_CODE_FAIL);
  }
} /* assert_handler */

/**
 * Provide the 'gc' implementation for the engine.
 *
 * @return undefined.
 */
static jerry_value_t
gc_handler (const jerry_value_t func_obj_val __attribute__((unused)), /**< function object */
            const jerry_value_t this_p __attribute__((unused)), /**< this arg */
            const jerry_value_t args_p[] __attribute__((unused)), /**< function arguments */
            const jerry_length_t args_cnt __attribute__((unused))) /**< number of function arguments */
{
  jerry_gc ();
  return jerry_create_undefined ();
} /* gc_handler */

/**
 * Provide the 'print' implementation for the engine.
 *
 * The routine converts all of its arguments to strings and outputs them using
 * 'printf'.
 *
 * The NUL character is output as "\u0000", other code points are output using
 * "%c" format argument.
 *
 * @return undefined - if all arguments could be converted to strings,
 *         error - otherwise.
 */
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

static void
print_usage (const char *name)
{
  printf ("Usage: %s [OPTION]... [FILE]...\n"
          "Try '%s --help' for more information.\n",
          name,
          name);
} /* print_usage */

static void
print_help (const char *name)
{
  printf ("Usage: %s [OPTION]... [FILE]...\n"
          "\n"
          "Options:\n"
          "  -h, --help\n"
          "  -v, --version\n"
          "  --mem-stats\n"
          "  --mem-stats-separate\n"
          "  --parse-only\n"
          "  --show-opcodes\n"
          "  --show-regexp-opcodes\n"
          "  --start-debug-server\n"
          "  --save-snapshot-for-global FILE\n"
          "  --save-snapshot-for-eval FILE\n"
          "  --save-literals-list-format FILE\n"
          "  --save-literals-c-format FILE\n"
          "  --exec-snapshot FILE\n"
          "  --log-level [0-3]\n"
          "  --abort-on-fail\n"
          "  --no-prompt\n"
          "\n",
          name);
} /* print_help */

/**
 * Check whether an error is a SyntaxError or not
 *
 * @return true - if param is SyntaxError
 *         false - otherwise
 */
static bool
jerry_value_is_syntax_error (jerry_value_t error_value) /**< error value */
{
  assert (jerry_is_feature_enabled (JERRY_FEATURE_ERROR_MESSAGES));

  if (!jerry_value_is_object (error_value))
  {
    return false;
  }

  jerry_value_t prop_name = jerry_create_string ((const jerry_char_t *)"name");
  jerry_value_t error_name = jerry_get_property (error_value, prop_name);
  jerry_release_value (prop_name);

  if (jerry_value_has_error_flag (error_name)
      || !jerry_value_is_string (error_name))
  {
    return false;
  }

  jerry_size_t err_str_size = jerry_get_string_size (error_name);
  jerry_char_t err_str_buf[err_str_size];

  jerry_size_t sz = jerry_string_to_char_buffer (error_name, err_str_buf, err_str_size);
  jerry_release_value (error_name);

  if (sz == 0)
  {
    return false;
  }

  if (!strcmp ((char *) err_str_buf, "SyntaxError"))
  {
    return true;
  }

  return false;
} /* jerry_value_is_syntax_error */

/**
 * Convert string into unsigned integer
 *
 * @return converted number
 */
static uint32_t
str_to_uint (const char *num_str_p) /**< string to convert */
{
  assert (jerry_is_feature_enabled (JERRY_FEATURE_ERROR_MESSAGES));

  uint32_t result = 0;

  while (*num_str_p != '\0')
  {
    assert (*num_str_p >= '0' && *num_str_p <= '9');

    result *= 10;
    result += (uint32_t) (*num_str_p - '0');
    num_str_p++;
  }

  return result;
} /* str_to_uint */

/**
 * Print error value
 */
static void
print_unhandled_exception (jerry_value_t error_value) /**< error value */
{
  assert (jerry_value_has_error_flag (error_value));

  jerry_value_clear_error_flag (&error_value);
  jerry_value_t err_str_val = jerry_value_to_string (error_value);
  jerry_size_t err_str_size = jerry_get_string_size (err_str_val);
  jerry_char_t err_str_buf[256];

  if (err_str_size >= 256)
  {
    const char msg[] = "[Error message too long]";
    err_str_size = sizeof (msg) / sizeof (char) - 1;
    memcpy (err_str_buf, msg, err_str_size);
  }
  else
  {
    jerry_size_t sz = jerry_string_to_char_buffer (err_str_val, err_str_buf, err_str_size);
    assert (sz == err_str_size);
    err_str_buf[err_str_size] = 0;

    if (jerry_is_feature_enabled (JERRY_FEATURE_ERROR_MESSAGES) && jerry_value_is_syntax_error (error_value))
    {
      uint32_t err_line = 0;
      uint32_t err_col = 0;

      /* 1. parse column and line information */
      for (uint32_t i = 0; i < sz; i++)
      {
        if (!strncmp ((char *) (err_str_buf + i), "[line: ", 7))
        {
          i += 7;

          char num_str[8];
          uint32_t j = 0;

          while (i < sz && err_str_buf[i] != ',')
          {
            num_str[j] = (char) err_str_buf[i];
            j++;
            i++;
          }
          num_str[j] = '\0';

          err_line = str_to_uint (num_str);

          if (strncmp ((char *) (err_str_buf + i), ", column: ", 10))
          {
            break; /* wrong position info format */
          }

          i += 10;
          j = 0;

          while (i < sz && err_str_buf[i] != ']')
          {
            num_str[j] = (char) err_str_buf[i];
            j++;
            i++;
          }
          num_str[j] = '\0';

          err_col = str_to_uint (num_str);
          break;
        }
      } /* for */

      if (err_line != 0 && err_col != 0)
      {
        uint32_t curr_line = 1;

        bool is_printing_context = false;
        uint32_t pos = 0;

        /* 2. seek and print */
        while (buffer[pos] != '\0')
        {
          if (buffer[pos] == '\n')
          {
            curr_line++;
          }

          if (err_line < SYNTAX_ERROR_CONTEXT_SIZE
              || (err_line >= curr_line
                  && (err_line - curr_line) <= SYNTAX_ERROR_CONTEXT_SIZE))
          {
            /* context must be printed */
            is_printing_context = true;
          }

          if (curr_line > err_line)
          {
            break;
          }

          if (is_printing_context)
          {
            jerry_port_log (JERRY_LOG_LEVEL_ERROR, "%c", buffer[pos]);
          }

          pos++;
        }

        jerry_port_log (JERRY_LOG_LEVEL_ERROR, "\n");

        while (--err_col)
        {
          jerry_port_log (JERRY_LOG_LEVEL_ERROR, "~");
        }

        jerry_port_log (JERRY_LOG_LEVEL_ERROR, "^\n");
      }
    }
  }

  jerry_port_log (JERRY_LOG_LEVEL_ERROR, "Script Error: %s\n", err_str_buf);
  jerry_release_value (err_str_val);
} /* print_unhandled_exception */

/**
 * Register a JavaScript function in the global object.
 */
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
    jerry_port_log (JERRY_LOG_LEVEL_WARNING, "Warning: failed to register '%s' method.", name_p);
    print_unhandled_exception (result_val);
  }

  jerry_release_value (result_val);
} /* register_js_function */

/**
 * Check whether JerryScript has a requested feature enabled or not. If not,
 * print a warning message.
 *
 * @return the status of the feature.
 */
static bool
check_feature (jerry_feature_t feature, /**< feature to check */
               const char *option) /**< command line option that triggered this check */
{
  if (!jerry_is_feature_enabled (feature))
  {
    jerry_port_default_set_log_level (JERRY_LOG_LEVEL_WARNING);
    jerry_port_log (JERRY_LOG_LEVEL_WARNING, "Ignoring '%s' option because this feature is disabled!\n", option);
    return false;
  }
  return true;
} /* check_feature */

/**
 * Check whether a usage-related condition holds. If not, print an error
 * message, print the usage, and terminate the application.
 */
static void
check_usage (bool condition, /**< the condition that must hold */
             const char *name, /**< name of the application (argv[0]) */
             const char *msg, /**< error message to print if condition does not hold */
             const char *opt) /**< optional part of the error message */
{
  if (!condition)
  {
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "%s", msg);
    if (opt != NULL)
    {
      jerry_port_log (JERRY_LOG_LEVEL_ERROR, "%s", opt);
    }
    jerry_port_log (JERRY_LOG_LEVEL_ERROR, "\n");

    print_usage (name);
    exit (JERRY_STANDALONE_EXIT_CODE_FAIL);
  }
} /* check_usage */

int
main (int argc,
      char **argv)
{
  if (argc > JERRY_MAX_COMMAND_LINE_ARGS)
  {
    jerry_port_log (JERRY_LOG_LEVEL_ERROR,
                    "Error: too many command line arguments: %d (JERRY_MAX_COMMAND_LINE_ARGS=%d)\n",
                    argc,
                    JERRY_MAX_COMMAND_LINE_ARGS);

    return JERRY_STANDALONE_EXIT_CODE_FAIL;
  }

  const char *file_names[JERRY_MAX_COMMAND_LINE_ARGS];
  int files_counter = 0;

  jerry_init_flag_t flags = JERRY_INIT_EMPTY;

  const char *exec_snapshot_file_names[JERRY_MAX_COMMAND_LINE_ARGS];
  int exec_snapshots_count = 0;

  bool is_parse_only = false;
  bool is_save_snapshot_mode = false;
  bool is_save_snapshot_mode_for_global_or_eval = false;
  const char *save_snapshot_file_name_p = NULL;

  bool is_save_literals_mode = false;
  bool is_save_literals_mode_in_c_format_or_list = false;
  const char *save_literals_file_name_p = NULL;

  bool is_repl_mode = false;
  bool no_prompt = false;

  for (int i = 1; i < argc; i++)
  {
    if (!strcmp ("-h", argv[i]) || !strcmp ("--help", argv[i]))
    {
      print_help (argv[0]);
      return JERRY_STANDALONE_EXIT_CODE_OK;
    }
    else if (!strcmp ("-v", argv[i]) || !strcmp ("--version", argv[i]))
    {
      printf ("Version: %d.%d%s\n", JERRY_API_MAJOR_VERSION, JERRY_API_MINOR_VERSION, JERRY_COMMIT_HASH);
      return JERRY_STANDALONE_EXIT_CODE_OK;
    }
    else if (!strcmp ("--mem-stats", argv[i]))
    {
      if (check_feature (JERRY_FEATURE_MEM_STATS, argv[i]))
      {
        jerry_port_default_set_log_level (JERRY_LOG_LEVEL_DEBUG);
        flags |= JERRY_INIT_MEM_STATS;
      }
    }
    else if (!strcmp ("--mem-stats-separate", argv[i]))
    {
      if (check_feature (JERRY_FEATURE_MEM_STATS, argv[i]))
      {
        jerry_port_default_set_log_level (JERRY_LOG_LEVEL_DEBUG);
        flags |= JERRY_INIT_MEM_STATS_SEPARATE;
      }
    }
    else if (!strcmp ("--parse-only", argv[i]))
    {
      is_parse_only = true;
    }
    else if (!strcmp ("--show-opcodes", argv[i]))
    {
      if (check_feature (JERRY_FEATURE_PARSER_DUMP, argv[i]))
      {
        jerry_port_default_set_log_level (JERRY_LOG_LEVEL_DEBUG);
        flags |= JERRY_INIT_SHOW_OPCODES;
      }
    }
    else if (!strcmp ("--show-regexp-opcodes", argv[i]))
    {
      if (check_feature (JERRY_FEATURE_REGEXP_DUMP, argv[i]))
      {
        jerry_port_default_set_log_level (JERRY_LOG_LEVEL_DEBUG);
        flags |= JERRY_INIT_SHOW_REGEXP_OPCODES;
      }
    }
    else if (!strcmp ("--start-debug-server", argv[i]))
    {
      if (check_feature (JERRY_FEATURE_DEBUGGER, argv[i]))
      {
        flags |= JERRY_INIT_DEBUGGER;
      }
    }
    else if (!strcmp ("--save-snapshot-for-global", argv[i])
             || !strcmp ("--save-snapshot-for-eval", argv[i]))
    {
      check_usage (i + 1 < argc, argv[0], "Error: no file specified for ", argv[i]);
      check_usage (save_snapshot_file_name_p == NULL, argv[0], "Error: snapshot file name already specified", NULL);

      if (check_feature (JERRY_FEATURE_SNAPSHOT_SAVE, argv[i++]))
      {
        is_save_snapshot_mode = true;
        is_save_snapshot_mode_for_global_or_eval = !strcmp ("--save-snapshot-for-global", argv[i - 1]);
        save_snapshot_file_name_p = argv[i];
      }
    }
    else if (!strcmp ("--exec-snapshot", argv[i]))
    {
      check_usage (i + 1 < argc, argv[0], "Error: no file specified for ", argv[i]);

      if (check_feature (JERRY_FEATURE_SNAPSHOT_EXEC, argv[i++]))
      {
        assert (exec_snapshots_count < JERRY_MAX_COMMAND_LINE_ARGS);
        exec_snapshot_file_names[exec_snapshots_count++] = argv[i];
      }
    }
    else if (!strcmp ("--save-literals-list-format", argv[i])
             || !strcmp ("--save-literals-c-format", argv[i]))
    {
      check_usage (i + 1 < argc, argv[0], "Error: no file specified for ", argv[i]);
      check_usage (save_literals_file_name_p == NULL, argv[0], "Error: literal file name already specified", NULL);

      if (check_feature (JERRY_FEATURE_SNAPSHOT_SAVE, argv[i++]))
      {
        is_save_literals_mode = true;
        is_save_literals_mode_in_c_format_or_list = !strcmp ("--save-literals-c-format", argv[i - 1]);
        save_literals_file_name_p = argv[i];
      }
    }
    else if (!strcmp ("--log-level", argv[i]))
    {
      check_usage (i + 1 < argc, argv[0], "Error: no level specified for ", argv[i]);
      check_usage (strlen (argv[i + 1]) == 1 && argv[i + 1][0] >= '0' && argv[i + 1][0] <= '3',
                   argv[0], "Error: wrong format for ", argv[i]);

      jerry_port_default_set_log_level ((jerry_log_level_t) (argv[++i][0] - '0'));
    }
    else if (!strcmp ("--abort-on-fail", argv[i]))
    {
      jerry_port_default_set_abort_on_fail (true);
    }
    else if (!strcmp ("--no-prompt", argv[i]))
    {
      no_prompt = true;
    }
    else if (!strcmp ("-", argv[i]))
    {
      file_names[files_counter++] = argv[i];
    }
    else if (!strncmp ("-", argv[i], 1))
    {
      check_usage (false, argv[0], "Error: unrecognized option: %s\n", argv[i]);
    }
    else
    {
      file_names[files_counter++] = argv[i];
    }
  }

  if (is_save_snapshot_mode)
  {
    check_usage (files_counter == 1,
                 argv[0], "Error: --save-snapshot-* options work with exactly one script", NULL);
    check_usage (exec_snapshots_count == 0,
                 argv[0], "Error: --save-snapshot-* and --exec-snapshot options can't be passed simultaneously", NULL);
  }

  if (is_save_literals_mode)
  {
    check_usage (files_counter == 1,
                argv[0], "Error: --save-literals-* options work with exactly one script", NULL);
  }

  if (files_counter == 0
      && exec_snapshots_count == 0)
  {
    is_repl_mode = true;
  }

#ifndef CONFIG_DISABLE_ES2015_PROMISE_BUILTIN
  jerry_port_default_jobqueue_init ();
#endif /* !CONFIG_DISABLE_ES2015_PROMISE_BUILTIN */
  jerry_init (flags);

  register_js_function ("assert", assert_handler);
  register_js_function ("gc", gc_handler);
  register_js_function ("print", print_handler);

  jerry_value_t ret_value = jerry_create_undefined ();

  if (jerry_is_feature_enabled (JERRY_FEATURE_SNAPSHOT_EXEC))
  {
    for (int i = 0; i < exec_snapshots_count; i++)
    {
      size_t snapshot_size;
      const uint32_t *snapshot_p = read_file (exec_snapshot_file_names[i], &snapshot_size);

      if (snapshot_p == NULL)
      {
        ret_value = jerry_create_error (JERRY_ERROR_COMMON, (jerry_char_t *) "Snapshot file load error");
      }
      else
      {
        ret_value = jerry_exec_snapshot (snapshot_p,
                                         snapshot_size,
                                         true);
      }

      if (jerry_value_has_error_flag (ret_value))
      {
        break;
      }
    }
  }

  if (!jerry_value_has_error_flag (ret_value))
  {
    for (int i = 0; i < files_counter; i++)
    {
      size_t source_size;
      const jerry_char_t *source_p = (jerry_char_t *) read_file (file_names[i], &source_size);

      if (source_p == NULL)
      {
        ret_value = jerry_create_error (JERRY_ERROR_COMMON, (jerry_char_t *) "Source file load error");
        break;
      }

      if (!jerry_is_valid_utf8_string (source_p, (jerry_size_t) source_size))
      {
        ret_value = jerry_create_error (JERRY_ERROR_COMMON, (jerry_char_t *) ("Input must be a valid UTF-8 string."));
        break;
      }

      if (is_save_snapshot_mode || is_save_literals_mode)
      {
        static uint32_t snapshot_save_buffer[ JERRY_SNAPSHOT_BUFFER_SIZE ];

        if (is_save_snapshot_mode)
        {
          size_t snapshot_size = jerry_parse_and_save_snapshot ((jerry_char_t *) source_p,
                                                                source_size,
                                                                is_save_snapshot_mode_for_global_or_eval,
                                                                false,
                                                                snapshot_save_buffer,
                                                                JERRY_SNAPSHOT_BUFFER_SIZE);
          if (snapshot_size == 0)
          {
            ret_value = jerry_create_error (JERRY_ERROR_COMMON, (jerry_char_t *) "Snapshot saving failed!");
          }
          else
          {
            FILE *snapshot_file_p = fopen (save_snapshot_file_name_p, "w");
            fwrite (snapshot_save_buffer, sizeof (uint8_t), snapshot_size, snapshot_file_p);
            fclose (snapshot_file_p);
          }
        }

        if (!jerry_value_has_error_flag (ret_value) && is_save_literals_mode)
        {
          const size_t literal_buffer_size = jerry_parse_and_save_literals ((jerry_char_t *) source_p,
                                                                            source_size,
                                                                            false,
                                                                            snapshot_save_buffer,
                                                                            JERRY_SNAPSHOT_BUFFER_SIZE,
                                                                            is_save_literals_mode_in_c_format_or_list);
          if (literal_buffer_size == 0)
          {
            ret_value = jerry_create_error (JERRY_ERROR_COMMON, (jerry_char_t *) "Literal saving failed!");
          }
          else
          {
            FILE *literal_file_p = fopen (save_literals_file_name_p, "w");
            fwrite (snapshot_save_buffer, sizeof (uint8_t), literal_buffer_size, literal_file_p);
            fclose (literal_file_p);
          }
        }
      }
      else
      {
        ret_value = jerry_parse_named_resource ((jerry_char_t *) file_names[i],
                                                strlen (file_names[i]),
                                                source_p,
                                                source_size,
                                                false);

        if (!jerry_value_has_error_flag (ret_value) && !is_parse_only)
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
  }

  if (is_repl_mode)
  {
    const char *prompt = !no_prompt ? "jerry> " : "";
    bool is_done = false;

    while (!is_done)
    {
      uint8_t *source_buffer_tail = buffer;
      size_t len = 0;

      printf ("%s", prompt);

      /* Read a line */
      while (true)
      {
        if (fread (source_buffer_tail, 1, 1, stdin) != 1 && len == 0)
        {
          is_done = true;
          break;
        }
        if (*source_buffer_tail == '\n')
        {
          break;
        }
        source_buffer_tail ++;
        len ++;
      }
      *source_buffer_tail = 0;

      if (len > 0)
      {
        /* Evaluate the line */
        jerry_value_t ret_val_eval = jerry_eval (buffer, len, false);

        if (!jerry_value_has_error_flag (ret_val_eval))
        {
          /* Print return value */
          const jerry_value_t args[] = { ret_val_eval };
          jerry_value_t ret_val_print = print_handler (jerry_create_undefined (),
                                                       jerry_create_undefined (),
                                                       args,
                                                       1);
          jerry_release_value (ret_val_print);
#ifndef CONFIG_DISABLE_ES2015_PROMISE_BUILTIN
          jerry_release_value (ret_val_eval);
          ret_val_eval = jerry_port_default_jobqueue_run ();

          if (jerry_value_has_error_flag (ret_value))
          {
            print_unhandled_exception (ret_value);
          }
#endif /* !CONFIG_DISABLE_ES2015_PROMISE_BUILTIN */
        }
        else
        {
          print_unhandled_exception (ret_val_eval);
        }

        jerry_release_value (ret_val_eval);
      }
    }
  }

  int ret_code = JERRY_STANDALONE_EXIT_CODE_OK;

  if (jerry_value_has_error_flag (ret_value))
  {
    print_unhandled_exception (ret_value);

    ret_code = JERRY_STANDALONE_EXIT_CODE_FAIL;
  }
#ifndef CONFIG_DISABLE_ES2015_PROMISE_BUILTIN
  else
  {
    jerry_release_value (ret_value);
    ret_value = jerry_port_default_jobqueue_run ();

    if (jerry_value_has_error_flag (ret_value))
    {
      print_unhandled_exception (ret_value);
      ret_code = JERRY_STANDALONE_EXIT_CODE_FAIL;
    }
  }
#endif /* !CONFIG_DISABLE_ES2015_PROMISE_BUILTIN */
  jerry_release_value (ret_value);
  jerry_cleanup ();

  return ret_code;
} /* main */
