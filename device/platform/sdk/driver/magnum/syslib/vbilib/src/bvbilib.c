/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvbilib.c $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 9/10/12 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/vbilib/src/bvbilib.c $
 * 
 * Hydra_Software_Devel/23   9/10/12 4:18p darnstein
 * SW7552-344: change some Coverity conventional comments.
 * 
 * Hydra_Software_Devel/22   1/8/10 1:12p darnstein
 * SW3556-979: I added an API function that points to the oldest piece of
 * data in the encode queue.
 * 
 * Hydra_Software_Devel/21   10/31/08 5:43p darnstein
 * PR41248: Port over conventional comments from 7401 branch.
 * 
 * Hydra_Software_Devel/20   9/11/07 5:16p darnstein
 * PR25708: First release of SCTE encoder software.
 * 
 * Hydra_Software_Devel/19   5/26/06 1:20p darnstein
 * PR21710: A field handle can be dequeued for encoding at any one of
 * three times: top field, bottom field, or FRAME. FRAME is the new
 * choice. The field handles themselves continue to be defined as top
 * field, bottom field, or both. The new logic is when a field handle is
 * dequeued at time FRAME, it is always encoded immediately. The rest of
 * the logic is the same: a "top" field handle is only dequeued and
 * encoded at "top field" time, same for bottoms. When setting up field
 * interrupt callbacks, for progressive video output, the argument
 * BAVC_Polarity_eFrame should be used now.
 * 
 * Hydra_Software_Devel/18   3/22/05 11:11a darnstein
 * PR 14477:  Measure encode queue depth.  For diagnostic purposes.
 * 
 * Hydra_Software_Devel/17   2/28/05 5:44p darnstein
 * PR 14040: Don't use BERR_TRACE() when a "queue full" condition is
 * encountered. This wil prevent the complaints from flooding in at 50 to
 * 60 Hz.
 * 
 * Hydra_Software_Devel/16   2/28/05 2:25p darnstein
 * PR 14040: BVBIlib_Encode_Enqueue_isr() simply refuses service if the
 * BVBIlib encoding queue is full.
 * 
 * Hydra_Software_Devel/15   11/30/04 4:25p darnstein
 * PR 13391: Handle an error condition a bit more nicely.  In reality,
 * this is only an improvement if the user has passed in a bad parameter
 * to BVBIlib_Decode_Data_isr().
 * 
 * Hydra_Software_Devel/14   11/11/04 2:09p darnstein
 * PR 12900: Call BDBG_WRN when queue overflows.
 * 
 * Hydra_Software_Devel/13   7/27/04 11:11a darnstein
 * PR 9080: don't use private functions to access field handle usage count
 * in BVBI porting interface.  It is against the rules.
 * 
 * Hydra_Software_Devel/12   6/17/04 5:58p darnstein
 * PR 9080: special handling when encoding field handles with polarity
 * mask zero. Before, placing such a field handle on the encode queue
 * would stall the queue forever.
 * 
 * Hydra_Software_Devel/11   2/27/04 6:16p darnstein
 * PR 9080: fix misuse of BDBG_ASSERT macro.
 * 
 * Hydra_Software_Devel/10   2/9/04 10:30a darnstein
 * PR 9080: Add function BERR_Code BVBIlib_Encode_NoData_isr() to API.
 * Bug fix: In BVBIlib_encode_data_isr(), if next field handle has the
 * wrong parity, then instruct BVBI module to encode nothing for next
 * field.
 * 
 * Hydra_Software_Devel/9   1/30/04 3:07p darnstein
 * PR 9080: fix a stupid bug in the "flush" function.
 * 
 * Hydra_Software_Devel/8   1/27/04 6:23p darnstein
 * PR 9080: Don't check for overrun errors on encoding.  It is only
 * possible for closed caption encoding, anyway.
 * 
 * Hydra_Software_Devel/7   1/15/04 4:34p darnstein
 * PR 9080: fix a problem with usage count management of field handles.
 * 
 * Hydra_Software_Devel/6   1/8/04 7:25p darnstein
 * PR 9080: Add capability to query for encoding errors from field
 * handles.  Bug fixes.
 * 
 * Hydra_Software_Devel/5   12/19/03 5:09p darnstein
 * PR 9080: several changes. Including: adapt to changed BAVC enum for
 * field polarity.
 * 
 * Hydra_Software_Devel/4   12/3/03 2:54p darnstein
 * Add function BVBIlib_Encode_Flush_isr().
 * 
 * Hydra_Software_Devel/3   10/21/03 3:01p darnstein
 * Fix usage of BDBG_ENTER(), BDBG_LEAVE().
 * 
 * Hydra_Software_Devel/2   10/17/03 3:46p darnstein
 * Encoding function turns of VBI if there is no queued data for it to
 * encode.
 * 
 * Hydra_Software_Devel/1   10/14/03 11:31a darnstein
 * Initial version
 * 
 ***************************************************************************/
