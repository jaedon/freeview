/*
  FILE : stub_fs.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_fs.h"

DWORD VA_FS_Open(const char * szFilename, tVA_FS_Mode eOpeningMode)
{
    return 0;
}

INT VA_FS_Close(DWORD dwFileHandle)
{
    return 0;
}

INT VA_FS_Read (DWORD dwFileHandle, UINT32 *pNbReadBytes, BYTE *pBuffer)
{
    return 0;
}

INT VA_FS_Write (DWORD dwFileHandle, UINT32 *pNbWriteBytes, BYTE * pBuffer)
{
    return 0;
}

INT VA_FS_Seek (DWORD dwFileHandle, 
                tVA_FS_SeekOrigin eOrigin, INT32 iOffset, 
                UINT32 *pNewPositionFromBegin)
{
    return 0;
}

INT VA_FS_GetSize (const char * szFilename, UINT32 *pFileSize)
{
    return 0;
}

INT VA_FS_Remove (const char * szFilename)
{
    return 0;
}

UINT32 VA_FS_GetFilesCount(void)
{
    return 0;
}

INT VA_FS_GetFiles(UINT32 *pFilesCount, tVA_FS_FileInfo aFiles[])
{
    return 0;
}

/* End of File */