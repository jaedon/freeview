/****************************************************************************/
/*																			*/
/*	 FILE:	  physdrv.c 													*/
/*	 DATE:	  31/07/1997													*/
/*	 PURPOSE: DVB-CI PHYSICAL Driver Access 								*/
/*			High Level API													*/
/*			- PHYS_DRVOpen													*/
/*			- PHYS_DRVClose 												*/
/*			- PHYS_DRVWrite 												*/
/*			- PHYS_DRVRead													*/
/*																			*/
/*			Init, Read and Write routines (access to low level driver API)	*/
/*			- PHYS_InitModule												*/
/*			- PHYS_ResetModule												*/
/*			- PHYS_SendData 												*/
/*			- PHYS_ReceiveData												*/
/*			- PHYS_ReceiveIdle												*/
/*			- PHYS_ReadByte 												*/
/*			- PHYS_WriteByte												*/
/*			- PHYS_CheckCIS 												*/
/*			- PHYS_Tuple													*/
/*																			*/
/*	 TAB STOPS : 4															*/
/*	 HISTORY:																*/
/*	 Version	Date			Written by		 Notes						*/
/*	 1.0		31/07/1997		TGA, Teamlog	 Initial implementation 	*/
/*	 1.02		15/09/1997		TGA, Teamlog	 I2C modifications			*/
/*		- PHYS_SendData() : Test the FR bit before calling the Module to	*/
/*							Host Transfer command (I2C define Version)		*/
/*		- PHYS_SendData() : Test the WE bit after calling the Module to Host*/
/*							Transfer command (I2C define Version)			*/
/*		- PHYS_ReceiveData() : If the received data length is equal to 0,	*/
/*							   the function returns M_DATA	(I2C define 	*/
/*							   Version) 									*/
/*	1.03		19/09/1997		TGA, Teamlog	OLD define deletion 		*/
/*					- H2M define option for using Host to Module transfer	*/
/*						and Module to Host transfer 						*/
/*	1.04		19/11/1997		TGA, Teamlog	 Add st_drvheader structure */
/*						in PHYS_DRVRead 									*/
/*																			*/
/*--------------------------------------------------------------------------*/
/*	 GENERAL COMMENTS														*/
/*																			*/
/****************************************************************************/
//#include "simrm.h"

#include "dvb_cod.h"
#include "cipack_msg.h"
#include "itc.h"                    /* Inter-Task Communication             */
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
#include "pdriver_cimax.h"                /* low level PHYSICAL driver Access     */
#else
#include "pdriver.h"                /* low level PHYSICAL driver Access     */
#endif
#include "physdrv.h"
#include "ci.h"

/* parkbi modify */
#include "ctype.h"
//#include "string.h"
//#include "vkernel.h"

#include "ci_reshandler.h"			/* for CIRH_GetTcId() */
#include "trans.h"					/* for TRPT_GetTcid() and TRPT_Set_Timer() */
#include "pal_ci.h"

/*--------------------------------------------------------------------------*/
/* Global Variables 														*/
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/* Local Variables															*/
/*--------------------------------------------------------------------------*/

/* I/O register values */
static volatile unsigned char  Command_Register=0;
static volatile unsigned char  Status_Register=0;
static volatile unsigned char  Size_LS_Register=0;
static volatile unsigned char  Size_MS_Register=0;

static DRVPhys DRVTab[PAL_MAX_CISLOT_NUMBER];			/* slot informations table		*/
static short DRVNextIndex;					/* index of next polled slot	*/

static unsigned char *pDRVData; 			/* intermediate buffer for read/send collision */

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// hmkim, 2009-03-20 : work-around for 더글라스 CI 드라이버 이슈 (H/W ECO 안된 Douglas 플랫폼)
// 호스트에서 캠으로 데이터를 보낸 후에 Tsb 응답이 올 때 간혹 이 응답의 데이터가 잘못되어 있는 문제를 핸들링하기 위해 추가함 (단, length 는 올바르게 온다)
// 캠에서 보낸 Tsb 응답은 정확하나 BSP 거쳐 잘못된 데이터가 올라오는 경우로 이를 CI 스택에서 잘못 처리하게 됨으로써 스택 오동작이 야기됨 -> [CI_RW_ERROR] 로그 상황 발생 !!
// 적용된 에러 핸들링은 굳이 더글라스 플랫폼이 아니라 일반적으로 이 처리를 넣어주는 것이 좋을 것으로 생각됨.
// 밑에 있는 변수는 DRV_CI_Write(), DRV_CI_Read() 관련해서만 사용되고 이 함수들은 동시 호출이 안되므로 굳이 세마포어로 보호할 필요는 없음.

#define TSB_ERROR_HANDLING
#ifdef TSB_ERROR_HANDLING
static unsigned char uc_Expected_Tsb[6] = {0x01, 0x00, 0x80, 0x02, 0x01, 0x00};
#endif
// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


/* High Level API */
/* ============== */


/* ======================================================================== */
/* NAME : PHYS_DRVOpen														*/
/*																			*/
/* DESCRIPTION : open the PHYSICAL Driver : 								*/
/*		- Initialize slot management table, 								*/
/*		- Open the low level PHYSICAL Driver								*/
/*																			*/
/* INPUT PARAMETERS :														*/
/*																			*/
/* OUTPUT PARAMETERS :														*/
/*																			*/
/* RETURN CODE																*/
/*		> 0 : OK (number of initialized slots								*/
/*		 -1 : error 														*/
/*																			*/
/* ======================================================================== */
int PHYS_DRVOpen(void)
{
int i;
int slot;
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
int ret = 0;
#endif

	/* Allocate intermediate buffer for read/send collision */
	pDRVData = ITC_DynAlloc(MAXLNSIZE);
	if (pDRVData == NULL)
	{
		PrintErr(("PHYS_DRVOpen : not enought memory (%d) !!!\n", MAXLNSIZE));
		return -1;
	}

	VK_memset(pDRVData, 0, MAXLNSIZE);

	for (i=0; i < PAL_CI_GetNumberOfModule(); i++)
	{
		/* Initialize the slot table */
		DRVTab[i].State 	 = DRV_IDLE;
		DRVTab[i].firstSLPDU = NULL;

		/* Open the low level PHYSICAL Driver */
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
		slot = DRV_CI_Open((unsigned char *)DRV_DEV);
		if (slot < 0)
		{
			PrintErr(("PHYS_DRVOpen : wrong slot %d !!!\n", slot));
			DRVTab[i].Slot_id = INVALID_CI_SLOT_ID;
			return -1;
		}
#else
		ret = DRV_CI_Open ((unsigned char *)DRV_DEV, &slot);
		if(ret != 0)
		{
			PrintErr(("PHYS_DRVOpen : wrong slot %d !!!\n", slot));
			DRVTab[i].Slot_id = INVALID_CI_SLOT_ID;
			return -1;
		}
#endif
		DRVTab[i].Slot_id = (unsigned short)slot;
		#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
		DRVTab[i].Ci_plus = 0;
		#endif

		PrintDbg(("PHYS_DRVOpen : Open PHYSICAL low level driver for slot %d\n", DRVTab[i].Slot_id));
	}

	DRVNextIndex = 0;

	return i;
}


/* ======================================================================== */
/* NAME : PHYS_DRVWrite 													*/
/*																			*/
/* DESCRIPTION : Send a LPDU to the PHYSICAL Driver 						*/
/*																			*/
/* INPUT PARAMETERS :														*/
/*		slot  : slot identifier 											*/
/*		pbuff : pointer to the LPDU to send 								*/
/*		len   : length of pbuff (LPDU)										*/
/*																			*/
/* OUTPUT PARAMETERS :														*/
/*																			*/
/* RETURN CODE																*/
/*		   0 : OK															*/
/*		  -1 : error														*/
/*																			*/
/* ======================================================================== */
int PHYS_DRVWrite(unsigned short slot, unsigned char *pbuff, unsigned short len)
{
unsigned char Ret;			/* return code					 */
int index;					/* DRVTab index for current slot */
int count;					/* timeout management			 */

	for (index=0; index < PAL_CI_GetNumberOfModule(); index++)
		if (DRVTab[index].Slot_id == slot)
			break;

	if (index >= PAL_CI_GetNumberOfModule())
		return -1;			/* slot_id not found */

	count = 0;
	do
	{
		Ret= PHYS_SendData(index, pbuff, len);
		if (Ret == M_BUSY)
		{
			ITC_Sleep(PHYS_DELAY);
			count++;
			if(count==PHYS_TIMEOUT)
			{
				PrintErr(("[CI_RW_ERROR] PHYS_DRVWrite : BUSY !!!\n"));
				return -1;	/* not sent */
			}
		}
	}
	while (Ret == M_BUSY);

	if (Ret == OK)
	{
#if 0
		char buf_tpdu[1000];
		int j;
		for (j=0;j<(int)len;j++)
			sprintf((char *)(buf_tpdu+(3*j)),"%02x ",*(pbuff+j));
		buf_tpdu[(3*len)-1] = '\0';
		PrintDbg(("PHYS_DRVWrite : %d bytes sent to the module %d : %s\n", len, slot, buf_tpdu));
#endif
		return 0;
	}
	else
	{
		PrintErr(("[CI_RW_ERROR] PHYS_DRVWrite : ERROR(%d) !!!\n", Ret));
		return -1;
	}
}


