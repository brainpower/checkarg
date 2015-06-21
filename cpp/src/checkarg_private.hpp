/*
 * Copyright (c) 2013-2015 brainpower <brainpower at mailbox dot org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
