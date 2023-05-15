/*
	Viaccess OS API
		Driver API Manual ACS 3.0 - Doc. Ref. 2217, p.59
*/

/*******************************************************************/
/********************      Header Files    *************************/
/*******************************************************************/
#include "va_def.h"
#include "va_os.h"
#include "va_os_api.h"
#include "vkernel.h"

#include <stdarg.h>
#include <string.h>


/*******************************************************************/
/********************      Global Variables     ********************/
/*******************************************************************/


/*******************************************************************/
/********************      Defines         *************************/
/*******************************************************************/



// #define DEBUG_THREAD_AND_MUTEX

#define MJD_1970_1_1	0x9e8b

#define VA_OS_PRINT_BUF_LEN		1024

#define VA_OS_VALID_DATA			0x1234abcd

#define VA_OS_MAX_PERIODIC_CALL	5

#if defined(CONFIG_PROD_PROFILE_MINIOCTO)
// #define USE_PERIODIC_CALL_TASK
// VA-5SDÀÇ °æ¿ì ÀÌ°ÍÀ» »ç¿ëÇÏ°Ô µÇ¸é Viaccess Driver Test ÀÇ OS Suite, OS_MUT_LOCK_UNLOCK_2 Ç×¸ñ¿¡¼­ fail µÊ.
// ÀÏ¹ÝÀûÀ¸·Î »ç¿ëÇÏÁö ¾Ê´Â °ÍÀÌ ¸ÂÀ» µí ÇÏÁö¸¸ ±âÁ¸ ¸ðµ¨¿¡¼­ ¹®Á¦°¡ ÀÖ¾î Ãß°¡ÇÑ ÄÚµå¶ó ÇÏ´Ï VA-5SD¿¡ ÇÑÇØ¼­¸¸ »ç¿ëÇÏÁö ¾Ê´Â °ÍÀ¸·Î ÇÑ´Ù -> CONFIG_PROD_PROFILE_MINIOCTO ¿¡¼­´Â »ç¿ëÇÏÁö ¾Ê´Â °ÍÀ¸·Î ÇÑ´Ù.
// -> va_def.h ¿¡ ¼±¾ðµÈ °¢ task priority ¸¦ Æ©´×ÇÏ¸é ÀÌ define À» Àû¿ëÇØµµ µÈ´Ù.
#else // org
#define USE_PERIODIC_CALL_TASK
#endif
//#define USE_TICK_TO_GET_RUNTIME
#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST) && defined(OS_LINUX)
#define USE_VA_MEMORY_MANAGER	// ¸¸¾à, kernel¿¡¼­ memory °ü¸®¸¦ ¾ÈÇØÁÖ¸é, ÀÌ°Å¶óµµ ½á¾ß ÇÑ´Ù..¼Óµµ°¡ ´À·ÁÁö°ÚÁö¸¸,,, -> ÀÏ¹ÝÀûÀ¸·Î »ç¿ëÇÏÁö ¾Ê´Â °ÍÀÌ ¸ÂÀ» µí ÇÏÁö¸¸ ¸®´ª½º È¯°æ¿¡¼­´Â ¾îÂ¿ ¼ö ¾ø´Ù°í ÇÔ.
#endif

#if !defined(CONFIG_PROD_CPU_BCM)	// bxr-hd(bcm)Àº os count mutex¸¦ »ç¿ëÇÏ¹Ç·Î ¿©±â¼­´Â countÇÏÁö ¾Ê´Â´Ù
// MW¿¡¼­ count mutex Ã³¸®¸¦ ÇÏ±â À§ÇØ »ç¿ë
#define USE_MW_MUTEX_COUNTER
#endif

#ifdef USE_VA_MEMORY_MANAGER
#define VA_OS_ALLOC_TABLE_SIZE		2048

typedef struct {
	HUINT8 *pucMemory;
	HUINT32 ulSize;
} VA_OS_AllocTable;
#endif

typedef struct {
#ifdef  USE_PERIODIC_CALL_TASK
	UINT32 ulNextWakeup;
#endif
	UINT32 ulDuration;
	UINT32 ulVaData;
	UINT32 ulTimerId;
	tVA_OS_PeriodicCallFct pfPeriodicCall;
} VA_OS_PeriodicCall;

// ref. kVA_SETUP_MUTEX_SIZE_IN_DWORD
typedef enum
{
	eVA_OS_MUTEX_VALID = 0,
	eVA_OS_MUTEX_ID,
	eVA_OS_MUTEX_COUNT,
	eVA_OS_MUTEX_RFU1,	// reserved for future use
	eVA_OS_MUTEX_RFU2,
	eVA_OS_MUTEX_RFU3,
	eVA_OS_MUTEX_MAX
}eMutexArrayIndicator;


/********************************************************************
*	External variables & function prototypes
********************************************************************/

/********************************************************************
*	Static variables
********************************************************************/
static char s_szVaPrintBuffer[VA_OS_PRINT_BUF_LEN];

#ifdef USE_VA_MEMORY_MANAGER
static VA_OS_AllocTable s_stAllocTable[VA_OS_ALLOC_TABLE_SIZE];
static HUINT32 s_stAllocTablePtr;
#endif

#ifndef USE_TICK_TO_GET_RUNTIME
static UINT32 s_ulRuntime; // unit: 100ms
#endif

static VA_OS_PeriodicCall *sVaOsPeriodicCall;
#ifdef  USE_PERIODIC_CALL_TASK
static UINT32 sVaOsTid;
#endif

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/
#ifdef  USE_PERIODIC_CALL_TASK
static void VA_OS_Task(void * args);
#else
static void VA_OS_TimerCallBack(unsigned long ulArgSize, void *pArg);
#endif
static void VA_OS_MJD2YMD(UINT16 mjd, tVA_OS_Tm *ymd);
static UINT16 VA_OS_SEC2MJD(UINT32 secs);
static void VA_OS_SEC2UTC(UINT32 secs, UINT8 *utc);
static void VA_OS_UTC2YMD(UINT8 *utc, tVA_OS_Tm *ymd);
static UINT8 VA_OS_BCD2BIN(UINT8 bcd);
static UINT8 VA_OS_BIN2BCD ( UINT8 bin );

