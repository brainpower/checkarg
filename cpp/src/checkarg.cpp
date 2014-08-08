
#include "checkarg.hpp"
#include "checkarg_private.hpp"
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
 * because it's non-intrusive, you dont need to 'build your project arount it'.
 *
 * Commandline options use the common so-called GNU style,
 * e.g. use double dashes and look something like these: <br>
 *  -\-option[=value] <br>
 *  -\-option [value] <br>
 * Additionally, abreviations of those in so-called POSIX style can be added.
 * They look like this: <br>
 *  -o [value]
 * I'll call them long and short options, respectively.
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
	{CA_INVARG, "Unknown command line argument"},
	{CA_INVVAL, "Value given to non-value argument"},
	{CA_MISSVAL, "Missing value of argument"},
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
CheckArgPrivate::CheckArgPrivate(CheckArg* ca, const int argc, char** argv, const std::string &appname)
	: argc(argc), argv(argv), parent(ca), appname(appname), autohelp_on(false),
	  usage_line(appname + " [options]") {}

CheckArgPrivate::CheckArgPrivate(CheckArg* ca, const int argc, char** argv, const std::string &appname, const std::string &desc)
	: argc(argc), argv(argv), parent(ca), appname(appname), descr(desc), autohelp_on(false),
	  usage_line(appname + " [options]") {}

CheckArgPrivate::CheckArgPrivate(CheckArg* ca, const int argc, char** argv, const std::string &appname, const std::string &desc, const std::string &appendix)
	: argc(argc), argv(argv), parent(ca), appname(appname), descr(desc), appendix(appendix), autohelp_on(false),
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

void
CheckArgPrivate::ca_error(int eno, const std::string &info, ...) const {
#ifdef CA_PRINTERR
  va_list al;
  va_start(al, info);
  char *buff;
  int i=vasprintf(&buff,info.c_str(),al);
  if (i > -1){
    std::cerr << "Error: " << errors[eno] << buff << std::endl;
    free(buff);
  }
#endif
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
	p->valid_args[lopt] = { has_val, sopt, help };
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
                  std::function<int(CheckArgPtr,const std::string &, const std::string &)> cb,
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
                  std::function<int(CheckArgPtr,const std::string &, const std::string &)> cb,
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
	p->autohelp_on = true;         // switch autohelp to on
	return CA_ALLOK;
}

/**
 * \brief parse the commnd line, this should come after any add() or add_autohelp()
 * \return CA_ALLOK on success, some other code from CAError otherwise
 * \see CAError
 */
int CheckArg::parse(){
	int ret = CA_ALLOK;
	for(int i=1; i<p->argc; ++i){ // start with 1 here, because argv[0] is special
		ret = p->arg(p->argv[i]);
		if( ret != CA_ALLOK ) break;
	}
	if( ! p->next_is_val_of.empty() ){
		p->ca_error(CA_MISSVAL, ": "+ string(p->argv[p->argc-1]) + "!");
		return CA_MISSVAL;
	}

	// free strings not necessary anymore, e.g. those for '--help'
	p->appname.clear();
	p->appendix.clear();
	p->usage_line.clear();
	p->descr.clear();
	p->posarg_help_descr.clear();
	p->posarg_help_usage.clear();
	p->next_is_val_of.clear();
	for( auto arg : p->valid_args)
		arg.second.help.clear();

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

int CheckArgPrivate::arg(const std::string &arg){
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

	// it's some positional arg
	pos_args.push_back(arg);
	return CA_ALLOK;
}

int CheckArgPrivate::arg_long(const std::string &arg){
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
			valid_args[real_arg].value = val;
		} else if( pos->second.has_val ){
			// value of arg is the next arg, remember that for the next call of arg
			next_is_val_of = real_arg;
		} else { // there's no value defined by add()
			if( !val.empty() ){
				// error?
				ca_error(CA_INVVAL, ": --" + real_arg + "!");
				return CA_INVVAL;
			}
			valid_args[real_arg].value = "x"; // mark arg as seen
		}

		if( !pos->second.has_val || !val.empty()) {
			// if arg has no val, or val is found already, call callback now, if there's one
			return call_cb(real_arg);
		}
		return CA_ALLOK;
	} else {
		ca_error(CA_INVARG, ": --" + real_arg + "!");
		return CA_INVARG;
	}
}

int CheckArgPrivate::arg_short(const std::string &arg){
	size_t len = arg.size();
	for(int i=0; i < len; ++i){
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
			ca_error(CA_INVARG, string(": -") + arg[i] + "!");
			return CA_INVARG;
		}
	}
	return CA_ALLOK;
}

int CheckArgPrivate::call_cb(const std::string &arg){
	auto cbpos = valid_args.find(arg);
	if( cbpos != valid_args.end() && cbpos->second.cb ){
		int cbret = cbpos->second.cb(CheckArgPtr(parent), arg, cbpos->second.value);
		if(cbret != CA_ALLOK) {
			// if callback returns anything other than CA_ALLOK, there's been an error
			ca_error(CA_CALLBACK, ": %d!", cbret);
			return CA_CALLBACK;
		}
	}
	return CA_ALLOK;
}


int checkarg::show_autohelp(CheckArgPtr ca, const std::string&, const std::string &val){
	stringstream ss;
	size_t space = 0;
	for( auto &kv : ca->p->valid_args )
		space = max(space, kv.first.size() );

	space += 2; // add 2 more spaces

	ss << "Usage: " << ca->p->usage_line << " " << ca->p->posarg_help_usage << endl;

	if(!ca->p->descr.empty()) ss << endl << ca->p->descr << endl;

	ss << endl << "Options:" << endl;
	for(auto it=ca->p->valid_args.begin(); it != ca->p->valid_args.end(); ++it){
		auto sarg = ca->p->valid_args.find(it->first);
		if(sarg != ca->p->valid_args.end()) ss << "   -" << sarg->second.sopt << ",";
		else ss << "      ";
		ss << " --" << it->first << string(space-it->first.size(), ' ')
		   << ca->p->valid_args[it->first].help << endl;
	}
	if(!ca->p->posarg_help_descr.empty())
		ss << endl
			 << "Positional Arguments:" << endl
			 << ca->p->posarg_help_descr << endl;
	if(!ca->p->appendix.empty()) ss << endl << ca->p->appendix << endl;

	cout << ss.str() << flush;

	exit(0); // always exit after showing help
}
