/*******************************************************************
	File Description
********************************************************************/
/**
	@file	   	ir_status.h \n
	@brief	main \n

	Description:   \n
	Module: MW/CAS/ IRDETO \n

	Copyright (c) 2010 HUMAX Co., Ltd. \n
	All rights reserved. \n
*/




HUINT8	CAS_IR_DRV_GetNumberOfServices(IR_SVC_STATUS_INFO_MAIN **stSvcStatusMain);
HUINT8	CAS_IR_DRV_GetInformationOfElementary(HUINT16 usHandle, HUINT32 ulResourceId, IR_ELEM_STATE **stSvcStatusInfoElement);
HIR_ERR	CAS_IR_DRV_GetInformationOfEachService(HUINT32 eSvcType, HUINT16 usHandle, IR_SVC_STATUS_INFO_EACH *pstSvcStatusInfoEach);
HIR_ERR CAS_IR_DRV_SendServiceDefineOfEcmMonitor(HUINT16 ushSCELL, HUINT8 bStart);

void	CAS_IR_DRV_UpdateStatus_SC(HUINT32 eSvcType, ia_word16 wServiceHandle, HUINT32 Source, HUINT32 usRscId, HUINT32 usEcmOrEmmPID, ia_status_st status);
void	CAS_IR_DRV_UpdateStatus(HUINT32 eSvcType, ia_word16 wServiceHandle, HUINT32 Source, HUINT32 usEsPIDorRscID, HUINT32 usEcmOrEmmPID, ia_status_st status, HUINT32 ulResourceID);

HUINT32	CAS_IR_DRV_AnyProductQuery(HUINT32 resourceId);
HIR_ERR	CAS_IR_DRV_QueryScData(HUINT32 ulScRscId);
HUINT32	CAS_IR_DRV_HomingChannelQuery(HUINT32 resourceId);

#if defined(IRPLAT_FN_QUERY_VENDOR_ID)
HIR_ERR CAS_IR_DRV_QueryVendorID(HUINT32 resourceId);
#endif

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL) || defined(CONFIG_MW_CAS_IRDETO_RECOVERY_RESCAN_HOME)
HUINT32	xsvc_cas_IrGetHomeChannelInfo(IR_SC_HOME_CHANNEL_INFO *pHomeChannel);
#endif

#if defined(CONFIG_OP_DIGITURK)
HIR_ERR xsvc_cas_IrDrvRemoteChannelQuery(HUINT32 ulScRscId, HUINT8* pucBuf, HUINT32 ulLength);
HIR_ERR xsvc_cas_IrDrvQueryProductCheck(HUINT32 ulScRscId, HUINT8 ucGracePeriod, ia_bool bKeepTrack,
																		HUINT8 ucProductCount, HUINT32* pstProduct);
HIR_ERR xsvc_cas_IrDrvQueryTmsData(HUINT32 ulScRscId);
#endif

HIR_ERR	xsvc_cas_IrDrvQueryProductList(HUINT32 ulScRscId, HUINT8 ucGracePeriod, ia_bool bKeepTrack, ia_bool bAnySector, HUINT8 ucSectorCount, HUINT8* pucSectorList);
HIR_ERR	xsvc_cas_IrDrvQueryUserData(HUINT32 ulScRscId);


#define _____STATUS_SCREEN_____
HIR_ERR xsvc_cas_IrGetClientVersionString(HUINT8 *pucClientVersion);
HIR_ERR	xsvc_cas_IrGetClientVersion(HUINT16 *pusLength, HUINT8 **ppString);
#if defined(CONFIG_APUI_RAY_MINI)
HIR_ERR	xsvc_cas_IrGetClientVersionDevice(HUINT16 *pusLengthName, HUINT16 *pusLengthInfo,  HUINT8 **pStringName, HUINT8 **pStringInfo);
#endif

#define _____SOFTCELL_SERVICE_STATUS_____
HIR_ERR	xsvc_cas_IrSvcStatusGetNumberOfServices(IR_SVC_STATUS_INFO_MAIN **stSvcStatusMain, HUINT8 *pucNumberOfServices);
HIR_ERR	xsvc_cas_IrSvcStatusGetInformationOfEachService(HUINT32 eSvcType, HUINT16 usHandle, IR_SVC_STATUS_INFO_EACH *stSvcStatusEach);
HUINT8	xsvc_cas_IrSvcStatusGetInformationOfElement(HUINT16 usHandle, HUINT32 ulResourceId, IR_ELEM_STATE **stSvcStatusElement);
HIR_ERR	xsvc_cas_IrSvcStatusGetGlobalStatus(Handle_t hAction, IR_STATE *pGlobalStatus);
HIR_ERR	xsvc_cas_IrDrvGetGlobalStatusOfService(HUINT32 unPlayHandle, IR_STATE *pGlobalStatus);

#if	defined(IRPLAT_PVR_DRM) && defined(IRPLAT_SYS_PVR)
HIR_ERR	xsvc_cas_IrSvcStatusGetPlaybackDrmInfo(Handle_t hAction, IR_PVRDRM_INFO *pstPvrDrmInfo);
#endif

HIR_ERR	xsvc_cas_IrComponentsCount (HUINT16 *pusCount);
HIR_ERR	xsvc_cas_IrComponentsGetName (HUINT16 usIndex, HUINT8 *szName);
HIR_ERR	xsvc_cas_IrComponentsGetVersion (HUINT16 usIndex, HUINT8 *pucMajor, HUINT8 *pucMinor, HUINT32 *pulSubVer);

HIR_ERR	xsvc_cas_IrGetComponentsVersion(HUINT16	*pucCount, HUINT16 **pusLength, HUINT8 ***pString);

#define _____ECM_EMM_MONITORING_____
HIR_ERR	xsvc_cas_IrStartMonitoring(HUINT16 ushSCELL, HUINT8 ucType);
HIR_ERR	xsvc_cas_IrStopMonitoring(void);
#if defined(IRPLAT_SC_QUERY_EXTENDED_PRODUCT_LIST_ID) // prevent issue fixed
extern HIR_ERR CAS_IR_DRV_QueryExtendedProductList(HUINT32 ulScRscId, ia_bool bAnySector, HUINT8 ucSectorCount, HUINT8* pucSectorList);
#endif

