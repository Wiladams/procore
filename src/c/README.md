
** pcoredef.h **<p>
Contains some useful macros that are used by all functions, such as PC_NOEXCEPT_C<p>

** asciiset.h **<p>
A set that allows you to set singular bits to represent ascii numbers from 0 to 255.  This is
in use to avoid pulling in std::bitset, which is not useable in C.<p>

** bithacks.h **<p>
A number of bit manipulation routines.<p>

** bspan.h **<p>
Represents a contingous span of bytes.  The bspan does not 'own' the memory, it only points at it,
providing convenienc routines<p>

** convspan.h **<p>
Various routines to convert from bytes to numeric values.  All of the standard integers, plus double values are directly converted from their byte patterns.  Additionally, there are routines to parse text representations of numeric values into the standard numbers.<p>

** lc3.h **<p>
A Little Computer 3 (LC3) simulator.  This single file will run programs compiled to run against an lc3 simulator.<p>

** lexutil.h **<p>
Various routines that operate against bspan.  Trimming leading and trailing characters, separating out tokens, and various other useful routines that are not in the bspan core itself.

** mbuff.h **<p>
Representation of a chunk of memory. The mbuff owns the data, and if the structure is destroyed, the memory it contains will be freed.<p>


** xmlcore.h **<p>
Core data types to represent an XML Element, it namespace, and attributes<p>

** xmlscan.h **<p>
A very simple pull model xml scanner, that works in the fashion of an iterator.  The pull model makes it relatively easy to construct a DOM, or perform any other operations on a stream of XML tokens.<p>
