// SPDX-License-Identifier: MIT
// Copyright (c) 2013-2021 brainpower <brainpower at mailbox dot org>

#include "config.h"
#include "checkarg++.hpp"

class CheckArgPrivate {
private:
  CheckArgPrivate(CheckArgRPtr ca, const int argc, char** argv, const std::string &appname);
  CheckArgPrivate(CheckArgRPtr ca, const int argc, char** argv, const std::string &appname, const std::string &desc);
  CheckArgPrivate(CheckArgRPtr ca, const int argc, char** argv, const std::string &appname, const std::string &desc, const std::string &appendix);

  int arg(const std::string &arg);
  int arg_long(const std::string &arg);
  int arg_short(const std::string &arg);
  int call_cb(const std::string &arg);


  int ca_error(int eno, const char *info="!", ...) const;

  static std::map<int,std::string> errors;

  struct Opt{
    bool has_val = false;
    char sopt    = 0;
    std::string help;
    std::function<int(CheckArgRPtr, const std::string &,const std::string &)> cb;
    std::string value;
  };

  std::map<std::string,Opt>  valid_args;
  std::map<char,std::string> short2long;

  std::vector<std::string> pos_args;

  const int argc;
  char **   argv;
  CheckArgRPtr parent;

  std::string appname;
  std::string descr;
  std::string appendix;
  bool pos_arg_sep;
  std::string usage_line;
  std::string posarg_help_descr, posarg_help_usage;
  //std::string _argv0;

  // state
  std::string next_is_val_of;

  friend class CheckArg;
  friend int checkarg::show_autohelp(CheckArgRPtr, const std::string &, const std::string &);
};

#if not HAS_VASPRINTF
	extern int vasprintf(char** strp, const char* format, va_list ap);
#endif

