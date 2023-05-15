#include	<hlib.h>
#include	<dlib.h>
#include <obama_rpcprotocol.h>
#include	<obama_int.h>
#include	<otl.h>

//#include	<di_crypt.h>
//#include	<pal_sys.h>

#include	<octo_common.h>
#include	<db_param.h>
#include	<bus.h>

#include	<mgr_common.h>
#include	<mgr_action.h>

#include 	<svc_sys.h>
typedef struct
{
	HBOOL		bInitCrypt;
	HUINT8		*pSrcBuffer;
	HUINT8		*pDstBuffer;
	HUINT32		 ulBuffSize;
} OxPlatformContext_t;

static OxPlatformContext_t		s_platformContext;

#define	____OAPI_PLATFORM_RPC_____________________________________________________________________

static HERROR	om_platform_GetKey (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OxPlatformContext_t *pstContext)
{
	HUINT8	*pucData = HLIB_STD_MemDup(SVC_SYS_GetEncryptionKey(), 8);
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	HxLOG_Warning("Key !! (%x %x %x %x %x %x %x %x)\n", pucData[0], pucData[1], pucData[2], pucData[3], pucData[4], pucData[5], pucData[6], pucData[7]);

	HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_OK, "b", pucData , 8);

	OxAP_Free(pucData);
	return ERR_OK;
}

static HERROR	om_platform_Encrypt (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OxPlatformContext_t *pstContext)
{
	HUINT32	ulSize, ulBlockNo;
	HUINT8	*pucData;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	ulBlockNo   = HxOBJECT_INT(argv[0]);
	ulSize      = HxOBJECT_BIN_SIZE(argv[1]);
	pucData     = HxOBJECT_BIN_DATA(argv[1]);

	HxLOG_Debug("request[%d] ulSize[%d]\n", requestId, ulSize);
	if (ulSize > pstContext->ulBuffSize)
	{
		HxLOG_Error("Crypt buffer size is too long. [%d > %d]\n", ulSize, pstContext->ulBuffSize);
		HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_FAIL, NULL);
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstContext->pSrcBuffer, pucData, ulSize);
	if (SVC_SYS_Encrypt(SVC_SYS_GetEncryptionKey(), FALSE, pucData, pstContext->pDstBuffer, ulSize) != ERR_OK)
	{
		HxLOG_Error("Decrypt failed!\n");
		HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_FAIL, NULL);
		return ERR_FAIL;
	}

	HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_OK, "ib", ulBlockNo, pstContext->pDstBuffer, ulSize);

	return ERR_OK;
}

static HERROR	om_platform_Decrypt (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OxPlatformContext_t *pstContext)
{
	HUINT32	ulSize, ulBlockNo;
	HUINT8	*pucData;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	ulBlockNo   = HxOBJECT_INT(argv[0]);
	ulSize      = HxOBJECT_BIN_SIZE(argv[1]);
	pucData     = HxOBJECT_BIN_DATA(argv[1]);

	HxLOG_Debug("nSessionId[%d] ulSize[%d]\n", requestId, ulSize);
	if (ulSize > pstContext->ulBuffSize)
	{
		HxLOG_Error("Crypt buffer size is too long. [%d > %d]\n", ulSize, pstContext->ulBuffSize);
		HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_FAIL, NULL);
		return ERR_FAIL;
	}

	HxSTD_MemCopy(pstContext->pSrcBuffer, pucData, ulSize);
	if (SVC_SYS_Decrypt(SVC_SYS_GetEncryptionKey(), FALSE, pstContext->pSrcBuffer, pstContext->pDstBuffer, ulSize) != ERR_OK)
	{
		HxLOG_Error("Decrypt failed!\n");
		HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_FAIL, NULL);
		return ERR_FAIL;
	}
	HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_OK, "ib", ulBlockNo, pstContext->pDstBuffer, ulSize);

	return ERR_OK;
}

static HERROR	om_platform_ExtractThumbnail(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, OxPlatformContext_t *pstContext)
{
	HBOOL		bEncrypt;
	HCHAR		*pszUrl;
	HINT32		nWidth, nHeight;
	HUINT32		ulTime, ulVideoCodec;
	HUINT32		ulVideoPid;
	HCHAR		*pszTargetUrl;
#if defined(CONFIG_MW_MM_PVR)
	HUINT8		*pucBuffer;
#endif
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr != ERR_OK)
	{
		HxLOG_Error("OBAMA_RpcProtocol_GetRpcHandle failed. \n");
	}

	pszUrl		= HxOBJECT_STR(argv[0]);
	ulTime		= HxOBJECT_INT(argv[1]);
	nWidth		= HxOBJECT_INT(argv[2]);
	nHeight		= HxOBJECT_INT(argv[3]);
	ulVideoPid	= HxOBJECT_INT(argv[4]);
	ulVideoCodec= HxOBJECT_INT(argv[5]);
	bEncrypt	= (HxOBJECT_INT(argv[6])) ? TRUE : FALSE;
	pszTargetUrl= HxOBJECT_STR(argv[7]);
