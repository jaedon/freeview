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

/* Static Variables */
static unsigned long sulCmdHwtestExtChannelTestTId = 0;
static unsigned long sulCmdHwtestExtChannelTestQId = 0;

#define CMD_HWTEST_CHANNEL_TEST_FILENAME_IN_PARAMS_QUICKTEST "/media/channel_qlist.txt"
#define CMD_HWTEST_CHANNEL_TEST_FILENAME_IN_PARAMS_DETAILTEST "/media/channel_dlist.txt"
#define CMD_HWTEST_CHANNEL_TEST_FILENAME_IN_PARAMS_CHJUMPTEST "/media/channel_chclist.txt"

typedef struct __cmdhwtestext_channel_test_node__
{
	void *pPrevNode;
	void *pNextNode;
	CMD_HWTEST_EXT_CHANNEL_TEST_INFO nodeData;
} CMDHWTESTEXT_CHANNEL_TEST_NODE;

static CMDHWTESTEXT_CHANNEL_TEST_NODE *pChannelTestRootNode = NULL;
static CMDHWTESTEXT_CHANNEL_TEST_NODE *pChannelTestCurNode = NULL;
static CMDHWTESTEXT_CHANNEL_TEST_NODE *pChannelCurPageNode = NULL;
static unsigned long sulHWChannelTestRequestID = 1000;
static char scInfoDisplayString[501];
static char scInfoDisplayString2[4097];
static char scInfoDisplayString3[100];
static CMDHWTESTEXT_CHANNEL_TEST_NODE *pChannelTestTempNode;
static CMD_HWTEST_EXT_TESTSTATUS_INFO sChannelExtTestStatusInfo;


static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_Node_Init
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pCurNode
);
static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_Node_Add
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pRootNode, 
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pNewNode
);
static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_Node_ReleaseAll
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pRootNode
);
static HUINT32 internal_cmdhwtestext_ChannelTest_KeyCallback
(
	HI_KEY_DEVICE eDevice, 
	HUINT32 ulKeyCode, 
	HI_KEY_TYPE eKeyType
);

static void internal_cmdhwtestext_ChannelTest_LockCallback (
	HUINT32 ulInstanceId, 
	HUINT32 unRequestID, 
	HI_CH_Type_e eChannelType , 
	HI_CH_SignalEvents etSignalInfo );

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_SendMsgToTask
(
	unsigned long cmd, 
	unsigned long param1, 
	unsigned long param2, 
	unsigned long param3
);

static void internal_cmdhwtestext_ChannelTestTask(void *pParam);
static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_ConvertDecStringToInt
(
	unsigned char *pData, 
	unsigned long data_length, 
	int *pReturnInt
);


static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_ParsingListNode
(
	char *pReadBuffer, 
	unsigned long data_length, 
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pNewNode
);


static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_LoadInfoFromFile
(	
	CMD_HWTEST_EXT_TESTSTATUS_INFO *pTestStatusInfo,
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pRootNode
);

