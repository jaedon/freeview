/**
 * di_smartcard.c
*/

/**
 * @defgroup		DI_SMARTCARD DI_SMARTCARD : DI_SMARTCARD module
 * @author		Heeyoung Kim
 * @note			DI_SMARTCARD APIs
 * @brief			Define DI_SMARTCARD APIs
 * @file 			di_smartcard.c
*/

/*******************************************************************/
/**************************** Header Files ****************************/
/*******************************************************************/
#if defined(CONFIG_VK_STDLIB)
#else
#include 	<stdio.h>
#include 	<string.h>
#endif
#include <unistd.h>

#include "di_err.h"
#include "drv_err.h"
#include "linden_trace.h"
#include "taskdef.h"
#include "vkernel.h"

#include "nexus_platform.h"
#include "nexus_smartcard.h"

#include "bstd.h"
#include "bkni.h"

#include "di_smartcard.h"
#include "drv_smartcard.h"
#include "drv_gpio.h"
#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
#include "drv_pm.h"
#endif

/*******************************************************************/
/****************************** define *******************************/
/*******************************************************************/

#define DEBUG_MODULE        TRACE_MODULE_DI_SC

#define ENABLE_SC_MINIMUN_DEBUG

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
#if defined(CONFIG_MULTI_SMARTCARD)
#define NUM_SMARTCARD 2
#else
#define NUM_SMARTCARD 1
#endif

#define MAX_DATA_SIZE  255

#define MAX_ATR_SIZE 33 //32
#define DI_SC_MAX_BAUD_RATE		6500000

#define DI_SC_DEFAULT_BLOCK_WAIT_TIME    		(0x04) 		/* ISO/IEC 7816-3 default */
#define DI_SC_DEFAULT_CHARACTER_WAIT_TIME  		(0x0D)		//DEFAULT_CWI = 13
#define DI_SC_DEFAULT_WORK_WAIT_TIME       		(0x0A) 		/* ISO/IEC 7816-3 default */
#if defined(CONFIG_CAS_NA) && defined(CONFIG_CAS_NA_CAK7) && defined(CONFIG_CAS_NA_ISO7816)
#define DI_SC_T1_DATA_TIMEOUT				3800
#define DI_SC_DEFAULT_TIMEOUT				1000		/* msec */
#else
#define DI_SC_DEFAULT_TIMEOUT				1000		/* msec */
#endif

#define DI_SC_DEFAULT_BLOCK_GUARD_TIME 		22

#define	DI_SC_T0_HEADER_NUM		5
#define	DI_SC_T0_BYTE_TO_BYTE_NUM		1
#define DI_SC_T0_ACK_CHECK				1

#if 0	//reference
typedef enum
{
	DI_SC_CLK_EXTERNAL				= 0,
//ext use
	DI_SC_CLK_EXTERNAL_3600KHZ,		// etu_clkdiv = 001 & SC_CLK=100 ; 36MHz/7 = 3600 MHz
	DI_SC_CLK_EXTERNAL_4000KHZ,		// etu_clkdiv = 010 & SC_CLK=010 ; 36MHz/6 = 4 MHz
	DI_SC_CLK_EXTERNAL_4500KHZ,		// etu_clkdiv = 111 & SC_CLK=000 ; 36MHz/5 = 4.5 MHz--> NDS, NAGRA T=1 default
	DI_SC_CLK_EXTERNAL_9000KHZ,		// etu_clkdiv = 011 & SC_CLK=000 ; 36MHz/4 = 9 MHz
//int use
	DI_SC_CLK_INTERNAL_1687KHZ, 		// etu_clkdiv = 000 & SC_CLK=111 ; 27MHz/16 = 1687.5 MHz
	DI_SC_CLK_INTERNAL_2700KHZ,		// etu_clkdiv = 001 & SC_CLK=100 ; 27MHz/5/2 = 2.7 MHz
	DI_SC_CLK_INTERNAL_3375KHZ,		// etu_clkdiv = 111 & SC_CLK=000; 27MHz/8 = 3.375 MHz
	DI_SC_CLK_INTERNAL_3857KHZ,		// etu_clkdiv = 110 ; 27MHz/7 = 3.857 MHz
	DI_SC_CLK_INTERNAL_4500KHZ,		// etu_clkdiv = 101 & SC_CLK=000; 27MHz/6 = 4.5 MHz --> NDS, NAGRA T=1 default
//int no use
	DI_SC_CLK_INTERNAL_3000KHZ,			// etu_clkdiv = 010 & SC_CLK=010 ; 27MHz/3/3 = 3 MHz
	DI_SC_CLK_INTERNAL_5400KHZ,			// etu_clkdiv = 100 ; 27MHz/5 = 5.4 MHz
	DI_SC_CLK_INTERNAL_6750KHZ,			// etu_clkdiv = 011 ; 27MHz/4 = 6.75 MHz
	DI_SC_CLK_INTERNAL_9000KHZ,			// etu_clkdiv = 010 ; 27MHz/3 = 9 MHz
	DI_SC_CLK_INTERNAL_13500KHZ,			// etu_clkdiv = 001 ; 27MHz/2 = 13.5 MHz
	DI_SC_CLK_INTERNAL_27000KHZ			// etu_clkdiv = 000 ; 27MHz/1 = 27 MHz
}DI_SC_CLK;
#endif

typedef enum
{
	 P_SC_VccLevel_e5V = 0,   /* 5v is default value */
	 P_SC_VccLevel_e3V = 1    /* 3v  */
} P_SC_VccLevel;

 typedef enum
 {
	 DI_SC_CLOCK_INTERNAL_27M = 0,
	 DI_SC_CLOCK_EXTERNAL_24M,
	 DI_SC_CLOCK_EXTERNAL_36M,
	 DI_SC_CLOCK_MAX
 } DI_SC_CLOCK;

#define DI_SC_CLK_INTERNAL_3857KHZ	3857000	// etu_clkdiv = 110 ; 27MHz/7 = 3.857 MHz

 typedef enum
 {
	DI_SC_STATE_Unknown = 0,			 /* Unknown state (perhaps not yet initialized).  Persistent. */
	DI_SC_STATE_ColdReset,				 /* A cold reset has been requested but is not yet complete.  Transient. */
	DI_SC_STATE_WarmReset, 			 /* A warm reset has been requested but is not yet complete.  Transient. */
	DI_SC_STATE_ResetDone,				 /* The slot/card reset has completed.  Persistent. */
	DI_SC_STATE_Activate,				 /* The slot/card is currently being activated, but activation is not yet complete.  Transient. */
	DI_SC_STATE_ReceiveDecodeATR,	 /* The ATR is being received or decoded.  Transient. */
	DI_SC_STATE_Ready,					 /* The slot/card is initialized and is awaiting sends/receives.  Persistent. */
	DI_SC_STATE_Transmitting,			 /* The slot/card is currently transmitting.  Transient. */
	DI_SC_STATE_Transmitted,			 /* The slot/card has completed its transmission.  Persistent. */
	DI_SC_STATE_Receiving,				 /* The slot/card is currently receiving.  Transient. */
	DI_SC_STATE_Ignore,					 /* The slot/card is ignoring events/commands.  Persistent. */
	DI_SC_STATE_Initialized, 				/* The slot/card has been initialized, but the ATR has not yet been received.  Persistent. */
	DI_SC_STATE_MAX_STATE			 /* A value indicating the total number of possible states.  The state returned from bsmartcard_get_status should never exceed this value. */
} DI_SC_STATE;

typedef struct _stCallback
{
	pfnDI_SC_EVT_CALLBACK pfnCallback;
	void *pParam;
}stCALLBACK;

typedef	struct	_DI_SC_info
{
	unsigned char 				ucInit;
	DI_SC_PROTOCOL			nScProtocol;
	DI_SC_CLOCK 				nClockSourceType;
	unsigned long				ulScClock;

	unsigned char				ucScFI;
	unsigned char				ucScDI;
	unsigned char				ucScN;
	unsigned char				ucScCWI;
	unsigned char				ucScBWI;
}DI_SmartcardParams;

typedef struct _stSmartcardInstance
{
	NEXUS_SmartcardHandle hSmartcard;
	unsigned int slotId;		//hardware slot number.
	unsigned long semId;
	stCALLBACK pstSmartcardCallback;

	unsigned char s_aucAtrBuf[MAX_ATR_SIZE];

	DI_SmartcardParams stDrvScInfo;
	DI_SC_STATE state;
	DI_SC_STATUS s_ucCardExist;

	HBOOL bAutoDeactiveFlag;
	HBOOL bInitialized;
} DI_SmartcardInstance;

/*******************************************************************/
/************************ global variables *****************************/
/*******************************************************************/

/*******************************************************************/
/************************ static variables ******************************/
/*******************************************************************/
static DI_SmartcardInstance   stSmartcard[NUM_SMARTCARD];
static unsigned char s_ucScModuleInit[NUM_SMARTCARD] = {FALSE};

static int F[16] = { 372, 372, 558, 744, 1116, 1488, 1860, -1, -1, 512, 768, 1024, 1536, 2048, -1, -1 };
static int D[16] = { -1, 1, 2, 4, 8, 16, 32, -1, 12, 20, -1, -1, -1, -1, -1, -1 };


#if defined(CONFIG_CAS_NA) || defined(CONFIG_CAS_VE)
//20090701 :: POOH :: support NA T1
static const DI_SC_CLOCK initialClockSource = DI_SC_CLOCK_EXTERNAL_36M;
#elif defined(CONFIG_CAS_IR)
//20090701 :: POOH :: support IR T0
static const DI_SC_CLOCK initialClockSource = DI_SC_CLOCK_INTERNAL_27M;
#elif defined(CONFIG_CAS_ARIB)
//20090701 :: POOH :: support BCAS CCAS T1
static const DI_SC_CLOCK initialClockSource = DI_SC_CLOCK_EXTERNAL_24M;
#elif defined(CONFIG_CAS_T14)
//20090701 :: POOH :: support NA T14
static const DI_SC_CLOCK initialClockSource = DI_SC_CLOCK_EXTERNAL_24M;
#else
//20090701 :: POOH :: temp
static const DI_SC_CLOCK initialClockSource = DI_SC_CLOCK_INTERNAL_27M;
#endif

typedef struct
{
      unsigned char sc_clkdiv;         /* clk divider in SC_CLK_CMD register */
      unsigned char sc_prescale;    /* prescale in SC_PRESCALE register */
      unsigned char sc_bauddiv;     /* baud rate divider in SC_CLK_CMD register */
      unsigned char sc_etuclkdiv;   /* etu clk divider in SC_CLK_CMD register */
} DI_SC_DF;

