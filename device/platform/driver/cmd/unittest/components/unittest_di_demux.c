/**
 * unittest_di_demux.c
*/

/**
 * @author		Kevin Woo
*/


/*******************************************************************/
/**************************** Header Files *************************/
/*******************************************************************/
#include "linden_trace.h"
#include "unittest.h"

#include "di_uart.h"
#include "di_err.h"
#include "di_demux.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#define DEBUG_MODULE        TRACE_MODULE_DI_DEMUX

#define TEST_DEMUX_ID	0
#define PSI_FILTER_TEST_PID 0
#define PES_FILTER_TEST_PID 41 //372
#define PES_TEST_REQID 1
#define PSI_TEST_REQID 2

#define TEST_FILTER_LEN 8

#define NUM_OF_RESERVED_CHANNEL 2

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/*******************************************************************/
/************************ global variables *************************/
/*******************************************************************/


HUINT8 ucValue[TEST_FILTER_LEN];
HUINT8 ucMask[TEST_FILTER_LEN];
HUINT8 ucExclusion[TEST_FILTER_LEN];

static HUINT32 gPatCount = 0; //PAT parsing
static HUINT32 gPESCount = 0;

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/


/*******************************************************************/
/*********************** Function Prototypes ***********************/
/*******************************************************************/

static DI_ERR_CODE ut_di_demux_getSectionLength(HUINT8 *pucSectionBuf, HUINT32 *ulSectionLen)
{
	/*Section Length : 13 - 24 bins in Section Table */
	*ulSectionLen = (((pucSectionBuf[1]&0xF)<<8)|(pucSectionBuf[2]))+3;
	return DI_ERR_OK;
}

static DI_ERR_CODE ut_di_demux_getPESLength(HUINT8 *pucPESBuf, HUINT32 *ulPESLen)
{
	/*PES_packet_length : 33th - 48th bin in PES_packet*/
	*ulPESLen = (((pucPESBuf[4])<<8)|(pucPESBuf[5]))+6;
	return DI_ERR_OK;
}

static DI_ERR_CODE ut_di_demux_getSizeAligned(HUINT32 ulSizeNotAligned, HUINT32 *ulSizeAligned)
{
	if(ulSizeNotAligned%4)
	{
		ulSizeNotAligned += 4- (ulSizeNotAligned%4);
	}
	*ulSizeAligned = ulSizeNotAligned;
	return DI_ERR_OK;
}

static DI_ERR_CODE ut_cb_demux_PSI_Notify(HUINT32 ulDemuxId, HUINT8 *pucBuf, HUINT32 ulRequestId)
{
	DI_ERR_CODE ret = DI_ERR_ERROR;
	DI_ERR_CODE result;
	HUINT32 ulSectionLen = 0;
	HUINT32 ulSectionLenAligned = 0;

	DONE_ERR(NULL == pucBuf);
	DONE_ERR(PSI_TEST_REQID != ulRequestId);

	if ( pucBuf[0] == 0x00 )
	{
		/*PAT recieved */
		DI_UART_Print("PAT-%s(%d): TSID(0x%x%x)\n", __FUNCTION__, __LINE__, pucBuf[3],pucBuf[4]);
        gPatCount++;

		/*PAT dump*/
		result = ut_di_demux_getSectionLength(pucBuf, &ulSectionLen);
		result = ut_di_demux_getSizeAligned(ulSectionLen, &ulSectionLenAligned);
		DONE_ERR(DI_ERR_OK != result);

		DI_UART_Dump(pucBuf, ulSectionLenAligned, 16);
	}
	else
	{
		/* not PAT*/
	}

	ret = DI_ERR_OK;
done:
	DD_MEM_Free(pucBuf);
	return ret;
}

DI_ERR_CODE	ut_cb_demux_PES_Notify(HUINT32 ulDemuxId, HUINT8 *pucBuf,  HUINT32 ulRequestID)
{
	DI_ERR_CODE ret = DI_ERR_ERROR;
	DI_ERR_CODE result;
	HUINT32 packetSize;

	DONE_ERR(NULL == pucBuf);
	DONE_ERR(PES_TEST_REQID != ulRequestID);

	/*PES packet received*/
	gPESCount++;
	result = ut_di_demux_getPESLength(pucBuf, &packetSize);
	result = ut_di_demux_getSizeAligned(packetSize, &packetSize);
	DONE_ERR(result != DI_ERR_OK);

	DI_UART_Dump(pucBuf, packetSize, 16);

	ret = DI_ERR_OK;
done:
	DD_MEM_Free(pucBuf);
	return ret;
}

static DI_ERR_CODE ut_cb_demux_AllocBuffer(HUINT32 usBufLen, HUINT8 **ppucBuf)
{
	HUINT8 *pBuf = NULL;

	pBuf = DD_MEM_Alloc(usBufLen);
	if(pBuf == NULL)
		return DI_ERR_ERROR;
	*ppucBuf = pBuf;
	return DI_ERR_OK;
}

