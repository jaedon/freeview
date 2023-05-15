/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include <octo_common.h>
#ifdef DxMAX_TS_NUM
#undef DxMAX_TS_NUM
#endif

#include <svc_cas.h>

#include "local_include/_xmgr_cas_ir.h"
#include "local_include/_xmgr_cas_ir_adapt.h"
#include "local_include/_xmgr_cas_ir_util.h"
#include "local_include/_xmgr_cas_ir_ui.h"

#include <mgr_action.h>

#include <ir_api.h>
#include <svc_pipe.h>
#include <ir_ctrl.h>
//#include <_svc_cas_mgr_control.h>


#if defined(CONFIG_DEBUG)
#include <ir_evt.h>
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)
#include <iruc_api.h>
#endif

/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Definitions    **************************/
/*******************************************************************/


#define	ApCasIrAdaptAssert	HxLOG_Error


/*******************************************************************/
/********************      Macro Definition     ********************/
/*******************************************************************/



/*******************************************************************/
/********************      Typedef         *************************/
/*******************************************************************/



/*******************************************************************/
/********************      Local Functions      ********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_ADAPT_LOCAL_FUNCTION______________________________________________________



/*******************************************************************/
/********************	   Public Functions 	********************/
/*******************************************************************/
#define _____XMGR_CAS_IR_ADAPT_PUBLIC_FUNCTION______________________________________________________
#if 0
void	xmgr_cas_ir_GetErrorCodeText(HINT32 nIrErrCode, HUINT8 *szErrorCodeText, HUINT32 ulTextBufSize)
{
	//PrintDebug("Error Code = %x\n", nIrErrCode);
	// Spec 711886-3.02.06 DVB Client integration guide 순서
	switch (nIrErrCode)
	{
	case AXI_STATUSCODE_IDX_ServiceUnknownE37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E37-32");
		break;
	case AXI_STATUSCODE_IDX_ServiceIsNotCurrentlyRunningE38:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E38-32");
		break;
	case AXI_STATUSCODE_IDX_LocatingServiceE39:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E39-32");
		break;
	case AXI_STATUSCODE_IDX_DecoderMemoryFullE40:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E40-32");
		break;
	case AXI_STATUSCODE_IDX_ServiceIsNotCurrentlyAvailableE41:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E41-32");
		break;
	case AXI_STATUSCODE_IDX_ParentalControlLockE42:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E42-32");
		break;
	case AXI_STATUSCODE_IDX_NotAllowedInThisCountryE43:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E43-32");
		break;
	case AXI_STATUSCODE_IDX_NoEventInformationE44:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E44-32");
		break;
	case AXI_STATUSCODE_IDX_ServiceNotAllowedE45:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E45-32");
		break;
	case AXI_STATUSCODE_IDX_ReadingSatInformationE46:				// 10
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E46-32");
		break;
	case AXI_STATUSCODE_IDX_BouquetBlockE47:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E47-32");
		break;
	case AXI_STATUSCODE_IDX_NoSignalE48:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E48-32");
		break;
	case AXI_STATUSCODE_IDX_LNBOverloadE49:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E49-32");
		break;
	case AXI_STATUSCODE_IDX_NoServicesAvailableE50:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E50-32");
		break;
	case AXI_STATUSCODE_IDX_SearchingForSignalE52:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E52-32");
		break;
	case AXI_STATUSCODE_IDX_InvalidTuningParametersE64:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E64-32");
		break;
	case AXI_STATUSCODE_IDX_ServiceNotScrambledE00:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E00-32");
		break;
	case AXI_STATUSCODE_IDX_IncorrectPINE53:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E53-32");
		break;
	case AXI_STATUSCODE_IDX_IPPVOKE54:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E54-32");
		break;
	case AXI_STATUSCODE_IDX_CAMnotCompatibleE56:					// 20
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E56-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E57:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E57-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E58:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E58-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E66:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E66-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E67:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E67-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E68:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E68-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E69:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E69-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E01:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E01-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E02:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E02-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E03:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E03-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E14:								// 30
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E14-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E35:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E35-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E36:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E36-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E70:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E70-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E71:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E71-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E72:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E72-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E73:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E73-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E74:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E74-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E75:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E75-32");
		break;
	case AXI_STATUSCODE_IDX_TODO_E76:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E76-32");
		break;

	// Message Error
	case AXI_STATUSCODE_IDX_E100_2:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E100-2");
		break;
	case AXI_STATUSCODE_IDX_F101_2:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"F101-2");
		break;
	case AXI_STATUSCODE_IDX_F102_2:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"F102-2");
		break;

	// General
	case AXI_STATUSCODE_IDX_D100_3:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-3");
		break;
	case AXI_STATUSCODE_IDX_E101_3:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-3");
		break;

	// DVB client status message
	// DVB EMM Service
	case AXI_STATUSCODE_IDX_D100_5:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-5");
		break;
	// DVB Descrambler Service
	case AXI_STATUSCODE_IDX_D100_8:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-8");
		break;
	// PVR Record Service
	case AXI_STATUSCODE_IDX_D100_27:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-27");
		break;
	// PVR Playback Service
	case AXI_STATUSCODE_IDX_D100_28:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-28");
		break;
	// Smartcard Resource
	case AXI_STATUSCODE_IDX_D00_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D00-4");
		break;
	case AXI_STATUSCODE_IDX_E04_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E04-4");
		break;
	case AXI_STATUSCODE_IDX_E05_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E05-4");
		break;
	case AXI_STATUSCODE_IDX_E06_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E06-4");
		break;
	case AXI_STATUSCODE_IDX_I07_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I07-4");
		break;
	// EMM Source (CAT) Resource
	case AXI_STATUSCODE_IDX_D100_6:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-6");
		break;
	case AXI_STATUSCODE_IDX_D101_6:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D101-6");
		break;
	case AXI_STATUSCODE_IDX_E102_6:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E102-6");
		break;
	case AXI_STATUSCODE_IDX_D103_6:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D103-6");
		break;
	case AXI_STATUSCODE_IDX_D104_6:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D104-6");
		break;
	case AXI_STATUSCODE_IDX_E105_6:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E105-6");
		break;
	// ECM Source (PMT) Resource
	case AXI_STATUSCODE_IDX_D100_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-9");
		break;
	case AXI_STATUSCODE_IDX_D101_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D101-9");
		break;
	case AXI_STATUSCODE_IDX_E16_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E16-4");
		break;
	case AXI_STATUSCODE_IDX_E18_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E18-4");
		break;
	case AXI_STATUSCODE_IDX_E19_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E19-4");
		break;
	case AXI_STATUSCODE_IDX_E20_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E20-4");
		break;
	case AXI_STATUSCODE_IDX_E21_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E21-4");
		break;
	case AXI_STATUSCODE_IDX_D29_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D29-4");
		break;
	case AXI_STATUSCODE_IDX_E30_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E30-4");
		break;
	case AXI_STATUSCODE_IDX_E32_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E32-4");
		break;
	case AXI_STATUSCODE_IDX_E33_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E33-4");
		break;
	case AXI_STATUSCODE_IDX_I34_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I34-4");
		break;
	case AXI_STATUSCODE_IDX_E80_35:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E80-35");
		break;
	case AXI_STATUSCODE_IDX_E81_35:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E81-35");
		break;
	case AXI_STATUSCODE_IDX_E100_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E100-4");
		break;
	case AXI_STATUSCODE_IDX_E101_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-4");
		break;
	case AXI_STATUSCODE_IDX_D102_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D102-9");
		break;
	case AXI_STATUSCODE_IDX_I102_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I102-4");
		break;
	case AXI_STATUSCODE_IDX_D103_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D103-9");
		break;
	case AXI_STATUSCODE_IDX_E104_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E104-9");
		break;
	case AXI_STATUSCODE_IDX_D105_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E105-9");
		break;
	case AXI_STATUSCODE_IDX_E106_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E106-9");
		break;
	case AXI_STATUSCODE_IDX_E107_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E107-4");
		break;
	case AXI_STATUSCODE_IDX_E108_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E108-4");
		break;
	case AXI_STATUSCODE_IDX_E109_4: // E109 ~ E123 이 여기로 날아 온다.
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E109-4"); // 다른게 처리 해야함.
		break;
