/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXMLBUILDERGENERALUNICODE_C
#include <drmbase64.h>
#include <drmcrt.h>
#include <drmutilities.h>
#include <oemcipher.h>
#include <drmxmlbuilder.h>
#include <drmxmlbuilderconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define CB_XML_BUFFER_MINIMUM  100
#define CWCH_DWORD_STACK       (SIZEOF(DRM_DWORD)/SIZEOF(DRM_WCHAR))
#define CCH_SIMULATION_MININUM (2*CWCH_DWORD_STACK)

/*
***********************************************************************
** types used within this file
***********************************************************************
*/

const DRM_DISCARDABLE DRM_DWORD g_cbXMLBuilderMinimum = SIZEOF (_XMBContext)
                                      + __CB_DECL(CB_XML_BUFFER_MINIMUM)
                                      + SIZEOF (DRM_WCHAR);

/***************************************************************************** */

/******************************************************************************
** Function:   _PushDWORD
**
** Synopsis:   push a DWORD value on the end of the stack and decrement it
**
** Arguments:  f_pcontextXML - the XML context in use; must be inited
**             f_dwValue     - the value to push
******************************************************************************/
static DRM_RESULT _PushDWORD( __inout _XMBContext *f_pcontextXML,
                              __in    DRM_DWORD    f_dwValue)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  iPos = 0;

    ChkArg (f_pcontextXML != NULL);
    ChkBOOL(f_pcontextXML->wNextOpenNodePos >= CWCH_DWORD_STACK, DRM_E_BUFFERTOOSMALL);

    iPos = f_pcontextXML->wNextOpenNodePos;
    f_pcontextXML->XmlString [iPos+1] = (DRM_WCHAR) (f_dwValue  & 0x0000FFFF);
    f_pcontextXML->XmlString [iPos]   = (DRM_WCHAR) (f_dwValue >> 16);
    f_pcontextXML->wNextOpenNodePos  -= CWCH_DWORD_STACK;

ErrorExit:
    return dr;
}

/******************************************************************************
** Function:   _GetPushedDWORD
**
** Synopsis:   pop a DWORD value from the end of the stack
**
** Arguments:  f_pcontextXML - the XML context in use; must be inited
**             f_iOffset     - an offset from the current stack position
**             f_pdwValue    - pointer to variable to hold the retrieved value
******************************************************************************/
static DRM_RESULT _GetPushedDWORD(
    __in  const _XMBContext *f_pcontextXML,
    __in        DRM_DWORD    f_iOffset,
    __out       DRM_DWORD   *f_pdwValue )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(f_pcontextXML != NULL
        && f_pdwValue    != NULL);

    ChkArg((f_pcontextXML->wNextOpenNodePos + f_iOffset) >= CWCH_DWORD_STACK);

    *f_pdwValue  =  (DRM_DWORD)  (f_pcontextXML->XmlString [f_pcontextXML->wNextOpenNodePos + f_iOffset + 1]);
    *f_pdwValue += ((DRM_DWORD) ((f_pcontextXML->XmlString [f_pcontextXML->wNextOpenNodePos + f_iOffset])) << 16);

ErrorExit:
    return dr;
}

/*
** Trim all leading and trailing blanks in given string
** return TRUE if resulting string length > 0
*/
static DRM_BOOL _AllTrim2(
    __in const DRM_CONST_STRING *pdstrString,
    __out      DRM_CONST_STRING *pdstrTrimedString)
{
    DRM_BOOL fResult = FALSE;

    if ( pdstrString             == NULL
      || pdstrTrimedString       == NULL
      || pdstrString->pwszString == NULL
      || pdstrString->cchString  == 0 )
    {
        goto ErrorExit;
    }

    *pdstrTrimedString = *pdstrString;

    /* trim leading blanks */
    while ( pdstrTrimedString->pwszString[0] == g_wchSpace  && pdstrTrimedString->cchString>0 )
    {
        ++pdstrTrimedString->pwszString;
        --pdstrTrimedString->cchString;
    }

    /* trim trailing blanks */
    while ( pdstrTrimedString->cchString>0
        && pdstrTrimedString->pwszString[pdstrTrimedString->cchString-1] == g_wchSpace )
    {
        --pdstrTrimedString->cchString;
    }

    fResult = pdstrTrimedString->cchString > 0;

ErrorExit:
    return fResult;
}

