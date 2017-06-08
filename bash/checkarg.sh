##
# This provides a getopt-like functionality.
#
#	For general information sbout how this works,
#	see README.md at the projects root or
#	at https://github.com/brainpower/checkarg/blob/master/README.md
#
# Positional arguments are appended to 'checkarg_pos_args'
# which is declared as indexed array on sourcing this file..
# Those are arguments passed which are not preceeded by a dash
# and are not a value of an value-type argument.
#
# Using CheckArg as a developer can be divided in two phases,
# pre-parse() and post-parse().
# In the pre-parse phase, you'll initialize CheckArg using checkarg_init
# and define your commandline options using the checkarg_add* functions.
#
# In the post-parse phase, you can check, which options where given
# and get their values, if they've got one,
# using checkarg_isset and checkarg_value.
#
# Parsing is done by the checkarg_parse function.
#
# Callbacks will, for now, be called while parsing,
# if an option has no value, immediately after it was parsed,
# otherwise, immediately after the value was parsed.
# Callbacks have to be some shell function or command,
# which will be tried to be executed directly without any eval.
# The long option name and the value (if any)
# will be passed to that function or command.
#
# If you want to set global variables in callbacks, it is advised
# to use ALL UPPERCASE variable names, or you may get in conflict
# with internal variables declared local in checkarg's functions.
#
#	For example usages see tests/test*.sh.
# Requires bash >= 4 or other compatible shell, like zsh
#
###
#
# Copyright (c) 2013-2015 brainpower <brainpower at mailbox dot org>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
##

if [ -z "$CHECKARG_SH" ]; then # "include guard"
  CHECKARG_SH=1

_revert_nounset=0
if set -o | grep unset | grep -q off; then
	set -o nounset
	_revert_nounset=1
fi

##
# initialize public global vars used by checkarg
##
# version string
readonly CA_VER_STRING="1.2.0"

declare -a  checkarg_pos_args;

##
# initialize "private" vars
##
declare -A _checkarg_args;
declare -A _checkarg_args_cb;
declare -A _checkarg_args_help;
declare -A _checkarg_args_values;
declare -A _checkarg_short_long;

_checkarg_appname=''
_checkarg_descr=''
_checkarg_appendix=''
_checkarg_pos_arg_sep=0
_checkarg_usage_line=''
_checkarg_posarg_descr=''
_checkarg_posarg_usage=''

_checkarg_next_is_val_of=''

##
# "public" checkarg functions
##

##
# initializes checkarg
#
# $1: appname
# $2: description (opt.)
# $3: appendix (opt.)
##
function checkarg_init(){
	if [[ -z "$1" ]]; then
		return 1
	else

		_checkarg_appname="$1"
		if [[ -n "$2" ]]; then
			_checkarg_descr="$2"
		fi
		if [[ -n "$3" ]]; then
			_checkarg_appendix="$3"
		fi
	fi
}

##
# add an option to the list of valid options
#
# $1: sopt: char used for the short option
# $2: lopt: long name of the option
# $3: help: description used by "autohelp"
# $4: has_val: specify if option has a value, (opt.)
#              0 for no (default), 1 for yes
##
function checkarg_add(){
	if [[ -z "$1" ]] || [[ -z "$2" ]] || [[ -z $3 ]]; then
		return 1
	else
		eval "_checkarg_short_long[$1]='$2'"
		eval "_checkarg_args[$2]='${4:-0}'"
		eval "_checkarg_args_help[$2]='$3'"
	fi
}

##
# add an option to the list of valid options
# and register the given callback
#
# $1: sopt: char used for the short option
# $2: lopt: long name of the option
# $3: command: command or function to be called
# $4: help: description used by autohelp
# $5: has_val: specify if option has a value, (opt.)
#              0 for no (default), 1 for yes
##
function checkarg_add_cb(){
	if [[ -z "$1" ]] || [[ -z "$2" ]] || [[ -z "$3" ]] || [[ -z $4 ]]; then
		return 1
	else
		eval "_checkarg_short_long[$1]='$2'"
		eval "_checkarg_args[$2]='${5:-0}'"
		eval "_checkarg_args_help[$2]='$4'"
		eval "_checkarg_args_cb[$2]='$3'"
	fi
}

##
# add an option to the list of valid options
# without a short option
#
# $1: lopt: long name of the option
# $2: help: description used by "autohelp
# $3: has_val: specify if option has a value, (opt.)
#              0 for no (default), 1 for yes
##
function checkarg_add_long(){
	if [[ -z "$1" ]] || [[ -z "$2" ]]; then
		return 1
	fi

	eval "_checkarg_args[$1]='${3:-0}'"
	eval "_checkarg_args_help[$1]='$2'"
}

