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

    --option[=value]
    --option [value]
    
Additionally, abreviations of those in so-called POSIX style can be added.<br>
They look like this: 

    -o [value]
    
In the docs I'll call them long and short options, respectively.
