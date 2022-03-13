// SPDX-License-Identifier: MIT
// Copyright (c) 2013-2021 brainpower <brainpower at mailbox dot org>

#include "checkargpp.hpp"
#include "checkargpp_private.hpp"

#if CA_PRINTERR
#include <cstdarg>
#endif

#include <iostream>
#include <sstream>
// #include <format>

#ifdef HAS_STD_FILESYSTEM
#include <filesystem>
namespace fs = std::filesystem;
#endif

using std::map;

using std::cout;
using std::endl;
using std::flush;

using std::string;
using std::stringstream;

using std::vector;

using checkarg::CheckArgPrivate;
using checkarg::str_to_upper;

/**
 * \class CheckArg
 * \brief a lightweight and easy to use command line parser
 *
 * It's inspired by ArgumentParser from python,
 * therefore, if you've used that one before, you'll find this one quite similar.
 * It is, intentionally, in no way a complete reimplementation or port of the former.
 *
 * The main goal is for it to be easy to understand and use.
 * Secondly, its intended to be non-intrusive and lightweight.
 * Therefore it pulls no 'external' dependencies and relies completely on the STL.
 *
 * That way it's pretty easy to integrate into an existing project,
 * because it's non-intrusive, you dont need to 'build your project around it'.
 *
 * Commandline options use the common so-called GNU style,
 * e.g. use double dashes and look something like these: <br>
 *
 *   program --option=value --quiet --option2 <value><br>
 *
 * Additionally, abreviations of those in so-called POSIX style can be added.
 * They look like this: <br>
 *
 *   program -o <value> -q -f <value>
 *
 * And they can be combined:
 *
 *   program -o <value> -qf <value>
 *   program -o <value> -qf<value>
 *
 * Note that above commands are equivalent.
 * This means after a value type option is found in a grouped option
 * the remainder is always considered to be the value,
 * even if you meant it to be an option.<br>
 * So these commands are not the same:
 *
 *   program -o <value> -qfi other_file
 *   program -o <value> -q -f other_file -i
 *
 * In the first command, 'i' is considered the value of -f,
 * other_file a positional argument, not the value of either -f or -i.
 *
 * In the docs I'll call the GNU style and POSIX style options "long" and "short"
 * options, respectively.
 *
 * Old style or traditional style options are *not* supported and I don't plan to do
 * so.<br> You may convince me otherwise, though.<br> Old style options (like tar
 * supports) look like this: tar xfL <value_for_f> <value_for_L>
 *
 *
 * Using CheckArg as a developer can be divided in two phases, pre-parse() and
 * post-parse(). In the pre-parse phase, you'll define your commandline options using
 * the add() members. In the post-parse phase, you can check, which options where given
 * and get their values, if they've got one.
 *
 * Callbacks will, for now, be called while parsing,
 * if an option has no value, immediately after it was parsed,
 * otherwise, immediately after the value was parsed.
 *
 * Maybe they'll be split in immediate and deferred ones later,
 * but defferred ones would currently have no advantage over doing<br>
 * <code>  if( ca.isset("option") ) doOption();</code><br> after parse(),
 * so in favor of being lightweight, they're not implementet for now.
 */

map<int, string> CheckArgPrivate::errors = {
  {CA_ALLOK,    "Everything is fine"               },
  {CA_ERROR,    "An Error occurred"                },
  {CA_INVOPT,   "Unknown command line option"      },
  {CA_INVVAL,   "Value given to non-value option"  },
  {CA_MISSVAL,  "Missing value of option"          },
  {CA_CALLBACK, "Callback returned with error code"},
};

// c'tors

/**
 * \brief constructs a CheckArg object
 * \param appname the name of your application, will be used in the usage-line
 */
CheckArg::CheckArg(const string &appname)
    : p(new CheckArgPrivate(this, appname)) {}
/**
 * \brief constructs a CheckArg object
 * \param appname the name of your application, will be used in the usage-line
 * \param desc description of your programm, used in help
 */
