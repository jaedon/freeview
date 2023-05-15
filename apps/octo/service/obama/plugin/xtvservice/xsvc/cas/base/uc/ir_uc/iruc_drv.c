/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/
/* File Description ~. jichoi.
User powers on the device & powers off the device.
When the user powers on the device:
The application must initialize the Universal Client library using UniversalClient_StartCaClient.
This will cause the client to create any necessary background threads and allocate global resources to prepare the client for use.

The application must open a service to use for receiving EMMs, using UniversalClient_OpenService.
The application must obtain the body of the CAT from the DVB Transport Stream and pass it to the client using UniversalClient_DVB_NotifyCAT. The client will in turn parse the CAT and request to open an EMM stream using the SPI implementation. (see UniversalClientSPI_Stream_Open)
*/

#include <octo_common.h>
#include <db_svc.h>
#include <pal_pipe.h>

//#include <_svc_cas_mgr.h>

//#include "ir_drv.h"

#include <UniversalClient_API.h> // needed for base API calls
#include <UniversalClient_Common_API.h> // needed for service management functions
#include <UniversalClient_DVB_API.h> //needed for DVB-specific functions

#include "iruc_api.h"
#include "iruc_drv.h"
#include "ir_evt.h"

#include "iruc_spi.h"
#include "pal_dsc.h"
#include <ir_nvr.h>

// This callback is used provided by the application to ::UniversalClient_StartCaClient in order to
// receive notifications about particular events. It is optional, but is provided here to
// illustrate how it might be used.

/* TODO : one tuner 에서 하나의 TP안에서만 처리 할 시에 사용 한다.
        : 우선은 임시적인 것이다 나중에 context 문제가 처리 되면 이 define은 지원야 할 것으로 보인다.
*/
#define __FORJUSTONETUNER__


#define	IRUC_STATUS_TEXT_LEN	(UC_STATUS_LENGTH-1)  /*에러메시지 정의가 모두 8자여서, -1을 함. */

static unsigned long	s_ulUCCAAPISema;
IRUC_SVC_INFO 	g_IrUcSvcInfo[IR_HUMAX_MAX_SERVICE]; // for ECM
IRUC_TP_INFO	g_IrUcTpInfo[IR_HUMAX_MAX_TP]; // for EMM

#ifdef	_____ECM_EMM_MONITORING_____
HUINT16	s_usSCELLforMonitor = 0;
HUINT8	s_ucTypeForMonitor = IR_MONITOR_STOP;
	#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
		HUINT32	s_EmmMonitorCount[CAS_MAX_NUM_OF_SC];
	#endif
#endif

HBOOL local_cas_iruc_IS_ECM_SVCCONTEXT(HUINT32 SvcContext);
void local_cas_iruc_Drv_StartService(CAS_CAI_IF_msgPID	*pMsgPID);
void local_cas_iruc_Drv_IamSelected(CAS_CAI_IF_msgCAINST_SELECT *pMsgSelect);
void local_cas_iruc_Drv_IamDeSelected(CAS_CAI_IF_msgCAINST_SELECT *pMsgSelect);
void local_cas_iruc_Drv_StopService(CAS_CAI_IF_msgPID	*pMsgPID);
void local_cas_iruc_Drv_SetPID(CAS_CAI_IF_msgPIDList	*pMsgPIDList);
void local_cas_iruc_Drv_StartTS(CAS_CAI_IF_msgTS	*pMsgTS);
void local_cas_iruc_Drv_StopTS(CAS_CAI_IF_msgTS	*pMsgTS);
void local_cas_iruc_Drv_CAT_Update(CAS_CAI_IF_msgCAT	*pMsgSI);
void local_cas_iruc_Drv_PMT_Update(CAS_CAI_IF_msgPMT	*pMsgSI);

HINT32 CAS_IRUC_DRV_Open_UC_Svc(HUINT32 UcSvcContext, uc_service_handle * in_pUCSvcHandle);
HINT32 CAS_IRUC_DRV_Close_UC_Svc(uc_service_handle *in_pUCSvcHandle);

void CAS_IRUC_DRV_Client_Notify_PMT(uc_service_handle UC_SvcHandle, HUINT8 *pRawPmt, HUINT16 usPmtSize );
void CAS_IRUC_DRV_Client_Notify_CAT(uc_service_handle UC_SvcHandle,HUINT8 *pRawCat, HUINT16 usCatSize );


//extern functions
extern HINT32 CAS_IRUC_SPI_DEVICE_LoadAllUniqueIdentifers(void);

#if defined(CONFIG_DEBUG)
#include "cmdtool.h"
void local_cas_iruc_Dbg_PrintTpInfo(void);
void CMD_Register_MW_CAS_UCDRV_Debug(void);
HUINT32 local_cas_iruc_Drv_Dbg_PrintSvcInfo(void);
static int local_cmd_UcDrvCommand(void *szArgList);
extern void local_cas_iruc_StreamSPI_DBG_Info(void);

HINT8* local_cas_iruc_Dbg_PrintSvcMsgType(uc_service_message_type msgType);
HINT8* local_cas_iruc_Dbg_PrintGlobalMsgType(uc_global_message_type msgType);
HINT8* local_cas_iruc_Dbg_PrintValidClientType(uc_valid_client_type msgType);
#endif

void xsvc_cas_IrUcFreeStreamStatus(void);
void xsvc_cas_IrUcInitStreamStatus(void);
HINT32 xsvc_cas_IrUcSetTMSService(HUINT32 ucSvcindex);

