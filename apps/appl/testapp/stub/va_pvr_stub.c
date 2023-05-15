/*
  FILE : stub_pvr.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_pvr.h"

    DWORD VA_PVR_OpenEcmChannel ( DWORD              dwAcsId,
                                  tVA_PVR_RecordType eRecordType,
                                  WORD               wEcmChannelPid )
    {
      return 0;
    }

    INT VA_PVR_CloseEcmChannel ( DWORD   dwStbEcmChannelHandle )
    {
      return 0;
    }

    INT VA_PVR_RecordEcm (  DWORD        dwStbEcmChannelHandle,
                            UINT32       uiEcmLength,
                            BYTE *       pEcm,
                            void *       pUserData )
    {
      return 0;
    }

    INT VA_PVR_WriteMetadata (  DWORD    dwAcsId,
                                UINT32   uiMetadataLength,
                                BYTE *   pMetadata )
    {
      return 0;
    }

    INT VA_PVR_ReadMetadata (   DWORD    dwAcsId,
                                UINT32   uiMetadataLength,
                                BYTE *   pMetadata )
    {
      return 0;
    }

/* End of File */