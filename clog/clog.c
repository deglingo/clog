/* clog.c -
 */

#include "clog.h"

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>



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
  msg = g_strdup_vprintf(format, args);
  fprintf(stderr, "%s:%s:%d: %s\n", domain, fname, lineno, msg);
  fflush(stderr);
  /* fprintf(stderr, "%s: %s\n", domain, msg); */
  g_free(msg);
  if (level == CL_LOG_LEVEL_ERROR)
    abort();
}



#define MAXCMDLINE 16

static void _add_option ( char **cmdline,
                          const char *option )
{
  int count;
  for (count = 0; cmdline[count]; count++);
  if (count >= (MAXCMDLINE-1))
    CL_ERROR("MAXCMDLINE reached");
  cmdline[count++] = strdup(option);
  cmdline[count] = NULL;
}



/* cl_backtrace:
 */
void cl_backtrace ( void )
{
  int p[2];
  pid_t pid;
  if (pipe(p) < 0)
    CL_ERROR("pipe() failed: %s", strerror(errno));
  pid = fork();
  if (pid < 0)
    {
      CL_ERROR("fork() failed: %s", strerror(errno));
    }
  else if (pid == 0)
    {
      char *cmdline[MAXCMDLINE] = { NULL, };
      char strpid[12]; /* let's assume 32bits pids */
      char exelink[64], *exepath;
      int c;
      sprintf(strpid, "%d", getppid());
      /* [fixme] get executable path */
      sprintf(exelink, "/proc/%d/exe", getppid());
      if (!(exepath = realpath(exelink, NULL)))
        CL_ERROR("could not get executable path: %s", strerror(errno));
      _add_option(cmdline, "gdb");
      _add_option(cmdline, "-q");
      _add_option(cmdline, "-batch");
      _add_option(cmdline, "-ex");
      _add_option(cmdline, "bt");
      _add_option(cmdline, exepath);
      _add_option(cmdline, strpid);
      fprintf(stderr, "CMD:");
      for (c = 0; cmdline[c]; c++)
        fprintf(stderr, " %s", cmdline[c]);
      fprintf(stderr, "\n");
      close(p[0]);
      dup2(p[1], 1);
      execvp("gdb", cmdline);
      CL_ERROR("execvp failed: %s", strerror(errno));
      abort();
    }
  else
    {
      int status;
      char buf[4096];
      close(p[1]);
      while (1)
        {
          ssize_t r;
          r = read(p[0], buf, 4095);
          if (r < 0) {
            CL_ERROR("read failed: %s", strerror(errno));
          } else if (r == 0) {
            break;
          } else {
            buf[r] = 0;
            fprintf(stderr, "%s", buf);
          }
        }
      close(p[0]);
      if (waitpid(pid, &status, 0) <= 0)
        CL_ERROR("waitpid failed: %s", strerror(errno));
      if (status != 0)
        CL_DEBUG("ERROR: gdb failed (%d)", status);
    }
}