/*
** Create context for the builder
*/
static DRM_RESULT _CreateContext(
    __in     DRM_DWORD    cbXmlContext,
    __inout _XMBContext *pbXmlContext,
    __in     DRM_BOOL     fIsSimMode)
{
    DRM_RESULT   dr = DRM_SUCCESS;

    if (cbXmlContext < g_cbXMLBuilderMinimum)
    {
        ChkDR (DRM_E_BUFFERTOOSMALL);
    }

    ZEROMEM(pbXmlContext, cbXmlContext);
    pbXmlContext->wSize            = cbXmlContext;
    pbXmlContext->wBuffSize        = ((cbXmlContext - SIZEOF(_XMBContext)) / SIZEOF(DRM_WCHAR));
    pbXmlContext->wNextStringPos   = 0;
    pbXmlContext->wNextOpenNodePos = pbXmlContext->wBuffSize - CWCH_DWORD_STACK; /* next position to be used on stack */
    pbXmlContext->fInited          = TRUE;
    pbXmlContext->fIsSimMode       = fIsSimMode;
    pbXmlContext->wMaxStackUsed    = 0;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function: _SaveContextState
**
** Synopsis: Saves context state to allow rollback of failed operation
**
** Arguments:   pbXmlContext      - pointer to _XMBContext which state should be saved
**              pbXmlContextState - pointer to _XMBContext where state will be saved
**
** Returns:     DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_RESULT _SaveContextState(
    __in    const _XMBContext *pbXmlContext,
    __inout       _XMBContext *pbXmlContextState )
{
    DRM_RESULT   dr = DRM_SUCCESS;

    ChkArg( pbXmlContext != NULL );
    ChkArg( pbXmlContextState != NULL );

    if ( pbXmlContext->fInited )
    {
        pbXmlContextState->wMaxStackUsed    = pbXmlContext->wMaxStackUsed;
        pbXmlContextState->wSize            = pbXmlContext->wSize;
        pbXmlContextState->wNextStringPos   = pbXmlContext->wNextStringPos;
        pbXmlContextState->wNextOpenNodePos = pbXmlContext->wNextOpenNodePos;
    }

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function: _RestoreContextState
**
** Synopsis: Restores context state from saved context
**
** Arguments:   pbXmlContext      - pointer to _XMBContext which state should be restored
**              pbXmlContextState - pointer to _XMBContext from where state will be restored
**
** Returns:     DRM_SUCCESS on success.
**
**********************************************************************/

static DRM_RESULT _RestoreContextState(
    __inout       _XMBContext *pbXmlContext,
    __in    const _XMBContext *pbXmlContextState )
{
    DRM_RESULT   dr = DRM_SUCCESS;

    ChkArg( pbXmlContext != NULL );
    ChkArg( pbXmlContextState != NULL );

    if ( pbXmlContext->fInited )
    {
        pbXmlContext->wMaxStackUsed    = pbXmlContextState->wMaxStackUsed;
        pbXmlContext->wSize            = pbXmlContextState->wSize;
        pbXmlContext->wNextStringPos   = pbXmlContextState->wNextStringPos;
        pbXmlContext->wNextOpenNodePos = pbXmlContextState->wNextOpenNodePos;
    }

ErrorExit:
    return dr;
}

/*
** Open a new node nested from current opened node
*/
static DRM_RESULT _OpenNode(
    __inout       _XMBContext      *pContext,
    __in    const DRM_CONST_STRING *pdstrNodeName )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_DWORD        wNodePos     = 0;
    DRM_CONST_STRING dstrNodeName = EMPTY_DRM_STRING;

    ChkArg(_AllTrim2(pdstrNodeName, &dstrNodeName));

    if ( pContext->fIsSimMode )
    {
        /* fake mode pushes 2 DWORDs, one for node position, one for tag name size */

        if (pContext->wNextOpenNodePos < CCH_SIMULATION_MININUM)
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }
        wNodePos = pContext->wNextStringPos + 1;

        /* calculate length of the open tag now  2 = "<>" */

        pContext->wNextStringPos += dstrNodeName.cchString + 2;

        /* push the node position to end of XML string */

        ChkDR(_PushDWORD(pContext, wNodePos));

        /* push the tag name length to end of XML string */

        ChkDR(_PushDWORD(pContext, dstrNodeName.cchString));

        /* keep track of max stack used */
        {

            DRM_DWORD wCurrStack = (pContext->wBuffSize - 1 - pContext->wNextOpenNodePos) / 2;

            if ( pContext->wMaxStackUsed < wCurrStack )
            {
                pContext->wMaxStackUsed = wCurrStack;
            }
        }

    }
    else
    {
        if ( pContext->wNextStringPos + dstrNodeName.cchString                     < pContext->wNextStringPos
          || pContext->wNextStringPos + dstrNodeName.cchString + CWCH_DWORD_STACK  < pContext->wNextStringPos
          || pContext->wNextStringPos + dstrNodeName.cchString + CWCH_DWORD_STACK >= pContext->wNextOpenNodePos )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        /* print "<tag>" to the XML string*/
        ChkOverflow(pContext->wBuffSize, pContext->wBuffSize - pContext->wNextStringPos);
        ChkDR( DRM_STR_StringCchCopyNW(&pContext->XmlString[pContext->wNextStringPos],
                                   pContext->wBuffSize - pContext->wNextStringPos,
                                   g_dstrOpenTag.pwszString,
                                   g_dstrOpenTag.cchString) );
        pContext->wNextStringPos += 1;
        wNodePos = pContext->wNextStringPos;

        ChkDR( DRM_STR_StringCchCopyNW(&pContext->XmlString[pContext->wNextStringPos],
                                   pContext->wBuffSize - pContext->wNextStringPos,
                                   dstrNodeName.pwszString,
                                   dstrNodeName.cchString) );
        pContext->wNextStringPos += dstrNodeName.cchString;

        ChkDR( DRM_STR_StringCchCopyNW(&pContext->XmlString[pContext->wNextStringPos],
                                   pContext->wBuffSize - pContext->wNextStringPos,
                                   g_dstrCloseTag.pwszString,
                                   g_dstrCloseTag.cchString) );
        pContext->wNextStringPos += 1;

        /* push the node position to end of XML string */

        ChkDR(_PushDWORD(pContext, wNodePos));
    }

ErrorExit:
    return dr;
}