CheckArg::CheckArg(const string &appname, const string &desc)
    : p(new CheckArgPrivate(this, appname, desc)) {}
/**
 * \brief constructs a CheckArg object
 * \param appname the name of your application, will be used in the usage-line
 * \param desc description of your programm, used in help
 * \param appendix text which will be added at the end of help
 */
CheckArg::CheckArg(const string &appname, const string &desc, const string &appendix)
    : p(new CheckArgPrivate(this, appname, desc, appendix)) {}


#ifdef HAS_STD_FILESYSTEM
CheckArg::CheckArg()
    : p(new CheckArgPrivate(this, std::string{})) {}
#endif


CheckArg::~CheckArg() = default;


// private c'tors
CheckArgPrivate::CheckArgPrivate(CheckArg *const ca, const string &appname)
    : parent(ca)
    , appname(appname)
    , pos_arg_sep(false) {
  if (!appname.empty()) usage_line = appname + " [options]";
}

CheckArgPrivate::CheckArgPrivate(
  CheckArg *const ca, const string &appname, const string &desc)
    : parent(ca)
    , appname(appname)
    , descr(desc)
    , pos_arg_sep(false) {
  if (!appname.empty()) usage_line = appname + " [options]";
}

CheckArgPrivate::CheckArgPrivate(
  CheckArg *const ca, const string &appname, const string &desc, const string &appendix)
    : parent(ca)
    , appname(appname)
    , descr(desc)
    , appendix(appendix)
    , pos_arg_sep(false) {
  if (!appname.empty()) usage_line = appname + " [options]";
}

/**
 * \brief set help's text for positional arguments
 * \param usage text to be appended to the usage line, like " [files...]"
 * \param descr text describing the positional arguments, will be used in help
 */
void
CheckArg::set_posarg_help(const string &usage, const string &descr) {
  p->posarg_help_usage = usage;
  p->posarg_help_descr = descr;
}


/**
 * \brief set a completely own usage line
 * \param str the usage line to use
 */
void
CheckArg::set_usage_line(const string &str) {
  p->usage_line = str;
}


/**
 * \brief get argv[0], the programms "callname"
 * \return argv[0]
 * \deprecated use CheckArg::callname() instead
 * \see CheckArg::callname
 */
[[deprecated("use CheckArg::callname() instead.")]] string
CheckArg::argv0() {
  return p->callname;
}

/**
 * \brief get argv[0], the programms "callname"
 * \return argv[0]
 */
string
CheckArg::callname() {
  return p->callname;
}

/**
 * \brief get all positional arguments
 *
 * all arguments not being options like '-h' or '-\-help'
 * or values of the former are considered positional arguments
 *
 * For Example:<br>
 *  <code>  ./ca_user pos0 -h -i input.txt pos1 -t pos2 pos3</code><br>
 *  all arguments starting with pos are positional arguments,
 *  given, that '-i' has a value and '-t' and '-h' have not
 * \return vector of positional arguments
 */
std::vector<string>
CheckArg::pos_args() const {
  return p->pos_args;
}


/**
 * \brief get error message for given errno
 * \param errno error number
 * \return error message string
 * \see CAError
 */
string
CheckArg::str_err(const int errno) {
  return CheckArgPrivate::errors[errno];
}

int
CheckArgPrivate::ca_error(int eno, const char *info, ...) const {
#if CA_PRINTERR
  va_list al;
  va_start(al, info);
  char *buff;
  int i = vasprintf(&buff, info, al);
  if (i > -1) {
    std::cerr << "Error: " << errors[eno] << buff << endl;
    free(buff);
  }
  va_end(al);
#endif
  return eno;
}

// bigger functions
//


