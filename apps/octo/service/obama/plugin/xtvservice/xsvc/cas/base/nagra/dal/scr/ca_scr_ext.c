/**************************************************************************************************/
#define ________CA_SCR_EXT_Private_Include________________________________________________
/**************************************************************************************************/
#include "vkernel.h"
#include "na_glob.h"

STATIC NaPrmKeyInfoCallback_t s_stKeyDataCallback;

// local function

static void local_na_scrext_CallKeyDataInfoCallback(TUnsignedInt16 xEmi, HUINT8 *pucKey, HUINT32 ulKeyLength)
{
	NaPrmKeyInfo_t	stKeyInfo;

	/* VOD Service 일 경우 imma에서 decryption 동작을 하므로 key 정보를 TV magager 단으로 올려 보내 줌, 일단 PVR일경우에 대해서는 고려치 않음. */
	if(s_stKeyDataCallback.pfnKeyDataCallback != NULL)
	{
		stKeyInfo.usEmi = xEmi;
		stKeyInfo.xTSid = s_stKeyDataCallback.xTSid;
		stKeyInfo.ulPlaybackHandle = s_stKeyDataCallback.ulPlaybackHandle;
		HxSTD_MemCopy(&stKeyInfo.ucKey, pucKey, ulKeyLength);
		stKeyInfo.ulKeyLength = ulKeyLength;

		s_stKeyDataCallback.pfnKeyDataCallback((void *)&stKeyInfo);
	}
}

static void local_na_scrext_RegisterKeyDataCallback(TTransportSessionId xTSid, HUINT32 ulPlaybackHandle, PrmKeyInfo_Calback_Function fnKeyDataCallback)
{
	s_stKeyDataCallback.xTSid = xTSid;
	s_stKeyDataCallback.ulPlaybackHandle = ulPlaybackHandle;
	s_stKeyDataCallback.pfnKeyDataCallback = fnKeyDataCallback;
}

// global function

HERROR CA_NA_ScrExtCallKeyDataInfo(TUnsignedInt16 xEmi, HUINT8 *pucKey, HUINT32 ulKeyLength)
{
	if(pucKey == NULL)
	{
		HxLOG_Error("pucKey is NULL\n");
		return ERR_FAIL;
	}

	local_na_scrext_CallKeyDataInfoCallback(xEmi, pucKey, ulKeyLength);

	return ERR_OK;
}

HERROR CAS_NA_ScrExtRegisterKeyDataCallback(TTransportSessionId xTSid, HUINT32 ulPlaybackHandle, PrmKeyInfo_Calback_Function fnKeyDataCallback)
{
	local_na_scrext_RegisterKeyDataCallback(xTSid, ulPlaybackHandle, fnKeyDataCallback);

	return ERR_OK;
}

HERROR  CAS_NA_ScrExtUnRegisterKeyDataCallback(void)
{
	local_na_scrext_RegisterKeyDataCallback(0, 0, NULL);

	return ERR_OK;
}

