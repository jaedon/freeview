/**
 * @file	di_channel_priv.h
 * @brief	Channel Analog module DI layer header file
 * @version $Revision: 0.9.0 $
 * @date 	$Date: 2008/06/24 12:00:00 $
 * @author 	Linus Lee
 */

/** @defgroup DI_CH_ANALOG
 *  This is CHANNEL module of DI layer.
 *  @{
 */

#ifndef _DI_CHANNEL_PRIV_H_
#define _DI_CHANNEL_PRIV_H_
/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "vkernel.h"

#include "drv_err.h"

#include "di_channel.h"
#include "di_channel_a.h"
#include "di_channel_s.h"
#include "di_channel_t.h"
#include "di_channel_c.h"

#include "di_uart.h"

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

#if defined(CONFIG_FRONTEND_SAT)&&defined(CONFIG_FRONTEND_CAB)&&defined(CONFIG_FRONTEND_TER)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL+CONFIG_NUM_OF_CAB_CHANNEL+CONFIG_NUM_OF_TER_CHANNEL)
#elif defined(CONFIG_FRONTEND_SAT)&&defined(CONFIG_FRONTEND_CAB)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL+CONFIG_NUM_OF_CAB_CHANNEL)
#elif defined(CONFIG_FRONTEND_SAT)&&defined(CONFIG_FRONTEND_TER)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL+CONFIG_NUM_OF_TER_CHANNEL)
#elif defined(CONFIG_FRONTEND_CAB)&&defined(CONFIG_FRONTEND_TER)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_CAB_CHANNEL+CONFIG_NUM_OF_TER_CHANNEL)
#elif defined(CONFIG_FRONTEND_SAT)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_SAT_CHANNEL)
#elif defined(CONFIG_FRONTEND_CAB)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_CAB_CHANNEL)
#elif defined(CONFIG_FRONTEND_TER)
#define NUM_OF_CHANNEL	(CONFIG_NUM_OF_TER_CHANNEL)
#else
#define NUM_OF_CHANNEL 0
#endif



#if defined(CONFIG_DEMOD_TSMF)
typedef enum DRV_CH_TSMF_STATUS
{
	DRV_CH_TSMF_IDLE = 0,
	DRV_CH_TSMF_SEARCHING,
	DRV_CH_TSMF_RECEIVED,
	DRV_CH_TSMF_TIMEOUT,
	DRV_CH_TSMF_WAIT_LIVEDEMOD
} DRV_CH_TSMF_STATUS_e;

typedef void(*pfnCH_TSMFNotify)(HUINT32 ulDemodId, HUINT8 *pucBuf);
DRV_Error DRV_CH_TSMF_Init(void);
DRV_Error DRV_CH_TSMF_RegisterTSMFCallback (pfnCH_TSMFNotify pfnTSMFNotify);
DRV_Error DRV_CH_TSMF_GetStatus(HUINT8 ucTSMFIdx, DRV_CH_TSMF_STATUS_e *peStatus);
DRV_Error DRV_CH_TSMF_RequestSearch(HUINT32 nDemodId);
DRV_Error DRV_CH_TSMF_CancleSearch(HUINT32 nDemodId);
DRV_Error DRV_CH_TSMF_EnableMonitor(void);
DRV_Error DRV_CH_TSMF_DisableMonitor(void);

#if (NEXUS_VERSION >= 1290)
DRV_Error DRV_CH_TSMF_GetHandle(HUINT32 ulDemodId, HUINT32* pulTsmfHandle);
DRV_Error DRV_CH_TSMF_SetHandle(HUINT32 ulDemodId, HUINT32 ulTsmfHandle);
#endif
#endif /* CONFIG_DEMOD_TSMF */


#define HAVE_C99 /* BCM compile option */

#define CH_UNUSED(x)  { volatile void *pvUnused; pvUnused = (void *)&(x); }

