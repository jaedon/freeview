/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXMLPARSERGENERAL_C
#include <drmxmlparser.h>
#include <drmcrt.h>
#include <drmbase64.h>
#include <drmutilities.h>
#include <oemcipher.h>
#include <oemshahmac.h>
#include <oem.h>
#include <drmxmlhash.h>
#include <drmxmlparserconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

PRAGMA_STRICT_GS_PUSH_ON;

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

/*
***********************************************************************
** types used within this file
***********************************************************************
*/

typedef enum {
    eTagNone = 0,
    eOpenTag = 1,     /* <tag ...> */
    eCloseTag,      /* </tag> */
    eEmptyTag,      /* <tag ... /> */
    eCDataSection,  /* <![CDATA[...]]> */
}  _ETAGTYPE;

typedef enum {
    eInvalidData   = 0,
    eOpaqueData    = 1, /* treat the data as binary blob - allow any symbol in it */
    eTrimSpaces    = 2, /* trim leading and trailing spaces: old code behavior for attr values */
    eStandardValue = 3, /* follow the standards: trim spaces, don't allow special symbols like '<'. Not supported yet. */
}   _EDATATYPE;

typedef struct _tag_XMLTAG
{
    _ETAGTYPE       m_eTagType;
    DRM_SUBSTRING   m_dasstrTag;
    DRM_SUBSTRING   m_dasstrAttrName;
    DRM_SUBSTRING   m_dasstrAttrValue;
    DRM_SUBSTRING   m_dasstrCData;
    DRM_DWORD       m_ichTagBegin; /* index of '<' */
    DRM_DWORD       m_ichTagEnd;   /* index of '>' */
}  _XMLTAGA;

typedef struct _tag_XMLTAGW
{
    _ETAGTYPE        m_eTagType;         /* Tag type */
    const DRM_WCHAR *m_pwszBase;         /* XML data string with this tag */
    DRM_SUBSTRING    m_dasstrTag;        /* XML tag name */
    DRM_SUBSTRING    m_dasstrAttrName;   /* Name of one of XML attributes that XML tag has, can be NULL */
    DRM_SUBSTRING    m_dasstrAttrValue;  /* Value of XML attibute m_dasstrAttrName, or NULL */
    DRM_SUBSTRING    m_dasstrCData;      /* string data in a tag <![CDATA[data]]> */
    DRM_DWORD        m_ichTagBegin;      /* Index of '<' */
    DRM_DWORD        m_ichTagEnd;        /* index of '>' */
}  _XMLTAGW;

#define SCAN_FOR(condition)  \
{                            \
    while (ichCurr != ichEnd \
       && !(condition))      \
    {                        \
        ichCurr++;           \
    }                        \
    if (ichCurr >= ichEnd)   \
    {                        \
        break;               \
    }                        \
}

#define SCAN_WHILE(condition)    \
{                                \
    while(ichCurr != ichEnd      \
       && (condition))           \
    {                            \
        if (++ichCurr >= ichEnd) \
        {                        \
            break;               \
        }                        \
    }                            \
}

#define NEXT_CHAR            \
{                            \
    if (++ichCurr >= ichEnd) \
    {                        \
        break;               \
    }                        \
}

#define SKIP_SPACEW                         \
{                                           \
    while (ichCurr < ichEnd                 \
       &&  _IsSpaceW(pwszBase[ichCurr]))    \
    {                                       \
        ichCurr++;                          \
    }                                       \
    if (ichCurr >= ichEnd)                  \
    {                                       \
        break;                              \
    }                                       \
}

#define SKIP_SPACEA                                 \
{                                                   \
    while (ichCurr < ichEnd                         \
       &&  _IsSpaceA(GET_CHAR(f_pszBase,ichCurr)))  \
    {                                               \
        ichCurr++;                                  \
    }                                               \
    if (ichCurr >= ichEnd)                          \
    {                                               \
        break;                                      \
    }                                               \
}

/*
***********************************************************************
** static functions
***********************************************************************
*/

#define _IsAlphaNumW(f_wch) (DRMCRT_iswalpha(f_wch) || DRMCRT_iswdigit(f_wch))

#define _IsAlphaNumA(f_ch) ( (f_ch >= '0' && f_ch <= '9')        \
                          || (f_ch >= 'a' && f_ch <= 'z')        \
                          || (f_ch >= 'A' && f_ch <= 'Z'))

#define _IsSymbolW(f_wch) ( _IsAlphaNumW(f_wch)                  \
                         || f_wch == g_wchUnderscore             \
                         || f_wch == g_wchColon                  \
                         || f_wch == g_wchMinus                  \
                         || f_wch == g_wchPeriod )

#define _IsSymbolA(f_ch) ( _IsAlphaNumA(f_ch)                    \
                        || f_ch == '_'                           \
                        || f_ch == ':'                           \
                        || f_ch == '-'                           \
                        || f_ch == '.' )

#define _IsFirstNameSymbolW(f_wch) ( _IsAlphaNumW(f_wch)         \
                         || f_wch == g_wchUnderscore             \
                         || f_wch == g_wchColon )


#define _IsFirstNameSymbolA(f_ch) ( _IsAlphaNumA(f_ch)           \
                        || f_ch == '_'                           \
                        || f_ch == ':' )


#define _IsSpaceW(f_wch) ( f_wch > 0           && ( NATIVE_WCHAR(f_wch) < NATIVE_WCHAR (g_wchExclamationMark) ) )
#define _IsSpaceA(f_ch)  ( f_ch > 0            && f_ch < '!' )
#define _IsQuoteW(f_wch) ( f_wch == g_wchQuote || f_wch == g_wchSingleQuote )
#define _IsQuoteA(f_ch)  ( f_ch == '\''        || f_ch == '\"' )



/*********************************************************************
** Function  : _AllTrimW
**
** Synopsis  : Trim all leading and trailing blanks in given string.
**
** Arguments :
** [f_pdstr] : string to be trimmed
**
** Returns   : DRM_BOOL
**             TRUE if resulting string length > 0
*********************************************************************/
static DRM_NO_INLINE DRM_BOOL _AllTrimW(
    __inout DRM_CONST_STRING *f_pdstr )
{
    DRM_BOOL fOK = FALSE;

    if ( f_pdstr == NULL )
    {
        goto ErrorExit;
    }

    /*
    ** trim space in node data, if any
    */
    while ( f_pdstr->cchString > 0
        && _IsSpaceW( f_pdstr->pwszString [0] ) )
    {
        f_pdstr->pwszString++;
        f_pdstr->cchString--;
    }

    while ( f_pdstr->cchString > 0
        && _IsSpaceW( f_pdstr->pwszString [f_pdstr->cchString - 1] ) )
    {
        f_pdstr->cchString--;
    }

    fOK = ( f_pdstr->cchString > 0 );

ErrorExit:
    return fOK;
}


/*********************************************************************
** Function  : _AllTrimA
**
** Synopsis  : Trim all leading and trailing blanks in given string.
**
** Arguments
** [f_pszBase]  : base string
** [f_pdasstr]  : pointer to substring we trim
**
** Returns   : DRM_BOOL
**             TRUE if resulting string length > 0
*********************************************************************/
static DRM_NO_INLINE DRM_BOOL _AllTrimA(
    __in     const DRM_CHAR       *f_pszBase,
    __inout        DRM_SUBSTRING  *f_pdasstr )
{
    DRM_BOOL fOK = FALSE;

    if ( f_pszBase == NULL
        || f_pdasstr == NULL )
    {
        goto ErrorExit;
    }

    /*
    ** trim space in node data, if any
    */
    while (  f_pdasstr->m_cch > 0
        && _IsSpaceA( GET_CHAR( f_pszBase, f_pdasstr->m_ich ) ) )
    {
        f_pdasstr->m_ich++;
        f_pdasstr->m_cch--;
    }

    while ( f_pdasstr->m_cch > 0
       &&  _IsSpaceA( GET_CHAR( f_pszBase, ( f_pdasstr->m_ich + f_pdasstr->m_cch - 1 ) ) ) )
    {
        f_pdasstr->m_cch--;
    }

    fOK = ( f_pdasstr->m_cch > 0 );

ErrorExit:
    return fOK;
}


/************************************************************************************
** Function  :    _GetNextAttributeW
**
** Synopsis  :  Function looks for 'attribute="value"' construction in the data string.
**              It increases the pointer to a current position in a string if succeeds.
**
** Arguments :
** [f_pdstrXML]         : points somewhere inside XML tag, either after a tag name,
**                        or between attributes, or before the closing bracket:
**                        <name # attr1='val1'# attr2='val2' # attrN='valN'# />,
**                        and there may be no attributes in a tag at all.
**
** [f_pichCurr]         : Place in XML string to begin parsing from. Returns
**                        position next after parsed attribute-value construction.
**                        If parsing failed it does not change value so that a caller
**                        may try different parser.
**
** [f_eDataType]        : Tells if we follow XML standard requirements
**                        when parsing attribute value.
**
** [f_pdastrAttrName]   : Attribute name, may be NULL.
**
** [f_pdastrAttrValue]  : Attribute value, may be NULL.
**
** Returns   : DRM_BOOL. False if there is no attribute/value pair found.
**
** Note: If XML tag is well-formed but does not have attributes at all
**       or there are no attributes between current position and closing bracket
**       then the function also returns false.
************************************************************************************/
static DRM_NO_INLINE DRM_BOOL _GetNextAttributeW(
    __in  const DRM_CONST_STRING *f_pdstrXML,
    __in  const _EDATATYPE        f_eDataType,
    __inout     DRM_DWORD        *f_pichCurr,
    __out_opt   DRM_SUBSTRING    *f_pdastrAttrName,
    __out_opt   DRM_SUBSTRING    *f_pdastrAttrValue )
{
    const DRM_WCHAR *pwszBase    = f_pdstrXML->pwszString;
    DRM_DWORD        ichEnd      = f_pdstrXML->cchString;
    DRM_DWORD        ichCurr     = *f_pichCurr;
    DRM_DWORD        dwPos       = 0;
    DRM_SUBSTRING    dastrName   = {0};
    DRM_SUBSTRING    dastrValue  = {0};
    DRM_BOOL         fParsedOkay = FALSE;
    DRM_WCHAR        wchQuote    = ONE_WCHAR( '\0', '\0' );

    while ( TRUE ) /* loop is needed for SKIP...() and NEXT macros */
    {
        SKIP_SPACEW;

        /*
        ** Check for attribute name, stop parsing if failed
        */
        if ( !_IsSymbolW( pwszBase [ichCurr] ) )
        {
            goto ErrorExit;
        }

        /*
        ** Found attribute name
        */
        dastrName.m_ich = ichCurr;

        /*
        ** Scan for a terminator.
        */
        SCAN_WHILE(_IsSymbolW(pwszBase [ichCurr]));

        dastrName.m_cch = ichCurr - dastrName.m_ich;

        SKIP_SPACEW;

        /*
        ** Check for '=' and skip it
        */
        if ( pwszBase [ichCurr] != g_wchEqual )
        {
            goto ErrorExit;
        }
        NEXT_CHAR;

        SKIP_SPACEW;

        /*
        ** Check for quotes around attribute value: "value" or 'value'.
        ** Remember which type of quote is used, and skip it.
        */
        if ( !_IsQuoteW( pwszBase [ichCurr] ) )
        {
            goto ErrorExit;
        }
        wchQuote = pwszBase [ichCurr];
        NEXT_CHAR;

        /*
        ** Found attribute value
        */
        dastrValue.m_ich = ichCurr;

        /*
        ** Scan for the terminating quote.
        */
        SCAN_FOR( pwszBase [ichCurr] == wchQuote );

        dastrValue.m_cch = ichCurr - dastrValue.m_ich;

        NEXT_CHAR;

        if (  f_eDataType == eTrimSpaces
           || f_eDataType == eStandardValue )
        {
            while ( dastrValue.m_cch > 0
                &&  _IsSpaceW( pwszBase [dastrValue.m_ich
                                       + dastrValue.m_cch
                                       - 1] ) )
            {
               dastrValue.m_cch--;
            }

            if ( f_eDataType == eStandardValue )
            {
                /*
                ** In addition to trimming spaces
                ** verify that value has only symbol characters,
                ** and the first letter if either alphanum, '_' or ':'
                */
                if ( !_IsFirstNameSymbolW( pwszBase[dastrValue.m_ich] ) )
                {
                    goto ErrorExit;
                }
                for ( dwPos = dastrValue.m_ich + 1; dwPos < dastrValue.m_cch; dwPos++ )
                {
                    if ( !_IsSymbolW( pwszBase[dwPos] ) )
                    {
                        goto ErrorExit;
                    }
                }
            }
        }

        /*
        ** Parsed the attribute successfully
        */
        *f_pichCurr = ichCurr;
        fParsedOkay = TRUE;

        break;
    }

ErrorExit:
    /*
    ** save attribute and value string if caller wants
    */
    if( f_pdastrAttrName != NULL )
    {
        f_pdastrAttrName->m_cch = dastrName.m_cch;
        f_pdastrAttrName->m_ich  = dastrName.m_ich;
    }
    if( f_pdastrAttrValue != NULL )
    {
        f_pdastrAttrValue->m_cch = dastrValue.m_cch;
        f_pdastrAttrValue->m_ich  = dastrValue.m_ich;
    }

    return fParsedOkay;
}