static DI_ERR_CODE	ut_cb_demux_FreeBufferForError( HUINT8 *ppucBuf)
{
	if(ppucBuf != NULL)
		DD_MEM_Free(ppucBuf);
	return DI_ERR_OK;
}

static UT_RESULT ut_getCapability(void)
{
	DI_ERR_CODE ret = DI_ERR_ERROR;
	DI_ERR_CODE result;
	DI_DEMUX_CAPABILITY			stCapability;
	DI_DEMUX_TOTALCAPABILITY	stTotalCap;
	HUINT32						i;

	result = DI_DEMUX_GetTotalCapability (&stTotalCap);
	DONE_ERR(DI_ERR_OK != result);

	DI_UART_Print("############### Total Demux Capability ###############\n");
	DI_UART_Print("NumOfDemux : %d\n",stTotalCap.NumOfDemux);
	DI_UART_Print("NumOfTotalSectoinFilter : %d\n\n",stTotalCap.NumOfTotalSectoinFilter);

	for(i = 0; i < stTotalCap.NumOfDemux; i++)
	{
		result = DI_DEMUX_GetCapability(i, &stCapability);
		DONE_ERR(DI_ERR_OK != result);

		DI_UART_Print("############### demuxid = %d Demux Capability ###############\n", i);
		DI_UART_Print("ulCapability : %d\n",stCapability.ulCapability);
		DI_UART_Print("NumOfPESFilter : %d\n",stCapability.NumOfPESFilter);
		DI_UART_Print("NumOfSectoinFilter : %d\n",stCapability.NumOfSectoinFilter);
		DI_UART_Print("NumOfSectoinFilterPerPid : %d\n",stCapability.NumOfSectoinFilterPerPid);
		DI_UART_Print("NumOfMaximumFilterLength : %d\n",stCapability.NumOfMaximumFilterLength);
		DI_UART_Print("NumOfPrimer : %d\n",stCapability.NumOfPrimer);
		DI_UART_Print("NumOfAvailableStcChannel : %d\n\n",stCapability.NumOfAvailableStcChannel);
	}
	ret = UT_OK;

done:
	return UT_OK;
}

static UT_RESULT ut_getNumOfDemux(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDmx;

	result = DI_DEMUX_GetNumOfDemux(&ulNumOfDmx);
	DONE_ERR(DI_ERR_OK != result);

	DI_UART_Print("ulNumOfDmx : %d\n", ulNumOfDmx);

	ret = UT_OK;
done:
	return ret;
}

static UT_RESULT ut_getPath(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDmx;
	DI_DEMUX_INPUTDEVICETYPE etProducer;
	HUINT32 ulSourceId;
	DI_DEMUX_OUTPUTDEVICETYPE etConsumer;
	HUINT32  ulConsumerId;
	HINT32 nCI_Id;
	HUINT32 i;

	result = DI_DEMUX_GetNumOfDemux(&ulNumOfDmx);
	DONE_ERR(DI_ERR_OK != result);

	for(i=0;i<ulNumOfDmx;i++)
	{
		result = DI_DEMUX_GetPath(i, &etProducer, &ulSourceId, &etConsumer, &ulConsumerId, &nCI_Id);
		DONE_ERR(DI_ERR_OK != result);

		DI_UART_Print("############### demuxid = %d Demux Path Info ###############\n", i);
		DI_UART_Print("etProducer = %d, ulSourceId = %d, etConsumer = %d\n", etProducer, ulSourceId, etConsumer);
		DI_UART_Print("etProducer = %d, ulSourceId = %d\n\n", etProducer, ulSourceId);
	}

	ret = UT_OK;
done:
	return ret;
}


static UT_RESULT ut_dumpPAT(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HINT32 demux_num = 0;
	HINT32 tuner_num = 0;
	HULONG start_time, end_time;

	result = DI_DEMUX_SetPath(demux_num, DI_DEMUX_INPUT_TUNER, tuner_num, DI_DEMUX_OUTPUT_DECODER, 0, -1);
	DONE_ERR(DI_ERR_OK != result);

	result = DI_DEMUX_RegisterSectionCallback(demux_num,ut_cb_demux_PSI_Notify, ut_cb_demux_AllocBuffer);
	DONE_ERR(DI_ERR_OK != result);

	VK_MEM_Memset(ucValue,0x0,TEST_FILTER_LEN);
	VK_MEM_Memset(ucMask,0x0,TEST_FILTER_LEN);
	VK_MEM_Memset(ucExclusion,0x0,TEST_FILTER_LEN);

	ucMask[0] = 0xFF;

	/* Set up the filter */
	result = DI_DEMUX_SetSectionFilter(demux_num, PSI_FILTER_TEST_PID, PSI_TEST_REQID, FALSE, 15, ucValue, ucMask, ucExclusion, FALSE);
	DONE_ERR(DI_ERR_OK != result);

	start_time = VK_TIMER_GetSystemTick();
	while(1)
	{
		if(gPatCount >= 5)
		{
			break;
		}

		end_time = VK_TIMER_GetSystemTick();

		/* 1 minute timeout */
		if((end_time - start_time) >= 60000)
		{
			PrintError("[%s:%d] timeout!!!\n", __FUNCTION__, __LINE__);
			break;
		}
		VK_TASK_Sleep(100);
	}

	result = DI_DEMUX_RegisterSectionCallback(demux_num, NULL, NULL);
	DONE_ERR(DI_ERR_OK != result);

	if(gPatCount != 0)
	{
		gPatCount = 0;
		ret = UT_OK;
	}

done:
	return ret;
}

