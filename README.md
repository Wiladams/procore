# procore
Programming Core - Data Structures and routines that get used over and over again


The files here have been culled from various projects such as ndt, waavstudio, vdj, srt, and whatnot.  The routines and structures might evolve, but their core intent remains fairly stable.  These routines skew towards handling memory for serialization and tokenization.<p>

The current implementation has two different interfaces.  One level is in the src/c directory.  These implementations are pure C, and provide an easy interface to scripting environments.  The other is in the src/cplusplus directory.  These are classes and structures that are meant to be easily incorporated into C++
code bases.<p>

The 'C' interfaces are pretty low level, and to a certain extent inconvenient, but they should be accessible
from everywhere, with the lowest level of C99 compiler.  If that is your constraint, then use those 
interfaces.<p>

The 'C++' interfaces mostly wrap, and extend the C interfaces with syntactic sugar and conveniences.  If you
can use C++, this will make code much more succinct.<p>

Everything is is presented in header files, so there is no 'library' as such.  The easiest way to integrate this code into your own project is to simply copy and include the desired header files into your own project, and compile away.<p>

Namespace:  For the C++ side, everything is in the 'pcore' namespace.  The C side does not have namespaces, nor reference parameters.<p>

The C++ dependencies should be at the level of C++ 11, but some conveniences from higher levels might creep in from time to time.  The intention is to require nothing more than C++11 level of compatibility.<p>


== Design Goals and Constraints
    * Do not use exceptions<p>
    * Do not use templates if at all possible<p>
    * Do not required standard libraries if at all possible<p>
    * Do not depend on anything outside these files<p>

These are just design principles, rather than hard fast rules.  The general idea being, keep things as simple and independent as possible to make it more likely the routines can be used in different environments.<p>
