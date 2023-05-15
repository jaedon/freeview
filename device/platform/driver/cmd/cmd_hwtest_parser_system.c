#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif
#include <time.h>
#include <unistd.h>

#if 0
#include "bchp_ddr_iobuf.h"
#include "bchp_memc_0_ddr.h"
#include "bchp_memc_1_ddr.h"
#endif

#include "vkernel.h"
#include "htype.h"
#include "taskdef.h"
#include "cmdtool.h"
#include "cmd_hwtest_interface.h"
#include "cmd_hwtest_parser_system.h"

#include "di_front_panel.h"
#include "di_system.h"
#include "drv_err.h"
#include "drv_i2c.h"
#if defined(CONFIG_CABLE_MODEM)
#include "di_cm.h"
#endif

#include "nexus_platform.h"
#include "nexus_memory.h"

#include <sys/time.h>

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

#define UNUSED(x) (void)(x)


//#define	_DEBUG_
//#define	_DEBUG_ENTER_EXIT_

#ifdef	_DEBUG_
#define	PrintData	DI_UART_Print
#define	PrintError	DI_UART_Print
#else
#define	PrintData	while (0) ((int (*)(char *, ...)) 0)
#define	PrintError	DI_UART_Print
#endif

#ifdef	_DEBUG_ENTER_EXIT_
#define	PrintEnter()	DI_UART_Print("(+)%s\n\r", __FUNCTION__)
#define	PrintExit()	DI_UART_Print("(-)%s\n\r", __FUNCTION__)
#else
#define	PrintEnter()
#define	PrintExit()
#endif

#if defined(CONFIG_SUPPORT_CPU_TEMP)
unsigned int	max_cpu_temp = 120;
HBOOL	cpu_log = 0;

static void Cmd_TempTimer(unsigned long tid, void *arg)
{
	DI_SYS_CPU_INFO_t cpuInfo;
	static int cpuMax = 0;
#if defined(CONFIG_CABLE_MODEM)		
	HINT32		ulTemp;
	HINT32		ulMax;
#endif

	BSTD_UNUSED(tid);
	BSTD_UNUSED(arg);

	DI_SYSTEM_GetSystemInfo(DI_SYS_CPU_INFO, (void *)&cpuInfo, sizeof(DI_SYS_CPU_INFO_t));
#if defined(CONFIG_CABLE_MODEM)	
	DI_CM_GetCmSavedTemp(&ulMax,&ulTemp);
#endif
	if(cpuInfo.ulTemp > cpuMax)
	{
		cpuMax = cpuInfo.ulTemp;
	}
#if defined(CONFIG_CABLE_MODEM)
	CMD_Printf("CPU Temp(%d) Max(%d), CM Temp(%d.%d) Max(%d.%d)\n",cpuInfo.ulTemp/1000,cpuMax/1000,ulTemp/10,ulTemp%10,ulMax/10,ulMax%10);
#else	
	CMD_Printf("CPU Temp(%d) Max(%d)\n",cpuInfo.ulTemp/1000,cpuMax/1000);
#endif
}

#endif

