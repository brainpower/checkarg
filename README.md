CheckArg<br>
is a lightweight and easy to use command line parser.

It's inspired by ArgumentParser from python,<br>
therefore, if you've used that one before, you'll find this one quite similar.<br>
It is, intentionally, in no way a complete reimplementation or port of the former.

The main goal is for it to be easy to understand and use.<br>
Secondly, its intended to be non-intrusive and lightweight.<br>
Therefore it pulls no 'external' dependencies and relies completely on the STL (C++),
Java API or C Standard Library, respectively.

That way it's pretty easy to integrate into an existing project,<br>
because it's non-intrusive, you dont need to 'build your project arount it'.

Commandline options use the common so-called GNU style,<br>
e.g. use double dashes and look something like these: 

    program --option=value --quiet --option2 <value>
    
Additionally, abreviations of those in so-called POSIX style (or UNIX style) can be added.<br>
They look like this: 

    program -o [value] -q -f <value>
    
And they can be combined:

    program -o <value> -qf <value>
    program -o <value> -qf<value>
    
Note that above commands are equivalent.
This means after a value type option is found in a grouped option
the remainder is always considered to be the value,
even if you meant it to be an option.<br>
So these commands are not the same:

    program -o <value> -qfi other_file
    program -o <value> -q -f other_file -i

In the first command, 'i' is the value for -f,
other_file is a positional argument, not the value of either -f or -i.


In the docs I'll call the GNU style and POSIX style options "long" and "short" options, respectively.

Old style or traditional style options are *not* supported and I don't plan to do so.<br>
You may convince me otherwise, though.<br>
Old style options (like tar supports) look like this:

    tar xfL <value_for_f> <value_for_L>
