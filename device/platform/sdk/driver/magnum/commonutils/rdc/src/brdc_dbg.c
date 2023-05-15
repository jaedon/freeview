/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brdc_dbg.c $
 * $brcm_Revision: Hydra_Software_Devel/14 $
 * $brcm_Date: 8/13/12 3:21p $
 *
 * $brcm_Log: /magnum/commonutils/rdc/7038/brdc_dbg.c $
 * 
 * Hydra_Software_Devel/14   8/13/12 3:21p yuxiaz
 * SW7425-3626: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 * 
 * Hydra_Software_Devel/13   8/9/12 4:42p yuxiaz
 * SW7425-3626: Replace uncache memory access with cache memory access.
 *
 * Module Description:
 *
 * Revision History:
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "brdc.h"
#include "brdc_private.h"

#include "brdc_dbg.h"

BDBG_MODULE(BRDC_DBG);


/***************************************************************************
 *
 */
BERR_Code BRDC_DBG_SetList(
	BRDC_List_Handle  hList
	)
{
	BERR_Code  eErr = BERR_SUCCESS;
	
	/* initialize list state */
	hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;

	BKNI_EnterCriticalSection();
	hList->pulCurListAddr = BRDC_List_GetStartAddress_isr(hList);

	/* get number of entries */
	eErr = BERR_TRACE(BRDC_List_GetNumEntries_isr(hList, &hList->ulNumEntries));
	BKNI_LeaveCriticalSection();
	
	return eErr;
}


/***************************************************************************
 *
 */
BERR_Code BRDC_DBG_SetList_isr(
	BRDC_List_Handle  hList
	)
{
	BERR_Code  eErr = BERR_SUCCESS;
	
	/* initialize list state */
	hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;

	hList->pulCurListAddr = BRDC_List_GetStartAddress_isr(hList);

	/* get number of entries */
	eErr = BERR_TRACE(BRDC_List_GetNumEntries_isr(hList, &hList->ulNumEntries));

	/*printf("++++++++++++++++++++++++++++++++++++++++++++++ hList->ulNumEntries = %d\n", hList->ulNumEntries);*/
	
	return eErr;
}


/***************************************************************************
 *
 */
