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
#include "cmd_hwtest_ext_thermal.h"
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

#define PRINT_ERROR_IN_CMD_HWTESTExt() printf(">>>>Error(%s,%d)\n", __FUNCTION__, __LINE__);


static unsigned long sulCmdHwtestExtThermalTestQid = 0;
static unsigned long sulCmdHwtestExtThermalTestTid = 0;
static HINT32 internal_cmdhwtestext_ThermalTest_KeyCallback(HI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, HI_KEY_TYPE eKeyType);
static void internal_cmdhwtestext_ThermalTestTask(void *pParam);
static unsigned long sulCmdHwtestExtThermalTestQid2 = 0;
static unsigned long sulCmdHwtestExtThermalTestTid2 = 0;

static unsigned long HddTempTimerId;
static unsigned long sulCmdHwtestTemperatureQId = 0;
static unsigned long sulCmdHwtestTemperatureTId = 0;
static HBOOL g_hbTemperatureTaskInit = FALSE;
static HBOOL g_hbTemperatureTaskStart = FALSE;

static unsigned long FanAutoTimerId;
unsigned long sulCmdHwtestFANQId = 0;
static unsigned long sulCmdHwtestFANTId = 0;
static HBOOL g_hbFanTaskInit = FALSE;
static HBOOL g_hbFanMonitorAuto = FALSE;



static void internal_cmdhwtestext_TempValueDetectionTask(void *pParam);
static unsigned long sulCmdHwtestExtDisplayReadyStatus = 0;
void P_CMDHWTEST_EXT_ThermalTest_BeginThermalTest(unsigned int isStart, unsigned int mode)
{   
    unsigned long s_msg[4];
    if ( isStart == 1 )
    {
        s_msg[0] = 1;
        s_msg[1] = mode;
        s_msg[2] = 0;
        s_msg[3] = 0;
        HI_KEY_RegisterKeyEventCallback(internal_cmdhwtestext_ThermalTest_KeyCallback);
        HI_VK_MSG_Send(sulCmdHwtestExtThermalTestQid, s_msg, 4*sizeof(unsigned long));
    }
    else if ( isStart == 10 )
    {
        s_msg[0] = 10;
        s_msg[1] = mode;
        s_msg[2] = 0;
        s_msg[3] = 0;
        HI_KEY_RegisterKeyEventCallback(internal_cmdhwtestext_ThermalTest_KeyCallback);
        HI_VK_MSG_Send(sulCmdHwtestExtThermalTestQid, s_msg, 4*sizeof(unsigned long));
    }
    else 
    {
        s_msg[0] = 0;
        s_msg[1] = 0;
        s_msg[2] = 0;
        s_msg[3] = 0;
    	HI_KEY_RegisterKeyEventCallback(AP_HwTest_KeyCallBack);        
        HI_VK_MSG_Send(sulCmdHwtestExtThermalTestQid, s_msg, 4*sizeof(unsigned long));
    }
}
static HINT32 internal_cmdhwtestext_ThermalTest_KeyCallback(HI_KEY_DEVICE eDevice, HUINT32 ulKeyCode, HI_KEY_TYPE eKeyType)
{
    unsigned long s_msg[4];
    s_msg[0] = 3;
    s_msg[1] = eDevice;
    s_msg[2] = ulKeyCode;
    s_msg[3] = eKeyType;
    HI_VK_MSG_Send(sulCmdHwtestExtThermalTestQid, s_msg, 4*sizeof(unsigned long));    
}
#define INTERNAL_CMDHWTESTEXT_FAN_MAX_PWM_LEVEL 255
#define INTERNAL_CMDHWTESTEXT_FAN_DEFAULT_PWM_LEVEL 180
static unsigned long sulCmdHwtestExtThermalTestFanLevel = INTERNAL_CMDHWTESTEXT_FAN_DEFAULT_PWM_LEVEL;

