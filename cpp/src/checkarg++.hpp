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

#ifndef CHECKARG_HPP
#define CHECKARG_HPP

#define CA_VER_STRING "1.2.0"

#include <memory> // shared_ptr
#include <string>
#include <map>
#include <vector>
#include <functional>

#ifndef NDEBUG
#define CA_PRINTERR
#endif


// forward declarations
class CheckArg;
typedef std::shared_ptr<CheckArg> CheckArgPtr;
typedef CheckArg* const CheckArgRPtr;
class CheckArgPrivate;
typedef std::shared_ptr<CheckArgPrivate> CheckArgPrivatePtr;

// autohelp callback
namespace checkarg{
  int show_autohelp(CheckArgRPtr, const std::string &, const std::string &);
};

// the checkarg class
class CheckArg {

public:
  // c'tors
  CheckArg(const int argc, char** argv, const std::string &appname);
  CheckArg(const int argc, char** argv, const std::string &appname, const std::string &desc);
  CheckArg(const int argc, char** argv, const std::string &appname, const std::string &desc, const std::string &appendix);

  // pre-parse add functions
  int add(const char sopt, const std::string &lopt, const std::string &help, bool has_val=false);
  int add(const char sopt, const std::string &lopt, std::function<int(CheckArgRPtr, const std::string &, const std::string &)> cb, const std::string &help, bool has_val=false);
  int add(const std::string &lopt, const std::string &help, bool has_val=false);
  int add(const std::string &lopt, std::function<int(CheckArgRPtr,const std::string &, const std::string &)> cb, const std::string &help, bool has_val=false);

  int add_autohelp();

  // do parse!
  int parse();

  // set some autohelp strings
  void set_posarg_help(const std::string &usage, const std::string &descr );
  void set_usage_line(const std::string &str);

  // getters
  std::string argv0();
  std::vector<std::string> pos_args() const;
  std::string value(const std::string &arg) const;

  // check if an arg was given on command line
  bool isset(const std::string &arg) const;

  // print autohelp or usage line
  void show_help();
  void show_usage();

  // get error string for errno
  static std::string str_err(const int errno);

private:

  CheckArgPrivatePtr p;

  /**
   * \brief function which will be the callback for '-\-help'
   * if add_autohelp() was called
   * \see add_autohelp()
   */
  friend int checkarg::show_autohelp(CheckArgRPtr, const std::string &, const std::string &);
};

// return codes
enum CAError{
  CA_ALLOK=0,
  CA_ERROR,
  CA_INVARG,
  CA_INVVAL,
  CA_MISSVAL,
  CA_CALLBACK,
};


#endif //CHECKARG_HPP
