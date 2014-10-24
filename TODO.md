General
=======
- support option and (positinal-)argument separator: --
    like in git checkout --option -- file1 file2


- Maybe add support for easier managing of multiple options,
  like ```programm -vvv``` or ```program -i file1 -i file2```.<br>
  Currently you'd have to use a callback,
  which would be called on every occurance of an option.<br>
  Like ```int verbose_cb() { verbose_level++; }``` to support ```-vvv```.<br>
  Havn't come up with any easy implementation of this yet.<br>
  I'll probably have to add another map for value options,
  like map&lt;string,vector&lt;string&gt;&gt;... <br>
  and some kind of counter for non-value options.

- maybe support (optionally) subcommands, like: git commit --option
  Currently, you'd have to check poss_args[0] yourself.


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