##
# add an option to the list of valid options
# without a short option but register a callback
#
# $1: lopt:    long name of the option
# $2: command: command or function to be called
# $3: help:    description used by "autohelp"
# $4: has_val: specify if option has a value, (opt.)
#              0 for no (default), 1 for yes
##
function checkarg_add_long_cb(){
	if [[ -z "$1" ]] || [[ -z "$2" ]] || [[ -z $3 ]]; then
		return 1
	fi

	eval "_checkarg_args[$1]='${4:-0}'"
	eval "_checkarg_args_help[$1]='$3'"
	eval "_checkarg_args_cb[$1]='$2'"
}
##
# this adds the options '-h' and '--help' with
# an automatically generated help-message.
##

function checkarg_add_autohelp(){
	checkarg_add_cb 'h' "help" "_checkarg_show_help" "show this help message and exit"
}


##
# Calling this will do the parsing and checking.
# It takes a list of passed arguments.
#
# You'd want to call it like this: checkarg_parse "$@"
#
# Note: Please make sure to add arguments to checkarg's list
# of valid arguments before calling this.
# Preferably using the checkarg_add* functions.
##
function checkarg_parse(){
	local ret=0

	for arg in "$@"; do
		_checkarg_arg "$arg";
		ret="$?"

		[[ "$ret" != 0 ]] && return "$ret";
	done

	if [[ -n "$_checkarg_next_is_val_of" ]]; then
		return 4
	fi

	# TODO: cleanup?
	#
	return "$ret"
}

##
# auto-help related setters
##

# $1: text to be appended to usage line for positional args
# $2: description of the positional args
function checkarg_set_posarg_help(){
	_checkarg_posarg_descr="$2"
	_checkarg_posarg_usage="$1"
}

# $1: set the usage line used in autohelp
function checkarg_set_usage_line(){
	_checkarg_usage_line="$1"
}

# $1: long opt name
# returns 0 if opt was given, 1 if not
function checkarg_isset(){
	[[ -n "${_checkarg_args_values[$1]}" ]]
}

# $1: long opt name
# prints argument value
function checkarg_value(){
	if [[ -n "${_checkarg_args_values[$1]}" ]]; then
		printf "${_checkarg_args_values[$1]}"
	else
		return 1
	fi
}

# $1: errno
# prints error message
function checkarg_strerr(){
	local ret=1
	case "$1" in
		0|CA_ALLOK)
			printf "Everything is fine"
			ret=0
			;;
		1|CA_ERROR)
			printf "An error occurred"
			ret=1
			;;
		2|CA_INVARG)
			printf "Unknown command line argument"
			ret=2
			;;
		3|CA_INVVAL)
			printf "Value given to non-value argument"
			ret=3
			;;
		4|CA_MISSVAL)
			printf "Missing value of argument"
			ret=4
			;;
		5|CA_CALLBACK)
			printf "Callback returned with error code"
			ret=5
			;;
		*)
			printf "An unknown error occurred"
			ret=-1
			;;
	esac
	return "$ret"
}

##
# "private" checkarg functions
##

# $1: arg
function _checkarg_arg(){
	local ret=0

	# if the separator '--' was given, all following args are posargs,
	# so skip any parsing and add them directly to posargs list.
	#
	if [[ "$_checkarg_pos_arg_sep" == 0 ]]; then

		if [[ -n "$_checkarg_next_is_val_of" ]]; then
			# got a value of an option?

			eval "_checkarg_args_values[$_checkarg_next_is_val_of]='$1'"

			if [[ -n "${_checkarg_args_cb[$_checkarg_next_is_val_of]}" ]]; then
				${_checkarg_args_cb[$_checkarg_next_is_val_of]} "$_checkarg_next_is_val_of" "$1"
				ret=$?
			fi

			_checkarg_next_is_val_of=''

			return $ret
		fi

		if [[ "${1:0:1}" = "-" ]]; then # it's an option
			if [[ "${1:1:1}" = "-" ]]; then # it's a long option
				if [[ "$1" = "--" ]]; then # it's the posarg sep: '--'
					_checkarg_pos_arg_sep=1
					return 0
				fi

				_checkarg_arg_long "${1:2}" # omit dashes
				return $?
			fi

			_checkarg_arg_short "${1:1}" # omit dash
			return $?
		fi
	fi

	# if it's fallen through to here, it's a positional arg
	checkarg_pos_args+=("$1")
	return 0
}

