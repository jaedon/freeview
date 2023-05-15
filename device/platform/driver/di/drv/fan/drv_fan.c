/********************************************************************
 * $Workfile:   drv_fan.c  $
 * $Modtime:   2 July 2008 10:00:00  $
 *
 * Author     Hyun Seok Choi & Kyu Young Cho
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/********************************************************************
 Header Files
 ********************************************************************/
/* include common utils first! */
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include "nexus_platform.h"
#if defined(CONFIG_DI_SHARED_LIB)
//NULL
#else
#include "linden_trace.h"
#include "nexus_pwm_init.h"
#include "nexus_pwm.h"
#endif
#include "di_config.h"
#include "di_uart.h"
#include "di_system.h"
#if defined(CONFIG_DEMOD_TEMPERATURE)
#include "di_channel.h"
#endif
#include "drv_smart.h"
#include "drv_gpio.h"
#include "drv_micom.h"
#include "vkernel.h"
#include "taskdef.h"

#include "drv_fan.h"

/*******************************************************************/
/****************************** define *****************************/
/*******************************************************************/
#if defined(CONFIG_DI_SHARED_LIB)
#ifdef FAN_DEBUG
#define PrintInfo(...)				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintDebug(fmt, ...) 		VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintError(fmt, ...)    	VK_DBG_Print (fmt, ## __VA_ARGS__)
#define PrintEnter(...)				VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit(...)				VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintInfo(...)
#define PrintDebug(...)
#define PrintError(...)				VK_DBG_Print (__VA_ARGS__)
#define PrintEnter(...)
#define PrintExit(...)
#endif
#else
#define DEBUG_MODULE	TRACE_MODULE_DI_FAN
#define PrintInfo		DI_UART_Print

#ifndef DEBUG_MODULE
#undef PrintDebug
#undef PrintError
#undef PrintExit
#undef PrintEnter

#ifdef DRV_FAN_DEBUG
#define PrintDebug(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#define PrintError(fmt, ...)	DI_UART_Print (fmt , ## __VA_ARGS__)
#else
#define PrintDebug(fmt, ...)
#define PrintError(fmt, ...)	VK_printf (fmt , ## __VA_ARGS__)
#endif

#ifdef DRV_FAN_DEBUG
#define PrintEnter		VK_printf("(+)%s\n", __FUNCTION__)
#define PrintExit		VK_printf("(-)%s\n", __FUNCTION__)
#else
#define PrintEnter()
#define PrintExit()
#endif
#endif
#endif

#define AssertDRV(arg1, arg2) if(arg1 != DRV_OK) {VK_printf(\
					"ERROR code : 0x%x, At %s() function, FILE: %s, LINE: %d\n",arg1, __func__, __FILE__,__LINE__); return arg2;}

#define DRV_FAN_TASK_STACK_SIZE		BASIC_TASK_STACK_SIZE
#define DRV_FAN_TASK_PRIORITY		USER_PRIORITY5

#if defined(CONFIG_DI_SHARED_LIB)
//NULL
#else
#if defined(CONFIG_FAN_PWM_NUM)
#define FAN_PWM_NUM					CONFIG_FAN_PWM_NUM
#else
#if defined(CONFIG_FAN_PWM2_TEMP)
#if (NEXUS_VERSION < 750)
#define	FAN_PWM_NUM					NEXUS_PwmChannelNumber_e2
#else
#define	FAN_PWM_NUM					2
#endif
#else
#if (NEXUS_VERSION < 750)
#define	FAN_PWM_NUM					NEXUS_PwmChannelNumber_e0
#else
#define	FAN_PWM_NUM					0
#endif
#endif//CONFIG_FAN_PWM2_TEMP
#endif// CONFIG_FAN_PWM_NUM
#endif//CONFIG_DI_SHARED_LIB

#define PWM_PERIOD_INTERVAL_FREQ_50KHZ		(0xfe)
#define PWM_CONTROL_WORD_FREQ_50KHZ			(0x7900)
#define PWM_ON_INTERVAL_MAX					(214)	// 255 까지 가능하나, iCord-HD+ B/D 에서 214 일 때, 11V DC 출력하므로, 기준점으로 삼는다.

#define ONE_SECOND							(1000)

#if defined(CONFIG_NEW_FAN_SCENARIO)
extern HUINT8 s_FanExecuteTemp[7];
extern HUINT32 s_FanFreq;
#endif

#if defined(CONFIG_FAN_SCENARIO_30)
extern HUINT8	s_FanExecuteTemp[7];
extern HUINT8	s_FanExecuteCPUTemp[7];
#if defined(CONFIG_DEMOD_TEMPERATURE)
extern HUINT8	s_FanExecuteDEMODTemp[7];
#endif
extern HUINT32	s_FanFreq;
#endif

#if defined(CONFIG_JAPAN_SPEC)
extern HUINT8	s_FanExecuteTemp[7];
extern HUINT8	s_FanExecuteCPUTemp[7];
extern HUINT8	s_FanExecuteCMTemp[7];
extern HUINT32	s_FanFreq;
#endif

#if defined(CONFIG_FAN_FEEDBACK_BY_LEVEL)
extern HUINT32	s_FanLockLevel;
static GPIO_STATE_t s_FanLockGpioLevel;
#endif

//#define FAN_TASK_TEST 1
#if defined(FAN_TASK_TEST)
#define	THRESHOLD_TEMP						(10)	// test threshold temp is 28
#else
#if defined(CONFIG_NEW_FAN_SCENARIO)
#define THRESHOLD_TEMP						s_FanExecuteTemp[0]
#elif defined(CONFIG_FAN_SCENARIO_30)
#define THRESHOLD_TEMP						s_FanExecuteTemp[0]
#define	THRESHOLD_CPU_TEMP					s_FanExecuteCPUTemp[0]
#elif defined(CONFIG_JAPAN_SPEC)
#define THRESHOLD_TEMP						s_FanExecuteTemp[0]
#define THRESHOLD_CPU_TEMP					s_FanExecuteCPUTemp[0]
#define THRESHOLD_CM_TEMP					s_FanExecuteCMTemp[0]
#else
#define	THRESHOLD_TEMP						(50)	// normal threshold temp is 51

#endif
#endif

#if defined(CONFIG_NEW_FAN_SCENARIO)
#define INITIAL_THRESHOLD					s_FanExecuteTemp[0]		/* 세트 부팅 후 FAN 이 처음으로 동작하게 되는 온도. */
#define LOW_THRESHOLD						s_FanExecuteTemp[1]		/* FAN OFF 되는 온도. */
#define LOW_MIDDLE_THRESHOLD				s_FanExecuteTemp[2]
#define MIDDLE_THRESHOLD					s_FanExecuteTemp[3]		/* 동작 중 FAN OFF 되었다가 다시 FAN ON 되는 온도. */
#define MIDDLE_HIGH_THRESHOLD				s_FanExecuteTemp[4]
#define HIGH_THRESHOLD						s_FanExecuteTemp[5]
#define SHUTDOWN_THRESHOLD					s_FanExecuteTemp[6]

#elif defined(CONFIG_FAN_SCENARIO_30)
#define INITIAL_THRESHOLD					s_FanExecuteTemp[0]		/* 세트 부팅 후 FAN 이 처음으로 동작하게 되는 온도. */
#define LOW_THRESHOLD						s_FanExecuteTemp[1]		/* FAN OFF 되는 온도. */
#define LOW_MIDDLE_THRESHOLD				s_FanExecuteTemp[2]
#define MIDDLE_THRESHOLD					s_FanExecuteTemp[3]		/* 동작 중 FAN OFF 되었다가 다시 FAN ON 되는 온도. */
#define MIDDLE_HIGH_THRESHOLD				s_FanExecuteTemp[4]
#define HIGH_THRESHOLD						s_FanExecuteTemp[5]
#define SHUTDOWN_THRESHOLD					s_FanExecuteTemp[6]

#define INITIAL_CPU_THRESHOLD				s_FanExecuteCPUTemp[0]		// 105
#define	LOW_CPU_THRESHOLD					s_FanExecuteCPUTemp[1]		// 105
#define	LOW_MIDDLE_CPU_THRESHOLD			s_FanExecuteCPUTemp[2]		// 105
#define	MIDDLE_CPU_THRESHOLD				s_FanExecuteCPUTemp[3]		// 105
#define	MIDDLE_HIGH_CPU_THRESHOLD			s_FanExecuteCPUTemp[4]		// 105
#define	HIGH_CPU_THRESHOLD					s_FanExecuteCPUTemp[5]		// 105
#define	SHUTDOWN_CPU_THRESHOLD				s_FanExecuteCPUTemp[6]		// 105

#if defined(CONFIG_DEMOD_TEMPERATURE)
#define INITIAL_DEMOD_THRESHOLD				s_FanExecuteDEMODTemp[0]		
#define	LOW_DEMOD_THRESHOLD				s_FanExecuteDEMODTemp[1]		
#define	LOW_MIDDLE_DEMOD_THRESHOLD			s_FanExecuteDEMODTemp[2]		
#define	MIDDLE_DEMOD_THRESHOLD				s_FanExecuteDEMODTemp[3]		
#define	MIDDLE_HIGH_DEMOD_THRESHOLD			s_FanExecuteDEMODTemp[4]		
#define	HIGH_DEMOD_THRESHOLD				s_FanExecuteDEMODTemp[5]		
#define	SHUTDOWN_DEMOD_THRESHOLD			s_FanExecuteDEMODTemp[6]		
#else
#define INITIAL_DEMOD_THRESHOLD				100
#define	LOW_DEMOD_THRESHOLD				100
#define	LOW_MIDDLE_DEMOD_THRESHOLD			100
#define	MIDDLE_DEMOD_THRESHOLD				100
#define	MIDDLE_HIGH_DEMOD_THRESHOLD			100
#define	HIGH_DEMOD_THRESHOLD				100
#define	SHUTDOWN_DEMOD_THRESHOLD			100
#endif
#elif defined(CONFIG_JAPAN_SPEC)
#define INITIAL_THRESHOLD					s_FanExecuteTemp[0]		/* 세트 부팅 후 FAN 이 처음으로 동작하게 되는 온도. */
#define LOW_THRESHOLD						s_FanExecuteTemp[1]		/* FAN OFF 되는 온도. */
#define LOW_MIDDLE_THRESHOLD				s_FanExecuteTemp[2]
#define MIDDLE_THRESHOLD					s_FanExecuteTemp[3]		/* 동작 중 FAN OFF 되었다가 다시 FAN ON 되는 온도. */
#define MIDDLE_HIGH_THRESHOLD				s_FanExecuteTemp[4]
#define HIGH_THRESHOLD						s_FanExecuteTemp[5]
#define SHUTDOWN_THRESHOLD					s_FanExecuteTemp[6]

