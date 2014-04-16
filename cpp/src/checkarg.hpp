#ifndef CHECKARG_HPP
#define CHECKARG_HPP

enum {
	CA_ALLOK=0,
	CA_ERROR,
	CA_INVARG,
	CA_INVVAL,
	CA_MISSVAL,
	CA_CALLBACK,
};

#include <memory> // shared_ptr
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>

class CheckArg;
typedef std::shared_ptr<CheckArg> CheckArgPtr;

namespace checkarg{
	int show_autohelp(CheckArgPtr, const std::string &, const std::string &);
};

class CheckArg {

public:
	CheckArg(const int argc, char** argv, const std::string &appname);
	CheckArg(const int argc, char** argv, const std::string &appname, const std::string &desc);
	CheckArg(const int argc, char** argv, const std::string &appname, const std::string &desc, const std::string &appendix);

	//~ int add(const char sopt, const std::string &help, bool has_val=false);
	//~ int add(const char sopt, function<void(const std::string &, const std::string &)> cb, const std::string &help, bool has_val=false);
	int add(const char sopt, const std::string &lopt, const std::string &help, bool has_val=false);
	int add(const char sopt, const std::string &lopt, std::function<int(CheckArgPtr, const std::string &, const std::string &)> cb, const std::string &help, bool has_val=false);
	int add(const std::string &lopt, const std::string &help, bool has_val=false);
	int add(const std::string &lopt, std::function<int(CheckArgPtr,const std::string &, const std::string &)> cb, const std::string &help, bool has_val=false);

	void add_posarg_help(const std::string &usage, const std::string &descr ) { _posarg_help_usage = usage; _posarg_help_descr = descr; }
	int add_autohelp();

	int parse();

	void set_usage_line(const std::string &str) { _usage_line = str; }

	std::string argv0() { return _argv[0]; }
	std::vector<std::string> pos_args() const { return _pos_args; }
	std::string value(const std::string &arg) const;
	bool isset(const std::string &arg) const;

	static std::string str_err(const int errno){ return _errors[errno]; }

private:

	int arg(const std::string &arg);
	int arg_long(const std::string &arg);
	int arg_short(const std::string &arg);

	//~ int register_val_callback();

	friend int checkarg::show_autohelp(CheckArgPtr, const std::string &, const std::string &);

	void ca_error(int eno, const std::string &info="!") const {
#ifdef CA_PRINTERR
		std::cerr << "Error: " << _errors[eno] << info << std::endl;
#endif
	}

	std::string long2short(const std::string &larg) const;

	static std::map<int,std::string> _errors;

	std::map<std::string,std::function<int(CheckArgPtr, const std::string &,const std::string &)>> _valid_args_cb;
	std::map<std::string, bool> _valid_args;
	std::map<std::string,std::string> _valid_args_vals;
	std::map<std::string,std::string> _autohelp;
	std::map<char,std::string> _short2long;

	std::vector<std::string> _pos_args;

	const int _argc;
	char **   _argv;

	std::string _appname;
	std::string _descr;
	std::string _appendix;
	bool _autohelp_on;
	std::string _usage_line;
	std::string _posarg_help_descr, _posarg_help_usage;
	//std::string _argv0;

	// state
	std::string _next_is_val_of;

};

#endif //CHECKARG_HPP
