/**
 * cmd_hwtest.h
*/

/**
 * @defgroup		CMD_HWTEST
 * @author			Chung Dong Chul
 * @note			
 * @brief			
 * @file 			cmd_hwtest.h
*/

#ifndef __CMD_HWTEST_EXT_CHANNEL_H__
#define __CMD_HWTEST_EXT_CHANNEL_H__

#include "di_channel.h"
#include "cmd_hwtest_ext_channel_priv.h"

typedef enum __cmd_hwtest_ext_channel_test_error_type__
{
	CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK = 11,
	CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER,
	CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_INVALID_PARAMETER,
	CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_LACK_OF_NUM_PARAMETER,
	CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OPERATION_ERROR,
	CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NO_FILE_EXIST,
} CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE;

typedef enum __cmd_hwtest_ext_channel_test_type__
{
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST = 11,
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST = 12,	
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST = 13,	
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_KEY_RECEIVED,	
	CMD_HWTEST_EXT_CHANNEL_TEST_QUIT_TEST,
	CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK,
	CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK2,	
	CMD_HWTEST_EXT_CHANNEL_TEST_LOCK_RESULT,
	CMD_HWTEST_EXT_CHANNEL_TEST_PAT_RECEIVED,
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_NONE,
}CMD_HWTEST_EXT_CHANNEL_TEST_TYPE;


typedef enum __cmd_hwtest_ext_channel_test_tuner_type__
{
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT = 1,
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER = 2,
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB = 4,
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_ANA = 8,	
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_NONE = 30,	
}CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE;

typedef struct __cmd_hwtest_ext_channel_test_sat_params__
{
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE tunerType;
	unsigned long tunerId;
	CMD_HWTEST_EXT_CHANNEL_TEST_SAT_ANTENNA_TYPE antennaType;
	CMD_HWTEST_EXT_CHANNEL_SAT_TRANSSPEC transSpec;
	CMD_HWTEST_EXT_CHANNEL_SAT_MODULATION modulation;
	unsigned long frequencyMHz;
	unsigned long symbolRate;
	CMD_HWTEST_EXT_CHANNEL_SAT_POLARITY polarization;
	CMD_HWTEST_EXT_CHANNEL_SAT_DISEQC_IN diseqcIn;
	CMD_HWTEST_EXT_CHANNEL_SAT_TONEBURST toneBurst;
	CMD_HWTEST_EXT_CHANNEL_SAT_22K_TONE b22kTone;
	CMD_HWTEST_EXT_CHANNEL_SAT_FEC fecValue;
	CMD_HWTEST_EXT_CHANNEL_SAT_DISEQC_VER diseqcVer;
	CMD_HWTEST_EXT_CHANNEL_SAT_LNB_VOL lnbVoltage;
	CMD_HWTEST_EXT_CHANNEL_SAT_ROLL_OFF rolloff;
	CMD_HWTEST_EXT_CHANNEL_SAT_PILOT pilotValue;
	unsigned long scdMasterUnitID;
	unsigned long scdUserBandNum;
	unsigned long scdUserBandFreq;	
} CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO;

typedef struct __cmd_hwtest_ext_channel_test_ter_params__
{
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE tunerType;
	unsigned long tunerId;
	unsigned long frequencykHz;
	CMD_HWTEST_EXT_CHANNEL_TER_BANDWIDTH bandwidth;
	CMD_HWTEST_EXT_CHANNEL_TER_SYSTEM system;
	CMD_HWTEST_EXT_CHANNEL_TER_CODERATE codeRate;
	CMD_HWTEST_EXT_CHANNEL_TER_GUARDINTERVAL guardInterval;
	CMD_HWTEST_EXT_CHANNEL_TER_HIERARCHY hierarchy;
	CMD_HWTEST_EXT_CHANNEL_TER_MODULATION modulation;
	CMD_HWTEST_EXT_CHANNEL_TER_OFFSET offset;
	CMD_HWTEST_EXT_CHANNEL_TER_STREAM stream;
	CMD_HWTEST_EXT_CHANNEL_TER_TRANSMODE transMode;
	unsigned long plpip;
	CMD_HWTEST_EXT_CHANNEL_TER_PREAMBLE_FORMAT preambleformat;
	CMD_HWTEST_EXT_CHANNEL_TER_PREAMBLE_MIXED preamblemixed;
	CMD_HWTEST_EXT_CHANNEL_TER_PILOT_PATTERN pilotpattern;
	CMD_HWTEST_EXT_CHANNEL_TER_EXTENDEDCARRIER extendedCarrier;
	CMD_HWTEST_EXT_CHANNEL_TER_PAPRMODE paprmode;	
	unsigned int plpnumblocks;
} CMD_HWTEST_EXT_CHANNEL_TEST_TER_INFO;

