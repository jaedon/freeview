/*
  FILE : stub_schip.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_schip.h"

    INT   VA_SCHIP_SetChipsetMode ( tVA_SCHIP_CipheringMode eChipsetMode )
    {
         return 0;
    }

    INT VA_SCHIP_GetChipsetMode ( tVA_SCHIP_CipheringMode* eChipsetMode )
    {
         return 0;
    }

    INT   VA_SCHIP_SetSessionKey ( UINT32 uiSessionKeyLength, const BYTE *pSessionKey )
    {
         return 0;
    }

    DWORD VA_SCHIP_GetChipsetId ( void )
    {
         return 0;
    }
	
/* End of File */