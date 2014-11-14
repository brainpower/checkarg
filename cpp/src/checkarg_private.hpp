/*
 * Copyright (C) 2013-2014 brainpower <brainpower at gulli dot com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "checkarg++.hpp"

#ifdef CA_PRINTERR
#include <cstdarg>
#include <iostream>
#endif

class CheckArgPrivate {
private:
  CheckArgPrivate(CheckArg* ca, const int argc, char** argv, const std::string &appname);
  CheckArgPrivate(CheckArg* ca, const int argc, char** argv, const std::string &appname, const std::string &desc);
  CheckArgPrivate(CheckArg* ca, const int argc, char** argv, const std::string &appname, const std::string &desc, const std::string &appendix);

  int arg(const std::string &arg);
  int arg_long(const std::string &arg);
  int arg_short(const std::string &arg);
  int call_cb(const std::string &arg);


  int ca_error(int eno, const std::string &info="!", ...) const;

  static std::map<int,std::string> errors;

  struct Opt{
    bool has_val = false;
    char sopt    = 0;
    std::string help;
    std::function<int(CheckArgPtr, const std::string &,const std::string &)> cb;
    std::string value;
  };

  std::map<std::string,Opt>  valid_args;
  std::map<char,std::string> short2long;

  std::vector<std::string> pos_args;

  const int argc;
  char **   argv;
  CheckArg* parent;

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
  friend int checkarg::show_autohelp(CheckArgPtr, const std::string &, const std::string &);
};

#if defined(__MINGW32__) || defined(__MINGW64__) || defined(_MSC_VER) || defined(__WIN32__)
  extern int vasprintf(char** strp, const char* format, va_list ap);
#endif