#define INITIAL_CPU_THRESHOLD				s_FanExecuteCPUTemp[0]		// 105
#define LOW_CPU_THRESHOLD					s_FanExecuteCPUTemp[1]		// 105
#define LOW_MIDDLE_CPU_THRESHOLD			s_FanExecuteCPUTemp[2]		// 105
#define MIDDLE_CPU_THRESHOLD				s_FanExecuteCPUTemp[3]		// 105
#define MIDDLE_HIGH_CPU_THRESHOLD			s_FanExecuteCPUTemp[4]		// 105
#define HIGH_CPU_THRESHOLD					s_FanExecuteCPUTemp[5]		// 105
#define SHUTDOWN_CPU_THRESHOLD				s_FanExecuteCPUTemp[6]		// 105

#define INITIAL_CM_THRESHOLD				s_FanExecuteCMTemp[0]		// 105
#define LOW_CM_THRESHOLD					s_FanExecuteCMTemp[1]		// 105
#define LOW_MIDDLE_CM_THRESHOLD				s_FanExecuteCMTemp[2]		// 105
#define MIDDLE_CM_THRESHOLD					s_FanExecuteCMTemp[3]		// 105
#define MIDDLE_HIGH_CM_THRESHOLD			s_FanExecuteCMTemp[4]		// 105
#define HIGH_CM_THRESHOLD					s_FanExecuteCMTemp[5]		// 105
#define SHUTDOWN_CM_THRESHOLD				s_FanExecuteCMTemp[6]		// 105

#define FAN_LOCK_THRESHOLD					5						 /* low speed 일때 대략 60 정도의 speed 로 읽힘 */

#else
#define INITIAL_THRESHOLD					(THRESHOLD_TEMP+5)		// 55
#define	LOW_THRESHOLD						(THRESHOLD_TEMP)		// 50
#define	LOW_MIDDLE_THRESHOLD				(THRESHOLD_TEMP+3)		// 53
#define	MIDDLE_THRESHOLD					(THRESHOLD_TEMP+5)		// 55
#define	MIDDLE_HIGH_THRESHOLD				(THRESHOLD_TEMP+8)		// 58
#define	HIGH_THRESHOLD						(THRESHOLD_TEMP+10)		// 60
#define	SHUTDOWN_THRESHOLD					(THRESHOLD_TEMP+18)		// 68
#endif

#define FAN_SLEEP_TIMEOUT					(ONE_SECOND)		// sleep_time is 1 second at least
#define HWTEST_SLEEP_TIMEOUT				(30*ONE_SECOND)		// CONFIG_HWTEST Sleep is 30 sec

#define FAN_10_SEC							(10)
#define FAN_1_MIN							(1*60)
#define FAN_2_MIN							(2*60)
#define FAN_5_MIN							(5*60)

#define	FAN_FEEDBACK_INTERVAL				(1000)		// 1 sec

#if defined(CONFIG_DI_SHARED_LIB)
#define MAX_INTR_GPIO_LENGTH 20

/* BELOW IS IN DI_SO ONLY */
#define IOCTLDRIVER_MAGIC 'H'
#define IOCTLPWM_GET_CTRL_WORD				_IOR(IOCTLDRIVER_MAGIC, 3, ioctl_driver_info)
#define IOCTLPWM_SET_CTRL_WORD				_IOW(IOCTLDRIVER_MAGIC, 4, ioctl_driver_info)
#define IOCTLPWM_GET_FREQ_MODE				_IOR(IOCTLDRIVER_MAGIC, 5, ioctl_driver_info)
#define IOCTLPWM_SET_FREQ_MODE				_IOW(IOCTLDRIVER_MAGIC, 6, ioctl_driver_info)
#define IOCTLPWM_GET_ONINTERVAL				_IOR(IOCTLDRIVER_MAGIC, 7, ioctl_driver_info)
#define IOCTLPWM_SET_ONINTERVAL				_IOW(IOCTLDRIVER_MAGIC, 8, ioctl_driver_info)
#define IOCTLPWM_GET_PERIOD_INTERVAL		_IOR(IOCTLDRIVER_MAGIC, 9, ioctl_driver_info)
#define IOCTLPWM_SET_PERIOD_INTERVAL		_IOW(IOCTLDRIVER_MAGIC, 10, ioctl_driver_info)
#define IOCTLPWM_START						_IOW(IOCTLDRIVER_MAGIC, 11, ioctl_driver_info)
#define IOCTLPWM_STOP						_IOW(IOCTLDRIVER_MAGIC, 12, ioctl_driver_info)
#define IOCTLPWM_OPEN						_IOW(IOCTLDRIVER_MAGIC, 13, ioctl_driver_info)
#endif

#if defined(CONFIG_NEW_FAN_SCENARIO) || defined(CONFIG_FAN_SCENARIO_30) || defined(CONFIG_JAPAN_SPEC)
extern HUINT32 DRV_CFG_GetFanInterval(HUINT32 ulVolt);
#endif
#if defined(CONFIG_DEMOD_TEMPERATURE)
extern int DRV_CH_GetDeviceStauts(int nChannelId, DI_CH_DeviceStatus * pstDeviceStatus);

#endif

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/

#if defined(CONFIG_DI_SHARED_LIB)
/* BELOW IS IN DI_SO ONLY */
typedef struct
{
	unsigned int addr;
	unsigned int value;
}__attribute__((packed)) ioctl_driver_info;

typedef enum PwmFreqModeType
{
    PwmFreqModeType_eVariable = 0,
    PwmFreqModeType_eConstant
} PwmFreqModeType;
#endif

/*******************************************************************/
/************************ static variables *************************/
/*******************************************************************/
static DRV_FAN_CALLBACK s_pfFanCallback[DRV_FAN_EVENT_END];

/*********************************************************************
 * Global/Static Variable Definition
 *********************************************************************/
#if defined(CONFIG_DI_SHARED_LIB)
//NULL
#else
static NEXUS_PwmChannelHandle	s_PwmChannelHandle = NULL;
#endif

static unsigned int s_ulFanSecCnt = 0;
static unsigned long s_ulFanTaskID = 0;
unsigned long	s_ulFanFeedbackTimerId = 0;

#if !defined(CONFIG_FAN_FEEDBACK_BY_LEVEL)
static unsigned int ulFanSpeedCount = 0;
#endif

#if defined(CONFIG_DI_SHARED_LIB)
static int g_fd = 0;
#endif

HBOOL	g_bFanDispInfo = FALSE;

static DRV_FAN_SETTING s_stFanSetting;

static void P_FAN_Event_Task(void *p);
#if defined(CONFIG_FAN_FEEDBACK_BY_LEVEL)
static void P_FAN_GetFeedBackByLevel(void);
#endif

/*****************************************************************************
* function: P_FAN_CalculateFeedbackSpeed
* description:
* input:	tid, arg
* return:	void
******************************************************************************/
#if defined(CONFIG_DI_SHARED_LIB)
static void P_FAN_CalculateFeedbackSpeed(void)
{
	const char intr_info[] =  "/proc/bcmdriver/interruptsL2";
	int fd;
	unsigned int num_intr = 0;

	fd = open(intr_info, O_RDWR);
	if(fd != -1){
		char buf[MAX_INTR_GPIO_LENGTH];
		char *pStart;
		ssize_t read_cnt=0;

		VK_memset(buf, 0x00, MAX_INTR_GPIO_LENGTH);

		read_cnt = read(fd, (void *)buf, sizeof(buf));
		if(read_cnt > 0 && read_cnt < MAX_INTR_GPIO_LENGTH ){
			buf[read_cnt] = '\n';
			pStart = strrchr(buf, ' ');
			if(pStart){
				num_intr = VK_atoi(pStart);
				ulFanSpeedCount += num_intr;
			}
		}
		else{
			PrintError("L2 interrupts read error, read_cnt = %d\n", read_cnt);
		}
		num_intr = 0;
		write(fd, &num_intr, sizeof(int));
		close(fd);
	}
	s_stFanSetting.ulFanSpeed_Freq = ulFanSpeedCount;

	// Feedback count clear
	ulFanSpeedCount = 0;

}
#elif !defined(CONFIG_FAN_FEEDBACK_BY_LEVEL) && (defined(CONFIG_PRODUCT_IMAGE_HWTEST) || defined(CONFIG_PRODUCT_IMAGE_FACTORY))
static void P_FAN_CalculateFeedbackSpeed(unsigned long tid, void *arg)
{
	BSTD_UNUSED(arg);

	if(tid != s_ulFanFeedbackTimerId)
	{
		PrintError("Invalid TimerId : tid(0x%x), s_ulFanFeedbackTimerId(0x%x) \n", tid, s_ulFanFeedbackTimerId);
		return;
	}

	// ulFanSpeed_Freq variable is updated every 1 sec. --> ulFanSpeed_Freq is fan feedback Hz
	s_stFanSetting.ulFanSpeed_Freq = (ulFanSpeedCount*1000)/FAN_FEEDBACK_INTERVAL;

	// Feedback count clear
	ulFanSpeedCount = 0;

}