#if 0
	case AXI_STATUSCODE_IDX_E116_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E116-4");
		break;
	case AXI_STATUSCODE_IDX_E117_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E117-4");
		break;
	case AXI_STATUSCODE_IDX_E118_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E118-4");
		break;
#endif

	case AXI_STATUSCODE_IDX_E124_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E124-4");
		break;
	// DVB Descrambler Resource
	case AXI_STATUSCODE_IDX_D100_10:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-10");
		break;
	case AXI_STATUSCODE_IDX_E101_10:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-10");
		break;
	// Monitoring Resource
	case AXI_STATUSCODE_IDX_D100_11:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-11");
		break;
	case AXI_STATUSCODE_IDX_E101_11:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-11");
		break;
	// IPPV Module Status
	case AXI_STATUSCODE_IDX_D22_12:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D22-12");
		break;
	case AXI_STATUSCODE_IDX_D23_12:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D23-12");
		break;
	case AXI_STATUSCODE_IDX_E24_12:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E24-12");
		break;
	case AXI_STATUSCODE_IDX_D25_12:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D25-12");
		break;
	case AXI_STATUSCODE_IDX_D26_12:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D26-12");
		break;
	case AXI_STATUSCODE_IDX_E27_12:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E27-12");
		break;
	case AXI_STATUSCODE_IDX_E28_12:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E28-12");
		break;
	// SC Marriage Module status
	case AXI_STATUSCODE_IDX_E17_13:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E17-13");
		break;
	// Session Management Resource
	case AXI_STATUSCODE_IDX_D100_29:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-29");
		break;
	case AXI_STATUSCODE_IDX_E101_29:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-29");
		break;
//check it -->	case IR_EVT_DecIdMsg_PVR_NoMsk:
	case AXI_STATUSCODE_IDX_E102_29:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E102-29");
		break;
	case AXI_STATUSCODE_IDX_E103_29:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E103-29");
		break;
	case AXI_STATUSCODE_IDX_E104_29:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E104-29");
		break;
	// Crypto Operation Resource
	case AXI_STATUSCODE_IDX_D100_30:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-30");
		break;
	case AXI_STATUSCODE_IDX_E101_30:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-30");
		break;
	case AXI_STATUSCODE_IDX_D102_30:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D102-30");
		break;
	// Macrovision Resource
	case AXI_STATUSCODE_IDX_D100_31:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-31");
		break;
	case AXI_STATUSCODE_IDX_E101_31:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-31");
		break;

#if defined(CONFIG_MW_CAS_IRDETO_MATURITY_RATING)
	case AXI_STATUSCODE_IDX_E136_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E136-4");
		break;
	case AXI_STATUSCODE_IDX_E137_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E137-4");
		break;
	case AXI_STATUSCODE_IDX_E138_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E138-4");
		break;
	case IR_EVT_MRPIN_ReCheck:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"");
		break;
#endif
	case AXI_STATUSCODE_IDX_E133_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E133-4");
		break;
	case AXI_STATUS_CODE_IDX_E127_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E127-4");
		break;
	case AXI_STATUS_CODE_IDX_E128_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E128-4");
		break;
	case AXI_STATUSCODE_IDX_E129_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E129-4");
		break;
	case AXI_STATUS_CODE_IDX_E130_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E130-4");
		break;
	case AXI_STATUS_CODE_IDX_E131_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E131-4");
		break;
	case AXI_STATUS_CODE_IDX_E132_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E132-4");
		break;
	case AXI_STATUS_CODE_IDX_E134_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E134-4");
		break;
#if 0
	case AXI_STATUSCODE_IDX_E140_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E140-4");
		break;
	case AXI_STATUSCODE_IDX_E145_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E145-4");
		break;
	case AXI_STATUSCODE_IDX_E151_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E151-4");
		break;
#endif
#if 1//defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
	case AXI_STATUS_CODE_IDX_E152_4:	// Preview Time out
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E152-4");
		break;
