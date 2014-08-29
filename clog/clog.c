/* clog.c -
 */

#include "clog.h"

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/* cl_log:
 */
void cl_log ( const char *domain,
              ClLogLevel level,
              const char *fname,
              int lineno,
              const char *format,
              ... )
{
  va_list args;
  va_start(args, format);
  cl_logv(domain, level, fname, lineno, format, args);
  va_end(args);
}



/* cl_logv:
 */
void cl_logv ( const char *domain,
               ClLogLevel level,
               const char *fname,
               int lineno,
               const char *format,
               va_list args )
{
  char *msg;
  vasprintf(&msg, format, args);
  fprintf(stderr, "%s:%s:%d: %s\n", domain, fname, lineno, msg);
  free(msg);
  if (level == CL_LOG_LEVEL_ERROR)
    abort();
}
