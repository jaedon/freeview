/*
  FILE : stub_dmx.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_dmx.h"

DWORD VA_DMX_AllocateSectionFilter(
    DWORD dwAcsId, DWORD dwVaFilterHandle, WORD  wPid,
    tVA_DMX_NotifyFilteredSection pfVA_DMX_NotifyFilteredSection)
{
  return 0;
}

INT VA_DMX_FreeSectionFilter( DWORD dwStbFilterHandle )
{
  return 0;
}

INT VA_DMX_SetSectionFilterParameters(
                                      DWORD dwStbFilterHandle,
                                      UINT32 uiLength, BYTE *pValue, BYTE *pMask,
                                      tVA_DMX_NotificationMode eNotificationMode)
{
  return 0;
}

INT VA_DMX_StartSectionFilter(DWORD  dwStbFilterHandle)
{
  return 0;
}

INT VA_DMX_StopSectionFilter(DWORD  dwStbFilterHandle)
{
  return 0;
}

/* End of File */