static const DI_SC_DF BSCD_36M_DF[10][14] = {
	/* D = 0 */
	{{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
	{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0},
	{0,0,0,0}, {0,0,0,0}},

	/* D = 1 */
	{{0x01,0x0B,0x1F,0x08}, {0x01,0x0B,0x1F,0x08}, {0x01,0x11,0x1F,0x08},
	{0x01,0x17,0x1F,0x08}, {0x01,0x23,0x1F,0x08}, {0x01,0x2F,0x1F,0x08},
	{0x01,0x3B,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	{0x01,0x0F,0x20,0x08}, {0x01,0x17,0x20,0x08}, {0x01,0x1F,0x20,0x08},
	{0x01,0x2F,0x20,0x08}, {0x01,0x3F,0x20,0x08}},

	/* D = 2 */
	{{0x01,0x05,0x1F,0x08}, {0x01,0x05,0x1F,0x08}, {0x01,0x08,0x1F,0x08},
	{0x01,0x0B,0x1F,0x04}, {0x01,0x11,0x1F,0x08}, {0x01,0x17,0x1F,0x08},
	{0x01,0x1D,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	{0x01,0x07,0x20,0x08}, {0x01,0x0B,0x20,0x08}, {0x01,0x0F,0x20,0x08},
	{0x01,0x17,0x20,0x08}, {0x01,0x1F,0x20,0x08}},

	/* D = 3 */
	{{0x01,0x02,0x1F,0x08}, {0x01,0x02,0x1F,0x08}, {0x02,0x08,0x1F,0x04},
	{0x01,0x05,0x1F,0x08}, {0x01,0x08,0x1F,0x08}, {0x01,0x0B,0x1F,0x08},
	{0x01,0x0E,0x1F,0x08}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	{0x01,0x03,0x20,0x08}, {0x01,0x05,0x20,0x08}, {0x01,0x07,0x20,0x08},
	{0x01,0x0B,0x20,0x08}, {0x01,0x0F,0x20,0x08}},

	/* D = 4 */
	{{0x02,0x02,0x1F,0x04}, {0x02,0x02,0x1F,0x04}, {0x04,0x08,0x1F,0x02},
	{0x01,0x02,0x1F,0x08}, {0x02,0x08,0x1F,0x04}, {0x01,0x05,0x1F,0x08},
	{0x02,0x0E,0x1F,0x04}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	{0x01,0x01,0x20,0x08}, {0x01,0x02,0x20,0x08}, {0x01,0x03,0x20,0x08},
	{0x01,0x05,0x20,0x08}, {0x01,0x07,0x20,0x08}},

	/* D = 5 */
	{{0x04,0x02,0x1F,0x02}, {0x04,0x02,0x1F,0x02}, {0x08,0x08,0x1F,0x01},
	{0x02,0x02,0x1F,0x04}, {0x04,0x08,0x1F,0x02}, {0x01,0x02,0x1F,0x08},
	{0x04,0x0E,0x1F,0x02}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	{0x02,0x01,0x20,0x04}, {0x02,0x02,0x20,0x04}, {0x01,0x01,0x20,0x08},
	{0x01,0x02,0x20,0x08}, {0x01,0x03,0x20,0x08}},

	/* D = 6 */
	{{0x08,0x02,0x1F,0x01}, {0x08,0x02,0x1F,0x01}, {0x10,0x08,0x1F,0x01},
	{0x04,0x02,0x1F,0x02}, {0x08,0x08,0x1F,0x01}, {0x04,0x05,0x1F,0x02},
	{0x08,0x0E,0x1F,0x01}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	{0x04,0x01,0x20,0x02}, {0x04,0x02,0x20,0x02}, {0x02,0x01,0x20,0x04},
	{0x02,0x02,0x20,0x04}, {0x01,0x01,0x20,0x08}},

	/* D = 7 */
	{{0x10,0x02,0x1F,0x01}, {0x10,0x02,0x1F,0x01}, {0x20,0x08,0x1F,0x01},
	 {0x08,0x02,0x1F,0x01}, {0x10,0x08,0x1F,0x01}, {0x04,0x02,0x1F,0x02},
	 {0x10,0x0E,0x1F,0x01}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	 {0x08,0x01,0x20,0x01}, {0x08,0x02,0x20,0x01}, {0x04,0x01,0x20,0x02},
	 {0x04,0x02,0x20,0x02}, {0x02,0x01,0x20,0x04}},

	/* D = 8 */
	{{0x02,0x01,0x1F,0x04}, {0x02,0x01,0x1F,0x04}, {0x02,0x02,0x1F,0x04},
	{0x02,0x03,0x1F,0x04}, {0x02,0x05,0x1F,0x04}, {0x02,0x07,0x1F,0x04},
	{0x02,0x09,0x1F,0x04}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	{0x03,0x03,0x20,0x03}, {0x02,0x03,0x20,0x04}, {0x03,0x07,0x20,0x03},
	{0x02,0x07,0x20,0x04}, {0x03,0x0F,0x20,0x03}},

	/* D = 9 */
	{{0x05,0x02,0x1F,0x02}, {0x05,0x02,0x1F,0x02}, {0x0A,0x08,0x1F,0x01},
	{0x05,0x05,0x1F,0x02}, {0x05,0x08,0x1F,0x02}, {0x05,0x0B,0x1F,0x02},
	{0x02,0x05,0x1F,0x04}, {0x00,0x00,0x00,0x00}, {0x00,0x00,0x00,0x00},
	{0x05,0x03,0x20,0x02}, {0x05,0x05,0x20,0x02}, {0x05,0x07,0x20,0x02},
	{0x05,0x0B,0x20,0x02}, {0x05,0x0F,0x20,0x02}}

};
static void DRV_SC_GetParamFor36Mhz_Priv(unsigned char ucFi, unsigned char ucDi,
								unsigned char *ucScClkDiv, unsigned char * unPrescale,
								unsigned char * ucBaudDiv, unsigned char  *ucEtuClkDiv)
{
	*ucScClkDiv = BSCD_36M_DF[ucDi][ucFi].sc_clkdiv;
	*unPrescale = BSCD_36M_DF[ucDi][ucFi].sc_prescale;
	*ucBaudDiv = BSCD_36M_DF[ucDi][ucFi].sc_bauddiv;
	*ucEtuClkDiv = BSCD_36M_DF[ucDi][ucFi].sc_etuclkdiv;
}


/*******************************************************************/
/*********************** extern function prototypes ***********************/
/*******************************************************************/


/*******************************************************************/
/*********************** Static function prototypes ***********************/
/*******************************************************************/

static DI_SmartcardInstance *P_SC_GetInstance(HUINT32 unDeviceId)
{
	return &stSmartcard[unDeviceId];
}

static void P_SC_InsertCallback(void *context, int param)
{
	DI_SmartcardInstance *pSmartcardInstance;
	pfnDI_SC_EVT_CALLBACK pfnCallback;
	int returnCode = VK_OK;

	HUINT32	unSCId;

	BSTD_UNUSED(context);

	PrintEnter();

	unSCId = (HUINT32)param;

	if(unSCId >= NUM_SMARTCARD)
	{
		return;
	}

	pSmartcardInstance = P_SC_GetInstance(unSCId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintDebug("[P_SC_InsertCallback] NOT Initialized : unSCId(%x)\n\r", unSCId);
		return;

	}

	returnCode = VK_SEM_Get(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[P_SC_InsertCallback] ERROR: VK_SEM_Get \r\n");
		return;
	}

	if (pSmartcardInstance->s_ucCardExist == DI_SC_PRESENT)
	{
		PrintDebug("[P_SC_InsertCallback] (pSmartcardInstance->s_ucCardExist == DI_SC_PRESENT) \r\n");
		goto ERR;
	}
	pSmartcardInstance->s_ucCardExist = DI_SC_PRESENT;
	pfnCallback = pSmartcardInstance->pstSmartcardCallback.pfnCallback;

	returnCode = VK_SEM_Release(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[P_SC_InsertCallback] ERROR: VK_SEM_Release \r\n");
		return;
	}

	if(pfnCallback != NULL)
	{
#if defined(ENABLE_SC_MINIMUN_DEBUG)
		PrintError("[01;31m[P_SC_InsertCallback] [ %d ] Call callback function[00m\n\r",unSCId);
#else
		PrintDebug("[01;31m[P_SC_InsertCallback] [ %d ] Call callback function[00m\n\r",unSCId);
#endif
		pfnCallback(unSCId, DI_SC_PRESENT);
	}
	else
	{
#if defined(ENABLE_SC_MINIMUN_DEBUG)
		PrintError("[01;31m[P_SC_InsertCallback] [ %d ] callback is NULL[00m\n\r",unSCId);
#else
		PrintDebug("[01;31m[P_SC_InsertCallback] [ %d ] callback is NULL[00m\n\r",unSCId);
#endif

	}

	PrintExit();

	return;

ERR:
	returnCode = VK_SEM_Release(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[P_SC_InsertCallback] ERROR: VK_SEM_Release \r\n");
		return;
	}

	PrintExit();

	return;
}

// TODO.......................................................................................................................
static void P_SC_RemoveCallback(void *context, int param)
{
	DI_SmartcardInstance *pSmartcardInstance;
	pfnDI_SC_EVT_CALLBACK pfnCallback;
	int returnCode = VK_OK;

	HUINT32	unSCId;

	BSTD_UNUSED(context);

	PrintEnter();

	unSCId = (HUINT32)param;

	if(unSCId >= NUM_SMARTCARD)
	{
		return;
	}

	pSmartcardInstance = P_SC_GetInstance(unSCId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintDebug("[P_SC_RemoveCallback] NOT Initialized : unSCId(%x)\n\r", unSCId);
		return;
	}

	returnCode = VK_SEM_Get(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[P_SC_RemoveCallback] ERROR: VK_SEM_Get \r\n");
		return;
	}
	if (pSmartcardInstance->s_ucCardExist == DI_SC_NOT_PRESENT)
	{
		PrintDebug("[P_SC_InsertCallback] (pSmartcardInstance->s_ucCardExist == DI_SC_NOT_PRESENT) \r\n");
		goto ERR;
	}

	pSmartcardInstance->s_ucCardExist = DI_SC_NOT_PRESENT;
	pfnCallback = pSmartcardInstance->pstSmartcardCallback.pfnCallback;

	returnCode = VK_SEM_Release(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[P_SC_RemoveCallback] ERROR: VK_SEM_Release \r\n");
		return;
	}

	if(pfnCallback != NULL)
	{
#if defined(ENABLE_SC_MINIMUN_DEBUG)
		PrintError("[01;31m[P_SC_RemoveCallback] [ %d ] Call callback function[00m\n\r",unSCId);
#else
		PrintDebug("[01;31m[P_SC_RemoveCallback] [ %d ] Call callback function[00m\n\r",unSCId);
#endif
		pfnCallback(unSCId, DI_SC_NOT_PRESENT);
	}
	else
	{
#if defined(ENABLE_SC_MINIMUN_DEBUG)
		PrintError("[01;31m[P_SC_RemoveCallback] [ %d ] callback is NULL[00m\n\r",unSCId);
#else
		PrintDebug("[01;31m[P_SC_RemoveCallback] [ %d ] callback is NULL[00m\n\r",unSCId);
#endif
	}

	PrintExit();

	return;

ERR:
	returnCode = VK_SEM_Release(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[P_SC_RemoveCallback] ERROR: VK_SEM_Release \r\n");
		return;
	}
	PrintExit();

	return;
}

#if (NEXUS_VERSION >= 1200)
static void P_SC_CardCallback(void *context, int param)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    DI_SmartcardInstance *pSmartcardInstance;
    NEXUS_SmartcardHandle smartcard;
    HUINT32 unSCId;
    NEXUS_SmartcardStatus status;

    BSTD_UNUSED(context);

    unSCId = (HUINT32)param;
    pSmartcardInstance = P_SC_GetInstance(unSCId);
    smartcard = pSmartcardInstance->hSmartcard;

    if( pSmartcardInstance->bInitialized == FALSE )
    {
        PrintDebug("[P_SC_CardCallback] NOT Initialized : unSCId(%x)\n\r", unSCId);
        return;
    }

    rc = NEXUS_Smartcard_GetStatus(smartcard, &status);
    if (rc != NEXUS_SUCCESS)
    {
		PrintError("[P_SC_CardCallback] ERROR: NEXUS_Smartcard_GetStatus (%08X)\n", rc);
		return;
    }

    if (status.cardPresent == false)
    {
		P_SC_RemoveCallback(NULL, (int)unSCId);
    }
    else
    {
		P_SC_InsertCallback(NULL, (int)unSCId);
    }
}
#endif

#if defined(CONFIG_CAS_ARIB) || defined(CONFIG_SMARTCARD_VCC_CONTROL_GPIO)
static HBOOL P_SC_SetVccLevel(HUINT32 unSlotId,P_SC_VccLevel in_vccLevel)
{
	DRV_Error	nRet = DRV_OK;
	GPIO_ID_t gpio_id;

	PrintEnter("[P_SC_SetVccLevel]\n\r");

	if(unSlotId >= NUM_SMARTCARD)
	{
		PrintError("Invalid DeviceID(%d)\r\n", unSlotId);
		return DI_ERR_INVALID_PARAM;
	}

	gpio_id = unSlotId ? GPIO_ID_SC1_PWR_SEL : GPIO_ID_SC0_PWR_SEL ;

	switch (in_vccLevel)
	{
		case DI_SC_VccLevel_e3V:
			nRet = DRV_GPIO_Write(gpio_id, GPIO_LOW);
			if(nRet != DRV_OK)
			{
				PrintError("DRV_GPIO_Write() returns error\n");
				return FALSE;
			}
			break;

		case DI_SC_VccLevel_e5V:
			nRet = DRV_GPIO_Write(gpio_id, GPIO_HI);
			if(nRet != DRV_OK)
			{
				PrintError("DRV_GPIO_Write() returns error\n");
				return FALSE;
			}
			break;

		default:
			PrintError("Invalid in_vccLevel = %d\n",in_vccLevel);
			return FALSE;
			break;
	}
	PrintExit("[P_SC_SetVccLevel]");

	return	TRUE;
}
#endif


void DRV_SC_ReInit(void)
{
	DI_SmartcardInstance *pSmartcardInstance;
	HUINT32 ulDeviceId = 0;

	for(ulDeviceId = 0; ulDeviceId < NUM_SMARTCARD; ulDeviceId++)
	{
		pSmartcardInstance = P_SC_GetInstance(ulDeviceId);
		if( pSmartcardInstance->bInitialized != FALSE )
		{
			PrintDebug("[DRV_SC_ReInit] instanceId: %d, s_ucCardExist: %d\n",ulDeviceId, pSmartcardInstance->s_ucCardExist);
			pSmartcardInstance->s_ucCardExist = DI_SC_UNKNOWN;
		}
	}
}


DRV_Error DRV_SC_Init(HUINT32 unDeviceId)
{
	DI_SmartcardInstance *pSmartcardInstance;
	NEXUS_SmartcardSettings smartcardSettings;
	NEXUS_SmartcardStatus CardStatus;
	HUINT32 instanceId = unDeviceId;	//software slot number.

#if defined(CONFIG_CAS_ARIB)
	DRV_Error	nRet	= DRV_OK;
#endif
	NEXUS_Error	rc = NEXUS_SUCCESS;
	char semName[4] = {'S','C','0',0};
	int returnCode = VK_OK;

	PrintEnter("[ %d ]",unDeviceId);

	if((unDeviceId >= NUM_SMARTCARD) ||( unDeviceId >= NEXUS_NUM_SMARTCARD_CHANNELS))
	{
#if defined(CONFIG_SMARTCARD_SLOT1) // Only use slot1. Don.t support multi smartcard.
		if(unDeviceId == NUM_SMARTCARD)
		{
			PrintDebug("[DRV_SC_Init] unDeviceId = %d\r\n",unDeviceId);
			instanceId = 0; // Change software slot number.
		}
		else
		{
			return DRV_ERR_INVALID_PARAMETER;
		}
#else
		return DRV_ERR_INVALID_PARAMETER;
#endif
	}
	PrintDebug("[DRV_SC_Init] instanceId = %d\r\n",instanceId);


	pSmartcardInstance = P_SC_GetInstance(instanceId);

	pSmartcardInstance->slotId = unDeviceId; //Save hardware slot number.

	if( pSmartcardInstance->bInitialized != FALSE )
	{
		PrintError("[DRV_SC_Init] Initialized : unDeviceId(%x) instanceId(%x)\n\r", pSmartcardInstance->slotId,instanceId);
		return DRV_ERR;
	}

	if (pSmartcardInstance->hSmartcard)
	{
		/* already open */
		return DRV_ERR;
	}

	if ( s_ucScModuleInit[instanceId] != TRUE)
	{
		s_ucScModuleInit[instanceId] = TRUE;
	}
	pSmartcardInstance->stDrvScInfo.nClockSourceType = initialClockSource;
	PrintError("[DRV_SC_Init] Clock type = %d\r\n",pSmartcardInstance->stDrvScInfo.nClockSourceType);

	semName[2] = instanceId;

	returnCode = VK_SEM_Create(&pSmartcardInstance->semId, semName, DEFAULT_SUSPEND_TYPE);
	if(returnCode != VK_OK)
	{
		return DRV_ERR;
	}
	pSmartcardInstance->state = DI_SC_STATE_Unknown;

	NEXUS_Smartcard_GetDefaultSettings(&smartcardSettings);
	smartcardSettings.standard = NEXUS_SmartcardStandard_eIso;
	smartcardSettings.protocol = NEXUS_SmartcardProtocol_eT0;
#if (NEXUS_VERSION < 1200)
	smartcardSettings.insertCard.callback = P_SC_InsertCallback;
	smartcardSettings.insertCard.context = NULL;
	smartcardSettings.insertCard.param = (int)instanceId;
	smartcardSettings.removeCard.callback = P_SC_RemoveCallback;
	smartcardSettings.removeCard.context = NULL;
	smartcardSettings.removeCard.param = (int)instanceId;
#else
	smartcardSettings.cardCallback.callback = P_SC_CardCallback;
	smartcardSettings.cardCallback.context = NULL;
	smartcardSettings.cardCallback.param = (int)instanceId;
#endif
#if defined(CONFIG_CAS_ARIB) || defined(CONFIG_SMARTCARD_VCC_CONTROL_GPIO)
	smartcardSettings.setVcc = false; 	/* If true, modify Vcc value */
#else
	smartcardSettings.setVcc = TRUE; 	/* If true, modify Vcc value */
	smartcardSettings.vcc = 0; /* 0 = 5V(default), 1 = 3V */
#endif
	smartcardSettings.sourceClockFreq = 27000000; /* 0 = 5V(default), 1 = 3V */
#if (NEXUS_VERSION >= 1304)
	smartcardSettings.setPinmux = false;
#endif

/*
 * 20091008 :: POOH :: all projects don't use autoDeactiveReq
 *			This is main mistaken point.
 *			Specially, NA had become many temporary correct points.
 */
#if 0	//#if !(defined(CONFIG_CAS_NA))
	smartcardSettings.autoDeactiveReq = true;
	pSmartcardInstance->bAutoDeactiveFlag = true;
#else
	pSmartcardInstance->bAutoDeactiveFlag = false;
#endif

	smartcardSettings.isPresHigh = true;
	pSmartcardInstance->hSmartcard = NEXUS_Smartcard_Open(pSmartcardInstance->slotId, &smartcardSettings);
	if ( pSmartcardInstance->hSmartcard == NULL )
	{
		PrintExit("ERROR");
		VK_SEM_Destroy(pSmartcardInstance->semId);
		return DRV_ERR;
	}

	pSmartcardInstance->bInitialized = TRUE;
	pSmartcardInstance->state = DI_SC_STATE_Initialized;
#if defined(CONFIG_CAS_ARIB)
	if(pSmartcardInstance->slotId == 0)
	{
		nRet = DRV_GPIO_Write(GPIO_ID_SC0_DIV, GPIO_LOW);
		if(nRet != DRV_OK)
		{
			PrintError("DRV_GPIO_Write() returns error\n");
			/*return DI_ERR_ERROR;*/
		}
	}
	else //if(unDeviceId == 1)
	{
		nRet = DRV_GPIO_Write(GPIO_ID_SC1_DIV, GPIO_LOW);
		if(nRet != DRV_OK)
		{
			PrintError("DRV_GPIO_Write() returns error\n");
			/*return DI_ERR_ERROR;*/
		}
	}
#endif
#if defined(CONFIG_CAS_ARIB) || defined(CONFIG_SMARTCARD_VCC_CONTROL_GPIO)
	//default value
	P_SC_SetVccLevel(pSmartcardInstance->slotId,P_SC_VccLevel_e5V);
#endif


#if 0
	/* This may error if no card is present, the error may be safely ignored.  It is required to finish the interface initialization. */
	rc = NEXUS_Smartcard_Reset(pSmartcardInstance->hSmartcard, false); /*cold reset */
	if ( rc != NEXUS_SUCCESS)
	{
		PrintExit("ERROR");
		return DRV_ERR;
	}


//	NEXUS_Smartcard_DetectCard(pSmartcardInstance->hSmartcard);
#endif
	rc = NEXUS_Smartcard_GetStatus(pSmartcardInstance->hSmartcard, &CardStatus);
	if ( rc != NEXUS_SUCCESS)
	{
		PrintExit("ERROR");
		return DRV_ERR;
	}
	PrintError("[DRV_SC_Init] [ %d ] cardPresent : %d state : %d\n\r",
		instanceId,CardStatus.cardPresent,CardStatus.state);

	pSmartcardInstance->s_ucCardExist = CardStatus.cardPresent;

#if defined(CONFIG_BRCM_MOD_NEXUS_POWER_MANAGEMENT)
	DRV_PM_RegisterCallback(DRV_SC_ReInit, NULL);
#endif

	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_SC_UnInit(HUINT32 unDeviceId)
{
	DI_SmartcardInstance *pSmartcardInstance;

	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	NEXUS_Smartcard_Close(pSmartcardInstance->hSmartcard);

	VK_SEM_Destroy(pSmartcardInstance->semId);

	pSmartcardInstance->bInitialized = FALSE;

	return DRV_OK;
}

static DRV_Error DRV_SC_GetSettingValue(unsigned char ucFI, unsigned char ucDI,
							unsigned char *ucScClkDiv, unsigned char *ucPrescale, unsigned char *ucBaudDiv )
{
	if ((F[ucFI] <0) ||(D[ucDI] <0 ))
	{
		*ucScClkDiv = 0;
		*ucPrescale = 0;
		*ucBaudDiv = 0;
		PrintError("[DRV_SC_GetSettingValue] Wrong ucFI (%d) or ucDI(%d) \n", ucFI, ucDI);
		return	DRV_ERR_INVALID_PARAMETER;
	}

	/* get ucBaudDiv value */
	if (ucFI > 8)
		*ucBaudDiv = 0x20;	//32
	else
		*ucBaudDiv = 0x1F;	//31

	/* get ucScClkDiv value */
	if (ucDI ==1 || ucDI ==2)
		*ucScClkDiv = 1;
	else if (ucDI == 3)
	{
		if (ucFI == 2)
			*ucScClkDiv = 2;
		else
			*ucScClkDiv = 1;
	}
	else if (ucDI == 4)
	{
		if (ucFI == 2)
			*ucScClkDiv = 4;
		else if (ucFI == 0 || ucFI == 1 || ucFI == 4 || ucFI == 6)
			*ucScClkDiv = 2;
		else
			*ucScClkDiv = 1;
	}
	else if (ucDI == 5)
	{
		switch (ucFI)
		{
			case	0:
			case	1:
			case	4:
			case	6:				*ucScClkDiv = 4;				break;
			case	2:				*ucScClkDiv = 8;				break;
			case	3:
			case	9:
			case	10:			*ucScClkDiv = 2;				break;
			default :				*ucScClkDiv = 1;				break;	//5, 11, 12, 13
		}
	}
	else if (ucDI == 6)
	{
		switch (ucFI)
		{
			case	0:
			case	1:
			case	4:
			case	6:				*ucScClkDiv = 8;				break;
			case	2:				*ucScClkDiv = 16;				break;
			case	3:
			case	9:
			case	10:			*ucScClkDiv = 4;				break;
			case	5:
			case	11:
			case	12:			*ucScClkDiv = 2;				break;
			default :				*ucScClkDiv = 1;				break;	//13
		}
	}
	else if (ucDI == 8)
	{
		switch (ucFI)
		{
			case	9:
			case	11:
			case	13:			*ucScClkDiv = 3;				break;
			default :				*ucScClkDiv = 2;				break;	//0, 1, 2, 3, 4, 5, 6, 10, 12
		}
	}
	else if (ucDI == 9)
	{
		switch (ucFI)
		{
			case	2:				*ucScClkDiv = 10;				break;
			case	6:				*ucScClkDiv = 2;				break;
			default :				*ucScClkDiv = 5;				break;	//0, 1, 3, 4, 5, 9, 10, 11, 12, 13
		}
	}
	else
	{
		*ucScClkDiv = 0;
		*ucPrescale = 0;
		*ucBaudDiv = 0;
		PrintError("[DRV_SC_GetSettingValue] get ucScClkDiv value: Wrong ucFI(%d), ucDI(%d)\n", ucFI, ucDI);
		return	DRV_ERR;
	}

	/* get ucPrescale value */
	if (ucFI==0 || ucFI==1)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x0B;				break;
			case	2:				*ucPrescale = 0x05;				break;
			case	8:				*ucPrescale = 0x01;				break;
			default :				*ucPrescale = 0x02;				break;	// 3, 4, 5, 6, 9
		}
	}
	else if (ucFI==2)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x11;				break;
			case	8:				*ucPrescale = 0x02;				break;
			default :				*ucPrescale = 0x08;				break;	// 2, 3, 4, 5, 6, 9
		}
	}
	else if (ucFI==3)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x17;				break;
			case	2:				*ucPrescale = 0x0B;				break;
			case	3:
			case	9:				*ucPrescale = 0x05;				break;
			case	8:				*ucPrescale = 0x03;				break;
			default :				*ucPrescale = 0x02;				break;		// 4, 5, 6
		}
	}
	else if (ucFI==4)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x23;				break;
			case	2:				*ucPrescale = 0x11;				break;
			case	8:				*ucPrescale = 0x05;				break;
			default :				*ucPrescale = 0x08;				break;	// 3, 4, 5, 6, 9
		}
	}
	else if (ucFI==5)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x2F;				break;
			case	2:				*ucPrescale = 0x17;				break;
			case	3:
			case	9:				*ucPrescale = 0x0B;				break;
			case	4:				*ucPrescale = 0x05;				break;
			case	8:				*ucPrescale = 0x07;				break;
			default :				*ucPrescale = 0x02;				break;		// 5, 6
		}
	}
	else if (ucFI==6)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x3B;				break;
			case	2:				*ucPrescale = 0x1D;				break;
			case	8:				*ucPrescale = 0x09;				break;
			case	9:				*ucPrescale = 0x05;				break;
			default :				*ucPrescale = 0x0E;				break;		// 3, 4, 5, 6
		}
	}
	else if (ucFI==9)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x0F;				break;
			case	2:				*ucPrescale = 0x07;				break;
			case	3:
			case	8:
			case	9:				*ucPrescale = 0x03;				break;
			default :				*ucPrescale = 0x01;				break;	// 4, 5, 6
		}
	}
	else if (ucFI==10)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x17;				break;
			case	2:				*ucPrescale = 0x0B;				break;
			case	3:
			case	9:				*ucPrescale = 0x05;				break;
			case	8:				*ucPrescale = 0x03;				break;
			default :				*ucPrescale = 0x02;				break;	// 4, 5, 6
		}
	}
	else if (ucFI==11)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x1F;				break;
			case	2:				*ucPrescale = 0x0F;				break;
			case	3:
			case	8:
			case	9:				*ucPrescale = 0x07;				break;
			case	4:				*ucPrescale = 0x03;				break;
			default :				*ucPrescale = 0x01;				break;	// 5, 6
		}
	}
	else if (ucFI==12)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x2F;				break;
			case	2:				*ucPrescale = 0x17;				break;
			case	3:
			case	9:				*ucPrescale = 0x0B;				break;
			case	4:				*ucPrescale = 0x05;				break;
			case	8:				*ucPrescale = 0x07;				break;
			default :				*ucPrescale = 0x02;				break;	// 5, 6
		}
	}
	else if (ucFI==13)
	{
		switch (ucDI)
		{
			case	1:				*ucPrescale = 0x3F;				break;
			case	2:				*ucPrescale = 0x1F;				break;
			case	4:				*ucPrescale = 0x07;				break;
			case	5:				*ucPrescale = 0x03;				break;
			case	6:				*ucPrescale = 0x01;				break;
			default :				*ucPrescale = 0x0F;				break;	// 3, 8, 9
		}
	}
	else
	{
		*ucScClkDiv = 0;
		*ucPrescale = 0;
		*ucBaudDiv = 0;
		PrintError("[DRV_SC_GetSettingValue] get ucPrescale value : Wrong ucFI(%d), ucDI(%d)\n", ucFI, ucDI);
		return	DRV_ERR;
	}

	PrintDebug("[DRV_SC_GetSettingValue] ucFI:%d, ucDI:%d, ucScClkDiv:%d, ucPrescale:%d, ucBaudDiv:%d\r\n",
				ucFI, ucDI, *ucScClkDiv, *ucPrescale, *ucBaudDiv);

	return DRV_OK ;
}