#endif
#if 0
	case AXI_STATUSCODE_IDX_E154_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E154-4");
		break;
	case AXI_STATUSCODE_IDX_D200_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D200-4");
		break;
	case AXI_STATUSCODE_IDX_E201_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E201-4");
		break;
#endif
#if defined(CONFIG_OP_DIGITURK)
	case AXI_STATUSCODE_IDX_E501:	// Expired content delete message, Digiturk에서만 사용
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E501");
		break;
	case AXI_STATUSCODE_IDX_E120_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E120-4");
		break;
#endif

	case AXI_STATUSCODE_IDX_I34_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I34-9");
		break;
#if defined(CONFIG_APUI_TRUEVISION)
	case AXI_STATUSCODE_IDX_I55_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I55-4");
		break;
#endif
	//	for DRM functions
	case AXI_STATUSCODE_IDX_E110_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E110-37");
		break;
	case AXI_STATUSCODE_IDX_E111_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E111-37");
		break;
	case AXI_STATUSCODE_IDX_E112_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E112-37");
		break;
	case AXI_STATUSCODE_IDX_I113_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"I113-37");
		break;
	case AXI_STATUSCODE_IDX_I114_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"I114-37");
		break;
	case AXI_STATUSCODE_IDX_E115_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E115-37");
		break;
	case AXI_STATUSCODE_IDX_D116_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E116-37");
		break;
	case AXI_STATUSCODE_IDX_E117_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E117-37");
		break;
	case AXI_STATUSCODE_IDX_E118_37:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E118-37");
		break;

	//DRM - SOURCE_SMARTCARD
	case AXI_STATUSCODE_IDX_D126_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"D126-4");
		break;
#if defined(CONFIG_OP_ALMAJD)
	case AXI_STATUSCODE_IDX_ALMAJD_240:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E30-4"); // irdeto error code 는 E30-4 이다. almajd 는 error code 를 두개 표시한다.
		break;
	case AXI_STATUSCODE_IDX_ALMAJD_254:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E30-4"); // irdeto error code 는 E30-4 이다. almajd 는 error code 를 두개 표시한다.
		break;
	case AXI_STATUSCODE_IDX_ALMAJD_255:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E30-4"); // irdeto error code 는 E30-4 이다. almajd 는 error code 를 두개 표시한다.
		break;
#endif
	default:
		snprintf((char*)szErrorCodeText, ulTextBufSize, "E?? (%03X)", nIrErrCode);		// prevent fix
		break;
	}
}
#endif

void	xmgr_cas_ir_GetErrorText(HINT32	nIrErrCode, HUINT8 *szErrorText)
{
	HUINT8			*szErrCodeText;

	if (szErrorText == NULL)
	{
		return;
	}

	szErrCodeText = OxAP_Malloc (64);
	if (szErrCodeText == NULL)
	{
		goto ERROR;
	}
	xmgr_cas_iruc_GetErrorCodeText(nIrErrCode, szErrCodeText, 64);
	snprintf((char*)szErrorText, 64, "%s", szErrCodeText);

ERROR:
	if (szErrCodeText != NULL)				{ OxAP_Free (szErrCodeText); }



}


HERROR		xmgr_cas_ir_AlarmDataProcessing(void *pDataNa, HUINT8 *alarm_type)
{
	//return xmgr_cas_ir_alarm_message_function(pDataNa, alarm_type);
	*alarm_type = eCAS_IR_ALARM_BANNER;
	return ERR_OK;
}