#define __CH_DEBUG__
//#define __CH_RELEASE_MODE_DEBUG__
//#define __CH_MONITOR__
//#define __CH_SUPPORT_PRINT_SNR__
//#define CH_TAGGED_API
//#define CH_PRINT_DI_LEVEL_ZAPPING_TIME


#define CH_Print_NOP() (void)0

#ifdef __CH_DEBUG__

#if 0
#if 0
#define CH_DI_Print(x, y)	( ((x) <= show_di_ch) ? (DI_UART_Print("@DI_CH "),DI_UART_Print y) : 0 )
#else
#define CH_DI_Print(x, y)	( (((x) == 0) || ((x) == show_di_ch)) ? (DI_UART_Print("@DI_CH "),DI_UART_Print y) : ( (6 == show_di_ch) ? (DI_UART_Print("@DI_CH "),DI_UART_Print y) : 0 ) )
#endif
#else
/* NEW DI DEBUG PRINT */
#define CH_DI_Print(x, y)	( (((x) == 0) || (((x) & show_di_ch) != 0)) ? (DI_UART_Print("@DI_CH "),DI_UART_Print y) : 0 )
#endif

#if defined(CONFIG_DEMOD_TSMF)
#define CH_DI_TSMF_Print(x, y)	( ((x) <= show_di_ch_tsmf) ? (DI_UART_Print("@DI_CH_TSMF "),DI_UART_Print y) : 0 )
#endif

#if defined(CONFIG_CABLE_MODEM)
#define CM_DI_Print(x, y)	( ((x) <= show_di_cm) ? (DI_UART_Print("@DI_CM "),DI_UART_Print y) : 0 )
#endif

#if !defined(CONFIG_DEBUG)
#ifdef CH_DI_Print
#undef CH_DI_Print
#define CH_DI_Print(...)    ((void)0)
#endif
#ifdef CH_DI_TSMF_Print
#undef CH_DI_TSMF_Print
#define CH_DI_TSMF_Print(...)    ((void)0)
#endif
#endif

#if defined(HAVE_C99)

#if defined(__CH_RELEASE_MODE_DEBUG__)
#define CH_PrintFunc(...) {DI_UART_Print("[DI_CHANNEL] \t\t%s() : ", __FUNCTION__); \
							DI_UART_Print(__VA_ARGS__); \
							DI_UART_Print("(%s:%d)\n", __FILE__,__LINE__);}

#define CH_PrintData		DI_UART_Print

#define CH_PrintDebug(...) {DI_UART_Print("[DI_CHANNEL] %s() : ", __FUNCTION__); \
							DI_UART_Print(__VA_ARGS__);}
#else
#define CH_PrintFunc(...)		CH_Print_NOP()
#define CH_PrintData(...)		CH_Print_NOP()
#define CH_PrintDebug(...)		CH_Print_NOP()
#endif

#define CH_PrintErr(...) {DI_UART_Print("[DI_CHANNEL] Error : %s() : ", __FUNCTION__); \
							DI_UART_Print(__VA_ARGS__); \
							DI_UART_Print("(%s:%d)\n", __FILE__,__LINE__);}

#define CH_PrintMon			DI_UART_Print

#else /* defined(HAVE_C99) */

#define CH_PrintFunc(fmt, args...)		DI_UART_Print("[DI_CHANNEL] \t\t%s() : " fmt "(%s:%d)\n", \
																	__FUNCTION__,## args,__FILE__,__LINE__)
#define CH_PrintData					DI_UART_Print
#define CH_PrintDebug(fmt, args...) 	DI_UART_Print("[DI_CHANNEL] %s() : " fmt, __FUNCTION__,## args)
#define CH_PrintErr(fmt, args...) 		DI_UART_Print("[DI_CHANNEL] Error : %s() : " fmt "(%s:%d)\n", \
																	__FUNCTION__,## args,__FILE__,__LINE__)
#define CH_PrintMon						DI_UART_Print

#endif /* defined(HAVE_C99) */

#else	/* Release mode  */

#define CH_DI_Print(x, y)               CH_Print_NOP()

#if defined(HAVE_C99)

