/*
 * Copyright (c) 2013-2018 brainpower <brainpower at mailbox dot org>
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
#include "checkarg_private.hpp"

#if CA_PRINTERR
#include <cstdarg>
#endif

#include <sstream>
#include <iostream>
using namespace std;

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
 * In the docs I'll call the GNU style and POSIX style options "long" and "short" options, respectively.
 *
 * Old style or traditional style options are *not* supported and I don't plan to do so.<br>
 * You may convince me otherwise, though.<br>
 * Old style options (like tar supports) look like this:
 *   tar xfL <value_for_f> <value_for_L>
 *
 *
 * Using CheckArg as a developer can be divided in two phases, pre-parse() and post-parse().
 * In the pre-parse phase, you'll define your commandline options using the add() members.
 * In the post-parse phase, you can check, which options where given
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


map <int,string>
CheckArgPrivate::errors = {
	{CA_ALLOK,  "Everything is fine"},
	{CA_ERROR,  "An Error occurred"},
	{CA_INVARG, "Unknown command line option"},
	{CA_INVVAL, "Value given to non-value option"},
	{CA_MISSVAL, "Missing value of option"},
	{CA_CALLBACK, "Callback returned with error code"},
};

// c'tors

/**
 * \brief constructs a CheckArg object
 * \param argc the number of arguments, you'll want to pass argc gotten from main()
 * \param argv the list of arguments, you'll want to pass argv from main()
 * \param appname the name of your application, will be used in the usage-line
 */
CheckArg::CheckArg(const int argc, char ** argv, const std::string &appname)
  : p(new CheckArgPrivate(this, argc,argv,appname)){
}
/**
 * \brief constructs a CheckArg object
 * \param argc the number of arguments, you'll want to pass argc gotten from main()
 * \param argv the list of arguments, you'll want to pass argv from main()
 * \param appname the name of your application, will be used in the usage-line
 * \param desc description of your programm, used in help
 */
CheckArg::CheckArg(const int argc, char ** argv, const std::string &appname, const std::string &desc)
  : p(new CheckArgPrivate(this, argc, argv, appname, desc)){
}
/**
 * \brief constructs a CheckArg object
 * \param argc the number of arguments, you'll want to pass argc gotten from main()
 * \param argv the list of arguments, you'll want to pass argv from main()
 * \param appname the name of your application, will be used in the usage-line
 * \param desc description of your programm, used in help
 * \param appendix text which will be added at the end of help
 */
CheckArg::CheckArg(const int argc, char ** argv, const std::string &appname, const std::string &desc, const std::string &appendix)
  : p(new CheckArgPrivate(this, argc,argv,appname,desc,appendix)){
}

// private c'tors
CheckArgPrivate::CheckArgPrivate(CheckArgRPtr ca, const int argc, char** argv, const std::string &appname)
  : argc(argc), argv(argv), parent(ca), appname(appname), pos_arg_sep(false),
    usage_line(appname + " [options]") {}

CheckArgPrivate::CheckArgPrivate(CheckArgRPtr ca, const int argc, char** argv, const std::string &appname, const std::string &desc)
  : argc(argc), argv(argv), parent(ca), appname(appname), descr(desc), pos_arg_sep(false),
    usage_line(appname + " [options]") {}

CheckArgPrivate::CheckArgPrivate(CheckArgRPtr ca, const int argc, char** argv, const std::string &appname, const std::string &desc, const std::string &appendix)
  : argc(argc), argv(argv), parent(ca), appname(appname), descr(desc), appendix(appendix), pos_arg_sep(false),
    usage_line(appname + " [options]") {}


/**
 * \brief set help's text for positional arguments
 * \param usage text to be appended to the usage line, like " [files...]"
 * \param descr text describing the positional arguments, will be used in help
 */
void
CheckArg::set_posarg_help(const std::string &usage, const std::string &descr ) {
  p->posarg_help_usage = usage;
  p->posarg_help_descr = descr;
}


/**
 * \brief set a completely own usage line
 * \param str the usage line to use
 */
void
CheckArg::set_usage_line(const std::string &str) {
  p->usage_line = str;
}


/**
 * \brief get argv[0], the programms "name"
 * \return argv[0]
 */