static void internal_cmdhwtestext_util_inttochar(unsigned int inValue, char *pReturnCh)
{
    switch(inValue)
    {
        case 0:
            *pReturnCh = '0';
            break;
        case 1:
            *pReturnCh = '1';
            break;
        case 2:
            *pReturnCh = '2';
            break;
        case 3:
            *pReturnCh = '3';
            break;
        case 4:
            *pReturnCh = '4';
            break;
        case 5:
            *pReturnCh = '5';
            break;
        case 6:
            *pReturnCh = '6';
            break;
        case 7:
            *pReturnCh = '7';
            break;
        case 8:
            *pReturnCh = '8';
            break;
        case 9:
            *pReturnCh = '9';
            break;
            
    }
}
static void internal_cmdhwtestext_util_intToString(unsigned int inValue, char *pReturn, int length)
{
    int tempLength = length;
    int tempInValue = inValue;
    int tempInt = 0;
    int tempdivider  = 0;
    int tempValue = 0;
    int tempbufindex = 0;
    char tempCh = 0x00;



    while(tempLength != 0 )
    {
        tempdivider = 1;
        for ( tempInt = 0 ; tempInt < (tempLength-1) ; tempInt++ )
        {
            tempdivider = tempdivider * 10;
        }

        tempValue = tempInValue / tempdivider ;
        internal_cmdhwtestext_util_inttochar(tempValue, &tempCh);
        pReturn[tempbufindex] = tempCh;

        tempInValue = tempInValue % tempdivider;
        tempLength--;
        tempbufindex++;
    }    
    
}

static void internal_cmdhwtestext_SetPwmForFan(unsigned long pwm_level)
{
    if ( pwm_level > INTERNAL_CMDHWTESTEXT_FAN_MAX_PWM_LEVEL )
    {
        sulCmdHwtestExtThermalTestFanLevel = INTERNAL_CMDHWTESTEXT_FAN_MAX_PWM_LEVEL;
    }
    else 
    {
        sulCmdHwtestExtThermalTestFanLevel = pwm_level;
    }
#if defined(CONFIG_FAN)
    DRV_FAN_SetOnInterval(sulCmdHwtestExtThermalTestFanLevel);
#endif
}
static void internal_cmdhwtestext_SetPwmForFanByChangeDegree(unsigned long change_pwm_level, unsigned int isUp)
{
    if ( change_pwm_level == 0 )
    {
        return;
    }
    if ( isUp == 1 )
    {
        sulCmdHwtestExtThermalTestFanLevel = sulCmdHwtestExtThermalTestFanLevel + change_pwm_level;
    }
    else
    {
        sulCmdHwtestExtThermalTestFanLevel = sulCmdHwtestExtThermalTestFanLevel - change_pwm_level;
    }
    if ( sulCmdHwtestExtThermalTestFanLevel > INTERNAL_CMDHWTESTEXT_FAN_MAX_PWM_LEVEL )
    {
        sulCmdHwtestExtThermalTestFanLevel = INTERNAL_CMDHWTESTEXT_FAN_MAX_PWM_LEVEL;
    }
#if defined(CONFIG_FAN)
    DRV_FAN_SetOnInterval(sulCmdHwtestExtThermalTestFanLevel);
#endif
}
static void internal_cmdhwtestext_SetPwmForFanReset(void)
{
    sulCmdHwtestExtThermalTestFanLevel = INTERNAL_CMDHWTESTEXT_FAN_MAX_PWM_LEVEL;
#if defined(CONFIG_FAN) 
    DRV_FAN_SetOnInterval(0);
#endif
}
void internal_cmdhwtestext_thermaltestPVRTestReady(void)
{
    sulCmdHwtestExtDisplayReadyStatus = 0;    
}

static void internal_cmdhwtestext_FanAutoMonitorTimeNotify(unsigned long tid, void *arg)
{
	unsigned long s_msg;

	s_msg = NULL;
	
	VK_MSG_Send(sulCmdHwtestFANQId, &s_msg, sizeof(unsigned long));
}