BERR_Code BRDC_DBG_GetListEntry(
	BRDC_List_Handle     hList,
	BRDC_DBG_ListEntry  *peEntry,
	uint32_t             aulArgs[4]
	)
{
	BERR_Code  eErr = BERR_SUCCESS;
	uint32_t   ulEntry;
	
	/* clear out arguments */
	BKNI_Memset(aulArgs, 0x0, sizeof(uint32_t) * 4);
	
	/* no more entries in list? */
	if(hList->ulNumEntries == 0)
	{
		/* next entry better be a command or we stopped in mid command */
		if(hList->eNextEntry != BRDC_DBG_ListEntry_eCommand)
		{
			/* error */
			eErr = BERR_TRACE(BERR_UNKNOWN);
			goto done;
		}
		
		/* list terminated correctly */
		*peEntry = BRDC_DBG_ListEntry_eEnd;
		goto done;
	}
	
	/* get next entry in list */
	*peEntry = hList->eNextEntry;
	ulEntry = *(hList->pulCurListAddr++);
	--hList->ulNumEntries;
	
	/* current entry should be a command? */
	if(hList->eNextEntry == BRDC_DBG_ListEntry_eCommand)
	{
		/* which command? */
		aulArgs[0] = BRDC_GET_OPCODE(ulEntry);
		hList->ulCurrCommand = aulArgs[0];
		hList->iCommandIndex = 0;
		hList->iDataCount = 0;
		
		/* get other command entries */
		switch(hList->ulCurrCommand)
		{
		case BRDC_OP_REG_TO_VAR_OPCODE:
		case BRDC_OP_IMM_TO_VAR_OPCODE:
			/* variable stored in bits 5-0 */
			aulArgs[1] = ulEntry & UINT32_C(0x3F);
			break;

		case BRDC_OP_VAR_TO_REG_OPCODE:
		case BRDC_OP_COND_SKIP_OPCODE:
			/* variable stored in bits 17-12 */
			aulArgs[1] = (ulEntry >> 12) & UINT32_C(0x3F);
			break;
			
		case BRDC_OP_IMMS_TO_REG_OPCODE:
		case BRDC_OP_IMMS_TO_REGS_OPCODE:
			/* count-1 stored in bits 11-0 (store data count) */
			aulArgs[1] = (ulEntry & UINT32_C(0xFFF)) + 1;
			hList->iDataCount = aulArgs[1];
			break;
		
		case BRDC_OP_REG_TO_REG_OPCODE:
		case BRDC_OP_REGS_TO_REGS_OPCODE:
		case BRDC_OP_REG_TO_REGS_OPCODE:
		case BRDC_OP_REGS_TO_REG_OPCODE:
			/* count-1 stored in bits 11-0 */
			aulArgs[1] = (ulEntry & UINT32_C(0xFFF)) + 1;
			break;

		case BRDC_OP_VAR_AND_VAR_TO_VAR_OPCODE:
		case BRDC_OP_VAR_OR_VAR_TO_VAR_OPCODE:
		case BRDC_OP_VAR_XOR_VAR_TO_VAR_OPCODE:
		case BRDC_OP_VAR_SUM_VAR_TO_VAR_OPCODE:
			/* bit 17-12 = src1; bit 11-6 = src2; bit 5-0 = dst */
			aulArgs[1] = (ulEntry >> 12) & UINT32_C(0x3F);
			aulArgs[2] = (ulEntry >>  6) & UINT32_C(0x3F);
			aulArgs[3] = (ulEntry      ) & UINT32_C(0x3F);
			break;
		
		case BRDC_OP_VAR_AND_IMM_TO_VAR_OPCODE:
		case BRDC_OP_VAR_OR_IMM_TO_VAR_OPCODE:
		case BRDC_OP_VAR_XOR_IMM_TO_VAR_OPCODE:
		case BRDC_OP_NOT_VAR_TO_VAR_OPCODE:
		case BRDC_OP_VAR_SUM_IMM_TO_VAR_OPCODE:
			/* bit 17-12 = src; bit 5-0 = dst */
			aulArgs[1] = (ulEntry >> 12) & UINT32_C(0x3F);
			aulArgs[2] = (ulEntry      ) & UINT32_C(0x3F);
			break;

		case BRDC_OP_VAR_ROR_TO_VAR_OPCODE:
			/* bit 22-18 = rotate; bit 17-12 = src; bit 5-0 = dst */
			aulArgs[1] = (ulEntry >> 18) & UINT32_C(0x1F);
			aulArgs[2] = (ulEntry >> 12) & UINT32_C(0x3F);
			aulArgs[3] = (ulEntry      ) & UINT32_C(0x3F);
			break;

		case BRDC_OP_NOP_OPCODE:
		case BRDC_OP_IMM_TO_REG_OPCODE:
		case BRDC_OP_SKIP_OPCODE:
		case BRDC_OP_EXIT_OPCODE:
			/* no extra data in command word */
			break;
		
		default:
			/* unknown command */
			eErr = BERR_TRACE(BERR_UNKNOWN);
			goto done;
		}
	
	/* next entry is not a command */	
	} else
	{
		/* register? */
		if(hList->eNextEntry == BRDC_DBG_ListEntry_eRegister)
		{
			/* convert RDC register to offset register type */
			aulArgs[0] = BRDC_REGISTER_TO_OFFSET(ulEntry);
			
		/* data */
		} else
		{
			/* unmodified data */
			aulArgs[0] = ulEntry;
		}
	}
	
	/* determine what the next value should be next time */
	switch(hList->ulCurrCommand)
	{
		case BRDC_OP_IMM_TO_REG_OPCODE:
			/* register then data */
			if(hList->iCommandIndex == 0)
			{
				/* register */
				hList->eNextEntry = BRDC_DBG_ListEntry_eRegister;
			} else if(hList->iCommandIndex == 1)
			{
				/* data */
				hList->eNextEntry = BRDC_DBG_ListEntry_eData;
			} else
			{
				/* next command */
				hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;
			}
			break;
			
		case BRDC_OP_VAR_TO_REG_OPCODE:
		case BRDC_OP_REG_TO_VAR_OPCODE:
		case BRDC_OP_COND_SKIP_OPCODE:
			/* register only */
			if(hList->iCommandIndex == 0)
			{
				/* register */
				hList->eNextEntry = BRDC_DBG_ListEntry_eRegister;
			} else
			{
				/* next command */
				hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;
			}
			break;

		case BRDC_OP_IMM_TO_VAR_OPCODE:
		case BRDC_OP_VAR_AND_IMM_TO_VAR_OPCODE:
		case BRDC_OP_VAR_OR_IMM_TO_VAR_OPCODE:
		case BRDC_OP_VAR_XOR_IMM_TO_VAR_OPCODE:
		case BRDC_OP_VAR_SUM_IMM_TO_VAR_OPCODE:
			/* data only */
			if(hList->iCommandIndex == 0)
			{
				/* register */
				hList->eNextEntry = BRDC_DBG_ListEntry_eData;
			} else
			{
				/* next command */
				hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;
			}
			break;
			
		case BRDC_OP_VAR_AND_VAR_TO_VAR_OPCODE:
		case BRDC_OP_VAR_OR_VAR_TO_VAR_OPCODE:
		case BRDC_OP_VAR_XOR_VAR_TO_VAR_OPCODE:
		case BRDC_OP_VAR_SUM_VAR_TO_VAR_OPCODE:
			/* no data follows command */
			hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;
			break;
			
		case BRDC_OP_IMMS_TO_REG_OPCODE:
		case BRDC_OP_IMMS_TO_REGS_OPCODE:
			/* single register then N data */
			if(hList->iCommandIndex == 0)
			{
				/* register */
				hList->eNextEntry = BRDC_DBG_ListEntry_eRegister;
			} else if(hList->iCommandIndex <= hList->iDataCount)
			{
				/* data */
				hList->eNextEntry = BRDC_DBG_ListEntry_eData;
			} else
			{
				/* next command */
				hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;
			}
			break;

		case BRDC_OP_REG_TO_REG_OPCODE:
		case BRDC_OP_REGS_TO_REGS_OPCODE:
		case BRDC_OP_REG_TO_REGS_OPCODE:
		case BRDC_OP_REGS_TO_REG_OPCODE:
			/* source register followed by destination register */
			if(hList->iCommandIndex <= 1)
			{
				/* register */
				hList->eNextEntry = BRDC_DBG_ListEntry_eRegister;
			} else
			{
				/* next command */
				hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;
			}
			break;
		
		case BRDC_OP_NOT_VAR_TO_VAR_OPCODE:
		case BRDC_OP_VAR_ROR_TO_VAR_OPCODE:
		case BRDC_OP_NOP_OPCODE:
		case BRDC_OP_SKIP_OPCODE:
		case BRDC_OP_EXIT_OPCODE:
			/* no data follows command */
			hList->eNextEntry = BRDC_DBG_ListEntry_eCommand;
			break;
			
		default:
			/* unknown command */
			eErr = BERR_TRACE(BERR_UNKNOWN);
			goto done;
	}
	
	/* next command index */
	++hList->iCommandIndex;

done:
	return eErr;
}	

