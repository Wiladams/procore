#ifndef XMLCORE_H_INCLUDED
#define XMLCORE_H_INCLUDED

#include "pcoredef.h"
#include "bspan.h"
#include "lexutil.h"


#ifdef __cplusplus
extern "C" {
#endif

enum XML_ELEMENT_TYPE {
    XML_ELEMENT_TYPE_INVALID = 0
	, XML_ELEMENT_TYPE_XMLDECL                      // <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    , XML_ELEMENT_TYPE_START_TAG                    // <tag>
	, XML_ELEMENT_TYPE_END_TAG                      // </tag>
	, XML_ELEMENT_TYPE_SELF_CLOSING                 // <tag/>
	//, XML_ELEMENT_TYPE_EMPTY_TAG                    // <br>
	, XML_ELEMENT_TYPE_CONTENT                      // <tag>content</tag>
	, XML_ELEMENT_TYPE_COMMENT                      // <!-- comment -->
	, XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION       // <?target data?>
	, XML_ELEMENT_TYPE_CDATA                        // <![CDATA[<greeting>Hello, world!</greeting>]]>
	, XML_ELEMENT_TYPE_DOCTYPE                      // <!DOCTYPE greeting SYSTEM "hello.dtd">
	, XML_ELEMENT_TYPE_ENTITY                       // <!ENTITY hello "Hello">
};

//============================================================
// XmlName
//============================================================

struct xmlname_t {
    bspan fNamespace;
    bspan fName;
};
typedef struct xmlname_t xmlname;

static INLINE int xml_name_init(xmlname *a) PC_NOEXCEPT_C;
static INLINE int xml_name_reset(xmlname *a, const bspan * inChunk) PC_NOEXCEPT_C;
static INLINE int xml_name_weak_assign(xmlname *a, const xmlname *b) PC_NOEXCEPT_C;
static INLINE int xml_name_name(const xmlname *a, bspan *name) PC_NOEXCEPT_C;
static INLINE int xml_name_namespace(const xmlname *a, bspan *nspace) PC_NOEXCEPT_C;
static INLINE int xml_name_equals(const xmlname* a, const char* nspace, const char* name) PC_NOEXCEPT_C;







struct xmlelement_t
{
    int fElementKind;
    bspan fData;
    bspan fNameSpan;
    xmlname fXmlName;
};
typedef struct xmlelement_t xmlelement;

static INLINE int xml_element_init(xmlelement *e) PC_NOEXCEPT_C;
static INLINE int xml_element_iniit_copy(xmlelement *a, const xmlelement *b) PC_NOEXCEPT_C;

static INLINE int xml_element_is_valid(const xmlelement *e) PC_NOEXCEPT_C;
static INLINE int xml_element_scanNameSpan(xmlelement *e) PC_NOEXCEPT_C;

static INLINE int xml_element_is_kind(const xmlelement *e, int kind) PC_NOEXCEPT_C;
static INLINE int xml_element_get_kind(const xmlelement *e) PC_NOEXCEPT_C;
static INLINE int xml_element_set_kind(xmlelement *e, int kind) PC_NOEXCEPT_C;

static INLINE int xml_element_get_data(const xmlelement *e, bspan *d) PC_NOEXCEPT_C;




//
// IMPLEMENTATION
//
static INLINE asciiset * xmlwspchars() PC_NOEXCEPT_C
{
    static asciiset chars;
    static bool initialized=false;

    if (!initialized){
        asciiset_init_from_cstr(&chars, " \r\n\t");
        initialized = true;
    }

    return &chars;
}

static INLINE asciiset * xmlcolonchar() PC_NOEXCEPT_C
{
    static asciiset chars;
    static bool initialized=false;

    if (!initialized){
        asciiset_init_from_cstr(&chars, ":");
        initialized = true;
    }

    return &chars;
}

// xmlname
//
static INLINE int xml_name_init(xmlname *a) PC_NOEXCEPT_C
{
    bspan_init(&a->fName);
    bspan_init(&a->fNamespace);

    return 0;
}

static INLINE int xml_name_reset(xmlname *a, const bspan * inChunk) PC_NOEXCEPT_C
{
    lex_front_token(inChunk, xmlwspchars(), xmlcolonchar(), &a->fNamespace, &a->fName);

    if (!bspan_is_valid(&a->fName))
    {
        bspan_init_copy(&a->fName, &a->fNamespace);
        bspan_reset(&a->fNamespace);
    }

    return 0;
}

static INLINE int xml_name_weak_assign(xmlname *a, const xmlname *b) PC_NOEXCEPT_C
{
    bspan_init_copy(&a->fName, &b->fName);
    bspan_init_copy(&a->fNamespace, &b->fNamespace);

    return 0;
}

static INLINE int xml_name_name(const xmlname *a, bspan *name) PC_NOEXCEPT_C
{
    bspan_init_copy(name, &a->fName);
    return 0;
}

static INLINE int xml_name_namespace(const xmlname *a, bspan *nspace) PC_NOEXCEPT_C
{
    bspan_init_copy(nspace, &a->fNamespace);
    return 0;
}

// name comparison
static INLINE int xml_name_equals(const xmlname* a, const char* nspace, const char* name) PC_NOEXCEPT_C
{

}



// Representation of an xml element
// The xml scanner will generate these
static INLINE int xml_element_init(xmlelement *e) PC_NOEXCEPT_C
{
    if (NULL == e)
        return -1;
    
    e->fElementKind = XML_ELEMENT_TYPE_INVALID;
    bspan_init(&e->fData);
    bspan_init(&e->fNameSpan);
    xml_name_init(&e->fXmlName);

    return 0;
}

// A copy constructor, or assignment operator
static INLINE int xml_element_init_copy(xmlelement *a, const xmlelement *b) PC_NOEXCEPT_C
{
    if ((NULL == a) || (NULL == b))
        return -1;
    
    a->fElementKind = b->fElementKind;
    bspan_init_copy(&a->fNameSpan, &b->fNameSpan);
    bspan_init_copy(&a->fData, &b->fData);
    xml_name_weak_assign(&a->fXmlName, &b->fXmlName);
}

static INLINE int xml_element_init_from_data(xmlelement *e, int kind, const bspan * data) PC_NOEXCEPT_C
{
    if (NULL == e)
        return -1;
    
    xml_element_init(e);

    e->fElementKind = kind;
    bspan_init_copy(&e->fData, data);


    if ((kind == XML_ELEMENT_TYPE_START_TAG) ||
                (kind == XML_ELEMENT_TYPE_SELF_CLOSING) ||
                (kind == XML_ELEMENT_TYPE_END_TAG))
    {
                xml_element_scanNameSpan(e);
    }

    return 0;
}

static INLINE int xml_element_is_valid(const xmlelement *e) PC_NOEXCEPT_C
{
    if ((NULL == e) || (e->fElementKind == XML_ELEMENT_TYPE_INVALID))
        return 0;

    return 1;
}

static int xml_element_scanNameSpan(xmlelement *e) PC_NOEXCEPT_C
{
    if (NULL == e)
        return -1;
    
    bspan s;
    bspan_init_copy(&s, &e->fData);

    bool start = false;
    bool end = false;

    // If the chunk is empty, just return
    if (!bspan_is_valid(&s))
        return -1;

    // Check if the tag is end tag
    if (bspan_front(&s) == '/')
    {
        bspan_advance(&s,1);
        end = true;
    }
    else {
        start = true;
    }

    // Get tag name
    //bspan_init_copy(&e->fNameSpan, &s);
    //e->fNameSpan.fEnd = s.fStart;

    lex_skip_until_charset(&s, xmlwspchars(), &e->fNameSpan, &s);
    xml_name_reset(&e->fXmlName,&e->fNameSpan);

    // Modify the data chunk to point to the next attribute
    // part of the element
    bspan_init_copy(&e->fData, &s);

    return 0;
}





        // determines whether the element is currently empty
//        constexpr bool isEmpty() const { return fElementKind == XML_ELEMENT_TYPE_INVALID; }

//        explicit operator bool() const { return !isEmpty(); }

        // Returning information about the element
        //ByteSpan nameSpan() const { return fNameSpan; }
        //XmlName xmlName() const { return fXmlName; }
        //ByteSpan tagName() const { return fXmlName.name(); }
        //ByteSpan tagNamespace() const { return fXmlName.ns(); }
        //ByteSpan name() const { return fXmlName.name(); }

// Convenience for what kind of tag it is
static INLINE int xml_element_is_kind(const xmlelement* e, int kind) PC_NOEXCEPT_C { if (NULL == e) return 0;  return e->fElementKind == kind ? 1 : 0; }
static INLINE int xml_element_get_kind(const xmlelement *e) PC_NOEXCEPT_C { return e->fElementKind; }
static INLINE int xml_element_set_kind(xmlelement *e, int kind) PC_NOEXCEPT_C { e->fElementKind = kind;}

static INLINE int xml_element_get_data(const xmlelement *e, bspan *d) PC_NOEXCEPT_C {
	if (!xml_element_is_valid(e) || (NULL == d))
        return -1;

    bspan_init_copy(d, &e->fData);
}


		
static INLINE bool xml_element_isXmlDecl(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_XMLDECL); }
static INLINE bool xml_element_isStart(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_START_TAG); }
static INLINE bool xml_element_isSelfClosing(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_SELF_CLOSING); }
static INLINE bool xml_element_isEnd(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_END_TAG); }
static INLINE bool xml_element_isComment(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_COMMENT); }
static INLINE bool xml_element_isProcessingInstruction(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION); }
static INLINE bool xml_element_isContent(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_CONTENT); }
static INLINE bool xml_element_isCData(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_CDATA); }
static INLINE bool xml_element_isDoctype(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_DOCTYPE); }
static INLINE bool xml_element_isEntityDeclaration(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_ENTITY); }




#ifdef __cplusplus
}
#endif

#endif // XMLCORE_H_INCLUDED