static void internal_cmdhwtestext_FanMonitorTask(void *pParam)
{
	unsigned long r_msg;
	unsigned char temperature_value;
	unsigned int error_value;
	char tempString[30];
	unsigned int prepare_rate = 0;
	unsigned long tempInt;
#if defined(CONFIG_FAN)
	DRV_FAN_SETTING fanSetting;
#endif
	
	while(1)
	{
		error_value = HI_VK_MSG_Receive(sulCmdHwtestFANQId, &r_msg, sizeof(unsigned long));
		if ( error_value == 0 )
		{
			if(r_msg == HI_KEY_REMOCON_YELLOW || g_hbFanMonitorAuto)
			{
				tempString[30] = '\0';
#if defined(CONFIG_FAN)
				DRV_FAN_GetInfo(&fanSetting);
#endif	
				VK_MEM_Memset(tempString, 0x00, 30);
				VK_MEM_Memcpy(tempString, "PWM Duty Level:   /255\n		   ", 30);
#if defined(CONFIG_FAN)
				internal_cmdhwtestext_util_intToString(fanSetting.usPWM_OnInterval, &tempString[15], 3);				 			
#endif
				P_SendNewMsgStringToHandler(tempString);
				
				VK_MEM_Memset(tempString, 0x00, 30);
				VK_MEM_Memcpy(tempString, "FAN Freqency :	  Hz\n			 ", 30);
#if defined(CONFIG_FAN)
				internal_cmdhwtestext_util_intToString(fanSetting.ulFanSpeed_Freq, &tempString[15], 3);
#endif
				P_SendNewMsgStringToHandler(tempString);
			}

		}
	}
}

static void internal_cmdhwtestext_HddTemperatureTimeNotify(unsigned long tid, void *arg)
{
	unsigned long s_msg;

	s_msg = NULL;
	
	VK_MSG_Send(sulCmdHwtestTemperatureQId, &s_msg, sizeof(unsigned long));
}

static void internal_cmdhwtestext_HDDTempMonitorTask(void *pParam)
{
	unsigned long r_msg;
	char temperature_value;
	unsigned int error_value;
	char tempString[30];
	unsigned int prepare_rate = 0;
	unsigned long tempInt;
	
#if defined(CONFIG_HDD_SMART)
	DRV_SMART_Read_Attr_Temp(&temperature_value);
#endif

	VK_MEM_Memset(tempString, 0x00, 30);				
	VK_MEM_Memcpy(tempString, ">>HDD Temperature :	  Celsi \n		", 30);
	internal_cmdhwtestext_util_intToString(temperature_value, &tempString[17], 3);				  
	P_SendNewMsgStringToHandler(tempString);

	
	while(1)
	{
		error_value = HI_VK_MSG_Receive(sulCmdHwtestTemperatureQId, &r_msg, sizeof(unsigned long));
		if ( error_value == 0 )
		{
			tempString[30] = '\0';

#if defined(CONFIG_HDD_SMART)
			DRV_SMART_Read_Attr_Temp(&temperature_value);
#endif

			VK_MEM_Memset(tempString, 0x00, 30);                
			VK_MEM_Memcpy(tempString, ">>HDD Temperature :    Celsi \n      ", 30);
			internal_cmdhwtestext_util_intToString(temperature_value, &tempString[17], 3);                
			P_SendNewMsgStringToHandler(tempString);
		}
	}
}

