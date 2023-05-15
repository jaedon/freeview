#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include "nexus_timebase.h"
#include "bstd_defs.h"
#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "di_err.h"
#include "di_demux.h"
#include "drv_demux.h"

#define HWTEST_CMD		szCmd
#define HWTEST_PARAM	szParam
#define HWTEST_PARAM1	szParam1
#define HWTEST_PARAM2	szParam2
#define GET_ARGS		int iResult=CMD_ERR; \
						char *HWTEST_CMD=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM1=CMD_NextArg((char **)&szArgs); \
						char *HWTEST_PARAM2=CMD_NextArg((char **)&szArgs);
#define CMD_IS(x)		(HWTEST_CMD!=NULL && VK_strcmp(HWTEST_CMD, x)==0)
#define PARAM_IS(x)		(HWTEST_PARAM!=NULL && VK_strcmp(HWTEST_PARAM, x)==0)
#define PARAM1_IS(x)	(HWTEST_PARAM1!=NULL && VK_strcmp(HWTEST_PARAM1, x)==0)
#define PARAM2_IS(x)	(HWTEST_PARAM2!=NULL && VK_strcmp(HWTEST_PARAM2, x)==0)
#define READABLE_IN_HEX(sz,val) CMD_ReadAsHex(sz, &val)
#define READABLE_IN_DEC(sz,val) CMD_ReadAsDec(sz, &val)

#define GET_NEXT_ARG(variable)		char *variable = CMD_NextArg((char **)&szArgs)

#define DATA_PRINT_LEN	10
#define TEST_DEMUX_ID	0
#define PSI_FILTER_TEST_PID 0
#define PES_FILTER_TEST_PID 0 //372
#define PES_TEST_REQID 1
#define PSI_TEST_REQID 2

#define TEST_FILTER_LEN 8
HUINT8 ucValue[TEST_FILTER_LEN];
HUINT8 ucMask[TEST_FILTER_LEN];
HUINT8 ucExclusion[TEST_FILTER_LEN];

/*---Sunwoo---------------------------*/
typedef struct
{
	HUINT8	stream_type;
	HUINT16	elementary_pid;
	HUINT16	es_info_length;
}ES_Info;

typedef struct
{
	HUINT16	pid;
	HUINT8	table_id;
	HUINT16	section_length;
	HUINT16	program_number;
	HUINT8	section_number;
	HUINT8	last_section_number;
	HUINT16	pcr_pid;
	HUINT16	program_info_length;
	int	es_num;
	HBOOL	show;

	ES_Info es_array[20];
}PMT_Info;

typedef struct
{
	HUINT8	table_id;
	HUINT16	section_length;
	HUINT16	transport_stream_id;
	HUINT8	section_number;
	HUINT8	last_section_number;
	HINT32	pmt_amount;
	HINT32	target;
	HINT32	demux_num;
	HINT32	tuner_num;
	HINT32	sectionReceived;
	HBOOL	show;

	PMT_Info pmt_array[20];
}PAT_Info;

typedef enum
{
	RIGHT12BITS,
	RIGHT13BITS,
	FULL2BYTES
}member_size;

static PAT_Info gPAT;

/*------------------------------------*/
HUINT16	P_hwtest_DEMUX_GetPartialValue(HUINT8 *pucBuf, HUINT32 location, member_size size)
{
	HUINT16 result = 0;
	HUINT8 first, second;

	VK_MEM_Memcpy(&first, &pucBuf[location], 1);
	VK_MEM_Memcpy(&second, &pucBuf[location+1], 1);

	if(size == RIGHT13BITS)
		result = (first*0x100 +second) & (0x1fff);
	else if (size == RIGHT12BITS)
		result = (first*0x100 + second) & (0x0fff);
	else if (size == FULL2BYTES)
		result = (first*0x100+second);

	return result;
}
DI_ERR_CODE	hwtest_CB_DEMUX_PES_Notify(HUINT32 ulDemuxId, HUINT8 *pucBuf,  HUINT32 ulRequestID)
{
	int i;

	BSTD_UNUSED(ulDemuxId);
	BSTD_UNUSED(ulRequestID);
	CMD_Printf("[%s] received: ",__FUNCTION__);
	for(i=0; i<DATA_PRINT_LEN; i++)
	{
		CMD_Printf("[%d]:0x%02x ",i,pucBuf[i]);
	}
	CMD_Printf("\n");
	return DI_ERR_OK;
}

DI_ERR_CODE	hwtest_CB_DEMUX_PSI_Notify(HUINT32 ulDemuxId, HUINT8 *pucBuf,  HUINT32 ulRequestId)
{
	int i;

	BSTD_UNUSED(ulDemuxId);
	BSTD_UNUSED(ulRequestId);

	VK_printf("========hwtest_CB_DEMUX_PSI_Notify\n");
	CMD_Printf("[%s] received: ",__FUNCTION__);
	for(i=0; i<DATA_PRINT_LEN; i++)
	{
		CMD_Printf("[%d]:0x%02x ",i,pucBuf[i]);
	}
	CMD_Printf("\n");




	return DI_ERR_OK;
}
static unsigned int pat_received_count = 0;
DI_ERR_CODE hwtest_CB_DEMUX_NotifyCallback(HUINT32 ulDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId)
{
	if ( pucBuf[0] == 0x00 )
	{
		VK_printf("PAT-%s(%d): TSID(0x%x%x)\n", __FUNCTION__, __LINE__, pucBuf[3],pucBuf[4]);
        if ( CMD_GetDemuxCallback() != NULL )
        {
        	pat_received_count++;
			if ( pat_received_count % 2 == 0 )
			{
            	(int *)(CMD_GetDemuxCallback())();
			}
        }
	}
	else
	{
		VK_printf("nonPAT-%s(%d)\n", __FUNCTION__, __LINE__);
	}
	DD_MEM_Free(pucBuf);
	return DI_ERR_OK;
}