DI_ERR_CODE DI_SC_GetCapability(HUINT32  *pNumDevice)
{
	*pNumDevice = NUM_SMARTCARD;
	return	DI_ERR_OK;
}

DI_ERR_CODE DI_SC_SetVccLevel(HUINT32 unDeviceId,DI_SC_VccLevel in_vccLevel)
{
	DI_SmartcardInstance *pSmartcardInstance;

#if defined(CONFIG_CAS_ARIB) || defined(CONFIG_SMARTCARD_VCC_CONTROL_GPIO)
	HBOOL bRet = TRUE;
#endif

	HAPPY(in_vccLevel);
	PrintEnter("[DI_SC_SetVccLevel]\n\r");

	if(unDeviceId >= NUM_SMARTCARD)
	{
		PrintError("Invalid DeviceID(%d)\r\n", unDeviceId);
		return DI_ERR_INVALID_PARAM;
	}

	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_SetVccLevel] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_CAS_ARIB) || defined(CONFIG_SMARTCARD_VCC_CONTROL_GPIO)
	switch (in_vccLevel)
	{
		case DI_SC_VccLevel_e3V:
			bRet = P_SC_SetVccLevel(pSmartcardInstance->slotId,P_SC_VccLevel_e3V);
			if(bRet != TRUE)
			{
				PrintError("DRV_GPIO_Write() returns error\n");
				return DI_ERR_ERROR;
			}

			break;

		case DI_SC_VccLevel_e5V:
			bRet = P_SC_SetVccLevel(pSmartcardInstance->slotId,P_SC_VccLevel_e5V);
			if(bRet != TRUE)
			{
				PrintError("DRV_GPIO_Write() returns error\n");
				return DI_ERR_ERROR;
			}
			break;

		default:
			PrintError("Invalid in_vccLevel = %d\n",in_vccLevel);
			return DI_ERR_ERROR;
			break;
	}