BERR_Code BRDC_DBG_GetListEntry_isr(
	BRDC_List_Handle     hList,
	BRDC_DBG_ListEntry	*peEntry,
	uint32_t			 aulArgs[4]
	)
{
	return (BRDC_DBG_GetListEntry(hList, peEntry, aulArgs));
}

/***************************************************************************
 *
 */
BERR_Code BRDC_DBG_DumpList(
	BRDC_List_Handle  hList
	)
{
	BERR_Code  eErr = BERR_SUCCESS;
	BRDC_DBG_ListEntry  eEntry = BRDC_DBG_ListEntry_eCommand;
	uint32_t            aulArgs[4];
	
	/* store the list to be dumped */
	eErr = BERR_TRACE(BRDC_DBG_SetList(hList));
	if(eErr != BERR_SUCCESS)
	{
		/* error */
		goto done;
	}
	
	/* get first entry */
	eErr = BERR_TRACE(BRDC_DBG_GetListEntry(hList, &eEntry, aulArgs));
		if(eErr != BERR_SUCCESS)
	{
		/* error parsing list */
		BDBG_ERR(("error parsing list"));
		goto done;
	}
	
	/* traverse until all entries are displayed */
	while(eEntry != BRDC_DBG_ListEntry_eEnd)
	{
		/* display entry */
		switch(eEntry)
		{
		case BRDC_DBG_ListEntry_eCommand:
			/* which command? */
			switch(aulArgs[0])
			{
			case BRDC_OP_NOP_OPCODE:
				BDBG_MSG(("++ NOP"));
				break;
				
			case BRDC_OP_IMM_TO_REG_OPCODE:
				BDBG_MSG(("++ IMM TO REG"));
				break;
				
			case BRDC_OP_VAR_TO_REG_OPCODE:
				BDBG_MSG(("++ VAR TO REG: (%d)",
					aulArgs[1]));
				break;
				
			case BRDC_OP_REG_TO_VAR_OPCODE:
				BDBG_MSG(("++ REG TO VAR: (%d)",
					aulArgs[1]));
				break;
			
			case BRDC_OP_IMM_TO_VAR_OPCODE:
				BDBG_MSG(("++ IMM TO VAR: (%d)",
					aulArgs[1]));
				break;
				
			case BRDC_OP_IMMS_TO_REG_OPCODE:
				BDBG_MSG(("++ IMMS TO REG: (%d)",
					aulArgs[1]));
				break;
				
			case BRDC_OP_IMMS_TO_REGS_OPCODE:
				BDBG_MSG(("++ IMMS TO REGS: (%d)",
					aulArgs[1]));
				break;
				
			case BRDC_OP_REG_TO_REG_OPCODE:
				BDBG_MSG(("++ REG TO REG: (%d)",
					aulArgs[1]));
				break;

			case BRDC_OP_REGS_TO_REGS_OPCODE:
				BDBG_MSG(("++ REGS TO REGS: (%d)",
					aulArgs[1]));
				break;

			case BRDC_OP_REG_TO_REGS_OPCODE:
				BDBG_MSG(("++ REG TO REGS: (%d)",
					aulArgs[1]));
				break;

			case BRDC_OP_REGS_TO_REG_OPCODE:
				BDBG_MSG(("++ REGS TO REG: (%d)",
					aulArgs[1]));
				break;
			
			case BRDC_OP_VAR_AND_VAR_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR AND VAR TO VAR: (%d, %d, %d)",
					aulArgs[1], aulArgs[2], aulArgs[3]));
				break;
						
			case BRDC_OP_VAR_AND_IMM_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR AND IMM TO VAR: (%d, %d)",
					aulArgs[1], aulArgs[2]));
				break;
			
			case BRDC_OP_VAR_OR_VAR_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR OR VAR TO VAR: (%d, %d, %d)",
					aulArgs[1], aulArgs[2], aulArgs[3]));
				break;

			case BRDC_OP_VAR_OR_IMM_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR OR IMM TO VAR: (%d, %d)",
					aulArgs[1], aulArgs[2]));
				break;

			case BRDC_OP_VAR_XOR_VAR_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR XOR VAR TO VAR: (%d, %d, %d)",
					aulArgs[1], aulArgs[2], aulArgs[3]));
				break;

			case BRDC_OP_VAR_XOR_IMM_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR XOR IMM TO VAR: (%d, %d)",
					aulArgs[1], aulArgs[2]));
				break;

			case BRDC_OP_NOT_VAR_TO_VAR_OPCODE:
				BDBG_MSG(("++ NOT VAR TO VAR: (%d, %d)",
					aulArgs[1], aulArgs[2]));
				break;

			case BRDC_OP_VAR_ROR_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR ROR TO VAR: (%d, %d, %d)",
					aulArgs[1], aulArgs[2], aulArgs[3]));
				break;

			case BRDC_OP_VAR_SUM_VAR_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR SUM VAR TO VAR: (%d, %d, %d)",
					aulArgs[1], aulArgs[2], aulArgs[3]));
				break;

			case BRDC_OP_VAR_SUM_IMM_TO_VAR_OPCODE:
				BDBG_MSG(("++ VAR SUM IMM TO VAR: (%d, %d)",
					aulArgs[1], aulArgs[2]));
				break;

			case BRDC_OP_COND_SKIP_OPCODE:
				BDBG_MSG(("++ COND SKIP: (%d)",
					aulArgs[1]));
				break;

			case BRDC_OP_SKIP_OPCODE:
				BDBG_MSG(("++ SKIP "));
				break;

			case BRDC_OP_EXIT_OPCODE:
				BDBG_MSG(("++ EXIT "));
				break;

			default:
				/* unknown command */
				BDBG_ERR(("unknown command"));
				goto done;
			}
			break;
			
		case BRDC_DBG_ListEntry_eRegister:
			BDBG_MSG(("REGISTER: %08x", aulArgs[0]));
			break;
			
		case BRDC_DBG_ListEntry_eData:
			BDBG_MSG(("DATA:     %08x", aulArgs[0]));
			break;
			
		default:
			/* unknown */
			eErr = BERR_TRACE(BERR_UNKNOWN);
			break;
		}
			
		/* get next entry */
		eErr = BERR_TRACE(BRDC_DBG_GetListEntry(hList, &eEntry, aulArgs));
		if(eErr != BERR_SUCCESS)
		{
			/* error parsing list */
			BDBG_ERR(("error parsing list"));
			goto done;
		}
	}
	