static void internal_cmdhwtestext_ChannelTest_InitStringDictionary(void);

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_CreateLogFile
(
	CMD_HWTEST_EXT_TESTSTATUS_INFO testInfo
);

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_ShowNodeList
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pCurRootNode,
	unsigned int *pNumNode,
	unsigned int node_offset,
	unsigned int offset_direction
);

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_TryLock
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pNode
);



































 
/**********************************************************************************/
/* Channel Test Node Functions                                                    */
/**********************************************************************************/

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_Node_Init
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pCurNode
)
{
	if ( pCurNode == NULL )
	{
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	pCurNode->pPrevNode = NULL;
	pCurNode->pNextNode = NULL;
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_Node_Add
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pRootNode, 
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pNewNode
)
{
	CMDHWTESTEXT_CHANNEL_TEST_NODE  *pLastNode = NULL;
	if ( (pRootNode == NULL) || (pNewNode == NULL) )
	{
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	pLastNode = pRootNode ;
	while(1)
	{
		if ( pLastNode->pNextNode == NULL )
		{
			pLastNode->pNextNode = pNewNode;
			pNewNode->pPrevNode = pLastNode;
			pNewNode->pNextNode = NULL;
			break;
		}		
		else
		{
			pLastNode = pLastNode->pNextNode;
		}
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_Node_ReleaseAll
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pRootNode
)
{
	CMDHWTESTEXT_CHANNEL_TEST_NODE  *pFirstNode = NULL;
	CMDHWTESTEXT_CHANNEL_TEST_NODE  *pNextNode = NULL;	
	if ( (pRootNode == NULL) )
	{
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	pFirstNode = pRootNode ;
	pNextNode = NULL;
	while(1)
	{
		if ( pFirstNode->pNextNode == NULL )
		{
			VK_MEM_Free(pFirstNode);
			break;
		}		
		else
		{
			pNextNode = pFirstNode->pNextNode;
			VK_MEM_Free(pFirstNode);
			pFirstNode = pNextNode;
		}
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;	
}
/**********************************************************************************/
/* End of Channel Test Node                                                       */
/**********************************************************************************/


/**********************************************************************************/
/* Callbacks                                                                      */
/**********************************************************************************/
static HUINT32 internal_cmdhwtestext_ChannelTest_KeyCallback
(
	HI_KEY_DEVICE eDevice, 
	HUINT32 ulKeyCode, 
	HI_KEY_TYPE eKeyType
)
{
    unsigned long s_msg[4];
    s_msg[0] = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_KEY_RECEIVED;
    s_msg[1] = eDevice;
    s_msg[2] = ulKeyCode;
    s_msg[3] = eKeyType;
    HI_VK_MSG_Send(sulCmdHwtestExtChannelTestQId, s_msg, 4*sizeof(unsigned long));    		
}

static void internal_cmdhwtestext_ChannelTest_LockCallback (
	HUINT32 ulInstanceId, 
	HUINT32 unRequestID, 
	HI_CH_Type_e eChannelType , 
	HI_CH_SignalEvents etSignalInfo )
{
	if ( etSignalInfo == HI_CH_SIGNAL_LOCKED )
	{
		printf("================LOCKED=============\n");
	}
	else if ( etSignalInfo == HI_CH_SIGNAL_UNLOCK )
	{
		printf("================UNLOCKED=============\n");		
	}

	internal_cmdhwtestext_ChannelTest_SendMsgToTask(
		CMD_HWTEST_EXT_CHANNEL_TEST_LOCK_RESULT, ulInstanceId, unRequestID, etSignalInfo);	
}

/**********************************************************************************/
/* End of Callbacks                                                               */
/**********************************************************************************/

/**********************************************************************************/
/* Thread Functions                                                               */
/**********************************************************************************/
static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_SendMsgToTask
(
	unsigned long cmd, 
	unsigned long param1, 
	unsigned long param2, 
	unsigned long param3
)
{
	unsigned long s_msg[4];
	s_msg[0] = cmd;
	s_msg[1] = param1;
	s_msg[2] = param2;
	s_msg[3] = param3;
	HI_VK_MSG_Send(sulCmdHwtestExtChannelTestQId, s_msg, 4*sizeof(unsigned long));
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}





static void internal_cmdhwtestext_ChannelTestTask(void *pParam)
{
	unsigned long r_msg[4];
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE curType = 0;
	unsigned long timeout = 0xFFFFFFFF;
	int ret_value = 0;
	unsigned long node_numbers; 
	unsigned int offset = 0;
	unsigned int test_count = 0;
	unsigned int lock_test_count = 0;
	unsigned int total_test_repeat = 0;
	unsigned int test_repeat_count = 1;
	unsigned int test_repeat_index = 0;
	unsigned int key_detection_status = 0;
	unsigned int tempInt = 0;
	unsigned int cur_page_numbers = 0;
	unsigned int basic_node_count = 0;
	char *pInfoDisplayString = NULL;

	sChannelExtTestStatusInfo.test_total_test_num = 0;
	sChannelExtTestStatusInfo.test_total_num_of_node = 0;
	sChannelExtTestStatusInfo.scan_test_cur_num_node = 0;
	sChannelExtTestStatusInfo.scan_test_total_num_nodes = 0;
	sChannelExtTestStatusInfo.scan_test_cur_cycle_num = 0;
	sChannelExtTestStatusInfo.scan_test_total_cycle_num = 0;	
	sChannelExtTestStatusInfo.pLogOutputFileName = HI_VK_MEM_Alloc(257);
	sChannelExtTestStatusInfo.pLogOutputFileName[256] = '\0';
	sChannelExtTestStatusInfo.pParamInputFileName = HI_VK_MEM_Alloc(257);
	sChannelExtTestStatusInfo.pParamInputFileName[256] = '\0';
	sChannelExtTestStatusInfo.testType = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_NONE;

	while(1)
	{
		r_msg[0] =0x00;
		r_msg[1] =0x00;
		r_msg[2] =0x00;
		r_msg[3] =0x00;
		ret_value = HI_VK_MSG_ReceiveTimeout(
					sulCmdHwtestExtChannelTestQId, 
					r_msg, 
					4*sizeof(unsigned long),
					timeout);
		if ( ret_value == -2 ) /* Timeout */
		{
			
		}

			
		switch(r_msg[0])
		{
			case CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST:
			case CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST:
			case CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST:
				sChannelExtTestStatusInfo.testType = r_msg[0];
                sChannelExtTestStatusInfo.test_total_test_num++;
				HI_KEY_RegisterKeyEventCallback((pfnHI_KEY_Notify)(internal_cmdhwtestext_ChannelTest_KeyCallback));
				if ( r_msg[0] == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST )
				{
					HI_VK_MEM_Memset(sChannelExtTestStatusInfo.pParamInputFileName, ' ', 256);
					sprintf(sChannelExtTestStatusInfo.pParamInputFileName,"%s",CMD_HWTEST_CHANNEL_TEST_FILENAME_IN_PARAMS_QUICKTEST);
					sChannelExtTestStatusInfo.pParamInputFileName[256] = 0;

				}
				else if ( r_msg[0] == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST )
				{
					HI_VK_MEM_Memset(sChannelExtTestStatusInfo.pParamInputFileName, ' ', 256);
					sprintf(sChannelExtTestStatusInfo.pParamInputFileName,"%s",CMD_HWTEST_CHANNEL_TEST_FILENAME_IN_PARAMS_DETAILTEST);
					sChannelExtTestStatusInfo.pParamInputFileName[256] = 0;
				}
				else if ( r_msg[0] == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST )
				{
					HI_VK_MEM_Memset(sChannelExtTestStatusInfo.pParamInputFileName, ' ', 256);
					sprintf(sChannelExtTestStatusInfo.pParamInputFileName,"%s",CMD_HWTEST_CHANNEL_TEST_FILENAME_IN_PARAMS_CHJUMPTEST);
					sChannelExtTestStatusInfo.pParamInputFileName[256] = 0;
				}
				
				/* Initialization node */
				pChannelTestRootNode = HI_VK_MEM_Alloc(sizeof(CMDHWTESTEXT_CHANNEL_TEST_NODE));
				internal_cmdhwtestext_ChannelTest_Node_Init(pChannelTestRootNode);
				internal_cmdhwtestext_ChannelTest_LoadInfoFromFile(
						&sChannelExtTestStatusInfo,
						pChannelTestRootNode);

				printf("node_numbers(%ld)\n", sChannelExtTestStatusInfo.scan_test_total_num_nodes);

				if ( sChannelExtTestStatusInfo.scan_test_total_num_nodes != 0 )
				{
					if ( (r_msg[0] == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST)  )
					{
						sChannelExtTestStatusInfo.test_total_cur_node_num = 0;
						sChannelExtTestStatusInfo.scan_total_locked_num = 0;

						HI_VK_MEM_Memset(sChannelExtTestStatusInfo.pLogOutputFileName, ' ', 256);
						sprintf(sChannelExtTestStatusInfo.pLogOutputFileName, 
							"/media/channel_qlist_%05d_test_result.xls", 
							(int)sChannelExtTestStatusInfo.test_total_test_num);
						sChannelExtTestStatusInfo.pLogOutputFileName[256] = '\0';
						sChannelExtTestStatusInfo.scan_test_total_cycle_num = 1;
						sChannelExtTestStatusInfo.test_total_num_of_node = 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num * sChannelExtTestStatusInfo.scan_test_total_num_nodes;

						pInfoDisplayString = HI_VK_MEM_Alloc(500+1);
						HI_VK_MEM_Memset(pInfoDisplayString, ' ', 500);
						sprintf(pInfoDisplayString, "Auto Scan Test In Progress\n Totally (%ld) Repeated\n Increase\Decreate of repeat:PG Up/Down \nPress OK to begin\n", 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num);					
						pInfoDisplayString[500] = '\0';
						
						pChannelTestTempNode = pChannelTestRootNode->pNextNode;

						AP_HwTest_DisplayInfo(pInfoDisplayString);
						pChannelTestCurNode = pChannelTestRootNode;
						HI_VK_MEM_Free(pInfoDisplayString);
						sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST;
						sChannelExtTestStatusInfo.tunerType = pChannelTestTempNode->nodeData.tunerType;
						internal_cmdhwtestext_ChannelTest_CreateLogFile(sChannelExtTestStatusInfo);

					}
                    else if ( (r_msg[0] == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST)  )
					{
						sChannelExtTestStatusInfo.test_total_cur_node_num = 0;
						sChannelExtTestStatusInfo.scan_total_locked_num = 0;

						HI_VK_MEM_Memset(sChannelExtTestStatusInfo.pLogOutputFileName, ' ', 256);
						sprintf(sChannelExtTestStatusInfo.pLogOutputFileName, 
							"/media/channel_dlist_%05d_test_result.xls", 
							(int)sChannelExtTestStatusInfo.test_total_test_num);
						sChannelExtTestStatusInfo.pLogOutputFileName[256] = '\0';
						sChannelExtTestStatusInfo.scan_test_total_cycle_num = 1;
						sChannelExtTestStatusInfo.test_total_num_of_node = 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num * sChannelExtTestStatusInfo.scan_test_total_num_nodes;

						pInfoDisplayString = HI_VK_MEM_Alloc(500+1);
						HI_VK_MEM_Memset(pInfoDisplayString, ' ', 500);
						sprintf(pInfoDisplayString, "Auto Scan Test In Progress\n Totally (%ld) Repeated\n Increase\Decreate of repeat:PG Up/Down \nPress OK to begin\n", 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num);					
						pInfoDisplayString[500] = '\0';
						
						pChannelTestTempNode = pChannelTestRootNode->pNextNode;

						AP_HwTest_DisplayInfo(pInfoDisplayString);
						pChannelTestCurNode = pChannelTestRootNode;
						HI_VK_MEM_Free(pInfoDisplayString);
						sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST;
						sChannelExtTestStatusInfo.tunerType = pChannelTestTempNode->nodeData.tunerType;
						internal_cmdhwtestext_ChannelTest_CreateLogFile(sChannelExtTestStatusInfo);

					}
                    else if ( (r_msg[0] == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST)  )
					{
						sChannelExtTestStatusInfo.test_total_cur_node_num = 0;
						sChannelExtTestStatusInfo.scan_total_locked_num = 0;

						HI_VK_MEM_Memset(sChannelExtTestStatusInfo.pLogOutputFileName, ' ', 256);
						sprintf(sChannelExtTestStatusInfo.pLogOutputFileName, 
							"/media/channel_chclist_%05d_test_result.xls", 
							(int)sChannelExtTestStatusInfo.test_total_test_num);
						sChannelExtTestStatusInfo.pLogOutputFileName[256] = '\0';
						sChannelExtTestStatusInfo.scan_test_total_cycle_num = 1;
						sChannelExtTestStatusInfo.test_total_num_of_node = 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num * sChannelExtTestStatusInfo.scan_test_total_num_nodes;

						pChannelTestTempNode = pChannelTestRootNode->pNextNode;


						pChannelCurPageNode = pChannelTestRootNode->pNextNode;

						sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST;
						sChannelExtTestStatusInfo.tunerType = pChannelTestTempNode->nodeData.tunerType;
						internal_cmdhwtestext_ChannelTest_CreateLogFile(sChannelExtTestStatusInfo);
                        internal_cmdhwtestext_ChannelTest_ShowNodeList(pChannelCurPageNode, &tempInt, 0, 0);
					}                    
				}
				else 
				{
					internal_cmdhwtestext_ChannelTest_SendMsgToTask(
						CMD_HWTEST_EXT_CHANNEL_TEST_QUIT_TEST,0,0,0);	
				}
				break;

			case CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_KEY_RECEIVED:
				printf("key_received(%08x,%08x,%08x)\n", r_msg[1], r_msg[2], r_msg[3]);
				if ( (sChannelExtTestStatusInfo.rcuStatus == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST) || 
					 (sChannelExtTestStatusInfo.rcuStatus == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST ))
				{
					if ( r_msg[2] == HI_KEY_REMOCON_CH_PLUS )
					{
						sChannelExtTestStatusInfo.scan_test_total_cycle_num++;
						sChannelExtTestStatusInfo.test_total_num_of_node = 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num *sChannelExtTestStatusInfo.scan_test_total_num_nodes;
						pInfoDisplayString = HI_VK_MEM_Alloc(500+1);
						HI_VK_MEM_Memset(pInfoDisplayString, ' ', 500);
						sprintf(pInfoDisplayString, 
							"Auto Scan Test In Progress\n Totally (%ld) Repeated\n Increase\Decreate of repeat:PG Up/Down \nPress OK to begin\n", 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num);					
						pInfoDisplayString[500] = '\0';
						AP_HwTest_DisplayInfo(pInfoDisplayString);
						HI_VK_MEM_Free(pInfoDisplayString);						
					}
					else if ( r_msg[2] == HI_KEY_REMOCON_CH_MINUS )
					{
						if ( sChannelExtTestStatusInfo.scan_test_total_cycle_num != 1 )
						{
							sChannelExtTestStatusInfo.scan_test_total_cycle_num--;	
							
						}
						sChannelExtTestStatusInfo.test_total_num_of_node = 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num *sChannelExtTestStatusInfo.scan_test_total_num_nodes;						
						pInfoDisplayString = HI_VK_MEM_Alloc(500+1);
						HI_VK_MEM_Memset(pInfoDisplayString, ' ', 500);
						sprintf(pInfoDisplayString, 
							"Auto Scan Test In Progress\n Totally (%ld) Repeated\n Increase\Decreate of repeat:PG Up/Down \nPress OK to begin\n", 
							sChannelExtTestStatusInfo.scan_test_total_cycle_num);					
						pInfoDisplayString[500] = '\0';
						AP_HwTest_DisplayInfo(pInfoDisplayString);
						HI_VK_MEM_Free(pInfoDisplayString);							
					}
					else if ( r_msg[2] == HI_KEY_REMOCON_OK )
					{
						pInfoDisplayString = HI_VK_MEM_Alloc(501);
						HI_VK_MEM_Memset(pInfoDisplayString, ' ', 500);
						sprintf(pInfoDisplayString, "Auto Scan Test In Progress\n Totally (%d) Repeated\n", sChannelExtTestStatusInfo.scan_test_total_cycle_num);
						pInfoDisplayString[500] ='\0';	
						AP_HwTest_DisplayInfo(pInfoDisplayString);
						internal_cmdhwtestext_ChannelTest_SendMsgToTask(
						CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK, 0, 0, 0);
						key_detection_status = 0;
						HI_VK_MEM_Free(pInfoDisplayString);
						sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_NONE;

						
					}
					else if ( r_msg[2] == DI_KEY_REMOCON_EXIT )
					{
						P_DeleteInfoWindowEx();
						internal_cmdhwtestext_ChannelTest_SendMsgToTask(CMD_HWTEST_EXT_CHANNEL_TEST_QUIT_TEST,0,0,0);
					}
				}
                else if ( sChannelExtTestStatusInfo.rcuStatus == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST )
                {
					switch(r_msg[2])
					{
						case DI_KEY_REMOCON_EXIT:
							P_DeleteInfoWindowEx();
							internal_cmdhwtestext_ChannelTest_SendMsgToTask(
								CMD_HWTEST_EXT_CHANNEL_TEST_QUIT_TEST,0,0,0);
							break;
						case DI_KEY_REMOCON_0 :
						case DI_KEY_REMOCON_1 :
						case DI_KEY_REMOCON_2 :
						case DI_KEY_REMOCON_3 :
						case DI_KEY_REMOCON_4 :
						case DI_KEY_REMOCON_5 :
						case DI_KEY_REMOCON_6 :
						case DI_KEY_REMOCON_7 :
						case DI_KEY_REMOCON_8 :
						case DI_KEY_REMOCON_9 :							
							pChannelTestTempNode = pChannelCurPageNode;
							pChannelTestCurNode = pChannelCurPageNode;
							for ( tempInt = 0 ; tempInt < r_msg[2] - DI_KEY_REMOCON_0 ; tempInt++ )
							{
								pChannelTestTempNode = pChannelTestCurNode;
								pChannelTestCurNode = pChannelTestTempNode->pNextNode;
							}
							internal_cmdhwtestext_ChannelTest_SendMsgToTask(CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK2, 0, 0, 0);
                            sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_NONE;
							break;
						case DI_KEY_REMOCON_CH_MINUS:
							internal_cmdhwtestext_ChannelTest_ShowNodeList(pChannelCurPageNode, &basic_node_count, 10, 1);
							break;
						case DI_KEY_REMOCON_CH_PLUS:
							internal_cmdhwtestext_ChannelTest_ShowNodeList(pChannelCurPageNode, &basic_node_count, 10, 2);
							break;
					}
				}
			
				break;
				
			case CMD_HWTEST_EXT_CHANNEL_TEST_QUIT_TEST:
				if ( (sChannelExtTestStatusInfo.scan_test_total_num_nodes != 0) && 
					 (sChannelExtTestStatusInfo.testType != CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST) )
				{
					pInfoDisplayString = HI_VK_MEM_Alloc(501);
					VK_MEM_Memset(pInfoDisplayString, ' ', 500);
					sprintf(pInfoDisplayString, 
						"%05d channels locked totally %05d trials\n", 
						sChannelExtTestStatusInfo.scan_total_locked_num, 
						sChannelExtTestStatusInfo.test_total_num_of_node);	
					pInfoDisplayString[500] = '\0';
					AP_HwTest_DisplayInfo(pInfoDisplayString);
					HI_VK_MEM_Free(pInfoDisplayString);
					
				}
				internal_cmdhwtestext_ChannelTest_Node_ReleaseAll(pChannelTestRootNode);
		    	HI_KEY_RegisterKeyEventCallback(AP_HwTest_KeyCallBack);  
				sChannelExtTestStatusInfo.test_total_num_of_node = 0;
				sChannelExtTestStatusInfo.scan_test_cur_num_node = 0;
				sChannelExtTestStatusInfo.scan_test_total_num_nodes = 0;
				sChannelExtTestStatusInfo.scan_test_cur_cycle_num = 0;
				sChannelExtTestStatusInfo.scan_test_total_cycle_num = 0;	
				HI_VK_MEM_Memset(sChannelExtTestStatusInfo.pLogOutputFileName, ' ', 256);
				sChannelExtTestStatusInfo.pLogOutputFileName[256] = '\0';
				HI_VK_MEM_Memset(sChannelExtTestStatusInfo.pParamInputFileName, ' ', 256);
				sChannelExtTestStatusInfo.pParamInputFileName[256] = '\0';
				sChannelExtTestStatusInfo.testType = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_NONE;				
                sChannelExtTestStatusInfo.tunerType = CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_NONE;
                sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNELTEST_RCUSTATUS_NONE;
				break;

			case CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK2:

				if ( pChannelTestCurNode != NULL )
				{
					
					printf("CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK2 is called\n");
					if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT )
					{
    					P_CmdHwTestExt_ChannelTest_SAT_ChannelInfoDisplay(pChannelTestCurNode->nodeData.satInfo, 0);
					}
					else if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER )
					{
    					P_CmdHwTestExt_ChannelTest_TER_ChannelInfoDisplay(pChannelTestCurNode->nodeData.terInfo, 0);
					}
					else if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB )
					{
    					P_CmdHwTestExt_ChannelTest_CAB_ChannelInfoDisplay(pChannelTestCurNode->nodeData.cabInfo, 0);
					}
					internal_cmdhwtestext_ChannelTest_TryLock(pChannelTestCurNode);
				}		

				break;
			case CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK:
				if ( pChannelTestCurNode->pNextNode != NULL )
				{
				
					pChannelTestCurNode = pChannelTestCurNode->pNextNode;
					printf("CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK is called\n");
					if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT )
					{
						P_CmdHwTestExt_ChannelTest_SAT_ChannelInfoDisplay(pChannelTestCurNode->nodeData.satInfo,0);

					if ( internal_cmdhwtestext_ChannelTest_TryLock(pChannelTestCurNode) != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
					{
							sChannelExtTestStatusInfo.test_total_cur_node_num++;
						P_CmdHwTestExt_ChannelTest_SAT_ChannelInfoDisplay(pChannelTestCurNode->nodeData.satInfo,1);
						P_CmdHwTestExt_ChannelTest_SAT_ResultProcess(sChannelExtTestStatusInfo.testType, HI_CH_SIGNAL_UNLOCK, pChannelTestCurNode->nodeData.satInfo, &sChannelExtTestStatusInfo,sulHWChannelTestRequestID);						
						internal_cmdhwtestext_ChannelTest_SendMsgToTask(
						CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK, 0, 0, 0); 						
						
					}
					}
					else if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER )
					{
						P_CmdHwTestExt_ChannelTest_TER_ChannelInfoDisplay(pChannelTestCurNode->nodeData.terInfo,0);

						if ( internal_cmdhwtestext_ChannelTest_TryLock(pChannelTestCurNode) != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
						{
							sChannelExtTestStatusInfo.test_total_cur_node_num++;
							P_CmdHwTestExt_ChannelTest_TER_ChannelInfoDisplay(pChannelTestCurNode->nodeData.terInfo,1);
							P_CmdHwTestExt_ChannelTest_TER_ResultProcess(sChannelExtTestStatusInfo.testType, HI_CH_SIGNAL_UNLOCK, pChannelTestCurNode->nodeData.terInfo, &sChannelExtTestStatusInfo,sulHWChannelTestRequestID);						
							internal_cmdhwtestext_ChannelTest_SendMsgToTask(
							CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK, 0, 0, 0); 						
							
						}
					}
					else if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB )
					{
						P_CmdHwTestExt_ChannelTest_CAB_ChannelInfoDisplay(pChannelTestCurNode->nodeData.cabInfo,0);

						if ( internal_cmdhwtestext_ChannelTest_TryLock(pChannelTestCurNode) != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
						{
							sChannelExtTestStatusInfo.test_total_cur_node_num++;
							P_CmdHwTestExt_ChannelTest_CAB_ChannelInfoDisplay(pChannelTestCurNode->nodeData.cabInfo,1);
							P_CmdHwTestExt_ChannelTest_CAB_ResultProcess(sChannelExtTestStatusInfo.testType, HI_CH_SIGNAL_UNLOCK, pChannelTestCurNode->nodeData.cabInfo, &sChannelExtTestStatusInfo,sulHWChannelTestRequestID);						
							internal_cmdhwtestext_ChannelTest_SendMsgToTask(
							CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK, 0, 0, 0); 						
							
						}
					}

				}
				else
				{
					sChannelExtTestStatusInfo.scan_test_cur_cycle_num++;
					if ( sChannelExtTestStatusInfo.scan_test_cur_cycle_num < sChannelExtTestStatusInfo.scan_test_total_cycle_num )
					{
						pChannelTestCurNode = pChannelTestRootNode->pNextNode;
						printf("CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK is called\n");
	
						if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT )
						{	
							P_CmdHwTestExt_ChannelTest_SAT_ChannelInfoDisplay(pChannelTestCurNode->nodeData.satInfo,0);

						if ( internal_cmdhwtestext_ChannelTest_TryLock(pChannelTestCurNode) != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
						{
								sChannelExtTestStatusInfo.test_total_cur_node_num++;							
							P_CmdHwTestExt_ChannelTest_SAT_ChannelInfoDisplay(pChannelTestCurNode->nodeData.satInfo,1);
							P_CmdHwTestExt_ChannelTest_SAT_ResultProcess(sChannelExtTestStatusInfo.testType, HI_CH_SIGNAL_UNLOCK, pChannelTestCurNode->nodeData.satInfo, &sChannelExtTestStatusInfo,sulHWChannelTestRequestID);													
							internal_cmdhwtestext_ChannelTest_SendMsgToTask(
							CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK, 0, 0, 0);							
							
						}
					}
						else if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER )
						{	
							P_CmdHwTestExt_ChannelTest_TER_ChannelInfoDisplay(pChannelTestCurNode->nodeData.terInfo,0);

							if ( internal_cmdhwtestext_ChannelTest_TryLock(pChannelTestCurNode) != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
							{
								sChannelExtTestStatusInfo.test_total_cur_node_num++;
								P_CmdHwTestExt_ChannelTest_TER_ChannelInfoDisplay(pChannelTestCurNode->nodeData.terInfo,1);
								P_CmdHwTestExt_ChannelTest_TER_ResultProcess(sChannelExtTestStatusInfo.testType, HI_CH_SIGNAL_UNLOCK, pChannelTestCurNode->nodeData.terInfo, &sChannelExtTestStatusInfo,sulHWChannelTestRequestID);													
								internal_cmdhwtestext_ChannelTest_SendMsgToTask(
								CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK, 0, 0, 0);							
								
							}
						}
						else if ( pChannelTestCurNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB )
						{	
							P_CmdHwTestExt_ChannelTest_CAB_ChannelInfoDisplay(pChannelTestCurNode->nodeData.cabInfo,0);

							if ( internal_cmdhwtestext_ChannelTest_TryLock(pChannelTestCurNode) != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
							{
								sChannelExtTestStatusInfo.test_total_cur_node_num++;
								P_CmdHwTestExt_ChannelTest_CAB_ChannelInfoDisplay(pChannelTestCurNode->nodeData.cabInfo,1);
								P_CmdHwTestExt_ChannelTest_CAB_ResultProcess(sChannelExtTestStatusInfo.testType, HI_CH_SIGNAL_UNLOCK, pChannelTestCurNode->nodeData.cabInfo, &sChannelExtTestStatusInfo,sulHWChannelTestRequestID);													
								internal_cmdhwtestext_ChannelTest_SendMsgToTask(
								CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK, 0, 0, 0);							
								
							}
						}
					}
					else
					{
						internal_cmdhwtestext_ChannelTest_SendMsgToTask(
							CMD_HWTEST_EXT_CHANNEL_TEST_QUIT_TEST,0,0,0);
					}
				}
		
				break;
			case CMD_HWTEST_EXT_CHANNEL_TEST_LOCK_RESULT:
				sChannelExtTestStatusInfo.test_total_cur_node_num++;
				if ( pChannelTestCurNode->nodeData.tunerType == 
						CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT )
				{					
					P_CmdHwTestExt_ChannelTest_SAT_ResultProcess(
						sChannelExtTestStatusInfo.testType, 
						r_msg[3], 
						pChannelTestCurNode->nodeData.satInfo, 
						&sChannelExtTestStatusInfo,
						r_msg[2]);
					if ( (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST) ||
 						 (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST))
					{
						internal_cmdhwtestext_ChannelTest_SendMsgToTask(
							CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK,0,0,0);
					}
                    else if ( (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST ))
                    {
                        internal_cmdhwtestext_ChannelTest_ShowNodeList(pChannelCurPageNode, &tempInt, 0, 0);
                        sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST;
                    }
				}
				else if ( pChannelTestCurNode->nodeData.tunerType == 
						CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER )
				{					
					P_CmdHwTestExt_ChannelTest_TER_ResultProcess(
						sChannelExtTestStatusInfo.testType, 
						r_msg[3], 
						pChannelTestCurNode->nodeData.terInfo, 
						&sChannelExtTestStatusInfo,
						r_msg[2]);
					if ( (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST) ||
						 (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST))
					{
						internal_cmdhwtestext_ChannelTest_SendMsgToTask(
							CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK,0,0,0);
					}
					else if ( (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST ))
					{
						internal_cmdhwtestext_ChannelTest_ShowNodeList(pChannelCurPageNode, &tempInt, 0, 0);
						sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST;
					}
				}
				else if ( pChannelTestCurNode->nodeData.tunerType == 
						CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB )
				{					
					P_CmdHwTestExt_ChannelTest_CAB_ResultProcess(
						sChannelExtTestStatusInfo.testType, 
						r_msg[3], 
						pChannelTestCurNode->nodeData.cabInfo, 
						&sChannelExtTestStatusInfo,
						r_msg[2]);
					if ( (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_QUICK_SCAN_TEST) ||
						 (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_DETAIL_SCAN_TEST))
					{
						internal_cmdhwtestext_ChannelTest_SendMsgToTask(
							CMD_HWTEST_EXT_CHANNEL_TEST_TRY_LOCK,0,0,0);
					}
					else if ( (sChannelExtTestStatusInfo.testType == CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST ))
					{
						internal_cmdhwtestext_ChannelTest_ShowNodeList(pChannelCurPageNode, &tempInt, 0, 0);
						sChannelExtTestStatusInfo.rcuStatus = CMD_HWTEST_EXT_CHANNEL_TEST_TYPE_CH_JUMP_TEST;
					}
				}
				break;		
			case CMD_HWTEST_EXT_CHANNEL_TEST_PAT_RECEIVED:
				break;
			default:
				break;
		}
	}
}
/**********************************************************************************/
/* End of Thread Functions                                                        */
/**********************************************************************************/

/**********************************************************************************/
/* Parsing Functions                                                              */
/**********************************************************************************/
static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_ConvertDecStringToInt
(
	unsigned char *pData, 
	unsigned long data_length, 
	int *pReturnInt
)
{
	unsigned long tempInt = 0;
	unsigned long finalInt = 0;
	unsigned long offset = 0;
	if ( (pData == NULL ) || (pReturnInt == NULL))
	{
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	for(offset = 0 ; offset < data_length ; offset++ )
	{
		switch(pData[offset])
		{
			case '0':				
				tempInt = 0;
				break;
			case '1':
				tempInt = 1;
				break;
			case '2':
				tempInt = 2;
				break;
			case '3':
				tempInt = 3;
				break;
			case '4':
				tempInt = 4;
				break;
			case '5':
				tempInt = 5;
				break;
			case '6':
				tempInt = 6;
				break;
			case '7':
				tempInt = 7;
				break;
			case '8':
				tempInt = 8;
				break;
			case '9':
				tempInt = 9;
				break;				
		}
		finalInt = finalInt * 10 + tempInt;
	}
	*pReturnInt = finalInt;
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_ParsingListNode
(
	char *pReadBuffer, 
	unsigned long data_length, 
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pNewNode
)
{
	unsigned long offset = 0;
	unsigned long prev_position = 0;
	unsigned long next_position = 0;	
	unsigned long data_offset = 0;
	unsigned long tempInt = 0;
	unsigned long channelType = 0;
	CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE errorType = CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OPERATION_ERROR;
	if ( (pReadBuffer == NULL) || (pNewNode == NULL ))
	{
		printf("%s_Error(%d)\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	internal_cmdhwtestext_ChannelTest_ConvertDecStringToInt(&pReadBuffer[0],1,&channelType);
	switch(channelType)
	{
		case CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT: /* Sat */
			pNewNode->nodeData.tunerType = DI_CH_TYPE_SAT;
			/* Check type */
			for ( offset = 0 ; offset < data_length ; offset++ )
			{
				if ( pReadBuffer[offset] == '/')
				{
					next_position = offset;
					
					errorType = internal_cmdhwtestext_ChannelTest_ConvertDecStringToInt(&pReadBuffer[prev_position], 
						 (next_position-prev_position), &tempInt);
					if ( errorType != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
					{
						return errorType;
					}
					prev_position = offset+1;		
					printf("tempInt(%d,%d)\n", data_offset, tempInt);
					errorType = P_CmdHwTestExt_ChannelTest_SAT_ParsingInfo(tempInt, data_offset, &(pNewNode->nodeData.satInfo));
					if ( errorType != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
					{
						return errorType;
					}
					data_offset++;					
				}
			}
			
			break;
		case CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER: /* Ter */
			pNewNode->nodeData.tunerType = DI_CH_TYPE_TER;
			/* Check type */
			for ( offset = 0 ; offset < data_length ; offset++ )
			{
				if ( pReadBuffer[offset] == '/')
				{
					next_position = offset;
			
					errorType = internal_cmdhwtestext_ChannelTest_ConvertDecStringToInt(&pReadBuffer[prev_position], 
						 (next_position-prev_position), &tempInt);
					if ( errorType != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
					{
						return errorType;
					}
					prev_position = offset+1;		
					printf("tempInt(%d,%d)\n", data_offset, tempInt);
					errorType = P_CmdHwTestExt_ChannelTest_TER_ParsingInfo(tempInt, data_offset, &(pNewNode->nodeData.terInfo));
					if ( errorType != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
					{
						return errorType;
					}
					data_offset++;					
				}
			}			
			break;
		case CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB: /* Cab */
			pNewNode->nodeData.tunerType = DI_CH_TYPE_CAB;
			/* Check type */
			for ( offset = 0 ; offset < data_length ; offset++ )
			{
				if ( pReadBuffer[offset] == '/')
				{
					next_position = offset;
					
					errorType = internal_cmdhwtestext_ChannelTest_ConvertDecStringToInt(&pReadBuffer[prev_position], 
						 (next_position-prev_position), &tempInt);
					if ( errorType != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
					{
						return errorType;
					}
					prev_position = offset+1;		
					printf("tempInt(%d,%d)\n", data_offset, tempInt);
					errorType = P_CmdHwTestExt_ChannelTest_CAB_ParsingInfo(tempInt, data_offset, &(pNewNode->nodeData.cabInfo));
					if ( errorType != CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK )
					{
						return errorType;
					}
					data_offset++;					
				}
			}			
			break;
		default: /* Error */
			break;
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
	
}

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_LoadInfoFromFile
(	
	CMD_HWTEST_EXT_TESTSTATUS_INFO *pTestStatusInfo,
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pRootNode
)
{
	char tempCh = 0x00;
	unsigned long first_scan_position = 0;
	unsigned long last_scan_position = 0;
	unsigned long num_of_node = 0;
	unsigned char *pReadBuffer = NULL;
	unsigned long data_length = 0;
	if ( (pRootNode == NULL) || (pTestStatusInfo == NULL ))
	{
		printf("(%s)-Error(%d):Invalid Pointer\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;
	}
	
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pNewNode = NULL;

	pTestStatusInfo->test_total_num_of_node = 0; /* This is total numbers */
	pTestStatusInfo->scan_test_cur_num_node = 0; /* cur node number in one cycle */
	pTestStatusInfo->scan_test_total_num_nodes = 0; /* total node numbers in one cycle */
	pTestStatusInfo->scan_test_cur_cycle_num = 0; /* cur cycle number */
	pTestStatusInfo->scan_test_total_cycle_num = 1; /* total cycle numbers */

	if ( pTestStatusInfo->pParamInputFileName == NULL)
	{
		printf("(%s)-Error(%d):Invalid File Name\n", __FUNCTION__, __LINE__);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;		
	}
	FILE *fpTestInfo = fopen(pTestStatusInfo->pParamInputFileName, "r+b");
	if ( fpTestInfo == NULL )
	{
		printf("(%s)-Error(%d):Cannot open file(%s)\n", __FUNCTION__, __LINE__, pTestStatusInfo->pParamInputFileName);
		return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NO_FILE_EXIST;
	}
	fseek(fpTestInfo,0,SEEK_SET);
	num_of_node = 0;
	first_scan_position = 0;
	last_scan_position = 0;
	while(1)
	{
		fseek(fpTestInfo, last_scan_position, SEEK_SET);		
		fread(&tempCh, sizeof(char), sizeof(char), fpTestInfo);
		if ( feof(fpTestInfo) !=  0 )
		{
			break;
		}	
		if ( tempCh == '\n' )
		{
			num_of_node++;
			fseek(fpTestInfo, first_scan_position, SEEK_SET);
			pReadBuffer = HI_VK_MEM_Alloc(last_scan_position - first_scan_position);
			data_length = (last_scan_position - first_scan_position);
			fread(pReadBuffer, (last_scan_position - first_scan_position)*sizeof(char), sizeof(char), fpTestInfo);
			printf("ReadBuffer(%s)\n", pReadBuffer);
			pNewNode = HI_VK_MEM_Alloc(sizeof(CMDHWTESTEXT_CHANNEL_TEST_NODE));
			internal_cmdhwtestext_ChannelTest_Node_Init(pNewNode);
			internal_cmdhwtestext_ChannelTest_ParsingListNode(pReadBuffer, data_length, pNewNode);					
			internal_cmdhwtestext_ChannelTest_Node_Add(pRootNode, pNewNode);
			HI_VK_MEM_Free(pReadBuffer);
			first_scan_position = last_scan_position+1;
		}
		last_scan_position++;		
	}
	fclose(fpTestInfo);
	pTestStatusInfo->scan_test_total_num_nodes = num_of_node;
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}

static void internal_cmdhwtestext_ChannelTest_InitStringDictionary(void)
{
	P_CmdHwTestExt_ChannelTest_SAT_InitStringDictionary();
	P_CmdHwTestExt_ChannelTest_TER_InitStringDictionary();
	P_CmdHwTestExt_ChannelTest_CAB_InitStringDictionary();

}

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_CreateLogFile
(
	CMD_HWTEST_EXT_TESTSTATUS_INFO testInfo
)
{
	if ( (testInfo.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT ))
	{
		P_CmdHwTestExt_ChannelTest_SAT_CreateSATLogFile(testInfo.pLogOutputFileName, testInfo.testType);
	}
	else if ( (testInfo.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER ))
	{
		P_CmdHwTestExt_ChannelTest_TER_CreateTERLogFile(testInfo.pLogOutputFileName, testInfo.testType);
	}
	else if ( (testInfo.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB ))
	{
		P_CmdHwTestExt_ChannelTest_CAB_CreateTERLogFile(testInfo.pLogOutputFileName, testInfo.testType);
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}
static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_ShowNodeList
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pCurRootNode,
	unsigned int *pNumNode,
	unsigned int node_offset,
	unsigned int offset_direction
)
{

	/* first check number of node */
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pTempNode;
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pTempNode2;

	unsigned int node_num = 0;
	unsigned int tempInt = 0;
	unsigned int offset = 0;
	unsigned int total_pages = 0;
	unsigned int cur_page = 0;
	VK_MEM_Memset(scInfoDisplayString2, 0x32, 4096);





	if ( node_offset == 0 && offset_direction == 0 )
	{
		if ( pCurRootNode != NULL )
		{
			node_num++;
		}
		pTempNode = pCurRootNode;	
		while(1)
		{
			if ( pTempNode->pNextNode != NULL )
			{
				node_num++;
				pTempNode = pTempNode->pNextNode;
				if ( node_num == 10 )
				{
					break;
				}				
			}
			else
			{
				break;
			}		
		}
		printf("total_node(%d)\n", node_num);
		pTempNode = pCurRootNode;

		pTempNode2 = pChannelTestRootNode->pNextNode;
		for(tempInt = 1 ; tempInt < sChannelExtTestStatusInfo.scan_test_total_num_nodes ; tempInt++ )
		{
			if ( pTempNode == pTempNode2 )
			{
				break;
			}
			if ( pTempNode2->pNextNode != NULL )
			{
				pTempNode2 = pTempNode2->pNextNode;
			}
			else
			{
				break;
			}

		}
		
		if ( (sChannelExtTestStatusInfo.scan_test_total_num_nodes % 10) != 0 )
		{
			total_pages = sChannelExtTestStatusInfo.scan_test_total_num_nodes /10 + 1;
		}
		else
		{
			total_pages = sChannelExtTestStatusInfo.scan_test_total_num_nodes /10;
		}
		
		if ( (tempInt % 10) != 0 )
		{
			cur_page = tempInt /10 + 1;
		}
		else
		{
			cur_page = tempInt /10;
		}
		HI_VK_MEM_Memset(scInfoDisplayString2, ' ', 4096);
		HI_VK_MEM_Memset(scInfoDisplayString3, ' ', 100);			
		sprintf(scInfoDisplayString3, "	%d page / %d #", cur_page, total_pages);

		HI_VK_MEM_Memcpy(&scInfoDisplayString2[0], &scInfoDisplayString3[0], 100);




		for ( offset = 0 ; offset < node_num ; offset++ )
		{
			printf("offset_value(%d, %d)\n", offset, pTempNode->nodeData.tunerType);
			HI_VK_MEM_Memset(scInfoDisplayString3, 0x32, 100);
			if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT )
			{
			    P_CmdHwTestExt_ChannelTest_SAT_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.satInfo ) ;
				pTempNode = pTempNode->pNextNode;
				tempInt = offset * 100+100;
				HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
				printf("%s\n", scInfoDisplayString3);
			}
			else if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER)
			{
			    P_CmdHwTestExt_ChannelTest_TER_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.terInfo ) ;
				pTempNode = pTempNode->pNextNode;
				tempInt = offset * 100+100;
				HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
				printf("%s\n", scInfoDisplayString3);
			}
			else if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB)
			{
			    P_CmdHwTestExt_ChannelTest_CAB_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.cabInfo ) ;
				pTempNode = pTempNode->pNextNode;
				tempInt = offset * 100+100;
				HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
				printf("%s\n", scInfoDisplayString3);
			}
		}

		scInfoDisplayString2[4096] = '\0';
		AP_HwTest_DisplayInfoEx(scInfoDisplayString2, 4096);
		*pNumNode = node_num;	
	}
	else if ( node_offset == 10 && offset_direction == 1 )
	{
		printf("%s(%d)\n", __FUNCTION__, __LINE__);
		pTempNode = pCurRootNode;	
		node_num = 0;
		while(1)
		{
			if ( pTempNode->pNextNode != NULL )
			{
				node_num++;

				if ( node_num == 10 )
				{
		printf("%s(%d)\n", __FUNCTION__, __LINE__);
					break;
				}				
				pTempNode = pTempNode->pNextNode;
			}
			else
			{
					printf("%s(%d)\n", __FUNCTION__, __LINE__);
				break;
			}		
		}
		node_num = 0;
		printf("%s(%d)\n", __FUNCTION__, __LINE__);		
		if ( pTempNode->pNextNode != NULL )
		{
				printf("%s(%d)\n", __FUNCTION__, __LINE__);		
			pCurRootNode = pTempNode->pNextNode;
			pTempNode = pCurRootNode;	
			while(1)
			{
				if ( pTempNode->pNextNode != NULL )
				{
					node_num++;
					pTempNode = pTempNode->pNextNode;
					if ( node_num == 10 )
					{
						printf("%s(%d)\n", __FUNCTION__, __LINE__);		
						break;
					}				
				}
				else
				{
					printf("%s(%d)\n", __FUNCTION__, __LINE__);						
					break;
				}		
			}
			printf("total_node(%d)\n", node_num);
			pTempNode = pCurRootNode;
			pChannelCurPageNode = pTempNode;
			pTempNode2 = pChannelTestRootNode->pNextNode;
			for(tempInt = 1 ; tempInt < sChannelExtTestStatusInfo.scan_test_total_num_nodes ; tempInt++ )
			{
				if ( pChannelCurPageNode == pTempNode2 )
				{
					break;
				}

				if ( pTempNode2->pNextNode != NULL )
				{
					pTempNode2 = pTempNode2->pNextNode;
				}
				else
				{
					break;
				}

			}
			
			if ( (sChannelExtTestStatusInfo.scan_test_total_num_nodes % 10) != 0 )
			{
				total_pages = sChannelExtTestStatusInfo.scan_test_total_num_nodes /10 + 1;
			}
			else
			{
				total_pages = sChannelExtTestStatusInfo.scan_test_total_num_nodes /10;
			}
			
			if ( (tempInt % 10) != 0 )
			{
				cur_page = tempInt /10 + 1;
			}
			else
			{
				cur_page = tempInt /10;
			}
			HI_VK_MEM_Memset(scInfoDisplayString2, ' ', 4096);

			HI_VK_MEM_Memset(scInfoDisplayString3, ' ', 100);			
			sprintf(scInfoDisplayString3, " %d page / %d #", cur_page, total_pages);

			HI_VK_MEM_Memcpy(&scInfoDisplayString2[0], &scInfoDisplayString3[0], 100);
			for ( offset = 0 ; offset < node_num ; offset++ )
			{
				HI_VK_MEM_Memset(scInfoDisplayString3, 0x32, 100);
				if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT )
				{
				    P_CmdHwTestExt_ChannelTest_SAT_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.satInfo ) ;
					pTempNode = pTempNode->pNextNode;
					tempInt = offset * 100 + 100;
					HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
					/*scInfoDisplayString2[100*offset+100] = '\n';*/
					printf("%s\n", scInfoDisplayString3);
				}
				else if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER )
				{
				    P_CmdHwTestExt_ChannelTest_TER_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.terInfo ) ;
					pTempNode = pTempNode->pNextNode;
					tempInt = offset * 100 + 100;
					HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
					/*scInfoDisplayString2[100*offset+100] = '\n';*/
					printf("%s\n", scInfoDisplayString3);
				}
				else if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB )
				{
				    P_CmdHwTestExt_ChannelTest_CAB_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.cabInfo ) ;
					pTempNode = pTempNode->pNextNode;
					tempInt = offset * 100 + 100;
					HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
					/*scInfoDisplayString2[100*offset+100] = '\n';*/
					printf("%s\n", scInfoDisplayString3);
				}
			}

			scInfoDisplayString2[4096] = '\0';
			AP_HwTest_DisplayInfoEx(scInfoDisplayString2, 4096);
			*pNumNode = node_num;				
		}
	}
	else if ( offset_direction == 2 )
	{
		pTempNode = pCurRootNode;	
		node_num =0;
		while(1)
		{
			if ( pTempNode->pPrevNode != NULL )
			{

				pTempNode = pTempNode->pPrevNode;
				if ( node_num == 10 )
				{
					break;
				}				
				node_num++;
			}
			else
			{
				return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_NULL_POINTER;				
			}		
		}
		node_num = 0;
		if ( pTempNode->pNextNode != NULL )
		{
			pCurRootNode = pTempNode->pNextNode;
			pTempNode = pCurRootNode;	
			while(1)
			{
				if ( pTempNode->pNextNode != NULL )
				{
					node_num++;
					pTempNode = pTempNode->pNextNode;
					if ( node_num == 10 )
					{
						break;
					}				
				}
				else
				{
					break;
				}		
			}
			printf("total_node(%d)\n", node_num);
			pTempNode = pCurRootNode;
			pChannelCurPageNode = pTempNode;			
			HI_VK_MEM_Memset(scInfoDisplayString2, ' ', 4096);


			pTempNode2 = pChannelTestRootNode->pNextNode;
			for(tempInt = 1 ; tempInt < sChannelExtTestStatusInfo.scan_test_total_num_nodes ; tempInt++ )
			{
				if ( pChannelCurPageNode == pTempNode2 )
				{
					break;
				}


				if ( pTempNode2->pNextNode != NULL )
				{
					pTempNode2 = pTempNode2->pNextNode;
				}
				else
				{
					break;
				}

			}

			if ( (sChannelExtTestStatusInfo.scan_test_total_num_nodes % 10) != 0 )
			{
				total_pages = sChannelExtTestStatusInfo.scan_test_total_num_nodes /10 + 1;
			}
			else
			{
				total_pages = sChannelExtTestStatusInfo.scan_test_total_num_nodes /10;
			}

			if ( (tempInt % 10) != 0 )
			{
				cur_page = tempInt /10 + 1;
			}
			else
			{
				cur_page = tempInt /10;
			}

			HI_VK_MEM_Memset(scInfoDisplayString3, ' ', 100);			
			sprintf(scInfoDisplayString3, " %d page / %d #", cur_page, total_pages);

			HI_VK_MEM_Memcpy(&scInfoDisplayString2[0], &scInfoDisplayString3[0], 100);

			
			for ( offset = 0 ; offset < node_num ; offset++ )
			{
				HI_VK_MEM_Memset(scInfoDisplayString3, 0x32, 100);
				if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_SAT )
				{
    			    P_CmdHwTestExt_ChannelTest_SAT_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.satInfo ) ;
					pTempNode = pTempNode->pNextNode;
					tempInt = offset * 100+100;
					HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
					/*scInfoDisplayString2[100*offset+100] = '\n';*/
					printf("%s\n", scInfoDisplayString3);
				}
				else if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_TER )
				{
    			    P_CmdHwTestExt_ChannelTest_TER_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.terInfo ) ;
					pTempNode = pTempNode->pNextNode;
					tempInt = offset * 100+100;
					HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
					/*scInfoDisplayString2[100*offset+100] = '\n';*/
					printf("%s\n", scInfoDisplayString3);
				}
				else if ( pTempNode->nodeData.tunerType == CMD_HWTEST_EXT_CHANNEL_TEST_TUNERTYPE_CAB )
				{
    			    P_CmdHwTestExt_ChannelTest_CAB_NodeDisplayString(scInfoDisplayString3, offset, pTempNode->nodeData.cabInfo ) ;
					pTempNode = pTempNode->pNextNode;
					tempInt = offset * 100+100;
					HI_VK_MEM_Memcpy(&scInfoDisplayString2[tempInt], &scInfoDisplayString3[0], 100);
					/*scInfoDisplayString2[100*offset+100] = '\n';*/
					printf("%s\n", scInfoDisplayString3);
				}
			}

			scInfoDisplayString2[4096] = '\0';
			AP_HwTest_DisplayInfoEx(scInfoDisplayString2, 4096);
			*pNumNode = node_num;				
		}
	}	

}

