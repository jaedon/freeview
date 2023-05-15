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

#if defined(CONFIG_MW_CAS_IRDETO_UCAS)	// ewlee 20100720
#include <iruc_api.h>	// ewlee 20100219
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
	case AXI_STATUSCODE_IDX_E116_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E116-4");
		break;
	case AXI_STATUSCODE_IDX_E117_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E117-4");
		break;
	case AXI_STATUSCODE_IDX_E118_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E118-4");
		break;
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

	case AXI_STATUSCODE_IDX_E140_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E140-4");
		break;
	case AXI_STATUSCODE_IDX_E145_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E145-4");
		break;
	case AXI_STATUSCODE_IDX_E151_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E151-4");
		break;

#if 1//defined(CONFIG_MW_CAS_IRDETO_PREVIEW)
	case AXI_STATUS_CODE_IDX_E152_4:	// Preview Time out
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8*)"E152-4");
		break;
#endif
	case AXI_STATUSCODE_IDX_E154_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E154-4");
		break;
	case AXI_STATUSCODE_IDX_D200_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"D200-4");
		break;
	case AXI_STATUSCODE_IDX_E201_4:
		MWC_UTIL_DvbStrcpy(szErrorCodeText, (HUINT8 *)"E201-4");
		break;

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

	xmgr_cas_ir_GetErrorCodeText(nIrErrCode, szErrCodeText, 64);

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

// ewlee 20100219 - get UC status message
#if defined(CONFIG_MW_CAS_IRDETO_UCAS)	// ewlee 20100720
void	xmgr_cas_iruc_GetErrorText(HINT32	nIrErrCode, HUINT8 *szErrorText)
{
	switch (nIrErrCode)
	{
	case IRUC_STATUS_MSG_D029:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_D029_BEING_DESCRAMBLED));
		break;
	case IRUC_STATUS_MSG_D100:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_D100_EMM_ACCEPTED));
		break;
	case IRUC_STATUS_MSG_D101:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_D101_NO_MATCHED_CA));
		break;
	case IRUC_STATUS_MSG_D126:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_D126_PVR_RECORD_ACCEPTED));
		break;
	case IRUC_STATUS_MSG_D127:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_D127_PVR_PLAYBACK_ACCEPTED));
		break;
	case IRUC_STATUS_MSG_E101:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E101_CAT_PMT_ABSENT));
		break;
	case IRUC_STATUS_MSG_E102:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E102_NO_ECM_EMM_RECEIVED));
		break;
	case IRUC_STATUS_MSG_E103:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E103_ECM_REJECTED));
		break;
	case IRUC_STATUS_MSG_E104:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E104_EMM_REJECTED));
		break;
	case IRUC_STATUS_MSG_E106:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E106_INCORRECT_CA_SYSTEM_IS));
		break;
	case IRUC_STATUS_MSG_E016:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E016_NOT_ENTITLED));
		break;
	case IRUC_STATUS_MSG_E017:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E017_INCORRECT_SECTOR));
		break;
	case IRUC_STATUS_MSG_E030:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E030_INCORRECT_PKEY));
		break;
	case IRUC_STATUS_MSG_E031:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E031_INCORRECT_GKEY));
		break;
	case IRUC_STATUS_MSG_E032:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E032_INCORRECT_TG));
		break;
	case IRUC_STATUS_MSG_E033:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E033_INCORRECT_CWDK));
		break;
	case IRUC_STATUS_MSG_E055:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E055_MACROVISION_NOT_SUPPORTED));
		break;
	case IRUC_STATUS_MSG_E139:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E139_CA_REGIONAL_INFO_ABSENT));
		break;
	case IRUC_STATUS_MSG_E140:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E140_REGIONAL_BLOCKING));
		break;
	case IRUC_STATUS_MSG_E130:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E130_PVR_NOT_ALLOWED));
		break;
	case IRUC_STATUS_MSG_E128:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E128_PVR_SERVICE_NOT_ENTITLED));
		break;
	case IRUC_STATUS_MSG_E092:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E092_PVR_RECORD_FORBIDDEN));
		break;
	case IRUC_STATUS_MSG_E131:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E131_PVR_MSK_ABSENT));
		break;
	case IRUC_STATUS_MSG_E094:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E094_SET_PVR_SK));
		break;
	case IRUC_STATUS_MSG_E095:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E095_INVALID_PVR_SESSION_KEY));
		break;
	case IRUC_STATUS_MSG_E135:
		MWC_UTIL_DvbStrcpy(szErrorText, GetStrRsc(LOC_CAS_IR_UC_MSG_E135_PVR_COPY_CONTROL_ERROR));
		break;
	}
}
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
			eMsg = AXI_STATUSCODE_IDX_None;
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
			eMsg = AXI_STATUSCODE_IDX_None;
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
				pEvtData->ExtData.ulErrIndex = AXI_STATUSCODE_IDX_None;
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

