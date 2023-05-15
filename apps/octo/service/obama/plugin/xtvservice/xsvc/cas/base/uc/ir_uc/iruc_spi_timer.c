/*
 * (c) 2011-2012 Humax Co., Ltd.
 * This program is produced by Humax Co., Ltd. ("Humax") and
 * the proprietary Software of Humax and its licensors. Humax provides you, as an Authorized Licensee,
 * non-assignable, non-transferable and non-exclusive license to use this Software.
 * You acknowledge that this Software contains valuable trade secrets of Humax and by using this Software
 * you agree to the responsibility to take all reasonable efforts to protect the any information
 * you receive from Humax. You are not permitted to duplicate, modify, distribute, sell or lease and
 * reverse engineer or extract the source code of this Software unless you have Humax's written permission to do so.
 * If you have no authorized license, discontinue using this Software immediately.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND HUMAX MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS,
 * IMPLIED OR STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.
 * IN NO EVENT SHALL HUMAX BE LIABLE FOR LOST PROFITS, REVENUES, OR DATA, FINANCIAL LOSSES OR INDIRECT, SPECIAL,
 * CONSEQUENTIAL, EXEMPLARTY OR PUNITIVE DAMAGES WHATSOEVER RELATING TO YOUR USE OR INABILITY TO USE THE SOFTWARE.

 * This License is effective until terminated. You may terminate this License at any time by destroying all copies
 * of the Software including all documentation. This License will terminate immediately without notice from Humax
 * to you if you fail to comply with any provision of this License. Upon termination, you must destroy all copies
 * of the Software and all documentation.

 * The laws of the Republic of Korea will apply to any disputes arising out of or relating to this Copyright Notice.
 * All claims arising out of or relating to this Copyright Notice will be litigated in the Seoul Central District Court,
 * in the Republic of Korea.
*/

#include "octo_common.h"
#include "UniversalClient_Common_SPI.h"
#include "UniversalClient_Common_API.h"

#include "iruc_spi.h"

#define ______SPI_TIMER________________

typedef struct
{
	uc_uint32			timerID;
	HUINT8				status;		// 0 stop, 1 start
	uc_timer_info			spi_timer_open_st;
} SPI_TIMER_INFO_STRUCT;


#define IRUC_SPI_TIMER_MAX_NUM		16

/********************************************************************
*	Static variables
********************************************************************/
static SPI_TIMER_INFO_STRUCT 		s_stSpi_Timer_Info[IRUC_SPI_TIMER_MAX_NUM];
static HULONG					s_ulSpiTimerSem;

/********************************************************************
*	Static functions Prototypes
*	The following functions will be used as inner functions
********************************************************************/


/********************************************************************
*	function name :	 local_SpiTimer_Callback
*	input : timer ID, parameter.
*	output : none
*	description :
********************************************************************/
static void local_SpiTimer_Callback(uc_uint32 timerId, void * timerContext)
{

	HUINT32	i;

	for( i = 0; i < IRUC_SPI_TIMER_MAX_NUM; i++ )
	{
		if( s_stSpi_Timer_Info[i].timerID == timerId )
		{
			if( s_stSpi_Timer_Info[i].spi_timer_open_st.isRepetitive == FALSE )
			{
				s_stSpi_Timer_Info[i].status = 0;	/* STOP */
			}

			if(s_stSpi_Timer_Info[i].spi_timer_open_st.onTimeCallback!=NULL)
				
			{
			       s_stSpi_Timer_Info[i].spi_timer_open_st.onTimeCallback(s_stSpi_Timer_Info[i].timerID, s_stSpi_Timer_Info[i].spi_timer_open_st.timerContext);
			       HxLOG_Print(" == Timer Call back == \n");
			}	
			return;
		}
	}

}

/** @defgroup timerspi Cloaked CA Agent Timer SPIs
 *  Cloaked CA Agent Timer SPIs
 *
 *  Cloaked CA Agent will do timing via these SPIs.
 *
 *  @{
 */

