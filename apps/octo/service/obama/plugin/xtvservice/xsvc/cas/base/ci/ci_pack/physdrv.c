
/*************************/
/* renewal by hmkim, 2010-03 */
/*************************/

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
#include "itc.h"                    /* Inter-Task Communication */
#include "pdriver.h"                /* low level PHYSICAL driver Access */
#include "physdrv.h"
#include "ci.h"

/* parkbi modify */
#include "ctype.h"
//#include "string.h"
//#include "vkernel.h"

#include "ci_reshandler.h"			/* for CIRH_GetTcId() */
#include "trans.h"					/* for TRPT_GetTcid() and TRPT_Set_Timer() */

#include <_svc_cas_sub_dev_api.h>

/*--------------------------------------------------------------------------*/
/* Local Definitions ////////////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/



/*
	R/W robust code enabled flag :

	0 : don't use --> performance �� �߿�� �ȴٸ� �̰� ��� (����Ʈ)
	1 : use for write operation only --> write operation �ÿ��� robust �ڵ� ���� (CI+ �� ��� ū �������� MHEG �����͸� read �ϰ� �ǹǷ� read operation �ÿ��� �ƿ� �������� �ʵ��� �ϴ� ����)
	2 : use for both of read and write operations
*/
#define PHYS_RW_ROBUST_CODE_ENABLED_FLAG	(0)

/*
	R/W timing margin :

	LG �� �ڱ�� CI ����¡���� ���� ������ �����ߴٴ� �ַ��. ���� �̷� margin �� �ʿ������� �ټ� �ǹ������� �ϴ� ������ �д�.

	���� ���� :

	1. PHYS_ResetModule() �� buffer nego �� ���� R/W ������ ����ؼ��� �ȵȴ�. ��Ȥ ķ ���Խ� ķ�� �������� �ʴ� ��찡 �߻��Ѵ�.
	2. CI+ �� ��� ū �������� MHEG �����͸� read �ϰ� �ǹǷ� read operation �ÿ��� ����ؼ��� �ȵȴ�.

	�̿� ���� ������ timing margin �� ��ǻ� �Ϲ� write operation �ÿ��� �����.
*/
#if (PHYS_RW_ROBUST_CODE_ENABLED_FLAG)
#define PHYS_DelayMargin	{ if (DRVTab[index].State == DRV_CONNECT) ITC_Sleep(PHYS_DELAY); }
#else
#define PHYS_DelayMargin
#endif

/*--------------------------------------------------------------------------*/
/* Local Variables /////////////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

/* I/O register values */
static unsigned char Command_Register = 0;
static unsigned char Status_Register  = 0;
static unsigned char Size_LS_Register = 0;
static unsigned char Size_MS_Register = 0;

static DRVPhys DRVTab[CARM_MAX_CISLOT_NUMBER];	/* slot informations table */
static short DRVNextIndex;					/* index of next polled slot */
static unsigned char *pDRVData;				/* intermediate buffer for read/send collision */

/*--------------------------------------------------------------------------*/
/* Work-around //////////////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

// hmkim, 2009-03-20 : work-around for ���۶� CI ����̹� �̽� (H/W ECO �ȵ� Douglas �÷���)
// ȣ��Ʈ���� ķ���� �����͸� ���� �Ŀ� Tsb ������ �� �� ��Ȥ �� ������ �����Ͱ� �߸��Ǿ� �ִ� ������ �ڵ鸵�ϱ� ���� �߰��� (��, length �� �ùٸ��� �´�)
// ķ���� ���� Tsb ������ ��Ȯ�ϳ� BSP ���� �߸��� �����Ͱ� �ö���� ���� �̸� CI ���ÿ��� �߸� ó���ϰ� �����ν� ���� �������� �߱�� -> [CI_RW_ERROR] �α� ��Ȳ �߻� !!
// ����� ���� �ڵ鸵�� ���� ���۶� �÷����� �ƴ϶� �Ϲ������� �� ó���� �־��ִ� ���� ���� ������ ������.
// �ؿ� �ִ� ������ svc_cas_DevCiWrite(), svc_cas_DevCiRead() �����ؼ��� ���ǰ� �� �Լ����� ���� ȣ���� �ȵǹǷ� ���� ��������� ��ȣ�� �ʿ�� ����.
#define TSB_ERROR_HANDLING
#ifdef TSB_ERROR_HANDLING
static unsigned char uc_Expected_Tsb[6] = {0x01, 0x00, 0x80, 0x02, 0x01, 0x00};
#endif

// hmkim, 2010-03 : work-around for TRPT_Timeout()
HUINT8 g_aucCamReplied[CARM_MAX_CISLOT_NUMBER]; // to check reply for Polling or Trcv.

/*--------------------------------------------------------------------------*/
/* For Debugging /////////////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

#ifdef CONFIG_DEBUG

int g_PalCi_PhysDrv_Debug_Flag = 0; // turn on/off R/W monitor

static char _tpdu_type[30];
static char * get_tpdu_type (unsigned char *data, unsigned short len)
{
	if (len >= 12)
	{
		snprintf(_tpdu_type, sizeof(_tpdu_type), "%02x.%02x.%02x|%02x|%02x.%02x.%02x",
			data[0],	// tcid (0x01/0x02)
			data[1],	// more (0x80) / last (0x00)
			data[2],	// tpdu tag (0x80~0x88, 0xa0/0xa1) if tpdu is not chained
			data[5],	// spdu tag (0x90~0x96) if tpdu is not chained and tpdu length field is one byte
			data[9],	// apdu tag (0x9f) if tpdu is not chained and tpdu length field is one byte and spdu tag is 0x90 -> for apdu tag, see "ci_res.h"
			data[10],	// apdu tag ...
			data[11]);	// apdu tag ...

		return _tpdu_type;
	}

	if (len >= 6)
	{
		snprintf(_tpdu_type, sizeof(_tpdu_type), "%02x.%02x.%02x|%02x",
			data[0],	// tcid (0x01/0x02)
			data[1],	// more (0x80) / last (0x00)
			data[2],	// tpdu tag (0x80~0x88, 0xa0/0xa1) if tpdu is not chained
			data[5]);	// spdu tag (0x90~0x96) if tpdu is not chained and tpdu length field is one byte

		return _tpdu_type;
	}

	if (len >= 3) // ȣ��Ʈ���� ������ ��� TPDU �� �ּ� 5 ����Ʈ �̻��̳�...
	{
		snprintf(_tpdu_type, sizeof(_tpdu_type), "%02x.%02x.%02x",
			data[0],	// tcid (0x01/0x02)
			data[1],	// more (0x80) / last (0x00)
			data[2]);	// tpdu tag (0x80~0x88, 0xa0/0xa1) if tpdu is not chained

		return _tpdu_type;
	}

	if (len >= 2) // �̰� TPDU �� �ƴ϶� ķ �ʱ�ȭ���� ���� ������ nego �� ���� 2 ����Ʈ...
	{
		snprintf(_tpdu_type, sizeof(_tpdu_type), "%02x.%02x",
			data[0],
			data[1]);
	}

	return NULL;
}
static int Is_ca_pmt (unsigned char *data, unsigned short len)
{
	unsigned char ca_pmt[] = {0x9f, 0x80, 0x32};

	/* CI_CA_PMT : for example,
		01 00 a0 25 01 90 02 00 03 9f 80 32 1c 03 03 2c 07 00 07 01 09 04 06 04 f3 8c 02 0d ad 00 00 04 0d b6 00 00 04 0d b7 00 00 */

	if (len >= 12)
	{
		if (data[0] == 0x01 && HxSTD_memcmp(&data[9], ca_pmt, 3) == 0)
			return 1;	// ca_pmt for tcid 1 cam
		if (data[0] == 0x02 && HxSTD_memcmp(&data[9], ca_pmt, 3) == 0)
			return 2;	// ca_pmt for tcid 2 cam
	}

	return 0;
}
static int Is_date_time_reply (unsigned char *data, unsigned short len)
{
	unsigned char date_time_reply[] = {0x9f, 0x84, 0x41};

	/* CI_DATE_TIME_REPLY : for example,
		01 00 a0 0e 01 90 02 00 04 9f 84 41 05 xx xx xx xx xx,
		01 00 a0 10 01 90 02 00 04 9f 84 41 07 xx xx xx xx xx yy yy */

	if (len == 18 || len == 20)
	{
		if (data[0] == 0x01 && HxSTD_memcmp(&data[9], date_time_reply, 3) == 0)
			return 1;	// date/time for tcid 1 cam
		if (data[0] == 0x02 && HxSTD_memcmp(&data[9], date_time_reply, 3) == 0)
			return 2;	// date/time for tcid 2 cam
	}

	return 0;
}
static int Is_Poll (unsigned char *data, unsigned short len)
{
	unsigned char Tdata_last_with_no_data_1[] = {0x01, 0x00, 0xa0, 0x01, 0x01};
	unsigned char Tdata_last_with_no_data_2[] = {0x02, 0x00, 0xa0, 0x01, 0x02};

	if (len == 5)
	{
		if (HxSTD_memcmp(data, Tdata_last_with_no_data_1, 5) == 0)
			return 1; // poll for tcid 1 cam
		if (HxSTD_memcmp(data, Tdata_last_with_no_data_2, 5) == 0)
			return 2; // poll for tcid 2 cam
	}

	return 0;
}
static int Is_Trcv (unsigned char *data, unsigned short len)
{
	unsigned char Trcv_1[] = {0x01, 0x00, 0x81, 0x01, 0x01};
	unsigned char Trcv_2[] = {0x02, 0x00, 0x81, 0x01, 0x02};

	if (len == 5)
	{
		if (HxSTD_memcmp(data, Trcv_1, 5) == 0)
			return 1; // receive data command for tcid 1 cam

		if (HxSTD_memcmp(data, Trcv_2, 5) == 0)
			return 2; // receive data command for tcid 2 cam
	}

	return 0;
}
static int Is_Tsb_with_no_da (unsigned char *data, unsigned short len)
{
	unsigned char Tsb_with_no_da_1[] = {0x01, 0x00, 0x80, 0x02, 0x01, 0x00};
	unsigned char Tsb_with_no_da_2[] = {0x02, 0x00, 0x80, 0x02, 0x02, 0x00};

	if (len == 6)
	{
		if (HxSTD_memcmp(data, Tsb_with_no_da_1, 6) == 0)
				return 1;	// Tsb with no data available from tcid 1 cam

		if (HxSTD_memcmp(data, Tsb_with_no_da_2, 6) == 0)
				return 2;	// Tsb with no data available from tcid 2 cam
	}

	return 0;
}
static int Is_Tsb_with_da (unsigned char *data, unsigned short len)
{
	unsigned char Tsb_with_da_1[] = {0x01, 0x00, 0x80, 0x02, 0x01, 0x80};
	unsigned char Tsb_with_da_2[] = {0x02, 0x00, 0x80, 0x02, 0x02, 0x80};

	if (len == 6)
	{
		if (HxSTD_memcmp(data, Tsb_with_da_1, 6) == 0)
				return 1;	// Tsb with data available from tcid 1 cam

		if (HxSTD_memcmp(data, Tsb_with_da_2, 6) == 0)
				return 2;	// Tsb with data available from tcid 2 cam
	}

	return 0;
}

#else
static char * get_tpdu_type (unsigned char *data, unsigned short len)
{
	return NULL;
}
static int Is_ca_pmt (unsigned char *data, unsigned short len)
{
	return 0;
}
static int Is_date_time_reply (unsigned char *data, unsigned short len)
{
	return 0;
}
static int Is_Poll (unsigned char *data, unsigned short len)
{
	return 0;
}
static int Is_Trcv (unsigned char *data, unsigned short len)
{
	return 0;
}
static int Is_Tsb_with_no_da (unsigned char *data, unsigned short len)
{
	return 0;
}
static int Is_Tsb_with_da (unsigned char *data, unsigned short len)
{
	return 0;
}

#endif

/*--------------------------------------------------------------------------*/
/*  Local Function Prototypes ////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

/*--- Internal routines accessing low level PHYSICAL Driver ---*/

