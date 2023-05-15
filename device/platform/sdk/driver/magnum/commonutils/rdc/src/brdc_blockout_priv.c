/***************************************************************************
 *     Copyright (c) 2005-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: brdc_blockout_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 8/13/12 3:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/rdc/7038/brdc_blockout_priv.c $
 * 
 * Hydra_Software_Devel/8   8/13/12 3:21p yuxiaz
 * SW7425-3626: Replace uncache memory access with cache
 * memory access: use the new API names in RDC.
 * 
 * Hydra_Software_Devel/7   8/9/12 9:56a yuxiaz
 * SW7425-3626: Replace uncache memory access with cache memory access.
 * 
 * Hydra_Software_Devel/6   6/9/09 6:21p pntruong
 * PR55861: Added support for hw blockout and timestamp.
 *
 * Hydra_Software_Devel/5   3/24/09 5:28p albertl
 * PR52513: Moved RDC debug globals into hList structure.
 *
 * Hydra_Software_Devel/4   7/9/08 10:32a jessem
 * PR 44647: Corrected bounds check on ulRegBlock parameter in
 * BRDC_P_ValidateBlockOutRegisters.
 *
 * Hydra_Software_Devel/3   3/5/08 10:53a jessem
 * PR 38623: Removed stdio.h and errno.h.
 *
 * Hydra_Software_Devel/2   2/28/08 5:55p jessem
 * PR 38623: Changed register validation to support 3563 only.
 *
 * Hydra_Software_Devel/1   2/28/08 5:46p jessem
 * PR 38623: Initial version
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"

#include "bchp_common.h"
#include "brdc.h"
#include "brdc_dbg.h"
#include "brdc_private.h"
#include "brdc_blockout_priv.h"

BDBG_MODULE(BRDC_BLOCKOUT);

typedef struct
{
	uint32_t ulOpcode;
	uint32_t ulReadSize;
	uint32_t ulWriteSize;
	uint32_t ulArg;
	uint32_t ulNumImmData;
	uint32_t ulSrcReg;
	uint32_t ulDstReg;
} BRDC_RulEntry;

typedef struct
{
	uint32_t ulReg2BlockOutStart;
	uint32_t ulReg2BlockOutEnd;
} BRDC_Reg2BlockOut;

/* Checks if register block is to blocked out andif so return start and end registers that are blocked. */
static bool BRDC_P_CheckAndGetBlockOutRegs
	( BRDC_Handle          hRdc,
	  uint32_t             ulRegister,
	  uint32_t             ulBlockSize,
	  BRDC_Reg2BlockOut   *pstReg2BlockOut,
	  uint32_t            *pNumRegRangesBlocked )
{
	uint32_t i, j;
	bool bGotStart = false, bGotEnd = false;
	bool bBlockOut = false;
	uint32_t ulReg2CheckStart;
	uint32_t ulReg2CheckEnd;
	BRDC_BlockOut *pstRegBlockOutList = hRdc->astBlockOut;

	*pNumRegRangesBlocked = 0;

	for (i=0; i<BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT; i++)
	{
		uint32_t ulRegBlockOutListStart, ulRegBlockOutListEnd;
		bGotStart = bGotEnd = false;

		if (pstRegBlockOutList[i].bEnable)
		{
			ulRegBlockOutListStart = pstRegBlockOutList[i].ulStartRegAddr;
			ulRegBlockOutListEnd = pstRegBlockOutList[i].ulStartRegAddr +
								((pstRegBlockOutList[i].ulBlockSize-1) * sizeof(uint32_t));

			ulReg2CheckStart = ulRegister;
			ulReg2CheckEnd = ulRegister + ((ulBlockSize-1) * sizeof(uint32_t));

			for (j=0; j<ulBlockSize; j++)
			{
				if ((ulReg2CheckStart >= ulRegBlockOutListStart) &&
					(ulReg2CheckStart <= ulRegBlockOutListEnd))
				{
					if (!bGotStart)
					{
						bGotStart = true;
						pstReg2BlockOut[*pNumRegRangesBlocked].ulReg2BlockOutStart =
							(ulReg2CheckStart <= ulRegBlockOutListStart) ? ulRegBlockOutListStart : ulReg2CheckStart;
					}
				}

				if (!bGotEnd && bGotStart)
				{
					bGotEnd = bBlockOut = true;
					pstReg2BlockOut[*pNumRegRangesBlocked].ulReg2BlockOutEnd =
						(ulReg2CheckEnd < ulRegBlockOutListEnd) ? ulReg2CheckEnd : ulRegBlockOutListEnd;
					(*pNumRegRangesBlocked)++;
					break;
				}

				ulReg2CheckStart += sizeof(uint32_t);

				if (bBlockOut)
				{
					BDBG_MSG(("Register block 0x%x to 0x%x blocked",
						pstReg2BlockOut[*pNumRegRangesBlocked].ulReg2BlockOutStart,
						pstReg2BlockOut[*pNumRegRangesBlocked].ulReg2BlockOutEnd));
				}
			}
		}
	}

	if (!bBlockOut)
	{
		BDBG_MSG(("Reg block 0x%x to 0x%x not blocked", ulRegister,
			ulRegister + (ulBlockSize - 1) * 4));
	}

	return bBlockOut;
}


