/*
  FILE : stub_sc.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_sc.h"
#if 0
    INT VA_SC_Reset( DWORD dwScSlot, UINT8 uiPPSNegotiationValue )
    { 
      return 0;
    }

    INT VA_SC_GetState(DWORD dwScSlot, tVA_SC_State *pScState)
    { 
      return 0;
    }

    INT VA_SC_SendCommand(DWORD dwScSlot, UINT32 uiCommandSize, BYTE *pCommand)
    { 
      return 0;
    }

    INT VA_SC_Activate(DWORD dwScSlot)
    { 
      return 0;
    }

    INT VA_SC_Deactivate(DWORD dwScSlot)
    { 
      return 0;
    }

    INT VA_SC_Lock(DWORD dwScSlot)
    { 
      return 0;
    }

    INT VA_SC_Unlock(DWORD dwScSlot)
    { 
      return 0;
    }
#endif
#if 0
	INT VA_SC_ResetDone(DWORD dwScSlot, UINT8 uiAtrSize, BYTE *pAtr, UINT8 uiPPSNegotiatedvalue)
	{
		      return 0;
	}

	INT VA_SC_ResetFailed(DWORD dwScSlot)
	{
		      return 0;		
	}
	INT VA_SC_CardInserted(DWORD dwScSlot)
	{
		      return 0;		
	}

	INT VA_SC_CardExtracted(DWORD dwScSlot)
	{
		      return 0;		
	}

	INT VA_SC_CommandDone(DWORD dwScSlot, UINT32 uiReturnSize, BYTE *pReturn)
	{
		      return 0;		
	}
	
	INT VA_SC_CommandFailed(DWORD dwScSlot)
	{
		      return 0;		
	}
#endif		
/* End of File */