/*****************************************************************************
* function: P_FAN_InterruptHandler
* description
* input:	void
* return:	void
******************************************************************************/
static void P_FAN_InterruptHandler(void *context, int param)
{
	BSTD_UNUSED(context);
	BSTD_UNUSED(param);

	ulFanSpeedCount++;

	return;
}

/*****************************************************************************
* function:
* description
* input:
* return:
******************************************************************************/
static void P_FAN_Interrupt_Init(void)
{

	/*********************************************************************/
	/* Platform Dependent한 부분 임. 각 Plaotform에 맞게 interrupt를 setting 해주어야 함 */
	/*********************************************************************/
	/* 1. Interrupt PIO Pin을 input mode로 동작하도록 초기화 한다.
	--> HDR-5000 에서는 DDI_HW_Init() 의 DDI_GPIO_Init() & DDI_GPIO_INTR_Open() 를 통해
	GPIO Interrupt 관련 사항은 초기화 된다. 단지 enable 만 안됨.
	*/

	/* 2. Interrupt Handler를 동록한다. */
	DRV_GPIO_INTR_RegisterEventNotify(GPIO_ID_FAN_SPEED, P_FAN_InterruptHandler, (void*)NULL, (int)NULL);

	/* 3. Interrupt PIO Pin을 enable 시키고, Interrupt를 enable 시킨다. */
	DRV_GPIO_INTR_Enable(GPIO_ID_FAN_SPEED);
}

/*****************************************************************************
* function: P_FAN_Interrupt_Uninit
* description:
* input:	void
* return:	void
******************************************************************************/
static void P_FAN_Interrupt_Uninit(void)
{
	DRV_GPIO_INTR_Disable(GPIO_ID_FAN_SPEED);
}
#endif

#if	defined(CONFIG_HDD_SMART)
static int P_FAN_GetSmartTemp(int port_num, int *temperature)
{
#define SMART_CMD			(0x031F)
#define SMART_BUF_SIZE 		(516)
#define TEMP_INDEX 			(204)
#define SD_DEV_MAX			(8)
	char path[256] = {0,};
	char link[256] = {0,};
	int i;
	int len;
	int fd = -1;
	unsigned char buf[SMART_BUF_SIZE];
	int rc = -1;

	/* default value */
	*temperature = 51;

	DONE_ERR((port_num < 1));	

	for(i=0; i<SD_DEV_MAX; i++)
	{
		VK_MEM_Memset(path, 0, sizeof(path));
		VK_MEM_Memset(link, 0, sizeof(link));

		VK_snprintf(path, sizeof(path)-1, "/sys/block/sd%c/device", 'a'+i);
		len = readlink(path, link, sizeof(link)-1);

		if (len > 0 && (unsigned long)len < sizeof(link))
		{
			link[len] = '\0';
			if (VK_strstr(link, "ahci"))
			{
				break;
			}
		}

	}
	DONE_ERR((i == SD_DEV_MAX));

	VK_snprintf(path, sizeof(path)-1, "/dev/sd%c", 'a'+i);
	fd = open(path, O_RDONLY | O_NONBLOCK);
	DONE_ERR((fd < 0));

	VK_MEM_Memset(buf, 0, sizeof(buf));
	buf[0] = 0xb0;
	buf[1] = 0xe0;
	buf[2] = 0xd5;
	buf[3] = 1;
	buf[4] = 0x4f;
	buf[5] = 0xc2;

	rc = ioctl(fd, SMART_CMD, &buf);
	DONE_ERR((rc < 0));

	*temperature = (buf[TEMP_INDEX] & 0x80) ? (int)(0xFFFFFF00 | buf[TEMP_INDEX]) : (int)buf[TEMP_INDEX];

	rc = 0;
done:	
	if (fd >= 0) close(fd);
	return rc;
}
#endif

