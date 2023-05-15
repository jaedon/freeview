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

static char	scChannelTestStringDictionary_TERHead[35][21];
static char scChannelTestStringDictionary_TERType[1][9];
static char scChannelTestStringDictionary_TERResult[8][17];
static char scChannelTestStringDictionary_TERBandwidth[6][9];
static char scChannelTestStringDictionary_TERSystem[2][9];
static char scChannelTestStringDictionary_TERCoderate[8][9];
static char scChannelTestStringDictionary_TERGuardInterval[8][9];
static char scChannelTestStringDictionary_TERHierarchy[5][9];
static char scChannelTestStringDictionary_TERModulation[5][9];
static char scChannelTestStringDictionary_TEROffset[8][9];
static char scChannelTestStringDictionary_TERStream[2][9];
static char scChannelTestStringDictionary_TERTransmode[7][9];
static char scChannelTestStringDictionary_TERPreambleFormat[2][9];
static char scChannelTestStringDictionary_TERPreambleMixed[2][9];
static char scChannelTestStringDictionary_TERPilotPattern[9][9];
static char scChannelTestStringDictionary_TERExtendedCarrier[2][9];
static char scChannelTestStringDictionary_TERPaprMode[4][9];

static unsigned int ter_pat_received_ok = 0;		



static void internal_cmdhwtestext_ChannelTest_TER_PATCallback(void)
{
	ter_pat_received_ok++;		
	printf("internal_cmdhwtestext_ChannelTest_TER_PATCallback is called\n");
}

void P_CmdHwTestExt_ChannelTest_TER_ChannelInfoDisplay(CMD_HWTEST_EXT_CHANNEL_TEST_TER_INFO terInfo, unsigned int mode)
{
	char *pInfoDisplayString = HI_VK_MEM_Alloc(501);
	if ( mode == 0 )
	{
		VK_MEM_Memset(pInfoDisplayString, ' ', 500);
		sprintf(pInfoDisplayString, "Tuner: %lu, Freq: %lu kHz, Bandwidth: %s\nSystem: %s, CodeRate: %s, Modulation: %s\nTransMode: %s\n 			Try to Lock\n", 
				terInfo.tunerId, 
				terInfo.frequencykHz,
				&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
				&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
				&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],
				&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],
				&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0]);
			
		pInfoDisplayString[500] = '\0';
	}
	else if ( mode == 1 )
	{
		VK_MEM_Memset(pInfoDisplayString, ' ', 500);
		sprintf(pInfoDisplayString, "Tuner: %lu, Freq: %lu kHz, Bandwidth : %s\nSystem: %s, CodeRate: %s, Modulation: %s\nTransMode: %s\n 			Try to Lock Error\n", 
				terInfo.tunerId, 
				terInfo.frequencykHz,
				&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
				&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
				&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],
				&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],
				&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0]);

			
		pInfoDisplayString[500] = '\0';	
	}
	AP_HwTest_DisplayInfo(pInfoDisplayString);
	HI_VK_MEM_Free(pInfoDisplayString);
}