done:
	return eErr;
}


/**************************************************************
*
* CaptureBuffer
*
*/

BERR_Code BRDC_DBG_CreateCaptureBuffer(BRDC_DBG_CaptureBuffer *buffer, int size)
{
	BKNI_Memset(buffer, 0, sizeof(*buffer));
	buffer->size = size;
	buffer->mem = BKNI_Malloc(size);
	if (!buffer->mem)
		return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
	buffer->enable = true;
	return 0;
}

void BRDC_DBG_DestroyCaptureBuffer(BRDC_DBG_CaptureBuffer *buffer)
{
	BKNI_Free(buffer->mem);
}

static void BRDC_DBG_Write_isr(BRDC_DBG_CaptureBuffer *buffer, const void *mem_, int size)
{
	const uint8_t *mem = (uint8_t*)mem_;
	int remainder = 0;
	if (size > buffer->size - buffer->writeptr) {
		int temp = buffer->size - buffer->writeptr;
		remainder = size - temp;
		size = temp;
	}
	
	BKNI_Memcpy(&buffer->mem[buffer->writeptr], mem, size);
	buffer->writeptr += size;
	if (buffer->writeptr == buffer->size)
		buffer->writeptr = 0;
		
	if (remainder)
		BRDC_DBG_Write_isr(buffer, &mem[size], remainder);
}

