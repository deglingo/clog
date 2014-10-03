/* clog.h -
 */

#ifndef _CLOG_H_
#define _CLOG_H_

#include <stdarg.h>

typedef enum
  {
    CL_LOG_LEVEL_DEBUG = 1 << 0,
    CL_LOG_LEVEL_ERROR = 1 << 1,
  }
  ClLogLevel;

#define CL_LOG(lvl, msg...) do {                \
    cl_log(CL_LOG_DOMAIN, lvl,                  \
           __FILE__, __LINE__,                  \
           msg);                                \
  } while (0)

#define CL_DEBUG(msg...) do {                   \
    CL_LOG(CL_LOG_LEVEL_DEBUG, msg);            \
  } while (0)

#define CL_ERROR(msg...) do {                   \
    CL_LOG(CL_LOG_LEVEL_ERROR, msg);            \
  } while (0)

void cl_log ( const char *domain,
              ClLogLevel level,
              const char *fname,
              int lineno,
              const char *format,
              ... )
  __attribute__((format(printf, 5, 6)));

void cl_logv ( const char *domain,
               ClLogLevel level,
               const char *fname,
               int lineno,
               const char *format,
               va_list args );

#endif