int CMD_HwTest_Parser_Cpu(void *szArgs)
{
#if defined(CONFIG_SUPPORT_CPU_TEMP)
	DI_SYS_CPU_INFO_t cpuInfo;
	DI_ERR_CODE diError;
	unsigned char tempString[200];
#endif
	GET_ARGS;

	UNUSED(szParam);
	UNUSED(szParam1);
	UNUSED(szParam2);

	if CMD_IS("temp")
	{
#if defined(CONFIG_SUPPORT_CPU_TEMP)
		if (HWTEST_PARAM)
		{
			static HUINT32 snCmdTmpTimer = 0;
			if PARAM_IS("aging")
			{
				if PARAM1_IS("on")
				{
					CMD_Printf("Temp aging is on\n");
					if(snCmdTmpTimer ==0)
					{
						VK_TIMER_EventEvery((unsigned long)1000,
							Cmd_TempTimer,NULL,0,(unsigned long *)&snCmdTmpTimer);
					}

				}
				else
				{
					if(snCmdTmpTimer != 0)
					{
						CMD_Printf("Temp aging is off\n");
						VK_TIMER_Cancel(snCmdTmpTimer);
						snCmdTmpTimer = 0;
						
					}
				}
				iResult=CMD_OK;
			}
		}
		else
		{
			diError = DI_SYSTEM_GetSystemInfo(DI_SYS_CPU_INFO, (void *)&cpuInfo, sizeof(DI_SYS_CPU_INFO_t));
			if (diError != DI_ERR_OK)
			{
				CMD_Printf("[%s] DI_SYSTEM_GetSystemInfo(DI_SYS_CPU_INFO) Error(0x%x)\n", diError);

		        if (CMD_GetMsgUI() != NULL)
		        {
		            (int *)(CMD_GetMsgUI())("Reading CPU temp error\n");
		        }
			}
			else
			{
				CMD_Printf("[%s] CPU Temp = %d C(%d.%03d V)\n", __FUNCTION__, cpuInfo.ulTemp / 1000, cpuInfo.ulVoltage / 1000, cpuInfo.ulVoltage % 1000);

		        if (CMD_GetMsgUI() != NULL)
		        {
					VK_snprintf(tempString, 199, "CPU Temp = %d C(%d.%03d V)\n", cpuInfo.ulTemp / 1000, cpuInfo.ulVoltage / 1000, cpuInfo.ulVoltage % 1000);
		            (int *)(CMD_GetMsgUI())(tempString);
		        }
			}
		}
#endif
	}
	else if CMD_IS("maxtemp")
	{
#if defined(CONFIG_SUPPORT_CPU_TEMP)
		unsigned int	maxtemp;

		if (HWTEST_PARAM)
		{
			maxtemp = strtol(HWTEST_PARAM, NULL, 10);
			if (maxtemp > 0)
			{
				max_cpu_temp = maxtemp;
				CMD_Printf("[%s] max temperature of cpu is set to %d \n", __FUNCTION__, max_cpu_temp);
			}
		}
#endif //defined(CONFIG_SUPPORT_CPU_TEMP)
	}
	else if CMD_IS("log")
	{
#if defined(CONFIG_SUPPORT_CPU_TEMP)
		if PARAM_IS("on")
		{
			cpu_log = TRUE;
			CMD_Printf("cpu log on\n");
		}
		else if PARAM_IS("off")
		{
			cpu_log = FALSE;
			CMD_Printf("cpu log off\n");
		}
#endif //defined(CONFIG_SUPPORT_CPU_TEMP)
	}
	else if CMD_IS("usage")
	{
		unsigned int unCPUUsage=0;
		DI_SYSTEM_GetCPUUsage(&unCPUUsage);
		CMD_Printf("CPU USAGE = %d percentage\n",unCPUUsage);

		return CMD_OK;
	}
	else if CMD_IS("process")
	{
		DI_SYS_PROCESS_STATUS_TABLE_t processStatus;
		HUINT32 i;

		DI_SYSTEM_GetProcessStatus	(&processStatus);

			for(i=0; i< processStatus.unNumOfEntries;i++)
			{
				CMD_Printf(" i = %d , .unPid = 0x%x[%d] ",i,processStatus.ProcessStatus[i].unPid,processStatus.ProcessStatus[i].unPid);
				CMD_Printf(" eState = %d, CPUTIME=%d, priority =%d, cmd=%s, size = %d\n",processStatus.ProcessStatus[i].eState,processStatus.ProcessStatus[i].unCPUTime,processStatus.ProcessStatus[i].unPriority,processStatus.ProcessStatus[i].strCommand,processStatus.ProcessStatus[i].unSize);
			}
			CMD_Printf("TotalNumOfEntry = %d\n", processStatus.unNumOfEntries);

		return CMD_OK;
	}
	else if CMD_IS("arch")
	{
		DI_SYS_PROCESSOR_ARCH_TABLE_t ProcessorArchtecture;
		HUINT32 i;

		DI_SYSTEM_GetProcessorInfo(&ProcessorArchtecture);

		CMD_Printf(" Num of Processor : %d \n", ProcessorArchtecture.unNumOfEntries);
		for(i=0; i< ProcessorArchtecture.unNumOfEntries; i++)
		{
			CMD_Printf(" Processor Architecture[%d] : %s \n",i, ProcessorArchtecture.ProcessorArch[i].strArch);
		}

		return CMD_OK;
	}

    return 0;
}