/**
 * Open a timer.
 *
 * @param[out] pTimerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @param[in] pTimerInfo pointer of data transmitted to timer driver, pTimerInfo
 *            points to structure ::uc_timer_info.
 *
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Timer_Open(uc_uint32* pTimerId,uc_timer_info* pTimerInfo)
{
	HUINT32 i;

	VK_SEM_Get(s_ulSpiTimerSem);

	for( i = 0; i < IRUC_SPI_TIMER_MAX_NUM; i++ )
	{
		if( s_stSpi_Timer_Info[i].timerID == UC_INVALID_HANDLE_VALUE )
			break;
	}

	if( i == IRUC_SPI_TIMER_MAX_NUM )
	{
		HxLOG_Print(" no more timer ID space\n");
		VK_SEM_Release(s_ulSpiTimerSem);
		return UC_ERROR_TOO_MANY_HANDLES;
	}

	HxLOG_Print(" Open - timeout(0x%X), isRepetitive(%d)\n", (HUINT32)pTimerInfo->timeout, pTimerInfo->isRepetitive);
	// ONE TIME.
	if( pTimerInfo->isRepetitive == FALSE )
	{

		if( VK_OK != VK_TIMER_EventAfter(pTimerInfo->timeout,
								(void *)local_SpiTimer_Callback,
								(void *)NULL,
								0,
								(unsigned long *)pTimerId) )
		{
			HxLOG_Error(" Timer Once Start Error\n") ;
			VK_SEM_Release(s_ulSpiTimerSem);
			return UC_ERROR_UNABLE_TO_CREATE;
		}
		else
		{
			if(pTimerInfo->onTimeCallback != NULL)
			{
				s_stSpi_Timer_Info[i].spi_timer_open_st.onTimeCallback = pTimerInfo->onTimeCallback;
			}
			s_stSpi_Timer_Info[i].spi_timer_open_st.timerContext = pTimerInfo->timerContext;
			s_stSpi_Timer_Info[i].spi_timer_open_st.isRepetitive = pTimerInfo->isRepetitive;
			s_stSpi_Timer_Info[i].spi_timer_open_st.timeout = pTimerInfo->timeout;
			s_stSpi_Timer_Info[i].status = 1;
			s_stSpi_Timer_Info[i].timerID = *pTimerId;
			HxLOG_Print(" Open : ONCE - Timer idx[%d], TimerId:%d \n", i,s_stSpi_Timer_Info[i].timerID );
		}
	}
	// CONTINUE
	else
	{
		if( VK_OK != VK_TIMER_EventEvery(pTimerInfo->timeout,
								(void *)local_SpiTimer_Callback,
								NULL,
								0,
								(unsigned long *)pTimerId) )
		{
			HxLOG_Error(" Timer Every Start Error\n") ;
			VK_SEM_Release(s_ulSpiTimerSem);
			return UC_ERROR_UNABLE_TO_CREATE;
		}
		else
		{
			if(pTimerInfo->onTimeCallback != NULL)
			{
				s_stSpi_Timer_Info[i].spi_timer_open_st.onTimeCallback = pTimerInfo->onTimeCallback;
			}
			s_stSpi_Timer_Info[i].spi_timer_open_st.timerContext = pTimerInfo->timerContext;
			s_stSpi_Timer_Info[i].spi_timer_open_st.isRepetitive = pTimerInfo->isRepetitive;
			s_stSpi_Timer_Info[i].spi_timer_open_st.timeout = pTimerInfo->timeout;
			s_stSpi_Timer_Info[i].status = 1;
			s_stSpi_Timer_Info[i].timerID = *pTimerId;
			HxLOG_Print(" Open : CONTINUE - Timer idx[%d], TimerId:%d \n", i,s_stSpi_Timer_Info[i].timerID );			
		}
	}

	VK_SEM_Release(s_ulSpiTimerSem);

	return UC_ERROR_SUCCESS;
}


/**
 * Close the timerId
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Timer_Close(uc_uint32 timerId)
{
	HUINT32		i;

	VK_SEM_Get(s_ulSpiTimerSem);

	HxLOG_Print(" Close - timerId:%d \n",timerId);

	for( i = 0; i < IRUC_SPI_TIMER_MAX_NUM; i++ )
	{
		if( s_stSpi_Timer_Info[i].timerID == timerId )
		{
			if( s_stSpi_Timer_Info[i].status== 1 )
			{
				VK_TIMER_Cancel(s_stSpi_Timer_Info[i].timerID);
			}

			VK_memset(&s_stSpi_Timer_Info[i], 0, sizeof(SPI_TIMER_INFO_STRUCT));
			s_stSpi_Timer_Info[i].timerID = UC_INVALID_HANDLE_VALUE;
			s_stSpi_Timer_Info[i].spi_timer_open_st.onTimeCallback=NULL;
			break;
		}
	}

	if( i == IRUC_SPI_TIMER_MAX_NUM )
	{
		HxLOG_Print(" Close - invalid reference ID\n");
		VK_SEM_Release(s_ulSpiTimerSem);
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	VK_SEM_Release(s_ulSpiTimerSem);
	return UC_ERROR_SUCCESS;
}


/**
 * Stops the timer.
 *
 * This method is called when Cloaked CA Agent wants to stop a timer.
 * Cloaked CA Agent may reuse the timer indicated by \b timerId later on.
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Timer_Stop(uc_uint32 timerId)
{
	HUINT32		i;

	VK_SEM_Get(s_ulSpiTimerSem);

	HxLOG_Print(" stop - TimerId(%d) \n", timerId);

	for( i = 0; i < IRUC_SPI_TIMER_MAX_NUM; i++ )
	{
		if(( s_stSpi_Timer_Info[i].timerID == timerId ) && ( s_stSpi_Timer_Info[i].status == 1 ))
		{
			VK_TIMER_Cancel(s_stSpi_Timer_Info[i].timerID);
			s_stSpi_Timer_Info[i].timerID = 0;
			s_stSpi_Timer_Info[i].status = 0;

			if(s_stSpi_Timer_Info[i].spi_timer_open_st.onTimeCallback != NULL)
			{
				s_stSpi_Timer_Info[i].spi_timer_open_st.onTimeCallback = NULL;
			}
			
		}
		break;
	}

	if( i == IRUC_SPI_TIMER_MAX_NUM )
	{
		HxLOG_Print(" STOP - invalid reference ID\n");
		VK_SEM_Release(s_ulSpiTimerSem);
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	VK_SEM_Release(s_ulSpiTimerSem);
	return UC_ERROR_SUCCESS;
}


/**
 * Start the timer.
 *
 * Cloaked CA Agent calls this to restart the timer which stopped is by ::UniversalClientSPI_Timer_Stop before.
 *
 * @param[in] timerId identity for the timer, Cloaked CA Agent uses this to map to a timer.
 * @retval
 *    ::UC_ERROR_SUCCESS
 * @retval
 *     Other The method may return any other errors listed in \ref result "Result Code List".
 */
