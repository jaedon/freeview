/**************************************************************
 *	@file		apk_network_pppoe.c
 *	appkit for network
 *
 *	http://www.humaxdigital.com
 *
 *	@data			2013-11-11
 *	@author			humax
 *	@brief			appkit for network
 **************************************************************/
#include 	<apk.h>
#include 	<oapi.h>
#include 	<napi.h>

#if defined(CONFIG_SUPPORT_NIMA)

#define FuncEnter HxLOG_Debug("   (+)[%s:%d] Enter!!\n",__FUNCTION__,__LINE__)
#define FuncLeave HxLOG_Debug("   (-)[%s:%d] Leave -- \n",__FUNCTION__,__LINE__)

/**************************************************************
	PPPoE public funtions
 **************************************************************/
HERROR		__apk_network_pppoe_copyInfo(NAPIPPPOEInfo_t *pstDstInfo, APKS_PPPOE_Info_t *pstSrcInfo)
{
	if( !pstSrcInfo )
	{
		return ERR_FAIL;
	}

	pstDstInfo->dev_id = pstSrcInfo->dev_id;
	HxSTD_MemCopy(pstDstInfo->szUserID, pstSrcInfo->szUserID, APKD_PPPOE_ID_LEN_MAX);
	HxSTD_MemCopy(pstDstInfo->szUserPW, pstSrcInfo->szUserPW, APKD_PPPOE_PW_LEN_MAX);

	return ERR_OK;
}

HERROR		APK_NETWORK_PPPOE_Connect(APKS_PPPOE_Info_t *pstPPPOEInfo)
{
	HERROR hRet = ERR_OK;
	NAPIPPPOEInfo_t pppoe_info;
	FuncEnter;

	if( !pstPPPOEInfo)
		return ERR_FAIL;

	HxSTD_MemSet(&pppoe_info, 0x00, sizeof(NAPIPPPOEInfo_t));

	HxLOG_Debug("   (+)[%s:%d] dev_id : %d\n",__FUNCTION__,__LINE__, pstPPPOEInfo->dev_id);

	hRet = __apk_network_pppoe_copyInfo(&pppoe_info, pstPPPOEInfo);
	if(hRet != ERR_OK)
		return ERR_FAIL;

	hRet = NAPI_PPPOE_Connect(&pppoe_info);

	FuncLeave;
	return hRet;
}

HERROR		APK_NETWORK_PPPOE_Disconnect(HINT32 nIndex)
{
	HERROR hRet = ERR_OK;
	FuncEnter;
	hRet = NAPI_PPPOE_Disconnect(nIndex);
	FuncLeave;
	return hRet;
}

HERROR		APK_NETWORK_PPPOE_GetInfo(APKS_PPPOE_Info_t *pstPPPOEInfo)
{
	HERROR hRet = ERR_OK;
	NAPIPPPOEInfo_t pppoe_info;
	FuncEnter;

	if( !pstPPPOEInfo)
		return ERR_FAIL;

	hRet = NAPI_PPPOE_GetInfo(&pppoe_info);

	if(hRet != ERR_OK)
		return ERR_FAIL;

	HxSTD_MemSet(&pppoe_info, 0x00, sizeof(NAPIPPPOEInfo_t));

	pstPPPOEInfo->dev_id = pppoe_info.dev_id;
	HxSTD_MemCopy(pstPPPOEInfo->szUserID, pppoe_info.szUserID, APKD_PPPOE_ID_LEN_MAX);
	HxSTD_MemCopy(pstPPPOEInfo->szUserPW, pppoe_info.szUserPW, APKD_PPPOE_PW_LEN_MAX);

	FuncLeave;
	return ERR_OK;
}

HERROR		APK_NETWORK_PPPOE_SetInfo(APKS_PPPOE_Info_t *pstPPPOEInfo)
{
	HERROR hRet = ERR_OK;
	NAPIPPPOEInfo_t pppoe_info;
	FuncEnter;

	if( !pstPPPOEInfo)
		return ERR_FAIL;

	HxSTD_MemSet(&pppoe_info, 0x00, sizeof(NAPIPPPOEInfo_t));
	hRet = __apk_network_pppoe_copyInfo(&pppoe_info, pstPPPOEInfo);

	if(hRet != ERR_OK)
		return ERR_FAIL;

	hRet = NAPI_PPPOE_SetInfo(&pppoe_info);
	if(hRet != ERR_OK )
		return ERR_FAIL;

	FuncLeave;
	return hRet;
}

HERROR	APK_NETWORK_PPPOE_GetOnOff(HINT32 nIndex, HBOOL *bOn)
{
	HERROR hRet = ERR_OK;
	FuncEnter;

	hRet = NAPI_PPPOE_GetOnOff(nIndex, bOn);

	FuncLeave;
	return hRet;
}

HERROR	APK_NETWORK_PPPOE_SetOnOff(HINT32 nIndex, HBOOL bOn)
{
	HERROR hRet = ERR_OK;
	FuncEnter;

	hRet = NAPI_PPPOE_SetOnOff(nIndex, bOn);

	FuncLeave;
	return hRet;
}



#else



HERROR		APK_NETWORK_PPPOE_Connect(APKS_PPPOE_Info_t *pstPPPOEInfo)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_PPPOE_Disconnect(HINT32 nIndex)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_PPPOE_GetInfo(APKS_PPPOE_Info_t *pstPPPOEInfo)
{
	return ERR_FAIL;
}

HERROR		APK_NETWORK_PPPOE_SetInfo(APKS_PPPOE_Info_t *pstPPPOEInfo)
{
	return ERR_FAIL;
}

HERROR	APK_NETWORK_PPPOE_GetOnOff(HINT32 nIndex, HBOOL *bOn)
{
	return ERR_FAIL;
}

HERROR	APK_NETWORK_PPPOE_SetOnOff(HINT32 nIndex, HBOOL bOn)
{
	return ERR_FAIL;
}


#endif
