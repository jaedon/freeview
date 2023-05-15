/*
  FILE : stub_prot.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_prot.h"

INT VA_PROT_SuspendFreeChannels()
{
  return 0;
}

INT VA_PROT_ResumeFreeChannels()
{
  return 0;
}

INT VA_PROT_SetDigitalOutputProtection(WORD wProtection)
{
  return 0;
}

INT VA_PROT_GetCapabilities(WORD *pOutputProtection, WORD *pCopyProtection)
{
    return 0;
}

INT VA_PROT_SetCopyProtection(DWORD dwAcsId, WORD wCopyProtection)
{
    return 0;
}

INT VA_PROT_StartFingerprintingSession(DWORD dwAcsId,
                                       const char *pszFingerprintStr,
                                       UINT8 uiFontSize,
                                       tVA_PROT_Color stForegroundColor,
                                       tVA_PROT_Color stBackgroundColor)
{
    return 0;
}

INT VA_PROT_DisplayFingerprint(DWORD dwAcsId,
                               UINT8 uiHorizontalPercent,
                               UINT8 uiVerticalPercent,
                               UINT16 uiDuration)
{
    return 0;
}

INT VA_PROT_StopFingerprintingSession(DWORD dwAcsId)
{
    return 0;
}

INT VA_PROT_SetCertificateList(tVA_PROT_CertificateListType eCertificateListType,
                               UINT32 uiLength,
                               const BYTE *pCertificateList)
{
    return 0;
}

INT VA_PROT_SetCiPlusContentInfos(DWORD dwAcsId,                                  
                                  const tVA_PROT_CiPlusInfos *pstCiPlusInfos)
{
    return 0;
}

INT VA_PROT_SetCiPlusKeyLifetime(UINT32 uiKeyLifetime)
{
    return 0;
}

/* End of File */