/*
** Close the current opened node
*/
static DRM_RESULT DRM_CALL _CloseCurrNode(
    __inout   _XMBContext *pXmlContext,
    __in      DRM_BOOL     fOkayToCloseRoot,
    __out_opt DRM_STRING  *pdstrXMLString)   /* optional parameter */
{
    DRM_RESULT   dr          = DRM_SUCCESS;
    DRM_DWORD    wLen        = 0;
    DRM_DWORD    wNodePos    = 0;
    DRM_DWORD    cwchMinStack = 0;

    ChkArg(pXmlContext->fInited);

    /*
    ** make sure the stack is not empty. Stack is count by CWCH_DWORD_STACK.
    ** to close a node, we must have at least ONE CWCH_DWORD_STACK on stack.
    ** if fOkayToCloseRoot is FALSE, we must have at least two CWCH_DWORD_STACK
    ** on stack.
    */
    cwchMinStack = 2 * CWCH_DWORD_STACK;  /* do not pop off the root node */
    if (fOkayToCloseRoot)
    {
        cwchMinStack = CWCH_DWORD_STACK;  /* root node */
    }

    if ( (pXmlContext->wNextOpenNodePos + cwchMinStack) >= pXmlContext->wBuffSize )    /* do not pop off the root node !*/
    {
        dr = DRM_E_NOMORE;    /* stack is empty */
        goto ErrorExit;
    }

    if ( !pXmlContext->fIsSimMode )
    {
        /* parse length of node name */

        ChkDR(_GetPushedDWORD(pXmlContext, CWCH_DWORD_STACK, &wNodePos));

        if ( pdstrXMLString != NULL )
        {
            pdstrXMLString->pwszString = &(pXmlContext->XmlString[wNodePos - 1]);
        }

        while (pXmlContext->XmlString [wNodePos + wLen] != g_wchSpace
            && pXmlContext->XmlString [wNodePos + wLen] != g_wchGreaterThan )
        {
            wLen++;
        }

        if ( pXmlContext->wNextStringPos + wLen      < pXmlContext->wNextStringPos
          || pXmlContext->wNextStringPos + wLen + 3  < pXmlContext->wNextStringPos
          || pXmlContext->wNextStringPos + wLen + 3 >= pXmlContext->wNextOpenNodePos )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        /* print </tag> to the XML string*/
        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[pXmlContext->wNextStringPos],
                                   pXmlContext->wBuffSize - pXmlContext->wNextStringPos,
                                   g_dstrOpenEndTag.pwszString,
                                   g_dstrOpenEndTag.cchString) );
        pXmlContext->wNextStringPos += 2;

        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[pXmlContext->wNextStringPos],
                                   pXmlContext->wBuffSize - pXmlContext->wNextStringPos,
                                  &pXmlContext->XmlString[wNodePos],
                                   wLen) );
        pXmlContext->wNextStringPos += wLen;

        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[pXmlContext->wNextStringPos],
                                   pXmlContext->wBuffSize - pXmlContext->wNextStringPos,
                                   g_dstrCloseTag.pwszString,
                                   g_dstrCloseTag.cchString) );
        pXmlContext->wNextStringPos += 1;

        /* pop the node position from end of XML string */
        pXmlContext->wNextOpenNodePos += CWCH_DWORD_STACK;
    }
    else
    {
        DRM_DWORD cchPop = 0;

        ChkDR(_GetPushedDWORD(pXmlContext, CWCH_DWORD_STACK, &cchPop));
        ChkDR(_GetPushedDWORD(pXmlContext, 2 * CWCH_DWORD_STACK, &wNodePos));

        /* calculate length of the close tag now 3 = "</>" */

        pXmlContext->wNextStringPos += cchPop + 3;

        /* pop the node position and tag name size from end of XML string */

        pXmlContext->wNextOpenNodePos += 2 * CWCH_DWORD_STACK;
    }

    if ( pdstrXMLString != NULL )
    {
        pdstrXMLString->cchString = pXmlContext->wNextStringPos - wNodePos + 1;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMB_SimulateCreateDocument(
    __in    const DRM_DWORD         cbXmlContext,
    __inout       _XMBContext      *pbXmlContext,
    __in    const DRM_CONST_STRING *pdstrRootNodeName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    /* create the context */
    ChkArg(pbXmlContext != NULL
        && cbXmlContext != 0    );

    ChkDRMString(pdstrRootNodeName);

    ChkDR(_CreateContext(cbXmlContext, pbXmlContext, TRUE));

    /* open the root node */
    ChkDR( _OpenNode(pbXmlContext, pdstrRootNodeName) );

ErrorExit:
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_CreateDocument(
    __in    const DRM_DWORD         cbXmlContext,
    __inout       _XMBContext      *pbXmlContext,
    __in    const DRM_CONST_STRING *pdstrRootNodeName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CreateDocument );
    /* create the context */
    ChkArg(pbXmlContext != NULL
        && cbXmlContext != 0    );

    ChkArg((((DRM_DWORD_PTR) pbXmlContext) % sizeof (DRM_DWORD)) == 0);

    ChkDRMString(pdstrRootNodeName);

    ChkDR(_CreateContext(cbXmlContext, pbXmlContext, FALSE));

    /* open the root node */
    ChkDR( _OpenNode(pbXmlContext, pdstrRootNodeName) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMB_ReallocDocument(
    __in          _XMBContext *pbOldXmlContext,
    __in    const DRM_DWORD    cbNewXmlContext,
    __inout       _XMBContext *pbNewXmlContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ReallocDocument );

    ChkArg( pbOldXmlContext != NULL
         && cbNewXmlContext != 0
         && pbNewXmlContext != NULL
         && pbOldXmlContext->fInited );

    if ( cbNewXmlContext <= pbOldXmlContext->wSize )
    {
        ChkDR(DRM_E_BUFFERTOOSMALL);
    }

    /* create new context using new buffer */
    if ( pbOldXmlContext != pbNewXmlContext )
    {
        ChkDR(_CreateContext(cbNewXmlContext, pbNewXmlContext, pbOldXmlContext->fIsSimMode));

        /* copy XML String to new context */
        MEMCPY(pbNewXmlContext->XmlString, pbOldXmlContext->XmlString, pbOldXmlContext->wNextStringPos * SIZEOF(DRM_WCHAR));
        pbNewXmlContext->wNextStringPos = pbOldXmlContext->wNextStringPos;

        /* copy node positions info to new context */
        pbNewXmlContext->wNextOpenNodePos = pbNewXmlContext->wBuffSize
                                          - pbOldXmlContext->wBuffSize
                                          + pbOldXmlContext->wNextOpenNodePos;

        MEMMOVE(&pbNewXmlContext->XmlString[pbNewXmlContext->wNextOpenNodePos],
                &pbOldXmlContext->XmlString[pbOldXmlContext->wNextOpenNodePos],
                (pbOldXmlContext->wBuffSize - pbOldXmlContext->wNextOpenNodePos) * SIZEOF(DRM_WCHAR));

        /* invalidate the old context */
        pbOldXmlContext->fInited = FALSE;
    }

    // the new buffer is the same as the old buffer except bigger.
    else
    {
        DRM_DWORD dwOldSize = pbOldXmlContext->wBuffSize;
        DRM_DWORD dwOldNextOpenNodePos = pbOldXmlContext->wNextOpenNodePos;

        pbNewXmlContext->wSize     = cbNewXmlContext;
        pbNewXmlContext->wBuffSize = ((cbNewXmlContext - SIZEOF(_XMBContext)) / SIZEOF(DRM_WCHAR));

        /* copy node positions info to new context */
        pbNewXmlContext->wNextOpenNodePos = pbNewXmlContext->wBuffSize - dwOldSize + dwOldNextOpenNodePos;

        MEMMOVE(&pbNewXmlContext->XmlString[pbNewXmlContext->wNextOpenNodePos],
                &pbOldXmlContext->XmlString[dwOldNextOpenNodePos],
                (dwOldSize - dwOldNextOpenNodePos) * SIZEOF(DRM_WCHAR));
    }
ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_CloseDocument(
    __inout _XMBContext *pbXmlContext,
    __out   DRM_STRING  *pdstrXML)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CloseDocument );

    ChkArg( pbXmlContext != NULL
         && pdstrXML     != NULL
         && pbXmlContext->fInited );

    while (TRUE)
    {
        dr = _CloseCurrNode(pbXmlContext, TRUE, NULL);
        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            break;
        }
        else
        {
            ChkDR(dr);
        }
    }

    if ( pbXmlContext->fIsSimMode )
    {
        DRM_DWORD cbOverhead = DRM_XMB_OVERHEAD;

        pdstrXML->pwszString = NULL;
        pdstrXML->cchString  = pbXmlContext->wNextStringPos;

        /* add extra room for simulate mode so that the call can allow enough
        ** buffer size for creating this document */
        if ( cbOverhead < (pbXmlContext->wMaxStackUsed * SIZEOF(DRM_WCHAR) + SIZEOF(_XMBContext)) )
        {
            cbOverhead = pbXmlContext->wMaxStackUsed * SIZEOF(DRM_WCHAR) + SIZEOF(_XMBContext);
        }
        pdstrXML->cchString += (cbOverhead + 1) / SIZEOF(DRM_WCHAR);
    }
    else
    {
        pdstrXML->pwszString = pbXmlContext->XmlString;
        pdstrXML->cchString  = pbXmlContext->wNextStringPos;
    }


    /* invalidate the context */
    pbXmlContext->fInited = FALSE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_OpenNode(
    __inout       _XMBContext      *pbXmlContext,
    __in    const DRM_CONST_STRING *pdstrNodeName)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_OpenNode );

    ChkArg( pbXmlContext != NULL
         && pbXmlContext->fInited    );
    ChkDRMString(pdstrNodeName);

    /* open the root node */
    ChkDR( _OpenNode(pbXmlContext, pdstrNodeName) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_CloseCurrNode(
    __inout   _XMBContext *pbXmlContext,
    __out_opt DRM_STRING  *pdstrXMLFragment)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_CloseCurrNode );

    ChkArg( pbXmlContext != NULL );
    ChkDR(_CloseCurrNode(pbXmlContext, FALSE, pdstrXMLFragment));

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_SaveState(
    __inout const _XMBContext *pbXmlContext,
    __out         XMBState    *pState )
{
    DRM_RESULT   dr = DRM_SUCCESS;

    ChkArg( pbXmlContext != NULL  &&  pState != NULL );

    pState->cwchMaxStackUsed    = pbXmlContext->wMaxStackUsed;
    pState->iwchNextOpenNodePos = pbXmlContext->wNextOpenNodePos;
    pState->iwchNextStringPos   = pbXmlContext->wNextStringPos;

ErrorExit:

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_RestoreState(
    __inout       _XMBContext *pbXmlContext,
    __in    const XMBState    *pState )
{
    DRM_RESULT   dr = DRM_SUCCESS;

    ChkArg( pbXmlContext != NULL  &&  pState != NULL );

    pbXmlContext->wMaxStackUsed    = pState->cwchMaxStackUsed;
    pbXmlContext->wNextOpenNodePos = pState->iwchNextOpenNodePos;
    pbXmlContext->wNextStringPos   = pState->iwchNextStringPos;

ErrorExit:

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMB_GetCurrNodeName(
    __in  _XMBContext *pbXmlContext,
    __out DRM_STRING  *pdstrNodeName)
{
    DRM_RESULT   dr       = DRM_SUCCESS;
    DRM_DWORD    wLen     = 0;
    DRM_DWORD    wNodePos = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_GetCurrNodeName );

    ChkArg( pbXmlContext  != NULL
         && pdstrNodeName != NULL
         && pbXmlContext->fInited);

    if ( pbXmlContext->fIsSimMode )
    {
        ChkDR(DRM_E_LOGICERR);  /* this function does not support fake mode */
    }

    /* parse length of node name */

    ChkDR(_GetPushedDWORD(pbXmlContext, CWCH_DWORD_STACK, &wNodePos));

    while (pbXmlContext->XmlString [wNodePos + wLen] != g_wchSpace
        && pbXmlContext->XmlString [wNodePos + wLen] != g_wchGreaterThan)
    {
        wLen++;
    }

    pdstrNodeName->pwszString = &pbXmlContext->XmlString[wNodePos];
    pdstrNodeName->cchString  = wLen;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_API DRM_RESULT DRM_CALL DRM_XMB_GetContextSize(
    __in  const _XMBContext *pbXmlContext,
    __out       DRM_DWORD   *pcbXmlContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( pbXmlContext  != NULL
         && pcbXmlContext != NULL
         && pbXmlContext->fInited );

    *pcbXmlContext = pbXmlContext->wSize;

ErrorExit:
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddAttribute(
    __inout       _XMBContext      *pXmlContext,
    __in    const DRM_CONST_STRING *pdstrAttrName,
    __in    const DRM_CONST_STRING *pdstrAttrValue )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD    wReqSize = 0;
    DRM_DWORD    wOffset  = 0;
    DRM_DWORD    wDst     = 0;
    DRM_DWORD    wSrc     = 0;
    DRM_DWORD    i        = 0;
    DRM_CONST_STRING dstrAttrName  = EMPTY_DRM_STRING,
                     dstrAttrValue = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddAttribute );

    ChkArg( pXmlContext != NULL
         && pXmlContext->fInited);

    ChkDRMString(pdstrAttrName);
    ChkDRMString(pdstrAttrValue);

    if ( (pXmlContext->wNextOpenNodePos + ( pXmlContext->fIsSimMode? 2 : 1 )) == pXmlContext->wBuffSize )
    {
        ChkDR(DRM_E_NOMORE);    /* stack is empty */
    }

    /* make sure it's not blank */
    ChkArg(_AllTrim2(pdstrAttrName, &dstrAttrName));

    /* trim leading and trialing blanks, but blank attrib value is allowed */
    _AllTrim2(pdstrAttrValue, &dstrAttrValue);

    /* calculate the buffer size needed to insert the ' attr="value"' 4 = space + equal sign + "" */
    wReqSize = dstrAttrName.cchString;

    ChkOverflow( wReqSize + dstrAttrValue.cchString , wReqSize );
    wReqSize += dstrAttrValue.cchString;

    ChkOverflow( wReqSize + g_dstrEqualQuote.cchString , wReqSize );
    wReqSize += g_dstrEqualQuote.cchString;

    ChkOverflow( wReqSize + g_dstrSpace.cchString , wReqSize );
    wReqSize += g_dstrSpace.cchString;

    ChkOverflow( wReqSize + g_dstrQuote.cchString , wReqSize );
    wReqSize += g_dstrQuote.cchString;

    if ( !pXmlContext->fIsSimMode )
    {
        if ( (pXmlContext->wNextStringPos + wReqSize)  < pXmlContext->wNextStringPos
          || (pXmlContext->wNextStringPos + wReqSize) >= pXmlContext->wNextOpenNodePos )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        /* at this point, we've got the required buffer length,
        ** now parse for '>' start from the current node name */
        ChkDR(_GetPushedDWORD(pXmlContext, CWCH_DWORD_STACK, &wOffset));

        while ( wOffset < pXmlContext->wBuffSize
           &&   pXmlContext->XmlString[wOffset] != g_wchGreaterThan )
        {
            wOffset++;
        }

        if ( wOffset >= pXmlContext->wBuffSize
        ||   pXmlContext->XmlString[wOffset] != g_wchGreaterThan )
        {
            ChkDR(DRM_E_NO_XML_CLOSE_TAG);
        }

        /* now, XmlString[wOffset] contains the '>' char, insert attr=value in there
        ** first, move everything, if any, from '>' to give space for the insertion
        */
        wDst = pXmlContext->wNextStringPos+wReqSize-1;
        wSrc = pXmlContext->wNextStringPos-1;
        for (i=0; i<(pXmlContext->wNextStringPos - wOffset); i++)
        {
            pXmlContext->XmlString[wDst--] = pXmlContext->XmlString[wSrc--];
        }

        if( wOffset + wReqSize  < wOffset
         || wOffset + wReqSize >= pXmlContext->wBuffSize )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        /* now, insert it in there */
        ChkOverflow( pXmlContext->wBuffSize , pXmlContext->wBuffSize - wOffset );
        ChkDR( DRM_STR_StringCchCopyNW( &pXmlContext->XmlString[wOffset],
                                    pXmlContext->wBuffSize - wOffset,
                                    g_dstrSpace.pwszString,
                                    g_dstrSpace.cchString ) );
        wOffset += g_dstrSpace.cchString;

        ChkOverflow( pXmlContext->wBuffSize , pXmlContext->wBuffSize - wOffset );
        ChkDR( DRM_STR_StringCchCopyNW( &pXmlContext->XmlString[wOffset],
                                    pXmlContext->wBuffSize - wOffset,
                                    dstrAttrName.pwszString,
                                    dstrAttrName.cchString) );
        wOffset += dstrAttrName.cchString;

        ChkOverflow( pXmlContext->wBuffSize , pXmlContext->wBuffSize - wOffset );
        ChkDR( DRM_STR_StringCchCopyNW( &pXmlContext->XmlString[wOffset],
                                    pXmlContext->wBuffSize - wOffset,
                                    g_dstrEqualQuote.pwszString,
                                    g_dstrEqualQuote.cchString ) );
        wOffset += g_dstrEqualQuote.cchString;

        ChkOverflow( pXmlContext->wBuffSize , pXmlContext->wBuffSize - wOffset );
        ChkDR( DRM_STR_StringCchCopyNW( &pXmlContext->XmlString[wOffset],
                                    pXmlContext->wBuffSize - wOffset,
                                    dstrAttrValue.pwszString,
                                    dstrAttrValue.cchString) );
        wOffset += dstrAttrValue.cchString;

        ChkOverflow( pXmlContext->wBuffSize , pXmlContext->wBuffSize - wOffset );
        ChkDR( DRM_STR_StringCchCopyNW( &pXmlContext->XmlString[wOffset],
                                    pXmlContext->wBuffSize - wOffset,
                                    g_dstrQuote.pwszString,
                                    g_dstrQuote.cchString) );
        wOffset += g_dstrQuote.cchString;
        pXmlContext->XmlString[wOffset] = g_wchGreaterThan;

    }

    /* update next XML string position */
    pXmlContext->wNextStringPos += wReqSize;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddData(
    __inout       _XMBContext      *pXmlContext,
    __in    const DRM_CONST_STRING *pdstrData )
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    DRM_DWORD        wOffset  = 0;
    DRM_DWORD        wDst     = 0;
    DRM_DWORD        wSrc     = 0;
    DRM_DWORD        i        = 0;
    DRM_CONST_STRING dstrData = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddData );

    ChkArg( pXmlContext != NULL
         && pXmlContext->fInited    );
    ChkDRMString(pdstrData);

    if ( (pXmlContext->wNextOpenNodePos + ( pXmlContext->fIsSimMode? 2 : 1 )) == pXmlContext->wBuffSize )
    {
        ChkDR(DRM_E_NOMORE);    /* stack is empty */
    }

    if ( !_AllTrim2(pdstrData, &dstrData) )     /* a blank string found */
    {
        goto ErrorExit;
    }

    if ( !pXmlContext->fIsSimMode )
    {
        /* check if we have enough space */
        if ( (pXmlContext->wNextStringPos + dstrData.cchString)  < pXmlContext->wNextStringPos
          || (pXmlContext->wNextStringPos + dstrData.cchString) >= pXmlContext->wNextOpenNodePos)
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        /* at this point, we've got the required buffer length,
        ** now parse for '>' start from the current node name */

        ChkDR(_GetPushedDWORD(pXmlContext, CWCH_DWORD_STACK, &wOffset));

        while ( wOffset < pXmlContext->wBuffSize
           &&   pXmlContext->XmlString[wOffset] != g_wchGreaterThan )
        {
            wOffset++;
        }

        if ( wOffset >= pXmlContext->wBuffSize
        ||   pXmlContext->XmlString[wOffset] != g_wchGreaterThan )
        {
            ChkDR(DRM_E_NO_XML_CLOSE_TAG);
        }

        /* now, XmlString[wOffset] contains the '>' char, insert data after it.
        ** first, move everything, if any, after '>' to give space for the insertion
        */
        wOffset += 1;

        wDst = pXmlContext->wNextStringPos + dstrData.cchString - 1;
        wSrc = pXmlContext->wNextStringPos-1;

        for (i=0; i<(pXmlContext->wNextStringPos - wOffset); i++)
        {
            pXmlContext->XmlString[wDst--] = pXmlContext->XmlString[wSrc--];
        }

        /* now, insert it in there */
        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[wOffset],
                                   pXmlContext->wBuffSize - wOffset,
                                   dstrData.pwszString,
                                   dstrData.cchString) );
        wOffset += dstrData.cchString;
        pXmlContext->XmlString[wOffset] = g_wchLessThan;

    }

    /* update next XML string position */
    pXmlContext->wNextStringPos += dstrData.cchString;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function:   DRM_XMB_ReserveSpace