static void internal_cmdhwtestext_ThermalTestTask(void *pParam)
{
    unsigned long r_msg[4];
    unsigned long s_msg[4];
    unsigned int tunerId;
    unsigned char InfoString1[500]="   Thermal Test In Progress - 1P+2R from Tuner\n   Usage\n   Up/Down Arraw Key - Change PWM Output level 1\n   Page Up/Down Key - Change PWM Output level 10\n   Exit Key - Quit this test\n";
    unsigned char InfoString2[500]="   Thermal Test In Progress - 1P+2R from HDD\n   Usage\n   Up/Down Arraw Key - Change PWM Output level 1\n   Page Up/Down Key - Change PWM Output level 10\n   Exit Key - Quit this test\n";
    unsigned char InfoString3[500]="   Thermal Test In Progress \n      Copy Stream from USB To HDD\n";
    while(1)
    {
        HI_VK_MSG_Receive(sulCmdHwtestExtThermalTestQid, r_msg, 4*sizeof(unsigned long));
        switch(r_msg[0])
        {
            case 1: /* Test Begin */
                s_msg[0] = 1;
                s_msg[1] = 2000;
                s_msg[2] = 0;
                s_msg[3] = 0;
                sulCmdHwtestExtDisplayReadyStatus  = 1;
                internal_cmdhwtestext_SetPwmForFan(INTERNAL_CMDHWTESTEXT_FAN_DEFAULT_PWM_LEVEL);
                VK_MSG_Send(sulCmdHwtestExtThermalTestQid2, s_msg, 4*sizeof(unsigned long));
                CMD_RegisterPVRTestReadyCallback(internal_cmdhwtestext_thermaltestPVRTestReady);
                if ( r_msg[1] == 0 )
                {
                    AP_HwTest_DisplayInfo(InfoString1);
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test1.nts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test2.nts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test3.nts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test4.nts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test1.ts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test2.ts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test3.ts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test4.ts");                    
                    P_LockingEx(&tunerId);
                    HI_VK_TASK_Sleep(5000);
                    P_PvrStartEx(tunerId,0,0);
                }
                else 
                {
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test1.nts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test2.nts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test3.nts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test4.nts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test1.ts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test2.ts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test3.ts");
                    VK_SYSTEM_Command("rm -rf /mnt/hd2/hdd_thermal_test4.ts");

                    AP_HwTest_DisplayInfo(InfoString2);                
                    P_PvrStartEx(0,0,1);
                }
/*                P_PvrStartEx(1);*/
                break;
            case 2: /* Test Stop */
                s_msg[0] = 0;
                s_msg[1] = 2000;
                s_msg[2] = 0;
                s_msg[3] = 0;
                P_PvrStartEx(0,1,0);
                internal_cmdhwtestext_SetPwmForFanReset();
              	HI_KEY_RegisterKeyEventCallback(AP_HwTest_KeyCallBack);
                VK_MSG_Send(sulCmdHwtestExtThermalTestQid2, s_msg, 4*sizeof(unsigned long));   
                CMD_RegisterPVRTestReadyCallback(NULL);
                break;
            case 3: /* RCU Received */
                if (r_msg[1] == HI_RCU_KEY )
                {
                    switch(r_msg[2])
                    {
                        case HI_KEY_REMOCON_ARROWUP:
                            internal_cmdhwtestext_SetPwmForFanByChangeDegree(1,1);
                            break;
                        case HI_KEY_REMOCON_ARROWDOWN:
                            internal_cmdhwtestext_SetPwmForFanByChangeDegree(1,0);
                            break;
                        case HI_KEY_REMOCON_CH_PLUS:
                            internal_cmdhwtestext_SetPwmForFanByChangeDegree(10,1);                            
                            break;
                        case HI_KEY_REMOCON_CH_MINUS:
                            internal_cmdhwtestext_SetPwmForFanByChangeDegree(10,0);                            
                            break;
                        case HI_KEY_REMOCON_EXIT:
                            s_msg[0] = 2;
                            s_msg[1] = 0;
                            s_msg[2] = 0;
                            s_msg[3] = 0;
                            HI_VK_MSG_Send(sulCmdHwtestExtThermalTestQid, s_msg, 4*sizeof(unsigned long));
                            break;
                        default: 
                            continue;
                            break;
                    }
                }
                else
                {
                    continue;
                }
                break;
            case 4: /* Thermal Value detected */
                break;
            case 5: /* RPM Detected */
                break;
            case 10:
                AP_HwTest_DisplayInfo(InfoString3);
                P_SendNewMsgStringToHandler("Coyping Stream File\n");
                VK_SYSTEM_Command("cp /media/hdd_thermal_stream.ts /mnt/hd2/");
                P_SendNewMsgStringToHandler("Coyping Stream Info Files\n");                
                VK_SYSTEM_Command("cp /media/hdd_thermal_stream.nts /mnt/hd2/");
                VK_SYSTEM_Command("cp /media/hdd_thermal_stream.info /mnt/hd2/");
                P_SendNewMsgStringToHandler("All files are copied\n");         
                s_msg[0] = 2;
                s_msg[1] = 0;
                s_msg[2] = 0;
                s_msg[3] = 0;
                HI_VK_MSG_Send(sulCmdHwtestExtThermalTestQid, s_msg, 4*sizeof(unsigned long));                
                break;
                
            default:
                break;                
        }
    }
}
static void internal_cmdhwtest_TempValueDetectionTask(void *pParam)
{
    unsigned long timeoutValue = 0xFFFFFFFF;
    unsigned long update_begin = 0;
    unsigned long r_msg[4];
#if defined(CONFIG_FAN)
    DRV_FAN_SETTING fanSetting;
#endif
    char temperature_value;
    unsigned int error_value;
    char tempString[16];
    char tempString2[20];
    char tempString3[30];
    unsigned int prepare_rate = 0;
    unsigned long tempInt;
    while(1)
    {
        error_value = HI_VK_MSG_ReceiveTimeout(sulCmdHwtestExtThermalTestQid2, r_msg, 4*sizeof(unsigned long), timeoutValue);
        if ( error_value == 2 )
        {
            if ( update_begin == 1 )
            {
#if defined(CONFIG_FAN)
                DRV_FAN_GetInfo(&fanSetting);
#endif
#if defined(CONFIG_HDD_SMART)
			    DRV_SMART_Read_Attr_Temp(&temperature_value);
#endif

                tempString[0] ='P';
#if defined(CONFIG_FAN)
                internal_cmdhwtestext_util_intToString(fanSetting.usPWM_OnInterval, &tempString[1], 3);
#endif
                tempString[4] = '/';
                tempString[5] = 'F';
#if defined(CONFIG_FAN)
                internal_cmdhwtestext_util_intToString(fanSetting.ulFanSpeed_Freq, &tempString[6], 4);
#endif
                tempString[10] = '/';
                tempString[11] = 'T';
                internal_cmdhwtestext_util_intToString(temperature_value, &tempString[12], 3);               
                tempString[15] = '\0';
                for ( tempInt = 0 ; tempInt < 15; tempInt++ )
                {
                    tempString2[tempInt] = tempString[tempInt];
                }
                tempString2[15] = '\n';
                tempString2[16] = '\0';
                DI_FRONT_PANEL_Display(tempString);

                VK_MEM_Memset(tempString3, 0x00, 30);
                VK_MEM_Memcpy(tempString3, "PWM Duty Level:   /255\n           ", 30);
#if defined(CONFIG_FAN)
                internal_cmdhwtestext_util_intToString(fanSetting.usPWM_OnInterval, &tempString3[15], 3);                
#endif
                tempString3[30] = '\0';

                P_SendNewMsgStringToHandler(tempString3);
                VK_MEM_Memset(tempString3, 0x00, 30);
                VK_MEM_Memcpy(tempString3, "FAN Freqency :    Hz\n           ", 30);
#if defined(CONFIG_FAN)
                internal_cmdhwtestext_util_intToString(fanSetting.ulFanSpeed_Freq, &tempString3[15], 3);                
#endif


                P_SendNewMsgStringToHandler(tempString3);
                VK_MEM_Memset(tempString3, 0x00, 30);                
                VK_MEM_Memcpy(tempString3, "HDD Temperature :    Celsi\n      ", 30);
                internal_cmdhwtestext_util_intToString(temperature_value, &tempString3[17], 3);                
                P_SendNewMsgStringToHandler(tempString3);


                if  ( sulCmdHwtestExtDisplayReadyStatus == 1 )
                {
                    P_SendNewProgressMsgStringToHandler("Preparing...", prepare_rate);
                    prepare_rate = prepare_rate + 1;
                }
                else if ( sulCmdHwtestExtDisplayReadyStatus == 0 && prepare_rate != 0 )
                {
                    P_SendNewProgressMsgStringToHandler("Preparing...", 100);

                    prepare_rate = 0;
                }
            }
            else
            {
                timeoutValue == 0xFFFFFFFF;
                continue;
            }
        }
        else if (  error_value == 0 )
        {
            switch(r_msg[0])
            {
                case 0:
                    update_begin = 0 ;
                    prepare_rate = 0;
                    timeoutValue = 0xFFFFFFFF;
                    break;
                case 1:
                    update_begin = 1;
                    timeoutValue = r_msg[1];
                    break;
                default:
                    break;                        
            }
        }
        else
        {
            continue;
        }
            
        
    }
}