/**
 * \brief add a command line option the parser shall accept
 * \param sopt the short option, like 'h' for '-h'
 * \param lopt the long option, like 'help' for '-\-help'
 * \param help a short description for the option, used in generated help
 * \param value_type does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int
CheckArg::add(
  const char sopt, const string &lopt, const string &help,
  const CAValueType value_type) {
  p->valid_args[lopt] = {
    .value_type = value_type,
    .sopt       = sopt,
    .help       = help,
  };
  p->short2long[sopt] = lopt;
  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param sopt the short option, like 'h' for '-h'
 * \param lopt the long option, like 'help' for '-\-help'
 * \param help a short description for the option, used in generated help
 * \param value_type does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int
CheckArg::add(
  const char sopt, const string &lopt, const string &help, const CAValueType value_type,
  const string &value_name) {
  p->valid_args[lopt] = {
    .value_type = value_type,
    .sopt       = sopt,
    .help       = help,
  };
  if (value_name.empty()) { p->valid_args[lopt].value_name = str_to_upper(lopt); }
  else {
    p->valid_args[lopt].value_name = value_name;
  }
  p->short2long[sopt] = lopt;
  return CA_ALLOK;
}
/**
 * \brief add a command line option the parser shall accept
 * \param sopt the short option, like 'h' for '-h'
 * \param lopt the long option, like 'help' for '-\-help'
 * \param cb a callback to be called whenever the option is encountered
 * \param help a short description for the option, used in generated help
 * \param value_type does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int
CheckArg::add(
  const char sopt, const string &lopt, const string &help,
  std::function<int(CheckArg *const, const string &, const string &)> cb,
  const CAValueType value_type) {
  p->valid_args[lopt] = {
    .value_type = value_type,
    .sopt       = sopt,
    .cb         = cb,
    .help       = help,
  };
  p->short2long[sopt] = lopt;
  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param sopt the short option, like 'h' for '-h'
 * \param lopt the long option, like 'help' for '-\-help'
 * \param cb a callback to be called whenever the option is encountered
 * \param help a short description for the option, used in generated help
 * \param value_type does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int
CheckArg::add(
  const char sopt, const string &lopt, const string &help,
  std::function<int(CheckArg *const, const string &, const string &)> cb,
  const CAValueType value_type, const string &value_name) {
  p->valid_args[lopt] = {
    .value_type = value_type,
    .sopt       = sopt,
    .cb         = cb,
    .help       = help,
  };

  if (value_name.empty()) { p->valid_args[lopt].value_name = str_to_upper(lopt); }
  else {
    p->valid_args[lopt].value_name = value_name;
  }
  p->short2long[sopt] = lopt;
  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param lopt the long option, like 'help' for '-\-help'
 * \param help a short description for the option, used in generated help
 * \param value_type does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int
CheckArg::add(const string &lopt, const string &help, const CAValueType value_type) {
  p->valid_args[lopt] = {.value_type = value_type, .help = help};

  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param lopt the long option, like 'help' for '-\-help'
 * \param help a short description for the option, used in generated help
 * \param value_type does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int
CheckArg::add(
  const string &lopt, const string &help, const CAValueType value_type,
  const string &value_name) {

  p->valid_args[lopt] = {
    .value_type = value_type,
    .help       = help,
  };

  if (value_name.empty()) { p->valid_args[lopt].value_name = str_to_upper(lopt); }
  else {
    p->valid_args[lopt].value_name = value_name;
  }

  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param lopt the long option, like 'help' for '-\-help'
 * \param cb a callback to be called whenever the option is encountered
 * \param help a short description for the option, used in generated help
 * \param value_type does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int
CheckArg::add(
  const string &lopt, const string &help,
  std::function<int(CheckArg *const, const string &, const string &)> cb,
  const CAValueType value_type) {
  p->valid_args[lopt] = {.value_type = value_type, .cb = cb, .help = help};

  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param lopt the long option, like 'help' for '-\-help'
 * \param cb a callback to be called whenever the option is encountered
 * \param help a short description for the option, used in generated help
 * \param value_type does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int
CheckArg::add(
  const string &lopt, const string &help,
  std::function<int(CheckArg *const, const string &, const string &)> cb,
  const CAValueType value_type, const string &value_name) {
  p->valid_args[lopt] = {
    .value_type = value_type,
    .cb         = cb,
    .help       = help,
  };

  if (value_name.empty()) { p->valid_args[lopt].value_name = str_to_upper(lopt); }
  else {
    p->valid_args[lopt].value_name = value_name;
  }
  return CA_ALLOK;
}


/**
 * \brief add auto generated '-\-help' message
 * \return CA_ALLOK
 */
