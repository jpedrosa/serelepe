#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jerryscript.h"

int
main (int argc, char *argv[]) {

  // JERRY_FEATURE_CPOINTER_32_BIT, /**< 32 bit compressed pointers */
  // JERRY_FEATURE_ERROR_MESSAGES, /**< error messages */
  // JERRY_FEATURE_JS_PARSER, /**< js-parser */
  // JERRY_FEATURE_MEM_STATS, /**< memory statistics */
  // JERRY_FEATURE_PARSER_DUMP, /**< parser byte-code dumps */
  // JERRY_FEATURE_REGEXP_DUMP, /**< regexp byte-code dumps */
  // JERRY_FEATURE_SNAPSHOT_SAVE, /**< saving snapshot files */
  // JERRY_FEATURE_SNAPSHOT_EXEC, /**< executing snapshot files */
  // JERRY_FEATURE_DEBUGGER, /**< debugging */
  // JERRY_FEATURE_VM_EXEC_STOP, /**< stopping ECMAScript execution */
  // JERRY_FEATURE__COUNT /**< number of features. NOTE: must be at the end of the list */

  printf("JERRY_FEATURE_CPOINTER_32_BIT: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_CPOINTER_32_BIT) ? "true" : "false");
  printf("JERRY_FEATURE_ERROR_MESSAGES: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_ERROR_MESSAGES) ? "true" : "false");
  printf("JERRY_FEATURE_JS_PARSER: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_JS_PARSER) ? "true" : "false");
  printf("JERRY_FEATURE_MEM_STATS: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_MEM_STATS) ? "true" : "false");
  printf("JERRY_FEATURE_PARSER_DUMP: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_PARSER_DUMP) ? "true" : "false");
  printf("JERRY_FEATURE_REGEXP_DUMP: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_REGEXP_DUMP) ? "true" : "false");
  printf("JERRY_FEATURE_SNAPSHOT_SAVE: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_SNAPSHOT_SAVE) ? "true" : "false");
  printf("JERRY_FEATURE_SNAPSHOT_EXEC: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_SNAPSHOT_EXEC) ? "true" : "false");
  printf("JERRY_FEATURE_DEBUGGER: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_DEBUGGER) ? "true" : "false");
  printf("JERRY_FEATURE_VM_EXEC_STOP: %s\n",
    jerry_is_feature_enabled(JERRY_FEATURE_VM_EXEC_STOP) ? "true" : "false");

  return 0;
}
