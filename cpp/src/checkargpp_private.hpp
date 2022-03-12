// SPDX-License-Identifier: MIT
// Copyright (c) 2013-2021 brainpower <brainpower at mailbox dot org>

#include "checkargpp.hpp"
#include "config.h"

#include <algorithm>
#include <locale>

namespace checkarg {

std::string str_to_upper(const std::string &src);

struct Opt {
  ::CAValueType value_type = CAValueType::CA_VT_NONE;
  char sopt                = 0;
  std::function<int(CheckArg *const, const std::string &, const std::string &)> cb;
  std::string help;
  std::string value;
  std::string value_name;
};

class CheckArgPrivate {
private:
  CheckArgPrivate(CheckArg *const ca, const std::string &appname);
  CheckArgPrivate(
    CheckArg *const ca, const std::string &appname, const std::string &desc);
  CheckArgPrivate(
    CheckArg *const ca, const std::string &appname, const std::string &desc,
    const std::string &appendix);

  int arg(const std::string &arg);
  int arg_long(const std::string &arg);
  int arg_short(const std::string &arg);
  int call_cb(const std::string &arg);


  int ca_error(int eno, const char *info = "!", ...) const;

  static std::map<int, std::string> errors;


  std::map<std::string, Opt> valid_args;
  std::map<char, std::string> short2long;

  std::vector<std::string> pos_args;

  CheckArg *const parent;

  std::string appname;
  std::string descr;
  std::string appendix;
  bool pos_arg_sep = false;
  bool cleared     = true;
  std::string usage_line;
  std::string posarg_help_descr, posarg_help_usage;
  std::string callname;
  // std::string _argv0;

  // state
  std::string next_is_val_of;

  friend class ::CheckArg;
  friend int
  checkarg::show_autohelp(CheckArg *const, const std::string &, const std::string &);
};

}  // namespace checkarg

#if not HAS_VASPRINTF
extern int vasprintf(char **strp, const char *format, va_list ap);
#endif
