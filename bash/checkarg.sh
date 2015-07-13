##
# This is a getopt-like function, it uses the contents
# of the associative array 'checkarg_valid_args', the keys are the options,
# the values are passed to eval. You can set global vars that way
# or execute functions... well, everything eval accepts.
#
# It supports value-type options, you have to indicate this by setting
# 'checkarg_has_val' to the var name in which the value shall be stored.
# (without the preceeding dollar)
# But there's a convenience function for that: checkarg_add_val
#
# Long options are also supportet, just use '-help' for '--help'
# as key with checkarg_args, like this gets you a '--version' option:
#  checkarg_add '-version' 'print_version=1' "Print programm's version and exit."
# Or simply use the convenience function checkarg_add_long.
#
# Positional arguments are appended to 'checkarg_pos_args'
# which is declared as indexed array on sourcing this file..
# Those are arguments passed which are not preceeded by a dash
# and are not a value of an value-type argument.
#
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
# $1: sopt
# $2: lopt
# $3: help
# $4: has_val (0 no, 1 yes)
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
# $1: sopt
# $2: lopt
# $3: command name
# $4: help
# $5: has_val (0 no, 1 yes)
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
# $1: lopt
# $2: help
# $3: has_val (opt.)
##
function checkarg_add_long(){
	if [[ -z "$1" ]] || [[ -z "$2" ]]; then
		return 1
	fi

	eval "_checkarg_args[$1]='${3:-0}'"
	eval "_checkarg_args_help[$1]='$2'"
}

##
# $1: lopt
# $2: command name
# $3: help
# $4: has_val (opt.)
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
# this adds the arguments '-h' and '--help' with
# an automatically generated help-message.
##

function checkarg_add_autohelp(){
	checkarg_add_cb 'h' "help" "_checkarg_show_help" "show this help message and exit"
}


##
# Calling this will do the parsing and checking.
# It takes a list of passed arguments.
#
# You'd want to call it like this: checkarg "$@"
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

		[[ "$ret" != 0 ]] && break;
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
				${_checkarg_args_cb[$1]} "$_checkarg_next_is_val_of" "$1"
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
				if [[ -n "${1:$(( i+1 ))}" ]]; then
					_checkarg_args_values="${1:$(( i+1 ))}"
					if [[ "${_checkarg_args[$lopt]}" = 0 ]]; then
						if [[ -n "${_checkarg_args_cb[$lopt]}" ]]; then
							${_checkarg_args_cb[$lopt]} "$lopt" "$value"
							return $?
						else
							return 0
						fi
					fi
				else
					_checkarg_next_is_val_of="$lopt"
				fi
				return 0
			else
				_checkarg_args_values="x" # mark non-value opt as seen

				if [[ "${_checkarg_args[$lopt]}" = 0 ]]; then
					if [[ -n "${_checkarg_args_cb[$lopt]}" ]]; then
						${_checkarg_args_cb[$lopt]} "$lopt" ""
						return $?
					else
						return 0
					fi
				fi
				return 0

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
			_checkarg_args_values="x" # mark non-value option as seen
		fi

		if [[ "${_checkarg_args[$arg]}" = 0 ]] || [[ "$arg" != "$value" ]]; then
			${_checkarg_args_cb[$1]} "$_checkarg_next_is_val_of" "$1"
			return $?
		fi
		return 0

	else
		#echo "Error: Unknown long command line argument: $1" >&2
		return 2
	fi
}

function _checkarg_show_help(){
	local space=0

	for key in ${!_checkarg_args[@]}; do
		space="$(( space > ${#key} ? space : ${#key} ))"
	done

	printf "Usage: %s [options]%s\n\n" "$_checkarg_appname" "$_checkarg_posarg_usage"

	if [[ -n "$_checkarg_descr" ]]; then
		printf "$_checkarg_descr\n\n"
	fi

	printf "Options:\n"

	for key in ${!_checkarg_args[@]}; do

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
	for key in "${!_checkarg_short_long[@]}"; do
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
