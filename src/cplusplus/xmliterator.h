#include "xmlscan.h"

namespace pcore {
    // XmlElementIterator
    // scans XML generating a sequence of XmlElements
    // This is a forward only non-writeable iterator
    // 
    // Usage:
    //   XmlElementIterator iter(xmlChunk, false);
    // 
    //   while(iter.next())
    //   {
    //     printXmlElement(*iter);
    //   }
    //
    //.  or, you can do it this way
    //.  
    //.  iter++;
    //.  while (iter) {
    //.    printXmlElement(*iter);
    //.    iter++;
    //.  }
    //
    struct XmlElementIterator {
    private:
        xmliterparams fParams{};
        xmliteratorstate fState{};
        xmlelement fCurrentElement{};

    public:
		XmlElementIterator(const bspan& inChunk, bool autoScanAttributes = false)
            : fState{ XML_ITERATOR_STATE_CONTENT, inChunk, inChunk }
        {
			fParams.fAutoScanAttributes = autoScanAttributes;
            // We do not advance in the constructor, leaving that
            // for the user to call 'next()' to get the first element
            //next();
        }

		// return 'true' if the node we're currently sitting on is valid
        // return 'false' if otherwise
        explicit operator bool() { return xml_element_is_valid(&fCurrentElement); }

        // These operators make it operate like an iterator
        const xmlelement& operator*() const { return fCurrentElement; }
        const xmlelement* operator->() const { return &fCurrentElement; }

        // Increment the iterator either pre, or post notation
        XmlElementIterator& operator++() { next(); return *this; }
        XmlElementIterator& operator++(int) { next(); return *this; }

        // Return the current value of the iteration
        const xmlelement & next() 
        {
            xml_iter_next_element(&fParams, &fState, &fCurrentElement);

            return fCurrentElement;
        }
    };
}   // namespace pcore