void P_CMDHWTEST_EXT_InitThermalTest(void)
{
#if defined(USE_RECORD) && defined(USE_HDD_SMART)    
    HI_VK_MSG_Create(30*4*sizeof(unsigned long), 4*sizeof(unsigned long), "CMDHWTESTTherQ", &sulCmdHwtestExtThermalTestQid, HI_VK_SUSPENDTYPE_FIFO);
    HI_VK_TASK_Create(internal_cmdhwtestext_ThermalTestTask, USER_PRIORITY6, 32*1024, "CMDHWTESTTherT", NULL, &sulCmdHwtestExtThermalTestTid, HI_VK_SUSPENDTYPE_FIFO);
    HI_VK_TASK_Start(sulCmdHwtestExtThermalTestTid);

    HI_VK_MSG_Create(30*4*sizeof(unsigned long), 4*sizeof(unsigned long), "CMDHWTESTTherQ2", &sulCmdHwtestExtThermalTestQid2, HI_VK_SUSPENDTYPE_FIFO);
    HI_VK_TASK_Create(internal_cmdhwtest_TempValueDetectionTask, USER_PRIORITY6, 32*1024, "CMDHWTESTTherT2", NULL, &sulCmdHwtestExtThermalTestTid2, HI_VK_SUSPENDTYPE_FIFO);
    HI_VK_TASK_Start(sulCmdHwtestExtThermalTestTid2);
#endif
}