/************************************************************************************
** Function  :    _GetNextAttributeA
**
** Synopsis  :  Function looks for 'attribute="value"' construction in the data string.
**              It increases the pointer to a current position in a string if succeeds.
**
** Arguments :
** [f_pszBase]          : base string with XML data
**
** [f_pdasstrXML]       : points somewhere inside XML tag, either after a tag name,
**                        or between attributes, or before the closing bracket:
**                        <name # attr1='val1'# attr2='val2' # attrN='valN'# />,
**                        and there may be no attributes in a tag at all.
**
** [f_eDataType]        : Tells if we follow XML standard requirements
**                        when parsing attribute value.
**
** [f_pichCurr]         : Place in XML string to begin parsing from. Returns
**                        position next after parsed attribute-value construction.
**                        If parsing failed it does not change value so that a caller
**                        may try different parser.
**
** [f_pdasstrAttrName]  : attribute name, may be NULL.
**
** [f_pdasstrAttrValue] : attribute value, may be NULL.
**
** Returns   : DRM_BOOL. False if there is no attribute/value pair found.
**
** Note: If XML tag is well-formed but does not have attributes at all
**       or there are no attributes between current position and closing bracket
**       then the function also returns false.
************************************************************************************/
static DRM_NO_INLINE DRM_BOOL _GetNextAttributeA(
    __in  const DRM_CHAR          *f_pszBase,
    __in  const DRM_SUBSTRING     *f_pdasstrXML,
    __in  const _EDATATYPE         f_eDataType,
    __inout     DRM_DWORD         *f_pichCurr,
    __out_opt   DRM_SUBSTRING     *f_pdasstrAttrName,
    __out_opt   DRM_SUBSTRING     *f_pdasstrAttrValue )
{
    DRM_DWORD        ichEnd      = f_pdasstrXML->m_ich
                                 + f_pdasstrXML->m_cch;
    DRM_DWORD        ichCurr     = *f_pichCurr;
    DRM_DWORD        dwPos       = 0;
    DRM_SUBSTRING    dastrName   = {0};
    DRM_SUBSTRING    dastrValue  = {0};
    DRM_BOOL         fParsedOkay = FALSE;
    DRM_CHAR         chQuote     = '\0';

    while ( TRUE ) /* loop is needed for SCAN, SKIP and NEXT macros */
    {
        SKIP_SPACEA;

        /*
        ** Check for attribute name, stop parsing if failed
        */
        if ( !_IsSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) )
        {
            goto ErrorExit;
        }

        /*
        ** Found attribute name
        */
        dastrName.m_ich = ichCurr;

        /*
        ** Scan for a terminator.
        */
        SCAN_WHILE( _IsSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) );

        dastrName.m_cch = ichCurr - dastrName.m_ich;

        SKIP_SPACEA;

        /*
        ** Check for '=' and skip it
        */
        if ( GET_CHAR( f_pszBase, ichCurr ) != '=' )
        {
            goto ErrorExit;
        }
        NEXT_CHAR;

        SKIP_SPACEA;

        /*
        ** Check for quotes around attribute value: "value" or 'value'.
        ** Remember which type of quote is used, and skip it.
        */
        if ( !_IsQuoteA( GET_CHAR( f_pszBase, ichCurr ) ) )
        {
            goto ErrorExit;
        }
        chQuote = (DRM_CHAR)GET_CHAR( f_pszBase, ichCurr );
        NEXT_CHAR;

        /*
        ** Found attribute value
        */
        dastrValue.m_ich = ichCurr;

        /*
        ** Scan for the terminating quote.
        */
        SCAN_FOR( GET_CHAR( f_pszBase, ichCurr ) == chQuote );

        dastrValue.m_cch = ichCurr - dastrValue.m_ich;

        NEXT_CHAR;

        if ( f_eDataType == eTrimSpaces || f_eDataType == eStandardValue )
        {
            while ( dastrValue.m_cch > 0
                &&  _IsSpaceA( GET_CHAR( f_pszBase,
                                       dastrValue.m_ich + dastrValue.m_cch - 1 ) ) )
            {
               dastrValue.m_cch--;
            }
            if ( f_eDataType == eStandardValue )
            {
                /*
                ** In addition to trimming spaces
                ** verify that value has only symbol characters,
                ** and the first letter if either alphanum, '_' or ':'
                */
                if ( !_IsFirstNameSymbolA( GET_CHAR( f_pszBase, dastrValue.m_ich ) ) )
                {
                    goto ErrorExit;
                }
                for ( dwPos = dastrValue.m_ich + 1; dwPos < dastrValue.m_cch; dwPos++ )
                {
                    if ( !_IsSymbolA( GET_CHAR( f_pszBase, dwPos ) ) )
                    {
                        goto ErrorExit;
                    }
                }
            }
        }

        /*
        ** Parsed the attribute successfully
        */
        *f_pichCurr = ichCurr;
        fParsedOkay = TRUE;

        break;
    }

ErrorExit:
    /*
    ** save attribute and value string if caller wants
    */
    if( f_pdasstrAttrName != NULL )
    {
        f_pdasstrAttrName->m_cch = dastrName.m_cch;
        f_pdasstrAttrName->m_ich  = dastrName.m_ich;
    }
    if( f_pdasstrAttrValue != NULL )
    {
        f_pdasstrAttrValue->m_cch = dastrValue.m_cch;
        f_pdasstrAttrValue->m_ich  = dastrValue.m_ich;
    }

    return fParsedOkay;
}


/**********************************************************************************************
** Function  : _ScanTagW
**
** Synopsis  : Finds the first well-formed XML tag of the type that we support;
**             for example, comments tag will be ignored.
**
** Arguments :
**    [f_pdstrXML]     : XML data string where we look for XML tag
**    [f_pichCurr]     : pointer to start position in XML data string, may be modified
**    [f_ptagParsed]   : output XML tag
**
** Returns   : DRM_BOOL : True if found one of the following:
**                      1. CDATA tag: <![CDATA[xxxx]]>
**                      2. Close tag: </tagName>
**                      3. Open tag:  <tagName attrib="value">
**                      4. Empty tag:  <tagName attrib="value" />
**
** Notes     :  1. f_pichCurr will be changed only if function succeeded - it will point
**              to the next character after end of parsed tag.
**              2. f_ptagParsed may contain invalid data if function failed.
**
**********************************************************************************************/
static DRM_NO_INLINE DRM_BOOL _ScanTagW(
    __in const DRM_CONST_STRING *f_pdstrXML,
    __inout    DRM_DWORD        *f_pichCurr,
    __out      _XMLTAGW         *f_ptagParsed )
{
    const DRM_WCHAR *pwszBase    =  f_pdstrXML->pwszString;
    DRM_DWORD        ichCurr     = *f_pichCurr;
    DRM_DWORD        ichEnd      =  f_pdstrXML->cchString;
    DRM_BOOL         fParsedOkay =  FALSE;
    DRM_DWORD        ichOpenTag  =  0;        /* position of '<' */


#if DBG
    ZEROMEM(f_ptagParsed, SIZEOF(_XMLTAGW));  /* init return value */
#endif

    f_ptagParsed->m_pwszBase = pwszBase;
    f_ptagParsed->m_eTagType = eTagNone;

    while ( ichCurr < ichEnd )
    {
        /*
        ** Look for '<'
        */
        SCAN_FOR( pwszBase [ichCurr] == g_wchLessThan );

        ichOpenTag = ichCurr;

        NEXT_CHAR;

        /*
        ** ------------------------------------------------------------------
        ** '</...'
        ** parse the close tag
        ** ------------------------------------------------------------------
        */
        if ( pwszBase [ichCurr] == g_wchForwardSlash )
        {
            NEXT_CHAR;

            if ( _IsSymbolW( pwszBase [ichCurr] ) )
            {
                f_ptagParsed->m_eTagType = eCloseTag;

                goto PARSE_TAG;
            }
            /*
            ** Bad tag: look for '>' to end it, ignore the tag and continue.
            */
            SCAN_FOR( pwszBase [ichCurr] == g_wchGreaterThan );

            NEXT_CHAR;
            continue;
        }

        /*
        ** ------------------------------------------------------------------
        ** '<?...'
        ** parse the Process Instruction: not supported, ignore and continue
        ** ------------------------------------------------------------------
        */
        else if ( pwszBase [ichCurr] == g_wchQuestionMark )
        {
            NEXT_CHAR;

            SCAN_FOR(pwszBase [ichCurr] == g_wchGreaterThan);

            NEXT_CHAR;
            continue;
        }

        /*
        ** ------------------------------------------------------------------
        ** '<!...'
        ** parse the Special tag, read CDATA tag, ignore others and continue
        ** ------------------------------------------------------------------
        */
        else if ( pwszBase [ichCurr] == g_wchExclamationMark )
        {
            NEXT_CHAR;

            if ( pwszBase [ichCurr] == g_wchMinus )
            {
                /*
                ** '<!-...'
                ** Comment tag, look for terminating '-->', ignore and continue.
                */
                SCAN_FOR( ichCurr < ichEnd - 2
                    && pwszBase [ichCurr]     == g_wchMinus
                    && pwszBase [ichCurr + 1] == g_wchMinus
                    && pwszBase [ichCurr + 2] == g_wchGreaterThan );

                ichCurr+= 2; /* set ichCurr to '>' in '--> */

                NEXT_CHAR;
                continue;
            }

            /*
            ** '<![...'
            ** Could be CDATA section, otherwise it's a bad tag
            */
            else if ( pwszBase [ichCurr] == g_wchLeftBracket )
            {
                NEXT_CHAR;
                /*
                ** Check if this is CDATA section: '<![CDATA[...'
                */
                if ( ( ichCurr + g_dstrTagCDATAOpenBracket.cchString ) >= ichEnd
                    || ( DRMCRT_wcsncmp( pwszBase + ichCurr,
                                 g_dstrTagCDATAOpenBracket.pwszString,
                                 g_dstrTagCDATAOpenBracket.cchString) != 0 ) )
                {
                    /*
                    ** not CDATA section, look for '>' to end it, ignore the tag and continue.
                    */
                    SCAN_FOR( pwszBase [ichCurr] == g_wchGreaterThan );

                    NEXT_CHAR;
                    continue;
                }
                else
                {
                    /*
                    ** CDATA section found, parse it right here.
                    ** The tag's name is always "CDATA".
                    */
                    f_ptagParsed->m_dasstrTag.m_ich = ichCurr;
                    f_ptagParsed->m_dasstrTag.m_cch = g_dstrTagCDATA.cchString;

                    /*
                    ** Skip "CDATA["
                    */
                    ichCurr += g_dstrTagCDATAOpenBracket.cchString;

                    f_ptagParsed->m_dasstrCData.m_ich = ichCurr;
                    f_ptagParsed->m_dasstrCData.m_cch = 0;

                    /*
                    ** Scan for terminating ']]>'
                    */
                    SCAN_FOR( ichCurr + 2 < ichEnd
                        && pwszBase [ichCurr]     == g_wchRightBracket
                        && pwszBase [ichCurr + 1] == g_wchRightBracket
                        && pwszBase [ichCurr + 2] == g_wchGreaterThan );

                    /*
                    ** CDATA tag parsed successfully,
                    ** save data and exit the main while loop
                    */
                    f_ptagParsed->m_dasstrCData.m_cch = ichCurr
                                                        - f_ptagParsed->m_dasstrCData.m_ich;

                    f_ptagParsed->m_eTagType    = eCDataSection;
                    f_ptagParsed->m_ichTagBegin = ichOpenTag;
                    f_ptagParsed->m_ichTagEnd   = ichCurr;

                    fParsedOkay = TRUE;
                    break;
                }
            }
            /*
            ** inline DTD tag which is not '<!...' and not '<!-...' and not '<![...'
            ** ignore and continue
            */
            else if ( _IsSymbolW( pwszBase [ichCurr] ) )
            {
                NEXT_CHAR;

                SCAN_FOR( pwszBase [ichCurr] == g_wchGreaterThan );

                NEXT_CHAR;
                continue;
            }
        }
        /*
        ** ------------------------------------------------------------------
        ** '<#...'
        ** Open tag, parse it.
        ** ------------------------------------------------------------------
        */
        else if ( _IsFirstNameSymbolW( pwszBase [ichCurr] ) ) /* first symbol cannot be '.' or '-' */
        {
PARSE_TAG:
            /*
            ** Scan for tag name.
            */
            f_ptagParsed->m_dasstrTag.m_ich = ichCurr;

            SCAN_WHILE(_IsSymbolW(pwszBase [ichCurr]));

            f_ptagParsed->m_dasstrTag.m_cch = ichCurr - f_ptagParsed->m_dasstrTag.m_ich;

            /*
            ** We've got the tag name.
            ** Skip whitespaces and parse attr name.
            */
            SKIP_SPACEW;

            if( _GetNextAttributeW( f_pdstrXML,
                                    eTrimSpaces,
                                    &ichCurr,
                                    &f_ptagParsed->m_dasstrAttrName,
                                    &f_ptagParsed->m_dasstrAttrValue ) )
            {
                /*
                ** Error: there can be no attributes in </tagname> nodes
                */
                if ( f_ptagParsed->m_eTagType == eCloseTag )
                {
                    break;
                }

                /*
                ** We have the first attribute value, skip the rest.
                */
                while( _GetNextAttributeW( f_pdstrXML,
                                           eOpaqueData,
                                           &ichCurr,
                                           NULL,
                                           NULL ) )
                {
                }
            }

            /*
            ** Find the end of tag.
            */
            SCAN_FOR( pwszBase [ichCurr] == g_wchGreaterThan );

            if ( f_ptagParsed->m_eTagType != eCloseTag )
            {
                if ( pwszBase [ichCurr - 1] == g_wchForwardSlash )
                {
                    f_ptagParsed->m_eTagType = eEmptyTag;
                }
                else
                {
                    f_ptagParsed->m_eTagType = eOpenTag;
                }
            }

            f_ptagParsed->m_ichTagBegin = ichOpenTag;
            f_ptagParsed->m_ichTagEnd   = ichCurr;

            fParsedOkay = TRUE;

            NEXT_CHAR;
            break;  /* that's it, we parsed a tag */
        }
    }

    if ( fParsedOkay )
    {
        *f_pichCurr = ichCurr;
    }

    return fParsedOkay;
}