static bool BRDC_P_PopulateRevisedRul_isr
	( BRDC_Handle        hRdc,
	  BRDC_List_Handle   hList,
	  uint32_t         **pulOrigRulAddr,
	  uint32_t         **pulRevisedRulAddr,
	  BRDC_RulEntry     *pstRulEntry )
{
	uint32_t            ulOpcode;
	uint32_t            i;
	BRDC_DBG_ListEntry  eEntry;
	uint32_t            aulArgs[4];
	bool                bBlockOut;
	uint32_t            ulNumRegRangesBlocked;

	BRDC_Reg2BlockOut  astReg2BlockOut[BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT];

	BKNI_Memset((void *)astReg2BlockOut, 0x0,
		sizeof(BRDC_Reg2BlockOut) * BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT);

	/* check if RUL has blocked out registers */
	bBlockOut = BRDC_P_CheckAndGetBlockOutRegs(hRdc,
				pstRulEntry->ulDstReg, pstRulEntry->ulWriteSize,
				astReg2BlockOut, &ulNumRegRangesBlocked);

	if (bBlockOut)
	{
		uint32_t ulWriteSize = 0;
		uint32_t ulDstReg = pstRulEntry->ulDstReg;
		uint32_t ulSrcReg = pstRulEntry->ulSrcReg;
		uint32_t ulNumImmData;

		if ((pstRulEntry->ulOpcode == BRDC_OP_IMMS_TO_REGS_OPCODE) ||
			(pstRulEntry->ulOpcode == BRDC_OP_REGS_TO_REGS_OPCODE) ||
			(pstRulEntry->ulOpcode == BRDC_OP_REG_TO_REGS_OPCODE))
		{
			for (i=0; i<ulNumRegRangesBlocked; i++)
			{
				/* get write size */
				ulWriteSize = (astReg2BlockOut[i].ulReg2BlockOutStart - ulDstReg) / sizeof(uint32_t);

				/* Check if the RUL is completely inside a blocked out range. If so, exclude it. */
				if (ulWriteSize > 0)
				{
					ulOpcode = pstRulEntry->ulOpcode | ((ulWriteSize & UINT32_C(0xFFF)) - 1);
					*(*pulRevisedRulAddr) = ulOpcode;
					(*pulRevisedRulAddr)++;

					if ((pstRulEntry->ulOpcode == BRDC_OP_REGS_TO_REGS_OPCODE) ||
						(pstRulEntry->ulOpcode == BRDC_OP_REG_TO_REGS_OPCODE))
					{
						*(*pulRevisedRulAddr) = BRDC_REGISTER(ulSrcReg);
						(*pulRevisedRulAddr)++;
					}

					*(*pulRevisedRulAddr) = BRDC_REGISTER(ulDstReg);
					(*pulRevisedRulAddr)++;
				}

				if (pstRulEntry->ulOpcode == BRDC_OP_IMMS_TO_REGS_OPCODE)
				{
					uint32_t j;

					/* The number of imm data includes the inculded and excluded data.
					   The excluded data will be dropped. */
					ulNumImmData = ulWriteSize + (((astReg2BlockOut[i].ulReg2BlockOutEnd -
									astReg2BlockOut[i].ulReg2BlockOutStart) / sizeof(uint32_t)) + 1);

					/* Get immediate data */
					for (j=0; j<ulNumImmData; j++)
					{
						/* Increment original RUL list to keep in pace with the
						   RUL entry retrieval order and amount. */
						(*pulOrigRulAddr)++;

						if(BRDC_DBG_GetListEntry_isr(hList, &eEntry, aulArgs))
						{
							/* error */
							BDBG_ERR(("ERROR parsing list %d", __LINE__));
						}
						else if (eEntry == BRDC_DBG_ListEntry_eEnd)
						{
							BDBG_ERR(("Incorrect number of RUL entries"));
						}
						else
						{
							/* ignore imm data after write size since the dest for these data are
							   blocked but continue to get it to set the RUL pointer
							   to the correct location */
							if (j < ulWriteSize)
							{
								*(*pulRevisedRulAddr) = aulArgs[0];
								(*pulRevisedRulAddr)++;
							}
						}
					}
				}

				/* get next dest reg */
				ulDstReg = astReg2BlockOut[i].ulReg2BlockOutEnd + (1 * sizeof(uint32_t));

				/* get next src reg */
				if (pstRulEntry->ulOpcode == BRDC_OP_REGS_TO_REGS_OPCODE)
				{
					ulSrcReg += (ulWriteSize * sizeof(uint32_t)) + (astReg2BlockOut[i].ulReg2BlockOutEnd -
								astReg2BlockOut[i].ulReg2BlockOutStart);
				}
			}

			/* Check if there are some remaining writes to be made */
			if (ulDstReg < (pstRulEntry->ulDstReg + (pstRulEntry->ulWriteSize * sizeof(uint32_t))))
			{
				ulWriteSize = ((pstRulEntry->ulDstReg + (pstRulEntry->ulWriteSize * sizeof(uint32_t))) -
								ulDstReg) / sizeof(uint32_t);

				ulOpcode = pstRulEntry->ulOpcode | ((ulWriteSize & UINT32_C(0xFFF)) - 1);
				*(*pulRevisedRulAddr) = ulOpcode;
				(*pulRevisedRulAddr)++;

				if ((pstRulEntry->ulOpcode == BRDC_OP_REGS_TO_REGS_OPCODE) ||
						(pstRulEntry->ulOpcode == BRDC_OP_REG_TO_REGS_OPCODE))
				{
					*(*pulRevisedRulAddr) = BRDC_REGISTER(ulSrcReg);
					(*pulRevisedRulAddr)++;
				}

				*(*pulRevisedRulAddr) = BRDC_REGISTER(ulDstReg);
				(*pulRevisedRulAddr)++;

				if (pstRulEntry->ulOpcode == BRDC_OP_IMMS_TO_REGS_OPCODE)
				{
					/* Get immediate data */
					for (i=0; i<ulWriteSize; i++)
					{
						/* Increment original RUL list to keep in pace with the
						   RUL entry retrieval order and amount. */
						(*pulOrigRulAddr)++;

						if (BRDC_DBG_GetListEntry_isr(hList, &eEntry, aulArgs))
						{
							/* error */
							BDBG_ERR(("ERROR parsing list %d", __LINE__));
						}
						else if (eEntry == BRDC_DBG_ListEntry_eEnd)
						{
							BDBG_ERR(("Incorrect number of RUL entries"));
						}
						else
						{
							*(*pulRevisedRulAddr) = aulArgs[0];
							(*pulRevisedRulAddr)++;
						}
					}
				}
			}
		}
		else
		{
			BDBG_MSG(("Register 0x%x blocked", pstRulEntry->ulDstReg));
		}
	}

	return bBlockOut;
}