#endif
	PrintExit("[DI_SC_SetVccLevel]");

	return	DI_ERR_OK;
}




DI_ERR_CODE DI_SC_DownUpVCC(HUINT32 unDeviceId, unsigned int unDownTime /* ms */)
{
	NEXUS_Error rc = NEXUS_SUCCESS;
	DI_SmartcardInstance *pSmartcardInstance;

	PrintEnter("[DI_SC_DownUpVCC]\n\r");

	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}
	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_DownUpVCC] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

#if 1
	BSTD_UNUSED(unDownTime);

	rc = NEXUS_Smartcard_Reset(pSmartcardInstance->hSmartcard,false );	/*cold reset */
	if ( rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_DownUpVCC] BSCD_Channel_ResetIFD - FAILED: rc (%#lx)\r\n", rc);
		return DI_ERR_ERROR;
	}
#else
	rc =NEXUS_Smartcard_PowerICC(pSmartcardInstance->hSmartcard,unDownTime);
	if ( rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_DownUpVCC] ERROR: NEXUS_Smartcard_PowerICC \r\n");
		return	DI_ERR_ERROR;
	}
#endif
	PrintExit("[DI_SC_DownUpVCC]");

	return	DI_ERR_OK;
}

DI_ERR_CODE DI_SC_Deactivate( HUINT32 unDeviceId )
{
	NEXUS_Error rc = NEXUS_SUCCESS;
	DI_SmartcardInstance *pSmartcardInstance;

	PrintEnter("[DI_SC_Deactivate]\n\r");

	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}
	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_Deactivate] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

	rc =NEXUS_Smartcard_Deactivate(pSmartcardInstance->hSmartcard);
	if ( rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_Deactivate] ERROR: NEXUS_Smartcard_Deactivate \r\n");
		return	DI_ERR_ERROR;
	}

	PrintExit("[DI_SC_Deactivate]");

	return	DI_ERR_OK;
}

DI_ERR_CODE DI_SC_NegotiatePTS( HUINT32 unDeviceId,
						unsigned char *pucWriteBuf, int ucNumToWrite,
						unsigned char *pucReadBuf, int *pucNumRead )
{
	NEXUS_Error	rc = NEXUS_SUCCESS;
	DI_ERR_CODE errorCode = DI_ERR_OK;
	int returnCode = VK_OK;
	DI_SmartcardInstance *pSmartcardInstance;
	NEXUS_SmartcardStatus CardStatus;

	unsigned int uNumDataWritten;
	unsigned int uNumDataRead;

	PrintEnter("[DI_SC_NegotiatePTS]\n\r");

	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}

	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_NegotiatePTS] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

	rc = NEXUS_Smartcard_GetStatus(pSmartcardInstance->hSmartcard, &CardStatus);

	if(rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_NegotiatePTS] NEXUS_Smartcard_GetStatus - FAILED : rc:(%#lx)\r\n", rc);
		return DI_ERR_ERROR;
	}

	if( CardStatus.cardPresent != TRUE )
	{
		PrintError("[DI_SC_NegotiatePTS] SmartCard is not present in ucDeviceId(0x%x)\r\n", unDeviceId);
		*pucNumRead = (unsigned int)0;
		return DI_ERR_ERROR;
	}

	returnCode = VK_SEM_Get(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[DI_SC_NegotiatePTS] ERROR: VK_SEM_Get \r\n");
		*pucNumRead = (unsigned int)(unsigned int)0;
		return DI_ERR_ERROR;
	}

	rc = NEXUS_Smartcard_Write(pSmartcardInstance->hSmartcard, pucWriteBuf, (unsigned long)ucNumToWrite, &uNumDataWritten);
	if ( rc != NEXUS_SUCCESS)
	{
		errorCode = DI_ERR_ERROR;
		PrintError("[DI_SC_NegotiatePTS] faulure to send \r\n");
		goto RTN;
	}
	PrintDebug("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);

	rc = NEXUS_Smartcard_Read(pSmartcardInstance->hSmartcard, pucReadBuf, (unsigned long)ucNumToWrite, &uNumDataRead);
	if ( rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_NegotiatePTS] Error(0x%X) in NEXUS_Smartcard_Read\r\n", rc);
		errorCode = DI_ERR_ERROR;
		goto RTN;
	}
	PrintDebug("[01;35m[%d]Read data length : (%X)[00m\r\n", __LINE__,uNumDataRead);

RTN:
	if (errorCode != DI_ERR_OK)
		*pucNumRead = (unsigned int) 0;
	else
		*pucNumRead = (unsigned int) uNumDataRead;

	returnCode = VK_SEM_Release(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[DI_SC_NegotiatePTS] ERROR: VK_SEM_Release \r\n");
		*pucNumRead = (unsigned int)0;
		return DI_ERR_ERROR;
	}

	PrintExit("[DI_SC_NegotiatePTS]");

	return errorCode;

}

DI_ERR_CODE DI_SC_SetParams( HUINT32 unDeviceId, DI_SC_PROTOCOL nProtocol,
						unsigned long ulMinClock, unsigned long ulSrcBaudrate,
						unsigned char ucFI, unsigned char ucDI, unsigned char ucN,
						unsigned char ucCWI, unsigned char ucBWI )
{
	NEXUS_Error	rc = NEXUS_SUCCESS;
	DRV_Error	nRet	= DRV_OK;

	DI_SmartcardInstance *pSmartcardInstance;
	DI_SC_CLOCK		nClockSourceType = DI_SC_CLOCK_INTERNAL_27M;
	NEXUS_SmartcardClockSource clockSource;
	NEXUS_SmartcardSettings newNexusSettings;

	unsigned long clockFrequency;
	unsigned char ucNewScClkDiv=0, ucNewPrescale=0, ucNewBaudDiv=0;
	unsigned long ulClockFrequency = 0;
	unsigned long					ulWWT=0, ulBWT=0;

#if !defined(CONFIG_DEBUG)
	BSTD_UNUSED(ulMinClock);
#endif

	PrintEnter();

	PrintDebug("[DI_SC_SetParams] Protocol(%d), MinClock(%d), SrcBaudrate(%d), FI(%d), DI(%d), N(%d), CWI(%d), BWI(%d)\r\n",
				nProtocol, ulMinClock, ulSrcBaudrate, ucFI, ucDI,  ucN, ucCWI, ucBWI);
	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}
	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_TransferData] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}
#if 0
	if( (nProtocol != DI_T_0) && (nProtocol != DI_T_1) && (nProtocol != DI_T_14) )
	{
		PrintError("[DI_SC_SetParams] Invailid Protocol (%d)\r\n", nProtocol);
		return DI_ERR_INVALID_PARAM;
	}
#endif
	pSmartcardInstance->state = DI_SC_STATE_ReceiveDecodeATR;

	NEXUS_Smartcard_GetSettings(pSmartcardInstance->hSmartcard, &newNexusSettings);

	newNexusSettings.timeOut.value = DI_SC_DEFAULT_TIMEOUT;
	newNexusSettings.timeOut.unit = NEXUS_TimerUnit_eMilliSec;

#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
	newNexusSettings.preScale = 0;		/* Set Prescale */
	newNexusSettings.scClkDiv = 0;		/* Set SC Clock Divider */
	newNexusSettings.etuClkDiv = 0;		/* correct ETU Clock Divider */
#endif

	/* Asynchronous Protocol Types. */
	switch (nProtocol)
		{
		case DI_T_0:
		case DI_T_0_IR:
		case DI_T_0_CX:
			newNexusSettings.standard = NEXUS_SmartcardStandard_eIso;
			newNexusSettings.protocol = NEXUS_SmartcardProtocol_eT0;
#if defined(CONFIG_CAS_NA) || defined(CONFIG_CAS_VE)
			nClockSourceType = DI_SC_CLOCK_EXTERNAL_36M;
			newNexusSettings.sourceClockFreq = 36000000;
#elif defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
			nClockSourceType = DI_SC_CLOCK_EXTERNAL_24M;
			newNexusSettings.sourceClockFreq = 24000000;
#else

			nClockSourceType = DI_SC_CLOCK_INTERNAL_27M;
			newNexusSettings.sourceClockFreq = 27000000;
#endif
			clockSource = NEXUS_SmartcardClockSource_eInternalClock;
			clockFrequency = 0;

			if(nProtocol == DI_T_0_CX)
			{
				ulClockFrequency = DI_SC_CLK_INTERNAL_3857KHZ;
			}
			else
			{
				ulClockFrequency = 0;
		}

			break;

		case DI_T_1:
		case DI_T_1_NA:
		case DI_T_1_ARIB:
			newNexusSettings.standard = NEXUS_SmartcardStandard_eIso;
			newNexusSettings.protocol = NEXUS_SmartcardProtocol_eT1;
#if defined(CONFIG_CAS_ARIB)
			nClockSourceType = DI_SC_CLOCK_EXTERNAL_24M;
			clockSource = NEXUS_SmartcardClockSource_eExternalClock;
			clockFrequency = 24000000;
#else
			if(nProtocol == DI_T_1_NA)
			{
				nClockSourceType = DI_SC_CLOCK_EXTERNAL_36M;
				clockSource = NEXUS_SmartcardClockSource_eExternalClock;
				newNexusSettings.sourceClockFreq = 36000000;
				clockFrequency = 36000000;
			}
			else 	if(nProtocol == DI_T_1_ARIB)
			{
				nClockSourceType = DI_SC_CLOCK_EXTERNAL_24M;
				clockSource = NEXUS_SmartcardClockSource_eExternalClock;
				newNexusSettings.sourceClockFreq = 24000000;
				clockFrequency = 24000000;
			}
			else	//if(nProtocol == DI_T_1)
			{
				nClockSourceType = DI_SC_CLOCK_EXTERNAL_36M;
				clockSource = NEXUS_SmartcardClockSource_eExternalClock;
				newNexusSettings.sourceClockFreq = 36000000;
				clockFrequency = 36000000;
			}
#endif
			break;

		case DI_T_14:
			newNexusSettings.standard = NEXUS_SmartcardStandard_eIrdeto;
			newNexusSettings.protocol = NEXUS_SmartcardProtocol_eT14;
			nClockSourceType = DI_SC_CLOCK_EXTERNAL_24M;
			newNexusSettings.sourceClockFreq = 24000000;
			clockSource = NEXUS_SmartcardClockSource_eExternalClock;
			clockFrequency = 24000000;
			break;

		default:
			newNexusSettings.standard = NEXUS_SmartcardStandard_eIso;
			newNexusSettings.protocol = NEXUS_SmartcardProtocol_eT0;
			nClockSourceType = DI_SC_CLOCK_INTERNAL_27M;
			newNexusSettings.sourceClockFreq = 27000000;
			clockSource = NEXUS_SmartcardClockSource_eInternalClock;
			clockFrequency = 0;
			break;
	}
	PrintDebug("[01;36m[DI_SC_SetParams] Smart card protocol(%d) standard(%d)[00m\r\n", newNexusSettings.protocol , newNexusSettings.standard);
	PrintDebug("[01;36m[DI_SC_SetParams] clockSource(%d) clockFrequency(%ld)[00m\r\n", clockSource , clockFrequency);
	PrintDebug("[01;36m[DI_SC_SetParams] nClockSourceType(%d) useClockSource(%d)[00m\r\n", nClockSourceType , pSmartcardInstance->stDrvScInfo.nClockSourceType );

	if (pSmartcardInstance->stDrvScInfo.nClockSourceType  != nClockSourceType)
	{
		PrintError("[DI_SC_SetParams] Change Clock\r\n");
		pSmartcardInstance->stDrvScInfo.nClockSourceType  = nClockSourceType;
	}
	else
	{
		PrintError("[DI_SC_SetParams] Don't Change Clock\r\n");
	}