#if defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
void	xmgr_cas_ir_GetPreviewTimeText(HUINT8	nLeftTime, HUINT8 *szErrorText)
{
	HUINT8			szTmpDest[256/*IR_TEXT_SIZE256*/]={0,};

	if (szErrorText == NULL)
	{
		return;
	}

	MWC_UTIL_DvbUtf8Sprintf(szTmpDest, "VIEW_WITH_PREVIEW %d", nLeftTime);
	MWC_UTIL_DvbStrcpyStrOnly(szErrorText, szTmpDest);
}
#endif

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)	// ewlee 20100720
#if defined(CONF_IR_UCCA_STATUS_CODE_FULL_SUPPORT) // Device Integration overview (rev.34.0) 및 UniveralClient_API.h for  2.8.0 library 를 참조하여 정리함.
void	xmgr_cas_iruc_GetErrorCodeText(HINT32	nIrErrCode, HUINT8 *szErrorCodeText, HUINT32 ulTextBufSize)
{
	switch (nIrErrCode)
	{
#if 0	
	// D, I Intanal Status String은 Universalclient_API.h @ cca_2.7
	case IRUC_STATUS_MSG_D029_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D029-0"); // "Service is currently descrambled"
		break;
	case IRUC_STATUS_MSG_D100_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-0"); // "EMM service OK or FTA service."
		break;
	case IRUC_STATUS_MSG_D101_0: // 기존 코드과 달리 spec 2.8에 따라 출력 ... 
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D101-0"); // "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_D126_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D126-0"); // "PVR Record Request Accepted"
		break;
	case IRUC_STATUS_MSG_D127_0:		
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D127-0"); // "PVR Playback Request Accepted"
		break;
	case IRUC_STATUS_MSG_D500_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D500-0"); // "CCA package successfully loaded."
		break;
	case IRUC_STATUS_MSG_D000_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D000-4"); // "Card In"
		break;
	case IRUC_STATUS_MSG_D029_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D029-4"); // "Service is currently descrambled"
		break;
	case IRUC_STATUS_MSG_D100_5:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-5"); // "DVB EMM Service Ok"
		break;
	case IRUC_STATUS_MSG_D100_8:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-8");// "Descrambler Service Ok"
		break;
	case IRUC_STATUS_MSG_D101_6: 
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D101-6");// "No Matched Vendor ID."
		break;
	case IRUC_STATUS_MSG_D102_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D102-9");// "No Vendor ID"
		break;
	case IRUC_STATUS_MSG_D105_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D105-9");// "FTA Service"
		break;
	case IRUC_STATUS_MSG_D106_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D106-9");// "Please insert the correct Smartcard."
		break;
	case IRUC_STATUS_MSG_I101_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I101-0");// "Incomplete Definition"
		break;
	case IRUC_STATUS_MSG_I102_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I102-0");// "No ECM/EMM data received."
		break;
	case IRUC_STATUS_MSG_I300_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I300-0");// "New CG for FSU product has been received."
		break;
	case IRUC_STATUS_MSG_I301_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I301-0"); // "New CG for Push VOD product has been received."
		break;
	case IRUC_STATUS_MSG_I302_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I302-0");// "CG Mismatch."
		break;
	case IRUC_STATUS_MSG_I303_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I303-0");// "SG Mismatch."
		break;
	case IRUC_STATUS_MSG_I304_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I304-0");// "Invalid P-Key Index."
		break;
	case IRUC_STATUS_MSG_I305_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I305-0");// Invalid G-Key Index.
		break;
	case IRUC_STATUS_MSG_I306_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I306-0");// "Invalid Timestamp received."
		break;
	case IRUC_STATUS_MSG_I307_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I307-0");// "Variant Mismatch."
		break;
	case IRUC_STATUS_MSG_I308_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I308-0");// "P-Key Hash Mismatch."
		break;
	case IRUC_STATUS_MSG_I309_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I309-0");// "Opcode Restricted."
		break;
	case IRUC_STATUS_MSG_I310_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I310-0");//"Invalid VOD Nonce."
		break;
	case IRUC_STATUS_MSG_I311_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I311-0");// "TMS Failed."
		break;
	case IRUC_STATUS_MSG_I312_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I312-0");// "Homing Channel Failed."
		break;
	case IRUC_STATUS_MSG_I313_0:  // 
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I313-0");// "Invalid Middleware User Data."
		break;
	case IRUC_STATUS_MSG_I314_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I314-0");// "Flexi Flash Failed."
		break;
	case IRUC_STATUS_MSG_I315_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I315-0");// SN Mapping EMM handled successfully.
		break;
	case IRUC_STATUS_MSG_I316_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I316-0");// No valid entitlement found.
		break;
	case IRUC_STATUS_MSG_I317_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I317-0");// "No valid sector found."
		break;
	case IRUC_STATUS_MSG_I318_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I318-0");// "Sector Overwrite EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I319_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I319-0");// "Pairing EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I320_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I320-0");// "Product Key EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I321_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I321-0");// "Product Overwrite EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I322_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I322-0");// "Region Control EMM handled successfully."
		break;
/*	case IRUC_STATUS_MSG_I323_0:  // IRDETO PVR은 지원되지 않는다. 
		MWC_UTIL_DvbStrcpy(szErrText, GetStrRsc()); // "PVR MSK EMM handled successfully."
		MWC_UTIL_DvbUtf8Sprintf((char*)szErrorText, "I323-0  %s", szErrText);
		break;*/
	case IRUC_STATUS_MSG_I324_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I324-0");// "VOD Product Overwrite EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I325_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I325-0");// "VOD Asset ID EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I326_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I326-0");// "Nationality EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I327_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I327-0");// "Product Delete EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I328_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I328-0");// "Entitlement Property EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I329_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I329-0");// "Timestamp EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I330_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I330-0");// "P-Key mismatch."
		break;
	case IRUC_STATUS_MSG_I331_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I331-0");// "User Area EMM handled successfully."
		break;		
	case IRUC_STATUS_MSG_I332_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I332-0");// "Application Data EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I333_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I333-0");// "Filter Criteria EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I334_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I334-0");// "Package EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I335_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I335-0");// "Block Download EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I336_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I336-0");// "IRD EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I337_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I337-0");// "Unique SN Mapping EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I338_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I338-0");// "Signed CCP CAM EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I339_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I339-0"); // "TM Message EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I340_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I340-0");// "Macrovision Configuration EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I341_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I341-0");// "Extended TMS Message EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I342_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I342-0");// "Reset To Factory State EMM handled successfully."
		break;
	case IRUC_STATUS_MSG_I343_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I343-0");// "Invalid change version."
		break;
	case IRUC_STATUS_MSG_I344_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I344-0");// "Block Download EMM duplication."
		break;		
	case IRUC_STATUS_MSG_I007_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I007-4");// "Checking Smartcard."
		break;
	case IRUC_STATUS_MSG_I034_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I034-9");// "Validating subscription, please wait."
		break;
	case IRUC_STATUS_MSG_I055_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I055-4");// "Subscription update in progress."
		break;
	case IRUC_STATUS_MSG_I102_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I102-4");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_I139_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"I139-4"); // "Validating subscription, please wait."
		break;
	case IRUC_STATUS_MSG_E104_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E104-0");// "Invalid EMM received."
		break;
	case IRUC_STATUS_MSG_E150_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E150-0");// "The Pre-Enablement product is expired."
		break;
	case IRUC_STATUS_MSG_E151_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E151-0");// "PESK mismatch."
		break;		
	case IRUC_STATUS_MSG_E501_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E501-0"); // "FlexiFlash not initialized."
		break;
	case IRUC_STATUS_MSG_E502_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E502-0"); //"Invalid CFG data."
		break;
	case IRUC_STATUS_MSG_E503_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E503-0");// "Invalid CCA package data."
		break;
	case IRUC_STATUS_MSG_E504_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E504-0");// "CCA package not loaded."
		break;
	case IRUC_STATUS_MSG_E505_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E505-0");// "Invalid CCA package index."
		break;
	case IRUC_STATUS_MSG_E506_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E506-0");// "Duplicated VM ID."
		break;
	case IRUC_STATUS_MSG_E507_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E507-0");// "Creat VM context failed."
		break;
	case IRUC_STATUS_MSG_E508_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E508-0");// "Creat VM failed."
		break;
	case IRUC_STATUS_MSG_E509_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E509-0");// "Too many vm created."
		break;
	case IRUC_STATUS_MSG_E510_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E510-0");// "VM load image failed."
		break;		
	case IRUC_STATUS_MSG_E511_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E511-0"); // "VM memmap failed."
		break;				
	case IRUC_STATUS_MSG_E512_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E512-0"); // "VM execution failed."
		break;		
	case IRUC_STATUS_MSG_E513_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E513-0");// "VM IO failed."
		break;		
	case IRUC_STATUS_MSG_E514_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E514-0");// "VM reset failed.."
		break;		
	case IRUC_STATUS_MSG_E515_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E515-0");// "Root key check failed."
		break;		
	case IRUC_STATUS_MSG_E516_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E516-0");// "Invalid package signature."
		break;		
	case IRUC_STATUS_MSG_E517_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E517-0");// "Unsupported compress algorithm."
		break;		
	case IRUC_STATUS_MSG_E518_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E518-0"); // "Invalid SYS ID."
		break;		
	case IRUC_STATUS_MSG_E519_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E5129-0"); // "Client type mismatch."
		break;		
	case IRUC_STATUS_MSG_E520_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E520-0");// "The client version too low to support FlexiFlash download."
		break;				
	case IRUC_STATUS_MSG_E521_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E521-0");// "The client version too low to support FlexiFlash download."
		break;
#endif
// 4.3 Error Banner
	case IRUC_STATUS_MSG_E016_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E016-0");// "No permission to view this channel."
		break;		
	case IRUC_STATUS_MSG_E017_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E017-0");// "No permission to view this channel."
		break;
	case IRUC_STATUS_MSG_E018_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E018-0");	// "No permission to view this channel."
		break;
	case IRUC_STATUS_MSG_E030_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E030-0"); // "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E031_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E031-0");// "Currently unable to view this channel."
		break;		
	case IRUC_STATUS_MSG_E032_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E032-0");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E033_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E033-0");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E055_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E055-0");// "No permission to view this channel."
		break;
	case IRUC_STATUS_MSG_E094_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E094-0");// "Unable to playback the recording."
		break;
	case IRUC_STATUS_MSG_E101_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-0");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E102_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E102-0");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E103_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E103-0");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E106_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E106-0");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E111_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E111-0");// "No permission to carry out any recording or playback."
		break;		
	case IRUC_STATUS_MSG_E128_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E128-0");// "No permission to carry out any recording or playback."
		break;		
	case IRUC_STATUS_MSG_E131_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E131-0");// "No permission to carry out any recording or playback."
		break;
	case IRUC_STATUS_MSG_E129_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E129-0");// "Unable to playback the recording."
		break;
	case IRUC_STATUS_MSG_E130_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E130-0");// "Currently unable to carry out any recording."
		break;
	case IRUC_STATUS_MSG_E139_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E139-0");// "Validating subscription, please wait."
		break;
	case IRUC_STATUS_MSG_E140_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E140-0");// "This channel is not available in this location."
		break;
	case IRUC_STATUS_MSG_E600_0:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E600-0"); // "Receiver is not enabled yet to view this service."
		break;