int
CheckArg::add_autohelp() {
  p->valid_args["help"] = {
    .value_type = CA_VT_NONE,  // add --help with no value
    .sopt       = 'h',
    .cb         = checkarg::show_autohelp,  // set the autohelp callback
    .help       = "show this help message and exit",
  };
  p->short2long['h'] = "help";  // add -h mapped to --help
  return CA_ALLOK;
}

/**
 * \brief reset internal state after a parse
 * this is automatically called by Checkarg::parse() if you call it twice
 */
void
CheckArg::reset() {
  p->callname.clear();
  // clear and reset positional args in case of reuse / a second parse
  p->pos_arg_sep = false;
  p->pos_args.clear();
  // clear this in case it was set last time
  p->next_is_val_of.clear();

  // clear values and isset-state
  for (auto &opt : p->valid_args) { opt.second.value.clear(); }

  p->cleared = true;
}

/**
 * \brief parse the commnd line
 * this should be called after all add()s and add_autohelp()
 * \param argc the number of arguments, you'll want to pass argc gotten from main()
 * \param argv the list of arguments, you'll want to pass argv from main()
 * \return CA_ALLOK on success, some other code from CAError otherwise
 * \see CAError
 */
int
CheckArg::parse(int argc, char **argv) {
  if (argv == nullptr) {
    return p->ca_error(CA_ERROR, ": argv must have at least one element!");
  }
  return parse(vector<string>(argv, argv + argc));
}

/**
 * \brief parse the commnd line
 * this should be called after all add()s and add_autohelp()
 * \param argv the vector of arguments, you'll want to pass argv from main()
 * \return CA_ALLOK on success, some other code from CAError otherwise
 * \see CAError
 */
int
CheckArg::parse(const vector<string> &argv) {
  // FIXME: return a ParsedArgs object or something?
  int ret = CA_ALLOK;

  if (argv.size() == 0) {
    return p->ca_error(CA_ERROR, ": argv must have at least one element!");
  }

  if (!p->cleared) reset();
  p->cleared = false;  // we'll soon have state again

  p->callname = argv[0];

#ifdef HAS_STD_FILESYSTEM
  if (p->appname.empty()) {
    p->appname = fs::path(p->callname).filename();
    if (p->usage_line.empty()) p->usage_line = p->appname + " [options]";
  }
#endif

  // for(auto &arg : p->argv | std::views::drop(1)) {
  // start with 1 here, because argv[0] is special
  auto end = argv.cend();
  for (auto arg = argv.cbegin() + 1; arg != end; ++arg) {
    ret = p->arg(*arg);
    if (ret != CA_ALLOK) goto error;
  }
  if (!p->next_is_val_of.empty()) {
    return p->ca_error(CA_MISSVAL, ": %s!", argv.back().c_str());
  }

error:
  return ret;
}


/**
 * \brief get the value of a given option
 * \warning you shouldn't call this before parse()!
 * \param arg the long name of the option to get the value of
 * \return the value
 */
string
CheckArg::value(const string &arg) const {
  auto pos = p->valid_args.find(arg);
  if (pos != p->valid_args.end()) { return pos->second.value; }
  return "";
}

/**
 * \brief check if an option was given on the command line
 * \param arg the option specified by it's long name
 * \return true if give, false otherwise
 */