#ifndef __CH_RELEASE_MODE_DEBUG__
#define CH_PrintFunc(...)		CH_Print_NOP()
#define CH_PrintData(...)		CH_Print_NOP()
#define CH_PrintMon(...)		CH_Print_NOP()

#else
#define CH_PrintFunc(...) {DI_UART_Print("[DI_CHANNEL] \t\t%s() : ", __FUNCTION__); \
							DI_UART_Print(__VA_ARGS__); \
							DI_UART_Print("(%s:%d)\n", __FILE__,__LINE__);}
#define CH_PrintData			DI_UART_Print
#define CH_PrintMon			DI_UART_Print
#endif

#define CH_PrintDebug(...)		CH_Print_NOP()
#define CH_PrintErr(...) {DI_UART_Print("[DI_CHANNEL] Error : %s() : ", __FUNCTION__); \
							DI_UART_Print(__VA_ARGS__); \
							DI_UART_Print("(%s:%d)\n", __FILE__,__LINE__);}

#else /* defined(HAVE_C99) */

#ifndef __CH_RELEASE_MODE_DEBUG__
#define CH_PrintFunc(fmt, args...)		CH_Print_NOP()
#define CH_PrintData(fmt, args...)		CH_Print_NOP()
#define CH_PrintMon(fmt, args...)		CH_Print_NOP()

#else
#define CH_PrintFunc(fmt, args...)		DI_UART_Print("[DI_CHANNEL] \t\t%s() : " fmt "(%s:%d)\n", \
																	__FUNCTION__,## args,__FILE__,__LINE__)
#define CH_PrintData		DI_UART_Print
#define CH_PrintMon		DI_UART_Print

#endif

#define CH_PrintDebug(fmt, args...)	CH_Print_NOP()

#define CH_PrintErr(fmt, args...) 		DI_UART_Print("[DI_CHANNEL] Error : %s() : " fmt "(%s:%d)\n", \
																	__FUNCTION__,## args,__FILE__,__LINE__)

#endif /* defined(HAVE_C99) */

#endif /* __CH_DEBUG__ */


#define CH_MSGQ_SIZE				64
#define CH_TASK_PRIORITY			61
#define CH_TASK_STACK_SIZE			(1024*2)//(1024*2)

#define NAME_LENGTH				15

#define CONST_CH_Q_SIZE			16 /* (HUINT32 * 4) */

#define DI_CH_CANCEL_TUNE			(4)



#define KHZ_TO_HZ		1000
#define MHZ_TO_HZ		1000000


// HAVE TO BE REMOVED !!
#define CH_LOG_0	0  //Error, Warning
#define CH_LOG_1	1 // Tuning parameters in Tune(), LOCK/UNLOCK notice just after Tune(), any message in Init()
#define CH_LOG_2	2 // Signal information, TS information like, plp ids, cell ids, tsmf, tmcc parameters.
#define CH_LOG_3	3 // Lock/Unlock status in callback task
#define CH_LOG_4	4 // Arguments and messages in each function


#define CH_LOG_DEFAULT	0x00	// This is default level - Error, Warning
#define CH_LOG_TUNE		0x01	// tune - Tuning parameters in Tune(), LOCK/UNLOCK notice just after Tune(), any message in Init()
#define CH_LOG_SIGINFO	0x02	// siginfo - Signal information (RF related)
#define CH_LOG_STATUS	0x04    // status - Any messages for Lock/Unlock status (not info)
#define CH_LOG_ARGS		0x08	// args - Arguments and messages in each function
#define CH_LOG_TSINFO	0x10	// tsinfo - TS information like, plp ids, cell ids, tsmf, tmcc parameters
#define CH_LOG_OPT1		0x20
#define CH_LOG_OPT2		0x40
#define CH_LOG_OPT3		0x80
#define CH_LOG_ALL		0xFF	// This is only used in cmd function

/******************************************************************************
 * Data Types
 ******************************************************************************/
typedef int CHANNEL_ID_t;
typedef int TUNER_ID_t;
/*typedef int DI_CHANNEL_ERR;*/



