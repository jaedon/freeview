#include <stdio.h>

#include "htype.h"
#include <hlib.h>

#include "pal_power.h"
#include <pal_sc.h>

#include <_svc_cas_sub_dev_api.h>

#include "ir_platform.h"
#include "vd_humax.h"
#include "util.h"
#include "ir_api.h"
#include "ir_evt.h"
#include "ir_nvr.h"
#include "ir_msg.h"





#define	CA_MAX_LENGTH_SC_SERIAL_NUMBER				11
#define	EEPROM_SIZE_MAILBOX_INFO			(sizeof(MAILBOX_INFO))
#define	EEPROM_SIZE_MAILBOX_TABLE		(MAX_NUMBER_MAIL_MESSAGE * sizeof(MAILBOX_TABLE))

#define	NVRAM_OFFSET_MAIL_PARTITION		0
#define	NVRAM_OFFSET_MAILBOX_INFO		(NVRAM_OFFSET_MAIL_PARTITION)

static HULONG			s_ulIrMsgSemId;
static MAILBOX_INFO		s_stIrMailboxInfo;
static MAILBOX_TABLE	s_stIrAttrFingerPrint;
static HUINT8			s_ucAttrFpIdx;


#define	IRMSG_ATTR_FP_IDX(idx)	((idx) + MAX_NUMBER_MAIL_MESSAGE * 10)

#define IRMSG_MUTEX_GET 	VK_SEM_Get(s_ulIrMsgSemId)
#define IRMSG_MUTEX_RELEASE 	VK_SEM_Release(s_ulIrMsgSemId)

void local_cas_ir_MSG_DRV_Reset(void);
void local_cas_ir_MSG_DRV_TextMsgProcess(Handle_t hAct, HUINT8 *pData, HUINT16 usLength);
void local_cas_ir_MSG_DRV_MsgProcess(Handle_t hAct, MAILBOX_TABLE *pMail_msg);
void local_cas_ir_MSG_DRV_ProcCntrMsg(Handle_t hAct, HUINT8 *pData, HUINT16 usLength);
HBOOL local_cas_ir_MSG_DRV_GetMailMessage(MAILBOX_TABLE *mail_msg, HUINT8 *pbData);
HUINT8 local_cas_ir_MSG_DRV_AddMailMessage( MAILBOX_TABLE *mail_msg );
HUINT8 local_cas_ir_MSG_DRV_AddAttrFingerPrint (MAILBOX_TABLE *mail_msg);
HUINT8 local_cas_ir_MSG_DRV_GetEmptyRoomOfMailboxTable(  void);
HUINT8 local_cas_ir_MSG_DRV_FindLowestPriorityMailMessage( void );
HUINT8	local_cas_ir_MSG_DRV_FindFirstExpiredMailMessage(UNIXTIME curunixtime, HxDATETIME_Date_t* curDate, HxDATETIME_Time_t* curTime);
void local_cas_ir_MSG_DRV_GetLastMailTime( UNIXTIME *punixtime, HxDATETIME_Date_t *dt, HxDATETIME_Time_t *tm);
HBOOL local_cas_ir_MSG_DRV_GetAttributedDisplay(MAILBOX_TABLE *mail_msg, HUINT8 *pbData);
void local_cas_ir_MSG_DRV_Attributed_DisplayProcess(Handle_t hAct, HUINT8 *pData, HUINT16 usLength);
HIR_ERR local_cas_ir_MSG_DRV_LoadFromNVRAM( void );
HIR_ERR local_MSG_DRV_StorageUserMailInfoWrite( void );
HIR_ERR local_cas_ir_MSG_DRV_StorageSetMailReadStatusWrite(HUINT32 ulIndex);
HIR_ERR local_cas_ir_MSG_DRV_StorageSetMailValidStatusWrite(HUINT32 ulIndex);
HIR_ERR local_cas_ir_MSG_DRV_FlushMailBox(void);


/********************************************************************
 External Variable Declaration
 ********************************************************************/
//extern IR_DRV_SC_INFO g_IrDrvScInfo[HUMAX_VD_SC_SLOT_COUNT];


/********************************************************************
*	External function prototypes
********************************************************************/
extern	HIR_ERR CAS_IR_DRV_QueryScData(HUINT32 ulScRscId);
extern	HIR_ERR CAS_IR_DRV_RegionQuery(HUINT32 ulScRscId, HUINT8 ucSector);
#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
extern	HUINT32 CAS_IR_DRV_HomingChannelQuery(HUINT32 resourceId);
#endif


// 디버깅 용도
void CAS_IR_MemDump(HUINT8 *pszStr, HUINT8 *pucBuf, HUINT32 ulLen)
{
#ifdef CONFIG_DEBUG
	int i;

	HxLOG_Info(pszStr);

	for(i = 0 ; i < ulLen ; i++)
	{
		if(i % 16 == 0)
			VK_Print("\n");

		HxLOG_Info("%02x ", pucBuf[i]);
	}

	HxLOG_Info("\n");
#endif
}


void CAS_IR_MSG_DRV_Initialise( void )
{
	HINT32 nErr;

	HxLOG_Info("\n");

	nErr = VK_SEM_Create(&s_ulIrMsgSemId, "s_ulIrMsgSemId", VK_SUSPENDTYPE_FIFO);
	if(nErr != VK_OK)
	{
		HxLOG_Print("Can't create ir_msg sema. (%d)\n", nErr);
		HxLOG_Critical("\n\n");
		return ;
	}

	IRMSG_MUTEX_GET;

	if( HIR_OK != local_cas_ir_MSG_DRV_LoadFromNVRAM() )
	{
		HxLOG_Print("Load mail failed\n");

		/* whole e-mails shall be erased. */
		local_cas_ir_MSG_DRV_Reset();
		HxLOG_Critical("\n\n");
	}

	IRMSG_MUTEX_RELEASE;
}

void CAS_IR_MSG_DRV_ProcessDecoderDataEMM(Handle_t hAct, HUINT8 *p)
{
	HUINT8 ucDestinationID;
	HUINT16	usMessageLength;

	IRMSG_MUTEX_GET;

	HxLOG_Info("\n");

	ucDestinationID = (p[0] & 0xF0) >> 4;
	usMessageLength = ((p[0] & 0x0F) << 8) | p[1];
	p += 2;

	switch(ucDestinationID)
	{
		case DECODER_MSG_DESTINATION_ID_TEXT:		// Text messages
			HxLOG_Print("[DECODER_MSG_DESTINATION_ID_TEXT]\n");
			local_cas_ir_MSG_DRV_TextMsgProcess(hAct, p, usMessageLength);
			break;

		case DECODER_MSG_DESTINATION_ID_CNTR:		// Decoder Control
			HxLOG_Print("[DECODER_MSG_DESTINATION_ID_CNTR]\n");
			local_cas_ir_MSG_DRV_ProcCntrMsg(hAct, p, usMessageLength);
			break;

		case DECODER_MSG_DESTINATION_ID_CAM:		// CAM messages(to CAM)(not used)
			HxLOG_Print("[CAM MESSAGES - NOT USED\n");
			break;

		case DECODER_MSG_DESTINATION_ID_PROFDEC:		// Prof-Dec messages
			HxLOG_Print("[PROF-DEC MESSAGES]\n");
			break;

		case DECODER_MSG_DESTINATION_ID_ATTRIBUTED_DISPLAY:		// Attributed Display
			//CAS_IR_MemDump("DECODER_MSG_DESTINATION_ID_ATTRIBUTED_DISPLAY", p, usMessageLength);
			local_cas_ir_MSG_DRV_Attributed_DisplayProcess(hAct, p, usMessageLength) ;
			break;

		case DECODER_MSG_DESTINATION_ID_OPENCABLE_HOST:
			HxLOG_Print("[OPENCABLE_HOST MESSAGES]\n");
			break;
		case DECODER_MSG_DESTINATION_ID_CIPLUS_CAM:
			HxLOG_Print("[CIPLUS-CAM MESSAGES]\n");
			break;

		default:
			HxLOG_Print("[UNKNOWN DECODER MESSAGES] - [%08x]\n", ucDestinationID);
			break;
	}

	IRMSG_MUTEX_RELEASE;
	return;
}

HBOOL CAS_IR_MSG_DRV_GetValid( HUINT8 ucMailIndex, HUINT16 *psFlag )
{
	if( ucMailIndex >= MAX_NUMBER_MAIL_MESSAGE )
		return FALSE;

	if (psFlag == NULL)
		return FALSE;

	IRMSG_MUTEX_GET;

	*psFlag = s_stIrMailboxInfo.valid[ucMailIndex];

	IRMSG_MUTEX_RELEASE;

	return TRUE;
}

HBOOL CAS_IR_MSG_DRV_GetReadState( HUINT8 ucMailIndex, HUINT16 *pbFlag )
{
	if( ucMailIndex >= MAX_NUMBER_MAIL_MESSAGE )
		return FALSE;

	if (pbFlag == NULL)
		return FALSE;

	IRMSG_MUTEX_GET;

	*pbFlag = s_stIrMailboxInfo.bReadStatus[ucMailIndex];

	IRMSG_MUTEX_RELEASE;

	return TRUE;
}

HBOOL CAS_IR_MSG_DRV_SetReadState( HUINT8 ucMailIndex, HUINT16 sFlag )
{
	if( ucMailIndex >= MAX_NUMBER_MAIL_MESSAGE )
		return FALSE;

	IRMSG_MUTEX_GET;

	s_stIrMailboxInfo.bReadStatus[ucMailIndex] = sFlag;

	local_cas_ir_MSG_DRV_StorageSetMailReadStatusWrite(ucMailIndex);

	IRMSG_MUTEX_RELEASE;

	return TRUE;
}

/******************************************************************************************************/
//      Function : HUINT8 CAS_IR_MSG_DRV_DoYouHaveUnreadMail() -> 안 읽은 mail이 있나 없나..
/******************************************************************************************************/
HUINT8 CAS_IR_MSG_DRV_DoYouHaveUnreadMail(void)
{
	HUINT8	i;
	HUINT8	ucTypeFlag = 0x00;  // 0xf0 : Normal , 0x0f : Announce

	if (s_stIrMailboxInfo.ucListValid == 0)
	{
		return ucTypeFlag;
	}

	IRMSG_MUTEX_GET;

	for(i = 0; i < MAX_NUMBER_MAIL_MESSAGE; i++)
	{
		if (s_stIrMailboxInfo.valid[s_stIrMailboxInfo.aucMailIndexList[i]] != IR_MAIL_VALID_OK)
			continue;

		if (s_stIrMailboxInfo.bReadStatus[s_stIrMailboxInfo.aucMailIndexList[i]] == IR_MAIL_READ_STATUS_NOT_READ)
		{
			switch (s_stIrMailboxInfo.mailbox_table[s_stIrMailboxInfo.aucMailIndexList[i]].ucType)
			{
			case EPG_MESSAGE_TYPE_TEXT_MAILBOX:
				ucTypeFlag |= 0xf0;
				break;
			case EPG_MESSAGE_TYPE_TEXT_ANNOUNCEMENT:
				ucTypeFlag |= 0x0f;
				break;
			default:
				break;
			}
		}
	}

	IRMSG_MUTEX_RELEASE;

	return ucTypeFlag;
}

HBOOL CAS_IR_MSG_DRV_DeleteMail( HUINT8 ucMailIndex )
{
	HMSG	msg;

	HxSTD_memset(&msg, 0, sizeof(HMSG));

	HxLOG_Print("MSGDRV - CAS_IR_MSG_DRV_DeleteMail : ucMailIndex = %d\n", ucMailIndex);
	if( ucMailIndex >= MAX_NUMBER_MAIL_MESSAGE )
		return FALSE;

	IRMSG_MUTEX_GET;

	s_stIrMailboxInfo.valid[ucMailIndex] = IR_MAIL_VALID_NOT_OK;

	local_cas_ir_MSG_DRV_StorageSetMailValidStatusWrite(ucMailIndex);

	IRMSG_MUTEX_RELEASE;
	return TRUE;
}


