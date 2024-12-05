#ifndef XMLSCAN_H_INCLUDED
#define XMLSCAN_H_INCLUDED


#include "xmlcore.h"

#ifdef __cplusplus
extern "C" {
#endif


    //============================================================
    // readCData()
    //============================================================

    static bool readCData(bspan *src, bspan * dataChunk) noexcept
    {
        // Skip past the ![CDATA[
        bspan_advance(src, 8);

        bspan_init_from_pointers(dataChunk, bspan_begin(src), bspan_begin(src));

        // Extend the data chunk until we find the closing ]]>
        bspan endCData;
        lex_find_cstr(src, "]]>", &endCData);
        dataChunk->fEnd = endCData.fStart;

        bspan_weak_assign(src, &endCData);

        // skip past the closing ]]>.
        bspan_advance(src, 3);

        return true;
    }

    //============================================================
    // readComment()
    //============================================================
    static bool readComment(bspan * src, bspan *dataChunk) noexcept
	{
		// Skip past the !--
        bspan_advance(src, 3);

        bspan_weak_assign(dataChunk, src);

		dataChunk->fEnd = src->fStart;

		// Extend the data chunk until we find the closing -->
		//bspan endComment = chunk_find_cstr(src, "-->");
        bspan endComment;
        lex_find_cstr(src, "-->", &endComment);

		dataChunk.fEnd = endComment.fStart;

        bspan_weak_assign(src, &endComment);

		// skip past the closing -->
        bspan_advance(src, 3);

		return true;
	}
    
    //============================================================
	// readEntityDeclaration()
    // A processing instruction.  We are at the beginning of
    // !ENTITY  name 'quoted value' >
    // Return a name and value
	//============================================================
    static bool readEntityDeclaration(bspan *src, bspan *dataChunk) noexcept
    {
        // Skip past the !ENTITY
        bspan_advance(src, 7);

        dataChunk = src;

		// skip until we see the closing '>' character
		src = chunk_find_char(src, '>');

        dataChunk.fEnd = src.fStart;

        // skip past that character and return
		src++;

		return true;
    }
    
    //============================================================
    // readDoctype
    // Reads the doctype chunk, and returns it as a ByteSpan
    // fSource is currently sitting at the beginning of !DOCTYPE
    // Note: https://www.tutorialspoint.com/xml/xml_dtds.htm
    //============================================================
    static bool readDoctype(bspan * src, bspan * dataChunk) noexcept
    {
        // skip past the !DOCTYPE to the first whitespace character
        src += 8;


        // Skip past the whitespace
        // to get to the beginning of things
        src = chunk_ltrim(src, xmlwsp);

        // Get the name of the root element
        auto element = chunk_token(src, xmlwsp);
        
        // Trim whitespace as usual
        src = chunk_ltrim(src, xmlwsp);
        
        // If the next thing we see is a '[', then we have 
        // an 'internal' DTD.  Read to the closing ']>' and be done
		if (*src == '[')
		{
			// Skip past the opening '['
			src++;

			// Find the closing ']>'
			ByteSpan endDTD = chunk_find_cstr(src, "]>");
			dataChunk = src;
			dataChunk.fEnd = endDTD.fStart;

			// Skip past the closing ']>'
			src.fStart = endDTD.fStart + 2;
            return true;
		}
        
        // If we've gotten here, we have an 'external' DTD
		// It can either be a SYSTEM or PUBLIC DTD
        // First check for a PUBLIC DTD
		if (src.startsWith("PUBLIC"))
		{
			// Skip past the PUBLIC
			src += 6;

            ByteSpan publicId{};
			ByteSpan systemId{};
            
            // Skip white space before the quoted bytes
            lex_ltrim(src, xmlwspchars());

            lex_read_quoted(src, publicId);
            
            // Skip white space before the quoted bytes
            lex_ltrim(src, xmlwspchars());
            lex_read_quoted(src, systemId);

			// Skip past the whitespace
			src = chunk_ltrim(src, xmlwsp);

			// If we have a closing '>', then we're done
			if (*src == '>')
			{
				src++;
                dataChunk.fStart = src.fStart;
				dataChunk.fEnd = src.fStart;
                
				return true;
			}

			// If we have an opening '[', then we have more to parse
			if (*src == '[')
			{
				// Skip past the opening '['
				src++;

				// Find the closing ']>'
				ByteSpan endDTD = chunk_find_cstr(src, "]>");
				dataChunk = src;
				dataChunk.fEnd = endDTD.fStart;

				// Skip past the closing ']>'
				src.fStart = endDTD.fStart + 2;
				return true;
			}
		}
		else if (src.startsWith("SYSTEM"))
		{
			// Skip past the SYSTEM
			src += 6;

            ByteSpan systemId{};
			readQuoted(src, systemId);

			// Skip past the whitespace
			src = chunk_ltrim(src, xmlwsp);

			// If we have a closing '>', then we're done
			if (*src == '>')
			{
				src++;
				return true;
			}

			// If we have an opening '[', then we have more to parse
			if (*src == '[')
			{
				// Skip past the opening '['
				src++;

				// Find the closing ']>'
				ByteSpan endDTD = chunk_find_cstr(src, "]>");
				dataChunk = src;
				dataChunk.fEnd = endDTD.fStart;

				// Skip past the closing ']>'
				src.fStart = endDTD.fStart + 2;
				return true;
			}
		}

		// We have an invalid DTD
		return false;
    }
    
    //============================================================
    // readTag()
    //============================================================

    static bool readTag(bspan * src, bspan * dataChunk) noexcept
    {
        const unsigned char* srcPtr = src.fStart;
		const unsigned char* endPtr = src.fEnd;
        
        dataChunk = src;
        dataChunk.fEnd = src.fStart;

		while ((srcPtr < endPtr) && (*srcPtr != '>'))
			srcPtr++;
        
		// if we get to the end of the input, before seeing the closing '>'
        // the we return false, indicating we did not read
        if (srcPtr == endPtr)
			return false;
        
        // we did see the closing, so capture the name into 
        // the data chunk, and trim whitespace off the end.
        dataChunk.fEnd = srcPtr;
        dataChunk = chunk_rtrim(dataChunk, xmlwsp);

        // move past the '>'
        srcPtr++;
        src.fStart = srcPtr;


        return true;
    }


// XML_ITERATOR_STATE
// This enum represents the states the xml iterator
// can be in.
enum XML_ITERATOR_STATE 
{
    XML_ITERATOR_STATE_CONTENT = 0
    , XML_ITERATOR_STATE_START_TAG

};
    
// xmliterparams_t
//
// The set of parameters that configure how the iterator
// will operate
struct xmliterparams_t {
    bool fSkipComments;
    bool fSkipProcessingInstructions;
    bool fSkipWhitespace;
    bool fSkipCData;
    bool fAutoScanAttributes;
};
typedef struct xmliterparams_t xmliterparams;

// XmlIteratorState
// The information needed for the iterator to continue
// after it has returned a value.
struct xmliteratorstate_t {
    int fState;
    bspan fSource;
    bspan fMark;
};
typedef struct xmliteratorstate_t xmliteratorstate;

static int xml_iter_state_init(xmliteratorstate *s) noexcept
{
    s->fState = XML_ITERATOR_STATE_CONTENT;
    bspan_init(&s->fSource);
    bspan_init(&s->fMark);

    return 0;
}

static int xml_iter_params_init(xmliterparams *p) noexcept
{
    p->fSkipComments = true;
    p->fSkipProcessingInstructions = false;
    p->fSkipWhitespace = true;
    p->fSkipCData = false;
    p->fAutoScanAttributes = false;

    return 0;
}

// XmlElementGenerator
// A function to get the next element in an iteration
static int xml_iter_next_element(const xmliterparams * params, xmliteratorstate * st, xmlelement *elem)
{
        
    while (bspan_is_valid(&st->fSource))
    {
        switch (st->fState)
        {
            case XML_ITERATOR_STATE_CONTENT: {

                if (bspan_front(&st->fSource) == '<')
                {
                    // Change state to beginning of start tag
                    // for next turn through iteration
                    st->fState = XML_ITERATOR_STATE_START_TAG;

                    if (!bspan_shallow_equal(&st->fSource, &st->fMark))
                    {
                        // Encapsulate the content in a chunk
                        bspan content;
                        bspan_init_from_pointers(&content, bspan_begin(&st->fMark), bspan_begin(&st->fSource));

                        // collapse whitespace
                        // if the content is all whitespace
                        // don't return anything
                        // BUGBUG - deal with XML preserve whitespace
                        if (params->fSkipWhitespace)
                            lex_trim(&content, xmlwspchars());
                        
                        if (bspan_is_valid(&content))
                        {
                            // Set the state for next iteration
                            bspan_advance(&st->fSource,1);
                            bspan_weak_assign(&st->fMark, &st->fSource);
                            xml_element_init_from_data(elem, XML_ELEMENT_TYPE_CONTENT, &content);

                            return 0;
                        }
                    }

                    bspan_advance(&st->fSource,1);
                    bspan_weak_assign(&st->fMark, &st->fSource);
                }
                else {
                    bspan_advance(&st->fSource, 1);
                }
            }
            break;

            case XML_ITERATOR_STATE_START_TAG: {
                // Create a chunk that encapsulates the element tag 
                // up to, but not including, the '>' character
                int kind = XML_ELEMENT_TYPE_START_TAG;
                bspan elementChunk;
                bspan_init_from_pointers(&elementChunk, bspan_begin(&st->fSource), bspan_begin(&st->fSource));

                if (st.fSource.startsWith("?xml"))
                {
                    kind = XML_ELEMENT_TYPE_XMLDECL;
                    readTag(st.fSource, elementChunk);
                }
                else if (st.fSource.startsWith("?"))
                {
                    kind = XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION;
                    readTag(st.fSource, elementChunk);
                }
                else if (st.fSource.startsWith("!DOCTYPE"))
                {
                    kind = XML_ELEMENT_TYPE_DOCTYPE;
                    readDoctype(st.fSource, elementChunk);
                }
                else if (st.fSource.startsWith("!--"))
                {
                    kind = XML_ELEMENT_TYPE_COMMENT;
                    readComment(st.fSource, elementChunk);
                }
                else if (st.fSource.startsWith("![CDATA["))
                {
                    kind = XML_ELEMENT_TYPE_CDATA;
                    readCData(st.fSource, elementChunk);
                }
                else if (st.fSource.startsWith("!ENTITY"))
                {
                    kind = XML_ELEMENT_TYPE_ENTITY;
                    readEntityDeclaration(st.fSource, elementChunk);
                }
                else if (st.fSource.startsWith("/"))
                {
                    kind = XML_ELEMENT_TYPE_END_TAG;
                    readTag(st.fSource, elementChunk);
                }
                else {
                    readTag(st.fSource, elementChunk);
                    if (chunk_ends_with_char(elementChunk, '/'))
                        kind = XML_ELEMENT_TYPE_SELF_CLOSING;
                }

                st->fState = XML_ITERATOR_STATE_CONTENT;
                st.fMark = st.fSource;

                xml_element_init_from_data(elem, kind, &elementChunk);

                return 0;
            }
            break;

            default:
                // Just advance to next character
                bspan_advance(&st->fSource,1);
                break;

        }
    }
        
    return 0;
}
    
#ifdef __cplusplus
}
#endif

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



#endif  // XMLSCAN_H_INCLUDED
