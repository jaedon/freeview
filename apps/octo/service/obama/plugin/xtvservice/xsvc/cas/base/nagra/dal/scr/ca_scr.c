/**************************************************************************************************/
#define ________CA_SCR_Private_Include_____________________________________________________
/**************************************************************************************************/
#include "na_glob.h"

//#include "pal_sys.h"
//#include "pal_dsc.h"
#include "util.h"
#include "ca_scr.h"
//#include "di_dsc.h"
//#include "di_crypt.h"
//#include "mw_media.h"
#include <_svc_cas_mgr_common.h>
#include <_svc_cas_mgr_main.h>
//#include "mgr_action.h"

/**************************************************************************************************/
#define ________CA_SCR_Golbal_Value________________________________________________________
/**************************************************************************************************/
extern HERROR CA_NA_ScrExtCallKeyDataInfo(TUnsignedInt16 xEmi, HUINT8 *pucKey, HUINT32 ulKeyLength);


/**************************************************************************************************/
#define ________CA_SCR_Private_Define_______________________________________________________
/**************************************************************************************************/






/**************************************************************************************************/
#define ________CA_SCR_Private_Type_Define_________________________________________________
/**************************************************************************************************/
typedef enum
{
	SCR_PLAYER_LIVE = 0,
	SCR_PLAYER_MEDIA,
	SCR_PLAYER_RECORD
} SCR_PLAYER_TYPE;

typedef struct ScrChannelInfo
{
	TBoolean			bUsed;
	TUnsignedInt32		ulActionId;
	TUnsignedInt32		ulDscVideoIndex;
	TUnsignedInt32		ulDscAudioIndex;
	SCR_PLAYER_TYPE 	eScrType;
	TUnsignedInt16		usEmi;
	TTransportSessionId	ulSessionId;
	TSize                    xScrDecryptionKeySize;
	TUnsignedInt8*          pxScrDecryptionKey;
} SCR_SessionInfo_t;

#define 	MAX_SCR_POOL 	10

/**************************************************************************************************/
#define ________CA_SCR_Private_Static_Value_________________________________________________
/**************************************************************************************************/
static SCR_SessionInfo_t		s_stScrSession[MAX_SCR_POOL] = {{0,}, };

/**************************************************************************************************/
#define ________CA_SCR_Private_Static_Prototype______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________CA_SCR_Private_Static_Prototype_Body_________________________________________
/**************************************************************************************************/
static HERROR local_caScr_checkUsedTsId(TTransportSessionId xTransportSessionId, TTransportSessionId *xScrSessionId)
{
	TBoolean			bFound = FALSE;
	TUnsignedInt32		usIndex = 0;
	//SCR_SessionInfo_t		*pFreeScrSession = NULL;
#if defined(CONFIG_MW_CAS_NAGRA_SES_DVLTEST)
	// dvl test시에는 아래처럼 해주지 않으면 stop & play 반복하면 항상 xTransportSessionId=1만 들어온다. 즉, index=0만....
	// 그래서 이 함수가 항상 fail 됨. 이를 방지하지 위해서.
	HUINT16			nCnt = 0;
#endif
	HxLOG_Print("[local_caScr_checkUsedTsId] +!! \n");

	bFound = FALSE;

	for(usIndex = 0; usIndex < MAX_SCR_POOL; usIndex++)
	{
		if (s_stScrSession[usIndex].bUsed == TRUE && s_stScrSession[usIndex].ulSessionId == xTransportSessionId)
		{
			bFound = TRUE;
#if defined(CONFIG_MW_CAS_NAGRA_SES_DVLTEST)
			nCnt++;
#endif
		}
	}

#if defined(CONFIG_MW_CAS_NAGRA_SES_DVLTEST)
	if (nCnt != MAX_SCR_POOL)
	{
		bFound = FALSE;
	}
#endif

	if(bFound == FALSE)
	{
		for(usIndex = 0; usIndex < MAX_SCR_POOL; usIndex++)
		{
			if (s_stScrSession[usIndex].bUsed == FALSE)
			{
				HxLOG_Print("usIndex= %d, xScrSessionId = %d\n", usIndex, usIndex);
				*xScrSessionId = usIndex;
				return ERR_OK;
			}
		}
	}
	HxLOG_Print("[local_caScr_checkUsedTsId] -!! \n");

	return ERR_FAIL;
}