/**********************************************************************************************
** Function  : _ScanTagA
**
** Synopsis  : Finds the first well-formed XML tag of the type that we support;
**             for example, comments tag will be ignored.
**
** Arguments :
**    [f_pszBase]        : base string
**    [f_pdasstrXML]     : pointer to XML data string where we look for XML tag
**    [f_pichCurr]       : pointer to start position in XML data string, may be modified
**    [f_ptagParsed]     : output XML tag
**
** Returns   : DRM_BOOL : True if found one of the following:
**                      1. CDATA tag: <![CDATA[xxxx]]>
**                      2. Close tag: </tagName>
**                      3. Open tag:  <tagName attrib="value">
**                      4. Empty tag:  <tagName attrib="value" />
**
** Notes     :  1. f_pichCurr will be changed only if function succeeded - it will point
**              to the next character after end of parsed tag.
**              2. f_ptagParsed may contain invalid data if function failed.
**
**********************************************************************************************/
static DRM_NO_INLINE DRM_BOOL _ScanTagA(
    __in const DRM_CHAR      *f_pszBase,
    __in const DRM_SUBSTRING *f_pdasstrXML,
    __inout    DRM_DWORD     *f_pichCurr,
    __out      _XMLTAGA      *f_ptagParsed )
{
    DRM_DWORD ichEnd      = f_pdasstrXML->m_ich + f_pdasstrXML->m_cch;
    DRM_DWORD ichCurr     = *f_pichCurr;
    DRM_BOOL  fParsedOkay = FALSE;
    DRM_DWORD ichOpenTag  = 0;

#if DBG
    ZEROMEM(f_ptagParsed, SIZEOF(_XMLTAGA));    /* init return value */
#endif

    f_ptagParsed->m_eTagType = eTagNone;

    while ( ichCurr < ichEnd )
    {
        /*
        ** Look for '<'
        */
        SCAN_FOR( GET_CHAR( f_pszBase, ichCurr ) == '<' );

        ichOpenTag = ichCurr;

        NEXT_CHAR;

        /*
        ** ------------------------------------------------------------------
        ** '</...'
        ** parse the close tag
        ** ------------------------------------------------------------------
        */
        if ( GET_CHAR( f_pszBase, ichCurr ) == '/' )
        {
            NEXT_CHAR;

            if ( _IsSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) )
            {
                f_ptagParsed->m_eTagType = eCloseTag;
                goto PARSE_TAG;
            }
            /*
            ** Bad tag: look for '>' to end it, ignore the tag and continue.
            */
            SCAN_FOR( GET_CHAR( f_pszBase, ichCurr ) == '>' );

            NEXT_CHAR;
            continue;
        }
        /*
        ** ------------------------------------------------------------------
        ** '<?...'
        ** parse the Process Instruction: not supported, ignore and continue
        ** ------------------------------------------------------------------
        */
        else if ( GET_CHAR( f_pszBase, ichCurr ) == '?' )
        {
            NEXT_CHAR;

            SCAN_FOR( GET_CHAR( f_pszBase, ichCurr ) == '>' );

            NEXT_CHAR;
            continue;
        }

        /*
        ** ------------------------------------------------------------------
        ** '<!...'
        ** parse the Special tag, read CDATA tag, ignore others and continue
        ** ------------------------------------------------------------------
        */
        else if ( GET_CHAR( f_pszBase, ichCurr ) == '!' )
        {
            NEXT_CHAR;

            if ( GET_CHAR( f_pszBase, ichCurr ) == '-' )
            {
                /*
                ** '<!-...'
                ** Comment tag, look for terminating '-->', ignore and continue.
                */
                SCAN_FOR( ichCurr < ichEnd - 2
                        && GET_CHAR( f_pszBase, ichCurr )     == '-'
                        && GET_CHAR( f_pszBase, ichCurr + 1 ) == '-'
                        && GET_CHAR( f_pszBase, ichCurr + 2 ) == '>' );

                ichCurr += 2; /* set ichCurr to '>' in '--> */

                NEXT_CHAR;
                continue;
            }
            /*
            ** '<![...'
            ** Could be CDATA section, otherwise it's a bad tag
            */
            else if ( GET_CHAR( f_pszBase, ichCurr ) == '[' )
            {
                NEXT_CHAR;
                /*
                ** Check if this is CDATA section: '<![CDATA[...'
                */
                if ( ichCurr + g_dastrTagCDATAOpenBracket.cchString >= ichEnd
                    || DRM_BYT_CompareBytes( f_pszBase,
                                             ichCurr,
                                             g_dastrTagCDATAOpenBracket.pszString,
                                             0,
                                             g_dastrTagCDATAOpenBracket.cchString ) != 0 )
                {
                    /*
                    ** not CDATA section, scan for terminating '>' and ignore this tag
                    */
                    SCAN_FOR( GET_CHAR( f_pszBase, ichCurr ) == '>' );

                    NEXT_CHAR;
                    continue;
                }
                else
                {
                    /*
                    ** CDATA section found, parse it right here.
                    ** The tag's name is always "CDATA".
                    */
                    f_ptagParsed->m_dasstrTag.m_ich = ichCurr;
                    f_ptagParsed->m_dasstrTag.m_cch = g_dastrTagCDATA.cchString;

                    /*
                    ** Skip "CDATA["
                    */
                    ichCurr += g_dastrTagCDATAOpenBracket.cchString;

                    f_ptagParsed->m_dasstrCData.m_ich = ichCurr;
                    f_ptagParsed->m_dasstrCData.m_cch = 0;

                    /*
                    ** Scan for terminating ']]>'
                    */
                    SCAN_FOR( ichCurr < ichEnd - 2
                        && GET_CHAR( f_pszBase, ichCurr )     == ']'
                        && GET_CHAR( f_pszBase, ichCurr + 1 ) == ']'
                        && GET_CHAR( f_pszBase, ichCurr + 2 ) == '>' );

                    f_ptagParsed->m_dasstrCData.m_cch = ichCurr
                                                        - f_ptagParsed->m_dasstrCData.m_ich;
                    /*
                    ** CDATA tag parsed successfully,
                    ** save data and exit the main while loop
                    */
                    f_ptagParsed->m_eTagType    = eCDataSection;
                    f_ptagParsed->m_ichTagBegin = ichOpenTag;

                    f_ptagParsed->m_ichTagEnd = ichCurr;
                    fParsedOkay               = TRUE;
                    break;
                }
            }
            /*
            ** inline DTD tag which is not '<!...' and not '<!-...' and not '<![...'
            ** ignore and continue
            */
            else if ( _IsSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) )
            {
                NEXT_CHAR;
                SCAN_FOR(GET_CHAR(f_pszBase, ichCurr) == '>');
                NEXT_CHAR;
                continue;
            }
        }
        /*
        ** ------------------------------------------------------------------
        ** '<#...'
        ** Open tag, parse it.
        ** ------------------------------------------------------------------
        */
        else if ( _IsFirstNameSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) ) /* first symbol cannot be '.' or '-' */
        {
PARSE_TAG:
            /*
            ** Scan for tag name.
            */
            f_ptagParsed->m_dasstrTag.m_ich = ichCurr;

            SCAN_WHILE( _IsSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) );

            f_ptagParsed->m_dasstrTag.m_cch = ichCurr - f_ptagParsed->m_dasstrTag.m_ich;

            /*
            ** We've got the tag name.
            ** Skip whitespaces and parse attr name.
            */
            SKIP_SPACEA;

            if( _GetNextAttributeA( f_pszBase,
                                    f_pdasstrXML,
                                    eTrimSpaces,
                                    &ichCurr,
                                    &f_ptagParsed->m_dasstrAttrName,
                                    &f_ptagParsed->m_dasstrAttrValue ) )
            {
                /*
                ** Error: there can be no attributes in </tagname> nodes
                */
                if ( f_ptagParsed->m_eTagType == eCloseTag )
                {
                    break;
                }

                /*
                ** We have the first attribute value, skip the rest.
                */
                while( _GetNextAttributeA( f_pszBase,
                                           f_pdasstrXML,
                                           eOpaqueData,
                                           &ichCurr,
                                           NULL,
                                           NULL ) )
                {
                }
            }
            /*
            ** Find the end of tag.
            */
            SCAN_FOR( GET_CHAR( f_pszBase, ichCurr ) == '>' );

            if ( f_ptagParsed->m_eTagType != eCloseTag )
            {
                if ( GET_CHAR( f_pszBase, ichCurr - 1 ) == '/' )
                {
                    f_ptagParsed->m_eTagType = eEmptyTag;
                }
                else
                {
                    f_ptagParsed->m_eTagType = eOpenTag;
                }
            }

            f_ptagParsed->m_ichTagBegin = ichOpenTag;
            f_ptagParsed->m_ichTagEnd   = ichCurr;

            fParsedOkay = TRUE;

            NEXT_CHAR;
            break;  /* that's it, we parsed a tag */
        }
    }

    if (fParsedOkay)
    {
        *f_pichCurr = ichCurr;
    }

    return fParsedOkay;
}


/**********************************************************************
** Function  :    _ScanNodeForAttributeW
**
** Synopsis  :  Function looks for an attribute's value in the first XML tag
**              it finds in the string.
**
** Arguments
** [f_pdstrNode]       : points to a node whose attributes are scanned,
**                       should look like
**                       <name attr1="val1" attr1="val2" ... attrN="valN">
** [f_pdstrAttr]       : attribute name to retrieve, assume it is never NULL
** [f_ptagParsed]      : XML tag found
**
** Returns   : DRM_BOOL
************************************************************************/
static DRM_NO_INLINE DRM_BOOL _ScanNodeForAttributeW(
    __in const DRM_CONST_STRING *f_pdstrNode,
    __in const DRM_CONST_STRING *f_pdstrAttr,
    __out      _XMLTAGW         *f_ptagParsed )
{
    const DRM_WCHAR *pwszBase    = f_pdstrNode->pwszString;
    DRM_DWORD        ichEnd      = f_pdstrNode->cchString;
    DRM_DWORD        ichCurr     = 0;
    DRM_BOOL         fParsedOkay = FALSE;
    DRM_CONST_STRING dstrAttr    = EMPTY_DRM_STRING;

#if DBG
    ZEROMEM( f_ptagParsed, SIZEOF( _XMLTAGW ) );  /* init return value */
#endif

    while ( TRUE )
    {
        SCAN_FOR( pwszBase [ichCurr] == g_wchLessThan );   /* Find the next '<' */

        if ( pwszBase [ichCurr] != g_wchLessThan )
        {
            goto ErrorExit;   /* cannot find  '<' */
        }

        NEXT_CHAR;

        /*
        ** Parse the contents of a tag
        */
        if ( !_IsFirstNameSymbolW( pwszBase [ichCurr] ) )  /* '<#...' */
        {
            goto ErrorExit;  /* cannot find an element tag */
        }

        /*
        ** Scan for tag name.
        */
        f_ptagParsed->m_dasstrTag.m_ich = ichCurr;

        /*
        ** Scan for a terminator.
        */
        SCAN_WHILE( _IsSymbolW( pwszBase [ichCurr] ) );

        f_ptagParsed->m_dasstrTag.m_cch = ichCurr - f_ptagParsed->m_dasstrTag.m_ich;

        /*
        ** At this point, we've got the tag name
        */

        SKIP_SPACEW;   /* skip any whitespace and parse attributes */

        if ( _IsSymbolW( pwszBase [ichCurr] ) )  /* <... #... */
        {
PARSE_ATTR:
            if( _GetNextAttributeW( f_pdstrNode,
                                    eTrimSpaces,
                                    &ichCurr,
                                    &f_ptagParsed->m_dasstrAttrName,
                                    &f_ptagParsed->m_dasstrAttrValue ) )
            {
                dstrAttr.pwszString = pwszBase + f_ptagParsed->m_dasstrAttrName.m_ich;
                dstrAttr.cchString  =            f_ptagParsed->m_dasstrAttrName.m_cch;

                if ( DRM_UTL_DSTRStringsEqual( &dstrAttr, f_pdstrAttr ) )
                {
                    /*
                    ** Scan till there is no more attributes
                    */
                    while( _GetNextAttributeW( f_pdstrNode,
                                               eOpaqueData,
                                               &ichCurr,
                                               NULL,
                                               NULL ) )
                    {
                    }

                    /*
                    ** Now scan for '>' to make sure this is a legal tag
                    */
                    SCAN_FOR( pwszBase [ichCurr] == g_wchGreaterThan );

                    fParsedOkay = TRUE;

                    f_ptagParsed->m_ichTagBegin = 0;
                    f_ptagParsed->m_ichTagEnd   = 0;
                    DRMASSERT( ichCurr > 1 );
                    if ( pwszBase [ichCurr - 1] == g_wchForwardSlash )
                    {
                        f_ptagParsed->m_eTagType = eEmptyTag;
                    }
                    else
                    {
                        f_ptagParsed->m_eTagType = eOpenTag;
                    }
                    goto ErrorExit;        /* we found it */
                }
                else
                {
                    SKIP_SPACEW;   /* skip any whitespace */

                    if ( _IsAlphaNumW( pwszBase [ichCurr] ) )
                    {
                        /*
                        * Parse another attribute
                        */
                        goto PARSE_ATTR;
                    }
                }
            }
        }
        break;
    }

ErrorExit:
    return fParsedOkay;
}


