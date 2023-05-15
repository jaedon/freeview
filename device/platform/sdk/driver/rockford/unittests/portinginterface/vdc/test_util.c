/***************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: test_util.c $
* $brcm_Revision: Hydra_Software_Devel/1 $
* $brcm_Date: 10/11/12 12:10p $
*
* Module Description:
*
* Revision History:
*
* $brcm_Log: /rockford/unittests/portinginterface/vdc/test_util.c $
* 
* Hydra_Software_Devel/1   10/11/12 12:10p syang
* SW7346-572: add long-time-lock for vdc_test
*
***************************************************************************/

#include "commands.h"

BDBG_MODULE(BTST);

/***************************************************************************
*
*/
BERR_Code BTST_P_ApplyChanges_tagged
( BTST_P_Context  *pContext, const char *fileName, unsigned uiLine )
{
	BERR_Code rc = BERR_MAKE_CODE(BERR_VDC_ID, 0xffff);

	if(!pContext->hVdc)
	{
		goto done;
	}

	rc = BVDC_ApplyChanges(pContext->hVdc);
	if (BERR_SUCCESS!=rc)
	{
		BDBG_ERR(("BVDC_ApplyChanges failed at %s:%d!!!", fileName, uiLine));
	}
	
done:
	if(pContext->bApplyError)
	{
		BDBG_ASSERT(BERR_SUCCESS == rc);
	}
	return rc;
}

#define TST_MUTEX_WAIT_MILLI_SEC  10
/***************************************************************************
*
*/
void BTST_P_AcquireLongTimeLock_tagged
( BTST_P_LongTimeLock  *pLongTimeLock, const char *fileName, unsigned uiLine )
{
	unsigned ii;
	
	BERR_Code rc;

	BDBG_ASSERT(pLongTimeLock->hMutex);
	for(ii=0; true; ii++)
	{
		rc = BKNI_AcquireMutex(pLongTimeLock->hMutex);
		if (BERR_SUCCESS==rc)
		{
			if (false==pLongTimeLock->bLocked)
			{
				/* lock it */
				pLongTimeLock->bLocked = true;
				pLongTimeLock->fileName = fileName;
				pLongTimeLock->uiLine = uiLine;
				
				BKNI_ReleaseMutex(pLongTimeLock->hMutex);
				break;
			}
			else
			{
				/* some one has the lock, we release the mutex (to avoid holding  
				 * mutex too long), yield a little time, and will try again */
				BKNI_ReleaseMutex(pLongTimeLock->hMutex);
				if (ii>pLongTimeLock->uiMaxTrys)
				{
					BDBG_WRN(("tried %d times to acquire lock!!! held at %s:%d", ii, pLongTimeLock->fileName, pLongTimeLock->uiLine));
				}
				BKNI_Sleep(TST_MUTEX_WAIT_MILLI_SEC);
			}
		}
		else
		{
			/* should never be here */
			BDBG_ERR(("failed to acquire mutex for acquire long time lock!!!"));
		}
	}
}

/***************************************************************************
*
*/
void BTST_P_ReleaseLongTimeLock
( BTST_P_LongTimeLock  *pLongTimeLock)
{
	BERR_Code rc;

	BDBG_ASSERT(pLongTimeLock->hMutex);
	for(;;)
	{
		rc = BKNI_AcquireMutex(pLongTimeLock->hMutex);
		if (BERR_SUCCESS==rc)
		{
			pLongTimeLock->bLocked = false;
			pLongTimeLock->fileName = NULL;
			pLongTimeLock->uiLine = 0;
			BKNI_ReleaseMutex(pLongTimeLock->hMutex);
			break;
		}
		else
		{
			/* should never be here */
			BDBG_ERR(("failed to acquire mutex for release long time lock!!!"));
		}
	}
}

/***************************************************************************
*
*/
BERR_Code BTST_P_InitLongTimeLock
( BTST_P_LongTimeLock  *pLongTimeLock, unsigned uiMaxWaitSec)
{
	BERR_Code rc;

	pLongTimeLock->bLocked = false;
	pLongTimeLock->uiMaxTrys = (uiMaxWaitSec * 1000) / TST_MUTEX_WAIT_MILLI_SEC;
	pLongTimeLock->fileName = NULL;
	pLongTimeLock->uiLine = 0;
    rc = BKNI_CreateMutex(&pLongTimeLock->hMutex);
	if (NULL == pLongTimeLock->hMutex)
	{
		/* should not be here */
		rc = BERR_MAKE_CODE(BERR_VDC_ID, 0xfffe);
	}
	
	return BERR_TRACE(rc);
}

/***************************************************************************
*
*/
void BTST_P_UninitLongTimeLock
( BTST_P_LongTimeLock  *pLongTimeLock)
{

	if (pLongTimeLock->bLocked)
	{
		BDBG_ERR(("destroy a locked long-time-lock!!!"));			
		pLongTimeLock->bLocked = false;
	}
    BKNI_DestroyMutex(pLongTimeLock->hMutex);
	pLongTimeLock->hMutex = NULL;
}


/* end of file */