static unsigned char PHYS_InitModule(int index, unsigned int Host_Size, unsigned short *Negociate_Size);
static unsigned char PHYS_Card(int index);
static unsigned char PHYS_CD(int index);
static unsigned char PHYS_CIS(int index);
static unsigned char PHYS_WriteCommand(int index, unsigned char data, int flag);
static unsigned char PHYS_ReadStatus(int index, int flag);
static unsigned short PHYS_WriteSize(int index, unsigned short size);
static unsigned short PHYS_ReadSize(int index);
static unsigned char PHYS_ResetModule(int index, unsigned int Host_Size, unsigned short *Negociate_Size);
static unsigned char PHYS_SendData(int index, unsigned char *pData, unsigned short Size_in /*, int flag */);
static void PHYS_ReceiveIdle(int index);
static unsigned char PHYS_ReceiveData(int index, unsigned char *pData, unsigned short *Size_out, int flag);
static int PHYS_CheckCIS(unsigned char *cis, int len, unsigned short *addrcor, unsigned char *cor, unsigned int *ciplus, unsigned int *ciprof);
static int PHYS_Tuple(unsigned mtuple, unsigned char tuple, unsigned char **pcis, int *len, int *first, int *itpcc_radr, int *dvb, int *last, unsigned int *ciplus, unsigned int *ciprof);
#ifndef USE_DOUGLAS_PRIMITIVE_FUNC
static int PHYS_GetAddrCcrs(int fd, unsigned short *addrCcrs);
#endif

/*--------------------------------------------------------------------------*/
/*  Public Functions ////////////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

/*--- High Level API ---*/

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
	int i, slot;

	for (i = 0; i < svc_cas_DevCiGetNumberOfModule(); i++)
	{
		/* Initialize the slot table */
		DRVTab[i].State = DRV_IDLE;
		DRVTab[i].firstSLPDU = NULL;

		/* Open the low level PHYSICAL Driver */
		slot = svc_cas_DevCiOpen((unsigned char *)svc_cas_DevCiGetDeviceName());
		if (slot < 0)
		{
			HxLOG_Error("PHYS_DRVOpen : wrong slot %d !!!\n", slot);
			DRVTab[i].Slot_id = CARM_INVALID_CI_SLOT_ID;
			return -1;
		}

		DRVTab[i].Slot_id = (unsigned short)slot;
		DRVTab[i].Ci_plus = 0;
		DRVTab[i].Ci_prof = 0;

		HxLOG_Print("PHYS_DRVOpen : Open PHYSICAL low level driver for slot %d\n", DRVTab[i].Slot_id);
	}

	DRVNextIndex = 0;

	if (i > 0)
	{
		/* Allocate intermediate buffer for read/send collision */
		pDRVData = ITC_DynAlloc(MAXLNSIZE);
		if (pDRVData == NULL)
		{
			HxLOG_Error("PHYS_DRVOpen : not enough memory (%d) !!!\n", MAXLNSIZE);
			return -1;
		}

		HxSTD_memset(pDRVData, 0, MAXLNSIZE);
	}

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
	unsigned char Ret;			/* return code */
	int index;					/* DRVTab index for current slot */
	int count;					/* timeout management */
	int retry;					/* error management */

	for (index = 0; index < svc_cas_DevCiGetNumberOfModule(); index++)
	{
		if (DRVTab[index].Slot_id == slot)
			break;
	}

	if (index >= svc_cas_DevCiGetNumberOfModule())
		return -1; /* slot_id not found */

	#if 0 // �̷� ������ �߻����� ����.
	if (DRVTab[index].firstSLPDU)
	{
		SLPDU *p;
		SLPDU *free_p;

		HxLOG_Error("PHYS_DRVWrite : DRVTab[%d].firstSLPDU is NOT NULL (error) !!!\n", index);

		/* free the linked list */
		p = DRVTab[index].firstSLPDU;
		do
		{
			free_p = p;
			p = p->next;
			ITC_DynFree(free_p->buffer);
			ITC_DynFree((unsigned char *)free_p);
		} while (p);
		DRVTab[index].firstSLPDU = NULL;
	}
	#endif

	#ifdef CONFIG_DEBUG
	if (g_PalCi_PhysDrv_Debug_Flag == 1)
	{
		int answer;
		int x;
		if ((answer = Is_ca_pmt(pbuff, len)))
			HxLOG_Print("[CAPMT%d(%d)]", answer, len);
		else if ((answer = Is_date_time_reply(pbuff, len)))
			HxLOG_Print("[DT%d]", answer);
		else if ((answer = Is_Poll(pbuff, len)))
			HxLOG_Print("[?%d]", answer);
		else if ((answer = Is_Trcv(pbuff, len)))
			HxLOG_Print("[RCV%d]", answer);
		else
		{
			HxLOG_Print("[W(%d):", len);
			for (x = 0; x < len; x++)
				HxLOG_Print(" %02x", pbuff[x]);
			HxLOG_Print("]");
		}
	}
	else if (g_PalCi_PhysDrv_Debug_Flag == 2)
	{
		int answer;
		int x;
		if ((answer = Is_ca_pmt(pbuff, len)))
			HxLOG_Print("[CAPMT%d(%d):", answer, len);
		else if ((answer = Is_date_time_reply(pbuff, len)))
			HxLOG_Print("[DT%d:", answer);
		else if ((answer = Is_Poll(pbuff, len)))
			HxLOG_Print("[?%d:", answer);
		else if ((answer = Is_Trcv(pbuff, len)))
			HxLOG_Print("[RCV%d:", answer);
		else
		{
			HxLOG_Print("[W(%d):", len);
		}
		for (x = 0; x < len-1; x++)
			HxLOG_Print("0x%02x,", pbuff[x]);
		HxLOG_Print("0x%02x]", pbuff[x]);
	}
	#endif

	g_aucCamReplied[slot] = 0;

	count = 0;
	retry = 0;

	while (1)
	{
		Ret = PHYS_SendData(index, pbuff, len /*, 1 */);
		if (Ret == M_BUSY)
		{
			if (count == PHYS_TIMEOUT)
			{
				HxLOG_Error("[CI_RW_ERROR] PHYS_DRVWrite : M_BUSY !!!\n");
				return -1;	/* not sent */
			}

			ITC_Sleep(PHYS_DELAY);
			count++;

			continue; // retry
		}

		if (Ret == T_ERR)
		{
			if (retry == 1)
			{
				HxLOG_Error("[CI_RW_ERROR] PHYS_DRVWrite : T_ERR !!!\n");
				return -1;	/* not sent */
			}

			ITC_Sleep(PHYS_DELAY);
			retry++;

			continue; // retry
		}

		break; // exit while
	}

	if (Ret == OK)
	{
		#if 0 // #ifdef _CI_DEBUG_
		char buf_tpdu[1000];
		int j;
		for (j=0;j<(int)len;j++)
			sprintf((char *)(buf_tpdu+(3*j)),"%02x ",*(pbuff+j));
		buf_tpdu[(3*len)-1] = '\0';
		HxLOG_Print("PHYS_DRVWrite : %d bytes sent to the module %d : %s\n", len, slot, buf_tpdu));
		#endif

		return 0;
	}
	else
	{
		HxLOG_Error("[CI_RW_ERROR] PHYS_DRVWrite : NOT OK (%d) !!!\n", Ret);
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
	int iRet;		/* return code */
	DRV infos;		/* received event and associated data */
	int Ret;		/* PHYSICAL Driver return code */
	int offset; 	/* intermediate length */
	int index;		/* DRVTab[] index */
	int nbpoll; 	/* number of polling */
	unsigned short drvlen;	/* length of received data */

	iRet = 0;
	index = DRVNextIndex;
	nbpoll = 0;
//	offset = sizeof(ph_data) - 1 + sizeof(st_drvheader);		/* header size until ph_data structure end */
	offset = sizeof(unsigned short) * 2 + sizeof(st_drvheader);	/* header size until ph_data structure end */
//	HxLOG_Print("PHYS_DRVRead : sizeof(ph_data)=%d, sizeof(DRV)=%d, offset=%d, offset2=%d\n", sizeof(ph_data), sizeof(DRV), offset, offset2));

	do
	{
		if (DRVTab[index].State == DRV_CONNECT)
		{	/* wait for received data from inserted Module or wait for Module Extraction */
			/* ========================================================================= */
			/* the calling buffer is re-used for optimization . the PH_DATA message fixed size header is added after the reception */
			drvlen = 0;
			Ret = PHYS_ReceiveData(index, pbuff + offset, &drvlen, 1);

			#ifdef CONFIG_DEBUG
			if (g_PalCi_PhysDrv_Debug_Flag == 1)
			{
				unsigned char *tmp = pbuff + offset;
				int answer;
				int x;
				if (Ret != OK)
					;
				else
				{
					if ((answer = Is_Tsb_with_no_da(tmp, drvlen)))
				{
					HxLOG_Print("[.%d]\n", answer);
				}
				else if ((answer = Is_Tsb_with_da(tmp, drvlen)))
				{
					HxLOG_Print("[!%d]\n", answer);
				}
				else
				{
					HxLOG_Print("[R(%d,%d):", Ret, drvlen);
					for (x = 0; x < drvlen; x++)
						HxLOG_Print(" %02x", tmp[x]);
					HxLOG_Print("]\n");
				}
			}
			}
			else if (g_PalCi_PhysDrv_Debug_Flag == 2)
			{
				unsigned char *tmp = pbuff + offset;
				int answer;
				int x;
				if (Ret != OK)
					;
				else
				{
					if ((answer = Is_Tsb_with_no_da(tmp, drvlen)))
					{
						HxLOG_Print(" --> [.%d:", answer);
					}
					else if ((answer = Is_Tsb_with_da(tmp, drvlen)))
					{
						HxLOG_Print(" --> [!%d:", answer);
					}
					else
					{
						HxLOG_Print(" --> [R(%d,%d):", Ret, drvlen);
					}
					for (x = 0; x < drvlen-1; x++)
						HxLOG_Print("0x%02x,", tmp[x]);
					HxLOG_Print("0x%02x]\n", tmp[x]);
				}
			}
			#endif

			switch (Ret)
			{
			case OK :
				/* signal the PH_DATA event to the PHYSICAL Task */
				/* fill the received buffer with PH_DATA */
				infos.header.type = PH_DATA;
//				infos.header.len = sizeof(ph_data) - 1 + drvlen;
				infos.header.len  = sizeof(unsigned short) * 2 + drvlen;
				infos.body.data.slot_id = DRVTab[index].Slot_id;
				infos.body.data.len     = drvlen;

				/* header adding */
				*len = infos.header.len + sizeof(st_drvheader);
				HxSTD_memcpy(pbuff, (char *)&infos, offset);

				#if 0 // #ifdef _CI_DEBUG_
				{
					char buf_tpdu[1000];
					int j;

					/* display the received buffer in ASCII */
					for (j=0;j<(int)drvlen;j++)
						sprintf((char *)(buf_tpdu+(3*j)),"%02x ",*(pbuff+offset+j));
					buf_tpdu[(3*drvlen)-1] = '\0';
					if (drvlen > 50)
						buf_tpdu[200] = '\0';
					HxLOG_Print("PHYS_DRVRead : %d received bytes from the module %d : %s\n", drvlen, DRVTab[index].Slot_id, buf_tpdu));
				}
				#endif

				g_aucCamReplied[DRVTab[index].Slot_id] = PH_DATA;
				iRet = PH_DATA;
				break;

			case T_ERR :
				/* signal the PH_ERROR event to the PHYSICAL Task */
				/* fill the received buffer with PH_ERROR */
				infos.header.type = PH_ERROR;
				infos.header.len  = sizeof(ph_error);
				infos.body.errhw.slot_id = DRVTab[index].Slot_id;
				infos.body.errhw.status  = LN_PHYSICAL;

				*len = infos.header.len + sizeof(st_drvheader);
				HxSTD_memcpy(pbuff, (char *)&infos, *len);

				g_aucCamReplied[DRVTab[index].Slot_id] = PH_ERROR;
				iRet = PH_ERROR;
				break;

#ifdef CONFIG_MW_CI_PLUS
			case R_IIR :
				HxLOG_Print("<R_IIR>\n"); // �߿� �α��̹Ƿ� �׽� ��� ����.
#endif
			case W_SEACH :
				/* Set new state */
				DRVTab[index].State = DRV_DISCONNECT;

				/* signal the PH_EXTRACT event to the PHYSICAL Task */
				/* fill the received buffer with PH_EXTRACT */
				infos.header.type = PH_EXTRACT;
				infos.header.len  = sizeof(ph_extract);
				infos.body.extract.slot_id = DRVTab[index].Slot_id;

				*len = infos.header.len + sizeof(st_drvheader);
				HxSTD_memcpy(pbuff, (char *)&infos, *len);

				g_aucCamReplied[DRVTab[index].Slot_id] = PH_EXTRACT;
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
			if (Ret != W_SEACH && Ret != OK)
			{
				HxLOG_Error("PHYS_DRVRead : PHYS_InitModule return %d !!!\n", Ret);
			}

			switch (Ret)
			{
			case OK :	// 0x00
				/* signal the PH_INSERT event to the PHYSICAL Task */
				/* fill the received buffer with PH_INSERT */
				infos.header.type = PH_INSERT;
				infos.header.len  = sizeof(ph_insert);
				infos.body.insert.slot_id = DRVTab[index].Slot_id;
				infos.body.insert.status  = LN_OK;
				infos.body.insert.size	  = DRVTab[index].Negociated_size;

				*len = infos.header.len + sizeof(st_drvheader);
				HxSTD_memcpy(pbuff, (char *)&infos, *len);

				/* Set new state */
				DRVTab[index].State = DRV_CONNECT;

				iRet = PH_INSERT;
#if 1
				// IRHD-5100S, HD-FOX+ (BCM-7325 ���) ���� ���� ����.
				// ���꿡�� ����ϴ� Irdeto ķ(Rev1.1)�� ��� ķ �ʱ�ȭ ���Ŀ� profile �� �ȿö���� ������ ����.
				// ���� �����̸� �ָ� �ϴ� �ذ��� �ǳ� �߰� ���� �ʿ�.
				VK_TASK_Sleep(50); // ITC_TIMEOUT
#endif
				break;

			case KO :	// 0x07
				/* signal the wrong PH_INSERT event to the PHYSICAL Task */
				/* fill the received buffer with PH_INSERT */
				infos.header.type = PH_INSERT;
				infos.header.len  = sizeof(ph_insert);
				infos.body.insert.slot_id = DRVTab[index].Slot_id;
				infos.body.insert.status  = LN_PHYSICAL;	/* wrong CIS */
				infos.body.insert.size	  = DRVTab[index].Negociated_size;

				*len = infos.header.len + sizeof(st_drvheader);
				HxSTD_memcpy(pbuff, (char *)&infos, *len);

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
			index++; /* try the next index if nothing receveid */
			if (index == svc_cas_DevCiGetNumberOfModule())
				index = 0;
		}
	} while ((iRet == 0) && (nbpoll < svc_cas_DevCiGetNumberOfModule()));

	DRVNextIndex++; /* index of the next PHYS_DRVRead */
	if (DRVNextIndex == svc_cas_DevCiGetNumberOfModule())
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
	SLPDU *p;
	SLPDU *free_p;

	if (pDRVData)
	{
		ITC_DynFree(pDRVData);
		pDRVData = NULL;
	}

	for (i = 0; i < svc_cas_DevCiGetNumberOfModule(); i++)
	{
		/* Close the low level PHYSICAL Driver for a slot */
		if (DRVTab[i].Slot_id != CARM_INVALID_CI_SLOT_ID)
		{
			svc_cas_DevCiClose(DRVTab[i].Slot_id);
			DRVTab[i].Slot_id = CARM_INVALID_CI_SLOT_ID;
		}

		if (DRVTab[i].firstSLPDU)
		{
			/* free the linked list */
			p = DRVTab[i].firstSLPDU;
			do
			{
				free_p = p;
				p = p->next;
				ITC_DynFree(free_p->buffer);
				ITC_DynFree((unsigned char *)free_p);
			} while (p);
			DRVTab[i].firstSLPDU = NULL;
		}
	}

	return 0;
}

/*--------------------------------------------------------------------------*/
/*  Local Functions ////////////////////////////////////////////////////////////*/
/*--------------------------------------------------------------------------*/

/*--- Low Level management routines and low level driver access ---*/

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
static unsigned char PHYS_InitModule(int index, unsigned int Host_Size, unsigned short *Negociate_Size)
{
	int Ret;

	DRVTab[index].State = DRV_DISCONNECT;

	/* Poll the Card Detect and wait for complete module initialization */

	Ret = PHYS_Card(index);
	if (Ret != OK)
	{
		DRVTab[index].State = DRV_IDLE;
		return W_SEACH;	/* no card detect */
	}

	HxLOG_Print("PHYS_InitModule : the module %d is inserted\n", DRVTab[index].Slot_id);

	/* Check CIS and write COR */

	// hmkim :
	// ���۷��� ����̹� ������ ���� �� PHYS_Card()���� ķ ready�� ��Ȯ�� üũ���� �����Ƿ� (ķ ready üũ�� ķ detection üũ�� ������. ��, ķ detection �� 2�� üũ�ϴ� ��)
	// CIS üũ ���� sleep Ÿ���� �ø�.
	// �׷��� ���� ��� DRV_CI_ReadCIS()�� ������ CIS�� �߸��� ��찡 ���� �߻���.
	// ķ�� ���� ready ������ �ùٸ��� ���� ��찡 �ְ� (Giga CAM)
	// ����̹� ������ ���� �̿� ���� �Ǿ� �����Ƿ� �Ϲ����� �ڵ�� ������.
	#ifndef CONFIG_PROD_CPU_BCM
 	VK_TASK_Sleep(1000); // org 500
 	#else
	VK_TASK_Sleep(3500); // ��ε��� workaround : fail to read CIS w/ Neotion CAM for Canal Ready with development board and power unplugged. also see P_CI_Task().
	#endif

	Ret = PHYS_CIS(index);
	switch (Ret)
	{
	case OK :
		HxLOG_Print("PHYS_InitModule : the CIS of module %d is valid\n", DRVTab[index].Slot_id);
		/* Initialize the DVB Module */
		/* ========================= */
		if (PHYS_ResetModule(index, Host_Size, Negociate_Size))
		{
			HxLOG_Error("PHYS_InitModule : the module %d can't be initialized !!!\n", DRVTab[index].Slot_id);
			DRVTab[index].State = DRV_IDLE;
			if (svc_cas_DevCiResetModule(DRVTab[index].Slot_id) != ERR_OK)	// add by chojh
			{
				HxLOG_Error("PHYS_InitModule : fail to reset slot %d !!!\n", DRVTab[index].Slot_id);
			}
			return(W_SEACH);
		}
		else
		{
			HxLOG_Print("PHYS_InitModule : the module %d is initialized\n", DRVTab[index].Slot_id);
			return(OK);
		}
		break;

	case KO :
		HxLOG_Error("PHYS_InitModule : the CIS of module %d is wrong !!!\n", DRVTab[index].Slot_id);
		if (svc_cas_DevCiResetModule(DRVTab[index].Slot_id) != ERR_OK) // add by thyou  2007.08.14
		{
			HxLOG_Error("PHYS_InitModule : fail to reset slot %d !!!\n", DRVTab[index].Slot_id);
		}
		/* If a CAM is reseted during initialization, the state is same with CAM Extracted state. */
		DRVTab[index].State = DRV_DISCONNECT;
		return W_SEACH;
//		return(KO);	/* wrong CIS , warn the upper layers */
	default:
HxLOG_Error("PHYS_InitModule : the CIS of module %d is %d\n", DRVTab[index].Slot_id, Ret);
		return (W_SEACH);	/* error : no detected module */
	}

// 2010.03.16. Prevent ����. unreachable: This code cannot be reached: "DRVTab[index].State = 1/*sh..."
//	DRVTab[index].State = DRV_IDLE;

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
static unsigned char PHYS_Card(int index)
{
	int count;				/* timeout management */
	DRV_stSignal signal;	/* DRV_Ioctl argument */
	int Ret;				/* return code */

	signal.sig = DRV_CARD_DETECT;
	Ret = svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_TSIGNAL, &signal);
	if ((Ret > 0) && (signal.value))
	{
		/* Card Detect : wait for complete initialization */
		count = 0;
		signal.sig = DRV_READY_BUSY;
		signal.value = 0;

		while (1)
		{
			Ret = svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_TSIGNAL, &signal);
			if (Ret < 0)
				return MEM_ERR;

			if (signal.value)
				return OK;

			if (count == PHYS_TIMEOUT)
				return MEM_ERR; /* no module */

			ITC_Sleep(PHYS_DELAY);
			count++;
		}
	}

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
static unsigned char PHYS_CD(int index)
{
	DRV_stSignal signal;	/* DRV_Ioctl argument */
	int Ret;				/* return code */

	signal.sig = DRV_CARD_DETECT;
	signal.value = 0;

	Ret = svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_TSIGNAL, &signal);
	if ((Ret > 0) && (signal.value))
		return OK;
	else
		return W_SEACH;
}

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
static unsigned char PHYS_CIS(int index)
{
	unsigned char *cis; 			/* CIS buffer */
	unsigned short addrcor; 		/* COR Address */
	unsigned char cor;				/* COR value */
	unsigned int ciplus;			/* CI+ CAM detected */
	unsigned int ciprof;			/* CI+ additional features (bitmask) */
	int iRet;						/* return code */
	DRV_stMem mem;					/* driver parameters */
	unsigned short cisLen;

#ifndef USE_DOUGLAS_PRIMITIVE_FUNC

	unsigned short addrccrs;

	iRet = PHYS_GetAddrCcrs(DRVTab[index].Slot_id, &addrccrs);
	if (iRet < 0)
	{
		HxLOG_Error("PHYS_CIS : PHYS_GetAddrCcrs reports wrong CIS (%d) !!!\n", iRet);
		return KO;		/* wrong CIS */
	}

	cisLen = addrccrs >> 1;

	cis = (unsigned char *)ITC_DynAlloc(cisLen);
	if (!cis)
	{
		HxLOG_Error("PHYS_CIS : not enough memory (%d) !!!\n", cisLen);
		return MEM_ERR;
	}

	mem.addr   = DRV_MEMADDR;
	mem.len    = cisLen;
	mem.pbytes = cis;

	iRet = svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_READMEM, &mem);
	if (iRet < 0)
	{
		ITC_DynFree(cis);
		HxLOG_Error("PHYS_CIS : DRV_Ioctl error (%d) !!!\n", iRet);
		return MEM_ERR;	/* error while reading CIS */
	}