HIR_ERR CAS_IR_MSG_DRV_DeleteAllMail (HUINT8 ucMsgType)
{
	HUINT8	i;
	HIR_ERR	err;

	IRMSG_MUTEX_GET;

	for (i = 0 ; i < MAX_NUMBER_MAIL_MESSAGE ; i++)
	{
		if (s_stIrMailboxInfo.valid[i] == IR_MAIL_VALID_NOT_OK)
			continue;
		if (s_stIrMailboxInfo.mailbox_table[i].ucType != ucMsgType)
			continue;

		s_stIrMailboxInfo.valid[i] = IR_MAIL_VALID_NOT_OK;
	}
	err = local_cas_ir_MSG_DRV_FlushMailBox();
	IRMSG_MUTEX_RELEASE;

	return err;
}


HIR_ERR CAS_IR_MSG_DRV_DeleteAllTypesMail (void)
{
	HUINT8	i;
	HIR_ERR	err;

	IRMSG_MUTEX_GET;

	for (i = 0 ; i < MAX_NUMBER_MAIL_MESSAGE ; i++)
	{
		if (s_stIrMailboxInfo.valid[i] == IR_MAIL_VALID_NOT_OK)
			continue;
		s_stIrMailboxInfo.valid[i] = IR_MAIL_VALID_NOT_OK;
	}
	err = local_cas_ir_MSG_DRV_FlushMailBox();
	IRMSG_MUTEX_RELEASE;

	return err;
}

void CAS_IR_MSG_DRV_AllClearExpiredNormalAttributeMail(void)
{
	HINT32			i;
	HUINT8			nMailCnt;
	MAILBOX_TABLE	MailInfo;
	HUINT8			aMailIdxList[MAX_NUMBER_MAIL_MESSAGE];
	HUINT32			result;

	// TODO: this function makes no sense in structual form ,
	// TODO: it herited from neo, it shall be changed with local function not using API call,
	// TODO: do not write muxtex. until being changed. jichoi.

	result = xsvc_cas_IrGetMailList(&nMailCnt, aMailIdxList);

	for (i=0 ; i<nMailCnt ; i++)
	{
		// svc_cas_CtrlGet 사용해도 된다.
		result = xsvc_cas_IrGetMailBox(aMailIdxList[i], &MailInfo);
		if (result == HIR_OK)
		{
			if(xsvc_cas_IrCheckValidOfMail(aMailIdxList[i]) != TRUE)
				continue;

			if(MailInfo.bAttributeMsg == FALSE)
				continue;

			if((MailInfo.Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_NORMAL)
				&&(MailInfo.Atrb_Display.unDuration != 0))
			{
				CAS_IR_MSG_DRV_DeleteMail(aMailIdxList[i]);
			}
		}
	}
}

/******************************************************************************************************/
//      Function : void CAS_IR_MSG_DRV_GetMailInfo() -> 현재 저장된 mail의 개수와 저장된 순서의 index list를 얻어가는 함수.
// 				*paucMailIndexList는 이 안에서 alloc하고 주므로 사용하고 free할 것..
/******************************************************************************************************/
void CAS_IR_MSG_DRV_GetMailInfo( HUINT8 *pucNumberOfSavedMail, HUINT8 *paucMailIndexList )
{
	HUINT8			i;
	HUINT8			j;

	// prevent fix(added)
	if (pucNumberOfSavedMail == NULL || paucMailIndexList == NULL)
	{
		HxLOG_Critical("\n\n");
		return;
	}
	// the end of fix

	IRMSG_MUTEX_GET;

	*pucNumberOfSavedMail = 0;

	HxLOG_Print(" s_stIrMailboxInfo.ucListValid = %s\n", (s_stIrMailboxInfo.ucListValid==TRUE)?"OK":"NOT VALID");

	*pucNumberOfSavedMail = 0;
	if(s_stIrMailboxInfo.ucListValid)
	{
		for (i = 0, j = 0; i < MAX_NUMBER_MAIL_MESSAGE; i++)
		{
			if( s_stIrMailboxInfo.valid[s_stIrMailboxInfo.aucMailIndexList[i]] == IR_MAIL_VALID_OK)
			{
				paucMailIndexList[j] = s_stIrMailboxInfo.aucMailIndexList[i];
				j++;
			}
		}
		*pucNumberOfSavedMail = j;
	}

	IRMSG_MUTEX_RELEASE;

	return;
}

/******************************************************************************************************/
//      Function : BOOL CAS_IR_MSG_DRV_GetMailBox() -> 하나의 mail을 ucMailIndex로 가져가는 함수.
// 				*pMailBox는 caller에서 alloc이던 static이던 선언한 후에 줄 것.
/******************************************************************************************************/
HBOOL CAS_IR_MSG_DRV_GetMailBox( HUINT8 ucMailIndex, MAILBOX_TABLE *pMailBox )
{
	IRMSG_MUTEX_GET;
	HxLOG_Info(": ucMailIndex = %d\n", ucMailIndex);

	if( ucMailIndex >= MAX_NUMBER_MAIL_MESSAGE )
	{
		// attributed finger print는 flash에 저장되지 않고, 따로 메모리에 저장되어 관리 됨
		if ( ucMailIndex == IRMSG_ATTR_FP_IDX(s_ucAttrFpIdx) )
		{
			HxSTD_memcpy(pMailBox, &s_stIrAttrFingerPrint, SIZE_MAILBOX_TABLE);
			HxLOG_Print("Mail index (0x%d) is attributed finger print\n", ucMailIndex );
			IRMSG_MUTEX_RELEASE;
			return TRUE;
		}

		HxLOG_Print("Mail index (0x%d) is over max-mailbox index\n", ucMailIndex );
		IRMSG_MUTEX_RELEASE;
		return FALSE;
	}

	HxSTD_memcpy((HUINT8 *)pMailBox, (HUINT8 *)&(s_stIrMailboxInfo.mailbox_table[ucMailIndex]), SIZE_MAILBOX_TABLE);

#if 0//defined(CONFIG_DEBUG)
	HxLOG_Print("\t[%d][%c,%c][%d:%d:%d]:%s(len=%d)\n",
		ucMailIndex,
		(pMailBox->ucType == EPG_MESSAGE_TYPE_TEXT_MAILBOX) ? 'M' : 'A',
		(pMailBox->msg_class == EPG_MESSAGE_CLASS_NORMAL) ? 'N' : 'F',
		pMailBox->add_time.ucHour, pMailBox->add_time.ucMinute, pMailBox->add_time.ucSecond,
		pMailBox->text, pMailBox->msg_length );
#endif
	IRMSG_MUTEX_RELEASE;
	return TRUE;
}

#if defined(CONFIG_DEBUG)
void CAS_IR_MSG_DRV_DbgMailBox(void)
{
	HUINT8 num;
	HUINT8 mail_list[MAX_NUMBER_MAIL_MESSAGE];
	int i;
	MAILBOX_TABLE MailBox;

	CAS_IR_MSG_DRV_GetMailInfo(&num, mail_list);

	for(i=0; i< num; i++)
	{
		CAS_IR_MSG_DRV_GetMailBox( mail_list[i], &MailBox);

		HLIB_CMD_Printf("\t[%d][%c,%c][%d:%d:%d][%d:%d:%d]:%s(len=%d)\n",
			mail_list[i],
			(MailBox.ucType == EPG_MESSAGE_TYPE_TEXT_MAILBOX) ? 'M' : 'A',
			(MailBox.msg_class == EPG_MESSAGE_CLASS_NORMAL) ? 'N' : 'F',
			MailBox.add_date.usYear, MailBox.add_date.ucMonth, MailBox.add_date.usYear,
			MailBox.add_time.ucHour, MailBox.add_time.ucMinute, MailBox.add_time.ucSecond,
			MailBox.text, MailBox.msg_length );
	}
}


void CAS_IR_MSG_DRV_DbgCntrMsg(Handle_t hAct, HUINT8 *pData, HUINT16 usLength)
{
	local_cas_ir_MSG_DRV_ProcCntrMsg(hAct, pData, usLength);
}
#endif