#if defined(CONF_IR_UCCA_STATUS_CODE_FULL_SUPPORT) // 기존 Status Code가 일부만 적용이 되어 있어, Device Integration overview (rev.34.0) 및 UniveralClient_API.h @ cca_2.7 를 참조하여 정리함. 검증 되지 않았으므로, 검즐 필요함!!!
HUINT16 local_cas_iruc_GetStatusCode(char *pstrError)
{
	HUINT16 result = IRUC_STATUS_MSG_NONE;

	if (pstrError == NULL)
		return result;

	if (strncmp(pstrError, ERR_MSG_D029, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D029_0;
	else if (strncmp(pstrError, ERR_MSG_D100, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D100_0;
	else if (strncmp(pstrError, ERR_MSG_D101, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D101_0;
	else if (strncmp(pstrError, ERR_MSG_D126, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D126_0;
	else if (strncmp(pstrError, ERR_MSG_D127, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D127_0;
	else if (strncmp(pstrError, ERR_MSG_D500, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D500_0;
	else if (strncmp(pstrError, ERR_MSG_D000_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D000_4;
	else if (strncmp(pstrError, ERR_MSG_D029_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D029_4;
	else if (strncmp(pstrError, ERR_MSG_D100_5, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D100_5;
	else if (strncmp(pstrError, ERR_MSG_D100_8, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D100_8;
	else if (strncmp(pstrError, ERR_MSG_D101_6, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D101_6;
	else if (strncmp(pstrError, ERR_MSG_D102_9, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D102_9;
	else if (strncmp(pstrError, ERR_MSG_D105_9, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D105_9;
	else if (strncmp(pstrError, ERR_MSG_D106_9, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D106_9;
	else if (strncmp(pstrError, ERR_MSG_I101, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I101_0;
	else if (strncmp(pstrError, ERR_MSG_I102, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I102_0;
	else if (strncmp(pstrError, ERR_MSG_I300, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I300_0;
	else if (strncmp(pstrError, ERR_MSG_I301, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I301_0;
	else if (strncmp(pstrError, ERR_MSG_I302, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I302_0;
	else if (strncmp(pstrError, ERR_MSG_I303, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I303_0;
	else if (strncmp(pstrError, ERR_MSG_I304, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I304_0;
	else if (strncmp(pstrError, ERR_MSG_I305, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I305_0;
	else if (strncmp(pstrError, ERR_MSG_I306, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I306_0;
	else if (strncmp(pstrError, ERR_MSG_I307, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I307_0;
	else if (strncmp(pstrError, ERR_MSG_I308, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I308_0;
	else if (strncmp(pstrError, ERR_MSG_I309, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I309_0;
	else if (strncmp(pstrError, ERR_MSG_I310, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I310_0;
	else if (strncmp(pstrError, ERR_MSG_I311, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I311_0;
	else if (strncmp(pstrError, ERR_MSG_I312, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I312_0;
	else if (strncmp(pstrError, ERR_MSG_I313, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I313_0;
	else if (strncmp(pstrError, ERR_MSG_I314, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I314_0;
	else if (strncmp(pstrError, ERR_MSG_I315, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I315_0;
	else if (strncmp(pstrError, ERR_MSG_I316, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I316_0;
	else if (strncmp(pstrError, ERR_MSG_I317, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I317_0;
	else if (strncmp(pstrError, ERR_MSG_I318, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I318_0;
	else if (strncmp(pstrError, ERR_MSG_I319, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I319_0;
	else if (strncmp(pstrError, ERR_MSG_I320, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I320_0;
	else if (strncmp(pstrError, ERR_MSG_I321, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I321_0;
	else if (strncmp(pstrError, ERR_MSG_I322, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I322_0;
	else if (strncmp(pstrError, ERR_MSG_I323, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I323_0;
	else if (strncmp(pstrError, ERR_MSG_I324, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I324_0;
	else if (strncmp(pstrError, ERR_MSG_I325, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I325_0;
	else if (strncmp(pstrError, ERR_MSG_I326, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I326_0;
	else if (strncmp(pstrError, ERR_MSG_I327, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I327_0;
	else if (strncmp(pstrError, ERR_MSG_I328, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I328_0;
	else if (strncmp(pstrError, ERR_MSG_I329, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I329_0;
	else if (strncmp(pstrError, ERR_MSG_I330, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I330_0;
	else if (strncmp(pstrError, ERR_MSG_I331, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I331_0;
	else if (strncmp(pstrError, ERR_MSG_I332, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I332_0;
	else if (strncmp(pstrError, ERR_MSG_I333, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I333_0;
	else if (strncmp(pstrError, ERR_MSG_I334, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I334_0;
	else if (strncmp(pstrError, ERR_MSG_I335, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I335_0;
	else if (strncmp(pstrError, ERR_MSG_I336, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I336_0;
	else if (strncmp(pstrError, ERR_MSG_I337, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I337_0;
	else if (strncmp(pstrError, ERR_MSG_I338, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I338_0;
	else if (strncmp(pstrError, ERR_MSG_I339, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I339_0;
	else if (strncmp(pstrError, ERR_MSG_I340, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I340_0;
	else if (strncmp(pstrError, ERR_MSG_I341, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I341_0;
	else if (strncmp(pstrError, ERR_MSG_I342, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I342_0;
	else if (strncmp(pstrError, ERR_MSG_I343, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I343_0;
	else if (strncmp(pstrError, ERR_MSG_I344, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I344_0;
	else if (strncmp(pstrError, ERR_MSG_I007_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I007_4;
	else if (strncmp(pstrError, ERR_MSG_I034_9, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I034_9;
	else if (strncmp(pstrError, ERR_MSG_I055_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I055_4;
	else if (strncmp(pstrError, ERR_MSG_I102_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I102_4;
	else if (strncmp(pstrError, ERR_MSG_I139_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_I139_4;
	else if (strncmp(pstrError, ERR_MSG_E104, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E104_0;
	else if (strncmp(pstrError, ERR_MSG_E129, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E129_0;
	else if (strncmp(pstrError, ERR_MSG_E131, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E131_0;
	else if (strncmp(pstrError, ERR_MSG_E150, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E150_0;
	else if (strncmp(pstrError, ERR_MSG_E151, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E151_0;
	else if (strncmp(pstrError, ERR_MSG_E501, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E501_0;
	else if (strncmp(pstrError, ERR_MSG_E502, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E502_0;
	else if (strncmp(pstrError, ERR_MSG_E503, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E503_0;
	else if (strncmp(pstrError, ERR_MSG_E504, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E504_0;
	else if (strncmp(pstrError, ERR_MSG_E505, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E505_0;
	else if (strncmp(pstrError, ERR_MSG_E506, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E506_0;
	else if (strncmp(pstrError, ERR_MSG_E507, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E507_0;
	else if (strncmp(pstrError, ERR_MSG_E508, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E508_0;
	else if (strncmp(pstrError, ERR_MSG_E509, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E509_0;
	else if (strncmp(pstrError, ERR_MSG_E510, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E510_0;
	else if (strncmp(pstrError, ERR_MSG_E511, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E511_0;
	else if (strncmp(pstrError, ERR_MSG_E512, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E512_0;
	else if (strncmp(pstrError, ERR_MSG_E513, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E513_0;
	else if (strncmp(pstrError, ERR_MSG_E514, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E514_0;
	else if (strncmp(pstrError, ERR_MSG_E515, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E515_0;
	else if (strncmp(pstrError, ERR_MSG_E516, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E516_0;
	else if (strncmp(pstrError, ERR_MSG_E517, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E517_0;
	else if (strncmp(pstrError, ERR_MSG_E518, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E518_0;
	else if (strncmp(pstrError, ERR_MSG_E519, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E519_0;
	else if (strncmp(pstrError, ERR_MSG_E520, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E520_0;
	else if (strncmp(pstrError, ERR_MSG_E521, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E521_0;
	else if (strncmp(pstrError, ERR_MSG_E600, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E600_0;
// 4.3 Error Banner
	else if (strncmp(pstrError, ERR_MSG_E016, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E016_0;
	else if (strncmp(pstrError, ERR_MSG_E017, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E017_0;
	else if (strncmp(pstrError, ERR_MSG_E018, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E018_0;
	else if (strncmp(pstrError, ERR_MSG_E030, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E030_0;
	else if (strncmp(pstrError, ERR_MSG_E031, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E031_0;
	else if (strncmp(pstrError, ERR_MSG_E032, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E032_0;
	else if (strncmp(pstrError, ERR_MSG_E033, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E033_0;
	else if (strncmp(pstrError, ERR_MSG_E055, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E055_0;
	else if (strncmp(pstrError, ERR_MSG_E094, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E094_0;
	else if (strncmp(pstrError, ERR_MSG_E101, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E101_0;
	else if (strncmp(pstrError, ERR_MSG_E102, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E102_0;
	else if (strncmp(pstrError, ERR_MSG_E103, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E103_0;
	else if (strncmp(pstrError, ERR_MSG_E106, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E106_0;
	else if (strncmp(pstrError, ERR_MSG_E111, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E111_0;
	else if (strncmp(pstrError, ERR_MSG_E128, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E128_0;
	else if (strncmp(pstrError, ERR_MSG_E131, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E131_0;
	else if (strncmp(pstrError, ERR_MSG_E129, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E129_0;
	else if (strncmp(pstrError, ERR_MSG_E030, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E130_0;
	else if (strncmp(pstrError, ERR_MSG_E139, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E139_0;
	else if (strncmp(pstrError, ERR_MSG_E140, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E140_0;
	else if (strncmp(pstrError, ERR_MSG_E600, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E600_0;
// 5.2.2. Error Banner
	else if (strncmp(pstrError, ERR_MSG_E000_32, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E000_32;
	else if (strncmp(pstrError, ERR_MSG_E004_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E004_4;
	else if (strncmp(pstrError, ERR_MSG_E005_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E005_4;
	else if (strncmp(pstrError, ERR_MSG_E006_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E006_4;
	else if (strncmp(pstrError, ERR_MSG_E016_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E016_4;
	else if (strncmp(pstrError, ERR_MSG_E017_13, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E017_13;
	else if (strncmp(pstrError, ERR_MSG_E018_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E018_4;
	else if (strncmp(pstrError, ERR_MSG_E030_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E030_4;
	else if (strncmp(pstrError, ERR_MSG_E032_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E032_4;
	else if (strncmp(pstrError, ERR_MSG_E033_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E033_4;
	else if (strncmp(pstrError, ERR_MSG_E038_32, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E038_32;
	else if (strncmp(pstrError, ERR_MSG_E045_32, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E045_32;
	else if (strncmp(pstrError, ERR_MSG_E080_35, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E080_35;
	else if (strncmp(pstrError, ERR_MSG_E081_35, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E081_35;
	else if (strncmp(pstrError, ERR_MSG_E100_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E100_4;
	else if (strncmp(pstrError, ERR_MSG_E101_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E101_4;
	else if (strncmp(pstrError, ERR_MSG_E106_9, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E106_9;
	else if (strncmp(pstrError, ERR_MSG_E107_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E107_4;
	else if (strncmp(pstrError, ERR_MSG_E108_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E108_4;
	else if (strncmp(pstrError, ERR_MSG_E109_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E109_4;
	else if (strncmp(pstrError, ERR_MSG_E118_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E118_4;
	else if (strncmp(pstrError, ERR_MSG_E136_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E136_4;
	else if (strncmp(pstrError, ERR_MSG_E137_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E137_4;
	else if (strncmp(pstrError, ERR_MSG_E140_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E140_4;
	else if (strncmp(pstrError, ERR_MSG_E141_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E141_4;
	else if (strncmp(pstrError, ERR_MSG_E142_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E142_4;
	else if (strncmp(pstrError, ERR_MSG_E144_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E144_4;
	else if (strncmp(pstrError, ERR_MSG_E152_4, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E152_4;
	return result;
}

#else
// ewlee 20100817
HUINT16 local_cas_iruc_GetStatusCode(char *pstrError)
{
	HUINT16 result = IRUC_STATUS_MSG_NONE;

	if (pstrError == NULL)
		return result;

	if (strncmp(pstrError, ERR_MSG_D029, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D029;
	else if (strncmp(pstrError, ERR_MSG_D100, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D100;
	else if (strncmp(pstrError, ERR_MSG_D101, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D101;
	else if (strncmp(pstrError, ERR_MSG_D126, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D126;
	else if (strncmp(pstrError, ERR_MSG_D127, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_D127;
	else if (strncmp(pstrError, ERR_MSG_E101, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E101;
	else if (strncmp(pstrError, ERR_MSG_E102, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E102;
	else if (strncmp(pstrError, ERR_MSG_E103, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E103;
	else if (strncmp(pstrError, ERR_MSG_E104, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E104;
	else if (strncmp(pstrError, ERR_MSG_E106, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E106;
	else if (strncmp(pstrError, ERR_MSG_E016, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E016;
	else if (strncmp(pstrError, ERR_MSG_E017, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E017;
	else if (strncmp(pstrError, ERR_MSG_E030, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E030;
	else if (strncmp(pstrError, ERR_MSG_E031, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E031;
	else if (strncmp(pstrError, ERR_MSG_E032, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E032;
	else if (strncmp(pstrError, ERR_MSG_E033, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E033;
	else if (strncmp(pstrError, ERR_MSG_E055, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E055;
	else if (strncmp(pstrError, ERR_MSG_E139, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E139;
	else if (strncmp(pstrError, ERR_MSG_E140, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E140;
	else if (strncmp(pstrError, ERR_MSG_E130, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E130;
	else if (strncmp(pstrError, ERR_MSG_E128, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E128;
	else if (strncmp(pstrError, ERR_MSG_E131, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E131;
	else if (strncmp(pstrError, ERR_MSG_E094, IRUC_STATUS_TEXT_LEN) == 0)
		result = IRUC_STATUS_MSG_E094;

	return result;
}
#endif


/**********************************************************************************
*         Not used Function for 2010.05.07
***********************************************************************************/

void local_cas_iruc_CHECK_CURRENT_ENV(void)
{
	HUINT8 	securetype;
	HUINT32 	caid;
	HUINT16 	lockid;

	UniversalClient_GetSecureType( &securetype);
	HxLOG_Print("[%s],  Secure Type = %d, 0x%X\n",__FUNCTION__, securetype, securetype);

	UniversalClient_GetLockId(&lockid);
	HxLOG_Print("[%s],  Lock ID = %d, 0x%X\n",__FUNCTION__,  lockid, lockid);

	UniversalClient_GetCASystemID(&caid);
	HxLOG_Print("[%s], CA ID = %d, 0x%X\n",__FUNCTION__,  caid, caid);
}

HUINT8	CAS_IRUC_GetIrSvcInfoIDXbyActHandle(Handle_t hAction)

{
	HINT32	i;

#if defined(CONFIG_DEBUG)
//	HxLOG_Print("hAction (%x)\n", hAction);
#endif
	for (i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++)
	{
		if ((g_IrUcSvcInfo[i].hAct == hAction) || (svc_cas_SubSvcRmGetActionId(g_IrUcSvcInfo[i].hAct) == svc_cas_SubSvcRmGetActionId(hAction)))
			return i;
	}

	return IRUC_HUMAX_MAX_SERVICE;
}

void local_cas_iruc_UC_GlobalMsg_ProcCallback(uc_global_message_type message, void* lpVoid)
{
	int 		i;
#if defined(CONFIG_DEBUG)
	HxLOG_Print("\n\t ### Global CB (%s)\n", local_cas_iruc_Dbg_PrintGlobalMsgType(message));
#endif

    // different messages require different handling. See the documentation for each message type for
    // details on how each one is used.
    switch(message)
    {
	case UC_GLOBAL_DEBUG:
       {
		// simple debug/informational messages. In this case, lpVoid points to the NULL-terminated text of the message.
		uc_char *pText = (uc_char *)lpVoid;
		HxLOG_Print("[%s] Global debug msg: %s\n",__FUNCTION__ , pText);
		break;
	}

	case UC_GLOBAL_SMARTCARD_RAW_IRD_MESSAGE :	// SoftCell
	case UC_GLOBAL_RAW_IRD_MESSAGE:				// CCA
		// ewlee 20100209 - use irdeto softcell procedure because there is no difference between softcell and universal client
		if (lpVoid != NULL)
		{
			uc_buffer_st	*pBytes = (uc_buffer_st *) lpVoid;
#if 0 //test
			for (i=0; i<pBytes->length;i++)
			{
				uc_char	ch = *(pBytes->bytes + i);
				HxLOG_Print("Message data(len: %d) : %d - %c (0x%02X)\n", pBytes->length, i, ch, ch);
			}
#endif
				for( i = 0; i < IRUC_VIEW_ACTION; i++ )
			{
				CAS_IR_MSG_DRV_ProcessDecoderDataEMM(g_IrUcTpInfo[i].hAction , (HUINT8 *) pBytes->bytes);
			//	CAS_IR_MSG_DRV_ProcessDecoderDataEMM(NULL, (HUINT8 *) pBytes->bytes);
			}
				break;
		}
	case UC_GLOBAL_SERIAL_NUMBER_CHANGED:
			if (lpVoid != NULL)
			{
				uc_buffer_st	*pBytes = (uc_buffer_st *) lpVoid;
				HUINT8		i = 0;

				for(i = 0; i < pBytes->length ; i++)
				{
					HxLOG_Print("%02x ", pBytes->bytes[i]);
				}
				HxLOG_Print("\n");

				//HxLOG_Print("new serial num (0x%x), length (%d)\n", (HUINT32 *)pBytes->bytes, pBytes->length);
				break;
			}
	case UC_GLOBAL_OOB_EMM_STATUS_REPLY:
	case UC_GLOBAL_NOTIFY_ENTITLEMENT_CHANGE  :
	case UC_GLOBAL_NOTIFY_CASECTION_COUNT :
	case UC_GLOBAL_NOTIFY_REGION_SUBREGION_INFO :
	case UC_GLOBAL_NOTIFY_NATIONALITY_CHANGED :
	case UC_GLOBAL_NOTIFY_HOMING_CHANNEL_DATA_CHANGED :
	case UC_GLOBAL_NOTIFY_MIDDLEWARE_USER_DATA_CHANGED :
			HxLOG_Print("Not implemented yet : 0x%x\n", message);
			break;

		case UC_GLOBAL_NOTIFY_CA_DATA_CLEARED :
			HxLOG_Print("UC_GLOBAL_NOTIFY_CA_DATA_CLEARED : 0x%x\n", message);
/* Comment : uc lib 에서 CA 를 지운 후에 날라오는 msg  이다 혹시 몰라서 여기서 한번 더 지운다..
             필요 없어 지운다. */
#if 0
			if (HIR_OK == CAS_IR_NVR_Partition_Write(IRNVR_PART_CA, 0, aTemp, IR_NVRAM_PARTITION_CA_SIZE ) )
			{
				HxLOG_Print("Erase CA partition :ok \n");
			}
			else
			{
				HxLOG_Print("Erase CA partition :failed \n");
			}
#endif
			break;

	case UC_GLOBAL_NOTIFY_RESET_TO_FACTORY :
			HxLOG_Print("recieved UC_GLOBAL_NOTIFY_RESET_TO_FACTORY : 0x%x\n", message);

/* Comment : don't need => user가 원할 시에 해주는 함수이다.
			if(UC_ERROR_SUCCESS==UniversalClient_ResetCAData())
			{
				HxLOG_Print(" UniversalClient_ResetCAData :ok \n");

			}
			else
			{
				HxLOG_Print(" UniversalClient_ResetCAData :failed \n");
			}
*/
		break;

	case UC_GLOBAL_NOTIFY_TMS_DATA_CHANGED :
		HxLOG_Print("[%s] UC_GLOBAL_NOTIFY_TMS_DATA_CHANGED : 0x%x \n", __FUNCTION__, message);
#if 0
// get TMS data
			uc_result				result = UC_ERROR_SUCCESS;
			uc_buffer_st			stBytes;
			uc_byte					ucTmsData[UC_BYTES_GENERIC_SIZE];

			VK_memset(ucTmsData,0,UC_BYTES_GENERIC_SIZE);
			stBytes.bytes =  &ucTmsData[0];
			stBytes.length = 32;
			result = UniversalClient_GetTmsData(&stBytes);

			HxLOG_Print("[UniversalClient_GetTmsData] TMS data length:[0x%X] result=0x%X \n",  stBytes.length,result));

			if (result != UC_ERROR_SUCCESS)
			{
				HxLOG_Print("[UniversalClient_GetTmsData] TMS Data:failed. 0x%X\n", result));
			}

			for( i = 0; i < 32 ; i++ )
			{
				if((i%16)==0)
				{
					VK_Print("\n");
				}

				VK_Print("%02x ",ucTmsData[i]);
			}
#endif



		break;

	case UC_GLOBAL_NOTIFY_SMARTCARD_STATUS :
		if (lpVoid != NULL)
		{
			uc_smartcard_status *pScStatus = (uc_smartcard_status *) lpVoid;
			HxLOG_Print("[%s] S/C ID (%d), status (%s)\n",__FUNCTION__ , pScStatus->smartcardId, pScStatus->status);
			break;
		}
	case UC_GLOBAL_NOTIFY_SMARTCARD_NATIONALITY_CHANGED :
		HxLOG_Print("[%s] Not implemented yet : 0x%x\n",__FUNCTION__ , message);
		break;

		case UC_GLOBAL_NOTIFY_VALID_CLIENT_CHANGED :
			if (lpVoid != NULL)
			{
				uc_valid_client_type *pValidClient = (uc_valid_client_type *) lpVoid;
#if defined(CONFIG_DEBUG)
				HxLOG_Print("[%s] Valid Client Type (%s)\n",__FUNCTION__ , local_cas_iruc_Dbg_PrintValidClientType(*pValidClient));
#endif
				break;
			}

		case UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE :
			HxLOG_Print(" receive UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE : 0x%x\n", message);
			VK_SEM_Get(s_ulUCCAAPISema);
			if (lpVoid != NULL)
			{
				uc_flexiflash_msg 	*pBytes = (uc_flexiflash_msg *)lpVoid;
				HxLOG_Print(" SecureCoreStatus:[%s], ProgressInfo:[%s] \n", pBytes->secureCoreListStatus, pBytes->packagesDownloadProgressInfo);
				for( i = 0; i < IRUC_VIEW_ACTION; i++ )
				{
					memset(g_IrUcTpInfo[i].secureCoreListStatus, 0x00, IRUC_SECURE_CORE_LENGTH);
					memset(g_IrUcTpInfo[i].packagesDownloadProgressInfo, 0x00, IRUC_SECURE_CORE_LENGTH);
					snprintf(g_IrUcTpInfo[i].secureCoreListStatus, IRUC_SECURE_CORE_LENGTH, "%s", pBytes->secureCoreListStatus) ;
					snprintf(g_IrUcTpInfo[i].packagesDownloadProgressInfo, IRUC_SECURE_CORE_LENGTH, "%s", pBytes->packagesDownloadProgressInfo);
					HxLOG_Print(" secure status [%s] : [%s]\n", pBytes->secureCoreListStatus,g_IrUcTpInfo[0].secureCoreListStatus);

// flexiflash
/* Comment :
   100% 되면 core file을 rootfs로 옮기기 위해서 msg 던진다..
   uc lib 에서 flash write 하는 시간이 잇어서 copy 할 시에는 5초 후에 copy 하게 한다.
					 VK_Print(" progress = %c%c%c percent \n",g_IrUcTpInfo[i].packagesDownloadProgressInfo[6],\
					                         				  g_IrUcTpInfo[i].packagesDownloadProgressInfo[7],\
					                         				  g_IrUcTpInfo[i].packagesDownloadProgressInfo[8]); */

					if(strncmp(&g_IrUcTpInfo[i].packagesDownloadProgressInfo[6], "100", 3) == 0)
					{

/* hAction만 임시로 저장 해서 PS index1 copy 될 시에 사용 한다. */
						CAS_IRUC_SetActionHandleFlexiFlash(g_IrUcTpInfo[i].hAction);
					}

				}
    }
				VK_SEM_Release(s_ulUCCAAPISema);
			break;
		default:
			HxLOG_Print("[%s] ERROR!! Not defined Message! : %d\n",__FUNCTION__ , message);
			break;
	}

#if defined(CONFIG_DEBUG)
	HxLOG_Print("\n");
#endif
	return;
}


// This callback is for the EMM and ECM services. You can use the same callback for
// all services and simply distinguish which service is used based on the pMessageProcData
void local_cas_iruc_UC_SVC_ProcCallback( void *pMessageProcData, uc_service_message_type message,  void *pVoid)
{
	HINT32 i;
	HINT16 	ucErrCode = 0;
	HMSG	hmsg;

#if defined(CONFIG_DEBUG)
//	HxLOG_Print("\n\t[%s] @@@ UC_SVC_ProcCallback (%s) \n",__FUNCTION__, local_cas_iruc_Dbg_PrintSvcMsgType(message) );
#endif

	VK_SEM_Get(s_ulUCCAAPISema);

	switch(message)
	{
		case UC_ECM_STATUS:	/* 0x00000000 */
		{
			// an ECM arrived. The status of the ECM is contained within a structure.
			uc_ecm_status_st *pEcmStatus = (uc_ecm_status_st *)pVoid;

			// technically, different transport protocols can have different information.
			// For now, only DVB is supported.
			if (pEcmStatus->caStream.protocolType != UC_STREAM_DVB)
			{
				HxLOG_Info("[%s]It's not DVB Service\n",__FUNCTION__);
				break;
			}

			// check to make sure this is the right service. == SVC_CONTEXT

			//find service context
			for(i=0; i<IRUC_HUMAX_MAX_SERVICE; i++)
			{
				if((HUINT32) pMessageProcData == g_IrUcSvcInfo[i].UcSvcContext)
				{
					HxLOG_Info("\t Received ECM status: Context = 0x%X, PID= 0x%X,  Status (%s)\n",
							(HINT32) pMessageProcData,  pEcmStatus->caStream.pid, pEcmStatus->statusMessage);

					snprintf(g_IrUcSvcInfo[i].szEcmStatusMsg, 256, "%s", pEcmStatus->statusMessage) ;

					/* hjlee3, ECM PID   */
					g_IrUcSvcInfo[i].PidList[i].usEmmorEcmPID=pEcmStatus->caStream.pid;
					break;
				}
			}
			break;
		}

		case UC_EMM_STATUS:	/* 0x00000001 */
		{
			// an EMM arrived. The status of the EMM is contained within another structure.
			uc_emm_status_st *pEmmStatus = (uc_emm_status_st *)pVoid;

			// technically, different transport protocols can have different information.
			// For now, only DVB is supported.
			if ((HUINT32) pEmmStatus->caStream.protocolType != UC_STREAM_DVB)
			{
				HxLOG_Info("It's not DVB Service\n");
				break;
			}

			// check to make sure this is the right service. == SVC_CONTEXT

			//find service context
			for(i=0; i<IRUC_HUMAX_MAX_TP; i++)
			{
				if( (HUINT32) pMessageProcData == g_IrUcTpInfo[i].UcSvcContext)
				{
					HxLOG_Info("[%s] Received EMM status UcSvc context = 0x%X, PID= 0x%X,  Message(%s).\n", __FUNCTION__,
							(HINT32)pMessageProcData,  pEmmStatus->caStream.pid, pEmmStatus->statusMessage);

					snprintf(g_IrUcTpInfo[i].szEmmStatusMsg, 256, "%s", pEmmStatus->statusMessage) ;

					/* hjlee3, EMM PID */
					g_IrUcTpInfo[i].emmPid=pEmmStatus->caStream.pid;
					break;
				}
			}
			break;
		}
/*status 체크하는 msg UC_SERVICE_STATUS_FOR_CONVERGENT_CLIENT 와 겹쳐져서 지운다.*/
		case UC_SERVICE_STATUS:		/* 0x00000002 */
			HxLOG_Info("[%s] NOT Implemented Status!!!\n",__FUNCTION__);
						break;

		case UC_SERVICE_PVR_RECORD_STATUS_REPLY: 						/* 0x00000003 */
			HxLOG_Info("[%s] NOT Implemented Status!!!\n",__FUNCTION__);
			break;

		case UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY:						/* 0x00000004 */
			HxLOG_Info("[%s] NOT Implemented Status!!!\n",__FUNCTION__);
			break;

		case UC_SERVICE_PVR_SESSION_METADATA_REPLY:					/* 0x00000005 */
			HxLOG_Info("[%s] NOT Implemented Status!!!\n",__FUNCTION__);
			break;

		case UC_SERVICE_ECM_MONITOR_STATUS:							/* 0x00000006 */
			HxLOG_Info("[%s] NOT Implemented UC_SERVICE_ECM_MONITOR_STATUS Status!!!\n",__FUNCTION__);
//			uc_service_monitor_status_st *pECMMonitoring = (uc_service_monitor_status_st *) pVoid;
			break;

		case UC_SERVICE_EMM_MONITOR_STATUS:							/* 0x00000007 */
			HxLOG_Info("[%s] NOT UC_SERVICE_EMM_MONITOR_STATUS Implemented Status!!!\n",__FUNCTION__);
//			uc_service_monitor_status_st *pEMMMonitoring = (uc_service_monitor_status_st *) pVoid;
			break;

		case UC_SERVICE_ECM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT:	/* 0x00000008 */
			HxLOG_Info(" receive UC_SERVICE_ECM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT from UC\n");
			{
				uc_convergent_client_service_monitor_status_st *pClientECMMonitoring = (uc_convergent_client_service_monitor_status_st *) pVoid;
				if(pClientECMMonitoring->serviceType == UC_SERVICE_DESCRAMBLE)
				{
					for(i=0; i<IRUC_HUMAX_MAX_SERVICE; i++)
					{
						HxLOG_Info("[%s] Received ECM Moinitor staus: UC Handle= (0x%04X), SvcHandle(0x%04X) Status (%d)\n", __FUNCTION__ ,
								(HINT32)pClientECMMonitoring->serviceHandle, g_IrUcSvcInfo[i].UC_SvcHandle , pClientECMMonitoring->serviceType);

						if(pClientECMMonitoring->serviceHandle == g_IrUcSvcInfo[i].UC_SvcHandle)
						{
	/*	TODO: hjlee3,
			  g_IrUcSvcInfo[i].UCmonitorInfo 저장 하는 부분과 사용 하는 부분이 간섭을 일으킬수도 잇어 보이는데
		      이부분 추후에 다시 확인 해보자. */
							HxSTD_memset(&g_IrUcSvcInfo[i].UCmonitorInfo,0,sizeof(uc_convergent_client_service_monitor_status_st));
	#if 0
							if(pClientECMMonitoring->cloakedCAMonitorInfo.valid==TRUE)
								HxLOG_Print ("\t UCCA:StatusProc ECM-Monitoring(%s)\n",  pClientECMMonitoring->cloakedCAMonitorInfo.message);
							else
								HxLOG_Print("\t SoftCell:StatusProc ECM-Monitoring(%s)\n",  pClientECMMonitoring->softcellMonitorInfo.message);
	#endif
							HxSTD_memcpy(&g_IrUcSvcInfo[i].UCmonitorInfo, pClientECMMonitoring,sizeof(uc_convergent_client_service_monitor_status_st));
							break;
						}
					}
				}
			}
			break;

		case UC_SERVICE_EMM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT:	/* 0x00000009 */
			HxLOG_Info(" receive UC_SERVICE_EMM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT from UC \n");
			{
				uc_convergent_client_service_monitor_status_st *pClientEMMMonitoring = (uc_convergent_client_service_monitor_status_st *) pVoid;

				for(i=0; i<IR_HUMAX_MAX_TP; i++)
				{
				if(pClientEMMMonitoring->serviceType == UC_SERVICE_EMM)
				{
						HxLOG_Info("[%s] Received EMM Moinitor staus: UC Handle(0x%04x), UC_SvcHandle(0x%04x),serviceType:%d\n", __FUNCTION__,
									(HINT32)pClientEMMMonitoring->serviceHandle, g_IrUcTpInfo[i].UC_SvcHandle,pClientEMMMonitoring->serviceType);

						if(pClientEMMMonitoring->serviceHandle == g_IrUcTpInfo[i].UC_SvcHandle)
						{
							HxSTD_memset(&g_IrUcTpInfo[i].UCmonitorInfo,0, sizeof(uc_convergent_client_service_monitor_status_st));
	#if 0
							if(pClientEMMMonitoring->cloakedCAMonitorInfo.valid==TRUE)
	//							HxLOG_Print("\t UCCA:StatusProc EMM-Monitoring(%s)\n",  pClientECMMonitoring->cloakedCAMonitorInfo.message);
								HxLOG_Print("\t UCCA:StatusProc EMM-Monitoring \n");
							else
	//							HxLOG_Print("\t SoftCell:StatusProc EMM-Monitoring(%s)\n",  pClientECMMonitoring->softcellMonitorInfo.message);
								HxLOG_Print("\t SoftCell:StatusProc EMM-Monitoring \n");
	#endif
							HxSTD_memcpy(&g_IrUcTpInfo[i].UCmonitorInfo, pClientEMMMonitoring, sizeof(uc_convergent_client_service_monitor_status_st));
							break;
						}
					}
				}
			}
			break;

		case UC_SERVICE_STATUS_FOR_CONVERGENT_CLIENT:					/* 0x0000000a */
		{
			uc_convergent_client_service_status_st *pCCSvcStatus = (uc_convergent_client_service_status_st *)pVoid;
				for(i=0; i<IRUC_HUMAX_MAX_SERVICE; i++)
				{
					if(g_IrUcSvcInfo[i].UC_SvcHandle == pCCSvcStatus->serviceHandle )
					{

			HxLOG_Info("[%s]  ######## ########### \n",__FUNCTION__);
			HxLOG_Info("[%s]  ######## UC_SERVICE_STATUS_FOR_CONVERGENT_CLIENT ########### \n",__FUNCTION__);
			HxLOG_Info("[%s]  ######## ########### \n",__FUNCTION__);
						HxLOG_Info("[%s] Status Valid: CCA(%d), SoftCell(%d) \n",__FUNCTION__,pCCSvcStatus->cloakedCAStatus.isValid,pCCSvcStatus->softcellStatus.isValid);
			if( pCCSvcStatus->cloakedCAStatus.isValid == TRUE )
			{
#if 0//defined(__SOFTCELL_REC_EXCEPTION__)
	/* muti-descramble 시에 맞게 들어와서 다시 막는다. */
//TODO :: hActionType 에서 SoftCell 에서 Rec 관련 관련 정보라면 해당 recording을 stop 하기 위한 msg 를 전달 한다.
					HxLOG_Print("[%s] g_IrUcSvcInfo[%d].hActionType = %d \n",__FUNCTION__,i , g_IrUcSvcInfo[i].hActionType);
					if (g_IrUcSvcInfo[i].hActionType != eCAS_ActionType_Live)
					{
						HxSTD_memset(&hmsg, 0, sizeof(HMSG));
						hmsg.usMsgClass=IREVT_CLASS_SVC_RECORDING_FORCED_STOP ;
						hmsg.ulParam = "Record should stop because of a issue" ;
						hmsg.usParam = IRUC_STATUS_MSG_E999;

								HxLOG_Print("[%s] Recording :: Received CCA SVC, idx[%d] SvcHandle= 0x%X, Errcode(0x%X) \n",__FUNCTION__,i,
								(HINT32)pCCSvcStatus->serviceHandle, hmsg.usParam);

						CAS_IR_EVT_SendEvent(g_IrUcSvcInfo[i].hAct , &hmsg);
					}
					else
#endif
					{
						/*hjlee3, CCA와 SoftCell 어디서 동작하는지 구분하는 flag */
						g_IrUcSvcInfo[i].bCCA = TRUE;
								HxLOG_Info ("[%s] Received CCA SVC staus: Handle= 0x%X, Status (%s)\n", __FUNCTION__ ,
								(HINT32)pCCSvcStatus->serviceHandle,  pCCSvcStatus->cloakedCAStatus.statusMessage);

						snprintf(g_IrUcSvcInfo[i].szSvcStatusMsg, 256, "%s", pCCSvcStatus->cloakedCAStatus.statusMessage) ;

						HxSTD_memset(&hmsg, 0, sizeof(HMSG));
						ucErrCode = local_cas_iruc_GetStatusCode(pCCSvcStatus->cloakedCAStatus.statusMessage);
						if((IRUC_STATUS_MSG_D029_0 <= ucErrCode) &&  (ucErrCode <= IRUC_STATUS_MSG_D106_9))
						{
							hmsg.usMsgClass = IREVT_CLASS_CAS_DESCRAMBLED_WELL;
						}
						else
						{
							hmsg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_SERVICE;
						}

						hmsg.ulParam = (HUINT32)pCCSvcStatus->cloakedCAStatus.statusMessage;
						hmsg.usParam = ucErrCode;
						CAS_IR_EVT_SendEvent(g_IrUcSvcInfo[i].hAct , &hmsg);
#if	defined(CONFIG_IRDETO_HMX_PVR)
						//	Descramble 확인시 DRM OK를 내려준다.
						svc_cas_MgrSubUpdateCasEtcEvent(g_IrUcSvcInfo[i].hAct, (Handle_t)NULL, eDxCAS_GROUPID_IR, eSEVT_CAS_DRM_NONE, 0, 0, 0);
#endif
				}

				for(i=0; i<IRUC_HUMAX_MAX_TP; i++)
				{
					if(pCCSvcStatus->serviceHandle == g_IrUcTpInfo[i].UC_SvcHandle)
					{
									HxLOG_Info("[%s] Received CCA TP status: Handle= 0x%X, Status (%s)\n", __FUNCTION__,
								(HINT32)pCCSvcStatus->serviceHandle,  pCCSvcStatus->cloakedCAStatus.statusMessage);
						snprintf(g_IrUcTpInfo[i].szSvcStatusMsg, 256, "%s", pCCSvcStatus->cloakedCAStatus.statusMessage) ;
						break;
					}
				}

				if( i == IRUC_HUMAX_MAX_TP)
				{
								HxLOG_Info("[%s] Cannot found CCA Handle (0x%X)\n", __FUNCTION__,  pCCSvcStatus->serviceHandle);
				}
			}
			else if( pCCSvcStatus->softcellStatus.isValid == TRUE )
			{
							HxLOG_Info(" SoftCell (svc_handle : 0x%04X,0x%04X) \n",pCCSvcStatus->serviceHandle,g_IrUcSvcInfo[i].UC_SvcHandle);

#if 0//defined(__SOFTCELL_REC_EXCEPTION__)
//TODO :: hActionType 에서 SoftCell 에서 Rec 관련 관련 정보라면 해당 recording을 stop 하기 위한 msg 를 전달 한다.
						HxLOG_Print("[%s] g_IrUcSvcInfo[%d].hActionType = %d \n",__FUNCTION__,i , g_IrUcSvcInfo[i].hActionType);
						if (g_IrUcSvcInfo[i].hActionType != eCAS_ActionType_Live)
						{
							HxSTD_memset(&hmsg, 0, sizeof(HMSG));
							hmsg.usMsgClass=IREVT_CLASS_SVC_RECORDING_FORCED_STOP ;
							hmsg.ulParam = "Record should stop because of a issue" ;
							hmsg.usParam = IRUC_STATUS_MSG_E999;
							HxLOG_Print("[%s] Recording :: Received SoftCell SVC, idx[%d] SvcHandle= 0x%X, Errcode(0x%X) \n",__FUNCTION__,i,
									(HINT32)pCCSvcStatus->serviceHandle,  hmsg.usParam );

							CAS_IR_EVT_SendEvent(g_IrUcSvcInfo[i].hAct , &hmsg);
						}
						else
#endif
						{
									HxLOG_Info("[%s] Live Received SoftCell SVC idx[%d] : Handle= 0x%X, Status (%s)\n",__FUNCTION__,i,
										(HINT32)pCCSvcStatus->serviceHandle,  pCCSvcStatus->softcellStatus.statusMessage);
						/*hjlee3, CCA와 SoftCell 어디서 동작하는지 구분하는 flag */
						g_IrUcSvcInfo[i].bCCA = FALSE;
						snprintf(g_IrUcSvcInfo[i].szSvcStatusMsg, 256, "%s", pCCSvcStatus->softcellStatus.statusMessage) ;

						HxSTD_memset(&hmsg, 0, sizeof(HMSG));
						ucErrCode = local_cas_iruc_GetStatusCode(pCCSvcStatus->softcellStatus.statusMessage);
						if((IRUC_STATUS_MSG_D029_0 <= ucErrCode) &&  (ucErrCode <= IRUC_STATUS_MSG_D106_9))
						{
							hmsg.usMsgClass = IREVT_CLASS_CAS_DESCRAMBLED_WELL;
						}
						else
						{
							hmsg.usMsgClass = IREVT_CLASS_CAS_ERROR_FOR_SERVICE;
						}
						hmsg.ulParam = (HUINT32)pCCSvcStatus->softcellStatus.statusMessage;
						hmsg.usParam = ucErrCode;
						CAS_IR_EVT_SendEvent(g_IrUcSvcInfo[i].hAct , &hmsg);
#if	defined(CONFIG_IRDETO_HMX_PVR)
						//	Descramble 확인시 DRM OK를 내려준다.
						svc_cas_MgrSubUpdateCasEtcEvent(g_IrUcSvcInfo[i].hAct, (Handle_t)NULL, eDxCAS_GROUPID_IR, eSEVT_CAS_DRM_NONE, 0, 0, 0);
#endif
				}

				for(i=0; i<IRUC_HUMAX_MAX_TP; i++)
				{
					if(pCCSvcStatus->serviceHandle == g_IrUcTpInfo[i].UC_SvcHandle)
					{
									HxLOG_Info("[%s] Received SoftCell TP status: Handle= 0x%X, Status (%s)\n",__FUNCTION__,
								(HINT32)pCCSvcStatus->serviceHandle,  pCCSvcStatus->cloakedCAStatus.statusMessage);

						snprintf(g_IrUcTpInfo[i].szSvcStatusMsg, 256, "%s", pCCSvcStatus->cloakedCAStatus.statusMessage) ;
						break;
					}
				}

				if( i == IRUC_HUMAX_MAX_TP)
				{
								HxLOG_Info("[%s]Cannot found SoftCell Handle (0x%X)\n",__FUNCTION__, pCCSvcStatus->serviceHandle);
				}
			}
			else
			{
								HxLOG_Info("[%s] WRONG Service Status (0x%X)\n",__FUNCTION__, pCCSvcStatus->serviceHandle);
						}

						break;
					}

			}
		}
			break;
		case UC_SERVICE_INFO_FOR_SOFTCELL_PREVIEW:						/* 0x0000000b */
			HxLOG_Print("[%s]  receive UC_SERVICE_INFO_FOR_SOFTCELL_PREVIEW \n",__FUNCTION__);
		{
			uc_service_info_for_softcell_preview_st *pSCellPreview = (uc_service_info_for_softcell_preview_st *)pVoid;

			for(i=0; i<IRUC_HUMAX_MAX_TP; i++)
			{
				if( pSCellPreview->serviceHandle == g_IrUcSvcInfo[i].UC_SvcHandle)
				{
					//1 TODO??
				}
			}
			}
			break;
		default:
			HxLOG_Info("Received unknown message: 0x%08X\n", message);
			break;
	}

	VK_SEM_Release(s_ulUCCAAPISema);
#if defined(CONFIG_DEBUG)
	HxLOG_Info("\n");
#endif
	return;
}


#define _______________MSG_FROM_CAI__________________


static void local_cas_iruc_Drv_Make_UcSvcContext(HUINT8 ActionID_in, HBOOL IsECM, HUINT32 *pUcSvcContext_out)
{
	*pUcSvcContext_out =  ( (  ((IsECM) ? IRUC_ECM_PREFIX : IRUC_EMM_PREFIX)  << 24 ) | ActionID_in  ) & 0xFF0000FF; //prefix/00/00/action id
}
static void local_cas_iruc_Drv_ClearTpInfo(IRUC_TP_INFO *pIrUcTpInfo)
{
	if (NULL != pIrUcTpInfo->pCatRaw)
	{
		OxCAS_Free(pIrUcTpInfo->pCatRaw);
		pIrUcTpInfo->pCatRaw = NULL;
	}

	pIrUcTpInfo->hAction = HANDLE_NULL;
	pIrUcTpInfo->ucActId = 0xFF;

	pIrUcTpInfo->ulDemuxId = RxRSCID_NULL;
	pIrUcTpInfo->usTsUniqueId = IRUC_INVALID_TS_UNIQUE_ID ;
	pIrUcTpInfo->UC_SvcHandle = IRUC_INVALID_UC_SVCHANDLE;

	pIrUcTpInfo->UcSvcContext = IRUC_INVALID_UC_SVC_CONTEXT;;
	pIrUcTpInfo->ucUsed	=	FALSE;

	VK_MEM_Memset(pIrUcTpInfo->szEmmStatusMsg, 0x00, sizeof(pIrUcTpInfo->szEmmStatusMsg) );
	VK_MEM_Memset(pIrUcTpInfo->szSvcStatusMsg, 0x00, sizeof(pIrUcTpInfo->szSvcStatusMsg) );
// hjlee3, monitoring init
	VK_MEM_Memset(&pIrUcTpInfo->UCmonitorInfo, 0x00, sizeof(uc_convergent_client_service_monitor_status_st) );
	pIrUcTpInfo->UCStreamStatusInfo=NULL;
}

void local_cas_iruc_Drv_ClearPidListInfo(HUINT32 MainIndex, HUINT32 SubIndex)
{
	IRUC_PID_INFO *pPidInfo =  &(g_IrUcSvcInfo[MainIndex].PidList[SubIndex]);

	HxLOG_Print("[%s] Main idx=%d, Sub Idx=%d\n", __FUNCTION__,MainIndex, SubIndex);

	pPidInfo->usPID = IRUC_INVALID_PID;
	pPidInfo->ucTypeFromPMT = eEsType_None;
	pPidInfo->ucTypeFromPlay = eEsType_None;
	pPidInfo->ucAllocatedDsc = FALSE ;
}

void local_cas_iruc_Drv_ClearServiceInfo(HUINT32 index)
{
	HUINT8	i;
	IRUC_SVC_INFO *pSvcInfo = &g_IrUcSvcInfo[index] ;

    HxLOG_Print("[%s] index=%d\n",__FUNCTION__, index);

	for( i = 0; i < IRUC_SVC_MAX_PID; i++ )
	{
		local_cas_iruc_Drv_ClearPidListInfo(index, i);
	}
	pSvcInfo->ucStreamCount = 0;

	if( pSvcInfo->pPmtRaw != NULL )
	{
		OxCAS_Free(pSvcInfo->pPmtRaw);
		pSvcInfo->pPmtRaw = NULL;
	}

	pSvcInfo->IsValidPmt = 0;

	pSvcInfo->hIrSvc = HANDLE_NULL;
	pSvcInfo->hSvc = HANDLE_NULL;
	pSvcInfo->usTsUniqueId = IRUC_INVALID_TS_UNIQUE_ID;
	pSvcInfo->ulDemuxId = IRUC_STREAM_MAX_NUM;


	pSvcInfo->IsMonitoring = 0;
//	pSvcInfo->eSvcCasKind = 0;
	pSvcInfo->IsSentSvcDefine = 0;
//	pSvcInfo->UC_SvcType = SERVICE_INVALID;
	pSvcInfo->IsRunning = 0;
	pSvcInfo->UC_SvcHandle = IRUC_INVALID_UC_SVCHANDLE;
	pSvcInfo->UcSvcContext = IRUC_INVALID_UC_SVC_CONTEXT;


	VK_MEM_Memset(pSvcInfo->szEcmStatusMsg, 0x00, sizeof(pSvcInfo->szEcmStatusMsg) );
	VK_MEM_Memset(pSvcInfo->szSvcStatusMsg, 0x00, sizeof(pSvcInfo->szSvcStatusMsg) );
// hjlee3, monitoring init
	VK_MEM_Memset(&pSvcInfo->UCmonitorInfo, 0x00, sizeof(uc_convergent_client_service_monitor_status_st));
	pSvcInfo->UCStreamStatusInfo=NULL;

}


IRUC_TP_INFO *local_cas_iruc_Drv_FindIrTp(Handle_t hAction)
{
	HINT32	i;

	for (i=0; i<IRUC_HUMAX_MAX_TP; i++)
	{
		if(( g_IrUcTpInfo[i].hAction == hAction ) || (GET_ACTION_ID(hAction) == GET_ACTION_ID(g_IrUcTpInfo[i].hAction)) )
		{
			HxLOG_Print("[%s] !!! Found g_IrUcTpInfo[%d] hAction = 0x%X !!!! \n", __FUNCTION__, i, g_IrUcTpInfo[i].hAction);
			return &g_IrUcTpInfo[i];
		}
	}

	return (IRUC_TP_INFO*)NULL;
}

IRUC_TP_INFO *local_cas_iruc_Drv_FindIrTpInfoByActId(HUINT8 ucActId)
{
	HINT32	i;

	for (i=0; i<IRUC_HUMAX_MAX_TP; i++)
	{
		if (g_IrUcTpInfo[i].ucActId == ucActId )
		{
			HxLOG_Print("[%s] !!! Found g_IrUcTpInfo[%d] hAction = 0x%X !!!! \n", __FUNCTION__, i, g_IrUcTpInfo[i].hAction);
			return &g_IrUcTpInfo[i];
	}
	}
	return (IRUC_TP_INFO*)NULL;
}

IRUC_TP_INFO *local_cas_iruc_Drv_CompareTpInfoByTsUniqueId(Handle_t hAction )
{
	HINT32	i;

	for (i=0; i<IRUC_HUMAX_MAX_TP; i++)
	{
		if (g_IrUcTpInfo[i].hAction == hAction)
		{
			HxLOG_Print("[%s] !!! Found g_IrUcTpInfo[%d] hAction = 0x%X !!!! \n", __FUNCTION__,i, hAction);
			return &g_IrUcTpInfo[i];
		}
	}

	return (IRUC_TP_INFO*)NULL;
}


IRUC_TP_INFO *local_cas_iruc_Drv_FindIrTpInfoByTsUniqueId(HUINT16 usTsUniqueId)
{
	HINT32	i;

	for (i=0; i<IRUC_HUMAX_MAX_TP; i++)
	{
		if (g_IrUcTpInfo[i].usTsUniqueId == usTsUniqueId && g_IrUcTpInfo[i].ucUsed != TRUE)
		{
			HxLOG_Print("[%s] !!! Found g_IrUcTpInfo[%d] hAction = 0x%X !!!! \n", __FUNCTION__, i, g_IrUcTpInfo[i].hAction);
			return &g_IrUcTpInfo[i];
	        }
	}

	return (IRUC_TP_INFO*)NULL;
}
#if defined( __FORJUSTONETUNER__)
/* use just one tuner model ,
   send CAT one time to UC Client */
HUINT8 local_cas_iruc_Drv_FindUsedIrTpInfo(void)
{
	HINT32	i;

	for (i=0; i<IRUC_HUMAX_MAX_TP; i++)
	{
		if (g_IrUcTpInfo[i].ucUsed == TRUE)
		{
			HxLOG_Print("[%s] Found g_IrUcTpInfo[%d] hAction = 0x%X !!!! \n", __FUNCTION__, i, g_IrUcTpInfo[i].hAction);
			return TRUE;
	    }
	}

	return FALSE;
}
#endif

void local_cas_iruc_Drv_ProcessMsgFromCAI(HMSG *msg)
{

	HxLOG_Print("\n");

	if( msg->ulParam == (HUINT32)NULL )
	{
		HxLOG_Print("[%s] ulParams' value has Null\n", __FUNCTION__);
		return ;
	}

	HxLOG_Print("[%s] UC mgr message type = 0x%X \n", __FUNCTION__, msg->usParam);

	switch(msg->usParam)
	{
	case CASMGR_CAI_START_SERVICE:
		local_cas_iruc_Drv_StartService((CAS_CAI_IF_msgPID *)(msg->ulParam));
		break;

	case CASMGR_CAI_STOP_SERVICE:
		local_cas_iruc_Drv_StopService((CAS_CAI_IF_msgPID *)(msg->ulParam));
		break;

	case CASMGR_SI_PMT_UPDATE:
		local_cas_iruc_Drv_PMT_Update((CAS_CAI_IF_msgPMT *)(msg->ulParam));
		break;

	case CASMGR_CAI_SET_PID:
		local_cas_iruc_Drv_SetPID((CAS_CAI_IF_msgPIDList *)(msg->ulParam));
		break;

	case CASMGR_CAI_STOP_TS:
		local_cas_iruc_Drv_StopTS((CAS_CAI_IF_msgTS *)(msg->ulParam));
		break;

#if 0
	case CASMGR_IR_WRITE_MAIL:
		MSG_DRV_WriteMail();
		break;
#endif
	case CASMGR_SI_CAT_UPDATE:
		local_cas_iruc_Drv_CAT_Update((CAS_CAI_IF_msgCAT *)(msg->ulParam));
		break;

	case CASMGR_CAINST_SELECTDESELECT:
	{
		CAS_CAI_IF_msgCAINST_SELECT		*pMsgSelect;

		pMsgSelect = (CAS_CAI_IF_msgCAINST_SELECT *)(msg->ulParam);

		if( 0 != pMsgSelect->IsSelect ) // Value 1 is selected.
		{
			local_cas_iruc_Drv_IamSelected(pMsgSelect );
		}
		else /* if pre-comp.def "IRPLAT_SYS_1CAS1SC" is declared, below condition must not be done */
		{
			local_cas_iruc_Drv_IamDeSelected(pMsgSelect);
		}
		break;
	}

	default:
		HxLOG_Print("[%s] NOT Processed.(msg type = 0x%X)\n", __FUNCTION__, msg->usParam);
		break;
	}
}


void local_cas_iruc_Drv_StartService(CAS_CAI_IF_msgPID	*pMsgPID)
{
	HUINT32			i;
	HUINT8			ucNumOfOpenService = 0;
	HUINT32			svcInfoIndex = 0;
	HUINT32	ulDemuxId;
	HERROR			hErr;
	HMSG			hMsg;
	Handle_t		hIrSvc	= pMsgPID->hIrSvc;
	Handle_t		hAct	= pMsgPID->hAct;
	Handle_t		hSvc 	= pMsgPID->hSvc;
	SvcCas_ActionType_e 	hActionType= pMsgPID->eActType;
	HBOOL	flagFoundSameAction= FALSE;

	HxLOG_Print("[%s]\n",__FUNCTION__);

	if(hActionType == eCAS_ActionType_Recording)
		HxLOG_Print("[%s] #### This is Recording or Tsr or Playback  #### hAct:0x%X hIrSvc:0x%X, hSvc:0x%X \n", __FUNCTION__ ,hAct, pMsgPID->hIrSvc, pMsgPID->hSvc);
	else if(hActionType == eCAS_ActionType_Playback)
		HxLOG_Print("[%s] #### This is Playback  #### hAct:0x%X hIrSvc:0x%X, hSvc:0x%X \n", __FUNCTION__ ,hAct, pMsgPID->hIrSvc, pMsgPID->hSvc);
	else if(hActionType == eCAS_ActionType_Tsr_Rec)
		HxLOG_Print("[%s] #### This is Tsr  #### hAct:0x%X hIrSvc:0x%X, hSvc:0x%X \n", __FUNCTION__ ,hAct, pMsgPID->hIrSvc, pMsgPID->hSvc);
	else if(hActionType == eCAS_ActionType_Tsr_Play)
		HxLOG_Print("[%s] #### This is Tsr Play  #### hAct:0x%X hIrSvc:0x%X, hSvc:0x%X \n", __FUNCTION__ ,hAct, pMsgPID->hIrSvc, pMsgPID->hSvc);
	else if(hActionType == eCAS_ActionType_Live)
		HxLOG_Print("[%s] $$$$ This is Live  $$$$ hAct:0x%X hIrSvc:0x%X, hSvc:0x%X \n", __FUNCTION__ ,hAct, pMsgPID->hIrSvc, pMsgPID->hSvc);
	else
		HxLOG_Print("[%s] $$$$  what this type ???   $$$$ hAct:0x%X hIrSvc:0x%X, hSvc:0x%X \n", __FUNCTION__ ,hAct, pMsgPID->hIrSvc, pMsgPID->hSvc);

	// 현재 몇개의 service가 softcell에 open되었는지 확인.
	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].IsRunning )
		{
			ucNumOfOpenService++;
		}

		if( (g_IrUcSvcInfo[i].hIrSvc != HANDLE_NULL) && (g_IrUcSvcInfo[i].hIrSvc == hIrSvc) )
		{
			flagFoundSameAction = TRUE;
			svcInfoIndex = i;
		}
	}

	if( FALSE == flagFoundSameAction )
	{
		HxLOG_Print("[%s] NEW IR SVC\n" , __FUNCTION__);
		for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
		{
			if( g_IrUcSvcInfo[i].hIrSvc == HANDLE_NULL )
				break;
		}

		if( i == IRUC_HUMAX_MAX_SERVICE )
		{
			HxLOG_Print("START SVC : service pool is full!!\n");
			return;
		}

		hErr = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(hIrSvc), eRxRSCTYPE_DEMUX, &ulDemuxId);
		if(hErr != HIR_OK)
		{
			HxLOG_Print("[%s] svc_cas_SubSvcRmGetResourceId: Error: 0x%X\n", __FUNCTION__, hErr);
			return;
		}

		svcInfoIndex = i;

		g_IrUcSvcInfo[i].hIrSvc = hIrSvc;
		g_IrUcSvcInfo[i].hAct = hAct;
		g_IrUcSvcInfo[i].hSvc = hSvc;
		g_IrUcSvcInfo[i].usTsUniqueId = GET_UNIQUE_ID(hSvc);
		g_IrUcSvcInfo[i].ulDemuxId = ulDemuxId;
		g_IrUcSvcInfo[i].hActionType = hActionType; // action type

		HxLOG_Print("[%s] nDmxId = %d, svcInfoIndex = %d , hActionType = %d \n", __FUNCTION__, ulDemuxId, i ,hActionType);

		/* notification to ir event machine , event of starting service. */
		hMsg.usMsgClass = IREVT_CLASS_SVC_DESCRAMBLE_OPEN;
		CAS_IR_EVT_SendEvent(g_IrUcSvcInfo[i].hAct, &hMsg);

		// 2. Open a service for ECMs
		local_cas_iruc_Drv_Make_UcSvcContext( GET_ACTION_ID(g_IrUcSvcInfo[i].hAct), TRUE, &(g_IrUcSvcInfo[i].UcSvcContext));

		if(UC_ERROR_SUCCESS == CAS_IRUC_DRV_Open_UC_Svc(g_IrUcSvcInfo[i].UcSvcContext, &g_IrUcSvcInfo[i].UC_SvcHandle))
		{
			g_IrUcSvcInfo[i].IsRunning = 1;
		}

		HxLOG_Print("[%s] g_IrUcSvcInfo[%d].UC_SvcHandle = %0x, hAct=0x%X, hIrSvc=0x%X, hSvc=0x%X \n\n", __FUNCTION__,i , g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].hAct, g_IrUcSvcInfo[i].hIrSvc, g_IrUcSvcInfo[i].hSvc);


		/* TMS set */
		if(xsvc_cas_IrUcSetTMSService(i))
		{
			HxLOG_Print("[%s] TMS Set FAIL SVC_index(%d) \n\n", __FUNCTION__ ,i);
		}

	}
	else
	{
		HxLOG_Print("[%s] Same MW SERVICE in Index(%d)\n", __FUNCTION__,i);
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}

}

void local_cas_iruc_Drv_IamSelected(CAS_CAI_IF_msgCAINST_SELECT *pMsgSelect)
{
	HUINT32		i;

	HxLOG_Print("[%s] hIrSvc = 0x%X \n", __FUNCTION__, pMsgSelect->hIrSvc);

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].hIrSvc == pMsgSelect->hIrSvc )
		{
			HxLOG_Print("[%s] Same hIrSvc = 0x%X \n", __FUNCTION__, pMsgSelect->hIrSvc);
			break;
		}
	}

	if( i == IRUC_HUMAX_MAX_SERVICE )
	{
		HxLOG_Print("[%s] not started service!!\n",__FUNCTION__);
		return;
	}

	// 3. Open a service for ECMs
//	local_cas_iruc_Drv_Make_UcSvcContext( GET_ACTION_ID(g_IrUcSvcInfo[i].hAct), TRUE, 	&(g_IrUcSvcInfo[i].UcSvcContext) ) ;
//	CAS_IRUC_DRV_Open_UC_Svc(g_IrUcSvcInfo[i].UcSvcContext, &g_IrUcSvcInfo[i].UC_SvcHandle);

/* Comment : UCCA open 됫다면 Running으로 본다.  */
//	g_IrUcSvcInfo[i].IsRunning = 1;
	g_IrUcSvcInfo[i].ucIsSelected = 1;
}


void local_cas_iruc_Drv_IamDeSelected(CAS_CAI_IF_msgCAINST_SELECT *pMsgSelect)
{
	HUINT32		i;

	HxLOG_Print("[%s] : hIrSvc -0x%X\n", __FUNCTION__, pMsgSelect->hIrSvc);
	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].hIrSvc == pMsgSelect->hIrSvc )
			break;
	}

	if( i == IRUC_HUMAX_MAX_SERVICE )
	{
		HxLOG_Print("[%s] not registered service!!\n", __FUNCTION__);
		return;
	}

	if( g_IrUcSvcInfo[i].IsRunning )
	{
/* stp service 가 날라 가서  구지 여기서는 할 필요 없어 보인다. */
//		CAS_IRUC_DRV_Close_UC_Svc(&g_IrUcSvcInfo[i].UC_SvcHandle);
//		g_IrUcSvcInfo[i].IsRunning = 0;
  		g_IrUcSvcInfo[i].ucIsSelected = 0;
	}
	else
	{
		// Deselect라는 건 이미 select된 걸 deselect하는 거고, 이미 select라는 건 SoftCell에 open해서 사용하고 있는 것이라는 말이므로,
		// 여기로 오는건 ERROR!!!
		HxLOG_Print("[%s] not running service resource!!\n", __FUNCTION__);
	}
}


void local_cas_iruc_Drv_StopService(CAS_CAI_IF_msgPID	*pMsgPID)
{
	HUINT32		i;
	HUINT8		ucNumOfOpenService = 0;
	Handle_t	hIrSvc;

	HxLOG_Print("[%s] hIrSvc - 0x%X\n",__FUNCTION__ , pMsgPID->hIrSvc);

	hIrSvc = pMsgPID->hIrSvc;

	// 현재 몇개의 service가 softcell에 open되었는지 확인.
	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].IsRunning )
			ucNumOfOpenService++;
	}

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( (g_IrUcSvcInfo[i].hIrSvc != HANDLE_NULL) && (g_IrUcSvcInfo[i].hIrSvc == hIrSvc) )
		{
			break;
		}
	}

	if( i == IRUC_HUMAX_MAX_SERVICE )
	{
		HxLOG_Print("[%s]  not registerd service1!!\n", __FUNCTION__);
	}
	else
	{
		HxLOG_Print("[%s]  STOP & CLEAR for index(%d) ,Running(%d) \n", __FUNCTION__, i, g_IrUcSvcInfo[i].IsRunning);

		if( g_IrUcSvcInfo[i].IsRunning )
		{

			if(g_IrUcSvcInfo[i].UC_SvcHandle != IRUC_INVALID_UC_SVCHANDLE)
			{
				CAS_IRUC_DRV_Close_UC_Svc(&g_IrUcSvcInfo[i].UC_SvcHandle);
			}

			local_cas_iruc_Drv_ClearServiceInfo(i);
		}
		else
		{
			HxLOG_Print("[%s] ### No Opened Service : Just Clear\n", __FUNCTION__);
			local_cas_iruc_Drv_ClearServiceInfo(i);

		}
	}

}

void local_cas_iruc_Drv_SetPID(CAS_CAI_IF_msgPIDList	*pMsgPIDList)
{
	HUINT8	i, j,k,ucElementCnt=0;
	HBOOL	flagNeedToSendSvcDefine = FALSE;
	Handle_t	hIrSvc;
	HUINT16 	usPidIndex;
//	HUINT8	ucSource, ucOutput;

	HxLOG_Print("[%s] hIrSvc - 0x%X\n", __FUNCTION__ , pMsgPIDList->hIrSvc);

	hIrSvc = pMsgPIDList->hIrSvc;

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( (g_IrUcSvcInfo[i].hIrSvc != HANDLE_NULL) && (g_IrUcSvcInfo[i].hIrSvc == hIrSvc) )
		{
			if( g_IrUcSvcInfo[i].hIrSvc == hIrSvc)
			{
				HxLOG_Print("[%s] Found Set PID IDX=%d hIrSvc - 0x%X\n", __FUNCTION__ , i, pMsgPIDList->hIrSvc);
				break;
			}
		}
	}

	if( i == IRUC_HUMAX_MAX_SERVICE )
	{
		HxLOG_Print("[%s] not registerd service! hIrSvc=0x%x \n",__FUNCTION__, hIrSvc);
		return;
	}

	// 들어온 astPidInfo를 돌면서...
	for( k= 0; k < IRUC_SVC_MAX_PID; k++ )
	{
//		HxLOG_Print("[%s] element Pid = 0x%x, EsType = %d \n",__FUNCTION__, pMsgPIDList->astPidInfo[k].usPID, pMsgPIDList->astPidInfo[k].ucType);
		if( pMsgPIDList->astPidInfo[k].usPID == IRUC_INVALID_PID )
		{
//			HxLOG_Print("[%s] This element Pid(0x%04x) don't need so Pass  \n",__FUNCTION__,pMsgPIDList->astPidInfo[k].usPID);
			continue;
		}

		// INVALID_PID가 아니면 ServiceInfo의 PisList에 동일한 PID가 이미 있는가 확인한 후...
		// Type 도 비교해야 한다. !!! jichoi.

/* TODO: component Pid check. 제대로 setting이 되냐 확인 필요 하다. */
		for(j=0; j< IRUC_SVC_MAX_PID; j++)
		{
			if(( g_IrUcSvcInfo[i].PidList[j].usPID == pMsgPIDList->astPidInfo[k].usPID)
				&& (g_IrUcSvcInfo[i].PidList[j].ucTypeFromPlay== pMsgPIDList->astPidInfo[k].ucType))
			{
				usPidIndex = i;
				break;
			}
		}

		if(j == IRUC_SVC_MAX_PID)
		{
			// 이전에 set되지 않은 새로운 PID가 들어온 것이므로 새로 set하고 service define도 보내자.
			for( j = 0; j < IRUC_SVC_MAX_PID; j++ )
			{
				if( g_IrUcSvcInfo[i].PidList[j].usPID == IRUC_INVALID_PID )
				{
					g_IrUcSvcInfo[i].PidList[j].usPID = pMsgPIDList->astPidInfo[k].usPID;
					g_IrUcSvcInfo[i].PidList[j].ucTypeFromPlay = pMsgPIDList->astPidInfo[k].ucType;

					HxLOG_Print("[%s] Component New g_IrUcSvcInfo[%d][%d] = 0x%X:0x%X \n",__FUNCTION__,i,j, g_IrUcSvcInfo[i].PidList[j].usPID,pMsgPIDList->astPidInfo[k].usPID);
					flagNeedToSendSvcDefine = TRUE;

					break;
				}
			}

			if( j == IRUC_SVC_MAX_PID )
			{
				HxLOG_Print("[%s] SET PID ERROR - no more space for new PID!!!\n");
			}

//			g_IrUcSvcInfo[i].ucStreamCount = pMsgPIDList->ulPidInfoCnt;

		}
		else
		{
			// 이미 존재하는 PID이다.
			HxLOG_Print("[%s] Already (%d)th pid (= 0x%X) allocated in(index = %d). \n", __FUNCTION__, j, pMsgPIDList->astPidInfo[j].usPID, usPidIndex);

		}
	}

	for( j = 0; j < IRUC_SVC_MAX_PID; j++ )
	{
		if( g_IrUcSvcInfo[i].PidList[j].usPID != IRUC_INVALID_PID )
		{
			ucElementCnt++;
		}
	}

	g_IrUcSvcInfo[i].ucStreamCount = ucElementCnt;
	HxLOG_Print("[%s] g_IrUcSvcInfo[%d].StreamCount= %d \n", __FUNCTION__,i, g_IrUcSvcInfo[i].ucStreamCount);
	// TODO: Integration required.
	// 새로운 PID가 하나라도 있으면...
	if( TRUE == flagNeedToSendSvcDefine  )
	{
		if( g_IrUcSvcInfo[i].UC_SvcHandle != IRUC_INVALID_UC_SVCHANDLE )
		{
			if( g_IrUcSvcInfo[i].IsRunning )
			{
				HxLOG_Print("[%s] How???\n", __FUNCTION__);
//				local_cas_iruc_Drv_Make_UcSvcContext( GET_ACTION_ID(g_IrUcSvcInfo[i].hAct), TRUE, 	&(g_IrUcSvcInfo[i].UcSvcContext) ) ;
//				CAS_IRUC_DRV_Open_UC_Svc(g_IrUcSvcInfo[i].UcSvcContext, &g_IrUcSvcInfo[i].UC_SvcHandle);
			}
		}
		else
		{
			HxLOG_Print("[%s] Open Reply is not yet received\n",__FUNCTION__);
		}
	}
	else
	{
		HxLOG_Print("[%s] SAME PID LIST!!\n",__FUNCTION__);
	}

}

void local_cas_iruc_Drv_StartTS(CAS_CAI_IF_msgTS	*pMsgTS)
{
	IRUC_TP_INFO	*pIrUcTpInfo;
	HERROR			hErr;
	HUINT32	ulDemuxId;

	HxLOG_Print("[%s] unique id = 0x%X, hAct=0x%X\n",__FUNCTION__, pMsgTS->usTsUniqueId, pMsgTS->hAction);

	if (pMsgTS == NULL)
		return;

//	pIrUcTpInfo = local_cas_iruc_Drv_FindIrTpInfoByTsUniqueId(pMsgTS->usTsUniqueId);
	pIrUcTpInfo = local_cas_iruc_Drv_CompareTpInfoByTsUniqueId(pMsgTS->hAction);
	if (pIrUcTpInfo != NULL)
	{
		HxLOG_Print("[%s] Already started unique Ts id(0x%X)\n",__FUNCTION__, pMsgTS->usTsUniqueId);
		return;
	}

/* TODO: [0]:view [1]:rec 으로 사용 해야 한다.
         rec 라는 inform을 받거나 SVC info에 hActionType 처럼 param으로 저장 해 놔야 한 다. */
	pIrUcTpInfo = local_cas_iruc_Drv_FindIrTpInfoByTsUniqueId(IRUC_INVALID_TS_UNIQUE_ID);
	if (pIrUcTpInfo != NULL)
	{
		pIrUcTpInfo->hAction= pMsgTS->hAction;
		pIrUcTpInfo->ucActId = GET_ACTION_ID(pMsgTS->hAction);

		HxLOG_Print("[%s] unique id = 0x%X, action handle=0x%X, Tp act id=%d\n", __FUNCTION__, pMsgTS->usTsUniqueId, pMsgTS->hAction, pIrUcTpInfo->ucActId);

		hErr = svc_cas_SubSvcRmGetResourceId(svc_cas_SubSvcRmGetActionId(pIrUcTpInfo->hAction), eRxRSCTYPE_DEMUX, &ulDemuxId);
		if(hErr != HIR_OK)
		{
			HxLOG_Print("[%s] svc_cas_SubSvcRmGetResourceId: Error: 0x%X\n", __FUNCTION__, hErr);
			return;
		}

		pIrUcTpInfo->ulDemuxId = ulDemuxId;
		pIrUcTpInfo->usTsUniqueId = pMsgTS->usTsUniqueId;
        pIrUcTpInfo->svcType = SERVICE_DVB_EMM ;
//		HxLOG_Print("[%s] pIrUcTpInfo->ulDemuxId= %d\n",__FUNCTION__ , ulDemuxId);
//		HxLOG_Print("[%s] pIrUcTpInfo->usTsUniqueId= %d\n",__FUNCTION__ , pIrUcTpInfo->usTsUniqueId);

		// 2. Open a service for EMMs
		local_cas_iruc_Drv_Make_UcSvcContext(pIrUcTpInfo->ucActId, FALSE, &(pIrUcTpInfo->UcSvcContext));
		hErr =CAS_IRUC_DRV_Open_UC_Svc(pIrUcTpInfo->UcSvcContext, &(pIrUcTpInfo->UC_SvcHandle));
		HxLOG_Print("[%s] g_IrUcTpInfo addr:0x%X  UC_SvcHandle=0x%X \n\n",__FUNCTION__, pIrUcTpInfo ,pIrUcTpInfo->UC_SvcHandle);

		if(hErr !=UC_ERROR_SUCCESS)
		{
			HxLOG_Print("[%s] local_cas_iruc_Drv_StartTS, FAIL, result = %d\n",__FUNCTION__,  hErr );
			return;
		}
		pIrUcTpInfo->ucUsed = TRUE;

		HxLOG_Print("[%s] local_cas_iruc_Drv_StartTS, OK, with UC_SvcHandle = 0x%X\n", __FUNCTION__, pIrUcTpInfo->UC_SvcHandle );
	}
	else
	{
		HxLOG_Print("[%s] TsInfo was full!!\n", __FUNCTION__);
	}
}

void local_cas_iruc_Drv_StopTS(CAS_CAI_IF_msgTS	*pMsgTS)
{
	IRUC_TP_INFO							*pIrUcTpInfo;
	HUINT32								nRet;

	HxLOG_Print("[%s] unique id = 0x%X, hAct=0x%X ActId:%d \n", __FUNCTION__, pMsgTS->usTsUniqueId, pMsgTS->hAction, GET_ACTION_ID(pMsgTS->hAction));

	pIrUcTpInfo = local_cas_iruc_Drv_FindIrTpInfoByActId( GET_ACTION_ID(pMsgTS->hAction));
	if (pIrUcTpInfo == NULL)
	{
		HxLOG_Print("[%s] not started TpInfo!! usTsUniqueId[0x%X]\n", __FUNCTION__, pMsgTS->usTsUniqueId);

#if defined(CONFIG_DEBUG)
		local_cas_iruc_Dbg_PrintTpInfo();
#endif
		return;
	}

	HxLOG_Print("[%s] pIrUcTpInfo->usTsUniqueId = 0x%X, ActionId = %d, pMsgTS->usTsUniqueId= 0x%X \n ", __FUNCTION__,pIrUcTpInfo->usTsUniqueId, pIrUcTpInfo->ucActId, pMsgTS->usTsUniqueId);
/* 의미가 모지 다음 tp 와 같다면 clear 하지 않는다고 하는 것인가 ??
   live + tsr + rec 에서 하나의 tunner 에서는 stop을 할필요가 없다고 하는 걸로 보임. */
#if 0
	if(pIrUcTpInfo->usTsUniqueId == pMsgTS->usTsUniqueId )
	{
		HxLOG_Print("[%s] Next tp info is same, then it must be continued, no action needed\n ", __FUNCTION__);
		return ;
	}
#endif
	HxLOG_Print("Ts Close needed\n ");

#if defined(CONFIG_DEBUG)
	if(IRUC_INVALID_TS_UNIQUE_ID == pMsgTS->usTsUniqueId)
	{
		HxLOG_Print("[%s] Next tp info is empty,\n ", __FUNCTION__);
	}
	else
	{
		HxLOG_Print("[%s] Ts will definitely is being chnaged,\n ", __FUNCTION__);
	}
#endif

	nRet  = CAS_IRUC_DRV_Close_UC_Svc(&pIrUcTpInfo->UC_SvcHandle);
	if (nRet != UC_ERROR_SUCCESS )
	{
		HxLOG_Print("[%s] CAS_IRUC_DRV_Close_UC_Svc, failed ret= %d\n", __FUNCTION__, nRet);
	}

	local_cas_iruc_Drv_ClearTpInfo(pIrUcTpInfo);
}



void local_cas_iruc_Drv_CAT_Update(CAS_CAI_IF_msgCAT	*pMsgSI)
{
	IRUC_TP_INFO					*pUcTpInfo;

	HxLOG_Print("[%s] hAction= 0x%X, usTsUniqueId= 0x%X\n",__FUNCTION__ , pMsgSI->hAction, pMsgSI->usTsUniqueId);

	if (pMsgSI == NULL || pMsgSI->pRawData == NULL)
		return;

	if (pMsgSI->hAction == HANDLE_NULL)
	{
		HxLOG_Print("[%s]  local_cas_iruc_Drv_CAT_Update : ERROR - pMsgSI->hAction == HANDLE_NULL\n",__FUNCTION__ );
		return;
	}


	pUcTpInfo = local_cas_iruc_Drv_FindIrTp(pMsgSI->hAction);
	if (pUcTpInfo == NULL)
	{
		CAS_CAI_IF_msgTS MsgTs;
		MsgTs.usTsUniqueId 	=  (pMsgSI->usTsUniqueId);
		MsgTs.hAction=  (pMsgSI->hAction);
#if defined(__FORJUSTONETUNER__)
/* use just one tuner model , send CAT one time to UC Client in same TP */
		if(local_cas_iruc_Drv_FindUsedIrTpInfo())
		{
			HxLOG_Print("[%s] Already used same TP exists, don't update CAT \n",__FUNCTION__);
			return;
		}
#endif
		local_cas_iruc_Drv_StartTS(&MsgTs);
		pUcTpInfo = local_cas_iruc_Drv_FindIrTp(pMsgSI->hAction);
	}
	else
	{
		// TODO: 실제 playing중의 cat update. 지금은 같은 TP내에서 service change를 해도 발생하고 잇음.
		HxLOG_Print("[%s] CAT upate occurs !!! \n",__FUNCTION__ );
	}

	if(pUcTpInfo == NULL)
	{
		HxLOG_Print("[%s] Add Tp info failed \n",__FUNCTION__ );
		return;
	}

	HxLOG_Print("[%s] pUcTpInfo UcSvcContext=0x%X, UC_SvcHandle=%d, nCatSize= %d \n",__FUNCTION__, pUcTpInfo->UcSvcContext, pUcTpInfo->UC_SvcHandle, pUcTpInfo->nCatSize);

	//{{{  Step: CAT가 실제로 같은것이 들어온 상황에서  이미 활성화 되어 잇는 softcell emm 을 다시 재 시작하지 않기 위해서 체크.
	if ((pMsgSI->pRawData !=NULL)
		&& (pUcTpInfo->pCatRaw !=NULL)
		&& (pUcTpInfo->nCatSize == pMsgSI->usLength) )
	{
		HxLOG_Print("[%s] pUcTpInfo->pCatRaw= 0x%X, pMsgSI->pRawData= 0x%X\n",__FUNCTION__ ,(HINT32) pUcTpInfo->pCatRaw, (HINT32)pMsgSI->pRawData);

		// TODO: Section CRC가 보이면 그걸로만 비교하자. too complexity high.
		if (memcmp(pUcTpInfo->pCatRaw, pMsgSI->pRawData, pMsgSI->usLength) == 0)
		{
			if(pUcTpInfo->UC_SvcHandle != IRUC_INVALID_UC_SVCHANDLE)
			{
				HxLOG_Print("[%s] You don't need update cat due of Same CAT updated\n",__FUNCTION__ );
				if(pMsgSI->pRawData != NULL)
				{
					OxCAS_Free(pMsgSI->pRawData);
				}

				return;
			}
			else
			{
				//section 은 valid, same 한 것이 잇지만.  실제 tp info update 가 필요한 경우.
				//condition 이 valid 하다면 , below 에서 update 될 것이다.
			}
		}
		else
		{
			HxLOG_Print("[%s] You have to update CAT because of Different CAT updated\n",__FUNCTION__ );
			//Just NOTIFY updated CAT
			//CAS_IRUC_DRV_Close_UC_Svc(&pUcTpInfo->UC_SvcHandle);

			//just clear UC service handle.
			//pUcTpInfo->UC_SvcHandle = IRUC_INVALID_UC_SVCHANDLE;

		}
	}
	else
	{
		HxLOG_Print("[%s] There were no stored CAT in TpInfo, Then You need update cat\n",__FUNCTION__ );
	}

	if( pUcTpInfo->pCatRaw != NULL)
	{
		HxLOG_Print("[%s] free stored CatRaw \n",__FUNCTION__ );
		OxCAS_Free(pUcTpInfo->pCatRaw);
	}

	if( pMsgSI->usLength ==0  )
	{
		HxLOG_Print("[%s] CAT Length == 0 so don't Notify \n",__FUNCTION__ );
		return ;
	}

	pUcTpInfo->nCatSize = 0;

	pUcTpInfo->pCatRaw = OxCAS_Malloc(pMsgSI->usLength);
	if (pUcTpInfo->pCatRaw == NULL)
	{
		HxLOG_Print("[%s] Memalloc error. \n",__FUNCTION__ );
		pUcTpInfo->IsValidCat = FALSE;
		return;
	}

	HxSTD_memcpy(pUcTpInfo->pCatRaw, pMsgSI->pRawData, pMsgSI->usLength);

// hjlee, test CAT RAW data
#if 0
	HxLOG_Print("\n");
	for (i = 0; i < pMsgSI->usLength; i++)
	{
		if(i==15){ VK_Print("\n"); }
		HxLOG_Print("%02x ", pUcTpInfo->pCatRaw[i]);
	}
	HxLOG_Print("\n");
#endif
	pUcTpInfo->nCatSize = pMsgSI->usLength;

	OxCAS_Free(pMsgSI->pRawData);
	pMsgSI->pRawData = NULL;

	pUcTpInfo->IsValidCat = TRUE;
	// Step : CAT 를 Ir tp info에 적재하는 과정
	HxLOG_Print("\n");

	/* hjlee3, 메뉴에서 선택 하게 하기위해서  svc type 정해 준다. */
	pUcTpInfo->svcType = SERVICE_DVB_EMM ;
	CAS_IRUC_DRV_Client_Notify_CAT(pUcTpInfo->UC_SvcHandle, pUcTpInfo->pCatRaw, pUcTpInfo->nCatSize);
}

void local_cas_iruc_Drv_PMT_Update(CAS_CAI_IF_msgPMT	*pMsgSI)
{
	HUINT32		i, k;
	Handle_t		hIrSvc;

	HxLOG_Print("[%s] hIrSvc= 0x%X, usTsUniqueId= 0x%X\n",__FUNCTION__ , pMsgSI->hIrSvc, pMsgSI->usTsUniqueId);

	if (pMsgSI == NULL || pMsgSI->pRawData == NULL)
	{
		HxLOG_Print("[%s] ERROR : local_cas_iruc_Drv_PMT_Update NULL\n",__FUNCTION__ );
		return;
	}

	hIrSvc = pMsgSI->hIrSvc;

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].hIrSvc == hIrSvc )
		{
			HxLOG_Print("[%s] PMT update idx=%d hAct=0x%X, hIrSvc= 0x%X !! \n",__FUNCTION__, i,g_IrUcSvcInfo[i].hAct,hIrSvc );
			break;
		}
	}

	if( i == IRUC_HUMAX_MAX_SERVICE )
	{
		HxLOG_Print("[%s] hIrSvc is not exist in service pool!!\n",__FUNCTION__ );
		OxCAS_Free(pMsgSI->pRawData);
		return ;
	}


	if( g_IrUcSvcInfo[i].pPmtRaw != NULL )
	{
		HxLOG_Print("[%s] pmt dynamic updated\n",__FUNCTION__  );
		OxCAS_Free(g_IrUcSvcInfo[i].pPmtRaw);
		g_IrUcSvcInfo[i].pPmtRaw = NULL;
	}

	g_IrUcSvcInfo[i].pPmtRaw = OxCAS_Malloc(pMsgSI->usLength);
	if (g_IrUcSvcInfo[i].pPmtRaw == NULL)
	{
		g_IrUcSvcInfo[i].IsValidPmt = FALSE;
		return;
	}

	HxSTD_memcpy(g_IrUcSvcInfo[i].pPmtRaw, pMsgSI->pRawData, pMsgSI->usLength);

	if(pMsgSI->pRawData != NULL)
	{
		OxCAS_Free(pMsgSI->pRawData);
		pMsgSI->pRawData = NULL;
	}

	g_IrUcSvcInfo[i].IsValidPmt = TRUE;
/* hjlee3, 여기서 descramble type 정해 준다. */
	g_IrUcSvcInfo[i].svcType = SERVICE_DVB_DESCRAMBLE;

	// SCELL_LiveSvcHandle이 아직 적혀지지 않았을 수 있다. 즉, Open reply가 아직 오지 않았을 수 있으므로,
	// 만약 그렇다면 Open Reply가 올 때 다시 하도록 한다.
	if( g_IrUcSvcInfo[i].UC_SvcHandle != IRUC_INVALID_UC_SVCHANDLE )
	{
		if(g_IrUcSvcInfo[i].hActionType == eCAS_ActionType_Recording)
			HxLOG_Print("[%s] PMT Recording update.. g_IrUcSvcInfo[%d]. UC_SvcHandle:0x%X, UcSvcContext:0x%X, hAct:0x%X, hIrSvc:0x%X, hSvc:0x%X ActionType:%d \n",__FUNCTION__,i ,g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].UcSvcContext, g_IrUcSvcInfo[i].hAct, g_IrUcSvcInfo[i].hIrSvc, g_IrUcSvcInfo[i].hSvc, g_IrUcSvcInfo[i].hActionType);
		else if(g_IrUcSvcInfo[i].hActionType == eCAS_ActionType_Playback)
			HxLOG_Print("[%s] PMT Playback update.. g_IrUcSvcInfo[%d]. UC_SvcHandle:0x%X, UcSvcContext:0x%X, hAct:0x%X, hIrSvc:0x%X, hSvc:0x%X ActionType:%d \n",__FUNCTION__,i ,g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].UcSvcContext, g_IrUcSvcInfo[i].hAct, g_IrUcSvcInfo[i].hIrSvc, g_IrUcSvcInfo[i].hSvc, g_IrUcSvcInfo[i].hActionType);
		else if(g_IrUcSvcInfo[i].hActionType == eCAS_ActionType_Tsr_Rec)
			HxLOG_Print("[%s] PMT Tsr update.. g_IrUcSvcInfo[%d]. UC_SvcHandle:0x%X, UcSvcContext:0x%X, hAct:0x%X, hIrSvc:0x%X, hSvc:0x%X ActionType:%d \n",__FUNCTION__,i ,g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].UcSvcContext, g_IrUcSvcInfo[i].hAct, g_IrUcSvcInfo[i].hIrSvc, g_IrUcSvcInfo[i].hSvc, g_IrUcSvcInfo[i].hActionType);
		else if(g_IrUcSvcInfo[i].hActionType == eCAS_ActionType_Tsr_Play)
			HxLOG_Print("[%s] PMT Tsr play update.. g_IrUcSvcInfo[%d]. UC_SvcHandle:0x%X, UcSvcContext:0x%X, hAct:0x%X, hIrSvc:0x%X, hSvc:0x%X ActionType:%d \n",__FUNCTION__,i ,g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].UcSvcContext, g_IrUcSvcInfo[i].hAct, g_IrUcSvcInfo[i].hIrSvc, g_IrUcSvcInfo[i].hSvc, g_IrUcSvcInfo[i].hActionType);
		else if(g_IrUcSvcInfo[i].hActionType == eCAS_ActionType_Live)
			HxLOG_Print("[%s] PMT Live update.. g_IrUcSvcInfo[%d]. UC_SvcHandle:0x%X, UcSvcContext:0x%X, hAct:0x%X, hIrSvc:0x%X, hSvc:0x%X ActionType:%d \n",__FUNCTION__,i ,g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].UcSvcContext, g_IrUcSvcInfo[i].hAct, g_IrUcSvcInfo[i].hIrSvc, g_IrUcSvcInfo[i].hSvc, g_IrUcSvcInfo[i].hActionType);
		else
			HxLOG_Print("[%s] PMT etc update  what ??? .. g_IrUcSvcInfo[%d]. UC_SvcHandle:0x%X, UcSvcContext:0x%X, hAct:0x%X, hIrSvc:0x%X, hSvc:0x%X ActionType:%d \n",__FUNCTION__,i ,g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].UcSvcContext, g_IrUcSvcInfo[i].hAct, g_IrUcSvcInfo[i].hIrSvc, g_IrUcSvcInfo[i].hSvc, g_IrUcSvcInfo[i].hActionType);

		// TODO: 즉.. one service 시청중에 pmt 가 update 된 경우를 의미한다.
		// TODO: 이경우에 뒤에 set descramble이 오지 않기에 그런것으로 보인다.
#if 1
		for( k = 0; k < IRUC_SVC_MAX_PID; k++ )
		{
			if( g_IrUcSvcInfo[i].PidList[k].ucTypeFromPlay != (HUINT8)eEsType_None )
			{
				HxLOG_Print("[%s] Found New SVC g_IrUcSvcInfo[%d].PidList[%d]\n",__FUNCTION__,i,k);
				break;
			}
		}

		if( k != IRUC_SVC_MAX_PID )
		{
			CAS_IRUC_DRV_Client_Notify_PMT(g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].pPmtRaw, pMsgSI->usLength) ;
		}
		else
		{
			HxLOG_Print("[%s] What???.. \n",__FUNCTION__ );
		}
#else
/* Comment  : Recording에서  pmt update 후에 setpid가 오는 경우가 잇다. 우선은 무조건 보내는 것으로 해본다.  */
		CAS_IRUC_DRV_Client_Notify_PMT(g_IrUcSvcInfo[i].UC_SvcHandle, g_IrUcSvcInfo[i].pPmtRaw, pMsgSI->usLength) ;
#endif
	}
	else
	{
		HxLOG_Print("[%s] PMT: UC SVC Open  not yet opened\n",__FUNCTION__ );
	}
}

static	IRUC_SVC_INFO		*local_cas_iruc_Drv_FindIrSvcInfoByUC(HUINT16 usSCELLHandle)
{
	HINT32		i;
//	HxLOG_Print("[%s] g_IrUcSvcInfo.svchandle(%08x):usSCELLHandle(%08x) \n",__FUNCTION__ , g_IrUcSvcInfo[0].UC_SvcHandle, usSCELLHandle);
	for (i = 0; i < IR_HUMAX_MAX_SERVICE; i++)
	{
		if (g_IrUcSvcInfo[i].UC_SvcHandle == usSCELLHandle && (g_IrUcSvcInfo[i].PidList[i].usPID!=IRUC_INVALID_PID))
			return (IRUC_SVC_INFO *)&(g_IrUcSvcInfo[i]);
	}
	HxLOG_Print("[local_cas_iruc_Drv_FindIrTp] !!! ERROR !!!! \n");
	return NULL;
}


static	IRUC_TP_INFO	*local_cas_iruc_Drv_FindIrTpInfoByUC(HUINT16 usSCELLHandle)
{
	HINT32		i;

	for (i = 0; i < IRUC_HUMAX_MAX_TP; i++)
	{
//		for( j = 0; j < CAS_MAX_NUM_OF_SC; j++ )
		{
			if (g_IrUcTpInfo[i].UC_SvcHandle == usSCELLHandle )
				return (IRUC_TP_INFO*)&(g_IrUcTpInfo[i]);
		}
	}
	HxLOG_Print("[local_cas_iruc_Drv_FindIrTp] !!! ERROR !!!! \n");
	return NULL;
}


void CAS_IRUC_Main_ProcessMsg(HMSG *msg)
{
	HxLOG_Print("[%s] message class  type = 0x%X\n",__FUNCTION__ , msg->usMsgClass );

	switch ( msg->usMsgClass )
	{
	case eIR_MSG_CLASS_CAI:
		local_cas_iruc_Drv_ProcessMsgFromCAI(msg);
		break;

	default:
		HxLOG_Print("[%s] Unkown message type = 0x%X\n",__FUNCTION__ , msg->usMsgClass );
		break;
	}

	/* followings are added, 2008-08-11 */
	if((void*)(msg->ulParam) != NULL)
	{
		OxCAS_Free((void*)(msg->ulParam));
	}
}

#define _______IRUC_STATUS_API_________
/********************************************************************************
********************************************************************************/
HUINT8 CAS_IRUC_DRV_GetInformationOfElementary(HUINT16 usHandle, HUINT32 ulResourceId, IR_ELEM_STATE **stSvcStatusInfoElement)
{
	HUINT8	i = 0, ucCount = 0;

	IRUC_SVC_INFO		*pstIrSvcInfo;
	IR_ELEM_STATE		*pWork;

	pstIrSvcInfo = local_cas_iruc_Drv_FindIrSvcInfoByUC(usHandle);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("not matched service info(0x%X)\n", usHandle );
		return 0;
	}

	ucCount = pstIrSvcInfo->ucStreamCount;
	if( ucCount )
	{
		*stSvcStatusInfoElement = (IR_ELEM_STATE *)OxCAS_Malloc(sizeof(IR_ELEM_STATE) * ucCount);
		if (*stSvcStatusInfoElement == NULL)
		{
			HxLOG_Print(": Mem Alloc Error\n");
			ucCount = 0;
			return ucCount;
		}
		pWork = *stSvcStatusInfoElement;

		for( i = 0; (i < ucCount) && (pstIrSvcInfo->PidList[i].usPID != IRUC_INVALID_PID)&& (pstIrSvcInfo->PidList[i].usPID != 0); i++ )
		{
			pWork[i].usEsPID		= pstIrSvcInfo->PidList[i].usPID;

			#if 1//debug
				HxLOG_Print("\t\t(%d)usEsPID \t\t= 0x%x\n", i, pWork[i].usEsPID);
//				HxLOG_Print("\t\t(%d)usEcmOrEmmPID \t= 0x%x\n", i, pWork[i].usEcmOrEmmPID);
			#endif
		}
	}
	ucCount=i;

	HxLOG_Print("ucCount= %d\n", ucCount);

	return ucCount;
}

/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IRUC_DRV_SendServiceDefineOfEmmMonitor(HUINT16 ushSCELL, HUINT8 bStart)
{
	IRUC_TP_INFO	*pstIrTpInfo;
	HUINT32	i;
	HUINT32	TLVlenth=4;
	HUINT8 pTLV[4] = {0,};

	HxLOG_Print("[CAS_IRUC_DRV_SendServiceDefineOfEmmMonitor] :usSCELLHandle(%x) bStart(%x)\n", ushSCELL, bStart);
	pstIrTpInfo = local_cas_iruc_Drv_FindIrTpInfoByUC(ushSCELL);
	if (pstIrTpInfo == NULL)
	{
		HxLOG_Print("no matched SoftCELL handle!!!\n");
		return HIR_NOT_OK;
	}

	for (i = 0; i < IRUC_HUMAX_MAX_TP; i++)
	{
		HxSTD_memset(&(pstIrTpInfo->UCmonitorInfo), 0, sizeof(uc_convergent_client_service_monitor_status_st));
		#ifdef __WA_EMM_MONITOR_CLOSE_ERR__
			s_EmmMonitorCount[i] = 0;
		#endif
	}

	pTLV[0] = UC_TLV_TAG_FOR_MONITOR_SWITCH;
	pTLV[1] = 0;
	pTLV[2] = 1;
	pTLV[3] = bStart;

	return xsvc_cas_IrUcSetConfigService(ushSCELL,TLVlenth, pTLV);
}

/********************************************************************************
********************************************************************************/
HIR_ERR CAS_IRUC_DRV_SendServiceDefineOfEcmMonitor(HUINT16 ushSCELL, HUINT8 bStart)
{
	HINT32		i;
	HUINT32	TLVlenth=4;
	HUINT8 pTLV[4] = {0,};
	IRUC_SVC_INFO		*pstIrSvcInfo;

	HxLOG_Print(" SCELLHandle (%x) bStart (%x)\n", ushSCELL, bStart);

	pstIrSvcInfo = local_cas_iruc_Drv_FindIrSvcInfoByUC(ushSCELL);
	if (pstIrSvcInfo == NULL)
	{
		HxLOG_Print("no matched SoftCELL handle!!\n");
		return HIR_NOT_OK;
	}

	for (i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++)
	{
			HxSTD_memset(&(pstIrSvcInfo->UCmonitorInfo), 0, sizeof(uc_convergent_client_service_monitor_status_st));
	}

	pTLV[0] = UC_TLV_TAG_FOR_MONITOR_SWITCH;
	pTLV[1] = 0;
	pTLV[2] = 1;
	pTLV[3] = bStart;

	return xsvc_cas_IrUcSetConfigService(ushSCELL,TLVlenth, pTLV);
}




#define _______IRUC_DRV_API_________

HBOOL local_cas_iruc_IS_ECM_SVCCONTEXT(HUINT32 SvcContext)
{
	if(  IRUC_ECM_PREFIX == ( (SvcContext >>24) & 0xFF) )
		return TRUE;

	return FALSE;
}

HBOOL CAS_IRUC_CheckDescramblePid(HUINT32 SvcContext, HUINT16 ulPid)
{
	HUINT32 i, j;

	HxLOG_Print("[%s] Check allocated PID \n",__FUNCTION__);

	if(IRUC_INVALID_UC_SVC_CONTEXT == SvcContext)
	{
		HxLOG_Print("[%s] SvcContext is invalid\n",__FUNCTION__);
		return FALSE;
	}

	if(TRUE != local_cas_iruc_IS_ECM_SVCCONTEXT(SvcContext))
	{
		HxLOG_Print("[%s] It's EMM SvcContext!!!\n",__FUNCTION__);
		return FALSE;
	}

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].UcSvcContext == SvcContext )
		{
			for( j = 0; j < IRUC_SVC_MAX_PID; j++ )
			{
				if(g_IrUcSvcInfo[i].PidList[j].usPID ==ulPid)
				{
					/* 해당 pid는 할당이 되엇다고 넣어 준다. */
					g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc = TRUE;
					HxLOG_Print("Found Component Pid(0x%X) PIDList[%d](0x%X) in SvcInfo\n", ulPid, j,g_IrUcSvcInfo[i].PidList[j].usPID);
					return TRUE;
				}
			}
			HxLOG_Print("[%s] Couldn't find Pid(0x%X)in SvcContext(0x%X)\n",__FUNCTION__, ulPid, SvcContext);
			return FALSE;
		}
	}
	HxLOG_Print("[%s] Couldn't find SvcContext(0x%X)\n",__FUNCTION__, SvcContext);
	return FALSE;
}

HBOOL CAS_IRUC_CheckAllocateDscOtherPid(HUINT32 SvcContext, HUINT16 ulPid)
{

	HUINT32 i, j;
	if(IRUC_INVALID_UC_SVC_CONTEXT == SvcContext)
	{
		HxLOG_Error("[%s] SvcContext is invalid\n",__FUNCTION__ );
		return FALSE;
	}

	if(TRUE != local_cas_iruc_IS_ECM_SVCCONTEXT(SvcContext))
	{
		HxLOG_Error("[%s] It's EMM SvcContext!!!\n", __FUNCTION__ );
		return FALSE;
	}

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].UcSvcContext == SvcContext )
		{
			for( j = 0; j < IRUC_SVC_MAX_PID; j++ )
			{
//				HxLOG_Print("Compare Component Pid(0x%X) PIDList[%d](0x%X), ucAllocatedDsc[%d] \n", ulPid, j,g_IrUcSvcInfo[i].PidList[j].usPID,g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc);
/* 	이전에 사용된 pid이냐
     pid 값이 잇으면 담으로 넘어간다.
*/
				if(g_IrUcSvcInfo[i].PidList[j].usPID == ulPid && (g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc == TRUE))
				{
					HxLOG_Print("[%s] Found Other Component Pid(0x%X) PIDList[%d](0x%X) in SvcInfo \n", __FUNCTION__, ulPid, j, g_IrUcSvcInfo[i].PidList[j].usPID);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

HBOOL CAS_IRUC_SetOtherDescramblePid(HUINT32 SvcContext, HUINT16 ulPid, HUINT8 *cComponentCnt, 	HUINT32 *ulDscIndex, HUINT16 *usOtherPidList)
{
	HUINT32 i,j;
	HUINT32 ulDmxID;
	Handle_t hAct;
	HUINT8 ucOtherComponntCnt=0;
	HUINT32 ulDscIdx=0;
	// humax side 등록 되어잇는 pid 에대해서 descrambler open 한다.
	HxLOG_Print("[CAS_IRUC_SetOtherDescramblePid] descrambler open for Other PID \n");

	if(TRUE != local_cas_iruc_IS_ECM_SVCCONTEXT(SvcContext))
	{
		HxLOG_Error("[%s] It's EMM SvcContext!!!\n", __FUNCTION__ );
		return FALSE;
	}

	if(ERR_OK != CAS_IRUC_GetActHandDmxIDbySvcContext(SvcContext,&hAct, &ulDmxID) )
	{
		HxLOG_Error("[%s] Cannot get Dmx ID\n", __FUNCTION__ );
		return FALSE;
	}

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].UcSvcContext == SvcContext)
		{
			for( j = 0; j < IRUC_SVC_MAX_PID; j++)
			{
//				HxLOG_Print("[CAS_IRUC_SetOtherDescramblePid] check other Pid(0x%X) PIDList[%d](0x%X), ucAllocatedDsc[%d] \n", ulPid, j,g_IrUcSvcInfo[i].PidList[j].usPID,g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc);

/* Comment :  이미 Descrambler가 open이 되엇으면 pass */
				if(g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc == TRUE && g_IrUcSvcInfo[i].PidList[j].usPID == ulPid)
				{
					/* 이미 descrmabler open 되어서 pass 한다. */
					HxLOG_Print("[CAS_IRUC_SetOtherDescramblePid] This PID already set DSC Pid(0x%X) PIDList[%d](0x%X), ucAllocatedDsc[%d] \n", ulPid, j, g_IrUcSvcInfo[i].PidList[j].usPID, g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc);
					continue;
				}

				if(g_IrUcSvcInfo[i].PidList[j].usPID != IRUC_INVALID_PID)
				{
					if(ERR_OK != svc_cas_DevDscOpen(ulDmxID,  ePAL_DSC_DSCTYPE_DVB, ePAL_DSC_DATATYPE_PES, &ulDscIdx ) )
					{
						HxLOG_Error("[%s] Cannot open descrambler of other PID \n", __FUNCTION__ );
					}

					if(ERR_OK != svc_cas_DevDscSetPid(ulDscIdx, g_IrUcSvcInfo[i].PidList[j].usPID))
					{
						HxLOG_Error("[%s] Cannot set other PID with descrambler\n", __FUNCTION__ );
					}

					g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc = TRUE;
					usOtherPidList[ucOtherComponntCnt] = g_IrUcSvcInfo[i].PidList[j].usPID;
					ulDscIndex[ucOtherComponntCnt] = ulDscIdx ;
					ulDscIdx=0;
					HxLOG_Print("[%s] Other PID Descrambler setting is OK find Component Pid(0x%X)in SvcContext(0x%04X) , DscIndex (0x%X) \n", __FUNCTION__, g_IrUcSvcInfo[i].PidList[j].usPID, SvcContext, ulDscIdx);
					ucOtherComponntCnt ++;
				}
			}
		}
	}

	*cComponentCnt = ucOtherComponntCnt;
	if(ucOtherComponntCnt > 0) return TRUE;
	else return FALSE;

}

HBOOL CAS_IRUC_ResetOtherDescramblePid(HUINT32 SvcContext, HUINT16 ulPid)
{
	HINT32 i,j;

// humax side 등록 되어잇는 pid 에대해서 descrambler open 한다.
	if(TRUE != local_cas_iruc_IS_ECM_SVCCONTEXT(SvcContext))
	{
		HxLOG_Error("[%s] It's EMM SvcContext!!!\n", __FUNCTION__ );
		return FALSE;
	}

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].UcSvcContext == SvcContext )
		{
			for( j = 0; j < IRUC_SVC_MAX_PID; j++ )
			{
//				HxLOG_Print("[%s] Remove other PID of humax side Compare Component Pid(0x%X) PIDList[%d](0x%X) \n", __FUNCTION__, ulPid, j, g_IrUcSvcInfo[i].PidList[j].usPID);
				if((g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc == TRUE) && (g_IrUcSvcInfo[i].PidList[j].usPID == ulPid))
				{
					/* 이미 descrmabler open 되어서 pass 한다. */
					g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc = FALSE;
					HxLOG_Print("[%s] Reset SvcContext(0x%X) PID(0x%X)  ucAllocatedDsc(%d) \n", __FUNCTION__, SvcContext, g_IrUcSvcInfo[i].PidList[j].usPID, g_IrUcSvcInfo[i].PidList[j].ucAllocatedDsc);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


HINT32 CAS_IRUC_GetActHandDmxIDbySvcContext(HUINT32 SvcContext, Handle_t *pActHOut, HUINT32 *pulDmxIdOut)
{
	// TODO: USE MUTEX.
	HINT32 i;

	if(IRUC_INVALID_UC_SVC_CONTEXT == SvcContext)
	{
		HxLOG_Print("SvcContext is invalid\n");
		return ERR_FAIL;
	}

	if(TRUE == local_cas_iruc_IS_ECM_SVCCONTEXT(SvcContext))
	{
		for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
		{
			if( g_IrUcSvcInfo[i].UcSvcContext == SvcContext )
			{
				*pActHOut = g_IrUcSvcInfo[i].hAct ;
				*pulDmxIdOut = g_IrUcSvcInfo[i].ulDemuxId;

//				HxLOG_Print("Found UcSvcContext in UcDrv SVC Info in index(%d)\n", i );
				return ERR_OK;
			}
		}
	}
	else // for emm.
	{
		for( i = 0; i < IRUC_HUMAX_MAX_TP; i++ )
		{
			if( g_IrUcTpInfo[i].UcSvcContext == SvcContext )
			{
				*pActHOut = g_IrUcTpInfo[i].hAction ;
				*pulDmxIdOut = g_IrUcTpInfo[i].ulDemuxId;

//				HxLOG_Print("Found UcSvcContext in UcDrv TP Info in index(%d)\n", i );
				return ERR_OK;
			}
		}
	}
	HxLOG_Print("Cannot find UcSvcContext in All table" );
	return ERR_FAIL;
}

void CAS_IRUC_DRV_Init(void)
{
	HUINT32	i;

#if 1
    if( ERR_OK != xsvc_cas_IrLoaderInit() )
	{
		HxLOG_Print("[%s] read loader info error!!\n",__FUNCTION__);
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}

#else
// hjlee3 모지?? 확인해보자.
	// Irdeto common loader initialize.
	SvcCas_CtrlParam_t				stIrCtrlParam;

	stIrCtrlParam.ulControlType		= eCACTRL_IR_LOADER_Init;
	stIrCtrlParam.pvIn				= NULL;

	if( ERR_OK != CASMGR_CTRL_Call(eDxCAS_GROUPID_IR, eCACTRL_IR_LOADER_Init, stIrCtrlParam))
	{
		HxLOG_Print("read loader info error!!\n");
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}
#endif

	HxLOG_Print("\n");
	if(ERR_OK != CAS_IRUC_SPI_DEVICE_LoadAllUniqueIdentifers() )

	{
		HxLOG_Print("read irUC manufactorer indentifers' info error!!\n");
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		local_cas_iruc_Drv_ClearServiceInfo(i);
	}

	for( i = 0; i < IRUC_HUMAX_MAX_TP; i++ )
	{
		local_cas_iruc_Drv_ClearTpInfo(&g_IrUcTpInfo[i]);
	}
	HxLOG_Print("\n");

	CAS_IRUC_SPI_STREAM_Init();
	CAS_IRUC_SPI_PS_Init();
	CAS_IRUC_SPI_TIMER_Init();

	if (UC_ERROR_SUCCESS != CAS_IRUC_SPI_SC_Init()) {
		HxLOG_Print("CAS_IRUC_SPI_SC_Init: ERROR!!!\n");
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
	}
	CAS_IR_MSG_DRV_Initialise();

	HxLOG_Print("-\n");
}

//#define __TESTCODEFORNONSECURE__
void CAS_IRUC_Client_Init(void)
{
	uc_result result = UC_ERROR_SUCCESS;
	HUINT16 ret;
	uc_ca_client_config pConfigInfo;
	uc_unified_client_config *pUconfiginfo=NULL;
	HxLOG_Print("[%s] UC Init -- START\n",__FUNCTION__);


	if( VK_OK != VK_SEM_Create(&s_ulUCCAAPISema, "UCCAAPISema", VK_SUSPENDTYPE_FIFO) )
	{
		HxLOG_Print("[%s] The Client inital Semaphore ERR \n",__FUNCTION__);
		return ;
	}

// rm /var/lib/humaxtv/cas/iruc_backup/*
// rm -rf /var/lib/humaxtv/cas/*
// ls -al /var/lib/humaxtv/cas/irUC
// ls -al /var/lib/humaxtv/cas/iruc_backup

#if defined(__TESTCODEFORNONSECURE__)
/* test code for flexiflash, remove cas area */
 	ret = VK_SYSTEM_Command("rm -Rf /var/lib/humaxtv/cas/*");
	if(ret == 0 ) HxLOG_Print(" rm OK , /var/lib/humaxtv/cas/* \n ");
	else HxLOG_Print("  rm Fail ,  /var/lib/humaxtv/cas/* \n");

#else
	HxLOG_Print("Copy IrUC_PSFile_0 and IrUC_PSFile_9 /var/lib/humaxtv/cas/irUC/ \n");

	if(svc_cas_DevFsIsDirExist("/var/lib/humaxtv/cas/irUC") != TRUE)
	{
		ret = VK_SYSTEM_Command("mkdir -m 777 /var/lib/humaxtv/cas/irUC");
		if(ret == 0 ) HxLOG_Print(" mkdir OK , /var/lib/humaxtv/cas/irUC \n ");
		else HxLOG_Print("  mkdir Fail ,  /var/lib/humaxtv/cas/irUC \n");
	}

#if 0
// E600이 되게 한다. 지우지 않는다.
// 지워서 문제 가 된다면 PS1과 PS_9 만 지워야 한다.
	else
	{
		ret = VK_SYSTEM_Command("rm /var/lib/humaxtv/cas/irUC/* ");
		if(ret == 0 ) HxLOG_Print("  Delete All file in irUC Folder => OK  \n ");
		else HxLOG_Print(" Delete All file in irUC Folder ==> Fail \n");
	}
#endif

	ret = VK_SYSTEM_Command("cp /opt/share/IrUC_PSFile_9 /var/lib/humaxtv/cas/irUC/");
	if(ret == 0 ) HxLOG_Print(" cp OK , /opt/share/IrUC_PSFile_9 \n ");
	else HxLOG_Print(" cp Fail , /opt/share/IrUC_PSFile_9 \n ");

	ret = VK_SYSTEM_Command("cp /opt/share/IrUC_PSFile_1 /var/lib/humaxtv/cas/irUC/");
	if(ret == 0 ) HxLOG_Print(" cp OK , /opt/share/IrUC_PSFile_1 to /var/lib/humaxtv/cas/irUC/ \n ");
	else HxLOG_Print(" cp Fail , /opt/share/IrUC_PSFile_1 \n ");

	if(svc_cas_DevFsIsFileExist("/var/lib/humaxtv/cas/iruc_backup/IrUC_PSFile_1") == TRUE)
	{
		ret = VK_SYSTEM_Command("rm /var/lib/humaxtv/cas/irUC/IrUC_PSFile_1");
		if(ret == 0 ) HxLOG_Print(" rm irUC/IrUC_PSFile_1 \n ");
		else HxLOG_Print(" cp Fail , irUC/IrUC_PSFile_1\n ");


		ret = VK_SYSTEM_Command("cp /var/lib/humaxtv/cas/iruc_backup/IrUC_PSFile_1 /var/lib/humaxtv/cas/irUC/");
		if(ret == 0 ) HxLOG_Print(" cp OK , iruc_backup/IrUC_PSFile_1\n ");
		else HxLOG_Print(" cp Fail , iruc_backup/IrUC_PSFile_1 \n ");
	}

	ret = VK_SYSTEM_Command("chmod 777 /var/lib/humaxtv/cas/irUC/*");
	if(ret == 0 ) HxLOG_Print(" chmod 777 OK , IrUC_PSFile_1, IrUC_PSFile_9 \n ");
	else HxLOG_Print(" chmod 777 Fail, IrUC_PSFile_1, IrUC_PSFile_9 \n ");
#endif

	CAS_IRUC_SPI_OS_Init();

	HxLOG_Print("[%s] [UniversalClient_StartCaClient] 0x%x \n",__FUNCTION__, result);
	result = UniversalClient_StartCaClient(local_cas_iruc_UC_GlobalMsg_ProcCallback);
	if(result == 0)
	{
		HxLOG_Print("[%s] The Client inital OK~ result 0x%x\n",__FUNCTION__, result);
	}
	else
	{
		HxLOG_Print("[%s]  ERR ERR ERR : The Client initial FAIL!!! (0x%x)\n",__FUNCTION__, result);
	}

/* hjlee3,	configClient 를 type B 로 설정 */
	pUconfiginfo=(uc_unified_client_config *)OxCAS_Malloc(sizeof(uc_unified_client_config));
	if(pUconfiginfo!=NULL)
	{

		pUconfiginfo->isCloakedCAWithHighPriority = UC_FALSE;
		pUconfiginfo->isAutoClientSwitching =  UC_FALSE; //UC_TRUE UC_FALSE
		pUconfiginfo->irdMessageSource = UC_SOURCE_BOTH;
		pConfigInfo.pConfigData = (void *)pUconfiginfo;

	}

	pConfigInfo.configType = UC_CONFIG_TYPE_FOR_UNIFIED_CLIENT;
	result = UniversalClient_ConfigClient(&pConfigInfo);
	if(result != 0)
	{
		HxLOG_Print("[%s]  == Set ConfigClient Fail ==  (0x%x) \n",__FUNCTION__, result);
	}

	if(pUconfiginfo!=NULL)
	{
		OxCAS_Free((void*)pUconfiginfo);
		pUconfiginfo = NULL;
	}

	HxLOG_Print("[%s] UC Init -- END\n", __FUNCTION__);

	return;
}

void CAS_IRUC_Client_Terminate(void)
{
//	UserPowersOffDevice();
}


void CAS_IRUC_DRV_Client_Notify_PMT(uc_service_handle UC_SvcHandle, HUINT8 *pRawPmt, HUINT16 usPmtSize )
{
	uc_buffer_st	bytes;  // this 'bytes' structure is the standard way to pass around blocks of data with length.
	uc_result result;

    HxLOG_Print("[%s] #### UC notify PMT UC_SvcHandle (0x%x) \n",__FUNCTION__,UC_SvcHandle);
	HxLOG_Print("[%s] pRawPmt = 0x%X, usPmtSize = %d \n", __FUNCTION__,(HINT32) pRawPmt, usPmtSize );

	bytes.bytes = pRawPmt;
	bytes.length = usPmtSize;

	result = UniversalClient_DVB_NotifyPMT(UC_SvcHandle,&bytes);

	if(UC_ERROR_SUCCESS != result)
	{
		HxLOG_Print("[%s] UniversalClient_DVB_Notify PMT : Error, result = 0x%X \n\n",__FUNCTION__, result );
	}
	else
	{
		HxLOG_Print("[%s] UniversalClient_DVB_Notify PMT : OK \n\n",__FUNCTION__ );
	}


}

void CAS_IRUC_DRV_Client_Notify_CAT(uc_service_handle UC_SvcHandle,HUINT8 *pRawCat, HUINT16 usCatSize )
{
	uc_buffer_st bytes;  // this 'bytes' structure is the standard way to pass around blocks of data with length.
	uc_result result;

	HxLOG_Print("[%s] #### UC notify CAT UC_SvcHandle (0x%x)\n",__FUNCTION__, UC_SvcHandle);
	HxLOG_Print("[%s] pRawCat = 0x%X, usCatSize = %d\n",__FUNCTION__, (HINT32) pRawCat, usCatSize );

	bytes.bytes = pRawCat;
	bytes.length = usCatSize;

	result = UniversalClient_DVB_NotifyCAT(UC_SvcHandle,&bytes);

	if(UC_ERROR_SUCCESS != result)
	{
		HxLOG_Print("[%s]  UniversalClient_DVB_NotifyCAT : Error, result = 0x%X \n\n", __FUNCTION__, result );
	}
	else
	{
		HxLOG_Print("[%s]  UniversalClient_DVB_NotifyCAT : OK \n\n",__FUNCTION__ );
	}
}

HINT32 CAS_IRUC_DRV_Open_UC_Svc(HUINT32 UcSvcContext, uc_service_handle * in_pUCSvcHandle)
{
	uc_result result;

	HxLOG_Print("#### UCCA Service OPEN  = \n");

	result = UniversalClient_OpenService(
					(void *) UcSvcContext,
					// pServiceContext -- normally we could use this field to identify the context
					// for the stream open parameters (uc_stream_open_params). For example purposes,
					// this is a hardcoded value.
					local_cas_iruc_UC_SVC_ProcCallback,
					(void *)UcSvcContext,
					// pMessageProcData -- we can use this to identify which service we're receiving
					// a message for. For example purposes, this is a hardcoded value.
					in_pUCSvcHandle );

	HxLOG_Print(" #### UC OPEN UcSvcContext = 0x%X, pUCSvcHandle(%d), result(%d) \n", UcSvcContext, *in_pUCSvcHandle, result);

	return result;
}

HINT32 CAS_IRUC_DRV_Close_UC_Svc(uc_service_handle *in_pUCSvcHandle)
{
	uc_result result;

	HxLOG_Print("[%s] SvcHandle= 0x%X\n", __FUNCTION__, *in_pUCSvcHandle );

	result = UniversalClient_CloseService(in_pUCSvcHandle );

	HxLOG_Print("[%s] #### UC CLOSE , result = 0x%x \n\n", __FUNCTION__, result);

	return result;
}

#define _______IRUC_SERVICE_STATUS_API_________
/********************************************************************************
********************************************************************************/
HUINT16 CAS_IRUC_GetStatusCode(char *pstrError)
{
	return local_cas_iruc_GetStatusCode(pstrError);
}

void xsvc_cas_IrUcInitStreamStatus(void)
{
	HUINT32 i;

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		g_IrUcSvcInfo[i].UCStreamStatusInfo = NULL;
	}

	for( i = 0; i < IRUC_HUMAX_MAX_TP; i++ )
	{
		g_IrUcTpInfo[i].UCStreamStatusInfo = NULL;
	}
}

void xsvc_cas_IrUcFreeStreamStatus(void)
{
	HUINT32 i;
	for(i=0 ; i < IRUC_HUMAX_MAX_SERVICE ; i++)
	{
		if(g_IrUcSvcInfo[i].UCStreamStatusInfo)
		{
			UniversalClient_FreeStreamStatus(&g_IrUcSvcInfo[0].UCStreamStatusInfo);
			g_IrUcSvcInfo[i].UCStreamStatusInfo = NULL;
			HxLOG_Print("==> ECM Free stream status info \n");

		}
	}

	for(i=0 ; i < IRUC_HUMAX_MAX_TP ; i++)
	{
		if(g_IrUcTpInfo[i].UCStreamStatusInfo)
		{
			UniversalClient_FreeStreamStatus(&g_IrUcTpInfo[0].UCStreamStatusInfo);
			g_IrUcTpInfo[i].UCStreamStatusInfo = NULL;
			HxLOG_Print("==> ECM Free stream status info \n");
		}
	}
}

HINT32 xsvc_cas_IrUcGetStatusService(HUINT8 *pucTotalUcSvcCnt, IRUC_SERVICE_STATUS **ppSvcStatus_shallbefree )
{

	HUINT32 ulServiceCnt, i, nValidUcSvcCnt=0, ulComponentindex=0;
	uc_result			uc_ret = UC_ERROR_SUCCESS;
	IRUC_SERVICE_STATUS *astIrucSvcStatus = NULL;
	uc_convergent_client_service_status_st buffConvCSstatus;
	HUINT8	ucTotalCount=0,ucEcmCnt=1,ucEmmCnt=1;
	HUINT32	ulStreamIndex=0;

	VK_SEM_Get(s_ulUCCAAPISema);

	memset(&buffConvCSstatus, 0x00, sizeof(uc_convergent_client_service_status_st));
	HxLOG_Print("\n");

	ulServiceCnt = IRUC_HUMAX_MAX_SERVICE+IRUC_HUMAX_MAX_TP;
    	ulServiceCnt++;

	astIrucSvcStatus = OxCAS_Malloc(ulServiceCnt*sizeof(IRUC_SERVICE_STATUS));

	if (astIrucSvcStatus == NULL)
	{
		HxLOG_Error("Fail Mem allocation !!\n");
		goto FUNC_OUT;
	}

	for(i= 0; i< IRUC_HUMAX_MAX_SERVICE ; i++ )
	{
		g_IrUcSvcInfo[i].ulStreamCount = 0;
/* Descramble ECM*/
		if(g_IrUcSvcInfo[i].UC_SvcHandle != IRUC_INVALID_UC_SVCHANDLE)
		{
/* ecm pid, status 정보 저장. */
			HxLOG_Print("\n[%s] ECM: Svchandle(0x%x)\n",__FUNCTION__, g_IrUcSvcInfo[i].UC_SvcHandle);
			uc_ret = UniversalClient_GetStreamStatus(g_IrUcSvcInfo[i].UC_SvcHandle,&g_IrUcSvcInfo[i].ulStreamCount,&g_IrUcSvcInfo[i].UCStreamStatusInfo);
			if(uc_ret != UC_ERROR_SUCCESS || g_IrUcSvcInfo[i].ulStreamCount == 0 || g_IrUcSvcInfo[i].UCStreamStatusInfo == NULL)
			{
				//UniversalClient_FreeStreamStatus(&g_IrUcSvcInfo[i].UCStreamStatusInfo);
				HxLOG_Print("[%s] ==> NOT FOUND ECM stream status info ",__FUNCTION__);
				g_IrUcSvcInfo[i].ulStreamCount = 0; // SUCCESS가 아닐 경우 배열의 Count는 무조건 0로 하여, count때문에 발생할 수 있는 문제를 없앤다.
				//goto FUNC_OUT;	/* FTA Service에서도 보여주기 ECM/EMM service menu를 보여주기 위해 */
			}

			/* mult-stream for ECM 지원 */
			HxLOG_Print("\t ECM: Num of Stream = %d, \n", g_IrUcSvcInfo[i].ulStreamCount);
			astIrucSvcStatus[nValidUcSvcCnt].ucStreamCnt = g_IrUcSvcInfo[i].ulStreamCount;
			for(ulStreamIndex=0; ulStreamIndex<g_IrUcSvcInfo[i].ulStreamCount; ulStreamIndex++)
			{
				HxLOG_Print("\t ECM:%d Status = %s, \n", ulStreamIndex, g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage);
                                HxSTD_MemCopy(astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].aucStatusString, g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage, UC_CONNECTION_STATUS_STRING_SIZE);

                                HxLOG_Print("\t ECM:%d ECMPID = 0x%04X, \n", ulStreamIndex, g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid);
				astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].EcmEmmPid = g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid;

				 HxLOG_Print("\t ECM: Num of ES PID: %d\n", g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount);
				astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].ucComponentCnt = g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount;
				for(ulComponentindex=0 ; ulComponentindex < g_IrUcSvcInfo[i].UCStreamStatusInfo->componentCount; ulComponentindex++)
				{
					 HxLOG_Print("\t Status ES PIDList: 0x%04x \n", g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray[ulComponentindex].pid);
					astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex]=g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray[ulComponentindex].pid;
				}
			}

			astIrucSvcStatus[nValidUcSvcCnt].bCCA = g_IrUcSvcInfo[i].bCCA ;
			astIrucSvcStatus[nValidUcSvcCnt].hUC_SVC = g_IrUcSvcInfo[i].UC_SvcHandle;

			uc_ret = UniversalClient_GetServiceStatus( astIrucSvcStatus[nValidUcSvcCnt].hUC_SVC,
													(uc_byte *)&astIrucSvcStatus[nValidUcSvcCnt].aucSVCStatusString );
			astIrucSvcStatus[nValidUcSvcCnt].aucSVCStatusString[UC_CONNECTION_STATUS_STRING_SIZE-1] = '\0';
			astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[0].aucStatusString[UC_CONNECTION_STATUS_STRING_SIZE-1] = '\0';
			HxLOG_Print(" Status: [%s][%s] \n", astIrucSvcStatus[nValidUcSvcCnt].aucSVCStatusString, astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[0].aucStatusString);

			if(uc_ret == UC_ERROR_SUCCESS)
			{
				astIrucSvcStatus[nValidUcSvcCnt].bValid =TRUE;
				astIrucSvcStatus[nValidUcSvcCnt].bEcmType = TRUE;

/* 여기서 hardcoding 해도 상관은 없을거 같다. */
				astIrucSvcStatus[nValidUcSvcCnt].eSvcType = g_IrUcSvcInfo[i].svcType;
				HxLOG_Print(" ECM:SoftCell Valid[0x%02x:0x%02x] \n",g_IrUcSvcInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.valid, g_IrUcSvcInfo[i].UCmonitorInfo.softcellMonitorInfo.valid);
				if(g_IrUcSvcInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.valid == TRUE)
				{
					// CCA ECM
					astIrucSvcStatus[nValidUcSvcCnt].eMonitorSvcType = g_IrUcSvcInfo[i].UCmonitorInfo.serviceType;
					astIrucSvcStatus[nValidUcSvcCnt].bMonitoringValid = TRUE;
					HxSTD_memcpy(astIrucSvcStatus[nValidUcSvcCnt].aucMonitormessage, g_IrUcSvcInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.message, UC_MONITOR_MSG_LENGTH);
					HxLOG_Print("[%s] ECM CCA Monitor [%s] \n",__FUNCTION__,g_IrUcSvcInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.message);

				}
				else if(g_IrUcSvcInfo[i].UCmonitorInfo.softcellMonitorInfo.valid == TRUE)
				{
					// SoftCell ECM
					astIrucSvcStatus[nValidUcSvcCnt].eMonitorSvcType = g_IrUcSvcInfo[i].UCmonitorInfo.serviceType;
					astIrucSvcStatus[nValidUcSvcCnt].bMonitoringValid = TRUE;
					HxSTD_memcpy(astIrucSvcStatus[nValidUcSvcCnt].aucMonitormessage, g_IrUcSvcInfo[i].UCmonitorInfo.softcellMonitorInfo.message, UC_MONITOR_MSG_LENGTH);
					HxLOG_Print("[%s] ECM SoftCell Monitor [%s] \n",__FUNCTION__,g_IrUcSvcInfo[i].UCmonitorInfo.softcellMonitorInfo.message);
				}

				astIrucSvcStatus[nValidUcSvcCnt].ucInstanceCount=ucEcmCnt;

				ucEcmCnt++;
				nValidUcSvcCnt++; // 0:ECM, 1:EMM for CCA, 2:EMM for Softcell
				ucTotalCount ++;
			}
			HxLOG_Print("[%s]==> ECM count 0x%08x:%d \n",__FUNCTION__,astIrucSvcStatus[nValidUcSvcCnt-1].hUC_SVC, nValidUcSvcCnt);

		}
	}

	HxLOG_Print("[%s] ==>  EMM START 0 \n",__FUNCTION__);


	for(i= 0; i< IRUC_HUMAX_MAX_TP ; i++ )
	{
		HxLOG_Print("[%s]==>  EMM START 1  \n",__FUNCTION__);
		g_IrUcTpInfo[i].ulStreamCount = 0;
		if(g_IrUcTpInfo[i].UC_SvcHandle != IRUC_INVALID_UC_SVCHANDLE)
		{

/* emm pid, status 정보 저장.*/
			HxLOG_Print("[%s] ==>  EMM START 2  \n",__FUNCTION__);
			uc_ret = UniversalClient_GetStreamStatus(g_IrUcTpInfo[i].UC_SvcHandle,&g_IrUcTpInfo[i].ulStreamCount,&g_IrUcTpInfo[i].UCStreamStatusInfo);
			if(uc_ret != UC_ERROR_SUCCESS || g_IrUcTpInfo[i].ulStreamCount == 0)
			{
				//UniversalClient_FreeStreamStatus(&g_IrUcTpInfo[i].UCStreamStatusInfo);
				HxLOG_Print("[%s] ==> NOT FOUND EMM stream status info ",__FUNCTION__);
				g_IrUcTpInfo[i].ulStreamCount=0;// SUCCESS가 아닐 경우 배열의 Count는 무조건 0로 하여, count때문에 발생할 수 있는 문제를 없앤다.
				//goto FUNC_OUT;	/* FTA Service에서도 보여주기 ECM/EMM service menu를 보여주기 위해 */
			}

			/* mult-stream for EMM 지원 */
			HxLOG_Print("EMM: Num of Stream = %d, \n", g_IrUcTpInfo[i].ulStreamCount);
			astIrucSvcStatus[nValidUcSvcCnt].ucStreamCnt = g_IrUcTpInfo[i].ulStreamCount;
			for(ulStreamIndex=0; ulStreamIndex<g_IrUcTpInfo[i].ulStreamCount; ulStreamIndex++)
			{
				HxLOG_Print("EMM: %d Status = %s, \n", ulStreamIndex, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage);
				HxSTD_memcpy (astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].aucStatusString, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage, UC_CONNECTION_STATUS_STRING_SIZE);

				HxLOG_Print("Status CCA EMM PID: 0x%04x \n",g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid);
				astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].EcmEmmPid = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid;


				HxLOG_Print("EMM: Num of ES PID: %d\n", g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount);
				astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].ucComponentCnt = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount;

				if (g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount && g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray != NULL)
				{
					for(ulComponentindex=0 ; ulComponentindex < g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount; ulComponentindex++)
					{
						astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex] = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray[ulComponentindex].pid;
						HxLOG_Print("[%s]  Status CCA ES PID: 0x%04x \n",__FUNCTION__,astIrucSvcStatus[i].stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex]);
					}
				}

			}

			/* CCA or SoftCell 구분 */
			astIrucSvcStatus[nValidUcSvcCnt].bCCA = g_IrUcSvcInfo[i].bCCA ; // ECM에서  CCA과 Softcell 구분 할 수 있도록 하기 때문에, SvcInfo룰 사용
			astIrucSvcStatus[nValidUcSvcCnt].hUC_SVC = g_IrUcTpInfo[i].UC_SvcHandle ;
			uc_ret = UniversalClient_GetConvergentServiceStatus( astIrucSvcStatus[nValidUcSvcCnt].hUC_SVC, &buffConvCSstatus);

			HxLOG_Print("[%s] EMM: CCA hUC_SVC:%08x, CloakedCA_status[%s] \n",__FUNCTION__,astIrucSvcStatus[nValidUcSvcCnt].hUC_SVC, buffConvCSstatus.cloakedCAStatus.statusMessage);
			if(uc_ret == UC_ERROR_SUCCESS)
			{
				/* for CCA */  /* multi stream으로 지원 ... */
				///* [Service Status]: from UniversalClient_GetConvergentServiceStatus() */
				memcpy(astIrucSvcStatus[nValidUcSvcCnt].aucSVCStatusString, buffConvCSstatus.cloakedCAStatus.statusMessage, UC_CONNECTION_STATUS_STRING_SIZE);
				astIrucSvcStatus[nValidUcSvcCnt].hUC_internalSVC = buffConvCSstatus.cloakedCAStatus.internalServiceHandle;
				astIrucSvcStatus[nValidUcSvcCnt].bValid		= buffConvCSstatus.cloakedCAStatus.isValid;
				astIrucSvcStatus[nValidUcSvcCnt].bEcmType	= FALSE;
				astIrucSvcStatus[nValidUcSvcCnt].bCCA		= TRUE;
				astIrucSvcStatus[nValidUcSvcCnt].ucInstanceCount = nValidUcSvcCnt;
				astIrucSvcStatus[nValidUcSvcCnt].ucInstanceCount=ucEmmCnt;
/* 여기서 hardcoding 해도 상관은 없을거 같다. */
				astIrucSvcStatus[nValidUcSvcCnt].eSvcType = g_IrUcTpInfo[i].svcType;
				HxLOG_Print("[%s]  EMM:SoftCell Valid[0x%02x:0x%02x] \n",__FUNCTION__,g_IrUcTpInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.valid, g_IrUcTpInfo[i].UCmonitorInfo.softcellMonitorInfo.valid);

				if(g_IrUcTpInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.valid == TRUE)
				{
					// ECM 인지 EMM 인지 구분은 할 수 잇지만 display를 하는 부분에서 구분해야 할 것으로 보인다.
					astIrucSvcStatus[nValidUcSvcCnt].eMonitorSvcType = g_IrUcTpInfo[i].UCmonitorInfo.serviceType;
					astIrucSvcStatus[nValidUcSvcCnt].bMonitoringValid = TRUE;
					HxSTD_memcpy(astIrucSvcStatus[nValidUcSvcCnt].aucMonitormessage, g_IrUcTpInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.message, UC_MONITOR_MSG_LENGTH);
					HxLOG_Print("[%s] EMM CCA Monitor [%s] \n",__FUNCTION__,g_IrUcTpInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.message);
				}

//				HxLOG_Print("==> EMM count for CCA:   %08x:%d:%d:%d:%d \n",astIrucSvcStatus[nValidUcSvcCnt].hUC_internalSVC,
//										astIrucSvcStatus[nValidUcSvcCnt].bValid, astIrucSvcStatus[nValidUcSvcCnt].bEcmType,
//										astIrucSvcStatus[nValidUcSvcCnt].bCCA, nValidUcSvcCnt);
				nValidUcSvcCnt++;

/* save for Smart */
///multi - stream 지원
				HxLOG_Print("Save for Smart\n");
				HxLOG_Print("EMM: Num of Stream = %d, \n", g_IrUcTpInfo[i].ulStreamCount);
				astIrucSvcStatus[nValidUcSvcCnt].ucStreamCnt = g_IrUcTpInfo[i].ulStreamCount;
				for(ulStreamIndex=0; ulStreamIndex<g_IrUcTpInfo[i].ulStreamCount; ulStreamIndex++)
				{
					/* 스트림 정보 저장 */
					HxLOG_Print("[%s] EMM: %d Status = %s, \n", __FUNCTION__, ulStreamIndex, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage);
					HxSTD_memcpy (astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].aucStatusString, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage, UC_CONNECTION_STATUS_STRING_SIZE);

					HxLOG_Print("[%s] Status CCA EMM PID: 0x%04x \n", __FUNCTION__, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid);
					astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].EcmEmmPid = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid;

					HxLOG_Print("[%s] EMM: Num of ES PID: %d\n", __FUNCTION__, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount);
					astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].ucComponentCnt = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount;
					/* 컴포넌드 ES 정보 저장 */
					if (g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount && g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray != NULL)
					{
						for(ulComponentindex=0; ulComponentindex<g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount;ulComponentindex++)
						{
							astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex] = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray[ulComponentindex].pid;
							HxLOG_Print("[%s] Status CCA ES PID: 0x%04x \n", __FUNCTION__, astIrucSvcStatus[nValidUcSvcCnt].stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex]);
						}
					}
				}

				/* CCA or SoftCell 구분 */
				astIrucSvcStatus[nValidUcSvcCnt].bCCA = g_IrUcSvcInfo[i].bCCA ;
				/* for smart card / softcell */
				astIrucSvcStatus[nValidUcSvcCnt].hUC_SVC = g_IrUcTpInfo[i].UC_SvcHandle ;
				/* [Global Service Status]: from UniversalClient_GetConvergentServiceStatus() */
				memcpy(astIrucSvcStatus[nValidUcSvcCnt].aucSVCStatusString, buffConvCSstatus.softcellStatus.statusMessage, UC_CONNECTION_STATUS_STRING_SIZE);
				HxLOG_Print("[%s] EMM: SoftCell svchandles: %08x SoftCell_status[%s] \n",__FUNCTION__, g_IrUcTpInfo[i].UC_SvcHandle, buffConvCSstatus.softcellStatus.statusMessage);
				astIrucSvcStatus[nValidUcSvcCnt].hUC_internalSVC = buffConvCSstatus.softcellStatus.internalServiceHandle;
				astIrucSvcStatus[nValidUcSvcCnt].bValid		= buffConvCSstatus.softcellStatus.isValid;
				astIrucSvcStatus[nValidUcSvcCnt].bEcmType = FALSE;
				astIrucSvcStatus[nValidUcSvcCnt].bCCA		= FALSE;
				astIrucSvcStatus[nValidUcSvcCnt].ucInstanceCount=ucEmmCnt;
/* 여기서 hardcoding 해도 상관은 없을거 같다. */
				astIrucSvcStatus[nValidUcSvcCnt].eSvcType = g_IrUcTpInfo[i].svcType;

				if(g_IrUcTpInfo[i].UCmonitorInfo.softcellMonitorInfo.valid == TRUE)
				{
					astIrucSvcStatus[nValidUcSvcCnt].eMonitorSvcType = g_IrUcTpInfo[i].UCmonitorInfo.serviceType;
					astIrucSvcStatus[nValidUcSvcCnt].bMonitoringValid = TRUE;
					HxSTD_memcpy(astIrucSvcStatus[nValidUcSvcCnt].aucMonitormessage, g_IrUcTpInfo[i].UCmonitorInfo.softcellMonitorInfo.message, UC_MONITOR_MSG_LENGTH);
					HxLOG_Print("[%s] EMM SoftCell Monitor aucStatusString[%s] \n",__FUNCTION__,g_IrUcTpInfo[i].UCmonitorInfo.softcellMonitorInfo.message);

				}

//				HxLOG_Print("==> EMM count for SoftCell:   %08x:%08x:%d:%d:%d:%d \n",astIrucSvcStatus[nValidUcSvcCnt].hUC_SVC,
//										astIrucSvcStatus[nValidUcSvcCnt].hUC_internalSVC, astIrucSvcStatus[nValidUcSvcCnt].bValid,
//										astIrucSvcStatus[nValidUcSvcCnt].bEcmType, astIrucSvcStatus[nValidUcSvcCnt].bCCA, nValidUcSvcCnt);
				nValidUcSvcCnt++;
				ucEmmCnt++;
				ucTotalCount ++;
			}
		}
	}

FUNC_OUT:
	VK_SEM_Release(s_ulUCCAAPISema);
/* CCA EMM and SoftCell 하나로 친다. */
	*pucTotalUcSvcCnt = ucTotalCount;
	if (astIrucSvcStatus != NULL)
	{
		*ppSvcStatus_shallbefree = astIrucSvcStatus;
	}


	return 0;
}


HINT32 xsvc_cas_IrUcGetEachStatusService(HUINT32 eSvcType, HUINT16 usHandle, IRUC_SERVICE_STATUS **ppEachSvcStatus_shallbefree, IRUC_SVC_STATUS_INFO_e eGetType)
{

	HUINT32 ulServiceCnt, i, nValidUcSvcCnt=0, ulComponentindex=0;
	uc_result			uc_ret = UC_ERROR_SUCCESS;
	uc_convergent_client_service_status_st buffConvCSstatus;
	HUINT8	ucTotalCount=0,ucEcmCnt=1,ucEmmCnt=1;
	HUINT32	ulStreamIndex=0;

	IRUC_SERVICE_STATUS stEachSvcStatus;

	VK_SEM_Get(s_ulUCCAAPISema);

	memset(&buffConvCSstatus, 0x00, sizeof(uc_convergent_client_service_status_st));
	HxLOG_Print("\n");

	HxSTD_MemSet(&stEachSvcStatus, 0, sizeof(IRUC_SERVICE_STATUS));

	ulServiceCnt = IRUC_HUMAX_MAX_SERVICE+IRUC_HUMAX_MAX_TP;
    	ulServiceCnt++;

	for(i= 0; i< IRUC_HUMAX_MAX_SERVICE ; i++ )
	{
		g_IrUcSvcInfo[i].ulStreamCount = 0;

/* Descramble ECM*/
		if(g_IrUcSvcInfo[i].UC_SvcHandle != IRUC_INVALID_UC_SVCHANDLE)
		{
/* ecm pid, status 정보 저장. */
			HxLOG_Print("\n[%s] ECM: Svchandle(0x%x)\n",__FUNCTION__, g_IrUcSvcInfo[i].UC_SvcHandle);
			uc_ret = UniversalClient_GetStreamStatus(g_IrUcSvcInfo[i].UC_SvcHandle,&g_IrUcSvcInfo[i].ulStreamCount,&g_IrUcSvcInfo[i].UCStreamStatusInfo);
			if(uc_ret != UC_ERROR_SUCCESS || g_IrUcSvcInfo[i].ulStreamCount == 0 || g_IrUcSvcInfo[i].UCStreamStatusInfo == NULL)
			{
				//UniversalClient_FreeStreamStatus(&g_IrUcSvcInfo[i].UCStreamStatusInfo);
				HxLOG_Print("[%s] ==> NOT FOUND ECM stream status info ",__FUNCTION__);
				g_IrUcSvcInfo[i].ulStreamCount = 0; // SUCCESS가 아닐 경우 배열의 Count는 무조건 0로 하여, count때문에 발생할 수 있는 문제를 없앤다.
				//goto FUNC_OUT;	/* FTA Service에서도 보여주기 ECM/EMM service menu를 보여주기 위해 */
			}

			HxLOG_Print("\n[%s.%d] g_IrUcSvcInfo[i].svcType  : 0x%X,  g_IrUcSvcInfo[i].UC_SvcHandle : 0x%X \n",__FUNCTION__,__LINE__,
				g_IrUcSvcInfo[i].svcType , g_IrUcSvcInfo[i].UC_SvcHandle);

			if (g_IrUcSvcInfo[i].svcType == eSvcType && g_IrUcSvcInfo[i].UC_SvcHandle == usHandle)
			{
				stEachSvcStatus.ucStreamCnt		= g_IrUcSvcInfo[i].ulStreamCount;
				for(ulStreamIndex=0; ulStreamIndex<g_IrUcSvcInfo[i].ulStreamCount; ulStreamIndex++)
				{
					HxSTD_MemCopy(stEachSvcStatus.stStreamStatus[ulStreamIndex].aucStatusString,  g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage, UC_CONNECTION_STATUS_STRING_SIZE);
					HxLOG_Print("[%s] Each ECM: Status[%s], \n",__FUNCTION__, stEachSvcStatus.stStreamStatus[ulStreamIndex].aucStatusString);
					stEachSvcStatus.stStreamStatus[ulStreamIndex].EcmEmmPid = g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid;
					stEachSvcStatus.stStreamStatus[ulStreamIndex].ucComponentCnt = g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount;
					for(ulComponentindex=0 ; ulComponentindex < g_IrUcSvcInfo[i].UCStreamStatusInfo->componentCount; ulComponentindex++)
					{
						stEachSvcStatus.stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex]=g_IrUcSvcInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray[ulComponentindex].pid;
						HxLOG_Print("[%s] Status ES PID: 0x%04x \n",__FUNCTION__, stEachSvcStatus.stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex]);
					}
				}

				stEachSvcStatus.bCCA = g_IrUcSvcInfo[i].bCCA ;
				stEachSvcStatus.hUC_SVC = g_IrUcSvcInfo[i].UC_SvcHandle;

				uc_ret = UniversalClient_GetServiceStatus( stEachSvcStatus.hUC_SVC,
														(uc_byte *)&stEachSvcStatus.aucSVCStatusString );
				HxLOG_Print(" Status: [%s][%s] \n", stEachSvcStatus.aucSVCStatusString, stEachSvcStatus.stStreamStatus[0].aucStatusString);

				if(uc_ret == UC_ERROR_SUCCESS)
				{
					stEachSvcStatus.bValid =TRUE;
					stEachSvcStatus.bEcmType = TRUE;

	/* hjlee3, 여기서 hardcoding 해도 상관은 없을거 같다. */
					stEachSvcStatus.eSvcType = g_IrUcSvcInfo[i].svcType;
					HxLOG_Print(" ECM:SoftCell Valid[0x%02x:0x%02x] \n",g_IrUcSvcInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.valid, g_IrUcSvcInfo[i].UCmonitorInfo.softcellMonitorInfo.valid);
					if(g_IrUcSvcInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.valid == TRUE)
					{
						// CCA ECM
						stEachSvcStatus.eMonitorSvcType = g_IrUcSvcInfo[i].UCmonitorInfo.serviceType;
						stEachSvcStatus.bMonitoringValid = TRUE;
						HxSTD_MemCopy(stEachSvcStatus.aucMonitormessage,
							g_IrUcSvcInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.message, UC_MONITOR_MSG_LENGTH);
						stEachSvcStatus.aucMonitormessage[UC_MONITOR_MSG_LENGTH-1] = '\0';
						HxLOG_Print("[%s] ECM CCA Monitor [%s] \n",__FUNCTION__,stEachSvcStatus.aucMonitormessage);

					}
					else if(g_IrUcSvcInfo[i].UCmonitorInfo.softcellMonitorInfo.valid == TRUE)
					{
						// SoftCell ECM
						stEachSvcStatus.eMonitorSvcType = g_IrUcSvcInfo[i].UCmonitorInfo.serviceType;
						stEachSvcStatus.bMonitoringValid = TRUE;
						HxSTD_MemCopy(stEachSvcStatus.aucMonitormessage,
							g_IrUcSvcInfo[i].UCmonitorInfo.softcellMonitorInfo.message, UC_MONITOR_MSG_LENGTH);
						stEachSvcStatus.aucMonitormessage[UC_MONITOR_MSG_LENGTH-1] = '\0';
						HxLOG_Print("[%s] ECM SoftCell Monitor [%s] \n",__FUNCTION__,stEachSvcStatus.aucMonitormessage);
					}

					stEachSvcStatus.ucInstanceCount=ucEcmCnt;

					ucEcmCnt++;
					nValidUcSvcCnt++;
					ucTotalCount ++;
				}
				HxLOG_Print("[%s]==> ECM count 0x%08x:%d \n",__FUNCTION__,stEachSvcStatus.hUC_SVC, nValidUcSvcCnt);
			}

		}
	}

	HxLOG_Print("[%s] ==>  EMM START 0 \n",__FUNCTION__);


	for(i= 0; i< IRUC_HUMAX_MAX_TP ; i++ )
	{
		HxLOG_Print("[%s]==>  EMM START 1  \n",__FUNCTION__);
		g_IrUcTpInfo[i].ulStreamCount = 0;
		if(g_IrUcTpInfo[i].UC_SvcHandle != IRUC_INVALID_UC_SVCHANDLE)
		{

/* emm pid, status 정보 저장.*/
			HxLOG_Print("[%s] ==>  EMM START 2  \n",__FUNCTION__);
			uc_ret = UniversalClient_GetStreamStatus(g_IrUcTpInfo[i].UC_SvcHandle,&g_IrUcTpInfo[i].ulStreamCount,&g_IrUcTpInfo[i].UCStreamStatusInfo);
			if(uc_ret != UC_ERROR_SUCCESS || g_IrUcTpInfo[i].ulStreamCount == 0)
			{
				//UniversalClient_FreeStreamStatus(&g_IrUcTpInfo[i].UCStreamStatusInfo);
				HxLOG_Print("[%s] ==> NOT FOUND EMM stream status info ",__FUNCTION__);
				g_IrUcTpInfo[i].ulStreamCount=0;// SUCCESS가 아닐 경우 배열의 Count는 무조건 0로 하여, count때문에 발생할 수 있는 문제를 없앤다.
				//goto FUNC_OUT;	/* FTA Service에서도 보여주기 ECM/EMM service menu를 보여주기 위해 */
			}

			if (g_IrUcTpInfo[i].svcType == eSvcType && g_IrUcTpInfo[i].UC_SvcHandle == usHandle)
			{

				/* mult-stream for EMM 지원 */
				stEachSvcStatus.ucStreamCnt = g_IrUcTpInfo[i].ulStreamCount;
				for(ulStreamIndex=0; ulStreamIndex<g_IrUcTpInfo[i].ulStreamCount; ulStreamIndex++)
				{
					HxSTD_memcpy (stEachSvcStatus.stStreamStatus[ulStreamIndex].aucStatusString, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage, UC_CONNECTION_STATUS_STRING_SIZE);
					stEachSvcStatus.stStreamStatus[ulStreamIndex].EcmEmmPid = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid;
					stEachSvcStatus.stStreamStatus[ulStreamIndex].ucComponentCnt = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount;

					if (g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount && g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray != NULL)
					{
						for(ulComponentindex=0 ; ulComponentindex < g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount; ulComponentindex++)
						{
							stEachSvcStatus.stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex] = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray[ulComponentindex].pid;
							HxLOG_Print("[%s]  Status CCA ES PID: 0x%04x \n",__FUNCTION__,stEachSvcStatus.stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex]);
						}
					}

				}

				/* CCA or SoftCell 구분 */
				stEachSvcStatus.bCCA = g_IrUcSvcInfo[i].bCCA ;
				stEachSvcStatus.hUC_SVC = g_IrUcTpInfo[i].UC_SvcHandle ;
				uc_ret = UniversalClient_GetConvergentServiceStatus( stEachSvcStatus.hUC_SVC, &buffConvCSstatus);

				HxLOG_Print("[%s] EMM: CCA hUC_SVC:%08x, CloakedCA_status[%s] \n",__FUNCTION__,stEachSvcStatus.hUC_SVC, buffConvCSstatus.cloakedCAStatus.statusMessage);
				if(uc_ret == UC_ERROR_SUCCESS)
				{
					if (eGetType == eIRUC_STATUS_Info_CCA)
					{
						/* for CCA */
						memcpy(stEachSvcStatus.aucSVCStatusString, buffConvCSstatus.cloakedCAStatus.statusMessage, UC_CONNECTION_STATUS_STRING_SIZE);
						stEachSvcStatus.hUC_internalSVC = buffConvCSstatus.cloakedCAStatus.internalServiceHandle;
						stEachSvcStatus.bValid		= buffConvCSstatus.cloakedCAStatus.isValid;
						stEachSvcStatus.bEcmType	= FALSE;
						stEachSvcStatus.bCCA		= TRUE;
						//stEachSvcStatus.ucInstanceCount = nValidUcSvcCnt;
						stEachSvcStatus.ucInstanceCount=ucEmmCnt;
		/* hjlee3, 여기서 hardcoding 해도 상관은 없을거 같다. */
						stEachSvcStatus.eSvcType = g_IrUcTpInfo[i].svcType;
						HxLOG_Print("[%s]  EMM:SoftCell Valid[0x%02x:0x%02x] \n",__FUNCTION__,g_IrUcTpInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.valid, g_IrUcTpInfo[i].UCmonitorInfo.softcellMonitorInfo.valid);

						if(g_IrUcTpInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.valid == TRUE)
						{
							// ECM 인지 EMM 인지 구분은 할 수 잇지만 display를 하는 부분에서 구분해야 할 것으로 보인다.
							stEachSvcStatus.eMonitorSvcType = g_IrUcTpInfo[i].UCmonitorInfo.serviceType;
							stEachSvcStatus.bMonitoringValid = TRUE;
							HxSTD_memcpy(stEachSvcStatus.aucMonitormessage, g_IrUcTpInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.message, UC_MONITOR_MSG_LENGTH);
							HxLOG_Print("[%s] EMM CCA Monitor [%s] \n",__FUNCTION__,g_IrUcTpInfo[i].UCmonitorInfo.cloakedCAMonitorInfo.message);
						}

		//				HxLOG_Print("==> EMM count for CCA:   %08x:%d:%d:%d:%d \n",astIrucSvcStatus[nValidUcSvcCnt].hUC_internalSVC,
		//										astIrucSvcStatus[nValidUcSvcCnt].bValid, astIrucSvcStatus[nValidUcSvcCnt].bEcmType,
		//										astIrucSvcStatus[nValidUcSvcCnt].bCCA, nValidUcSvcCnt);
						nValidUcSvcCnt++;

		/* save for Smart */
		///multi - stream 지원

						stEachSvcStatus.ucStreamCnt = g_IrUcTpInfo[i].ulStreamCount;
						for(ulStreamIndex=0; ulStreamIndex<g_IrUcTpInfo[i].ulStreamCount; ulStreamIndex++)
						{
							/* 스트림 정보 저장 */
							HxLOG_Print("[%s] EMM: %d Status = %s, \n", __FUNCTION__, ulStreamIndex, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage);
							HxSTD_memcpy (stEachSvcStatus.stStreamStatus[ulStreamIndex].aucStatusString, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].streamStatusMessage, UC_CONNECTION_STATUS_STRING_SIZE);
							HxLOG_Print("[%s] Status CCA EMM PID: 0x%04x \n", __FUNCTION__, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid);
							stEachSvcStatus.stStreamStatus[ulStreamIndex].EcmEmmPid = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].caStream.pid;
							HxLOG_Print("[%s] EMM: Num of ES PID: %d\n", __FUNCTION__, g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount);
							stEachSvcStatus.stStreamStatus[ulStreamIndex].ucComponentCnt = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount;
							/* 컴포넌드 ES 정보 저장 */
							if (g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount && g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray != NULL)
							{
								for(ulComponentindex=0; ulComponentindex<g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentCount;ulComponentindex++)
								{
									stEachSvcStatus.stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex] = g_IrUcTpInfo[i].UCStreamStatusInfo[ulStreamIndex].componentStreamArray[ulComponentindex].pid;
									HxLOG_Print("[%s] Status CCA ES PID: 0x%04x \n", __FUNCTION__, stEachSvcStatus.stStreamStatus[ulStreamIndex].EsPidList[ulComponentindex]);
								}
							}
						}
					}

					if (eGetType == eIRUC_STATUS_Info_SOFTCELL)
					{

						/* CCA or SoftCell 구분 */
						stEachSvcStatus.bCCA = g_IrUcSvcInfo[i].bCCA ;
						/* for smart card / softcell */
						stEachSvcStatus.hUC_SVC = g_IrUcTpInfo[i].UC_SvcHandle ;
						memcpy(stEachSvcStatus.aucSVCStatusString, buffConvCSstatus.softcellStatus.statusMessage, UC_CONNECTION_STATUS_STRING_SIZE);
						HxLOG_Print("[%s] EMM: SoftCell svchandles: %08x SoftCell_status[%s] \n",__FUNCTION__, g_IrUcTpInfo[i].UC_SvcHandle, buffConvCSstatus.softcellStatus.statusMessage);
						stEachSvcStatus.hUC_internalSVC = buffConvCSstatus.softcellStatus.internalServiceHandle;
						stEachSvcStatus.bValid		= buffConvCSstatus.softcellStatus.isValid;
						stEachSvcStatus.bEcmType = FALSE;
						stEachSvcStatus.bCCA		= FALSE;
						stEachSvcStatus.ucInstanceCount=ucEmmCnt;
		/* hjlee3, 여기서 hardcoding 해도 상관은 없을거 같다. */
						stEachSvcStatus.eSvcType = g_IrUcTpInfo[i].svcType;

						if(g_IrUcTpInfo[i].UCmonitorInfo.softcellMonitorInfo.valid == TRUE)
						{
							stEachSvcStatus.eMonitorSvcType = g_IrUcTpInfo[i].UCmonitorInfo.serviceType;
							stEachSvcStatus.bMonitoringValid = TRUE;
							HxSTD_memcpy(stEachSvcStatus.aucMonitormessage, g_IrUcTpInfo[i].UCmonitorInfo.softcellMonitorInfo.message, UC_MONITOR_MSG_LENGTH);
							HxLOG_Print("[%s] EMM SoftCell Monitor aucStatusString[%s] \n",__FUNCTION__,g_IrUcTpInfo[i].UCmonitorInfo.softcellMonitorInfo.message);

						}

		//				HxLOG_Print("==> EMM count for SoftCell:   %08x:%08x:%d:%d:%d:%d \n",stEachSvcStatus.hUC_SVC,
		//										stEachSvcStatus.hUC_internalSVC, stEachSvcStatus.bValid,
		//										stEachSvcStatus.bEcmType, stEachSvcStatus.bCCA, nValidUcSvcCnt);
					}
					nValidUcSvcCnt++;
					ucEmmCnt++;
					ucTotalCount ++;
				}
			}
		}
	}

	HxSTD_MemCopy(*ppEachSvcStatus_shallbefree, &stEachSvcStatus, sizeof(IRUC_SERVICE_STATUS));

//FUNC_OUT:
	VK_SEM_Release(s_ulUCCAAPISema);

	return 0;
}

HINT32 CAS_IRUC_API_SVC_GetStatusLoader(void)
{
	HxLOG_Print("\n");

	return 0;
}

HINT32 CAS_IRUC_API_GetSmartcardStatus(HUINT32 smartcardId, uc_smartcard_status	*stBuffScStatus)
{
	uc_result				result;

	// get smartcard status
	result = UniversalClient_GetSmartcardStatus(smartcardId, stBuffScStatus);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Print("[UniversalClient_GetSmartcardStatus] SC Status:failed. 0x%X\n", result);
	}
	return result;
}

HINT32 CAS_IRUC_API_GetSmartcardInfo(HUINT32 smartcardId,uc_smartcard_info	*stBuffScInfo)
{
	uc_result				result;

	result = UniversalClient_GetSmartcardInfo(smartcardId, stBuffScInfo);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Print("[xsvc_cas_IrUcGetSmartcardInfo] SC Info:failed. 0x%X\n", result);
	}

	return result;
}

HINT32 xsvc_cas_IrUcGetStatusProduct(HUINT32 *pulProductCnt, IRUC_PRODUCT_STATUS **ppstProductstatus_shallbeFree )
{
	HUINT32 ulProductCnt = 0, i;
	uc_product_status *pProductInfoList, *pDummy;
	uc_result	uc_ret;
	IRUC_PRODUCT_STATUS		*ptrProductList = NULL;

	HxLOG_Print("[%s] \n",__FUNCTION__);

	uc_ret= UniversalClient_GetProductList(&ulProductCnt, &pDummy );
	HxLOG_Print("[%s] -- start -- \n",__FUNCTION__);

	if(UC_ERROR_SUCCESS != uc_ret)
	{
		HxLOG_Print("[%s] UniversalClient_GetProductList, failed.(0x%X)\n",__FUNCTION__ , uc_ret);
		return ERR_FAIL;
	}

	if(ulProductCnt == 0)
	{
		HxLOG_Print("[%s] UniversalClient_GetProductList no Data\n",__FUNCTION__ );
		*pulProductCnt = ulProductCnt;
		return ERR_FAIL;
	}

	ptrProductList = OxCAS_Malloc(ulProductCnt*sizeof(IRUC_PRODUCT_STATUS));

	uc_ret= UniversalClient_GetProductList(&ulProductCnt, &pProductInfoList );

	for(i =0; i< ulProductCnt; i++)
	{
		//structure copy.
		HxSTD_MemCopy(ptrProductList[i].aucProductID, pProductInfoList[i].product_id, sizeof(HUINT8) * UC_SIZE_PRODUCT_ID);
		ptrProductList[i].ucSectorNumber= pProductInfoList[i].sector_number;
		ptrProductList[i].ulStartDate   = pProductInfoList[i].startingDate;
		ptrProductList[i].ulDuration    = pProductInfoList[i].duration;
		ptrProductList[i].bEntitled= pProductInfoList[i].entitled;
		ptrProductList[i].usProductType = pProductInfoList[i].productType;
		HxLOG_Print("Product ID : %s\n", ptrProductList[i].aucProductID);
	}
	if (ptrProductList != NULL)
	{
		*pulProductCnt = ulProductCnt;
		*ppstProductstatus_shallbeFree = ptrProductList;
	}

	uc_ret = UniversalClient_FreeProductList( &pProductInfoList) ;

	if(UC_ERROR_SUCCESS != uc_ret)
	{
		//just report it.
		HxLOG_Print("UniversalClient_FreeProductList, failed. (0x%X)\n", uc_ret);
	}
	return ERR_OK;
}

// ewlee 20100205 - get client status information
HINT32 xsvc_cas_IrUcGetStatusClient(IRUC_CLIENT_STATUS *pClientStatus, HUINT32 *tmsDataLen)
{
	uc_result	result = UC_ERROR_SUCCESS;
	uc_buffer_st			stBytes;
	uc_ca_section_count		stEcmEmmCount;
	uc_smartcard_status		stBuffScStatus;
	uc_smartcard_info		stBuffScInfo;
	uc_uint32 				ucCaSystemID = 0;

	HxLOG_Info("[%s] start 0x%X\n",__FUNCTION__ );
	if (pClientStatus == NULL)
	{
		HxLOG_Info("Universal Client Version:failed. 0x%X\n", result);
		return ERR_FAIL;
	}

	memset(&stBuffScStatus, 0x00, sizeof(uc_smartcard_status));
	stBuffScStatus.smartcardId = 0;
	memset(&stBuffScInfo,   0x00, sizeof(uc_smartcard_info));
	stBuffScInfo.smartcardId = 0;


	// get version
	stBytes.bytes = (uc_byte *) pClientStatus->ucVersion;
	stBytes.length = UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetVersion(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("Universal Client Version:failed. 0x%X\n", result);
		return ERR_FAIL;
	}

	// get ClientID
	stBytes.bytes = (uc_byte *) pClientStatus->ucClientID;
	stBytes.length = UC_BUFFER_GENERIC_SIZE ;
	result = UniversalClient_GetClientIDString(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetClientIDString] ClientID:failed. 0x%X\n", result);
	}

	// get CSSN
	stBytes.bytes = (uc_byte *) pClientStatus->ucCSSN;
	stBytes.length = IRUC_DEVICE_CSSN_LEN;
	result = UniversalClientSPI_Device_GetCSSN(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("Universal Client Device CSSN:failed. 0x%X\n", result);
	}

	// get serial number
	stBytes.bytes = (uc_byte *) pClientStatus->ucSerialNumber;
	stBytes.length = UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetSerialNumber(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		snprintf(pClientStatus->ucSerialNumber, UC_BYTES_GENERIC_SIZE, "%s", (char *)"no serial");
		HxLOG_Info("[UniversalClient_GetSerialNumber] Serial Number:failed. 0x%X\n", result);
	}

	/* Transfer bin Biggendian to String a Serial Number */
	{
		HUINT32 shiftI = 0;
		HUINT32 uSum = 0;
		for(shiftI=0; shiftI<stBytes.length; shiftI++){
			if (shiftI != 0 )
				uSum <<= 8;
			uSum |=  (unsigned long long)stBytes.bytes[shiftI];
		}
		/* Check if SN is out of range. */
		if (shiftI > sizeof(HUINT32)) {
			HxLOG_Info("Serial Number is bigger than 32bits!!!\n");
			return ERR_FAIL;
		}
		snprintf(pClientStatus->ucSerialNumber, UC_BYTES_GENERIC_SIZE, "%u", uSum);
	}

	// get Lock ID
	result = UniversalClient_GetLockId(&(pClientStatus->nLockID));
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("Universal Client Lock ID:failed. 0x%X\n", result);
	}

	// get CA System ID
	result = UniversalClient_GetCASystemID(&(pClientStatus->nCASystemID));
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("Universal Client CA System ID:failed. 0x%X\n", result);
	}

	stBytes.bytes = (uc_byte *) pClientStatus->ucSecureCoreVersion;
	stBytes.length = UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetSecureCoreVersion(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetSecureCoreVersion] Secure Type:failed. 0x%X\n", result);
	}

//	VK_SEM_Get(s_ulUCCAAPISema);
	// get securecoreVersion
	memset(pClientStatus->ucSecureCoreLoadStatus, 0x00, IRUC_SECURE_CORE_LENGTH);
	memset(pClientStatus->packagesDownloadProgressInfo, 0x00, IRUC_SECURE_CORE_LENGTH);
	VK_MEM_Memcpy(pClientStatus->ucSecureCoreLoadStatus, g_IrUcTpInfo[0].secureCoreListStatus, IRUC_SECURE_CORE_LENGTH);
	HxLOG_Info("[%s] copy secure status [%s] : [%s]\n",__FUNCTION__, g_IrUcTpInfo[0].secureCoreListStatus, pClientStatus->ucSecureCoreLoadStatus);
	VK_MEM_Memcpy(pClientStatus->packagesDownloadProgressInfo, g_IrUcTpInfo[0].packagesDownloadProgressInfo, IRUC_SECURE_CORE_LENGTH);
//	VK_SEM_Release(s_ulUCCAAPISema);

	// get Secure Type
	result = UniversalClient_GetSecureType(&(pClientStatus->ucSecureType));
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetSecureType] Secure Type:failed. 0x%X\n", result);
	}


	// get Nationality
	stBytes.bytes	= (uc_byte *) pClientStatus->ucNationality;
	stBytes.length	= UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetNationality(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		if ( result == UC_ERROR_NO_NATIONALITY) {
			snprintf(stBytes.bytes, 15, "%s", "NO NATIONALITY");
			stBytes.length = sizeof("NO NATIONALITY") + 1;
		} else {
			snprintf(stBytes.bytes, 6, "%s", "ERROR");
			stBytes.length = sizeof("ERROR") + 1;
		}
		HxLOG_Info("[UniversalClient_GetNationality] Nationality:failed. 0x%X\n", result);
	}

	// get ECM, EMM count
	result = UniversalClient_GetEcmEmmCount(&stEcmEmmCount);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("Universal Client ECM EMM Count:failed. 0x%X\n", result);
	//	return ERR_FAIL;
	}
	else
	{
		pClientStatus->nEcmCount = stEcmEmmCount.ecm_count;
		pClientStatus->nEmmCount = stEcmEmmCount.emm_count;
	}

	// get softcellversion
	stBytes.bytes = (uc_byte *) pClientStatus->ucSoftcellVersion;
	stBytes.length = UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetSoftcellVersion(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetSoftcellVersion] Softcell Version:failed. 0x%X\n", result);
		stBytes.bytes[0] = '\0'; // terminate
	}

	// get softcellbuildinfo
	stBytes.bytes = (uc_byte *) pClientStatus->ucBuildInfo;
	stBytes.length = UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetSoftcellBuildInfo(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetSoftcellBuildInfo] Softcell BuildInforation:failed. 0x%X\n", result);
	}

	// get  Build Information
	stBytes.bytes = (uc_byte *) pClientStatus->ucCCABuildInfo;
	stBytes.length = UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetBuildInformation(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetBuildInformation] CCA BuildInformation:failed. 0x%X\n", result);
	}

	// get smartcard status
	stBuffScStatus.smartcardId = 0;
	result = UniversalClient_GetSmartcardStatus(0, &stBuffScStatus);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetSmartcardStatus] SC Status:failed. 0x%X\n", result);
	}
	memcpy(pClientStatus->ucSmartCardStatus, stBuffScStatus.status, UC_STATUS_LENGTH );
	HxLOG_Info("[%s] SmartCard status [%s]\n", __FUNCTION__ , pClientStatus->ucSmartCardStatus);

	// get smartcard Info
	stBuffScInfo.smartcardId = 0;
	result = UniversalClient_GetSmartcardInfo(0, &stBuffScInfo);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetSmartcardInfo] SC Info:failed. 0x%X\n", result);
		memset(&stBuffScInfo, 0x00, sizeof(stBuffScInfo));
	}

	memcpy(pClientStatus->ucScserialNumber, stBuffScInfo.serialNumber, UC_MAX_SMARTCARD_SERIAL_NUMBER_LENTH );
	pClientStatus->nsmartcardType		= stBuffScInfo.smartcardType;
	pClientStatus->ucmajorVersion		= stBuffScInfo.majorVersion;
	pClientStatus->ucminorVersion		= stBuffScInfo.minorVersion;

	/*extend info */
	pClientStatus->ucbuild			= stBuffScInfo.build;
	pClientStatus->ucvariant		= stBuffScInfo.variant;
	pClientStatus->npatchLevel		= stBuffScInfo.patchLevel;
	pClientStatus->nownerId			= stBuffScInfo.ownerId;

	stBytes.bytes 					= (uc_byte *) pClientStatus->ucSmartCardNationality;
	stBytes.length 					= UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetSmartcardNationality(0, &stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Print("[UniversalClient_GetSmartcardNationality]  SC Nationality:failed. 0x%X\n", result);
		stBytes.bytes[0] = '\0';
	}


	result = UniversalClient_GetSmartcardCASystemID(0, &ucCaSystemID);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("[UniversalClient_GetSmartcardCASystemID]  SC CASystemID:failed. 0x%X\n", result);
		ucCaSystemID = 0;
	}
	pClientStatus->nSmartCardCASystemID	= ucCaSystemID;

	// get TMS data
//	snprintf(pClientStatus->ucTmsData, UC_BYTES_GENERIC_SIZE-1," ");	// hjlee3 test temp
	stBytes.bytes = (uc_byte *) pClientStatus->ucTmsData;
	stBytes.length = UC_BYTES_TMS_SIZE;
	result = UniversalClient_GetTmsData(&stBytes);
	*tmsDataLen = stBytes.length;
	HxLOG_Info("[UniversalClient_GetTmsData] TMS length:[%d] \n", *tmsDataLen);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("Universal Client TMS Data:failed. 0x%X\n", result);
	}

#if 0
TMS data check.
	uc_uint32 i = 0;
	for( i = 0; i < 32 ; i++ )
	{
		if((i%16)==0 && (i!=0))
		{
			HxLOG_Print("\n");
		}

		HxLOG_Print("%02x ",stBytes.bytes[i]);
	}
#endif

	// get capabilities
	stBytes.bytes = (uc_byte *) pClientStatus->ucCapabilities;
	stBytes.length = UC_BYTES_GENERIC_SIZE;
	result = UniversalClient_GetCapabilities(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Info("Universal Client Capabilities:failed. 0x%X\n", result);
	}

	return ERR_OK;
}

HINT32 xsvc_cas_IrUcGetStatusRegion(HUINT32 *pulRegionCnt, IRUC_REGION_STATUS **ppstRegionstatus_shallbeFree  )
{
	uc_uint32 pRegioninfoCount = 0;
	uc_region_information *ptrRegioninfoList, *ptrDummy = NULL;
	uc_result	uc_ret;
	HUINT32		i=0;
	IRUC_REGION_STATUS		*ptrRegionList;

	uc_ret = UniversalClient_GetRegionInfoList(&pRegioninfoCount, &ptrDummy);

	if(UC_ERROR_SUCCESS != uc_ret )
	{
		HxLOG_Print("UniversalClient_GetRegionInfoList, failed.(0x%X)\n", uc_ret);
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	if(pRegioninfoCount ==0)
	{
		HxLOG_Print("UniversalClient_GetRegionInfoList, no Data\n");
		HxLOG_Critical("\n[%s:%d]\n",__FUNCTION__,__LINE__);
		return ERR_FAIL;
	}

	ptrRegionList= OxCAS_Malloc(pRegioninfoCount*sizeof(IRUC_REGION_STATUS));

	/***********	 hard coding test만 해봤음. real data test해야함	***********/
	uc_ret = UniversalClient_GetRegionInfoList(&pRegioninfoCount, &ptrRegioninfoList);

	for(i = 0; i< pRegioninfoCount ; i++)
	{
		ptrRegionList[i].ucSectorNumber = ptrRegioninfoList[i].sector_number;
		ptrRegionList[i].ucRegion = ptrRegioninfoList[i].region;
		ptrRegionList[i].ucSubRegion = ptrRegioninfoList[i].subregion;
	}

	*pulRegionCnt = pRegioninfoCount;
	*ppstRegionstatus_shallbeFree = ptrRegionList;

	uc_ret = UniversalClient_FreeRegionInfoList(&ptrRegioninfoList) ;
	if(UC_ERROR_SUCCESS != uc_ret )
	{
		//just report it.
		HxLOG_Print("UniversalClient_FreeProductList, failed. (0x%X)\n", uc_ret);
	}
	return ERR_OK;
}

HINT32 xsvc_cas_IrUcSetConfigService(uc_service_handle  serviceHandle, HUINT32  TLVLength,  HUINT8 *pTLV)
{
	uc_result	uc_ret;
	HxLOG_Print("[xsvc_cas_IrUcSetConfigService] Set TVL \n");
	uc_ret = UniversalClient_ConfigService(serviceHandle, TLVLength, (const void *)pTLV);

	if(UC_ERROR_SUCCESS != uc_ret )
	{
		//just report it.
		HxLOG_Print("xsvc_cas_IrUcSetConfigService, failed. (0x%X)\n", uc_ret);
	}

	return ERR_OK;
}
// TMS or Pingerfrinting 받기 위해서는 이 함수로 set 을 해야 하는 것으로 보인다.
//#define __SET_TMS_ATTRIBUTE__

HINT32 xsvc_cas_IrUcSetTMSService(HUINT32 ucSvcindex)
{
	uc_result	uc_ret;

	DbSvc_Info_t	svcInfo;

	HxLOG_Print("[%s] Set TMS or PingerFrinting \n",__FUNCTION__ );

#if defined(__SET_TMS_ATTRIBUTE__)
/* TODO : DbSvc_SUPPORT_DXINFO 에 의해서 DB 에서 get 하는 방법 변경된듯 하다 .
          우선은 막는다. __SET_TMS_ATTRIBUTE__ 이걸로 막는다. 나중에 연결 필요 하다. */

/* Comment : TMS Attribute 를 사용하는 OP라면 이곳을 OP 사양에 맞춰서 Set 해야 한다.
			 사용 하지 않는다면 막는다. */

	g_IrUcSvcInfo[ucSvcindex].ulattributeCount = 2;
	g_IrUcSvcInfo[ucSvcindex].pTmsAttributesItemInfoList[0].length = 32;
	g_IrUcSvcInfo[ucSvcindex].pTmsAttributesItemInfoList[0].startbit = 10;
	g_IrUcSvcInfo[ucSvcindex].pTmsAttributesItemInfoList[0].value = 2;
	g_IrUcSvcInfo[ucSvcindex].pTmsAttributesItemInfoList[1].length = 32;
	g_IrUcSvcInfo[ucSvcindex].pTmsAttributesItemInfoList[1].startbit = 20;
	g_IrUcSvcInfo[ucSvcindex].pTmsAttributesItemInfoList[1].value = 0;

	uc_ret = UniversalClient_SetTMSAttributes((uc_uint16)g_IrUcSvcInfo[ucSvcindex].ulattributeCount, &(g_IrUcSvcInfo[ucSvcindex].pTmsAttributesItemInfoList[0]));
	if(UC_ERROR_SUCCESS != uc_ret )
	{
		HxLOG_Print("[%s] Set TMS SetAttributes , failed. (0x%X)\n",__FUNCTION__ , uc_ret);
		return ERR_FAIL;
	}

	memset(&svcInfo, 0, sizeof(DbSvc_Info_t));
	DB_SVC_GetServiceInfo(g_IrUcSvcInfo[ucSvcindex].hSvc, &svcInfo);
	HxLOG_Print("[%s] Set TMS or PingerFrinting SvcId:0x%04X, TsId:0x%04X, TsId:0x%04X \n", __FUNCTION__, svcInfo.usSvcId, svcInfo.usTsId, svcInfo.usOnId);

#if 1
	uc_ret = UniversalClient_SetTunedInfo(svcInfo.usSvcId, svcInfo.usTsId, svcInfo.usOnId);
	HxLOG_Print(" Set TMS or PingerFrinting SvcId:0x%04X, TsId:0x%04X, TsId:0x%04X, return: 0x%X \n",svcInfo.usSvcId, svcInfo.usTsId, svcInfo.usOnId, uc_ret);
	if(UC_ERROR_SUCCESS != uc_ret)
	{
		//just report it.
		HxLOG_Print("[%s] Set TMS SetTunedInfo , failed. (0x%X)\n",__FUNCTION__ , uc_ret);
		return ERR_FAIL;
	}
#else
/* support to multiful descramble
	uc_ret = UniversalClient_SetTunedInfoList(tunedInfoCount, &pTunedInfoList) ;
	if(UC_ERROR_SUCCESS != uc_ret )
	{
		//just report it.
		UcDrvError(DBG_TRACE,(" Set TMS service , failed. (0x%X)\n", uc_ret));
		return HIR_NOT_OK;
	}
*/
#endif

#endif
	return ERR_OK;

}

HINT32	xsvc_cas_IrUcStartMonitoring(HUINT16 ushSCELL, HUINT8 ucType)
{
	HxLOG_Print("[%s] start Monitoring =\n",__FUNCTION__ );

	if( s_ucTypeForMonitor == IR_MONITOR_STOP)
	{
		switch(ucType)
		{
			case IR_MONITOR_ECM:
				CAS_IRUC_DRV_SendServiceDefineOfEcmMonitor(ushSCELL, 1);
				s_usSCELLforMonitor = ushSCELL;
				s_ucTypeForMonitor = ucType;
				break;

			case IR_MONITOR_EMM:
				CAS_IRUC_DRV_SendServiceDefineOfEmmMonitor(ushSCELL, 1);
				s_usSCELLforMonitor = ushSCELL;
				s_ucTypeForMonitor = ucType;
				break;
		}
	}
	return HIR_OK;
}

HINT32	xsvc_cas_IrUcStopMonitoring(void)
{
	HxLOG_Print("[%s]\n",__FUNCTION__ );

	if( s_ucTypeForMonitor != IR_MONITOR_STOP )
	{
		switch(s_ucTypeForMonitor)
		{
			case IR_MONITOR_ECM:
				CAS_IRUC_DRV_SendServiceDefineOfEmmMonitor(s_usSCELLforMonitor, 0);
				break;

			case IR_MONITOR_EMM:
				CAS_IRUC_DRV_SendServiceDefineOfEmmMonitor(s_usSCELLforMonitor, 0);
				break;
		}

		s_ucTypeForMonitor = IR_MONITOR_STOP;
	}
	return HIR_OK;
}


HINT32 xsvc_cas_IrUcGetClientID(HUINT8 *poutClientID, HUINT32 uiLength)
{
	HUINT8					ucClientID[UC_BUFFER_GENERIC_SIZE] = {0,};			/* Client ID */
	uc_result				result = UC_ERROR_SUCCESS;
	uc_buffer_st			stBytes;

	stBytes.bytes			= (uc_byte *) ucClientID;
	stBytes.length			= UC_BUFFER_GENERIC_SIZE ;
	result = UniversalClient_GetClientIDString(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Print("[UniversalClient_GetClientIDString] ClientID:failed. 0x%X\n", result);
		return HIR_NOT_OK;
	}

	/* to prevent a buffer overflow. */
	if (uiLength > UC_BUFFER_GENERIC_SIZE){
		uiLength = UC_BUFFER_GENERIC_SIZE;
	}
	memcpy(poutClientID, ucClientID, uiLength);
	return HIR_OK;
}

HUINT32 xsvc_cas_iruc_API_CopySerialNum(HUINT8 ucSlotID, HUINT8 *pData)
{
/* comment : g_IrUcSvcInfo[0].ucHashCodevalue 에 해당 값을 사용 한다.
          multi descramble 이 된다 하더라도 view에 해당 하는 것만 알고 잇음 되므로
          g_IrUcSvcInfo[N]에  multi descramble 을 염려 할 필요는 없어 보인다. */
	HxSTD_memcpy(pData, g_IrUcSvcInfo[0].ucHashCodevalue, CLIENTSERIALNUMBER);
	return HIR_OK;
}

void local_cas_iruc_MSR_DRV_HashCodeConvert(char* szInUcSerialNumber, char* szOutHashedCode )
{
	/* szInUcSerialNumber input format: "2009068066" */

	char paucConversionLetterTable[20]={0,};
	char paucSerialNum[CLIENTSERIALNUMBER]={0,};
	int i,j;

	/*step 1. make fingerprint hashcode reverse-conversion table & initialize string variables*/
	for(i=0;i<20; i++)
	{
		paucConversionLetterTable[i]= 'B' + i;
	}

	HxSTD_memcpy( paucSerialNum, szInUcSerialNumber, 10);
	paucSerialNum[10] = szInUcSerialNumber[12];
	paucSerialNum[11] = 0;

	/*step 2. Convert from lowest digit */
	HxLOG_Print("\n HashCode: ");
	for(i=0;i<CA_MAX_LENGTH_CLIENT_SERIAL_NUMBER; i++)
	{
		/* loop digit */
		for(j=0;j<10;j++)
		{
			if( paucSerialNum[10- i] == ('0' + j) )
			{
				szOutHashedCode[i] = paucConversionLetterTable[9-j+i] ;
				HxLOG_Print("%c ",szOutHashedCode[i]);
				break;
			}
		}
	}
}

void HUMAX_VdSc_HashCodeConvert(void)
{
	uc_buffer_st	stBytes;
	uc_result		result = UC_ERROR_SUCCESS;
	HUINT8		ucSerialNumber[CLIENTSERIALNUMBER]={0,};

/*
TODO: hjlee3  pigerprint 을 위해서 사용 해야 하는 hashcode convert에서
smartcard id 가 필요 하다 UCCA 에서 pinger print를 하기 위해서 이런 방법을 사용 해야 하는가 ?
UniversalClient_GetSmartcardStatus , UniversalClient_GetSmartcardInfo 은 smartcardID를 사용 하지 않는다고 한다.
이거 어케 대체 해야 하는가 ?
hash 로만 사용 해야 하는가 ???  */
/*
	// get CSSN
	stBytes.bytes = (uc_byte *) ucCSSN;
	stBytes.length = IRUC_DEVICE_CSSN_LEN;
	result = UniversalClientSPI_Device_GetCSSN(&stBytes);
	if (result != UC_ERROR_SUCCESS)
	{
		HxLOG_Print("[UniversalClientSPI_Device_GetCSSN] CSSN:failed. 0x%X\n", result);
	}
*/
	stBytes.bytes = (uc_byte *) ucSerialNumber;
	stBytes.length = CLIENTSERIALNUMBER;
	result = UniversalClient_GetSerialNumber(&stBytes);
	if (result == UC_ERROR_SUCCESS)
	{
		local_cas_iruc_MSR_DRV_HashCodeConvert(ucSerialNumber, g_IrUcSvcInfo[0].ucHashCodevalue);
	}
	else
	{
		snprintf(ucSerialNumber, UC_BYTES_GENERIC_SIZE, "%s", (char *)"no serial");
		HxLOG_Print("[UniversalClient_GetSerialNumber] Serial Number:failed(%s). 0x%X\n",ucSerialNumber, result );

	}

}

#if defined(CONFIG_DEBUG)

#define _________IRUC_DRV_DEBUG_FUNCTIONS______________

#if 0
static CMD_TABLE_ITEM_t	s_UcDrvDbgInfo =
{
		"Universal Client",
		NULL, //local_cmd_DbgCasIrdeto,
		{NULL, NULL, NULL, NULL,  NULL},
		NULL
};
#endif

HINT8* local_cas_iruc_Dbg_PrintSvcMsgType(uc_service_message_type msgType)
{
	switch(msgType)
	{
		ENUM_TO_STR(UC_ECM_STATUS);
		ENUM_TO_STR(UC_EMM_STATUS);
		ENUM_TO_STR(UC_SERVICE_STATUS);
		ENUM_TO_STR(UC_SERVICE_PVR_RECORD_STATUS_REPLY);
		ENUM_TO_STR(UC_SERVICE_PVR_PLAYBACK_STATUS_REPLY);
		ENUM_TO_STR(UC_SERVICE_PVR_SESSION_METADATA_REPLY);
		ENUM_TO_STR(UC_SERVICE_ECM_MONITOR_STATUS);
		ENUM_TO_STR(UC_SERVICE_EMM_MONITOR_STATUS);
		ENUM_TO_STR(UC_SERVICE_ECM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT);
		ENUM_TO_STR(UC_SERVICE_EMM_MONITOR_STATUS_FOR_CONVERGENT_CLIENT);
		ENUM_TO_STR(UC_SERVICE_STATUS_FOR_CONVERGENT_CLIENT);
		ENUM_TO_STR(UC_SERVICE_INFO_FOR_SOFTCELL_PREVIEW);
	}


	return "Unknown SvcMsgType";
}

HINT8* local_cas_iruc_Dbg_PrintGlobalMsgType(uc_global_message_type msgType)
{
	switch(msgType)
	{
		ENUM_TO_STR(UC_GLOBAL_DEBUG);
		ENUM_TO_STR(UC_GLOBAL_RAW_IRD_MESSAGE);
		ENUM_TO_STR(UC_GLOBAL_SERIAL_NUMBER_CHANGED);
		ENUM_TO_STR(UC_GLOBAL_OOB_EMM_STATUS_REPLY);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_ENTITLEMENT_CHANGE);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_CASECTION_COUNT);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_REGION_SUBREGION_INFO);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_TMS_DATA_CHANGED);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_NATIONALITY_CHANGED);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_HOMING_CHANNEL_DATA_CHANGED);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_MIDDLEWARE_USER_DATA_CHANGED);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_RESET_TO_FACTORY);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_CA_DATA_CLEARED);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_SMARTCARD_STATUS);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_SMARTCARD_NATIONALITY_CHANGED);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_VALID_CLIENT_CHANGED);
		ENUM_TO_STR(UC_GLOBAL_SMARTCARD_RAW_IRD_MESSAGE);
		ENUM_TO_STR(UC_GLOBAL_NOTIFY_FLEXIFLASH_MESSAGE);
	}
	return "Unknown GlobalMsgType";
}


HINT8* local_cas_iruc_Dbg_PrintValidClientType(uc_valid_client_type msgType)
{
	switch(msgType)
	{
		ENUM_TO_STR(UC_CLIENT_UNDEFINED);
		ENUM_TO_STR(UC_SOFTCELL_VALID);
		ENUM_TO_STR(UC_CLOAKEDCA_VALID);
		ENUM_TO_STR(UC_BOTH_VALID);
	}

	return "Unknown ValidClientType";
}

void local_cas_iruc_Dbg_PrintTpInfo(void)
{
	HUINT32	i; //, j, k, l;
//	HUINT8	ucIndex = 0xFF;
//	HUINT8	aucTemp[128];

	HxLOG_Print("==================== UC TS INFO ====================\n");

	for( i = 0; i < IRUC_HUMAX_MAX_TP; i++ )
	{
		if( g_IrUcTpInfo[i].hAction != HANDLE_NULL )
		{
			HxLOG_Print("[%d]UcSvcContext : 0x%X\n", i, g_IrUcTpInfo[i].UcSvcContext);
			HxLOG_Print("[%d]hAction : 0x%X\n", i, g_IrUcTpInfo[i].hAction);
			HxLOG_Print("[%d]ucActId : 0x%X\n", i, g_IrUcTpInfo[i].ucActId);
			HxLOG_Print("[%d]nDmxId : 0x%X\n", i, g_IrUcTpInfo[i].ulDemuxId);
			HxLOG_Print("[%d]usTsUniqueId : 0x%X\n", i, g_IrUcTpInfo[i].usTsUniqueId);
//			HxLOG_Print("[%d]UC_SvcHandle : %d\n", i, g_IrUcTpInfo[i].UC_SvcType);

//			if( g_IrUcTpInfo[i].UC_SvcType == SERVICE_DVB_EMM )
//				HxLOG_Print("\tSERVICE_DVB_EMM\n");
//			else
//				HxLOG_Print("\tINVALID TYPE for g_IrUcTpInfo\n");

			HxLOG_Print("[%d]UC_SvcHandle : 0x%X\n", i, g_IrUcTpInfo[i].UC_SvcHandle);
			HxLOG_Print("[%d] EMM STATUS  -  %s\n", i, g_IrUcTpInfo[i].szEmmStatusMsg);
			HxLOG_Print("[%d] SVC STATUS  -  %s\n", i, g_IrUcTpInfo[i].szSvcStatusMsg);
/*
(%03d) %04x %02x %02x %02x %02x %02x %02x
wEmmCount “%03d”
wEmmPid “%04x”
bAddressControl “%02x”
bScStatus1 “%02x”
bScStatus2 “%02x”
abEMMPayload[3] ”%02x %02x %02x”[Global Status, Sector Status, Sector change indicator]
*/

#if 0
			HxLOG_Print("[%d]Emm monitioring : hSvc= 0x%X, wRscId = 0x%X\n",g_IrUcTpInfo[i].EmmMonitoring.wServiceHandle, g_IrUcTpInfo[i].EmmMonitoring.wResourceId);
			sprintf(aucTemp, "(%03d) %04x %02x %02x %02x %02x %02x %02x %02x",
				g_IrUcTpInfo[i].EmmMonitoring.wEmmCount,\
				g_IrUcTpInfo[i].EmmMonitoring.wEmmPid,\
				g_IrUcTpInfo[i].EmmMonitoring.bAddressControl,\
				g_IrUcTpInfo[i].EmmMonitoring.wEmmCount,\
				g_IrUcTpInfo[i].EmmMonitoring.bScStatus1,\
				g_IrUcTpInfo[i].EmmMonitoring.bScStatus2,\
				g_IrUcTpInfo[i].EmmMonitoring.abEmmPayload[0],\
				g_IrUcTpInfo[i].EmmMonitoring.abEmmPayload[1],\
				g_IrUcTpInfo[i].EmmMonitoring.abEmmPayload[2] );
			HxLOG_Print("\t Display Data = %s\n", aucTemp);
#endif

		}
		else
		{
			HxLOG_Print("[%d]EMPTY\n", i);
		}

		HxLOG_Print("----------------------------------------------------------------------\n");
	}
	HxLOG_Print("\n\n========================== END ==========================\n");
}


HUINT32 local_cas_iruc_Drv_Dbg_PrintSvcInfo(void)
{
	HUINT32	 	i, j;

	HxLOG_Print("\n\n==================== UC SERVICE INFO ====================\n");

	for( i = 0; i < IRUC_HUMAX_MAX_SERVICE; i++ )
	{
		if( g_IrUcSvcInfo[i].hIrSvc != HANDLE_NULL )
		{
			HxLOG_Print("[%d]UcSvcContext : 0x%X\n", i, g_IrUcSvcInfo[i].UcSvcContext);
			HxLOG_Print("[%d]HUC-SVC HANDLE : 0x%X\n", i, g_IrUcSvcInfo[i].hIrSvc);
			HxLOG_Print("[%d]Action HANDLE : 0x%X\n", i, g_IrUcSvcInfo[i].hAct);
			HxLOG_Print("[%d]SVC HANDLE : 0x%X\n", i, g_IrUcSvcInfo[i].hSvc);
			HxLOG_Print("[%d]Demux ID: %d\n", i, g_IrUcSvcInfo[i].ulDemuxId);
			HxLOG_Print("[%d]UC_SvcHandle : %d\n", i, g_IrUcSvcInfo[i].UC_SvcHandle);

			//HxLOG_Print("\t\t[0:NONE, 1:FTA, 2:IRDETO, 3:OTHER]\n");

			HxLOG_Print("[%d] PID LIST - %d\n", i, g_IrUcSvcInfo[i].ucStreamCount);
			for( j = 0; j < g_IrUcSvcInfo[i].ucStreamCount; j++ )
			{
				HxLOG_Print("\t\t[PID : 0x%04X \n", g_IrUcSvcInfo[i].PidList[j].usPID);
				#if 0
				switch(g_IrUcSvcInfo[i].PidList[j].ucTypeFromPMT)
				{
					case eEsType_Video:
						HxLOG_Print("VIDEO(%d), ", g_IrUcSvcInfo[i].PidList[j].ucTypeFromPMT);
						break;

					case eEsType_Audio:
						HxLOG_Print("AUDIO(%d), ", g_IrUcSvcInfo[i].PidList[j].ucTypeFromPMT);
						break;

					case eEsType_Subtitle:
					case eEsType_Teletext:
					case eEsType_Data:
						HxLOG_Print("DATA(%d), ", g_IrUcSvcInfo[i].PidList[j].ucTypeFromPMT);
						break;

					default:
						HxLOG_Print("UNKNOWN(%d), ", g_IrUcSvcInfo[i].PidList[j].ucTypeFromPMT);
						break;
				}

				HxLOG_Print("\n");
				#endif
			}


			HxLOG_Print("[%d] ECM STATUS  -%s\n", i, g_IrUcSvcInfo[i].szEcmStatusMsg);
			HxLOG_Print("[%d] SVC STATUS  -%s\n", i, g_IrUcSvcInfo[i].szSvcStatusMsg);

		}
		else
		{
			HxLOG_Print("[%d]EMPTY\n", i);
		}
		HxLOG_Print("----------------------------------------------------------------------\n");
	}
	HxLOG_Print("\n\n========================== END ==========================\n");

	//IR_DRV_DBG_DSC_INFO();

	return 0;
}


void CMD_Register_MW_CAS_UCDRV_Debug(void)
{
#if 0
	CMD_AddCommandTableItem(&s_UcDrvDbgInfo);
#endif
	CMD_RegisterWord(local_cmd_UcDrvCommand,
						"uc",						/* keyword */
						"irdeto universal debug api",	/* help   */
						"uc svc |tp |sc |stream |mail| erase|etc ");	/* usage  */
	return;
}


static int local_cmd_UcDrvCommand(void *szArgList)
{
	char *aucArg=NULL;
	HUINT8 aucStr[20]={0,};

	/* 1st argument */
	aucArg = (char *)HLIB_CMD_GetNextArg((HCHAR**)&szArgList);
	if( aucArg == NULL )
		return HxCMD_ERR;
	if( sscanf(aucArg, "%s", aucStr) == 0 )
		return HxCMD_ERR;

	HLIB_STD_StrUpper(aucStr);

	if (strcmp(aucStr, "SVC") == 0)
	{
		local_cas_iruc_Drv_Dbg_PrintSvcInfo();
		return HxCMD_OK;
	}
	else if (strcmp(aucStr, "TP") == 0)
	{
		//local_cas_iruc_Drv_Dbg_PrintTpInfo();
		local_cas_iruc_Dbg_PrintTpInfo();
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "STREAM") == 0)
	{
		local_cas_iruc_StreamSPI_DBG_Info();
		return HxCMD_OK;
	}
	else if(strcmp(aucStr, "MAIL") == 0 )
	{

	}
	else if(strcmp(aucStr, "ERASE") == 0 )
	{

		aucArg = (char *)CMD_NextArg((char **)&szArgList);
		if(aucArg == NULL)
		{
			return CMD_ERR;
		}
		if( snprintf(aucStr, 20, "%s", aucArg) == 0  )
		{
			return CMD_ERR;
		}

		CMD_StrToUpper((HUINT8 *)aucStr);

		if(strcmp(aucStr, "CA") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_CA_SIZE] = {0x00,};
			if (HIR_OK == CAS_IR_NVR_Partition_Write(IRNVR_PART_CA, 0, aTemp, IR_PARTITION_CA_SIZE ) )
			{
				HxLOG_Print("Erase CA partition :ok\n");
			}
			else
			{
				HxLOG_Print("Erase CA partition :failed\n");
			}
		}
		else if(strcmp(aucStr, "IR") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_IRDETO_SIZE] = {0x00,};
			if (HIR_OK == CAS_IR_NVR_Partition_Write(IRNVR_PART_IRDETO, 0, aTemp, IR_PARTITION_IRDETO_SIZE ) )
			{
				HxLOG_Print("Erase irdeto partition :ok\n");
			}
			else
			{
				HxLOG_Print("Erase irdeto  partition :failed\n");
			}
		}
		else if(strcmp(aucStr, "LOADER") == 0)
		{
			HUINT8 aTemp[IR_PARTITION_LOADER_SIZE] = {0x00,};
			if (HIR_OK == CAS_IR_NVR_Partition_Write(IRNVR_PART_LOADER, 0, aTemp, IR_PARTITION_LOADER_SIZE ) )
			{
				HxLOG_Print("Erase Loader partition :ok\n");
			}
			else
			{
				HxLOG_Print("Erase Loader partition :failed\n");
			}
		}

	}
	else if(strcmp(aucStr, "ETC") == 0)
	{
		return HxCMD_OK;
	}
	return HxCMD_ERR;
}

#endif

HIR_ERR xsvc_cas_IrUcSvcStatusGetNumberOfServices(IR_SVC_STATUS_INFO_MAIN **stSvcStatusMain, HUINT8 *pucNumberOfServices)
{
	IRUC_SERVICE_STATUS *pstSvcStatus = NULL;
	HUINT8 count = 0;

	if( HIR_OK != xsvc_cas_IrUcGetStatusService(&count, &pstSvcStatus))
	{
		*pucNumberOfServices = count;
		return HIR_NOT_OK;
	}
	*pucNumberOfServices = count;

	if (count != 0)
	{
		int i = 0;
		IR_SVC_STATUS_INFO_MAIN *pstSvcStatusMain = OxCAS_Malloc(sizeof(IR_SVC_STATUS_INFO_MAIN) * count);

		if (pstSvcStatusMain == NULL)
			return HIR_NOT_OK;

		for(i = 0; i <  count ; i ++)
		{
			pstSvcStatusMain[i].hAction		=	pstSvcStatus[i].hAction;
			pstSvcStatusMain[i].eSvcType	=	pstSvcStatus[i].eSvcType;
			pstSvcStatusMain[i].ucInstanceCount = pstSvcStatus[i].ucInstanceCount;
			pstSvcStatusMain[i].usSCellSvcHandle= (HUINT16)pstSvcStatus[i].hUC_SVC;

			HxLOG_Print("\nSVC_STATUS_INFO[%d] hAction : 0x%X,  eSvcType : %d, ucInstanceCount : %d, SvcHandle : 0x%X\n",i,
				pstSvcStatusMain[i].hAction	,
				pstSvcStatusMain[i].eSvcType,
				pstSvcStatusMain[i].ucInstanceCount,
				pstSvcStatusMain[i].usSCellSvcHandle);
		}
		*stSvcStatusMain = pstSvcStatusMain;
		HLIB_STD_MemFree(pstSvcStatus);
	}
	return HIR_OK;
}

/********************************************************************
 Description: 각 service들의 세부적인 정보를 얻어내는 함수.
 		   DVB Client Integration Guide에 있는 service status menu를 그리는 spec에 맞게 맞추어진 정보임.
 ********************************************************************/
HIR_ERR xsvc_cas_IrUcSvcStatusGetInformationOfEachService(HUINT32 eSvcType, HUINT16 usHandle, IRUC_SERVICE_STATUS *stSvcStatusEach, IRUC_SVC_STATUS_INFO_e eGetType )
{
	HxLOG_Debug("Get EachServicee! SvcType : 0x%x,  usHandle : 0x%X , stSvcStatusEach : 0x%X \n", eSvcType, usHandle , stSvcStatusEach);
	if (HIR_OK != xsvc_cas_IrUcGetEachStatusService(eSvcType, usHandle, &stSvcStatusEach, eGetType))
	{
		return HIR_NOT_OK;
	}

	return HIR_OK;
}



