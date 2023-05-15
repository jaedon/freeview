/*
  FILE : stub_crypt.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_crypt.h"

#define STUB { return 0 ;}

INT VA_CRYPT_GetCapabilities ( tVA_CRYPT_Capabilities * pCapabilities ) STUB
INT VA_CRYPT_ImportKey       ( const BYTE * pWrappedKey , UINT32 uiKeySize , DWORD dwWrappingKeyId , DWORD * pImportedKeyId ) STUB
INT VA_CRYPT_ExportKey       ( BYTE * pWrappedKey , UINT32 * puiKeySize , DWORD dwWrappingKeyId , DWORD dwExportKeyId ) STUB
INT VA_CRYPT_ReleaseKey      ( DWORD dwKeyId ) STUB
INT VA_CRYPT_CipherOpen      ( const tVA_CRYPT_Config * pConfig , DWORD * pHandle ) STUB
INT VA_CRYPT_CipherProcess   ( DWORD dwHandle , const BYTE * pSrc , BYTE * pDst , UINT32 uiSize ) STUB
INT VA_CRYPT_CipherClose     ( DWORD dwHandle ) STUB

UINT32 VA_CRYPT_GetRandom    ( void ) STUB

/* End of File */