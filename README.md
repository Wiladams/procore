# procore
Programming Core - Data Structures and routines that get used over and over again


The files here have been culled from various projects such as ndt, waavstudio, vdj, srt, and whatnot.  The routines and structures might evolve, but they are relatively stable in their intent.  These routines skew towards handling memory for serialization and tokenization.<p>

There are two levels of implementation.  One level is in the src/c directory.  These implementations
are pure C, and provide an easy interface to scripting environments.  The other is in the src/cplusplus
directory.  These are classes and structures that are meant to be easily incorporated into C++
code bases.<p>

Everything is typically in a header, so they're pretty easy to integrate into other things.  There's no 'library' and thus no 'build' as such.  Just grab what you need and copy it into your project, and you're all set.<p>

Namespace:  For the C++ side, everything is in the 'pcore' namespace.  The C side does not have namespaces, nor reference parameters.<p>

Also, there may be C++ v20 dependencies.  Rolling back to C++ 11 will probably make it even more universal.  Something to tackle over time.


Design Goals and Constraints
    * Do not use exceptions
    * Do not use templates if at all possible
    * Do not use standard library calls
    * Do not depend on anything outside these files

These are just design principles, rather than hard fast rules.  The general idea being, keep things as simple as possible to make it more likely the routines can be used in different environments.
