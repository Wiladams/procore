# procore
Programming Core - Data Structures and routines that get used over and over


The files here have been culled from various projects such as ndt, waavstudio, vdj, srt, and whatnot.  The routines and structures
continuously evolve, but they seem the be relatively stable.

Everything is in a header, so they're pretty easy to integrate into other things.  There's no 'library' and thus no 'build' to worry about.  Just grab what you need and copy it into your project, and you're all set.

Namespace:  On initial checking, everything is in the 'waavs' namespace.  This will change to the 'pcore' namespace so it's more generic.

Also, there may be C++ v20 dependencies.  Rolling back to C++ 11 will probably make it even more universal.  Something to tackle over time.


Design Goals and Constraints
    * Do not use exceptions
    * Do not use templates (Exceptions: std::bitset)
    * Do not use standard library calls
    * Do not depend on anything outside these files