void P_CmdHwTestExt_ChannelTest_TER_NodeDisplayString(char *pReturnString, int offset, CMD_HWTEST_EXT_CHANNEL_TEST_TER_INFO terInfo)
{
	sprintf(pReturnString, "%02d: %lu,%lu,%s,%s,%s,%s,%s,%s#", 
		offset, 
		terInfo.tunerId,
		terInfo.frequencykHz,
		&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
		&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
		&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],
		&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],
		&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0],
		&scChannelTestStringDictionary_TEROffset[terInfo.offset][0]);	
}

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_TER_LockTry(
	CMD_HWTEST_EXT_CHANNEL_TEST_TER_INFO terInfo,
	unsigned long requestId)
{
	HI_TuneParams tuneParams;

	tuneParams.terParams.ulFrequency = terInfo.frequencykHz;		
	tuneParams.terParams.etBandWidth=terInfo.bandwidth;		
	tuneParams.terParams.etModulation=terInfo.modulation;		
	tuneParams.terParams.etHierarchy=terInfo.hierarchy;		
	tuneParams.terParams.etCodeRate=terInfo.codeRate;			
	tuneParams.terParams.etTransMode = terInfo.transMode;		
	tuneParams.terParams.etGuardInterval = terInfo.guardInterval;	
	tuneParams.terParams.etStream = terInfo.stream;			
	tuneParams.terParams.etOffset = terInfo.offset;			
	tuneParams.terParams.etTerSystem = terInfo.system;		
	tuneParams.terParams.ulPlpId = terInfo.plpip;			
	tuneParams.terParams.etPreambleFormat = terInfo.preambleformat;
	tuneParams.terParams.etPreambleMixed = terInfo.preamblemixed;
	tuneParams.terParams.etPilotPattern = terInfo.pilotpattern;
	tuneParams.terParams.etExtendedCarrier = terInfo.extendedCarrier;
	tuneParams.terParams.etPaprMode = terInfo.paprmode;
	tuneParams.terParams.ulPlpNumBlocks = terInfo.plpnumblocks;
	
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	if ( HI_CH_DI_RequestLock(requestId, 
		terInfo.tunerId,
		HI_CH_TYPE_TER,
		tuneParams) != 0 )
	{
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OPERATION_ERROR;
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}
	

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_TER_ResultProcess
(
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE curType,
	HI_CH_SignalEvents chSignalEvent,
	CMD_HWTEST_EXT_CHANNEL_TEST_TER_INFO terInfo,
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

		ter_pat_received_ok = 0;
		if ( chSignalEvent == HI_CH_SIGNAL_LOCKED )
		{
			pTestStatusInfo->scan_total_locked_num++;
		}
		VK_MEM_Memset(pShortMsgDisplay, ' ', 200);
		sprintf(pShortMsgDisplay, "%05d/%05d:%s\n", 
			pTestStatusInfo->test_total_cur_node_num, 
			pTestStatusInfo->test_total_num_of_node,
			&scChannelTestStringDictionary_TERResult[chSignalEvent][0]);

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
			//CMD_RegisterDemuxTestCallback(internal_cmdhwtestext_ChannelTest_TER_PATCallback);						
			P_DEMUX_SectionFilterTest(1,0,terInfo.tunerId);
			for ( tempCount = 0 ; tempCount < 7 ; tempCount ++ )
			{
				HI_CH_DI_GetSignalInfo(
					terInfo.tunerType, 
					terInfo.tunerId, 
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
					
					printf("signalInfo4(%lu,%lu,%f)\n", 
					tune_result.chSignalInfo.ul_RfAgc,
					tune_result.chSignalInfo.ul_IfAgc,
					tune_result.chSignalInfo.f_SignalInputPower);
					
				VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);
				if ( ter_pat_received_ok != 1 )
				{
					sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, Bandwidth: %s\nSystem: %s, CodeRate: %s, Modulation: %s\nTransMode: %s, Strength: %lu, Quality: %lu\nPower: %f, AGC: %lu, RF_AGC: %lu, IF_AGC: %lu\nBER: %f, SNR: %f, PAT Received\n", 
							terInfo.tunerId, 
							terInfo.frequencykHz,
							&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
							&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
							&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],
							&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],
							&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0],
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
					sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, Bandwidth: %s\nSystem: %s, CodeRate: %s, Modulation: %s\nTransMode: %s, Strength: %lu, Quality: %lu\nPower: %f, AGC: %lu, RF_AGC: %lu, IF_AGC: %lu\nBER: %f, SNR: %f, No PAT Received\n", 
							terInfo.tunerId, 
							terInfo.frequencykHz,
							&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
							&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
							&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],
							&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],
							&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0],
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
			P_DEMUX_SectionFilterTest(0,0,terInfo.tunerId);
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


			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, Bandwidth: %s\nSystem: %s, CodeRate: %s, Modulation: %s\nTransMode: %s\n			UNLOCKED\n", 
					terInfo.tunerId, 
					terInfo.frequencykHz,
					&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
					&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
					&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],
					&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],
					&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0]);
						
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
		if ( ter_pat_received_ok != 0 )
		{
			fprintf(fpPid, "%08x\t%s\t%lu\t%lu\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lu\t%s\t%s\t%s\t%s\t%s\t%lu\t%lu\t%lu\t%lu\t%f\t%f\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%s\n",
				requestId, 
				&scChannelTestStringDictionary_TERResult[chSignalEvent][0],
				terInfo.tunerId,
				terInfo.frequencykHz,
				&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
				&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
				&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],				
				&scChannelTestStringDictionary_TERGuardInterval[terInfo.guardInterval][0],				
				&scChannelTestStringDictionary_TERHierarchy[terInfo.hierarchy][0],	
				&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],	
				&scChannelTestStringDictionary_TEROffset[terInfo.offset][0],	
				&scChannelTestStringDictionary_TERStream[terInfo.stream][0],	
				&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0],	
				terInfo.plpip,
				&scChannelTestStringDictionary_TERPreambleFormat[terInfo.preambleformat][0],					
				&scChannelTestStringDictionary_TERPreambleMixed[terInfo.preamblemixed][0],					
				&scChannelTestStringDictionary_TERPilotPattern[terInfo.pilotpattern][0],					
				&scChannelTestStringDictionary_TERExtendedCarrier[terInfo.extendedCarrier][0],	
				&scChannelTestStringDictionary_TERPaprMode[terInfo.paprmode][0],	
				terInfo.plpnumblocks,
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
			fprintf(fpPid, "%08x\t%s\t%lu\t%lu\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lu\t%s\t%s\t%s\t%s\t%s\t%lu\t%lu\t%lu\t%lu\t%f\t%f\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%s\n",
				requestId, 
				&scChannelTestStringDictionary_TERResult[chSignalEvent][0],
				terInfo.tunerId,
				terInfo.frequencykHz,
				&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
				&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
				&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],				
				&scChannelTestStringDictionary_TERGuardInterval[terInfo.guardInterval][0],				
				&scChannelTestStringDictionary_TERHierarchy[terInfo.hierarchy][0],	
				&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],	
				&scChannelTestStringDictionary_TEROffset[terInfo.offset][0],	
				&scChannelTestStringDictionary_TERStream[terInfo.stream][0],	
				&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0],	
				terInfo.plpip,
				&scChannelTestStringDictionary_TERPreambleFormat[terInfo.preambleformat][0],					
				&scChannelTestStringDictionary_TERPreambleMixed[terInfo.preamblemixed][0],					
				&scChannelTestStringDictionary_TERPilotPattern[terInfo.pilotpattern][0],					
				&scChannelTestStringDictionary_TERExtendedCarrier[terInfo.extendedCarrier][0],	
				&scChannelTestStringDictionary_TERPaprMode[terInfo.paprmode][0],	
				terInfo.plpnumblocks,
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
				"NO PAT Received");

		}
		fclose(fpPid);
		HI_VK_MEM_Free(pWriteBuffer);
		HI_VK_MEM_Free(pLongMsgDisplay);
		HI_VK_MEM_Free(pShortMsgDisplay);	       
        ter_pat_received_ok  =0;
	}
	else if ( (curType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST)  )
	{
		ter_pat_received_ok = 0;
		
		if ( chSignalEvent == HI_CH_SIGNAL_LOCKED )
		{
			pTestStatusInfo->scan_total_locked_num++;
		}
		VK_MEM_Memset(pShortMsgDisplay, ' ', 200);
		sprintf(pShortMsgDisplay, "%05d/%05d:%s\n", 
			pTestStatusInfo->test_total_cur_node_num, 
			pTestStatusInfo->test_total_num_of_node,
			&scChannelTestStringDictionary_TERResult[chSignalEvent][0]);
        printf("satResult(%d,%s,%s)\n", chSignalEvent, &scChannelTestStringDictionary_TERResult[chSignalEvent][0],&scChannelTestStringDictionary_TERResult[0][0]);
        

		pShortMsgDisplay[200] = '\0';
		P_SendNewMsgStringToHandler(pShortMsgDisplay);
		
		P_SendNewProgressMsgStringToHandler(
			"Channel Test is now ... \n", 
			((pTestStatusInfo->test_total_cur_node_num*100)/(pTestStatusInfo->test_total_num_of_node)));

		HI_VK_TASK_Sleep(200);

		if (chSignalEvent == HI_CH_SIGNAL_LOCKED)
		{
			VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);
			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, Bandwidth: %s\nSystem: %s, CodeRate: %s, Modulation: %s\nTransMode: %s\n			LOCKED\n", 
					terInfo.tunerId, 
					terInfo.frequencykHz,
					&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
					&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
					&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],
					&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],
					&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0]);



			pLongMsgDisplay[4096] = '\0';
			AP_HwTest_DisplayInfo(pLongMsgDisplay);						
			HI_VK_TASK_Sleep(1000);
		}
		else
		{
			VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);
			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu kHz, Bandwidth : %s\nSystem: %s, CodeRate: %s, Modulation: %s\nTransMode: %s\n			UNLOCKED\n", 
					terInfo.tunerId, 
					terInfo.frequencykHz,
					&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
					&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
					&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],
					&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],
					&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0]);					
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
		fprintf(fpPid, "%08x\t%s\t%lu\t%lu\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lu\t%s\t%s\t%s\t%s\t%s\t%lu\t\n",
				requestId, 
				&scChannelTestStringDictionary_TERResult[chSignalEvent][0],
				terInfo.tunerId,
				terInfo.frequencykHz,
				&scChannelTestStringDictionary_TERBandwidth[terInfo.bandwidth][0],
				&scChannelTestStringDictionary_TERSystem[terInfo.system][0],
				&scChannelTestStringDictionary_TERCoderate[terInfo.codeRate][0],				
				&scChannelTestStringDictionary_TERGuardInterval[terInfo.guardInterval][0],				
				&scChannelTestStringDictionary_TERHierarchy[terInfo.hierarchy][0],	
				&scChannelTestStringDictionary_TERModulation[terInfo.modulation][0],	
				&scChannelTestStringDictionary_TEROffset[terInfo.offset][0],	
				&scChannelTestStringDictionary_TERStream[terInfo.stream][0],	
				&scChannelTestStringDictionary_TERTransmode[terInfo.transMode][0],	
				terInfo.plpip,
				&scChannelTestStringDictionary_TERPreambleFormat[terInfo.preambleformat][0],					
				&scChannelTestStringDictionary_TERPreambleMixed[terInfo.preamblemixed][0],					
				&scChannelTestStringDictionary_TERPilotPattern[terInfo.pilotpattern][0],					
				&scChannelTestStringDictionary_TERExtendedCarrier[terInfo.extendedCarrier][0],	
				&scChannelTestStringDictionary_TERPaprMode[terInfo.paprmode][0],	
				terInfo.plpnumblocks);	

		fclose(fpPid);
		HI_VK_MEM_Free(pWriteBuffer);
		HI_VK_MEM_Free(pLongMsgDisplay);
		HI_VK_MEM_Free(pShortMsgDisplay);		

	
	}
		

	HI_CH_DI_CancelLock(terInfo.tunerId, terInfo.tunerType);	
	
}


CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_TER_InitStringDictionary(void)
{
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERType[0][0], "	  TER", 8);
	scChannelTestStringDictionary_TERType[0][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERResult[0][0], "LOCKED          ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERResult[1][0], "UNLOCKED        ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERResult[2][0], "ANTENNA FAULT   ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERResult[3][0], "ANTENNA OK      ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERResult[4][0], "TSMF GODD       ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERResult[5][0], "TSMF_BAD        ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERResult[6][0], "TSMF_EWS_ON     ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERResult[7][0], "TSMF_EWS_OFF    ", 16);
	scChannelTestStringDictionary_TERResult[0][16] = '\0';
	scChannelTestStringDictionary_TERResult[1][16] = '\0';
	scChannelTestStringDictionary_TERResult[2][16] = '\0';
	scChannelTestStringDictionary_TERResult[3][16] = '\0';
	scChannelTestStringDictionary_TERResult[4][16] = '\0';
	scChannelTestStringDictionary_TERResult[5][16] = '\0';
	scChannelTestStringDictionary_TERResult[6][16] = '\0';
	scChannelTestStringDictionary_TERResult[7][16] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERBandwidth[0][0], "8MHz    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERBandwidth[1][0], "7MHz    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERBandwidth[2][0], "6MHz    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERBandwidth[3][0], "5MHz    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERBandwidth[4][0], "10MHz   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERBandwidth[5][0], "1P7MHz  ", 8);
	scChannelTestStringDictionary_TERBandwidth[0][8] = '\0';
	scChannelTestStringDictionary_TERBandwidth[1][8] = '\0';
	scChannelTestStringDictionary_TERBandwidth[2][8] = '\0';
	scChannelTestStringDictionary_TERBandwidth[3][8] = '\0';
	scChannelTestStringDictionary_TERBandwidth[4][8] = '\0';
	scChannelTestStringDictionary_TERBandwidth[5][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERSystem[0][0], "DVBT    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERSystem[1][0], "DVBT2   ", 8);
	scChannelTestStringDictionary_TERSystem[0][8] = '\0';
	scChannelTestStringDictionary_TERSystem[1][8] = '\0';


	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERCoderate[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERCoderate[1][0], "1/2T    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERCoderate[2][0], "2/3T    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERCoderate[3][0], "3/4T    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERCoderate[4][0], "5/6T    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERCoderate[5][0], "7/8T    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERCoderate[6][0], "3/5T2   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERCoderate[7][0], "4/5T2   ", 8);
	scChannelTestStringDictionary_TERCoderate[0][8] = '\0';
	scChannelTestStringDictionary_TERCoderate[1][8] = '\0';
	scChannelTestStringDictionary_TERCoderate[2][8] = '\0';
	scChannelTestStringDictionary_TERCoderate[3][8] = '\0';
	scChannelTestStringDictionary_TERCoderate[4][8] = '\0';
	scChannelTestStringDictionary_TERCoderate[5][8] = '\0';
	scChannelTestStringDictionary_TERCoderate[6][8] = '\0';
	scChannelTestStringDictionary_TERCoderate[7][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERGuardInterval[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERGuardInterval[1][0], "1/32T   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERGuardInterval[2][0], "1/16T   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERGuardInterval[3][0], "1/8T    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERGuardInterval[4][0], "1/4T    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERGuardInterval[5][0], "1/128T2 ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERGuardInterval[6][0], "19/128T2", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERGuardInterval[7][0], "19/256T2", 8);
	scChannelTestStringDictionary_TERGuardInterval[0][8] = '\0';
	scChannelTestStringDictionary_TERGuardInterval[1][8] = '\0';
	scChannelTestStringDictionary_TERGuardInterval[2][8] = '\0';
	scChannelTestStringDictionary_TERGuardInterval[3][8] = '\0';
	scChannelTestStringDictionary_TERGuardInterval[4][8] = '\0';
	scChannelTestStringDictionary_TERGuardInterval[5][8] = '\0';
	scChannelTestStringDictionary_TERGuardInterval[6][8] = '\0';
	scChannelTestStringDictionary_TERGuardInterval[7][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHierarchy[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHierarchy[1][0], "NONE    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHierarchy[2][0], "1       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHierarchy[3][0], "2       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHierarchy[4][0], "4       ", 8);
	scChannelTestStringDictionary_TERHierarchy[0][8] = '\0';
	scChannelTestStringDictionary_TERHierarchy[1][8] = '\0';
	scChannelTestStringDictionary_TERHierarchy[2][8] = '\0';
	scChannelTestStringDictionary_TERHierarchy[3][8] = '\0';
	scChannelTestStringDictionary_TERHierarchy[4][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERModulation[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERModulation[1][0], "QPSK    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERModulation[2][0], "16QAM   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERModulation[3][0], "64QAM   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERModulation[4][0], "256QAM  ", 8);
	scChannelTestStringDictionary_TERModulation[0][8] = '\0';
	scChannelTestStringDictionary_TERModulation[1][8] = '\0';
	scChannelTestStringDictionary_TERModulation[2][8] = '\0';
	scChannelTestStringDictionary_TERModulation[3][8] = '\0';
	scChannelTestStringDictionary_TERModulation[4][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TEROffset[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TEROffset[1][0], "NONE    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TEROffset[2][0], "1/6P    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TEROffset[3][0], "1/6N    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TEROffset[4][0], "2/6P    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TEROffset[5][0], "2/6N    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TEROffset[6][0], "3/6P    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TEROffset[7][0], "3/6N    ", 8);
	scChannelTestStringDictionary_TEROffset[0][8] = '\0';
	scChannelTestStringDictionary_TEROffset[1][8] = '\0';
	scChannelTestStringDictionary_TEROffset[2][8] = '\0';
	scChannelTestStringDictionary_TEROffset[3][8] = '\0';
	scChannelTestStringDictionary_TEROffset[4][8] = '\0';
	scChannelTestStringDictionary_TEROffset[5][8] = '\0';
	scChannelTestStringDictionary_TEROffset[6][8] = '\0';
	scChannelTestStringDictionary_TEROffset[7][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERStream[0][0], "HIGH PRI", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERStream[1][0], "LOW PRI ", 8);
	scChannelTestStringDictionary_TERStream[0][8] = '\0';
	scChannelTestStringDictionary_TERStream[1][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERTransmode[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERTransmode[1][0], "2K      ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERTransmode[2][0], "8K      ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERTransmode[3][0], "4K(T2)  ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERTransmode[4][0], "1K(T2)  ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERTransmode[5][0], "16K(T2) ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERTransmode[6][0], "32K(T2) ", 8);
	scChannelTestStringDictionary_TERTransmode[0][8] = '\0';
	scChannelTestStringDictionary_TERTransmode[1][8] = '\0';
	scChannelTestStringDictionary_TERTransmode[2][8] = '\0';
	scChannelTestStringDictionary_TERTransmode[3][8] = '\0';
	scChannelTestStringDictionary_TERTransmode[4][8] = '\0';
	scChannelTestStringDictionary_TERTransmode[5][8] = '\0';
	scChannelTestStringDictionary_TERTransmode[6][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPreambleFormat[0][0], "SISO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPreambleFormat[1][0], "MISO    ", 8);
	scChannelTestStringDictionary_TERPreambleFormat[0][8] = '\0';
	scChannelTestStringDictionary_TERPreambleFormat[1][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPreambleMixed[0][0], "NOT MIX ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPreambleMixed[1][0], "MIXED   ", 8);
	scChannelTestStringDictionary_TERPreambleMixed[0][8] = '\0';
	scChannelTestStringDictionary_TERPreambleMixed[1][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[1][0], "1       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[2][0], "2       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[3][0], "3       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[4][0], "4       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[5][0], "5       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[6][0], "6       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[7][0], "7       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPilotPattern[8][0], "8       ", 8);

	scChannelTestStringDictionary_TERPilotPattern[0][8] = '\0';
	scChannelTestStringDictionary_TERPilotPattern[1][8] = '\0';
	scChannelTestStringDictionary_TERPilotPattern[2][8] = '\0';
	scChannelTestStringDictionary_TERPilotPattern[3][8] = '\0';
	scChannelTestStringDictionary_TERPilotPattern[4][8] = '\0';
	scChannelTestStringDictionary_TERPilotPattern[5][8] = '\0';
	scChannelTestStringDictionary_TERPilotPattern[6][8] = '\0';
	scChannelTestStringDictionary_TERPilotPattern[7][8] = '\0';
	scChannelTestStringDictionary_TERPilotPattern[8][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERExtendedCarrier[0][0], "NOT USE ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERExtendedCarrier[1][0], "USED    ", 8);
	scChannelTestStringDictionary_TERExtendedCarrier[0][8] = '\0';
	scChannelTestStringDictionary_TERExtendedCarrier[1][8] = '\0';
	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPaprMode[0][0], "NONE    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPaprMode[1][0], "ACE     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPaprMode[2][0], "TR      ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERPaprMode[3][0], "ACETR   ", 8);

	scChannelTestStringDictionary_TERPaprMode[0][8] = '\0';
	scChannelTestStringDictionary_TERPaprMode[1][8] = '\0';
	scChannelTestStringDictionary_TERPaprMode[2][8] = '\0';
	scChannelTestStringDictionary_TERPaprMode[3][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[0][0], "Request Id          ", 20);
	scChannelTestStringDictionary_TERHead[0][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[1][0], "Tuner Status        ", 20);
	scChannelTestStringDictionary_TERHead[1][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[2][0], "Tuner Id            ", 20);
	scChannelTestStringDictionary_TERHead[2][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[3][0], "Frequency           ", 20);
	scChannelTestStringDictionary_TERHead[3][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[4][0], "Bandwidth           ", 20);
	scChannelTestStringDictionary_TERHead[4][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[5][0], "System              ", 20);
	scChannelTestStringDictionary_TERHead[5][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[6][0], "CodeRate            ", 20);
	scChannelTestStringDictionary_TERHead[6][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[7][0], "Guard Interval      ", 20);
	scChannelTestStringDictionary_TERHead[7][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[8][0], "Hierarchy           ", 20);
	scChannelTestStringDictionary_TERHead[8][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[9][0], "Modulation          ", 20);
	scChannelTestStringDictionary_TERHead[9][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[10][0], "Offset              ", 20);
	scChannelTestStringDictionary_TERHead[10][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[11][0], "Stream              ", 20);
	scChannelTestStringDictionary_TERHead[11][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[12][0], "Transmode           ", 20);
	scChannelTestStringDictionary_TERHead[12][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[13][0], "PlpIp               ", 20);
	scChannelTestStringDictionary_TERHead[13][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[14][0], "Preamble Format     ", 20);
	scChannelTestStringDictionary_TERHead[14][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[15][0], "Preamble Mixed      ", 20);
	scChannelTestStringDictionary_TERHead[15][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[16][0], "Pilot Pattern       ", 20);
	scChannelTestStringDictionary_TERHead[16][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[17][0], "Extended Carrier    ", 20);
	scChannelTestStringDictionary_TERHead[17][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[18][0], "Papr Mode           ", 20);
	scChannelTestStringDictionary_TERHead[18][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[19][0], "Plp Num Blocks      ", 20);
	scChannelTestStringDictionary_TERHead[19][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[20][0], "Signal Strength     ", 20);
	scChannelTestStringDictionary_TERHead[20][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[21][0], "Signal Quality      ", 20);
	scChannelTestStringDictionary_TERHead[21][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[22][0], "AGC                 ", 20);
	scChannelTestStringDictionary_TERHead[22][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[23][0], "BER                 ", 20);
	scChannelTestStringDictionary_TERHead[23][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[24][0], "SNR                 ", 20);
	scChannelTestStringDictionary_TERHead[24][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[25][0], "Emergency Flag      ", 20);
	scChannelTestStringDictionary_TERHead[25][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[26][0], "Corrected No        ", 20);
	scChannelTestStringDictionary_TERHead[26][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[27][0], "Uncorrected No      ", 20);
	scChannelTestStringDictionary_TERHead[27][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[28][0], "UnErrored No        ", 20);
	scChannelTestStringDictionary_TERHead[28][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[29][0], "InBand BER          ", 20);
	scChannelTestStringDictionary_TERHead[29][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[30][0], "UnCorrected Flag    ", 20);
	scChannelTestStringDictionary_TERHead[30][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[31][0], "RF AGC              ", 20);
	scChannelTestStringDictionary_TERHead[31][20] = '\0';	    
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[32][0], "IF AGC              ", 20);
	scChannelTestStringDictionary_TERHead[32][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[33][0], "Signal Power        ", 20);
	scChannelTestStringDictionary_TERHead[33][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_TERHead[34][0], "PAT Received        ", 20);
	scChannelTestStringDictionary_TERHead[34][20] = '\0';	    
}
	

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_TER_ParsingInfo
(
	unsigned int inData, 
	unsigned int data_offset, 
	CMD_HWTEST_EXT_CHANNEL_TEST_TER_INFO *pTERInfo
)
{
	if ( pTERInfo == NULL )
	{
		printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	switch(data_offset)
	{
		case 0:/* Tuner Type */
			if ( inData != CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->tunerType = CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER;
			break;
		case 1: /* Tuner Id */
			if ( inData > HI_CH_GetNumOfInstance() )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->tunerId = inData;
			break;						
		case 2:/* Frequency */
			pTERInfo->frequencykHz= inData;			
			break;
		case 3: /* bandwidth */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_BANDWIDTH_1P7MHZ)
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->bandwidth= inData;			
			break;
		case 4: /*System */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_SYSTEM_DVBT2)
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->system = inData;			
			break;
		case 5: /* code rate */			
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_CODERATE_4OVER5T2)
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}			
			pTERInfo->codeRate = inData;			
			break;
		case 6: /* guard interval*/
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_GUARDINTERVAL_19OVER256T2 )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}	

			pTERInfo->guardInterval= inData;			
			break;
		case 7: /* Hierarchy */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_HIERARCHY_4 )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->hierarchy= inData;			
			break;			
		case 8: /* Modulation  */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_MODULATION_256QAM )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->modulation = inData;			
			break;	
		case 9: /* offset */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_OFFSET_3OVER6N )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->offset = inData;			
			break;				
		case 10: /* stream */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_STREAM_LOW_PRIORITY )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->stream= inData;			
			break;
		case 11: /* transmode */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_TRANSMODE_32KT2)
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->transMode= inData;			
			break;			
		case 12: /* plpId */
			pTERInfo->plpip= inData;			
			break;				
		case 13: /* preamble format */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_PREAMBLE_FORMAT_SISO )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->preambleformat = inData;			
			break;			
		case 14: /* preamble mixed */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_PREAMBLE_MIXED_MIXED)
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->preamblemixed = inData;			
			break;			
		case 15: /* pilot pattern */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_PILOT_PATTERN_8)
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pTERInfo->pilotpattern= inData;			
			break;		
		case 16: /* Extended Carrier */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_EXTENDEDCARRIER_USED )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}

			pTERInfo->extendedCarrier = inData;
			break;
		case 17:
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TER_PAPRMODE_ACETR )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}			
			pTERInfo->paprmode= inData;
			break;
		case 18: 
			pTERInfo->plpnumblocks= inData;
			break;
		default:
			return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			break;			
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
	
}

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_TER_CreateTERLogFile
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
	
		fprintf(fpPid, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
				&scChannelTestStringDictionary_TERHead[0][0], 
				&scChannelTestStringDictionary_TERHead[1][0], 
				&scChannelTestStringDictionary_TERHead[2][0], 
				&scChannelTestStringDictionary_TERHead[3][0], 
				&scChannelTestStringDictionary_TERHead[4][0], 
				&scChannelTestStringDictionary_TERHead[5][0], 
				&scChannelTestStringDictionary_TERHead[6][0], 
				&scChannelTestStringDictionary_TERHead[7][0], 
				&scChannelTestStringDictionary_TERHead[8][0], 
				&scChannelTestStringDictionary_TERHead[9][0], 				
				&scChannelTestStringDictionary_TERHead[10][0], 
				&scChannelTestStringDictionary_TERHead[11][0], 
				&scChannelTestStringDictionary_TERHead[12][0], 
				&scChannelTestStringDictionary_TERHead[13][0], 
				&scChannelTestStringDictionary_TERHead[14][0], 
				&scChannelTestStringDictionary_TERHead[15][0], 
				&scChannelTestStringDictionary_TERHead[16][0], 
				&scChannelTestStringDictionary_TERHead[17][0], 
				&scChannelTestStringDictionary_TERHead[18][0], 
				&scChannelTestStringDictionary_TERHead[19][0]);		


		printf("fclose-begin\n");		
	}
	else if ( (testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST)  ||
	          (testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST) )
	{
			fprintf(fpPid, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
				&scChannelTestStringDictionary_TERHead[0][0], 
				&scChannelTestStringDictionary_TERHead[1][0], 
				&scChannelTestStringDictionary_TERHead[2][0], 
				&scChannelTestStringDictionary_TERHead[3][0], 
				&scChannelTestStringDictionary_TERHead[4][0], 
				&scChannelTestStringDictionary_TERHead[5][0], 
				&scChannelTestStringDictionary_TERHead[6][0], 
				&scChannelTestStringDictionary_TERHead[7][0], 
				&scChannelTestStringDictionary_TERHead[8][0], 
				&scChannelTestStringDictionary_TERHead[9][0], 				
				&scChannelTestStringDictionary_TERHead[10][0], 
				&scChannelTestStringDictionary_TERHead[11][0], 
				&scChannelTestStringDictionary_TERHead[12][0], 
				&scChannelTestStringDictionary_TERHead[13][0], 
				&scChannelTestStringDictionary_TERHead[14][0], 
				&scChannelTestStringDictionary_TERHead[15][0], 
				&scChannelTestStringDictionary_TERHead[16][0], 
				&scChannelTestStringDictionary_TERHead[17][0], 
				&scChannelTestStringDictionary_TERHead[18][0], 
				&scChannelTestStringDictionary_TERHead[19][0],
				&scChannelTestStringDictionary_TERHead[20][0], 
				&scChannelTestStringDictionary_TERHead[21][0], 
				&scChannelTestStringDictionary_TERHead[22][0], 
				&scChannelTestStringDictionary_TERHead[23][0], 
				&scChannelTestStringDictionary_TERHead[24][0], 
				&scChannelTestStringDictionary_TERHead[25][0], 
				&scChannelTestStringDictionary_TERHead[26][0], 
				&scChannelTestStringDictionary_TERHead[27][0], 
				&scChannelTestStringDictionary_TERHead[28][0], 
				&scChannelTestStringDictionary_TERHead[29][0],
				&scChannelTestStringDictionary_TERHead[30][0],
				&scChannelTestStringDictionary_TERHead[31][0],
				&scChannelTestStringDictionary_TERHead[32][0],
				&scChannelTestStringDictionary_TERHead[33][0],
				&scChannelTestStringDictionary_TERHead[34][0]);
	}
	fsync(fpPid);
	fclose(fpPid);	
}