/**********************************************************************
** Function  :    _ScanNodeForAttributeA
**
** Synopsis  :  Function looks for an attribute's value in the first XML tag
**              it finds in the string.
**
** Arguments
** [f_pszBase]        : data string
** [f_pdstrNode]      : substring with a node whose attributes are scanned,
**                      should look like
**                      <name attr1="val1" attr1="val2" ... attrN="valN">
** [f_pdastrAttrName] : attribute name to retrieve, assume it is never NULL
** [f_ptagParsed]     : XML tag found
**
** Returns   : DRM_BOOL
************************************************************************/
static DRM_NO_INLINE DRM_BOOL _ScanNodeForAttributeA(
    __in const DRM_CHAR              *f_pszBase,
    __in const DRM_SUBSTRING         *f_pdasstrNode,
    __in const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __out      _XMLTAGA              *f_ptagParsed )
{
    DRM_DWORD ichEnd      = f_pdasstrNode->m_ich
                          + f_pdasstrNode->m_cch;
    DRM_DWORD ichCurr     = f_pdasstrNode->m_ich;
    DRM_BOOL  fParsedOkay = FALSE;

#if DBG
    ZEROMEM( f_ptagParsed, SIZEOF( _XMLTAGA ) ); /* init return value */
#endif

    while ( TRUE )
    {
        SCAN_FOR( GET_CHAR( f_pszBase, ichCurr ) == '<' ); /* Find the next '<' */

        if ( GET_CHAR( f_pszBase, ichCurr ) != '<' )
        {
            goto ErrorExit;   /* cannot find  '<' */
        }

        NEXT_CHAR;

        /*
        ** Parse the contents of a tag
        */
        if ( !_IsFirstNameSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) )  /* '<#...' */
        {
            goto ErrorExit;  /* cannot find an element tag */
        }

        /*
        ** Scan for tag name.
        */
        f_ptagParsed->m_dasstrTag.m_ich = ichCurr;

        /*
        ** Scan for a terminator.
        */
        SCAN_WHILE( _IsSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) );

        f_ptagParsed->m_dasstrTag.m_cch = ichCurr - f_ptagParsed->m_dasstrTag.m_ich;

        /*
        ** At this point, we've got the tag name
        */
        SKIP_SPACEA;   /* skip any whitespace and parse attr name. */

        if ( _IsSymbolA( GET_CHAR( f_pszBase, ichCurr ) ) )  /* <... #... */
        {
PARSE_ATTR:
            if( _GetNextAttributeA( f_pszBase,
                                    f_pdasstrNode,
                                    eTrimSpaces,
                                    &ichCurr,
                                    &f_ptagParsed->m_dasstrAttrName,
                                    &f_ptagParsed->m_dasstrAttrValue ) )
            {

                /* check if this is the attr we want */
                if ( DRM_UTL_DASSTRStringsEqual( f_pszBase,
                                                 &f_ptagParsed->m_dasstrAttrName,
                                                 f_pdastrAttrName ) )
                {
                    /*
                    ** Scan till there is no more attributes
                    */
                    while( _GetNextAttributeA( f_pszBase,
                                               f_pdasstrNode,
                                               eOpaqueData,
                                               &ichCurr,
                                               NULL,
                                               NULL ) )
                    {}

                    /*
                    ** Scan for '>' to make sure this is a legal tag
                    */
                    SCAN_FOR( GET_CHAR( f_pszBase, ichCurr ) == '>' );

                    fParsedOkay = TRUE;

                    f_ptagParsed->m_ichTagBegin = 0;
                    f_ptagParsed->m_ichTagEnd   = 0;
                    DRMASSERT( ichCurr > 1 );
                    if ( GET_CHAR( f_pszBase, ichCurr - 1 ) == '/' )
                    {
                        f_ptagParsed->m_eTagType = eEmptyTag;
                    }
                    else
                    {
                        f_ptagParsed->m_eTagType = eOpenTag;
                    }

                    goto ErrorExit;        /* we found it */
                }
                else
                {
                    SKIP_SPACEA;   /* skip any whitespace and parse attr value. */

                    if ( _IsAlphaNumA( GET_CHAR( f_pszBase, ichCurr ) ) )
                    {
                        goto PARSE_ATTR;    /* keep going */
                    }
                }
            }
        }

        break;
    }

ErrorExit:
    return fParsedOkay;
}



/******************************************************************************************************************
** Function  :    _CheckTagW
**
** Synopsis  :    Checks if some XML tag matches tagname, attribute and value parameters.
**                If f_pdstrTag is NULL or empty then try to match only by attribute and value.
**                If f_pdstrTag string is present then match by tag name
**                and by attribute and value only if attribute is not empty.
**                If all three string parameters are NULL then the function returns tag's type.
**                If the tag name is NULL and tag type is eCloseTag then the function returns eCloseTag type.
**
** Arguments :
**     [f_ptagParsed]     : some XML tag
**     [f_pdstrTag]       : pointer to XML tag name, may be NULL
**     [f_pdstrAttrName]  : pointer to XML tag's attribute name, may be NULL
**     [f_pdstrAttrValue] : pointer to XML tag's attribute value, may be NULL
**
** Returns   : _ETAGTYPE     : if there is no match - eTagNone, otherwise f_ptagParsed type
******************************************************************************************************************/
static DRM_NO_INLINE _ETAGTYPE _CheckTagW(
    __in_opt const _XMLTAGW         *f_ptagParsed,
    __in     const DRM_CONST_STRING *f_pdstrTag,
    __in_opt const DRM_CONST_STRING *f_pdstrAttrName,
    __in_opt const DRM_CONST_STRING *f_pdstrAttrValue )
{
    const DRM_WCHAR *pwszBase       = NULL; /* XML data string */
    DRM_BOOL   fWork                = FALSE;
    _ETAGTYPE  eStatus              = eTagNone;
    DRM_CONST_STRING dstrParsed     = EMPTY_DRM_STRING;   /* temporary storage of parsed tag's data */

    if ( f_ptagParsed == NULL )
    {
        return eTagNone;
    }

    pwszBase = f_ptagParsed->m_pwszBase;
    dstrParsed.cchString  = f_ptagParsed->m_dasstrTag.m_cch;
    dstrParsed.pwszString = pwszBase + f_ptagParsed->m_dasstrTag.m_ich;

    /*
    ** If tag name is empty or if it matches name of a parsed tag
    ** we will need to compare attribute and value.
    ** Otherwise tag names don't match, and check failed.
    */
    fWork = ( f_pdstrTag == NULL
           || f_pdstrTag->cchString  == 0
           || f_pdstrTag->pwszString == NULL );

    fWork = fWork || DRM_UTL_DSTRStringsEqual( f_pdstrTag, &dstrParsed );

    if ( fWork )
    {
        if ( f_ptagParsed->m_eTagType == eCloseTag )
        {
            eStatus = eCloseTag; /* don't compare attribute and value for closing tags like </tagname> */
        }
        else if ( f_pdstrAttrName  == NULL
               || f_pdstrAttrName->pwszString == NULL
               || f_pdstrAttrName->cchString == 0 )
        {
            eStatus = f_ptagParsed->m_eTagType; /* empty attribute name is ignored */
        }
        else
        {
            /*
            ** check the attribute and its value, return eTagNone if value is not provided or does not match
            */
            dstrParsed.cchString  = f_ptagParsed->m_dasstrAttrName.m_cch;
            dstrParsed.pwszString = pwszBase + f_ptagParsed->m_dasstrAttrName.m_ich;

            if ( DRM_UTL_DSTRStringsEqual( &dstrParsed, f_pdstrAttrName ) )
            {

                dstrParsed.cchString  = f_ptagParsed->m_dasstrAttrValue.m_cch;
                dstrParsed.pwszString = pwszBase + f_ptagParsed->m_dasstrAttrValue.m_ich;

                if ( f_pdstrAttrValue != NULL
                    && DRM_UTL_DSTRStringsEqual( &dstrParsed, f_pdstrAttrValue ) )
                {
                    eStatus = f_ptagParsed->m_eTagType;
                }
            }
        }
    }

    return eStatus;
}


static DRM_NO_INLINE _ETAGTYPE _CheckTagA(
    __in const DRM_CHAR              *f_pszBase,
    __in const _XMLTAGA              *f_ptagParsed,
    __in const DRM_CHAR              *f_pszBaseTag,
    __in const DRM_SUBSTRING         *f_pdasstrTag,
    __in const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __in const DRM_ANSI_CONST_STRING *f_pdasstrAttrValue )
{
    DRM_BOOL      fWork       = FALSE;
    _ETAGTYPE     eStatus     = eTagNone;

    fWork = f_pdasstrTag->m_cch == 0
         || f_pszBaseTag        == NULL;

    fWork = (fWork
          || DRM_UTL_DASSSTRStringsEqual( f_pszBase,
                                         &f_ptagParsed->m_dasstrTag,
                                          f_pszBaseTag,
                                          f_pdasstrTag ) );

    if (fWork)
    {
        if ( f_ptagParsed->m_eTagType == eCloseTag )
        {
            eStatus = eCloseTag;
        }

        /* attribute is ignored */
        else if ( f_pdastrAttrName            == NULL
             ||   f_pdastrAttrName->cchString == 0 )
        {
            eStatus = f_ptagParsed->m_eTagType;
        }

        /*
        ** Check the attribute and its value. Note that this only checks
        ** for the first attribute and may not work correctly if an element
        ** has multiple attributes unless _ScanNodeForAttributeA is used
        ** to find the required attribute when the check against the
        ** first attribute fails.
        */

        else if ( DRM_UTL_DASSTRStringsEqual(  f_pszBase,
                                              &f_ptagParsed->m_dasstrAttrName,
                                               f_pdastrAttrName )
              && DRM_UTL_DASSTRStringsEqual(  f_pszBase,
                                             &f_ptagParsed->m_dasstrAttrValue,
                                              f_pdasstrAttrValue ) )
        {
                eStatus = f_ptagParsed->m_eTagType;
        }
    }

    return eStatus;
}

/**********************************************************************************************************
** Function  :    _GetXMLSubNodeW
**
** Synopsis  :    Scans XML string and looks for XML node under XML tag with given name and attribute data.
**                Caller can specify how many XML hierarchy layers to skip.
**
** Arguments :
**    [f_pdstrXML]         : pointer into XML string data, not necessarily the beginning
**    [f_pdstrTag]         : pointer to XML tag name
**    [f_pdstrAttrName]    : pointer to XML tag's attribute name, may be NULL
**    [f_pdstrAttrValue]   : pointer to XML tag's attribute value, may be NULL
**    [f_iNode]            : look for f_iNode-th occurence of XML tag in f_pdstrXML
                             on the same hierarchy level; 0 means the first occurence.
**    [f_pdstrNodeOut]     : pointer to XML node under found XML tag,
**                           either f_pdstrNodeOut or f_pdstrNodeDataOut can be NULL but not both
**    [f_pdstrNodeDataOut] : pointer to XML node's data under found XML tag
**                           either f_pdstrNodeOut or f_pdstrNodeDataOut can be NULL but not both
**    [f_cLayersToSkip]    : number of XML hierarchy levels to skip down before beginning to match XML tags
**
**   Returns               : DRM_RESULT
**
**********************************************************************************************************/
static DRM_NO_INLINE DRM_RESULT _GetXMLSubNodeW(
    __in     const DRM_CONST_STRING *f_pdstrXML,
    __in     const DRM_CONST_STRING *f_pdstrTag,
    __in_opt const DRM_CONST_STRING *f_pdstrAttrName,
    __in_opt const DRM_CONST_STRING *f_pdstrAttrValue,
    __in     const DRM_DWORD         f_iNode,
    __out_opt      DRM_CONST_STRING *f_pdstrNodeOut,
    __out_opt      DRM_CONST_STRING *f_pdstrNodeDataOut,
    __in     const DRM_DWORD         f_cLayersToSkip )
{
    DRM_RESULT dr               = DRM_E_NO_XML_OPEN_TAG;
    DRM_DWORD  cNodes           = 0;
    DRM_DWORD  ichCurr          = 0;
    DRM_DWORD  cTagsUnmatched   = 0;   /* counts the balance of opening and closing tags,
                                          use this counter to skip XML hierarchy levels */
    DRM_DWORD  cTagsMatched     = 0;   /* balance of tags that passed _CheckTagW() */
    DRM_DWORD  ichNodeBegin     = 0;
    DRM_DWORD  ichNodeEnd       = 0;
    DRM_DWORD  ichDataBegin     = 0;
    DRM_DWORD  ichDataEnd       = 0;
    _ETAGTYPE  eStatus          = eTagNone;
    _XMLTAGW   xmltagW          = { eTagNone, 0 };
    DRM_CONST_STRING dstrNode   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNodeData = EMPTY_DRM_STRING;
    DRM_BOOL fUseXMLHash        = FALSE;
    DRM_XML_HASH_CONTEXT *poHashContext = NULL;
    DRM_XML_HASH_SIG oSig       = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLPARSER, PERF_FUNC__GetXMLSubNodeW );
    DRM_PROFILING_DETAIL_LEVEL(6);

    ChkDRMString( f_pdstrXML );
    ChkDRMString( f_pdstrTag );

    ChkArg( f_pdstrNodeOut    != NULL
        || f_pdstrNodeDataOut != NULL );

    fUseXMLHash = ( DRM_XMLHASH_IsXmlHashSupported()
                 && ( f_pdstrAttrName  == NULL )
                 && ( f_pdstrAttrValue == NULL ) );

    if ( fUseXMLHash )
    {
        ChkDR( DRM_XMLHASH_GetContext( &poHashContext ) );

        if ( poHashContext != NULL )
        {
            ChkDR( DRM_XMLHASH_CalcParamSig( poHashContext,
                                             f_pdstrXML,
                                             f_pdstrTag,
                                             f_iNode,
                                             f_cLayersToSkip,
                                             &oSig ) );

            ChkDR( DRM_XMLHASH_Lookup( poHashContext,
                                       &oSig,
                                       f_pdstrXML,
                                       f_pdstrTag,
                                       &dstrNode,
                                       &dstrNodeData ) );

            if ( dr == DRM_SUCCESS )
            {
                goto SuccessExit;
            }
        }
    }

    while ( TRUE )
    {
        /*
        ** scan for tag at currIndex
        */
        if ( !_ScanTagW( f_pdstrXML, &ichCurr, &xmltagW ) )
        {
            if ( cTagsMatched == 0 )
            {
                ChkDR( DRM_E_NO_XML_OPEN_TAG );
            }
            else
            {
                ChkDR( DRM_E_NO_XML_CLOSE_TAG );
            }
        }

        /*
        ** skip XML hierarchy levels
        */
        if ( cTagsUnmatched != f_cLayersToSkip )
        {
            if ( xmltagW.m_eTagType == eOpenTag )
            {
                cTagsUnmatched++;
            }
            else if ( xmltagW.m_eTagType == eCloseTag )
            {
                if ( cTagsUnmatched > 0 )
                {
                    cTagsUnmatched--;
                }
                else
                {
                    ChkDR( DRM_E_NO_XML_OPEN_TAG );
                }
            }
            continue;
        }

        /*
        ** check if we have a match to the node tag
        */
        eStatus = _CheckTagW( &xmltagW,
                               f_pdstrTag,
                               f_pdstrAttrName,
                               f_pdstrAttrValue );

        switch ( eStatus )
        {
        case eOpenTag:     /* open tag found */
            /*
            ** this is the first matched open tag found.
            ** if cTagsMatched > 0 it means the matched opend tag is nested.
            */
            if ( cTagsMatched == 0 )
            {
                ichNodeBegin = xmltagW.m_ichTagBegin;
                ichDataBegin = xmltagW.m_ichTagEnd + 1;
            }
            cTagsMatched++;
            break;

        case eCloseTag:    /* close tag found */
            /*
            ** we get </tag> before we get <tag>
            */
            if ( cTagsMatched == 0 )
            {
                ChkDR( DRM_E_NO_XML_OPEN_TAG );
            }
            cTagsMatched--;

            if ( cTagsMatched == 0 )  /* make sure it is not nested */
            {
                /*
                ** we've got the matching <tag> and </tag>,
                ** exit the loop if this is the node we want
                */
                if ( cNodes == f_iNode )
                {
                    ichNodeEnd = xmltagW.m_ichTagEnd;
                    ichDataEnd = xmltagW.m_ichTagBegin - 1;
                    goto PREPARENODE;
                }
                ichNodeBegin = 0;
                ichDataBegin = 0;
                cNodes++;
            }
            break;

        case eEmptyTag:
            if ( cTagsMatched == 0 )
            {
                /*
                ** we've got the matching <tag/>,
                ** exit the loop if this is the node we want
                */
                if ( cNodes == f_iNode )
                {
                    ichNodeBegin = xmltagW.m_ichTagBegin;
                    ichNodeEnd   = xmltagW.m_ichTagEnd;
                    ichDataBegin = 0;
                    ichDataEnd   = 0;
                    goto PREPARENODE;
                }
                ichNodeBegin = 0;
                cNodes++;
            }
            break;

        default:   /* eTagNone or eCDataSection */
            if ( xmltagW.m_eTagType == eOpenTag )
            {
                cTagsUnmatched++;
            }
            else if ( xmltagW.m_eTagType == eCloseTag )
            {
                if ( cTagsUnmatched > f_cLayersToSkip )
                {
                    --cTagsUnmatched;
                }
                else
                {
                    /*
                    ** end of 'f_cLayersToSkip' layers exits, the target tag is not found
                    */
                    ChkDR( DRM_E_NO_XML_OPEN_TAG );
                }
            }
            break;
        }
    }