/* ======================================================================== */
/* NAME : PHYS_DRVRead														*/
/*																			*/
/* DESCRIPTION : Read a message from the PHYSICAL Driver					*/
/*																			*/
/* INPUT PARAMETERS :														*/
/*																			*/
/* OUTPUT PARAMETERS :														*/
/*		pbuff : pointer to the message										*/
/*		len   : length of pbuff 											*/
/*																			*/
/* RETURN CODE																*/
/*		  >=0: type of received event										*/
/*		   0 : nothing received 											*/
/*		  -1 : error														*/
/*																			*/
/* ======================================================================== */
int PHYS_DRVRead(unsigned char *pbuff, unsigned short *len)
{
	int iRet;		/* return code						 */
	DRV infos;		/* received event and associated data*/
	int Ret;		/* PHYSICAL Driver return code		 */
	int offset; 	/* intermediate length				 */
	int index;		/* DRVTab[] index					 */
	int nbpoll; 	/* number of polling				 */
	unsigned short drvlen;	/* length of received data	 */

#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	int result = 0;		/* PHYSICAL Driver return code		 */
#endif
	iRet = 0;
	index = DRVNextIndex;
	nbpoll = 0;
//	offset = sizeof(ph_data) - 1 + sizeof(st_drvheader);		/* header size until ph_data structure end */
	offset = sizeof(unsigned short)*2 + sizeof(st_drvheader);	/* header size until ph_data structure end */
	//	PrintDbg(("PHYS_DRVRead : sizeof(ph_data)=%d, sizeof(DRV)=%d, offset=%d, offset2=%d\n", sizeof(ph_data), sizeof(DRV), offset, offset2));

#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	do
	{
		if (DRVTab[index].State == DRV_CONNECT)
		{	/* wait for received data from inserted Module or wait for Module Extraction */
			/* ========================================================================= */
			/* the calling buffer is re-used for optimization . the PH_DATA message fixed size header is added after the reception */
			Ret = PHYS_ReceiveData(index, pbuff + offset, &drvlen, 1);
			switch (Ret)
			{
			case OK :
				/* signal the PH_DATA event to the PHYSICAL Task */
				/* fill the received buffer with PH_DATA		 */
				infos.header.type = PH_DATA;
//				infos.header.len  = sizeof(ph_data) -1 + drvlen;
				infos.header.len  = sizeof(unsigned short)*2 + drvlen;
				infos.body.data.slot_id = DRVTab[index].Slot_id;
				infos.body.data.len 	= drvlen;

				/* header adding */
				*len = infos.header.len + sizeof(st_drvheader);
				VK_memcpy(pbuff, (char *)&infos, offset);

				iRet = PH_DATA;
#if 0
				{
					char buf_tpdu[1000];
					int j;

					/* display the received buffer in ASCII */
					for (j=0;j<(int)drvlen;j++)
						sprintf((char *)(buf_tpdu+(3*j)),"%02x ",*(pbuff+offset+j));
					buf_tpdu[(3*drvlen)-1] = '\0';
					if (drvlen > 50)
						buf_tpdu[200] = '\0';
					PrintDbg(("PHYS_DRVRead : %d received bytes from the module %d : %s\n", drvlen, DRVTab[index].Slot_id, buf_tpdu));
				}
#endif
				break;

			case T_ERR :
				/* signal the PH_ERROR event to the PHYSICAL Task */
				/* fill the received buffer with PH_ERROR		  */
				infos.header.type = PH_ERROR;
				infos.header.len  = sizeof(ph_error);
				infos.body.errhw.slot_id = DRVTab[index].Slot_id;
				infos.body.errhw.status  = LN_PHYSICAL;

				*len = infos.header.len + sizeof(st_drvheader);
				VK_memcpy(pbuff, (char *)&infos, *len);

				iRet = PH_ERROR;
				break;

#ifdef CI_PLUS_ENABLE
			case R_IIR :
				VK_Print("<R_IIR>\n"); // 중요 로그이므로 항시 찍어 주자.
#endif
			case W_SEACH :
				/* Set new state */
				DRVTab[index].State = DRV_DISCONNECT;

				/* signal the PH_EXTRACT event to the PHYSICAL Task */
				/* fill the received buffer with PH_EXTRACT 		*/
				infos.header.type = PH_EXTRACT;
				infos.header.len  = sizeof(ph_extract);
				infos.body.extract.slot_id = DRVTab[index].Slot_id;

				*len = infos.header.len + sizeof(st_drvheader);
				VK_memcpy(pbuff, (char *)&infos, *len);

				iRet = PH_EXTRACT;
				break;

			default :
				/* no complete data received */
				break;
			}

		}
		else
		{	/* wait for Module Insertion and Initialization */
			/* ============================================ */
			Ret = PHYS_InitModule(index, MAXLNSIZE, (unsigned short *)(&DRVTab[index].Negociated_size));
			if(Ret != W_SEACH && Ret != OK) {
				PrintErr(("PHYS_DRVRead : PHYS_InitModule return [%d] !!!\n", Ret));
			}

			switch (Ret)
			{
			case OK :	// 0x00
				/* signal the PH_INSERT event to the PHYSICAL Task */
				/* fill the received buffer with PH_INSERT		   */
				infos.header.type = PH_INSERT;
				infos.header.len  = sizeof(ph_insert);
				infos.body.insert.slot_id = DRVTab[index].Slot_id;
				infos.body.insert.status  = LN_OK;
				infos.body.insert.size	  = DRVTab[index].Negociated_size;

				*len = infos.header.len + sizeof(st_drvheader);
				VK_memcpy(pbuff, (char *)&infos, *len);

				/* Set new state */
				DRVTab[index].State = DRV_CONNECT;

				iRet = PH_INSERT;
				break;

			case KO :	// 0x07
				/* signal the wrong PH_INSERT event to the PHYSICAL Task */
				/* fill the received buffer with PH_INSERT			   */
				infos.header.type = PH_INSERT;
				infos.header.len  = sizeof(ph_insert);
				infos.body.insert.slot_id = DRVTab[index].Slot_id;
				infos.body.insert.status  = LN_PHYSICAL;		/* wrong CIS */
				infos.body.insert.size	  = DRVTab[index].Negociated_size;

				*len = infos.header.len + sizeof(st_drvheader);
				VK_memcpy(pbuff, (char *)&infos, *len);

				/* Set new state */
				DRVTab[index].State = DRV_CONNECT;

				iRet = PH_INSERT;
				break;

			default :
				break;
			}
		}

		if (iRet == 0)
		{
			nbpoll++;
			index++;				/* try the next index if nothing receveid */
			if (index == PAL_CI_GetNumberOfModule())
				index = 0;
		}

	} while((iRet == 0) && (nbpoll < PAL_CI_GetNumberOfModule()));
#else
	do
	{
		if (DRVTab[index].State == DRV_CONNECT)
		{	/* wait for received data from inserted Module or wait for Module Extraction */
			/* ========================================================================= */
			/* the calling buffer is re-used for optimization . the PH_DATA message fixed size header is added after the reception */

			result = DRV_CI_DataAvailable(DRVTab[index].Slot_id);
			if(result == 1)
			{
				Ret = PHYS_ReceiveData(index, pbuff + offset, &drvlen, 1);
				switch (Ret)
				{
				case OK :
					/* signal the PH_DATA event to the PHYSICAL Task */
					/* fill the received buffer with PH_DATA		 */
					infos.header.type = PH_DATA;
	//				infos.header.len  = sizeof(ph_data) -1 + drvlen;
					infos.header.len  = sizeof(unsigned short)*2 + drvlen;
					infos.body.data.slot_id = DRVTab[index].Slot_id;
					infos.body.data.len 	= drvlen;

					/* header adding */
					*len = infos.header.len + sizeof(st_drvheader);
					VK_memcpy(pbuff, (char *)&infos, offset);

					iRet = PH_DATA;
#if 0
					{
						char buf_tpdu[1000];
						int j;

						/* display the received buffer in ASCII */
						for (j=0;j<(int)drvlen;j++)
							sprintf((char *)(buf_tpdu+(3*j)),"%02x ",*(pbuff+offset+j));
						buf_tpdu[(3*drvlen)-1] = '\0';
						if (drvlen > 50)
							buf_tpdu[200] = '\0';
						PrintDbg(("PHYS_DRVRead : %d received bytes from the module %d : %s\n", drvlen, DRVTab[index].Slot_id, buf_tpdu));
					}
#endif
					break;

				case T_ERR :
					/* signal the PH_ERROR event to the PHYSICAL Task */
					/* fill the received buffer with PH_ERROR		  */
					infos.header.type = PH_ERROR;
					infos.header.len  = sizeof(ph_error);
					infos.body.errhw.slot_id = DRVTab[index].Slot_id;
					infos.body.errhw.status  = LN_PHYSICAL;

					*len = infos.header.len + sizeof(st_drvheader);
					VK_memcpy(pbuff, (char *)&infos, *len);

					iRet = PH_ERROR;
					break;

#ifdef CI_PLUS_ENABLE
				case R_IIR :
					VK_Print("<R_IIR>\n"); // 중요 로그이므로 항시 찍어 주자.
#endif
				case W_SEACH :
					/* Set new state */
					DRVTab[index].State = DRV_DISCONNECT;

					/* signal the PH_EXTRACT event to the PHYSICAL Task */
					/* fill the received buffer with PH_EXTRACT 		*/
					infos.header.type = PH_EXTRACT;
					infos.header.len  = sizeof(ph_extract);
					infos.body.extract.slot_id = DRVTab[index].Slot_id;

					*len = infos.header.len + sizeof(st_drvheader);
					VK_memcpy(pbuff, (char *)&infos, *len);

					iRet = PH_EXTRACT;
					break;

				default :
					/* no complete data received */
					break;
				}
			}
			else
			{
				if(result == 0)
				{
					/* signal the PH_INSERT event to the PHYSICAL Task */
					/* fill the received buffer with PH_INSERT		   */
					infos.header.type = PH_INSERT;
					infos.header.len  = sizeof(ph_insert);
					infos.body.insert.slot_id = DRVTab[index].Slot_id;
					infos.body.insert.status  = LN_OK;
					infos.body.insert.size	  = DRVTab[index].Negociated_size;

					*len = infos.header.len + sizeof(st_drvheader);
					VK_memcpy(pbuff, (char *)&infos, *len);

					/* Set new state */
					DRVTab[index].State = DRV_CONNECT;
					iRet = PH_INSERT;
				}
				else
				{
					/* Set new state */
					DRVTab[index].State = DRV_DISCONNECT;

					/* signal the PH_EXTRACT event to the PHYSICAL Task */
					/* fill the received buffer with PH_EXTRACT 		*/
					infos.header.type = PH_EXTRACT;
					infos.header.len  = sizeof(ph_extract);
					infos.body.extract.slot_id = DRVTab[index].Slot_id;

					*len = infos.header.len + sizeof(st_drvheader);
					VK_memcpy(pbuff, (char *)&infos, *len);

					iRet = PH_EXTRACT;
				}
			}

		}
		else
		{	/* wait for Module Insertion and Initialization */
			/* ============================================ */
			Ret = PHYS_InitModule(index, MAXLNSIZE, (unsigned short *)(&DRVTab[index].Negociated_size));
			switch (Ret)
			{
			case OK :	// 0x00
				/* signal the PH_INSERT event to the PHYSICAL Task */
				/* fill the received buffer with PH_INSERT		   */
				infos.header.type = PH_INSERT;
				infos.header.len  = sizeof(ph_insert);
				infos.body.insert.slot_id = DRVTab[index].Slot_id;
				infos.body.insert.status  = LN_OK;
				infos.body.insert.size	  = DRVTab[index].Negociated_size;

				*len = infos.header.len + sizeof(st_drvheader);
				VK_memcpy(pbuff, (char *)&infos, *len);

				/* Set new state */
				DRVTab[index].State = DRV_CONNECT;
				iRet = PH_INSERT;
				break;

			case KO :	// 0x07
				/* signal the wrong PH_INSERT event to the PHYSICAL Task */
				/* fill the received buffer with PH_INSERT			   */
				infos.header.type = PH_INSERT;
				infos.header.len  = sizeof(ph_insert);
				infos.body.insert.slot_id = DRVTab[index].Slot_id;
				infos.body.insert.status  = LN_PHYSICAL;		/* wrong CIS */
				infos.body.insert.size	  = DRVTab[index].Negociated_size;

				*len = infos.header.len + sizeof(st_drvheader);
				VK_memcpy(pbuff, (char *)&infos, *len);

				/* Set new state */
				DRVTab[index].State = DRV_CONNECT;

				iRet = PH_INSERT;
				break;

			default :
				break;
			}
		}

		if (iRet == 0)
		{
			nbpoll++;
			index++;				/* try the next index if nothing receveid */
			if (index == PAL_CI_GetNumberOfModule())
				index = 0;
		}

	} while((iRet == 0) && (nbpoll < PAL_CI_GetNumberOfModule()));
#endif
	DRVNextIndex++; /* index of the next PHYS_DRVRead */
	if (DRVNextIndex == PAL_CI_GetNumberOfModule())
		DRVNextIndex = 0;

	return iRet;
}


/* ======================================================================== */
/* NAME : PHYS_DRVClose 													*/
/*																			*/
/* DESCRIPTION : close the PHYSICAL Driver :								*/
/*																			*/
/* INPUT PARAMETERS :														*/
/*																			*/
/* OUTPUT PARAMETERS :														*/
/*																			*/
/* RETURN CODE																*/
/*	 0 : OK 																*/
/*	-1 : error																*/
/*																			*/
/* ======================================================================== */
int PHYS_DRVClose(void)
{
int i;
unsigned char *p;
unsigned char *free_p;

	if (pDRVData)
	{
		ITC_DynFree(pDRVData);
		pDRVData = NULL;
	}

	for(i=0; i < PAL_CI_GetNumberOfModule(); i++)
	{
		/* Close the low level PHYSICAL Driver for a slot */
		if (DRVTab[i].Slot_id != INVALID_CI_SLOT_ID)
		{
			DRV_CI_Close(DRVTab[i].Slot_id);
			DRVTab[i].Slot_id = INVALID_CI_SLOT_ID;
		}

		if (DRVTab[i].firstSLPDU)
		{
			/* free the linked list */
			p = (unsigned char *)DRVTab[i].firstSLPDU;
			while (((SLPDU*)p)->next)
			{
				free_p = p;
				p = ((SLPDU*)p)->next;
				ITC_DynFree(free_p);
			}
			ITC_DynFree(p);
		}
	}

	return 0;
}

/* End of High Level API */
/* ===================== */
/* Low Level management routines and low level driver access */
/* ========================================================= */

/*******************************************************************************
Name:			unsigned char PHYS_InitModule(int index, unsigned int Decoder_Size, unsigned int far *Negociate_Size)
Description:	Make Initialisation of I/O window.
In: 			int index : DRVTab index
				unsigned int Host_Size: Size of Host exchange buffer.
Out:			unsigned int far *Negociate_Size: Size of exchange buffer after negociation
Return value:	Status:
						OK: complete initilisation done.
						KO: wrong CIS
						W_SEACH:	PHYS_InitModule() doesn't find any I/O window.
******************************************************************************/
unsigned char PHYS_InitModule(int index, unsigned int Host_Size, unsigned short *Negociate_Size)
{
	int Ret;

	DRVTab[index].State = DRV_DISCONNECT;

	/* Poll the Card Detect and wait for complete module initialization */
	/* ================================================================ */
	Ret = PHYS_Card(index);
	if (Ret != OK)
	{
		DRVTab[index].State = DRV_IDLE;
		return W_SEACH;		/* no card detect */
	}

	PrintDbg(("PHYS_InitModule : the module %d is inserted\n", DRVTab[index].Slot_id));

	/* Check CIS and write COR */
	/* ======================= */

	// hmkim :
	// 더글러스 드라이버 구현에 따라 위 PHYS_Card()에서 캠 ready가 정확히 체크되지 않으므로 (캠 ready 체크와 캠 detection 체크가 동일함. 즉, 캠 detection 만 2번 체크하는 꼴)
	// CIS 체크 전의 sleep 타임을 늘림.
	// 그렇지 않을 경우 DRV_CI_ReadCIS()로 가져온 CIS가 잘못된 경우가 종종 발생함.
	// 캠에 따라 ready 정보가 올바르지 못한 경우가 있고 (Giga CAM)
	// 드라이버 구현이 거의 이와 같이 되어 있으므로 일반적인 코드로 적용함.
 	VK_TASK_Sleep(1000); // org 500

	Ret = PHYS_CIS(index);
	switch (Ret) {
	case OK :

		PrintDbg(("PHYS_InitModule : the CIS of module %d is valid\n", DRVTab[index].Slot_id));

		/* Initialize the DVB Module */
		/* ========================= */
		if(PHYS_ResetModule(index, Host_Size, Negociate_Size))
		{
			PrintErr(("PHYS_InitModule : the module %d can't be initialized !!!\n", DRVTab[index].Slot_id));
			DRVTab[index].State = DRV_IDLE;
			if (PAL_CI_ResetModule(DRVTab[index].Slot_id) != ERR_OK)	// add by chojh
			{
				PrintErr(("PHYS_InitModule : fail to reset slot %d !!!\n", DRVTab[index].Slot_id));
			}
			return(W_SEACH);
		}
		else
		{
			PrintDbg(("PHYS_InitModule : the module %d is initialized\n", DRVTab[index].Slot_id));
			return(OK);
		}
		break;

	case KO :
		PrintErr(("PHYS_InitModule : the CIS of module %d is wrong !!!\n", DRVTab[index].Slot_id));
		if (PAL_CI_ResetModule(DRVTab[index].Slot_id) != ERR_OK) // add by thyou  2007.08.14
		{
			PrintErr(("PHYS_InitModule : fail to reset slot %d !!!\n", DRVTab[index].Slot_id));
		}
		return(KO); 			/* wrong CIS , warn the upper layers */
		break;

	default:
		//PrintErr(("PHYS_InitModule : the CIS of module %d is %d\n", DRVTab[index].Slot_id, Ret));
		return (W_SEACH);		/* error : no detected module */
		break;
	}

	DRVTab[index].State = DRV_IDLE;
	return(W_SEACH);
}


/*******************************************************************************
Name:			int PHYS_Card(int index)

Description:	check the Card Detect Signal and wait for complete module initialization
In: 				int index : DRVTab index
Out:
Return value:	W_SEACH : no card detect
				MEM_ERR : error
				OK : signal detected
******************************************************************************/
unsigned char PHYS_Card(int index)
{
int count;			/* timeout management */
int Ret;			/* return code		  */
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
DRV_stSignal signal;/* DRV_Ioctl argument */
#else
	int signal = 0;
#endif

#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	signal.sig = DRV_CARD_DETECT;
	Ret = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_TSIGNAL, &signal);
	if ((Ret > 0) && (signal.value))
#else
	Ret = DRV_TsSignal(DRVTab[index].Slot_id, &signal);
	
	if ((Ret == 0) && (signal))
#endif
	{
		/* Card Detect : wait for complete initialization */
		count = 0;
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
		signal.sig = DRV_READY_BUSY;
#endif
		do
		{
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
			Ret = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_TSIGNAL, &signal);
			if (Ret < 0) return MEM_ERR;
			PrintDbg(("PHYS_Card : DRV_Ioctl Ret. = %d signal = %d\n",Ret,signal.value));
#else
			Ret = DRV_TsSignal(DRVTab[index].Slot_id, &signal);
			if (Ret < 0) return MEM_ERR;
#endif

			ITC_Sleep(PHYS_DELAY);
			count++;
			if(count==PHYS_TIMEOUT) return MEM_ERR; /* no module */
		}
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
		while (! signal.value);
#else
		while (!signal);
#endif
		PrintDbg(("PHYS_Card : Return OK\n"));

		return OK;
	}
	else
		return W_SEACH;
}


/*******************************************************************************
Name:			int PHYS_CD(int index)

Description:	check the Card Detect Signal
In: 				int index : DRVTab index
Out:
Return value:	W_SEACH : no card detect
				OK : signal detected
******************************************************************************/
unsigned char PHYS_CD(int index)
{
int Ret;			/* return code		  */
#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
DRV_stSignal signal;/* DRV_Ioctl argument */

	signal.sig = DRV_CARD_DETECT;
	Ret = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_TSIGNAL, &signal);
	if ((Ret > 0) && (signal.value))
#else
	int signal = 0;

	Ret = DRV_TsSignal(DRVTab[index].Slot_id, &signal);

	if(Ret == 0)
#endif
		return OK;
	else
		return W_SEACH;
}