bool
CheckArg::isset(const string &arg) const {
  auto pos = p->valid_args.find(arg);
  return pos != p->valid_args.end() && !(pos->second.value.empty());
}

/**
 * \brief print the current usage line to stdout
 */
void
CheckArg::show_usage() {
  cout << usage() << endl;
}

/**
 * \brief get the current usage line as string
 * \return usage line
 */
string
CheckArg::usage() {
  stringstream ss;
  ss << "Usage: " << p->usage_line;
  if (!p->posarg_help_usage.empty()) ss << " " << p->posarg_help_usage;
  return ss.str();
}


#if 0
void
CheckArg::_autohelp() {
  stringstream ss;
  size_t space = 0;
  for( auto &kv : p->valid_args )
    space = std::max(space, kv.first.size() + kv.second.value_name.size() );
  space += 2; // separate the longest by 2 spaces

  ss << std::format("Usage: {s} {s}");
}
#endif

/**
 * \brief get current autogenerated help message as string
 * \return help message
 */
string
CheckArg::autohelp() {
  stringstream ss;
  size_t space = 0;
  for (auto &kv : p->valid_args) {
    size_t vsize = 0;
    if (kv.second.value_type != CA_VT_NONE) {
      vsize = kv.second.value_name.size();
      // FIXME: maybe always show equals sign to mark value options with empty name?
      if (vsize > 0) ++vsize;  // account for the equals sign
    }
    space = std::max(space, kv.first.size() + vsize);
  }

  space += 2;  // add 2 more spaces

  ss << usage() << "\n";

  if (!p->descr.empty()) ss << endl << p->descr << endl;

  ss << endl << "Options:" << endl;
  for (auto it = p->valid_args.begin(); it != p->valid_args.end(); ++it) {
    auto opt = it->second;

    if (opt.sopt)
      ss << "   -" << opt.sopt << ",";
    else
      ss << "      ";

    ss << " --" << it->first;

    switch (opt.value_type) {
    // case CA_VT_OPTIONAL:
    // if (!opt.value_name.empty()) {
    //   ss << "=[" << opt.value_name << "]"
    //      << string(space - it->first.size() - opt.value_name.size() - 3, ' ');
    //   break;
    // }
    // // fallthrough to default is intended
    case CA_VT_REQUIRED:
      if (!opt.value_name.empty()) {
        ss << "=" << opt.value_name
           << string(space - it->first.size() - opt.value_name.size() - 1, ' ');
        break;
      }
      // fallthrough to default is intended
    default:
      ss << string(space - it->first.size(), ' ');
      break;
    }

    ss << opt.help << endl;
  }
  if (!p->posarg_help_descr.empty())
    ss << endl << "Positional Arguments:" << endl << p->posarg_help_descr << endl;
  if (!p->appendix.empty()) ss << endl << p->appendix << endl;

  return ss.str();
}

/**
 * \brief print current help on stdout
 */
void
CheckArg::show_help() {
  cout << autohelp() << flush;
}


// private members:

int
CheckArgPrivate::arg(const string &arg) {
  if (!pos_arg_sep) {
    // if the separator '--' was given, all following args are positional

    if (!next_is_val_of.empty()) {
      // _next val of should be an opt with value
      // static_assert( _valid_args[_next_is_val_of].value_type );

      valid_args[next_is_val_of].value = arg;
      auto ret                         = call_cb(next_is_val_of);
      next_is_val_of.clear();
      return ret;
    }

    if (arg[0] == '-') {    // it's an arg
      if (arg[1] == '-') {  // it's a long one
        return arg_long(arg.substr(2));
      }

      // it's a short one or a group of short ones
      return arg_short(arg.substr(1));
    }
  }

  // it's some positional arg
  pos_args.push_back(arg);
  return CA_ALLOK;
}

