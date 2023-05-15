
/*-------------------------- System Includes --------------------------------*/
#include "VMClient.h"
#include "vmx_vr_iptvh_testcmd_join.h"
#include<svc_pipe.h>
/*-------------------------- Local Includes ---------------------------------*/

/* input parameter for VCAS */
#define VMXIPTV_TEST_DEAULT_BOOT_IPADDRESS				"public2.verimatrix.com"
#define VMXIPTV_TEST_DEFAULT_BOOT_PORT					12686
#define VMXIPTV_TEST_DEFAULT_VCAS_VERSION				eVcasVersion_340
#define VMXIPTV_TEST_DEFAULT_STORE						"/mnt/nfs/Verimatrix.store" // test only

#define VMXIPTV_SUCCESS						0
#define VMXIPTV_VERSION_STRING_MAX_LEN   	50
#define VMXIPTV_MAX_SERVICE_NUM				10

enum
{
	eVMXIPTV_BASIC_TYPE= 0,
	eVMXIPTV_CWPP_TYPE,
	eVMXIPTV_REFSW_TYPE,
	eVMXIPTV_MAX_TYPE
};

typedef struct
{
	HUINT8 ucUserParamFlag;
	HUINT8 usUserParamData[50];
} VmxIptv_UserParam_t;

typedef struct
{
	HUINT8 ucUniqueId[128];
	HUINT16 usIdSize;
} VmxIptv_UniqueId_t;

typedef struct
{
	eEncryptionAlgorithm_t eEncryptionAlgorithm;
	VmxIptv_UserParam_t stUserParam;
	HUINT8 usChannelRatingLevel;
	void		* pCtx;
} VmxIptv_Service_t;

typedef struct
{
	void		* pCtx;

	HUINT8  ucTestType;
	HBOOL	bOnlineMode;
	HUINT8 	ucBootIpAddress[50];
	HUINT16	usBootPort;
	HUINT8 	pucVmxStore[50];

	HUINT16 usCASystemID;
	HUINT8 ucClientVerStr[VMXIPTV_VERSION_STRING_MAX_LEN];
	VmxIptv_UniqueId_t stUniqueId;
	eVcasVersion_t 	eVcasVersion;
	HUINT8  ucCurrentRatingLevel;

	HUINT8  fname_in[50];
	HUINT8  fname_out[50];

	VmxIptv_Service_t stVmxIptvService[VMXIPTV_MAX_SERVICE_NUM]; // 일단 현재 service는 1개로 간주한다.
} VmxIptv_Manager_t;

STATIC VmxIptv_Manager_t  s_stVmxIPtv_Manager;

STATIC VmxIptv_Manager_t *vmxiptv_cmd_GetMgr(void);
STATIC HBOOL vmxiptv_cmd_MgrInit(VmxIptv_Manager_t * pVmxIptvMgr, HBOOL bOnlineMode, HUINT8 *pucIpBootAddress, HUINT16 usBootPort,  HUINT8 *pucVmxStore, eVcasVersion_t eVcasVersion);
STATIC void vmxiptv_cmd_DbgPrintMgrr(VmxIptv_Manager_t *pVmxIptvMgr);

STATIC void vmxiptv_cmd_JoinWithExample(void *pContext, VmxIptv_Manager_t * pVmxIptvMgr)
{
#if defined(EXAMPLE)
	if( pVmxIptvMgr == NULL )
	{
		HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
		return;
	}

//	vmx_vr_iptvh_testcmd_ExampleJoinRefsw(pContext, &pVmxIptvMgr->fname_in[0], &pVmxIptvMgr->ucBootIpAddress[0], pVmxIptvMgr->usBootPort, &pVmxIptvMgr->pucVmxStore[0], pVmxIptvMgr->eVcasVersion, pVmxIptvMgr->bOnlineMode);
//	vmx_vr_iptvh_testcmd_ExampleJoinRefsw(pVmxIptvMgr->pCtx, &pVmxIptvMgr->fname_in[0], &pVmxIptvMgr->ucBootIpAddress[0], pVmxIptvMgr->usBootPort, &pVmxIptvMgr->pucVmxStore[0], pVmxIptvMgr->eVcasVersion, pVmxIptvMgr->bOnlineMode);
	vmx_vr_iptvh_testcmd_ExampleJoinBasic(pVmxIptvMgr->pCtx, &pVmxIptvMgr->fname_in[0], &pVmxIptvMgr->fname_out[0], &pVmxIptvMgr->ucBootIpAddress[0], pVmxIptvMgr->usBootPort, &pVmxIptvMgr->pucVmxStore[0], pVmxIptvMgr->eVcasVersion, pVmxIptvMgr->bOnlineMode);
	#if 0
	if(pVmxIptvMgr->ucTestType == eVMXIPTV_REFSW_TYPE)
	{
		vmx_vr_iptvh_testcmd_ExampleJoinRefsw(pVmxIptvMgr->pCtx, &pVmxIptvMgr->fname_in[0], &pVmxIptvMgr->ucBootIpAddress[0], pVmxIptvMgr->usBootPort, &pVmxIptvMgr->pucVmxStore[0], pVmxIptvMgr->eVcasVersion, pVmxIptvMgr->bOnlineMode);
	}
	else if(pVmxIptvMgr->ucTestType == eVMXIPTV_CWPP_TYPE)
	{
//		example_join_cwpp(pVmxIptvMgr->pCtx, &pVmxIptvMgr->fname_in[0], &pVmxIptvMgr->ucBootIpAddress[0], pVmxIptvMgr->usBootPort, &pVmxIptvMgr->pucVmxStore[0], pVmxIptvMgr->eVcasVersion, pVmxIptvMgr->bOnlineMode);
	}
	else if(pVmxIptvMgr->ucTestType == eVMXIPTV_BASIC_TYPE)
	{
		example_join_basic(pVmxIptvMgr->pCtx, &pVmxIptvMgr->fname_in[0], &pVmxIptvMgr->fname_out[0], &pVmxIptvMgr->ucBootIpAddress[0], pVmxIptvMgr->usBootPort, &pVmxIptvMgr->pucVmxStore[0], pVmxIptvMgr->eVcasVersion, pVmxIptvMgr->bOnlineMode);
	}
	#endif
#endif
}