#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
/*******************************************************************************
Name:			int PHYS_CIS(int index)

Description:	Read the CIS, Check it and Write the COR
In: 				int index : DRVTab index
Out:
Return value:
Return value:	KO : wrong CIS
				MEM_ERR : error
				OK : signal detected
******************************************************************************/
unsigned char PHYS_CIS(int index)
{
	unsigned char *cis; 			/* CIS buffer		 */
	unsigned short addrcor; 		/* COR Address		 */
	unsigned char cor;				/* COR value		 */
	#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	unsigned char ciplus;			/* CI+ CAM detected  */
	#endif
	int iRet;						/* return code		 */
	DRV_stMem mem;					/* driver parameters */
	unsigned short cisLen;

#ifndef USE_DOUGLAS_PRIMITIVE_FUNC

	unsigned short addrccrs;
	if (PHYS_GetAddrCcrs(DRVTab[index].Slot_id, &addrccrs) < 0)
		return KO;		/* wrong CIS */
	cisLen = addrccrs >> 1;

	cis = (unsigned char *)ITC_DynAlloc(cisLen);
	if(!cis)
	{
		PrintErr(("PHYS_CIS : not enought memory (%d) !!!\n", cisLen));
		return MEM_ERR;
	}

	mem.addr   = DRV_MEMADDR;
	mem.len    = cisLen;
	mem.pbytes = cis;
	iRet = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_READMEM, &mem);
	if (iRet < 0)
	{
		ITC_DynFree(cis);
		PrintErr(("PHYS_CIS : DRV_Ioctl error (%d) !!!\n", iRet));
		return MEM_ERR; 	/* error while reading CIS */
	}

#else /* for douglas */

	cisLen = 1024; /* <TODO_CI> : 체크 요함 */ /* see CI_CIS_SIZE at the DRV_ci_drv.h */
	cis = (unsigned char *)ITC_DynAlloc(cisLen);
	if(!cis)
	{
		PrintErr(("PHYS_CIS : not enought memory (%d) !!!\n", cisLen));
		return MEM_ERR;
	}

	VK_memset(cis, 0, cisLen);
	iRet = DRV_CI_ReadCIS(DRVTab[index].Slot_id, cis, &mem.rlen);
	if (iRet < 0)
	{
		ITC_DynFree(cis);
		PrintErr(("PHYS_CIS : DRV_CI_ReadCIS error (%d) !!!\n", iRet));
		return MEM_ERR; 	/* error while reading CIS */
	}

//	mem.rlen = 0x1fe/2; /* <TODO_CI> : 체크 요함 */ /* DRV_CI_ReadCIS()에서 len 얻어오게 함으로써 해결 */

#endif

#ifdef _CI_DEBUG_ // #ifdef _CI_PLUS_DEBUG_ // CI+ 스펙에 명시된 CI Plus Compatibility Identification 확인 용도임.
{
	int i;
	VK_Print("\n --- CIS : cisLen = %d, rlen = %d ---\n", cisLen, mem.rlen);
	for (i = 0; i < mem.rlen; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n %03d :", i);
		VK_Print(" %02X", cis[i]);
	}
	VK_Print("\n\n");

/* Irdeto blue 캠 - 일명 유로 캠 (CIv1) : 255 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 15 05 00 53
43 4D 00 44 56 42 20 43 41 20 4D 6F 64 75 6C 65
00 FF 20 04 FF FF 01 00 1A 15 01 0F FE 01 01 C0
0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30 1B
11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03 50
FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0 09
44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F 43
49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 0C 60 C2 00 41 10 84 00 03 00 00 41 00 2A
02 04 50 33 08 48 C0 00 4C 28 00 20 00 80 00 42
40 80 05 C4 09 40 42 08 20 00 02 45 48 40 40 00
08 09 00 02 00 00 00 C0 00 00 00 06 1A 2C 00 18
40 00 02 0C 00 22 20 00 00 10 10 40 00 04 00 81
44 0C 00 24 02 11 00 50 00 18 40 92 40 10 00
*/

/* AlphaCrypt 캠 (CIv1) : 255 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 15 05 00 53
43 4D 00 44 56 42 20 43 41 20 4D 6F 64 75 6C 65
00 FF 20 04 FF FF 01 00 1A 15 01 0F FE 01 01 C0
0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30 1B
11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03 50
FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0 09
44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F 43
49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
06 08 00 08 02 E5 11 06 20 70 44 D9 83 00 2C 92
00 80 0A 08 00 0B A0 00 00 63 28 D0 04 12 04 0E
00 80 00 58 84 20 41 67 01 45 0C 80 E0 00 80 0C
20 00 44 02 20 00 00 C8 04 00 B9 88 00 60 C8 33
26 00 6C 08 10 10 C3 58 01 D8 00 80 A0 20 CC 14
08 04 10 05 04 01 82 B1 30 1C 00 A3 00 40 A0
*/

/* SmarDTV CI+ prototype 캠 : 511 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 19 05 00 53
6D 61 72 44 54 56 00 44 56 42 20 43 41 20 4D 6F
64 75 6C 65 00 FF 20 04 FF FF 01 00 1A 15 01 0F
FE 03 01 C0 0E 41 02 44 56 42 5F 43 49 5F 56 31
2E 30 30 1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60
20 03 03 50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D
56 22 C0 09 44 56 42 5F 48 4F 53 54 00 C1 0E 44
56 42 5F 43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF
83 EF AD ED 41 02 BE E3 00 21 9D AD 98 C4 1A 02
F5 0F 1C 78 23 93 67 10 B5 EB 13 29 A3 37 06 15
E6 9C 71 02 B5 08 68 C1 DA 43 03 EA 9A 92 64 8D
75 56 08 DA C3 90 C8 21 3B 1A A2 26 75 24 42 02
0F 80 C4 E0 23 68 83 26 A4 61 72 25 C1 AF C8 46
81 4C 49 4A B6 5A A2 24 19 62 0A D5 7F 32 CA 81
5B 2A 5C B1 82 00 00 E5 63 66 94 CA 70 42 5B 10
69 05 C2 CB 36 22 34 27 0C 8A 57 22 08 C3 4A 43
2F 8E C5 8A 22 B8 4E 23 3A 77 95 EE FC 00 38 08
46 3E 8E 29 0C F7 82 B6 B4 72 C9 B9 DA 32 20 02
17 98 17 57 E5 27 82 46 33 E7 E1 36 5E 42 08 08
B3 E5 25 2B CE 29 FA 93 1A A4 88 2E 03 AD 4A 16
05 E8 DD 7A 63 C1 84 67 18 60 53 E5 67 72 C0 1E
3D 2E E5 48 66 81 82 74 28 20 43 64 05 89 C9 02
2F EE 52 04 9B 06 3F B0 3D 16 03 60 AA 17 4B 34
11 A4 87 18 47 28 25 21 04 62 41 34 E6 A7 2B 36
00 76 C2 69 BB 33 C4 06 20 B9 7D 23 1D 03 C2 07
14 05 D4 40 2F 1C 4C 07 AC EE DB 75 A1 A9 8D 5D
06 90 B1 41 9B D4 8C 57 02 FA ED A1 CF 38 DC 44
3E 0C 82 03 3B 54 20 26 1A 60 D2 A6 B1 2A 0A 10
B5 2F 56 C2 D6 20 45 81 F6 60 51 D5 36 28 EA 06
44 9A 10 34 52 BE BF 5D 33 78 18 87 64 30 E0 3A
C7 9A FE 0A 3C A7 64 03 9B A1 48 26 30 63 E1 00
19 8A DA E8 89 70 61 54 2B 70 D2 CC A4 76 0D
*/

/* SmarDTV CI+ prototype 캠 : 511 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 19 05 00 53
6D 61 72 44 54 56 00 44 56 42 20 43 41 20 4D 6F
64 75 6C 65 00 FF 20 04 FF FF 01 00 1A 15 01 0F
FE 03 01 C0 0E 41 02 44 56 42 5F 43 49 5F 56 31
2E 30 30 1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60
20 03 03 50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D
56 22 C0 09 44 56 42 5F 48 4F 53 54 00 C1 0E 44
56 42 5F 43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF
5C 68 31 71 69 7D DC 99 18 E0 13 0A 38 50 A8 06
D0 08 E3 06 49 05 03 5F 09 2E 72 64 00 34 33 00
6C 0E 51 B7 03 0B C5 4B 7A A0 89 00 38 42 18 8C
1D 19 9E 27 CD 25 81 0D 2F 4E 81 2F 39 4E 2C 1D
57 8E 00 72 5A 33 40 49 21 24 C3 F2 89 54 14 0B
85 00 36 35 50 96 D0 01 15 0A 03 20 64 4D 58 18
25 A8 8B 05 90 54 64 C1 04 2A EC 41 77 BA 14 BB
AB A9 FB 3C 48 5A 90 55 04 E1 72 20 6A ED B0 98
48 0F BA 32 56 50 45 82 64 62 46 18 1B 35 5F 44
9A 9E 86 9A 60 1D 57 9F 67 7C 12 19 51 47 40 10
8F 4A D3 C3 48 28 24 DF 0D 21 04 FA 57 0D 77 10
54 63 50 3E D8 00 21 46 87 17 43 69 2E AE F2 FA
64 2C 0B 8A 35 29 17 05 48 49 14 A8 13 32 7C 28
5E 85 F6 E4 03 66 09 75 35 6F 85 12 A8 D5 2D A4
23 23 36 98 60 89 B6 92 02 00 39 4E 5E 40 7D 22
FC 6B D7 19 02 D8 06 00 A3 B9 77 41 61 21 9E 99
43 01 52 11 8D 2E AE B2 5C 99 94 8E AC 94 FE 62
45 00 24 62 15 A0 73 4B BE 04 08 4A 51 CD 68 80
20 94 F2 B4 29 34 30 0A 61 E2 12 56 24 AD 20 03
CF E9 82 F8 EA 2E D8 09 6E 7B 3A 0A 28 76 70 68
28 89 92 18 03 BB 20 81 3F 20 EE 3E 3A 81 99 11
AE 9C 12 24 50 0B 81 7E F7 45 2D 0C B0 E5 0B 96
FB 23 71 B4 1E F0 4F 09 DA B7 43 82 FC 6F 4B F4
E5 AF 81 F6 25 47 14 6D 6A 29 46 04 BA D7 D8
*/

/* Neotion 짝퉁 CI+ 캠 with CI+ MHEG demo : 255 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 12 05 00 4E
45 4F 54 49 4F 4E 00 4E 50 34 00 43 49 00 FF 20
04 FF FF 01 00 1A 15 01 0F FE 01 01 C0 0E 41 02
44 56 42 5F 43 49 5F 56 31 2E 30 30 1B 0E C2 41
09 37 55 4D 5D 56 56 AA 60 20 03 03 1B 26 CF 04
19 37 55 4D 5D 56 56 22 20 C0 09 44 56 42 5F 48
4F 53 54 00 C1 0E 44 56 42 5F 43 49 5F 4D 4F 44
55 4C 45 00 14 00 FF 7D 65 1C 54 15 7E B1 81 A6
9B 53 F2 26 83 AD 4E E6 1C 40 F5 91 73 57 C5 81
F8 96 9B B9 CE EF 7F 26 33 BB D0 67 9A 5B 31 8A
CD 1A 3D B5 BE A1 8B A1 52 1C E1 81 E3 6C 4A 51
AC 58 3E B7 EA A2 F8 8E 3B CF 92 76 A1 1F 60 95
6E F5 0F 44 10 27 FB BA 0B CD D1 60 34 4E E6 57
65 4C B0 02 5F 69 B1 79 6B 36 F7 65 35 BD 7A 5B
88 47 0A C2 03 F1 5A D5 52 34 45 22 8C D1 74 6E
8D F3 7D 9D 67 57 CD A6 8B 1D 63 54 D7 3E 7E
*/

/* Neotion CI+ prototype 캠 : 255 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 12 05 00 4E
45 4F 54 49 4F 4E 00 43 41 4D 00 43 49 00 FF 20
04 FF FF 01 00 1A 15 01 0F FE 01 01 C0 0E 41 02
44 56 42 5F 43 49 5F 56 31 2E 30 30 1B 0E C2 41
09 37 55 4D 5D 56 56 AA 60 20 03 03 1B 26 CF 04
19 37 55 4D 5D 56 56 22 20 C0 09 44 56 42 5F 48
4F 53 54 00 C1 0E 44 56 42 5F 43 49 5F 4D 4F 44
55 4C 45 00 14 00 FF 1E 1A 38 ED A0 3F 4F 30 98
1A 58 28 41 E4 B1 DE 2E 00 82 A2 91 82 41 31 33
64 C3 B1 FB B2 D5 64 7D 9F 79 BA 87 43 9A E7 7A
5D ED 25 19 0B 01 F9 86 A1 88 2B 10 9C 6F 40 E0
A7 5E 9B 51 FC E5 DF 6A 65 2F F5 7A ED 73 66 57
95 50 D3 28 AE 2D 88 06 81 E3 72 E6 F8 46 16 2A
BF 8B AB 43 F4 6E E3 AD D3 FF 37 7F B6 F7 3C EC
08 00 86 20 8A F4 42 41 45 29 A5 50 CA 08 14 00
3A 0F DF F1 F6 EB BD 57 BB AC 17 01 A5 BB E2
*/

/* SMiT CI+ prototype 캠 : 272 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 16 05 00 53
4D 49 54 00 44 56 42 20 43 41 20 4D 6F 64 75 6C
65 00 FF 20 04 FF FF 01 00 1A 15 01 0F 20 02 01
C0 0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30
1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03
50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0
09 44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F
43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
FF 0B 35 20 9F C1 12 FE 0F 92 E0 9F 88 B6 60 5F
CE 55 33 51 4B 96 26 8E 0B 0F 88 6E CA 77 46 E5
B9 B1 CC 8D FB F5 E1 DF B5 3B F6 DE BD EA 70 F0
48 91 80 19 81 58 03 08 A8 A2 7B 03 8F CE 22 D3
B8 CA AB E8 25 47 DF A1 0E F3 15 50 80 E3 91 8E
E4 20 48 BF 7C EC E1 F9 AC A0 BF 56 84 64 FF 39
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

/* SMiT CI+ prototype 캠 : 272 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 16 05 00 53
4D 49 54 00 44 56 42 20 43 41 20 4D 6F 64 75 6C
65 00 FF 20 04 FF FF 01 00 1A 15 01 0F 20 02 01
C0 0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30
1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03
50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0
09 44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F
43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
2C AC 32 75 8B A4 39 58 23 CE CD CE 8D 86 0C 2C
75 FC 11 F6 ED 4E 9C 07 AE 59 BD 21 B9 D1 16 07
A8 93 73 85 06 74 34 C1 C5 C4 A1 A8 A4 A0 4B 28
67 15 50 30 67 E3 B2 68 81 62 6A 91 04 12 ED D5
A3 26 7A 78 AE D5 23 F6 59 CF 11 01 2E 50 31 0A
C4 3A 2F 81 FA B3 F4 96 62 BB 45 9B 74 81 F4 04
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

/* SMiT CI+ prototype 캠 : 272 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 16 05 00 53
4D 49 54 00 44 56 42 20 43 41 20 4D 6F 64 75 6C
65 00 FF 20 04 FF FF 01 00 1A 15 01 0F 20 02 01
C0 0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30
1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03
50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0
09 44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F
43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
AC D6 1B 8A 17 44 49 59 76 ED 8D 91 A6 E4 2B BA
B7 D7 1A E4 4B A0 BF 28 C2 1B 4C 52 E7 EC 7A 1C
81 42 BC EA D0 91 85 64 B8 08 2F AF BA 0A 52 8F
C1 DE DE 05 58 BE 0F 25 B0 0A 85 DB CA 0F 8D BD
92 6F 40 FC 74 06 55 4B 39 F6 91 DF 70 76 3E 81
F9 34 35 66 4A 1B A8 FC A0 B9 DB AA 21 DF 9E 34
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

/* SMiT CI+ prototype 캠 with 정식 인증서 : 272 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 2D 05 00 53
4D 49 54 00 44 56 42 20 43 41 20 4D 6F 64 75 6C
65 00 24 63 6F 6D 70 61 74 69 62 6C 65 5B 63 69
70 6C 75 73 3D 31 5D 24 00 FF 20 04 FF FF 01 00
1A 15 01 0F 20 02 01 C0 0E 41 02 44 56 42 5F 43
49 5F 56 31 2E 30 30 1B 11 C9 41 19 37 55 4E 5E
1D 56 AA 60 20 03 03 50 FF FF 1B 25 CF 04 09 37
55 4D 5D 1D 56 22 C0 09 44 56 42 5F 48 4F 53 54
00 C1 0E 44 56 42 5F 43 49 5F 4D 4F 44 55 4C 45
00 14 00 FF 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

/* Digital Keystone HPNX DVB CI+ 테스트 킷 캠 : 255 bytes
1D 02 00 FF 1C 03 00 F8 0F 15 34 05 00 44 69 67
69 74 61 6C 20 4B 65 79 73 74 6F 6E 65 00 48 50
4E 58 20 44 56 42 00 57 69 74 68 20 49 6E 74 65
72 72 75 70 74 20 53 75 70 70 6F 72 74 00 FF 20
04 00 00 00 00 1A 17 01 0F FE 01 01 C0 10 41 02
44 56 42 5F 43 49 5F 56 31 2E 30 30 00 FF 1B 20
C6 04 18 22 20 C0 09 44 56 42 5F 48 4F 53 54 00
C1 0E 44 56 42 5F 43 49 5F 4D 4F 44 55 4C 45 00
14 00 FF 00 01 00 FF FF 00 00 00 00 00 00 FF 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/
}
#endif // _CI_DEBUG_ // _CI_PLUS_DEBUG_

	/* Check the CIS and find the COR Address */
	#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	if (PHYS_CheckCIS(cis, mem.rlen, &addrcor, &cor, &ciplus) < 0)
	#else
	if (PHYS_CheckCIS(cis, mem.rlen, &addrcor, &cor) < 0)
	#endif
	{
		PrintErr(("PHYS_CIS : PHYS_CheckCIS reports wrong CIS !!!\n"));
		ITC_DynFree(cis);
		return KO;		/* wrong CIS */
	}
	ITC_DynFree(cis);

	#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	PrintDbg(("PHYS_CIS : addrcor=%x, cor=%x, ciplus=%d\n", addrcor, cor, ciplus));
	DRVTab[index].Ci_plus = (unsigned short)ciplus;
	#else
	PrintDbg(("PHYS_CIS : addrcor=%x, cor=%x\n", addrcor, cor));
	#endif

#ifndef USE_DOUGLAS_PRIMITIVE_FUNC

	/* the CIS is OK, addrcor contains the COR Address and cor the COR value */
	mem.addr   = addrcor >> 1;
	mem.len    = 1; 	/* one byte */
	mem.pbytes = &cor;

	iRet = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_WRITEMEM, &mem);
	if ((iRet >= 0) && (mem.rlen == 1))
		return OK;
	else
		return MEM_ERR; /* error while writing the COR */