void P_CMDHWTEST_StartHDDTempMonitor(void)
{
#if defined(USE_RECORD) && defined(USE_HDD_SMART)   
	if(g_hbTemperatureTaskInit==FALSE)
	{
		HI_VK_MSG_Create(30*sizeof(unsigned long), sizeof(unsigned long), "CMDHWTESTTempTest", &sulCmdHwtestTemperatureQId, HI_VK_SUSPENDTYPE_FIFO);
		HI_VK_TASK_Create(internal_cmdhwtestext_HDDTempMonitorTask, USER_PRIORITY6, 32*1024, "CMDHWTESTTempTest", NULL, &sulCmdHwtestTemperatureTId, HI_VK_SUSPENDTYPE_FIFO);
		g_hbTemperatureTaskInit = TRUE;
}

	if(g_hbTemperatureTaskStart==FALSE)
	{
		HI_VK_TASK_Start(sulCmdHwtestTemperatureTId);
		VK_TIMER_EventEvery(60000, internal_cmdhwtestext_HddTemperatureTimeNotify, NULL, 0, &HddTempTimerId);
		g_hbTemperatureTaskStart=TRUE;
		P_SendNewMsgStringToHandler(">>HDD Temp Monitor START ");
	}
#endif	
}
void P_CMDHWTEST_StopHDDTempMonitor(void)
{
#if defined(USE_RECORD) && defined(USE_HDD_SMART)   
	if(g_hbTemperatureTaskStart==TRUE)
	{
		VK_TIMER_Cancel(HddTempTimerId);
		HI_VK_TASK_Stop(sulCmdHwtestTemperatureTId);
		g_hbTemperatureTaskStart=FALSE;
		P_SendNewMsgStringToHandler(">>HDD Temp Monitor STOP ");
	}
#endif	
}
void P_CMDHWTEST_InitFanMonitor(void)
{
#if defined(USE_FAN)
	if(g_hbFanTaskInit==FALSE)
	{
		HI_VK_MSG_Create(30*sizeof(unsigned long), sizeof(unsigned long), "CMDHWTESTFanTest", &sulCmdHwtestFANQId, HI_VK_SUSPENDTYPE_FIFO);
		HI_VK_TASK_Create(internal_cmdhwtestext_FanMonitorTask, USER_PRIORITY6, 32*1024, "CMDHWTESTFanTest", NULL, &sulCmdHwtestFANTId, HI_VK_SUSPENDTYPE_FIFO);
		HI_VK_TASK_Start(sulCmdHwtestFANTId);	

		g_hbFanTaskInit = TRUE;
	}
#endif	
}
void P_CMDHWTEST_StartFanAutoMonitor(void)
{
#if defined(USE_FAN)
	if(g_hbFanTaskInit==TRUE)
	{
		if(g_hbFanMonitorAuto==FALSE)
		{
			VK_TIMER_EventEvery(3000, internal_cmdhwtestext_FanAutoMonitorTimeNotify, NULL, 0, &FanAutoTimerId);
			g_hbFanMonitorAuto=TRUE;
			P_SendNewMsgStringToHandler(">>FAN Auto Monitor START ");
		}
	}
#endif	
}

void P_CMDHWTEST_StopFanAutoMonitor(void)
{
#if defined(USE_FAN)
	if(g_hbFanTaskInit==TRUE)
	{
		if(g_hbFanMonitorAuto==TRUE)
		{
			VK_TIMER_Cancel(FanAutoTimerId);
			g_hbFanMonitorAuto=FALSE;
			P_SendNewMsgStringToHandler(">>FAN Auto Monitor STOP ");
		}
	}
#endif	
}


