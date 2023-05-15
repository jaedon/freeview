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

#include "osd.h"
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

static char	scChannelTestStringDictionary_SATHead[35][21];
static char scChannelTestStringDictionary_SATType[1][9];
static char scChannelTestStringDictionary_SATResult[8][17];
static char scChannelTestStringDictionary_SATAntennaType[4][9];
static char scChannelTestStringDictionary_SATTransSpec[2][9];
static char scChannelTestStringDictionary_SATModulation[3][9];
static char scChannelTestStringDictionary_SATPolarization[5][9];
static char scChannelTestStringDictionary_SATDiseqcInput[9][9];
static char scChannelTestStringDictionary_SATToneBurst[3][9];
static char scChannelTestStringDictionary_SAT22KTone[2][9];
static char scChannelTestStringDictionary_SATFEC[12][9];
static char scChannelTestStringDictionary_SATDiseqcVersion[5][9];
static char scChannelTestStringDictionary_SATLNBVoltage[2][9];
static char scChannelTestStringDictionary_SATRollOff[3][9];
static char scChannelTestStringDictionary_SATPilot[3][9];


static unsigned int sat_pat_received_ok = 0;		



static void internal_cmdhwtestext_ChannelTest_SAT_PATCallback(void)
{ 
	sat_pat_received_ok++;		
	printf("internal_cmdhwtestext_ChannelTest_SAT_PATCallback is called\n");
}

void P_CmdHwTestExt_ChannelTest_SAT_ChannelInfoDisplay(CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO satInfo, unsigned int mode)
{
	char *pInfoDisplayString = HI_VK_MEM_Alloc(501);
	if ( mode == 0 )
	{
		VK_MEM_Memset(pInfoDisplayString, ' ', 500);
		sprintf(pInfoDisplayString, "Tuner: %lu, Freq: %lu MHz\nSymbol: %lu, FEC : %s\n			Try to Lock\n", 
				satInfo.tunerId, 
				satInfo.frequencyMHz,
				satInfo.symbolRate,
				&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0]);
			
		pInfoDisplayString[500] = '\0';
	}
	else if ( mode == 1 )
	{
		VK_MEM_Memset(pInfoDisplayString, ' ', 500);
		sprintf(pInfoDisplayString, "Tuner: %u, Freq: %lu MHz\nSymbol : %lu, FEC : %s\n			Try to Lock Error\n", 
				satInfo.tunerId, 
				satInfo.frequencyMHz,
				satInfo.symbolRate,
				&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0]);
			
		pInfoDisplayString[500] = '\0';	
	}
	AP_HwTest_DisplayInfo(pInfoDisplayString);
	HI_VK_MEM_Free(pInfoDisplayString);
}