**
** Synopsis:   API that reserves a specific number of characters in the XML builder
**             context.
**
** Arguments:
**
** [f_poXMBContext]         -- Pointer to an initialized XML builder context.
** [f_cchData]              -- Number of wide characters to reserve in the XML builder
**                             context.
** [f_ppwchData]            -- Pointer to pointer that receives the address of
**                             the internal XML builder context buffer that has been
**                             reserved.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the size of buffer in the XML
**                          builder context is not big enough to accomodate the data
**                          to be added.
**
** Note:                    The purpose of this API is to reserve a buffer of specific
**                          size in the XML builder context first and then fill the
**                          buffer in other functions (such as file read). Without this
**                          API, other functions have to store the data to be added to the
**                          XML builder context in a temporary buffer, and then call
**                          DRM_XMB_AddData() to add the data to the XML builder context.
**                          If the data to be added has a large size, the memory usage will
**                          be doubled compared with using this API.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_ReserveSpace(
    __inout                       _XMBContext *f_poXMBContext,
    __in                          DRM_DWORD    f_cchData,
    __deref_out_ecount(f_cchData) DRM_WCHAR  **f_ppwchData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_ReserveSpace );

    ChkArg( f_poXMBContext != NULL &&
            f_poXMBContext->fInited );

    ChkArg( f_cchData > 0 );

    ChkArg( f_ppwchData != NULL );

    if ( !f_poXMBContext->fIsSimMode )
    {
        /* Check whether there is enough space. */
        if ( ( f_poXMBContext->wNextStringPos + f_cchData ) < f_poXMBContext->wNextStringPos ||
             ( f_poXMBContext->wNextStringPos + f_cchData ) >= f_poXMBContext->wNextOpenNodePos )
        {
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        *f_ppwchData = &f_poXMBContext->XmlString[ f_poXMBContext->wNextStringPos ];
    }

    /* Update the output buffer pointer. */
    f_poXMBContext->wNextStringPos += f_cchData;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddCData(
    __inout       _XMBContext      *pXmlContext,
    __in    const DRM_CONST_STRING *pdstrCData )
{
    DRM_RESULT       dr        = DRM_SUCCESS;
    DRM_DWORD        wReqSize  = 0;
    DRM_DWORD        wOffset   = 0;
    DRM_DWORD        wDst      = 0;
    DRM_DWORD        wSrc      = 0;
    DRM_DWORD        i         = 0;
    DRM_CONST_STRING dstrCData = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddCData );

    ChkArg( pXmlContext != NULL
         && pXmlContext->fInited    );
    ChkDRMString(pdstrCData);

    if ( (pXmlContext->wNextOpenNodePos + ( pXmlContext->fIsSimMode? 2 : 1 )) < pXmlContext->wNextOpenNodePos
       || (pXmlContext->wNextOpenNodePos + ( pXmlContext->fIsSimMode? 2 : 1 )) >= pXmlContext->wBuffSize )
    {
        ChkDR(DRM_E_NOMORE);    /* stack is empty */
    }

    /* trim leading and trialing blanks, but blank CDATA value is allowed */
    _AllTrim2(pdstrCData, &dstrCData);

    /* calculate the buffer size needed to insert <!<CDATA[xxx]]> */
    wReqSize = dstrCData.cchString;

    ChkOverflow( wReqSize + g_dstrOpenCDATATag.cchString , wReqSize );
    wReqSize += g_dstrOpenCDATATag.cchString;

    ChkOverflow( wReqSize + g_dstrCloseCDATATag.cchString , wReqSize );
    wReqSize += g_dstrCloseCDATATag.cchString;

    if ( !pXmlContext->fIsSimMode )
    {
        if ( pXmlContext->wNextStringPos + wReqSize  < pXmlContext->wNextStringPos
         ||  pXmlContext->wNextStringPos + wReqSize >= pXmlContext->wNextOpenNodePos
         ||  pXmlContext->wNextStringPos + wReqSize >= pXmlContext->wBuffSize )
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        /* at this point, we've got the required buffer length,
        ** now parse for '>' start from the current node name */
        ChkDR(_GetPushedDWORD(pXmlContext, CWCH_DWORD_STACK, &wOffset));

        while ( wOffset < pXmlContext->wBuffSize
           &&   pXmlContext->XmlString[wOffset] != g_wchGreaterThan )
        {
            wOffset++;
        }

        if ( wOffset >= pXmlContext->wBuffSize
        ||   pXmlContext->XmlString[wOffset] != g_wchGreaterThan )
        {
            ChkDR(DRM_E_NO_XML_CLOSE_TAG);
        }

        /* now, XmlString[wOffset] contains the '>' char, insert CDATA after it
        ** first, move everything, if any, after '>' to give space for the insertion
        */
        wOffset += 1;
        wDst = pXmlContext->wNextStringPos+wReqSize-1;
        wSrc = pXmlContext->wNextStringPos-1;

        for (i = 0; i < (pXmlContext->wNextStringPos - wOffset); i++)
        {
            pXmlContext->XmlString[wDst--] = pXmlContext->XmlString[wSrc--];
        }

        /* now, insert it in there */
        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[wOffset],
                                   pXmlContext->wBuffSize - wOffset,
                                   g_dstrOpenCDATATag.pwszString,
                                   g_dstrOpenCDATATag.cchString) );
        wOffset += g_dstrOpenCDATATag.cchString;

        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[wOffset],
                                   pXmlContext->wBuffSize - wOffset,
                                   dstrCData.pwszString,
                                   dstrCData.cchString) );
        wOffset += dstrCData.cchString;

        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString[wOffset],
                                   pXmlContext->wBuffSize - wOffset,
                                   g_dstrCloseCDATATag.pwszString,
                                   g_dstrCloseCDATATag.cchString) );

    }

    /* update next XML string position */
    pXmlContext->wNextStringPos += wReqSize;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_API DRM_RESULT DRM_CALL DRM_XMB_AppendNode(
    __inout       _XMBContext      *pXmlContext,
    __in    const DRM_CONST_STRING *pdstrXmlString)
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    DRM_CONST_STRING dstrNode = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AppendNode );

    ChkArg( pXmlContext != NULL
         && pXmlContext->fInited    );
    ChkDRMString( pdstrXmlString );

    /* open the root node */
    ChkArg(_AllTrim2(pdstrXmlString, &dstrNode));

    if (! pXmlContext->fIsSimMode)
    {
        if ( pXmlContext->wNextStringPos + dstrNode.cchString  < pXmlContext->wNextStringPos
          || pXmlContext->wNextStringPos + dstrNode.cchString >= pXmlContext->wNextOpenNodePos)
        {
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        ChkOverflow( pXmlContext->wBuffSize, pXmlContext->wBuffSize - pXmlContext->wNextStringPos );

        ChkDR( DRM_STR_StringCchCopyNW(&pXmlContext->XmlString [pXmlContext->wNextStringPos],
                                   pXmlContext->wBuffSize - pXmlContext->wNextStringPos,
                                   dstrNode.pwszString,
                                   dstrNode.cchString) );
    }

    pXmlContext->wNextStringPos += dstrNode.cchString;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_NO_INLINE DRM_DWORD DRM_CALL DRM_XMB_RequiredCharsForTag (
    __in const DRM_DWORD cchTag,
    __in const DRM_DWORD cchData,
    __in const DRM_DWORD cchAttrLabel,
    __in const DRM_DWORD cchAttrText)
{
    DRM_DWORD  cch = 0;

    cch = (cchTag * 2) +  5; /* <TAG></TAG> */

    cch += cchData;

    if (cchAttrLabel != 0)
    {
        cch += cchAttrLabel + 4;  /* two quotes, space and equal sign */
        cch += cchAttrText;
    }

    return cch;
}


DRM_API DRM_NO_INLINE DRM_DWORD DRM_CALL DRM_XMB_RequiredCharsForTagNoClose (
    __in const DRM_DWORD cchTag,
    __in const DRM_DWORD cchData,
    __in const DRM_DWORD cchAttrLabel,
    __in const DRM_DWORD cchAttrText)
{
    DRM_DWORD  cch = 0;

    cch = cchTag +  2; /* <TAG> */

    cch += cchData;

    if (cchAttrLabel != 0)
    {
        cch += cchAttrLabel + 4;  /* two quotes, space and equal sign */
        cch += cchAttrText;
    }

    return cch;
}


DRM_API DRM_NO_INLINE DRM_DWORD DRM_CALL DRM_XMB_RequiredCharsForAttribute (
    __in const DRM_DWORD cchAttrLabel,
    __in const DRM_DWORD cchAttrText)
{
    DRM_DWORD  cch = 0;

    if (cchAttrLabel != 0)
    {
        cch += cchAttrLabel + 4;  /* two quotes, space and equal sign */
        cch += cchAttrText;
    }

    return cch;
}


/**********************************************************************
**
** Function: DRM_XMB_WriteTag
**
** Synopsis: Adds XML node, it's value and attribute to XML context
**
** Arguments:   pbDataOut      - pointer to XML context
**              pdstrTag       - pointer to node name
**              pdstrData      - pointer to node value
**              pdstrAttrLabel - pointer to node name
**              pdstrAttrText  - pointer to node value
**              wtt            - value from WriteTagType enum
**                                     which specifies if tag should be closed
**
** Returns:     DRM_SUCCESS on success.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_WriteTag (
    __inout        _XMBContext      *pbDataOut,
    __in     const DRM_CONST_STRING *pdstrTag,
    __in_opt const DRM_CONST_STRING *pdstrData,
    __in_opt const DRM_CONST_STRING *pdstrAttrLabel,
    __in_opt const DRM_CONST_STRING *pdstrAttrText,
    __in           WriteTagType      wtt)
{
    DRM_RESULT  dr               = DRM_SUCCESS;
    _XMBContext oXMLContextSaved = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_WriteTag );

    ChkArg(pbDataOut != NULL
        && pdstrTag  != NULL);

    /* Save state of context to rollback if buffer overflows */
    ChkDR( _SaveContextState( pbDataOut, &oXMLContextSaved ) );

    ChkDR (DRM_XMB_OpenNode (pbDataOut, pdstrTag));

    if (pdstrAttrLabel != NULL
    &&  pdstrAttrText  != NULL)
    {
        ChkDR (DRM_XMB_AddAttribute (pbDataOut, pdstrAttrLabel, pdstrAttrText));
    }

    if (pdstrData != NULL)
    {
        ChkDR (DRM_XMB_AddData (pbDataOut, pdstrData));
    }

    if (wtt == wttClosed)
    {
        ChkDR (DRM_XMB_CloseCurrNode (pbDataOut,  NULL));
    }

ErrorExit:
    /* Restore state of context if buffer is not enough */
    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        _RestoreContextState( pbDataOut, &oXMLContextSaved );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
} /* DRM_XMB_WriteTag */