/******************************************************************************
 * Data Types
 ******************************************************************************/
typedef enum
{
	CH_S_POLAR 			= 0x1,			/**< Hor, Ver */
	CH_S_CODERATE		= 0x2,			/**< 1/2, 2/3, etc */
	CH_S_MODULATION 	= 0x4,			/**< BPSK, QPSK */
	CH_S_PILOT 			= 0x8			/**< On, Off */
} CH_S_AutoMode_e;

typedef enum
{
	CH_C_MODULATION		= 0x1			/**< 64, 256, 128, 32, 16QAM */

} CH_C_AutoMode_e;


typedef enum
{
	CH_CMD_IDLE,
	CH_CMD_LOCKING,
	CH_CMD_AUTOSEARCH,
	CH_CMD_NOT_DEFINED

}CH_Command_e;

typedef enum
{
	CH_STATUS_IDLE,
	CH_STATUS_LOCKING,
	CH_STATUS_SEARCHING,
	CH_STATUS_LOCKED,
	CH_STATUS_CANCLE,
	CH_STATUS_UNLOCK,
	CH_STATUS_ANT_FAULT
}CH_Status_e;

typedef enum
{
	CH_ACTION_IDLE,
	CH_ACTION_ON,
	CH_ACTION_OFF
}CH_Active_e;

typedef enum
{
	CH_TS_PATH_ON,
	CH_TS_PATH_OFF
}CH_TsStatus_e;

typedef enum
{
	CH_POWER_ON,
	CH_POWER_OFF
}CH_Power_e;

typedef struct
{
	CH_Status_e 		etPreviousState;
	CH_Status_e 		etCurrentState;
}
CH_ControlState_t;

typedef struct
{
	HULONG 			ulTaskID;
	HULONG 			ulQueueID;
	HULONG 			ulSemID;
	HULONG 			ulEventID;
}CH_Resource_t;

typedef struct
{
	int 				nTunerID;
	HUINT32				unCapability;
}CH_InfoTuner_t;

typedef struct
{
	HUINT32 			unBusID;
	HUINT32				unDevAddr;
	HUINT32				unTunerAddr;
	HUINT32				unIfAddr;
	HUINT32				unLnbAddr;
}CH_InfoI2c_t;

//#if defined(MODEL_ICORDHD2)
/*jeehye : temporary (There are defined "di_common.h" at douglas)*/
typedef enum
{
	CH_TS_MODE_SERIAL,
	CH_TS_MODE_PARALLEL,
	CH_TS_MODE_MTSIF
}CH_TsOutMode_e;

typedef enum
{
	CH_POLAR_HOR,					/**< Horizontal 18 Volt */
	CH_POLAR_VER					/**< Vertical 13 Volt */
} CH_Polarization_e;
//#endif

typedef struct
{
	CH_TsStatus_e		etTsOnOff;
	CH_TsOutMode_e		etTsOutMode;
}CH_InfoTsStatus_t;


typedef struct
{
	HUINT32					unRequestID;
	void *					pvPreviousParam;
	void *					pvRequestedParam;
	void *					pvWorkingParam;
	HUINT32					ulAutoSearchMode;
}CH_TuningParam_t;

typedef struct
{
	DI_CH_Type_e 					etChannelType;
	int 							nTunerID;
	int 							nUnitId;

	CH_InfoI2c_t					stChannelI2c;

	CH_Resource_t					stChannelRsc;

	CH_Active_e						etChannelAction;

	CH_InfoTsStatus_t				stTsStatus;

	CH_ControlState_t				stChannelControl;

	pfnDI_CH_NotifySignalInfo 		SignalCallback;

	CH_TuningParam_t				stTuningInfo;

	DI_CH_InputSource_e				etRfInputSource;
}CH_InfoChannel_t;



/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/

/******************************************************************************
 * Local(static) Function Prototypes
 ******************************************************************************/


#endif /* _DI_CHANNEL_PRIV_H_ */

/** @} */