int CMD_HwTest_Parser_Standby(void *szArgs)
{
	UNUSED(szArgs);
    return 0;
}

int CMD_HwTest_Parser_Vcxo(void *szArgs)
{
	UNUSED(szArgs);
    return 0;
}

int CMD_HwTest_Parser_Mem(void *szArgs)
{
	GET_ARGS;

	UNUSED(szParam);
	UNUSED(szParam1);
	UNUSED(szParam2);
	if CMD_IS("info")
	{
		VK_SYSTEM_Command("cat /proc/meminfo");
		iResult=CMD_OK;
	}
	else if CMD_IS("alloc")
	{
		char *test_buf;
		int input_val;

		DI_UART_Print("+=========================================================================\n");
		VK_SYSTEM_Command("free");
		DI_UART_Print("--------------------------------------------------------------------------\n");
		READABLE_IN_DEC(HWTEST_PARAM, input_val);
		DI_UART_Print("%dMB is allocated.\n", input_val);
		input_val = input_val * 1024 * 1024;
		test_buf = (char*)VK_MEM_Alloc(sizeof(int)*input_val);
		DI_UART_Print("--------------------------------------------------------------------------\n");
		VK_MEM_Memset(test_buf , 0, input_val);
		VK_SYSTEM_Command("free");
		DI_UART_Print("-=========================================================================\n");
		iResult=CMD_OK;
	}
	else
	{
		DI_UART_Print("Wrong Command :: right command = mem info\n");
	}

	return iResult;
}


int CMD_HwTest_Parser_Bmem(void *szArgs)
{
	GET_ARGS;

	UNUSED(szParam);
	UNUSED(szParam1);
	UNUSED(szParam2);
	if CMD_IS("info")
	{
#if (NEXUS_VERSION >= 1402)
		NEXUS_Memory_PrintHeaps();
#endif
		if (szParam == NULL)
		{
			NEXUS_PlatformConfiguration	platformConfig;
			HINT32						i;

			NEXUS_Platform_GetConfiguration(&platformConfig);
			for (i=0;i<NEXUS_MAX_HEAPS;i++)
			{
				if (platformConfig.heap[i])
				{
					/* Nexus requires export debug_mem=y to track allocations. */
					DI_UART_Print("+=========================================================================\n", i);
					DI_UART_Print("Heap[%d] dump:\n", i);
					NEXUS_Heap_Dump(platformConfig.heap[i]);
					DI_UART_Print("-=========================================================================\n", i);
				}
			}
		}
		else
		{
			NEXUS_PlatformConfiguration	platformConfig;
			HINT32						i = VK_atoi((char *)szParam);

			NEXUS_Platform_GetConfiguration(&platformConfig);
			if (platformConfig.heap[i])
			{
				/* Nexus requires export debug_mem=y to track allocations. */
				DI_UART_Print("+=========================================================================\n", i);
				DI_UART_Print("Heap[%d] dump:\n", i);
				NEXUS_Heap_Dump(platformConfig.heap[i]);
				DI_UART_Print("-=========================================================================\n", i);
			}
		}

		iResult=CMD_OK;
	}
	else
	{
		DI_UART_Print("Wrong Command :: right command = mem info\n");
	}

	return iResult;
}

static unsigned long suiI2CTestKicking = 0;
static unsigned long suiI2CTestTaskID = 0;

static char s_i2cTaskCaTempStr[200];