PREPARENODE:
    /*
    ** prepare f_pdstrNodeOut & f_pdstrNodeDataOut
    */
    dstrNode.pwszString = f_pdstrXML->pwszString + ichNodeBegin;
    dstrNode.cchString  = (DRM_DWORD) ( ichNodeEnd - ichNodeBegin + 1 );

    if ( eStatus == eEmptyTag )
    {
        dstrNodeData.pwszString = NULL;
        dstrNodeData.cchString  = 0;
    }
    else
    {
        dstrNodeData.pwszString = f_pdstrXML->pwszString + ichDataBegin;
        dstrNodeData.cchString = (DRM_DWORD) ( ichDataEnd - ichDataBegin + 1 );

        /*
        ** trim space in node data, if any
        */
        _AllTrimW( &dstrNodeData );
    }
    dr = DRM_SUCCESS;

    if ( fUseXMLHash && poHashContext != NULL )
    {
        ChkDR( DRM_XMLHASH_Update( poHashContext,
                                   &oSig,
                                   f_pdstrXML,
                                   &dstrNode,
                                   &dstrNodeData ) );
    }

SuccessExit:
    if ( f_pdstrNodeOut != NULL )
    {
        f_pdstrNodeOut->pwszString = dstrNode.pwszString;
        f_pdstrNodeOut->cchString = dstrNode.cchString;
    }

    if ( f_pdstrNodeDataOut != NULL )
    {
        f_pdstrNodeDataOut->pwszString = dstrNodeData.pwszString;
        f_pdstrNodeDataOut->cchString = dstrNodeData.cchString;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


static DRM_NO_INLINE DRM_RESULT _GetXMLSubNodeA(
    __in      const DRM_CHAR              *f_pszXMLBase,
    __in      const DRM_SUBSTRING         *f_pdasstrXML,
    __in      const DRM_CHAR              *f_pszTagBase,
    __in      const DRM_SUBSTRING         *f_pdasstrTag,
    __in_opt  const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __in_opt  const DRM_ANSI_CONST_STRING *f_pdasstrAttrValue,
    __in      const DRM_DWORD              f_iNode,
    __out_opt       DRM_SUBSTRING         *f_pdasstrNode,     /* these two parameters can either be NULL but not both */
    __out_opt       DRM_SUBSTRING         *f_pdasstrNodeData,
    __in      const DRM_DWORD              f_iLayer,
    __in            DRM_BOOL               f_fSkipValidation )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD     cTagsMatched   = 0;
    DRM_DWORD     cNodes         = 0;
    DRM_DWORD     cTagsUnmatched = 0;   /* increment until we find the matching tag */
    DRM_DWORD     ichNodeBegin   = 0;
    DRM_DWORD     ichDataBegin   = 0;
    DRM_DWORD     ichNodeEnd     = 0;
    DRM_DWORD     ichDataEnd     = 0;
    DRM_DWORD     ichCurr        = f_pdasstrXML->m_ich;
    _ETAGTYPE     eStatus        = eTagNone;
    _XMLTAGA      xmltag         = { eTagNone, 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMXMLPARSER, PERF_FUNC__GetXMLSubNodeA);
    DRM_PROFILING_DETAIL_LEVEL(6);

    ChkArg( f_pszXMLBase != NULL );
    ChkArg( f_pszTagBase != NULL );
    ChkArg(f_pdasstrXML != NULL );
    ChkArg( f_pdasstrTag != NULL);
    ChkArg(f_pdasstrNode != NULL || f_pdasstrNodeData != NULL);

    while (TRUE)
    {
        /* scan for tag at currIndex */
        if (! _ScanTagA(f_pszXMLBase, f_pdasstrXML, &ichCurr, &xmltag))
        {
            if (cTagsMatched == 0)
            {
                ChkDR(DRM_E_NO_XML_OPEN_TAG);
            }
            else
            {
                ChkDR(DRM_E_NO_XML_CLOSE_TAG);
            }
        }

        /* check if we have a match to the node tag */
        if (cTagsUnmatched != f_iLayer)
        {
            if (xmltag.m_eTagType == eOpenTag)
            {
                cTagsUnmatched++;
            }
            else if (xmltag.m_eTagType == eCloseTag)
            {
                if (cTagsUnmatched > 0)
                {
                    cTagsUnmatched--;
                }
                else
                {
                    ChkDR(DRM_E_NO_XML_OPEN_TAG);
                }
            }
            continue;
        }

        eStatus = _CheckTagA(f_pszXMLBase,
                             &xmltag,
                             f_pszTagBase,
                             f_pdasstrTag,
                             f_pdastrAttrName,
                             f_pdasstrAttrValue);
        switch (eStatus)
        {
        case eOpenTag:     /* open tag found */
            /* this is the first matched open tag found. if cTagsMatched>0, it
            ** means the matched opend tag is nested.
            */
            if (cTagsMatched == 0)
            {
                ichNodeBegin = xmltag.m_ichTagBegin;
                ichDataBegin = xmltag.m_ichTagEnd + 1;
                if ( f_fSkipValidation
                    && ( cNodes == f_iNode ) )
                {
                    ichNodeEnd = f_pdasstrXML->m_ich + f_pdasstrXML->m_cch - 1;
                    ichDataEnd = f_pdasstrXML->m_ich + f_pdasstrXML->m_cch - 1;
                    goto PREPARENODE;
                }
            }
            cTagsMatched++;
            break;

        case eCloseTag:    /* close tag found */
            if (cTagsMatched == 0)
            {
                /* we get </tag> before we get <tag> */
                ChkDR(DRM_E_NO_XML_OPEN_TAG);
            }

            cTagsMatched--;

            if (cTagsMatched == 0)  /* make sure it is not nested */
            {
                /* we've got the matching <tag> and </tag>,
                ** exit the loop if this is the node we want
                */
                if (cNodes == f_iNode)
                {
                    ichNodeEnd = xmltag.m_ichTagEnd;
                    ichDataEnd = xmltag.m_ichTagBegin - 1;
                    goto PREPARENODE;
                }
                ichNodeBegin = 0;
                ichDataBegin = 0;
                cNodes++;
            }
            break;

        case eEmptyTag:
            if (cTagsMatched == 0)
            {
                /* we've got the matching <tag/>,
                ** exit the loop if this is the node we want
                */
                if (cNodes == f_iNode)
                {
                    ichNodeBegin = xmltag.m_ichTagBegin;
                    ichNodeEnd   = xmltag.m_ichTagEnd;
                    ichDataBegin = 0;
                    ichDataEnd   = 0;
                    goto PREPARENODE;
                }
                ichNodeBegin = 0;
                cNodes++;
            }
            break;

        default:
            if (xmltag.m_eTagType == eOpenTag)
            {
                cTagsUnmatched++;
            }
            else if (xmltag.m_eTagType == eCloseTag)
            {
                if (cTagsUnmatched > f_iLayer)
                {
                    cTagsUnmatched--;
                }
                else
                {
                    /* end of 'f_cLayers' layers exits, the target tag is not found */
                    ChkDR(DRM_E_NO_XML_OPEN_TAG);
                }
            }
            break;
        }
    }

PREPARENODE:
    /* prepare f_pdstrNode & f_pdstrNodeData */
    if (f_pdasstrNode != NULL)
    {
        f_pdasstrNode->m_ich = ichNodeBegin;
        f_pdasstrNode->m_cch = (ichNodeEnd - ichNodeBegin + 1);
    }

    if (f_pdasstrNodeData != NULL)
    {
        if (eStatus == eEmptyTag)
        {
            f_pdasstrNodeData->m_cch = 0;
            f_pdasstrNodeData->m_ich = 0;
        }
        else
        {
            f_pdasstrNodeData->m_ich = ichDataBegin;
            f_pdasstrNodeData->m_cch = (ichDataEnd - ichDataBegin + 1);

            _AllTrimA(f_pszXMLBase, f_pdasstrNodeData);
        }
    }
    dr = DRM_SUCCESS;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_XML_TranslateXMLError(dr);
}


/*********************************************************************
** Function  : _GetXMLNodeCDataA
**
** Synopsis  : returns data from CDATA node,
**             may return zero-length substring
**
** Arguments :
**    [f_pszXMLBase]   : string with XML data
**    [f_pdasstrXML]   : points to XML node with CDATA,
**                       cannot be NULL, nor point to zero-length substring
**    [f_pdasstrCData] : returned CDATA string, must not be NULL
**
** Returns   : DRM_RESULT
**
** Notes     : The function allows one opening tag before CDATA tag.
**             If it finds two opening tags or tag of other types
**             it returns error.
*********************************************************************/
static DRM_NO_INLINE DRM_RESULT _GetXMLNodeCDataA(
    __in  const DRM_CHAR      *f_pszXMLBase,
    __in  const DRM_SUBSTRING *f_pdasstrXML,
    __out       DRM_SUBSTRING *f_pdasstrCData )
{
    DRM_RESULT dr            = DRM_E_NO_XML_CDATA;
    DRM_BOOL   fOpenTagFound = FALSE;
    DRM_DWORD  ichCurr       = 0;
    _XMLTAGA   xmltagA       = { eTagNone, 0 };

    ChkArg( f_pszXMLBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_pdasstrCData != NULL );

    ichCurr = f_pdasstrXML->m_ich;

    while ( TRUE )
    {
        /*
        ** scan for tag at currIndex
        */
        if ( !_ScanTagA( f_pszXMLBase, f_pdasstrXML, &ichCurr, &xmltagA ) )
        {
            if ( !fOpenTagFound )
            {
                ChkDR( DRM_E_NO_XML_OPEN_TAG );      /* open tag not found yet */
            }
            else
            {
                ChkDR( DRM_E_NO_XML_CDATA );        /* could not find CDATA */
            }
        }

        switch ( xmltagA.m_eTagType )
        {
            case eTagNone:
            case eCloseTag:
            case eEmptyTag:
                ChkDR( DRM_E_NO_XML_CDATA );    /* could not find CDATA */
                __fallthrough;

            case eOpenTag:
                if ( fOpenTagFound )
                {
                    ChkDR( DRM_E_INVALID_XML_TAG ); /* an open tag found already */
                }

                fOpenTagFound = TRUE;
                break;

            case eCDataSection:
                if ( !fOpenTagFound )
                {
                    ChkDR( DRM_E_NO_XML_OPEN_TAG ); /* an open tag not found yet */
                }

                f_pdasstrCData->m_cch  = xmltagA.m_dasstrCData.m_cch;
                f_pdasstrCData->m_ich  = xmltagA.m_dasstrCData.m_ich;

                dr = ( _AllTrimA( f_pszXMLBase, f_pdasstrCData )
                      ? DRM_SUCCESS
                      : DRM_E_NO_XML_CDATA );

                goto ErrorExit;
        }
    }

ErrorExit:
    return dr;
}


/*********************************************************************
** Function  : _GetXMLNodeCDataW
**
** Synopsis  : .
**
**
** Arguments :
**    [f_pdstrNode]     :   pointer to XML data string with CDATA node.
**                          must be not NULL, and string should be not empty.
**    [f_pdstrCData]    :   returned CDATA string, must not be NULL.
**
** Returns   : DRM_RESULT
**
** Notes     : The function allows one opening tag before CDATA tag.
**             If it finds two opening tags or tag of other types
**             it returns error.
*********************************************************************/
static DRM_NO_INLINE DRM_RESULT _GetXMLNodeCDataW(
    __in const DRM_CONST_STRING *f_pdstrNode,
    __out      DRM_CONST_STRING *f_pdstrCData )
{
    DRM_RESULT dr            = DRM_E_NO_XML_CDATA;
    DRM_BOOL   fOpenTagFound = FALSE;
    DRM_DWORD  ichCurr       = 0;
    _XMLTAGW   xmltagW       = { eTagNone, 0 };

    ChkDRMString( f_pdstrNode );

    ChkArg( f_pdstrCData != NULL );

    while ( TRUE )
    {
        /*
        ** scan for tag at currIndex
        */
        if ( !_ScanTagW( f_pdstrNode, &ichCurr, &xmltagW ) )
        {
            if ( !fOpenTagFound )
            {
                ChkDR( DRM_E_NO_XML_OPEN_TAG );      /* open tag not found yet */
            }
            else
            {
                ChkDR( DRM_E_NO_XML_CDATA );        /* could not find CDATA */
            }
        }

        switch ( xmltagW.m_eTagType )
        {
            case eTagNone:
            case eCloseTag:
            case eEmptyTag:
                ChkDR( DRM_E_NO_XML_CDATA );    /* could not find CDATA */
                __fallthrough;

            case eOpenTag:
                if ( fOpenTagFound )
                {
                    ChkDR( DRM_E_INVALID_XML_TAG ); /* an open tag found already */
                }

                fOpenTagFound = TRUE;
                break;

            case eCDataSection:
                if ( !fOpenTagFound )
                {
                    ChkDR( DRM_E_NO_XML_OPEN_TAG ); /* an open tag not found yet */
                }

                f_pdstrCData->cchString  = xmltagW.m_dasstrCData.m_cch;
                f_pdstrCData->pwszString = f_pdstrNode->pwszString + xmltagW.m_dasstrCData.m_ich;

                dr = ( _AllTrimW( f_pdstrCData )
                      ? DRM_SUCCESS
                      : DRM_E_NO_XML_CDATA );

                goto ErrorExit;
        }
    }

ErrorExit:
    return dr;
}