#include "bstd.h"
#include "berr.h"
#include "berr_ids.h"
#include "bdbg.h"                /* Dbglib */
#include "bkni.h"                /* malloc */
#include "bvbi.h"                /* VBI p.i. API */
#include "bvbi_prot.h"           /* VBI p.i. protected data structures */
#include "bvbilib.h"             /* This module. */
#include "bvbilib_priv.h"        /* VBI lib internal data structures */

BDBG_MODULE(BVBIlib);

/***************************************************************************
* Forward declarations of static (private) functions
***************************************************************************/


/***************************************************************************
* Implementation of "BVBIlib_" API functions
***************************************************************************/

/***************************************************************************
 *
 */
BERR_Code BVBIlib_Open(
    BVBIlib_Handle  *pVbilHandle,   
    BVBI_Handle        vbiHandle
)
{
	BVBIlib_P_Handle *pVbilib;

	BDBG_ENTER(BVBIlib_Open);

	if((!pVbilHandle) ||
	   (!vbiHandle)   )
	{
		BDBG_ERR(("Invalid parameter\n"));
		BDBG_LEAVE(BVBIlib_Open);
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Alloc the main VBI context. */
	pVbilib = (BVBIlib_P_Handle*)(BKNI_Malloc(sizeof(BVBIlib_P_Handle)));

	if(!pVbilib)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVbilib, 0x0, sizeof(BVBIlib_P_Handle));

	/* Initialize magic number to the size of the struct */
	pVbilib->ulBlackMagic = sizeof(BVBIlib_P_Handle);

	/* Store the VBI handle for later use */
	pVbilib->hBvbi = vbiHandle;

	/* Initialize empty lists of decode and encode contexts */
	BLST_Q_INIT(&pVbilib->decode_contexts);
	BLST_Q_INIT(&pVbilib->encode_contexts);

	/* All done. now return the new fresh context to user. */
	*pVbilHandle = (BVBIlib_Handle)pVbilib;

	BDBG_LEAVE(BVBIlib_Open);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
void BVBIlib_Close( BVBIlib_Handle vbilHandle )
{
	BVBIlib_P_Handle *pVbil;

	BDBG_ENTER(BVBIlib_Close);

	/* check parameters */
	BVBILIB_P_GET_CONTEXT(vbilHandle, pVbil);
	BDBG_ASSERT (pVbil != NULL);

	/* Refuse service if user left any decoder objects open */
	BDBG_ASSERT (BLST_Q_EMPTY (&pVbil->decode_contexts)); 

	/* Refuse service if user left any encoder objects open */
	BDBG_ASSERT (BLST_Q_EMPTY (&pVbil->encode_contexts));

	/* The handle is about to become invalid */
	pVbil->ulBlackMagic = 0;

	/* Release context in system memory */
	BKNI_Free((void*)pVbil);

	BDBG_LEAVE(BVBIlib_Close);
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Decode_Create( 
	BVBIlib_Handle         vbilHandle,	
	BVBIlib_List_Handle   vbillHandle,
	BVBI_Decode_Handle         decHdl,
	BVBIlib_Decode_Handle *  pDeclHdl )
{
	BVBIlib_P_Handle *pVbil;
	BVBIlib_P_Decode_Handle *pVbildec;

	BDBG_ENTER(BVBIlib_Decode_Create);

	BVBILIB_P_GET_CONTEXT(vbilHandle, pVbil);
	if((!pVbil       ) ||
	   (!vbillHandle)  ||
	   (!decHdl      ) ||
	   (!pDeclHdl    )  )
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Alloc the main context. */
	pVbildec = 
		(BVBIlib_P_Decode_Handle*)(BKNI_Malloc(
			sizeof(BVBIlib_P_Decode_Handle)));
	if(!pVbildec)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* TODO: verify that the supplied VBI_Handle "owns" the supplied
	   VBI_Decode_Handle */

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVbildec, 0x0, sizeof(BVBIlib_P_Decode_Handle));

	/* Initialize magic number to the size of the struct */
	pVbildec->ulBlackMagic = sizeof(BVBIlib_P_Decode_Handle);

	/* Store all handles passed in */
	pVbildec->pVbilib = pVbil;
	pVbildec->hVbiDec = decHdl;
	pVbildec->hVbill  = vbillHandle;

	/* Join up with the controlling BVBIlib_Handle */
	BLST_Q_INSERT_HEAD (&pVbil->decode_contexts, pVbildec, link);

	/* All done. now return the new fresh context to user. */
	*pDeclHdl = (BVBIlib_Decode_Handle)pVbildec;

	BDBG_LEAVE(BVBIlib_Decode_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Encode_Create( 
	BVBIlib_Handle         vbilHandle,	
	BVBIlib_List_Handle   vbillHandle,
	BVBI_Encode_Handle         encHdl,
	int                     nMaxQueue, 
	BVBIlib_Encode_Handle *  pEnclHdl )
{
	BVBIlib_P_Handle *pVbil;
	BVBIlib_P_Encode_Handle *pVbilenc;
	BVBIlib_P_FieldHanger *hanger;
	int iHandle;

	BDBG_ENTER(BVBIlib_Encode_Create);

	BVBILIB_P_GET_CONTEXT(vbilHandle, pVbil);
	if((!pVbil       ) ||
	   (!vbillHandle)  ||
	   (!encHdl      ) ||
	   (!pEnclHdl    )  )
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Alloc the main context. */
	pVbilenc = 
		(BVBIlib_P_Encode_Handle*)(BKNI_Malloc(
			sizeof(BVBIlib_P_Encode_Handle)));
	if(!pVbilenc)
	{
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	}

	/* TODO: verify that the supplied VBI_Handle "owns" the supplied
	   VBI_Encode_Handle */

	/* Clear out the context and set defaults. */
	BKNI_Memset((void*)pVbilenc, 0x0, sizeof(BVBIlib_P_Encode_Handle));

	/* Initialize magic number to the size of the struct */
	pVbilenc->ulBlackMagic = sizeof(BVBIlib_P_Encode_Handle);

	/* Store handles and options passed in */
	pVbilenc->pVbilib   = pVbil;
	pVbilenc->hVbiEnc   = encHdl;
	pVbilenc->hVbill    = vbillHandle;
	pVbilenc->nMaxQueue = nMaxQueue;

	/* Join up with the controlling BVBIlib_Handle */
	BLST_Q_INSERT_HEAD (&pVbil->encode_contexts, pVbilenc, link);

	/* Initialize empty lists of field handle hangers */
	BLST_Q_INIT(&pVbilenc->encode_queue);
	pVbilenc->encode_queue_length = 0;
	BLST_Q_INIT(&pVbilenc->empty_hangers);

	/* Create a sufficient number of empty handle hangers */
	for (iHandle = 0 ; iHandle < nMaxQueue ; ++iHandle)
	{
		hanger = 
			(BVBIlib_P_FieldHanger*)(BKNI_Malloc(
				sizeof(BVBIlib_P_FieldHanger)));
		if(!hanger)
		{
			BVBIlib_Encode_Destroy ((BVBIlib_Encode_Handle)pVbilenc);
			BDBG_LEAVE(BVBIlib_Encode_Create);
			return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
		}
		hanger->hField = NULL;
		BLST_Q_INSERT_HEAD (&pVbilenc->empty_hangers, hanger, link);
	}

	/* All done. now return the new fresh context to user. */
	*pEnclHdl = (BVBIlib_Encode_Handle)pVbilenc;

	BDBG_LEAVE(BVBIlib_Encode_Create);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Decode_Destroy( BVBIlib_Decode_Handle declHdl	)
{
	BVBIlib_P_Decode_Handle *pVbildec;

	BDBG_ENTER(BVBIlib_Decode_Destroy);

	/* check parameters */
	BVBILIB_P_GET_DECODE_CONTEXT(declHdl, pVbildec);
	if(!pVbildec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Unlink from parent handle */
	BLST_Q_REMOVE (&pVbildec->pVbilib->decode_contexts, pVbildec, link);

	/* The handle is now invalid */
	pVbildec->ulBlackMagic = 0;

	/* Release context in system memory */
	BKNI_Free((void*)pVbildec);

	BDBG_LEAVE(BVBIlib_Decode_Destroy);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Encode_Destroy( BVBIlib_Encode_Handle enclHdl	)
{
	BVBIlib_P_Encode_Handle *pVbilenc;
	BVBIlib_P_FieldHanger *hanger;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBIlib_Encode_Destroy);

	/* check parameters */
	BVBILIB_P_GET_ENCODE_CONTEXT(enclHdl, pVbilenc);
	if(!pVbilenc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Unlink from parent handle */
	BLST_Q_REMOVE (&pVbilenc->pVbilib->encode_contexts, pVbilenc, link);

	/* Clear out list of unused field handle hangers */
	/* coverity[alias] */
	/* coverity[USE_AFTER_FREE] */
	while ((hanger = BLST_Q_FIRST (&pVbilenc->empty_hangers)) != NULL)
	{
		BLST_Q_REMOVE (&pVbilenc->empty_hangers, hanger, link);
		/* coverity[freed_arg] */
		BKNI_Free ((void*)hanger);
	}

	/* Complain if user still has some field handles in queue */
	if (!BLST_Q_EMPTY (&pVbilenc->encode_queue))
	{
		eErr = BVBIlib_USER_LEAK;
	}

	/* The handle is now invalid */
	pVbilenc->ulBlackMagic = 0;

	/* Release context in system memory */
	BKNI_Free((void*)pVbilenc);

	BDBG_LEAVE(BVBIlib_Encode_Destroy);
	return eErr;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Decode_Data_isr(
	BVBIlib_Decode_Handle     declHdl, 
	BVBI_Field_Handle *  pFieldHandle,
	BAVC_Polarity            polarity  
)
{
	BVBIlib_P_Decode_Handle *pVbildec;
	BVBI_Field_Handle hField = 0;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBIlib_Decode_Data_isr);

	/* check parameters */
	BVBILIB_P_GET_DECODE_CONTEXT(declHdl, pVbildec);
	if(!pVbildec)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Obtain a free field handle */
	eErr = BVBIlib_List_Obtain_isr (pVbildec->hVbill, &hField);
	if (eErr != BERR_SUCCESS)
	{
		hField = 0;
		BDBG_LEAVE(BVBIlib_Decode_Data_isr);
		return eErr;
	}

	/* Call into BVBI module to decode */
	eErr = BVBI_Decode_Data_isr (pVbildec->hVbiDec, hField, polarity);

	/* Prevent a memory leak */
	if (eErr != BERR_SUCCESS)
	{
		BVBIlib_List_Return_isr (pVbildec->hVbill, hField);
		hField = 0;
	}

	/* Return data to caller */
	*pFieldHandle = hField;

	BDBG_LEAVE(BVBIlib_Decode_Data_isr);
	return eErr;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Encode_Enqueue_isr(
	BVBIlib_Encode_Handle      enclHdl,
	BVBI_Field_Handle      fieldHandle
)
{
	BVBIlib_P_Encode_Handle *pVbilenc;
	BVBIlib_P_FieldHanger *hanger;

	BDBG_ENTER(BVBIlib_Encode_Enqueue_isr);

	/* check parameters */
	BVBILIB_P_GET_ENCODE_CONTEXT(enclHdl, pVbilenc);
	if(!pVbilenc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* If there are no empty hangers available */
	if ((hanger = BLST_Q_FIRST (&pVbilenc->empty_hangers)) == NULL)
	{
		/* The input queue must be full.  Deny service. */
		return BVBIlib_QUEUE_FULL;
		/* Programming note: no BERR_TRACE(), because this statement is likely
		   to occur 50 to 60 times per second if it occurs at all. */
	}

	/* Use the first empty hanger (obtained above) */
	BLST_Q_REMOVE (&pVbilenc->empty_hangers, hanger, link);

	/* Enqueue caller's data for subsequent encoding */
	hanger->hField = fieldHandle;
	BLST_Q_INSERT_TAIL (&pVbilenc->encode_queue, hanger, link);
	++pVbilenc->encode_queue_length;

	/* Increment use count of field handle */
	BVBI_Field_Increment_UsageCount_isr (fieldHandle);

	BDBG_LEAVE(BVBIlib_Encode_Enqueue_isr);
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Encode_Data_isr(
	BVBIlib_Encode_Handle      enclHdl,
	BAVC_Polarity             polarity
)
{
	BVBIlib_P_Encode_Handle *pVbilenc;
	BVBIlib_P_FieldHanger *hanger;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBIlib_Encode_Data_isr);

	/* check parameters */
	BVBILIB_P_GET_ENCODE_CONTEXT(enclHdl, pVbilenc);
	if(!pVbilenc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* If there is a queued field handle available */
	if ((hanger = BLST_Q_FIRST (&pVbilenc->encode_queue)) != NULL)
	{
		/* Check polarity */
		uint32_t myMask;
		uint32_t fieldMask;
		myMask = (uint32_t)1 << (uint32_t)polarity;
		eErr = BVBI_Field_GetPolarity_isr (hanger->hField, &fieldMask);
		BDBG_ASSERT (eErr == BERR_SUCCESS);
		if (fieldMask == 0)
		{
			/* A field handle with no polarity.  Get rid of it, or 
			 * it will stay in the queue forever. The following code 
			 * block is a repeat of what appears below, but without 
			 * comments. 
			 */
			BLST_Q_REMOVE (&pVbilenc->encode_queue, hanger, link);
			--pVbilenc->encode_queue_length;
			BVBI_Field_Decrement_UsageCount_isr (hanger->hField);
			if (BVBI_Field_Get_UsageCount_isr (hanger->hField) == 0)
			{
				BVBIlib_List_Return_isr (pVbilenc->hVbill, hanger->hField);
			}
			hanger->hField = NULL;
			BLST_Q_INSERT_TAIL (&pVbilenc->empty_hangers, hanger, link);

			/* Cause the encoder hardware to output no VBI next field */
			eErr = BVBI_Encode_Data_isr (pVbilenc->hVbiEnc, NULL, polarity);
		}
		else if ((polarity == BAVC_Polarity_eFrame) ||
		         ((myMask & fieldMask) == myMask   )   )
		{
			/*
			 * Normal case
			 */
			/* Remove from input queue */
			BLST_Q_REMOVE (&pVbilenc->encode_queue, hanger, link);
			--pVbilenc->encode_queue_length;

			/* Give to hardware for encoding at next field */
			eErr = 
				BVBI_Encode_Data_isr (
					pVbilenc->hVbiEnc, hanger->hField, polarity);

			/* Decrement use count of field handle */
			BVBI_Field_Decrement_UsageCount_isr (hanger->hField);

			/* Recycle the field handle if appropriate */
			if (BVBI_Field_Get_UsageCount_isr (hanger->hField) == 0)
			{
				BVBIlib_List_Return_isr (pVbilenc->hVbill, hanger->hField);
			}

			/* Recycle the hanger in any case */
			hanger->hField = NULL;
			BLST_Q_INSERT_TAIL (&pVbilenc->empty_hangers, hanger, link);
		}
		else
		{
			/* 
			 * Field mismatch. The field handle stays in the queue until 
			 * the next interrupt.
			 */
			/* Cause the encoder hardware to output no VBI next field */
			eErr = BVBI_Encode_Data_isr (pVbilenc->hVbiEnc, NULL, polarity);
		}
	}
	else
	{
		/* Cause the encoder hardware to output no VBI next field */
		eErr = BVBI_Encode_Data_isr (pVbilenc->hVbiEnc, NULL, polarity);
	}

	BDBG_LEAVE(BVBIlib_Encode_Encode_Data_isr);
	return eErr;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Encode_GetOldestDatum_isr(
	BVBIlib_Encode_Handle enclHdl,  
	BVBI_Field_Handle* pFieldHandle)
{
	BVBIlib_P_Encode_Handle *pVbilenc;
	BVBIlib_P_FieldHanger *hanger;
	BVBI_Field_Handle hField = NULL;

	BDBG_ENTER(BVBIlib_Encode_GetOldestDatum_isr);

	/* check parameters */
	BVBILIB_P_GET_ENCODE_CONTEXT(enclHdl, pVbilenc);
	if ((!pVbilenc) || (!pFieldHandle))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* If there is a queued field handle available */
	if ((hanger = BLST_Q_FIRST (&pVbilenc->encode_queue)) != NULL)
	{
		hField = hanger->hField;
	}

	/* Return what we got, if anything. */
	*pFieldHandle = hField;
	return BERR_SUCCESS;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Encode_NoData_isr(
	BVBIlib_Encode_Handle      enclHdl,
	BAVC_Polarity             polarity
)
{
	BVBIlib_P_Encode_Handle *pVbilenc;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBIlib_Encode_NoData_isr);

	/* check parameters */
	BVBILIB_P_GET_ENCODE_CONTEXT(enclHdl, pVbilenc);
	if(!pVbilenc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Cause the encoder hardware to output no VBI next field */
	eErr = BVBI_Encode_Data_isr (pVbilenc->hVbiEnc, NULL, polarity);

	BDBG_LEAVE(BVBIlib_Encode_Encode_Data_isr);
	return eErr;
}


/***************************************************************************
 *
 */
BERR_Code BVBIlib_Encode_Flush_isr( BVBIlib_Encode_Handle enclHdl )
{
	BVBIlib_P_Encode_Handle *pVbilenc;
	BVBIlib_P_FieldHanger *hanger;
	BERR_Code eErr = BERR_SUCCESS;

	BDBG_ENTER(BVBIlib_Encode_Flush_isr);

	/* check parameters */
	BVBILIB_P_GET_ENCODE_CONTEXT(enclHdl, pVbilenc);
	if(!pVbilenc)
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* While there are queued field handles available */
	while ((hanger = BLST_Q_FIRST (&pVbilenc->encode_queue)) != NULL)
	{
		/* Remove from input queue */
		BLST_Q_REMOVE (&pVbilenc->encode_queue, hanger, link);
		--pVbilenc->encode_queue_length;

		/* Decrement use count of field handle */
		BVBI_Field_Decrement_UsageCount_isr (hanger->hField);

		/* Recycle the field handle if appropriate */
		if (BVBI_Field_Get_UsageCount_isr (hanger->hField) == 0)
		{
			BVBIlib_List_Return_isr (pVbilenc->hVbill, hanger->hField);
		}

		/* Recycle the hanger too */
		hanger->hField = NULL;
		BLST_Q_INSERT_TAIL (&pVbilenc->empty_hangers, hanger, link);
	}

	BDBG_LEAVE(BVBIlib_Encode_Encode_Flush_isr);
	return eErr;
}


/***************************************************************************
 *
 */
int BVBIlib_GetQueueLength (BVBIlib_Encode_Handle enclHdl)
{
	BVBIlib_P_Encode_Handle *pVbilenc;

	BDBG_ENTER(BVBIlib_Encode_Flush_isr);

	/* check parameters */
	BVBILIB_P_GET_ENCODE_CONTEXT(enclHdl, pVbilenc);
	BDBG_ASSERT (pVbilenc);

	/* Return value as requested */
	return pVbilenc->encode_queue_length;
}

/* End of File */
