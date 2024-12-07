#ifndef XMLSCAN_H_INCLUDED
#define XMLSCAN_H_INCLUDED


#include "xmlcore.h"

#ifdef __cplusplus
extern "C" {
#endif


//============================================================
// readCData()
//============================================================

static int xml_scan_read_cdata(bspan *src, bspan * dataChunk) PC_NOEXCEPT_C
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
static int xml_scan_read_comment(bspan * src, bspan *dataChunk) PC_NOEXCEPT_C
{
	// Skip past the !--
    bspan_advance(src, 3);

    bspan_weak_assign(dataChunk, src);

	dataChunk->fEnd = src->fStart;

	// Extend the data chunk until we find the closing -->
	//bspan endComment = chunk_find_cstr(src, "-->");
    bspan endComment;
    lex_find_cstr(src, "-->", &endComment);

	dataChunk->fEnd = endComment.fStart;

    src->fStart = endComment.fEnd++;
    //bspan_weak_assign(src, &endComment);

	// skip past the end of the comment  '-->'
    bspan_advance(src, 3);

	return 0;
}
    
    //============================================================
	// readEntityDeclaration()
    // A processing instruction.  We are at the beginning of
    // !ENTITY  name 'quoted value' >
    // Return a name and value
	//============================================================
static int xml_scan_read_entity_declaration(bspan *src, bspan *dataChunk) PC_NOEXCEPT_C
{
    // Skip past the !ENTITY
    bspan_advance(src, 7);

    dataChunk = src;

	// skip until we see the closing '>' character
	lex_find_char(src, '>', src);

    bspan_set_end(dataChunk, bspan_begin(src));

    // skip past that character and return
	bspan_advance(src, 1);

    return 0;
}
    
//============================================================
// xml_scan_read_doctype
//
// Reads the doctype chunk, and returns it as a ByteSpan
// fSource is currently sitting at the beginning of !DOCTYPE
// Note: https://www.tutorialspoint.com/xml/xml_dtds.htm
//============================================================
static int xml_scan_read_doctype(bspan * src, bspan * dataChunk) PC_NOEXCEPT_C
{
        // skip past the !DOCTYPE to the first whitespace character
        src += 8;


        // Skip past the whitespace
        // to get to the beginning of things
        lex_ltrim(src, wspcharset());

        // Get the name of the root element
        bspan tok;
        lex_front_token(src, wspcharset(), wspcharset(), &tok, src);

        // Trim whitespace as usual
        lex_ltrim(src, wspcharset());
        
        // If the next thing we see is a '[', then we have 
        // an 'internal' DTD.  Read to the closing ']>' and be done
		if (bspan_front(src) == '[')
		{
			// Skip past the opening '['
            bspan_advance(src, 1);

			// Find the closing ']>'
			bspan endDTD;
            lex_find_cstr(src, "]>", &endDTD);
			bspan_init_from_pointers(dataChunk, src->fStart, endDTD.fStart);

			// Skip past the closing ']>'
			src->fStart = endDTD.fStart + 2;

            return 0;
		}
        
        // If we've gotten here, we have an 'external' DTD
		// It can either be a SYSTEM or PUBLIC DTD
        // First check for a PUBLIC DTD
		if (lex_begins_with_cstr(src, "PUBLIC"))
		{
			// Skip past the PUBLIC
            bspan_advance(src, 6);

            bspan publicId{};
			bspan systemId{};
            
            // Skip white space before the quoted bytes
            lex_ltrim(src, xmlwspchars());
            lex_read_quoted(src, &publicId, src);
            
            // Skip white space before the quoted bytes
            lex_ltrim(src, xmlwspchars());
            lex_read_quoted(src, &systemId, src);

			// Skip past the whitespace
			lex_ltrim(src, wspcharset());

			// If we have a closing '>', then we're done
			if (bspan_front(src) == '>')
			{
                bspan_advance(src, 1);
                bspan_init_from_pointers(dataChunk, src->fStart, src->fStart);
                
				return true;
			}

			// If we have an opening '[', then we have more to parse
			if (bspan_front(src) == '[')
			{
				// Skip past the opening '['
                bspan_advance(src, 1);

				// Find the closing ']>'
				bspan endDTD;
                lex_find_cstr(src, "]>", &endDTD);
				
                bspan_init_from_pointers(dataChunk, bspan_begin(src), bspan_begin(&endDTD));


				// Skip past the closing ']>'
				src->fStart = endDTD.fStart + 2;
				
                return 0;
			}
		}
		else if (lex_begins_with_cstr(src, "SYSTEM"))
		{
			// Skip past the SYSTEM
            bspan_advance(src, 6);

            bspan systemId;

			lex_read_quoted(src, &systemId, src);

			// Skip past the whitespace
			lex_ltrim(src, wspcharset());

			// If we have a closing '>', then we're done
			if (bspan_front(src) == '>')
			{
                bspan_advance(src, 1);

				return true;
			}

			// If we have an opening '[', then we have more to parse
			if (bspan_front(src) == '[')
			{
				// Skip past the opening '['
                bspan_advance(src, 1);

				// Find the closing ']>'
				bspan endDTD;
                lex_find_cstr(src, "]>", &endDTD);
                bspan_init_from_pointers(dataChunk, bspan_begin(src), bspan_begin(&endDTD));

				// Skip past the closing ']>'
				src->fStart = endDTD.fStart + 2;
				
                return 0;
			}
		}

	// We have an invalid DTD
	return -1;
}
    
//============================================================
// readTag()
//============================================================
static int xml_scan_read_tag(bspan * src, bspan * dataChunk) PC_NOEXCEPT_C
{
    const unsigned char* srcPtr = bspan_begin(src);
	const unsigned char* endPtr = bspan_end(src);
        
    bspan_init_from_pointers(dataChunk, bspan_begin(src), bspan_begin(src));

    // BUGBUG - should be lex_find_char()
	while ((srcPtr < endPtr) && (*srcPtr != '>'))
		srcPtr++;
        
	// if we get to the end of the input, before seeing the closing '>'
    // the we return false, indicating we did not read
    if (srcPtr == endPtr)
		return false;
        
    // we did see the closing, so capture the name into 
    // the data chunk, and trim whitespace off the end.
    bspan_set_end(dataChunk, srcPtr);
    lex_rtrim(dataChunk, wspcharset());

    // move past the '>'
    srcPtr++;
    bspan_set_begin(src, srcPtr);


    return 0;
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

static int xml_iter_state_init(xmliteratorstate *s) PC_NOEXCEPT_C
{
    s->fState = XML_ITERATOR_STATE_CONTENT;
    bspan_init(&s->fSource);
    bspan_init(&s->fMark);

    return 0;
}

static int xml_iter_state_init_from_source(xmliteratorstate *s, bspan *src) PC_NOEXCEPT_C
{
    xml_iter_state_init(s);
    bspan_weak_assign(&s->fSource, src);
    bspan_weak_assign(&s->fMark, src);

    return 0;
}

static int xml_iter_is_valid(xmliteratorstate *s)
{
    return bspan_is_valid(&s->fSource);
}

static int xml_iter_params_init(xmliterparams *p) PC_NOEXCEPT_C
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
static int xml_iter_next_element(const xmliterparams * params, xmliteratorstate * st, xmlelement *elem) PC_NOEXCEPT_C
{
        
    while (xml_iter_is_valid(st))
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

                if (lex_begins_with_cstr(&st->fSource,"?xml"))
                {
                    kind = XML_ELEMENT_TYPE_XMLDECL;
                    xml_scan_read_tag(&st->fSource, &elementChunk);
                }
                else if (lex_begins_with_cstr(&st->fSource,"?"))
                {
                    kind = XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION;
                    xml_scan_read_tag(&st->fSource, &elementChunk);
                }
                else if (lex_begins_with_cstr(&st->fSource,"!DOCTYPE"))
                {
                    kind = XML_ELEMENT_TYPE_DOCTYPE;
                    xml_scan_read_doctype(&st->fSource, &elementChunk);
                }
                else if (lex_begins_with_cstr(&st->fSource,"!--"))
                {
                    kind = XML_ELEMENT_TYPE_COMMENT;
                    xml_scan_read_comment(&st->fSource, &elementChunk);
                }
                else if (lex_begins_with_cstr(&st->fSource,"![CDATA["))
                {
                    kind = XML_ELEMENT_TYPE_CDATA;
                    xml_scan_read_cdata(&st->fSource, &elementChunk);
                }
                else if (lex_begins_with_cstr(&st->fSource,"!ENTITY"))
                {
                    kind = XML_ELEMENT_TYPE_ENTITY;
                    xml_scan_read_entity_declaration(&st->fSource, &elementChunk);
                }
                else if (lex_begins_with_cstr(&st->fSource,"/"))
                {
                    kind = XML_ELEMENT_TYPE_END_TAG;
                    xml_scan_read_tag(&st->fSource, &elementChunk);
                }
                else {
                    xml_scan_read_tag(&st->fSource, &elementChunk);
                    if (bspan_back(&elementChunk) == '/')
                        kind = XML_ELEMENT_TYPE_SELF_CLOSING;
                }

                st->fState = XML_ITERATOR_STATE_CONTENT;
                
                bspan_weak_assign(&st->fMark, &st->fSource);

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
        
    return -1;
}
    
#ifdef __cplusplus
}
#endif

/*
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
*/


#endif  // XMLSCAN_H_INCLUDED
