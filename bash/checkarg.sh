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
#
# Positional arguments are appended to 'checkarg_pos_args'
# which is declared as indexed array on sourcing this file..
# Those are arguments passed which are not preceeded by a dash
# and are not a value of an value-type argument.
#
# Requires bash >= 4 or other compatible shell, like zsh
#
#
#  Copyright (C) 2013  brainpower <brainpower at gulli dot com>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

if [ -z "$CHECKARG_SH" ]; then # include guard
  CHECKARG_SH=1

##
# initialize global vars used by checkarg
##
declare -a  checkarg_pos_args;
declare -A _checkarg_valid_args;
declare -A _checkarg_autohelp;
_checkarg_has_val=''
_checkarg_autohelp_descr=''
_checkarg_autohelp_appendix=''
_checkarg_autohelp_posargname=''

##
# "public" checkarg functions
##

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
function checkarg(){
	for arg in "$@"; do
		_checkarg_arg "$arg" || return 1
	done
}

##
# This adds a command line argument and the code to be evaluated,
# if it's found, to the list of valid arguments.
# You can also pass it a short description of the argument,
# which will be used by 'checkarg_autohelp'.
# This description is optional, but note that autohelp
# wont print any decent description if you dont specify one.
#
# Of course long arguments can also be value-type arguments.
# Even arguments of the form --long=value are supported
# You could add all four types with this function,
# but I'd recommend to use the convenience-functions below wherever possible.
#
# Examples: (for simple, long, value-type and long value-type argument)
#    checkarg_add 'v' 'print_version=1' "Print programm's version and exit."
#    checkarg_add '-version' 'print_version=1' "Print programm's version and exit."
#    checkarg_add 'i' '_checkarg_has_val=input_file' 'The file to read from.'
#    checkarg_add '-input' '_checkarg_has_val=input_file' 'The file to read from.'
##
function checkarg_add(){
	if [ -n "$2" ]; then eval "_checkarg_valid_args[$1]='$2'"
	else                 eval "_checkarg_valid_args[$1]=''"
	fi
	if [ -n "$3" ]; then eval "_checkarg_autohelp[$1]='$3'"
	else                 eval "_checkarg_autohelp[$1]=''"
	fi
}

##
# This is a convenience function for adding value-type arguments.
#
# Example:
#    checkarg_add_val 'i' 'input_file' 'The file to read from.'
##

function checkarg_add_val(){
	checkarg_add "$1" "_checkarg_has_val='$2'" "$3"
}

##
# This is a convenience function for adding long arguments.
#
# Example:
#    checkarg_add_long 'version' 'print_version=1' "Print programm's version and exit."
##

function checkarg_add_long(){
	checkarg_add "-$1" "$2" "$3"
}

##
# This is a convenience function for adding long value-type arguments.
#
# Example:
#    checkarg_add_long_val 'input' 'input_file' 'The file to read from.'
##

function checkarg_add_long_val(){
	checkarg_add_long "$1" "_checkarg_has_val='$2'" "$3"
}

##
# this adds the arguments '-h' and '--help' with
# an automatically generated help-message.
# It takes the name of the programm.
#
# Example:
#    checkarg_add_autohelp "mySuperScript.sh"
##

function checkarg_add_autohelp(){
	checkarg_add_long 'help' "_checkarg_show_help '$1'" "show this help message and exit"
	checkarg_add      'h'    "_checkarg_show_help '$1'" "show this help message and exit"
	_checkarg_autohelp_descr="$2"
	_checkarg_autohelp_appendix="$3"
	_checkarg_autohelp_posargname="$4"
}

##
# "private" checkarg functions
##

function _checkarg_arg(){
	if [ -n "$_checkarg_has_val" ]; then
		_checkarg_eval_has_val "$1"
	elif [ "${1:0:1}" = "-" ]; then # check if it is an argument or just weirdness
		if [ "${1:1:1}" = "-" ]; then # it's a long arg
			_checkarg_long_arg "$1"
		else # it's a short one
			_checkarg_short_arg "$1"
		fi
	else
		checkarg_pos_args+=("$1")
		return 0 # ignore everything unknown
	fi
}

function _checkarg_eval_has_val(){
	eval "$_checkarg_has_val=\"$1\""
	_checkarg_has_val=''
}

function _checkarg_short_arg(){
	local str="${1#?}"
	local len=${#str} # length of the string, minus the dash

	for i in $(eval echo "{1..$len}") ; do
		local arg="${1:$i:1}"
		local ev="${_checkarg_valid_args[$arg]}"
		if [ -n "${ev}" ]; then
			eval "${ev}"

			if [ -n "$_checkarg_has_val" ] && [ ! $i = $len ]; then
			#  if arg is of value-type and val-type arg is not last one in arg-group -> remainder is value
				(( i++ ))
				_checkarg_eval_has_val "${1:$i}" # set the var specified in has_val to this string
				return 0 # stop parsing
			fi
		else
			echo "Error: Unknown short command line argument: -${1:$i:1}" >&2
			return 2
		fi
	done
}

function _checkarg_long_arg(){
	# if theres an '=' in the arg, check the first part
	local raw="${1:1}"
	local arg="${raw%%=*}"

	if [ -n "${_checkarg_valid_args[$arg]}" ]; then
		eval "${_checkarg_valid_args[$arg]}"
		if [ -n "$_checkarg_has_val" ] && [ ! "$arg" = "$raw" ]; then
			# if arg is of value-type and has an '=' in it -> remainder is value
			_checkarg_eval_has_val "${raw#*=}" # set the var specified in has_val to this string
		fi
	else
		echo "Error: Unknown long command line argument: $1" >&2
		return 2
	fi
}

function _checkarg_show_help(){
	echo "Usage: $1 [Options] $_checkarg_autohelp_posargname"
	echo ""
	echo "$_checkarg_autohelp_descr"
	echo ""
	echo "Options:"
	for key in ${!_checkarg_valid_args[@]}; do
		echo -e "   -$key   \t${_checkarg_autohelp[$key]}"
	done
	echo "$_checkarg_autohelp_appendix"

	exit 0
}

fi # CHECKARG_SH