// 5.2.2. Error Banner
	case IRUC_STATUS_MSG_E000_32:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E000-32");// "Service not scrambled"
		break;
	case IRUC_STATUS_MSG_E004_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E004-4");// "Please insert Smartcard"
		break;
	case IRUC_STATUS_MSG_E005_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E005-4");// "Unknown Smartcard"
		break;
	case IRUC_STATUS_MSG_E006_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E006-4");// "Smartcard Failure"
		break;
	case IRUC_STATUS_MSG_E016_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E016-4");// "No permission to view this channel."
		break;
	case IRUC_STATUS_MSG_E017_13:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E017-13");// "Service is currently scrambled."
		break;
	case IRUC_STATUS_MSG_E018_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E018-4");// "This channel is not available in this location."
		break;
	case IRUC_STATUS_MSG_E030_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E030-4");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E032_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E032-4");// "Viewing is temporarily blocked, Please stay tuned"
		break;
	case IRUC_STATUS_MSG_E033_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E033-4");// Check if the correct smart card is inserted.
		break;
	case IRUC_STATUS_MSG_E038_32:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E038-32");// "Service is not currently running."
		break;
	case IRUC_STATUS_MSG_E045_32:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E045-32");// "Service not allowed"
		break;
	case IRUC_STATUS_MSG_E080_35:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E080-35");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E081_35:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E081-35");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E100_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E100-4");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E101_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-4");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E106_9:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E106-9"); // "Please insert the correct Smartcard."
		break;
	case IRUC_STATUS_MSG_E107_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E107-4");// "The Smartcard is not fully authorised."
		break;
	case IRUC_STATUS_MSG_E108_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E108-4");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E109_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E109-4");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E118_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E118-4");// "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E136_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E136-4");// "This channel is age restricted."
		break;
	case IRUC_STATUS_MSG_E137_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E137-4"); // "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E140_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E140-4");// "This channel is not available in this location."
		break;
	case IRUC_STATUS_MSG_E141_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E141-4"); // "Currently unable to view this channel."
		break;
	case IRUC_STATUS_MSG_E142_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E142-4");// "Insert the secondary card into the primary decoder to continue viewing with the secondary decoder."
		break;
	case IRUC_STATUS_MSG_E144_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E144-4");// "Please wait. If the problem persists, insert the secondary card into the primary decoder to continue viewing with the secondary decoder."
		break;
	case IRUC_STATUS_MSG_E152_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E152-4");// "Preview time period time out"
		break;
	case IRUC_STATUS_MSG_E037_32:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E037-32");// AXI_STATUSCODE_IDX_ServiceUnknownE37, // Service Unknown from Humax
		break;
	case IRUC_STATUS_MSG_E044_32:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E044-32");// AXI_STATUSCODE_IDX_NoEventInformationE44, // No Event Information from Humax
		break;
	case IRUC_STATUS_MSG_E048_32:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E048-32");// AXI_STATUSCODE_IDX_NoSignalE48, // No Signal from Humax
		break;
	case IRUC_STATUS_MSG_E052_32:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E052-32");// AXI_STATUSCODE_IDX_SearchingForSignalE52, // Searching for signal from Humax
		break;
	case IRUC_STATUS_MSG_E042_32:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E042-32");// AXI_STATUSCODE_IDX_ParentalControlLockE42, // Parental Control Lock (and/or Ask for Parental PIN)
		break;
	}
}

