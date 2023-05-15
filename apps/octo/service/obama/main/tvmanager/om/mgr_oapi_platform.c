
#include	<hlib.h>
#include	<dlib.h>
#include <obama_rpcprotocol.h>

#include	<obama_int.h>
#include	<otl.h>

//#include	<di_crypt.h>
//#include	<pal_sys.h>

#include	<octo_common.h>

#include	<db_param.h>
#include	<svc_sys.h>
#include	<bus.h>

#include	<mgr_common.h>
#include	<mgr_action.h>

typedef struct
{
	HBOOL		bInitCrypt;
	HUINT8		*pSrcBuffer;
	HUINT8		*pDstBuffer;
	HUINT32		 ulBuffSize;
} MgrPlatformContext_t;

static MgrPlatformContext_t		s_platformContext;

#define	____OAPI_PLATFORM_RPC_____________________________________________________________________
static HERROR	obama_platform_data_EnCrypt (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, MgrPlatformContext_t *pstContext)
{
	HUINT32	ulSize, ulBlockNo;
	HUINT8	*pucData;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);


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

static HERROR	obama_platform_data_DeCrypt (HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, MgrPlatformContext_t *pstContext)
{
	HUINT32	ulSize, ulBlockNo;
	HUINT8	*pucData;
	HERROR	hErr = ERR_FAIL;
	HINT32	nRpcHandle = 0;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);

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

static HERROR	obama_platform_video_ExtractImage(HINT32 nClientHandle, HINT32 argc, const HxObject_t *argv[], HINT32 requestId, MgrPlatformContext_t *pstContext)
{
	HBOOL		bEncrypt;
	HCHAR		*pszUrl;
	HINT32		nWidth, nHeight;
	HUINT32		ulTime, ulVideoCodec;
	HUINT32		ulVideoPid;
	HCHAR		*pszTargetUrl;
	HUINT8		*pucBuffer;

	pszUrl		= HxOBJECT_STR(argv[0]);
	ulTime		= HxOBJECT_INT(argv[1]);
	nWidth		= HxOBJECT_INT(argv[2]);
	nHeight		= HxOBJECT_INT(argv[3]);
	ulVideoPid	= HxOBJECT_INT(argv[4]);
	ulVideoCodec= HxOBJECT_INT(argv[5]);
	bEncrypt	= (HxOBJECT_INT(argv[6])) ? TRUE : FALSE;
	pszTargetUrl= HxOBJECT_STR(argv[7]);

	pucBuffer = SVC_PVR_ExtractFrameFromFile((HCHAR*)pszUrl, ulTime, nWidth, nHeight, ulVideoPid, ulVideoCodec, bEncrypt);
	if (pucBuffer == NULL)
	{
		//OBAMA_Response(requestId, ERR_FAIL, NULL);
		return ERR_FAIL;
	}

	//	encoding JPEG
	HLIB_JPEG_CreateFile(pszTargetUrl, nWidth, nHeight, pucBuffer, HLIB_JPEG_ConvertBGRA2RGB);
	HLIB_STD_MemFree(pucBuffer);

	//OBAMA_Response(requestId, ERR_OK, NULL);

	return ERR_OK;
}

#define	____OAPI_CRYPT_________________________________________________________________________
static void		mgr_oapi_platform_Init(MgrPlatformContext_t *pstContext)
{
	HERROR	hErr;
	HINT32	nRpcHandle = 0;

	HxSTD_MemSet(pstContext, 0, sizeof(MgrPlatformContext_t));

	pstContext->pSrcBuffer  = (HUINT8 *)SVC_SYS_CryptMemAlloc(CRYPT_BUFFER_SIZE);
	pstContext->pDstBuffer  = (HUINT8 *)SVC_SYS_CryptMemAlloc(CRYPT_BUFFER_SIZE);
	pstContext->ulBuffSize  = (HUINT32)CRYPT_BUFFER_SIZE;

	hErr = OBAMA_RpcProtocol_GetRpcHandle(&nRpcHandle);
	if(hErr == ERR_OK)
	{
		HLIB_RPC_RegisterRequest (nRpcHandle, "platform.data.encrypt", "ib", (HxRPC_Func_t)obama_platform_data_EnCrypt, (void*)pstContext,
			"platform.data.crypt (uint32 blockno, int size, uint8 *data)");
		HLIB_RPC_RegisterRequest (nRpcHandle, "platform.data.decrypt", "ib", (HxRPC_Func_t)obama_platform_data_DeCrypt, (void*)pstContext,
			"platform.data.decrypt (uint32 blockno, int size, uint8 *data)");

		HLIB_RPC_RegisterCall (nRpcHandle, "platform.video.extract", "siiiiiis", (HxRPC_Func_t)obama_platform_video_ExtractImage, (void*)pstContext,
			"platform.video.extract (char *url, uint32 time, int width, int height, uint16 videopid, uint32 videocodec, int encrypted, char *outfile)");

	}


}

BUS_Result_t	PROC_OapiPlatform(HINT32 message, Handle_t hAction, HINT32 p1, HINT32 p2, HINT32 p3)
{
	BUS_Result_t	result = ERR_BUS_NO_ERROR;

	switch (message)
	{
	case eMEVT_BUS_CREATE:
		mgr_oapi_platform_Init(&s_platformContext);
		break;

	case eMEVT_BUS_DESTROY:
		break;

	default:
		break;
	}
	return (result == MESSAGE_BREAK) ? MESSAGE_BREAK : MESSAGE_PASS;
}


/* end of file */