DI_ERR_CODE hwtest_CB_DEMUX_Get_Pid(HUINT32 ulDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId)
{
	BSTD_UNUSED(ulDemuxId);
	BSTD_UNUSED(ulRequestId);
#define check_pat	1

	int i;

	/* PAT parsing */
	if(pucBuf[0] == 0x00)
	{
		/* table_id */
		gPAT.sectionReceived = 1;
		VK_MEM_Memcpy(&gPAT.table_id, &pucBuf[0], 1);
		/* section_length */
		gPAT.section_length = P_hwtest_DEMUX_GetPartialValue(pucBuf, 1, RIGHT12BITS);
		/* transport stream id */
		//VK_MEM_Memcpy(&gPAT.transport_stream_id, &pucBuf[3], 2);
		gPAT.transport_stream_id = P_hwtest_DEMUX_GetPartialValue(pucBuf, 3, FULL2BYTES);
		/* section number */
		VK_MEM_Memcpy(&gPAT.section_number, &pucBuf[6], 1);
		/* last section number */
		VK_MEM_Memcpy(&gPAT.last_section_number, &pucBuf[7], 1);
		/* pmt amount */
		gPAT.pmt_amount = (gPAT.section_length - 9)/4;

		/* pmt array */
		for(i=0; i<gPAT.pmt_amount; i++)
		{
			/* program number */
			//VK_MEM_Memcpy(&gPAT.pmt_array[i].program_number, &pucBuf[8+(4*i)], 2);
			gPAT.pmt_array[i].program_number = P_hwtest_DEMUX_GetPartialValue(pucBuf, 8+(4*i), FULL2BYTES);
			/* network pid || program map pid */
			gPAT.pmt_array[i].pid = P_hwtest_DEMUX_GetPartialValue(pucBuf, 10+(4*i), RIGHT13BITS);
		}

		if(gPAT.show == FALSE)
		{
			CMD_Printf("\n-------------------<PAT>-------------------\n");
			CMD_Printf("demux id		: %d\n", gPAT.demux_num);
			CMD_Printf("tuner id		: %d\n", gPAT.tuner_num);
			CMD_Printf("table_id		: 0x%02x\n", gPAT.table_id);
			CMD_Printf("section_length		: %dbytes\n", gPAT.section_length);
			CMD_Printf("transport stream id 	: 0x%04x\n", gPAT.transport_stream_id);
			CMD_Printf("section number		: %d\n", gPAT.section_number);
			CMD_Printf("last section number 	: %d\n\n", gPAT.last_section_number);
			for(i=0; i<gPAT.pmt_amount; i++)
			{
				CMD_Printf("PMT PID #%d: [%d] Program number[0x%04x]\n", i, gPAT.pmt_array[i].pid, gPAT.pmt_array[i].program_number);
			}
			CMD_Printf("\n");

			gPAT.show = TRUE;

		}
		return DI_ERR_OK;

	}
	else if (pucBuf[0] == 0x02)
	{
		int current;

		/* table id */
		VK_MEM_Memcpy(&gPAT.pmt_array[gPAT.target].table_id, &pucBuf[0], 1);
		/* section length */
		gPAT.pmt_array[gPAT.target].section_length = P_hwtest_DEMUX_GetPartialValue(pucBuf, 1, RIGHT12BITS);
		/* program number */
		//VK_MEM_Memcpy(&gPAT.pmt_array[gPAT.target].program_number, &pucBuf[3], 2);
		gPAT.pmt_array[gPAT.target].program_number = P_hwtest_DEMUX_GetPartialValue(pucBuf, 3, FULL2BYTES);
		/* section number */
		VK_MEM_Memcpy(&gPAT.pmt_array[gPAT.target].section_number, &pucBuf[6], 1);
		/* last section number */
		VK_MEM_Memcpy(&gPAT.pmt_array[gPAT.target].last_section_number, &pucBuf[7], 1);
		/* pcr pid */
		gPAT.pmt_array[gPAT.target].pcr_pid = P_hwtest_DEMUX_GetPartialValue(pucBuf, 8, RIGHT13BITS);
		/* program info length */
		gPAT.pmt_array[gPAT.target].program_info_length = P_hwtest_DEMUX_GetPartialValue(pucBuf, 10, RIGHT12BITS);

		/* stream type & elementary pid & es info length */
		current = 12+ gPAT.pmt_array[gPAT.target].program_info_length;
		gPAT.pmt_array[gPAT.target].es_num = 0;

		while(current < gPAT.pmt_array[gPAT.target].section_length-4)
		{
			/* stream type */
			VK_MEM_Memcpy(&gPAT.pmt_array[gPAT.target].es_array[gPAT.pmt_array[gPAT.target].es_num].stream_type,
							 &pucBuf[current], 1);
			current += 1;
			/* audio pid || video pid */
			gPAT.pmt_array[gPAT.target].es_array[gPAT.pmt_array[gPAT.target].es_num].elementary_pid =
										P_hwtest_DEMUX_GetPartialValue(pucBuf, current, RIGHT13BITS);
			current += 2;
			/* es info length */
			gPAT.pmt_array[gPAT.target].es_array[gPAT.pmt_array[gPAT.target].es_num].es_info_length =
										P_hwtest_DEMUX_GetPartialValue(pucBuf, current, RIGHT12BITS);
			current += 2;
			current += gPAT.pmt_array[gPAT.target].es_array[gPAT.pmt_array[gPAT.target].es_num].es_info_length;
			gPAT.pmt_array[gPAT.target].es_num++;
		}

		if(gPAT.pmt_array[gPAT.target].show == FALSE)
		{

			CMD_Printf("\n----------------<PMT %02dth>-----------------\n",gPAT.target);
			CMD_Printf("demux id		: %d\n", gPAT.demux_num);
			CMD_Printf("tuner id		: %d\n", gPAT.tuner_num);
			CMD_Printf("table_id		: 0x%02x\n", gPAT.pmt_array[gPAT.target].table_id);
			CMD_Printf("pid			: 0x%04x\n",gPAT.pmt_array[gPAT.target].pid);
			CMD_Printf("section_length		: %dbytes\n", gPAT.pmt_array[gPAT.target].section_length);
			CMD_Printf("program number		: 0x%04x\n", gPAT.pmt_array[gPAT.target].program_number);
			CMD_Printf("section number		: %d\n", gPAT.pmt_array[gPAT.target].section_number);
			CMD_Printf("last section number 	: %d\n", gPAT.pmt_array[gPAT.target].last_section_number);
			CMD_Printf("pcr pid 		: 0x%04x\n", gPAT.pmt_array[gPAT.target].pcr_pid);
			CMD_Printf("program info length 	: %02d\n\n", gPAT.pmt_array[gPAT.target].program_info_length);

			for(i = 0; i < gPAT.pmt_array[gPAT.target].es_num; i++)
			{
				CMD_Printf("Elementary PID #%d:[%d] Stream type[0x%02x]\n", i,
									gPAT.pmt_array[gPAT.target].es_array[i].elementary_pid,
									gPAT.pmt_array[gPAT.target].es_array[i].stream_type);
			}

			gPAT.pmt_array[gPAT.target].show = TRUE;
			DI_DEMUX_ReleaseSectionFilter(gPAT.demux_num, 2);
			DRV_DEMUX_BackupCallback(FALSE);
		}


		return DI_ERR_OK;

	}
	else
		CMD_Printf("This packet has first byte 0x%02x\n", pucBuf[0]);

	return DI_ERR_OK;
}