void	xmgr_cas_iruc_ExtendErrorText(HINT32 nIrErrCode, HUINT8 *szErrorCodeText, HUINT32 ulTextBufSize)
{
	HUINT8 *szErrText	= NULL;
	HUINT8 *szClientID	= NULL;
	HIR_ERR	err = HIR_OK;
	HUINT32 		ulTmsDataLen = 0;
	IRUC_CLIENT_STATUS	*pstClientStatus = NULL;

	szErrText	= OxAP_Malloc (TEXT_SIZE256);
	szClientID	= OxAP_Malloc (TEXT_SIZE64);
	pstClientStatus = OxAP_Malloc(sizeof(IRUC_CLIENT_STATUS));		
	if( szErrText == NULL )	{ goto ERROR; }
	if( szClientID == NULL) { goto ERROR; }
	if( pstClientStatus == NULL) { goto ERROR; }

	err = xsvc_cas_IrUcGetClientID(szClientID, TEXT_SIZE64);
	if (err != HIR_OK) { goto ERROR; }

	HxSTD_memset(pstClientStatus, 0, sizeof(IRUC_CLIENT_STATUS));
	xsvc_cas_IrUcGetStatusClient(pstClientStatus, &ulTmsDataLen);

	switch (nIrErrCode)
	{
		case IRUC_STATUS_MSG_E016_0:
		case IRUC_STATUS_MSG_E017_0:
		case IRUC_STATUS_MSG_E018_0:
		case IRUC_STATUS_MSG_E030_0:
		case IRUC_STATUS_MSG_E031_0:
		case IRUC_STATUS_MSG_E032_0:
		case IRUC_STATUS_MSG_E033_0:
		case IRUC_STATUS_MSG_E055_0:
		case IRUC_STATUS_MSG_E094_0:
		case IRUC_STATUS_MSG_D101_0:
		case IRUC_STATUS_MSG_E101_0:
		case IRUC_STATUS_MSG_E102_0:
		case IRUC_STATUS_MSG_E103_0:
		case IRUC_STATUS_MSG_E106_0:
		case IRUC_STATUS_MSG_E111_0:
		case IRUC_STATUS_MSG_E128_0:
		case IRUC_STATUS_MSG_E131_0:
		case IRUC_STATUS_MSG_E129_0:
		case IRUC_STATUS_MSG_E130_0:
		case IRUC_STATUS_MSG_E139_0:
		case IRUC_STATUS_MSG_E140_0:
		case IRUC_STATUS_MSG_E600_0:
			/* client number만 추가 */
			snprintf(szErrText, TEXT_SIZE256, " %s", szClientID); 
			MWC_UTIL_DvbStrcat(szErrorCodeText, szErrText); // Client ID string 추가 
			break;
			
		case IRUC_STATUS_MSG_E016_4: 
		case IRUC_STATUS_MSG_E030_4:
		case IRUC_STATUS_MSG_E033_4:
		case IRUC_STATUS_MSG_E080_35:
		case IRUC_STATUS_MSG_E081_35:
		case IRUC_STATUS_MSG_E100_4:
		case IRUC_STATUS_MSG_E101_4:
		case IRUC_STATUS_MSG_E106_9:
		case IRUC_STATUS_MSG_E108_4:			
		case IRUC_STATUS_MSG_E109_4:
		case IRUC_STATUS_MSG_E118_4:
		case IRUC_STATUS_MSG_E137_4:
		case IRUC_STATUS_MSG_E141_4:
			/* Smart card number 만추가*/
			snprintf(szErrText, TEXT_SIZE256, " %s", (char *)pstClientStatus->ucScserialNumber);	
			MWC_UTIL_DvbStrcat(szErrorCodeText, szErrText); // Smart card number 추가 
			break;

		case IRUC_STATUS_MSG_E107_4:
			/* Smart card number 및 CSSN 추가 */
			snprintf(szErrText, TEXT_SIZE256, " %s %02X%02X%02X%02X", (char *)pstClientStatus->ucScserialNumber, \
				pstClientStatus->ucCSSN[0], pstClientStatus->ucCSSN[1],pstClientStatus->ucCSSN[2],pstClientStatus->ucCSSN[3]);
			MWC_UTIL_DvbStrcat(szErrorCodeText, szErrText); // Smart card number 및 CSSN 추가 
			break;
	}
ERROR:
	if (szErrText != NULL)			{ OxAP_Free (szErrText); }
	if (szClientID != NULL) 			{ OxAP_Free (szClientID); }
	if (pstClientStatus != NULL)		{ OxAP_Free (pstClientStatus); }
}
#else 
// ewlee 20100219 - get UC status message
void	xmgr_cas_iruc_GetErrorCodeText(HINT32	nIrErrCode, HUINT8 *szErrorCodeText, HUINT32 ulTextBufSize)
{
/*
   TODO: hjlee3 string 집어 넣어야 한다.
   확인 해서 집어 넣어라.
*/
	switch (nIrErrCode)
	{
	case IRUC_STATUS_MSG_D029:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D029-0");
		break;
	case IRUC_STATUS_MSG_D100:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D100-0");
		break;
	case IRUC_STATUS_MSG_D101:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D101-0");
		break;
	case IRUC_STATUS_MSG_D126:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D126-0");
		break;
	case IRUC_STATUS_MSG_D127:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D127-0");
		break;
	case IRUC_STATUS_MSG_E101:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E101-0");
		break;
	case IRUC_STATUS_MSG_E102:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E102-0");
		break;
	case IRUC_STATUS_MSG_E103:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E103-0");
		break;
	case IRUC_STATUS_MSG_E104:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E104-0");
		break;
	case IRUC_STATUS_MSG_E106:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E106-0");
		break;
	case IRUC_STATUS_MSG_E016:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E016-0");
		break;
	case IRUC_STATUS_MSG_E017:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E017-0");
		break;
	case IRUC_STATUS_MSG_E030:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E030-0");
		break;
	case IRUC_STATUS_MSG_E031:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E031-0");
		break;
	case IRUC_STATUS_MSG_E032:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E032-0");
		break;
	case IRUC_STATUS_MSG_E033:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E033-0");
		break;
	case IRUC_STATUS_MSG_E055:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E055-0");
		break;
	case IRUC_STATUS_MSG_E139:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E139-0");
		break;
	case IRUC_STATUS_MSG_E140:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E140-0");
		break;
	case IRUC_STATUS_MSG_E130:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E130-0");
		break;
	case IRUC_STATUS_MSG_E128:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E128-0");
		break;
	case IRUC_STATUS_MSG_E092:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E092-0");
		break;
	case IRUC_STATUS_MSG_E131:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E131-0");
		break;
	case IRUC_STATUS_MSG_E094:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E094-0");
		break;
	case IRUC_STATUS_MSG_E095:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E095-0");
		break;
	case IRUC_STATUS_MSG_E135:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E135-0");
		break;
	}
}
#endif
#endif

