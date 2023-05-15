/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmcrt.h>
#include <drmerror.h>

ENTER_PK_NAMESPACE_CODE;

static const char * g_pszUnknownError            = "UNKNOWN";
static const char * g_pszUnknownErrorDescription = "No matching error code found";

/*****************************************************************************
** Function:    DRM_ERR_IsErrorCodeKnown
**
** Synopsis:    Looks up the error code in the table to see if it is a known error.
**
** Arguments:   [f_drErrorCode]      : DRM_RESULT to look up
**
** Returns:     TRUE                 - Error Code is recognized
**              FALSE                - Error Code is not recognized
******************************************************************************/
DRM_API DRM_BOOL DRM_CALL DRM_ERR_IsErrorCodeKnown( DRM_RESULT f_drErrorCode )
{
    DRM_DWORD index  = 0;
    DRM_BOOL  fFound = FALSE;

    while ( !fFound
         && g_rgoPKErrorMap[index].m_dastrName.pszString != NULL )
    {
        if ( g_rgoPKErrorMap[index].m_drValue == f_drErrorCode)
        {
            fFound = TRUE;
        }
        index++;
    }

    return fFound;
}

/*****************************************************************************
** Function:    DRM_ERR_GetErrorNameFromCode
**
** Synopsis:    Gets the symbolic name associated with the given error code.
**              It optionally retrieves the description string.
**
** Arguments:   [f_drErrorCode]      : DRM_RESULT to look up
**              [f_ppszDescription]  : pointer to a DRM_CHAR* to receive the description.
**                                     This can be NULL.
**
** Note:        The output pointers are pointers into the global array and SHOULD NOT be freed.
**
** Returns:     DRM_CHAR* holding the string name of the error code. If the Error Code
**              is not found, then the return value is the string "UNKNOWN".
******************************************************************************/
DRM_API DRM_CHAR * DRM_CALL DRM_ERR_GetErrorNameFromCode( DRM_RESULT f_drErrorCode, 
                                                    const DRM_CHAR **f_ppszDescription)
{
    DRM_DWORD  index        = 0;
    DRM_CHAR  *pszErrorName = ( DRM_CHAR * ) g_pszUnknownError;

    if ( f_ppszDescription != NULL )
    {
        *f_ppszDescription = g_pszUnknownErrorDescription;
    }

    while ( g_rgoPKErrorMap[index].m_dastrName.pszString != NULL )
    {
        if ( g_rgoPKErrorMap[index].m_drValue == f_drErrorCode)
        {
            pszErrorName = g_rgoPKErrorMap[index].m_dastrName.pszString;
            if ( f_ppszDescription != NULL )
            {
                *f_ppszDescription = g_rgoPKErrorMap[index].m_dastrDescription.pszString;
            }
            break;
        }
        index++;
    }

    return pszErrorName;
}

/*****************************************************************************
** Function:    DRM_ERR_GetErrorCodeFromStringA
**
** Synopsis:    Gets the error value associated with the given string. If it
**              recognizes the string as a number, it parses that number as
**              the error code.
**
** Arguments:   [f_pszString]        : pointer to string to look up
**              [f_cchString]        : count of characters in string
**              [f_pdrErrorCode]     : pointer to a DRM_RESULT to receive the value
**
** Note:        If the error code is not found, the output value is 0xFFFFFFFF.
**
** Returns:     DRM_SUCCESS             - on success
**              DRM_E_NOT_FOUND         - if the string is not found in the list 
**                                        or able to be converted to a number.
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ERR_GetErrorCodeFromStringA(const DRM_CHAR   *f_pszString,
                                                                  DRM_DWORD   f_cchString,
                                                                  DRM_RESULT *f_pdrErrorCode)
{
    DRM_RESULT dr          = DRM_E_NOT_FOUND;
    DRM_DWORD  index       = 0;
    DRM_RESULT drErrorCode = 0xFFFFFFFF;

    /*
    ** Before searching the array, see if the string is a number
    */
    dr = DRMCRT_AtoDWORD( f_pszString, 
                          f_cchString, 
                          0,             /* Try to determine the base */
           (DRM_DWORD *) &drErrorCode );

    /*
    ** reset the error code to the Unknown value if no conversion happened.
    */
    if ( dr == DRM_S_FALSE )
    {
        dr = DRM_E_NOT_FOUND;
        drErrorCode = 0xFFFFFFFF;
    }

    /*
    ** Now search the array until we match the string.
    */
    while ( dr                                         != DRM_SUCCESS
         && g_rgoPKErrorMap[index].m_dastrName.pszString != NULL )
    {
        /*
        ** Check if the strings are the same length before doing the text compare.
        ** This makes the lookup quicker because we only compare strings if they are
        ** the same length and it prevents the situation where the search string is
        ** is a match to the beginning of an error name but not a match to the error name.
        */
        if ( f_cchString == g_rgoPKErrorMap[index].m_dastrName.cchString
          &&           0 == DRMCRT_strncmp( g_rgoPKErrorMap[index].m_dastrName.pszString, 
                                            f_pszString, 
                                            f_cchString ) )
        {
            drErrorCode = g_rgoPKErrorMap[index].m_drValue;
            dr = DRM_SUCCESS;
        }
        index++;
    }

    *f_pdrErrorCode = drErrorCode;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
