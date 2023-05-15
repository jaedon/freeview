
#include	<hlib.h>
#include	<oapi.h>
#include	<obama_int.h>

#include "../oapi_mem.h"

static HBOOL s_bKeyInit = FALSE;
static HUINT8 s_ucKey[8];


#if	0			//	Notify시 Sem Lock걸림. DaMa와 같은 이슈..
static void		oapi_platform_data_OnResponse (HERROR hErr, HINT32 argc, const HxObject_t *argv[], HINT32 nRequestId, void *userdata)
{
	HUINT32			ulSize, ulBlockNo;
	HUINT8			*pucData;
	HxPair_t		*pair;
	HxReturnFunc_t	ret;

	HxLOG_Debug("%s(err:%d, argc:%d, userdata:0x%X)\n", __FUNCTION__, err, argc, userdata);

	pair = (HxPair_t *)userdata;
	if (pair == NULL)
		return;

	ulBlockNo = HxOBJECT_INT(argv[0]);
	ulSize    = HxOBJECT_BIN_SIZE(argv[1]);
	pucData   = HxOBJECT_BIN_DATA(argv[1]);
	ret  = (HxReturnFunc_t)pair->left;
	if (ret)
	{
		ret(err, pair->right, ulBlockNo, ulSize, pucData);
	}
	OxMEM_Free(pair);
}

HINT32	OAPI_PLATFORM_DATA_CryptBufferSize(void)
{
	return CRYPT_BUFFER_SIZE;
}

HINT32	OAPI_PLATFORM_DATA_Encrypt(HUINT32 ulBlockNo, HUINT32 ulSize, HUINT8 *pucData, HxReturnFunc_t ret, void *userdata)
{
	HxPair_t	*pair = NULL;

	HxLOG_Debug("ulSize[%d] \n", ulSize);

	if (ret)
	{
		pair = (HxPair_t*)OxMEM_Malloc(sizeof(HxPair_t));
		if (pair)
		{
			pair->left = ret;
			pair->right = userdata;
		}
	}

	return HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_platform_data_OnResponse, pair, "platform.data.encrypt", "ib", ulBlockNo, pucData, ulSize);
}

HINT32	OAPI_PLATFORM_DATA_Decrypt(HUINT32 ulBlockNo, HUINT32 ulSize, HUINT8 *pucData, HxReturnFunc_t ret, void *userdata)
{
	HxPair_t	*pair = NULL;

	HxLOG_Debug("ulSize[%d] \n", ulSize);

	if (ret)
	{
		pair = (HxPair_t*)OxMEM_Malloc(sizeof(HxPair_t));
		if (pair)
		{
			pair->left = ret;
			pair->right = userdata;
		}
	}

	return HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_platform_data_OnResponse, pair, "platform.data.decrypt", "ib", ulBlockNo, pucData, ulSize);
}
#endif

HINT32	OAPI_PLATFORM_DATA_Decrypt(HUINT32 ulBlockNo,HUINT8 *pucData,HUINT32 ulSize , HxRPC_Response_t pFunc,void *userdata)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
		HLIB_RPC_Request (nRpcHandle, pFunc, userdata, "platform.data.decrypt", "ib",ulBlockNo, pucData,ulSize);
	return hErr;
}

void	OAPI_PLATFORM_VIDEO_ExtractFrame(HCHAR *pszUrl, HUINT32 ulTime, HINT32 nWidth, HINT32 nHeight, HUINT16 usVideoPid, HUINT32 ulVideoCodec, HBOOL bEncrypt, HCHAR *pszOutFile)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);
	HLIB_RPC_Call (nRpcHandle, "platform.video.extract", "siiiiiis", pszUrl, ulTime, nWidth, nHeight, usVideoPid, ulVideoCodec, bEncrypt, pszOutFile);
	return ;
}

HERROR	OAPI_PLATFORM_GetKey(HUINT8 *pucKey)
{
	if (!pucKey)
	{
		HxLOG_Error("puckey is null.. \n");
		return ERR_FAIL;
	}

	if (TRUE == s_bKeyInit && FALSE == HxSTD_StrEmpty(s_ucKey))
	{
		HxSTD_MemCopy(pucKey, s_ucKey, 8);
		HxLOG_Print("[%s] [%d] Key !! (%x %x %x %x %x %x %x %x)\n",__FUNCTION__,__LINE__, s_ucKey[0], s_ucKey[1], s_ucKey[2], s_ucKey[3], s_ucKey[4], s_ucKey[5], s_ucKey[6], s_ucKey[7]);

	}
	else
	{
		HxLOG_Error("empty key.. \n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

STATIC void	oapi_platform_onKeyReceived(HERROR hErr, HINT32 nArgc, const HxObject_t *apArgv[], HINT32 nRequestId, void *userdata)
{
	HUINT8	*pucKey;

	pucKey = HxOBJECT_BIN_DATA(apArgv[0]);

	if (FALSE == HxSTD_StrEmpty(pucKey))
	{
		HxSTD_MemCopy(s_ucKey, pucKey, 8);
		HxLOG_Print("[%s] [%d] Key is updated!! (%x %x %x %x %x %x %x %x)\n",__FUNCTION__,__LINE__, s_ucKey[0], s_ucKey[1], s_ucKey[2], s_ucKey[3], s_ucKey[4], s_ucKey[5], s_ucKey[6], s_ucKey[7]);

		s_bKeyInit = TRUE;
	}

	return ;
}

void	OAPI_PLATFORM_KeyInit(void)
{
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OAPI_RpcProtocol_GetRpcHandle(&nRpcHandle);

	if (!s_bKeyInit)
	{
		HLIB_RPC_Request (nRpcHandle, (HxRPC_Response_t)oapi_platform_onKeyReceived, (void *)NULL, "platform.data.getkey", NULL);
	}

}