/**************************************************************************************************
** Function  :    _EnumXMLSubNodeW
**
** Synopsis  :    In the given XML string retrieves the next node either with a specified tag name
**                (if f_pdstrTag is not empty) or with just any node.
**
**                Function can be used by XML nodes enumerator.
**                Caller can specify how many XML hierarchy layers to skip.
**
** Arguments :
**    [f_pdstrXML]          : pointer to XML string data
**                            can point to some place in the middle of XML data
**    [f_pdstrTag]          : pointer to XML tag name, cannot be NULL, may be empty string.
**    [f_pdstrNode]         : pointer to XML node under found XML tag,
**                            either f_pdstrNodeOut or f_pdstrNodeDataOut can be NULL but not both
**    [f_pdstrNodeData]     : pointer to XML node's data under found XML tag
**    [f_pdstr1stAttrName]  : pointer to XML tag's attribute name, may be NULL
**    [f_pdstr1stAttrValue] : pointer to XML tag's attribute value, may be NULL
**    [f_pfIsLeaf]          : will be set to TRUE of the returned node is a leaf node
**    [f_cLayers]           : number of XML hierarchy levels to skip down
**
** Returns   : DRM_RESULT
****************************************************************************************************/
static DRM_RESULT _EnumXMLSubNodeW(
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in       DRM_CONST_STRING *f_pdstrTag,
    __out_opt  DRM_CONST_STRING *f_pdstrNode,
    __out_opt  DRM_CONST_STRING *f_pdstrNodeData,
    __out_opt  DRM_CONST_STRING *f_pdstr1stAttrName,
    __out_opt  DRM_CONST_STRING *f_pdstr1stAttrValue,
    __out      DRM_BOOL         *f_pfIsLeaf,
    __in const DRM_DWORD         f_cLayers )
{
    DRM_RESULT       dr               = DRM_E_NO_XML_OPEN_TAG;
    DRM_DWORD        cTagsMatched     = 0;
    DRM_DWORD        ichNodeBegin     = 0,
                     ichDataBegin     = 0,
                     ichNodeEnd       = 0,
                     ichDataEnd       = 0,
                     ichCurr          = 0;
    DRM_DWORD        cTagsUnmatched   = 0;        /* increment until we find the matching tag */
    _ETAGTYPE        eStatus          = eTagNone;
    _XMLTAGW         xmltag           = { eTagNone, 0 };
    DRM_CONST_STRING dstr1stAttrName  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstr1stAttrValue = EMPTY_DRM_STRING;

    ChkDRMString( f_pdstrXML );
    ChkArg( f_pdstrTag != NULL );
    ChkArg( f_pfIsLeaf     != NULL );
    ChkArg( f_pdstrNode    != NULL
        || f_pdstrNodeData != NULL );

    *f_pfIsLeaf = TRUE;

    xmltag.m_pwszBase = f_pdstrXML->pwszString;

    while ( TRUE )
    {
        /*
        ** scan for tag at currIndex
        */
        if ( !_ScanTagW( f_pdstrXML, &ichCurr, &xmltag ) )
        {
            if ( cTagsMatched == 0 )
            {
                ChkDR( DRM_E_NO_XML_OPEN_TAG );
            }
            else
            {
                ChkDR( DRM_E_NO_XML_CLOSE_TAG );
            }
        }

        /*
        ** skip XML hierarchy levels
        */
        if ( cTagsUnmatched != f_cLayers )
        {
            if ( xmltag.m_eTagType == eOpenTag )
            {
                cTagsUnmatched++;
            }
            else if ( xmltag.m_eTagType == eCloseTag )
            {
                if ( cTagsUnmatched>0 )
                {
                    cTagsUnmatched--;
                }
                else
                {
                    ChkDR( DRM_E_NO_XML_OPEN_TAG );
                }
            }
            continue;
        }

        /*
        ** grab the tag:
        ** if f_pdstrTag is empty string then just return a tag's type,
        ** else only grab the tag with a name that matches f_pdstrTag
        */
        eStatus = _CheckTagW( &xmltag,
                              f_pdstrTag,
                              NULL,
                              NULL );

        switch ( eStatus )
        {
            case eOpenTag:
                /*
                ** this is the first matched open tag found. if cTagsMatched>0, it
                ** means the matched opend tag is nested.
                */
                if ( cTagsMatched == 0 )
                {
                    ichNodeBegin = xmltag.m_ichTagBegin;
                    ichDataBegin = xmltag.m_ichTagEnd + 1;

                    /*
                    ** assign the tag name so that we know what to look for closing tag
                    */
                    f_pdstrTag->cchString  = xmltag.m_dasstrTag.m_cch;
                    f_pdstrTag->pwszString = f_pdstrXML->pwszString + xmltag.m_dasstrTag.m_ich;

                    /*
                    ** save the attr name/value
                    */
                    dstr1stAttrName.cchString  = xmltag.m_dasstrAttrName.m_cch;
                    dstr1stAttrName.pwszString = f_pdstrXML->pwszString + xmltag.m_dasstrAttrName.m_ich;
                    dstr1stAttrValue.cchString  = xmltag.m_dasstrAttrValue.m_cch;
                    dstr1stAttrValue.pwszString = f_pdstrXML->pwszString + xmltag.m_dasstrAttrValue.m_ich;
                }
                else if ( *f_pfIsLeaf )
                {
                    *f_pfIsLeaf = FALSE;
                }
                cTagsMatched++;
                break;

            case eCloseTag:
                if (cTagsMatched == 0) /* we get </tag> before we get <tag> */
                {
                    ChkDR(DRM_E_NO_XML_OPEN_TAG);
                }
                cTagsMatched--;

                if ( cTagsMatched == 0 )  /* make sure it is not nested */
                {
                    /*
                    ** we've got the matching <tag> and </tag>,
                    ** exit the loop if this is the node we want
                    */
                    ichNodeEnd = xmltag.m_ichTagEnd;
                    ichDataEnd = xmltag.m_ichTagBegin - 1;
                    goto PREPARENODE;
                }
                break;

            case eEmptyTag:
                if ( cTagsMatched == 0 )
                {
                    /*
                    ** we've got the matching <tag/>,
                    ** exit the loop, this is the node we want
                    */
                    ichNodeBegin = xmltag.m_ichTagBegin;
                    ichDataBegin = 0;
                    ichNodeEnd   = xmltag.m_ichTagEnd;
                    ichDataEnd   = 0;

                    /*
                    ** assign the tag name so that we know what to look for closing tag
                    */
                    f_pdstrTag->cchString  = xmltag.m_dasstrTag.m_cch;
                    f_pdstrTag->pwszString = f_pdstrXML->pwszString + xmltag.m_dasstrTag.m_ich;
                    goto PREPARENODE;
                }
                break;

            default: /* eStatus is eTagNone or eCDataSection */
                if ( xmltag.m_eTagType == eOpenTag )
                {
                    cTagsUnmatched++;
                    *f_pfIsLeaf = FALSE;
                }
                else if ( xmltag.m_eTagType == eCloseTag )
                {
                    if ( cTagsUnmatched > f_cLayers )
                    {
                        cTagsUnmatched--;
                    }
                    else
                    {
                        /*
                        ** end of 'f_cLayers' layers exits, the target tag is not found
                        */
                        ChkDR( DRM_E_NO_XML_OPEN_TAG );
                    }
                }
                break;
        }
    }

PREPARENODE:
    /*
    ** prepare f_pdstrNode & f_pdstrNodeData
    */
    if ( f_pdstrNode != NULL )
    {
        f_pdstrNode->pwszString = f_pdstrXML->pwszString  + ichNodeBegin;
        f_pdstrNode->cchString  = (DRM_DWORD) (ichNodeEnd - ichNodeBegin + 1);
    }

    if (f_pdstrNodeData != NULL)
    {
        if (eStatus == eEmptyTag)
        {
            f_pdstrNodeData->pwszString = NULL;
            f_pdstrNodeData->cchString  = 0;
        }
        else
        {
            f_pdstrNodeData->pwszString = f_pdstrXML->pwszString  + ichDataBegin;
            f_pdstrNodeData->cchString  = (DRM_DWORD) (ichDataEnd - ichDataBegin + 1);

            if (! _AllTrimW(f_pdstrNodeData))
            {
                f_pdstrNodeData->pwszString = NULL;
            }
        }
    }

    if (f_pdstr1stAttrName != NULL)
    {
        *f_pdstr1stAttrName = dstr1stAttrName;
    }

    if (f_pdstr1stAttrValue != NULL)
    {
        *f_pdstr1stAttrValue = dstr1stAttrValue;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*
***********************************************************************
** API methods
***********************************************************************
*/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetNode(
    __in      const DRM_CONST_STRING *f_pdstrXML,
    __in      const DRM_CONST_STRING *f_pdstrTag,
    __in_opt  const DRM_CONST_STRING *f_pdstrAttrName,
    __in_opt  const DRM_CONST_STRING *f_pdstrAttrValue,
    __in      const DRM_DWORD         f_iNode,            /* nth occurence of the node in the xml stirng, start from 0 */
    __out_opt       DRM_CONST_STRING *f_pdstrNode,        /* either of these 2 parameter can be NULL but not both */
    __out_opt       DRM_CONST_STRING *f_pdstrNodeData )   /* data enclosed by the immediate <tag>...</tag> in the given XML string */
{
    return DRM_XML_GetSubNode(f_pdstrXML,
                              f_pdstrTag,
                              f_pdstrAttrName,
                              f_pdstrAttrValue,
                              f_iNode,
                              f_pdstrNode,
                              f_pdstrNodeData,
                              0);
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetNodeA(
    __in_ecount( f_pdasstrXML->m_ich + f_pdasstrXML->m_cch ) const DRM_CHAR              *f_pszBase,
    __in                                                     const DRM_SUBSTRING         *f_pdasstrXML,
    __in                                                     const DRM_ANSI_CONST_STRING *f_pdastrTag,
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdasstrAttrValue,
    __in                                                     const DRM_DWORD              f_iNode,               /* nth occurence of the node in the xml stirng, start from 0 */
    __out_opt                                                      DRM_SUBSTRING         *f_pdasstrXmlNode,      /* either of these 2 parameter can be NULL but not both */
    __out_opt                                                      DRM_SUBSTRING         *f_pdasstrXmlNodeData ) /* data enclosed by the immediate <tag>...</tag> in the given XML string */
{
    return DRM_XML_GetSubNodeA(f_pszBase,
                               f_pdasstrXML,
                               f_pdastrTag,
                               f_pdastrAttrName,
                               f_pdasstrAttrValue,
                               f_iNode,
                               f_pdasstrXmlNode,
                               f_pdasstrXmlNodeData,
                               0);
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetSubNode(
    __in      const DRM_CONST_STRING *f_pdstrXML,
    __in      const DRM_CONST_STRING *f_pdstrTag,
    __in_opt  const DRM_CONST_STRING *f_pdstrAttrName,
    __in_opt  const DRM_CONST_STRING *f_pdstrAttrValue,
    __in      const DRM_DWORD         f_iNode,          /* nth occurence of the node in the xml stirng, start from 0 */
    __out_opt       DRM_CONST_STRING *f_pdstrNode,      /* these two parameters can either be NULL but not both */
    __out_opt       DRM_CONST_STRING *f_pdstrNodeData,
    __in            DRM_DWORD         f_iLayer )        /* outermost layer is 0 */
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDRMString( f_pdstrXML );
    ChkDRMString( f_pdstrTag );

    ChkArg( f_pdstrNode     != NULL
         || f_pdstrNodeData != NULL );

    ChkDR(_GetXMLSubNodeW(f_pdstrXML,
                          f_pdstrTag,
                          f_pdstrAttrName,
                          f_pdstrAttrValue,
                          f_iNode,
                          f_pdstrNode,
                          f_pdstrNodeData,
                          f_iLayer));

ErrorExit:
    return DRM_XML_TranslateXMLError(dr);
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetSubNodeA(
    __in_ecount( f_pdasstrXML->m_ich + f_pdasstrXML->m_cch ) const DRM_CHAR              *f_pszBase,
    __in                                                     const DRM_SUBSTRING         *f_pdasstrXML,
    __in                                                     const DRM_ANSI_CONST_STRING *f_pdastrTag,
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdasstrAttrValue,
    __in                                                     const DRM_DWORD              f_iNode,           /* nth occurence of the node in the xml stirng, start from 0 */
    __out_opt                                                      DRM_SUBSTRING         *f_pdasstrNode,     /* these two parameters can either be NULL but not both */
    __out_opt                                                      DRM_SUBSTRING         *f_pdasstrNodeData,
    __in                                                           DRM_DWORD              f_iLayer )         /* outermost layer is 0 */
{
    DRM_RESULT    dr        = DRM_SUCCESS;
    DRM_SUBSTRING dasstrTag = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMXMLPARSER, PERF_FUNC_DRM_XML_GetSubNodeA);

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkDRMANSIString( f_pdastrTag );
    ChkArg( f_pdasstrNode != NULL || f_pdasstrNodeData != NULL );

    dasstrTag.m_cch = f_pdastrTag->cchString;

    ChkDR( _GetXMLSubNodeA( f_pszBase,
                            f_pdasstrXML,
                            f_pdastrTag->pszString,
                            &dasstrTag,
                            f_pdastrAttrName,
                            f_pdasstrAttrValue,
                            f_iNode,
                            f_pdasstrNode,     /* these two parameters can either be NULL but not both */
                            f_pdasstrNodeData,
                            f_iLayer,
                            FALSE ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
} /* DRM_XML_GetSubNodeA */


/**********************************************************************
** Function  :    DRM_XML_GetNodeAttribute
**
** Synopsis  :
**
**
**
** Arguments :
**    [f_pdstrNode]       : the topmost node in this XML string will be worked on
**    [f_pdstrAttrName]   : attribute name to retrieve
**    [f_pdstrAttrValue]  : returned attrValue
**
** Returns   : DRM_RESULT
************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetNodeAttribute(
    __in const DRM_CONST_STRING *f_pdstrNode,
    __in const DRM_CONST_STRING *f_pdstrAttrName,
    __out      DRM_CONST_STRING *f_pdstrAttrValue )
{
    DRM_RESULT dr     = DRM_E_INVALID_XML_TAG;
    _XMLTAGW   xmltag = { eTagNone, 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLPARSER, PERF_FUNC_DRM_XML_GetNodeAttribute );

    ChkDRMString( f_pdstrNode );
    ChkDRMString( f_pdstrAttrName );
    ChkArg( f_pdstrAttrValue != NULL );

    if ( _ScanNodeForAttributeW( f_pdstrNode, f_pdstrAttrName, &xmltag ) )
    {
        f_pdstrAttrValue->pwszString = f_pdstrNode->pwszString + xmltag.m_dasstrAttrValue.m_ich;
        f_pdstrAttrValue->cchString  = xmltag.m_dasstrAttrValue.m_cch;

        dr = DRM_SUCCESS;
    }
    else
    {
        goto ErrorExit;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_XML_TranslateXMLError( dr );
}


/**********************************************************************
** Function:    DRM_XML_GetNodeAttributeA
** Synopsis:
** Arguments:   [pdwStamp] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetNodeAttributeA(
    __in_ecount( f_pdasstrNode->m_ich + f_pdasstrNode->m_cch ) const DRM_CHAR              *f_pszBase,
    __in                                                       const DRM_SUBSTRING         *f_pdasstrNode,
    __in                                                       const DRM_ANSI_CONST_STRING *f_pdastrAttrName,    /* attrName to retrieve */
    __out                                                            DRM_SUBSTRING         *f_pdasstrAttrValue ) /* returned attrValue */
{
    DRM_RESULT dr = DRM_E_INVALID_XML_TAG;
    _XMLTAGA   xmltag = { eTagNone, 0 };

    ChkArg(f_pszBase          != NULL);
    ChkArg(f_pdasstrAttrValue != NULL);

    ChkArg(f_pdasstrNode       != NULL
        && f_pdasstrNode->m_cch > 0);

    ChkArg(f_pdastrAttrName            != NULL
        && f_pdastrAttrName->pszString != NULL
        && f_pdastrAttrName->cchString  > 0);

    if (_ScanNodeForAttributeA(f_pszBase, f_pdasstrNode, f_pdastrAttrName, &xmltag))
    {
        f_pdasstrAttrValue->m_cch = xmltag.m_dasstrAttrValue.m_cch;
        f_pdasstrAttrValue->m_ich = xmltag.m_dasstrAttrValue.m_ich;

        dr = DRM_SUCCESS;
    }

ErrorExit:
    return DRM_XML_TranslateXMLError(dr);
}


/**********************************************************************
** Function:    DRM_XML_GetNodeCDataA
**
** Synopsis:    API for retrieving CDATA data from XML tag.
**
** Arguments:
**    [f_pszXMLBase]   : string with XML data
**    [f_pdasstrXML]   : points to XML node with CDATA,
**                       cannot be NULL, nor point to zero-length substring
**    [f_pdasstrCData] : returned CDATA string, must not be NULL.
**
** Returns             : DRM_RESULT
**
***********************************************************************
*/
DRM_API DRM_RESULT DRM_CALL DRM_XML_GetNodeCDataA(
    __in_ecount( f_pdasstrXML->m_ich + f_pdasstrXML->m_cch ) const DRM_CHAR      *f_pszXMLBase,
    __in                                                     const DRM_SUBSTRING *f_pdasstrXML,
    __out                                                          DRM_SUBSTRING *f_pdasstrCData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pszXMLBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_pdasstrCData != NULL );

    ChkDR( _GetXMLNodeCDataA( f_pszXMLBase,
                              f_pdasstrXML,
                              f_pdasstrCData ) );

ErrorExit:
    return DRM_XML_TranslateXMLError( dr );
}


/**********************************************************************
** Function:    DRM_XML_GetNodeCData
** Synopsis:
** Arguments:   [f_pdstrNode] --
**              [f_pdstrCData] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetNodeCData(
    __in const DRM_CONST_STRING *f_pdstrNode,
    __out      DRM_CONST_STRING *f_pdstrCData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDRMString( f_pdstrNode );
    ChkArg( f_pdstrCData != NULL );

    ChkDR( _GetXMLNodeCDataW( f_pdstrNode, f_pdstrCData ) );

ErrorExit:
    return DRM_XML_TranslateXMLError( dr );
}




/*******************************************************************************************
** Function  :    DRM_XML_GetSubNodeByPath
**
** Synopsis  :    Retrieves a node in XML string
**                given a "path" to a node down the XML hierarchy levels
**                that is the node's name with its parents' names.
**                Path looks like "node1/node2/.../nodeN",
**                and character separator can be any DRM_WCHAR
**
** Arguments :
**   [f_pdstrXML]         : XML string data
**   [f_pdstrXMLNode]     : Path to a node with its parents' names
**   [f_pdstrAttrName]    : Attribute name of the last XML node in the path, may be NULL
**   [f_pdstrAttrValue]   : Attribute value of the last XML node in the path, may be NULL
**   [f_pdstrXMLNodeOut]  : Output XML node, may be NULL
**   [f_pdstrXMLDataOut]  : Output XML node's data under found XML tag, may be NULL
**   [f_chSeparator]      : Character that separates nodes' names in the path
**
** Returns   : DRM_RESULT
*******************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_GetSubNodeByPath(
    __in      const DRM_CONST_STRING *f_pdstrXML,
    __in      const DRM_CONST_STRING *f_pdstrXMLNode,
    __in_opt  const DRM_CONST_STRING *f_pdstrAttrName,
    __in_opt  const DRM_CONST_STRING *f_pdstrAttrValue,
    __out_opt       DRM_CONST_STRING *f_pdstrXMLNodeOut,
    __out_opt       DRM_CONST_STRING *f_pdstrXMLDataOut,
    __in            DRM_WCHAR         f_chSeparator )
{
    DRM_RESULT       dr           = DRM_E_LOGICERR;
    const DRM_WCHAR* pch          = NULL;
    DRM_DWORD        cch          = 0;
    DRM_CONST_STRING dstrXml      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNode     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNodeName = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMXMLPARSER, PERF_FUNC_DRM_XML_GetSubNodeByPath );

    ChkDRMString( f_pdstrXML );
    ChkDRMString( f_pdstrXMLNode );

    ASSIGN_DRM_STRING( dstrXml, *f_pdstrXML );
    while( cch < f_pdstrXMLNode->cchString )
    {
        /*
        ** Find separator
        */
        pch = f_pdstrXMLNode->pwszString + cch;
        while( f_pdstrXMLNode->pwszString [cch] != f_chSeparator
            &&  cch  < f_pdstrXMLNode->cchString )
        {
            cch++;
        }

        dstrNodeName.pwszString = pch;
        dstrNodeName.cchString  = cch - ( (DRM_DWORD)( pch - f_pdstrXMLNode->pwszString ) );

        if ( *( f_pdstrXMLNode->pwszString + cch ) != f_chSeparator )
        {
            /*
            ** We are on the last segment.
            */
            if ( DRM_UTL_DSTRStringsEqual( &g_dstrTagCDATA, &dstrNodeName ) )
            {
                /*
                ** Requesting the CDATA section.  Ask for that.
                */
                ChkArg( f_pdstrXMLDataOut != NULL );
                ChkDR( DRM_XML_GetNodeCData( &dstrXml, f_pdstrXMLDataOut ) );
            }
            else
            {
                /*
                ** Just get the node and use the attributes parameters
                */
                ChkDR( DRM_XML_GetSubNode( &dstrXml,
                                           &dstrNodeName,
                                           f_pdstrAttrName,
                                           f_pdstrAttrValue,
                                           0,
                                           &dstrNode,
                                           f_pdstrXMLDataOut,
                                           1 ) );
            }
        }
        else
        {
            ChkDR( DRM_XML_GetSubNode( &dstrXml,
                                       &dstrNodeName,
                                       NULL,
                                       NULL,
                                       0,
                                       &dstrNode,
                                       f_pdstrXMLDataOut,
                                       1 ) );
        }

        ASSIGN_DRM_STRING( dstrXml, dstrNode );
        cch++;
    }
    if ( f_pdstrXMLNodeOut != NULL )
    {
        ASSIGN_DRM_STRING( *f_pdstrXMLNodeOut, dstrXml );
    }

ErrorExit:
    DRMASSERT( dr != DRM_E_LOGICERR );
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_XML_TranslateXMLError( dr );
}


/***********************************************************************************************************************
** Function  :    DRM_XML_GetSubNodeByPathA
**
** Synopsis  :    Function retrieves a node in XML string given a "path inside XML tree"
**                from the current position to the node,
**                that is the node's name with its parent nodes' names
**                starting from the node's name which is a child to the current one.
**                Path string looks like: "node1/node2/node3/node4".
**
** Arguments :
**    [f_pszBase]          : XML string data
**    [f_pdasstrXML]       : Offset into XML string data
**    [f_pdstrXMLNode]     : Path to a node with its parents' names
**    [f_pdstrAttrName]    : Attribute name of the last XML node in the path, may be NULL
**    [f_pdstrAttrValue]   : Attribute value of the last XML node in the path, may be NULL.
**                           These two arguments are needed if on the same level
**                           there are a few nodes with the same name.
**    [f_pdstrXMLNodeOut]  : Pointer to output XML node string, may be NULL.
**    [f_pdstrXMLDataOut]  : Pointer to output XML node's data, may be NULL.
**    [f_chSeparator]      : Character that separates nodes' names in the path string.
**
** Returns   : DRM_RESULT
***********************************************************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_GetSubNodeByPathA(
    __in_ecount( f_pdasstrXML->m_ich + f_pdasstrXML->m_cch ) const DRM_CHAR              *f_pszBase,
    __in                                                     const DRM_SUBSTRING         *f_pdasstrXML,
    __in                                                     const DRM_ANSI_CONST_STRING *f_pdastrNodePath,
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdastrAttrName,
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdastrAttrValue,
    __out_opt                                                      DRM_SUBSTRING         *f_pdasstrXMLNodeOut,
    __out_opt                                                      DRM_SUBSTRING         *f_pdasstrXMLDataOut,
    __in                                                           DRM_CHAR               f_chSeparator )
{
    DRM_RESULT    dr                 = DRM_E_LOGICERR;
    DRM_DWORD     ichCurrNodeBegin   = 0;                 /* start of current node name */
    DRM_DWORD     ichCurr            = 0;
    DRM_SUBSTRING dasstrCurrNodeName = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrXml          = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrNode         = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkDRMANSIString( f_pdastrNodePath );

    dasstrXml.m_ich = f_pdasstrXML->m_ich;
    dasstrXml.m_cch = f_pdasstrXML->m_cch;

    while( ichCurr < f_pdastrNodePath->cchString )
    {
        /*
        ** Find separator or the end of path string
        */
        while( GET_CHAR( f_pdastrNodePath->pszString, ichCurr ) != f_chSeparator
            &&  ichCurr < f_pdastrNodePath->cchString )
        {
            ichCurr++;
        }
        dasstrCurrNodeName.m_ich = ichCurrNodeBegin;
        dasstrCurrNodeName.m_cch = ichCurr - ichCurrNodeBegin;

        if ( ichCurr >= f_pdastrNodePath->cchString  )
        {
            /*
            ** Did not find more separators, we are on the last segment.
            */
            if ( DRM_UTL_DASSTRStringsEqual( f_pdastrNodePath->pszString,
                                             &dasstrCurrNodeName,
                                             &g_dastrTagCDATA ) )
            {
                /*
                ** Requesting the CDATA section. Ask for that.
                */
                ChkArg( f_pdasstrXMLDataOut != NULL );
                ChkDR( _GetXMLNodeCDataA( f_pszBase,
                                          &dasstrXml,
                                          f_pdasstrXMLDataOut ) );
            }
            else
            {
                /*
                ** Just get the node and use the attributes parameters
                */
                ChkDR( _GetXMLSubNodeA( f_pszBase,
                                        &dasstrXml,
                                        f_pdastrNodePath->pszString,
                                        &dasstrCurrNodeName,
                                        f_pdastrAttrName,
                                        f_pdastrAttrValue,
                                        0,
                                        &dasstrNode,
                                        f_pdasstrXMLDataOut,
                                        1,
                                        FALSE ) );
            }
        }
        else
        {
#if DBG
                ChkDR( _GetXMLSubNodeA( f_pszBase,
                                        &dasstrXml,
                                        f_pdastrNodePath->pszString,
                                        &dasstrCurrNodeName,
                                        NULL,
                                        NULL,
                                        0,
                                        &dasstrNode,
                                        f_pdasstrXMLDataOut,
                                        1,
                                        FALSE ) );
#else
            /*
            **  Use optimized version - no need to check end tag,
            **  just set to the end of the whole XML data string.
            */
                ChkDR( _GetXMLSubNodeA( f_pszBase,
                                        &dasstrXml,
                                        f_pdastrNodePath->pszString,
                                        &dasstrCurrNodeName,
                                        NULL,
                                        NULL,
                                        0,
                                        &dasstrNode,
                                        f_pdasstrXMLDataOut,
                                        1,
                                        TRUE ) );
#endif
        }

        /*
        ** down the next level in XML data string
        */
        dasstrXml.m_ich = dasstrNode.m_ich;
        dasstrXml.m_cch = dasstrNode.m_cch;

        /*
        ** look for the next node name in a node path string
        */
        ichCurrNodeBegin = dasstrCurrNodeName.m_ich + dasstrCurrNodeName.m_cch + 1;
        ichCurr = ichCurrNodeBegin;
    }
    if( f_pdasstrXMLNodeOut != NULL )
    {
        f_pdasstrXMLNodeOut->m_ich = dasstrXml.m_ich;
        f_pdasstrXMLNodeOut->m_cch = dasstrXml.m_cch;
    }

ErrorExit:
    DRMASSERT( dr != DRM_E_LOGICERR );
    return DRM_XML_TranslateXMLError( dr );
}