/**********************************************************************
**
** Function: DRM_XMB_WriteCDATATag
**
** Synopsis: Adds CData XML node, it's value and attribute to XML context
**
** Arguments:   pbDataOut      - pointer to XML context
**              pdstrTag       - pointer to node name
**              pdstrCDATA     - pointer to node value
**              pdstrAttrLabel - pointer to node name
**              pdstrAttrText  - pointer to node value
**              wtt            - value from WriteTagType enum
**                               which specifies if tag should be closed
**
** Returns:     DRM_SUCCESS on success.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_WriteCDATATag (
    __inout        _XMBContext      *pbDataOut,
    __in     const DRM_CONST_STRING *pdstrTag,
    __in_opt const DRM_CONST_STRING *pdstrCDATA,
    __in_opt const DRM_CONST_STRING *pdstrAttrLabel,
    __in_opt const DRM_CONST_STRING *pdstrAttrText,
    __in           WriteTagType      wtt)
{
    DRM_RESULT  dr               = DRM_SUCCESS;
    _XMBContext oXMLContextSaved = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_WriteCDATATag );

    ChkArg (pbDataOut  != NULL
        &&  pdstrTag   != NULL);

    /* Save state of context to rollback if buffer overflows */
    ChkDR( _SaveContextState( pbDataOut, &oXMLContextSaved ) );

    ChkDR (DRM_XMB_OpenNode (pbDataOut, pdstrTag));

    if (pdstrAttrLabel != NULL
    &&  pdstrAttrText  != NULL)
    {
        ChkDR (DRM_XMB_AddAttribute (pbDataOut, pdstrAttrLabel, pdstrAttrText));
    }

    if (pdstrCDATA != NULL)
    {
        ChkDR (DRM_XMB_AddCData (pbDataOut, pdstrCDATA));
    }

    if (wtt == wttClosed)
    {
        ChkDR (DRM_XMB_CloseCurrNode (pbDataOut,  NULL));
    }