/* This is hard-coding due to PI requirements.
		This way the PI picks up the right values based on the F and D factors. */

	newNexusSettings.baudDiv= 0;

	newNexusSettings.fFactor = ucFI;		/* Set F,	Clock Rate Conversion Factor */
	newNexusSettings.dFactor = ucDI;		/* Set D,	Baud Rate Adjustor */

	newNexusSettings.extClockDivisor = 1;

	/* JHK 08.03.02 For Iredeto smart card V5.0,
	  At getting ATR It be used as 3MHz and After this it be used as 6MHz clock */
	if ( newNexusSettings.standard == NEXUS_SmartcardStandard_eIrdeto &&
		newNexusSettings.protocol == NEXUS_SmartcardProtocol_eT14)
	{
		PrintDebug("[DI_SC_SetParams] NEXUS_SmartcardStandard_eIrdeto NEXUS_SmartcardProtocol_eT14 \r\n");
		PrintDebug("[DI_SC_SetParams] pSmartcardInstance->stDrvScInfo.nClockSourceType = %d \r\n",pSmartcardInstance->stDrvScInfo.nClockSourceType);
		PrintDebug("[DI_SC_SetParams] SrcBaudrate = %d \r\n",ulSrcBaudrate);

		//nagra T14 baudrate ==  9600
		//irdeto T14 baudrate == 10000
#if 0
		/* Baudrate = 9615 */
		/*#define BSCD_T14_IRDETO_CONSTANT_CLOCK_RATE_CONV_FACTOR		624*/
				newNexusSettings.ucScClkDiv = 4;
				newNexusSettings.ucEtuClkDiv = 1;
				newNexusSettings.unPrescale = 0x4D;
				newNexusSettings.baudDiv = 0x20;
		/* Baudrate = 9868 */
		/*#define BSCD_T14_IRDETO_CONSTANT_CLOCK_RATE_CONV_FACTOR		608*/
				newNexusSettings.ucScClkDiv = 4;
				newNexusSettings.ucEtuClkDiv = 1;
				newNexusSettings.unPrescale = 0x4A;
				newNexusSettings.baudDiv = 0x20;

		/* Baudrate = 10000 */
		/*#define BSCD_T14_IRDETO_CONSTANT_CLOCK_RATE_CONV_FACTOR		600*/
				newNexusSettings.ucScClkDiv = 4;
				newNexusSettings.ucEtuClkDiv = 1;
				newNexusSettings.unPrescale = 0x4A;
				newNexusSettings.baudDiv = 0x20;
		/* Baudrate = 10135 */
		/*#define BSCD_T14_IRDETO_CONSTANT_CLOCK_RATE_CONV_FACTOR		592*/
				newNexusSettings.ucScClkDiv = 4;
				newNexusSettings.ucEtuClkDiv = 1;
				newNexusSettings.unPrescale = 0x49;
				newNexusSettings.baudDiv = 0x20;
#endif

		if ( ulSrcBaudrate == 9600 )
		{
/* Baudrate = 9615 */
/*#define BSCD_T14_IRDETO_CONSTANT_CLOCK_RATE_CONV_FACTOR		624*/
			newNexusSettings.baudDiv = 0x20;
		}
		else //if ( ulSrcBaudrate == 10000 )
		{
/* Baudrate = 10000 */
/*#define BSCD_T14_IRDETO_CONSTANT_CLOCK_RATE_CONV_FACTOR		600*/
			newNexusSettings.baudDiv = 0x20;
		}
	}
	else 	if ( newNexusSettings.standard == NEXUS_SmartcardStandard_eIso &&
		newNexusSettings.protocol == NEXUS_SmartcardProtocol_eT0)
	{
		PrintDebug("[DI_SC_SetParams] NEXUS_SmartcardStandard_eIso NEXUS_SmartcardProtocol_eT0 \r\n");

		if(pSmartcardInstance->stDrvScInfo.nClockSourceType  == DI_SC_CLOCK_EXTERNAL_36M)
		{
			unsigned char ucScClkDiv, unPrescale, ucBaudDiv, ucEtuClkDiv;

			PrintDebug("[DI_SC_SetParams] (pSmartcardInstance->stDrvScInfo.nClockSourceType  == DI_SC_CLOCK_EXTERNAL_36M) \r\n");
			DRV_SC_GetParamFor36Mhz_Priv(ucFI, ucDI, &ucScClkDiv, &unPrescale, &ucBaudDiv, &ucEtuClkDiv);
			newNexusSettings.baudDiv = ucBaudDiv;		/* Set baud divisor */
		}
		else // if(pSmartcardInstance->stDrvScInfo.nClockSourceType  == DI_SC_CLOCK_INTERNAL_27M)
		{
			PrintDebug("[DI_SC_SetParams] (pSmartcardInstance->stDrvScInfo.nClockSourceType  == DI_SC_CLOCK_INTERNAL_27M) \r\n");
			if( ulClockFrequency == DI_SC_CLK_INTERNAL_3857KHZ ) /* Conax */
			{
#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
				newNexusSettings.etuClkDiv = 6;		// 24/6 = 4Mhz
#endif
				PrintDebug("[DI_SC_SetParams] ( nClockType == DI_SC_CLK_INTERNAL_3857KHZ ) \r\n");

				nRet = DRV_SC_GetSettingValue(ucFI, ucDI, &ucNewScClkDiv, &ucNewPrescale, &ucNewBaudDiv);
				if (nRet != DRV_OK)
				{
					PrintError("[DDI_SC_SetParams]DRV_SC_GetSettingValue - FAILED errCode:(%x)\r\n", nRet);
					return DI_ERR_ERROR;
				}
				newNexusSettings.baudDiv = ucNewBaudDiv;		/* Set baud divisor */
#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
				newNexusSettings.preScale = ucNewPrescale;		/* Set Prescale */
				newNexusSettings.scClkDiv = ucNewScClkDiv;		/* Set SC Clock Divider */
				newNexusSettings.etuClkDiv = newNexusSettings.etuClkDiv / newNexusSettings.scClkDiv;		/* correct ETU Clock Divider */
#endif
			}
			else
			{
				PrintDebug("[DI_SC_SetParams] Normal Type \r\n");
				newNexusSettings.baudDiv = 0;		/* Set baud divisor */
#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
				newNexusSettings.preScale = 0;		/* Set Prescale */
				newNexusSettings.scClkDiv = 0;		/* Set SC Clock Divider */
				newNexusSettings.etuClkDiv = 0;		/* correct ETU Clock Divider */
#endif
			}
		}
	}
	else
	{
		PrintDebug("[DI_SC_SetParams] NEXUS_SmartcardStandard_eIso NEXUS_SmartcardProtocol_eT1 \r\n");
		if(pSmartcardInstance->stDrvScInfo.nClockSourceType  == DI_SC_CLOCK_EXTERNAL_36M)
		{
			unsigned char ucScClkDiv, unPrescale, ucBaudDiv, ucEtuClkDiv;

			PrintDebug("[DI_SC_SetParams] (pSmartcardInstance->stDrvScInfo.nClockSourceType  == DI_SC_CLOCK_EXTERNAL_36M) \r\n");
			DRV_SC_GetParamFor36Mhz_Priv(ucFI, ucDI, &ucScClkDiv, &unPrescale, &ucBaudDiv, &ucEtuClkDiv);
			newNexusSettings.baudDiv = ucBaudDiv;		/* Set baud divisor */
		}
		else // if(pSmartcardInstance->stDrvScInfo.nClockSourceType  == DI_SC_CLOCK_INTERNAL_27M)
		{
			PrintDebug("[DI_SC_SetParams] (pSmartcardInstance->stDrvScInfo.nClockSourceType  == DI_SC_CLOCK_INTERNAL_27M) \r\n");
			if( ulClockFrequency == DI_SC_CLK_INTERNAL_3857KHZ ) /* Conax ...but the conax protocol is T0*/
			{
				PrintDebug("[DI_SC_SetParams] ( nClockType == DI_SC_CLK_INTERNAL_3857KHZ ) \r\n");
				nRet = DRV_SC_GetSettingValue(ucFI, ucDI, &ucNewScClkDiv, &ucNewPrescale, &ucNewBaudDiv);
				if (nRet != DRV_OK)
				{
					PrintError("[DDI_SC_SetParams]DRV_SC_GetSettingValue - FAILED errCode:(%x)\r\n", nRet);
					return DI_ERR_ERROR;
				}
				newNexusSettings.baudDiv = ucNewBaudDiv;		/* Set baud divisor */
			}
			else
			{
				PrintDebug("[DI_SC_SetParams] Normal Type \r\n");
				newNexusSettings.baudDiv = 0;		/* Set baud divisor */
			}
		}
	}

	if( 	newNexusSettings.protocol == NEXUS_SmartcardProtocol_eT0
		|| newNexusSettings.protocol == NEXUS_SmartcardProtocol_eT14 )
	{
		if( ucCWI == 0 )
			ucCWI = DI_SC_DEFAULT_WORK_WAIT_TIME;

		/*	WWT = (960 x WI x D) etu*/
		ulWWT = 960*ucCWI*D[ucDI];

		PrintDebug("[01;36m[DI_SC_SetParams] Protocol T= %s CWI(%d)[00m\r\n", ((nProtocol == DI_T_14)?"14":"0"),ucCWI);

		/* Set work waiting time */
		newNexusSettings.workWaitTime.value =	ulWWT;

		newNexusSettings.workWaitTime.unit = NEXUS_TimerUnit_eEtu;
		newNexusSettings.nullFilter = true;
		PrintDebug("[01;36m[DI_SC_SetParams] Smart card WWT(%d) [00m\r\n", newNexusSettings.workWaitTime.value);

#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
		/* conax PT : SC-16.0/3.1.3F - Labwise on 20140806 */
		/* conax PT : SC-16.0/3.1.3F - Labatus on 20141103 */
		if (nProtocol == DI_T_0_CX)
		{
			newNexusSettings.timeOut.value = newNexusSettings.workWaitTime.value;
			newNexusSettings.timeOut.unit = NEXUS_TimerUnit_eEtu;
		}
#endif

		/* Set Extra Guard Time  */
		if (ucN != 255)
			newNexusSettings.extraGuardTime.value =  ucN;
		else
			newNexusSettings.extraGuardTime.value =  0;

		PrintDebug("[01;36m[DI_SC_SetParams] Smart card EGT(%d) [00m\r\n", newNexusSettings.extraGuardTime.value);
		newNexusSettings.extraGuardTime.unit = NEXUS_TimerUnit_eEtu;
	}
	else
	{
		PrintDebug("[01;36m[DI_SC_SetParams] Protocol T= %s CWI(%d), BWI(%d)[00m\r\n", ((nProtocol == DI_T_14)?"14":"1"),ucCWI, ucBWI);

		if( ucCWI >= 16 )
			ucCWI = DI_SC_DEFAULT_CHARACTER_WAIT_TIME;

		if (ucBWI >=10)
			ucBWI = DI_SC_DEFAULT_BLOCK_WAIT_TIME;

		ulBWT = 11 + (( 1 << ucBWI ) * 960 * 372 * D[ucDI] / F[ucFI] );

		/* Set character Wait time Integer */
		newNexusSettings.characterWaitTime =  ucCWI ;

		/* Set block Wait time */
		newNexusSettings.blockWaitTime.value =  ulBWT ;
		newNexusSettings.blockWaitTime.unit = NEXUS_TimerUnit_eEtu;

		PrintDebug("[01;36m[DI_SC_SetParams] Smart card BWT(%d) [00m\r\n", newNexusSettings.blockWaitTime.value);

		// 20090806 :: POOH :: Satisfy Nagra GuardTime Spec
		// SCA_SC_TGUARD-SMART CARD TRANSMIT GUARD TIME REGISTER
		// 7816-3 : minimum value for T1 is 11 etu. 11-10(data lenth) = 1(guard etu)
		// NAGRA  : minimum guard time value for T0 T1 is 1.5 etu.

		// original  value = 12etu -1 = 11etu (7816-3 spec)
		// changed value = 12etu +1 = 13 etu (increase 3 etu)

		// set 0			  = 1
		// set 1-254	   = setting value
		// set 255			 = 1

		/* Set Extra Guard Time  */
#if defined(CONFIG_CAS_NA)
#if defined(CONFIG_CAS_NA_CAK7)
#if defined(CONFIG_CAS_NA_ISO7816)
	newNexusSettings.timeOut.value = DI_SC_T1_DATA_TIMEOUT;
	newNexusSettings.timeOut.unit = NEXUS_TimerUnit_eMilliSec;
	if ((ucN == 255) || (ucN == 0))
	{
		newNexusSettings.extraGuardTime.value =  1;
	}
	else
	{
		newNexusSettings.extraGuardTime.value =  ucN;
	}
#else
/*james nagra : 1940 : Guard time set */
/*  dont need to check in case of 255 ::
    Writing a value of 255 ($FF) maps to -1. This means that the total guard time is one ETU and characters
    are transmitted every 11 ETU. Note that this setting is only applicable in T=1 Mode. */
		newNexusSettings.extraGuardTime.value =  ucN;
#endif
#else
		if ((ucN == 255) || (ucN == 0))
			newNexusSettings.extraGuardTime.value =  1;
		else
			newNexusSettings.extraGuardTime.value =  ucN;

#endif

#else
		if((nProtocol == DI_T_1_NA) ||(nProtocol == DI_T_1))	//The T1 default is NA
		{
			if ((ucN == 255) || (ucN == 0))
				newNexusSettings.extraGuardTime.value =  1;
			else
				newNexusSettings.extraGuardTime.value =  ucN;
		}
		else
		{
			if (ucN != 255)
				newNexusSettings.extraGuardTime.value =  ucN;
			else
				newNexusSettings.extraGuardTime.value =  0;
		}
#endif

		newNexusSettings.extraGuardTime.unit = NEXUS_TimerUnit_eEtu;

		/* Set block Guard time */
		newNexusSettings.blockGuardTime.value =  DI_SC_DEFAULT_BLOCK_GUARD_TIME ;
		newNexusSettings.blockGuardTime.unit = NEXUS_TimerUnit_eEtu;
		PrintDebug("[01;36m[DI_SC_SetParams] Smart card EGT(%d) [00m\r\n", newNexusSettings.extraGuardTime.value);
	}