/***********************************************************************************************
**
** Function:    DRM_XML_EnumNextNode
** Synopsis:    Enumerate the next node from the given xml string,
**              optionally with a specified tag name.
**
** Arguments :
**    [f_pdstrXML]          : pointer to XML data string.
**    [f_pdstrTag]          : pointer to XML tag name, cannot be NULL, may be empty string.
**    [f_pdstrNode]         : pointer to XML node under found XML tag,
**                            either f_pdstrNodeOut or f_pdstrNodeDataOut
**                            can be NULL but not both.
**    [f_pdstrNodeData]     : pointer to XML node's data under found XML tag
**                            that is text between <tag>...</tag>
**    [f_pdstr1stAttrName]  : pointer to XML tag's attribute name, may be NULL
**    [f_pdstr1stAttrValue] : pointer to XML tag's attribute value, may be NULL
**    [f_pfIsLeaf]          : will be set to TRUE of the returned node is a leaf node
**
**  Returns:     DRM_RESULT
**
***********************************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_EnumNextNode(
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in       DRM_CONST_STRING *f_pdstrTag,
    __out_opt  DRM_CONST_STRING *f_pdstrNode,
    __out_opt  DRM_CONST_STRING *f_pdstrNodeData,
    __out_opt  DRM_CONST_STRING *f_pdstr1stAttrName,
    __out_opt  DRM_CONST_STRING *f_pdstr1stAttrValue,
    __out      DRM_BOOL         *f_pfIsLeaf )
{
    return DRM_XML_EnumNextSubNode(
                f_pdstrXML,
                f_pdstrTag,
                f_pdstrNode,
                f_pdstrNodeData,
                f_pdstr1stAttrName,
                f_pdstr1stAttrValue,
                f_pfIsLeaf,
                0);
}


/***********************************************************************************************
** Function:    DRM_XML_EnumNextSubNode
** Synopsis:    Enumerate the next subnode from the given xml string,
**              optionaly, from nth layer, and with a specified tag name
**
** Arguments :
**    [f_pdstrXML]          : pointer to XML data string
**    [f_pdstrTag]          : pointer to XML tag name, cannot be NULL, may be empty string.
**    [f_pdstrNode]         : pointer to XML node under found XML tag,
**                            either f_pdstrNodeOut or f_pdstrNodeDataOut
**                            can be NULL but not both.
**    [f_pdstrNodeData]     : pointer to XML node's data under found XML tag
**    [f_pdstr1stAttrName]  : pointer to XML tag's attribute name, may be NULL
**    [f_pdstr1stAttrValue] : pointer to XML tag's attribute value, may be NULL
**    [f_pfIsLeaf]          : will be set to TRUE of the returned node is a leaf node
**    [f_nAtNthLayer]       : number of XML hierarchy levels to skip down,
**                            outermost layer is 0
**
**  Returns:     DRM_RESULT
***********************************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_EnumNextSubNode(
    __in const DRM_CONST_STRING *f_pdstrXML,
    __in       DRM_CONST_STRING *f_pdstrTag,
    __out_opt  DRM_CONST_STRING *f_pdstrNode,
    __out_opt  DRM_CONST_STRING *f_pdstrNodeData,
    __out_opt  DRM_CONST_STRING *f_pdstr1stAttrName,
    __out_opt  DRM_CONST_STRING *f_pdstr1stAttrValue,
    __out      DRM_BOOL         *f_pfIsLeaf,
    __in       DRM_DWORD         f_nAtNthLayer )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDRMString( f_pdstrXML );
    ChkArg( f_pdstrTag != NULL );

    ChkArg( f_pdstrNode != NULL
        || f_pdstrNodeData != NULL );

    ChkDR( _EnumXMLSubNodeW( f_pdstrXML,
                             f_pdstrTag,
                             f_pdstrNode,
                             f_pdstrNodeData,
                             f_pdstr1stAttrName,
                             f_pdstr1stAttrValue,
                             f_pfIsLeaf,
                             f_nAtNthLayer ) );

ErrorExit:
    return DRM_XML_TranslateXMLError(dr);
}


/* Ensure that the XML is valid */
DRM_API DRM_BOOL DRM_CALL DRM_XML_Validate(
    __in const DRM_CONST_STRING *f_pdstrXML )
{
    DRM_RESULT       dr      = DRM_SUCCESS;
    DRM_DWORD        ich     = 0;
    _XMLTAGW         xmltag  = { eTagNone, 0 };
    DRM_CONST_STRING dstrTag = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNode= EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrData= EMPTY_DRM_STRING;

    /* Note that all error codes are meaningless in the end.  Only a
       true of false is returned */
    DRMASSERT( f_pdstrXML             != NULL
            && f_pdstrXML->pwszString != NULL
            && f_pdstrXML->cchString   > 0 );

    ChkDRMString( f_pdstrXML );

    /* ScanTag will skip all the xml preamble stuff and give me the
       first real XML tag */
    if( !_ScanTagW( f_pdstrXML, &ich, &xmltag )
     ||  xmltag.m_eTagType != eOpenTag )
    {
        ChkDR( DRM_E_INVALID_XML_TAG );
    }

    /* The first tag has been found. */
    dstrTag.cchString  = xmltag.m_dasstrTag.m_cch;
    dstrTag.pwszString = xmltag.m_pwszBase
                       + xmltag.m_dasstrTag.m_ich;

    /* Now do an XML GetNode to parse to the end of
       this tag and ensure it's valid. */

    ChkDR( _GetXMLSubNodeW( f_pdstrXML,
                           &dstrTag,
                            NULL,
                            NULL,
                            0,
                           &dstrNode,
                           &dstrData,
                            0 ) );

ErrorExit:
    return DRM_SUCCEEDED( dr );
}

