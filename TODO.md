General
=======
- add ```ca.show_usage()``` and ```ca.show_help()``` to manually show the help or usage message,
  so for example one can output the usage line if expected positional arguments are missing

    => DONE: cpp, c TODO: bash, java
    
- add marker for options with values in help message, currently one can't see if an option takes a value or not
  if it isn mentioned in the help message.

- Maybe add support for easier managing of multiple options,
  like ```programm -vvv``` or ```program -i file1 -i file2```.<br>
  Currently you'd have to use a callback,
  which would be called on every occurance of an option.<br>
  Like ```[&verbose_level]() -> int { verbose_level++; }``` to support ```-vvv```.<br>
  Havn't come up with any easy implementation of this yet.<br>
  I'll probably have to add another map for value options,
  like map&lt;string,vector&lt;string&gt;&gt;... <br>
  and some kind of counter for non-value options.

- maybe support (optionally) subcommands, like: git commit --option
  Currently, you'd have to check pos_args[0] yourself.
  Separate CheckArg Objects for each subcommand would be sensible.
  Some technique to tell CA objects which arguments belong to the subcommand
  and which are global ones is needed. Maybe a simple index.

- maybe support default values for value-based options.
  should be simple to implement: an additional map and adapted function parameters.
  Values will have to be strings for simplicity.
  But commandline arguments are strings in the first place, so thats no prob.

- maybe support general checks for positional arguments,
  like number or type, number could be used for subcommands,
  if set to 1:

    add_posarg("subcmd", 1, 1, "helpmsg")

	number would get specified by minimum and maximum,
	max 0 disables maximum check
	min 0 disables minimum check
	so both 0 would be like * when globbing,
	any number of args get caught.
	Specify this only once and last.

  value("subcmd") could return a list,
  since that may be useful for "multiple options"

- maybe support checking of the type of the values of options.
  types could be string, int, float, ... maybe file, path or dir...

- maybe support no-prefixes, like ```--no-install``` as counterpart to ```--install```
  use value opts for that, which will be set to true or false respectively
  
- maybe support getopts-like prefix handling. With getopts a long option can be recognized
  by giving any unique prefix of that option.
  For example giving ```--inst``` may be recognized as ```--install``` if there is no other option starting with 'inst'.
  This should probably only activated if desired, since it might have a substancial performance impact.

C++ Port
========
- ...

C Port
======
- improve documentation

Bash/Zsh Port
=============
- ...

Java Port
=========
- ...