#if 1
	PrintDebug("[DI_SC_GetParams] unDeviceId(0x%x) \n\r", unDeviceId);

	PrintDebug("[01;36m[DI_SC_SetParams] ############### Divide ###############[00m\r\n");
	PrintDebug("[01;36m[DI_SC_SetParams] BAUD DIV : \t0x%X[00m\r\n", newNexusSettings.baudDiv );
	PrintDebug("[01;36m[DI_SC_SetParams] ################ END ################[00m\r\n");

	PrintDebug("[DI_SC_SetParams] protocol : %d\n\r", newNexusSettings.protocol);
	PrintDebug("[DI_SC_SetParams] standard : %d\n\r", newNexusSettings.standard);
	PrintDebug("[DI_SC_SetParams] fFactor : %d\n\r", newNexusSettings.fFactor);
	PrintDebug("[DI_SC_SetParams] dFactor : %d\n\r", newNexusSettings.dFactor);
	PrintDebug("[DI_SC_SetParams] extClockDivisor : %d\n\r", newNexusSettings.extClockDivisor);
	PrintDebug("[DI_SC_SetParams] txRetries : %d\n\r", newNexusSettings.txRetries);
	PrintDebug("[DI_SC_SetParams] rxRetries : %d\n\r", newNexusSettings.rxRetries);
	PrintDebug("[DI_SC_SetParams] workWaitTime : %d\n\r", newNexusSettings.workWaitTime.value);
	PrintDebug("[DI_SC_SetParams] blockWaitTime : %d\n\r", newNexusSettings.blockWaitTime.value);
	PrintDebug("[DI_SC_SetParams] extraGuardTime : %d\n\r", newNexusSettings.extraGuardTime.value);
	PrintDebug("[DI_SC_SetParams] blockGuardTime : %d\n\r", newNexusSettings.blockGuardTime.value);
	PrintDebug("[DI_SC_SetParams] characterWaitTime : %d\n\r", newNexusSettings.characterWaitTime);
	PrintDebug("[DI_SC_SetParams] edcSetting : %d\n\r", newNexusSettings.edcSetting.edcEncode);
	PrintDebug("[DI_SC_SetParams] timeOut : %d\n\r", newNexusSettings.timeOut.value);
	PrintDebug("[DI_SC_SetParams] autoDeactiveReq : %d\n\r", newNexusSettings.autoDeactiveReq);
	PrintDebug("[DI_SC_SetParams] nullFilter : %d\n\r", newNexusSettings.nullFilter);
	PrintDebug("[DI_SC_SetParams] scPresDbInfo : %d\n\r", newNexusSettings.scPresDbInfo.isEnabled);
	PrintDebug("[DI_SC_SetParams] resetCardAction : %d\n\r", newNexusSettings.resetCardAction);
	PrintDebug("[DI_SC_SetParams] blockWaitTimeExt : %d\n\r", newNexusSettings.blockWaitTimeExt.value);
	PrintDebug("[DI_SC_SetParams] isPresHigh : %d\n\r", newNexusSettings.isPresHigh);

#endif
#if (NEXUS_VERSION < 1200)
	newNexusSettings.insertCard.callback = P_SC_InsertCallback;
	newNexusSettings.insertCard.context = NULL;
	newNexusSettings.insertCard.param = (int)unDeviceId;
	newNexusSettings.removeCard.callback = P_SC_RemoveCallback;
	newNexusSettings.removeCard.context = NULL;
	newNexusSettings.removeCard.param = (int)unDeviceId;
#else
	newNexusSettings.cardCallback.callback = P_SC_CardCallback;
	newNexusSettings.cardCallback.context = NULL;
	newNexusSettings.cardCallback.param = (int)unDeviceId;
#endif
#if defined(CONFIG_CAS_ARIB)
	newNexusSettings.setVcc = false; /* If true, modify Vcc value */
#else
	newNexusSettings.setVcc = TRUE; /* If true, modify Vcc value */
	newNexusSettings.vcc = 0; /* 0 = 5V(default), 1 = 3V */
#endif
	rc = NEXUS_Smartcard_SetSettings(pSmartcardInstance->hSmartcard, &newNexusSettings);

	if(rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_SetParams]NEXUS_Smartcard_SetSettings - FAILED : rc:(%#lx)\r\n", rc);
		return DI_ERR_ERROR;
	}

	pSmartcardInstance->state = DI_SC_STATE_Ready;
	pSmartcardInstance->stDrvScInfo.nScProtocol = nProtocol;
	pSmartcardInstance->stDrvScInfo.ulScClock = ulClockFrequency;

	PrintDebug("[DI_SC_SetParams] scClock : %d\n\r", pSmartcardInstance->stDrvScInfo.ulScClock);

	pSmartcardInstance->stDrvScInfo.ucScFI = ucFI;
	pSmartcardInstance->stDrvScInfo.ucScDI = ucDI;
	pSmartcardInstance->stDrvScInfo.ucScN = ucN;
	pSmartcardInstance->stDrvScInfo.ucScCWI = ucCWI;
	pSmartcardInstance->stDrvScInfo.ucScBWI = ucBWI;

	PrintExit();

	return DI_ERR_OK;

}

DI_ERR_CODE DI_SC_GetParams( HUINT32 unDeviceId, DI_SC_PROTOCOL *pnProtocol,
						unsigned long *pulClock, unsigned long *pulBaudrate,
						unsigned char *pucFI, unsigned char *pucDI, unsigned char *pucN,
						unsigned char *pucCWI, unsigned char *pucBWI )
{
	DI_SmartcardInstance *pSmartcardInstance;
	NEXUS_SmartcardSettings currentNexusSettings;

	PrintEnter("[DI_SC_GetParams]\n\r");

	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}
	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_GetParams] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

	NEXUS_Smartcard_GetSettings(pSmartcardInstance->hSmartcard, &currentNexusSettings);
#if 1
	PrintDebug("[DI_SC_GetParams] ------- Current Nexus Settings \n\r");
	PrintDebug("[DI_SC_GetParams] unDeviceId(0x%x) \n\r", unDeviceId);
	PrintDebug("[DI_SC_GetParams] protocol : %d\n\r", currentNexusSettings.protocol);
	PrintDebug("[DI_SC_GetParams] standard : %d\n\r", currentNexusSettings.standard);
	PrintDebug("[DI_SC_GetParams] fFactor : %d\n\r", currentNexusSettings.fFactor);
	PrintDebug("[DI_SC_GetParams] dFactor : %d\n\r", currentNexusSettings.dFactor);
	PrintDebug("[DI_SC_GetParams] extClockDivisor : %d\n\r", currentNexusSettings.extClockDivisor);
	PrintDebug("[DI_SC_GetParams] txRetries : %d\n\r", currentNexusSettings.txRetries);
	PrintDebug("[DI_SC_GetParams] rxRetries : %d\n\r", currentNexusSettings.rxRetries);
	PrintDebug("[DI_SC_GetParams] workWaitTime : %d\n\r", currentNexusSettings.workWaitTime.value);
	PrintDebug("[DI_SC_GetParams] blockWaitTime : %d\n\r", currentNexusSettings.blockWaitTime.value);
	PrintDebug("[DI_SC_GetParams] extraGuardTime : %d\n\r", currentNexusSettings.extraGuardTime.value);
	PrintDebug("[DI_SC_GetParams] blockGuardTime : %d\n\r", currentNexusSettings.blockGuardTime.value);
	PrintDebug("[DI_SC_GetParams] characterWaitTime : %d\n\r", currentNexusSettings.characterWaitTime);
	PrintDebug("[DI_SC_GetParams] edcSetting : %d\n\r", currentNexusSettings.edcSetting.edcEncode);
	PrintDebug("[DI_SC_GetParams] timeOut : %d\n\r", currentNexusSettings.timeOut.value);
	PrintDebug("[DI_SC_GetParams] autoDeactiveReq : %d\n\r", currentNexusSettings.autoDeactiveReq);
	PrintDebug("[DI_SC_GetParams] nullFilter : %d\n\r", currentNexusSettings.nullFilter);
	PrintDebug("[DI_SC_GetParams] scPresDbInfo : %d\n\r", currentNexusSettings.scPresDbInfo.isEnabled);
	PrintDebug("[DI_SC_GetParams] resetCardAction : %d\n\r", currentNexusSettings.resetCardAction);
	PrintDebug("[DI_SC_GetParams] blockWaitTimeExt : %d\n\r", currentNexusSettings.blockWaitTimeExt.value);
	PrintDebug("[DI_SC_GetParams] isPresHigh : %d\n\r", currentNexusSettings.isPresHigh);

	PrintDebug("\n[DI_SC_GetParams] ------- Current DI Settings \n\r");
	PrintDebug("[DI_SC_GetParams] unDeviceId(0x%x) \n\r", unDeviceId);
	PrintDebug("[DI_SC_GetParams] ucInit : %d\n\r", pSmartcardInstance->stDrvScInfo.ucInit);
	PrintDebug("[DI_SC_GetParams] nScProtocol : %d\n\r", pSmartcardInstance->stDrvScInfo.nScProtocol);
	PrintDebug("[DI_SC_GetParams] ulScClock : %d\n\r", pSmartcardInstance->stDrvScInfo.ulScClock);
	PrintDebug("[DI_SC_GetParams] ucScFI : %d\n\r", pSmartcardInstance->stDrvScInfo.ucScFI);
	PrintDebug("[DI_SC_GetParams] ucScDI : %d\n\r", pSmartcardInstance->stDrvScInfo.ucScDI);
	PrintDebug("[DI_SC_GetParams] ucScN : %d\n\r", pSmartcardInstance->stDrvScInfo.ucScN);
	PrintDebug("[DI_SC_GetParams] ucScCWI : %d\n\r", pSmartcardInstance->stDrvScInfo.ucScCWI);
	PrintDebug("[DI_SC_GetParams] ucScBWI : %d\n\r", pSmartcardInstance->stDrvScInfo.ucScBWI);
#endif

	*pnProtocol 	= pSmartcardInstance->stDrvScInfo.nScProtocol;
	*pulClock	= pSmartcardInstance->stDrvScInfo.ulScClock/1000;
#if 0
	*pulBaudrate	= pSmartcardInstance->stDrvScInfo.ulScBaudrate;
#else
	*pulBaudrate	= 0;
#endif
	*pucFI		= currentNexusSettings.fFactor;
	*pucDI		= currentNexusSettings.dFactor;
	*pucN		= currentNexusSettings.blockGuardTime.value;
	*pucBWI 	= currentNexusSettings.blockWaitTime.value;

	if (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0)
	{
		*pucCWI = currentNexusSettings.workWaitTime.value;
	}
	else  //T1, T14
	{
		*pucCWI = currentNexusSettings.characterWaitTime;
	}

	PrintExit("[DI_SC_GetParams]\n\r");

	return DI_ERR_OK;
}

