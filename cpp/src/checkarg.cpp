
#include "checkarg.hpp"
#include "bpstd.hpp"
#include <sstream>
using namespace std;

map <int,string>
CheckArg::_errors = {
	{CA_ALLOK,  "Everything is fine"},
	{CA_ERROR,  "An Error occurred"},
	{CA_INVARG, "Unknown command line argument"},
	{CA_INVVAL, "Value given to non-value argument"},
	{CA_MISSVAL, "Missing value of argument"},
	{CA_CALLBACK, "Callback returned with error code"},
};

CheckArg::CheckArg(const int argc, char** argv, const string &appname)
	: _argc(argc), _argv(argv), _appname(appname), _autohelp_on(false),
	  _usage_line(_appname + " [options] [positional args]") {}

CheckArg::CheckArg(const int argc, char** argv, const string &appname, const string &desc)
	: _argc(argc), _argv(argv), _appname(appname), _descr(desc),_autohelp_on(false),
	  _usage_line(_appname + " [options] [positional args]") {}

CheckArg::CheckArg(const int argc, char** argv, const string &appname, const string &desc, const string &appendix)
	: _argc(argc), _argv(argv), _appname(appname), _descr(desc), _appendix(appendix),_autohelp_on(false),
	  _usage_line(_appname + " [options] [positional args]") {}


//~ int CheckArg::add(const char sopt, const string &help, bool has_val){
	//~ _valid_args[sopt] = has_val;
	//~ _autohelp[sopt] = help;
//~ }
//~
//~ int CheckArg::add(const char sopt, function<void(const string &, const string &)> cb, const string &help, bool has_val){
	//~ _valid_args[sopt] = has_val;
	//~ _valid_args_cb[sopt] = cb;
	//~ _autohelp[sopt] = help;
//~ }

int CheckArg::add(const char sopt, const string &lopt, const string &help, bool has_val){
	//~ _valid_args[sopt] = has_val;
	_valid_args[lopt] = has_val;
	_autohelp[lopt] = help;
	_short2long[sopt] = lopt;
	return CA_ALLOK;
}

int CheckArg::add(const char sopt, const string &lopt, function<int(const string &, const string &)> cb, const string &help, bool has_val){
	//~ _valid_args[sopt] = has_val;
	_valid_args[lopt] = has_val;
	//~ _valid_args_cb[sopt] = cb;
	_valid_args_cb[lopt] = cb;
	_autohelp[lopt] = help;
	_short2long[sopt] = lopt;
	return CA_ALLOK;
}

int CheckArg::add(const string &lopt, const string &help, bool has_val){
	_valid_args[lopt] = has_val;
	_autohelp[lopt] = help;
	return CA_ALLOK;
}

int CheckArg::add(const string &lopt, function<int(const string &, const string &)> cb, const string &help, bool has_val){
	_valid_args[lopt] = has_val;
	_valid_args_cb[lopt] = cb;
	_autohelp[lopt] = help;
	return CA_ALLOK;
}

int CheckArg::add_autohelp(){
	_valid_args["help"] = false;
	_short2long['h'] = "help";
	_valid_args_cb["help"] = checkarg::show_autohelp;
	_autohelp_on = true;
	return CA_ALLOK;
}

int CheckArg::parse(){
	int ret = CA_ALLOK;
//	_argv0 = argv[0];
	for(int i=1; i<_argc; ++i){ // start with 1 here, because argv[0] is special
		ret = arg(_argv[i]);
		if( ret != CA_ALLOK ) break;
	}
	if( ! _next_is_val_of.empty() ){
		ca_error(CA_MISSVAL, ": "+ string(_argv[_argc-1]) + "!");
		return CA_MISSVAL;
	}
	return ret;
}

string CheckArg::value(const string &arg) const{

	auto pos = _valid_args_vals.find(arg);
	if( pos != _valid_args_vals.end() ){
		return pos->second;
	}
	//~ if( tmp.length() == 1 ){
		//~ pos = _valid_args_vals.find(_short2long[arg]);
		//~ if( pos != _valid_args_vals.end() ){
			//~ return *pos;
		//~ }
	//~ }
	return "";
}

bool CheckArg::isset(const string &arg) const {
	return _valid_args_vals.find(arg) != _valid_args_vals.end();
}