STATIC VmxIptv_Manager_t *vmxiptv_cmd_GetMgr(void)
{
	return &s_stVmxIPtv_Manager;
}


STATIC int vmxiptv_cmd_EcmCallback(void *pUserParam, descramble_param_t *dscParam)
{
	HUINT16 usCnt = 0;

	HxLOG_Print("[%s:%d] callback...........................\n", __FUNCTION__, __LINE__);
	if(pUserParam)
	{
		VmxIptv_UserParam_t *userparam;
		userparam = (VmxIptv_UserParam_t *)pUserParam;
		HxLOG_Print("ucUserParamFlag = %d, usUserParamData = %s\n", userparam->ucUserParamFlag, userparam->usUserParamData);
	}
	else
	{
		HxLOG_Print("[%s:%d] pUserParam is not exist\n", __FUNCTION__, __LINE__);
	}

	if( dscParam == NULL )
	{
		HxLOG_Print("[%s:%d] dscParam error\n", __FUNCTION__, __LINE__);
		return FALSE;
	}
	HxLOG_Print(".......... print dscParam ..........\n");
	HxLOG_Print("keylength = %d\n", dscParam->keylength);
	HxLOG_Print("iCwp_data_length = %d\n", dscParam->iCwp_data_length);
	if( dscParam->pCwp_data == NULL )
	{
		HxLOG_Print("CW - Clear\n");
	}
	else
	{
		HxLOG_Print("CW - Encrypted\n");
	}
	HxLOG_Print("encAlg = %d\n", dscParam->encAlg);
	//dscParam->odd
	{
		HxLOG_Print("odd : control_flag = %d\n", dscParam->odd.control_flag);
		if(dscParam->odd.control_flag == 1)
		{
			for(usCnt=0; usCnt<32; usCnt++)
			{
				if(usCnt%16==0)
					HxLOG_Print("\n");
				HxLOG_Print("0x%02x, ", dscParam->odd.iv[usCnt]);
			}
			for(usCnt=0; usCnt<32; usCnt++)
			{
				if(usCnt%16==0)
					HxLOG_Print("\n");
				HxLOG_Print("0x%02x, ", dscParam->odd.key[usCnt]);
			}
		}
	}
	//dscParam->even
	{
		HxLOG_Print("even : control_flag = %d\n", dscParam->even.control_flag);
		if(dscParam->even.control_flag == 1)
		{
			for(usCnt=0; usCnt<32; usCnt++)
			{
				if(usCnt%16==0)
					HxLOG_Print("\n");
				HxLOG_Print("0x%02x, \n", dscParam->even.iv[usCnt]);
			}
			for(usCnt=0; usCnt<32; usCnt++)
			{
				if(usCnt%16==0)
					HxLOG_Print("\n");
				HxLOG_Print("0x%02x, \n", dscParam->even.key[usCnt]);
			}
		}
	}
	HxLOG_Print("\n");

	return TRUE;
}