#else /* for douglas */

	iRet = DRV_CI_WriteCOR(DRVTab[index].Slot_id);
	if (iRet >= 0)
		return OK;
	else
		return MEM_ERR; /* error while writing the COR */

#endif
}

/*******************************************************************************
Name:			unsigned char PHYS_ResetModule(int index, unsigned int Host_Size, unsigned int *Negociate_Size)
Description:	Reset module.
In: 				int index : DRVTab index
					unsigned int Decoder_Size: Size of Host exchange buffer.
Out:				unsigned int *Negociate_Size: Size of exchange buffer after negociation.
Return value:	RS_ERR: Reset Error.
					MEM_ERR:	Memory allocation error.
******************************************************************************/
unsigned char PHYS_ResetModule(int index, unsigned int Host_Size, unsigned short *Negociate_Size)
{
	unsigned char	buff[MAXLNSIZE];
	unsigned short	Size;
	unsigned char	status;
	int 			count;
	DRV_ssSignal	signal;	/* DRV_Ioctl argument */
	int				Ret;

//	unsigned long	st, et;

	/* CLEAR the EMSTREAM pin */
	signal.cs  = 0;
	signal.pin = DRV_EMSTREAM;
	Ret = DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_SSIGNAL, &signal);
	if (Ret < 0)
	{
		PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
		return(RS_ERR);
	}

	/* set RS to Reset PSCAM Interface */
//	Command_Register |= (RS|DAIE|FRIE);
//  Command_Register |= RS;
	Command_Register = RS;
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
		return(RS_ERR);
	}

	/* down RS */
//  Command_Register &= (0xFF-RS);
	Command_Register = 0x00;
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
		return(RS_ERR);
	}

	/* wait for module is free (standard state of DVB interface) */
	count=0;
	while(1)
	{
		Status_Register = 0x00;
		if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register)) == W_SEACH)
		{
			PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return(RS_ERR);
		}

//		Status_Register &= STATUS_MASK;
		if(Status_Register & FR)
			break;

		ITC_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return(RS_ERR);
		}
	}

	/* perform buffer size exchange */
	/* set SR bit from Command register */
//	st = GetTick();
//  Command_Register |= SR;
	Command_Register = SR;
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
		return(RS_ERR);
	}

	count=0;
	while(1)
	{
		buff[0] = buff[1] = 0; // VK_memset(buff, 0, MAXLNSIZE);
		if((status=PHYS_ReceiveData(index, buff, &Size, 1)) == W_SEACH)
		{
			PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return(RS_ERR);
		}

		if(status != M_DATA && status != M_BUSY)
			break;

		ITC_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return(RS_ERR);
		}
	}

	(*Negociate_Size) = buff[0] * 256 + buff[1];

	#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
	VK_Print("PHYS_ResetModule : Size = %d, Negociate_Size = %d, Host_Size = %d\n", Size, *Negociate_Size, Host_Size);
	#endif

	if (Size != 2)
	{
		PrintErr(("[CI_RW_ERROR] PHYS_ResetModule : Wrong Size %d !!!\n", Size));
	}

	if((*Negociate_Size) > Host_Size)
	{
		PrintErr(("[CI_RW_ERROR] PHYS_ResetModule : Negociate_Size = %d > Host_Size = %d !!!\n", *Negociate_Size, Host_Size)); // 이 경우 지원 가능하다면 Host_Size (MAXLNSIZE) 를 늘려주는 것이 좋다.
		buff[0] = Host_Size >> 8;
		buff[1] = Host_Size;
		(*Negociate_Size) = Host_Size;
	}

	/* down SR bit from Command register */
//  Command_Register &= (0xFF-SR);
	Command_Register = 0x00;
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
		return(W_SEACH);
	}

//	et = GetTick();
//	PrintDbg(("SR lap time = %d\n", et - st));

//	st = GetTick();
/* set SW bit from Command register */
//  Command_Register |= SW;
	Command_Register = SW;
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
		return(W_SEACH);
	}

	/* send the buffer size to the module */
	count=0;
	do
	{
		// spec. dose not meet in PHYS_SendSize - 1999/6/15
		//		if((status=PHYS_SendSize(index, buff, 2))==W_SEACH)
		// modify meet for ci spec.
		if((status=PHYS_SendData(index, buff, 2)) == W_SEACH)
		{
			PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return(RS_ERR);
		}

		ITC_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return(RS_ERR);
		}

	} while(status == M_BUSY);

	/* down SW bit from Command register */
//  Command_Register &= (0xFF-SW);
	Command_Register = 0x00;
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register) == W_SEACH)
	{
		PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
		return(RS_ERR);
	}

//	et = GetTick();
//	PrintDbg(("SR lap time = %d\n", et - st));

	/* wait for module is free (standard state of DVB interface) */
	count=0;
	while(1)
	{
		Status_Register = 0x00;
		if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register)) == W_SEACH)
		{
			PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return(RS_ERR);
		}

//		Status_Register &= STATUS_MASK;
		if(Status_Register & FR)
			break;

		ITC_Sleep(PHYS_DELAY);
		count++;
		if(count == PHYS_TIMEOUT)
		{
			PrintErr(("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__));
			return(RS_ERR);
		}
	}

	return(0);
}

/*******************************************************************************
Name:			unsigned char PHYS_SendData( unsigned char *pData, unsigned short Size_in)
Description:	Send data to PSCAM.
In: 				int 			index:	 DRVTab index
					unsigned char	*pData:  Pointer to data array
					unsigned short	Size_in: size of data.
Out:				Nothing.
Return value:	Status:
						OK: 	No Pb.
						M_BUSY: DVB Module is busy.
						T_ERR:	Transfert Error
						W_SEARCH
******************************************************************************/
#ifdef	USE_CI_FLASH_SEM
extern	unsigned long	s_hCiAccess;
static	unsigned char	humax_ci_access_state;
typedef enum
{
	CI_ACCESS_OFF=0,
	CI_ACCESS_ON
}CI_ACCESS_STATE;
#endif
unsigned char PHYS_SendData(int index, unsigned char *pData, unsigned short Size_in)
{
	if(DRVTab[index].State == DRV_IDLE) return W_SEACH;

	/* perhaps received data are present  : Test DA */
#ifdef	USE_CI_FLASH_SEM
	VK_SEM_Get(s_hCiAccess);
	humax_ci_access_state=CI_ACCESS_ON;
#endif
	Status_Register = 0x00;
	if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

//	Status_Register &= STATUS_MASK;
	if (Status_Register & DA)
	{
#ifdef	USE_CI_FLASH_SEM
		VK_SEM_Release(s_hCiAccess);
#endif
		PHYS_ReceiveIdle(index);
#ifdef	USE_CI_FLASH_SEM
		VK_SEM_Get(s_hCiAccess);
		humax_ci_access_state=CI_ACCESS_ON;
#endif
	}

	/* set bit HC from Command register */
	#if 0 // org
	Command_Register |= HC;
	#else
	Command_Register = HC;
	#endif
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH) return(W_SEACH);

	/* Test FR */
	Status_Register = 0x00;
	if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
		/* Down  bit HC from Command register */
//		Command_Register &= (0xFF-HC);
		Command_Register = 0x00;
		if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
		{
#ifdef	USE_CI_FLASH_SEM
			humax_ci_access_state=CI_ACCESS_OFF;
			VK_SEM_Release(s_hCiAccess);
#endif
			return(W_SEACH);
		}
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

//	PrintDbg(("status=0x%02x\n", Status_Register));
//	Status_Register &= STATUS_MASK;

	if(!(Status_Register & FR)) /* if dvb module is busy */
	{
		/* Down  bit HC from Command register */
//		Command_Register &= (0xFF-HC);
		Command_Register = 0x00;
		if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
		{
#ifdef	USE_CI_FLASH_SEM
			humax_ci_access_state=CI_ACCESS_OFF;
			VK_SEM_Release(s_hCiAccess);
#endif
			return(W_SEACH);
		}
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(M_BUSY);
	}

	/* set bit HC from Command register */
//	Command_Register|=HC;
//	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH) return(W_SEACH);

	/* write data size */
	if(PHYS_WriteByte(index, OFF_SIZE_LS_REG, ( unsigned char)Size_in)==W_SEACH)
	{
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}
	if(PHYS_WriteByte(index, OFF_SIZE_MS_REG, ( unsigned char)(Size_in>>8))==W_SEACH)
	{
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

	/* write data */
	/* ========== */
	if (DRV_CI_Write(DRVTab[index].Slot_id, Size_in, pData) != (int)Size_in)
	{
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

	/* Verify if WE is down */
	Status_Register = 0x00;
	if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

//	Status_Register &= STATUS_MASK;

	if(Status_Register & WE)
	{
		/* Down  bit HC from Command register */
//		Command_Register &= (0xFF-HC);
		Command_Register = 0x00;
		if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
		{
#ifdef	USE_CI_FLASH_SEM
			humax_ci_access_state=CI_ACCESS_OFF;
			VK_SEM_Release(s_hCiAccess);
#endif
			return(W_SEACH);
		}
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(T_ERR);
	}

	/* Down  bit HC from Command register */
	#if 0 // org
	Command_Register &= (0xFF-HC);
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
	{
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}
	#else
	Command_Register = 0x00;
	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH)
	{
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}
	#endif

#ifdef TSB_ERROR_HANDLING
	uc_Expected_Tsb[0] = uc_Expected_Tsb[4] = pData[0];
#endif
#ifdef	USE_CI_FLASH_SEM
	VK_SEM_Release(s_hCiAccess);
#endif
	return(OK);
}

