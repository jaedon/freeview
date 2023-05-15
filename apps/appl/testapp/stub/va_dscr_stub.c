/*
  FILE : stub_dscr.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_dscr.h"

DWORD VA_DSCR_Open( DWORD dwStbStreamHandle )
{
  return 0;
}

INT VA_DSCR_Close( DWORD dwStbDescrHandle )
{
  return 0;
}

INT VA_DSCR_SetKeys(
    DWORD dwStbDescrHandle,
    UINT16 uiOddKeyLength, const BYTE  *pOddKey,
    UINT16 uiEvenKeyLength, const BYTE  *pEvenKey,
    void *pUserData )
{
  return 0;
}

INT VA_DSCR_SetParameters(
    DWORD dwStbStreamHandle,
    const tVA_DSCR_ConfigurationParameters *pstParameters )
{
    return 0;
}

/* End of File */