uc_result UniversalClientSPI_Timer_Start(uc_uint32 timerId)
{
	HUINT32		i;

	VK_SEM_Get(s_ulSpiTimerSem);

	HxLOG_Print(" start - TimerId(%d) \n", timerId);

	for( i = 0; i < IRUC_SPI_TIMER_MAX_NUM; i++ )
	{
		if(( s_stSpi_Timer_Info[i].timerID == timerId ) && ( s_stSpi_Timer_Info[i].status == 0 ))
		{
			// ONE TIME.
			if( s_stSpi_Timer_Info[i].spi_timer_open_st.isRepetitive == FALSE )
			{
				if( VK_TIMER_EventAfter(s_stSpi_Timer_Info[i].spi_timer_open_st.timeout,
										(void *)local_SpiTimer_Callback,
										NULL,
										0,
										(unsigned long *)&(s_stSpi_Timer_Info[i].timerID)) != VK_OK )
				{
					HxLOG_Print(" START - Timer Once Start Error\n");
					VK_SEM_Release(s_ulSpiTimerSem);
					return UC_ERROR_UNABLE_TO_CREATE;
				}

				s_stSpi_Timer_Info[i].status = 1;
			}
			// CONTINUE
			else
			{
				if( VK_TIMER_EventEvery(s_stSpi_Timer_Info[i].spi_timer_open_st.timeout,
										(void *)local_SpiTimer_Callback,
										NULL,
										0,
										(unsigned long *)&(s_stSpi_Timer_Info[i].timerID)) != VK_OK )
				{
					HxLOG_Print(" START - Timer Every Start Error\n");
					VK_SEM_Release(s_ulSpiTimerSem);
					return UC_ERROR_UNABLE_TO_CREATE;
				}

				s_stSpi_Timer_Info[i].status = 1;
			}
		}
		break;
	}

	if( i == IRUC_SPI_TIMER_MAX_NUM )
	{
		HxLOG_Print(" START - invalid timer ID\n");
		VK_SEM_Release(s_ulSpiTimerSem);
		return UC_ERROR_RESOURCE_NOT_FOUND;
	}

	VK_SEM_Release(s_ulSpiTimerSem);
	return UC_ERROR_SUCCESS;
}

/** @} */ /* End of timerspi */


/********************************************************************
*	function name :	 CAS_IRUC_SPI_TIMER_Initialise
*	input : void(None)
*	output : uc_result
*	description :
********************************************************************/
uc_result CAS_IRUC_SPI_TIMER_Init(void)
{
	uc_result		result = UC_ERROR_SUCCESS;
	HUINT32		res;
	HUINT32		i;

	VK_memset(&s_stSpi_Timer_Info, 0, sizeof(SPI_TIMER_INFO_STRUCT));

	for( i = 0; i < IRUC_SPI_TIMER_MAX_NUM; i++ )
	{
		s_stSpi_Timer_Info[i].timerID = UC_INVALID_HANDLE_VALUE;
		s_stSpi_Timer_Info[i].spi_timer_open_st.onTimeCallback = NULL; // hjlee3
	}

	res = VK_SEM_Create(&s_ulSpiTimerSem, "s_ulSpiTimerSem", VK_SUSPENDTYPE_FIFO);
	if( res != VK_OK )
	{
		HxLOG_Error(" Semaphore Create Error\n");
		result = UC_ERROR_OS_FAILURE;
	}

	return result;
}