#define _______Debug_Command____________________________________

#if defined(CONFIG_DEBUG)
STATIC HIR_ERR	xmgr_cas_ir_GetForcedMail (HUINT8 *pucMailIndex)
{
	HIR_ERR				err;
	HUINT8				i, ucCount;
	HUINT8				aucList[MAX_NUMBER_MAIL_MESSAGE];
	MAILBOX_TABLE		mailbox;
	SvcCas_CtrlParam_t			stIrCtrlParam;
	IR_GetMailListOutParam_t	stMailList;
	IR_GetMailStateOutParam_t	stMailReadState;
	IR_GetMailBoxOutParam_t  	stMailBox;

	if (pucMailIndex == NULL)
	{
		return HIR_INVALID_PARAMETER;
	}

	// IUC와의 호환을 위해 그냥 MW CAS_IR_ API를 사용한다.
	stIrCtrlParam.ulControlType			= eCACTRL_IR_GetMailList;
	stIrCtrlParam.pvOut				= &stMailList;
	stMailList.paucMailIndexList 			= aucList;
	err = SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailList, &stIrCtrlParam);
	if (err != HIR_OK)
	{
		return err;
	}
	ucCount = stMailList.ucNumberOfSavedMail;

	for (i = 0 ; i < ucCount ; i++)
	{
		stIrCtrlParam.ulControlType			= eCACTRL_IR_GetMailReadState;
		stIrCtrlParam.pvOut	= &stMailReadState;
		stMailReadState.ucMailIndex 			= aucList[i];

		if (SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailReadState, &stIrCtrlParam) != HIR_OK)
		{
			continue;
		}

		stIrCtrlParam.ulControlType 			= eCACTRL_IR_GetMailBox;
		stIrCtrlParam.pvOut				= &stMailBox;
		stMailBox.pstMailBox				= &mailbox;
		stMailBox.ucMailIndex 				= aucList[i];

		if (SVC_CAS_CtrlGet(eDxCAS_GROUPID_IR, eCACTRL_IR_GetMailBox, &stIrCtrlParam) != HIR_OK)
		{
			continue;
		}

		if (mailbox.msg_class != EPG_MESSAGE_CLASS_FORCED_DISPLAY)
		{
			continue;
		}

		if (mailbox.bAttributeMsg && mailbox.Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT)
		{
			continue;
		}

		*pucMailIndex = aucList[i];
		return HIR_OK;
	}
	return HIR_NOT_OK;
}
#endif