#else /* for douglas */

	cisLen = 1024; /* <TODO_CI> : üũ ���� */ /* see CI_CIS_SIZE at the DRV_ci_drv.h */

	cis = (unsigned char *)ITC_DynAlloc(cisLen);
	if (!cis)
	{
		HxLOG_Error("PHYS_CIS : not enough memory (%d) !!!\n", cisLen);
		return MEM_ERR;
	}

	HxSTD_memset(cis, 0, cisLen);

	iRet = DRV_CI_ReadCIS(DRVTab[index].Slot_id, cis, &mem.rlen);
	if (iRet < 0)
	{
		ITC_DynFree(cis);
		HxLOG_Error("PHYS_CIS : DRV_CI_ReadCIS error (%d) !!!\n", iRet);
		return MEM_ERR;	/* error while reading CIS */
	}

//	mem.rlen = 0x1fe/2; /* <TODO_CI> : üũ ���� -> DRV_CI_ReadCIS()���� len ������ �����ν� �ذ� */

#endif

#ifdef _CI_DEBUG_ // #ifdef _CI_PLUS_DEBUG_ // CI+ ���忡 ��õ� CI Plus Compatibility Identification Ȯ�� �뵵��.
{
	int i;
	HxLOG_Print("\n --- CIS : cisLen = %d, rlen = %d ---\n", cisLen, mem.rlen);
	for (i = 0; i < mem.rlen; i++)
	{
		if ((i & 15) == 0)
			HxLOG_Print("\n %03d :", i);
		HxLOG_Print(" %02X", cis[i]);
	}
	HxLOG_Print("\n\n");

/* Irdeto blue ķ - �ϸ� ���� ķ (CIv1) : 255 bytes
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

/* AlphaCrypt ķ (CIv1) : 255 bytes
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

/* SmarDTV CI+ old prototype ķ : 511 bytes
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

/* SmarDTV CI+ old prototype ķ : 511 bytes
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

/* Neotion ¦�� CI+ ķ with CI+ MHEG demo : 255 bytes
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

/* Neotion CI+ prototype ķ : 255 bytes
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

/* SMiT CI+ prototype ķ : 272 bytes
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

/* SMiT CI+ prototype ķ : 272 bytes
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

/* SMiT CI+ prototype ķ : 272 bytes
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

/* SMiT CI+ prototype ķ with ���� ������ : 272 bytes
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

/* Digital Keystone HPNX DVB CI+ �׽�Ʈ Ŷ ķ : 255 bytes
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
	ciplus = 0;
	ciprof = 0;
	if (PHYS_CheckCIS(cis, mem.rlen, &addrcor, &cor, &ciplus, &ciprof) < 0)
	{
		HxLOG_Error("PHYS_CIS : PHYS_CheckCIS reports wrong CIS !!!\n");
		ITC_DynFree(cis);
		return KO;		/* wrong CIS */
	}
	ITC_DynFree(cis);

	HxLOG_Print("PHYS_CIS : addrcor=%x, cor=%x, ciplus=%d\n", addrcor, cor, ciplus);
	DRVTab[index].Ci_plus = ciplus;
	DRVTab[index].Ci_prof = ciprof;