#if defined(CONFIG_PROD_CPU_ST) && defined(OS_LINUX)
int s_ulMutexSleepCnt = 0;
#endif

#ifndef USE_TICK_TO_GET_RUNTIME
static INT VA_INT_CountRuntime(DWORD dwViaccessData)
{
	s_ulRuntime ++;
	return 0;
}

void VA_INT_DebugPrintRuntime()
{
	UINT32 d, h, m, s, _100ms;

	_100ms = (s_ulRuntime % 10);
	s = (s_ulRuntime / 10) % 60;
	m = (s_ulRuntime / 600) % 60;
	h = (s_ulRuntime / 36000) % 24;
	d = (s_ulRuntime / 36000 / 24);

	HxLOG_Info("[RUNTIME=%d d %02d h %02d m %02d.%d]\n", d, h, m, s, _100ms);
}
#endif

void VA_OS_Init()
{
	int i, nErr;

	HxLOG_Print("[Enter]\n");

#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
	VA_OS_Adapt_SetTime();
#endif

#ifdef USE_VA_MEMORY_MANAGER
	VA_memset(s_stAllocTable, 0, sizeof(s_stAllocTable));
	s_stAllocTablePtr = 0;
#endif

//	s_szVaPrintBuffer = (char *)VA_MEM_Alloc(VA_OS_PRINT_BUF_LEN);
//	VA_ASSERT(s_szVaPrintBuffer != NULL);

	sVaOsPeriodicCall = (VA_OS_PeriodicCall *)VA_MEM_Alloc(sizeof(VA_OS_PeriodicCall) * VA_OS_MAX_PERIODIC_CALL);
	VA_ASSERT(sVaOsPeriodicCall != NULL);

	for (i = 0; i < VA_OS_MAX_PERIODIC_CALL; i++)
	{
		sVaOsPeriodicCall[i].ulDuration = 0;
	}

#ifdef  USE_PERIODIC_CALL_TASK
	nErr = VK_TASK_Create(VA_OS_Task, VA_OS_TASK_PRIORITY, VA_OS_TASK_STACK_SIZE, VA_OS_TASK_NAME, NULL, &sVaOsTid, 0);
	if (nErr)
	{
		HxLOG_Error("failed to create task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}

	nErr = VK_TASK_Start(sVaOsTid);
	if (nErr)
	{
		HxLOG_Error("failed to start task (%d)\n", nErr);
		VA_ASSERT(nErr == VK_OK);
	}
#endif

#ifndef USE_TICK_TO_GET_RUNTIME
	s_ulRuntime = 0;
	VA_OS_StartPeriodicCall(100, VA_INT_CountRuntime, 0);
#endif

	HxLOG_Print("[Exit]\n");
}

#ifdef  USE_PERIODIC_CALL_TASK
static void VA_OS_Task(void * args)
{
	int i;
	UINT32 ulCurTime;

HxLOG_Print("[Enter]\n");

	while (sVaOsPeriodicCall)
	{
		ulCurTime = VK_TIMER_GetSystemTick(); // * 1000 / VK_TIMER_GetTickFreq();

		for (i = 0; i < VA_OS_MAX_PERIODIC_CALL; i++)
		{
			if (sVaOsPeriodicCall[i].ulDuration)
			{
				if (ulCurTime >= sVaOsPeriodicCall[i].ulNextWakeup)
				{
// ÀÌ°É·Î´Â ¿¡·¯°¡ ´©ÀûµÈ´Ù..					sVaOsPeriodicCall[i].ulNextWakeup = ulCurTime + sVaOsPeriodicCall[i].ulDuration;
					sVaOsPeriodicCall[i].ulNextWakeup = sVaOsPeriodicCall[i].ulNextWakeup + sVaOsPeriodicCall[i].ulDuration;
					if (sVaOsPeriodicCall[i].pfPeriodicCall)
						sVaOsPeriodicCall[i].pfPeriodicCall(sVaOsPeriodicCall[i].ulVaData);
				}
			}
		}

		VK_TASK_Sleep(10);
	}

HxLOG_Print("[Exit]\n");
}
#else
static void VA_OS_TimerCallBack(unsigned long ulArgSize, void *pArg)
{
	int *pIdx;
	int nIdx, nErr;

#ifdef DEBUG_THREAD_AND_MUTEX // debugging only
	{
		unsigned long taskId = 0;
		VK_TASK_GetCurrentId(&taskId);
		HxLOG_Print("VA_OS_TimerCallBack(%x)\n", taskId);
	}
#endif

HxLOG_Print("[Enter]\n");

	pIdx = (int *)pArg;
	nIdx = *pIdx;

#if 0 // ÀÌ·± °æ¿ì´Â ¹ß»ýÇÏÁö ¾ÊÀ½
	VA_ASSERT(nIdx < VA_OS_MAX_PERIODIC_CALL);
	VA_ASSERT(sVaOsPeriodicCall[nIdx].ulDuration != 0);
	VA_ASSERT(sVaOsPeriodicCall[nIdx].pfPeriodicCall != NULL);
#endif

	nErr = sVaOsPeriodicCall[nIdx].pfPeriodicCall(sVaOsPeriodicCall[nIdx].ulVaData);
	if (nErr != kVA_OK)
	{
		HxLOG_Info("[timer idx:%d] callback return error [%d]\n", nIdx, nErr);
	}

HxLOG_Print("[Exit]\n");
}
#endif

/*--------------------------------------------------------------------------------------
				static functions for time conversion (copied from vtm_api.c for ACS1.1)
--------------------------------------------------------------------------------------*/

/*----------------------------------------------------------*
 *	Function :
 *		VA_OS_MJD2YMD
 *----------------------------------------------------------*
 */
static void
VA_OS_MJD2YMD ( UINT16 mjd, tVA_OS_Tm *ymd )
{
	UINT32 nStandardJd, nTmp1, nTmp2;
	UINT32 year, month, day;

        nStandardJd=mjd+2400001;
        nTmp1 = nStandardJd + 68569;
        nTmp2 = 4*nTmp1/146097;
        nTmp1 = nTmp1 - (146097 * nTmp2 + 3) / 4;
        year  = 4000 * (nTmp1 + 1) / 1461001;
        nTmp1 = nTmp1 - 1461 * year/4 + 31;
        month = 80 * nTmp1 / 2447;
        day   = nTmp1 - 2447 * month / 80;
        nTmp1 = month / 11;
        month = month + 2 - 12 * nTmp1;
        year  = 100 * (nTmp2 - 49) + year + nTmp1;

	ymd->uiYear	= year - 1900;
	ymd->uiMonth = month - 1;
	ymd->uiMonthDay = day;

	ymd->uiWeekDay	= (mjd + 3) % 7;
	ymd->uiYearDay = mjd - VA_OS_YMD2MJD( year, 1, 1);
}	// VA_OS_MJD2YMD()

/*----------------------------------------------------------*
 *	Function :
 *		VA_OS_YMD2MJD
 *----------------------------------------------------------*
 */
UINT16 VA_OS_YMD2MJD(UINT16 ucYear, UINT8 ucMonth, UINT8 ucMday)
{
	UINT32 nStandardJd;

        nStandardJd = ucMday - 32075
           + 1461 * (ucYear + 4800 - (14 - ucMonth)/12)/4
           + 367 * (ucMonth - 2 + (14 - ucMonth)/12*12)/12
           - 3 * ((ucYear + 4900 - (14 - ucMonth)/12)/100)/4;

        return nStandardJd-2400001;
}	// VA_OS_YMD2MJD()

/*----------------------------------------------------------*
 *	Function :
 *		VA_OS_SEC2MJD
 *----------------------------------------------------------*
 */
static UINT16
VA_OS_SEC2MJD ( UINT32 secs )
{
	return	(UINT16)(secs / (24L * 3600L)) + MJD_1970_1_1;

}	// VA_OS_SEC2MJD()

/*----------------------------------------------------------*
 *	Function :
 *		VA_OS_SEC2UTC
 *----------------------------------------------------------*
 */
static void
VA_OS_SEC2UTC ( UINT32 secs, UINT8 *utc )
{
	UINT8	hour, min, sec;
	UINT32 daysecond, hoursecond;

	daysecond = secs % 86400;
	hour = daysecond / 3600;
	hoursecond = daysecond - (hour * 3600);
	min = hoursecond / 60;
	sec = hoursecond % 60;

	*utc++ = VA_OS_BIN2BCD( hour );
	*utc++ = VA_OS_BIN2BCD( min );
	*utc   = VA_OS_BIN2BCD( sec );

}	// VA_OS_SEC2UTC()

/*----------------------------------------------------------*
 *	Function :
 *		VA_OS_UTC2YMD
 *----------------------------------------------------------*
 */
static void
VA_OS_UTC2YMD ( UINT8 *utc, tVA_OS_Tm *ymd )
{
	UINT32		secs;

	ymd->uiHour = VA_OS_BCD2BIN( utc[0] );
	ymd->uiMin = VA_OS_BCD2BIN( utc[1] );
	ymd->uiSec = VA_OS_BCD2BIN( utc[2] );

}	// VA_OS_UTC2YMD()

/*----------------------------------------------------------*
 *	Function :
 *		VA_OS_BCD2BIN
 *----------------------------------------------------------*
 */
static UINT8
VA_OS_BCD2BIN ( UINT8 bcd )
{
	return	( (bcd >> 4)*10 + (bcd & 0x0F) );

}	// VA_OS_BCD2BIN()

/*----------------------------------------------------------*
 *	Function :
 *		va_BIN2BCD
 *----------------------------------------------------------*
 */
static UINT8
VA_OS_BIN2BCD ( UINT8 bin )
{
	return	( ((bin / 10) << 4) | (bin % 10) );

}	// va_BIN2BCD()


/*--------------------------------------------------------------------------------------
				VIACCESS ACS 3.0 API
--------------------------------------------------------------------------------------*/

/*
	VA_OS_Alloc
		allocate a memory
	arguments
		uiSize
	return
		pointer to the allocated memory or NULL
*/
void *VA_OS_Alloc(UINT32 uiSize)
{
#ifdef USE_VA_MEMORY_MANAGER
	void *pMem = NULL;
	int i, idx;

	HxLOG_Print("[Enter]\n");

	for (i = 0, idx = s_stAllocTablePtr + 1; i < VA_OS_ALLOC_TABLE_SIZE; i++, idx++)
	{
		if (idx >= VA_OS_ALLOC_TABLE_SIZE)
		{
			idx = 0;
		}

		if (s_stAllocTable[idx].pucMemory == NULL)
		{
			// allocate memory
			pMem = (void *)VA_MEM_Alloc(uiSize);
			if (pMem != NULL)
			{
				s_stAllocTable[idx].pucMemory = pMem;
				s_stAllocTable[idx].ulSize = uiSize;
				break;
			}
		}
	}

	HxLOG_Print("[Exit]\n");
	return pMem;
#else
	void *pMem = NULL;

	HxLOG_Print("[Enter]\n");

	// allocate memory
	pMem = (void *)VA_MEM_Alloc(uiSize); // burst mem alloc test ½Ã¿¡ °¡¿ëÇÑ ¸Þ¸ð¸®°¡ ¾ø¾î NULL ÀÌ ¸®ÅÏµÉ ¼ö ÀÖÀ¸¹Ç·Î (¶ÇÇÑ ÀÌ·± °æ¿ì NULL ¸®ÅÏµÇ¾î¾ß¸¸ ÇÏ°í) ¾Æ·¡¿Í °°ÀÌ Ã³¸®ÇØ ÁÖ¾î¾ß ÇÑ´Ù.

	if (pMem)
		VA_memset(pMem, 0, uiSize);

	HxLOG_Print("[Exit]\n");
	return pMem;
#endif
}

/*
	VA_OS_Free
		free a memory
	arguments
		ptr
	return
		n/a
*/
void VA_OS_Free(void *ptr)
{
#ifdef USE_VA_MEMORY_MANAGER
	int i;

	HxLOG_Print("[Enter]\n");

	if (ptr == NULL)
		return;

	for (i = 0; i < VA_OS_ALLOC_TABLE_SIZE; i++)
	{
		if (s_stAllocTable[i].pucMemory == (HUINT8*)ptr)
		{
			// free memory
			VA_MEM_Free(ptr);
			s_stAllocTable[i].pucMemory = NULL;
			s_stAllocTable[i].ulSize = 0;
			break;
		}
	}
	HxLOG_Print("[Exit]\n");
#else
	HxLOG_Print("[Enter]\n");

	if (ptr == NULL)
		return;

	// free memory
	VA_MEM_Free(ptr);
	HxLOG_Print("[Exit]\n");
#endif
}

/*
	VA_OS_Printf
		print formatted output to a unique external communcation port (preferably a serial port)
	arguments
		pFormat
	return
		number of printed characters
*/
INT VA_OS_Printf(const char *pFormat, ...)
{
	va_list vl;

HxLOG_Print("[Enter]\n");

	va_start(vl, pFormat);
	vsprintf(s_szVaPrintBuffer, pFormat, vl);
	va_end(vl);

//	DDI_UART_Print("%s", s_szVaPrintBuffer);
#ifndef CONFIG_DEBUG // #ifndef DEBUG
	DI_UART_PrintRelease("[01;35m%s[00m", s_szVaPrintBuffer);
#else
	DI_UART_Print("[01;35m%s[00m", s_szVaPrintBuffer);
#endif

HxLOG_Print("[Exit]\n");
	return strlen(s_szVaPrintBuffer);
}

/*
	VA_OS_Getc
		read a character from the external communication port
	arguments
		uiDuration
	return
		ASCII
		kVA_TIMEOUT
*/
INT VA_OS_Getc(UINT32 uiDuration)
{
	HUINT8 cChar = 0;
	HERROR hError;

HxLOG_Print("[Enter(%d)]\n", uiDuration);

	if (uiDuration == kVA_OS_NO_WAIT)
	{
		// get character
		// serial input must be stored in a FIFO buffer (at least 16 characters)

		// if any character is not available
#if defined(CONFIG_PROD_VA5SD)
		hError = DI_UART_ReadByteNoWaitForViaccessGetc(&cChar);
#elif defined(CONFIG_PROD_CPU_BCM)
		hError = DRV_UART_ReadByteNoWaitForViaccessGetc(&cChar);
		if ((cChar == 0xff) || (cChar == 0))
		{
			hError = ERR_FAIL;
		}
#else
		// org
		hError = DI_UART_ReadByte_NoWait(&cChar);
#endif
		if(hError != ERR_OK)
		{
			cChar = kVA_TIMEOUT;
		}
	}
	else	// uiDuration == kVA_OS_WAIT_FOREVER
	{
 		// get character
		hError = DI_UART_ReadByte(&cChar);
		if(hError != ERR_OK)
		{
			cChar = kVA_TIMEOUT;
		}
	}

HxLOG_Print("[Exit(%x)]\n", cChar);
	return (INT)cChar;
}

/*
	VA_OS_InitializeSemaphore
		init a semaphore
	arguments
		pSemaphore
		uiValue
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_InitializeSemaphore(tVA_OS_Semaphore *pSemaphore, UINT32 uiValue)
{
	UINT32 *pulSemData;
	char strName[5];
	static int nSeq = 0;

	HxLOG_Print("[Enter(%d)]\n", uiValue);

	if (pSemaphore == NULL)
	{
		HxLOG_Error("pSemaphore = NULL\n");
		return kVA_INVALID_PARAMETER;
	}

	// init the semaphore
	// do not allocate memory to pSemaphore
	pulSemData = (UINT32 *)pSemaphore;
	pulSemData[0] = VA_OS_VALID_DATA;
	pulSemData[1] = 0;

	strName[0] = 'S';
	strName[1] = 'E';
	strName[2] = '0' + nSeq / 10;
	strName[3] = '0' + nSeq % 10;
	strName[4] = 0;
	nSeq++;

	if (uiValue == 0)
		VA_SEM_Create(strName, &pulSemData[1]);
	else
		VA_SEM_CreateWithCount(strName ,&pulSemData[1], uiValue);

//	pulSemData[2] = 0;

	HxLOG_Print("[Exit][%s, %x, %d] \n", strName, pulSemData[1], uiValue);
	return kVA_OK;
}

/*
	VA_OS_DeleteSemaphore
		delete a semaphore
	arguments
		pSemaphore
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_DeleteSemaphore(tVA_OS_Semaphore *pSemaphore)
{
	UINT32 *pulSemData;

	HxLOG_Print("[Enter]\n");

	if (pSemaphore == NULL)
	{
		HxLOG_Error("pSemaphore = NULL\n");
		return kVA_INVALID_PARAMETER;
	}

	pulSemData = (UINT32 *)pSemaphore;

	HxLOG_Info("[Semaphore Id = %x]\n", pulSemData[1]);

	// pSemaphore doesn't correspond to an initialized semaphore
	if (pulSemData[0] != VA_OS_VALID_DATA)
	{
		HxLOG_Error("invalid semaphore\n");
		return kVA_INVALID_PARAMETER;
	}

	// delete the semaphore
	// do not free pSemaphore
	VA_SEM_Destroy(pulSemData[1]);
	pulSemData[0] = 0;
	pulSemData[1] = 0;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_OS_AcquireSemaphore
		acquire a semaphore
	arguments
		pSemaphore
		uiDuration
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_AcquireSemaphore(tVA_OS_Semaphore *pSemaphore, UINT32 uiDuration)
{
	UINT32 *pulSemData;

HxLOG_Print("[Enter]\n");

	if (pSemaphore == NULL)
	{
		HxLOG_Error("pSemaphore = NULL\n");
		return kVA_INVALID_PARAMETER;
	}

	pulSemData = (UINT32 *)pSemaphore;

HxLOG_Info("[Semaphore Id = %x]\n", pulSemData[1]);

	// pSemaphore doesn't correspond to an initialized semaphore
	if (pulSemData[0] != VA_OS_VALID_DATA)
	{
		HxLOG_Error("invalid semaphore\n");
		return kVA_INVALID_PARAMETER;
	}

	if (uiDuration != kVA_OS_WAIT_FOREVER)
	{
		HxLOG_Error("wrong uiDuration = %d\n", uiDuration);
		return kVA_INVALID_PARAMETER;
	}

	// acquire a semaphore
	VA_ASSERT(pulSemData[1] != 0);
	VA_SEM_Get(pulSemData[1]);
//	pulSemData[2]++;

HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_OS_ReleaseSemaphore
		acquire a semaphore
	arguments
		pSemaphore
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_ReleaseSemaphore(tVA_OS_Semaphore *pSemaphore)
{
	UINT32 *pulSemData;

HxLOG_Print("[Enter]\n");

	if (pSemaphore == NULL)
	{
		HxLOG_Error("pSemaphore = NULL\n");
		return kVA_INVALID_PARAMETER;
	}

	pulSemData = (UINT32 *)pSemaphore;

HxLOG_Info("[Semaphore Id = %x]\n", pulSemData[1]);

	// pSemaphore doesn't correspond to an initialized semaphore
	if (pulSemData[0] != VA_OS_VALID_DATA)
	{
		HxLOG_Error("invalid semaphore\n");
		return kVA_INVALID_PARAMETER;
	}

	// release a semaphore
	VA_ASSERT(pulSemData[1] != 0);
	VA_SEM_Release(pulSemData[1]);
//	if (pulSemData[2] > 0)
//		pulSemData[2]--;

HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_OS_InitializeMutex
		initialize a mutex
	arguments
		pMutex
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_InitializeMutex(tVA_OS_Mutex *pMutex)
{
	UINT32 *pulMutexData;
	char strName[5];
	static int nMutexSeq = 0;

#ifdef DEBUG_THREAD_AND_MUTEX // debugging only
	{
		unsigned long taskId = 0;
		VK_TASK_GetCurrentId(&taskId);
		HxLOG_Print("VA_OS_InitializeMutex(%x) : %x\n", taskId, pMutex);
	}
#endif

	HxLOG_Print("[Enter][%x]\n", pMutex);

	if (pMutex == NULL)
	{
		HxLOG_Error("pMutex = NULL\n");
		return kVA_INVALID_PARAMETER;
	}

	// initialize a mutex
	// do not allocate memory to pMutex
	pulMutexData = (UINT32 *)pMutex;
	pulMutexData[eVA_OS_MUTEX_VALID] = VA_OS_VALID_DATA;
	pulMutexData[eVA_OS_MUTEX_ID] = 0;
	pulMutexData[eVA_OS_MUTEX_COUNT] = 0;

	strName[0] = 'M';
	strName[1] = 'U';
	strName[2] = '0' + nMutexSeq / 10;
	strName[3] = '0' + nMutexSeq % 10;
	strName[4] = 0;
	nMutexSeq++;

	if (VK_MUTEX_Create (&pulMutexData[eVA_OS_MUTEX_ID], strName) != VK_OK)
	{
		HxLOG_Error("fail to create\n");
		return kVA_INVALID_PARAMETER;
	}

	HxLOG_Print("[Exit][%x] \n", pulMutexData[eVA_OS_MUTEX_ID]);
	return kVA_OK;
}

/*
	VA_OS_DeleteMutex
		initialize a mutex
	arguments
		pMutex
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_DeleteMutex(tVA_OS_Mutex *pMutex)
{
	UINT32 *pulMutexData;

#ifdef DEBUG_THREAD_AND_MUTEX // debugging only
	{
		unsigned long taskId = 0;
		VK_TASK_GetCurrentId(&taskId);
		HxLOG_Print("VA_OS_DeleteMutex(%x) : %x\n", taskId, pMutex);
	}
#endif

	HxLOG_Print("[Enter(%x)]\n", pMutex);

	// pMutex doesn't correspond to an initialized mutex
	if (pMutex == NULL)
	{
		HxLOG_Error("pMutex = NULL\n");
		return kVA_INVALID_PARAMETER;
	}

	pulMutexData = (UINT32 *)pMutex;
	HxLOG_Info("[Mutex Id = %x]\n", pulMutexData[eVA_OS_MUTEX_ID]);

	// pMutex doesn't correspond to an initialized mutex
	if (pulMutexData[eVA_OS_MUTEX_VALID] != VA_OS_VALID_DATA)
	{
		HxLOG_Error("invalid mutex\n");
		return kVA_INVALID_PARAMETER;
	}

#if defined(USE_MW_MUTEX_COUNTER)
	if (pulMutexData[eVA_OS_MUTEX_COUNT])
	{
		HxLOG_Error("unlock first(%d)\n", pulMutexData[eVA_OS_MUTEX_COUNT]);
		return kVA_INVALID_PARAMETER;
	}
#endif

	// delete a mutex
	// do not free pMutex
	if (VK_MUTEX_Destroy (pulMutexData[eVA_OS_MUTEX_ID]) != VK_OK)
	{
		HxLOG_Error("fail to delete\n");
		return kVA_INVALID_PARAMETER;
	}

	pulMutexData[eVA_OS_MUTEX_VALID] = 0;
	pulMutexData[eVA_OS_MUTEX_ID] = 0;
	pulMutexData[eVA_OS_MUTEX_COUNT] = 0;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_OS_LockMutex
		lock a mutex
	arguments
		pMutex
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_LockMutex(tVA_OS_Mutex *pMutex)
{
	UINT32 *pulMutexData;

#ifdef DEBUG_THREAD_AND_MUTEX // debugging only
	{
		unsigned long taskId = 0;
		VK_TASK_GetCurrentId(&taskId);
		HxLOG_Print("VA_OS_LockMutex(%x) : %x\n", taskId, pMutex);
	}
#endif

HxLOG_Print("[Enter(%x)]\n", pMutex);

	// pMutex doesn't correspond to an initialized mutex
	if (pMutex == NULL)
	{
		HxLOG_Error("pMutex = NULL\n");
		return kVA_INVALID_PARAMETER;
	}

	pulMutexData = (UINT32 *)pMutex;
HxLOG_Info("[Mutex Id = %x]\n", pulMutexData[eVA_OS_MUTEX_ID]);

	// pMutex doesn't correspond to an initialized mutex
	if (pulMutexData[eVA_OS_MUTEX_VALID] != VA_OS_VALID_DATA)
	{
		HxLOG_Error("invalid mutex\n");
		return kVA_INVALID_PARAMETER;
	}

	// lock a mutex
	VA_ASSERT(pulMutexData[eVA_OS_MUTEX_ID] != 0);
	VK_MUTEX_Lock (pulMutexData[eVA_OS_MUTEX_ID]);

#if defined(USE_MW_MUTEX_COUNTER)
	pulMutexData[eVA_OS_MUTEX_COUNT]++;
#endif

#if defined(CONFIG_PROD_CPU_ST) && defined(OS_LINUX)
	if (s_ulMutexSleepCnt++ >= 200)
	{
		VK_TASK_Sleep(1);
		s_ulMutexSleepCnt = 0;
	}
#endif

HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_OS_UnlockMutex
		unlock a mutex
	arguments
		pMutex
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_UnlockMutex(tVA_OS_Mutex *pMutex)
{
	UINT32 *pulMutexData;

#ifdef DEBUG_THREAD_AND_MUTEX // debugging only
	{
		unsigned long taskId = 0;
		VK_TASK_GetCurrentId(&taskId);
		HxLOG_Print("VA_OS_UnlockMutex(%x) : %x\n", taskId, pMutex);
	}
#endif

HxLOG_Print("[Enter(%x)]\n", pMutex);

	// pMutex doesn't correspond to an initialized mutex
	if (pMutex == NULL)
	{
		HxLOG_Error("pMutex = NULL\n");
		return kVA_INVALID_PARAMETER;
	}

	pulMutexData = (UINT32 *)pMutex;
HxLOG_Info("[Mutex Id = %x]\n", pulMutexData[eVA_OS_MUTEX_ID]);

	// pMutex doesn't corresp/ond to an initialized mutex
	if (pulMutexData[eVA_OS_MUTEX_VALID] != VA_OS_VALID_DATA)
	{
		HxLOG_Error("invalid mutex\n");
		return kVA_INVALID_PARAMETER;
	}

#if 0 //defined(USE_MW_MUTEX_COUNTER)
// ÃÊ±â°³¹ß´Ü°è¿¡¼­ mutex°¡ Á¦´ë·Î µ¿ÀÛÇÏÁö ¾Ê¾Æ¼­ Ãß°¡ÇßÀ¸³ª, ÇöÀç mutex µ¿ÀÛ¿¡ ¹®Á¦°¡ ¾ø°í, ¾Æ·¡ ÄÚµå°¡ dead-lock ¹®Á¦¸¦ À¯¹ßÇÏ¿© »èÁ¦ÇÔ
	if (pulMutexData[eVA_OS_MUTEX_COUNT] == 0)
	{
		HxLOG_Error("lock first\n");
		return kVA_INVALID_PARAMETER;
	}
#endif

	// unlock a mutex
	VA_ASSERT(pulMutexData[eVA_OS_MUTEX_ID] != 0);
	if (VK_MUTEX_Unlock (pulMutexData[eVA_OS_MUTEX_ID]) != VK_OK)
	{
		HxLOG_Error("fail to unlock\n");
		return kVA_INVALID_PARAMETER;
	}

#if defined(USE_MW_MUTEX_COUNTER)
	pulMutexData[eVA_OS_MUTEX_COUNT]--;
#endif

// ¼öÁ¤ #3. Driver ÂÊ Task priority Æ©´×À¸·Î ¹®Á¦ ÇØ°áµÊÀ¸·Î½á ¾Æ·¡ sleep Àº ¿ÏÀü Á¦°Å.
#if 0 // #if defined(CONFIG_PROD_VA5SD) // ¼öÁ¤ #1. VA-5SD ¿¡¼­ ¸±¸®Áî¿ë ACS »ç¿ë½Ã ´Ù¸¥ MW Å¸½ºÅ©°¡ Á¤»ó µ¿ÀÛÇÏÁö ¾Ê´Â ¹®Á¦°¡ ÀÖ¾î Ãß°¡ÇÔ. Task priority Æ©´×À¸·Î´Â ÇØ°áÀÌ ¾ÈµÊ -.
//#ifndef CONFIG_MW_CAS_VIACCESS_DRIVER_TEST // ¼öÁ¤ #2. DTA library »ç¿ë½Ã¿¡µµ sleep À» ÁÖµµ·Ï ÇÔ (ÀÌ °æ¿ì va_pi_api.c ÀÇ USE_SEMA_TO_ALL_PI_FUNCS_CALLED_BY_VA define ÀÌ ÇÔ²² »ç¿ëµÇ¾î¾ß Å×½ºÆ®»ó¿¡ ¹®Á¦°¡ ¾ø´Ù)
{
	static long cnt = 0;
//	if ((cnt++ % 10000000) == 0) // signal locking ¾ÈµÊ -> driver channel task °¡ va main thread º¸´Ù priority °¡ ³·¾ÒÀ½.
//	if ((cnt++ % 1000000) == 0) // signal locking µÇ³ª ´ÊÀ½ ("The channel is scrambled or not available." ¹è³Ê Àá½Ã Ãâ·ÂµÊ)
	if ((cnt++ % 100000) == 0) // dolby channel ¿¡¼­ sound mute µÈ´Ù°í... -> ¿ª½Ã driver ÂÊ task priority ¹®Á¦.
		VK_TASK_Sleep(1);
}
//#endif
#endif

#if defined(CONFIG_PROD_CPU_BCM)
#if defined(CONFIG_MW_CAS_VIACCESS_DRIVER_TEST)
	// OS Suite, OS_MUT_LOCK_UNLOCK_2 Case Pass¸¦ À§ÇØ¼­ ÇÊ¿ä
	// unlock½Ã error³ª¸é¼­ vam ¸ÔÅëÀÌ µÇ¾î ¹ö¸®´Â ¹®Á¦ ¼öÁ¤.. Release mode¿¡¼­µµ ¿­¾î¾ß ÇÒ Áö Å×½ºÆ® ÇÊ¿ä
	VK_TASK_Sleep(1);
#endif
#endif

HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_OS_Sleep
		sleep
	arguments
		uiDuration: time (msec)
	return
		n/a
*/
void VA_OS_Sleep(UINT32 uiDuration)
{
HxLOG_Print("[Enter][%d]\n", uiDuration);

#if defined(CONFIG_PROD_VA5SD) // If uiDuration is equal to 0, the calling thread is moved to the end of the queue by the OS scheduler and a new thread gets to run --> VK_TASK_Sleep(0) ÀÌ ÀÌ·¸Áö ¾Ê¾Æ Ãß°¡ÇÔ.
	if (uiDuration == 0)
	{
		HxLOG_Info("uiDuration is zero\n");
		uiDuration = 1;
	}
#endif

#if defined(CONFIG_PROD_CPU_BCM)
	if (uiDuration == 0)
	{
		HxLOG_Info("uiDuration is zero\n");
		sched_yield();
		return;
	}
#endif

	// sleep
	VK_TASK_Sleep(uiDuration);
HxLOG_Print("[Exit]\n");
}

/*
	VA_OS_StartPeriodicCall
		request to the driver to periodically call the VA callback function given as a parameter
	arguments
		uiDurationPeriod: msec
		pfPeriodicCallFunction
		dwViaccessData: this VA data is returned in the callback function
	return
		dwHandle
		kVA_ILLEGAL_HANDLE
*/
DWORD VA_OS_StartPeriodicCall(UINT32 uiDurationPeriod, tVA_OS_PeriodicCallFct pfPeriodicCallFunction, DWORD dwViaccessData)
{
	int i;
#ifdef  USE_PERIODIC_CALL_TASK
	UINT32 ulCurTime;
#endif

	HxLOG_Print("[Enter][%d]\n", uiDurationPeriod);

	if (uiDurationPeriod == 0 || uiDurationPeriod < 100)
	{
		HxLOG_Error("wrong uiDurationPeriod = %d\n", uiDurationPeriod);
		return kVA_ILLEGAL_HANDLE;
	}

	// start periodic call
#ifdef  USE_PERIODIC_CALL_TASK
	ulCurTime = VK_TIMER_GetSystemTick(); // * 1000 / VK_TIMER_GetTickFreq();
#endif
	for (i = 0; i < VA_OS_MAX_PERIODIC_CALL; i++)
	{
		if (sVaOsPeriodicCall[i].ulDuration == 0)
		{
			sVaOsPeriodicCall[i].ulVaData = dwViaccessData;
			sVaOsPeriodicCall[i].pfPeriodicCall = pfPeriodicCallFunction;
#ifdef  USE_PERIODIC_CALL_TASK
			sVaOsPeriodicCall[i].ulNextWakeup = ulCurTime + uiDurationPeriod;
			sVaOsPeriodicCall[i].ulDuration = uiDurationPeriod;
#else
			sVaOsPeriodicCall[i].ulDuration = uiDurationPeriod;
			VK_TIMER_EventEvery(uiDurationPeriod, VA_OS_TimerCallBack, &i, sizeof(int), &sVaOsPeriodicCall[i].ulTimerId);
#endif
			break;
		}
	}

#ifdef DEBUG_THREAD_AND_MUTEX // debugging only
	{
		unsigned long taskId = 0;
		VK_TASK_GetCurrentId(&taskId);
		HxLOG_Print("VA_OS_StartPeriodicCall(%x) : %x -> %x\n", taskId, pfPeriodicCallFunction, i);
	}
#endif

	// if failed
	if (i == VA_OS_MAX_PERIODIC_CALL)
	{
		HxLOG_Error("no empty slot\n");
		return kVA_ILLEGAL_HANDLE;
	}

	HxLOG_Print("[Exit][%d]\n", i);
	return i;
}

/*
	VA_OS_StopPeriodicCall
		stop periodic call
	arguments
		dwHandle: handle returned by VA_OS_StartPeriodicCall()
	return
		kVA_OK
		kVA_INVALID_PARAMETER
*/
INT VA_OS_StopPeriodicCall(DWORD dwHandle)
{
#ifdef DEBUG_THREAD_AND_MUTEX // debugging only
	{
		unsigned long taskId = 0;
		VK_TASK_GetCurrentId(&taskId);
		HxLOG_Print("VA_OS_StopPeriodicCall(%x) : %x\n", taskId, dwHandle);
	}
#endif

	HxLOG_Print("[Enter][%d] \n", dwHandle);

	// dwHandle doesn't correspond to a started periodic call process
	if (dwHandle >= VA_OS_MAX_PERIODIC_CALL || sVaOsPeriodicCall[dwHandle].ulDuration == 0)
	{
		HxLOG_Error("wrong dwHandle = %x\n", dwHandle);
		return kVA_INVALID_PARAMETER;
	}

	// stop periodic call
#ifndef  USE_PERIODIC_CALL_TASK
	VK_TIMER_Cancel(sVaOsPeriodicCall[dwHandle].ulTimerId);
#endif
	sVaOsPeriodicCall[dwHandle].ulDuration = 0;

	HxLOG_Print("[Exit]\n");
	return kVA_OK;
}

/*
	VA_OS_GetTime
		get GMT time
	arguments
		pTime: allocated and freed by VA
	return
		n/a
*/
void VA_OS_GetTime(tVA_OS_Time *pTime)
{
	HxLOG_Print("[Enter][%x] \n", pTime);

	if (pTime == NULL)
		return;

	*pTime = (tVA_OS_Time)VA_OS_Adapt_GetTime();

	HxLOG_Print("[Exit]\[%ld]\n", *pTime);
}

/*
	VA_OS_TimeToTm
		translate time value from tVA_OS_Time to tVA_OS_Tm
	arguments
		pTime: allocated and freed by VA
		pTm: allocated and freed by VA
	return
		n/a
*/
void VA_OS_TimeToTm(const tVA_OS_Time *pTime, tVA_OS_Tm *pTm)
{
	UINT16 usMjd;
	UINT8 aucUtc[3];

	HxLOG_Print("[Enter]\n");

	if (pTime == NULL || pTm == NULL)
		return;

	// translate time to tm
	// time functions are copied from vtm_api.c (for ACS 1.1) and modified for ACS 3.0
	// tm_time_to_gmt (	const va_time	*current_time, gmt_time	*current_gmt_time )
	usMjd = VA_OS_SEC2MJD(*pTime);
	VA_OS_SEC2UTC(*pTime, aucUtc );

	VA_OS_MJD2YMD( usMjd, pTm );
	VA_OS_UTC2YMD( aucUtc, pTm );

//DDI_UART_Print("%d(%x) --> %d-%d-%d (%d/%d) %d:%d:%d\n", *pTime, *pTime, pTm->uiYear, pTm->uiMonth, pTm->uiMonthDay, pTm->uiWeekDay, pTm->uiYearDay, pTm->uiHour, pTm->uiMin, pTm->uiSec);
	HxLOG_Print("[Exit]\n");
}

/*
	VA_OS_TmToTime
		translate time value from tVA_OS_Tm to tVA_OS_Time
	arguments
		pTm: allocated and freed by VA
		pTime: allocated and freed by VA
	return
		n/a
*/
void VA_OS_TmToTime(const tVA_OS_Tm *pTm, tVA_OS_Time *pTime)
{
	UINT16 usMjd;

	HxLOG_Print("[Enter]\n");

	if (pTime == NULL || pTm == NULL)
		return;

	// pTm has wrong value (e.g. uiMonth = 13), then *pTime = (tVA_OS_Time)(-1)
	// typedef struct
	// {
	// 	UINT32 uiSec ;           --> [0, 59]
	// 	UINT32 uiMin ;           --> [0, 59]
	// 	UINT32 uiHour ;         --> [0, 23]
	// 	UINT32 uiMonthDay ; --> [1, 31]
	// 	UINT32 uiMonth ;       --> [0, 11]
	// 	UINT32 uiYear ;          --> since 1900
	// 	UINT32 uiWeekDay ;  --> [0, 6]     (The VA_OS_TmToTime function ignores the specified contents of the uiWeekDay and uiYearDay)
	// 	UINT32 uiYearDay ;    --> [0, 365] (The VA_OS_TmToTime function ignores the specified contents of the uiWeekDay and uiYearDay)
	// } tVA_OS_Tm;
	HxLOG_Info("M/D H:M:S = %d/%d  %d:%d:%d\n", pTm->uiMonth, pTm->uiMonthDay, pTm->uiHour, pTm->uiMin, pTm->uiSec);
	// if (pTm->uiHour > 24 || pTm->uiMin > 60 || pTm->uiSec > 60 || pTm->uiMonth > 12 || pTm->uiMonthDay > 31)
	if (pTm->uiHour > 23 || pTm->uiMin > 59 || pTm->uiSec > 59 || pTm->uiMonth > 11 || pTm->uiMonthDay == 0 || pTm->uiMonthDay > 31) // Viaccess Driver Test ÀÇ OS Suite, OS_TRANS_ERR_7 Ç×¸ñ¿¡¼­ÀÇ fail ¼öÁ¤.
	{
		*pTime = (tVA_OS_Time)(-1);
		return;
	}

	// translate tm to time
	// time functions are copied from vtm_api.c (for ACS 1.1) and modified for ACS 3.0
	// tm_time_from_gmt (	const gmt_time	*current_gmt_time , va_time			*current_time )

	usMjd = VA_OS_YMD2MJD(pTm->uiYear + 1900, pTm->uiMonth + 1, pTm->uiMonthDay);

	*pTime = (usMjd - MJD_1970_1_1) * 24 * 3600
			   + pTm->uiHour * 3600
			   + pTm->uiMin * 60
			   + pTm->uiSec;

//DDI_UART_Print("%d(%x) <-- %d-%d-%d (%d/%d) %d:%d:%d\n", *pTime, *pTime, pTm->uiYear, pTm->uiMonth, pTm->uiMonthDay, pTm->uiWeekDay, pTm->uiYearDay, pTm->uiHour, pTm->uiMin, pTm->uiSec);

	HxLOG_Print("[Exit]\n");
}

/*
	VA_OS_GetRunTime
		processor runtime starting from the moment the STB is switched on
	arguments
		pRunTime: allocated and freed by VA
	return
		n/a
*/
void VA_OS_GetRunTime(tVA_OS_RunTime *pRunTime)
{
#ifdef DEBUG_THREAD_AND_MUTEX // debugging only
	{
		unsigned long taskId = 0;
		VK_TASK_GetCurrentId(&taskId);
		HxLOG_Print("VA_OS_GetRunTime(%x)\n", taskId);
	}
#endif

	HxLOG_Print("[Enter]\n");

	if (pRunTime == NULL)
		return;

	// get run time
#ifdef USE_TICK_TO_GET_RUNTIME
	pRunTime->uiMilliSec = VK_TIMER_GetSystemTick(); // * 1000 / VK_TIMER_GetTickFreq();
	pRunTime->uiMicroSec = 0;
#else
	pRunTime->uiMilliSec = s_ulRuntime * 100;		// s_ulRuntime's unit : 100msec
	pRunTime->uiMicroSec = 0;
#endif

	HxLOG_Print("[Exit][%x, %x]\n", pRunTime->uiMilliSec, pRunTime->uiMilliSec);
}