void local_cas_ir_MSG_DRV_Reset(void)
{
	HUINT16 crc16;

	HxLOG_Info("\n");
	HxSTD_memset(&s_stIrMailboxInfo, 0x00, sizeof(MAILBOX_INFO) );
	s_stIrMailboxInfo.ucListValid = TRUE;

	crc16 = HLIB_MATH_GetCrc16 (&(s_stIrMailboxInfo.mailbox_table), MAX_NUMBER_MAIL_MESSAGE * sizeof(MAILBOX_TABLE));
	s_stIrMailboxInfo.crc16 = crc16;

	local_MSG_DRV_StorageUserMailInfoWrite();
}

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK)
void local_cas_ir_ProcUserPayload(Handle_t hAct, HUINT8 *pData, HUINT16 usLength)
{
	HUINT8	*p;
	HUINT8	bCntrMsg_Type;
	HUINT8	ucUserPayloadVersion;
	HUINT16	usQualifLength;
	HUINT8	ucQualifType, ucQualifSubType, ucUserPayloadVersion1;
	HUINT16	usCmdLangth;
	HUINT8	ucCmdType, ucCmdSubType;
	HUINT16	usDestination;
	HUINT8	ucTag;
	HUINT16	usDataLength;
	HMSG	msg;

	HxLOG_Info("\n");

	HxSTD_memset(&msg, 0, sizeof(HMSG));

	p = pData;
	bCntrMsg_Type = (*p++ >> 4) & 0x0f;

	if(bCntrMsg_Type == CONTROL_MESSAGE_TYPE_USER_PAYLOAD)
	{
		ucUserPayloadVersion = *p++;
		usQualifLength = CAS_IR_DRV_Get16Bits(p);
		p += 2;
		ucQualifType = *p++;
		ucQualifSubType = *p++;
		ucUserPayloadVersion1 = *p++;

		usCmdLangth = CAS_IR_DRV_Get16Bits(p);
		p += 2;

		ucCmdType = *p++;
		ucCmdSubType = *p++;

		usDestination = CAS_IR_DRV_Get16Bits(p);
		p += 2;

		ucTag = *p++;
		usDataLength = CAS_IR_DRV_Get16Bits(p);
		p += 2;

		if(usDestination == IR_UP_DESTINATION_CA_TASK)
		{
			switch(ucTag)
			{
				case IR_UP_CA_TAG_ITO_DDEMM_VIRTUAL_BILL:
				{
#if defined(CONFIG_DEBUG)
					HUINT16 usCnt;
#endif

					HUINT8 *pucBillData;

					if(usDataLength)
					{
						pucBillData = OxCAS_Malloc(usDataLength);
						if(pucBillData == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						VK_MEM_Memcpy(pucBillData, p, usDataLength);
						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CA_VIRTUAL_BILL;
						msg.usParam = usDataLength;
						msg.ulParam = (HUINT32)pucBillData;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}

					HxLOG_Print("IR_UP_CA_TAG_ITO_DDEMM_VIRTUAL_BILL len[%d]\n", usDataLength);
#if defined(CONFIG_DEBUG)
					for(usCnt = 0; usCnt < usDataLength; usCnt ++)
					{
						HxLOG_Print("[0x%02x]", p[usCnt]);
						if(usCnt%7 == 0)
						{
							HxLOG_Print("\n");
						}
					}
#endif
				}
					break;
				case IR_UP_CA_TAG_ITO_DDEMM_DM_PARAMETERS:
				{
					DataMiningParameters *pstDMP;

					HxLOG_Print("IR_UP_CA_TAG_ITO_DDEMM_DM_PARAMETERS len[%d]\n", usDataLength);

					if(usDataLength != 0)
					{
						pstDMP = (DataMiningParameters *)OxCAS_Malloc(sizeof(DataMiningParameters));
						if(pstDMP == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						VK_MEM_Memcpy(pstDMP->pucPhoneNo, p, 16);
						pstDMP->pucPhoneNo[15] = '\0';
						p += 16;
						pstDMP->ulIpAddress = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstDMP->usPort = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstDMP->usReportDate = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstDMP->usReportTime = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						VK_MEM_Memcpy(pstDMP->pucUsername, p, 10);
						pstDMP->pucUsername[9] = '\0';
						p += 10;
						VK_MEM_Memcpy(pstDMP->pucPassword, p, 20);
						pstDMP->pucPassword[19] = '\0';
						p += 20;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CA_DM_PARAMETERS;
						msg.usParam = sizeof(DataMiningParameters);
						msg.ulParam = (HUINT32)pstDMP;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}

				}
					break;
				case IR_UP_CA_TAG_ITO_DDEMM_FEATURE_BITMAP:
				{
					HUINT32 ulFeatureBitmap;

					HxLOG_Print("IR_UP_CA_TAG_ITO_DDEMM_FEATURE_BITMAP len[%d]\n", usDataLength);

					if(usDataLength)
					{
						ulFeatureBitmap = CAS_IR_DRV_Get32Bits(p);
						p += 4;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CA_FEATURE_BITMAP;
						msg.usParam = usDataLength;
						msg.ulParam = ulFeatureBitmap;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}

				}
					break;
				case IR_UP_CA_TAG_ITO_DDEMM_IPPV_IP_AND_PHONE_NO:
				{
					IPPVCallbackInformation *pstIppvCallBack;

					HxLOG_Print("IR_UP_CA_TAG_ITO_DDEMM_IPPV_IP_AND_PHONE_NO len[%d]\n", usDataLength);


					if(usDataLength != 0)
					{
						pstIppvCallBack = (IPPVCallbackInformation *)OxCAS_Malloc(sizeof(IPPVCallbackInformation));
						if(pstIppvCallBack == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						VK_MEM_Memcpy(pstIppvCallBack->pucPhoneNo, p, 16);
						pstIppvCallBack->pucPhoneNo[16] = '\0';
						p += 16;
						pstIppvCallBack->ulIpAddress = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstIppvCallBack->usPort = CAS_IR_DRV_Get16Bits(p);
						p += 2;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CA_IPPV_IP_AND_PHONE_NO;
						msg.usParam = sizeof(IPPVCallbackInformation);
						msg.ulParam = (HUINT32)pstIppvCallBack;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}

				}
					break;
				case IR_UP_CA_TAG_ITO_DDEMM_IPPV_DEBIT_LIMIT_THRESHOLD:
				{
					HUINT8 ucThreshold;

					HxLOG_Print("IR_UP_CA_TAG_ITO_DDEMM_IPPV_DEBIT_LIMIT_THRESHOLD len[%d]\n", usDataLength);
					if(usDataLength)
					{
						ucThreshold = *p;
						p ++;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CA_IPPV_DEBIT_LIMIT_THRESHOLD;
						msg.usParam = usDataLength;
						msg.ulParam = ucThreshold;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
				}
					break;
				case IR_UP_CA_TAG_ITO_DDEMM_IPPV_SCHEDULE:
				{
					IPPVSchedule *pstIppvSchedule;

					HxLOG_Print("IR_UP_CA_TAG_ITO_DDEMM_IPPV_SCHEDULE len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstIppvSchedule = (IPPVSchedule *)OxCAS_Malloc(sizeof(IPPVSchedule));
						if(pstIppvSchedule == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstIppvSchedule->ucStatus = *p;
						p ++;
						pstIppvSchedule->usReportDate = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstIppvSchedule->usReportTime = CAS_IR_DRV_Get16Bits(p);
						p += 2;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CA_IPPV_SCHEDULE;
						msg.usParam = sizeof(IPPVSchedule);
						msg.ulParam = (HUINT32)pstIppvSchedule;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_CA_TAG_ITO_DDEMM_REPORT_STATUS:
				{
					DecoderStatusTrigger *pstReportStatus;

					HxLOG_Print("IR_UP_CA_TAG_ITO_DDEMM_REPORT_STATUS len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstReportStatus = (DecoderStatusTrigger *)OxCAS_Malloc(sizeof(DecoderStatusTrigger));
						if(pstReportStatus == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						VK_MEM_Memcpy(pstReportStatus->pucPhoneNo, p, 16);
						p += 16;
						pstReportStatus->ulIpAddress = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstReportStatus->usPort = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						VK_MEM_Memcpy(pstReportStatus->pucUsername, p, 20);
						pstReportStatus->pucUsername[19] = '\0';
						p += 20;
						VK_MEM_Memcpy(pstReportStatus->pucPassword, p, 20);
						pstReportStatus->pucPassword[19] = '\0';
						p += 20;
						pstReportStatus->ucReportType = *p;
						p ++;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CA_REPORT_STATUS;
						msg.usParam = sizeof(DecoderStatusTrigger);
						msg.ulParam = (HUINT32)pstReportStatus;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_CA_TAG_ITO_DDEMM_SET_DOWNLOAD_TYPE:
				{
					DownloadType *pstDownloadType;

					HxLOG_Print("IR_UP_CA_TAG_ITO_DDEMM_SET_DOWNLOAD_TYPE len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstDownloadType = (DownloadType *)OxCAS_Malloc(sizeof(DownloadType));
						if(pstDownloadType == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstDownloadType->ucApplication = *p;
						p ++;
						pstDownloadType->ucSystem = *p;
						p ++;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CA_SET_DOWNLOAD_TYPE;
						msg.usParam = sizeof(DownloadType);
						msg.ulParam = (HUINT32)pstDownloadType;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}					break;
				default:
					HxLOG_Critical("\n\n");
					break;
			}
		}
		else if(usDestination == IR_UP_DESTINATION_EPG_APP)
		{
			switch(ucTag)
			{
				case IR_UP_EPG_TAG_ITO_DDEMM_DM_SETUP:
				{
					DataMiningStatus *pstDMStatus;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_DM_SETUP len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstDMStatus = (DataMiningStatus *)OxCAS_Malloc(sizeof(DataMiningStatus));
						if(pstDMStatus == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstDMStatus->ucDMStatus = *p;
						p ++;
						pstDMStatus->ucThershold = *p;
						p ++;
						pstDMStatus->usDelay = CAS_IR_DRV_Get16Bits(p);
						p += 2;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_DM_SETUP;
						msg.usParam = sizeof(DataMiningStatus);
						msg.ulParam = (HUINT32)pstDMStatus;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_FORCED_STANDBY:
					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_FORCED_STANDBY len[%d]\n", usDataLength);
					msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_FORCED_STANDBY;
					CAS_IR_EVT_SendEvent(hAct, &msg);
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_DISPLAY_CARD_NUMBER:
				{
					DisplayCardNumber *pstDispCardNo;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_DISPLAY_CARD_NUMBER len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstDispCardNo = (DisplayCardNumber *)OxCAS_Malloc(sizeof(DisplayCardNumber));
						if(pstDispCardNo == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstDispCardNo->usXPos = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstDispCardNo->usYPos = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstDispCardNo->ucDuration = *p;
						p ++;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_DISPLAY_CARD_NUMBER;
						msg.usParam = sizeof(DisplayCardNumber);
						msg.ulParam = (HUINT32)pstDispCardNo;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_REMOTE_TAG:
				{
					RemoteTagging *pstRemoteTagging;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_REMOTE_TAG len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstRemoteTagging = (RemoteTagging *)OxCAS_Malloc(sizeof(RemoteTagging));
						if(pstRemoteTagging == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstRemoteTagging->usChannelKey = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstRemoteTagging->ulTimeStart = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstRemoteTagging->usFrequency = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstRemoteTagging->ulExpiration = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstRemoteTagging->ulTimeBefore = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstRemoteTagging->ulTimeAfter = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						VK_MEM_Memcpy(pstRemoteTagging->pucServiceName, p, 30);
						pstRemoteTagging->pucServiceName[29] = '\0';
						p += 30;
						VK_MEM_Memcpy(pstRemoteTagging->pucEventName, p, 60);
						pstRemoteTagging->pucEventName[59] = '\0';
						p += 60;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_REMOTE_TAG;
						msg.usParam = sizeof(RemoteTagging);
						msg.ulParam = (HUINT32)pstRemoteTagging;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_REMOTE_RECORD:
				{
					RemoteRecording *pstRemoteRec;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_REMOTE_RECORD len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstRemoteRec = (RemoteRecording *)OxCAS_Malloc(sizeof(RemoteRecording));
						if(pstRemoteRec == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstRemoteRec->usNetworkId = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstRemoteRec->usTsId = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstRemoteRec->usServiceId = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstRemoteRec->ulTimeStart = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstRemoteRec->nEventId = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstRemoteRec->usDuration = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstRemoteRec->ulTimeBefore = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstRemoteRec->ulTimeAfter = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						VK_MEM_Memcpy(pstRemoteRec->pucServiceName, p, 30);
						pstRemoteRec->pucServiceName[29] = '\0';
						p += 30;
						VK_MEM_Memcpy(pstRemoteRec->pucEventName, p, 60);
						pstRemoteRec->pucEventName[59] = '\0';
						p += 60;
#if 0 // 현재 Old version만 사용 Phase 2에서 지원 ?(2010.09.24)
						pstRemoteRec->ucSeriesFlag = *p;
						p ++;
						pstRemoteRec->usFrequency = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstRemoteRec->ulExpiration = CAS_IR_DRV_Get32Bits(p);
						p += 4;
						pstRemoteRec->ucCancelFlag = *p;
						p ++;
#endif
						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_REMOTE_RECORD;
						msg.usParam = sizeof(RemoteRecording);
						msg.ulParam = (HUINT32)pstRemoteRec;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_DEL_PVOD_EVENT:
				{
					//DeletePushVodEvnet *pstDelPushVodEvt;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_DEL_PVOD_EVENT len[%d]\n", usDataLength);
#if 0
					if(usDataLength != 0)
					{
						pstDelPushVodEvt = (DeletePushVodEvnet *)OxCAS_Malloc(sizeof(DeletePushVodEvnet));
						if(pstDelPushVodEvt == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						VK_MEM_Memcpy(pstDelPushVodEvt->pucProductName, p, 14);
						p += 14;
						pstDelPushVodEvt->ucPersistentDelete = *p;
						p ++;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_DEL_PVOD_EVENT;
						msg.usParam = sizeof(DeletePushVodEvnet);
						msg.ulParam = (HUINT32)pstDelPushVodEvt;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
#endif
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_CLEAR_3PA_DATA:
				{
					Clear3PAData *pstClear3PA;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_CLEAR_3PA_DATA len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstClear3PA = (Clear3PAData *)OxCAS_Malloc(sizeof(Clear3PAData));
						if(pstClear3PA == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstClear3PA->ucClear3PAFlash = *p;
						p ++;
						pstClear3PA->ucClear3PAEEPROM = *p;
						p ++;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_CLEAR_3PA_DATA;
						msg.usParam = sizeof(Clear3PAData);
						msg.ulParam = (HUINT32)pstClear3PA;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_COLOUR_DISPLAY:
				{
					ColourDisplayData *pstColourDisp;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_CLEAR_3PA_DATA len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstColourDisp = (ColourDisplayData *)OxCAS_Malloc(sizeof(ColourDisplayData));
						if(pstColourDisp == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstColourDisp->usRow = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstColourDisp->usColumn = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstColourDisp->ucFlashing = *p;
						p ++;
						pstColourDisp->ucBackType = *p;
						p ++;
						pstColourDisp->ucBackColor = *p;
						p ++;
						pstColourDisp->ucFontColor = *p;
						p ++;
						pstColourDisp->ucDuration = *p;
						p ++;
						pstColourDisp->ucTextLen = *p;
						p ++;
						VK_MEM_Memcpy(pstColourDisp->pucText, p, pstColourDisp->ucTextLen);
						pstColourDisp->pucText[pstColourDisp->ucTextLen] = '\0';
						p += pstColourDisp->ucTextLen;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_COLOUR_DISPLAY;
						msg.usParam = sizeof(ColourDisplayData);
						msg.ulParam = (HUINT32)pstColourDisp;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_CS_PHONE_NO:
				{
					PhoneNumber *pstPhoneNo;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_CS_PHONE_NO len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstPhoneNo = (PhoneNumber *)OxCAS_Malloc(sizeof(PhoneNumber));
						if(pstPhoneNo == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						VK_MEM_Memcpy(pstPhoneNo->pucPhoneNo, p, 16);
						pstPhoneNo->pucPhoneNo[16] = '\0';
						p += 16;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO;
						msg.usParam = sizeof(PhoneNumber);
						msg.ulParam = (HUINT32)pstPhoneNo;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_PPV_PHONE_NO:
				{
					PhoneNumber *pstPhoneNo;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_PPV_PHONE_NO len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstPhoneNo = (PhoneNumber *)OxCAS_Malloc(sizeof(PhoneNumber));
						if(pstPhoneNo == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						VK_MEM_Memcpy(pstPhoneNo->pucPhoneNo, p, 16);
						pstPhoneNo->pucPhoneNo[15] = '\0';
						p += 16;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_PPV_PHONE_NO;
						msg.usParam = sizeof(PhoneNumber);
						msg.ulParam = (HUINT32)pstPhoneNo;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_UPDATE_HDD:
					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_UPDATE_HDD len[%d]\n", usDataLength);
					msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_UPDATE_HDD;
					CAS_IR_EVT_SendEvent(hAct, &msg);
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_FORMAT_HDD:
					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_FORMAT_HDD len[%d]\n", usDataLength);
					msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_FORMAT_HDD;
					CAS_IR_EVT_SendEvent(hAct, &msg);
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_EPG_FEATURES:
				{
					HUINT32 ulFeatureBitmap;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_EPG_FEATURES len[%d]\n", usDataLength);
					if(usDataLength)
					{
						ulFeatureBitmap = CAS_IR_DRV_Get32Bits(p);
						p += 4;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_EPG_FEATURES;
						msg.usParam = usDataLength;
						msg.ulParam = ulFeatureBitmap;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_USER_FOLDER_SETTINGS:
				{
					HUINT16 usUserFolderSize;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_EPG_FEATURES len[%d]\n", usDataLength);
					if(usDataLength)
					{
						usUserFolderSize = CAS_IR_DRV_Get16Bits(p);
						p += 4;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_USER_FOLDER_SETTINGS;
						msg.usParam = usDataLength;
						msg.ulParam = usUserFolderSize;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
				}
				break;
				case IR_UP_EPG_TAG_ITO_DDEMM_SET_OTHER_MODE:
				{
					OtherMode *pstOtherMode;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_SET_OTHER_MODE len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstOtherMode = (OtherMode *)OxCAS_Malloc(sizeof(OtherMode));
						if(pstOtherMode == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstOtherMode->ucEnableOtherMode = *p;
						p ++;
						VK_MEM_Memcpy(pstOtherMode->pucChipID, p, 8);
						p += 8;
						pstOtherMode->ucReserved1 = *p;
						p ++;
						pstOtherMode->ucReserved2 = *p;
						p ++;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_SET_OTHER_MODE;
						msg.usParam = sizeof(OtherMode);
						msg.ulParam = (HUINT32)pstOtherMode;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_EASY_SMS_DISPLAY:
				{
					EasySMSDisplayData *pstEasySMS;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_EASY_SMS_DISPLAY len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstEasySMS = (EasySMSDisplayData *)OxCAS_Malloc(sizeof(EasySMSDisplayData));
						if(pstEasySMS == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstEasySMS->usRow = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstEasySMS->usColumn = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstEasySMS->ucFlashing = *p;
						p ++;
						VK_MEM_Memcpy(pstEasySMS->pucSMSCode, p, 8);
						pstEasySMS->pucSMSCode[8] = '\0';
						p += 8;
						pstEasySMS->ucDuration = *p;
						p ++;
						pstEasySMS->ucTextLen = *p;
						p ++;
						VK_MEM_Memcpy(pstEasySMS->pucText, p, pstEasySMS->ucTextLen);
						pstEasySMS->pucText[pstEasySMS->ucTextLen] = '\0';
						p += pstEasySMS->ucTextLen;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_EASY_SMS_DISPLAY;
						msg.usParam = sizeof(EasySMSDisplayData);
						msg.ulParam = (HUINT32)pstEasySMS;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
#if 0
				case IR_UP_EPG_TAG_ITO_DDEMM_REMOTE_TAG_DS265:
					break;
#endif
				case IR_UP_EPG_TAG_ITO_DDEMM_DIGIAKTIF:
				{
					DigiaktifTaggingData *pstDigiaktifTag;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_DIGIAKTIF len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstDigiaktifTag = (DigiaktifTaggingData *)OxCAS_Malloc(sizeof(DigiaktifTaggingData));
						if(pstDigiaktifTag == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstDigiaktifTag->ucButtonEnable = *p;
						p ++;
						pstDigiaktifTag->ucButtonPosition = *p;
						p ++;
						pstDigiaktifTag->ucButtonId = *p;
						p ++;
						pstDigiaktifTag->usDisplayFrequency = CAS_IR_DRV_Get16Bits(p);
						p += 2;
						pstDigiaktifTag->ucTestMode = *p;
						p ++;
						pstDigiaktifTag->usReserved = CAS_IR_DRV_Get16Bits(p);
						p += 2;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_DIGIAKTIF;
						msg.usParam = sizeof(DigiaktifTaggingData);
						msg.ulParam = (HUINT32)pstDigiaktifTag;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				case IR_UP_EPG_TAG_ITO_DDEMM_SET_FTA_LIMIT:
				{
					FTALimit *pstFtaLimit;

					HxLOG_Print("IR_UP_EPG_TAG_ITO_DDEMM_SET_FTA_LIMIT len[%d]\n", usDataLength);
					if(usDataLength != 0)
					{
						pstFtaLimit = (FTALimit *)OxCAS_Malloc(sizeof(FTALimit));
						if(pstFtaLimit == NULL)
						{
							HxLOG_Critical("\n\n");
							return;
						}

						pstFtaLimit->ucEnableFtaLimit = *p;
						p ++;
						VK_MEM_Memcpy(pstFtaLimit->pucChipID, p, 8);
						p += 8;
						pstFtaLimit->ucDayLimit = *p;
						p ++;
						pstFtaLimit->ucReserved = *p;
						p ++;

						msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_SET_FTA_LIMIT;
						msg.usParam = sizeof(FTALimit);
						msg.ulParam = (HUINT32)pstFtaLimit;
						CAS_IR_EVT_SendEvent(hAct, &msg);
					}
					else
					{
						HxLOG_Critical("\n\n");
					}
				}
					break;
				default:
					HxLOG_Critical("\n\n");
					break;
			}
		}
		else
		{
			HxLOG_Critical("\n\n");
		}
	}
}
#endif

#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_UPC)
void local_cas_ir_ProcUserPayload(Handle_t hAct, HUINT8 *pData, HUINT16 usLength)
{
	HUINT8			*p;
	HUINT8			bCntrMsg_Type;
	HUINT8			ucUserPayloadVersion;
	HUINT16			usQualifLength;
	HUINT8			ucQualifType, ucQualifSubType, ucUserPayloadVersion1;
	HUINT16			usCmdLength;
	HUINT8			ucCmdType, ucCmdSubType;
	HMSG			msg;
	HxCountry_e		eCountryId = eCountryID_HUN;
	PhoneNumber 	*pstPhoneNo;
	HUINT8			ucUsageId = 0, i =0;

	HxLOG_Info("\n");

	HxSTD_memset(&msg, 0, sizeof(HMSG));

	p = pData;
	bCntrMsg_Type = (*p++ >> 4) & 0x0f;

	if(bCntrMsg_Type == CONTROL_MESSAGE_TYPE_USER_PAYLOAD)
	{
		ucUserPayloadVersion = *p++;
		usQualifLength = CAS_IR_DRV_Get16Bits(p);
		p += 2;
		ucQualifType = *p++;
		ucQualifSubType = *p++;
		ucUserPayloadVersion1 = *p++;

		usCmdLength = CAS_IR_DRV_Get16Bits(p);
		p += 2;

		ucCmdType = *p++;
		ucCmdSubType = *p++;

		switch(ucCmdType)
		{
			case IR_UP_CA_CMD_TYPE_UPC_PHONE1:
			case IR_UP_CA_CMD_TYPE_UPC_PHONE2:
			case IR_UP_CA_CMD_TYPE_UPC_PHONE3:
				pstPhoneNo = (PhoneNumber *)CA_MEM_Alloc(sizeof(PhoneNumber));
				if(pstPhoneNo == NULL)
				{
					HxLOG_Critical("\n\n");
					return;
				}

				VK_MEM_Memset(pstPhoneNo->pucPhoneNo, 0x00, 16);
				for(i = 0 ; i < 6 ; i++)
				{
					pstPhoneNo->pucPhoneNo[(i*2)] = (*(p+i) >> 4);
					pstPhoneNo->pucPhoneNo[(i*2)+1] = (*(p+i) & 0x0F);
				}
				pstPhoneNo->pucPhoneNo[13] = '\0';


				if(ucCmdType == IR_UP_CA_CMD_TYPE_UPC_PHONE1)
				{
					eCountryId = eCountryID_HUN;
				}
				else if(ucCmdType == IR_UP_CA_CMD_TYPE_UPC_PHONE2)
				{
					eCountryId = eCountryID_CZE;
				}
				else if(ucCmdType == IR_UP_CA_CMD_TYPE_UPC_PHONE3)
				{
					eCountryId = eCountryID_SVK;
				}

				pstPhoneNo->eCountryId = eCountryId;
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_EPG_CS_PHONE_NO;
				msg.usParam = sizeof(PhoneNumber);
				msg.ulParam = (HUINT32)pstPhoneNo;
				CAS_IR_EVT_SendEvent(hAct, &msg);
				break;

			case IR_UP_CA_CMD_TYPE_UPC_USAGE_ID:
				ucUsageId = *p++;

				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_USAGE_ID;
				msg.usParam = 1;
				msg.ulParam = ucUsageId;

				CAS_IR_EVT_SendEvent(hAct, &msg);
				break;

			default:
				break;
		}
	}

	return;
}
#endif

void local_cas_ir_MSG_DRV_TextMsgProcess(Handle_t hAct, HUINT8 *pData, HUINT16 usLength)
{
	HUINT8    		ucMsgType;
	MAILBOX_TABLE	mail_msg;

	HxLOG_Info("\n");

	HxSTD_memset(&mail_msg, 0, sizeof(MAILBOX_TABLE));

	ucMsgType = pData[0] & 0xf0;

	if( (ucMsgType == EPG_MESSAGE_TYPE_TEXT_MAILBOX) || (ucMsgType == EPG_MESSAGE_TYPE_TEXT_ANNOUNCEMENT) )
	{
	// Parse the mail message
		if( local_cas_ir_MSG_DRV_GetMailMessage(&mail_msg, pData + 1) == FALSE )
		{
			HxLOG_Print("invalid mail data\n");
			return;
		}
	}
	mail_msg.ucType = ucMsgType;

	local_cas_ir_MSG_DRV_MsgProcess(hAct, &mail_msg);
}

void local_cas_ir_MSG_DRV_MsgProcess(Handle_t hAct, MAILBOX_TABLE *pMail_msg)
{
	HUINT8			ucMsgClass;
	HMSG			msg;
	HUINT8			ucMailIndex;
	HBOOL			bAttrFingerPrint = FALSE;

#if defined(IRPLAT_FN_NO_SAVING_SAME_MAIL)
	{
		HUINT8			i;

	HxLOG_Info("\n");

	for ( i = 0; i < MAX_NUMBER_MAIL_MESSAGE; i++ )
	{
		if(s_stIrMailboxInfo.valid[i] == IR_MAIL_VALID_OK && strcmp(s_stIrMailboxInfo.mailbox_table[i].text, pMail_msg->text) == 0)
		{
			return;
		}
	}
	}
#endif

	HxSTD_memset(&msg, 0, sizeof(HMSG));

	/* pasting message class with it by classifying  attributed message type */
	if(pMail_msg->bAttributeMsg == TRUE)
	{
		 //attribute msg 도 기존 routine 을 이용할 수 있도록 함.
		if(pMail_msg->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_NORMAL)
		{
			 //normal attribute일 경우 기존 mail_msg normal 과 동일하게 icon만 표시할 수 있도록
			ucMsgClass = EPG_MESSAGE_CLASS_NORMAL;
		}
		else if(pMail_msg->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FORCEDTEXT)
		{
			 //forced attribute msg인 경우 기존 forced mail msg 와 동일한 msg를 보냄. 단 display 하는
			  //부분에서 attribute msg인지를 확인하여 일반 mail msg와 다른 루틴을 탐
			ucMsgClass = EPG_MESSAGE_CLASS_FORCED_DISPLAY;
		}
		else if(pMail_msg->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT
#if defined(CONFIG_MW_CAS_IRDETO_SUPPORT_COVERT_FINGERPRINT)
		|| pMail_msg->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION
#endif
		)
		{
			ucMsgClass = EPG_MESSAGE_CLASS_FORCED_DISPLAY;
			bAttrFingerPrint = TRUE;
		}
		else
		{
			// see 705410. 6.3
			HxLOG_Print("Currently, this attributed type is not supported.(0x%X)\n", pMail_msg->Atrb_Display.ucMsg_Type );
			return ;
		}
	}
	else
	{
		ucMsgClass = pMail_msg->msg_class;
	}

	// Add the mail message in the table
#if defined(CONFIG_OP_TRUEVISION)
	if (bAttrFingerPrint)
	{
		ucMailIndex = local_cas_ir_MSG_DRV_AddAttrFingerPrint(pMail_msg);

		msg.usMsgClass = IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY;
		msg.usParam = ucMailIndex;

		CAS_IR_EVT_SendEvent(hAct, &msg);
		return;
	}
	else
#endif
	ucMailIndex = local_cas_ir_MSG_DRV_AddMailMessage( pMail_msg );
	if( ucMailIndex < MAX_NUMBER_MAIL_MESSAGE)
	{
		if( ucMsgClass == EPG_MESSAGE_CLASS_NORMAL )
		{
			// ICON만 표시.
			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_NORMAL_MAIL;
		}
		else if( ucMsgClass == EPG_MESSAGE_CLASS_FORCED_DISPLAY )
		{
			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_FORCED_DISPLAY;
		}
		else
		{
			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_NORMAL_MAIL;
		}
		msg.usParam = ucMailIndex;

		CAS_IR_EVT_SendEvent(hAct, &msg);
	}
	else
	{
		HxLOG_Print("Invalid ucMailIndex = %d!!!!\n", ucMailIndex);
	}
}

void local_cas_ir_MSG_DRV_ProcCntrMsg(Handle_t hAct, HUINT8 *pData, HUINT16 usLength)
{
	HUINT8	bCntrMsg_Type;
	HUINT8	Download_data;
	HUINT8	PinCode_Index;
	HUINT16	PinCode;
	HUINT8	RecoveryType;
	HMSG	msg;

	HxLOG_Info("\n");

	HxSTD_memset(&msg, 0, sizeof(HMSG));

	bCntrMsg_Type = (pData[0] >> 4) & 0x0f;

	switch (bCntrMsg_Type)
	{
		case CONTROL_MESSAGE_TYPE_DOWNLOAD_CONTROL:
			HxLOG_Print("DOWNLOAD_CONTROL : pData[1] = 0x%X\n", pData[1] );

			Download_data = pData[1];
			if( Download_data & 0x40 )
			{
				HxLOG_Print("FORCED_DOWNLOAD\n");
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_FORCED_DOWNLOAD;
				CAS_IR_EVT_SendEvent(hAct, &msg);
			}
			else
			{
				if( Download_data & 0x80 )
				{
					HxLOG_Print("DOWNLOAD_ALLOWED_ENABLE\n");
					msg.usMsgClass = IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_ENABLE;
					msg.usParam = 0;
					CAS_IR_EVT_SendEvent(hAct, &msg);
				}
				else
				{
					HxLOG_Print("DOWNLOAD_ALLOWED_DISABLE\n");
					msg.usMsgClass = IREVT_CLASS_DECODER_MSG_DOWNLOAD_ALLOWED_DISABLE;
					msg.usParam = 0;
					CAS_IR_EVT_SendEvent(hAct, &msg);
				}
			}
			break;

		case CONTROL_MESSAGE_TYPE_CHANGE_PINCODE:
			HxLOG_Print("CHANGE_PINCODE\n");

			PinCode_Index = pData[1];
			PinCode	= (((HUINT16)pData[2] << 8 ) | (HUINT16)pData[3] );
			// App에서 BCD로 처리하므로 converting하지 않고 BCD를 바로 올림.
			//ConvertedPinCode = ((((PinCode) >> 12) & 0x0f) * 1000 + (((PinCode) >> 8) & 0x0f) * 100 + (((PinCode) >> 4) & 0x0f) * 10 + ((PinCode) & 0xf));

			HxLOG_Print("PIN [0x%X]\n", PinCode);

			msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE;
			msg.usParam = PinCode;
			CAS_IR_EVT_SendEvent(hAct, &msg);
			break;

		case CONTROL_MESSAGE_TYPE_DECODER_RECOVERY:
			HxLOG_Print("DECODER_RECOVERY : pDdata[1] = 0x%X\n", pData[1]);

			RecoveryType = (pData[1] >> 4) & 0x0f;
			if( RecoveryType == IR_CTRL_MSG_RECOVERY_RESET_IRD )
			{
				//CARM_SYS_Reboot();
				HxLOG_Print("[IR_CTRL_MSG_RECOVERY_RESET_IRD]\n");
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_RESET_IRD;
				CAS_IR_EVT_SendEvent(hAct, &msg);
			}
			else if( RecoveryType == IR_CTRL_MSG_RECOVERY_RESET_CH_ORDER ) //reset channel order
			{
				HUINT16 usBID;

				HxLOG_Print("[IR_CTRL_MSG_RECOVERY_RESET_CH_ORDER]\n");
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_RESET_CH_ORDER;
				usBID = (((HUINT16)pData[2] << 8 ) | (HUINT16)pData[3] );
#if defined(CONFIG_PROD_ENDIAN_LITTLE)
				usBID = CAS_IR_DRV_Get16Bits(&pData[2]);
#else
				usBID = *(unsigned short*)(&pData[2]);
#endif
				msg.usParam = usBID;
				CAS_IR_EVT_SendEvent(hAct, &msg);
			}
			else if( RecoveryType == IR_CTRL_MSG_RECOVERY_RESCAN ) //rescan service
			{
				HxLOG_Print("[IR_CTRL_MSG_RECOVERY_RESCAN]\n");
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_RESCAN;
				CAS_IR_EVT_SendEvent(hAct, &msg);
			}
			else if ( RecoveryType == IR_CTRL_MSG_RECOVERY_RESCAN_FROM_HOME)
			{
				HxLOG_Print("[IR_CTRL_MSG_RECOVERY_RESCAN_FROM_HOME]\n");
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_RESCAN_FROM_HOME;
				CAS_IR_EVT_SendEvent(hAct, &msg);
			}
			else if( RecoveryType == IR_CTRL_MSG_RECOVERY_RECONNECT) //reconnect service
			{
				HxLOG_Print("[IR_CTRL_MSG_RECOVERY_RECONNECT]\n");
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_RECONNECT;
				CAS_IR_EVT_SendEvent(hAct, &msg);
			}
#if defined(IRPLAT_FN_DDEMM_FORCE_TUNE)
			else if( RecoveryType == IR_CTRL_MSG_RECOVERY_FORCE_TUNE) //force tune
			{
#if defined(CONFIG_MW_CH_CABLE) || defined(CONFIG_OP_DIGITURK)
				unsigned short nid, tsid, svcid;

#if defined(CONFIG_PROD_ENDIAN_LITTLE)
				nid = CAS_IR_DRV_Get16Bits(&pData[2]);
				tsid = CAS_IR_DRV_Get16Bits(&pData[4]);
				svcid = CAS_IR_DRV_Get16Bits(&pData[6]);
#else
				nid = *(unsigned short*)(&pData[2]);
				tsid = *(unsigned short*)(&pData[4]);
				svcid = *(unsigned short*)(&pData[6]);
#endif
				HxLOG_Print("Force-Tune : nid=0x%x, tsid=0x%x, svcid=0x%x\n", nid, tsid, svcid);

				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_FORCE_TUNE;
				msg.usParam = (HUINT32)nid;
				msg.ulParam = (HUINT32)((tsid << 16) | svcid );
				CAS_IR_EVT_SendEvent(hAct, &msg);
#else
				HxLOG_Print("The Force TUNE is currently supported in only cable box.!!!\n");
#endif
			}
#endif
			else if( RecoveryType == IR_CTRL_MSG_RECOVERY_FORCE_SET_PIN) //set installer pincode
			{
				// in octo installer pin & parental pin shared the same value.
				//PinCode_Index = pData[1];
				PinCode	= (((HUINT16)pData[2] << 8 ) | (HUINT16)pData[3] );
				// App에서 BCD로 처리하므로 converting하지 않고 BCD를 바로 올림.
				//ConvertedPinCode = ((((PinCode) >> 12) & 0x0f) * 1000 + (((PinCode) >> 8) & 0x0f) * 100 + (((PinCode) >> 4) & 0x0f) * 10 + ((PinCode) & 0xf));

				HxLOG_Print("PIN [0x%X]\n", PinCode);
				msg.usMsgClass = IREVT_CLASS_DECODER_MSG_CHANGE_PINCODE;
				msg.usParam = PinCode;
				CAS_IR_EVT_SendEvent(hAct, &msg);
			}
			else
			{
				HxLOG_Print("It's not defined recovery type (0x%X) \n", RecoveryType);
			}

			break;

		case CONTROL_MESSAGE_TYPE_READ_SC_USER_DATA:
			HxLOG_Print("CONTROL_MESSAGE_TYPE_READ_SC_USER_DATA \n");

			if(HIR_OK != CAS_IR_DRV_QueryScData(0) )
			{
				HxLOG_Print("Query SC data error\n");
			}

#if defined(CONFIG_MW_CAS_IRDETO_HOMINGCHANNEL)
			if(HIR_OK != CAS_IR_DRV_HomingChannelQuery(0) )
			{
				HxLOG_Print("Query to homming channel by SC: error\n");
			}
#endif

			if(HIR_OK != CAS_IR_DRV_RegionQuery (0, 0) )
			{
				HxLOG_Print("Query to region : error\n");
			}

			break;

		case CONTROL_MESSAGE_TYPE_CALLBACK:
		case CONTROL_MESSAGE_TYPE_MONITOR_MODE:
			break;
#if defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_DIGITURK) || defined(IRPLAT_FN_DDEMM_USER_PAYLOAD_FOR_UPC)
		case CONTROL_MESSAGE_TYPE_USER_PAYLOAD:
			local_cas_ir_ProcUserPayload(hAct, pData, usLength);
			break;
#endif
	}
}

#if 0
static HERROR local_cas_ir_MSG_DRV_CopyMailTextWithLocaleCode(HUINT8	*szMail, HUINT8 *pucData, HUINT16 *ulLength)
{
#if defined(CONFIG_OP_RUSSIA_TVT) || defined(CONFIG_MW_CAS_IRDETO_USE_WINCP1256_MAIL_MSG)
	HUINT8	ulLocaleCodeLength = 3;
#else
	HUINT8	ulLocaleCodeLength = 0;
#endif

	if (*ulLength >= MAX_MAIL_TEXT_LENGTH)
	{
		*ulLength = MAX_MAIL_TEXT_LENGTH - 1; //for NULL char at the end of string
		*ulLength -= ulLocaleCodeLength;
	}

#if defined(CONFIG_OP_RUSSIA_TVT)
	// for Russian(Cyrillic) mail, 3byte 형식의 locale code (OCTO에서 처리하는 WIN CP 1251을 위함)
	*(szMail++) = 0x10;
	*(szMail++) = 0x5A;
	*(szMail++) = 0x45;
#elif defined(CONFIG_MW_CAS_IRDETO_USE_WINCP1256_MAIL_MSG)
	// for Arabic mail, 3byte 형식의 locale code (OCTO에서 처리하는 WIN CP 1256을 위함)
	*(szMail++) = 0x10;
	*(szMail++) = 0x5A;
	*(szMail++) = 0x46;
#endif

	HxSTD_memcpy(szMail, pucData, *ulLength);
	szMail[*ulLength] = '\0';

	*ulLength += ulLocaleCodeLength;

	return ERR_OK;
}
#endif

static HERROR local_cas_ir_MSG_DRV_CopyMailText(HUINT8	*szMail, HUINT8 *pucData, HUINT16 *ulLength)
{

	if (*ulLength >= MAX_MAIL_TEXT_LENGTH)
	{
		*ulLength = MAX_MAIL_TEXT_LENGTH - 1; //for NULL char at the end of string

	}

	HxSTD_memcpy(szMail, pucData, *ulLength);
	szMail[*ulLength] = '\0';



	return ERR_OK;
}

/*******************************************************************************************************/
//      Function : BOOL local_cas_ir_MSG_DRV_GetMailMessage() -> Get the information of mail message.
/*******************************************************************************************************/
HBOOL local_cas_ir_MSG_DRV_GetMailMessage(MAILBOX_TABLE *mail_msg, HUINT8 *pbData)
{
	HUINT8		boRet;
	HUINT16		wIx, wYear;
	HUINT32		time_date;

	HxLOG_Info("\n");

	wIx = 0;
	boRet = UTIL_GET_BITS( pbData[wIx], BIT_POS_FLUSH_BUFFER, BIT_MASK_FLUSH_BUFFER );
	mail_msg->msg_class = UTIL_GET_BITS( pbData[wIx], BIT_POS_MSG_CLASS, BIT_MASK_MSG_CLASS );
	mail_msg->compressed = UTIL_GET_BITS( pbData[wIx], BIT_POS_COMPRESSED, BIT_MASK_COMPRESSED );
	mail_msg->club_msg = UTIL_GET_BITS( pbData[wIx], BIT_POS_CLUB_MSG, BIT_MASK_CLUB_MSG );
	mail_msg->priority = UTIL_GET_BITS( pbData[wIx], BIT_POS_MSG_PRIORITY, BIT_MASK_MSG_PRIORITY );

	wIx = wIx + 1;

	HxLOG_Print("\n\t\t  local_cas_ir_MSG_DRV_GetMailMessage !! \n");
	HxLOG_Print("boRet : %x \n", boRet);
	HxLOG_Print("msg_class : %x \n", mail_msg->msg_class);
	HxLOG_Print("compressed : %x \n", mail_msg->compressed);
	HxLOG_Print("club_msg: %x \n", mail_msg->club_msg);
	HxLOG_Print("priority: %x \n", mail_msg->priority);

	if ( mail_msg->msg_class == EPG_MESSAGE_CLASS_TIMED )  //Get the expired time if it is a timed message
	{
		time_date = CAS_IR_DRV_Get24Bits( pbData + wIx );
		wYear = UTIL_GET_BITS( time_date, BIT_POS_YEAR, BIT_MASK_YEAR );
		mail_msg->del_date.usYear   = wYear + ((wYear < 95) ? 2000:1900);
		mail_msg->del_date.ucMonth = UTIL_GET_BITS( time_date, BIT_POS_MONTH, BIT_MASK_MONTH );
		mail_msg->del_date.ucDay   = UTIL_GET_BITS( time_date, BIT_POS_DAY, BIT_MASK_DAY );

		mail_msg->del_time.ucHour   = UTIL_GET_BITS( time_date, BIT_POS_HOUR, BIT_MASK_HOUR );
		mail_msg->del_time.ucMinute = UTIL_GET_BITS( time_date, BIT_POS_MINUTE, BIT_MASK_MINUTE );
		mail_msg->del_time.ucSecond = 0;
		wIx = wIx + 3;
	}

	if ( mail_msg->club_msg == TRUE )   // message is only for specific club ?
	{
		mail_msg->club = CAS_IR_DRV_Get16Bits( pbData + wIx );
		wIx = wIx + 2;
	}
	mail_msg->msg_length = pbData[wIx++];

	HxLOG_Print("msg_length : %d \n", mail_msg->msg_length);

#if 0
// arabic test code
	pbData[wIx + 0] = 0xc6;
	pbData[wIx + 1] = 0xc7;
	pbData[wIx + 2] = 0xc8;
	pbData[wIx + 3] = 0xc9;
	pbData[wIx + 4] = 0xcf;
	pbData[wIx + 5] = 0xd3;
	pbData[wIx + 6] = 0xdb;
	pbData[wIx + 7] = 0xdd;
	pbData[wIx + 8] = 0xe4;
	pbData[wIx + 9] = 0xe6;

	mail_msg->msg_length=10;



	{
		HUINT8 i=0;
		HxLOG_Info("local_cas_ir_MSG_DRV_GetMailMessage] raw mail message string: ");
		for(i=0; i<mail_msg->msg_length; i++)
		{
			HxLOG_Info("%x ", pbData[wIx + i]);
		}
		HxLOG_Info("\n");

	}

#endif


	if(mail_msg->msg_length >= MAX_NORMAL_TEXT_LENGTH)
	{
		HxLOG_Print("\n\t\t  TEXT LENGTH ERROR !! \n");
		boRet = FALSE;
		mail_msg->text[0] = '\0';
	}
	else
	{
		// TODO: shko  일단 locale code 생각하지 않는다. web ui 에서 처리?
		//local_cas_ir_MSG_DRV_CopyMailTextWithLocaleCode(mail_msg->text, &(pbData[wIx]), &mail_msg->msg_length);
		local_cas_ir_MSG_DRV_CopyMailText(mail_msg->text, &(pbData[wIx]), &mail_msg->msg_length);
		boRet = TRUE;
	}

	return boRet;
}


HUINT8 local_cas_ir_MSG_DRV_AddAttrFingerPrint (MAILBOX_TABLE *mail_msg)
{
	HxSTD_memcpy(&s_stIrAttrFingerPrint, mail_msg, SIZE_MAILBOX_TABLE);
	s_ucAttrFpIdx = (s_ucAttrFpIdx + 1) % MAX_NUMBER_MAIL_MESSAGE;
	return IRMSG_ATTR_FP_IDX(s_ucAttrFpIdx);
}


HUINT8 local_cas_ir_MSG_DRV_AddMailMessage( MAILBOX_TABLE *mail_msg )
{

	static UNIXTIME	sulPrevUnixTime = 0;
	static unsigned short	addMillisecond = 0;

	UNIXTIME		curUnixtime = 0;
	HxDATETIME_Time_t		curTime = {0, };
	HxDATETIME_Date_t		curDate = {0, };
	HIR_ERR		hErr = ERR_FAIL;

	HUINT8			ucMailIndex = MAX_NUMBER_MAIL_MESSAGE;
	HUINT16			i = 0;
	HUINT16			j = 0;
	HUINT16			crc16 = 0;
	//	HUINT32			nExpiredMailTimerId;


	HxLOG_Print("MAILMSG - local_cas_ir_MSG_DRV_AddMailMessage\n");

	// Get the arrival time
	// TODO: check Time API . in octo.
	if (VK_OK ==  VK_CLOCK_GetTime( (HULONG *)&curUnixtime)  )
	{
		HxLOG_Print("[GET TIME SUCCESS]\n");

		hErr  = HLIB_DATETIME_ConvertUnixTimeToTime(curUnixtime, &curTime);
		hErr |= HLIB_DATETIME_ConvertUnixTimeToDate(curUnixtime, &curDate);

		if(hErr != HIR_OK)
		{
			HxLOG_Critical("\n\n");
		}

		// 초까지 동일하면 나중에 받은 mail message가 먼저 받은 것처럼 표시되는 문제가 있어서 수정.
		if( curUnixtime == sulPrevUnixTime )
		{
			curTime.usMillisecond = addMillisecond++;
		}
		else
		{
			addMillisecond = 0;
		}
	}
	else
	{
		HxLOG_Print("\t\t\t\tMAILMSG - GET TIME FAIL\n");
		// TDT를 받기 전이거나 TDT가 없는 방송의 경우에 mail을 받으면 이전에 받은 mail중에 last mail의 시간 + 1분,
		// 이전에 받은 mail이 없으면 default time( 2003.1.1 0:0:0 )으로 setting.
		//주석상에는 왜 2003.1.1 일까. 그냥 0.0.0 으로 일단 둠.
		local_cas_ir_MSG_DRV_GetLastMailTime(&curUnixtime, &curDate, &curTime);
	}

	sulPrevUnixTime = curUnixtime;

	ucMailIndex = local_cas_ir_MSG_DRV_GetEmptyRoomOfMailboxTable();
	HxLOG_Print("\t\tMAILMSG - local_cas_ir_MSG_DRV_GetEmptyRoomOfMailboxTable : ucMailIndex = %d\n", ucMailIndex);

	if( ucMailIndex == MAX_NUMBER_MAIL_MESSAGE  )
	{
		// Check the expired mail message : timed mail이 있다면 현재 시간과 비교해서 지워버리자.
		ucMailIndex = local_cas_ir_MSG_DRV_FindFirstExpiredMailMessage( curUnixtime, &curDate, &curTime );
		if ( ucMailIndex < MAX_NUMBER_MAIL_MESSAGE )
		{
			HxLOG_Print("\t\tMAILMSG - MAIL DELETE1 : ucMailIndex = %d\n", ucMailIndex);
			HxSTD_memset((HUINT8 *)&(s_stIrMailboxInfo.mailbox_table[ucMailIndex]), 0, SIZE_MAILBOX_TABLE);
		}
		else
		{
			// If no room, then delete the lowest priority
			ucMailIndex = local_cas_ir_MSG_DRV_FindLowestPriorityMailMessage();
			if (ucMailIndex >= MAX_NUMBER_MAIL_MESSAGE)
			{
				HxLOG_Critical("\n\n");
				ucMailIndex = 0;
			}
			HxLOG_Print("\t\tMAILMSG - MAIL DELETE2 : ucMailIndex = %d\n", ucMailIndex);
			HxSTD_memset((HUINT8 *)&(s_stIrMailboxInfo.mailbox_table[ucMailIndex]), 0, SIZE_MAILBOX_TABLE);
		}
	}

	HxLOG_Print("Add_MailMsg ucMailIndex= %d \n", ucMailIndex);

	if( ucMailIndex < MAX_NUMBER_MAIL_MESSAGE )
	{
		HxSTD_memcpy( (HUINT8 *)(&(s_stIrMailboxInfo.mailbox_table[ucMailIndex])), (HUINT8 *)mail_msg, SIZE_MAILBOX_TABLE);

		s_stIrMailboxInfo.mailbox_table[ucMailIndex].unixtime				= curUnixtime;
		HxSTD_memcpy(&(s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_date), &curDate, sizeof(HxDATETIME_Date_t));
		HxSTD_memcpy(&( s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_time), &curTime, sizeof(HxDATETIME_Time_t));

		/*		s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_date.usYear         = curDate.usYear;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_date.ucMonth       = curDate.ucMonth;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_date.ucDay         = curDate.ucDay;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_time.ucHour        = curTime.ucHour;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_time.ucMinute      = curTime.ucMinute;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_time.ucSecond      = curTime.ucSecond;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].add_time.usMillisecond 	= curTime.usMillisecond;*/

		HxLOG_Print("[ucMailIndex = %d : DATE = %d:%d:%d TIME = %d:%d:%d:%d]\n", ucMailIndex, curDate.usYear, curDate.ucMonth, curDate.ucDay, curTime.ucHour, curTime.ucMinute, curTime.ucSecond, curTime.usMillisecond);
		HxLOG_Print("[Delete DATE = %d:%d:%d TIME = %d:%d:%d:%d]\n", mail_msg->del_date.usYear, mail_msg->del_date.ucMonth, mail_msg->del_date.ucDay,mail_msg->del_time.ucHour, mail_msg->del_time.ucMinute, mail_msg->del_time.ucSecond, mail_msg->del_time.usMillisecond);

		HxSTD_memcpy(&(s_stIrMailboxInfo.mailbox_table[ucMailIndex].del_date), &(mail_msg->del_date), sizeof(HxDATETIME_Date_t));
		HxSTD_memcpy(&( s_stIrMailboxInfo.mailbox_table[ucMailIndex].del_time), &(mail_msg->del_time), sizeof(HxDATETIME_Time_t));

		/*       s_stIrMailboxInfo.mailbox_table[ucMailIndex].del_date.usYear    = mail_msg->del_date.usYear;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].del_date.ucMonth   = mail_msg->del_date.ucMonth;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].del_date.ucDay     = mail_msg->del_date.ucDay;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].del_time.ucHour    = mail_msg->del_time.ucHour;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].del_time.ucMinute  = mail_msg->del_time.ucMinute;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].del_time.ucSecond  = mail_msg->del_time.ucSecond;*/

		s_stIrMailboxInfo.mailbox_table[ucMailIndex].priority   = mail_msg->priority;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].msg_class  = mail_msg->msg_class;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].compressed 	= mail_msg->compressed;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].club       = mail_msg->club;
		s_stIrMailboxInfo.mailbox_table[ucMailIndex].ucType = mail_msg->ucType;

		s_stIrMailboxInfo.mailbox_table[ucMailIndex].bAttributeMsg = mail_msg->bAttributeMsg;
		HxSTD_memcpy(&(s_stIrMailboxInfo.mailbox_table[ucMailIndex].Atrb_Display), &(mail_msg->Atrb_Display), sizeof(ATTRIBUTED_DISPLAY));

		HxLOG_Info("mail_msg->text: %s, mail_msg->msg_length: %d \n", mail_msg->text, mail_msg->msg_length);
		HxLOG_Info("s_stIrMailboxInfo.mailbox_table[%d]: %s, s_stIrMailboxInfo.mailbox_table[ucMailIndex].msg_length: %d \n", ucMailIndex, s_stIrMailboxInfo.mailbox_table[ucMailIndex].text, s_stIrMailboxInfo.mailbox_table[ucMailIndex].msg_length);


#if defined(IRPLAT_FN_STANDBY_SET_MAIL)
		{
			HUINT16 usDcmPowerMode;
			HUINT32 ulError;

			ulError = DCM_ReadPowerMode(&usDcmPowerMode);
			if (ulError == HIR_OK)
			{
				if ((usDcmPowerMode & 0xFF) == POWER_NORMAL && mail_msg->msg_class == EPG_MESSAGE_CLASS_FORCED_DISPLAY)
				{
				s_stIrMailboxInfo.bReadStatus[ucMailIndex] = IR_MAIL_READ_STATUS_READ;
				}
			}
		}
#endif

		for (i = 0, j = 0; i < MAX_NUMBER_MAIL_MESSAGE; i++)
		{
			if ( (s_stIrMailboxInfo.aucMailIndexList[i] != 0xFF)
			&&( s_stIrMailboxInfo.aucMailIndexList[i] != ucMailIndex)
			&&( s_stIrMailboxInfo.valid[s_stIrMailboxInfo.aucMailIndexList[i]] == IR_MAIL_VALID_OK) )
			{
				s_stIrMailboxInfo.aucMailIndexList[j] = s_stIrMailboxInfo.aucMailIndexList[i];
				j++;
			}
		}
		for (i = j; i < MAX_NUMBER_MAIL_MESSAGE; i++)
		{
			s_stIrMailboxInfo.aucMailIndexList[i] = 0xFF;
		}

		s_stIrMailboxInfo.aucMailIndexList[j] = ucMailIndex;
		s_stIrMailboxInfo.valid[ucMailIndex]      = IR_MAIL_VALID_OK;
		s_stIrMailboxInfo.bReadStatus[ucMailIndex] = IR_MAIL_READ_STATUS_NOT_READ;
		s_stIrMailboxInfo.ucListValid = TRUE;

		crc16 = HLIB_MATH_GetCrc16 (&(s_stIrMailboxInfo.mailbox_table), MAX_NUMBER_MAIL_MESSAGE * sizeof(MAILBOX_TABLE));
		s_stIrMailboxInfo.crc16 = crc16;

		HxLOG_Print("\t\tMAILMSG : Save Mail Info\n");
		local_MSG_DRV_StorageUserMailInfoWrite();


	}
	else
	{
		HxLOG_Print("\t\tMAILMSG - ERROR : local_cas_ir_MSG_DRV_AddMailMessage - ucMailIndex = %d\n", ucMailIndex);
		HxLOG_Critical("\n\n");
	}

	HxLOG_Print("MAILMSG - local_cas_ir_MSG_DRV_AddMailMessage : FINISH\n");

	return ucMailIndex;
}


/*******************************************************************************************************/
//      Function : int local_cas_ir_MSG_DRV_GetEmptyRoomOfMailboxTable() -> Get the index of empty room in table.
/*******************************************************************************************************/
HUINT8 local_cas_ir_MSG_DRV_GetEmptyRoomOfMailboxTable(  void)
{
	HUINT8 i;

	for ( i = 0; i < MAX_NUMBER_MAIL_MESSAGE; i++ )
	{
		if( s_stIrMailboxInfo.valid[i] == IR_MAIL_VALID_NOT_OK )
		{
			break;
		}
	}

	return i;
}

/************************************************************************************************************************/
//      Function : int CA_NODE_Am_FindLowestPriortyMailMessage() -> Find the index of the lowest priority mail message.
/************************************************************************************************************************/

HUINT8 local_cas_ir_MSG_DRV_FindLowestPriorityMailMessage( void )
{
	HUINT8		i;
	HUINT8		ucLowest;
	HUINT8		aucRead[MAX_NUMBER_MAIL_MESSAGE];
	HUINT8		ucReadCount;
	HUINT8		aucPrioGroup[MAX_NUMBER_MAIL_MESSAGE];
	HUINT8		ucPrioCount = 0;
	HUINT8		ucLowestPrior;
	UNIXTIME	earliestTime;
	MAILBOX_TABLE	*pMail;

	//2 1. 읽은 것들만 찾고,
	for (i = 0, ucReadCount = 0 ; i < MAX_NUMBER_MAIL_MESSAGE ; i++)
	{
		if (s_stIrMailboxInfo.valid[i] != IR_MAIL_VALID_OK)
		{
			return i;// vaild 하지 않은 인덱스는 무조건!!!
		}
		if (s_stIrMailboxInfo.bReadStatus[i] == IR_MAIL_READ_STATUS_READ)
		{
			aucRead[ucReadCount++] = i;
		}
	}
	if (ucReadCount == 0)
	{
		for (i = 0 ; i < MAX_NUMBER_MAIL_MESSAGE ; i++)
			aucRead[ucReadCount++] = i;
	}

	//2 2. 낮은 우선순위들만 찾고,
	ucLowestPrior = 0xFF;
	for (i = 0 ; i < ucReadCount ; i++)
	{
		pMail = &(s_stIrMailboxInfo.mailbox_table[aucRead[i]]);
		if (pMail->priority < ucLowestPrior)
		{
			ucLowestPrior = pMail->priority;
			ucPrioCount = 0;
		}
		aucPrioGroup[ucPrioCount++] = aucRead[i];
	}
	if (ucPrioCount == 0)
	{
		HxSTD_memcpy(aucPrioGroup, aucRead, sizeof(HUINT8) * MAX_NUMBER_MAIL_MESSAGE);
		ucPrioCount = ucReadCount;
	}

	//2 3. 그중 잴 먼저 들어온 놈
	ucLowest		= 0xFF;
	earliestTime	= 0xFFFFFFFF;
	for (i = 0 ; i < ucPrioCount ; i++)
	{
		pMail = &(s_stIrMailboxInfo.mailbox_table[aucPrioGroup[i]]);
		if (pMail->unixtime < earliestTime)
		{
			earliestTime = pMail->unixtime;
			ucLowest = aucPrioGroup[i];
		}
	}
	return ucLowest;
}

/******************************************************************************************************/
//      Function : int local_cas_ir_MSG_DRV_FindFirstExpiredMailMessage() -> Find the expired mail message.
/******************************************************************************************************/
HUINT8	local_cas_ir_MSG_DRV_FindFirstExpiredMailMessage(UNIXTIME curunixtime, HxDATETIME_Date_t* curDate, HxDATETIME_Time_t* curTime)
{
	HUINT8		i;
	HUINT32		ulDt0, ulDt1, ulTm0, ulTm1;

	ulDt0 = (curDate->usYear << 16)|(curDate->ucMonth << 8)|(curDate->ucDay);
	ulTm0 = (curTime->ucHour << 16)|(curTime->ucMinute << 8)|(curTime->ucSecond);

	for ( i = 0; i < MAX_NUMBER_MAIL_MESSAGE; i++ )
	{
		if ( s_stIrMailboxInfo.valid[i] == IR_MAIL_VALID_OK && s_stIrMailboxInfo.mailbox_table[i].msg_class == EPG_MESSAGE_CLASS_TIMED )
		{
			ulDt1 = s_stIrMailboxInfo.mailbox_table[i].del_date.usYear;
			ulDt1 = (ulDt1 << 8) | s_stIrMailboxInfo.mailbox_table[i].del_date.ucMonth;
			ulDt1 = (ulDt1 << 8) | s_stIrMailboxInfo.mailbox_table[i].del_date.ucDay;

			ulTm1 = s_stIrMailboxInfo.mailbox_table[i].del_time.ucHour;
			ulTm1 = (ulTm1 << 8) | s_stIrMailboxInfo.mailbox_table[i].del_time.ucMinute;
			ulTm1 = (ulTm1 << 8) | s_stIrMailboxInfo.mailbox_table[i].del_time.ucSecond;

			if ( ulDt0 >= ulDt1 && ulTm0 > ulTm1 )
				break;
		}
	}

	if (i == MAX_NUMBER_MAIL_MESSAGE)
	{
		return MAX_NUMBER_MAIL_MESSAGE;
	}

	HxLOG_Print("[local_cas_ir_MSG_DRV_FindFirstExpiredMailMessage : %d]\n", i);
	return i;
}

void local_cas_ir_MSG_DRV_GetLastMailTime( UNIXTIME *punixtime, HxDATETIME_Date_t *dt, HxDATETIME_Time_t *tm)
{
	HUINT8			i;
	HUINT8			index;
	HxDATETIME_t	dateTime;

	index = 0;
	if(s_stIrMailboxInfo.ucListValid)
	{
		for (i = 0; i < MAX_NUMBER_MAIL_MESSAGE; i++)
		{
			if ( s_stIrMailboxInfo.valid[s_stIrMailboxInfo.aucMailIndexList[i]] == IR_MAIL_VALID_OK)
			{
				index = s_stIrMailboxInfo.aucMailIndexList[i];
			}
		}
	}

	if( s_stIrMailboxInfo.valid[index] == IR_MAIL_VALID_OK)
	{
		HxLOG_Print("[MAIL EXIST]\n");
		HxSTD_memcpy((HUINT8 *)dt, (HUINT8 *)(&(s_stIrMailboxInfo.mailbox_table[index].add_date)), sizeof(HxDATETIME_Date_t));
		HxSTD_memcpy((HUINT8 *)tm, (HUINT8 *)(&(s_stIrMailboxInfo.mailbox_table[index].add_time)), sizeof(HxDATETIME_Time_t));

		tm->ucMinute += 1;
		if(tm->ucMinute >= 60)
		{
			tm->ucMinute = 0;
			tm->ucHour += 1;

			if(tm->ucHour >= 24)
			{
				tm->ucHour = 0;
				dt->ucDay += 1;

				if(dt->ucDay > 28)	//어차피 TDT 못 받았을 경우 예외처리니까..
				{
					dt->ucDay = 1;
					dt->ucMonth += 1;

					if(dt->ucMonth > 12)
					{
						dt->ucMonth = 1;
						dt->usYear += 1;
					}

				}
			}

		}
		*punixtime = s_stIrMailboxInfo.mailbox_table[index].unixtime;
	}
	else
	{
		HxLOG_Print("[MAIL DOESN'T EXIST]\n");
		HxSTD_memcpy(&(dateTime.stDate), dt, sizeof(HxDATETIME_Date_t));
		HxSTD_memcpy(&(dateTime.stTime), tm, sizeof(HxDATETIME_Time_t));

		if(HIR_OK != HLIB_DATETIME_ConvertDateTimeToUnixTime(&dateTime, punixtime) )
		{
			HxLOG_Critical("\n\n");
		}

	}
}

#define ________EMM_FINGERPRINT__________________________________________________

HBOOL local_cas_ir_MSG_DRV_GetAttributedDisplay(MAILBOX_TABLE *mail_msg, HUINT8 *pbData)
{
	HUINT16		wIx;
	HUINT8		ucTemp = 0, ucTmpMethod;

	wIx = 0;

	//msg type, 1 byte
	mail_msg->Atrb_Display.ucMsg_Type = pbData[wIx];
	HxLOG_Print("MsgType = %02x \n", pbData[wIx]);
	wIx++;

	mail_msg->ucType = mail_msg->Atrb_Display.ucMsg_Type;

	/*if(mail_msg->Atrb_Display.ucMsg_Type == ATTRIBUTED_MESSAGE_TYPE_FORCEDTEXT)
	mail_msg->ucType = ATTRIBUTED_MESSAGE_TYPE_FORCEDTEXT;
	else
	mail_msg->ucType = ATTRIBUTED_MESSAGE_TYPE_NORMAL;*/

	// extract Duration, 2byte
	mail_msg->Atrb_Display.unDuration = ( (pbData[wIx] << 8 ) | pbData[wIx+1] ) & 0xFFFF;
	HxLOG_Print("Duration = %02x \n", mail_msg->Atrb_Display.unDuration);
	wIx+=2;

	// extract Display Method, 1byte
	mail_msg->Atrb_Display.ucDisplay_Method =  pbData[wIx];
	ucTmpMethod = pbData[wIx];
	HxLOG_Print("Display Method = %02x \n", mail_msg->Atrb_Display.ucDisplay_Method);
	wIx++;

	// attributed 속성 parsing
	HxLOG_Print("nMethod = %02x\n", ucTmpMethod);

	ucTemp = ucTmpMethod & 0x01;

	if(ucTemp > 0)
	{
		mail_msg->Atrb_Display.bFlashing = FALSE;
	}
	else
	{
		mail_msg->Atrb_Display.bFlashing = TRUE;
	}

	HxLOG_Print("Flashing = %02x\n", mail_msg->Atrb_Display.bFlashing);

	ucTemp = ucTmpMethod & 0x02;
	if(ucTemp > 0)
	{
		mail_msg->Atrb_Display.bNormal = TRUE;
	}
	else
	{
		mail_msg->Atrb_Display.bNormal = FALSE;
	}

	HxLOG_Print("bNormal = %02x\n", mail_msg->Atrb_Display.bNormal);

	ucTemp = ucTmpMethod & 0xfc;
	ucTemp = ucTemp >> 2;

	//0-20 일 경우 20%
	if(ucTemp <= 20)
	{
		mail_msg->Atrb_Display.ucCoverage = 20;
	}
	//21-62일 경우 해당 비율대로 21-99% 를 조절
	else if(ucTemp <=62)
	{
		mail_msg->Atrb_Display.ucCoverage = 21 +((78*(ucTemp-21))/41);
	}
	//63일 경우 100%
	else
	{
		mail_msg->Atrb_Display.ucCoverage = 100;
	}

	HxLOG_Print("ucTemp = 0x%02x \n", ucTemp);
	HxLOG_Print("ucCoverage = 0x%02x\n", mail_msg->Atrb_Display.ucCoverage);

	//Fingerprint_Type (1:Covert, 0:Overt)
	ucTmpMethod = pbData[wIx];
	ucTemp = ucTmpMethod >> 7;
//	ucTemp = ucTmpMethod & 0x01;

	if(ucTemp > 0)
	{
		mail_msg->Atrb_Display.ucFingerprint_Type = BIT_FP_TYPE_COVERT;
	}
	else
	{
		mail_msg->Atrb_Display.ucFingerprint_Type = BIT_FP_TYPE_OVERT;
	}

	// extract Text Length, 12 bit-LSB,
	mail_msg->msg_length = ( (pbData[wIx] << 8) | pbData[wIx+1] )  & 0x0FFF ;
	wIx+=2;
	HxLOG_Print("ucCoverage = 0x%02x\n", mail_msg->Atrb_Display.ucCoverage);

	switch(mail_msg->ucType)
	{
		case ATTRIBUTED_MESSAGE_TYPE_NORMAL:
			mail_msg->msg_class = EPG_MESSAGE_CLASS_NORMAL;
			break;
		case ATTRIBUTED_MESSAGE_TYPE_FORCEDTEXT:
			mail_msg->msg_class = EPG_MESSAGE_CLASS_FORCED_DISPLAY;
			break;
		case ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT:
#if defined(CONFIG_MW_CAS_IRDETO_SUPPORT_COVERT_FINGERPRINT)
		case ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION:
#endif
			mail_msg->msg_class = EPG_MESSAGE_CLASS_FORCED_DISPLAY;
			break;
		default:
			mail_msg->msg_class = EPG_MESSAGE_CLASS_NORMAL;
			break;
	}

	mail_msg->ucType = EPG_MESSAGE_TYPE_TEXT_MAILBOX;	// attributed msg는 모두 mailbox 처리 한다

	if (mail_msg->msg_length > MAX_ATTRIBUTED_TEXT_LENGTH)
	{
		mail_msg->msg_length = MAX_ATTRIBUTED_TEXT_LENGTH;
	}

#if 0
//arabic test code
	pbData[wIx + 0] = 0xc6;
	pbData[wIx + 1] = 0xc7;
	pbData[wIx + 2] = 0xc8;
	pbData[wIx + 3] = 0xc9;
	pbData[wIx + 4] = 0xcf;
	pbData[wIx + 5] = 0xd3;
	pbData[wIx + 6] = 0xdb;
	pbData[wIx + 7] = 0xdd;
	pbData[wIx + 8] = 0xe4;
	pbData[wIx + 9] = 0xe6;

	mail_msg->msg_length=10;



	{
		HUINT8 i=0;
		HxLOG_Info("local_cas_ir_MSG_DRV_GetAttributedDisplay] raw mail message string: ");
		for(i=0; i<mail_msg->msg_length; i++)
		{
			HxLOG_Info("%x ", pbData[wIx + i]);
		}
		HxLOG_Info("\n");

	}
#endif


	HxLOG_Print("Message Length= %02x \n", mail_msg->msg_length);


	{
		// TODO: shko  일단 locale code 생각하지 않는다. web ui 에서 처리?
		//local_cas_ir_MSG_DRV_CopyMailTextWithLocaleCode(mail_msg->text, &(pbData[wIx]), &mail_msg->msg_length);
		local_cas_ir_MSG_DRV_CopyMailText(mail_msg->text, &(pbData[wIx]), &mail_msg->msg_length);
	}

	HxLOG_Print("mail_msg->text: %s \n", mail_msg->text);

	mail_msg->bAttributeMsg = TRUE;

	if (VK_OK !=  VK_CLOCK_GetTime((HULONG * )&(mail_msg->Atrb_Display.recUnixtime)  ) )
	{
		HxLOG_Print("Can't can time \n");
	}

	return TRUE;
}


void local_cas_ir_MSG_DRV_Attributed_DisplayProcess(Handle_t hAct, HUINT8 *pData, HUINT16 usLength)
{
	MAILBOX_TABLE	mail_msg;

	HxLOG_Print("************************************************\n");
	HxLOG_Print("MAILMSG - [local_cas_ir_MSG_DRV_Attributed_DisplayProcess]\n");

	HxSTD_memset(&mail_msg, 0, sizeof(MAILBOX_TABLE));

	if(*pData == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT)
	{
#if defined (CONFIG_MW_CAS_IRDETO_SCELL)
		if (xsvc_cas_IrGetSvcType((void*)hAct) == IR_CAS_TYPE_SCELL)
		{
			HUMAX_VdSc_HashCodeConvert();	// ewlee 20100817 - move to VD_SC.c file

			local_cas_ir_MSG_DRV_GetAttributedDisplay(&mail_msg, pData);
			local_cas_ir_MSG_DRV_MsgProcess(hAct, &mail_msg);
			/*		msg.usMsgClass = IREVT_CLASS_DECODER_MSG_ECM_FINGERPRINT;
			CAS_IR_EVT_SendEvent(0xFFFFFFFF, &msg);*/
		}
#endif
	}
	else if(*pData == ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION)
	{
		HxLOG_Info("receive ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION \n");
#if defined (CONFIG_MW_CAS_IRDETO_SCELL) && defined(CONFIG_MW_CAS_IRDETO_SUPPORT_COVERT_FINGERPRINT)
		if (xsvc_cas_IrGetSvcType((void*)hAct) == IR_CAS_TYPE_SCELL)
		{
			HUMAX_VdSc_HashCodeConvert();

			if( local_cas_ir_MSG_DRV_GetAttributedDisplay(&mail_msg, pData) == FALSE )
			{
				HxLOG_Print("\t\tMAILMSG - ERROR : invalid mail data\n");
				return;
			}

			local_cas_ir_MSG_DRV_MsgProcess(hAct,&mail_msg);

		}
#else
	//covert, overt finger print message는 처리하지 않음.
		HxLOG_Info("Do not support ATTRIBUTED_MESSAGE_TYPE_FINGERPRINT_COVERT_OVERT_OPTION \n");
		return;
#endif
	}
	else
	{
		if( local_cas_ir_MSG_DRV_GetAttributedDisplay(&mail_msg, pData) == FALSE )
		{
			HxLOG_Info("\t\tMAILMSG - ERROR : invalid mail data\n");
			return;
		}
		local_cas_ir_MSG_DRV_MsgProcess(hAct,&mail_msg);
	}
}


#define ________FROM_MSG_DRV_STORAGE____________________

HIR_ERR local_cas_ir_MSG_DRV_LoadFromNVRAM( void )
{
#if defined(CONFIG_DEBUG)
	HUINT32						i;
#endif
	int							result = 0;
	HUINT32						crc16;

	HxLOG_Info("\n");

	// Mail Box를 시간 순서로 ordering하지 않기 위해.. Mail Box에 mail이 도착한 순서를 index list로 관리..
	HxSTD_memset(&s_stIrMailboxInfo, 0, sizeof(MAILBOX_INFO));

	result = CAS_IR_NVR_FlashReadMail(NVRAM_OFFSET_MAILBOX_INFO, sizeof(MAILBOX_INFO), (HUINT8 *) &s_stIrMailboxInfo);
	if( result != HIR_OK )
	{
		HxLOG_Print("can't read Mails\n");
		goto Failure;
	}


	if( s_stIrMailboxInfo.ucListValid == TRUE)
	{
#if defined(CONFIG_DEBUG)
		HxLOG_Print("\t\tMAILMSG - ");
		for( i = 0; i < MAX_NUMBER_MAIL_MESSAGE; i++ )
		{
			HxLOG_Print("%02X ", s_stIrMailboxInfo.aucMailIndexList[i]);
		}
		HxLOG_Print("\n");
#endif
	}
	else
	{
		HxLOG_Print(" mailbox_info is not valid!\n");
		goto Failure;
	}

	crc16 = HLIB_MATH_GetCrc16 (&(s_stIrMailboxInfo.mailbox_table), MAX_NUMBER_MAIL_MESSAGE * sizeof(MAILBOX_TABLE));
	if(crc16 != s_stIrMailboxInfo.crc16)
	{
		HxLOG_Print("mail info boxs' CRC mismatched.\n");
		goto Failure;

	}

	return HIR_OK;

Failure:
	return HIR_NOT_OK;

}

HIR_ERR local_MSG_DRV_StorageUserMailInfoWrite( void )
{
	// TODO: TBD.
	local_cas_ir_MSG_DRV_FlushMailBox();
	return HIR_OK;
}

HIR_ERR local_cas_ir_MSG_DRV_StorageSetMailReadStatusWrite(HUINT32 ulIndex)
{
	// TODO: TBD.
	local_cas_ir_MSG_DRV_FlushMailBox();
	return HIR_OK;
}

HIR_ERR local_cas_ir_MSG_DRV_StorageSetMailValidStatusWrite(HUINT32 ulIndex)
{
	// TODO: TBD.
	local_cas_ir_MSG_DRV_FlushMailBox();
	return HIR_OK;
}

HIR_ERR local_cas_ir_MSG_DRV_FlushMailBox(void)
{
	HUINT32 iRet;

	iRet = CAS_IR_NVR_FlashWriteMail(NVRAM_OFFSET_MAILBOX_INFO, sizeof(MAILBOX_INFO), (HUINT8 *)&s_stIrMailboxInfo);
	if( iRet != HIR_OK )
	{
		HxLOG_Print("\t[local_MSG_DRV_StorageUserMailInfoWrite] Write - ERROR : WriteFile\n");
		return HIR_NOT_OK;
	}
	return HIR_OK;
}

