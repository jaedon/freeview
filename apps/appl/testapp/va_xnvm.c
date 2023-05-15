/*
  FILE : stub_xnvm.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_xnvm.h"
#if 0
DWORD   VA_XNVM_Open(DWORD dwVaXnvmHandle, tVA_XNVM_Segment eSegment)
{
    return 0;
}

INT     VA_XNVM_Close(DWORD dwStbXnvmHandle)
{
    return 0;
}

INT     VA_XNVM_Read (DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pReadData)
{
  return 0;
}

INT     VA_XNVM_Write(DWORD dwStbXnvmHandle, UINT32 uiOffset, UINT32 uiSize, BYTE* pWriteData)
{
  return 0;
}
#endif

INT     VA_XNVM_RequestDone(DWORD dwVaXnvmHandle)
{
	  return 0;
}

INT     VA_XNVM_RequestFailed(DWORD dwVaXnvmHandle)
{
	  return 0;
}

	
/* End of File */
