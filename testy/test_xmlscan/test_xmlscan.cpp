#include <cstdio>
#include <cstdlib>

#include "lexutil.h"
#include "bspanprint.h"
#include "xmlscan.h"

void printXmlElement(xmlelement* e)
{
    if (e == nullptr)
        return;

    printf("XML Element\n");
    printf("   Kind: % d\n", e->fElementKind);
    printf("   Name: ");
    writeSpan(e->fXmlName.fName);
    printf("\n");
    printf("Content:");
    writeSpan(e->fData);
    printf("\n\n");

}

void test_scan_next()
{
    printf("==== test_scan_next() ====\n");
    bspan xmlsrc;
    bspan_init_from_cstr(&xmlsrc, "<svg att1='1' att2='2' >This is some XML</svg>");

    xmliterparams fParams{};
    xmliteratorstate fState{};
    xmlelement fCurrentElement{};

    xml_iter_params_init(&fParams);
    xml_iter_state_init_from_source(&fState, &xmlsrc);

    while (true){
        int ret = xml_iter_next_element(&fParams, &fState, &fCurrentElement);
        if (ret != 0)
            break;

        printXmlElement(&fCurrentElement);
    }
}

int main(int argc, char* argv[])
{

    test_scan_next();
}