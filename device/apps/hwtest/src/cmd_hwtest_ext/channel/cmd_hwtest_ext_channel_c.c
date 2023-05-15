/**
 * cmd_hwtest.c
*/

/**
 * @defgroup		CMD_HWTEST
 * @author			Chung Dong Chul
 * @note
 * @brief
 * @file 			cmd_hwtest.c
*/
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <linux/hdreg.h>
#include <errno.h>
#include <fcntl.h>
#include <byteswap.h>
#include <unistd.h>
#include "cmd_hwtest.h"
#include "cmd_hwtest_ext_channel.h"
#include "cmd_hwtest_ext_channel_priv.h"
#include "cmd_hwtest_interface.h"

#include "cmdtool.h"
#include "di_init.h"
#include "di_uart.h"
#include "di_osd.h"
#include "di_key.h"
#include "di_audio.h"
#include "di_ci.h"
#include "di_channel_s.h"
#include "di_channel_c.h"
#include "di_channel_t.h"
#include "di_usb.h"
#include "di_usb_hid.h"
#include "drv_fan.h"
#if defined(USE_RECORD) && defined (USE_HDD_SMART)
#include "di_hdd.h"
#endif
#include "taskdef.h"

static char	scChannelTestStringDictionary_CABHead[24][21];
static char scChannelTestStringDictionary_CABType[1][9];
static char scChannelTestStringDictionary_CABResult[8][17];
static char scChannelTestStringDictionary_CABModulation[5][9];
static char scChannelTestStringDictionary_CABSpectrum[3][9];

static unsigned int cab_pat_received_ok = 0;		

static void internal_cmdhwtestext_ChannelTest_CAB_PATCallback(void)
{ 
	cab_pat_received_ok++;		
	printf("internal_cmdhwtestext_ChannelTest_CAB_PATCallback is called\n");
}

void P_CmdHwTestExt_ChannelTest_CAB_ChannelInfoDisplay(CMD_HWTEST_EXT_CHANNEL_TEST_CAB_INFO cabInfo, unsigned int mode)
{
	char *pInfoDisplayString = HI_VK_MEM_Alloc(501);
	if ( mode == 0 )
	{
		VK_MEM_Memset(pInfoDisplayString, ' ', 500);
		sprintf(pInfoDisplayString, "Tuner: %lu, Freq: %lu kHz, SymbolRate: %lu\nModulation: %s, Spectrum: %s, NET,TSID(%04d,%04d)\n 			Try to Lock\n", 
				cabInfo.tunerId, 
				cabInfo.frequencykHz,
				cabInfo.symbolRate,
				&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
				&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
				cabInfo.netId,
				cabInfo.TSID);
			
		pInfoDisplayString[500] = '\0';
	}
	else if ( mode == 1 )
	{
		VK_MEM_Memset(pInfoDisplayString, ' ', 500);
		sprintf(pInfoDisplayString, "Tuner: %lu, Freq: %lu kHz, SymbolRate: %lu\nModulation: %s, Spectrum: %s, NET,TSID(%04d,%04d)\n 			Try to Lock Error\n", 
				cabInfo.tunerId, 
				cabInfo.frequencykHz,
				cabInfo.symbolRate,
				&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
				&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
				cabInfo.netId,
				cabInfo.TSID);

			
		pInfoDisplayString[500] = '\0';	
	}
	AP_HwTest_DisplayInfo(pInfoDisplayString);
	HI_VK_MEM_Free(pInfoDisplayString);
}

