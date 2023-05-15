/**************************************************************************************************/
#define ________Irdeto_ctrl_Private_Include__________________________________________________
/**************************************************************************************************/
#include <_svc_cas_mgr_common.h>
#include <ir_api.h>
#include <ir_loader.h>

#include "vd_humax.h"

#include "ir_ctrl.h"
#include "ir_svc.h"
#include "ir_status.h"

/**************************************************************************************************/
#define ________Irdeto_ctrl_Golbal_Value____________________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Irdeto_ctrl_Private_Define___________________________________________________
/**************************************************************************************************/



/**************************************************************************************************/
#define ________Irdeto_ctrl_Private_Type_Define_____________________________________________
/**************************************************************************************************/

/**************************************************************************************************/
#define ________Irdeto_ctrl_Private_Static_Value______________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Irdeto_ctrl_Private_Static_Prototype__________________________________________
/**************************************************************************************************/


/**************************************************************************************************/
#define ________Irdeto_ctrl_Private_Static_Prototype_Body_____________________________________
/**************************************************************************************************/
static HERROR local_IR_LOADER_Init(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetManuCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetClientVersion(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetHWCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetStatusInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetLoadSeqNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetSignDate(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetSignTime(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetDnldDate(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetDnldTime(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetSerialNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetRandomNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetLoaderMajor(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetLoaderMinor(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_LOADER_GetLoaderType(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
static HERROR local_IR_LOADER_GetCssn(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
static HERROR local_IR_OTA_GetOtaInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_OTA_ClearOtaIno(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_OTA_SetOtaFlagUp(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_OTA_GetChannelParam(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_OTA_SetChannelParam(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetMailList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetMailBox(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_CheckValidOfMail(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetMailState(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_SetMailState(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_DoYouHaveUnreadMail(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_DeleteMail(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_AllClearExpiredNormalAttributeMail(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
static HERROR local_IR_MR_UpdatePIN(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_ReadAllUserPF(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_SetUserPF(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_DeleteUserPF(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif
static HERROR local_IR_GetSringIdxFromErrorCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetSmartCardInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_CheckValidEvtCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetGlobalStatusOfSC(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetDrvSmartCardInfo (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetNumberOfServices (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetInformationOfEachService (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetInformationOfElement (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);





#if defined(CONFIG_MW_CAS_IRDETO_SOL)
static HERROR local_IR_GetSolStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif


#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
static HERROR local_IR_GetFtaBlockStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif

#if defined(CONFIG_OP_ALMAJD)

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
static HERROR local_IR_GetAlmajdHomingChannel (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
#endif

static HERROR local_IR_GetRegionCode (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_GetFirstBoot (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);
static HERROR local_IR_ResetFirstBoot (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam);


#endif


/**************************************************************************************************/
#define ________Irdeto_ctrl_External_Functions_Body__________________________________________
/**************************************************************************************************/

static const SvcCas_Ctrl_t s_astIrdetoCtrlTable[eCACTRL_IR_MAX] =
{
	/* Irdeto loader informaion */
	{
		eCACTRL_IR_LOADER_Init,
		eCasCtrl_Control,
		local_IR_LOADER_Init,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetManuCode,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetManuCode,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetClientVersion,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetClientVersion,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetHWCode,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetHWCode,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetStatusInfo,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetStatusInfo, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetLoadSeqNum,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetLoadSeqNum, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetSignDate,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetSignDate, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetSignTime,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetSignTime, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetDnldDate,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetDnldDate, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetDnldTime,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetDnldTime, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetSerialNum,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetSerialNum, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetRandomNum,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetRandomNum, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetLoaderMajor,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetLoaderMajor, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetLoaderMinor,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetLoaderMinor, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_LOADER_GetLoaderType,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetLoaderType, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
	{
		eCACTRL_IR_LOADER_GetCssn,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_LOADER_GetCssn, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

	{
		eCACTRL_IR_OTA_ReadOTAInfo,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_OTA_GetOtaInfo, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_OTA_ClearOTAInfo,
		eCasCtrl_Control,
		local_IR_OTA_ClearOtaIno,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_OTA_SetFlagUp,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_IR_OTA_SetOtaFlagUp,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_OTA_ReadChannelParam,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_OTA_GetChannelParam,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_OTA_SetChannelParam,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_IR_OTA_SetChannelParam,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_GetMailList,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetMailList,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_GetMailBox,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetMailBox,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_CheckValidOfMail,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_CheckValidOfMail, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_GetMailReadState,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetMailState, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_SetMailReadState,
		eCasCtrl_Set,
		NULL,	// Control
		NULL,	// Get
		local_IR_SetMailState,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_DoYouHaveUnreadMail,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_DoYouHaveUnreadMail, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_DeleteMail,
		eCasCtrl_Control,
		local_IR_DeleteMail,	// Control
		NULL, 	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},


	{
		eCACTRL_IR_AllClearExpiredNormalAttributeMail,
		eCasCtrl_Control,
		local_IR_AllClearExpiredNormalAttributeMail,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_DRV_GetStringIdxFromErrorCode,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetSringIdxFromErrorCode, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_SC_Info,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetSmartCardInfo, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_DRV_CheckValidEvtCode,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_CheckValidEvtCode, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_DRV_GetGlobalStatusOfSC,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetGlobalStatusOfSC,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_DRV_GetDrvScInfo,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetDrvSmartCardInfo,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_SVCSTATUS_GetNumberOfServices,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetNumberOfServices,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_SVCSTATUS_GetInformationOfEachService,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetInformationOfEachService,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_SVCSTATUS_GetInformationOfElement,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetInformationOfElement,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
	/* 공통으로 사용될수 있는 기능을 넣는다.   */
#if defined(CONFIG_MW_CAS_IRDETO_SOL)
	{
		eCACTRL_IR_GetSolStatus,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetSolStatus, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#endif

#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)
	{
		eCACTRL_IR_GetFTABlockStatus,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetFtaBlockStatus, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#endif

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	{
		eCACTRL_IR_MR_UpdatePIN,
		eCasCtrl_Control,
		local_IR_MR_UpdatePIN,	// Control
		NULL, 	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},


	{
		eCACTRL_IR_MR_ReadAllUserPF,
		eCasCtrl_Control,
		local_IR_ReadAllUserPF,	// Control
		NULL, 	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_MR_SetUserPF,
		eCasCtrl_Control,
		local_IR_SetUserPF,	// Control
		NULL, 	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_MR_DeleteUserPF,
		eCasCtrl_Control,
		local_IR_DeleteUserPF,	// Control
		NULL, 	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif

/* 여기부터  OP관련된 기능을 넣는다.  */


#if defined(CONFIG_OP_ALMAJD)

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)

	{
		eCACTRL_IR_GetAlmajdHomingChannel,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetAlmajdHomingChannel, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif
	{
		eCACTRL_IR_GetRegionCode,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetRegionCode, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_GetFirstBoot,
		eCasCtrl_Get,
		NULL,	// Control
		local_IR_GetFirstBoot, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

	{
		eCACTRL_IR_ResetFirstBoot,
		eCasCtrl_Control,
		local_IR_ResetFirstBoot,	// Control
		NULL, // Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},

#endif

#if 0
	{
		eCACTRL_IR_MAX,
		eCasCtrl_None,
		NULL,	// Control
		NULL,	// Get
		NULL,	// Set
		NULL,	// Req
		NULL,	// Noti
	},
#endif
};

HERROR CA_IR_RegisterCtrlList(void)
{
	HERROR	hErr;

	hErr = svc_cas_MgrSubInitControl(eDxCAS_GROUPID_IR);
	if (hErr == ERR_OK)
	{
		hErr = svc_cas_MgrSubRegisterControl(eDxCAS_GROUPID_IR, eCACTRL_IR_MAX, (SvcCas_Ctrl_t *)&s_astIrdetoCtrlTable[0]);
	}

	return hErr;
}

//inline void *local_IR_GetInParam(SvcCas_CtrlParam_t *pstParam)
inline Ir_InParam_t *local_IR_GetInParam(SvcCas_CtrlParam_t *pstParam)
{
#if 0
	return (Ir_InParam_t *)pstParam->unArg.pstIr->putIn;
#else
	return (Ir_InParam_t *)pstParam->pvIn;
#endif
}

//inline void *local_IR_GetOutParam(SvcCas_CtrlParam_t *pstParam)
inline Ir_OutParam_t *local_IR_GetOutParam(SvcCas_CtrlParam_t *pstParam)
{
#if 0
	return (Ir_OutParam_t *)pstParam->unArg.pstIr->putOut;
#else
	return (Ir_OutParam_t *)pstParam->pvOut;
#endif
}

#if 0
static HERROR local_XXX(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	XXXInParam_t	*pstInParam;
	XXXOutParam_t	*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (XXXInParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (XXXOutParam_t *)local_IR_GetInParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

#if 0
	hErr = MW_CA_XXX(pstInParam-> , pstOutParam->);
#endif

	return hErr;
}
#endif

// eCACTRL_IR_LOADER_Init,
static HERROR local_IR_LOADER_Init(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;

	HAPPY(pstParam);

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderInit();

	return hErr;
}


// eCACTRL_IR_LOADER_GetManuCode
static HERROR local_IR_LOADER_GetManuCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetManuCodeOutParam_t	*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetManuCodeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetManuCode(&pstOutParam->ucManCode);

	return hErr;
}

// eCACTRL_IR_LOADER_GetClientVersion
static HERROR local_IR_LOADER_GetClientVersion(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetManuCodeOutParam_t	*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetManuCodeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetManuCode(&pstOutParam->ucManCode);

	return hErr;
}


// eCACTRL_IR_LOADER_GetManuCode
static HERROR local_IR_LOADER_GetHWCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetHwCodeOutParam_t	*pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetHwCodeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetHWCode(&pstOutParam->ucHwCode);

	return hErr;
}

static HERROR local_IR_LOADER_GetStatusInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetStatusOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetStatusOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetStatusInfo(&pstOutParam->stIRStatus);

	return hErr;
}

static HERROR local_IR_LOADER_GetLoadSeqNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetLoadSqeNumOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetLoadSqeNumOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetLoadSeqNum(&pstOutParam->ucLoadSeqNum);

	return hErr;
}


static HERROR local_IR_LOADER_GetSignDate(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetSignDateOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetSignDateOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetSignDate(&pstOutParam->usMJD);

	return hErr;
}


static HERROR local_IR_LOADER_GetSignTime(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetSignTimeOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetSignTimeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetSignTime(&pstOutParam->stTime);

	return hErr;
}

static HERROR local_IR_LOADER_GetDnldDate(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetDnldDateOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetDnldDateOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetDnldDate(&pstOutParam->usMJD);

	return hErr;
}

static HERROR local_IR_LOADER_GetDnldTime(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetDnldTimeOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetDnldTimeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetDnldTime(&pstOutParam->stTime);

	return hErr;
}

static HERROR local_IR_LOADER_GetSerialNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetSerialNumOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetSerialNumOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetSerialNum(&pstOutParam->unSerialNum);

	return hErr;
}

static HERROR local_IR_LOADER_GetRandomNum(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetRandomNumOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetRandomNumOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetRandomNum(&pstOutParam->unRandomNum);

	return hErr;
}

static HERROR local_IR_LOADER_GetLoaderMajor(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetLoaderMajOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetLoaderMajOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetLoaderMajor(&pstOutParam->ucLoaderMajor);

	return hErr;
}

static HERROR local_IR_LOADER_GetLoaderMinor(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetLoaderMinorOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetLoaderMinorOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetLoaderMinor(&pstOutParam->ucLoaderMinor);

	return hErr;
}

#if defined(CONFIG_MW_CAS_IRDETO_SECURE)
static HERROR local_IR_LOADER_GetCssn(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetCssnOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetCssnOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrSecureChipGetCSSN((HUINT32*)&(pstOutParam->ulCssn));

	return hErr;
}


#endif

static HERROR local_IR_LOADER_GetLoaderType(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_LOADER_GetLoaderTypeOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_LOADER_GetLoaderTypeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrLoaderGetLoaderType(&pstOutParam->ucLoaderType);

	return hErr;
}


static HERROR local_IR_OTA_GetOtaInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;
	IR_OTA_GetOtaInfoOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_OTA_GetOtaInfoOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrOtaReadOTAInfo(&pstOutParam->usDownloadServicePID, &pstOutParam->ucTableID);

	return hErr;
}


static HERROR local_IR_OTA_ClearOtaIno(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;

	HAPPY(pstParam);

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrOtaClearOTAInfo();

	return hErr;
}

static HERROR local_IR_OTA_SetOtaFlagUp(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;
	IR_OTA_SetOtaFlagUpInParam_t	 *pstInParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (IR_OTA_SetOtaFlagUpInParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrOtaSetFlagUp(pstInParam->bflagDoOTA, pstInParam->usDownloadServicePID, pstInParam->ucTableID);

	return hErr;
}

static HERROR local_IR_OTA_GetChannelParam(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR								hErr;
	IR_OTA_GetChannelParamOutParam_t		*pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_OTA_GetChannelParamOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrOtaReadChannelParam(pstOutParam->unTunerId, pstOutParam->pvDiChannel_param);

	return hErr;
}

static HERROR local_IR_OTA_SetChannelParam(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR								hErr;
	IR_OTA_SetChannelParamInParam_t	 *pstInParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (IR_OTA_SetChannelParamInParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrOtaSetChannelParam(pstInParam->unTunerId, pstInParam->pvDiChannel_param);

	return hErr;
}

static HERROR local_IR_GetMailList(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR					hErr;
	IR_GetMailListOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetMailListOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrGetMailList(&pstOutParam->ucNumberOfSavedMail, pstOutParam->paucMailIndexList);

	return hErr;
}

static HERROR local_IR_GetMailBox(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR			hErr;
	IR_GetMailBoxOutParam_t	 *pstOutParam;

	HxLOG_Info("ulControlType: %d\n", ulControlType);

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		HxLOG_Info("ulControlType: %d >= eCACTRL_IR_MAX\n", ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Info("pstParam == NULL\n");
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetMailBoxOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Info("pstOutParam == NULL\n");
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrGetMailBox(pstOutParam->ucMailIndex, pstOutParam->pstMailBox);

	return hErr;
}

static HERROR local_IR_CheckValidOfMail(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetCheckMailOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetCheckMailOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->bCheckMail = xsvc_cas_IrCheckValidOfMail(pstOutParam->ucMailIndex);

	return ERR_OK;
}

static HERROR local_IR_GetMailState(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetMailStateOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetMailStateOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->bReadMail = xsvc_cas_IrGetMailReadState(pstOutParam->ucMailIndex);

	return ERR_OK;
}

static HERROR local_IR_SetMailState(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR							hErr;
	IR_SetMailStateInParam_t	 *pstInParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (IR_SetMailStateInParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrSetMailReadState(pstInParam->ucMailIndex, pstInParam->bReadMail);

	return hErr;
}

static HERROR local_IR_DoYouHaveUnreadMail(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_DoUHaveUnReadMailOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_DoUHaveUnReadMailOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->ucTypeFlag = xsvc_cas_IrDoYouHaveUnreadMail();

	return ERR_OK;
}

static HERROR local_IR_DeleteMail(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR							hErr;
	IR_DeleteMailInParam_t	 	*pstInParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (IR_DeleteMailInParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrDeleteMail(pstInParam->ucMailIndex);

	return hErr;
}

static HERROR local_IR_AllClearExpiredNormalAttributeMail(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HAPPY(pstParam);

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	xsvc_cas_IrAllClearExpiredNormalAttributeMail();

	return ERR_OK;
}

static HERROR local_IR_GetSringIdxFromErrorCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetStrIdxFromErrCodeOutParam_t	 *pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetStrIdxFromErrCodeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->unStrIdx = xsvc_cas_IrDrvGetStringIdxFromErrorCode(pstOutParam->pstIrStatus);

	if(pstOutParam->unStrIdx == AXI_STATUSCODE_IDX_None)
	{
		return ERR_FAIL;
	}

	return ERR_OK;
}

static HERROR local_IR_GetSmartCardInfo(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR							hErr;
	IR_GetSCInfoOutParam_t	 *pstOutParam;

	HxLOG_Info("ulControlType: %d \n", ulControlType);
	if (ulControlType >= eCACTRL_IR_MAX)
	{
		HxLOG_Info("ulControlType: %d  >= eCACTRL_IR_MAX \n", ulControlType);
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		HxLOG_Info("pstParam == NULL \n");
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetSCInfoOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		HxLOG_Info("pstOutParam == NULL \n");
		return ERR_FAIL;
	}


	hErr = xsvc_cas_IrScInfo(pstOutParam->ucScSlotId, pstOutParam->pstIrApiScInfo);

	return hErr;
}

static HERROR local_IR_CheckValidEvtCode(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_CheckValidEvtCodeOutParam_t	 *pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_CheckValidEvtCodeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->ucEvtCode = xsvc_cas_IrDrvCheckValidEvtCode(pstOutParam->nEvtCode);

	return ERR_OK;
}

static HERROR local_IR_GetGlobalStatusOfSC(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetGlobalStatusOfScOutParam_t 		*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetGlobalStatusOfScOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->bGetStatus = XSVC_CAS_IR_DRV_GetGlobalStatusOfSC(pstOutParam->ucSlotId, &pstOutParam->ulStatus);

	return ERR_OK;
}

static HERROR local_IR_GetDrvSmartCardInfo (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetDrvSmartCardInfoOutParam_t		*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetDrvSmartCardInfoOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

//	pstOutParam->bGetStatus = XSVC_CAS_IR_DRV_GetScInfo(pstOutParam->ucSlotId, &pstOutParam->pstIrDrvScInfo); 다시 구현하자!!

	return ERR_OK;
}

static HERROR local_IR_GetNumberOfServices (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetNumberOfServicesOutParam_t		*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetNumberOfServicesOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->bGetStatus = xsvc_cas_IrSvcStatusGetNumberOfServices(&(pstOutParam->pstSvcStatusInfo), &pstOutParam->ucNumberOfServices);

	return ERR_OK;
}

static HERROR local_IR_GetInformationOfEachService (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetInformationOfEachServiceOutParam_t		*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetInformationOfEachServiceOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->bGetStatus = xsvc_cas_IrSvcStatusGetInformationOfEachService(pstOutParam->eSvcType, pstOutParam->usHandle, pstOutParam->pstSvcStatusInfoEach);

	return ERR_OK;
}

static HERROR local_IR_GetInformationOfElement (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetInformationOfElementOutParam_t		*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetInformationOfElementOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->ucNumOfElem = xsvc_cas_IrSvcStatusGetInformationOfElement(pstOutParam->usHandle, pstOutParam->ulResourceId, &(pstOutParam->pstSvcStatusInfoElem));

	return ERR_OK;
}




#if defined(CONFIG_MW_CAS_IRDETO_SOL)

static HERROR local_IR_GetSolStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetsolStatusOutParam_t		*pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetsolStatusOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->eSolState = xsvc_cas_IrGetSolStatus(pstOutParam->hAction, pstOutParam->ONID, pstOutParam->TSID);

	return ERR_OK;
}

#endif


#if defined(CONFIG_MW_CAS_IRDETO_FTA_BLOCK)

static HERROR local_IR_GetFtaBlockStatus(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	IR_GetFtaBlockStatusOutParam_t		*pstOutParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetFtaBlockStatusOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam->unFtaBlockStatus = xsvc_cas_IrGetFTABlockStatus(pstOutParam->hAction);

	return ERR_OK;
}

#endif


#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)

static HERROR local_IR_MR_UpdatePIN(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR							hErr;
	IR_MR_UpdatePinInParam_t		*pstInParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (IR_MR_UpdatePinInParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrMrUpdatePIN(pstInParam->hAction, pstInParam->aucDecimalPIN);

	return hErr;
}

static HERROR local_IR_ReadAllUserPF(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR								hErr;
	IR_GetReadAllUserPFOutParam_t	 *pstInParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (IR_GetReadAllUserPFOutParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrMrReadAllUserPF(pstInParam->resourceId, pstInParam->aucDecimalPIN);

	return hErr;
}


static HERROR local_IR_SetUserPF(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR							hErr;
	IR_MR_SetUserPFInParam_t	 	*pstInParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (IR_MR_SetUserPFInParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrMrSetUserPF(pstInParam->ucUserIdx, pstInParam->ucAge, pstInParam->aucDecimalPIN);

	return hErr;
}

static HERROR local_IR_DeleteUserPF(HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR							hErr;
	IR_MR_DeleteUserPFInParam_t	 *pstInParam;


	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstInParam = (IR_MR_DeleteUserPFInParam_t *)local_IR_GetInParam(pstParam);
	if (pstInParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrMrDeleteUserPF(pstInParam->ucUserIdx, pstInParam->aucDecimalPIN);

	return hErr;
}

#endif

#if defined(CONFIG_OP_ALMAJD)

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
static HERROR local_IR_GetAlmajdHomingChannel (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR								hErr;
	IR_GetAlmajdHomingChannelOutParam_t  	*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetAlmajdHomingChannelOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrGetAlmajdHomingChannel(pstOutParam->ucSlot, &pstOutParam->stHomeChannel);

	return hErr;
}

#endif

static HERROR local_IR_GetRegionCode (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR						hErr;
	IR_GetRegionCodeOutParam_t  	*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetRegionCodeOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrGetRegionCode(pstOutParam->ausRegionCode);

	return hErr;
}

static HERROR local_IR_GetFirstBoot (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR					hErr;
	IR_GetFirstBootOutParam_t  	*pstOutParam;

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	if (pstParam == NULL)
	{
		return ERR_FAIL;
	}

	pstOutParam = (IR_GetFirstBootOutParam_t *)local_IR_GetOutParam(pstParam);
	if (pstOutParam == NULL)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrGetFirstBoot(&pstOutParam->ucFirstBoot);

	return hErr;
}

static HERROR local_IR_ResetFirstBoot (HUINT32 ulControlType, SvcCas_CtrlParam_t *pstParam)
{
	HERROR	hErr;

	HAPPY(pstParam);

	if (ulControlType >= eCACTRL_IR_MAX)
	{
		return ERR_FAIL;
	}

	hErr = xsvc_cas_IrResetFirstBoot();

	return hErr;
}

#endif
