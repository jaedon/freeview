/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif
#include <hlib.h>

#include <mgr_cas.h>
#include <xmgr_cas.h>
#include <mgr_action.h>

#include <xmgr_cas_res_str.h>
#include <svc_cas.h>
#include <svc_sys.h>
#include <db_svc.h>
#include <db_param.h>
#include <isosvc.h>


#include "local_include/_xmgr_cas_vmx_util.h"
#include "local_include/_xmgr_cas_vmx.h"
#include "_xsvc_cas_vmx_ctrl.h"


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/

/*******************************************************************/
/********************      Static Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/

HCHAR *xmgr_cas_VmxUtil_EncodeString(HCHAR *string)
{
    HUINT32		Len = 0;
	HCHAR		*buf = NULL;

	Len = HLIB_STD_URLStrLen(string) + 1;
    buf = HLIB_STD_MemCalloc(Len+1);
    HLIB_STD_URLEncode(buf,string,Len);

    return buf;
}

HERROR xmgr_cas_VmxUtil_PostMessageSessionEventToOM(DxCAS_MmiSessionType_e eSessionType, BUS_Callback_t ulUiHandle, Handle_t hSession, HUINT32 ulSlotNumber, HINT32 eEvent)
{
	OxMgrCasUiEvtSession_t	*pstEvent = NULL;

	pstEvent = (OxMgrCasUiEvtSession_t *)HLIB_STD_MemCalloc(sizeof(OxMgrCasUiEvtSession_t));
	if(pstEvent == NULL)
	{
		HxLOG_Error("HLIB_STD_MemCalloc() error\n");
		return ERR_FAIL;
	}

	pstEvent->eSessionType	= eSessionType;
	pstEvent->ulUiHandle	= (HUINT32)ulUiHandle;
	pstEvent->hSession		= hSession;

	if(BUS_PostMessageToOM(NULL, eEvent, HANDLE_NULL, (HINT32)eDxCAS_GROUPID_VX, (HINT32)ulSlotNumber, (HINT32)pstEvent) != ERR_BUS_NO_ERROR)
	{
		HxLOG_Error("BUS_PostMessageToOM() error\n");
		return ERR_FAIL;
	}

	return ERR_OK;
}

// HLIB Codepage issue로 임시로 추가한 API
void xmgr_cas_VmxUtil_ConvertLanguageCode(HUINT8 *pszMessage)
{
	if(pszMessage[0] == EN300468_8859_3BTYE_FIRST_VALUE && pszMessage[1] == EN300468_8859_3BTYE_2ND_VALUE)
	{
		HUINT32 ulStrOnlyLen = MWC_UTIL_DvbStrlenStrOnly(pszMessage);

		switch(pszMessage[2])
		{
			case EN300468_8859_5_3BYTE_3TH_VALUE:
				pszMessage[2] = eHxCP_DVBLANG_VALUE_8859_05;
				break;

			default:
				return;
		}

		HxSTD_MemMove(&pszMessage[0], &pszMessage[2], ulStrOnlyLen);
		pszMessage[ulStrOnlyLen+1] = '\0';
	}
}

HCHAR* xmgr_cas_VmxUtil_GetIptvHybridVersionString(void)
{
	SvcCas_CtrlParam_t 					stParam;
	VMXIPTV_GetVersionStringOutParam_t	stOutParam;

	stParam.ulControlType	= eVmxIptvCtrl_GetVersionString;
	stParam.pvIn			= NULL;
	stParam.pvOut			= (void *)&stOutParam;

	if(SVC_CAS_CtrlGet(eDxCAS_GROUPID_VX, stParam.ulControlType, &stParam) != ERR_OK)
	{
		HxLOG_Error("SVC_CAS_CtrlSet(eVmxIptvCtrl_SetRatingLevel) error\n");
		return NULL;
	}

	return stOutParam.pszVmxVersion;
}

HCHAR* xmgr_cas_VmxUtil_GetWebVersionString(void)
{
	// xmgr_cas_VmxUtil_GetIptvHybridVersionString() 을 참고해서 구현 필요

	return NULL;
}

HCHAR* xmgr_cas_VmxUtil_GetGatewayVersionString(void)
{
	// xmgr_cas_VmxUtil_GetIptvHybridVersionString() 을 참고해서 구현 필요
	return NULL;
}

/* end of file */