void P_CmdHwTestExt_ChannelTest_CAB_NodeDisplayString(char *pReturnString, int offset, CMD_HWTEST_EXT_CHANNEL_TEST_CAB_INFO cabInfo)
{
	sprintf(pReturnString, "%02d: %lu,%lu,%lu,%s,%s,%05d,%05d#", 
		offset, 
		cabInfo.tunerId,
		cabInfo.frequencykHz,
		cabInfo.symbolRate,
		&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
		&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
		cabInfo.netId,
		cabInfo.TSID);
}

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_CAB_LockTry(
	CMD_HWTEST_EXT_CHANNEL_TEST_CAB_INFO cabInfo,
	unsigned long requestId)
{
	HI_TuneParams tuneParams;

	tuneParams.cabParams.ulFrequency = cabInfo.frequencykHz;		
	tuneParams.cabParams.ulSymbolRate = cabInfo.symbolRate;		
	tuneParams.cabParams.etModulation=cabInfo.modulation;		
	tuneParams.cabParams.etSpectrum=cabInfo.spectrum;		
	tuneParams.cabParams.nNetId=cabInfo.netId;			
	tuneParams.cabParams.nTsId = cabInfo.TSID;		
	
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	if ( HI_CH_DI_RequestLock(requestId, 
		cabInfo.tunerId,
		HI_CH_TYPE_CAB,
		tuneParams) != 0 )
	{
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OPERATION_ERROR;
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}
	

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_CAB_ResultProcess
(
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE curType,
	HI_CH_SignalEvents chSignalEvent,
	CMD_HWTEST_EXT_CHANNEL_TEST_CAB_INFO cabInfo,
	CMD_HWTEST_EXT_TESTSTATUS_INFO *pTestStatusInfo,
	unsigned int requestId
)
{
	char *pShortMsgDisplay = NULL;
	char *pLongMsgDisplay = NULL;
	char *pWriteBuffer =NULL;
	HI_CH_SignalInfo signalInfo;	
	CMD_HWTEST_EXT_CHANNEL_TEST_Result tune_result;	
	unsigned int test_count = 0;
	pShortMsgDisplay = HI_VK_MEM_Alloc(201);
	printf("%s(%d)\n", __FUNCTION__, chSignalEvent);
	if ( pShortMsgDisplay == NULL )
	{
		printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	pLongMsgDisplay = HI_VK_MEM_Alloc(4097);
	if ( pShortMsgDisplay == NULL )
	{
		HI_VK_MEM_Free(pShortMsgDisplay);
		printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}	

	if ( (curType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST ) || 
             (curType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST)  )
	{
		unsigned long tempCount = 0;

		cab_pat_received_ok = 0;
		if ( chSignalEvent == HI_CH_SIGNAL_LOCKED )
		{
			pTestStatusInfo->scan_total_locked_num++;
		}
		VK_MEM_Memset(pShortMsgDisplay, ' ', 200);
		sprintf(pShortMsgDisplay, "%05d/%05d:%s\n", 
			pTestStatusInfo->test_total_cur_node_num, 
			pTestStatusInfo->test_total_num_of_node,
			&scChannelTestStringDictionary_CABResult[chSignalEvent][0]);

		pShortMsgDisplay[200] = '\0';
		P_SendNewMsgStringToHandler(pShortMsgDisplay);
        if (curType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST )
        {
		    P_SendNewProgressMsgStringToHandler(
			    "Channel Test is now ... \n", 
			    ((pTestStatusInfo->test_total_cur_node_num*100)/(pTestStatusInfo->test_total_num_of_node)));
        }

		HI_VK_TASK_Sleep(200);
		
		if ( chSignalEvent == HI_CH_SIGNAL_LOCKED )
		{
			//CMD_RegisterDemuxTestCallback(internal_cmdhwtestext_ChannelTest_CAB_PATCallback);						
			P_DEMUX_SectionFilterTest(1,0,cabInfo.tunerId);
			for ( tempCount = 0 ; tempCount < 7 ; tempCount ++ )
			{
				HI_CH_DI_GetSignalInfo(
					cabInfo.tunerType, 
					cabInfo.tunerId, 
					&tune_result.chSignalInfo);

					printf("signalInfo1(%lu,%lu,%lu,%f)\n", 
					tune_result.chSignalInfo.ul_Strength,
					tune_result.chSignalInfo.ul_Quality,
					tune_result.chSignalInfo.ul_Agc,
					tune_result.chSignalInfo.f_Ber);

					printf("signalInfo2(%f,%lu,%lu,%lu)\n", 
					tune_result.chSignalInfo.f_Snr,
					tune_result.chSignalInfo.ul_Emergencyflag,
					tune_result.chSignalInfo.ul_CorrectedNo,
					tune_result.chSignalInfo.ul_UnCorrectedNo);	
	
					printf("signalInfo3(%lu,%lu,%lu)\n", 
					tune_result.chSignalInfo.ul_UnErroredNo,
					tune_result.chSignalInfo.ul_InbandBer,
					tune_result.chSignalInfo.ul_UnCorrectedFlag);
					
					printf("signalInfo4(%lu, %lu, %f)\n",
					tune_result.chSignalInfo.ul_RfAgc,
					tune_result.chSignalInfo.ul_IfAgc,
					tune_result.chSignalInfo.f_SignalInputPower);
					
				VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);
				if ( cab_pat_received_ok != 1 )
				{
					sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, SymbolRate: %lu\nModulation: %s, Spectrum: %s, NET,TSID(%04d,%04d)\nStrength: %lu, Quality: %lu, Power: %f\nAGC: %lu, RF_AGC: %lu, IF_AGC: %lu\nBER: %f, SNR: %f, PAT Received\n", 
							cabInfo.tunerId, 
							cabInfo.frequencykHz,
							cabInfo.symbolRate,
							&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
							&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
							cabInfo.netId,
							cabInfo.TSID,
							tune_result.chSignalInfo.ul_Strength,
							tune_result.chSignalInfo.ul_Quality,
							tune_result.chSignalInfo.f_SignalInputPower,
							tune_result.chSignalInfo.ul_Agc,
							tune_result.chSignalInfo.ul_RfAgc,
							tune_result.chSignalInfo.ul_IfAgc,
							tune_result.chSignalInfo.f_Ber,
							tune_result.chSignalInfo.f_Snr);

				}
				else
				{
					sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, SymbolRate: %lu\nModulation: %s, Spectrum: %s, NET,TSID(%04d,%04d)\nStrength: %lu, Quality: %lu, Power: %f\nAGC: %lu, RF_AGC: %lu, IF_AGC: %lu\nBER: %f, SNR: %f, No PAT Received\n", 
							cabInfo.tunerId, 
							cabInfo.frequencykHz,
							cabInfo.symbolRate,
							&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
							&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
							cabInfo.netId,
							cabInfo.TSID,
							tune_result.chSignalInfo.ul_Strength,
							tune_result.chSignalInfo.ul_Quality,
							tune_result.chSignalInfo.f_SignalInputPower,
							tune_result.chSignalInfo.ul_Agc,
							tune_result.chSignalInfo.ul_RfAgc,
							tune_result.chSignalInfo.ul_IfAgc,
							tune_result.chSignalInfo.f_Ber,
							tune_result.chSignalInfo.f_Snr);

				}


				pLongMsgDisplay[4096] = '\0';
				AP_HwTest_DisplayInfo(pLongMsgDisplay);
				HI_VK_TASK_Sleep(1000);
			}
			P_DEMUX_SectionFilterTest(0,0,cabInfo.tunerId);
			//CMD_RegisterDemuxTestCallback(NULL);

		}
		else
		{
			tune_result.chSignalInfo.ul_Strength = 0;
			tune_result.chSignalInfo.ul_Quality =0;
			tune_result.chSignalInfo.ul_Agc =0;
			tune_result.chSignalInfo.f_Ber =0;
			tune_result.chSignalInfo.f_Snr =0;
			tune_result.chSignalInfo.ul_Emergencyflag =0;
			tune_result.chSignalInfo.ul_CorrectedNo =0;
			tune_result.chSignalInfo.ul_UnCorrectedNo =0;	
			tune_result.chSignalInfo.ul_UnErroredNo =0;
			tune_result.chSignalInfo.ul_InbandBer =0;
			tune_result.chSignalInfo.ul_UnCorrectedFlag =0;	
			tune_result.chSignalInfo.ul_RfAgc = 0;
			tune_result.chSignalInfo.ul_IfAgc = 0;
			tune_result.chSignalInfo.f_SignalInputPower = 0;
			VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);


			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, SymbolRate : %lu\nModulation: %s, Spectrum: %s, NET,TSID(%04d,%04d)\n			UNLOCKED\n", 
						cabInfo.tunerId, 
						cabInfo.frequencykHz,
						cabInfo.symbolRate,
						&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
						&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
						cabInfo.netId,
						cabInfo.TSID);
						
			pLongMsgDisplay[4096] = '\0';
			AP_HwTest_DisplayInfo(pLongMsgDisplay);		
			HI_VK_TASK_Sleep(1000);
		}
        FILE *fpPid = fopen(pTestStatusInfo->pLogOutputFileName, "a+");
	    if ( fpPid == 0 )
	    {
	    	printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);
	        return;
	    }		

		pWriteBuffer = HI_VK_MEM_Alloc(1024);
		HI_VK_MEM_Memset(pWriteBuffer, ' ',1024);			
		printf("begin to record2\n");
		fseek(fpPid, 0, SEEK_END);
		if ( cab_pat_received_ok != 0 )
		{
			fprintf(fpPid, "%08x\t%s\t%lu\t%lu\t%lu\t%s\t%s\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%f\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%s\n",
				requestId, 
				&scChannelTestStringDictionary_CABResult[chSignalEvent][0],
				cabInfo.tunerId, 
				cabInfo.frequencykHz,
				cabInfo.symbolRate,
				&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
				&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
				cabInfo.netId,
				cabInfo.TSID,
				tune_result.chSignalInfo.ul_Strength,
				tune_result.chSignalInfo.ul_Quality,
				tune_result.chSignalInfo.ul_Agc,
				tune_result.chSignalInfo.f_Ber,
				tune_result.chSignalInfo.f_Snr,
				tune_result.chSignalInfo.ul_Emergencyflag,
				tune_result.chSignalInfo.ul_CorrectedNo,
				tune_result.chSignalInfo.ul_UnCorrectedNo, 
				tune_result.chSignalInfo.ul_UnErroredNo,
				tune_result.chSignalInfo.ul_InbandBer,
				tune_result.chSignalInfo.ul_UnCorrectedFlag,
				tune_result.chSignalInfo.ul_RfAgc,
				tune_result.chSignalInfo.ul_IfAgc,
				tune_result.chSignalInfo.f_SignalInputPower,
				"PAT Received");
		}
		else
		{
			fprintf(fpPid, "%08x\t%s\t%lu\t%lu\t%lu\t%s\t%s\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%f\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%s\n",
				requestId, 
				&scChannelTestStringDictionary_CABResult[chSignalEvent][0],
				cabInfo.tunerId, 
				cabInfo.frequencykHz,
				cabInfo.symbolRate,
				&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
				&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
				cabInfo.netId,
				cabInfo.TSID,
				tune_result.chSignalInfo.ul_Strength,
				tune_result.chSignalInfo.ul_Quality,
				tune_result.chSignalInfo.ul_Agc,
				tune_result.chSignalInfo.f_Ber,
				tune_result.chSignalInfo.f_Snr,
				tune_result.chSignalInfo.ul_Emergencyflag,
				tune_result.chSignalInfo.ul_CorrectedNo,
				tune_result.chSignalInfo.ul_UnCorrectedNo, 
				tune_result.chSignalInfo.ul_UnErroredNo,
				tune_result.chSignalInfo.ul_InbandBer,
				tune_result.chSignalInfo.ul_UnCorrectedFlag,
				tune_result.chSignalInfo.ul_RfAgc,
				tune_result.chSignalInfo.ul_IfAgc,
				tune_result.chSignalInfo.f_SignalInputPower,
				"No PAT Received");
		}
		fclose(fpPid);
		HI_VK_MEM_Free(pWriteBuffer);
		HI_VK_MEM_Free(pLongMsgDisplay);
		HI_VK_MEM_Free(pShortMsgDisplay);	       
        cab_pat_received_ok  =0;
	}
	else if ( (curType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST)  )
{
		cab_pat_received_ok = 0;
		
		if ( chSignalEvent == HI_CH_SIGNAL_LOCKED )
		{
			pTestStatusInfo->scan_total_locked_num++;
		}
		VK_MEM_Memset(pShortMsgDisplay, ' ', 200);
		sprintf(pShortMsgDisplay, "%05d/%05d:%s\n", 
			pTestStatusInfo->test_total_cur_node_num, 
			pTestStatusInfo->test_total_num_of_node,
			&scChannelTestStringDictionary_CABResult[chSignalEvent][0]);
        

		pShortMsgDisplay[200] = '\0';
		P_SendNewMsgStringToHandler(pShortMsgDisplay);
		
		P_SendNewProgressMsgStringToHandler(
			"Channel Test is now ... \n", 
			((pTestStatusInfo->test_total_cur_node_num*100)/(pTestStatusInfo->test_total_num_of_node)));

		HI_VK_TASK_Sleep(200);

		if (chSignalEvent == HI_CH_SIGNAL_LOCKED)
		{
			VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);
			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, SymbolRate: %lu\nModulation: %s, Spectrum: %s, NET,TSID(%04d,%04d)\n 			LOCKED\n", 
				cabInfo.tunerId, 
				cabInfo.frequencykHz,
				cabInfo.symbolRate,
				&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
				&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
				cabInfo.netId,
				cabInfo.TSID);



			pLongMsgDisplay[4096] = '\0';
			AP_HwTest_DisplayInfo(pLongMsgDisplay);						
			HI_VK_TASK_Sleep(1000);
		}
		else
		{
			VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);
			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, SymbolRate: %lu\nModulation: %s, Spectrum: %s, NET,TSID(%04d,%04d)\n 			UNLOCKED\n", 
				cabInfo.tunerId, 
				cabInfo.frequencykHz,
				cabInfo.symbolRate,
				&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
				&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
				cabInfo.netId,
				cabInfo.TSID);
			
			pLongMsgDisplay[4096] = '\0';
			AP_HwTest_DisplayInfo(pLongMsgDisplay);		
			HI_VK_TASK_Sleep(1000);
		}
	    FILE *fpPid = fopen(pTestStatusInfo->pLogOutputFileName, "a+");
	    if ( fpPid == 0 )
	    {
	    	printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);
	        return;
	    }		

		pWriteBuffer = HI_VK_MEM_Alloc(1024);
		HI_VK_MEM_Memset(pWriteBuffer, ' ',1024);			
		printf("begin to record2\n");
		fseek(fpPid, 0, SEEK_END);
		fprintf(fpPid, "%08x\t%s\t%lu\t%lu\t%lu\t%s\t%s\t%lu\t%lu\n",
				requestId, 
				&scChannelTestStringDictionary_CABResult[chSignalEvent][0],
				cabInfo.tunerId, 
				cabInfo.frequencykHz,
				cabInfo.symbolRate,
				&scChannelTestStringDictionary_CABModulation[cabInfo.modulation][0],
				&scChannelTestStringDictionary_CABSpectrum[cabInfo.spectrum][0],	
				cabInfo.netId,
				cabInfo.TSID);

		fclose(fpPid);
		HI_VK_MEM_Free(pWriteBuffer);
		HI_VK_MEM_Free(pLongMsgDisplay);
		HI_VK_MEM_Free(pShortMsgDisplay);		

	
	}
		

	HI_CH_DI_CancelLock(cabInfo.tunerId, cabInfo.tunerType);	
	
}


CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_CAB_InitStringDictionary(void)
{
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABType[0][0], "	  CAB", 8);
	scChannelTestStringDictionary_CABType[0][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABResult[0][0], "LOCKED          ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABResult[1][0], "UNLOCKED        ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABResult[2][0], "ANTENNA FAULT   ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABResult[3][0], "ANTENNA OK      ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABResult[4][0], "TSMF GODD       ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABResult[5][0], "TSMF_BAD        ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABResult[6][0], "TSMF_EWS_ON     ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABResult[7][0], "TSMF_EWS_OFF    ", 16);
	scChannelTestStringDictionary_CABResult[0][16] = '\0';
	scChannelTestStringDictionary_CABResult[1][16] = '\0';
	scChannelTestStringDictionary_CABResult[2][16] = '\0';
	scChannelTestStringDictionary_CABResult[3][16] = '\0';
	scChannelTestStringDictionary_CABResult[4][16] = '\0';
	scChannelTestStringDictionary_CABResult[5][16] = '\0';
	scChannelTestStringDictionary_CABResult[6][16] = '\0';
	scChannelTestStringDictionary_CABResult[7][16] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABModulation[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABModulation[1][0], "16QAM   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABModulation[2][0], "32QAM   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABModulation[3][0], "64QAM   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABModulation[4][0], "128QAM  ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABModulation[5][0], "256QAM  ", 8);
	scChannelTestStringDictionary_CABModulation[0][8] = '\0';
	scChannelTestStringDictionary_CABModulation[1][8] = '\0';
	scChannelTestStringDictionary_CABModulation[2][8] = '\0';
	scChannelTestStringDictionary_CABModulation[3][8] = '\0';
	scChannelTestStringDictionary_CABModulation[4][8] = '\0';
	scChannelTestStringDictionary_CABModulation[5][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[0][0], "Request Id          ", 20);
	scChannelTestStringDictionary_CABHead[0][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[1][0], "Tuner Status        ", 20);
	scChannelTestStringDictionary_CABHead[1][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[2][0], "Tuner Id            ", 20);
	scChannelTestStringDictionary_CABHead[2][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[3][0], "Frequency           ", 20);
	scChannelTestStringDictionary_CABHead[3][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[4][0], "Symbol Rate         ", 20);
	scChannelTestStringDictionary_CABHead[4][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[5][0], "Modulation          ", 20);
	scChannelTestStringDictionary_CABHead[5][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[6][0], "Spectrum            ", 20);
	scChannelTestStringDictionary_CABHead[6][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[7][0], "NetId               ", 20);
	scChannelTestStringDictionary_CABHead[7][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[8][0], "TSID                ", 20);
	scChannelTestStringDictionary_CABHead[8][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[9][0], "Signal Strength     ", 20);
	scChannelTestStringDictionary_CABHead[9][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[10][0], "Signal Quality      ", 20);
	scChannelTestStringDictionary_CABHead[10][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[11][0], "AGC                 ", 20);
	scChannelTestStringDictionary_CABHead[11][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[12][0], "BER                 ", 20);
	scChannelTestStringDictionary_CABHead[12][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[13][0], "SNR                 ", 20);
	scChannelTestStringDictionary_CABHead[13][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[14][0], "Emergency Flag      ", 20);
	scChannelTestStringDictionary_CABHead[14][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[15][0], "Corrected No        ", 20);
	scChannelTestStringDictionary_CABHead[15][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[16][0], "Uncorrected No      ", 20);
	scChannelTestStringDictionary_CABHead[16][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[17][0], "UnErrored No        ", 20);
	scChannelTestStringDictionary_CABHead[17][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[18][0], "InBand BER          ", 20);
	scChannelTestStringDictionary_CABHead[18][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[19][0], "UnCorrected Flag    ", 20);
	scChannelTestStringDictionary_CABHead[19][20] = '\0';	    
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[20][0], "RF AGC              ", 20);
	scChannelTestStringDictionary_CABHead[20][20] = '\0';	 
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[21][0], "IF AGC              ", 20);
	scChannelTestStringDictionary_CABHead[21][20] = '\0';	    
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[22][0], "Signal Power        ", 20);
	scChannelTestStringDictionary_CABHead[22][20] = '\0';	    
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_CABHead[23][0], "PAT Received        ", 20);
	scChannelTestStringDictionary_CABHead[23][20] = '\0';	 
	
}
	

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_CAB_ParsingInfo
(
	unsigned int inData, 
	unsigned int data_offset, 
	CMD_HWTEST_EXT_CHANNEL_TEST_CAB_INFO *pCABInfo
)
{
	if ( pCABInfo == NULL )
	{
		printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	switch(data_offset)
	{
		case 0:/* Tuner Type */
			if ( inData != CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pCABInfo->tunerType = CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB;
			break;
		case 1: /* Tuner Id */
			if ( inData > HI_CH_GetNumOfInstance() )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pCABInfo->tunerId = inData;
			break;						
		case 2:/* Frequency */
			pCABInfo->frequencykHz= inData;			
			break;
		case 3: /* symbol rate */
			pCABInfo->symbolRate= inData;			
			break;
		case 4: /*Modulation */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_CAB_MODULATION_256QAM)
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pCABInfo->modulation = inData;			
			break;
		case 5: /* spectrum */			
			if ( inData > CMD_HWTEST_EXT_CHANNEL_CAB_SPECTRUM_INVERTED)
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}			
			pCABInfo->spectrum = inData;			
			break;
		case 6: /* net id */
			pCABInfo->netId = inData;			
			break;
		case 7: /* TS ID */
			pCABInfo->TSID= inData;			
			break;			
		default:
			return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			break;			
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
	
}

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_CAB_CreateTERLogFile
(
	char *pLogFileName,
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE testType
)
{
	if ( pLogFileName == NULL )
	{
		printf("(%s)-Error(%d)\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
    FILE *fpPid = fopen(pLogFileName, "w");
    if ( fpPid == 0 )
    {
    	printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);
	return;
}
	if ( testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST )
	{
		printf("%s(CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST_\n", __FUNCTION__);
	
		fprintf(fpPid, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
				&scChannelTestStringDictionary_CABHead[0][0], 
				&scChannelTestStringDictionary_CABHead[1][0], 
				&scChannelTestStringDictionary_CABHead[2][0], 
				&scChannelTestStringDictionary_CABHead[3][0], 
				&scChannelTestStringDictionary_CABHead[4][0], 
				&scChannelTestStringDictionary_CABHead[5][0], 
				&scChannelTestStringDictionary_CABHead[6][0], 
				&scChannelTestStringDictionary_CABHead[7][0], 
				&scChannelTestStringDictionary_CABHead[8][0]);		

		printf("fclose-begin\n");		
	}
	else if ( (testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST)  ||
	          (testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST) )
	{
			fprintf(fpPid, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
				&scChannelTestStringDictionary_CABHead[0][0], 
				&scChannelTestStringDictionary_CABHead[1][0], 
				&scChannelTestStringDictionary_CABHead[2][0], 
				&scChannelTestStringDictionary_CABHead[3][0], 
				&scChannelTestStringDictionary_CABHead[4][0], 
				&scChannelTestStringDictionary_CABHead[5][0], 
				&scChannelTestStringDictionary_CABHead[6][0], 
				&scChannelTestStringDictionary_CABHead[7][0], 
				&scChannelTestStringDictionary_CABHead[8][0], 
				&scChannelTestStringDictionary_CABHead[9][0], 				
				&scChannelTestStringDictionary_CABHead[10][0], 
				&scChannelTestStringDictionary_CABHead[11][0], 
				&scChannelTestStringDictionary_CABHead[12][0], 
				&scChannelTestStringDictionary_CABHead[13][0], 
				&scChannelTestStringDictionary_CABHead[14][0], 
				&scChannelTestStringDictionary_CABHead[15][0], 
				&scChannelTestStringDictionary_CABHead[16][0], 
				&scChannelTestStringDictionary_CABHead[17][0], 
				&scChannelTestStringDictionary_CABHead[18][0], 
				&scChannelTestStringDictionary_CABHead[19][0], 
				&scChannelTestStringDictionary_CABHead[20][0], 
				&scChannelTestStringDictionary_CABHead[21][0], 
				&scChannelTestStringDictionary_CABHead[22][0], 
				&scChannelTestStringDictionary_CABHead[23][0]);
	}
	fsync(fpPid);
	fclose(fpPid);	
}