#if defined(CONFIG_MW_MM_PVR)
	pucBuffer = SVC_PB_ExtractThumbnailFromFile((HCHAR*)pszUrl, ulTime, nWidth, nHeight, ulVideoPid, ulVideoCodec, bEncrypt);
	if (pucBuffer == NULL)
	{
		//HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_FAIL, NULL);
		return ERR_FAIL;
	}

	//	encoding JPEG
	HLIB_JPEG_CreateFile(pszTargetUrl, nWidth, nHeight, pucBuffer, HLIB_JPEG_ConvertBGRA2RGB);
	HLIB_STD_MemFree(pucBuffer);
#endif

#if defined(CONFIG_DROP_ROOT_PRIVILEGES)
	if (HLIB_EXTRA_ChownWithParam(pszTargetUrl, UID_obama, GID_pvr) != ERR_OK)
	{
		HxLOG_Error("Fail to chown path = %s \n", pszTargetUrl);
	}

	if (HLIB_EXTRA_Chmod(pszTargetUrl, 0640) != ERR_OK)
	{
		HxLOG_Error("	Fail to chmod path = %s \n", pszTargetUrl);
	}
#endif

	//HLIB_RPC_Response(nRpcHandle, nClientHandle, requestId, ERR_OK, NULL);
	return ERR_OK;
}

STATIC void		om_platform_onKeyUpdate(void)
{
	HERROR			hErr = ERR_FAIL;
	HINT32			nRpcHandle = 0;



	HUINT8	*pucData = HLIB_STD_MemDup(SVC_SYS_GetEncryptionKey(), 8);

	if(pucData == NULL)
	{
		return;
	}

	HxLOG_Debug("Key !! (%x %x %x %x %x %x %x %x)\n", pucData[0], pucData[1], pucData[2], pucData[3], pucData[4], pucData[5], pucData[6], pucData[7]);

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if (hErr==ERR_OK)
	{
		HLIB_RPC_Notify(nRpcHandle, "platform.data.onKeyUpdated", "b", pucData, 8);
	}

	if (pucData)
		OxMGR_Free(pucData);
}


#define	____OAPI_CRYPT_________________________________________________________________________
void		OM_PLATFORM_Init(void)
{
	OxPlatformContext_t	*pstContext = &s_platformContext;
	HERROR	hErr;
	HINT32	nRpcHandle = 0;


	HxSTD_MemSet(pstContext, 0, sizeof(OxPlatformContext_t));

	pstContext->pSrcBuffer  = SVC_SYS_CryptMemAlloc(CRYPT_BUFFER_SIZE);
	pstContext->pDstBuffer  = SVC_SYS_CryptMemAlloc(CRYPT_BUFFER_SIZE);
	pstContext->ulBuffSize  = CRYPT_BUFFER_SIZE;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		HLIB_RPC_RegisterRequest (nRpcHandle, "platform.data.encrypt", "ib", (HxRPC_Func_t)om_platform_Encrypt, (void*)pstContext,
			"platform.data.crypt (uint32 blockno, int size, uint8 *data)");
		HLIB_RPC_RegisterRequest (nRpcHandle, "platform.data.decrypt", "ib", (HxRPC_Func_t)om_platform_Decrypt, (void*)pstContext,
			"platform.data.decrypt (uint32 blockno, int size, uint8 *data)");

		HLIB_RPC_RegisterCall (nRpcHandle, "platform.video.extract", "siiiiiis", (HxRPC_Func_t)om_platform_ExtractThumbnail, (void*)pstContext,
			"platform.video.extract (char *url, uint32 time, int width, int height, uint16 videopid, uint32 videocodec, int encrypted, char *outfile)");

		HLIB_RPC_RegisterRequest (nRpcHandle, "platform.data.getkey", NULL, (HxRPC_Func_t)om_platform_GetKey, (void*)pstContext,
			"get encryption key .. platform.data.getkey uint8 *data)");
	}

}

/* end of file */