#ifndef USE_DOUGLAS_PRIMITIVE_FUNC

	/* the CIS is OK, addrcor contains the COR Address and cor the COR value */
	mem.addr   = addrcor >> 1;
	mem.len    = 1; 	/* one byte */
	mem.pbytes = &cor;
	mem.rlen   = 0;

	iRet = svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_WRITEMEM, &mem);
	if ((iRet > 0) && (mem.rlen == 1))
		return OK;
	else
		return MEM_ERR; /* error while writing the COR */

#else /* for douglas */

	iRet = CARM_CI_WriteCOR(DRVTab[index].Slot_id);
	if (iRet >= 0)
		return OK;
	else
		return MEM_ERR; /* error while writing the COR */

#endif
}

/*******************************************************************************
Name:		   unsigned char PHYS_WriteCommand(int index, unsigned char data)
Description:	Write a byte on the I/O Windows.
In: 				int index,
					unsigned char data
Return value:	data:
						0xff: Windows error.
******************************************************************************/
static unsigned char PHYS_WriteCommand(int index, unsigned char data, int flag)
{
	DRV_stIO reg;	/* Ioctl parameter */

	/* Test Card Detect */
	if (PHYS_CD(index) == W_SEACH)
	{
		HxLOG_Error("PHYS_WriteCommand : PHYS_CD error (%d) !!!\n", flag);
		return(0xff);
	}

	Command_Register = data;

	reg.registr = OFF_COM_REG;
	reg.pvalue  = &Command_Register;
	if (svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_WRITEIO, &reg) < 0)
	{
		HxLOG_Error("PHYS_WriteCommand : svc_cas_DevCiIoctl error (%d) !!!\n", flag);
		return(0xff);
	}

	return(Command_Register);
}

/*******************************************************************************
Name:		   unsigned char PHYS_ReadStatus(int index)
Description:	Read data from I/O Windows.
In: 				int index
Return value:	Status:
						0xff: Windows error.
******************************************************************************/
static unsigned char PHYS_ReadStatus(int index, int flag)
{
	DRV_stIO reg;	/* Ioctl parameter */

	/* Test Card Detect */
	if (PHYS_CD(index) == W_SEACH)
	{
		HxLOG_Error("PHYS_ReadStatus : PHYS_CD error (%d) !!!\n", flag);
		return(0xff);
	}

	Status_Register = 0;

	reg.registr = OFF_STA_REG;
	reg.pvalue  = &Status_Register;
	if (svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_READIO, &reg) < 0)
	{
		HxLOG_Error("PHYS_ReadStatus : svc_cas_DevCiIoctl error (%d) !!!\n", flag);
		return(0xff);
	}

	return(Status_Register);
}

/*******************************************************************************
Name:		   unsigned short PHYS_WriteSize(int index, unsigned short size)
Description:	Write size on the I/O Windows.
In: 				int index,
					unsigned short size
Return value:	size:
						0: Windows error.
******************************************************************************/
static unsigned short PHYS_WriteSize(int index, unsigned short size)
{
	DRV_stIO reg;	/* Ioctl parameter */

	/* Test Card Detect */
	if (PHYS_CD(index) == W_SEACH)
	{
		HxLOG_Error("PHYS_WriteSize : PHYS_CD error !!!\n");
		return(0);
	}

	Size_LS_Register = (unsigned char)(size & 0x00ff);
	Size_MS_Register = (unsigned char)(size >> 8);

	reg.registr = OFF_SIZE_LS_REG;
	reg.pvalue  = &Size_LS_Register;
	if (svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_WRITEIO, &reg) < 0)
	{
		HxLOG_Error("PHYS_WriteSize : svc_cas_DevCiIoctl error for LS !!!\n");
		return(0);
	}

	PHYS_DelayMargin; /* add delay after LS */

	reg.registr = OFF_SIZE_MS_REG;
	reg.pvalue  = &Size_MS_Register;
	if (svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_WRITEIO, &reg) < 0)
	{
		HxLOG_Error("PHYS_WriteSize : svc_cas_DevCiIoctl error for MS !!!\n");
		return(0);
	}

	PHYS_DelayMargin; /* add delay after MS */

	return(size);
}