#else
/*******************************************************************************
Name:			int PHYS_CIS(int index)

Description:	Read the CIS, Check it and Write the COR
In: 				int index : DRVTab index
Out:
Return value:
Return value:	KO : wrong CIS
				MEM_ERR : error
				OK : signal detected
******************************************************************************/
unsigned char PHYS_CIS(int index)
{
	unsigned char *cis; 			/* CIS buffer		 */
	unsigned short addrcor; 		/* COR Address		 */
	unsigned char cor;				/* COR value		 */
	#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	unsigned char ciplus;			/* CI+ CAM detected  */
	#endif
	int iRet;						/* return code		 */
	unsigned short cisLen;
	unsigned short rcisLen = 0;
	unsigned char cnt = 0;

	cisLen = 1024; /* <TODO_CI> : 체크 요함 */ /* see CI_CIS_SIZE at the DRV_ci_drv.h */
	cis = (unsigned char *)ITC_DynAlloc(cisLen);
	if(!cis)
	{
		PrintErr(("PHYS_CIS : not enought memory (%d) !!!\n", cisLen));
		return MEM_ERR;
	}

	VK_memset(cis, 0, cisLen);
	iRet = DRV_CI_Get_CIS(DRVTab[index].Slot_id,cis ,&rcisLen);
	if (iRet != 0)
	{
		ITC_DynFree(cis);
		PrintErr(("PHYS_CIS : DRV_CI_ReadCIS error (%d) !!!\n", iRet));
		return MEM_ERR; 	/* error while reading CIS */
	}

#ifdef _CI_DEBUG_ // #ifdef _CI_PLUS_DEBUG_ // CI+ 스펙에 명시된 CI Plus Compatibility Identification 확인 용도임.
{
	int i;
	VK_Print("\n --- CIS : cisLen = %d, rlen = %d ---\n", cisLen, rcisLen);
	for (i = 0; i < rcisLen; i++)
	{
		if ((i & 15) == 0)
			VK_Print("\n %03d :", i);
		VK_Print(" %02X", cis[i]);
	}
	VK_Print("\n\n");

/* Irdeto blue 캠 - 일명 유로 캠 (CIv1) : 255 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 15 05 00 53
43 4D 00 44 56 42 20 43 41 20 4D 6F 64 75 6C 65
00 FF 20 04 FF FF 01 00 1A 15 01 0F FE 01 01 C0
0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30 1B
11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03 50
FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0 09
44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F 43
49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 0C 60 C2 00 41 10 84 00 03 00 00 41 00 2A
02 04 50 33 08 48 C0 00 4C 28 00 20 00 80 00 42
40 80 05 C4 09 40 42 08 20 00 02 45 48 40 40 00
08 09 00 02 00 00 00 C0 00 00 00 06 1A 2C 00 18
40 00 02 0C 00 22 20 00 00 10 10 40 00 04 00 81
44 0C 00 24 02 11 00 50 00 18 40 92 40 10 00
*/

/* AlphaCrypt 캠 (CIv1) : 255 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 15 05 00 53
43 4D 00 44 56 42 20 43 41 20 4D 6F 64 75 6C 65
00 FF 20 04 FF FF 01 00 1A 15 01 0F FE 01 01 C0
0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30 1B
11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03 50
FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0 09
44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F 43
49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
06 08 00 08 02 E5 11 06 20 70 44 D9 83 00 2C 92
00 80 0A 08 00 0B A0 00 00 63 28 D0 04 12 04 0E
00 80 00 58 84 20 41 67 01 45 0C 80 E0 00 80 0C
20 00 44 02 20 00 00 C8 04 00 B9 88 00 60 C8 33
26 00 6C 08 10 10 C3 58 01 D8 00 80 A0 20 CC 14
08 04 10 05 04 01 82 B1 30 1C 00 A3 00 40 A0
*/

/* SmarDTV CI+ prototype 캠 : 511 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 19 05 00 53
6D 61 72 44 54 56 00 44 56 42 20 43 41 20 4D 6F
64 75 6C 65 00 FF 20 04 FF FF 01 00 1A 15 01 0F
FE 03 01 C0 0E 41 02 44 56 42 5F 43 49 5F 56 31
2E 30 30 1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60
20 03 03 50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D
56 22 C0 09 44 56 42 5F 48 4F 53 54 00 C1 0E 44
56 42 5F 43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF
83 EF AD ED 41 02 BE E3 00 21 9D AD 98 C4 1A 02
F5 0F 1C 78 23 93 67 10 B5 EB 13 29 A3 37 06 15
E6 9C 71 02 B5 08 68 C1 DA 43 03 EA 9A 92 64 8D
75 56 08 DA C3 90 C8 21 3B 1A A2 26 75 24 42 02
0F 80 C4 E0 23 68 83 26 A4 61 72 25 C1 AF C8 46
81 4C 49 4A B6 5A A2 24 19 62 0A D5 7F 32 CA 81
5B 2A 5C B1 82 00 00 E5 63 66 94 CA 70 42 5B 10
69 05 C2 CB 36 22 34 27 0C 8A 57 22 08 C3 4A 43
2F 8E C5 8A 22 B8 4E 23 3A 77 95 EE FC 00 38 08
46 3E 8E 29 0C F7 82 B6 B4 72 C9 B9 DA 32 20 02
17 98 17 57 E5 27 82 46 33 E7 E1 36 5E 42 08 08
B3 E5 25 2B CE 29 FA 93 1A A4 88 2E 03 AD 4A 16
05 E8 DD 7A 63 C1 84 67 18 60 53 E5 67 72 C0 1E
3D 2E E5 48 66 81 82 74 28 20 43 64 05 89 C9 02
2F EE 52 04 9B 06 3F B0 3D 16 03 60 AA 17 4B 34
11 A4 87 18 47 28 25 21 04 62 41 34 E6 A7 2B 36
00 76 C2 69 BB 33 C4 06 20 B9 7D 23 1D 03 C2 07
14 05 D4 40 2F 1C 4C 07 AC EE DB 75 A1 A9 8D 5D
06 90 B1 41 9B D4 8C 57 02 FA ED A1 CF 38 DC 44
3E 0C 82 03 3B 54 20 26 1A 60 D2 A6 B1 2A 0A 10
B5 2F 56 C2 D6 20 45 81 F6 60 51 D5 36 28 EA 06
44 9A 10 34 52 BE BF 5D 33 78 18 87 64 30 E0 3A
C7 9A FE 0A 3C A7 64 03 9B A1 48 26 30 63 E1 00
19 8A DA E8 89 70 61 54 2B 70 D2 CC A4 76 0D
*/

/* SmarDTV CI+ prototype 캠 : 511 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 19 05 00 53
6D 61 72 44 54 56 00 44 56 42 20 43 41 20 4D 6F
64 75 6C 65 00 FF 20 04 FF FF 01 00 1A 15 01 0F
FE 03 01 C0 0E 41 02 44 56 42 5F 43 49 5F 56 31
2E 30 30 1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60
20 03 03 50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D
56 22 C0 09 44 56 42 5F 48 4F 53 54 00 C1 0E 44
56 42 5F 43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF
5C 68 31 71 69 7D DC 99 18 E0 13 0A 38 50 A8 06
D0 08 E3 06 49 05 03 5F 09 2E 72 64 00 34 33 00
6C 0E 51 B7 03 0B C5 4B 7A A0 89 00 38 42 18 8C
1D 19 9E 27 CD 25 81 0D 2F 4E 81 2F 39 4E 2C 1D
57 8E 00 72 5A 33 40 49 21 24 C3 F2 89 54 14 0B
85 00 36 35 50 96 D0 01 15 0A 03 20 64 4D 58 18
25 A8 8B 05 90 54 64 C1 04 2A EC 41 77 BA 14 BB
AB A9 FB 3C 48 5A 90 55 04 E1 72 20 6A ED B0 98
48 0F BA 32 56 50 45 82 64 62 46 18 1B 35 5F 44
9A 9E 86 9A 60 1D 57 9F 67 7C 12 19 51 47 40 10
8F 4A D3 C3 48 28 24 DF 0D 21 04 FA 57 0D 77 10
54 63 50 3E D8 00 21 46 87 17 43 69 2E AE F2 FA
64 2C 0B 8A 35 29 17 05 48 49 14 A8 13 32 7C 28
5E 85 F6 E4 03 66 09 75 35 6F 85 12 A8 D5 2D A4
23 23 36 98 60 89 B6 92 02 00 39 4E 5E 40 7D 22
FC 6B D7 19 02 D8 06 00 A3 B9 77 41 61 21 9E 99
43 01 52 11 8D 2E AE B2 5C 99 94 8E AC 94 FE 62
45 00 24 62 15 A0 73 4B BE 04 08 4A 51 CD 68 80
20 94 F2 B4 29 34 30 0A 61 E2 12 56 24 AD 20 03
CF E9 82 F8 EA 2E D8 09 6E 7B 3A 0A 28 76 70 68
28 89 92 18 03 BB 20 81 3F 20 EE 3E 3A 81 99 11
AE 9C 12 24 50 0B 81 7E F7 45 2D 0C B0 E5 0B 96
FB 23 71 B4 1E F0 4F 09 DA B7 43 82 FC 6F 4B F4
E5 AF 81 F6 25 47 14 6D 6A 29 46 04 BA D7 D8
*/

/* Neotion 짝퉁 CI+ 캠 with CI+ MHEG demo : 255 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 12 05 00 4E
45 4F 54 49 4F 4E 00 4E 50 34 00 43 49 00 FF 20
04 FF FF 01 00 1A 15 01 0F FE 01 01 C0 0E 41 02
44 56 42 5F 43 49 5F 56 31 2E 30 30 1B 0E C2 41
09 37 55 4D 5D 56 56 AA 60 20 03 03 1B 26 CF 04
19 37 55 4D 5D 56 56 22 20 C0 09 44 56 42 5F 48
4F 53 54 00 C1 0E 44 56 42 5F 43 49 5F 4D 4F 44
55 4C 45 00 14 00 FF 7D 65 1C 54 15 7E B1 81 A6
9B 53 F2 26 83 AD 4E E6 1C 40 F5 91 73 57 C5 81
F8 96 9B B9 CE EF 7F 26 33 BB D0 67 9A 5B 31 8A
CD 1A 3D B5 BE A1 8B A1 52 1C E1 81 E3 6C 4A 51
AC 58 3E B7 EA A2 F8 8E 3B CF 92 76 A1 1F 60 95
6E F5 0F 44 10 27 FB BA 0B CD D1 60 34 4E E6 57
65 4C B0 02 5F 69 B1 79 6B 36 F7 65 35 BD 7A 5B
88 47 0A C2 03 F1 5A D5 52 34 45 22 8C D1 74 6E
8D F3 7D 9D 67 57 CD A6 8B 1D 63 54 D7 3E 7E
*/

/* Neotion CI+ prototype 캠 : 255 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 12 05 00 4E
45 4F 54 49 4F 4E 00 43 41 4D 00 43 49 00 FF 20
04 FF FF 01 00 1A 15 01 0F FE 01 01 C0 0E 41 02
44 56 42 5F 43 49 5F 56 31 2E 30 30 1B 0E C2 41
09 37 55 4D 5D 56 56 AA 60 20 03 03 1B 26 CF 04
19 37 55 4D 5D 56 56 22 20 C0 09 44 56 42 5F 48
4F 53 54 00 C1 0E 44 56 42 5F 43 49 5F 4D 4F 44
55 4C 45 00 14 00 FF 1E 1A 38 ED A0 3F 4F 30 98
1A 58 28 41 E4 B1 DE 2E 00 82 A2 91 82 41 31 33
64 C3 B1 FB B2 D5 64 7D 9F 79 BA 87 43 9A E7 7A
5D ED 25 19 0B 01 F9 86 A1 88 2B 10 9C 6F 40 E0
A7 5E 9B 51 FC E5 DF 6A 65 2F F5 7A ED 73 66 57
95 50 D3 28 AE 2D 88 06 81 E3 72 E6 F8 46 16 2A
BF 8B AB 43 F4 6E E3 AD D3 FF 37 7F B6 F7 3C EC
08 00 86 20 8A F4 42 41 45 29 A5 50 CA 08 14 00
3A 0F DF F1 F6 EB BD 57 BB AC 17 01 A5 BB E2
*/

/* SMiT CI+ prototype 캠 : 272 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 16 05 00 53
4D 49 54 00 44 56 42 20 43 41 20 4D 6F 64 75 6C
65 00 FF 20 04 FF FF 01 00 1A 15 01 0F 20 02 01
C0 0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30
1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03
50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0
09 44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F
43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
FF 0B 35 20 9F C1 12 FE 0F 92 E0 9F 88 B6 60 5F
CE 55 33 51 4B 96 26 8E 0B 0F 88 6E CA 77 46 E5
B9 B1 CC 8D FB F5 E1 DF B5 3B F6 DE BD EA 70 F0
48 91 80 19 81 58 03 08 A8 A2 7B 03 8F CE 22 D3
B8 CA AB E8 25 47 DF A1 0E F3 15 50 80 E3 91 8E
E4 20 48 BF 7C EC E1 F9 AC A0 BF 56 84 64 FF 39
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

/* SMiT CI+ prototype 캠 : 272 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 16 05 00 53
4D 49 54 00 44 56 42 20 43 41 20 4D 6F 64 75 6C
65 00 FF 20 04 FF FF 01 00 1A 15 01 0F 20 02 01
C0 0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30
1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03
50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0
09 44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F
43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
2C AC 32 75 8B A4 39 58 23 CE CD CE 8D 86 0C 2C
75 FC 11 F6 ED 4E 9C 07 AE 59 BD 21 B9 D1 16 07
A8 93 73 85 06 74 34 C1 C5 C4 A1 A8 A4 A0 4B 28
67 15 50 30 67 E3 B2 68 81 62 6A 91 04 12 ED D5
A3 26 7A 78 AE D5 23 F6 59 CF 11 01 2E 50 31 0A
C4 3A 2F 81 FA B3 F4 96 62 BB 45 9B 74 81 F4 04
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

/* SMiT CI+ prototype 캠 : 272 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 16 05 00 53
4D 49 54 00 44 56 42 20 43 41 20 4D 6F 64 75 6C
65 00 FF 20 04 FF FF 01 00 1A 15 01 0F 20 02 01
C0 0E 41 02 44 56 42 5F 43 49 5F 56 31 2E 30 30
1B 11 C9 41 19 37 55 4E 5E 1D 56 AA 60 20 03 03
50 FF FF 1B 25 CF 04 09 37 55 4D 5D 1D 56 22 C0
09 44 56 42 5F 48 4F 53 54 00 C1 0E 44 56 42 5F
43 49 5F 4D 4F 44 55 4C 45 00 14 00 FF 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
AC D6 1B 8A 17 44 49 59 76 ED 8D 91 A6 E4 2B BA
B7 D7 1A E4 4B A0 BF 28 C2 1B 4C 52 E7 EC 7A 1C
81 42 BC EA D0 91 85 64 B8 08 2F AF BA 0A 52 8F
C1 DE DE 05 58 BE 0F 25 B0 0A 85 DB CA 0F 8D BD
92 6F 40 FC 74 06 55 4B 39 F6 91 DF 70 76 3E 81
F9 34 35 66 4A 1B A8 FC A0 B9 DB AA 21 DF 9E 34
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

/* SMiT CI+ prototype 캠 with 정식 인증서 : 272 bytes
1D 04 00 DB 08 FF 1C 03 00 08 FF 15 2D 05 00 53
4D 49 54 00 44 56 42 20 43 41 20 4D 6F 64 75 6C
65 00 24 63 6F 6D 70 61 74 69 62 6C 65 5B 63 69
70 6C 75 73 3D 31 5D 24 00 FF 20 04 FF FF 01 00
1A 15 01 0F 20 02 01 C0 0E 41 02 44 56 42 5F 43
49 5F 56 31 2E 30 30 1B 11 C9 41 19 37 55 4E 5E
1D 56 AA 60 20 03 03 50 FF FF 1B 25 CF 04 09 37
55 4D 5D 1D 56 22 C0 09 44 56 42 5F 48 4F 53 54
00 C1 0E 44 56 42 5F 43 49 5F 4D 4F 44 55 4C 45
00 14 00 FF 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/

/* Digital Keystone HPNX DVB CI+ 테스트 킷 캠 : 255 bytes
1D 02 00 FF 1C 03 00 F8 0F 15 34 05 00 44 69 67
69 74 61 6C 20 4B 65 79 73 74 6F 6E 65 00 48 50
4E 58 20 44 56 42 00 57 69 74 68 20 49 6E 74 65
72 72 75 70 74 20 53 75 70 70 6F 72 74 00 FF 20
04 00 00 00 00 1A 17 01 0F FE 01 01 C0 10 41 02
44 56 42 5F 43 49 5F 56 31 2E 30 30 00 FF 1B 20
C6 04 18 22 20 C0 09 44 56 42 5F 48 4F 53 54 00
C1 0E 44 56 42 5F 43 49 5F 4D 4F 44 55 4C 45 00
14 00 FF 00 01 00 FF FF 00 00 00 00 00 00 FF 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*/
}
#endif // _CI_DEBUG_ // _CI_PLUS_DEBUG_

	/* Check the CIS and find the COR Address */
	#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	if (PHYS_CheckCIS(cis, rcisLen, &addrcor, &cor, &ciplus) < 0)
	#else
	if (PHYS_CheckCIS(cis, mem.rlen, &addrcor, &cor) < 0)
	#endif
	{
		PrintErr(("PHYS_CIS : PHYS_CheckCIS reports wrong CIS !!!\n"));
		ITC_DynFree(cis);
		return KO;		/* wrong CIS */
	}
	ITC_DynFree(cis);

	#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	DRVTab[index].Ci_plus = (unsigned short)ciplus;
	#else
	PrintDbg(("PHYS_CIS : addrcor=%x, cor=%x\n", addrcor, cor));
	#endif

AGAIN:
	iRet = DRV_CI_Write_COR(DRVTab[index].Slot_id,addrcor,cor);
	if (iRet == 0)
	{
		return OK;
	}
	else
	{
		if(cnt > 10)
		{
		
			return MEM_ERR; /* error while writing the COR */
		}
		else
		{
			VK_TASK_Sleep(100);
			goto AGAIN;

		}
		cnt++;	
		
	}

	return OK;

}