typedef struct __cmd_hwtest_ext_channel_test_cab_params__
{
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE tunerType;
	unsigned long tunerId;
	unsigned long frequencykHz;
	unsigned long symbolRate;
	CMD_HWTEST_EXT_CHANNEL_CAB_MODULATION modulation;
	CMD_HWTEST_EXT_CHANNEL_CAB_SPECTRUM spectrum;
	unsigned long netId;
	unsigned long TSID;
} CMD_HWTEST_EXT_CHANNEL_TEST_CAB_INFO;

typedef struct __cmd_hwtest_ext_channel_test_info
{
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE tunerType;
	CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO satInfo;
	CMD_HWTEST_EXT_CHANNEL_TEST_TER_INFO terInfo;
	CMD_HWTEST_EXT_CHANNEL_TEST_CAB_INFO cabInfo;
} CMD_HWTEST_EXT_CHANNEL_TEST_INFO;

typedef struct __cmd_hwtest_ext_channel_test_result__
{
	HI_CH_SignalEvents chSignalEvent;
	HI_CH_SignalInfo chSignalInfo;	
	unsigned long tunerId;
	unsigned long requestId;
}CMD_HWTEST_EXT_CHANNEL_TEST_Result;
typedef enum __cmdhwtestext_channeltest_rcustatus__
{
	CMD_HWTEST_EXT_CHANNELTEST_RCUSTATUS_QSCANMODE = 11,
	CMD_HWTEST_EXT_CHANNELTEST_RCUSTATUS_DSCANMODE,
	CMD_HWTEST_EXT_CHANNELTEST_RCUSTATUS_CHJUMPMODE,	
	CMD_HWTEST_EXT_CHANNELTEST_RCUSTATUS_NONE,	
} CMD_HWTEST_EXT_CHANNELTEST_RCUSTATUS;

typedef struct __cmd_hwtest_ext_teststatus_info__
{
	unsigned long test_total_test_num; /* This is should be increased from 0 after booting up */
	unsigned long test_total_num_of_node; /* This is total numbers */
	unsigned long test_total_cur_node_num;
	unsigned long scan_test_cur_num_node; /* cur node number in one cycle */
	unsigned long scan_test_total_num_nodes; /* total node numbers in one cycle */
	unsigned long scan_test_cur_cycle_num; /* cur cycle number */
	unsigned long scan_test_total_cycle_num; /* total cycle numbers */
	unsigned long scan_total_locked_num;
	char *pLogOutputFileName;
	char *pParamInputFileName;
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE testType;
	CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE tunerType;
	CMD_HWTEST_EXT_CHANNELTEST_RCUSTATUS rcuStatus;
}CMD_HWTEST_EXT_TESTSTATUS_INFO ;

void P_CMDHWTEST_EXT_ChannelTestBegin
(
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE testType
);
void P_CMDHWTEST_EXT_InitChannelTest(void);

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_LockTry(
	CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO satInfo,
	unsigned long requestId);

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_ResultProcess
(
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE curType,
	HI_CH_SignalEvents chSignalEvent,
	CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO satInfo,
	CMD_HWTEST_EXT_TESTSTATUS_INFO *pTestStatusInfo,
	unsigned int requestId
);


CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_InitStringDictionary(void);
CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_SAT_ParsingInfo
(
	unsigned int inData, 
	unsigned int data_offset, 
	CMD_HWTEST_EXT_CHANNEL_TEST_SAT_INFO *pSatInfo
);

CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE P_CmdHwTestExt_ChannelTest_CreateSATLogFile
(
	char *pLogFileName,
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE testType
);


#endif
