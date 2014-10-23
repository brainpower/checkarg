General
=======
- Rename either cpp or c port a little to allow parallel installation
  of those on a system. Currently they've got the same library-filename.
  Maybe something among libcheckargxx.so, libcheckargpp.so or libcheckarg++.so.

- add copyright and license notes to all files

- Maybe add support for easier managing of multiple options,
  like ```programm -vvv``` or ```program -i file1 -i file2```.<br>
  Currently you'd have to use a callback,
  which would be called on every occurance of an option.<br>
  Like ```int verbose_cb() { verbose_level++; }``` to support ```-vvv```.<br>
  Havn't come up with any easy implementation of this yet.<br>
  I'll probably have to add another map for value options,
  like map<string,vector<string>>... <br>
  and some kind of counter for non-value options.


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
