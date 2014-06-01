#ifndef CHECKARG_HPP
#define CHECKARG_HPP

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
class CheckArgPrivate;
typedef std::shared_ptr<CheckArgPrivate> CheckArgPrivatePtr;

// autohelp callback
namespace checkarg{
	int show_autohelp(CheckArgPtr, const std::string &, const std::string &);
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
	int add(const char sopt, const std::string &lopt, std::function<int(CheckArgPtr, const std::string &, const std::string &)> cb, const std::string &help, bool has_val=false);
	int add(const std::string &lopt, const std::string &help, bool has_val=false);
	int add(const std::string &lopt, std::function<int(CheckArgPtr,const std::string &, const std::string &)> cb, const std::string &help, bool has_val=false);

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

  // get error string for errno
	static std::string str_err(const int errno);

private:

  CheckArgPrivatePtr p;

	friend int checkarg::show_autohelp(CheckArgPtr, const std::string &, const std::string &);
};

// return codes
enum {
	CA_ALLOK=0,
	CA_ERROR,
	CA_INVARG,
	CA_INVVAL,
	CA_MISSVAL,
	CA_CALLBACK,
};


#endif //CHECKARG_HPP
