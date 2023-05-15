/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmxmlparser.h>
#include <drmutilitieslite.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**
** Function :   DRM_XMU_MatchNodeFromTemplate
**
** Synopsis :   Deep compare a XML node with a given template. It will be
**              considered a match ONLY if all subnodes in the template exist
**              AND exactly match the subnodes in the template. The node might
**              be a superset of the template
**
** Arguments :  f_pdstrXmlNodeList  -   List of XML nodes in which we have to
**                                      find a match
**              f_pdstrXmlTemplate  -   Template for finding match
**              f_fAttributeMatch   -   Match the first attribute (if any) of
**                                      each node
**              f_pfMatch           -   TRUE/FALSE indicating match or not
** Returns :
**
** Notes :      Example:
**              f_pdstrXmlTemplate  :
**                      <KID>xyz</KID>
**                      <META>
**                          <LGPUBKEY>abc</LGPUBKEY>
**                      </META>
**
**              f_pdstrXmlNodeList  :   No match (LGPUBKEY different)
**                      <KID>xyz</KID>
**                      <LID>pqr</LID>
**                      <META>
**                          <LGPUBKEY>sfsgdgddsg</LGPUBKEY>
**                      </META>
**
**              f_pdstrXmlNodeList  :   No match (KID missing)
**                      <META>
**                          <LGPUBKEY>abc</LGPUBKEY>
**                      </META>
**
**              f_pdstrXmlNodeList  :   Exact match
**                      <KID>xyz</KID>
**                      <LID>pqr</LID>
**                      <META>
**                          <LGPUBKEY>abc</LGPUBKEY>
**                      </META>
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMU_MatchNodeFromTemplate(
    __in  const   DRM_CONST_STRING    *f_pdstrXmlNodeList,
    __in  const   DRM_CONST_STRING    *f_pdstrXmlTemplate,
    __in          DRM_BOOL             f_fAttributeMatch,
    __out         DRM_BOOL            *f_pfMatch )
{
    DRM_RESULT                  dr                      =   DRM_SUCCESS;
    DRM_BOOL                    fIsLeafNode             =   FALSE;
    DRM_CONST_STRING            dstrNextTag             =   EMPTY_DRM_STRING;
    DRM_CONST_STRING            dstrNextNode            =   EMPTY_DRM_STRING;
    DRM_CONST_STRING            dstrNextNodeData        =   EMPTY_DRM_STRING;
    DRM_CONST_STRING            dstrMatchingNodeData    =   EMPTY_DRM_STRING;
    DRM_CONST_STRING            dstrXmlNodeList         =   EMPTY_DRM_STRING;
    DRM_CONST_STRING            dstrXmlTemplate         =   EMPTY_DRM_STRING;
    DRM_CONST_STRING            dstrFirstAttrName       =   EMPTY_DRM_STRING;
    DRM_CONST_STRING            dstrFirstAttrValue      =   EMPTY_DRM_STRING;
    DRM_CONST_STRING           *pdstrFirstAttrName      =   NULL;
    DRM_CONST_STRING           *pdstrFirstAttrValue     =   NULL;

    /*
    **  Check input
    */
    ChkArg(f_pfMatch);

    /*
    **  Clear output
    */
    *f_pfMatch = TRUE;

    if (f_fAttributeMatch)
    {
        pdstrFirstAttrName  = &dstrFirstAttrName;
        pdstrFirstAttrValue = &dstrFirstAttrValue;
    }

    ASSIGN_DRM_STRING(dstrXmlNodeList, *f_pdstrXmlNodeList);
    ASSIGN_DRM_STRING(dstrXmlTemplate, *f_pdstrXmlTemplate);

    /*
    **  Enumerate all nodes in the template
    **  Loop will break if there is a mismatch
    */
    while(dstrXmlTemplate.cchString > 0)
    {
        INIT_DRM_STRING(dstrNextTag);

        /*
        **  Get next node from template
        */
        ChkDR(  DRM_XML_EnumNextNode(
                    &dstrXmlTemplate,
                    &dstrNextTag,
                    &dstrNextNode,
                    &dstrNextNodeData,
                    pdstrFirstAttrName,
                    pdstrFirstAttrValue,
                    &fIsLeafNode)   );

        /*
        **  Find matching node in xml tree
        */
        dr  =   DRM_XML_GetNode(
                    &dstrXmlNodeList,
                    &dstrNextTag,
                    pdstrFirstAttrName,
                    pdstrFirstAttrValue,
                    0,
                    NULL,
                    &dstrMatchingNodeData);

        if (dr == DRM_E_XMLNOTFOUND
         || dr == DRM_E_INVALIDARG)
        {
            /*
            **  Corresponding node not found in xml tree - Mismatch
            */
            dr = DRM_SUCCESS;
            *f_pfMatch = FALSE;
            break;
        }
        ChkDR(dr);

        if (fIsLeafNode)
        {
            if (!DRM_UTL_DSTRStringsEqual(
                    &dstrNextNodeData,
                    &dstrMatchingNodeData) )
            {
                *f_pfMatch = FALSE;
                break;
            }
        }
        else
        {
            /*
            **  We do not expect this recursion to go beyond a few levels.
            **  Hence using recursion to keep things simple.
            */
            ChkDR(  DRM_XMU_MatchNodeFromTemplate(
                        &dstrMatchingNodeData,
                        &dstrNextNodeData,
                        f_fAttributeMatch,
                        f_pfMatch)  );
            if (!(*f_pfMatch))
            {
                break;
            }
        }

        /*
        **  Advance the template buffer
        */
        dstrXmlTemplate.pwszString  +=  dstrNextNode.cchString;
        dstrXmlTemplate.cchString   -=  dstrNextNode.cchString;

    }   /*  End of while(TRUE) loop */

ErrorExit:
    return dr;
}



EXIT_PK_NAMESPACE_CODE;