function _checkarg_arg_short(){
	local len=${#1} # length of the arg

	for i in $(eval echo "{0..$(( len - 1))}") ; do
		local arg="${1:$i:1}"
		local lopt="${_checkarg_short_long[$arg]}"

		if [[ -n "$lopt" ]]; then
			if [[ "${_checkarg_args[$lopt]}" -gt 0 ]]; then
				local value="${1:$(( i+1  ))}"
				if [[ -n "${value}" ]]; then
					eval "_checkarg_args_values[$lopt]='${value}'" # _checkarg_args_values="${1:$(( i+1 ))}"

					if [[ -n "${_checkarg_args_cb[$lopt]}" ]]; then
						${_checkarg_args_cb[$lopt]} "${lopt}" "${value}"
						return $?
					else
						return 0
					fi
				else
					_checkarg_next_is_val_of="$lopt"
				fi
				return 0
			else
				eval "_checkarg_args_values[$lopt]='x'" # mark non-value option as seen

				if [[ -n "${_checkarg_args_cb[$lopt]}" ]]; then
					${_checkarg_args_cb[$lopt]} "$lopt" ""; ret="$?"

					[ "$ret" -gt 0 ] && return "$ret"
				fi

			fi
		else
			return 2 # INVARG
		fi
	done
	return 0
}

# $1: long opt without dashes
function _checkarg_arg_long(){
	# if theres an '=' in the arg, check the first part
	local arg="${1%%=*}"
	local value="${1#*=}"

	if [[ -n "${_checkarg_args[$arg]}" ]]; then
		if [[ "${_checkarg_args[$arg]}" -gt 0 ]] && [[ "$arg" != "$value" ]]; then
			eval "_checkarg_args_values[$arg]='$value'"

		elif [[ "${_checkarg_args[$arg]}" -gt 0 ]]; then
			_checkarg_next_is_val_of="$arg"

		elif [[ "$arg" != "$value" ]]; then
			# value given to non-value option
			return 3
		else
			eval "_checkarg_args_values[$arg]='x'" # mark non-value option as seen
		fi

		if [[ "${_checkarg_args[$arg]}" = 0 ]] || [[ "$arg" != "$value" ]]; then
			if [[ -n ${_checkarg_args_cb[$arg]} ]]; then
				${_checkarg_args_cb[$arg]} "$arg" "$value"
				return $?
			fi
		fi
		return 0

	else
		#echo "Error: Unknown long command line argument: $1" >&2
		return 2
	fi
}

function _checkarg_show_help(){
	local space=0
	local keys

	if [[ -z $ZSH_VERSION ]]; then
		keys=( "${!_checkarg_args[@]}" )
	else
		keys=( "${(@k)_checkarg_args}" )
	fi

	for key in "${keys[@]}"; do
		space="$(( space > ${#key} ? space : ${#key} ))"
	done

	printf "Usage: %s [options]%s\n\n" "$_checkarg_appname" "$_checkarg_posarg_usage"

	if [[ -n "$_checkarg_descr" ]]; then
		printf "$_checkarg_descr\n\n"
	fi

	printf "Options:\n"

	for key in ${keys[@]}; do

		sopt="$(_checkarg_find_sopt "$key")"
		if [[ -z "$sopt" ]]; then
			printf "       "
		else
			printf "   -%s, " "$sopt"
		fi

		printf -- "--%s%*s  %s\n" "$key" "$(( space - ${#key} ))" "" "${_checkarg_args_help[$key]}"
	done

	if [[ -n "$_checkarg_posarg_descr" ]]; then
		printf "\nPositional Arguments:\n%s\n" "$_checkarg_posarg_descr"
	fi

	if [[ -n "$_checkarg_appendix" ]]; then
		printf "\n%s\n" "$_checkarg_appendix"
	fi

	exit 0
}

function _checkarg_find_sopt(){
	local keys
	if [[ -z $ZSH_VERSION ]]; then
		keys=( "${!_checkarg_short_long[@]}" )
	else
		keys=( "${(@k)_checkarg_short_long}" )
	fi

	for key in "${keys[@]}"; do
		if [[ "${_checkarg_short_long[$key]}" == "$1" ]]; then
			printf -- "$key"
			return
		fi
	done
	return 1
}

if [[ $_revert_nounset -gt 0 ]]; then
	set +o nounset
fi

fi # CHECKARG_SH