/*******************************************************************************
Name:		   unsigned short PHYS_ReadSize(int index)
Description:	Read size from I/O Windows.
In: 				int index
Return value:	size:
						0: Windows error.
******************************************************************************/
static unsigned short PHYS_ReadSize(int index)
{
	DRV_stIO reg;	/* Ioctl parameter */

	/* Test Card Detect */
	if (PHYS_CD(index) == W_SEACH)
	{
		HxLOG_Error("PHYS_ReadSize : PHYS_CD error !!!\n");
		return(0);
	}

	Size_LS_Register = Size_MS_Register = 0;

	reg.registr = OFF_SIZE_LS_REG;
	reg.pvalue  = &Size_LS_Register;
	if (svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_READIO, &reg) < 0)
	{
		HxLOG_Error("PHYS_ReadSize : svc_cas_DevCiIoctl error for LS !!!\n");
		return(0);
	}

	reg.registr = OFF_SIZE_MS_REG;
	reg.pvalue  = &Size_MS_Register;
	if (svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_READIO, &reg) < 0)
	{
		HxLOG_Error("PHYS_ReadSize : svc_cas_DevCiIoctl error for MS !!!\n");
		return(0);
	}

	return((((unsigned short)Size_MS_Register << 8) & 0xff00) | Size_LS_Register);
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
static unsigned char PHYS_ResetModule(int index, unsigned int Host_Size, unsigned short *Negociate_Size)
{
	unsigned char	buff[2];
	unsigned short	Size;
	unsigned char	status;
	int 			count;
	DRV_ssSignal	signal;	/* DRV_Ioctl argument */
	int				Ret;

	/* clear the EMSTREAM pin */

	signal.cs  = 0;
	signal.pin = DRV_EMSTREAM;
	Ret = svc_cas_DevCiIoctl(DRVTab[index].Slot_id, DRV_SSIGNAL, &signal);
	if (Ret < 0)
	{
		HxLOG_Error("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return(RS_ERR);
	}

	/* set RS bit to reset the interface and wait for FR bit to be set */

	if (PHYS_WriteCommand(index, RS, __LINE__) != RS) return(RS_ERR);


	/* keep RS bit for a while (10ms recommend by Xcrypt) */

	//	ITC_Sleep(10);

	/* reset RS bit */

	if (PHYS_WriteCommand(index, 0x00, __LINE__) != 0x00) return(RS_ERR);

	count = 0;
	while (1)
	{
		if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(RS_ERR);

		if (status & FR)
			break;

		if (count == PHYS_TIMEOUT)
		{
			HxLOG_Error("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return(RS_ERR);
		}

		ITC_Sleep(PHYS_DELAY);
		count++;
	}

	/* perform buffer size exchange */
	/* ==================== */

	/* set SR bit and wait for DA bit to be set */

	if (PHYS_WriteCommand(index, SR, __LINE__) != SR) return(RS_ERR);

	count = 0;
	while (1)
	{
		if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(RS_ERR);

		if (status & DA)
			break;

		if (count == PHYS_TIMEOUT)
		{
			HxLOG_Error("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return(RS_ERR);
		}

		ITC_Sleep(PHYS_DELAY);
		count++;
	}

	/* read module buffer size */

	if ((Size = PHYS_ReadSize(index)) == 0) return(RS_ERR);

	if (Size != 2)
	{
		HxLOG_Error("PHYS_ResetModule : Wrong Size %d !!!\n", Size);
	}

	if (svc_cas_DevCiRead(DRVTab[index].Slot_id, 2, buff) != 2)
	{
		HxLOG_Error("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return(RS_ERR);
	}

	/* reset SR bit */

	if (PHYS_WriteCommand(index, 0x00, __LINE__) != 0x00) return(RS_ERR);

	/* check nego buffer size */

	(*Negociate_Size) = buff[0] * 256 + buff[1];

	#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
	HxLOG_Print("PHYS_ResetModule : Size = %d, Negociate_Size = %d, Host_Size = %d\n", Size, *Negociate_Size, Host_Size);
	#endif

	if ((*Negociate_Size) > Host_Size)
	{
		// ���� �ƴϳ� �ָ��� ���� �α׶� �־��. �� ��� ���� �����ϴٸ� Host_Size (MAXLNSIZE) �� �÷��ִ� ���� ����.
		HxLOG_Error("PHYS_ResetModule : Negociate_Size = %d > Host_Size = %d !!!\n", *Negociate_Size, Host_Size);
		buff[0] = Host_Size >> 8;
		buff[1] = Host_Size;
		(*Negociate_Size) = Host_Size;
	}

	/* set SW bit and wait for FR bit to be set */

	if (PHYS_WriteCommand(index, SW, __LINE__) != SW) return(RS_ERR);

	count = 0;
	while (1)
	{
		if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(RS_ERR);

		if (status & FR)
			break;

		if (count == PHYS_TIMEOUT)
		{
			HxLOG_Error("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
			return(RS_ERR);
		}

		ITC_Sleep(PHYS_DELAY);
		count++;
	}

	/* write nego buffer size */

	if (PHYS_WriteSize(index, 2) != 2) return(RS_ERR);

	if (svc_cas_DevCiWrite(DRVTab[index].Slot_id, 2, buff) != 2)
	{
		HxLOG_Error("PHYS_ResetModule : Error at %d lines !!!\n", __LINE__);
		return(RS_ERR);
	}

	/* reset SW bit */

	if (PHYS_WriteCommand(index, 0x00, __LINE__) != 0x00) return(RS_ERR);

	return(OK);
}

/*******************************************************************************
Name:			unsigned char PHYS_SendData( unsigned char *pData, unsigned short Size_in, int flag)
Description:	Send data to PSCAM.
In: 				int 					index:	 DRVTab index
				unsigned char		*pData:  Pointer to data array
				unsigned short		Size_in: size of data.
				int flag :			0 not used
									1 if the calling function is PHYS_DRVWrite
									2 if the calling function is PHYS_ResetModule -> ���� PHYS_ResetModule()���� �� �Լ��� ȣ������ ����.
Out:			Nothing.
Return value:	Status:
				OK: 	No Pb.
				M_BUSY: DVB Module is busy.
				T_ERR:	Transfert Error
				W_SEARCH
******************************************************************************/
static unsigned char PHYS_SendData(int index, unsigned char *pData, unsigned short Size_in /*, int flag */)
{
	unsigned char status;

	if (DRVTab[index].State == DRV_IDLE)
	{
		HxLOG_Error("PHYS_SendData(type=%s,size=%d) : DRV_IDLE !!!\n", get_tpdu_type(pData, Size_in), Size_in);
		return W_SEACH;
	}

	/* perhaps received data are present  : test DA */
	if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

	if (status & DA)
	{
		HxLOG_Error("PHYS_SendData(type=%s,size=%d) : DA\n", get_tpdu_type(pData, Size_in), Size_in);
		PHYS_ReceiveIdle(index); /* <TODO_CI> : �� �Լ��� ������ �߻��ϰų� �� ���� �ٽ� DA �� set �� ��쿡 ���� ó���� ���ؼ� ���� ��� �ʿ�... */
	}

	/* set bit HC from Command register */
	if (PHYS_WriteCommand(index, HC, __LINE__) != HC) return(W_SEACH);

	/* test FR */
	if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff)
	{
		/* down bit HC from Command register */
		if (PHYS_WriteCommand(index, 0x00, __LINE__) != 0x00) return(W_SEACH);
		return(W_SEACH);
	}

	if (!(status & FR)) /* if dvb module is busy */
	{
		HxLOG_Print("PHYS_SendData(type=%s,size=%d) : FR = 0\n", get_tpdu_type(pData, Size_in), Size_in);
		/* down bit HC from Command register */
		if (PHYS_WriteCommand(index, 0x00, __LINE__) != 0x00) return(W_SEACH);
		return(M_BUSY);
	}

	if (status & WE)
	{
		HxLOG_Error("PHYS_SendData(type=%s,size=%d) : WE\n", get_tpdu_type(pData, Size_in), Size_in);
	}

	/* write data size */
	/* ========== */

	#if 0 // �̷� ������ �߻����� ����.
	if ((Size_in == 0x0) || (DRVTab[index].State == DRV_CONNECT && Size_in > DRVTab[index].Negociated_size)) // �̷� ���� �߻��Ѵٸ� ���� �ڵ忡 ���� ������ �ִٴ� �Ŵ�. ����� �ʿ�.
	{
		HxLOG_Error("[CI_RW_ERROR] PHYS_SendData : write size invalid %d (MAXLNSIZE = %d, Nego = %d) !!!\n", Size_in, MAXLNSIZE, DRVTab[index].Negociated_size);
		return(W_SEACH);
	}
	#endif

	if (PHYS_WriteSize(index, Size_in) != Size_in) return(W_SEACH);

	/* write data */
	/* ======= */

#if (PHYS_RW_ROBUST_CODE_ENABLED_FLAG == 0)

	if (svc_cas_DevCiWrite(DRVTab[index].Slot_id, Size_in, pData) != (int)Size_in)
	{
		HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : svc_cas_DevCiWrite error (%d line) !!!\n", get_tpdu_type(pData, Size_in), Size_in, __LINE__);
		return(W_SEACH);
	}

#else

	if (Size_in == DRVTab[index].Negociated_size) /* ���� svc_cas_DevCiWrite() ���� �� ȣ��Ʈ�� CI+ authentication �����͸� write �� �� �ʹ� ���� �ɸ��� �ǹǷ�... */
	{
		if (svc_cas_DevCiWrite(DRVTab[index].Slot_id, Size_in, pData) != (int)Size_in)
		{
			HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : svc_cas_DevCiWrite error (%d line) !!!\n", get_tpdu_type(pData, Size_in), Size_in, __LINE__);
			return(W_SEACH);
		}
	}
	else /* Size_in > 2 and Size_in < nego size */
	{
		int i, retry;

		if (svc_cas_DevCiWrite(DRVTab[index].Slot_id, 1, pData) != 1)
		{
			HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : svc_cas_DevCiWrite error (%d line) !!!\n", get_tpdu_type(pData, Size_in), Size_in, __LINE__);
			return(W_SEACH);
		}

		PHYS_DelayMargin; /* add delay after 1st byte */

		for (i = 1; i < Size_in; i++)
		{
			retry = 0;
			while (1)
			{
				if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

				if ((status & WE) && !(status & FR)) /* When the first byte is written the module sets WE to 1 and sets FR to 0. */
					break;

				if (retry == PHYS_RETRY_RW_ERROR) /* �� ��Ȳ�� �Ǹ� ���⼭ �����ϰ� ���� �����Ͽ� ��ü operation �� �ٽ� �����ϵ��� �Ѵ�. */
				{
					HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : wrong status reg value (0x%02x) at %dth byte !!!\n", get_tpdu_type(pData, Size_in), Size_in, status, i);
					/* down bit HC from Command register and return T_ERR */
					if (PHYS_WriteCommand(index, 0x00, __LINE__) != 0x00) return(W_SEACH);
					return(T_ERR);
				}

				HxLOG_Error("PHYS_SendData(type=%s,size=%d) : wrong status reg value (0x%02x) at %dth byte\n", get_tpdu_type(pData, Size_in), Size_in, status, i);

				ITC_Sleep(PHYS_DELAY);
				retry++;
			}

			if (svc_cas_DevCiWrite(DRVTab[index].Slot_id, 1, &pData[i]) != 1)
			{
				HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : svc_cas_DevCiWrite error at %dth byte (%d line) !!!\n", get_tpdu_type(pData, Size_in), Size_in, i, __LINE__);
				return(W_SEACH);
			}
		}
	}

#endif

	/* verify if WE is down */
	if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

	if (status & WE)
	{
		HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : WE (0x%02x) !!!\n", get_tpdu_type(pData, Size_in), Size_in, status);

		/* really? one more try -> ���⼭ WE �� �ٽ� üũ�ϴ� ���� ���ʿ��� ����... �̷� ��Ȳ�� �߻��� ��츦 ���� last byte �� ����� �������� ���� ��쿴��. */

		ITC_Sleep(PHYS_DELAY);

		if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

		if (status & WE)
		{
			HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : WE (0x%02x) still !!!\n", get_tpdu_type(pData, Size_in), Size_in, status);

			/* ������ ����Ʈ�� �ٽ� write �غ��� */

			if (svc_cas_DevCiWrite(DRVTab[index].Slot_id, 1, &pData[Size_in-1]) != 1)
			{
				HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : svc_cas_DevCiWrite error at last byte (%d line) !!!\n", get_tpdu_type(pData, Size_in), Size_in, __LINE__);
				return(W_SEACH);
			}

			ITC_Sleep(PHYS_DELAY);

			if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

			if (status & WE)
			{
				HxLOG_Error("[CI_RW_ERROR] PHYS_SendData(type=%s,size=%d) : WE (0x%02x) failed !!!\n", get_tpdu_type(pData, Size_in), Size_in, status);

				/* down bit HC from Command register and return T_ERR */
				#if 0 /* �� ��� ���� �����Ͽ� ��ü operation �� �ٽ� �����ϵ��� �ϸ� write ���� WE �� set �Ǿ� �ִ� ���°� �ǰ� �̻��ϰԵ� Tsb with no DA �� 2�� ������. �Ͽ� �׳� OK �����ϵ��� ó���Ѵ�. */
				if (PHYS_WriteCommand(index, 0x00, __LINE__) != 0x00) return(W_SEACH);
				return(T_ERR);
				#endif
			}
		}
	}

	/* down bit HC from Command register */
	if (PHYS_WriteCommand(index, 0x00, __LINE__) != 0x00) return(W_SEACH);

#ifdef TSB_ERROR_HANDLING
	uc_Expected_Tsb[0] = uc_Expected_Tsb[4] = pData[0];
#endif

	return(OK);
}

/*******************************************************************************
Name:			void PHYS_ReceiveIdle( )
Description:	Receive data and stores LPDU for into a linked list
In: 				int index : Index into DRVTab[]
Out:			Nothing.
Return value:	Nothing
******************************************************************************/
/* <TODO_CI> : PHYS_ReceiveIdle() �Լ��� ������ ���� ������ ���ŵǾ�� �Ѵ�. �� �Լ��� ����ϴ� ���� �ٶ������� �ʴ�!

	���� �÷ο� :
	ȣ��Ʈ�� ������ A�� write �Ϸ� �� -> ��ħ ķ�� ������ B�� ������ ��
	-> �� �Լ��� ���� �ϴ� B�� read �Ͽ� �ӽ� ���� -> A�� write -> ���� read phase ���� B�� ���� ó�� ���� (���� ���� ����)

	�ٸ����� �÷ο� :
	ȣ��Ʈ�� ������ A�� write �Ϸ� �� -> ��ħ ķ�� ������ B�� ������ ��
	-> A�� write �Ϸ��� ���� �ӽ� �����ϰ� read phase �� ��ȯ�Ͽ� B�� read �ϰ� ó�� (���� ���� ����) -> �����Ǿ��� A�� write �Ѵ�.

	R/W �÷ο�� �������� ķ�� HC & FR / DA �� ������ ���� ���信 �����Ͽ� ��Ʈ�� �ǹǷ� �׻� ķ �켱���� ó���Ǿ�� ķ �� �÷ο�� �°� ó���� �ȴ�.

	Ư�� ������ A�� �����̶�� ���� ���� ���� :
	1. Whilst the module still has data to send, that is, the host has received status bytes with the DA indicator set, then the poll function shall be suspended.
	    It shall be restarted when a status byte is received with the DA indicator not set.
	2. The host does not send any additional polls whilst waiting for a poll response, even if its normal poll interval is exceeded.

	�ʿ��� �۾� :
	1. physical layer task �� union ���� ����� ph_infos �� ���� �ʿ�.
	2. �� �Լ��� �����ϰ� �� �Լ��� ȣ��Ǵ� �������� write operation �� �����ϰ� read phase �� ��ȯ�ϵ��� ó���ؾ� ��.
*/
static void PHYS_ReceiveIdle(int index)
{
	SLPDU *pSLPDU;
	SLPDU *p;
	unsigned short length;
	unsigned char status;

	/* call PHYS_ReceiveData without linked list management since DA is set and NEW DATA are present */
	if ((status = PHYS_ReceiveData(index, pDRVData, &length, 0)) == OK)
	{
		if ((pSLPDU = (SLPDU *)ITC_DynAlloc(sizeof(SLPDU))) == NULL)
		{
			HxLOG_Error("PHYS_ReceiveIdle : not enough memory for SLPDU (%d) !!!\n", sizeof(SLPDU));
			return;
		}
		if ((pSLPDU->buffer = ITC_DynAlloc(length)) == NULL)
		{
			HxLOG_Error("PHYS_ReceiveIdle : not enough memory for buffer (%d) !!!\n", length);
			ITC_DynFree((unsigned char *)pSLPDU);
			return;
		}
		HxSTD_memcpy(pSLPDU->buffer, pDRVData, length);
		pSLPDU->length = length;
		pSLPDU->next = NULL;

		if (DRVTab[index].firstSLPDU)
		{
			HxLOG_Error("PHYS_ReceiveIdle : DRVTab[%d].firstSLPDU has already SLPDU\n", index);
			/* Add LPDU at the end of the linked list */
			p = DRVTab[index].firstSLPDU;
			while (p->next)
				p = p->next;
			p->next = pSLPDU;
		}
		else
		{
			/* First LPDU into the linked list */
			DRVTab[index].firstSLPDU = pSLPDU;
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
			HxLOG_Print("PHYS_ReceiveIdle : %d bytes received from the module %d : %s\n", length, DRVTab[index].Slot_id, buf_tpdu);
		}
		#endif
	}
	else
	{
		HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveIdle : NOT OK (%d) !!!\n", status);
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
In: 				int index : 		Index into DRVTab[]
				int flag :		0 if the calling function is PHYS_ReceivedIdle
								1 if the calling function is PHYS_DRVRead
								2 if the calling function is PHYS_ResetModule -> ���� PHYS_ResetModule()���� �� �Լ��� ȣ������ ����.
Out:			unsigned char	*pData:    Pointer to data array
				unsigned short	*Size_Out: Size of data.
Return value:	Status:
				OK: 	No Pb
				M_DATA: No Data available.
				M_BUSY: PSCAM is busy.
				T_ERR:	Transfert Error.
				W_SEARCH
******************************************************************************/
static unsigned char PHYS_ReceiveData(int index, unsigned char *pData, unsigned short *Size_out, int flag)
{
	SLPDU *free_p;
	unsigned char status;

	if (DRVTab[index].State == DRV_IDLE)
	{
		HxLOG_Error("PHYS_ReceiveData(flag=%d) : DRV_IDLE !!!\n", flag);
		return(W_SEACH);
	}

	if (flag && DRVTab[index].firstSLPDU)
	{
		HxLOG_Print("PHYS_ReceiveData(flag=%d) : %d bytes had been received from the module %d\n", flag, DRVTab[index].firstSLPDU->length, DRVTab[index].Slot_id);

		/* Received data while sending previous data */
		HxSTD_memcpy(pData, DRVTab[index].firstSLPDU->buffer, DRVTab[index].firstSLPDU->length);
		*Size_out = DRVTab[index].firstSLPDU->length;

		/* free the received buffer into the linked list and update the head */
		free_p = DRVTab[index].firstSLPDU;
		DRVTab[index].firstSLPDU = DRVTab[index].firstSLPDU->next;
		ITC_DynFree(free_p->buffer);
		ITC_DynFree((unsigned char *)free_p);

		HxLOG_Error("PHYS_ReceiveData(flag=%d) : DRVTab[%d].firstSLPDU has another SLPDU\n", flag, index);

		return(OK);
	}

	if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

#ifdef CONFIG_MW_CI_PLUS
	if (status & IIR)
	{
//		HxLOG_Print("<IIR>");
		if (CI_IsCiPlusCam(DRVTab[index].Slot_id) > 0) /* �Ϲ� ķ�� ��� �������ϴ� ���� ���� �� �����Ƿ� CI+ ķ�� ���ؼ��� �ϵ��� ����. �׸��� ��Ȥ status �� �߸� ������ ��찡 ������ �� �� �� üũ�� ����. */
		{
			if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

			if (status & IIR)
			{
				return(R_IIR);
			}
		}
	}
#endif

	if (!(status & DA))
	{
		return(M_DATA); /* if no data available */ /* PHYS_ReceiveIdle() �κ��� ȣ��Ǿ� �� ������ �Ǹ� ������ ���̴�. �� ��� PHYS_ReceiveIdle() ���� ���� ����. */
	}

	if (status & RE)
	{
		HxLOG_Error("PHYS_ReceiveData(flag=%d) : RE\n", flag);
	}

	/* read data size */
	/* ========== */

	if ((*Size_out = PHYS_ReadSize(index)) == 0) return(W_SEACH);

	if (*Size_out > DRVTab[index].Negociated_size)
	{
		/* really? one more try */

		unsigned short Size_again;

		if ((Size_again = PHYS_ReadSize(index)) == 0) return(W_SEACH);

		if (Size_again > DRVTab[index].Negociated_size)
		{
			HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d) : read size invalid %d, %d (MAXLNSIZE = %d, Nego = %d) -> reset cam !!!\n",
				flag, *Size_out, Size_again, MAXLNSIZE, DRVTab[index].Negociated_size);

			if (svc_cas_DevCiResetModule(DRVTab[index].Slot_id) != ERR_OK)
			{
				HxLOG_Error("PHYS_ReceiveData(flag=%d) : fail to reset slot %d !!!\n", flag, DRVTab[index].Slot_id);
			}

			return(W_SEACH);
		}

		HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d) : read size invalid %d recovered to %d (MAXLNSIZE = %d, Nego = %d) !!!\n",
			flag, *Size_out, Size_again, MAXLNSIZE, DRVTab[index].Negociated_size);

		*Size_out = Size_again;
	}

	/* read data */
	/* ======= */

#if (PHYS_RW_ROBUST_CODE_ENABLED_FLAG != 2)

	if (svc_cas_DevCiRead(DRVTab[index].Slot_id, *Size_out, pData) != (int)*Size_out)
	{
		HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d,size=%d) : svc_cas_DevCiRead error (%d line) !!!\n", flag, *Size_out, __LINE__);
		return(W_SEACH);
	}

#else

	if (*Size_out == DRVTab[index].Negociated_size) /* ���� svc_cas_DevCiRead() ���� �� ū �������� CI+ MHEG �����͸� read �� �� �ʹ� ���� �ɸ��� �ǹǷ�... */
	{
		if (svc_cas_DevCiRead(DRVTab[index].Slot_id, *Size_out, pData) != (int)*Size_out)
		{
			HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d,size=%d) : svc_cas_DevCiRead error (%d line) !!!\n", flag, *Size_out, __LINE__);
			return(W_SEACH);
		}
	}
	else /* *Size_out > 2 and *Size_out < nego size */
	{
		int i, retry;

		if (svc_cas_DevCiRead(DRVTab[index].Slot_id, 1, pData) != 1)
		{
			HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d,size=%d) : svc_cas_DevCiRead error (%d line) !!!\n", flag, *Size_out, __LINE__);
			return(W_SEACH);
		}

		for (i = 1; i < *Size_out; i++)
		{
			retry = 0;
			while (1)
			{
				if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

				if ((status & RE) && !(status & DA)) /* When the first byte is read the module sets RE to 1 and sets DA to 0. */
					break;

				if (retry == PHYS_RETRY_RW_ERROR) /* �� ��Ȳ�� �Ǹ� ���⼭ �����ϰ� ���� �����Ѵ�. */
				{
					HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d,size=%d) : wrong status reg value (0x%02x) at %dth byte !!!\n", flag, *Size_out, status, i);
					return(T_ERR);
				}

				HxLOG_Error("PHYS_ReceiveData(flag=%d,size=%d) : wrong status reg value (0x%02x) at %dth byte\n", flag, *Size_out, status, i);

				ITC_Sleep(PHYS_DELAY);
				retry++;
			}

			if (svc_cas_DevCiRead(DRVTab[index].Slot_id, 1, &pData[i]) != 1)
			{
				HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d,size=%d) : svc_cas_DevCiRead error at %dth byte (%d line) !!!\n", flag, *Size_out, i, __LINE__);
				return(W_SEACH);
			}
		}
	}

