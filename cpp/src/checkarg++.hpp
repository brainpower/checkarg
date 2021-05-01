// SPDX-License-Identifier: MIT
// Copyright (c) 2013-2021 brainpower <brainpower at mailbox dot org>

#ifndef CHECKARG_HPP
#define CHECKARG_HPP

#include <memory> // shared_ptr
#include <string>
#include <map>
#include <vector>
#include <functional>


// forward declarations
class CheckArg;

namespace checkarg {
  class CheckArgPrivate;

  // autohelp callback
  int show_autohelp(CheckArg *const, const std::string &, const std::string &);
};

// return codes
enum CAError {
  CA_ALLOK=0,
  CA_ERROR,
  CA_INVOPT,
  CA_INVVAL,
  CA_MISSVAL,
  CA_CALLBACK,
};

enum CAValueType {
  CA_VT_NONE = 0,
  CA_VT_REQUIRED,
  // CA_VT_OPTIONAL,
};

// the checkarg class
class CheckArg {

public:
  // c'tors
  CheckArg(const int argc, char** argv, const std::string &appname);
  CheckArg(const int argc, char** argv, const std::string &appname, const std::string &desc);
  CheckArg(const int argc, char** argv, const std::string &appname, const std::string &desc, const std::string &appendix);

  CheckArg(const std::vector<std::string> &argv, const std::string &appname);
  CheckArg(const std::vector<std::string> &argv, const std::string &appname, const std::string &desc);
  CheckArg(const std::vector<std::string> &argv, const std::string &appname, const std::string &desc, const std::string &appendix);

#ifdef HAS_STD_FILESYSTEM
  // these try to autodetect `appname` from basename(argv[0])
  CheckArg(const int argc, char**argv);
  explicit CheckArg(const std::vector<std::string> &argv);
#endif

  // default destructor needed for unique_ptr on incomplete CheckArgPrivate
  ~CheckArg();

  // pre-parse add functions
  int add(
    const char sopt,
    const std::string &lopt,
    const std::string &help,
    const CAValueType has_val = CA_VT_NONE
  );
  int add(
    const char sopt,
    const std::string &lopt,
    const std::string &help,
    const std::string &value_name,
    const CAValueType has_val = CA_VT_REQUIRED
  );
  int add(
    const char sopt,
    const std::string &lopt,
    std::function<int(CheckArg *const, const std::string &, const std::string &)> cb,
    const std::string &help,
    const CAValueType has_val = CA_VT_NONE
  );
  int add(
    const char sopt,
    const std::string &lopt,
    std::function<int(CheckArg *const, const std::string &, const std::string &)> cb,
    const std::string &help,
    const std::string &value_name,
    const CAValueType has_val = CA_VT_REQUIRED
  );
  int add(
    const std::string &lopt,
    const std::string &help,
    const CAValueType has_val = CA_VT_NONE
  );
  int add(
    const std::string &lopt,
    const std::string &help,
    const std::string &value_name,
    const CAValueType has_val = CA_VT_REQUIRED
  );
  int add(
    const std::string &lopt,
    std::function<int(CheckArg *const,const std::string &, const std::string &)> cb,
    const std::string &help,
    const CAValueType has_val = CA_VT_NONE
  );
  int add(
    const std::string &lopt,
    std::function<int(CheckArg *const,const std::string &, const std::string &)> cb,
    const std::string &help,
    const std::string &value_name,
    const CAValueType has_val = CA_VT_REQUIRED
  );

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
  std::string autohelp();
  std::string usage();

  // check if an arg was given on command line
  bool isset(const std::string &arg) const;

  // print autohelp or usage line
  void show_help();
  void show_usage();

  // get error string for errno
  static std::string str_err(const int errno);

private:

  std::unique_ptr<checkarg::CheckArgPrivate> p;

  /**
   * \brief function which will be the callback for '-\-help'
   * if add_autohelp() was called
   * \see add_autohelp()
   */
  friend int checkarg::show_autohelp(CheckArg *const, const std::string &, const std::string &);
};


#endif //CHECKARG_HPP