STATIC void vmxiptv_cmd_DecodeCallback( void *pUserParam, unsigned long lChannel,  eVmDecodeResult eReason)
{
	HUINT16 i;
	HUINT8 reason_str[50];

	switch( eReason )
	{
		case eVmDecodeInit:
			sprintf(reason_str, "%s", "eVmDecodeInit");
			break;
		case eVmDecodeOk:
			sprintf(reason_str, "%s", "eVmDecodeOk");
			break;
		case eVmDecodeFailurePinRequired:
			sprintf(reason_str, "%s", "eVmDecodeFailurePinRequired");
			break;
		case eVmDecodeFailureKeyNotAvailable:
			sprintf(reason_str, "%s", "eVmDecodeFailureKeyNotAvailable");
			break;
		case eVmDecodeFailureBlackout:
			sprintf(reason_str, "%s", "eVmDecodeFailureBlackout");
			break;
		case eVmDecodeFailureOther:
			sprintf(reason_str, "%s", "eVmDecodeFailureOther");
			break;
		default:
			sprintf(reason_str, "%s", "default");
			break;
	}

	HxLOG_Print( "ContextIdx[%d] : DecodeFailCB on %lu  (0x%08x) for reason %s(%d)\n" , i,  lChannel, lChannel, reason_str, eReason);

	#if 0
	if ( (eReason == eVmDecodeFailurePinRequired) && pCtrl->bOverrideRatingLevel )
	{
		VMOverrideRatingLevel( pCtrl->pCtx );
		HxLOG_Print("Overriding Rating level\n");
	}
   	#endif
}

STATIC void vmxiptv_cmd_FingerprintingCallback(void *pUserParam, int FingerPrintingdataLength, void *pFingerPrintingData, int localdataLength, void *pLocalData)
{
	HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Print("localdataLength = %d\n", localdataLength);

	if(pUserParam)
	{
		VmxIptv_UserParam_t *userparam;
		userparam = (VmxIptv_UserParam_t *)pUserParam;
		HxLOG_Print("ucUserParamFlag = %d, usUserParamData = %s\n", userparam->ucUserParamFlag, userparam->usUserParamData);
	}
	else
	{
		HxLOG_Print("[%s:%d] pUserParam is not exist\n", __FUNCTION__, __LINE__);
	}

	if(pFingerPrintingData)
	{
		HxLOG_Print("pFingerPrintingData is exist\n");
	}
	else
	{
		HxLOG_Print("pFingerPrintingData is not exist\n");
	}

	HxLOG_Print("localdataLength = %d\n", localdataLength);
	if(pLocalData)
	{
		HxLOG_Print("pLocalData is exist\n");
	}
	else
	{
		HxLOG_Print("pLocalData is not exist\n");
	}
}

STATIC void vmxiptv_cmd_UserParamCallback(void *pUserParam, int userdataLength, void *pUserDataValues)
{
	HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Print("localdataLength = %d\n", userdataLength);
	if(pUserParam)
	{
		VmxIptv_UserParam_t *userparam;
		userparam = (VmxIptv_UserParam_t *)pUserParam;
		HxLOG_Print("ucUserParamFlag = %d, usUserParamData = %s\n", userparam->ucUserParamFlag, userparam->usUserParamData);
	}
	else
	{
		HxLOG_Print("[%s:%d] pUserParam is not exist\n", __FUNCTION__, __LINE__);
	}

	if(pUserDataValues)
	{
		HxLOG_Print("pUserDataValues is exist\n");
	}
	else
	{
		HxLOG_Print("pUserDataValues is not exist\n");
	}
}

STATIC void vmxiptv_cmd_CopyControlCallback(void *pUserParam, int dataLength, void *pCopyControlData)
{
	HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Print("dataLength = %d\n", dataLength);
	if(pUserParam)
	{
		VmxIptv_UserParam_t *userparam;
		userparam = (VmxIptv_UserParam_t *)pUserParam;
		HxLOG_Print("ucUserParamFlag = %d, usUserParamData = %s\n", userparam->ucUserParamFlag, userparam->usUserParamData);
	}
	else
	{
		HxLOG_Print("[%s:%d] pUserParam is not exist\n", __FUNCTION__, __LINE__);
	}

	if(pCopyControlData)
	{
		HxLOG_Print("pCopyControlData is exist\n");
	}
	else
	{
		HxLOG_Print("pCopyControlData is not exist\n");
	}
}

STATIC void vmxiptv_cmd_OpaqueDataCallback(void*pUserParam, unsigned int nDataSize, void *pIrdOpaqueData )
{
	HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Print("nDataSize = %d\n", nDataSize);
	if(pUserParam)
	{
		VmxIptv_UserParam_t *userparam;
		userparam = (VmxIptv_UserParam_t *)pUserParam;
		HxLOG_Print("ucUserParamFlag = %d, usUserParamData = %s\n", userparam->ucUserParamFlag, userparam->usUserParamData);
	}
	else
	{
		HxLOG_Print("[%s:%d] pUserParam is not exist\n", __FUNCTION__, __LINE__);
	}

	if(pIrdOpaqueData)
	{
		HxLOG_Print("pIrdOpaqueData is exist\n");
	}
	else
	{
		HxLOG_Print("pIrdOpaqueData is not exist\n");
	}
}