#endif

	if ((status = PHYS_ReadStatus(index, __LINE__)) == 0xff) return(W_SEACH);

	if (status & RE)
	{
		HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d,size=%d) : RE (0x%02x) !!!\n", flag, *Size_out, status);
//		return(T_ERR); /* <TODO_CI> : �̷� ��쿡 ���� ó���� ���ؼ� ���� ��� �ʿ�... */
	}

	if (status & DA)
	{
		HxLOG_Error("PHYS_ReceiveData(flag=%d,size=%d) : DA\n", flag, *Size_out);

#if 1
		{
			/* CI+ MHEG ������ ū �������� �����͵�(100KB, 200KB)�� ���۵� �� �ֱ� ������
			   ���� TM_TRANS �����δ� �� �ҽ� ������ ķ�� ���µǴ� ������ �߱��ϹǷ� ������ ���� �ٺ������� ������. */

			unsigned char ucTcId = 0;
			int index = 0;
			if (CIRH_GetTcId(DRVTab[index].Slot_id, &ucTcId) == ERR_OK && ucTcId != 0)
			{
				index = TRPT_GetTcid(ucTcId);
				if (index < 0)
				{
					HxLOG_Error("PHYS_ReceiveData(flag=%d,size=%d) : TRPT_GetTcid() error !!!\n", flag, *Size_out);
				}
				else
				{
					TRPT_Set_Timer(index);
				}
			}
			else
			{
				HxLOG_Error("PHYS_ReceiveData(flag=%d,size=%d) : CIRH_GetTcId() error !!!\n", flag, *Size_out);
			}
		}
#endif

//		return(M_BUSY); /* <TODO_CI> : �̷� ��쿡 �� �÷ο쿡�� �����͸� �ٽ� ���������� �ϴ� ���� ���� �� ������... ���� ��� �ʿ�... -> physical() ���� �ڵ鸵�ϵ��� �Ͽ��� */
	}

#ifdef TSB_ERROR_HANDLING
	if (*Size_out == 6 /* length �� 6 ���ν� ķ���� ������ TPDU �� �������� Tsb �ۿ� ����. */
		&&
			((pData[0] != 0x01 && pData[0] != 0x02)
			|| (pData[1] != 0x00)
			|| (pData[2] != 0x80)
			|| (pData[3] != 0x02)
			|| (pData[4] != pData[0])
			|| (pData[5] != 0x00 && pData[5] != 0x80)
			)
		)
	{
		// �ϴ� ���� ��� �ְ�...
		HxLOG_Error("[CI_RW_ERROR] PHYS_ReceiveData(flag=%d) : wrong Tsb = [ %02x %02x %02x %02x %02x %02x ] -> expected [ %02x %02x %02x %02x %02x (00 or 80) ] !!!\n",
			flag,			pData[0], pData[1], pData[2], pData[3], pData[4], pData[5],
			uc_Expected_Tsb[0], uc_Expected_Tsb[1], uc_Expected_Tsb[2], uc_Expected_Tsb[3], uc_Expected_Tsb[4]);

		// ���Ǵ� T_SB ������ ���� ����
		HxSTD_memcpy(pData, uc_Expected_Tsb, 6);
	}
#endif

	return(OK);
}

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
int PHYS_IsCiPlusCam(unsigned short slot)
{
	int index;

	for (index = 0; index < CARM_MAX_CISLOT_NUMBER; index++)
	{
		if (DRVTab[index].Slot_id == slot)
		{
			if (DRVTab[index].State == DRV_CONNECT)
			{
				return DRVTab[index].Ci_plus;
				}
				else
				{
				return -1; // ķ�� ���� ���� �ʴ� (���� ����)
				}
			}
	}

	return -1; // ķ�� ���� ���� �ʴ�. (�̻� ����)
}

int PHYS_OperatorProfileSupported(unsigned short slot)
{
	int index;

	for (index = 0; index < PAL_MAX_CISLOT_NUMBER; index++)
	{
		if (DRVTab[index].Slot_id == slot)
		{
			if (DRVTab[index].State == DRV_CONNECT)
			{
				return (DRVTab[index].Ci_prof & 0x00000001);
			}
			else
			{
				return -1; // ķ�� ���� ���� �ʴ� (���� ����)
			}
		}
	}

	return -1; // ķ�� ���� ���� �ʴ� (�̻� ����)
}

static int PHYS_CheckCIS(unsigned char *cis, int len, unsigned short *addrcor, unsigned char *cor, unsigned int *ciplus, unsigned int *ciprof)
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

	HxLOG_Print("PHYS_CheckCIS : CISTPL_DEVICE_0A\n");
	if (PHYS_Tuple(0, CISTPL_DEVICE_0A, &pcis, &lcis, &first, &addr, &dvb, &last, ciplus, ciprof) < 0)
	{
		HxLOG_Error("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0A\n");
		return -1;	/* not found or wrong CISTPL_DEVICE_0A */
	}

//	pcis = cis;
//	lcis = len;

	HxLOG_Print("PHYS_CheckCIS : CISTPL_DEVICE_0C\n");
	if (PHYS_Tuple(0, CISTPL_DEVICE_0C, &pcis, &lcis, &first, &addr, &dvb, &last, ciplus, ciprof) < 0)
	{
		HxLOG_Error("PHYS_CheckCIS : not found or wrong CISTPL_DEVICE_0C\n");
		return -1;	/* not found or wrong CISTPL_DEVICE_0C */
	}

//	pcis = cis;
//	lcis = len;

	HxLOG_Print("PHYS_CheckCIS : CISTPL_VERS_1\n");
	if ((iRet = PHYS_Tuple(0, CISTPL_VERS_1, &pcis, &lcis, &first, &addr, &dvb, &last, ciplus, ciprof)) < 0)
	{
		HxLOG_Error("PHYS_CheckCIS : not found or wrong CISTPL_VERS_1\n");
		return -1;	/* not found or wrong CISTPL_VERS_1 */
	}

//	pcis = cis;
//	lcis = len;

	HxLOG_Print("PHYS_CheckCIS : CISTPL_MANFID\n");
	if (PHYS_Tuple(0, CISTPL_MANFID, &pcis, &lcis, &first, &addr, &dvb, &last, ciplus, ciprof) < 0)
	{
		HxLOG_Error("PHYS_CheckCIS : not found or wrong CISTPL_MANFID\n");
		return -1;	/* not found or wrong CISTPL_MANFID */
	}