/*******************************************************************************
Name:			unsigned char PHYS_ResetModule(int index, unsigned int Host_Size, unsigned int *Negociate_Size)
Description:	Reset module.
In: 				int index : DRVTab index
					unsigned int Decoder_Size: Size of Host exchange buffer.
Out:				unsigned int *Negociate_Size: Size of exchange buffer after negociation.
Return value:	RS_ERR: Reset Error.
					MEM_ERR:	Memory allocation error.
******************************************************************************/
unsigned char PHYS_ResetModule(int index, unsigned int Host_Size, unsigned short *Negociate_Size)
{
	int				Ret;

	/* negociation size */
	Ret = DRV_CI_Negociation (DRVTab[index].Slot_id,(unsigned short )Host_Size, Negociate_Size);
	if(Ret != 0)
		return(RS_ERR);

	return(0);
}

unsigned char PHYS_SendData(int index, unsigned char *pData, unsigned short Size_in)
{
	int ret = 0;

	if(DRVTab[index].State == DRV_IDLE) return W_SEACH;

	/* perhaps received data are present  : Test DA */
#ifdef	USE_CI_FLASH_SEM
	VK_SEM_Get(s_hCiAccess);
	humax_ci_access_state=CI_ACCESS_ON;
#endif

	/* write data */
	/* ========== */
	ret = DRV_CI_Write_LPDU(DRVTab[index].Slot_id,pData,Size_in);
	if (ret != 0)
	{
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

#ifdef TSB_ERROR_HANDLING
	uc_Expected_Tsb[0] = uc_Expected_Tsb[4] = pData[0];
#endif
#ifdef	USE_CI_FLASH_SEM
	VK_SEM_Release(s_hCiAccess);
#endif
	return(OK);
}
#endif



#if 0
/*******************************************************************************
Name:			unsigned char PHYS_SendSize( unsigned char *pData, unsigned short Size_in)
Description:	Send data to PSCAM.
In: 				int 			index:	 DRVTab index
					unsigned char	*pData:  Pointer to data array
					unsigned short	Size_in: size of data.
Out:				Nothing.
Return value:	Status:
						OK: 	No Pb.
						M_BUSY: DVB Module is busy.
						T_ERR:	Transfert Error
						W_SEARCH
******************************************************************************/
unsigned char PHYS_SendSize(int index, unsigned char *pData, unsigned short Size_in)
{
	if(DRVTab[index].State == DRV_IDLE) return W_SEACH;

	/* perhaps received data are present  : Test DA */
	if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
		return(W_SEACH);

	Status_Register &= STATUS_MASK;
	if (Status_Register & DA)
		PHYS_ReceiveIdle(index);

	/* Test FR */
	if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
		return(W_SEACH);

//	PrintDbg(("status=0x%02x\n", Status_Register));
	Status_Register &= STATUS_MASK;

	if(!(Status_Register&FR)) /* if dvb module is busy */
		return(M_BUSY);

	/* set bit HC from Command register */
//	Command_Register|=HC;
//	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH) return(W_SEACH);

	/* write data size */
	if(PHYS_WriteByte(index, OFF_SIZE_LS_REG, ( unsigned char)Size_in)==W_SEACH) return(W_SEACH);
	if(PHYS_WriteByte(index, OFF_SIZE_MS_REG, ( unsigned char)(Size_in>>8))==W_SEACH) return(W_SEACH);

	/* write data */
	/* ========== */
	if (DRV_Write(DRVTab[index].Slot_id, Size_in, pData) != (int)Size_in)
		return(W_SEACH);

	/* Verify if WE is down */
	if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
		return(W_SEACH);
	Status_Register=STATUS_MASK&Status_Register;

	if(Status_Register&WE)
	{
		/* Down  bit HC from Command register */
		Command_Register&=(0xFF-HC);
		if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH) return(W_SEACH);
		return(T_ERR);
	}

	/* Down  bit HC from Command register */
//	Command_Register&=(0xFF-HC);
//	if(PHYS_WriteByte(index, OFF_COM_REG, Command_Register)==W_SEACH) return(W_SEACH);

	return(OK);
}
#endif

/*******************************************************************************
Name:			void PHYS_ReceiveIdle( )
Description:	Receive data and stores LPDU for into a linked list
In: 				int index : Index into DRVTab[]
Out:			Nothing.
Return value:	Nothing
******************************************************************************/
void PHYS_ReceiveIdle(int index)
{
	unsigned char *sLPDU;
	unsigned char *p;
	unsigned short length;

	/* call PHYS_ReceiveData without linked list management since DA is set and NEW DATA are present */
	if (PHYS_ReceiveData(index, pDRVData, &length, 0)==OK)
	{
//		sLPDU = ITC_DynAlloc((unsigned short)(length+sizeof(SLPDU)-1));
		sLPDU = ITC_DynAlloc(length + sizeof(unsigned char *) + sizeof(unsigned short));
		if (sLPDU == NULL)
		{
			PrintErr(("PHYS_ReceiveIdle : not enought memory (%d) !!!\n", length + sizeof(unsigned char *) + sizeof(unsigned short)));
			return;
		}

		((SLPDU *)sLPDU)->next	 = NULL;
		((SLPDU *)sLPDU)->length = length;
		VK_memcpy(((SLPDU *)sLPDU)->pLPDU, pDRVData, length);

		if (!DRVTab[index].firstSLPDU)
		{
			/* First LPDU into the linked list */
			DRVTab[index].firstSLPDU = (SLPDU *)sLPDU;
		}
		else
		{
			/* Add LPDU at the end of the linked list */
			p = (unsigned char *)(DRVTab[index].firstSLPDU);
			while (((SLPDU *)p)->next)
				p = ((SLPDU *)p)->next;

			p = sLPDU;					// modified by shinhs to enforce arm compiler not to generate an error
		}

#if 0 // #ifdef _CI_DEBUG_
		{
			char buf_tpdu[1000];
			int j;
			/* display the received buffer in ASCII */
			for (j=0;j<(int)length;j++)
				wsprintf((char *)(buf_tpdu+(3*j)),"%02x ",*(pDRVData+j));
			buf_tpdu[(3*length)-1] = '\0';
			if (length > 50)
				buf_tpdu[200] = '\0';
			PrintDbg(("PHYS_ReceiveIdle : %d received bytes from the module %d : %s\n", length, DRVTab[index].Slot_id, buf_tpdu));
		}
#endif
	}

	return;
}