/**************************************************************************************************/
#define ________CA_SCR_Public_Functions_Body_______________________________________________
/**************************************************************************************************/


TScrStatus scrOpenEncryptionSession
(
	TScrEncSessionHandler**   ppxScrEncSessionHandler,
	TTransportSessionId         xTransportSessionId
)
{
	HERROR				hError = ERR_OK;
	SCR_SessionInfo_t		*pNewScrSession = NULL;
    TTransportSessionId 	xScrSessionId;

	HxLOG_Print("\n");

	if (ppxScrEncSessionHandler == NULL)
	{
		HxLOG_Error("ppxScrEncSessionHandler is NULL\n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	HxLOG_Print("xTransportSessionId = %d\n", xTransportSessionId);

	if (xTransportSessionId == TRANSPORT_SESSION_ID_INVALID)
	{
		HxLOG_Error("TRANSPORT_SESSION_ID_INVALID fail!! \n");
		return SCR_ERROR_WRONG_TS_ID;
	}
#if 0
	if (xTransportSessionId >= MAX_SCR_POOL)
	{
		HxLOG_Error("MAX_SCR_POOL fail!! \n");
		return SCR_ERROR_WRONG_TS_ID;
	}
#endif

	hError = local_caScr_checkUsedTsId(xTransportSessionId, &xScrSessionId);
	if (hError != ERR_OK)
	{
		HxLOG_Error("local_caScr_checkUsedTsId fail!! \n");
		return SCR_ERROR;
	}

	*ppxScrEncSessionHandler = NULL;

	pNewScrSession = &s_stScrSession[xScrSessionId];

	pNewScrSession->bUsed = TRUE;
	pNewScrSession->ulSessionId = xTransportSessionId;
	pNewScrSession->eScrType = SCR_PLAYER_RECORD;

	*ppxScrEncSessionHandler = pNewScrSession;

	return SCR_NO_ERROR;
}


TScrStatus scrCloseEncryptionSession
(
	TScrEncSessionHandler*   pxScrEncSessionHandler
)
{
	TUnsignedInt32		usIndex = 0;

	HxLOG_Print("\n");

	if (pxScrEncSessionHandler == NULL)
	{
		HxLOG_Error("pxScrEncSessionHandler is NULL\n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	HxLOG_Print("pxScrEncSessionHandler = 0x%x\n", pxScrEncSessionHandler);

	for (usIndex = 0; usIndex < MAX_SCR_POOL; usIndex++)
	{
		if (pxScrEncSessionHandler == &s_stScrSession[usIndex])
		{
			if (s_stScrSession[usIndex].bUsed)
			{
				s_stScrSession[usIndex].bUsed = FALSE;
				break;
			}
			else
			{
				HxLOG_Error("not used handle!! \n");
				return SCR_ERROR_WRONG_HANDLER;
			}
		}
	}

	if (usIndex == MAX_SCR_POOL)
	{
		HxLOG_Error("not used handle!! \n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	return SCR_NO_ERROR;
}


TScrStatus scrSetEncryptionEmi
(
	TScrEncSessionHandler*  pxScrEncSessionHandler,
	TUnsignedInt16           xEncryptionEmi
)
{
	TUnsignedInt32		usIndex = 0;

	HxLOG_Print("\n");

	if (pxScrEncSessionHandler == NULL)
	{
		HxLOG_Error("pxScrEncSessionHandler is NULL\n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	HxLOG_Print("pxScrEncSessionHandler = 0x%x\n", pxScrEncSessionHandler);
	HxLOG_Print("xEncryptionEmi = 0x%x\n", xEncryptionEmi);

	for (usIndex = 0; usIndex < MAX_SCR_POOL; usIndex++)
	{
		if (pxScrEncSessionHandler == &s_stScrSession[usIndex])
		{
			if (s_stScrSession[usIndex].bUsed)
			{
				break;
			}
			else
			{
				HxLOG_Error("not used handle!! \n");
				return SCR_ERROR_WRONG_HANDLER;
			}
		}
	}

	if (usIndex == MAX_SCR_POOL)
	{
		HxLOG_Error("not used handle!! \n");
		return SCR_ERROR_WRONG_HANDLER;
	}
// Nagra에 해당 EMI를 어떤 걸 사용하는 지 물어보고 아래 값 사용 해야 함 by mslee 2013/08/30
// 해당 case에 s_stScrSession[usIndex].usEmi = xEncryptionEmi; 추가 하면 됨
	switch(xEncryptionEmi)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	case 0x4021:
#endif
	case 0x4023:		// AES-128 in CBC mode. Apple Live Streaming.
		s_stScrSession[usIndex].usEmi = xEncryptionEmi;
		break;
#endif

	default:
		// not Nagravision Standard
		HxLOG_Error("wrong EMI!! \n");
		s_stScrSession[usIndex].usEmi = xEncryptionEmi;
		return SCR_ERROR_WRONG_EMI;
	}

	return SCR_NO_ERROR;
}


TScrStatus scrSetClearTextEncryptionKey
(
	TScrEncSessionHandler*  pxScrEncSessionHandler,
	TSize                    xScrEncryptionKeySize,
	const TUnsignedInt8*          pxScrEncryptionKey
)
{
	TUnsignedInt32		usIndex = 0;

	HxLOG_Print("\n");

	if (pxScrEncSessionHandler == NULL)
	{
		HxLOG_Error("pxScrEncSessionHandler is NULL\n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	if (pxScrEncryptionKey == NULL)
	{
		HxLOG_Error("pxScrEncryptionKey is NULL\n");
		return SCR_ERROR;
	}

	HxLOG_Print("pxScrEncSessionHandler = 0x%x\n", pxScrEncSessionHandler);
	HxLOG_Print("xScrEncryptionKeySize = %d\n", xScrEncryptionKeySize);

	for (usIndex = 0; usIndex < MAX_SCR_POOL; usIndex++)
	{
		if (pxScrEncSessionHandler == &s_stScrSession[usIndex])
		{
			if (s_stScrSession[usIndex].bUsed)
			{
				break;
			}
			else
			{
				HxLOG_Error("not used handle!! \n");
				return SCR_ERROR_WRONG_HANDLER;
			}
		}
	}

	if (usIndex == MAX_SCR_POOL)
	{
		HxLOG_Error("not used handle!! \n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	if (xScrEncryptionKeySize > 0)
	{

	}
	else
	{
		HxLOG_Error("xScrEncryptionKeySize size < 0 Fail!! \n");
		return SCR_ERROR;
	}


	return SCR_NO_ERROR;
}


TScrStatus scrOpenDecryptionSession
(
	TScrDecSessionHandler** ppxScrDecSessionHandler,
	TTransportSessionId       xTransportSessionId
)
{
	HERROR				hError = ERR_OK;
	SCR_SessionInfo_t		*pNewScrSession = NULL;

    TTransportSessionId 	xScrSessionId;


	if (ppxScrDecSessionHandler == NULL)
	{
		HxLOG_Error("ppxScrDecSessionHandler is NULL\n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	HxLOG_Print("TTransportSessionId = %d\n", xTransportSessionId);

	if (xTransportSessionId == TRANSPORT_SESSION_ID_INVALID)
	{
		HxLOG_Error("TRANSPORT_SESSION_ID_INVALID fail!! \n");
		return SCR_ERROR_WRONG_TS_ID;
	}

	hError = local_caScr_checkUsedTsId(xTransportSessionId, &xScrSessionId);
	if (hError != ERR_OK)
	{
		HxLOG_Error("local_caScr_checkUsedTsId fail!! \n");
#if defined(CONFIG_MW_CAS_NAGRA_SES_DVLTEST)
		return SCR_ERROR;
#endif
		if (s_stScrSession[xTransportSessionId].xScrDecryptionKeySize != 0)
		{
			s_stScrSession[xTransportSessionId].xScrDecryptionKeySize = 0;

		}
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
		return SCR_ERROR;
#endif
	}

	*ppxScrDecSessionHandler = NULL;

	pNewScrSession = &s_stScrSession[xScrSessionId];

        HxLOG_Print("pNewScrSession->eScrType = %d\n", SCR_PLAYER_MEDIA);
	pNewScrSession->eScrType = SCR_PLAYER_MEDIA;

	pNewScrSession->bUsed = TRUE;
	pNewScrSession->ulSessionId = xTransportSessionId;

	*ppxScrDecSessionHandler = pNewScrSession;

	return SCR_NO_ERROR;
}


TScrStatus scrCloseDecryptionSession
(
	TScrDecSessionHandler* pxScrDecSessionHandler
)
{
	TUnsignedInt32		usIndex = 0;

	HxLOG_Print("\n");

	if (pxScrDecSessionHandler == NULL)
	{
		HxLOG_Error("pxScrDecSessionHandler is NULL\n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	HxLOG_Print("pxScrDecSessionHandler = 0x%x\n", pxScrDecSessionHandler);

	for (usIndex = 0; usIndex < MAX_SCR_POOL; usIndex++)
	{
		if (pxScrDecSessionHandler == &s_stScrSession[usIndex])
		{
			if (s_stScrSession[usIndex].bUsed)
			{
				s_stScrSession[usIndex].bUsed = FALSE;

				if (s_stScrSession[usIndex].eScrType == SCR_PLAYER_MEDIA)
				{
					if (s_stScrSession[usIndex].xScrDecryptionKeySize != 0)
					{
						s_stScrSession[usIndex].xScrDecryptionKeySize = 0;
					}
				}
				break;
			}
			else
			{
				HxLOG_Error("not used handle!! \n");
				return SCR_ERROR_WRONG_HANDLER;
			}
		}
	}

	if (usIndex == MAX_SCR_POOL)
	{
		HxLOG_Error("not used handle!! \n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	return SCR_NO_ERROR;
}


TScrStatus scrSetDecryptionEmi
(
	TScrDecSessionHandler*  pxScrDecSessionHandler,
	TUnsignedInt16           xDecryptionEmi
)
{
	TUnsignedInt32		usIndex = 0;

	HxLOG_Print("\n");

	if (pxScrDecSessionHandler == NULL)
	{
		HxLOG_Error("pxScrDecSessionHandler is NULL\n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	HxLOG_Print("pxScrDecSessionHandler = 0x%x\n", pxScrDecSessionHandler);
	HxLOG_Print("xDecryptionEmi = 0x%x\n", xDecryptionEmi);

	for (usIndex = 0; usIndex < MAX_SCR_POOL; usIndex++)
	{
		if (pxScrDecSessionHandler == &s_stScrSession[usIndex])
		{
			if (s_stScrSession[usIndex].bUsed)
			{
				break;
			}
			else
			{
				HxLOG_Error("not used handle!! \n");
				return SCR_ERROR_WRONG_HANDLER;
			}
		}
	}

	if (usIndex == MAX_SCR_POOL)
	{
		HxLOG_Error("not used handle!! \n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	switch(xDecryptionEmi)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	case 0x4021:
#endif
	case 0x4023:		// AES-128 in CBC mode. Apple Live Streaming.
		s_stScrSession[usIndex].usEmi = xDecryptionEmi;
		break;
#endif
	default:
		// not Nagravision Standard
		HxLOG_Error("wrong EMI!! \n");
		s_stScrSession[usIndex].usEmi = xDecryptionEmi;
		return SCR_ERROR_WRONG_EMI;
	}

	return SCR_NO_ERROR;
}

TScrStatus scrSetClearTextDecryptionKey
(
	TScrDecSessionHandler*  pxScrDecSessionHandler,
	TSize                    xScrDecryptionKeySize,
	const TUnsignedInt8*          pxScrDecryptionKey
)
{
	TUnsignedInt32		usIndex = 0;
	HERROR				hError = ERR_OK;

	HxLOG_Print("\n");

	if (pxScrDecSessionHandler == NULL)
	{
		HxLOG_Error("pxScrDecSessionHandler is NULL\n");
		return SCR_ERROR_WRONG_HANDLER;
	}

	if (pxScrDecryptionKey == NULL)
	{
		HxLOG_Error("pxScrDecryptionKey is NULL\n");
		return SCR_ERROR;
	}

	HxLOG_Print("pxScrDecSessionHandler = 0x%x\n", pxScrDecSessionHandler);
	HxLOG_Print("xScrDecryptionKeySize = 0x%x\n", xScrDecryptionKeySize);

	for (usIndex = 0; usIndex < MAX_SCR_POOL; usIndex++)
	{
		if (pxScrDecSessionHandler == &s_stScrSession[usIndex])
		{
			if (s_stScrSession[usIndex].bUsed)
			{
				break;
			}
			else
			{
				HxLOG_Error("not used handle!! \n");
				return SCR_ERROR_WRONG_HANDLER;
			}
		}
	}

	if (usIndex == MAX_SCR_POOL)
	{
		HxLOG_Error("not used handle!! \n");
		return SCR_ERROR_WRONG_HANDLER;
	}

#if defined(CONFIG_MW_CAS_NAGRA_OP_SES)
#if defined(CONFIG_MW_CAS_NAGRA_DALTEST)
	if (s_stScrSession[usIndex].usEmi != 0x4021 && s_stScrSession[usIndex].usEmi != 0x4023)
#else
	if (s_stScrSession[usIndex].usEmi != 0x4023)
#endif
#endif
	{
		return SCR_ERROR;
	}

	if (xScrDecryptionKeySize <= 0)
	{
		HxLOG_Error("xScrDecryptionKeySize size < 0 Fail!! \n");
		return SCR_ERROR;
	}

    if (s_stScrSession[usIndex].eScrType == SCR_PLAYER_MEDIA)
	{
#if defined(CONFIG_MW_CAS_NAGRA_OP_SES) //&& defined(CONFIG_MW_CAS_NAGRA_PRM_2_6)
		if(CA_NA_ScrExtCallKeyDataInfo(s_stScrSession[usIndex].usEmi, pxScrDecryptionKey, xScrDecryptionKeySize)==ERR_FAIL)
		{
			HxLOG_Error("CA_NA_ScrExtCallKeyDataInfo() is error\n");
		}
#endif
		if (hError != ERR_OK)
		{
			return SCR_ERROR;
		}
	}

	if(s_stScrSession[usIndex].pxScrDecryptionKey != NULL)
	{
		OxCAS_Free(s_stScrSession[usIndex].pxScrDecryptionKey);
	}

	s_stScrSession[usIndex].pxScrDecryptionKey = OxCAS_Malloc(xScrDecryptionKeySize);

	HxSTD_memcpy(s_stScrSession[usIndex].pxScrDecryptionKey, pxScrDecryptionKey, xScrDecryptionKeySize);

	s_stScrSession[usIndex].xScrDecryptionKeySize = xScrDecryptionKeySize;

	return SCR_NO_ERROR;
}


TScrStatus scrSet1LevelProtectedDecryptionKey
(
	TScrDecSessionHandler*  pxScrDecSessionHandler,
	TSize                    xL1ProtectingKeySize,
	const TUnsignedInt8*          pxL1ProtectingKey,
	TSize                    xProtectedDecryptionKeySize,
	const TUnsignedInt8*          pxProtectedDecryptionKey
)
{
        // NAGRA Certi. integrator 에게 해당 함수 사용해야 하는지 물어봐야 함 by mslee 2013/08/30
	return SCR_NO_ERROR;
}


TScrStatus scrSetLegacyDecryptionMode
(
	TScrDecSessionHandler*  pxScrDecSessionHandler
)
{
        // NAGRA Certi. integrator 에게 해당 함수 사용해야 하는지 물어봐야 함 by mslee 2013/08/30
	return SCR_NO_ERROR;
}