DI_ERR_CODE DI_SC_GetCardStatus( HUINT32 unDeviceId, DI_SC_STATUS *pnStatus )
{
	NEXUS_Error	rc = NEXUS_SUCCESS;
	DI_SmartcardInstance *pSmartcardInstance;
	NEXUS_SmartcardStatus CardStatus;

	PrintEnter("DI_SC_GetCardStatus\n\r");

	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}
	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_GetCardStatus] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

	rc = NEXUS_Smartcard_GetStatus(pSmartcardInstance->hSmartcard, &CardStatus);

	if(rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_GetCardStatus]NEXUS_Smartcard_GetStatus - FAILED : rc:(%#lx)\r\n", rc);
		return DI_ERR_ERROR;
	}

	PrintDebug("[DI_SC_GetCardStatus] [ %d ]cardPresent : %d state : %d\n\r",
		unDeviceId,CardStatus.cardPresent,CardStatus.state);

	if (CardStatus.cardPresent)
	{
		PrintDebug("[DI_SC_GetCardStatus] [ %d ]Card Present!!\r\n",unDeviceId);
		*pnStatus = DI_SC_PRESENT;
	}
	else
	{
		PrintDebug("[DI_SC_GetCardStatus] [ %d ]Card Not Present!!\r\n",unDeviceId);
		*pnStatus = DI_SC_NOT_PRESENT;
	}

	PrintExit("DI_SC_GetCardStatus\n\r");

 	return DI_ERR_OK;

}

DI_ERR_CODE DI_SC_Reset (HUINT32 unDeviceId, HUINT8 *pucAtr, HUINT32 *pucAtrlen)
{
	DI_SmartcardInstance *pSmartcardInstance;
	DI_ERR_CODE errorCode = DI_ERR_OK;
	NEXUS_Error	rc = NEXUS_SUCCESS;
	int returnCode = VK_OK;
	NEXUS_SmartcardStatus  CardStatus;
	unsigned int atrReadLength = 0;
	unsigned int i;


	PrintEnter("[DI_SC_Reset]\r\n");

	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}
	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_TransferData] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

	//this points make the errorCode is DI_ERR_OK
	returnCode = VK_SEM_Get(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[DI_SC_TransferData] ERROR: VK_SEM_Get \r\n");
		errorCode = DI_ERR_ERROR;
		goto RTN;
	}

	rc = NEXUS_Smartcard_GetStatus(pSmartcardInstance->hSmartcard,  &CardStatus);
	if(rc != NEXUS_SUCCESS)
	{
		errorCode = DI_ERR_ERROR;
		goto RTN;
	}

	if( CardStatus.cardPresent != TRUE )
	{
		PrintError("[DI_SC_Reset] SmartCard is not present in unDeviceId(0x%x)\r\n", unDeviceId);
		errorCode = DI_ERR_ERROR;
		goto RTN;
	}

	if ((pSmartcardInstance->bAutoDeactiveFlag ==true))
	{
		PrintDebug("[DI_SC_Reset] Call cold reset\n");

		rc = NEXUS_Smartcard_Reset(pSmartcardInstance->hSmartcard,false );	/*cold reset */

		if ( rc != NEXUS_SUCCESS)
		{
			PrintError("[DDI_SC_Reset] NEXUS_Smartcard_Reset - FAILED: rc (%#lx)\r\n", rc);
			errorCode = DI_ERR_ERROR;
			goto RTN;
		}
	}
	else
	{
/*
 * 20091008 :: POOH :: maybe this fix point is not necessary.
 *			because i add warm reset function.
 */
#if 0
#if defined(CONFIG_CAS_NA)
		rc = NEXUS_Smartcard_ResetTransBuffer(pSmartcardInstance->hSmartcard);
		if(rc != NEXUS_SUCCESS)
		{
			PrintError("[DDI_SC_Reset] NEXUS_Smartcard_ResetTransBuffer - FAILED: rc (%#lx)\r\n", rc);
			return DI_ERR_ERROR;
		}
#endif
#endif
		PrintDebug("[DI_SC_Reset] Call warm reset\n");

		rc = NEXUS_Smartcard_Reset(pSmartcardInstance->hSmartcard,true );	/*warm reset */

		if ( rc != NEXUS_SUCCESS)
		{
			PrintError("[DDI_SC_Reset] NEXUS_Smartcard_Reset - FAILED: rc (%#lx)\r\n", rc);
			errorCode = DI_ERR_ERROR;
			goto RTN;
		}
	}

	PrintDebug("[DI_SC_Reset] Call NEXUS_Smartcard_ResetCard \n");

	rc = NEXUS_Smartcard_ResetCard(pSmartcardInstance->hSmartcard, pSmartcardInstance->s_aucAtrBuf, MAX_ATR_SIZE, &atrReadLength);
	if(rc != NEXUS_SUCCESS)
	{
//2010061 :: POOH :: ir¸ðµ¨Áß failÀÌÁö¸¸ length°¡ ÀÖÀ» °æ¿ì atrÀ» ¿Ã¸®¸é Á¤»óµ¿ÀÛÇÏ´Â ¸ðµ¨ÀÌ ÀÖ¾î¼­
//               ³Ö¾úÀ¸³ª nagra cak7 micropross ¿¡¼­ ¹®Á¦°¡ µÇ¾î »èÁ¦ÇÔ.
//		if(atrReadLength<=0 || atrReadLength>MAX_ATR_SIZE)
		{
			PrintError("[DI_SC_Reset]atrReadLength<=0 || atrReadLength>MAX_ATR_SIZE : length_read(%d) \r\n", atrReadLength);
			errorCode = DI_ERR_ERROR;
			goto RTN;
		}
	}

	if (atrReadLength <= 0 || atrReadLength > MAX_ATR_SIZE)
	{
		PrintError("[DI_SC_Reset]Invalid length_read : length_read(%d) \r\n", atrReadLength);
		errorCode = DI_ERR_ERROR;
		goto RTN;
	}

	pSmartcardInstance->state = DI_SC_STATE_ResetDone;

	if( atrReadLength > 0 )
	{
		VK_memcpy( pucAtr, pSmartcardInstance->s_aucAtrBuf, MAX_ATR_SIZE );
		*pucAtrlen = atrReadLength;

		PrintDebug("[DI_SC_Reset] unDeviceId=0x%x\n", unDeviceId);
		PrintDebug("    -> ATR Length = %d\n\r", atrReadLength);
		PrintDebug("    -> ATR Data =  ");
		for (i = 0; i < atrReadLength; i++)
		{
			PrintDebug("%02X ", pSmartcardInstance->s_aucAtrBuf[i]);
		}
		PrintDebug("\n\r");
		pSmartcardInstance->state = DI_SC_STATE_ReceiveDecodeATR;
	}
#if defined(ENABLE_SC_MINIMUN_DEBUG)
	PrintError("	-> ATR Length = %d\n\r", *pucAtrlen);
#endif

RTN:
	if ( errorCode != DI_ERR_OK)
		*pucAtrlen = (unsigned int) 0;

	returnCode = VK_SEM_Release(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[DI_SC_Reset] ERROR: VK_SEM_Release \r\n");
		*pucAtrlen = (unsigned int) 0;
		return DI_ERR_ERROR;
	}

	PrintExit("[DI_SC_Reset]");

//	NEXUS_Smartcard_SetParamsFromAtr(pSmartcardInstance->hSmartcard);

	return errorCode;
}

#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
/* conax PT : SC-6.0/3.1.3-G - Labwise on 20140815 */

/* Modify from 2ms to 2.5ms conax PT : SC-6.0/3.1.3-G - TATA on 20141024
* Between the last of read character from card and the first of write character, need extra-guard time that is TC1. */
#define DI_SC_WAIT_BLOCK_GUARD_TIME		usleep(2600); // 56 etu + alpha(5~6etu) + alpha
#endif
DI_ERR_CODE DI_SC_TransferData( HUINT32 unDeviceId,
						unsigned char *pucWriteBuf, int ucNumToWrite,
						unsigned char *pucReadBuf, unsigned int *pulNumRead )
{
	DI_ERR_CODE errorCode = DI_ERR_OK;
	NEXUS_Error	rc = NEXUS_SUCCESS;
	int returnCode = VK_OK;
	DI_SmartcardInstance *pSmartcardInstance;

	unsigned int uNumDataWritten;
	unsigned int uNumDataRead;

	unsigned long	ulRcvDataNum;
	unsigned char	ucAckData;
	unsigned char	ucInsByte;
	unsigned char	ucProcedureByte;
	unsigned int 	uNumBytes = MAX_DATA_SIZE;

	PrintEnter("[DI_SC_TransferData]\r\n");

	if( unDeviceId >= NUM_SMARTCARD )
	{
		PrintError("Invalid DeviceID(%d)\r\n", unDeviceId);
		return DI_ERR_INVALID_PARAM;
	}

	if((pucWriteBuf == NULL) || (pucReadBuf == NULL) || (pulNumRead == NULL))
	{
		PrintError("[DI_SC_TransferData] Error, pucWriteBuf or pucReadBuf or pulNumRead is NULL pointer\r\n");
		return DI_ERR_INVALID_PARAM;
	}

	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_TransferData] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		*pulNumRead = (unsigned int)0;
		return DI_ERR_ERROR;
	}
	//this points make the errorCode is DI_ERR_OK
	returnCode = VK_SEM_Get(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[DI_SC_TransferData] ERROR: VK_SEM_Get \r\n");
		*pulNumRead = (unsigned int)(unsigned int)0;
		return DI_ERR_ERROR;
	}

	PrintDebug("[DI_SC_TransferData] unDeviceId:%d, ucNumToWrite(%d), pulNumRead(%d)\r\n", unDeviceId,  ucNumToWrite, *pulNumRead);
#if 0
	{
	int chk=0;
	PrintDebug("[01;36m========== TransferData(%d) ==========\r\n", ucNumToWrite);
	for ( chk =0; chk<ucNumToWrite;chk++)
		PrintDebug("%02X ", pucWriteBuf[chk]);
		PrintDebug("[00m\r\n");
	}