int CheckArg::arg(const string &arg){
	if( ! _next_is_val_of.empty() ){
		_valid_args_vals[_next_is_val_of] = arg;
		_next_is_val_of.clear();
		return CA_ALLOK;
	}

	if( arg[0] == '-' ){ // it's an arg
		if( arg[1] == '-' ) { // it's a long one
			return arg_long(arg.substr(2));
		}

		// it's a short one or a group of short ones
		return arg_short(arg.substr(1));
	}

	// it's some positional arg
	_pos_args.push_back(arg);
	return CA_ALLOK;
}

int CheckArg::arg_long(const string &arg){
	auto eqpos = arg.find('=');
	string real_arg, val;
	if( eqpos != string::npos ){
		real_arg = arg.substr(0, eqpos);
		val = arg.substr(eqpos+1);
	} else {
		real_arg = arg;
	}

	auto pos = _valid_args.find(real_arg);
	if( pos != _valid_args.end() ){
		if( pos->first == "help"  && _autohelp_on ) {
			stringstream ss;
			ss << "Usage: " << _usage_line << _posarg_help_usage << endl;
			if(!_descr.empty()) ss << endl << _descr << endl;
			ss << endl << "Options:" << endl;
			for(auto it=_valid_args.begin(); it != _valid_args.end(); ++it){
				auto sarg = long2short(it->first);
				if(!sarg.empty()) ss << "   -" << sarg << ",";
				else ss << "      ";
				ss << " --" << it->first << (it->first.size() > 7?"\n      \t":"\t")
				   << _autohelp[it->first] << endl;
			}
			if(!_posarg_help_descr.empty())
				ss << endl
					 << "Positional Arguments:" << endl 
					 << _posarg_help_descr << endl;
			if(!_appendix.empty()) ss << endl << _appendix << endl;
			val = ss.str();
		} else if( pos->second && !val.empty() ) {
			// arg has value defined, and value is given by '='
			_valid_args_vals[real_arg] = val;
		} else if( pos->second ){
			// value of arg is the next arg, remember that for the next call of arg
			_next_is_val_of = real_arg;
		} else { // there's no value defined by add()
			if( !val.empty() ){
				// error?
				ca_error(CA_INVVAL, ": --" + real_arg + "!");
				return CA_INVVAL;
			}
			_valid_args_vals[real_arg] = "x"; // mark arg as seen
		}

		if( !pos->second || !val.empty()) {
			// if arg has no val, or val is found already, call callback now, if there's one
			auto cbpos = _valid_args_cb.find(real_arg);
			if( cbpos != _valid_args_cb.end() ){
				int cbret = cbpos->second(real_arg, val);
				if(cbret != CA_ALLOK) {
					// if callback returns anything other than CA_ALLOK, there's been an error
					ca_error(CA_CALLBACK, bpstd::stringf(": %d!", cbret));
					return CA_CALLBACK;
				}
			}
		}
		return CA_ALLOK;
	} else {
		ca_error(CA_INVARG, ": --" + real_arg + "!");
		return CA_INVARG;
	}
}

int CheckArg::arg_short(const string &arg){
	size_t len = arg.size();
	for(int i=0; i < len; ++i){
		auto pos = _short2long.find(arg[i]);
		if( pos != _short2long.end() ){ // there is such a short arg registered
			if( _valid_args[pos->second] ) { // if has val,
				if( i < len-1 ){ //remainder is interpreted as val,
					_valid_args_vals[_short2long[arg[i]]] = arg.substr(i+1);
				} else { // or next_arg is treated as val
					_next_is_val_of = _short2long[arg[i]];
				}
				return CA_ALLOK; // no further looping, we're done.
			} else {
				_valid_args_vals[_short2long[arg[i]]] = "x"; // mark arg as seen
			}
		} else {
			ca_error(CA_INVARG, string(": -") + arg[i] + "!");
			return CA_INVARG;
		}
	}
	return CA_ALLOK;
}

int checkarg::show_autohelp(const string&, const string &val){
	cout << val << flush;

	exit(0); // always exit after showing help
}

std::string CheckArg::long2short(const std::string &larg) const {
	for(auto it=_short2long.begin(); it != _short2long.end(); ++it)
		if( it->second == larg ) return string(1,it->first);
	return "";
}