STATIC void vmxiptv_cmd_OSDCallback(void *pUserParam, unsigned int nDataSize, void *pOsdMessage )
{
	HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Print("nDataSize = %d\n", nDataSize);
	if(pUserParam)
	{
		VmxIptv_UserParam_t *userparam;
		userparam = (VmxIptv_UserParam_t *)pUserParam;
		HxLOG_Print("ucUserParamFlag = %d, usUserParamData = %s\n", userparam->ucUserParamFlag, userparam->usUserParamData);
	}
	else
	{
		HxLOG_Print("[%s:%d] pUserParam is not exist\n", __FUNCTION__, __LINE__);
	}

	if(pOsdMessage)
	{
		HxLOG_Print("pOsdMessage is exist\n");
	}
	else
	{
		HxLOG_Print("pOsdMessage is not exist\n");
	}
}

STATIC void vmxiptv_cmd_SWDecryptonCallback(void * pvArg, unsigned char * pData, unsigned long dwDataLen )
{
	HxLOG_Print("[%s:%d]\n", __FUNCTION__, __LINE__);
	HxLOG_Print("dwDataLen = %d\n", dwDataLen);
	if(pvArg)
	{
		HxLOG_Print("[%s:%d] pvArg is exist\n", __FUNCTION__, __LINE__);
	}
	else
	{
		HxLOG_Print("[%s:%d] pvArg is not exist\n", __FUNCTION__, __LINE__);
	}

	if(pData)
	{
		HxLOG_Print("pData is exist\n");
	}
	else
	{
		HxLOG_Print("pData is not exist\n");
	}
}

STATIC void vmxiptv_cmd_RegisterCallback(VmxIptv_Manager_t *pVmxIptvMgr)
{
	if( pVmxIptvMgr == NULL )
	{
		HxLOG_Print("[%s:%d] pVmxIptvMgr is error\n", __FUNCTION__, __LINE__);
		return;
	}

	VMRegisterUserdataCallback(pVmxIptvMgr->pCtx, vmxiptv_cmd_UserParamCallback, (void *)&pVmxIptvMgr->stVmxIptvService[0].stUserParam);
	// ECM Callback function
	VMRegisterFingerPrintingCallback(pVmxIptvMgr->pCtx, vmxiptv_cmd_FingerprintingCallback, (void *)&pVmxIptvMgr->stVmxIptvService[0].stUserParam);
	if(VMRegisterCopyControlCallback(pVmxIptvMgr->pCtx, vmxiptv_cmd_CopyControlCallback,(void *)&pVmxIptvMgr->stVmxIptvService[0].stUserParam) != VMXIPTV_SUCCESS)
	{
		HxLOG_Print("[%s:%d] VMRegisterCopyControlCallback is error\n", __FUNCTION__, __LINE__);
	}
	// EMM Cllback function
	VMRegisterIrdOpaqueCallback(vmxiptv_cmd_OpaqueDataCallback,(void *)&pVmxIptvMgr->stVmxIptvService[0].stUserParam);
	VMRegisterOsdMessageCallback(vmxiptv_cmd_OSDCallback, (void *)&pVmxIptvMgr->stVmxIptvService[0].stUserParam);

	#if 0
	// SW Decrytion callback 임시 test위해 추가함.
	if( VMSetCallback(pVmxIptvMgr->pCtx, /*vmxiptv_cmd_SWDecryptonCallback*/NULL,  188, (void*)&pVmxIptvMgr->fname_out[0]) != VMXIPTV_SUCCESS)
	{
		HxLOG_Print("[%s:%d] VMSetCallback is error\n", __FUNCTION__, __LINE__);
	}
	#endif
}