//	pcis = cis;
//	lcis = len;

	HxLOG_Print("PHYS_CheckCIS : CISTPL_CONFIG\n");
	if (PHYS_Tuple(0, CISTPL_CONFIG, &pcis, &lcis, &first, &addr, &dvb, &last, ciplus, ciprof) < 0)
	{
		HxLOG_Error("PHYS_CheckCIS : not found or wrong CISTPL_CONFIG\n");
		return -1;	/* not found or wrong CISTPL_CONFIG */
	}

	*addrcor = addr;
	first = 1;
	do
	{
		HxLOG_Print("PHYS_CheckCIS : CISTPL_CFTABLE_ENTRY\n");
		iRet = PHYS_Tuple(0, CISTPL_CFTABLE_ENTRY, &pcis, &lcis, &first, &addr, &dvb, &last, ciplus, ciprof);
	}
	while ((iRet == 0) || (iRet == -2));	/* one more entry */
	if (iRet < 0)
	{
		HxLOG_Error("PHYS_CheckCIS : not found or wrong CISTPL_CFTABLE_ENTRY\n");
		return -1;	/* not found or wrong CISTPL_CFTABLE_ENTRY */
	}

	HxLOG_Print("PHYS_CheckCIS : addr=%x, dvb=%x, last=%x\n", addr, dvb, last);

	HxLOG_Print("PHYS_CheckCIS : CISTPL_NO_LINK\n");
	if (PHYS_Tuple(0, CISTPL_NO_LINK, &pcis, &lcis, &first, &addr, &dvb, &last, ciplus, ciprof) < 0)
	{
		HxLOG_Error("PHYS_CheckCIS : not found or wrong CISTPL_NO_LINK\n");
		return -1;	/* not found or wrong CISTPL_NO_LINK */
	}

	HxLOG_Print("PHYS_CheckCIS : CISTPL_END\n");
	if (PHYS_Tuple(0, CISTPL_END, &pcis, &lcis, &first, &addr, &dvb, &last, ciplus, ciprof) < 0)
	{
		HxLOG_Error("PHYS_CheckCIS : not found or wrong CISTPL_END\n");
		return -1;	/* not found or wrong CISTPL_END */
	}

	/* the index of the DVB entry is the COR value */

//	if (dvb != DVB_INDEX)
//	{
HxLOG_Error("PHYS_CheckCIS : DVB entry not found (dvb=%x, last=%x)\n", dvb, last);
//		return -1;
//	}

	if (dvb != last)
	{
		HxLOG_Error("PHYS_CheckCIS : DVB entry found but... (dvb=%x, last=%x)\n", dvb, last);
	}

	*cor = dvb;
//	*cor = last;

	return 1;			/* good CIS */
}

/* ======================================================================== */
/* NAME : PHYS_GetCiPlusInfoFromString																*/
/*																								*/
/* DESCRIPTION : Get CI Plus and Operator Profile support information from CISTPL_VERS_1 INFO				*/
/*																								*/
/* INPUT PARAMETERS :																			*/
/*		pcInfoStr: string data of TPLLV1_INFO	which is existed in CISTPL_VERS_1							*/
/*		nInfoLength: the length of pcInfoStr															*/
/*		ciplus : the information that notify a host of a ciplus support										*/
/*		ciprof : the information that notify a host of the operator profile support								*/
/*																								*/
/* OUTPUT PARAMETERS :																			*/
/*		ciplus : the information that notify a host of a ciplus support										*/
/*		ciprof : the information that notify a host of the operator profile support								*/
/*																								*/
/* RETURN CODE																					*/
/*		   0 : OK																					*/
/*		  -1 : error																				*/
/* ======================================================================== */