#endif

	if ((pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0)
		|| (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_IR)
		|| (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_CX))
	{
		// Send Header 5 byte
		rc =NEXUS_Smartcard_Write(pSmartcardInstance->hSmartcard, pucWriteBuf, DI_SC_T0_HEADER_NUM, &uNumDataWritten);
		if ( rc != NEXUS_SUCCESS )
		{
			errorCode = DI_ERR_ERROR;
			PrintError("[DI_SC_TransferData] Error(0x%X) in NEXUS_Smartcard_Write\r\n", rc);
			goto RTN;
		}
		PrintDebug("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);

		// Read ACK
		rc = NEXUS_Smartcard_Read(pSmartcardInstance->hSmartcard, &ucAckData, DI_SC_T0_ACK_CHECK, &uNumDataRead);
		if ( rc != NEXUS_SUCCESS )
		{
			errorCode = DI_ERR_ERROR;
			PrintError("[DI_SC_TransferData] Error(0x%X) in NEXUS_Smartcard_Read() - ACK about Header\r\n", rc);
			goto RTN;
		}
		PrintDebug("[01;35m[%d]Read data length : (%X)[00m\r\n", __LINE__,uNumDataRead);

#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
		if (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_CX)
		{
			if (pSmartcardInstance->stDrvScInfo.ucScN == 0x2c)
			{
				DI_SC_WAIT_BLOCK_GUARD_TIME;
			}
		}
#endif

		if( (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_IR) || (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0))	//The T0 default is IR
		{
			int readbyte = 0;

			PrintDebug("[01;35m~~~~~~ AckByte(%X), Coming data length : (%X) ~~~~~~~~~[00m\r\n", ucAckData, pucWriteBuf[4]);

			ucNumToWrite-=DI_SC_T0_HEADER_NUM;
			if (ucAckData == pucWriteBuf[1])
			{
				PrintDebug("[01;35mucNumToWrite = %d[00m\r\n",ucNumToWrite);
				if(ucNumToWrite == 0)
				{
					readbyte = pucWriteBuf[4];
					uNumBytes =	readbyte + 2;
					PrintDebug("[01;35m[%d]uNumBytes : (%X)[00m\r\n", __LINE__,uNumBytes);
				}
				else
				{
					pucWriteBuf +=DI_SC_T0_HEADER_NUM;

					rc = NEXUS_Smartcard_Write(pSmartcardInstance->hSmartcard, pucWriteBuf, (size_t)ucNumToWrite, &uNumDataWritten);
					if ( rc != NEXUS_SUCCESS )
					{
						PrintError("[DI_SC_TransferData] Error(0x%X) in NEXUS_Smartcard_Write-all remaining data bytes\r\n");
						errorCode = DI_ERR_ERROR;
						goto RTN;
					}
					if(uNumDataWritten != 2)
					{
						PrintError("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);
					}
					PrintDebug("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);

					uNumBytes = 2;
				}
			}
			else if ((pucWriteBuf[1]^0xff) == ucAckData)
			{
				if (ucNumToWrite != 0 )
				{
					pucWriteBuf +=DI_SC_T0_HEADER_NUM;

					rc = NEXUS_Smartcard_Write(pSmartcardInstance->hSmartcard, pucWriteBuf, 1, &uNumDataWritten);
					if ( rc != NEXUS_SUCCESS )
					{
						PrintError("[DI_SC_TransferData] Error(0x%X) in NEXUS_Smartcard_Write-all remaining data bytes\r\n");
						errorCode = DI_ERR_ERROR;
						goto RTN;
					}
				}
				if(uNumDataWritten != 2)
				{
					PrintError("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);
				}
				PrintDebug("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);

				uNumBytes = 2;
			}
			else if (ucAckData == 0x60)
			{
				PrintDebug("[DI_SC_TransferData] ucAckData is 0x60 : Get Two Bytes\n");
				uNumBytes = 2;
			}
			else if (((ucAckData & 0xF0) == 0x60) || ((ucAckData & 0xF0) == 0x90)  )
			{
				#if 1	// if AckData is 0x6x or 0x9x, it's SW1
				pucReadBuf[0] = ucAckData;
				rc = NEXUS_Smartcard_Read(pSmartcardInstance->hSmartcard, &pucReadBuf[1], 1, &uNumDataRead);
				PrintDebug("[01;35m[%d]Read data length : (%X)[00m\r\n", __LINE__,uNumDataRead);
				ulRcvDataNum = 2;
				goto RTN;
				#else
				PrintDebug("[DI_SC_TransferData] ucAckData is 0x6X or 0x9X - Get One Byte\n");
				uNumBytes = 1;
				#endif
			}
			else
			{
				PrintError("[DI_SC_TransferData] Error in DI_SC_TransferData-invalid Ack Data\r\n");
				errorCode = DI_ERR_ERROR;
				goto RTN;
			}
		}
		else //DI_T_0_CX
		{
			uNumBytes = *pulNumRead;
			ucInsByte = pucWriteBuf[1];
			PrintDebug("[01;35m~~~~~~ ucRcvData(%X), WriteBuff(%x), INS Byte(0x%X) ~~~~~~~~~[00m\r\n", ucAckData, pucWriteBuf[1], ucInsByte);

				// Send Body
			pucWriteBuf +=DI_SC_T0_HEADER_NUM;
			ucNumToWrite-=DI_SC_T0_HEADER_NUM;

			while( ucNumToWrite )
			{
				ucProcedureByte = ucAckData^ucInsByte;
				PrintDebug("[01;35m~~~~~~ucProcedureByte(0x%X), ucRcvData(%X), ucInsByte(%x) ~~~~~~~~~[00m\r\n", ucProcedureByte, ucAckData, ucInsByte);

				if ( !ucProcedureByte )//(ucAckData == pucWriteBuf[1]) // INS - instruction byte
				{
					PrintDebug("[01;35m~~ ACK Same as INS ~~RemainData(0x%X)[00m\r\n", ucNumToWrite);
					if ( ucNumToWrite )
					{
						rc = NEXUS_Smartcard_Write(pSmartcardInstance->hSmartcard, pucWriteBuf, (unsigned long)ucNumToWrite, &uNumDataWritten);
						if ( rc != NEXUS_SUCCESS )
						{
							PrintError("[DI_SC_TransferData] Error(0x%X) in NEXUS_Smartcard_Write-all remaining data bytes\r\n");
							errorCode = DI_ERR_ERROR;
							goto RTN;
						}
						PrintDebug("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);
					}
					break;
				}
				else if ( ucProcedureByte == 0xFF)
				{
					PrintDebug("[01;35m~~ BYTE TO BYTE Transfer~~RemainData(0x%X)[00m\r\n", ucNumToWrite);
					rc = NEXUS_Smartcard_Write(pSmartcardInstance->hSmartcard, pucWriteBuf, (unsigned long)DI_SC_T0_BYTE_TO_BYTE_NUM, &uNumDataWritten);
					if ( rc != NEXUS_SUCCESS )
					{
						PrintError("[DDI_SC_TransferData] Error(0x%X) in BSCD_Channel_Transmit-the next data byte\r\n");
						errorCode = DI_ERR_ERROR;
						goto RTN;
					}
					PrintDebug("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);

					pucWriteBuf +=DI_SC_T0_BYTE_TO_BYTE_NUM;
					ucNumToWrite-=DI_SC_T0_BYTE_TO_BYTE_NUM;

					// when sending data is completed, waiting for SW1
					if (ucNumToWrite == 0)
						break;

					// Read ACK
						rc = NEXUS_Smartcard_Read(pSmartcardInstance->hSmartcard, &ucAckData, DI_SC_T0_ACK_CHECK, &uNumDataRead);
					if ( rc != NEXUS_SUCCESS )
					{
						errorCode = DI_ERR_ERROR;
						PrintError("[DI_SC_TransferData] Error(0x%X) in NEXUS_Smartcard_Read-ACK about ByteToByte\r\n", rc);
						goto RTN;
					}
					PrintDebug("[01;35m[%d]Read data length : (%X)[00m\r\n", __LINE__,uNumDataRead);
				}
				else // in case of SW1: waiting for SW2
				{
					pucReadBuf[0] = ucAckData;
					rc = NEXUS_Smartcard_Read(pSmartcardInstance->hSmartcard,&pucReadBuf[1], 1, &uNumDataRead);
					PrintDebug("[01;35m[%d]Read data length : (%X)[00m\r\n", __LINE__,uNumDataRead);
					ulRcvDataNum = 2;
					goto RTN;
				}
#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
				if (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_CX)
				{
					if (pSmartcardInstance->stDrvScInfo.ucScN == 0x2c)
					{
						DI_SC_WAIT_BLOCK_GUARD_TIME;
					}
				}
#endif
			}
		}
	}
	else
	{
		// send
		rc = NEXUS_Smartcard_Write(pSmartcardInstance->hSmartcard, pucWriteBuf, (unsigned long)ucNumToWrite, &uNumDataWritten);
		if ( rc != NEXUS_SUCCESS)
		{
			errorCode = DI_ERR_ERROR;
			PrintError("[DI_SC_TransferData] faulure to send \r\n");
			goto RTN;
		}
		PrintDebug("[01;35m[%d]Write data length : (%X)[00m\r\n", __LINE__,uNumDataWritten);
	}

	rc = NEXUS_Smartcard_Read(pSmartcardInstance->hSmartcard, pucReadBuf, (size_t)uNumBytes, &uNumDataRead);
	if ( rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_TransferData] Error(0x%X) in NEXUS_Smartcard_Read\r\n", rc);
		errorCode = DI_ERR_ERROR;
		goto RTN;
	}

#if 0
	{
		unsigned int chk=0;
		PrintDebug("[01;36m[%d]========== ReadData(%d) ==========\r\n", __LINE__,uNumDataRead);
		for ( chk =0; chk<uNumDataRead;chk++)
			PrintDebug("%02X ", pucReadBuf[chk]);
		PrintDebug("[00m\r\n");
	}
#endif

	PrintDebug("[01;35m[%d]Read data length : (%X)[00m\r\n", __LINE__,uNumDataRead);
	ulRcvDataNum = uNumDataRead;

RTN:
#if defined(CONFIG_CAS_CX) && defined(CONFIG_CAS_CX_ISO7816)
	if (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_CX)
	{
		if (pSmartcardInstance->stDrvScInfo.ucScN == 0x2c)
		{
			DI_SC_WAIT_BLOCK_GUARD_TIME;
		}
	}
#endif
	if ( errorCode != DI_ERR_OK)
		*pulNumRead = (unsigned int) 0;
	else
		*pulNumRead = (unsigned int) ulRcvDataNum;

	returnCode = VK_SEM_Release(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[DI_SC_TransferData] ERROR: VK_SEM_Release \r\n");
		*pulNumRead = (unsigned int)0;
		return DI_ERR_ERROR;
	}

	PrintExit("[DDI_SC_TransferData]\n\r");

	return errorCode;

}

DI_ERR_CODE DI_SC_ReadData ( HUINT32 unDeviceId,
						unsigned char *pucWriteBuf, int ucNumToWrite,
						unsigned char *pucReadBuf, int *pucNumRead )
{
	DI_SmartcardInstance *pSmartcardInstance;

	DI_ERR_CODE errorCode = DI_ERR_ERROR;
	unsigned int uNumDataRead;

	PrintEnter("[DI_SC_ReadData]\n\r");

	if( unDeviceId >= NUM_SMARTCARD )
	{
		PrintError("Invalid DeviceID(%d)\r\n", unDeviceId);
		return DI_ERR_INVALID_PARAM;
	}

	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if ((pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0)
		|| (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_IR)
		|| (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_CX))
	{
		uNumDataRead = 2;

		if(pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_CX)
		{
		if ((pucWriteBuf[0] == 0xDD) && (pucWriteBuf[1] == 0xCA))
			uNumDataRead = pucWriteBuf[4] + 2;
		}

		errorCode = DI_SC_TransferData(unDeviceId, pucWriteBuf, ucNumToWrite, pucReadBuf, &uNumDataRead);
		*pucNumRead = uNumDataRead;

		errorCode = DI_SC_TransferData(unDeviceId, pucWriteBuf, ucNumToWrite, pucReadBuf, &uNumDataRead);
		*pucNumRead = uNumDataRead;

		if( errorCode != DI_ERR_OK)
		{
			PrintError("DI_SC_ReadData] Error(0x%X) in DI_SC_TransferData() \r\n", errorCode );
		}
	}

	PrintExit("[DI_SC_ReadData]\n\r");

	return errorCode;
}

DI_ERR_CODE DI_SC_WriteData( HUINT32 unDeviceId,
				unsigned char *pucWriteBuf, int ucNumToWrite,
				unsigned char *pucReadBuf, int *pucNumRead )
{
	DI_SmartcardInstance *pSmartcardInstance;

	DI_ERR_CODE errorCode = DI_ERR_ERROR;
	unsigned int uNumDataWrite;

	PrintEnter("[DI_SC_WriteData]\r\n");

	if( unDeviceId >= NUM_SMARTCARD )
	{
		PrintError("Invalid DeviceID(%d)\r\n", unDeviceId);
		return DI_ERR_INVALID_PARAM;
	}

	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if ((pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0)
		|| (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_IR)
		|| (pSmartcardInstance->stDrvScInfo.nScProtocol == DI_T_0_CX))
	{
		uNumDataWrite = 2;
		errorCode = DI_SC_TransferData(unDeviceId, pucWriteBuf, ucNumToWrite, pucReadBuf, &uNumDataWrite);
		if( errorCode != DI_ERR_OK)
		{
			PrintError("DI_SC_WriteData] Error(0x%X) in DI_SC_TransferData() \r\n", errorCode );
			uNumDataWrite = 0;
		}
		*pucNumRead = (unsigned char) uNumDataWrite;
	}

	PrintExit("[DI_SC_WriteData]\n\r");

	return errorCode;
}

DI_ERR_CODE DI_SC_RegisterCallback (HUINT32 unDeviceId, pfnDI_SC_EVT_CALLBACK pfnSCCallback)
{
	DI_SmartcardInstance *pSmartcardInstance;
	int returnCode = VK_OK;

	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}

	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_RegisterCallback] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

	returnCode = VK_SEM_Get(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[DI_SC_RegisterCallback] ERROR: VK_SEM_Get \r\n");
		return DI_ERR_ERROR;
	}

	pSmartcardInstance->pstSmartcardCallback.pfnCallback = pfnSCCallback;

	returnCode = VK_SEM_Release(pSmartcardInstance->semId);
	if (returnCode != VK_OK)
	{
		PrintError("[DI_SC_RegisterCallback] ERROR: VK_SEM_Release \r\n");
		return DI_ERR_ERROR;
	}

	return	DI_ERR_OK;
}


DI_ERR_CODE DI_SC_Activate( HUINT32 unDeviceId )
{
	DI_ERR_CODE errorCode = DI_ERR_OK;
#if defined(CONFIG_TEMP_DI10)
	NEXUS_Error	rc = NEXUS_SUCCESS;
#endif
	DI_SmartcardInstance *pSmartcardInstance;


	PrintEnter("[DI_SC_Activate] Enter \r\n");

	if(unDeviceId >= NUM_SMARTCARD)
	{
		return DI_ERR_INVALID_PARAM;
	}
	pSmartcardInstance = P_SC_GetInstance(unDeviceId);

	if( pSmartcardInstance->bInitialized == FALSE )
	{
		PrintError("[DI_SC_DownUpVCC] NOT Initialized : ucDeviceId(%x)\n\r", unDeviceId);
		return DI_ERR_ERROR;
	}

#if defined(CONFIG_TEMP_DI10)
	rc = NEXUS_Smartcard_Activate(pSmartcardInstance->hSmartcard);
	if ( rc != NEXUS_SUCCESS)
	{
		PrintError("[DI_SC_Activate] NEXUS_Smartcard_Activate Error!(0x%X)\r\n", rc);
		return DI_ERR_ERROR;
	}
#endif

	PrintExit("[DI_SC_Activate] Exit");
	return errorCode;
}