/* Searches a list and looks for the specified register blocks. If found, create a new list that
   excludes these registers and replace the list with the new one. */
BERR_Code BRDC_P_ParseAndReplaceRul_isr
	( BRDC_List_Handle hList)
{
	BERR_Code           err = BERR_SUCCESS;
	BRDC_DBG_ListEntry  eEntry;
	uint32_t            aulArgs[4];
	bool                bRegWrite = false;
	bool                bReg2RegWrite = false;
	bool                bGo = false;
	uint32_t           *pulAddress, *pulStartAddress, *pulRevisedRulStartAddr, *pulRevisedRulCurrAddr;
	uint32_t            ulOpcode = 0, ulSrcRegAddr = 0, ulDstRegAddr = 0;
	uint32_t            ulNumEntries;
	BRDC_RulEntry       stRulEntry;
	BRDC_Handle      	hRdc;
	BRDC_List_Handle    hRdcBlockOutList;

	BDBG_ASSERT(hList);

	hRdc = hList->hRdc;
	hRdcBlockOutList = hRdc->hRdcBlockOutList;

	/* get cached address to list */
	pulStartAddress = pulAddress = BRDC_List_GetStartAddress_isr(hList);
	pulRevisedRulStartAddr = pulRevisedRulCurrAddr = BRDC_List_GetStartAddress_isr(hRdcBlockOutList);

	/* prepare to traverse this list */
	err = BRDC_DBG_SetList_isr(hList);
	if (err != BERR_SUCCESS)
	{
		/* error */
		BDBG_ERR(("ERROR parsing list %d", __LINE__));
		goto done;
	}

	/* get first entry in list */
	err = BRDC_DBG_GetListEntry_isr(hList, &eEntry, aulArgs);
	if (err != BERR_SUCCESS)
	{
		/* error */
		BDBG_ERR(("ERROR parsing list %d", __LINE__));
		goto done;
	}

	/* traverse until finished */
	while(eEntry != BRDC_DBG_ListEntry_eEnd)
	{
		/* command entry? */
		if(eEntry == BRDC_DBG_ListEntry_eCommand)
		{
			/* init  */
			BKNI_Memset((void *)&stRulEntry, 0x0, sizeof(BRDC_RulEntry));
			stRulEntry.ulReadSize = stRulEntry.ulWriteSize = 1;
			ulOpcode = ulSrcRegAddr = ulDstRegAddr = 0;
			bRegWrite = bReg2RegWrite = false;
			bGo = false;

			/* Get the opcode and its args */
			stRulEntry.ulOpcode = aulArgs[0];

			/* is this a write to register command? */
			switch(stRulEntry.ulOpcode)
			{
				case BRDC_OP_VAR_TO_REG_OPCODE:
					stRulEntry.ulArg = aulArgs[1];
					bRegWrite = true;
					break;
				case BRDC_OP_IMM_TO_REG_OPCODE:
					stRulEntry.ulNumImmData = 1;
					bRegWrite = true;
					break;
				case BRDC_OP_IMMS_TO_REG_OPCODE:
					stRulEntry.ulNumImmData = aulArgs[1];
					bRegWrite = true;
					break;
				case BRDC_OP_IMMS_TO_REGS_OPCODE:
					stRulEntry.ulReadSize = stRulEntry.ulWriteSize = stRulEntry.ulNumImmData = aulArgs[1];
					bRegWrite = true;
					break;
				case BRDC_OP_REG_TO_REG_OPCODE:
					stRulEntry.ulArg = aulArgs[1];
					bReg2RegWrite = true;
					break;
				case BRDC_OP_REGS_TO_REG_OPCODE:
					stRulEntry.ulReadSize = aulArgs[1];
					bReg2RegWrite = true;
					break;
				case BRDC_OP_REGS_TO_REGS_OPCODE:
					stRulEntry.ulReadSize = stRulEntry.ulWriteSize = aulArgs[1];
					bReg2RegWrite = true;
					break;
				case BRDC_OP_REG_TO_REGS_OPCODE:
					stRulEntry.ulWriteSize = aulArgs[1];
					bReg2RegWrite = true;
					break;
				default:
					break;
			}

			/* store opcode */
			ulOpcode = *pulAddress;

		}
		else if (eEntry == BRDC_DBG_ListEntry_eRegister)
		{
			if (bRegWrite)
			{
				stRulEntry.ulDstReg = aulArgs[0];
				ulDstRegAddr = *pulAddress;
				bGo = true;
			}
			else if (bReg2RegWrite)
			{
				if (stRulEntry.ulSrcReg)
				{
					/* Got dest register block */
					stRulEntry.ulDstReg = aulArgs[0];
					ulDstRegAddr = *pulAddress;
					bGo = true;
				}
				else
				{
					/* Got src register block */
					stRulEntry.ulSrcReg = aulArgs[0];
					ulSrcRegAddr = *pulAddress;
				}
			}
		}
		else /* eEntry == BRDC_DBG_ListEntry_eData */
		{
			bGo = false;
 		}

		/* RUL doesn't write to any registers.
		   Or get the rest of the immediate data if opcode
		   involves a register write that is not blocked.
		 */
		if (!bRegWrite && !bReg2RegWrite)
		{
			/* Simply add the unrevised RUL entry */
			*pulRevisedRulCurrAddr = *pulAddress;
			pulRevisedRulCurrAddr++;
		}
		else
		{
			if (bGo)
			{
				/* Revise the RUL entries if dest register/s is/are blocked.
				   Any immeditate data will be grabbed inside the call.*/
				if (BRDC_P_PopulateRevisedRul_isr(hRdc, hList,
					&pulAddress, &pulRevisedRulCurrAddr, &stRulEntry) == false)
				{
					/* Dest register/s aren't blocked so restore original RUL entries */
					*pulRevisedRulCurrAddr = ulOpcode;
					pulRevisedRulCurrAddr++;

					if (ulSrcRegAddr)
					{
						*pulRevisedRulCurrAddr = ulSrcRegAddr;
						pulRevisedRulCurrAddr++;
					}

					*pulRevisedRulCurrAddr = ulDstRegAddr;
					pulRevisedRulCurrAddr++;

					/* this allows immediate data, if any, to get stored */
					bRegWrite = bReg2RegWrite = false;
				}
			}
		}

		/* get next entry in list */
		pulAddress++;

		if(BRDC_DBG_GetListEntry_isr(hList, &eEntry, aulArgs))
		{
			/* error */
			BDBG_ERR(("ERROR parsing list %d", __LINE__));
			goto done;
		}
	}

	/* Recopy to current RUL */
	ulNumEntries = pulRevisedRulCurrAddr - pulRevisedRulStartAddr;

	BDBG_MSG(("%d revised RUL entries", ulNumEntries));
	BDBG_MSG(("%d orig RUL entries", (pulStartAddress - pulAddress)));

	BDBG_ASSERT(ulNumEntries < BRDC_P_RDC_BLOCKOUT_RUL_MAX_ENTRY);
	BKNI_Memcpy(pulStartAddress, pulRevisedRulStartAddr, sizeof(uint32_t) * ulNumEntries);
	BRDC_List_SetNumEntries_isr(hList, ulNumEntries);

done:
	return err;
}