static int PHYS_GetCiPlusInfoFromString(char* pcInfoStr, int nInfoLength, unsigned int *ciplus, unsigned int *ciprof)
{
	int i = 0;
	char* currentCharPoint;	// char point to indicate the location of string (used for whole info str)
	char* currentArrPoint;	// char point to indicate the location of string (used for the content of '$compatible' which is obtained from InfoStr as array)
	char acInfoStr[256] = {0x00,};		// maybe max tuple data length is 254
	char acInfoCmdStr[256] = {0x00,}; 	// maybe max tuple data length is 254


	for (i = 0; i < nInfoLength; i++)	// make the string to lower case
	{
		acInfoStr[i] = tolower(*(pcInfoStr + i));
	}

	currentCharPoint = strstr(acInfoStr, "$compatible[");

	if (currentCharPoint)		// if there is $compatible commend, then this Info string has to be checked.
	{
		currentCharPoint = currentCharPoint + 12;		// the number '12' is added for currentCharPoint to indicate after '$compatible['

		i = 0;
		while(*(currentCharPoint + i) != ']')
		{
			if(i > 255)
			{
				HxLOG_Error("[CIS] CIv1 or CI+ CAM but doesn't meet CI+ spec - not found ']' after '$compatible['\n");
				return -1;
			}
			acInfoCmdStr[i] = *(currentCharPoint + i);
			i++;
		}

		// check : "ciplus" token

		currentArrPoint = strstr(acInfoCmdStr, "ciplus");
		if(currentArrPoint)
		{
			currentArrPoint = currentArrPoint + 6;		// '6' is the number of characters of "ciplus"
			if(*currentArrPoint == '=')
			{
				if(*(currentArrPoint + 1) == '*')
				{
					*ciplus = atoi(currentArrPoint + 2); // ���忡 ������ currentArrPoint + 2 ���� ������ ���ڰ� ���;� �ϴµ�, ���� �ٸ� ���ڰ� �Է� �Ǿ����� ��쿡�� atoi ���� 0���� ���� �ȴ�. ��, �����Ͱ� �߸� �Ǿ��ٰ� ���� CI Plus ������ ���� �ʴ� ������ ����.

					HxLOG_Print("[CIS] CI+ CAM detected !! (ciplus = %d)\n", *ciplus);
				}
				else if(*(currentArrPoint + 1) == '-')
				{
					// ������ ������� ����. ������ ���߿� �߰� �Ǿ�� �� ����.
				}
				else if(*(currentArrPoint + 1) == '+')
				{
					// ������ ������� ����. ������ ���߿� �߰� �Ǿ�� �� ����.
				}
				else if(atoi(currentArrPoint + 1) > 0) //atoi ���� ���� 0���� ũ�ٴ� ���� ciplus ���� ��� ���� ���� �ٴ� �ǹ�
				{
					*ciplus = atoi(currentArrPoint + 1);
					VK_Print("[CIS] CI+ CAM detected !! (ciplus = %d)\n", *ciplus);
				}
				else
				{
					HxLOG_Error("[CIS] CI+ CAM but doesn't meet CI+ spec - wrong value for 'ciplus=x'\n");
					return -1;
				}

			}
			else
			{
				HxLOG_Error("[CIS] CI+ CAM but doesn't meet CI+ spec - not found 'ciplus=x'\n");
				return -1;
			}
		}
		else
		{
			HxLOG_Error("[CIS] CI+ CAM but doesn't meet CI+ spec - not found 'ciplus'\n");
			return -1;
		}

		// check : "ciprof" token

		currentArrPoint = strstr(acInfoCmdStr, "ciprof");

		if(currentArrPoint)
		{
			currentArrPoint = currentArrPoint + 6;		// '6' is the number of characters of "ciprof"
			if(*currentArrPoint == '=')
			{
				if(atoi(currentArrPoint + 1) > 0) 		//atoi ���� ���� 0���� ũ�ٴ� ���� ciprof ���� ��� ���� ���� �ٴ� �ǹ�
				{
					*ciprof = atoi(currentArrPoint + 1);
					HxLOG_Print("[CIS] Additional features detected !! (ciprof = %d)\n", *ciprof);
				}
				else
				{
					HxLOG_Error("[CIS] CI+ CAM but doesn't meet CI+ spec - wrong value for 'ciprof=x'\n");
					return -1;
				}
			}
			else
			{
				HxLOG_Error("[CIS] CI+ CAM but doesn't meet CI+ spec - not found 'ciprof=x'\n");
				return -1;
			}
		}
		else
		{
			HxLOG_Print("[CIS] CI+ CAM has no 'ciprof=x'\n");
			return 0;
		}
	}

	else
	{
		HxLOG_Print("[CIS] CIv1 or CI+ CAM but doesn't meet CI+ spec - not found '$compatible'\n"); // CIv1 ķ�̶�� ����
	}

	return 0;
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
static int PHYS_Tuple(unsigned mtuple, unsigned char tuple, unsigned char **pcis, int *len, int *first, int *itpcc_radr, int *dvb, int *last, unsigned int *ciplus, unsigned int *ciprof)
{
	int ocis;						/* current offset in the CIS */
	int link;						/* tuple link */
	int found;						/* 1 if the current tuple is found, else 0 */
	unsigned char ctuple;			/* current tuple */
	int sublen; 					/* intermediate length */
	unsigned char *subpcis; 		/* pointer to subtuple */
	int nb, nbt0 = 0, nbt1 = 0, i;/* intermediate variables */
	int ctpce_number = 0;			/* Configuration Index Value */
	char ctpcc_sz;					/* tuple's elementary item */
	char lastentry; 				/* 1 if the last entry is found */
	char ctpcc_rasz;
	char ctpcc_rmsz;
	char cpce_pd;

	ocis  = 0;
	found = 0;

	do
	{

//		if (tuple == CISTPL_END)
//		{
//			if ((**pcis == CISTPL_END) && (*len == 1))
//			{
//				*len = 0;	/* no more tuples */
//				return 0;	/* good CISTPL_END tuple */
//			}
//		}

		ctuple	= **pcis;
		link	= *(*pcis+1);
		*pcis	+= 2;
		ocis	+= 2;

		HxLOG_Print("PHYS_Tuple : tuple=%02X, ctuple=%02X, link=%02X(%d), len=%d\n", tuple, ctuple, link, link, *len);

		if (tuple == ctuple)
		{
			found = 1;

			/* tuple found ; control it */

			switch (tuple) {
			case CISTPL_VERS_1 :
				/* Control */
				if (*(*pcis + TPLLV1_MAJOR) != TPLLV1_MAJOR_VAL)
				{
					HxLOG_Error("PHYS_Tuple : not found or wrong TPLLV1_MAJOR\n");
					return -1;	/* wrong TPLLV1_MAJOR */
				}
				/* Control */
				if (*(*pcis + TPLLV1_MINOR) != TPLLV1_MINOR_VAL)
				{
					HxLOG_Error("PHYS_Tuple : not found or wrong TPLLV1_MINOR\n");
					return -1;	/* wrong TPLLV1_MINOR */
				}
#if 1 /* #ifdef CONFIG_MW_CI_PLUS */ /* CI+ ���� ���� �ƴϴ��� CI+ ķ�� ���Ե� �� �ְ� �̿� ���� �ڵ鸵�� �ʿ��� ���� �����Ƿ� ���������� ����/ó���� �д� (���忡�� ���յ�) */
				{
					/* check TPLLV1_INFO's "Additional Product Information" includes "ciplus=1" with case insensitive parsing */

					char info_str[256]; // maybe max tuple data length is 254
					int info_len;

					#if defined(_CI_DEBUG_) || defined(_CI_PLUS_DEBUG_)
					HxLOG_Print("\n --- CISTPL_VERS_1 : link = %02X(%d) ---\n\n ", link, link);
					for (i = TPLLV1_INFO; i < link; i++)
					{
						if (*(*pcis + i) == CISTPL_NULL)
							HxLOG_Print("\n ");
						else if (*(*pcis + i) == CISTPL_END)
							HxLOG_Print("\n\n");
						else
							HxLOG_Print("%c", *(*pcis + i));
					}
					#endif

					info_len = link - TPLLV1_INFO; /* info_len �� 254 �� ���� ���� �����Ƿ� �̿� ���� ���� �ڵ鸵�� �� �ʿ䰡 ����... */
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

						PHYS_GetCiPlusInfoFromString(info_str, info_len, ciplus, ciprof);
					}
					else
					{
						HxLOG_Print("[CIS] CIv1 or CI+ CAM but doesn't meet CI+ spec - not found or wrong TPLLV1_INFO\n");
					}
				}
#endif // #ifdef CONFIG_MW_CI_PLUS
				break;

			case CISTPL_CONFIG :
				ctpcc_sz = *(*pcis + TPCC_SZ);
				ctpcc_rasz = ctpcc_sz & TPCC_RASZ;	/* number of bytes of Configuration Register Base Address */
				if (ctpcc_rasz > NBRBYTES)	/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
				{
					HxLOG_Error("PHYS_Tuple : wrong Configuration Register Base Address (ctpcc_rasz = 0x%x)\n", ctpcc_rasz);
					return -1;	/* wrong Configuration Register Base Address */
				}

				ctpcc_rmsz = ctpcc_sz & TPCC_RMSZ;
				*last  = *(*pcis + TPCC_LAST);	/* index number of last entry in Card configuration Table */
				if (ctpcc_rasz == 0)
					*itpcc_radr = *(*pcis + TPCC_RADR);
				else
					*itpcc_radr = *(*pcis + TPCC_RADR) + (*(*pcis + TPCC_RADR + 1) << 8);
				/* Control */
				if (*itpcc_radr > MAXADDR)
				{
					HxLOG_Error("PHYS_Tuple : wrong Configuration Register Base Address (*itpcc_radr = 0x%x)\n", *itpcc_radr);
					return -1;	/* wrong Configuration Register Base Address */
				}
				/* Control */
				if (! ((*(*pcis + TPCC_RADR + ctpcc_rasz + 1)) & TPCC_RMSK))
				{
					HxLOG_Error("PHYS_Tuple : COR not present\n");
					return -1;	/* COR not present */
				}

				nb = TPCC_RADR + ctpcc_rasz + 1 + ctpcc_rmsz + 1;
				subpcis = *pcis + nb;
				sublen = *len - ocis - (link - nb);

				/* control CCST_CIF subtuple */
				HxLOG_Print("PHYS_Tuple : CCST_CIF\n");
				if (PHYS_Tuple(tuple, CCST_CIF, &subpcis, &sublen, first, itpcc_radr, dvb, last, ciplus, ciprof) < 0)
				{
					HxLOG_Error("PHYS_Tuple : not found or wrong CCST_CIF\n");
					return -1;	/* Control not found or wrong CCST_CIF */
				}
				break;

			case CISTPL_CFTABLE_ENTRY :
				if (*first & !((*(*pcis + TPCE_INDX)) & TPCE_INDX_MASK))	/* Intface bit and Default bit must be present in the first entry */
				{
					HxLOG_Error("PHYS_Tuple : not found or wrong CISTPL_CFTABLE_ENTRY\n");
					return -1;	/* Control wrong first CISTPL_CFTABLE_ENTRY tuple */
				}

				ctpce_number = *(*pcis + TPCE_INDX) & ~TPCE_INDX_MASK;	/* configuration Entry Number to write to the COR */
//				if (ctpce_number == DVB_INDEX)
//					*dvb = DVB_INDEX;	/* the DVB entry is present */

				*first = 0;

				if (*(*pcis + TPCE_INDX) & TPCE_INTFACE)
				{
					/* Control : TPCE_IF must be equal to TPCE_IF_VAL */
					if (*(*pcis + TPCE_IF) != TPCE_IF_VAL)
					{
						HxLOG_Print("PHYS_Tuple : not found or wrong TPCE_IF_VAL\n"); // CISTPL_CFTABLE_ENTRY �� 2�� �̻��� ��� ��� �� ��Ȳ�� �߻��ϹǷ� PrintDbg �� ����Ʈ �Ѵ�.
						lastentry = (char)-2;	/* wrong CISTPL_CFTABLE_ENTRY tuple */
						break;
					}
				}
				else
				{
					HxLOG_Error("PHYS_Tuple : not found or wrong TPCE_IF\n");
					lastentry = (char)-2;	/* Control : wrong CISTPL_CFTABLE_ENTRY tuple */
					break;
				}

				/* Control : I/O Space and power must be present */
				if (!(*(*pcis + TPCE_FS) & TPCE_FS_MASK))
				{
					HxLOG_Error("PHYS_Tuple : not found or wrong TPCE_FS_MASK\n");
					lastentry = (char)-2;	/* wrong CISTPL_CFTABLE_ENTRY tuple */
					break;
				}

				nb = 0;

				if (*(*pcis + TPCE_FS) & TPCE_FS_POWER)
				{
					cpce_pd = *(*pcis + TPCE_PD);
					/* find the number of bytes coding the TPCE_PD (power) */
					for (i=0; i < 8; i++)
					{
						if (cpce_pd & TPCE_PD_MASK)	/* one set bit means one following byte */
							nb++;
						cpce_pd = cpce_pd >> 1;
					}
					/* skip the TPCE_PD ; if the bit 7 of one power byte is set then a additional byte is present */
					for (i=TPCE_PD + 1; i <= TPCE_PD + nb; i++)
					{
						if (*(*pcis + i) & TPCE_EXT)
							nb++;
					}	/* end of power configuration skipping */
					nb++;	/* count the Power Description Structure Parameter Selection Byte */
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
				HxLOG_Print("PHYS_Tuple : STCE_EV\n");
				if (PHYS_Tuple(tuple, STCE_EV, &subpcis, &sublen, first, itpcc_radr, dvb, last, ciplus, ciprof) < 0)
				{	/* Control */
					lastentry = (char)-2;	/* not found or wrong STCE_EV */
					break;
				}

				/* Control the STCE_PD subtuple */
				HxLOG_Print("PHYS_Tuple : STCE_PD\n");
				if (PHYS_Tuple(tuple, STCE_PD, &subpcis, &sublen, first, itpcc_radr, dvb, last, ciplus, ciprof) < 0)
				{	/* Control */
					lastentry = (char)-2;	/* not found or wrong STCE_PD */
					break;
				}

//				if (ctpce_number == DVB_INDEX)
//					*dvb = DVB_INDEX;	/* the DVB entry is present */
				*dvb = ctpce_number;	/* the DVB entry is present */

//				if (ctpce_number == *last)
//					lastentry = 1;	/* end of one entry analysis, it's the last */
//				else
//					lastentry = 0;	/* not the last entry */
				break;

			case CISTPL_NO_LINK :
				if (link != CISTPL_NOLINKLEN)	/* Control */
				{
					HxLOG_Error("PHYS_Tuple : not found or wrong CISTPL_NOLINKLEN\n");
					return -1;					/* ======= wrong CISTPL_NO_LINK tuple */
				}
				break;

			/* subtuples */
			case CCST_CIF : 		/* if mtuple = CISTPL_CFTABLE_ENTRY then this tuple is STCE_EV */
				if (mtuple == CISTPL_CFTABLE_ENTRY)
				{	/* STCE_EV */
					/* Control */
					if (VK_strcmp((char*)*pcis, STCE_EV_VAL))
					{
						HxLOG_Error("PHYS_Tuple : not found or wrong STCE_EV_VAL\n");
						return -1;	/* wrong STCE_EV subtuple */
					}
				}
				else
				{	/* CCST_CIF */
					/* Control */
//					if (link != CCST_CIFLEN)
//					{
HxLOG_Error("PHYS_Tuple : not found or wrong CCST_CIFLEN\n");
//						return -1;	/* wrong CCST_CIF subtuple */
//					}
					/* Control */
					if (**pcis != CCST_CIF1)
					{
						HxLOG_Error("PHYS_Tuple : not found or wrong CCST_CIF1\n");
						return -1;	/* wrong CCST_CIF subtuple */
					}
					/* Control */
					if (*(*pcis + 1) != CCST_CIF2)
					{
						HxLOG_Error("PHYS_Tuple : not found or wrong CCST_CIF2\n");
						return -1;	/* wrong CCST_CIF subtuple */
					}

					/* STCI_STR must be "DVB_CI_Vx.xx" where x is digit */
					nb = VK_strlen(STCI_STR);
					/* Control */
					if (VK_strncmp((char*)(*pcis + 2), STCI_STR, nb))
					{
						HxLOG_Error("PHYS_Tuple : not found or wrong STCI_STR\n");
						return -1;	/* wrong CCST_CIF subtuple */
					}
					/* Control */
					if (!isdigit(*(*pcis + 2 + nb)))
					{
						HxLOG_Error("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;	/* wrong CCST_CIF subtuple */
					}
					/* Control */
					if (*(*pcis + 2 + nb + 1) != STCI_STRP)
					{
						HxLOG_Error("PHYS_Tuple : not found or wrong STCI_STRP\n");
						return -1;	/* wrong CCST_CIF subtuple */
					}
					/* Control */
					if (!isdigit(*(*pcis + 2 + nb + 2)))
					{
						HxLOG_Error("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;	/* wrong CCST_CIF subtuple */
					}
					/* Control */
					if (!isdigit(*(*pcis + 2 + nb + 3)))
					{
						HxLOG_Error("PHYS_Tuple : not found or wrong CCST_CIF subtuple - %d line\n", __LINE__);
						return -1;	/* wrong CCST_CIF subtuple */
					}
				}
				break;

			case STCE_PD :
				/* Control */
				if (VK_strcmp((char*)(*pcis), STCE_PD_VAL))
				{
					HxLOG_Error("PHYS_Tuple : not found or wrong STCE_PD_VAL\n");
					return -1;	/* wrong STCE_PD subtuple */
				}
				break;

			/* end-of-list tuple */
			case CISTPL_END :
				return 0;	/* good CISTPL_END tuple */

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
			HxLOG_Error("PHYS_Tuple : tuple not found (ocis=%d, len=%d)\n", ocis, *len);
			return -1;	/* tuple not found */
		}
		else
		{
			HxLOG_Print("PHYS_Tuple : next tuple=%02X following %d bytes from ctuple=%02X\n", **pcis, ocis, ctuple);
		}

	} while (!found);

	if (tuple == CISTPL_CFTABLE_ENTRY)
	{
		if (ctpce_number == *last || **pcis == CISTPL_NO_LINK) // if (ctpce_number == *last)
		{
			lastentry = 1;	/* end of one entry analysis, it's the last */
		}
		else
		{
			lastentry = 0;	/* not the last entry */
		}
		return lastentry;
	}
	else
		return 1;
}

int PHYS_SetRS(unsigned short slot)
{
	int index;

	for (index = 0; index < PAL_MAX_CISLOT_NUMBER; index++)
	{
		if (DRVTab[index].Slot_id == slot)
		{
			if (DRVTab[index].State == DRV_CONNECT)
			{
				DRVTab[index].ucSetRS = 1;
				return 0;
			}
		}
	}

	return -1;
}

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
static int PHYS_GetAddrCcrs(int fd, unsigned short *addrCcrs)
{
	int i;
	int iRet;				/* return code						*/
	DRV_stMem mem;					/* driver parameters */
	unsigned char cis[8];
	unsigned char radrSize;

	mem.len    = 8;
	mem.pbytes = cis;

	for (i = DRV_MEMADDR; i < MAXADDR; )
	{
		mem.addr = i;
		iRet = svc_cas_DevCiIoctl(fd, DRV_READMEM, &mem);
		if (iRet < 0)
			return -1;		/* error while reading CIS */
		if (cis[0] == CISTPL_CONFIG)
		{
			radrSize = cis[2] & TPCC_RASZ;			/* number of bytes of Configuration Register Base Address */
			if (radrSize > NBRBYTES)				/* Control : number = 0 or 1 means 1 or 2 bytes --> address between 0 and 0xfff */
			{
//				HxLOG_Error("PHYS_GetAddrCcrs : radrSize=%x\n", radrSize);
				return -2;							/* wrong Configuration Register Base Address */
			}
			if (radrSize == 0)
				*addrCcrs = cis[4];
			else
				*addrCcrs = cis[4] | (cis[5] << 8);
			if (*addrCcrs > MAXADDR)				/* Control */
			{
//				HxLOG_Error("PHYS_GetAddrCcrs : *addrCcrs=%x\n", *addrCcrs);
				return -3;							/* wrong Configuration Register Base Address */
			}
			return 0;
		}
		else
		{
			if (cis[0] == 0xff || !cis[1] || cis[1] == 0xff)
				return -4;
			i += cis[1] + 2;
		}
	}

	return -5;
}
#endif