DI_ERR_CODE	hwtest_CB_DEMUX_AllocBuffer(HUINT32 usBufLen, HUINT8 **ppucBuf)
{
	HUINT8 *pBuf = NULL;

	pBuf = DD_MEM_Alloc(usBufLen);
	if(pBuf == NULL)
		return DI_ERR_ERROR;
	*ppucBuf = pBuf;
	return DI_ERR_OK;
}

DI_ERR_CODE	hwtest_CB_DEMUX_FreeBufferForError( HUINT8 *ppucBuf)
{
	if(ppucBuf != NULL)
		DD_MEM_Free(ppucBuf);
	return DI_ERR_OK;
}


static void hwtest_CB_DEMUX_PcrError(void *data, int unused)
{
	BSTD_UNUSED(data);
	BSTD_UNUSED(unused);
	CMD_Printf("PCR Error~~~~\n");
}


const char *ucSourceName[] =
{
	"None\0",
	"Current\0",
	"Tuner\0",
	"Recorder\0",
	"Recorder(Scrambled)\0",
	"Player\0",
	"Player(Scrambled)\0",
	"MAX\0"
};

const char *ucDestName[] =
{
	"None\0",
	"Current\0",
	"Recorder\0",
	"Decoder\0",
	"External\0",
	"MAX\0"
};