/*********************************************************************
** Function  : DRM_XML_TranslateXMLError
**
** Synopsis  : Converts XML parsing errors into more meaningful ones.
**
** Arguments
** [f_drXML] : Input DRM_RESULT code
**
** Returns   : DRM_RESULT
**             output error code
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_TranslateXMLError(
    __in DRM_RESULT f_drXML )
{
    DRM_RESULT dr = f_drXML;

    switch (dr)
    {
    case DRM_E_NO_XML_OPEN_TAG:
        dr = DRM_E_XMLNOTFOUND;
        break;

    default:
        dr = f_drXML;
        break;
    }

    return dr;
}

/**********************************************************************
** Function:    DRM_XML_EnumNextNode
** Synopsis:    Enumerate the next node from the given xml string
** Arguments:   [f_pdstrXML] --
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************
*/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_EnumNextNodeA(
    __in_ecount( f_pdasstrXML->m_ich + f_pdasstrXML->m_cch ) const DRM_CHAR      *f_pszBase,
    __in                                                     const DRM_SUBSTRING *f_pdasstrXML,
    __in                                                     const DRM_DWORD      f_iNode,
    __out                                                          DRM_SUBSTRING *f_pdasstrTag,
    __out_opt                                                      DRM_SUBSTRING *f_pdasstrNode,           /* either of these 2 parameter can be NULL but not both */
    __out_opt                                                      DRM_SUBSTRING *f_pdasstrNodeData,       /* data enclosed by the immediate <tag>...</tag> in the given XML string */
    __out_opt                                                      DRM_SUBSTRING *f_pdasstr1stAttrName,    /* optional */
    __out_opt                                                      DRM_SUBSTRING *f_pdasstr1stAttrValue )  /* set to TRUE of the returned node is a leaf node */
{
    DRM_RESULT    dr = DRM_E_NO_XML_OPEN_TAG;
    DRM_DWORD     cRecursionDepth= 0;
    DRM_DWORD     cTagsMatched   = 0;
    DRM_DWORD     ichNodeBegin   = 0;
    DRM_DWORD     ichDataBegin   = 0;
    DRM_DWORD     ichNodeEnd     = 0;
    DRM_DWORD     ichDataEnd     = 0;
    DRM_DWORD     iNode          = 0;
    DRM_DWORD     ichCurr        = f_pdasstrXML->m_ich;
    _XMLTAGA      xmltag         = { eTagNone, 0 };
    DRM_SUBSTRING dasstr1stAttrName  = { 0 };
    DRM_SUBSTRING dasstr1stAttrValue = { 0 };

    ChkArg(f_pszBase    != NULL
        && f_pdasstrXML != NULL
        && f_pdasstrTag != NULL);

    ChkArg(f_pdasstrNode     != NULL
        || f_pdasstrNodeData != NULL);

    for( ; ; )
    {
        /* scan for tag at currIndex */
        if (! _ScanTagA(f_pszBase, f_pdasstrXML, &ichCurr, &xmltag))
        {
            if (cTagsMatched == 0)
            {
                ChkDR(DRM_E_NO_XML_OPEN_TAG);
            }
            else
            {
                ChkDR(DRM_E_NO_XML_CLOSE_TAG);
            }
        }

        switch (xmltag.m_eTagType)
        {
            case eOpenTag:
                if (cRecursionDepth == 0
                &&  ++iNode         == (f_iNode + 1))
                {
                    ichNodeBegin = xmltag.m_ichTagBegin;
                    ichDataBegin = xmltag.m_ichTagEnd + 1;

                    /* assign the tag name so that we know what to look for closing tag */

                    f_pdasstrTag->m_cch = xmltag.m_dasstrTag.m_cch;
                    f_pdasstrTag->m_ich = xmltag.m_dasstrTag.m_ich;

                    /* save the attr name/value */
                    dasstr1stAttrName.m_cch  = xmltag.m_dasstrAttrName.m_cch;
                    dasstr1stAttrName.m_ich  = xmltag.m_dasstrAttrName.m_ich;
                    dasstr1stAttrValue.m_cch = xmltag.m_dasstrAttrValue.m_cch;
                    dasstr1stAttrValue.m_ich = xmltag.m_dasstrAttrValue.m_ich;
                }
                else
                {
                    cRecursionDepth++;
                }
                cTagsMatched++;
                break;

            case eCloseTag:    /* close tag found */
                if (cTagsMatched == 0)
                {
                    /* we get </tag> before we get <tag> */
                    ChkDR(DRM_E_NO_XML_OPEN_TAG);
                }

                cTagsMatched--;

                if (cTagsMatched    == 0
                &&  cRecursionDepth == 0)
                {
                    /* we've got the matching <tag> and </tag>,
                    ** exit the loop if this is the node we want
                    */
                    ichNodeEnd   = xmltag.m_ichTagEnd;
                    ichDataEnd   = xmltag.m_ichTagBegin - 1;
                    goto PREPARENODE;
                }
                else
                {
                    cRecursionDepth--;
                }
                break;

            default:
                break;
        }
    }

PREPARENODE:
    /* prepare f_pdasstrNode & f_pdasstrNodeData */
    if (f_pdasstrNode != NULL)
    {
        f_pdasstrNode->m_ich = ichNodeBegin;
        f_pdasstrNode->m_cch = (DRM_DWORD) (ichNodeEnd - ichNodeBegin + 1);
    }

    if (f_pdasstrNodeData != NULL)
    {
        if (xmltag.m_eTagType == eEmptyTag)
        {
            f_pdasstrNodeData->m_ich = 0;
            f_pdasstrNodeData->m_cch = 0;
        }
        else
        {
            f_pdasstrNodeData->m_ich = ichDataBegin;
            f_pdasstrNodeData->m_cch = (DRM_DWORD) (ichDataEnd - ichDataBegin + 1);

            if (! _AllTrimA(f_pszBase, f_pdasstrNodeData))
            {
                f_pdasstrNodeData->m_cch = 0;
            }
        }
    }

    if (f_pdasstr1stAttrName != NULL)
    {
        *f_pdasstr1stAttrName = dasstr1stAttrName;
    }

    if (f_pdasstr1stAttrValue != NULL)
    {
        *f_pdasstr1stAttrValue = dasstr1stAttrValue;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    return DRM_XML_TranslateXMLError(dr);
} /* DRM_XML_EnumNextNode */

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XML_CountMatchingNodesA(
    __in_ecount( f_pdasstrXML->m_ich + f_pdasstrXML->m_cch ) const DRM_CHAR              *f_pszBase,
    __in                                                     const DRM_SUBSTRING         *f_pdasstrXML,
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdastrTag,          /* optional */
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdastr1stAttrName,  /* optional */
    __in_opt                                                 const DRM_ANSI_CONST_STRING *f_pdastr1stAttrValue, /* optional */
    __out                                                          DRM_DWORD             *f_cMatchingNodes )
{
    DRM_RESULT    dr    = DRM_SUCCESS;
    DRM_DWORD     iNode = 0;
    DRM_SUBSTRING dasstrTag          = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrDummy        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstr1stAttrLabel = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstr1stAttrValue = EMPTY_DRM_SUBSTRING;

    ChkArg(f_pszBase        != NULL
        && f_pdasstrXML     != NULL
        && f_cMatchingNodes != NULL);

    *f_cMatchingNodes = 0;

    for( ; ; )
    {
        dr = DRM_XML_EnumNextNodeA(f_pszBase,
                                   f_pdasstrXML,
                                   iNode++,
                                  &dasstrTag,
                                  &dasstrDummy,
                                   NULL,
                                  &dasstr1stAttrLabel,
                                  &dasstr1stAttrValue);

        if (dr == DRM_E_XMLNOTFOUND)
        {
            dr = DRM_SUCCESS;
            break;
        }
        else
        {
            ChkDR(dr);

            if ((f_pdastrTag          == NULL || DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstrTag,          f_pdastrTag))
            &&  (f_pdastr1stAttrName  == NULL || DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstr1stAttrLabel, f_pdastr1stAttrName))
            &&  (f_pdastr1stAttrValue == NULL || DRM_UTL_DASSTRStringsEqual(f_pszBase, &dasstr1stAttrValue, f_pdastr1stAttrValue)))
            {
                (*f_cMatchingNodes)++;
            }
        }
    }

ErrorExit:
    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;

PRAGMA_STRICT_GS_POP;

