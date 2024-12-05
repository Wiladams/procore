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
	, XML_ELEMENT_TYPE_EMPTY_TAG                    // <br>
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

int xml_name_reset(xmlname *a, const bspan * inChunk) PC_NOEXCEPT_C;
int xml_name_weak_assign(xmlname *a, const xmlname *b) PC_NOEXCEPT_C;
int xml_name_name(const xmlname *a, bspan *name) PC_NOEXCEPT_C;
int xml_name_namespace(const xmlname *a, bspan *nspace) PC_NOEXCEPT_C;

struct xmlelement_t
{
    int fElementKind{ XML_ELEMENT_TYPE_INVALID };
    bspan fData;
    bspan fNameSpan;
    xmlname fXmlName;
};
typedef struct xmlelement_t xmlelement;

static int xml_element_init(xmlelement *e) PC_NOEXCEPT_C;
static int xml_element_init_from_element(xmlelement *a, const xmlelement *b) PC_NOEXCEPT_C;

static int xml_element_is_valid(const xmlelement *e) PC_NOEXCEPT_C;
static int xml_element_scanNameSpan(xmlelement *e) PC_NOEXCEPT_C;



//
// IMPLEMENTATION
//
static asciiset * xmlwspchars() PC_NOEXCEPT_C
{
    static asciiset chars;
    static bool initialized=false;

    if (!initialized){
        asciiset_init_from_cstr(&chars, " \r\n\t");
        initialized = true;
    }

    return &chars;
}

static asciiset * xmlcolonchar() PC_NOEXCEPT_C
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
static int xml_name_init(xmlname *a) PC_NOEXCEPT_C
{
    bspan_init(&a->fName);
    bspan_init(&a->fNamespace);
}

static int xml_name_reset(xmlname *a, const bspan * inChunk) PC_NOEXCEPT_C
{
    lex_front_token(inChunk, xmlwspchars(), xmlcolonchar(), &a->fNamespace, &a->fName);

    if (!bspan_is_valid(&a->fName))
    {
        bspan_weak_assign(&a->fName, &a->fNamespace);
        bspan_reset(&a->fNamespace);
    }

    return 0;
}

static int xml_name_weak_assign(xmlname *a, const xmlname *b) PC_NOEXCEPT_C
{
    bspan_weak_assign(&a->fName, &b->fName);
    bspan_weak_assign(&a->fNamespace, &b->fNamespace);

    return 0;
}

static int xml_name_name(const xmlname *a, bspan *name) PC_NOEXCEPT_C
{
    bspan_weak_assign(name, &a->fName);
    return 0;
}

static int xml_name_namespace(const xmlname *a, bspan *nspace) PC_NOEXCEPT_C
{
    bspan_weak_assign(nspace, &a->fNamespace);
    return 0;
}




// Representation of an xml element
// The xml scanner will generate these
static int xml_element_init(xmlelement *e) PC_NOEXCEPT_C
{
    e->fElementKind = XML_ELEMENT_TYPE_INVALID;
    bspan_init(&e->fData);
    bspan_init(&e->fNameSpan);
    xml_name_init(&e->fXmlName);
}

int xml_element_init_from_element(xmlelement *a, const xmlelement *b) PC_NOEXCEPT_C
{
    a->fElementKind = b->fElementKind;
    bspan_weak_assign(&a->fNameSpan, &b->fNameSpan);
    bspan_weak_assign(&a->fData, &b->fData);
    xml_name_weak_assign(&a->fXmlName, &b->fXmlName);
}

static int xml_element_is_valid(xmlelement *e) PC_NOEXCEPT_C
{
    if ((nullptr == e) || (e->fElementKind == XML_ELEMENT_TYPE_INVALID))
        return 0;

    return 1;
}

static int xml_element_scanNameSpan(xmlelement *e) PC_NOEXCEPT_C
{
    bspan s;
    bspan_weak_assign(&s, &e->fData);

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
    //bspan_weak_assign(&e->fNameSpan, &s);
    //e->fNameSpan.fEnd = s.fStart;

    lex_skip_until_charset(&s, xmlwspchars(), &e->fNameSpan, &s);
    xml_name_reset(&e->fXmlName,&e->fNameSpan);

    // Modify the data chunk to point to the next attribute
    // part of the element
    bspan_weak_assign(&e->fData, &s);

    return 0;
}



static int xml_element_init_from_data(xmlelement *e, int kind, const bspan * data) PC_NOEXCEPT_C
{
    xml_element_init(e);

    e->fElementKind = kind;
    bspan_weak_assign(&e->fData, data);


    if ((kind == XML_ELEMENT_TYPE_START_TAG) ||
                (kind == XML_ELEMENT_TYPE_SELF_CLOSING) ||
                (kind == XML_ELEMENT_TYPE_END_TAG))
    {
                xml_element_scanNameSpan(e);
    }
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
static bool xml_element_is_kind(const xmlelement *e, int kind) PC_NOEXCEPT_C {return e->fElementKind == kind;}
static int xml_element_get_kind(const xmlelement *e) PC_NOEXCEPT_C { return e->fElementKind; }
static int xml_element_set_kind(xmlelement *e, int kind) PC_NOEXCEPT_C { e->fElementKind = kind;}

static int xml_element_get_data(const xmlelement *e, bspan *d) PC_NOEXCEPT_C {
    if (!xml_element_is_valid(e))
        return -1;

    bspan_weak_assign(d, &e->fData);
}


		
bool xml_element_isXmlDecl(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_XMLDECL); }
bool xml_element_isStart(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_START_TAG); }
bool xml_element_isSelfClosing(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_SELF_CLOSING); }
bool xml_element_isEnd(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_END_TAG); }
bool xml_element_isComment(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_COMMENT); }
bool xml_element_isProcessingInstruction(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_PROCESSING_INSTRUCTION); }
bool xml_element_isContent(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_CONTENT); }
bool xml_element_isCData(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_CDATA); }
bool xml_element_isDoctype(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_DOCTYPE); }
bool xml_element_isEntityDeclaration(const xmlelement *e) PC_NOEXCEPT_C { return xml_element_is_kind(e, XML_ELEMENT_TYPE_ENTITY); }




#ifdef __cplusplus
}
#endif

#endif // XMLCORE_H_INCLUDED