STATIC HBOOL vmxiptv_cmd_InitCA(VmxIptv_Manager_t *pVmxIptvMgr)
{
	void **pContext;
   	int    all_algorithms[1] = { -1 };
	HBOOL bContextFlag = TRUE;

	if( pVmxIptvMgr == NULL )
	{
		return FALSE;
	}

	if( VMSetVcasCommunicationProtocol(pVmxIptvMgr->eVcasVersion) != VMXIPTV_SUCCESS )
	{
		HxLOG_Print("[%s:%d] VMSetVcasCommunicationProtocol() : error\n", __FUNCTION__, __LINE__);
	}

	pContext = &pVmxIptvMgr->pCtx;

	pVmxIptvMgr->usCASystemID = VMGetCASystemID();
	HxSTD_memcpy(&pVmxIptvMgr->ucClientVerStr[0], VMGetVersionString(), VMXIPTV_VERSION_STRING_MAX_LEN);
	if( GetUniqueIdentifier(&pVmxIptvMgr->stUniqueId.ucUniqueId[0], sizeof(pVmxIptvMgr->stUniqueId.ucUniqueId), &pVmxIptvMgr->stUniqueId.usIdSize) != VMXIPTV_SUCCESS)
	{
		HxLOG_Print("[%s:%d] GetUniqueIdentifier() : error\n", __FUNCTION__, __LINE__);
	}

	if(pVmxIptvMgr->bOnlineMode== TRUE)
	{
		if ( VMConfig( &pVmxIptvMgr->pucVmxStore[0], &pVmxIptvMgr->ucBootIpAddress[0], pVmxIptvMgr->usBootPort, 2, 2, 0 ) != 0 )
		{
			bContextFlag = FALSE;
			HxLOG_Print("[%s:%d] VMConfig() : error\n", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		if( VMConfigOffline(&pVmxIptvMgr->pucVmxStore[0]) != VMXIPTV_SUCCESS )
		{
			bContextFlag = FALSE;
			HxLOG_Print("[%s:%d] VMConfigOffline() : error\n", __FUNCTION__, __LINE__);
		}
	}

	if( bContextFlag == TRUE )
	{
		VMLogVersion();
		*pContext =  VMCreateContext(NULL,1);

		HxLOG_Print("[%s:%d] *pContext  = 0x%08x\n", __FUNCTION__, __LINE__, *pContext );

		if ( *pContext  == NULL )
		{
			HxLOG_Print("[%s:%d] VMCreateContext() : error\n", __FUNCTION__, __LINE__);
			return FALSE;
		}

		if( VMRegisterControlWordCallback( *pContext, vmxiptv_cmd_EcmCallback, (void *)&pVmxIptvMgr->stVmxIptvService[0].stUserParam, 1, all_algorithms ) != VMXIPTV_SUCCESS )
		{
			HxLOG_Print("[%s:%d] VMRegisterControlWordCallback() : error\n", __FUNCTION__, __LINE__);
		}

		//vmxiptv_cmd_RegisterCallback(*pContext);

		VMSetErrorLevel(NULL, 5);

		if( VMResetStream( *pContext ) != VMXIPTV_SUCCESS )
		{
			HxLOG_Print("[%s:%d] VMResetStream() : error\n", __FUNCTION__, __LINE__);
		}

		pVmxIptvMgr->ucCurrentRatingLevel = VMGetRatingLevel( pVmxIptvMgr->pCtx, NULL, &pVmxIptvMgr->stVmxIptvService[0].usChannelRatingLevel);

		VMSetRatingLevel(pVmxIptvMgr->pCtx, 200); // test룰 위해 임시로 rating을 100으로 setting한다.
	}

	return TRUE;
}

STATIC HBOOL vmxiptv_cmd_CloseContext(VmxIptv_Manager_t *pVmxIptvMgr)
{
	void **pContext;

	if( pVmxIptvMgr == NULL )
	{
		HxLOG_Print("[%s:%d] pVmxIptvMgr : error\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	if( pVmxIptvMgr->pCtx == NULL )
	{
		HxLOG_Print("[%s:%d] pVmxIptvMgr->pCtx : error\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	//VMServiceStop(g->pCtx, ServiceIdx);

	pContext = &pVmxIptvMgr->pCtx;
	if( VMResetStream( *pContext ) != VMXIPTV_SUCCESS )
	{
		HxLOG_Print("[%s:%d] VMResetStream() : error\n", __FUNCTION__, __LINE__);
	}

	VMDestroyContext(pVmxIptvMgr->pCtx);
	pVmxIptvMgr->pCtx = NULL;

	return TRUE;
}

STATIC void vmxiptv_cmd_SetTestNum(int num)
{
	VmxIptv_Manager_t *pVmxIptvMgr = NULL;

	pVmxIptvMgr = vmxiptv_cmd_GetMgr();

	if(pVmxIptvMgr == NULL)
	{
		HxLOG_Print("[%s:%d] pVmxIptvMgr : error\n", __FUNCTION__, __LINE__);
		return;
	}

	if(num==0 ) // default setting  0 : offline
	{
		#if 0
		HBOOL bOnlineMode = ( num==1 ? TRUE : FALSE );
		vmxiptv_cmd_MgrInit(pVmxIptvMgr,
									bOnlineMode,
									VMXIPTV_TEST_DEAULT_BOOT_IPADDRESS,
									VMXIPTV_TEST_DEFAULT_BOOT_PORT,
									VMXIPTV_TEST_DEFAULT_STORE,
									VMXIPTV_TEST_DEFAULT_VCAS_VERSION);
		#endif
		pVmxIptvMgr->ucTestType++;

		if(pVmxIptvMgr->ucTestType == eVMXIPTV_MAX_TYPE )
		{
			pVmxIptvMgr->ucTestType = eVMXIPTV_BASIC_TYPE;
		}
	}
	else if(num==1) // default setting  1 : online
	{
		HBOOL bOnlineMode = ( num==1 ? TRUE : FALSE );
		vmxiptv_cmd_MgrInit(pVmxIptvMgr,
									bOnlineMode,
									VMXIPTV_TEST_DEAULT_BOOT_IPADDRESS,
									VMXIPTV_TEST_DEFAULT_BOOT_PORT,
									VMXIPTV_TEST_DEFAULT_STORE,
									VMXIPTV_TEST_DEFAULT_VCAS_VERSION);

		// 아래 함수에 대해 좀더 명확히 알아야함.  ( input para에 무었을 넣어야하는가 ? 임시로 1234 삽입)
		if( VMSetMiddlewareAuthenticator( (unsigned char *)"1234", 10 ) != VMXIPTV_SUCCESS )
		{
			HxLOG_Print("[%s:%d] VMSetMiddlewareAuthenticator() : error\n", __FUNCTION__, __LINE__);
		}
		if( vmxiptv_cmd_InitCA(pVmxIptvMgr) == FALSE)
		{
			HxLOG_Print("[%s:%d] vmxiptv_cmd_InitCA() error\n", __FUNCTION__, __LINE__);
		}
	}
	else if(num == 2)
	{
		if( pVmxIptvMgr->pCtx )
		{
			void **pContext;
		   	int    all_algorithms[1] = { -1 };
			pContext = &pVmxIptvMgr->stVmxIptvService[num-2].pCtx;
			*pContext =  VMCreateContext(NULL,1);

			HxSTD_memcpy(&pVmxIptvMgr->fname_in[0], "/mnt/nfs/car.ts",  30);
			HxSTD_memcpy(&pVmxIptvMgr->fname_out[0], "/mnt/nfs/out.ts",  30);

			pVmxIptvMgr->stVmxIptvService[num-2].stUserParam.ucUserParamFlag = 0;
			HxSTD_memcpy(&pVmxIptvMgr->stVmxIptvService[num-2].stUserParam.usUserParamData[0], "UserParam = car.ts", sizeof(pVmxIptvMgr->stVmxIptvService[num-2].stUserParam.usUserParamData));

			if( VMRegisterControlWordCallback( *pContext, vmxiptv_cmd_EcmCallback, (void *)&pVmxIptvMgr->stVmxIptvService[num-2].stUserParam, 1, all_algorithms ) != VMXIPTV_SUCCESS )
			{
				HxLOG_Print("[%s:%d] VMRegisterControlWordCallback() : error\n", __FUNCTION__, __LINE__);
			}

			VMRegisterDecodeFailureCallback( *pContext, vmxiptv_cmd_DecodeCallback, (void *)&pVmxIptvMgr->stVmxIptvService[num-2].stUserParam );

			vmxiptv_cmd_JoinWithExample(pVmxIptvMgr->stVmxIptvService[num-2].pCtx, pVmxIptvMgr);

		}
	}
	else if(num == 3)
	{
		if( pVmxIptvMgr->pCtx )
		{
			void **pContext;
		   	int    all_algorithms[1] = { -1 };
			pContext = &pVmxIptvMgr->stVmxIptvService[num-2].pCtx;
			*pContext =  VMCreateContext(NULL,1);
			HxSTD_memcpy(&pVmxIptvMgr->fname_in[0], "/mnt/nfs/rating_255.mpg",  40);

			pVmxIptvMgr->stVmxIptvService[num-2].stUserParam.ucUserParamFlag = 1;
			HxSTD_memcpy(&pVmxIptvMgr->stVmxIptvService[num-2].stUserParam.usUserParamData[0], "UserParam = rating_255", sizeof(pVmxIptvMgr->stVmxIptvService[num-2].stUserParam.usUserParamData));

			if( VMRegisterControlWordCallback( *pContext, vmxiptv_cmd_EcmCallback, (void *)&pVmxIptvMgr->stVmxIptvService[num-2].stUserParam, 1, all_algorithms ) != VMXIPTV_SUCCESS )
			{
				HxLOG_Print("[%s:%d] VMRegisterControlWordCallback() : error\n", __FUNCTION__, __LINE__);
			}
			vmxiptv_cmd_JoinWithExample(pVmxIptvMgr->stVmxIptvService[num-2].pCtx, pVmxIptvMgr);
		}
	}
	else if(num==4)
	{
		HUINT8  ucCurrentRatingLevel;
		HUINT8  ucSvcRatingLevel;

		ucCurrentRatingLevel = VMGetRatingLevel(pVmxIptvMgr->stVmxIptvService[0].pCtx, NULL, &ucSvcRatingLevel);
		HxLOG_Print("ucCurrentRatingLevel = %d, ucSvcRatingLevel = %d\n", ucCurrentRatingLevel, ucSvcRatingLevel);

		ucCurrentRatingLevel = VMGetRatingLevel(pVmxIptvMgr->stVmxIptvService[1].pCtx, NULL, &ucSvcRatingLevel);
		HxLOG_Print("ucCurrentRatingLevel = %d, ucSvcRatingLevel = %d\n", ucCurrentRatingLevel, ucSvcRatingLevel);
	}
	else if(num==5)
	{
		   	int    all_algorithms[1] = { -1 };
			pVmxIptvMgr->stVmxIptvService[0].stUserParam.ucUserParamFlag = 5;
			HxSTD_memcpy(&pVmxIptvMgr->stVmxIptvService[0].stUserParam.usUserParamData[0], "UserParam = rating_255-5", sizeof(pVmxIptvMgr->stVmxIptvService[0].stUserParam.usUserParamData));

			if( VMRegisterControlWordCallback( pVmxIptvMgr->stVmxIptvService[0].pCtx, vmxiptv_cmd_EcmCallback, (void *)&pVmxIptvMgr->stVmxIptvService[0].stUserParam, 1, all_algorithms ) != VMXIPTV_SUCCESS )
			{
				HxLOG_Print("[%s:%d] VMRegisterControlWordCallback() : error\n", __FUNCTION__, __LINE__);
			}

			vmxiptv_cmd_JoinWithExample(pVmxIptvMgr->stVmxIptvService[0].pCtx, pVmxIptvMgr);
	}
	else if(num==6)
	{
		   	int    all_algorithms[1] = { -1 };
			pVmxIptvMgr->stVmxIptvService[1].stUserParam.ucUserParamFlag = 6;
			HxSTD_memcpy(&pVmxIptvMgr->stVmxIptvService[1].stUserParam.usUserParamData[0], "UserParam = rating_255-6", sizeof(pVmxIptvMgr->stVmxIptvService[1].stUserParam.usUserParamData));

			if( VMRegisterControlWordCallback( pVmxIptvMgr->stVmxIptvService[1].pCtx, vmxiptv_cmd_EcmCallback, (void *)&pVmxIptvMgr->stVmxIptvService[1].stUserParam, 1, all_algorithms ) != VMXIPTV_SUCCESS )
			{
				HxLOG_Print("[%s:%d] VMRegisterControlWordCallback() : error\n", __FUNCTION__, __LINE__);
			}

			vmxiptv_cmd_JoinWithExample(pVmxIptvMgr->stVmxIptvService[1].pCtx, pVmxIptvMgr);
	}
	else if(num==7)
	{
		if( VMResetStream(  pVmxIptvMgr->stVmxIptvService[0].pCtx ) != VMXIPTV_SUCCESS )
		{
			HxLOG_Print("[%s:%d] VMResetStream() : error\n", __FUNCTION__, __LINE__);
		}
	}
	else if(num==8)
	{
		if( VMResetStream(  pVmxIptvMgr->stVmxIptvService[1].pCtx ) != VMXIPTV_SUCCESS )
		{
			HxLOG_Print("[%s:%d] VMResetStream() : error\n", __FUNCTION__, __LINE__);
		}
	}
	else if(num == 9)
	{
		if( vmxiptv_cmd_CloseContext(pVmxIptvMgr) == FALSE )
		{
			HxLOG_Print("[%s:%d] vmxiptv_cmd_CloseContext() : error\n", __FUNCTION__, __LINE__);
		}
	}
	else
	{
		HxLOG_Print("[%s:%d] command number is not valid\n", __FUNCTION__, __LINE__);
	}

	vmxiptv_cmd_DbgPrintMgrr(pVmxIptvMgr);
}

STATIC HBOOL vmxiptv_cmd_MgrInit(VmxIptv_Manager_t * pVmxIptvMgr, HBOOL bOnlineMode, HUINT8 *pucIpBootAddress, HUINT16 usBootPort,  HUINT8 *pucVmxStore, eVcasVersion_t eVcasVersion)
{
	if(pVmxIptvMgr == NULL)
	{
		HxLOG_Print("[%s:%d] pVmxIptvMgr : error\n", __FUNCTION__, __LINE__);
		return FALSE;
	}

	HxSTD_memset(pVmxIptvMgr, 0, sizeof(pVmxIptvMgr));
	pVmxIptvMgr->ucTestType = 0;
	pVmxIptvMgr->pCtx = NULL;
	pVmxIptvMgr->bOnlineMode = bOnlineMode;
	HxSTD_memcpy(&pVmxIptvMgr->ucBootIpAddress[0], pucIpBootAddress, HxSTD_StrLen(pucIpBootAddress)+1);
	pVmxIptvMgr->usBootPort = usBootPort;
	HxSTD_memcpy(&pVmxIptvMgr->pucVmxStore[0], pucVmxStore,  HxSTD_StrLen(pucVmxStore)+1);
	pVmxIptvMgr->eVcasVersion = eVcasVersion;
	pVmxIptvMgr->stVmxIptvService[0].eEncryptionAlgorithm = eEncAlg_null;

	HxSTD_memcpy(&pVmxIptvMgr->fname_in[0], "/mnt/nfs/car.ts",  30);
	return TRUE;
}

STATIC void vmxiptv_cmd_DbgPrintMgrr(VmxIptv_Manager_t *pVmxIptvMgr)
{
	if( pVmxIptvMgr == NULL )
	{
		return;
	}
	HxLOG_Print("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	HxLOG_Print("[%s:%d]\n",  __FUNCTION__, __LINE__);
	HxLOG_Print("+ ucTestType = %d\n", pVmxIptvMgr->ucTestType);
	HxLOG_Print("+ Boot Address = %s\n",&pVmxIptvMgr->ucBootIpAddress[0]);
	HxLOG_Print("+ Boot Port = %d\n", pVmxIptvMgr->usBootPort);
	HxLOG_Print("+ Store = %s\n",&pVmxIptvMgr->pucVmxStore[0]);
	HxLOG_Print("+ OnlineMode = %d\n",pVmxIptvMgr->bOnlineMode);
	HxLOG_Print("+ VCAS Version = %d\n",pVmxIptvMgr->eVcasVersion);
	HxLOG_Print("+ CASystemID = 0x%x\n",pVmxIptvMgr->usCASystemID);
	HxLOG_Print("+ Client Version = %s\n",&pVmxIptvMgr->ucClientVerStr[0]);
	HxLOG_Print("+ UniqueId = %s, size = %d\n",&pVmxIptvMgr->stUniqueId.ucUniqueId[0], pVmxIptvMgr->stUniqueId.usIdSize);
	HxLOG_Print("+ ucCurrentRatingLevel = %d\n",pVmxIptvMgr->ucCurrentRatingLevel);
	HxLOG_Print("--------- service -----------\n");
	HxLOG_Print("+ usChannelRatingLevel = %d\n",pVmxIptvMgr->stVmxIptvService[0].usChannelRatingLevel);
	HxLOG_Print("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
}

HINT32 VMXIPTV_CmdMgr_Execute(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20+1]={0,};

	HxLOG_Print("\n**********************************************************\n");
	HxLOG_Print("* Verimatrix IPTV Hybrid (command)\n");
	HxLOG_Print("* command number -\n");
	HxLOG_Print("**********************************************************\n\n");

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
	{
		HxLOG_Print("[%s:%d] command : error\n", __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}
	if( snprintf(aucStr, 20, "%s", aucArg) == 0 )
	{
		HxLOG_Print("[%s:%d] command error\n", __FUNCTION__, __LINE__);
		return HxCMD_ERR;
	}

	HLIB_STD_StrUpper(aucStr);

	if(strcmp(aucStr, "IPTV") == 0 )
	{
		int num;
		aucArg = HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if( aucArg != NULL )
		{
			if( sscanf(aucArg, "%d", &num) != 0 )
			{
				vmxiptv_cmd_SetTestNum(num);
				return HxCMD_OK;
			}
		}
	}
	else if(strcmp(aucStr, "VMX") == 0 )
	{
		aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
		if(aucArg == NULL)
		{
			return HxCMD_ERR;
		}
		if( snprintf(aucStr, 20, "%s", aucArg) == 0  )
		{
			return HxCMD_ERR;
		}
		HLIB_STD_StrUpper(aucStr);

		if(strcmp(aucStr, "SET") == 0)
		{

		}
		else
		{
			HxLOG_Print("invalid cmd\n");
		}
	}
	else
	{
		HxLOG_Print("humandks_cmd [%s:%d]\n", __FUNCTION__, __LINE__);
	}


	return HxCMD_ERR;
}

void xsvc_cas_VmxIptvCmdInit(HxCMD_Handle_t hCmdHandle)
{
#ifdef CONFIG_DEBUG
	HLIB_CMD_RegisterWord(hCmdHandle,
						VMXIPTV_CmdMgr_Execute,
						"cmdvmx",						/* keyword */
						"Verimatrix Iptv Debug, TV Manager\n cmdvmx [cmd] (test number)\n cmdvmx IPTV 0~9\n",	/* help   */
						"cmdvmx [cmd] ( IPTV [num]");	/* usage  */
#endif
}