BERR_Code BRDC_P_ValidateBlockOutRegisters
	( const BRDC_BlockOut *pstBlockOut,
	  uint32_t             ulRegBlock )
{
	BERR_Code err = BERR_SUCCESS;

	if (ulRegBlock >= BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT)
	{
		BDBG_ERR(("Only %d register blocks are allowed",
			BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT));
		err = BERR_INVALID_PARAMETER;
		return err;
	}
	else
	{
		uint32_t ulStartAddr;
		uint32_t ulEndAddr;

		ulStartAddr = pstBlockOut->ulStartRegAddr;
		ulEndAddr = pstBlockOut->ulStartRegAddr + (pstBlockOut->ulBlockSize * sizeof(uint32_t));

#ifdef BCHP_REGISTER_END
		if ((ulStartAddr > BCHP_REGISTER_END) || (ulEndAddr > BCHP_REGISTER_END))
		{
			BDBG_ERR(("Register block 0x%x to 0x%x is out of valid register range",
				ulStartAddr, ulEndAddr));
			err = BERR_INVALID_PARAMETER;
			return err;
		}
#endif
	}
	return err;
}

BERR_Code BRDC_P_RdcBlockOutInit
	( BRDC_Handle           hRdc )
{
	BERR_Code err = BERR_SUCCESS;

	BKNI_Memset((void *)hRdc->astBlockOut, 0x0, sizeof(BRDC_BlockOut) * BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT);

	err = BRDC_List_Create(hRdc, BRDC_P_RDC_BLOCKOUT_RUL_MAX_ENTRY, &hRdc->hRdcBlockOutList);
	if (err != BERR_SUCCESS)
	{
		BDBG_ERR(("ERROR: BRDC_List_Create"));
	}

	hRdc->bRdcBlockOutEnabled = false;
	return err;
}

BERR_Code BRDC_P_RdcBlockOutDestroy
	( BRDC_Handle           hRdc )
{
	BERR_Code err = BERR_SUCCESS;

	if (hRdc->hRdcBlockOutList)
	{
		/* deallocate RDC blockout list */
		err = BRDC_List_Destroy(hRdc->hRdcBlockOutList);
		if (err != BERR_SUCCESS)
		{
			BDBG_ERR(("ERROR: BRDC_List_Create"));
			return err;
		}
		hRdc->hRdcBlockOutList = NULL;
	}

	hRdc->bRdcBlockOutEnabled = false;

	return err;
}

bool BRDC_P_IsRdcBlockOutEnabled
	( BRDC_Handle			hRdc )
{
	uint32_t i;
	bool bEnabled = false;

	/* Find a register block that is enabled for block out */
	for(i=0; i<BRDC_MAX_RDC_BLOCKOUT_LIST_COUNT; i++)
	{
		if ((hRdc->astBlockOut[i].bEnable == true) && (hRdc->bRdcBlockOutEnabled))
		{
			bEnabled = true;
			break;
		}
	}
	return bEnabled;
}

/* End of file */