/* Debug Command msg Process */
void	XMGR_CAS_IR_CMD_Processing(HUINT8 *pszCmd, HUINT8 *pszMsgType, HUINT8 *pszmsg1, HUINT8 *pszmsg2)
{
#if defined(CONFIG_DEBUG) && 0
	HINT32			eMsg;
	SvcCas_MsgType_e		eMsgType = eCAS_MSG_TYPE_Ok;
	SvcCas_ShowType_e		eShowType = eCasShowType_Both;
	HERROR				errResult = ERR_FAIL;
	HMSG				msg;
	IREVT_Data			*pEvtData;

	STATIC	IREVT_Data			stEvtData;

	HxSTD_memset(&msg, 0, sizeof(HMSG));
#if	0
	pEvtData = OxCAS_Malloc(sizeof(IREVT_Data) );
#else
	pEvtData = &stEvtData;
#endif
	if( pEvtData == NULL)
		;
	//if(MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"forcetune") != 0)
	XMGR_CAS_IR_PARAM_UNUSED(pszmsg1);
	XMGR_CAS_IR_PARAM_UNUSED(pszmsg2);

	if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"sc") == 0)
	{
		eMsgType = eCAS_MSG_TYPE_Fail;
		eShowType = eCasShowType_None;
		if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"nocard") == 0)
		{
			eMsg = AXI_STATUSCODE_IDX_E04_4;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"sdtnorun") == 0)
		{
			eMsg = AXI_STATUSCODE_IDX_ServiceIsNotCurrentlyRunningE38;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"nosignal") == 0)
		{
			eMsg = AXI_STATUSCODE_IDX_NoSignalE48;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"svcok") == 0)
		{
			eMsg = AXI_STATUSCODE_IDX_D100_3;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"nopmt") == 0)
		{
			eMsg = AXI_STATUSCODE_IDX_D101_9;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"e30") == 0)
		{
			eMsg = AXI_STATUSCODE_IDX_E30_4;
		}
		else
		{
			eMsgType = eCAS_MSG_TYPE_Fail;
			eShowType = eCasShowType_Both;
			eMsg = IRUC_STATUS_MSG_NONE;
		}

		HxLOG_Print("cmd - smart card info![%d] \n", eMsg);

		if(pEvtData != NULL)
		{
			pEvtData->ulAxiCodeIdx = eMsg;
		}

	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"access") == 0)
	{
		if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"noeit") == 0)
		{
			eMsg = AXI_STATUSCODE_IDX_NoEventInformationE44;
		}
		else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"pclock") == 0)
		{
			eMsg = AXI_STATUSCODE_IDX_ParentalControlLockE42;
		}
		else
		{
			eMsg = IRUC_STATUS_MSG_NONE;
		}

		HxLOG_Print("cmd - access![%d] \n", eMsg);

		eMsgType = eCAS_MSG_TYPE_Fail;
		eShowType = eCasShowType_None;

		if(pEvtData != NULL)
		{
			pEvtData->ulAxiCodeIdx = eMsg;
		}

	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"mail") == 0)
	{
		HxLOG_Print("cmd - mail message![%s] \n", pszCmd);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;

		if(pEvtData != NULL)
		{
			if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"normal") == 0)
			{
				pEvtData->eIrEvtClass = IREVT_CLASS_DECODER_MSG_NORMAL_MAIL ;
			}
			else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"forced") == 0)
			{
				pEvtData->eIrEvtClass = IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY ;
				xmgr_cas_ir_GetForcedMail(&(pEvtData->ExtData.ucMail_Index));
			}
			else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"clear") == 0)
			{
				xsvc_cas_IrAllClearExpiredNormalAttributeMail();
			}
			else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"delete") == 0)
			{
				HUINT8					i;
				SvcCas_CtrlParam_t			stInParam;
				IR_DeleteMailInParam_t		stDeleteMail;

				stInParam.ulControlType		= eCACTRL_IR_DeleteMail;
				stInParam.pvIn				= &stDeleteMail;
				for(i=0; i<20; i++)
				{
					stDeleteMail.ucMailIndex = i;

					SVC_CAS_CtrlCall(eDxCAS_GROUPID_IR, eCACTRL_IR_DeleteMail, &stInParam);

				}
			}
		}

	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"fp") == 0)
	{
		HxLOG_Print("cmd - finger print![%s] \n", pszCmd);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;

		if(pEvtData != NULL)
		{
			if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"ecm") == 0)
			{
				pEvtData->eIrEvtClass =IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT;
				sprintf((char*)pEvtData->ExtData.aucHashedCode, "ECM-FP-MHOH-TEST");
			}
			else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"emm") == 0)
			{
				pEvtData->eIrEvtClass =IREVT_CLASS_DECODER_MSG_EMM_FINGERPRINT;
				sprintf((char*)pEvtData->ExtData.aucHashedCode, "EMM-FP-MHOH-TEST");
			}
			else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"attr") == 0)
			{
				pEvtData->eIrEvtClass =IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY;
				pEvtData->ExtData.ucMail_Index = (MAX_NUMBER_MAIL_MESSAGE -1);
			}
			else
			{
				HxLOG_Print("select ecm or emm \n");
			}
		}

	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"changepc") == 0)
	{
		HxLOG_Print("cmd - change pin code![%s] \n", pszCmd);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;

		if(pEvtData != NULL)
		{
			pEvtData->eIrEvtClass = IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE;
			pEvtData->ExtData.ulPinCodeBCDType = 0x1234;
		}
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"attr") == 0)
	{
		HxLOG_Print("cmd - Attributed Display![%s] \n", pszCmd);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;

		if(pEvtData != NULL)
		{
			pEvtData->eIrEvtClass = IREVT_CLASS_DECODER_MSG_ENHANCED_TEXT;
			sprintf((char*)pEvtData->ExtData.pucExt, "MHOH-TEST-ATTR-DISP");
		}
	}
	else if(MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"ota") == 0)
	{
		HxLOG_Print("cmd - OTA !![%s] \n", pszCmd);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;

		if(pEvtData != NULL)
		{
			if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"enable") == 0)
			{
				pEvtData->eIrEvtClass = IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE;
			}
			else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"disable") == 0)
			{
				pEvtData->eIrEvtClass = IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE;
			}
			else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"forced") == 0)
			{
				pEvtData->eIrEvtClass = IREVT_CLASS_DECODER_MSG_FORCED_DOWNLOAD;
			}
		}
	}
	else if (MWC_UTIL_DvbStrcmp(pszCmd, (HUINT8*)"pvrmsg") == 0)
	{
		HxLOG_Print("cmd - PVR Message![%s] \n", pszCmd);
		eMsgType = eCAS_MSG_TYPE_Command;
		eShowType = eCasShowType_Both;
		pEvtData->eIrEvtClass = IREVT_CLASS_DECODER_MSG_PVR;
		if(pEvtData != NULL)
		{
			if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"1") == 0)
			{
				pEvtData->ExtData.ulErrIndex = AXI_STATUSCODE_IDX_E16_4;
			}else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"2") == 0)
			{
				pEvtData->ExtData.ulErrIndex = AXI_STATUS_CODE_IDX_E127_4;
			}else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"3") == 0)
			{
				pEvtData->ExtData.ulErrIndex = AXI_STATUS_CODE_IDX_E128_4;
			}else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"4") == 0)
			{
				pEvtData->ExtData.ulErrIndex = AXI_STATUS_CODE_IDX_E130_4;
			}else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"5") == 0)
			{
				pEvtData->ExtData.ulErrIndex = AXI_STATUS_CODE_IDX_E131_4;
			}else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"6") == 0)
			{
				pEvtData->ExtData.ulErrIndex = AXI_STATUS_CODE_IDX_E132_4;
			}else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"7") == 0)
			{
				pEvtData->ExtData.ulErrIndex = AXI_STATUS_CODE_IDX_E134_4;
			}else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"8") == 0)
			{
				pEvtData->ExtData.ulErrIndex = AXI_STATUSCODE_IDX_I34_9;
			}else if (MWC_UTIL_DvbStrcmp(pszMsgType, (HUINT8*)"100") == 0)
			{
				pEvtData->ExtData.ulErrIndex = IR_EVT_F46;
			}else
				pEvtData->ExtData.ulErrIndex = IRUC_STATUS_MSG_NONE;
		}
	}
	else
	{
		HxLOG_Print("don't recognized[%s] message! \n", pszCmd);
	}

	errResult = svc_cas_MgrSubUpdateCasMessage(SVC_PIPE_GetActionHandle(MGR_ACTION_GetMainActionId()), eMsgType, eDxCAS_GROUPID_IR, (void*)pEvtData, eShowType);
	if (errResult != ERR_OK)
	{
		HxLOG_Critical("\n\n");
	}

#endif	// End of defined(CONFIG_DEBUG)
}

/* Command msg Process */
void	xmgr_cas_IrCMD(HUINT8 *pszCmd, HUINT8 *pszMsgType, HUINT8 *pszmsg1, HUINT8 *pszmsg2)
{
	XMGR_CAS_IR_CMD_Processing(pszCmd, pszMsgType, pszmsg1, pszmsg2);
}


// End of file