static void CMD_HwTest_Parser_I2c_TASK(void *pParam)
{
    int iCount=0;

    unsigned int iDevId;
    unsigned int iChipAddr;
    unsigned char ucBuff;
  	DRV_Error drverror;

    char *pStringDisplay = NULL;
    if ( pParam == NULL )
    {

    }

    while(1)
    {
        if ( suiI2CTestKicking == 1 )
        {
            VK_MEM_Memset((void *)&s_i2cTaskCaTempStr[0], 0x00, 200);
            suiI2CTestKicking = 0;
            pStringDisplay = CMD_AddString(pStringDisplay, "\n", 1, 1);
            for (iDevId=0 ; iDevId<NEXUS_NUM_I2C_CHANNELS ; iDevId++)
            {
                VK_snprintf(&s_i2cTaskCaTempStr[0], (int)18, "  \nI2C Port%03d : ", iDevId);
                pStringDisplay = CMD_AddString(pStringDisplay, &s_i2cTaskCaTempStr[0], 18, 0);
                VK_TASK_Sleep(10);
                iCount=0;
                for (iChipAddr=0 ; iChipAddr<=0x7f ; iChipAddr++)
                {
                    drverror = DRV_I2c_ReadA8_NoRetry(iDevId, iChipAddr, 0, &ucBuff, 1);
                    /*VK_printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>iChipAddr, drvError(%d,%08x)\n", iChipAddr, drverror);*/
                    if (drverror==DRV_OK)
                    {
                        VK_snprintf(&s_i2cTaskCaTempStr[0], (int)6, "0x%02X ", (iChipAddr<<1));
                        pStringDisplay = CMD_AddString(pStringDisplay, &s_i2cTaskCaTempStr[0], 6, 0);
                    		iCount++;
                    	}
	                    VK_TASK_Sleep(20); // To prevent "Bus Error"
	                    if ( (iChipAddr % 10 == 0) || (iChipAddr == 0x7F ))
	                    {
	                        if ( CMD_GetProgressbarUI() != NULL )
	                    	{
	                    		if (iDevId==0)
	                    			(int *)(CMD_GetProgressbarUI())("Port_0", (100*iChipAddr)/0x7f);
	                    		else if (iDevId==1)
	                    			(int *)(CMD_GetProgressbarUI())("Port_1", (100*iChipAddr)/0x7f);
	                    		else if (iDevId==2)
	                    			(int *)(CMD_GetProgressbarUI())("Port_2", (100*iChipAddr)/0x7f);
	                    		else if (iDevId==3)
	                    			(int *)(CMD_GetProgressbarUI())("Port_3", (100*iChipAddr)/0x7f);
	                    	}
	                    }
                }
                DI_UART_Print("==========================\n");
            }
    		/*VK_sprintf(s_i2cTaskCaTempStr, "\n");
    		VK_strcat(s_i2cTaskCaStr, s_i2cTaskCaTempStr);*/
            if ( CMD_GetInfoUI() != NULL )
            {
                (int *)(CMD_GetInfoUI())(pStringDisplay);
            }
            DD_MEM_Free(pStringDisplay);
            pStringDisplay = NULL;
        }
        else
        {
            VK_TASK_Sleep(1000);
        }
        VK_TASK_Sleep(1000);
    }
}

void P_CMD_HwTest_CreateI2C_TestTask(void)
{
	VK_TASK_Create(
        CMD_HwTest_Parser_I2c_TASK,
        USER_PRIORITY7,
        BASIC_TASK_STACK_SIZE,
        "I2CTTASK",
        (void *)0,
        (unsigned long *)&suiI2CTestTaskID,
        FALSE);
	VK_TASK_Start(suiI2CTestTaskID);
}

