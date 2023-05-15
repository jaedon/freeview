/*
  FILE : stub_net.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_net.h"

DWORD VA_NET_UnicastOpen(DWORD dwVaNetHandle, const char * pIPAddress,
                         UINT16 uiPort, UINT32 uiTimeout,
                         tVA_NET_Callback pfVaUnicastCallback)
{
      return 0;
}

INT VA_NET_UnicastReceive( DWORD dwStbNetHandle, UINT32 uiMaximumLength ,UINT32 uiTimeout )
{
  return 0;
}

INT VA_NET_UnicastSend ( DWORD dwStbNetHandle, UINT32 uiDataLength ,BYTE* pData, UINT32 uiTimeout )
{
  return 0;
}

INT VA_NET_UnicastClose( DWORD dwStbNetHandle )
{
  return 0;
}

DWORD VA_NET_MulticastOpen(DWORD dwVaNetHandle ,
                           const char *pIPAddress, UINT16 uiPort,
                           UINT32 uiBufferSize,
                         tVA_NET_Callback pfVaMulticastCallback)
{
  return 0;
}

INT VA_NET_MulticastClose( DWORD dwStbNetHandle )
{
  return 0;
}

/* End of File */