/**********************************************************************************/
/* End of Parsing Functions                                                       */
/**********************************************************************************/

/**********************************************************************************/
/* Channel Functions                                                              */
/**********************************************************************************/

static CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE internal_cmdhwtestext_ChannelTest_TryLock
(
	CMDHWTESTEXT_CHANNEL_TEST_NODE *pNode
)
{

	sulHWChannelTestRequestID++;

	if (( pNode != NULL ))
	{
		if ( pNode->nodeData.tunerType == DI_CH_TYPE_SAT )
		{
	HI_CH_DI_RegisterCallback(
		HI_CH_TYPE_SAT, 
		pNode->nodeData.satInfo.tunerId, 
		internal_cmdhwtestext_ChannelTest_LockCallback);	
			return P_CmdHwTestExt_ChannelTest_SAT_LockTry(pNode->nodeData.satInfo,sulHWChannelTestRequestID);				
		}
		
		else if ( pNode->nodeData.tunerType == DI_CH_TYPE_TER )
	{
			HI_CH_DI_RegisterCallback(
				HI_CH_TYPE_TER, 
				pNode->nodeData.terInfo.tunerId, 
				internal_cmdhwtestext_ChannelTest_LockCallback);	

			return P_CmdHwTestExt_ChannelTest_TER_LockTry(pNode->nodeData.terInfo,sulHWChannelTestRequestID);				
		}	
	
		else if ( pNode->nodeData.tunerType == DI_CH_TYPE_CAB )
		{
			HI_CH_DI_RegisterCallback(
				HI_CH_TYPE_CAB, 
				pNode->nodeData.cabInfo.tunerId, 
				internal_cmdhwtestext_ChannelTest_LockCallback);	

			return P_CmdHwTestExt_ChannelTest_CAB_LockTry(pNode->nodeData.cabInfo,sulHWChannelTestRequestID);				
		}
	}
	return CMD_HWTEST_EXT_CHANNEL_TEST_ERROR_TYPE_OK;
}
/**********************************************************************************/
/* End of Channel Functions                                                       */
/**********************************************************************************/
/**********************************************************************************/
/* Public Functions                                                               */
/**********************************************************************************/
void P_CMDHWTEST_EXT_InitChannelTest(void)
{
    HI_VK_MSG_Create(30*4*sizeof(unsigned long), 4*sizeof(unsigned long), 
    	"CMDHWTESTChanQ", &sulCmdHwtestExtChannelTestQId, HI_VK_SUSPENDTYPE_FIFO);
    HI_VK_TASK_Create(internal_cmdhwtestext_ChannelTestTask, USER_PRIORITY6, 32*1024, 
		"CMDHWTESTChanT", NULL, &sulCmdHwtestExtChannelTestTId, HI_VK_SUSPENDTYPE_FIFO);
    HI_VK_TASK_Start(sulCmdHwtestExtChannelTestTId);
	internal_cmdhwtestext_ChannelTest_InitStringDictionary();
	printf("end of init %s\n", __FUNCTION__);
}
void P_CMDHWTEST_EXT_ChannelTestBegin
(
	CMD_HWTEST_EXT_CHANNEL_TEST_TYPE testType
)
{
	internal_cmdhwtestext_ChannelTest_SendMsgToTask(testType, 0, 0,0);
}
/**********************************************************************************/
/* End of Public Functions                                                        */
/**********************************************************************************/