unsigned char PAT_CANDI[188] = {
	0x47, 0x40, 0x00, 0x15, 0x00, 0x00, 0xB0, 0x25, 0x04, 0x1B, 0xC7, 0x00, 0x00, 0x00, 0x00, 0xE0,
	0x10, 0x70, 0x31, 0xE0, 0x64, 0x70, 0x32, 0xE0, 0xC8, 0x70, 0x33, 0xE1, 0x2C, 0x70, 0x34, 0xE1,
	0x90, 0x70, 0x35, 0xE1, 0xF4, 0x70, 0x36, 0xE2, 0x58, 0x71, 0x6F, 0x10, 0x11, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

HINT32 psi_reqid_idx=0;
HUINT32 pktsubHandle;
HUINT32 pidchannel;
HUINT32 tscHandle = 0xFFFFFFFF;

#if defined(CONFIG_DI10)
extern DI_ERR_CODE DI_DEMUX_ReplacePacketModeOn(HUINT32 , HUINT16 , HUINT8 *, HUINT32 , HUINT32 *);
extern DI_ERR_CODE DI_DEMUX_ReplacePacketModeOff(HUINT32 );
#endif
int CMD_HwTest_Parser_Demux(void *szArgs)
{
	GET_ARGS;
	BSTD_UNUSED(HWTEST_PARAM2);
	DI_ERR_CODE diErr = DI_ERR_OK;

	// Slow Blanking : 12V(4:3), 6V(16:9), 0V(Int. TV)
	if CMD_IS("test")
	{
		if PARAM_IS("regcb")
		{
			//register callbacks for PES filtering
			diErr = DI_DEMUX_RegisterPESCallback(TEST_DEMUX_ID, DI_DEMUX_PESTYPE_SUBTITLE, hwtest_CB_DEMUX_PES_Notify,hwtest_CB_DEMUX_AllocBuffer,hwtest_CB_DEMUX_FreeBufferForError);
			if(diErr != DI_ERR_OK)
			{
				iResult=CMD_ERR;
				goto TEST_DEMUX_EXIT;
			}
			diErr = DI_DEMUX_RegisterSectionCallback(TEST_DEMUX_ID,hwtest_CB_DEMUX_PSI_Notify,hwtest_CB_DEMUX_AllocBuffer);
			if(diErr != DI_ERR_OK)
			{
				iResult=CMD_ERR;
				goto TEST_DEMUX_EXIT;
			}
			iResult = CMD_OK;
		}
		else if PARAM_IS("pesfilter")
		{
			if PARAM1_IS("start")
			{
				VK_MEM_Memset(ucValue,0x0,TEST_FILTER_LEN);
				VK_MEM_Memset(ucMask,0x0,TEST_FILTER_LEN);
				VK_MEM_Memset(ucExclusion,0x0,TEST_FILTER_LEN);
				ucMask[0]=0xFF;
				CMD_Printf("Start PES-filtering test\r\n");
				DI_DEMUX_StartPESFilter(TEST_DEMUX_ID, PES_FILTER_TEST_PID, DI_DEMUX_PESTYPE_SUBTITLE, PES_TEST_REQID);
				iResult=CMD_OK;
			}
			else if PARAM1_IS("stop")
			{
				CMD_Printf("Stop PES-filtering test\r\n");
				DI_DEMUX_StopPESFilter(TEST_DEMUX_ID, PES_TEST_REQID);
				iResult=CMD_OK;
			}
		}
		else if (PARAM_IS("psifilter") || PARAM_IS("sectionfilter"))
		{
			CMD_Printf("Start PSI(Section)-filtering test\r\n");
			if PARAM1_IS("start")
			{
				VK_MEM_Memset(ucValue,0x0,TEST_FILTER_LEN);
				VK_MEM_Memset(ucMask,0x0,TEST_FILTER_LEN);
				VK_MEM_Memset(ucExclusion,0x0,TEST_FILTER_LEN);
				ucMask[0]=0xFF;
				/*ucMask[1]=0xFF;
				ucMask[2]=0xFF;
				ucValue[0]= 0x4e;
				ucValue[1]= 0x18;
				ucValue[2]= 0x9e;*/
				CMD_Printf("Start PSI(Section)-filtering test\r\n");
				DI_DEMUX_SetSectionFilter(TEST_DEMUX_ID, PSI_FILTER_TEST_PID, PSI_TEST_REQID+psi_reqid_idx++, FALSE, TEST_FILTER_LEN
					                                      , ucValue, ucMask, ucExclusion, FALSE);
				iResult=CMD_OK;
			}
			else if PARAM1_IS("stop")
			{
				CMD_Printf("Stop PSI(Section)-filtering test\r\n");
				psi_reqid_idx--;
				if(psi_reqid_idx >= 0)
					DI_DEMUX_ReleaseSectionFilter(TEST_DEMUX_ID, PSI_TEST_REQID+psi_reqid_idx);
				else
					CMD_Printf("No more psi filters to release. (%d)\n",psi_reqid_idx);
				iResult=CMD_OK;
			}
			iResult=CMD_OK;
		}
		else if (PARAM_IS("tsc"))
		{
			if PARAM1_IS("start")
			{
				HUINT32 tscDemuxId;
				HUINT32 tscPid;
				GET_NEXT_ARG(sztscPid);
				if(HWTEST_PARAM2 == NULL || sztscPid == NULL)
				{
					CMD_Printf("[Usage] demux test tsc start [demuxid(hex)] [pid(hex)]\n");
					iResult = CMD_OK;
				}
				else
				{
					if ( (READABLE_IN_HEX(HWTEST_PARAM2, tscDemuxId) != 1 ) ||
                         (READABLE_IN_HEX(sztscPid,tscPid) != 1 ))
                    {
                        return 1;
                    }
					DI_DEMUX_StartTSCMonitor(tscDemuxId, (HUINT16)tscPid, &tscHandle);
					iResult=CMD_OK;
				}
			}
			else if PARAM1_IS("stop")
			{
				if(tscHandle != 0xFFFFFFFF)
				{
					DI_DEMUX_StopTSCMonitor(tscHandle);
					tscHandle = 0xFFFFFFFF;
				}
				iResult=CMD_OK;
			}
			else if PARAM1_IS("get")
			{
				DI_DEMUX_TsState_t peTsState;
				if(tscHandle != 0xFFFFFFFF)
				{
					DI_DEMUX_GetTSCStatus(tscHandle, &peTsState);
					CMD_Printf("TSC Status = %ld\n", peTsState);
				}
				iResult=CMD_OK;
			}
			iResult=CMD_OK;
		}
		else if PARAM_IS("rmxfeedback")
		{
			CMD_Printf("Route to internal remux-feedback path.\n");
			DRV_DEMUX_RouteToInternalRmxFeedback(0);
			iResult=CMD_OK;
		}
	}
	else if CMD_IS("sync")
	{
		int inputband_idx = 0;
		unsigned int SyncCount;
        if ( READABLE_IN_DEC(HWTEST_PARAM, inputband_idx) != 1)
        {
            return 1;
        }
		SyncCount = DRV_DEMUX_GetSyncCount(inputband_idx);
		CMD_Printf("Inputband %d's SyncVal = %ld\n", inputband_idx, SyncCount);
		iResult = CMD_OK;
	}
	else if CMD_IS("dsc")
	{
		if PARAM_IS("on")
		{
			DRV_DEMUX_ConnectPIDChannelToDSC(0,0,&pidchannel);
		}
		else if PARAM_IS("off")
		{
			DRV_DEMUX_DisconnectPIDChannelFromDSC(pidchannel);
		}
		else
		{
			CMD_Printf("Unknown command.\n");
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("psub")
	{
		if PARAM_IS("on")
		{
#if defined(CONFIG_DI10)
			DI_DEMUX_ReplacePacketModeOn(1,0,PAT_CANDI,188,&pktsubHandle);
#else
			DI_DEMUX_ReplacePacketModeOn(1,0, 0, PAT_CANDI,188,&pktsubHandle);
#endif

		}
		else if PARAM_IS("off")
		{
			DI_DEMUX_ReplacePacketModeOff(pktsubHandle);
		}
		else
		{
			CMD_Printf("Unknown command.\n");
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("pathinfo")
	{
		DI_DEMUX_INPUTDEVICETYPE etProducer;
		HUINT32 ulSourceId;
		DI_DEMUX_OUTPUTDEVICETYPE etConsumer;
		HUINT32  ulConsumerId;
		HINT32 nCI_Id;
		HUINT32 ulCiUsageCount;
		HBOOL bPassThrough;
		unsigned int dmxCount;
		unsigned int i;

		DI_DEMUX_GetNumOfDemux(&dmxCount);
		CMD_Printf("==========<DEMUX PathInfo>================\n");
		for(i=0;i<dmxCount;i++)
		{
			diErr = DI_DEMUX_GetPath(i, &etProducer, &ulSourceId, &etConsumer, &ulConsumerId, &nCI_Id);
			if(diErr == DI_ERR_OK)
			{
				CMD_Printf("[Path%d] ", i);
				CMD_Printf("[%s%d]-->",ucSourceName[etProducer], ulSourceId);
				if(nCI_Id > -1)
				{
					CMD_Printf("[CAM%d]",nCI_Id);
					DRV_DEMUX_GetCAMBypassStatus((unsigned short)nCI_Id, &ulCiUsageCount, &bPassThrough);
					if(bPassThrough == FALSE)
					{
						CMD_Printf("(Bypass)-->");
					}
					else
					{
						CMD_Printf("(Passthrough)-->");
					}
				}
				CMD_Printf("[DEMUX%d]-->",i);
				CMD_Printf("[%s%d]\n",ucDestName[etConsumer],ulConsumerId);

			}
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("pat")
	{//Sunwoo
		int demux_num;
		int tuner_num;
		VK_printf("demux pat test ==================\n");
		if(szParam == NULL ||szParam1 == NULL)
		{
			CMD_Printf("Choose demux [0~5] and tuner [0~1]\n");
			return DI_ERR_OK;
		}
		demux_num =  VK_atoi((char *)szParam);
		tuner_num = VK_atoi((char *)szParam1);

		/* 1. Set on the flag for showing the PAT contents */
		if(demux_num > 5 || demux_num < 0)
		{
			CMD_Printf("Out of range, Choose a correct demux agian [0~5]\n");
			return DI_ERR_OK;
		}
		else
		{
			if(gPAT.demux_num != demux_num)
				gPAT.demux_num = demux_num;

			gPAT.pmt_array[gPAT.target].show = TRUE;
			gPAT.show = FALSE;
		}

		if(tuner_num > 1 || tuner_num < 0)
		{
			CMD_Printf("Out of range, Choose a correct tuner again [0~1]\n");
			return DI_ERR_OK;
		}
		else
			gPAT.tuner_num = tuner_num;


		/* 2. Remove the formal path  */
		if(DRV_DEMUX_IsConnected(demux_num) == TRUE)
		{
			CMD_Printf("%dth demux was being used, now it's disconnected & re-connected to %dth tuner\n",demux_num, tuner_num);
			diErr = DI_DEMUX_ReleasePath(demux_num);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("DI_DEMUX_ReleasePath failed!\n");
				//return DI_ERR_ERROR;
			}
		}

		/* 3. Set the new path */
		diErr = DI_DEMUX_SetPath(demux_num, DI_DEMUX_INPUT_TUNER, tuner_num, DI_DEMUX_OUTPUT_DECODER, 0, -1);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("DI_DEMUX_SetPath failed!\n");
			return DI_ERR_ERROR;
		}

		/* 4. Backup the original callback function and register the new callback */
		DRV_DEMUX_BackupCallback(TRUE);

		diErr = DI_DEMUX_RegisterSectionCallback(demux_num,hwtest_CB_DEMUX_Get_Pid, hwtest_CB_DEMUX_AllocBuffer);
		if(diErr != DI_ERR_OK)
		{
			iResult=CMD_ERR;
			goto TEST_DEMUX_EXIT;
		}

		VK_MEM_Memset(ucValue,0x0,TEST_FILTER_LEN);
		VK_MEM_Memset(ucMask,0x0,TEST_FILTER_LEN);
		VK_MEM_Memset(ucExclusion,0x0,TEST_FILTER_LEN);

		ucMask[0] = 0xFF;

		/* 5. Set up the filter */
		gPAT.sectionReceived = 0;
		DI_DEMUX_SetSectionFilter(demux_num, PSI_FILTER_TEST_PID, PSI_TEST_REQID, FALSE, 15, ucValue, ucMask, ucExclusion, FALSE);

		while(gPAT.sectionReceived == 0)
		{
			VK_printf("Waiting data....\n");
			VK_TASK_Sleep(1000);
		}
		gPAT.sectionReceived = 0;
		DI_DEMUX_ReleaseSectionFilter(gPAT.demux_num, 2);
		DRV_DEMUX_BackupCallback(FALSE);

		iResult=CMD_OK;

	}
	else if CMD_IS("paton")
	{//Sunwoo
		int demux_num;
		int tuner_num;
		VK_printf("demux pat test ==================\n");
		if(szParam == NULL ||szParam1 == NULL)
		{
			CMD_Printf("Choose demux [0~5] and tuner [0~1]\n");
			return DI_ERR_OK;
		}
		demux_num =  VK_atoi((char *)szParam);
		tuner_num = VK_atoi((char *)szParam1);

		/* 1. Set on the flag for showing the PAT contents */
		if(demux_num > 5 || demux_num < 0)
		{
			CMD_Printf("Out of range, Choose a correct demux agian [0~5]\n");
			return DI_ERR_OK;
		}
		else
		{
			if(gPAT.demux_num != demux_num)
				gPAT.demux_num = demux_num;

			gPAT.pmt_array[gPAT.target].show = TRUE;
			gPAT.show = FALSE;
		}

		if(tuner_num > 1 || tuner_num < 0)
		{
			CMD_Printf("Out of range, Choose a correct tuner again [0~1]\n");
			return DI_ERR_OK;
		}
		else
			gPAT.tuner_num = tuner_num;


		/* 2. Remove the formal path  */
		if(DRV_DEMUX_IsConnected(demux_num) == TRUE)
		{
			CMD_Printf("%dth demux was being used, now it's disconnected & re-connected to %dth tuner\n",demux_num, tuner_num);
			diErr = DI_DEMUX_ReleasePath(demux_num);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("DI_DEMUX_ReleasePath failed!\n");
				//return DI_ERR_ERROR;
			}
		}

		/* 3. Set the new path */
		diErr = DI_DEMUX_SetPath(demux_num, DI_DEMUX_INPUT_TUNER, tuner_num, DI_DEMUX_OUTPUT_DECODER, 0, -1);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("DI_DEMUX_SetPath failed!\n");
			return DI_ERR_ERROR;
		}

		/* 4. Backup the original callback function and register the new callback */
		DRV_DEMUX_BackupCallback(TRUE);

		diErr = DI_DEMUX_RegisterSectionCallback(demux_num,hwtest_CB_DEMUX_NotifyCallback, hwtest_CB_DEMUX_AllocBuffer);
		if(diErr != DI_ERR_OK)
		{
			iResult=CMD_ERR;
			goto TEST_DEMUX_EXIT;
		}

		VK_MEM_Memset(ucValue,0x0,TEST_FILTER_LEN);
		VK_MEM_Memset(ucMask,0x0,TEST_FILTER_LEN);
		VK_MEM_Memset(ucExclusion,0x0,TEST_FILTER_LEN);

		ucMask[0] = 0xFF;

		/* 5. Set up the filter */
		DI_DEMUX_SetSectionFilter(demux_num, PSI_FILTER_TEST_PID, PSI_TEST_REQID, FALSE, 15, ucValue, ucMask, ucExclusion, FALSE);

		iResult=CMD_OK;

	}
	else if CMD_IS("pathoff")
	{//Sunwoo
		int demux_num;
		int tuner_num;
		VK_printf("demux patoff test ==================\n");
		if(szParam == NULL ||szParam1 == NULL)
		{
			CMD_Printf("Choose demux [0~5] and tuner [0~1]\n");
			return DI_ERR_OK;
		}
		demux_num =  VK_atoi((char *)szParam);
		tuner_num = VK_atoi((char *)szParam1);

		diErr = DI_DEMUX_SetPath(demux_num, DI_DEMUX_INPUT_NONE, tuner_num, DI_DEMUX_OUTPUT_DECODER, 0, -1);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("DI_DEMUX_SetPath failed!\n");
			return DI_ERR_ERROR;
		}
		iResult=CMD_OK;
	}
	else if CMD_IS("patoff")
	{//Sunwoo
		int demux_num;
		int tuner_num;
		VK_printf("demux patoff test ==================\n");
		if(szParam == NULL ||szParam1 == NULL)
		{
			CMD_Printf("Choose demux [0~5] and tuner [0~1]\n");
			return DI_ERR_OK;
		}
		demux_num =  VK_atoi((char *)szParam);
		tuner_num = VK_atoi((char *)szParam1);

		/* 1. Set on the flag for showing the PAT contents */
		if(demux_num > 5 || demux_num < 0)
		{
			CMD_Printf("Out of range, Choose a correct demux agian [0~5]\n");
			return DI_ERR_OK;
		}
		else
		{
			if(gPAT.demux_num != demux_num)
				gPAT.demux_num = demux_num;

			gPAT.pmt_array[gPAT.target].show = TRUE;
			gPAT.show = FALSE;
		}

		if(tuner_num > 1 || tuner_num < 0)
		{
			CMD_Printf("Out of range, Choose a correct tuner again [0~1]\n");
			return DI_ERR_OK;
		}
		else
			gPAT.tuner_num = tuner_num;

		DI_DEMUX_ReleaseSectionFilter(demux_num, PSI_TEST_REQID);
		DI_DEMUX_ReleasePath(demux_num);
		iResult=CMD_OK;

	}
	else if CMD_IS("pmt")
	{//Sunwoo
		int demux_num;
		int tuner_num;
		int target_pid;
		int i;

		if(szParam == NULL || szParam1 == NULL || szParam2 == NULL)
		{
			CMD_Printf("Choose demux [0~5], tuner [0~1] and program [1~%d]\n",gPAT.pmt_amount-1);
			return DI_ERR_OK;
		}
		demux_num = VK_atoi((char *)szParam);
		tuner_num = VK_atoi((char *)szParam1);
		target_pid = VK_atoi((char *)szParam2);

		/* 1. Search for the target pmt packet pid */
		for(i = 0; i<gPAT.pmt_amount; i++)
		{
			if(gPAT.pmt_array[i].pid == target_pid)
				gPAT.target = i;
		}

		/* 2. Set on the flag for showing PMT contents */
		if(gPAT.demux_num != demux_num)
		{
			gPAT.demux_num = demux_num;
			gPAT.tuner_num = tuner_num;
		}
		gPAT.pmt_array[gPAT.target].show = FALSE;
		gPAT.show = TRUE;

		/* 3. Remove the formal path */
		if(DRV_DEMUX_IsConnected(demux_num) == TRUE)
		{
			CMD_Printf("%dth demux was being used, now it's disconnected & re-connected to %dth tuner\n",demux_num, tuner_num);
			diErr = DI_DEMUX_ReleasePath(demux_num);
			if(diErr != DI_ERR_OK)
			{
				CMD_Printf("DI_DEMUX_ReleasePath failed!\n");
				//return DI_ERR_ERROR;
			}
		}

		/* 4. Set the new path */
		diErr = DI_DEMUX_SetPath(demux_num, DI_DEMUX_INPUT_TUNER, tuner_num, DI_DEMUX_OUTPUT_DECODER, 0, -1);
		if(diErr != DI_ERR_OK)
		{
			CMD_Printf("DI_DEMUX_SetPath failed!\n");
			return DI_ERR_ERROR;
		}

		/* 5. Save the original callback function & register the new callback function */
		DRV_DEMUX_BackupCallback(TRUE);
		diErr = DI_DEMUX_RegisterSectionCallback(demux_num,hwtest_CB_DEMUX_Get_Pid, hwtest_CB_DEMUX_AllocBuffer);
		if(diErr != DI_ERR_OK)
		{
			iResult=CMD_ERR;
			goto TEST_DEMUX_EXIT;
		}

		VK_MEM_Memset(ucValue,0x0,TEST_FILTER_LEN);
		VK_MEM_Memset(ucMask,0x0,TEST_FILTER_LEN);
		VK_MEM_Memset(ucExclusion,0x0,TEST_FILTER_LEN);
		ucValue[0] = 0x2;
		ucMask[0] = 0xFF;

		/* 6. Set the section filter */
		diErr = DI_DEMUX_SetSectionFilter(demux_num, gPAT.pmt_array[gPAT.target].pid, 2, FALSE, 15, ucValue, ucMask, ucExclusion, FALSE);
		if(diErr == DI_ERR_ERROR)
			CMD_Printf("DI_DEMUX_SetSectionFilter failed!\n");

		iResult=CMD_OK;
	}
	else if CMD_IS("info")
	{//Sunwoo
		DRV_Error dResult;
		HUINT32 demux_id;

		if(szParam == NULL)
		{
			CMD_Printf("Choose a demux id [0~6]\n");
			return CMD_OK;
		}
		else
			demux_id = VK_atoi((char *)szParam);
		if(demux_id>6)
		{
			CMD_Printf("Out of range, choose a right demux [0~6]\n");
			return CMD_OK;
		}

		dResult = DRV_DEMUX_PrintInfo(demux_id);
		if(dResult != DRV_OK)
		{
			CMD_Printf("DRV_DEMUX_PrintInfo failed!\n");
			return CMD_ERR;
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("filterinfo")
	{
		HUINT32 demux_id;
		if(szParam == NULL)
		{
			CMD_Printf("Choose a demux id [0~6]\n");
			return CMD_OK;
		}
		else
			demux_id = VK_atoi((char *)szParam);
		if(demux_id>6)
		{
			CMD_Printf("Out of range, choose a right demux [0~6]\n");
			return CMD_OK;
		}

		DRV_DEMUX_PrintFilterInfo(demux_id);
		iResult = CMD_OK;
	}
	else if CMD_IS("logon")
	{
		if PARAM_IS("pat")
		{
			DRV_DEMUX_SetSectionDbgMode(0,1);
		}
		else if PARAM_IS("pmt")
		{
			DRV_DEMUX_SetSectionDbgMode(1,1);
		}
		else if PARAM_IS("sdt")
		{
			DRV_DEMUX_SetSectionDbgMode(2,1);
		}
		else if PARAM_IS("eit")
		{
			DRV_DEMUX_SetSectionDbgMode(3,1);
		}
		else if PARAM_IS("avr")
		{
			DRV_DEMUX_SetSectionDbgMode(30,1);
		}
		else if PARAM_IS("alloc")
		{
			DRV_DEMUX_SetSectionDbgMode(31,1);
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("logoff")
	{
		if PARAM_IS("pat")
		{
			DRV_DEMUX_SetSectionDbgMode(0,0);
		}
		else if PARAM_IS("pmt")
		{
			DRV_DEMUX_SetSectionDbgMode(1,0);
		}
		else if PARAM_IS("sdt")
		{
			DRV_DEMUX_SetSectionDbgMode(2,0);
		}
		else if PARAM_IS("eit")
		{
			DRV_DEMUX_SetSectionDbgMode(3,0);
		}
		else if PARAM_IS("avr")
		{
			DRV_DEMUX_SetSectionDbgMode(30,0);
		}
		else if PARAM_IS("alloc")
		{
			DRV_DEMUX_SetSectionDbgMode(31,0);
		}
		iResult = CMD_OK;
	}
	else if CMD_IS("secdbg")
	{
		DRV_DEMUX_DebugSectionFilter();
		iResult = CMD_OK;
	}
#if defined(CONFIG_TS_MATRIX)
	else if CMD_IS("tsmatrix")
	{
		int tMCount;
		HUINT32 tsMatrixInfo[6];
		extern void DRV_GetTsMatrix(HUINT32 *);
		DRV_GetTsMatrix(tsMatrixInfo);
		for(tMCount=0; tMCount <6; tMCount++)
		{
			CMD_Printf("[4to1_Switch_%d] 0x%x\n", tMCount, tsMatrixInfo[tMCount]);
		}
		iResult = CMD_OK;
	}
#endif
	else if CMD_IS("setpath")
	{
		HINT32 nCI_Id;
		HUINT32 ulTunerId;

		ulTunerId =  VK_atoi((char *)szParam);
		nCI_Id = VK_atoi((char *)szParam1);

		DI_DEMUX_SetPath(0,2, ulTunerId,3, 0,nCI_Id);

		iResult = CMD_OK;
	}
	else if CMD_IS("checkstc")
	{
		HUINT32 ulMaxPcrError = 0xFF;
		NEXUS_TimebaseSettings timebaseSettings;
		NEXUS_StcChannelHandle stcChannel;
		NEXUS_StcChannelSettings stcSettings;

		if(szParam != NULL)
		{
			ulMaxPcrError =  VK_atoi((char *)szParam);
		}

		CMD_Printf(" Input %d  %fms \n",ulMaxPcrError,(ulMaxPcrError/90000.0)*1000);
		stcChannel = DRV_DEMUX_GetStcChannelHandle();
		NEXUS_StcChannel_GetSettings(stcChannel,&stcSettings);
		(void)NEXUS_Timebase_GetSettings(stcSettings.timebase, &timebaseSettings);
		timebaseSettings.sourceType 						= NEXUS_TimebaseSourceType_ePcr;
		timebaseSettings.pcrErrorCallback.callback = hwtest_CB_DEMUX_PcrError;
		timebaseSettings.sourceSettings.pcr.pidChannel = stcSettings.modeSettings.pcr.pidChannel;
		timebaseSettings.sourceSettings.pcr.maxPcrError 	= ulMaxPcrError;	/* in milliseconds: based on 90Khz clock */
		(void)NEXUS_Timebase_SetSettings(stcSettings.timebase, &timebaseSettings);

		stcSettings.modeSettings.pcr.maxPcrError = ulMaxPcrError;
		NEXUS_StcChannel_SetSettings(stcChannel, &stcSettings);
		iResult = CMD_OK;
	}

TEST_DEMUX_EXIT:
	return iResult;
}