int CMD_HwTest_Parser_I2C_scan(void *szArgs)
{
	HUINT32		iDevId;
	HUINT16		iChipAddr;
	HUINT8		ucBuff;
#if 0	
	HUINT8		iSubAddr;
#endif
	DRV_Error	drverror;
	HUINT8		aucDevId[128];
	HUINT16		aucAddr[128];
	HUINT32		bufWrite, bufRead;

	BSTD_UNUSED(szArgs);

	bufWrite = 0;
	for (iDevId=0 ; iDevId<NEXUS_NUM_I2C_CHANNELS ; iDevId++)
	{
		VK_TASK_Sleep(50);
		DI_UART_Print("=======I2C channel (%d)/4=======\n", iDevId);
		for (iChipAddr=0 ; iChipAddr<=0x7f ; iChipAddr++)
		{
			drverror = DRV_I2c_ReadA8_NoRetry(iDevId, iChipAddr, 0, &ucBuff, 1);
			if (drverror == DRV_OK)
			{
				DI_UART_Print("I2C port OK : 0x%02X\n", (iChipAddr<<1));
				aucDevId[bufWrite] = iDevId;
				aucAddr[bufWrite] = iChipAddr<<1;
				bufWrite++;
#if 0
				for (iSubAddr=0 ; iSubAddr<=0xff ; iSubAddr++)
				{
					drverror = DRV_I2c_ReadA8_NoRetry(iDevId, iChipAddr, iSubAddr, &ucBuff, 1);
					if (drverror!=DRV_OK)
					{
						DI_UART_Print("     FAIL             SubAddr (%02X)\n", iSubAddr);
					}
					VK_TASK_Sleep(30); // To prevent "Bus Error"
				}
#endif
			}
			VK_TASK_Sleep(30); // To prevent "Bus Error"
		}
		DI_UART_Print("=======End ===================\n");
	}
	for (bufRead=0 ; bufRead<bufWrite ; bufRead++)
	{
		DI_UART_Print("DEVICE ID (%d) : I2C port OK : 0x%04X\n", aucDevId[bufRead], aucAddr[bufRead]);
	}

	return 0;
}

int CMD_HwTest_Parser_I2c(void *szArgs)
{
	GET_ARGS;
	HUINT32  ulI2cChannel;
	DRV_I2C_CLOCK_E eClock;

	UNUSED(szParam2);

	if (CMD_IS("info"))
	{
		suiI2CTestKicking = 1;
		iResult = CMD_OK;
	}
	else if(CMD_IS("setclock"))
	{
#if defined(CONFIG_SCART_6417)
		if PARAM_IS("SCART")
			ulI2cChannel = I2C_CHANNEL_SCART;
		else
#endif
		if PARAM_IS("CHANNEL")
			ulI2cChannel = I2C_CHANNEL_CHANNEL;
#if defined(CONFIG_EEPROM)
		else if PARAM_IS("EEPROM")
			ulI2cChannel = I2C_CHANNEL_EEPROM;
#endif
		else if PARAM_IS("HDMI")
			ulI2cChannel = I2C_CHANNEL_HDMI;

		if PARAM1_IS("47")
			eClock = DRV_I2C_CLOCK_47K;
		else if PARAM1_IS("50")
			eClock = DRV_I2C_CLOCK_50K;
		else if PARAM1_IS("93")
			eClock = DRV_I2C_CLOCK_93K;
		else if PARAM1_IS("100")
			eClock = DRV_I2C_CLOCK_100K;
		else if PARAM1_IS("187")
			eClock = DRV_I2C_CLOCK_187K;
		else if PARAM1_IS("200")
			eClock = DRV_I2C_CLOCK_200K;
		else if PARAM1_IS("375")
			eClock = DRV_I2C_CLOCK_375K;
		else if PARAM1_IS("400")
			eClock = DRV_I2C_CLOCK_400K;

		iResult = DRV_I2c_SetClock(ulI2cChannel,eClock);
	}
    else if (CMD_IS("scan"))
	{
		CMD_HwTest_Parser_I2C_scan(szArgs);
        iResult = CMD_OK;
	}

    return iResult;
}

#if defined(CONFIG_CRYPTO)
#include "di_crypt.h"

extern void DRV_CRYPT_Test(DI_CRYPT_CryptMode cryptMode, DI_CRYPT_CryptKeyType cryptKey);