void BRDC_DBG_WriteCapture_isr(BRDC_DBG_CaptureBuffer *buffer, BRDC_Slot_Handle hSlot, BRDC_List_Handle hList)
{
	int n = hList->ulEntries * sizeof(uint32_t);
	uint32_t prefix = BRDC_DBG_RUL;

	/* assumed to be called in isr context, so no synchronization is needed */

	if (buffer->enable)
	{
		char timestamp[80];

		buffer->num_ruls++;
		buffer->total_bytes += n;
		
		BKNI_Snprintf(timestamp, 80, "Slot%d timestamp = %#x", hSlot->eSlotId, BRDC_Slot_GetTimerSnapshot_isr(hSlot));
		BRDC_DBG_LogErrorCode_isr(hSlot->hRdc, BRDC_DBG_RUL_TIMESTAMP, timestamp);

		BRDC_DBG_Write_isr(buffer, &prefix, sizeof(prefix));
		BRDC_DBG_Write_isr(buffer, &hList->ulEntries, sizeof(hList->ulEntries));
		BRDC_DBG_Write_isr(buffer, &hSlot->eSlotId, sizeof(hSlot->eSlotId));
		BMEM_FlushCache_isr(hSlot->hRdc->hMem, hList->pulRULCacheAddr, n);
		BRDC_DBG_Write_isr(buffer, hList->pulRULCacheAddr, n);
	}
#if 0
	/* internal metrics */
	if (buffer->num_ruls % 1000 == 0) {
		BDBG_WRN(("total %d, total_size %d", buffer->num_ruls, buffer->total_bytes));
	}
#endif
}

void BRDC_DBG_ReadCapture_isr(BRDC_Handle rdc, uint8_t *mem, int size, int *read)
{
#ifdef BRDC_USE_CAPTURE_BUFFER
	int max;
	BRDC_DBG_CaptureBuffer *buffer = &rdc->captureBuffer;
	
	if (buffer->readptr <= buffer->writeptr) {
		max = buffer->writeptr - buffer->readptr;
	}
	else {
		/* don't read around the wrap, let the app do that */
		max = buffer->size - buffer->readptr;
	}
	
	if (size > max)
		size = max;
	*read = size;
	
	if (size) {
		BKNI_Memcpy(mem, &buffer->mem[buffer->readptr], size);
		buffer->readptr += size;
		if (buffer->readptr == buffer->size)
			buffer->readptr = 0;
	}
#else
	BSTD_UNUSED(rdc);
	BSTD_UNUSED(mem);
	BSTD_UNUSED(size);
	*read = 0;
#endif
}

#ifdef BRDC_USE_CAPTURE_BUFFER
static int BKNI_Strlen(const char *str)
{
	int total = 0;
	while (*str++) total++;
	return total;
}
#endif

void BRDC_DBG_LogErrorCode_isr(BRDC_Handle rdc, uint32_t errortype, const char *str)
{
#ifdef BRDC_USE_CAPTURE_BUFFER
	uint32_t val[2];
	val[0] = errortype;
	val[1] = BKNI_Strlen(str);
	BRDC_DBG_Write_isr(&rdc->captureBuffer, val, sizeof(val));
	BRDC_DBG_Write_isr(&rdc->captureBuffer, str, val[1]);
#else
	BSTD_UNUSED(rdc);
	BSTD_UNUSED(errortype);
	BSTD_UNUSED(str);
#endif
}


void BRDC_DBG_EnableCapture_isr(BRDC_Handle rdc, bool enable)
{
#ifdef BRDC_USE_CAPTURE_BUFFER
	BRDC_DBG_CaptureBuffer *buffer = &rdc->captureBuffer;
	buffer->enable = enable;
#else
	BSTD_UNUSED(rdc);
	BSTD_UNUSED(enable);
#endif
}


/* end of file */