/*******************************************************************************
Name:			unsigned char PHYS_ReceiveData(
							int 		   index,
							unsigned char  *pData,
							unsigned short *Size_out,
							int flag)
Description:	Send data to PSCAM.
In: 				int index : 			   Index into DRVTab[]
					int flag :				   0 if the calling function is PHYS_ReceivedIdle
Out:				unsigned char	*pData:    Pointer to data array
					unsigned short	*Size_Out: Size of data.
Return value:	Status:
						OK: 	No Pb
						M_DATA: No Data available.
						M_BUSY: PSCAM is busy.
						T_ERR:	Transfert Error.
						W_SEARCH
******************************************************************************/
unsigned char PHYS_ReceiveData(int index, unsigned char *pData, unsigned short *Size_out, int flag)
{
	SLPDU *p;
#if defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	int ret = 0;
	unsigned short len = 0;
#endif

	if(DRVTab[index].State == DRV_IDLE)
	{
//		OutCh('!');
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
#endif
		return W_SEACH;
	}

	if (flag && DRVTab[index].firstSLPDU)
	{
		/* Received data while sending previous data */
		VK_memcpy(pData, DRVTab[index].firstSLPDU->pLPDU, DRVTab[index].firstSLPDU->length);
		*Size_out = DRVTab[index].firstSLPDU->length;

//		PrintDbg(("PHYS_ReceiveData : %d received bytes from the module %d\n", *Size_out, DRVTab[index].Slot_id));

		/* free the received buffer into the linked list and update the head */
		p = DRVTab[index].firstSLPDU;
		DRVTab[index].firstSLPDU = (SLPDU *)(DRVTab[index].firstSLPDU->next);
		ITC_DynFree((unsigned char *)p);
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
#endif
		return(OK);
	}

#ifdef	USE_CI_FLASH_SEM
	VK_SEM_Get(s_hCiAccess);
#endif

#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
	Status_Register = 0x00;
	if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
//		OutCh('@');
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

//	Status_Register &= STATUS_MASK;

#ifdef CI_PLUS_ENABLE
	if (Status_Register & IIR)
	{
//		VK_Print("<IIR>");
		if (CI_IsCiPlusCam(DRVTab[index].Slot_id)) // 일반 캠의 경우 오동작하는 넘이 있을 수 있으므로 CI+ 캠에 한해서만 하도록 하자.
		{
#ifdef	USE_CI_FLASH_SEM
			humax_ci_access_state=CI_ACCESS_OFF;
			VK_SEM_Release(s_hCiAccess);
#endif
			return(R_IIR);
		}
	}
#endif

	if(!(Status_Register & DA))
	{
//		OutCh('#');
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(M_DATA); /* if no data available */
	}

	/* data size */
	Size_LS_Register = Size_MS_Register = 0;
	if(PHYS_ReadByte(index, OFF_SIZE_LS_REG, (unsigned char *)(&Size_LS_Register))==W_SEACH)
	{
//		OutCh('$');
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}
	if(PHYS_ReadByte(index, OFF_SIZE_MS_REG, (unsigned char *)(&Size_MS_Register))==W_SEACH)
	{
//		OutCh('%');
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

	*Size_out = ((unsigned short)Size_MS_Register<<8) & 0xff00;
	*Size_out |= (unsigned short)(Size_LS_Register & 0x00ff);
	if((DRVTab[index].State == DRV_CONNECT) && (*Size_out > DRVTab[index].Negociated_size)/* || _ciTest[DRVTab[index].Slot_id]*/)
	{
		PrintErr(("[CI_RW_ERROR] PHYS_ReceiveData : read size invalid %d > %d -> PAL_CI_ResetModule(%d)...\n",
			*Size_out, DRVTab[index].Negociated_size, DRVTab[index].Slot_id));

//-----> modified by jhcho !!!
		if (PAL_CI_ResetModule(DRVTab[index].Slot_id) != ERR_OK)
		{
			PrintErr(("[CI_RW_ERROR] PHYS_ReceiveData : fail to reset slot %d !!!\n", DRVTab[index].Slot_id));
		}

#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

	if (*Size_out == 0xffff)
	{
//		OutCh('^');
		PrintErr(("[CI_RW_ERROR] PHYS_ReceiveData : read size invalid 0xffff !!!\n"));
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}
	if (*Size_out > MAXLNSIZE)
	{
//		OutCh('&');
		PrintErr(("[CI_RW_ERROR] PHYS_ReceiveData : read size invalid %d > %d !!!\n", *Size_out, MAXLNSIZE));
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}
//	PrintDbg(("read size %d\n", *Size_out));

	/* read data */
	/* ========= */
	if (DRV_CI_Read(DRVTab[index].Slot_id, *Size_out, pData) != (int)*Size_out)
	{
//		OutCh('*');
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

	Status_Register = 0x00;
	if(PHYS_ReadByte(index, OFF_STA_REG, (unsigned char *)(&Status_Register))==W_SEACH)
	{
//		OutCh('(');
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

	if (Status_Register & DA)
	{
		PrintErr(("PHYS_ReceiveData : DA\n"));

#if 1
		{
			/* CI+ MHEG 에서는 큰 사이즈의 데이터들(100KB, 200KB)이 전송될 수 있기 때문에
			   기존 TM_TRANS 값으로는 현 소스 구조상 캠이 리셋되는 문제를 야기하므로 다음과 같이 근본적으로 수정함. */

			unsigned char ucTcId = 0;
			index = 0;
			if (CIRH_GetTcId(DRVTab[index].Slot_id, &ucTcId) == ERR_OK && ucTcId != 0)
			{
				index = TRPT_GetTcid(ucTcId);
				if (index < 0)
				{
					PrintErr(("PHYS_ReceiveData : TRPT_GetTcid() error !!!\n"));
				}
				else
				{
					TRPT_Set_Timer(index);
				}
			}
			else
			{
				PrintErr(("PHYS_ReceiveData : CIRH_GetTcId() error !!!\n"));
			}
		}
#endif

//		return(M_DATA); /* <TODO_CI> : 이런 경우에 이 플로우에서 데이터를 다시 가져오도록 하는 것이 좋을 것 같은데... 좀더 고려 필요... */
	}

	if (Status_Register & RE)
	{
		PrintErr(("[CI_RW_ERROR] PHYS_ReceiveData : RE !!!\n"));
//		return(T_ERR); /* <TODO_CI> : 이런 경우에 대한 처리에 대해선 좀더 고려 필요... */
	}

#else

	/* read data */
	/* ========= */
	ret = DRV_CI_Read_LPDU(DRVTab[index].Slot_id, pData, &len);
	
	*Size_out = len;
	if (ret != 0)
	{
//		OutCh('*');
#ifdef	USE_CI_FLASH_SEM
		humax_ci_access_state=CI_ACCESS_OFF;
		VK_SEM_Release(s_hCiAccess);
#endif
		return(W_SEACH);
	}

#endif
#ifdef TSB_ERROR_HANDLING
	if (*Size_out == 6 // length 가 6 으로써 캠에서 보내는 TPDU 는 따져보면 Tsb 밖에 없다.
		&&
			((pData[0] != 0x01 && pData[0] != 0x02)
			|| (pData[1] != 0x00 && pData[1] != 0x01)
			|| (pData[2] != 0x80 /* && pData[2] != 0x83 && pData[2] != 0x84 && pData[2] != 0x85 && pData[2] != 0x86 && pData[2] != 0xA0 && pData[2] != 0xA1 */)
			|| (pData[3] != 0x02)
			|| (pData[4] != pData[0])
			|| (pData[5] != 0x00 && pData[5] != 0x80)
			)
		)
	{
		// 일단 에러 찍어 주고...
		PrintErr(("[CI_RW_ERROR] Read I/O mem failed : wrong Tsb = [ %02x %02x %02x %02x %02x %02x ] -> expected [ %02x %02x %02x %02x %02x (00 or 80) ] !!!\n",
			pData[0], pData[1], pData[2], pData[3], pData[4], pData[5],
			uc_Expected_Tsb[0], uc_Expected_Tsb[1], uc_Expected_Tsb[2], uc_Expected_Tsb[3], uc_Expected_Tsb[4]));

		// 기대되는 T_SB 값으로 강제 변경
		VK_memcpy(pData, uc_Expected_Tsb, 6);
	}
#endif

#ifdef	USE_CI_FLASH_SEM
	humax_ci_access_state=CI_ACCESS_OFF;
	VK_SEM_Release(s_hCiAccess);
#endif
	return(OK);
}

#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
/*******************************************************************************
Name:		   unsigned char PHYS_WriteByte(int index, int Reg, unsigned char data)
Description:	Write a byte on the I/O Windows.
In: 				int index,
					unsigned int Reg,
					unsigned char data,
Out:				Nothing
Return value:	Status:
						W_SEACH: Windows error.
******************************************************************************/
unsigned char PHYS_WriteByte(int index, int Reg, unsigned char data)
{
	DRV_stIO reg;	/* Ioctl parameter */

	/* Test Card Detect */
	if (PHYS_CD(index) == W_SEACH)
	{
		PrintErr(("PHYS_WriteByte : PHYS_CD error !!!\n"));
		return(W_SEACH);
	}

	reg.registr = Reg;
	reg.pvalue  = &data;
	if (DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_WRITEIO, &reg) < 0)
	{
		PrintErr(("PHYS_WriteByte : DRV_CI_Ioctl error !!!\n"));
		DRVTab[index].State = DRV_IDLE;
		return(W_SEACH);
	}
	else
	{
		return(OK);
	}
}


/*******************************************************************************
Name:		   unsigned char PHYS_ReadByte(int index, int Reg, unsigned char *data)
Description:	Read data from I/O Windows.
In: 				int index,
					unsigned int Reg,
Out:				unsigned char *data
Return value:	Status:
						W_SEACH: Windows error.
******************************************************************************/
unsigned char PHYS_ReadByte(int index, int Reg, unsigned char *data)
{
	DRV_stIO reg;	/* Ioctl parameter */

	/* Test Card Detect */
	if (PHYS_CD(index) == W_SEACH)
	{
		PrintErr(("PHYS_ReadByte : PHYS_CD error !!!\n"));
		return(W_SEACH);
	}

	reg.registr = Reg;
	reg.pvalue  = data;
	if (DRV_CI_Ioctl(DRVTab[index].Slot_id, DRV_READIO, &reg) < 0)
	{
		PrintErr(("PHYS_ReadByte : DRV_CI_Ioctl error !!!\n"));
		DRVTab[index].State = DRV_IDLE;
		return(W_SEACH);
	}
	else
	{
		return(OK);
	}
}
#endif

/* ======================================================================== */
/* NAME : PHYS_CheckCIS 													*/
/*																			*/
/* DESCRIPTION : Check the CIS												*/
/*		The following tuples must be found in this order :					*/
/*			- CISTPL_DEVICE_0A			   =============					*/
/*			- CISTPL_DEVICE_0C												*/
/*			- CISTPL_VERS_1 												*/
/*			- CISTPL_MANFID 												*/
/*			- CISTPL_CONFIG 												*/
/*			- CISTPL_CFTABLE_ENTRY											*/
/*			- CISTPL_NO_LINK 												*/
/*			- CISTPL_END													*/
/*		Other tuples are ignored											*/
/*																			*/
/* INPUT PARAMETERS :														*/
/*		cis : CIS buffer													*/
/*		len : length of the CIS buffer										*/
/*																			*/
/* OUTPUT PARAMETERS :														*/
/*		addrcor : COR address												*/
/*		cor 	: COR value 												*/
/*																			*/
/* RETURN CODE																*/
/*		   0 : OK															*/
/*		  -1 : error														*/
/*																			*/
/* ======================================================================== */
#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
int PHYS_IsCiPlusCam(unsigned short slot)
{
	int index;

	for (index = 0; index < PAL_MAX_CISLOT_NUMBER; index++)
	{
		if (DRVTab[index].Slot_id == slot)
		{
			if (DRVTab[index].State == DRV_CONNECT)
			{
				if (DRVTab[index].Ci_plus == 1)
				{
					return 1; // CI+ CAM detected
				}
				else
				{
					return 0; // CIv1 or CI+ CAM but doesn't meet CI+ spec
				}
			}
			else
			{
				PrintErr(("PHYS_IsCiPlusCam : wrong state (%d) !!!\n", DRVTab[index].State));
				return 0; // 캠이 꽂혀 있지 않다. 또는 캠이 꽂혀 있으나 이상 상태.
			}
		}
	}

	return 0; // 캠이 꽂혀 있지 않다.
}

int PHYS_CheckCIS(unsigned char *cis, int len, unsigned short *addrcor, unsigned char *cor, unsigned char *ciplus)
#else
int PHYS_CheckCIS(unsigned char *cis, int len, unsigned short *addrcor, unsigned char *cor)
#endif
{
unsigned char *pcis;	/* pointer to the remainding CIS	*/
int lcis;				/* length of the remainding CIS 	*/
int first;				/* 1 for the first configuration table entry, else 0 */
int addr;				/* configuration registers base address */
int dvb;				/* index number of the DVB entry	*/
int last;				/* index number of the last entry	*/
int iRet;				/* return code						*/

	dvb = 0;

	pcis = cis;
	lcis = len;

	PrintDbg(("PHYS_CheckCIS : CISTPL_DEVICE_0A\n"));
	if (PHYS_Tuple(0, CISTPL_DEVICE_0A, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintErr(("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0A\n"));
		return -1;		/* not found or wrong CISTPL_DEVICE_0A		*/
	}

//	pcis = cis;
//	lcis = len;

	PrintDbg(("PHYS_CheckCIS : CISTPL_DEVICE_0C\n"));
	if (PHYS_Tuple(0, CISTPL_DEVICE_0C, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintErr(("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0C\n"));
		return -1;		/* not found or wrong CISTPL_DEVICE_0C		*/
	}

//	pcis = cis;
//	lcis = len;

	PrintDbg(("PHYS_CheckCIS : CISTPL_VERS_1\n"));
	if ((iRet = PHYS_Tuple(0, CISTPL_VERS_1, &pcis, &lcis, &first, &addr, &dvb, &last)) < 0)
	{
		PrintErr(("PHYS_CheckCIS : not found or wrong CISTPL_VERS_1\n"));
		return -1;		/* not found or wrong CISTPL_VERS_1 		*/
	}

	#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	if (iRet == 2)
		*ciplus = 1;
	else
		*ciplus = 0;
	PrintDbg(("PHYS_CheckCIS : ciplus=%d\n", *ciplus));
	#endif

//	pcis = cis;
//	lcis = len;

	PrintDbg(("PHYS_CheckCIS : CISTPL_MANFID\n"));
	if (PHYS_Tuple(0, CISTPL_MANFID, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintErr(("PHYS_CheckCIS : not found or wrong CISTPL_MANFID\n"));
		return -1;		/* not found or wrong CISTPL_MANFID 		*/
	}

//	pcis = cis;
//	lcis = len;

	PrintDbg(("PHYS_CheckCIS : CISTPL_CONFIG\n"));
	if (PHYS_Tuple(0, CISTPL_CONFIG, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintErr(("PHYS_CheckCIS : not found or wrong CISTPL_CONFIG\n"));
		return -1;		/* not found or wrong CISTPL_CONFIG 		*/
	}

	*addrcor = addr;
	first = 1;
	do
	{
		PrintDbg(("PHYS_CheckCIS : CISTPL_CFTABLE_ENTRY\n"));
		iRet = PHYS_Tuple(0, CISTPL_CFTABLE_ENTRY, &pcis, &lcis, &first, &addr, &dvb, &last);
	}
	while ((iRet == 0) || (iRet == -2));		/* one more entry */
	if (iRet < 0)
	{
		PrintErr(("PHYS_CheckCIS : not found or wrong CISTPL_CFTABLE_ENTRY\n"));
		return -1;		/* not found or wrong CISTPL_CFTABLE_ENTRY	*/
	}

	PrintDbg(("PHYS_CheckCIS : addr=%x, dvb=%x, last=%x\n", addr, dvb, last));

	PrintDbg(("PHYS_CheckCIS : CISTPL_NO_LINK\n"));
	if (PHYS_Tuple(0, CISTPL_NO_LINK, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintErr(("PHYS_CheckCIS : not found or wrong CISTPL_NO_LINK\n"));
		return -1;		/* not found or wrong CISTPL_NO_LINK 		*/
	}

	PrintDbg(("PHYS_CheckCIS : CISTPL_END\n"));
	if (PHYS_Tuple(0, CISTPL_END, &pcis, &lcis, &first, &addr, &dvb, &last) < 0)
	{
		PrintErr(("PHYS_CheckCIS : not found or wrong CISTPL_END\n"));
		return -1;		/* not found or wrong CISTPL_END			*/
	}

	/* the index of the DVB entry is the COR value */

//	if (dvb != DVB_INDEX)
//	{
//		PrintErr(("PHYS_CheckCIS : DVB entry not found (dvb=%x, last=%x)\n", dvb, last));
//		return -1;
//	}

	if (dvb != last)
	{
		PrintErr(("PHYS_CheckCIS : DVB entry found but... (dvb=%x, last=%x)\n", dvb, last));
	}

	*cor = dvb;
//	*cor = last;

	return 1;			/* good CIS */
}


/* ======================================================================== */
/* NAME : PHYS_Tuple														*/
/*																			*/
/* DESCRIPTION : control the current CIS									*/
/*																			*/
/* INPUT PARAMETERS :														*/
/*		mtuple:main tuple if tuple is a subtuple or 0						*/
/*		tuple: tuple to control 											*/
/*		pcis : pointer to the tuple to control								*/
/*		len  : length of the remainding CIS with the current tuple			*/
/*		first: 1 for the first CISTPL_CFTABLE_ENTRY tuple, else 0			*/
/*		itpcc_radr : configuration registers base address					*/
/*		last : index number of the last entry								*/
/*																			*/
/* OUTPUT PARAMETERS :														*/
/*		pcis : pointer to the next tuple									*/
/*		len : length of the remainding CIS without the current tuple		*/
/*		radr: configuration registers base address (only for CISTPL_CONFIG) */
/*		first: 0 when an new entry is OK									*/
/*		itcc_radr : configuration registers base address					*/
/*		dvb : index number of the dvb entry 								*/
/*		last: index number of the last entry								*/
/*																			*/
/* RETURN CODE																*/
/*		   2 : OK (CI+ CAM detected)										*/
/*		   1 : OK															*/
/*		   0 : one configuration table entry ok ; not the last				*/
/*		  -1 : error														*/
/*		  -2 : wrong entry in the configuration Table						*/
/*																			*/
/* ======================================================================== */
int PHYS_Tuple(unsigned mtuple, unsigned char tuple, unsigned char **pcis, int *len, int *first, int *itpcc_radr, int *dvb, int *last)
{
	int ocis;						/* current offset in the CIS */
	int link;						/* tuple link				 */
	int found;						/* 1 if the current tuple is found, else 0 */
	unsigned char ctuple;			/* current tuple			 */
	int sublen; 					/* intermediate length		 */
	unsigned char *subpcis; 		/* pointer to subtuple		 */
	int nb, nbt0 = 0, nbt1 = 0, i;	/* intermediate variables	 */
	int ctpce_number = 0;			/* Configuration Index Value */
	char ctpcc_sz;					/* tuple's elementary item   */
	char lastentry; 				/* 1 if the last entry is found */
	char ctpcc_rasz;
	char ctpcc_rmsz;
	char cpce_pd;
#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	int ciplus = 0;
#endif

	ocis  = 0;
	found = 0;

	do
	{
//		if (tuple == CISTPL_END)
//		{
//			if ((**pcis == CISTPL_END) && (*len == 1))
//			{
//				*len = 0;			/* no more tuples		 */
//				return 0;			/* good CISTPL_END tuple */
//			}
//		}

		ctuple = **pcis;
		link   = *(*pcis+1);
		*pcis += 2;
		ocis  += 2;

		PrintDbg(("PHYS_Tuple : tuple=%02X, ctuple=%02X, link=%02X(%d), len=%d\n", tuple, ctuple, link, link, *len));

		if (tuple == ctuple)
		{
			found = 1;
			/* tuple found ; control it */

			switch (tuple) {
			case CISTPL_VERS_1 :
				if (*(*pcis + TPLLV1_MAJOR) != TPLLV1_MAJOR_VAL)	/* Control */
				{
					PrintErr(("PHYS_Tuple : not found or wrong TPLLV1_MAJOR\n"));
					return -1;	/* wrong TPLLV1_MAJOR */			/* ======= */
				}
				if (*(*pcis + TPLLV1_MINOR) != TPLLV1_MINOR_VAL)	/* Control */
				{
					PrintErr(("PHYS_Tuple : not found or wrong TPLLV1_MINOR\n"));
					return -1;	/* wrong TPLLV1_MINOR */			/* ======= */
				}
#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
				{
					/* check TPLLV1_INFO's "Additional Product Information" includes "ciplus=1" with case insensitive parsing */

					unsigned char info_str[256]; // maybe max tuple data length is 254
					int info_len;

					#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
					VK_Print("\n --- CISTPL_VERS_1 : link = %02X(%d) ---\n\n ", link, link);
					for (i = TPLLV1_INFO; i < link; i++)
					{
						if (*(*pcis + i) == CISTPL_NULL)
							VK_Print("\n ");
						else if (*(*pcis + i) == CISTPL_END)
							VK_Print("\n\n");
						else
							VK_Print("%c", *(*pcis + i));
					}
					#endif

					info_len = link - TPLLV1_INFO; // info_len 가 254 를 넘을 일이 없으므로 이에 대한 에러 핸들링은 할 필요가 없지...
					if (info_len > 0)
					{
						for (i = 0; i < info_len; i++)
						{
							if (*(*pcis + TPLLV1_INFO + i) == CISTPL_NULL)
								info_str[i] = ' ';
							else if (*(*pcis + TPLLV1_INFO + i) == CISTPL_END)
								info_str[i] = 0x00;
							else
								info_str[i] = *(*pcis + TPLLV1_INFO + i);
						}
						for (i = 0; i < info_len; i++)
						{
							info_str[i] = tolower(info_str[i]);
						}
						if (strstr(info_str, "ciplus=1"))
						{
							#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
							VK_Print("[CIS] CI+ CAM detected !!\n");
							#endif
							ciplus = 1;
						}
						else
						{
							PrintDbg(("[CIS] CIv1 or CI+ CAM but doesn't meet CI+ spec - not found ciplus=1\n"));
						}
					}
					else
					{
						PrintDbg(("[CIS] CIv1 or CI+ CAM but doesn't meet CI+ spec - not found or wrong TPLLV1_INFO\n"));
					}
				}
#endif // #ifdef CI_PLUS_ENABLE
				break;

			case CISTPL_CONFIG :
				ctpcc_sz = *(*pcis + TPCC_SZ);
				ctpcc_rasz = ctpcc_sz & TPCC_RASZ;		/* number of bytes of Configuration Register Base Address */
				if (ctpcc_rasz > NBRBYTES)				/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
				{
					PrintErr(("PHYS_Tuple : wrong Configuration Register Base Address (ctpcc_rasz = 0x%x)\n", ctpcc_rasz));
					return -1;							/* ======= wrong Configuration Register Base Address */
				}

				ctpcc_rmsz = ctpcc_sz & TPCC_RMSZ;
				*last  = *(*pcis + TPCC_LAST);			/* index number of last entry in Card configuration Table */
				if (ctpcc_rasz == 0)
					*itpcc_radr = *(*pcis + TPCC_RADR);
				else
					*itpcc_radr = *(*pcis + TPCC_RADR) + (*(*pcis + TPCC_RADR + 1) << 8);
				if (*itpcc_radr > MAXADDR)				/* Control */
				{
					PrintErr(("PHYS_Tuple : wrong Configuration Register Base Address (*itpcc_radr = 0x%x)\n", *itpcc_radr));
					return -1;							/* ======= wrong Configuration Register Base Address */
				}
				if (! ((*(*pcis + TPCC_RADR + ctpcc_rasz + 1)) & TPCC_RMSK))	/* Control */
				{
					PrintErr(("PHYS_Tuple : COR not present\n"));
					return -1;							/* COR not present		   ======= */
				}

				nb = TPCC_RADR + ctpcc_rasz + 1 + ctpcc_rmsz + 1;
				subpcis = *pcis + nb;
				sublen = *len - ocis - (link - nb);

				/* control CCST_CIF subtuple */
				PrintDbg(("PHYS_Tuple : CCST_CIF\n"));
				if (PHYS_Tuple(tuple, CCST_CIF, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{
					PrintErr(("PHYS_Tuple : not found or wrong CCST_CIF\n"));
					return -1;		/* Control not found or wrong CCST_CIF */
				}
				break;				/* =======							   */

			case CISTPL_CFTABLE_ENTRY :
				if (*first & !((*(*pcis + TPCE_INDX)) & TPCE_INDX_MASK))	/* Intface bit and Default bit must be present in the first entry */
				{
					PrintErr(("PHYS_Tuple : not found or wrong CISTPL_CFTABLE_ENTRY\n"));
					return -1;		/* Control wrong first CISTPL_CFTABLE_ENTRY tuple */
				}
									/* =======										  */
				ctpce_number = *(*pcis + TPCE_INDX) & ~TPCE_INDX_MASK;	/* configuration Entry Number to write to the COR */
//				if (ctpce_number == DVB_INDEX)
//					*dvb = DVB_INDEX;		/* the DVB entry is present */

				*first = 0;

				if (*(*pcis + TPCE_INDX) & TPCE_INTFACE)
				{
					if (*(*pcis + TPCE_IF) != TPCE_IF_VAL)				/* Control : TPCE_IF must be equal to TPCE_IF_VAL */
					{													/* =======										  */
						PrintDbg(("PHYS_Tuple : not found or wrong TPCE_IF_VAL\n")); // CISTPL_CFTABLE_ENTRY 가 2개 이상인 경우 통상 이 상황이 발생하므로 PrintDbg 로 프린트 한다.
						lastentry = (char)-2; 							/* wrong CISTPL_CFTABLE_ENTRY tuple */
						break;
					}
				}
				else
				{
					PrintErr(("PHYS_Tuple : not found or wrong TPCE_IF\n"));
					lastentry = (char)-2; 	/* Control : wrong CISTPL_CFTABLE_ENTRY tuple */
					break;					/* =======									  */
				}

				if (!(*(*pcis + TPCE_FS) & TPCE_FS_MASK))	/* Control : I/O Space and power must be present */
				{											/* =======										 */
					PrintErr(("PHYS_Tuple : not found or wrong TPCE_FS_MASK\n"));
					lastentry = (char)-2; 					/* wrong CISTPL_CFTABLE_ENTRY tuple */
					break;
				}

				nb = 0;

				if (*(*pcis + TPCE_FS) & TPCE_FS_POWER)
				{
					cpce_pd = *(*pcis + TPCE_PD);
					/* find the number of bytes coding the TPCE_PD (power)	*/
					for (i=0; i < 8; i++)
					{
						if (cpce_pd & TPCE_PD_MASK) /* one set bit means one following byte */
							nb++;
						cpce_pd = cpce_pd >> 1;
					}
					/* skip the TPCE_PD ; if the bit 7 of one power byte is set then a additional byte is present */
					for (i=TPCE_PD + 1; i <= TPCE_PD + nb; i++)
					{
						if (*(*pcis + i) & TPCE_EXT)
							nb++;
					}	/* end of power configuration skipping */
					nb++;		/* count the Power Description Structure Parameter Selection Byte */
				}

				if (*(*pcis + TPCE_FS) & TPCE_TIMING)
				{
					/* skip the timing configuration */
					if ((*(*pcis + TPCE_PD + nb) & TPCE_TIMING_WAIT) != TPCE_TIMING_WAIT)
					{
						nbt0 = 0;
						while (*(*pcis + TPCE_PD + nb + nbt0 + 1) & TPCE_EXT)
							nbt0++;
						nbt0++;
					}
					if ((*(*pcis + TPCE_PD + nb) & TPCE_TIMING_READY) != TPCE_TIMING_READY)
					{
						nbt1 = 0;
						while (*(*pcis + TPCE_PD + nb + nbt0 + nbt1 + 1) & TPCE_EXT)
							nbt1++;
						nbt1++;
					}
					nb = nb + nbt0 + nbt1 + 1;
				}	/* end of timing configuration skipping */

				/* skip the I/O Space */
				if (*(*pcis + TPCE_FS) & TPCE_IO)
				{
					if (*(*pcis + TPCE_PD + nb) & TPCE_IO_RANGE)
					{
						nbt0 = (*(*pcis + TPCE_PD + nb + 1) & TPCE_IO_SLN) >> TPCE_IO_SLNR;
						nbt1 = (*(*pcis + TPCE_PD + nb + 1) & TPCE_IO_SAN) >> TPCE_IO_SANR;
						nb = nb + nbt0 + nbt1 + 1;	/* + 1 to skip the I/O Range Descriptor */
					}
					nb++;	/* skip the I/O Space descriptor */
				}	/* end of I/O space configuration skipping */

				/* skip the IR Configuration */
				if (*(*pcis + TPCE_FS) & TPCE_IR)
				{
					if (*(*pcis + TPCE_PD + nb) & TPCE_IR_MASK)
						nb += 2;
					nb++;
				}	/* end of IR configuration skipping */

				/* skip the Mem space Configuration */
				if (*(*pcis + TPCE_FS) & TPCE_MEM)
				{
					nbt0 = (*(*pcis + TPCE_PD + nb) & TPCE_MEM_SL) >> TPCE_MEM_SLR;
					nbt1 = (*(*pcis + TPCE_PD + nb) & TPCE_MEM_SCA) >> TPCE_MEM_SCAR;
					if (*(*pcis + TPCE_PD + nb) & TPCE_MEM_HPA)
						nbt1 *= 2;
					nb = nb + nbt0 + nbt1 + 1;
				}	/* end of Mem space configuration skipping */

				/* Control the STCE_EV subtuple */
				nb = TPCE_PD + nb;
				subpcis = *pcis + nb;
				sublen = *len - ocis - nb;
				PrintDbg(("PHYS_Tuple : STCE_EV\n"));
				if (PHYS_Tuple(tuple, STCE_EV, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{						/* Control */
					lastentry = (char)-2; 	/* ======= not found or wrong STCE_EV */
					break;
				}

				/* Control the STCE_PD subtuple */
				PrintDbg(("PHYS_Tuple : STCE_PD\n"));
				if (PHYS_Tuple(tuple, STCE_PD, &subpcis, &sublen, first, itpcc_radr, dvb, last) < 0)
				{						/* Control */
					lastentry = (char)-2; 	/* ======= not found or wrong STCE_PD */
					break;
				}

//				if (ctpce_number == DVB_INDEX)
//					*dvb = DVB_INDEX;		/* the DVB entry is present */
				*dvb = ctpce_number;		/* the DVB entry is present */

//				if (ctpce_number == *last)
//				{
//					OutCh('&');
//					lastentry = 1;		/* end of one entry analysis, it's the last */
//				}
//				else
//				{
//					OutCh('*');
//					lastentry = 0;		/* not the last entry */
//				}
				break;

			case CISTPL_NO_LINK :
				if (link != CISTPL_NOLINKLEN)	/* Control */
				{
					PrintErr(("PHYS_Tuple : not found or wrong CISTPL_NOLINKLEN\n"));
					return -1;					/* ======= wrong CISTPL_NO_LINK tuple */
				}
				break;

			/* subtuples */
			case CCST_CIF : 		/* if mtuple = CISTPL_CFTABLE_ENTRY then this tuple is STCE_EV */
				if (mtuple == CISTPL_CFTABLE_ENTRY)
				{	/* STCE_EV */
					if (VK_strcmp((char*)*pcis, STCE_EV_VAL)) 	/* Control */
					{
						PrintErr(("PHYS_Tuple : not found or wrong STCE_EV_VAL\n"));
						return -1;						/* ======= wrong STCE_EV subtuple */
					}
				}
				else
				{	/* CCST_CIF */
//					if (link != CCST_CIFLEN)	/* Control */
//					{
//						PrintErr(("PHYS_Tuple : not found or wrong CCST_CIFLEN\n"));
//						return -1;				/* ======= wrong CCST_CIF subtuple */
//					}
					if (**pcis != CCST_CIF1)	/* Control */
					{
						PrintErr(("PHYS_Tuple : not found or wrong CCST_CIF1\n"));
						return -1;				/* ======= wrong CCST_CIF subtuple */
					}
					if (*(*pcis + 1) != CCST_CIF2)	/* Control */
					{
						PrintErr(("PHYS_Tuple : not found or wrong CCST_CIF2\n"));
						return -1;					/* ======= wrong CCST_CIF subtuple */
					}

					/* STCI_STR must be "DVB_CI_Vx.xx" where x is digit */
					nb = VK_strlen(STCI_STR);
					if (VK_strncmp((char*)(*pcis + 2), STCI_STR, nb))	/* Control */
					{
						PrintErr(("PHYS_Tuple : not found or wrong STCI_STR\n"));
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!isdigit(*(*pcis + 2 + nb)))		/* Control */
					{
						PrintErr(("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__));
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (*(*pcis + 2 + nb + 1) != STCI_STRP) /* Control */
					{
						PrintErr(("PHYS_Tuple : not found or wrong STCI_STRP\n"));
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!isdigit(*(*pcis + 2 + nb + 2)))	/* Control */
					{
						PrintErr(("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__));
						return -1;							/* ======= wrong CCST_CIF subtuple */
					}
					if (!isdigit(*(*pcis + 2 + nb + 3)))	/* Control */
					{
						PrintErr(("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__));
						return -1;							/* ====== wrong CCST_CIF subtuple */
					}
				}
				break;

			case STCE_PD :
				if (VK_strcmp((char*)(*pcis), STCE_PD_VAL)) /* Control */
				{
					PrintErr(("PHYS_Tuple : not found or wrong STCE_PD_VAL\n"));
					return -1;					/* ======= wrong STCE_PD subtuple */
				}
				break;

			/* end-of-list tuple */
			case CISTPL_END :
				return 0;			/* good CISTPL_END tuple */

			default :
				/* other tuples : no control */
				break;
			}
		}

		/* skip the current tuple */
		*pcis += link;
		ocis += link;
		*len -= link;
		if (*len <= 0)	/* Control */
		{
			PrintErr(("PHYS_Tuple : tuple not found (ocis=%d, len=%d)\n", ocis, *len));
			return -1;		/* ======= tuple not found */
		}
		else
		{
			PrintDbg(("PHYS_Tuple : next tuple=%02X following %d bytes from ctuple=%02X\n", **pcis, ocis, ctuple));
		}
	}
	while (!found);

	if (tuple == CISTPL_CFTABLE_ENTRY)
	{
		if (ctpce_number == *last || **pcis == CISTPL_NO_LINK) // if (ctpce_number == *last)
		{
			lastentry = 1;		/* end of one entry analysis, it's the last */
		}
		else
		{
			lastentry = 0;		/* not the last entry */
		}
		return lastentry;
	}
#if 1 /* #ifdef CI_PLUS_ENABLE */ /* CI+ 지원 모델이 아니더라도 CI+ 캠이 삽입될 수 있고 이에 대한 핸들링이 필요할 수도 있으므로 범용적으로 정의/처리해 둔다 (스펙에도 부합됨) */
	else if (tuple == CISTPL_VERS_1)
	{
		if (ciplus == 1)
			return 2;
		else
			return 1;
	}
#endif
	else
		return 1;
}

#if !defined(CONFIG_USE_EXTERNAL_CI_CONTROLLER)
/* ======================================================================== */
/* NAME : PHYS_GetAddrCcrs													*/
/*																			*/
/* DESCRIPTION : Get Address CCRS in CIS									*/
/*																			*/
/* INPUT PARAMETERS :														*/
/*		fd	:																*/
/*																			*/
/* OUTPUT PARAMETERS :														*/
/*		addrccrs : CCRs address 											*/
/*																			*/
/* RETURN CODE																*/
/*		   0 : OK															*/
/*		  -1 : error														*/
/*																			*/
/* ======================================================================== */
#ifndef USE_DOUGLAS_PRIMITIVE_FUNC
int PHYS_GetAddrCcrs(int fd, unsigned short *addrCcrs)
{
	int i;
	int iRet;				/* return code						*/
	DRV_stMem mem;					/* driver parameters */
	unsigned char cis[8];
	unsigned char radrSize;

	mem.len    = 8;
	mem.pbytes = cis;

	for(i = DRV_MEMADDR; i < MAXADDR; )
	{
		mem.addr = i;
		iRet = DRV_CI_Ioctl(fd, DRV_READMEM, &mem);
		if (iRet < 0)
			return -1;		/* error while reading CIS */
		if(cis[0] == CISTPL_CONFIG)
		{
			radrSize = cis[2] & TPCC_RASZ;			/* number of bytes of Configuration Register Base Address */
			if (radrSize > NBRBYTES)				/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
			{
//				OutCh('!');
				return -1;							/* ======= wrong Configuration Register Base Address */
			}
			if (radrSize == 0)
				*addrCcrs = cis[4];
			else
				*addrCcrs = cis[4] | (cis[5] << 8);
			if (*addrCcrs > MAXADDR)				/* Control */
			{
//				PrintDbg(("PHYS_GetAddrCcrs : addr=%x, max=%x\n", *addrCcrs, MAXADDR));
//				OutCh('@');
				return -1;							/* ======= wrong Configuration Register Base Address */
			}
			return 0;
		}
		else
		{
			if(cis[0] == 0xff || !cis[1] || cis[1] == 0xff)
				return -1;
			i += cis[1] + 2;
		}

	}
	return -1;
}
#endif
#endif

#ifdef	USE_CI_FLASH_SEM
void	CI_FLASH_Access_SEM_Get(void)
{
	int	loop_cnt=0;

	while(humax_ci_access_state==CI_ACCESS_ON && loop_cnt++<(ITC_TIMEOUT+20))
	{
		VK_TASK_Sleep(1);
	}
	VK_SEM_Get(s_hCiAccess);
}

void	CI_FLASH_Access_SEM_Release(void)
{
	VK_SEM_Release(s_hCiAccess);
}
#endif