int CMD_HwTest_Parser_Descrypt(void *szArgs)
{
	GET_ARGS;

	UNUSED(szParam);
	UNUSED(szParam1);
	UNUSED(szParam2);

	/* TDES and System key */
	DRV_CRYPT_Test(DI_CRYPT_TDES, DI_CRYPT_DEFAULT_SYSTEM_KEY);

	/* TDES and User key */
	DRV_CRYPT_Test(DI_CRYPT_TDES, DI_CRYPT_USER_KEY);

	/* AES and System key */
	DRV_CRYPT_Test(DI_CRYPT_AES, DI_CRYPT_DEFAULT_SYSTEM_KEY);

	/* AES and User key */
	DRV_CRYPT_Test(DI_CRYPT_AES, DI_CRYPT_USER_KEY);

	/* AES and CBC-MAC */
	DRV_CRYPT_Test(DI_CRYPT_AES_CBC_MAC, DI_CRYPT_DEFAULT_SYSTEM_KEY);

    return CMD_OK;
}
#endif

int CMD_HwTest_Parser_Gpio(void *szArgs)
{
	UNUSED(szArgs);
    return 0;
}

int CMD_HwTest_Parser_Reg(void *szArgs)
{
	UNUSED(szArgs);
    return 0;
}


int CMD_HwTest_Parser_Uart(void *szArgs)
{
	GET_ARGS;
	int					fd;
	char				bufp[4*1024] = {0,};
	FILE				*fp = NULL;
	char				fname[256] = {0,};
	int					size;
	int					totalsize;

	struct timeval		tTimeval;
	struct tm			*tTm;

	if (CMD_IS("log")) {
		if (PARAM_IS("dump")) {
			//get system time
			gettimeofday(&tTimeval, 0);
			tTm = localtime(&tTimeval.tv_sec);

			//make log-file name with time
			VK_snprintf(fname, sizeof(fname), "/media/LOG-%04d%02d%02d-%02d%02d%02d.log",
											tTm->tm_year+1900, tTm->tm_mon+1, tTm->tm_mday,
											tTm->tm_hour, tTm->tm_min, tTm->tm_sec);
			fp = fopen(fname, "w+");
			if (fp == NULL) {
				VK_printf("%s: dump error! can't open file\n", __FUNCTION__);
				return CMD_ERR;
			}
			totalsize = 0;
			while(1) {
				fd = open("/proc/hmxmsg", O_RDONLY, 0777);
				if(fd < 0) {
					VK_printf("%s: hmxmsg file open error\n", __FUNCTION__);
					close(fp);
					return CMD_ERR;
				}
				size = read(fd, bufp, 4095);
				if(size==0) break;
				fwrite(bufp, size, 1, fp);
				totalsize+=size;
				if(totalsize > 128*1024) break;
				close(fd);
			}
			VK_fflush(fp);
			fclose(fp);

			VK_printf("%s: last read-size(%d), total-size(%d)\n", __FUNCTION__, size, totalsize);
		}
		else if (PARAM_IS("info")) {
			fd = open("/proc/hmxmsg", O_WRONLY, 0777);
			if(fd < 0) {
				VK_printf("%s: hmxmsg file open error\n", __FUNCTION__);
				close(fp);
				return CMD_ERR;
			}
			write(fd, "info", 4);
			close(fd);
		}
		else if (PARAM_IS("clear")) {
			fd = open("/proc/hmxmsg", O_WRONLY, 0777);
			if(fd < 0){
				VK_printf("%s: hmxmsg file open error\n", __FUNCTION__);
				close(fp);
				return CMD_ERR;
			}
			write(fd, "clear", 5);
			close(fd);
		}
	}

	return CMD_OK;
}

#undef HWTEST_CMD
#undef HWTEST_PARAM
#undef HWTEST_PARAM1
#undef HWTEST_PARAM2
#undef GET_ARGS
#undef PARAM_IS
#undef PARAM1_IS
#undef PARAM2_IS
#undef READABLE_IN_HEX
#undef READABLE_IN_DEC