static UT_RESULT ut_dumpTeletextPES(void)
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HULONG start_time, end_time;

	result = DI_DEMUX_RegisterPESCallback(TEST_DEMUX_ID, DI_DEMUX_PESTYPE_TELETEXT, ut_cb_demux_PES_Notify,ut_cb_demux_AllocBuffer,ut_cb_demux_FreeBufferForError);
	DONE_ERR(DI_ERR_OK != result);

	result = DI_DEMUX_StartPESFilter(TEST_DEMUX_ID, PES_FILTER_TEST_PID, DI_DEMUX_PESTYPE_TELETEXT, PES_TEST_REQID);
	DONE_ERR(DI_ERR_OK != result);

	start_time = VK_TIMER_GetSystemTick();
	while(1)
	{
		if(gPESCount >= 5)
		{
			break;
		}

		end_time = VK_TIMER_GetSystemTick();
		/*1 minute timeout*/
		if((end_time - start_time) >= 60000)
		{
			PrintError("[%s:%d] timeout!!!\n", __FUNCTION__, __LINE__);
			break;
		}
		VK_TASK_Sleep(100);
	}

	result = DI_DEMUX_StopPESFilter(TEST_DEMUX_ID, PES_TEST_REQID);
	DONE_ERR(DI_ERR_OK != result);

	if(0 != gPESCount)
	{
		ret = UT_OK;
		gPESCount = 0;
	}

done:
	result = DI_DEMUX_RegisterPESCallback (TEST_DEMUX_ID, DI_DEMUX_PESTYPE_TELETEXT, NULL, NULL, NULL);
	if(DI_ERR_OK != result)
	{
		ret = UT_FAIL;
	}
	return ret;
}

static ut_setPath()
{
	UT_RESULT ret = UT_FAIL;
	DI_ERR_CODE result;
	HUINT32 ulNumOfDmx;
	DI_DEMUX_INPUTDEVICETYPE etProducer;
	DI_DEMUX_OUTPUTDEVICETYPE etConsumer;
	HUINT32  ulConsumerId;
	HUINT32 i;

	result = DI_DEMUX_GetNumOfDemux(&ulNumOfDmx);
	DONE_ERR(DI_ERR_OK != result);

	etProducer = DI_DEMUX_INPUT_TUNER;
	etConsumer = DI_DEMUX_OUTPUT_DECODER;
	DI_UART_Print("ulNumOfDmx = %d \n",ulNumOfDmx);
	for(i=0;i<ulNumOfDmx;i++)
	{

		result = DI_DEMUX_SetPath(i, etProducer, 0, etConsumer, 0, -1);
		if(i < ulNumOfDmx - NUM_OF_RESERVED_CHANNEL)
		{
			DONE_ERR(DI_ERR_OK != result);
		}
		else
		{
			DONE_ERR(DI_ERR_OK == result);
			break;
		}

		DI_UART_Print("############### demuxid = %d Demux Path Info ###############\n", i);
		DI_UART_Print("set PATH : Tuner %d --> Demux %d --> Decoder %d\n\n", 0, i, 0);
	}

	etProducer = DI_DEMUX_INPUT_PLAYER;
	result = DI_DEMUX_SetPath(i, etProducer, 0, etConsumer, 0, -1);
	DONE_ERR(DI_ERR_OK != result);

	etProducer = DI_DEMUX_INPUT_RECORDER;
	result = DI_DEMUX_SetPath(++i, etProducer, 0, etConsumer, 0, -1);
	DONE_ERR(DI_ERR_OK != result);

	ret = UT_OK;
done:
	return ret;
}

static UT_MENU_ITEM_T	menu_demux[] =
{
	MENU_START,
	MENU_FUNC(ut_getCapability, 20),
	MENU_FUNC(ut_getNumOfDemux, 20),
	MENU_FUNC(ut_setPath, 20),
	MENU_FUNC(ut_getPath, 20),
	MENU_FUNC(ut_dumpPAT, 20),
	MENU_FUNC(ut_dumpTeletextPES, 20),
	MENU_END,
};

static UT_MENU_ITEM_T	menu_main_demux[] =
{
	MENU_SUB(menu_demux),
};


void UT_DEMUX_Init(void)
{
	HINT32 i;
	HINT32	array_size;

	array_size = sizeof(menu_main_demux)/sizeof(menu_main_demux[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMainMenu(&menu_main_demux[i]);
	}
	array_size = sizeof(menu_demux)/sizeof(menu_demux[0]);
	for (i=0; i<array_size; i++)
	{
		UT_MENU_AddMenu(&menu_demux[i]);
	}
}