#if defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
/*****************************************************************************
* function: P_FAN_SetCtrlByGpio
* description:
* input:	void
* return:	void
******************************************************************************/
static HINT32 P_FAN_SetCtrlByGpio(HUINT16 usOnInterval)
{
	DRV_Error eRet = DRV_OK;
	GPIO_STATE_t val;

	if(usOnInterval)
	{
		val = GPIO_HI;
	}
	else
	{
		val = GPIO_LOW;
	}

	eRet = DRV_GPIO_Write(GPIO_ID_FAN_CONTROL, val);
	if(eRet != DRV_OK)
	{
		PrintError("[%s][%d], DRV_GPIO_Write error(%d)\n", __FUNCTION__, __LINE__, eRet);
		return -1;
	}
	return 0;
}
#endif
#if defined(CONFIG_FAN_FEEDBACK_BY_LEVEL)
/*****************************************************************************
* function: P_FAN_GetFeedBackByGpio
* description:
* input:	void
* return:	void
******************************************************************************/
static void P_FAN_GetFeedBackByLevel(void)
{
	DRV_Error eRet = DRV_OK;
	GPIO_STATE_t val;

	eRet = DRV_GPIO_Read(GPIO_ID_FAN_SPEED, &val);
	if(eRet != DRV_OK)
	{
		PrintError("[%s][%d], DRV_GPIO_Read error(%d)\n", __FUNCTION__, __LINE__, eRet);
		s_stFanSetting.ulFanSpeed_Freq = 0;
		return;
	}

	PrintDebug("[%s][%d], DRV_GPIO_Read val(%d)\n", __FUNCTION__, __LINE__, val);
	if(val != s_FanLockGpioLevel)
	{
		PrintDebug("[%s][%d], FAN OK : gpio(%d), fanlockgpiolevel(%d)\n", __FUNCTION__, __LINE__, val, s_FanLockGpioLevel);
		s_stFanSetting.ulFanSpeed_Freq = 60;
	}
	else
	{
		/* FAN LOCK */
		PrintDebug("[%s][%d], FAN Lock : gpio(%d), fanlockgpiolevel(%d)\n", __FUNCTION__, __LINE__, val, s_FanLockGpioLevel);
		s_stFanSetting.ulFanSpeed_Freq = 0;
	}
	return;
}
#endif
/*****************************************************************************
* function: DRV_FAN_Init
* description
* input:	void
* return:	DRV_OK
* 기타 참고자료 및 파일
******************************************************************************/
#if defined(CONFIG_DI_SHARED_LIB)
DRV_Error DRV_FAN_Init(void)
{
	DRV_Error errCode = DRV_OK;
	int i=0,rc = 0;
	ioctl_driver_info ctrl_info;

	PrintEnter();

	s_ulFanSecCnt = FAN_1_MIN;

	VK_memset(&s_stFanSetting, 0x00, sizeof(DRV_FAN_SETTING));

	// System Callback function is initialized by NULL
	for(i=0; i<DRV_FAN_EVENT_END; i++)
		s_pfFanCallback[i] = NULL;

	g_fd = open("/dev/nexus_pwm", O_RDWR|O_NDELAY);
	if(g_fd < 0){
		PrintError("[%d][%s], nexus_pwm open error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	ctrl_info.addr = 0;
	ctrl_info.value = 0;
	rc = ioctl(g_fd, IOCTLPWM_OPEN, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_OPEN){
		PrintError("[%s][%d], IOCTLPWM_OPEN error %d, %s\n", __FUNCTION__, __LINE__, ctrl_info.value, strerror(errno));
		return DRV_ERR;
	}

	ctrl_info.addr = 0;
	ctrl_info.value = PwmFreqModeType_eConstant;
	rc = ioctl(g_fd, IOCTLPWM_SET_FREQ_MODE, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_SET_FREQ_MODE){
		PrintError("[%s][%d], IOCTLPWM_SET_FREQ_MODE error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	ctrl_info.addr = 0;
#if defined(CONFIG_NEW_FAN_SCENARIO) || defined(CONFIG_FAN_SCENARIO_30) || defined(CONFIG_JAPAN_SPEC)
	ctrl_info.value = s_FanFreq;
#else
	ctrl_info.value = PWM_CONTROL_WORD_FREQ_50KHZ;
#endif
	rc = ioctl(g_fd, IOCTLPWM_SET_CTRL_WORD, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_SET_CTRL_WORD){
		PrintError("[%s][%d], IOCTLPWM_SET_CTRL_WORD error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}
#if defined(CONFIG_NEW_FAN_SCENARIO) || defined(CONFIG_FAN_SCENARIO_30) || defined(CONFIG_JAPAN_SPEC)
	s_stFanSetting.usPWM_cWord = s_FanFreq;
#else
	s_stFanSetting.usPWM_cWord = PWM_CONTROL_WORD_FREQ_50KHZ;
#endif
	ctrl_info.addr = 0;
	ctrl_info.value = PWM_PERIOD_INTERVAL_FREQ_50KHZ;
	rc = ioctl(g_fd, IOCTLPWM_SET_PERIOD_INTERVAL, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_SET_PERIOD_INTERVAL){
		PrintError("[%s][%d], IOCTLPWM_SET_PERIOD_INTERVAL error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}
	s_stFanSetting.usPWM_PeriodInterval = PWM_PERIOD_INTERVAL_FREQ_50KHZ;

	ctrl_info.addr = 0;
	ctrl_info.value = 0;
	rc = ioctl(g_fd, IOCTLPWM_START, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_START){
		PrintError("[%s][%d], IOCTLPWM_START error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	errCode = DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);	// first Ctrl Status is "STOP"
	if(errCode != DRV_OK)
	{
		PrintError("DRV_FAN_SetCtrl(): errCode(0x%x) \n", errCode);
		return DRV_ERR;
	}

	rc = VK_TASK_Create(P_FAN_Event_Task, DRV_FAN_TASK_PRIORITY, DRV_FAN_TASK_STACK_SIZE, "P_FAN_Event_Task", (void *)0, &s_ulFanTaskID, FALSE);
	rc |= VK_TASK_Start(s_ulFanTaskID);

	if(rc != VK_OK)
	{
		PrintError("Error(0x%x) in VK_TASK_Create()\n\r", rc);
		return DRV_ERR;
	}

	rc = VK_TIMER_EventEvery(FAN_FEEDBACK_INTERVAL,
							P_FAN_CalculateFeedbackSpeed,
							NULL,
							0,
							&s_ulFanFeedbackTimerId);
	if(rc != VK_OK)
	{
		PrintError("Error(0x%x) in VK_TIMER_EventEvery()\n\r", rc);
		return DRV_ERR;
	}

	PrintExit();

	return errCode;
}
#else
DRV_Error DRV_FAN_Init(void)
{
	DRV_Error errCode = DRV_OK;
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	NEXUS_Error			rc = NEXUS_SUCCESS;
	NEXUS_PwmChannelSettings	stPwmChnDefSettings;
#endif
#if !defined(CONFIG_FAN_FEEDBACK_BY_LEVEL)
	int		err = VK_OK;
#endif
	int i=0;
	int vkRet;

	PrintEnter();

	s_ulFanSecCnt = FAN_1_MIN;

	VK_memset(&s_stFanSetting, 0x00, sizeof(DRV_FAN_SETTING));

	// System Callback function is initialized by NULL
	for(i=0; i<DRV_FAN_EVENT_END; i++)
		s_pfFanCallback[i] = NULL;

#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	/* PWM Init & Open */
#if (NEXUS_VERSION < 750)
	NEXUS_Pwm_GetDefaultChannelSettings(FAN_PWM_NUM, &stPwmChnDefSettings);
#else
	NEXUS_Pwm_GetDefaultChannelSettings(&stPwmChnDefSettings);
#endif

	stPwmChnDefSettings.eFreqMode = NEXUS_PwmFreqModeType_eConstant;

#if (NEXUS_VERSION < 750)
	s_PwmChannelHandle = NEXUS_Pwm_OpenChannel(&stPwmChnDefSettings);
#else
	s_PwmChannelHandle = NEXUS_Pwm_OpenChannel(FAN_PWM_NUM, &stPwmChnDefSettings);
#endif
	if(s_PwmChannelHandle == NULL)
	{
		PrintError("NEXUS_Pwm_OpenChannel(): Error Null handle!\n");
		return DRV_ERR;
	}

#if defined(CONFIG_NEW_FAN_SCENARIO) || defined(CONFIG_FAN_SCENARIO_30) || defined(CONFIG_JAPAN_SPEC)
	rc = NEXUS_Pwm_SetControlWord(s_PwmChannelHandle, s_FanFreq);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_SetControlWord(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
	s_stFanSetting.usPWM_cWord = s_FanFreq;
#else
	rc = NEXUS_Pwm_SetControlWord(s_PwmChannelHandle, PWM_CONTROL_WORD_FREQ_50KHZ);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_SetControlWord(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
	s_stFanSetting.usPWM_cWord = PWM_CONTROL_WORD_FREQ_50KHZ;
#endif

	rc = NEXUS_Pwm_SetPeriodInterval(s_PwmChannelHandle, PWM_PERIOD_INTERVAL_FREQ_50KHZ);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_SetPeriodInterval(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
	s_stFanSetting.usPWM_PeriodInterval = PWM_PERIOD_INTERVAL_FREQ_50KHZ;

	rc = NEXUS_Pwm_Start(s_PwmChannelHandle);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_Start(): eNexusError(0x%08x) \n", rc);
		return DRV_ERR;
	}
#endif

	errCode = DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);	// first Ctrl Status is "STOP"
	if(errCode != DRV_OK)
	{
		PrintError("DRV_FAN_SetCtrl(): errCode(0x%x) \n", errCode);
		return DRV_ERR;
	}

	vkRet = VK_TASK_Create(P_FAN_Event_Task, DRV_FAN_TASK_PRIORITY, DRV_FAN_TASK_STACK_SIZE, "P_FAN_Event_Task", (void *)0, &s_ulFanTaskID, FALSE);
	if ( vkRet != VK_OK )
	{
		PrintError("Error(0x%x) in VK_TASK_Create()\n\r", vkRet );
		return DRV_ERR;
	}

	VK_TASK_Start(s_ulFanTaskID);

#if defined(CONFIG_FAN_FEEDBACK_BY_LEVEL)
	if(s_FanLockLevel)
	{
		s_FanLockGpioLevel = GPIO_HI;
	}
	else
	{
		s_FanLockGpioLevel = GPIO_LOW;
	}
#elif defined(CONFIG_PRODUCT_IMAGE_HWTEST) || defined(CONFIG_PRODUCT_IMAGE_FACTORY)
	/* Interrupt init for calculating fan speed. */	   
	P_FAN_Interrupt_Init();

	err = VK_TIMER_EventEvery(FAN_FEEDBACK_INTERVAL,
							P_FAN_CalculateFeedbackSpeed,
							NULL,
							0,
							&s_ulFanFeedbackTimerId);
	if(err != VK_OK)
	{
		PrintError("Error(0x%x) in VK_TIMER_EventEvery()\n\r", err);
		return DRV_ERR;
	}
#endif
	PrintExit();

	return errCode;
}
#endif

/*****************************************************************************
* function: DRV_FAN_Uninit
* description
* input:	void
* return:	DRV_OK
* DRV_FAN_Init()을 참고. Resource를 할당한 순서의 역순으로 해제함.
******************************************************************************/
#if defined(CONFIG_DI_SHARED_LIB)
DRV_Error DRV_FAN_Uninit(void)
{

	VK_TIMER_Cancel(s_ulFanFeedbackTimerId);

	VK_TASK_Stop(s_ulFanTaskID);

	VK_TASK_Destroy(s_ulFanTaskID);

	return DRV_OK;
}
#else
DRV_Error DRV_FAN_Uninit(void)
{
	int nVkErr = VK_OK;
#if !defined(CONFIG_FAN_FEEDBACK_BY_LEVEL) && (defined(CONFIG_PRODUCT_IMAGE_HWTEST) || defined(CONFIG_PRODUCT_IMAGE_FACTORY))
	VK_TIMER_Cancel(s_ulFanFeedbackTimerId);

	P_FAN_Interrupt_Uninit();
#endif
	nVkErr = VK_TASK_Stop(s_ulFanTaskID);
	if (nVkErr != VK_OK)
	{
		PrintError("[%s] [%d] Error(0x%x) \n\r",__FUNCTION__,__LINE__, nVkErr);
	}
	VK_TASK_Destroy(s_ulFanTaskID);
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	NEXUS_Pwm_Stop(s_PwmChannelHandle);

	NEXUS_Pwm_CloseChannel(s_PwmChannelHandle);
#endif
	return DRV_OK;
}
#endif

/*****************************************************************************
* function: DRV_FAN_SetCtrl
* description
* input:	fanCtrl
* return:	DRV_OK
* 기타 참고자료 및 파일
******************************************************************************/
DRV_Error DRV_FAN_SetCtrl(DRV_FAN_CTRL fanCtrl)
{
	HUINT32		ulVolt;
	HUINT16	        usOnInterval;
#if defined(CONFIG_DI_SHARED_LIB)
	ioctl_driver_info ctrl_info;
        HINT32          rc;
#else
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	NEXUS_Error	rc = NEXUS_SUCCESS;
#else
 	HINT32          rc;
#endif
#endif

	PrintEnter();

	if (fanCtrl == DRV_FAN_CTRL_VOLTAGE)
	{
		ulVolt = s_stFanSetting.ulVoltValue;	// user setting voltage
	}
	else
	{
		ulVolt = (HUINT32)fanCtrl;
	}

#if defined(CONFIG_NEW_FAN_SCENARIO) || defined(CONFIG_FAN_SCENARIO_30) || defined(CONFIG_JAPAN_SPEC)
	usOnInterval = DRV_CFG_GetFanInterval(ulVolt);
#else
	usOnInterval = (uint16_t)(ulVolt*PWM_ON_INTERVAL_MAX/FAN_VOLTAGE_MAX + (ulVolt*PWM_ON_INTERVAL_MAX)%FAN_VOLTAGE_MAX);
#endif
	PrintDebug("[%s] fanCtrl(%d), ulVolt(%d), usOnInterval(0x%x)\n", __func__, fanCtrl, ulVolt, usOnInterval);

#if defined(CONFIG_DI_SHARED_LIB)
	ctrl_info.addr = 0;
	ctrl_info.value = usOnInterval;
	rc = ioctl(g_fd, IOCTLPWM_SET_ONINTERVAL, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_SET_ONINTERVAL){
		PrintError("[%s][%d], IOCTLPWM_SET_ONINTERVAL error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}
#else
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	rc = NEXUS_Pwm_SetOnInterval(s_PwmChannelHandle, usOnInterval);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_SetControlWord(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
#else
	rc = P_FAN_SetCtrlByGpio(usOnInterval);
	if(rc != 0)
	{
		PrintError("P_FAN_SetCtrlByGpio(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
#endif
#endif

	s_stFanSetting.ulVoltValue = ulVolt;
	s_stFanSetting.nFanCtrl = fanCtrl;
	s_stFanSetting.usPWM_OnInterval = usOnInterval;

	if (s_stFanSetting.ulVoltValue == 0)
	{
		s_stFanSetting.ulFanSpeed_Freq = 0;
		s_stFanSetting.ulFanSpeed_AdjVolt = 0;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_FAN_SetCtrlDetail(HUINT32 ulVoltValue)
{
	DRV_Error		errCode = DRV_OK;

	PrintEnter();

	if (s_stFanSetting.nFanStatus != DRV_FAN_STATUS_HWTEST)
	{
		PrintDebug("[%s] Status Change :: Status(%d) -> CONFIG_HWTEST for CtrlDetail\n", __func__, s_stFanSetting.nFanStatus);
		s_stFanSetting.nFanStatus = DRV_FAN_STATUS_HWTEST;
	}

	/*  Value is voltage MAX is 11V */
	s_stFanSetting.ulVoltValue = (ulVoltValue>FAN_VOLTAGE_MAX) ? FAN_VOLTAGE_MAX : ulVoltValue;

	errCode = DRV_FAN_SetCtrl(DRV_FAN_CTRL_VOLTAGE);	// first Ctrl Status is "STOP"
	if(errCode != DRV_OK)
	{
		PrintError("DRV_FAN_SetCtrl(): errCode(0x%x) \n", errCode);
		return DRV_ERR;
	}

	PrintExit();

	return errCode;
}

#if defined(CONFIG_DI_SHARED_LIB)
DRV_Error DRV_FAN_SetCtrlFactory(DRV_FAN_CTRL fanCtrl)
{
	int rc;
	HUINT32 	ulVolt;
	HUINT16 	usOnInterval;
	ioctl_driver_info ctrl_info;

	PrintEnter();

	if (fanCtrl == DRV_FAN_CTRL_VOLTAGE)
	{
		ulVolt = s_stFanSetting.ulVoltValue;	// user setting voltage
	}
	else
	{
		ulVolt = (HUINT32)fanCtrl;
	}

#if defined(CONFIG_NEW_FAN_SCENARIO) || defined(CONFIG_FAN_SCENARIO_30) || defined(CONFIG_JAPAN_SPEC)
	usOnInterval = DRV_CFG_GetFanInterval(ulVolt);
#else
	usOnInterval = (uint16_t)(ulVolt*PWM_ON_INTERVAL_MAX/FAN_VOLTAGE_MAX + (ulVolt*PWM_ON_INTERVAL_MAX)%FAN_VOLTAGE_MAX);
#endif

	if((ulVolt > 0)&&(usOnInterval != 0))
	{
		if (s_stFanSetting.nFanStatus != DRV_FAN_STATUS_HWTEST)
		{
			PrintDebug("[%s] Status Change :: Status(%d) -> CONFIG_HWTEST for CtrlFactory\n", __func__, s_stFanSetting.nFanStatus);
			s_stFanSetting.nFanStatus = DRV_FAN_STATUS_HWTEST;
		}

	}
	else if((ulVolt == 0)&&(usOnInterval == 0))
	{
		if (s_stFanSetting.nFanStatus == DRV_FAN_STATUS_HWTEST)
		{
			s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
		}
	}
	else
	{
		s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
	}

	PrintDebug("[%s] fanCtrl(%d), ulVolt(%d), usOnInterval(0x%x)(%d)\n", __func__, fanCtrl, ulVolt, usOnInterval,s_stFanSetting.nFanStatus);

	ctrl_info.addr = 0;
	ctrl_info.value = usOnInterval;
	rc = ioctl(g_fd, IOCTLPWM_SET_ONINTERVAL, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_SET_ONINTERVAL){
		PrintError("[%s][%d], IOCTLPWM_SET_ONINTERVAL error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

	s_stFanSetting.ulVoltValue = ulVolt;
	s_stFanSetting.nFanCtrl = fanCtrl;
	s_stFanSetting.usPWM_OnInterval = usOnInterval;

	if (s_stFanSetting.ulVoltValue == 0)
	{
		s_stFanSetting.ulFanSpeed_Freq = 0;
		s_stFanSetting.ulFanSpeed_AdjVolt = 0;
	}

	PrintExit();

	return DRV_OK;

}
#else
DRV_Error DRV_FAN_SetCtrlFactory(DRV_FAN_CTRL fanCtrl)
{
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	NEXUS_Error rc = NEXUS_SUCCESS;
#else
	HINT32 rc;
#endif
	HUINT32 	ulVolt;
	uint16_t	usOnInterval;

	PrintEnter();

	if (fanCtrl == DRV_FAN_CTRL_VOLTAGE)
	{
		ulVolt = s_stFanSetting.ulVoltValue;	// user setting voltage
	}
	else
	{
		ulVolt = (HUINT32)fanCtrl;
	}

#if defined(CONFIG_NEW_FAN_SCENARIO) || defined(CONFIG_FAN_SCENARIO_30) || defined(CONFIG_JAPAN_SPEC)
	usOnInterval = DRV_CFG_GetFanInterval(ulVolt);
#else
	usOnInterval = (uint16_t)(ulVolt*PWM_ON_INTERVAL_MAX/FAN_VOLTAGE_MAX + (ulVolt*PWM_ON_INTERVAL_MAX)%FAN_VOLTAGE_MAX);
#endif

	if((ulVolt > 0)&&(usOnInterval != 0))
	{
		if (s_stFanSetting.nFanStatus != DRV_FAN_STATUS_HWTEST)
		{
			PrintDebug("[%s] Status Change :: Status(%d) -> CONFIG_HWTEST for CtrlFactory\n", __func__, s_stFanSetting.nFanStatus);
			s_stFanSetting.nFanStatus = DRV_FAN_STATUS_HWTEST;
		}

	}
	else if((ulVolt == 0)&&(usOnInterval == 0))
	{
		if (s_stFanSetting.nFanStatus == DRV_FAN_STATUS_HWTEST)
		{
			s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
		}
	}
	else
	{
		s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
	}

	PrintDebug("[%s] fanCtrl(%d), ulVolt(%d), usOnInterval(0x%x)(%d)\n", __func__, fanCtrl, ulVolt, usOnInterval,s_stFanSetting.nFanStatus);
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	rc = NEXUS_Pwm_SetOnInterval(s_PwmChannelHandle, usOnInterval);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_SetControlWord(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
#else
	rc = P_FAN_SetCtrlByGpio(usOnInterval);
	if(rc != 0)
	{
		PrintError("P_FAN_SetCtrlByGpio(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
#endif

	s_stFanSetting.ulVoltValue = ulVolt;
	s_stFanSetting.nFanCtrl = fanCtrl;
	s_stFanSetting.usPWM_OnInterval = usOnInterval;

	if (s_stFanSetting.ulVoltValue == 0)
	{
		s_stFanSetting.ulFanSpeed_Freq = 0;
		s_stFanSetting.ulFanSpeed_AdjVolt = 0;
	}

	PrintExit();

	return DRV_OK;

}
#endif

DRV_Error DRV_FAN_SetControlWord(HUINT16 usControlWord)
{
#if defined(CONFIG_DI_SHARED_LIB)
	int rc;
	ioctl_driver_info ctrl_info;
#else
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	NEXUS_Error	rc = NEXUS_SUCCESS;
#endif
#endif

	PrintEnter();

#if defined(CONFIG_DI_SHARED_LIB)
	ctrl_info.addr = 0;
	ctrl_info.value = usControlWord;
	rc = ioctl(g_fd, IOCTLPWM_SET_CTRL_WORD, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_SET_CTRL_WORD){
		PrintError("[%s][%d], IOCTLPWM_SET_CTRL_WORD error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}
#else
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	rc = NEXUS_Pwm_SetControlWord(s_PwmChannelHandle, usControlWord);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_SetControlWord(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
#endif
#endif
	s_stFanSetting.usPWM_cWord = usControlWord;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_FAN_SetPeriodInterval(HUINT16 usPeriodInterval)
{
#if defined(CONFIG_DI_SHARED_LIB)
	int rc;
	ioctl_driver_info ctrl_info;
#else
	NEXUS_Error	rc = NEXUS_SUCCESS;
#endif

	PrintEnter();

#if defined(CONFIG_DI_SHARED_LIB)
	ctrl_info.addr = 0;
	ctrl_info.value = usPeriodInterval;
	rc = ioctl(g_fd, IOCTLPWM_SET_PERIOD_INTERVAL, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_SET_PERIOD_INTERVAL){
		PrintError("[%s][%d], IOCTLPWM_SET_PERIOD_INTERVAL error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}
#else
	rc = NEXUS_Pwm_SetPeriodInterval(s_PwmChannelHandle, usPeriodInterval);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_SetPeriodInterval(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
#endif
	s_stFanSetting.usPWM_PeriodInterval = usPeriodInterval;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_FAN_SetOnInterval(HUINT16 usOnInterval)
{
#if defined(CONFIG_DI_SHARED_LIB)
	int rc;
	ioctl_driver_info ctrl_info;
#else
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	NEXUS_Error	rc = NEXUS_SUCCESS;
#else
	HINT32 rc;
#endif
#endif

	PrintEnter();

#if defined(CONFIG_DI_SHARED_LIB)
	ctrl_info.addr = 0;
	ctrl_info.value = usOnInterval;
	rc = ioctl(g_fd, IOCTLPWM_SET_ONINTERVAL, &ctrl_info);
	if(rc || ctrl_info.value != IOCTLPWM_SET_ONINTERVAL){
		PrintError("[%s][%d], IOCTLPWM_SET_ONINTERVAL error, %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return DRV_ERR;
	}

#else
#if !defined(CONFIG_FAN_CONTROLLED_BY_GPIO)
	rc = NEXUS_Pwm_SetOnInterval(s_PwmChannelHandle, usOnInterval);
	if(rc != NEXUS_SUCCESS)
	{
		PrintError("NEXUS_Pwm_SetOnInterval(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
#else
	rc = P_FAN_SetCtrlByGpio(usOnInterval);
	if(rc != 0)
	{
		PrintError("P_FAN_SetCtrlByGpio(): rc(0x%08x) \n", rc);
		return DRV_ERR;
	}
#endif
#endif
	s_stFanSetting.usPWM_OnInterval = usOnInterval;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_FAN_SetTestMode(HBOOL bTestMode)
{
	DRV_Error		errCode = DRV_OK;

	PrintEnter();

	if (bTestMode)
	{
		if (s_stFanSetting.nFanStatus != DRV_FAN_STATUS_HWTEST)
		{
			PrintDebug("[%s] Status Change :: Status(%d) -> CONFIG_HWTEST for test mode\n", __func__, s_stFanSetting.nFanStatus);
			s_stFanSetting.nFanStatus = DRV_FAN_STATUS_HWTEST;
		}
	}
	else	// set task mode
	{
		if (s_stFanSetting.nFanStatus == DRV_FAN_STATUS_HWTEST)
		{
			PrintDebug("[%s] Status Change :: CONFIG_HWTEST -> STATUS_INIT for auto (task) mode\n", __func__);
			s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;

			errCode = DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);	// first Ctrl Status is "STOP"
			if(errCode != DRV_OK)
			{
				PrintError("DRV_FAN_SetCtrl(): errCode(0x%x) \n", errCode);
				return DRV_ERR;
			}
		}
	}

	PrintExit();

	return errCode;
}

DRV_Error DRV_FAN_DisplayInfo(char* pMsg)
{
	DRV_Error		errCode = DRV_OK;
	unsigned char 	dispbuf[32];
	char caMsg[200];
	char caMsgTemp[100];

	PrintEnter();

	VK_memset(caMsg, 0x00, 200);
	VK_memset(caMsgTemp, 0x00, 100);

#if	0
	unsigned char 	a,b,c,e,f;

	a = (ucSmartTemp / 100) + '0';
	b = ((ucSmartTemp % 100) / 10) + '0';
	c = ((ucSmartTemp % 100) % 10) + '0';

	e = (s_stFanSetting.ulVoltValue / 10) + '0';
	f = (s_stFanSetting.ulVoltValue % 10) + '0';

	VK_sprintf((char *)dispbuf,"T=%c%c%c%c%c Fan=+%c%cV\n",a,b,c,0xDF,0x63,e,f);
#else
	VK_snprintf((char *)dispbuf, sizeof(dispbuf), "T=%02u%cF=%02uv %03uHz\n", s_stFanSetting.ucSmartTemp, 0xDF, s_stFanSetting.ulVoltValue, s_stFanSetting.ulFanSpeed_Freq);
#endif
#if !defined(CONFIG_DI_SHARED_LIB)
	errCode = DRV_MICOM_WriteString(dispbuf, VK_strlen((const char *)dispbuf));
	AssertDRV(errCode, DRV_ERR);
#endif
	PrintInfo("HDD Temp(%02u'C), nFanStatus(%u=%s), nFanCtrl(%u), ulVoltValue(%02uV)\n",
		s_stFanSetting.ucSmartTemp, s_stFanSetting.nFanStatus, s_stFanSetting.nFanStatus==DRV_FAN_STATUS_HWTEST ? "CONFIG_HWTEST-Manual":"TASK-Automatic",
		s_stFanSetting.nFanCtrl, s_stFanSetting.ulVoltValue);
	PrintInfo("usControlWord(0x%04x), usPeriod(0x%04x), usOnInterval(0x%04x)\n",
		s_stFanSetting.usPWM_cWord, s_stFanSetting.usPWM_PeriodInterval, s_stFanSetting.usPWM_OnInterval);
	VK_snprintf(caMsgTemp, sizeof(caMsgTemp), "HDD Temp : %02u'C\nFAN Voltage : %uV\n", s_stFanSetting.ucSmartTemp, s_stFanSetting.ulVoltValue);
	VK_strncat(caMsg, caMsgTemp, 99);

	//PrintInfo("FAN Speed (check every 5~10 sec) :: Freq(%dHz), Adjusted Voltage(%02uV) normalized by %dHz=11V\n\n",
	//	s_stFanSetting.ulFanSpeed_Freq, s_stFanSetting.ulFanSpeed_AdjVolt, FREQ_11V);
	PrintInfo("FAN Speed :: Freq(%dHz)\n\n", s_stFanSetting.ulFanSpeed_Freq);

	VK_snprintf(caMsgTemp, sizeof(caMsgTemp), "FAN Speed : %d rpm", s_stFanSetting.ulFanSpeed_Freq*60);
	VK_strncat(caMsg, caMsgTemp, 99);

	if (pMsg!=NULL)
		VK_strncpy(pMsg, caMsg, sizeof(caMsg));

	PrintExit();

	return errCode;
}

DRV_Error DRV_FAN_GetInfo(DRV_FAN_SETTING *pFanSetting)
{
	PrintEnter();

	if (pFanSetting == NULL)
	{
		PrintError("pFanSetting is NULL\n");
		return DRV_ERR;
	}

	VK_MEM_Memcpy(pFanSetting, &s_stFanSetting, sizeof(DRV_FAN_SETTING));

	PrintExit();

	return DRV_OK;
}

void DRV_FAN_RegisterEventNotify(DRV_FAN_EVENT eFanEvent, DRV_FAN_CALLBACK pfFanEventCallback)
{
#if defined(CONFIG_DI_SHARED_LIB)
	BSTD_UNUSED(eFanEvent);
	BSTD_UNUSED(pfFanEventCallback);
#else
	s_pfFanCallback[eFanEvent]= pfFanEventCallback;
#endif
	return;
}

/*****************************************************************************
* function: P_FAN_Event_Task
* description
* input:
* return:
*
******************************************************************************/
#if defined (CONFIG_FAN_SCENARIO_30)
extern DRV_Error DRV_SYSTEM_GetCPUInfo(DI_SYS_CPU_INFO_t *pCPUInfo);

static void P_FAN_Event_Task(void *p)
{
#if	defined(CONFIG_HDD_SMART) || !defined(CONFIG_DI_SHARED_LIB)
	DRV_Error		errCode = DRV_OK;
#endif
	HINT32	Temp=0,ulDemodTemp=0;
	int ucCPUTemp = 0;
	DI_SYS_FAN_INFO_t	stFanInfo;
	DI_SYS_CPU_INFO_t	stCPUInfo;
	DI_SYS_EVENT 		eSystemEvent;
#if defined(CONFIG_DEMOD_TEMPERATURE)
	DI_CH_DeviceStatus stChannelInfo;
#endif
	VK_MEM_Memset(&stCPUInfo, 0, sizeof(DI_SYS_CPU_INFO_t));

	p = p;

	while(1)
	{
		if(s_ulFanSecCnt == 0)
		{
#if	defined(CONFIG_HDD_SMART)
			if(P_FAN_GetSmartTemp(ATA_FIRST_HDD_PORT_NUM, &Temp) < 0)
			{
				PrintError("[%s] P_FAN_GetSmartTemp Error\n", __FUNCTION__);
				PrintError("Temp = THRESHOLD_TEMP(%d) for fan low voltage working.\n", LOW_THRESHOLD);
				Temp = 0; /* */
			}
#else
			// HDD SMART 외에 다른 온도 얻는 방법 사용시 여기에 coding~! 아니면, normal threshold temp 로 가정.
			Temp = LOW_THRESHOLD;
#endif
#if !defined(CONFIG_DI_SHARED_LIB)
			errCode = DRV_SYSTEM_GetCPUInfo(&stCPUInfo);
			if(errCode != DRV_OK)
			{
				PrintError("DRV_SYSTEM_GetCPUInfo Error (0x%x)\n", errCode);
			}

			ucCPUTemp = stCPUInfo.ulTemp/1000;
#else
			ucCPUTemp = 0;
#endif

#if defined(CONFIG_DEMOD_TEMPERATURE)
			errCode = DRV_CH_GetDeviceStauts(0,&stChannelInfo);
			if(errCode != DRV_OK)
			{
				PrintError("[%s] P_FAN_GetSmartTemp Error\n", __FUNCTION__);
				PrintError("Temp = THRESHOLD_TEMP(%d) for fan low voltage working.\n", LOW_THRESHOLD);
				ulDemodTemp = 0; /* */
			}
			else
			{
				ulDemodTemp=(stChannelInfo.temperature/1000);
			}

#else
			ulDemodTemp = 0;
#endif
			s_stFanSetting.ucSmartTemp = Temp;  /* To be */


			PrintError("[%s] DDI_SMART_Read_Attr_Temp : Temp = %d ucCPUTemp = %d ulDemodTemp = %d, Status = %d \n", __func__, Temp, ucCPUTemp,ulDemodTemp, s_stFanSetting.nFanStatus);

			// Default Status Check Time is 2 Min
			s_ulFanSecCnt=FAN_2_MIN;

			switch(s_stFanSetting.nFanStatus)
			{
				case DRV_FAN_STATUS_INIT:
					PrintDebug("[%s] STATUS_INIT :: Temp = %d , ucCPUTemp = %d\n", __func__, Temp, ucCPUTemp);
					if(Temp >= INITIAL_THRESHOLD || ucCPUTemp >= INITIAL_CPU_THRESHOLD || ulDemodTemp >= INITIAL_DEMOD_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_MIDDLE);
						PrintDebug("-> Fan On 9V\n");

						// Status Change : INIT -> NORMAL
						s_stFanSetting.nFanStatus = DRV_FAN_STATUS_NORMAL;
						PrintDebug("Status Change : INIT -> NORMAL\n");
					}
					else
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);
						PrintDebug("-> Fan OFF\n");
					}
					break;

				case DRV_FAN_STATUS_NORMAL:
					PrintDebug("[%s] STATUS_NORMAL :: Temp = %d ", __func__, Temp);

					/* HIGH_THRESHOLD */
					/* HDD 68, CPU 120*/


					if(Temp >= HIGH_THRESHOLD || ucCPUTemp >= HIGH_CPU_THRESHOLD || ulDemodTemp >= HIGH_DEMOD_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_HIGH);
						s_ulFanSecCnt=FAN_1_MIN;
						PrintDebug("-> Fan On 11V\n");
					}
					else if(Temp >= MIDDLE_HIGH_THRESHOLD || ucCPUTemp >= MIDDLE_HIGH_CPU_THRESHOLD || ulDemodTemp >= MIDDLE_HIGH_DEMOD_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_MIDDLE_HIGH);
						PrintDebug("-> Fan On 10V\n");
					}
					else if(Temp >= MIDDLE_THRESHOLD || ucCPUTemp >= MIDDLE_CPU_THRESHOLD || ulDemodTemp >= MIDDLE_DEMOD_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_MIDDLE);
						PrintDebug("-> Fan On 9V\n");
					}
					else if(Temp >= LOW_MIDDLE_THRESHOLD || ucCPUTemp >= LOW_MIDDLE_CPU_THRESHOLD || ulDemodTemp >= LOW_MIDDLE_DEMOD_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_LOW_MIDDLE);
						PrintDebug("-> Fan On 8V\n");
					}
					else if(Temp >= LOW_THRESHOLD || ucCPUTemp >= LOW_CPU_THRESHOLD || ulDemodTemp >= LOW_DEMOD_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_LOW);
						PrintDebug("-> Fan On 7V\n");
					}
					else	// temp < LOW_THRESHOLD
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);
						PrintDebug("-> Fan OFF\n");

						// Status Change : NORMAL -> INIT
						s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
						PrintDebug("Status Change : NORMAL -> INIT\n");
					}
					break;

				case DRV_FAN_STATUS_HWTEST:
					s_ulFanSecCnt=0;
					// CONFIG_HWTEST Sleep Time is "30" sec
					VK_TASK_Sleep(HWTEST_SLEEP_TIMEOUT);
					break;

				default:
					s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
					PrintDebug("[%s] default : Restart fan control so go to DDI_FAN_CTRL_INIT Temp = %d\n", __func__, Temp);
					s_ulFanSecCnt=0;
					break;
			}

			/* fan callback func. call for announcing fan infomation */
			if (s_pfFanCallback[DRV_FAN_EVENT_CHECK_TEMP]!=NULL)
			{
				// Fan info update
				stFanInfo.ucTemp = s_stFanSetting.ucSmartTemp;
				stFanInfo.ulFanCtrlVoltage = s_stFanSetting.ulVoltValue;
				stFanInfo.ulFanFeedback_Hz = s_stFanSetting.ulFanSpeed_Freq;

				(*s_pfFanCallback[DRV_FAN_EVENT_CHECK_TEMP])(&stFanInfo);
				PrintDebug("Callback func. is called with ucTemp(%d)v ulFanCtrlVoltage(%d), ulFanFeedback_Hz(%d)\n",
					stFanInfo.ucTemp, stFanInfo.ulFanCtrlVoltage, stFanInfo.ulFanFeedback_Hz);
			}

			if(s_stFanSetting.ulVoltValue >= DRV_FAN_CTRL_HIGH)
			{

				if(ucCPUTemp >= SHUTDOWN_CPU_THRESHOLD || s_stFanSetting.ucSmartTemp >= SHUTDOWN_THRESHOLD || ulDemodTemp >= SHUTDOWN_DEMOD_THRESHOLD)
				{
					PrintError("Over Temperature (CPU Temp=%d) (FAN Temp=%d) (DEMOD Temp=%d)!\n",ucCPUTemp,s_stFanSetting.ucSmartTemp,ulDemodTemp);
					if (s_pfFanCallback[DRV_FAN_EVENT_OVER_TEMPERATURE]!=NULL)
					{
						PrintError("CALL Over Temperature Event!\n");
						eSystemEvent = DI_SYS_EVENT_OVER_TEMPERATURE;
						(*s_pfFanCallback[DRV_FAN_EVENT_OVER_TEMPERATURE])(&eSystemEvent);
					}
				}
			}
		}
		PrintDebug("[%s] Check Fan status after %d sec \n", __func__, s_ulFanSecCnt);
		if (g_bFanDispInfo)
		{
			DRV_FAN_DisplayInfo(NULL);
		}

		if(s_ulFanSecCnt > 0)
		{
			VK_TASK_Sleep(FAN_SLEEP_TIMEOUT);
			s_ulFanSecCnt--;
		}
	}
}

#elif defined (CONFIG_JAPAN_SPEC)
#define COUNT_READ_TEMP	5
extern DRV_Error DRV_SYSTEM_GetCPUInfo(DI_SYS_CPU_INFO_t *pCPUInfo);
#if defined(CONFIG_CABLE_MODEM)
#if defined(CONFIG_CABLE_MODEM_BRCM_DSGCC)
extern DRV_Error DRV_CM_Bdsgcc_GetTemperature(HINT32 *pnCmSavedTemp, HINT32 *pnCmCurrentTemp);
#endif
#endif

static void P_FAN_Event_Task(void *p)
{
#if	defined(CONFIG_HDD_SMART) || !defined(CONFIG_DI_SHARED_LIB)
	DRV_Error		errCode = DRV_OK;
#endif
	int nHDDTemp[COUNT_READ_TEMP];
	unsigned char ucCPUTemp[COUNT_READ_TEMP];
	int nCmCurrentTemp[COUNT_READ_TEMP];
	int nCmSavedTemp[COUNT_READ_TEMP];
	int nHDDTempAvg = 0;
	int nCPUTempAvg = 0;
	int nCMTempAvg = 0;

	int nCount =0,nValidCnt_CPUTemp=0, nValidCnt_CMTemp=0, nValidCnt_HDDTemp=0;

	DI_SYS_FAN_INFO_t	stFanInfo;
	DI_SYS_CPU_INFO_t	stCPUInfo;
	DI_SYS_EVENT 		eSystemEvent;

	HUINT32 ulFanLockCount = 0;

	VK_MEM_Memset(&stCPUInfo, 0, sizeof(DI_SYS_CPU_INFO_t));

	p = p;

	while(1)
	{
		if(s_ulFanSecCnt == 0)
		{
			nValidCnt_CPUTemp = 0;
			nValidCnt_HDDTemp = 0;
			nValidCnt_CMTemp = 0;
			nHDDTempAvg = 0;
			nCPUTempAvg = 0;
			nCMTempAvg = 0;

			for(nCount = 0; nCount < COUNT_READ_TEMP; nCount ++)
			{
				VK_TASK_Sleep(100);	 /* 100ms delay */
#if	defined(CONFIG_HDD_SMART)
				if(P_FAN_GetSmartTemp(ATA_FIRST_HDD_PORT_NUM, &nHDDTemp[nCount]) < 0)
				{
					PrintError("[%s] P_FAN_GetSmartTemp Error\n", __FUNCTION__);
					PrintError("Temp = THRESHOLD_TEMP(%d) for fan low voltage working.\n", LOW_THRESHOLD);
					nHDDTemp[nCount] = 0; /* */
				}
				else
				{
					if(nHDDTemp[nCount] < 0)
					{
						PrintDebug("[%s]IGNORE-HDD(%d) = %d\n", __FUNCTION__, nCount, nHDDTemp[nCount]);
					}
					else
					{
						nValidCnt_HDDTemp++;
						nHDDTempAvg = nHDDTempAvg + nHDDTemp[nCount];
					}
				}
#else
				// HDD SMART 외에 다른 온도 얻는 방법 사용시 여기에 coding~! 아니면, normal threshold temp 로 가정.
				nHDDTemp[nCount] = LOW_THRESHOLD;
#endif
#if !defined(CONFIG_DI_SHARED_LIB)
				errCode = DRV_SYSTEM_GetCPUInfo(&stCPUInfo);
				if(errCode != DRV_OK)
				{
					PrintError("DRV_SYSTEM_GetCPUInfo Error (0x%x)\n", errCode);
					ucCPUTemp[nCount] = 0;
				}
				else
				{
					ucCPUTemp[nCount] = stCPUInfo.ulTemp/1000;
					nValidCnt_CPUTemp++;
					nCPUTempAvg = nCPUTempAvg + ucCPUTemp[nCount];
				}

#if defined(CONFIG_CABLE_MODEM)
#if defined(CONFIG_CABLE_MODEM_BRCM_DSGCC)
				errCode = DRV_CM_Bdsgcc_GetTemperature(&nCmSavedTemp[nCount],&nCmCurrentTemp[nCount]);
#endif
				if(errCode != DRV_OK)
				{
					PrintError("DI_CM_GetCmSavedTemp Error (0x%x)\n", errCode);
					nCmCurrentTemp[nCount] = 0;
				}
				else
				{
					nValidCnt_CMTemp++;
					nCMTempAvg = nCMTempAvg + (nCmCurrentTemp[nCount]/10);
				}
#endif
			}

#if defined(CONFIG_FAN_FEEDBACK_BY_LEVEL)
			P_FAN_GetFeedBackByLevel();
#endif
			 /* 5회 읽은 온도의 평균값으로 처리 */
			if(nValidCnt_HDDTemp > 0)
			{
				nHDDTempAvg = nHDDTempAvg/nValidCnt_HDDTemp;

			}
			if(nValidCnt_CPUTemp > 0)
			{
				nCPUTempAvg = nCPUTempAvg/nValidCnt_CPUTemp;
			}

			if(nValidCnt_CMTemp >0)
			{
				nCMTempAvg = nCMTempAvg/nValidCnt_CMTemp;
			}

			s_stFanSetting.ucSmartTemp = (unsigned char)nHDDTempAvg;  /* To be */

			PrintDebug("[%s] HDDTemp = %d, CPUTemp = %d, CMTemp = %d FANStatus = %d \n", __func__, nHDDTempAvg, nCPUTempAvg, nCMTempAvg,s_stFanSetting.nFanStatus);

			// Default Status Check Time is 2 Min
			s_ulFanSecCnt=FAN_2_MIN;
#endif

			switch(s_stFanSetting.nFanStatus)
			{
				case DRV_FAN_STATUS_INIT:
					PrintDebug("[%s] STATUS_INIT :: HDDTemp = %d , CPUTemp = %d, CMTemp = %d\n", __func__, nHDDTempAvg, nCPUTempAvg, nCMTempAvg);
					if(nHDDTempAvg >= INITIAL_THRESHOLD || nCPUTempAvg >= INITIAL_CPU_THRESHOLD || nCMTempAvg >= INITIAL_CM_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_LOW);
						PrintDebug("-> Fan On 8V\n");

						// Status Change : INIT -> NORMAL
						s_stFanSetting.nFanStatus = DRV_FAN_STATUS_NORMAL;
						PrintDebug("Status Change : INIT -> NORMAL\n");
						s_ulFanSecCnt=FAN_2_MIN;
					}
					else
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);
						PrintDebug("-> Fan OFF\n");
						s_ulFanSecCnt=FAN_5_MIN;
					}
					break;

				case DRV_FAN_STATUS_NORMAL:
					PrintDebug("[%s] STATUS_NORMAL :: HDDTemp = %d , CPUTemp = %d, CMTemp = %d, s_stFanSetting.ulFanSpeed_Freq =%d\n", __func__, nHDDTempAvg, nCPUTempAvg, nCMTempAvg,s_stFanSetting.ulFanSpeed_Freq);

					/* HIGH_THRESHOLD */
					/* HDD 65, CPU 115*/

					if(nHDDTempAvg >= HIGH_THRESHOLD || nCPUTempAvg >= HIGH_CPU_THRESHOLD || nCMTempAvg >= HIGH_CM_THRESHOLD || s_stFanSetting.ulFanSpeed_Freq <=FAN_LOCK_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_HIGH);
						s_ulFanSecCnt=FAN_1_MIN;
						PrintDebug("-> Fan On 11V\n");
					}
					else if(nHDDTempAvg >= LOW_THRESHOLD || nCPUTempAvg >= LOW_CPU_THRESHOLD || nCMTempAvg >= LOW_CM_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_LOW);
						PrintDebug("-> Fan On 8V\n");
					}
					else	// temp < LOW_THRESHOLD
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);
						PrintDebug("-> Fan OFF\n");

						// Status Change : NORMAL -> INIT
						s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
						PrintDebug("Status Change : NORMAL -> INIT\n");
					}
					break;

				case DRV_FAN_STATUS_HWTEST:
					s_ulFanSecCnt=0;
					// CONFIG_HWTEST Sleep Time is "30" sec
					VK_TASK_Sleep(HWTEST_SLEEP_TIMEOUT);
					break;

				default:
					s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
					PrintDebug("[%s] default : Restart fan control so go to DDI_FAN_CTRL_INIT HDDTemp = %d , CPUTemp = %d, CMTemp = %d\n", __func__, nHDDTempAvg, nCPUTempAvg, nCMTempAvg);
					s_ulFanSecCnt=0;
					break;
			}

			/* fan callback func. call for announcing fan infomation */
			if (s_pfFanCallback[DRV_FAN_EVENT_CHECK_TEMP]!=NULL)
			{
				// Fan info update
				stFanInfo.ucTemp = s_stFanSetting.ucSmartTemp;
				stFanInfo.ulFanCtrlVoltage = s_stFanSetting.ulVoltValue;
				stFanInfo.ulFanFeedback_Hz = s_stFanSetting.ulFanSpeed_Freq;

				(*s_pfFanCallback[DRV_FAN_EVENT_CHECK_TEMP])(&stFanInfo);
				PrintDebug("Callback func. is called with ucTemp(%d)v ulFanCtrlVoltage(%d), ulFanFeedback_Hz(%d)\n",
					stFanInfo.ucTemp, stFanInfo.ulFanCtrlVoltage, stFanInfo.ulFanFeedback_Hz);
			}

			if(s_stFanSetting.ulVoltValue >= DRV_FAN_CTRL_HIGH)
			{
				if(s_stFanSetting.ulFanSpeed_Freq <=FAN_LOCK_THRESHOLD)
				{
					if (s_pfFanCallback[DRV_FAN_EVENT_FAN_LOCKED]!=NULL)
					{
						PrintDebug("Fan Locked!\n");
						eSystemEvent = DI_SYS_EVENT_FAN_LOCKED;
						ulFanLockCount++;
						s_ulFanSecCnt=FAN_10_SEC;
						if(ulFanLockCount >= 6)
						{
							PrintError("CALL Fan Locked Event!!\n");
							(*s_pfFanCallback[DRV_FAN_EVENT_FAN_LOCKED])(&eSystemEvent);
						}
					}
				}
				else
				{
					ulFanLockCount = 0;
				}

				if(nHDDTempAvg >= HIGH_THRESHOLD || nCPUTempAvg >= HIGH_CPU_THRESHOLD || nCMTempAvg >= HIGH_CM_THRESHOLD)
				{
					PrintError("[Over Temperature] VOLT(%d) :: HDDTemp = %d , CPUTemp = %d, CMTemp = %d, s_stFanSetting.ulFanSpeed_Freq =%d\n", s_stFanSetting.ulVoltValue, nHDDTempAvg, nCPUTempAvg, nCMTempAvg,s_stFanSetting.ulFanSpeed_Freq);
					if (s_pfFanCallback[DRV_FAN_EVENT_OVER_TEMPERATURE]!=NULL)
					{
						PrintError("CALL Over Temperature Event!\n");
						eSystemEvent = DI_SYS_EVENT_OVER_TEMPERATURE;
						(*s_pfFanCallback[DRV_FAN_EVENT_OVER_TEMPERATURE])(&eSystemEvent);
					}
				}
			}
			else
			{
				ulFanLockCount = 0;
			}
		}

		PrintDebug("[%s] Check Fan status after %d sec \n", __func__, s_ulFanSecCnt);
		if (g_bFanDispInfo)
		{
			DRV_FAN_DisplayInfo(NULL);
		}

		if(s_ulFanSecCnt > 0)
		{
			VK_TASK_Sleep(FAN_SLEEP_TIMEOUT);
			s_ulFanSecCnt--;
		}
	}
}


#else

static void P_FAN_Event_Task(void *p)
{
	int	Temp=0;
	DI_SYS_FAN_INFO_t	stFanInfo;

	p = p;

	while(1)
	{
		if(s_ulFanSecCnt == 0)
		{
#if	defined(CONFIG_HDD_SMART)
			if(P_FAN_GetSmartTemp(ATA_FIRST_HDD_PORT_NUM, &Temp) < 0)
			{
				PrintError("[%s] P_FAN_GetSmartTemp Error\n", __FUNCTION__);
				PrintError("Temp = THRESHOLD_TEMP(%d) for fan low voltage working.\n", LOW_THRESHOLD);
				Temp = LOW_THRESHOLD;
			}
#else
			// HDD SMART 외에 다른 온도 얻는 방법 사용시 여기에 coding~! 아니면, normal threshold temp 로 가정.
			Temp = LOW_THRESHOLD;
#endif
			s_stFanSetting.ucSmartTemp = Temp;

			PrintDebug("[%s] DDI_SMART_Read_Attr_Temp : Temp = %d Status = %d \n", __func__, Temp, s_stFanSetting.nFanStatus);

			// Default Status Check Time is 2 Min
			s_ulFanSecCnt=FAN_2_MIN;

			switch(s_stFanSetting.nFanStatus)
			{
				case DRV_FAN_STATUS_INIT:
					PrintDebug("[%s] STATUS_INIT :: Temp = %d ", __func__, Temp);
					if(Temp >= INITIAL_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_MIDDLE);
						PrintDebug("-> Fan On 9V\n");

						// Status Change : INIT -> NORMAL
						s_stFanSetting.nFanStatus = DRV_FAN_STATUS_NORMAL;
						PrintDebug("Status Change : INIT -> NORMAL\n");
					}
					else
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);
						PrintDebug("-> Fan OFF\n");
					}
					break;

				case DRV_FAN_STATUS_NORMAL:
					PrintDebug("[%s] STATUS_NORMAL :: Temp = %d ", __func__, Temp);
					if(Temp >= HIGH_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_HIGH);
						PrintDebug("-> Fan On 11V\n");
					}
					else if(Temp >= MIDDLE_HIGH_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_MIDDLE_HIGH);
						PrintDebug("-> Fan On 10V\n");
					}
					else if(Temp >= MIDDLE_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_MIDDLE);
						PrintDebug("-> Fan On 9V\n");
					}
					else if(Temp >= LOW_MIDDLE_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_LOW_MIDDLE);
						PrintDebug("-> Fan On 8V\n");
					}
					else if(Temp >= LOW_THRESHOLD)
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_LOW);
						PrintDebug("-> Fan On 7V\n");
					}
					else	// temp < LOW_THRESHOLD
					{
						DRV_FAN_SetCtrl(DRV_FAN_CTRL_STOP);
						PrintDebug("-> Fan OFF\n");

						// Status Change : NORMAL -> INIT
						s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
						PrintDebug("Status Change : NORMAL -> INIT\n");
					}
					break;

				case DRV_FAN_STATUS_HWTEST:
					s_ulFanSecCnt=0;
					// CONFIG_HWTEST Sleep Time is "30" sec
					VK_TASK_Sleep(HWTEST_SLEEP_TIMEOUT);
					break;

				default:
					s_stFanSetting.nFanStatus = DRV_FAN_STATUS_INIT;
					PrintDebug("[%s] default : Restart fan control so go to DDI_FAN_CTRL_INIT Temp = %d\n", __func__, Temp);
					s_ulFanSecCnt=0;
					break;
			}

			/* fan callback func. call for announcing fan infomation */
			if (s_pfFanCallback[DRV_FAN_EVENT_CHECK_TEMP]!=NULL)
			{
				// Fan info update
				stFanInfo.ucTemp = s_stFanSetting.ucSmartTemp;
				stFanInfo.ulFanCtrlVoltage = s_stFanSetting.ulVoltValue;
				stFanInfo.ulFanFeedback_Hz = s_stFanSetting.ulFanSpeed_Freq;

				(*s_pfFanCallback[DRV_FAN_EVENT_CHECK_TEMP])(&stFanInfo);
				PrintDebug("Callback func. is called with ucTemp(%d)v ulFanCtrlVoltage(%d), ulFanFeedback_Hz(%d)\n",
					stFanInfo.ucTemp, stFanInfo.ulFanCtrlVoltage, stFanInfo.ulFanFeedback_Hz);
			}
		}
		PrintDebug("[%s] Check Fan status after %d sec \n", __func__, s_ulFanSecCnt);
		if (g_bFanDispInfo)
		{
			DRV_FAN_DisplayInfo(NULL);
		}

		if(s_ulFanSecCnt > 0)
		{
			VK_TASK_Sleep(FAN_SLEEP_TIMEOUT);
			s_ulFanSecCnt--;
		}
	}
}


#endif