int
CheckArgPrivate::arg_long(const string &arg) {
  if (arg.empty()) {
    // if the given arg was '--', arg is an empty string
    pos_arg_sep = true;
    return CA_ALLOK;
  }

  auto eqpos = arg.find('=');
  string real_arg, val;
  if (eqpos != string::npos) {
    real_arg = arg.substr(0, eqpos);
    val      = arg.substr(eqpos + 1);
  }
  else {
    real_arg = arg;
  }

  auto pos = valid_args.find(real_arg);
  if (pos != valid_args.end()) {
    if (pos->second.value_type && eqpos != string::npos) {
      // arg has value defined, and value is given by '='
      pos->second.value = val;
    }
    else if (pos->second.value_type) {
      // value of arg is the next arg, remember that for the next call of arg
      next_is_val_of = real_arg;
    }
    else {  // there's no value defined by add()
      if (!val.empty()) {
        // error?
        return ca_error(CA_INVVAL, ": --%s!", real_arg.c_str());
      }
      pos->second.value = "x";  // mark arg as seen
    }

    if (!pos->second.value_type || !val.empty()) {
      // if arg has no val, or val is found already, call callback now, if there's one
      return call_cb(real_arg);
    }
    return CA_ALLOK;
  }
  else {
    return ca_error(CA_INVOPT, ": --%s!", real_arg.c_str());
  }
}

int
CheckArgPrivate::arg_short(const string &arg) {
  size_t len = arg.size();
  for (size_t i = 0; i < len; ++i) {
    auto pos = short2long.find(arg[i]);
    if (pos != short2long.end()) {               // there is such a short arg registered
      if (valid_args[pos->second].value_type) {  // if has val,
        if (i < len - 1) {                       // remainder is interpreted as val,
          valid_args[pos->second].value = arg.substr(i + 1);
          return call_cb(pos->second);
        }
        else {  // or next_arg is treated as val
          next_is_val_of = pos->second;
        }
        return CA_ALLOK;  // no further looping, we're done.
      }
      else {
        valid_args[pos->second].value = "x";  // mark arg as seen
        auto ret                      = call_cb(pos->second);
        if (ret != CA_ALLOK) return ret;
      }
    }
    else {
      return ca_error(CA_INVOPT, ": -%c!", arg[i]);
    }
  }
  return CA_ALLOK;
}

int
CheckArgPrivate::call_cb(const string &arg) {
  auto cbpos = valid_args.find(arg);
  if (cbpos != valid_args.end() && cbpos->second.cb) {
    int cbret = cbpos->second.cb(parent, arg, cbpos->second.value);
    if (cbret != CA_ALLOK) {
      // if callback returns anything other than CA_ALLOK, there's been an error
      return ca_error(CA_CALLBACK, ": %d!", cbret);
    }
  }
  return CA_ALLOK;
}

int
checkarg::show_autohelp(CheckArg *const ca, const string &, const string &val) {
  ca->show_help();
  exit(0);  // always exit after showing help
}

string
checkarg::str_to_upper(const string &src) {
  auto &f = std::use_facet<std::ctype<char>>(std::locale());
  string result;
  result.reserve(src.size());
  std::transform(
    src.begin(), src.end(), std::back_inserter(result),
    [&](string::value_type c) -> string::value_type { return f.toupper(c); });
  return result;
}


#if CA_PRINTERR
#if not HAS_VASPRINTF

#include <cstring>
int
vasprintf(char **strp, const char *format, va_list ap) {
  int count;
  // Find out how long the resulting string is
  count = _vscprintf(format, ap);
  if (count == 0) {
    *strp = strdup("");
    return *strp == NULL ? -1 : 0;
  }
  else if (count < 0) {
    // Something went wrong, so return the error code (probably still requires checking
    // of "errno" though)
    return count;
  }
  // Allocate memory for our string
  *strp = (char *)malloc(count + 1);
  if (*strp == NULL) {
    abort();
    return -1;
  }
  // Do the actual printing into our newly created string
  return vsprintf(*strp, format, ap);
}

#endif
#endif
