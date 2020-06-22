#ifndef APEX_JOURNAL_PRIORITY_HPP
#define APEX_JOURNAL_PRIORITY_HPP

#include <syslog.h>

namespace apex::journal {

enum class priority {
  emergency = LOG_EMERG,
  alert = LOG_ALERT,
  critical = LOG_CRIT,
  error = LOG_ERR,
  warning = LOG_WARNING,
  notice = LOG_NOTICE,
  info = LOG_INFO,
  debug = LOG_DEBUG,
};

} /* namespace apex::journal */

#endif /* APEX_JOURNAL_PRIORITY_HPP */