void P_CmdHwTestExt_ChannelTest_SAT_NodeDisplayString(char *pReturnString, int offset, CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO satInfo)
{
	sprintf(pReturnString, "%02d: %lu,%lu,%lu,%s,%s,%s,%s,%s#", 
		offset, 
		satInfo.tunerId,
		satInfo.frequencyMHz,
		satInfo.symbolRate,
		&scChannelTestStringDictionary_SATDiseqcInput[satInfo.diseqcIn][0],
		&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0],
		&scChannelTestStringDictionary_SATPolarization[satInfo.polarization][0],
		&scChannelTestStringDictionary_SATTransSpec[satInfo.transSpec][0],						
		&scChannelTestStringDictionary_SATDiseqcVersion[satInfo.diseqcVer][0]);	

}

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_LockTry(
	CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO satInfo,
	unsigned long requestId)
{
	HI_TuneParams tuneParams;
				
	tuneParams.satParams.etAntennaType = satInfo.antennaType  ;
	tuneParams.satParams.ulFrequency = satInfo.frequencyMHz;
	tuneParams.satParams.ulSymbolRate = satInfo.symbolRate;
	tuneParams.satParams.etFecCodeRate = satInfo.fecValue ;
	tuneParams.satParams.etPolarization = satInfo.polarization ;
	tuneParams.satParams.etTransSpec = satInfo.transSpec;
	tuneParams.satParams.etModulation= satInfo.modulation;
	tuneParams.satParams.etPilot= satInfo.pilotValue;
	tuneParams.satParams.etRollOff= satInfo.rolloff;
	tuneParams.satParams.etLnbVoltage= satInfo.lnbVoltage;
	tuneParams.satParams.etDiseqcVersion= satInfo.diseqcVer;
	tuneParams.satParams.etDiseqcInput= satInfo.diseqcIn;
	tuneParams.satParams.etToneBurst= satInfo.toneBurst;
	tuneParams.satParams.b22kTone= satInfo.b22kTone;
	tuneParams.satParams.ulScdMasterUnitId= satInfo.scdMasterUnitID;
	tuneParams.satParams.ulScdUserBandNum= satInfo.scdUserBandNum;
	tuneParams.satParams.ulScdUserBandFreq= satInfo.scdUserBandFreq;
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	if ( HI_CH_DI_RequestLock(requestId, 
		satInfo.tunerId,
		HI_CH_TYPE_SAT,
		tuneParams) != 0 )
	{
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OPERATION_ERROR;
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}
	

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_ResultProcess
(
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE curType,
	HI_CH_SignalEvents chSignalEvent,
	CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO satInfo,
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

		sat_pat_received_ok = 0;
		if ( chSignalEvent == HI_CH_SIGNAL_LOCKED )
		{
			pTestStatusInfo->scan_total_locked_num++;
		}
		VK_MEM_Memset(pShortMsgDisplay, ' ', 200);
		sprintf(pShortMsgDisplay, "%05d/%05d:%s\n", 
			pTestStatusInfo->test_total_cur_node_num, 
			pTestStatusInfo->test_total_num_of_node,
			&scChannelTestStringDictionary_SATResult[chSignalEvent][0]);

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
			//CMD_RegisterDemuxTestCallback(internal_cmdhwtestext_ChannelTest_SAT_PATCallback);						
			P_DEMUX_SectionFilterTest(1,0,satInfo.tunerId);
			for ( tempCount = 0 ; tempCount < 7 ; tempCount ++ )
			{
				HI_CH_DI_GetSignalInfo(
					satInfo.tunerType, 
					satInfo.tunerId, 
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
				if ( sat_pat_received_ok != 1 )
				{
					sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu MHz, Symbol: %lu, FEC: %s\nStrength: %lu, Quality: %lu, Power: %f\nAGC: %lu, RF_AGC: %lu, IF_AGC: %lu\nBER: %f, SNR: %f, PAT Received\n", 
						satInfo.tunerId, 
						satInfo.frequencyMHz,
						satInfo.symbolRate,
						&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0],
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
					sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu MHz, Symbol: %lu, FEC: %s\nStrength: %lu, Quality: %lu, Power: %f\nAGC: %lu, RF_AGC: %lu, IF_AGC: %lu\nBER: %f, SNR: %f, No PAT Received\n", 

						satInfo.tunerId, 
						satInfo.frequencyMHz,
						satInfo.symbolRate,
						&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0],
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
			P_DEMUX_SectionFilterTest(0,0,satInfo.tunerId);
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

			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu MHz\nSymbol: %lu, FEC : %s\n             Unlocked\n", 
				satInfo.tunerId, 
				satInfo.frequencyMHz,
				satInfo.symbolRate,
				&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0]);						
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
		if  ( sat_pat_received_ok != 0 )
		{
			fprintf(fpPid, "%08x\t%s\t%08x\t%s\t%s\t%s\t%lu\t%lu\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%f\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%s\n",
				requestId, 
				&scChannelTestStringDictionary_SATResult[chSignalEvent][0],
				satInfo.tunerId,
				&scChannelTestStringDictionary_SATAntennaType[satInfo.antennaType][0],
				&scChannelTestStringDictionary_SATTransSpec[satInfo.transSpec][0],
				&scChannelTestStringDictionary_SATModulation[satInfo.modulation][0],
				satInfo.frequencyMHz,
				satInfo.symbolRate,
				&scChannelTestStringDictionary_SATPolarization[satInfo.polarization][0],
				&scChannelTestStringDictionary_SATDiseqcInput[satInfo.diseqcIn][0],
				&scChannelTestStringDictionary_SATToneBurst[satInfo.toneBurst][0],				
				&scChannelTestStringDictionary_SAT22KTone[satInfo.b22kTone][0],				
				&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0],				
				&scChannelTestStringDictionary_SATDiseqcVersion[satInfo.diseqcVer][0],				
				&scChannelTestStringDictionary_SATLNBVoltage[satInfo.lnbVoltage][0],					
				&scChannelTestStringDictionary_SATRollOff[satInfo.rolloff][0],				
				&scChannelTestStringDictionary_SATPilot[satInfo.pilotValue][0],		
				satInfo.scdMasterUnitID,
				satInfo.scdUserBandNum,
				satInfo.scdUserBandFreq,
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
			fprintf(fpPid, "%08x\t%s\t%08x\t%s\t%s\t%s\t%lu\t%lu\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%f\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%lu\t%f\t%s\n",
				requestId, 
				&scChannelTestStringDictionary_SATResult[chSignalEvent][0],
				satInfo.tunerId,
				&scChannelTestStringDictionary_SATAntennaType[satInfo.antennaType][0],
				&scChannelTestStringDictionary_SATTransSpec[satInfo.transSpec][0],
				&scChannelTestStringDictionary_SATModulation[satInfo.modulation][0],
				satInfo.frequencyMHz,
				satInfo.symbolRate,
				&scChannelTestStringDictionary_SATPolarization[satInfo.polarization][0],
				&scChannelTestStringDictionary_SATDiseqcInput[satInfo.diseqcIn][0],
				&scChannelTestStringDictionary_SATToneBurst[satInfo.toneBurst][0],				
				&scChannelTestStringDictionary_SAT22KTone[satInfo.b22kTone][0],				
				&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0],				
				&scChannelTestStringDictionary_SATDiseqcVersion[satInfo.diseqcVer][0],				
				&scChannelTestStringDictionary_SATLNBVoltage[satInfo.lnbVoltage][0],					
				&scChannelTestStringDictionary_SATRollOff[satInfo.rolloff][0],				
				&scChannelTestStringDictionary_SATPilot[satInfo.pilotValue][0],		
				satInfo.scdMasterUnitID,
				satInfo.scdUserBandNum,
				satInfo.scdUserBandFreq,
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
        sat_pat_received_ok  =0;
	}
	else if ( (curType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST)  )
	{
		sat_pat_received_ok = 0;
		
		if ( chSignalEvent == HI_CH_SIGNAL_LOCKED )
		{
			pTestStatusInfo->scan_total_locked_num++;
		}
		VK_MEM_Memset(pShortMsgDisplay, ' ', 200);
		sprintf(pShortMsgDisplay, "%05d/%05d:%s\n", 
			pTestStatusInfo->test_total_cur_node_num, 
			pTestStatusInfo->test_total_num_of_node,
			&scChannelTestStringDictionary_SATResult[chSignalEvent][0]);
        printf("satResult(%d,%s,%s)\n", chSignalEvent, &scChannelTestStringDictionary_SATResult[chSignalEvent][0],&scChannelTestStringDictionary_SATResult[0][0]);
        

		pShortMsgDisplay[200] = '\0';
		P_SendNewMsgStringToHandler(pShortMsgDisplay);
		
		P_SendNewProgressMsgStringToHandler(
			"Channel Test is now ... \n", 
			((pTestStatusInfo->test_total_cur_node_num*100)/(pTestStatusInfo->test_total_num_of_node)));

		HI_VK_TASK_Sleep(200);

		if (chSignalEvent == HI_CH_SIGNAL_LOCKED)
		{
			VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);
			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu MHz\nSymbol: %lu, FEC : %s\n            Locked\n", 
				satInfo.tunerId, 
				satInfo.frequencyMHz,
				satInfo.symbolRate,
				&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0]);


			pLongMsgDisplay[4096] = '\0';
			AP_HwTest_DisplayInfo(pLongMsgDisplay);						
			HI_VK_TASK_Sleep(1000);
		}
		else
		{
			VK_MEM_Memset(pLongMsgDisplay, ' ', 4096);
			sprintf(pLongMsgDisplay, "Tuner: %lu, Freq: %lu MHz\nSymbol: %lu, FEC : %s\n             Unlocked\n", 
				satInfo.tunerId, 
				satInfo.frequencyMHz,
				satInfo.symbolRate,
				&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0]);						
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
		fprintf(fpPid, "%08x\t%s\t%08x\t%s\t%s\t%s\t%lu\t%lu\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%lu\t%lu\t%lu\n",
				requestId, 
				&scChannelTestStringDictionary_SATResult[chSignalEvent][0],
				satInfo.tunerId,
				&scChannelTestStringDictionary_SATAntennaType[satInfo.antennaType][0],
				&scChannelTestStringDictionary_SATTransSpec[satInfo.transSpec][0],
				&scChannelTestStringDictionary_SATModulation[satInfo.modulation][0],
				satInfo.frequencyMHz,
				satInfo.symbolRate,
				&scChannelTestStringDictionary_SATPolarization[satInfo.polarization][0],
				&scChannelTestStringDictionary_SATDiseqcInput[satInfo.diseqcIn][0],
				&scChannelTestStringDictionary_SATToneBurst[satInfo.toneBurst][0],				
				&scChannelTestStringDictionary_SAT22KTone[satInfo.b22kTone][0],				
				&scChannelTestStringDictionary_SATFEC[satInfo.fecValue][0],				
				&scChannelTestStringDictionary_SATDiseqcVersion[satInfo.diseqcVer][0],				
				&scChannelTestStringDictionary_SATLNBVoltage[satInfo.lnbVoltage][0],					
				&scChannelTestStringDictionary_SATRollOff[satInfo.rolloff][0],				
				&scChannelTestStringDictionary_SATPilot[satInfo.pilotValue][0],		
				satInfo.scdMasterUnitID,
				satInfo.scdUserBandNum,
				satInfo.scdUserBandFreq);	

		fclose(fpPid);
		HI_VK_MEM_Free(pWriteBuffer);
		HI_VK_MEM_Free(pLongMsgDisplay);
		HI_VK_MEM_Free(pShortMsgDisplay);		

	
	}
		

	HI_CH_DI_CancelLock(satInfo.tunerId, satInfo.tunerType);	
	
}






CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_InitStringDictionary(void)
{
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATType[0][0], "	  SAT", 8);
	scChannelTestStringDictionary_SATType[0][8] = '\0';
	


	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATAntennaType[0][0], "LNB     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATAntennaType[1][0], "DISEQC  ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATAntennaType[2][0], "SCD     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATAntennaType[3][0], "SMATV   ", 8);
	scChannelTestStringDictionary_SATAntennaType[0][8] = '\0';
	scChannelTestStringDictionary_SATAntennaType[1][8] = '\0';
	scChannelTestStringDictionary_SATAntennaType[2][8] = '\0';
	scChannelTestStringDictionary_SATAntennaType[3][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATTransSpec[0][0], "DVBS    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATTransSpec[1][0], "DVBS2   ", 8);
	scChannelTestStringDictionary_SATTransSpec[0][8] = '\0';
	scChannelTestStringDictionary_SATTransSpec[1][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATModulation[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATModulation[1][0], "QPSK    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATModulation[2][0], "8PSK    ", 8);
	scChannelTestStringDictionary_SATModulation[0][8] = '\0';
	scChannelTestStringDictionary_SATModulation[1][8] = '\0';
	scChannelTestStringDictionary_SATModulation[2][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATPolarization[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATPolarization[1][0], "HOR     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATPolarization[2][0], "VER     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATPolarization[3][0], "LEFT    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATPolarization[4][0], "RIGHT   ", 8);
	scChannelTestStringDictionary_SATPolarization[0][8] = '\0';
	scChannelTestStringDictionary_SATPolarization[1][8] = '\0';
	scChannelTestStringDictionary_SATPolarization[2][8] = '\0';
	scChannelTestStringDictionary_SATPolarization[3][8] = '\0';
	scChannelTestStringDictionary_SATPolarization[4][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[0][0], "NONE    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[1][0], "A       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[2][0], "B       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[3][0], "C       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[4][0], "D       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[5][0], "TB_A    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[6][0], "TB_B    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[7][0], "SCD_A   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcInput[8][0], "SCD_B   ", 8);
	scChannelTestStringDictionary_SATDiseqcInput[0][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcInput[1][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcInput[2][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcInput[3][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcInput[4][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcInput[5][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcInput[6][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcInput[7][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcInput[8][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATToneBurst[0][0], "NONE    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATToneBurst[1][0], "A       ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATToneBurst[2][0], "B       ", 8);
	scChannelTestStringDictionary_SATToneBurst[0][8] = '\0';
	scChannelTestStringDictionary_SATToneBurst[1][8] = '\0';
	scChannelTestStringDictionary_SATToneBurst[2][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SAT22KTone[0][0], "OFF     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SAT22KTone[1][0], "ON      ", 8);
	scChannelTestStringDictionary_SAT22KTone[0][8] = '\0';
	scChannelTestStringDictionary_SAT22KTone[1][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[1][0], "1/2S    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[2][0], "2/3S    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[3][0], "3/4S    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[4][0], "5/6S    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[5][0], "7/8S    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[6][0], "3/5(S2) ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[7][0], "4/5(S2) ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[8][0], "8/9(S2) ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[9][0], "9/10(S2)", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[10][0], "5/11S   ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATFEC[11][0], "6/7S    ", 8);		
	scChannelTestStringDictionary_SATFEC[0][8] = '\0';
	scChannelTestStringDictionary_SATFEC[1][8] = '\0';
	scChannelTestStringDictionary_SATFEC[2][8] = '\0';
	scChannelTestStringDictionary_SATFEC[3][8] = '\0';
	scChannelTestStringDictionary_SATFEC[4][8] = '\0';
	scChannelTestStringDictionary_SATFEC[5][8] = '\0';
	scChannelTestStringDictionary_SATFEC[6][8] = '\0';
	scChannelTestStringDictionary_SATFEC[7][8] = '\0';
	scChannelTestStringDictionary_SATFEC[8][8] = '\0';
	scChannelTestStringDictionary_SATFEC[9][8] = '\0';
	scChannelTestStringDictionary_SATFEC[10][8] = '\0';
	scChannelTestStringDictionary_SATFEC[11][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcVersion[0][0], "1.0    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcVersion[1][0], "1.1    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcVersion[2][0], "1.2    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcVersion[3][0], "1.3    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATDiseqcVersion[4][0], "2.0    ", 8);
	scChannelTestStringDictionary_SATDiseqcVersion[0][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcVersion[1][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcVersion[2][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcVersion[3][8] = '\0';
	scChannelTestStringDictionary_SATDiseqcVersion[4][8] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATLNBVoltage[0][0], "STANDARD", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATLNBVoltage[1][0], "HIGH    ", 8);
	scChannelTestStringDictionary_SATLNBVoltage[0][8] = '\0';
	scChannelTestStringDictionary_SATLNBVoltage[1][8] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATRollOff[0][0], "020     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATRollOff[1][0], "025     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATRollOff[2][0], "035     ", 8);
	scChannelTestStringDictionary_SATRollOff[0][8] = '\0';
	scChannelTestStringDictionary_SATRollOff[1][8] = '\0';
	scChannelTestStringDictionary_SATRollOff[2][8] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATPilot[0][0], "AUTO    ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATPilot[1][0], "OFF     ", 8);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATPilot[2][0], "ON      ", 8);
	scChannelTestStringDictionary_SATPilot[0][8] = '\0';
	scChannelTestStringDictionary_SATPilot[1][8] = '\0';
	scChannelTestStringDictionary_SATPilot[2][8] = '\0';

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[0][0], "Request Id          ", 20);
	scChannelTestStringDictionary_SATHead[0][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[1][0], "Tuner Status        ", 20);
	scChannelTestStringDictionary_SATHead[1][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[2][0], "Tuner Id            ", 20);
	scChannelTestStringDictionary_SATHead[2][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[3][0], "Antenna Type        ", 20);
	scChannelTestStringDictionary_SATHead[3][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[4][0], "Trans Spce          ", 20);
	scChannelTestStringDictionary_SATHead[4][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[5][0], "Modulation          ", 20);
	scChannelTestStringDictionary_SATHead[5][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[6][0], "Frequency           ", 20);
	scChannelTestStringDictionary_SATHead[6][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[7][0], "SymbolRate          ", 20);
	scChannelTestStringDictionary_SATHead[7][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[8][0], "Polarization         ", 20);
	scChannelTestStringDictionary_SATHead[8][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[9][0], "Diseqc Input         ", 20);
	scChannelTestStringDictionary_SATHead[9][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[10][0], "Tone Burst          ", 20);
	scChannelTestStringDictionary_SATHead[10][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[11][0], "22K Tone            ", 20);
	scChannelTestStringDictionary_SATHead[11][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[12][0], "FEC Coderate        ", 20);
	scChannelTestStringDictionary_SATHead[12][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[13][0], "Diseqc Version      ", 20);
	scChannelTestStringDictionary_SATHead[13][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[14][0], "LNB Voltage         ", 20);
	scChannelTestStringDictionary_SATHead[14][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[15][0], "Roll Off            ", 20);
	scChannelTestStringDictionary_SATHead[15][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[16][0], "Pilot               ", 20);
	scChannelTestStringDictionary_SATHead[16][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[17][0], "SCD Master UNIT ID  ", 20);
	scChannelTestStringDictionary_SATHead[17][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[18][0], "SCD User Band Num   ", 20);
	scChannelTestStringDictionary_SATHead[18][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[19][0], "SCD User Band Freq  ", 20);
	scChannelTestStringDictionary_SATHead[19][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[20][0], "Signal Strength     ", 20);
	scChannelTestStringDictionary_SATHead[20][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[21][0], "Signal Quality      ", 20);
	scChannelTestStringDictionary_SATHead[21][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[22][0], "AGC                 ", 20);
	scChannelTestStringDictionary_SATHead[22][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[23][0], "BER                 ", 20);
	scChannelTestStringDictionary_SATHead[23][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[24][0], "SNR                 ", 20);
	scChannelTestStringDictionary_SATHead[24][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[25][0], "Emergency Flag      ", 20);
	scChannelTestStringDictionary_SATHead[25][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[26][0], "Corrected No        ", 20);
	scChannelTestStringDictionary_SATHead[26][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[27][0], "Uncorrected No      ", 20);
	scChannelTestStringDictionary_SATHead[27][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[28][0], "UnErrored No        ", 20);
	scChannelTestStringDictionary_SATHead[28][20] = '\0';
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[29][0], "InBand BER          ", 20);
	scChannelTestStringDictionary_SATHead[29][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[30][0], "UnCorrected Flag    ", 20);
	scChannelTestStringDictionary_SATHead[30][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[31][0], "RF AGC              ", 20);
	scChannelTestStringDictionary_SATHead[31][20] = '\0';		
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[32][0], "IF AGC              ", 20);
	scChannelTestStringDictionary_SATHead[32][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[33][0], "Signal Power        ", 20);
	scChannelTestStringDictionary_SATHead[33][20] = '\0';	
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATHead[34][0], "PAT Received        ", 20);
	scChannelTestStringDictionary_SATHead[34][20] = '\0';		

	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATResult[0][0], "LOCKED          ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATResult[1][0], "UNLOCKED        ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATResult[2][0], "ANTENNA FAULT   ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATResult[3][0], "ANTENNA OK      ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATResult[4][0], "TSMF GODD       ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATResult[5][0], "TSMF_BAD        ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATResult[6][0], "TSMF_EWS_ON     ", 16);
	HI_VK_MEM_Memcpy(&scChannelTestStringDictionary_SATResult[7][0], "TSMF_EWS_OFF    ", 16);
	
	scChannelTestStringDictionary_SATResult[0][16] = '\0';
	scChannelTestStringDictionary_SATResult[1][16] = '\0';
	scChannelTestStringDictionary_SATResult[2][16] = '\0';
	scChannelTestStringDictionary_SATResult[3][16] = '\0';
	scChannelTestStringDictionary_SATResult[4][16] = '\0';
	scChannelTestStringDictionary_SATResult[5][16] = '\0';
	scChannelTestStringDictionary_SATResult[6][16] = '\0';
	scChannelTestStringDictionary_SATResult[7][16] = '\0';
    printf("====== %s ============\n", &scChannelTestStringDictionary_SATResult[0][0]);
    
}
	

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_ParsingInfo
(
	unsigned int inData, 
	unsigned int data_offset, 
	CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO *pSatInfo
)
{
	if ( pSatInfo == NULL )
	{
		printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	switch(data_offset)
	{
		case 0:/* Tuner Type */
			if ( inData != CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->tunerType = CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT;
			break;
		case 1: /* Tuner Id */
			if ( inData > HI_CH_GetNumOfInstance() )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->tunerId = inData;
			break;						
		case 2:/* Antenna Type */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_TEST_SAT_ANTENNA_TYPE_SMATV )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->antennaType = inData;			
			break;
		case 3: /* Trans Spec */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_TRANSSPEC_DVBS2 )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->transSpec = inData;			
			break;
		case 4: /* Modulation */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_MODULATION_8PSK )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->modulation = inData;			
			break;
		case 5: /* Frequency */			
			pSatInfo->frequencyMHz = inData;			
			break;
		case 6: /* Symbol Rate */
			pSatInfo->symbolRate = inData;			
			break;
		case 7: /* Polarization */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_POLARITY_RIGHT )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->polarization = inData;			
			break;			
		case 8: /* Diseqc In */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_DISEQC_IN_SCDB )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->diseqcIn = inData;			
			break;	
		case 9: /* Tone Burst */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_TONEBURST_B )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->toneBurst = inData;			
			break;				
		case 10: /* 22K Burst */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_22K_TONE_ON )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->b22kTone = inData;			
			break;
		case 11: /* FEC */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_FEC_6OVER7 )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->fecValue = inData;			
			break;			
		case 12: /* FEC */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_DISEQC_VER_20 )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->diseqcVer = inData;			
			break;				
		case 13: /* FEC */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_LNB_VOL_HIGH )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->lnbVoltage = inData;			
			break;			
		case 14: /* FEC */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_ROLL_OFF_035 )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->rolloff = inData;			
			break;			
		case 15: /* FEC */
			if ( inData > CMD_HWTEST_EXT_CHANNEL_SAT_PILOT_ON )
			{
				printf("%s-Error(%d)\n", __FUNCTION__, __LINE__);				
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			}
			pSatInfo->pilotValue = inData;			
			break;		
		case 16:
			pSatInfo->scdMasterUnitID = inData;
			break;
		case 17:
			pSatInfo->scdUserBandNum = inData;
			break;
		case 18:
			pSatInfo->scdUserBandFreq = inData;
			break;
		default:
			return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER;
			break;			
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
	
}

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_CreateSATLogFile
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
				&scChannelTestStringDictionary_SATHead[0][0], 
				&scChannelTestStringDictionary_SATHead[1][0], 
				&scChannelTestStringDictionary_SATHead[2][0], 
				&scChannelTestStringDictionary_SATHead[3][0], 
				&scChannelTestStringDictionary_SATHead[4][0], 
				&scChannelTestStringDictionary_SATHead[5][0], 
				&scChannelTestStringDictionary_SATHead[6][0], 
				&scChannelTestStringDictionary_SATHead[7][0], 
				&scChannelTestStringDictionary_SATHead[8][0], 
				&scChannelTestStringDictionary_SATHead[9][0], 				
				&scChannelTestStringDictionary_SATHead[10][0], 
				&scChannelTestStringDictionary_SATHead[11][0], 
				&scChannelTestStringDictionary_SATHead[12][0], 
				&scChannelTestStringDictionary_SATHead[13][0], 
				&scChannelTestStringDictionary_SATHead[14][0], 
				&scChannelTestStringDictionary_SATHead[15][0], 
				&scChannelTestStringDictionary_SATHead[16][0], 
				&scChannelTestStringDictionary_SATHead[17][0], 
				&scChannelTestStringDictionary_SATHead[18][0], 
				&scChannelTestStringDictionary_SATHead[19][0]);		


		printf("fclose-begin\n");		
	}
	else if ( (testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST)  ||
	          (testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST) )
	{
			fprintf(fpPid, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
				&scChannelTestStringDictionary_SATHead[0][0], 
				&scChannelTestStringDictionary_SATHead[1][0], 
				&scChannelTestStringDictionary_SATHead[2][0], 
				&scChannelTestStringDictionary_SATHead[3][0], 
				&scChannelTestStringDictionary_SATHead[4][0], 
				&scChannelTestStringDictionary_SATHead[5][0], 
				&scChannelTestStringDictionary_SATHead[6][0], 
				&scChannelTestStringDictionary_SATHead[7][0], 
				&scChannelTestStringDictionary_SATHead[8][0], 
				&scChannelTestStringDictionary_SATHead[9][0], 				
				&scChannelTestStringDictionary_SATHead[10][0], 
				&scChannelTestStringDictionary_SATHead[11][0], 
				&scChannelTestStringDictionary_SATHead[12][0], 
				&scChannelTestStringDictionary_SATHead[13][0], 
				&scChannelTestStringDictionary_SATHead[14][0], 
				&scChannelTestStringDictionary_SATHead[15][0], 
				&scChannelTestStringDictionary_SATHead[16][0], 
				&scChannelTestStringDictionary_SATHead[17][0], 
				&scChannelTestStringDictionary_SATHead[18][0], 
				&scChannelTestStringDictionary_SATHead[19][0],
				&scChannelTestStringDictionary_SATHead[20][0], 
				&scChannelTestStringDictionary_SATHead[21][0], 
				&scChannelTestStringDictionary_SATHead[22][0], 
				&scChannelTestStringDictionary_SATHead[23][0], 
				&scChannelTestStringDictionary_SATHead[24][0], 
				&scChannelTestStringDictionary_SATHead[25][0], 
				&scChannelTestStringDictionary_SATHead[26][0], 
				&scChannelTestStringDictionary_SATHead[27][0], 
				&scChannelTestStringDictionary_SATHead[28][0], 
				&scChannelTestStringDictionary_SATHead[29][0],
				&scChannelTestStringDictionary_SATHead[30][0],
				&scChannelTestStringDictionary_SATHead[31][0],
				&scChannelTestStringDictionary_SATHead[32][0],
				&scChannelTestStringDictionary_SATHead[33][0],
				&scChannelTestStringDictionary_SATHead[34][0]);
	}
	fsync(fpPid);
	fclose(fpPid);	
}