std::string
CheckArg::argv0() {
  return p->argv[0];
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
std::vector<std::string>
CheckArg::pos_args() const {
  return p->pos_args;
}


/**
 * \brief get error message for given errno
 * \param errno error number
 * \return error message string
 * \see CAError
 */
std::string
CheckArg::str_err(const int errno){ return CheckArgPrivate::errors[errno]; }

int
CheckArgPrivate::ca_error(int eno, const char *info, ...) const {
#if CA_PRINTERR
  va_list al;
  va_start(al, info);
  char *buff;
  int i=vasprintf(&buff,info,al);
  if (i > -1){
    std::cerr << "Error: " << errors[eno] << buff << std::endl;
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
 * \param has_val does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int CheckArg::add(const char sopt, const std::string &lopt, const std::string &help, const bool has_val){
  p->valid_args[lopt].has_val = has_val;
  p->valid_args[lopt].sopt    = sopt;
  p->valid_args[lopt].help    = help;
  p->short2long[sopt] = lopt;
  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param sopt the short option, like 'h' for '-h'
 * \param lopt the long option, like 'help' for '-\-help'
 * \param cb a callback to be called whenever the option is encountered
 * \param help a short description for the option, used in generated help
 * \param has_val does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int CheckArg::add(const char sopt, const std::string &lopt,
                  std::function<int(CheckArgRPtr,const std::string &, const std::string &)> cb,
                  const std::string &help, const bool has_val){
  p->valid_args[lopt].has_val = has_val;
  p->valid_args[lopt].sopt    = sopt;
  p->valid_args[lopt].help    = help;
  p->valid_args[lopt].cb      = cb;
  p->short2long[sopt]         = lopt;
  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param lopt the long option, like 'help' for '-\-help'
 * \param help a short description for the option, used in generated help
 * \param has_val does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int CheckArg::add(const std::string &lopt, const std::string &help, const bool has_val){
  p->valid_args[lopt].has_val = has_val;
  p->valid_args[lopt].help    = help;
  return CA_ALLOK;
}

/**
 * \brief add a command line option the parser shall accept
 * \param lopt the long option, like 'help' for '-\-help'
 * \param cb a callback to be called whenever the option is encountered
 * \param help a short description for the option, used in generated help
 * \param has_val does the option have a value
 * \return a return code from CAError
 * \see CAError
 */
int CheckArg::add(const std::string &lopt,
                  std::function<int(CheckArgRPtr,const std::string &, const std::string &)> cb,
                  const std::string &help, const bool has_val){
  p->valid_args[lopt].has_val = has_val;
  p->valid_args[lopt].cb      = cb;
  p->valid_args[lopt].help    = help;
  return CA_ALLOK;
}


/**
 * \brief add auto generated '-\-help' message
 * \return CA_ALLOK
 */
int CheckArg::add_autohelp(){
  p->valid_args["help"].has_val = false; // add --help with no value
  p->valid_args["help"].sopt    = 'h';
  p->valid_args["help"].help    = "show this help message and exit";
  p->valid_args["help"].cb      = checkarg::show_autohelp; // set the autohelp callback
  p->short2long['h'] = "help";   // add -h mapped to --help
  return CA_ALLOK;
}

/**
 * \brief parse the commnd line
 * this should be called after all add()s and add_autohelp()
 * \return CA_ALLOK on success, some other code from CAError otherwise
 * \see CAError
 */
int CheckArg::parse(){
  int ret = CA_ALLOK;
  for(int i=1; i<p->argc; ++i){ // start with 1 here, because argv[0] is special
    ret = p->arg(p->argv[i]);
    if( ret != CA_ALLOK ) goto error;
  }
  if( ! p->next_is_val_of.empty() ){
    return p->ca_error(CA_MISSVAL, ": %s!", p->argv[p->argc-1]);
  }

  // free strings not necessary anymore, e.g. those for '--help'
  /* // show_help() prevents that now, as it could be called any time
   * // if you want to minimize memory, destroy the whole CheckArg object after use.
  p->appname.clear();
  p->appendix.clear();
  p->usage_line.clear();
  p->descr.clear();
  p->posarg_help_descr.clear();
  p->posarg_help_usage.clear();
  p->next_is_val_of.clear();
  for( auto arg : p->valid_args)
    arg.second.help.clear();
  */
error:
  return ret;
}


/**
 * \brief get the value of a given option
 * \warning you shouldn't call this before parse()!
 * \param arg the long name of the option to get the value of
 * \return the value
 */
string CheckArg::value(const std::string &arg) const{

  auto pos = p->valid_args.find(arg);
  if( pos != p->valid_args.end() ){
    return pos->second.value;
  }
  return "";
}

/**
 * \brief check if an option was given on the command line
 * \param arg the option specified by it's long name
 * \return true if give, false otherwise
 */
bool CheckArg::isset(const std::string &arg) const {
  auto pos = p->valid_args.find(arg);
  return pos != p->valid_args.end() && !(pos->second.value.empty());
}

/**
 * \brief print the current usage line to stdout
 */
void
CheckArg::show_usage(){
  cout << "Usage: " << p->usage_line << " " << p->posarg_help_usage << endl;
}

/**
 * \brief print current help on stdout
 */
void
CheckArg::show_help(){
  stringstream ss;
  size_t space = 0;
  for( auto &kv : p->valid_args )
    space = max(space, kv.first.size() );

  space += 2; // add 2 more spaces

  ss << "Usage: " << p->usage_line << " " << p->posarg_help_usage << endl;

  if(!p->descr.empty()) ss << endl << p->descr << endl;

  ss << endl << "Options:" << endl;
  for(auto it=p->valid_args.begin(); it != p->valid_args.end(); ++it){
    if(it->second.sopt) ss << "   -" << it->second.sopt << ",";
    else ss << "      ";
    ss << " --" << it->first << string(space-it->first.size(), ' ')
       << it->second.help << endl;
  }
  if(!p->posarg_help_descr.empty())
    ss << endl
       << "Positional Arguments:" << endl
       << p->posarg_help_descr << endl;
  if(!p->appendix.empty()) ss << endl << p->appendix << endl;

  cout << ss.str() << flush;
}


// private members:

int CheckArgPrivate::arg(const std::string &arg){
  if( ! pos_arg_sep ){
    // if the separator '--' was given, all following args are positional

    if( ! next_is_val_of.empty() ){
      // _next val of should be an opt with value
      //static_assert( _valid_args[_next_is_val_of].has_val );

      valid_args[next_is_val_of].value = arg;
      auto ret = call_cb(next_is_val_of);
      next_is_val_of.clear();
      return ret;
    }

    if( arg[0] == '-' ){ // it's an arg
      if( arg[1] == '-' ) { // it's a long one
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

int CheckArgPrivate::arg_long(const std::string &arg){
  if( arg.empty() ){
    // if the given arg was '--', arg is an empty string
    pos_arg_sep = true;
    return CA_ALLOK;
  }

  auto eqpos = arg.find('=');
  string real_arg, val;
  if( eqpos != string::npos ){
    real_arg = arg.substr(0, eqpos);
    val = arg.substr(eqpos+1);
  } else {
    real_arg = arg;
  }

  auto pos = valid_args.find(real_arg);
  if( pos != valid_args.end() ){
    if( pos->second.has_val && !val.empty() ) {
      // arg has value defined, and value is given by '='
      pos->second.value = val;
    } else if( pos->second.has_val ){
      // value of arg is the next arg, remember that for the next call of arg
      next_is_val_of = real_arg;
    } else { // there's no value defined by add()
      if( !val.empty() ){
        // error?
        return ca_error(CA_INVVAL, ": --%s!", real_arg.c_str());
      }
      pos->second.value = "x"; // mark arg as seen
    }

    if( !pos->second.has_val || !val.empty()) {
      // if arg has no val, or val is found already, call callback now, if there's one
      return call_cb(real_arg);
    }
    return CA_ALLOK;
  } else {
    return ca_error(CA_INVARG, ": --%s!", real_arg.c_str());
  }
}

int CheckArgPrivate::arg_short(const std::string &arg){
  size_t len = arg.size();
  for(size_t i=0; i < len; ++i){
    auto pos = short2long.find(arg[i]);
    if( pos != short2long.end() ){ // there is such a short arg registered
      if( valid_args[pos->second].has_val ) { // if has val,
        if( i < len-1 ){ //remainder is interpreted as val,
          valid_args[pos->second].value = arg.substr(i+1);
          return call_cb(pos->second);
        } else { // or next_arg is treated as val
          next_is_val_of = pos->second;
        }
        return CA_ALLOK; // no further looping, we're done.
      } else {
        valid_args[pos->second].value = "x"; // mark arg as seen
        auto ret = call_cb(pos->second);
        if(ret != CA_ALLOK )
          return ret;
      }
    } else {
      return ca_error(CA_INVARG, ": -%c!", arg[i]);
    }
  }
  return CA_ALLOK;
}

int CheckArgPrivate::call_cb(const std::string &arg){
  auto cbpos = valid_args.find(arg);
  if( cbpos != valid_args.end() && cbpos->second.cb ){
    int cbret = cbpos->second.cb(parent, arg, cbpos->second.value);
    if(cbret != CA_ALLOK) {
      // if callback returns anything other than CA_ALLOK, there's been an error
      return ca_error(CA_CALLBACK, ": %d!", cbret);
    }
  }
  return CA_ALLOK;
}

int checkarg::show_autohelp(CheckArgRPtr ca, const std::string&, const std::string &val){
	ca->show_help();
  exit(0); // always exit after showing help
}

#if CA_PRINTERR
#	if not HAS_VASPRINTF

#include <cstring>
int vasprintf(char** strp, const char* format, va_list ap)
{
  int count;
  // Find out how long the resulting string is
  count = _vscprintf(format, ap);
  if (count == 0) {
    *strp = strdup("");
    return *strp == NULL ? -1:0;
  } else if (count < 0) {
    // Something went wrong, so return the error code (probably still requires checking of "errno" though)
    return count;
  }
  //Allocate memory for our string
  *strp = (char*)malloc(count + 1);
  if (*strp == NULL) {
    abort();
    return -1;
  }
  // Do the actual printing into our newly created string
  return vsprintf(*strp, format, ap);
}

#	endif
#endif