ErrorExit:

    /* Restore state of context if buffer is not enough */
    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        _RestoreContextState( pbDataOut, &oXMLContextSaved );
    }
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
} /* DRM_XMB_WriteCDATATag */

DRM_API DRM_RESULT DRM_CALL DRM_XMB_RemainingBuffer(
    __in  const _XMBContext *f_pbXMB,
    __out       DRM_DWORD   *f_pcbRemaining )
{
    DRM_RESULT   dr       = DRM_SUCCESS;

    ChkArg(f_pbXMB        != NULL
        && f_pcbRemaining != NULL);

    ChkBOOL((f_pbXMB->wNextStringPos + CWCH_DWORD_STACK) < f_pbXMB->wNextOpenNodePos, DRM_E_BUFFERTOOSMALL);

    *f_pcbRemaining = (f_pbXMB->wNextOpenNodePos - (f_pbXMB->wNextStringPos + CWCH_DWORD_STACK));

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function: DRM_XMB_AddXMLNode
**
** Synopsis: Adds XML node and it's value to XML context
**
** Arguments:   pbXmlContext - pointer to XML context
**              dstrName     - pointer to node name
**              dstrName     - pointer to node value
**
** Returns:     DRM_SUCCESS on success.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddXMLNode (
    __inout       _XMBContext      *pbXMLContext,
    __in    const DRM_CONST_STRING *dstrName,
    __in    const DRM_CONST_STRING *dstrValue
)
{   DRM_RESULT  dr               = DRM_SUCCESS;
    _XMBContext oXMLContextSaved = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLBUILDER, PERF_FUNC_DRM_XMB_AddXMLNode );

    ChkArg( pbXMLContext != NULL && dstrName != NULL && dstrValue != NULL );

    /* Save state of context to rollback if buffer overflows */
    ChkDR( _SaveContextState( pbXMLContext, &oXMLContextSaved ) );

    /* Opens node, add data, closes node */
    ChkDR( DRM_XMB_OpenNode( pbXMLContext,  dstrName   ) );

    /* adds data */
    ChkDR( DRM_XMB_AppendNode( pbXMLContext, dstrValue ) );

    /*closes node */
    ChkDR( DRM_XMB_CloseCurrNode( pbXMLContext, NULL   ) );

ErrorExit:

    /* Restore state of context if buffer is not enough */
    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        _RestoreContextState( pbXMLContext, &oXMLContextSaved );
    }
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

