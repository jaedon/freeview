/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_fifo.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 6/6/11 3:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/bdbg_fifo.h $
 * 
 * Hydra_Software_Devel/2   6/6/11 3:27p vsilyaev
 * SW7405-4477: Routed all debug output through buffer and use external
 * application to extract and print debug output
 * 
 * Hydra_Software_Devel/1   4/4/11 6:09p vsilyaev
 * SW7405-5221: Implementation of lock-free multi-writer/multi-reader FIFO
 * 
 ***************************************************************************/

#ifndef BDBG_FIFO_H
#define BDBG_FIFO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BDBG_P_Atomic {
	volatile long atomic;
} BDBG_P_Atomic;

typedef struct BDBG_Fifo_Token BDBG_Fifo_Token;

struct BDBG_Fifo_Token {
    BDBG_P_Atomic *marker;
};

typedef struct BDBG_Fifo *BDBG_Fifo_Handle;
typedef const struct BDBG_Fifo *BDBG_Fifo_CHandle;
typedef struct BDBG_FifoReader *BDBG_FifoReader_Handle;

typedef struct BDBG_Fifo_CreateSettings {
    size_t elementSize; /* size of the single element, mandatory */
    unsigned nelements; /* [optional] number of elements */
    size_t bufferSize; /* [optional] size of the preallocated buffer */
    void *buffer;       /* [optional] pointer to preallocated buffer */
} BDBG_Fifo_CreateSettings;



#define BERR_FIFO_NO_DATA   17
#define BERR_FIFO_BUSY      18
#define BERR_FIFO_OVERFLOW  19


void BDBG_Fifo_GetDefaultCreateSettings(BDBG_Fifo_CreateSettings *createSettings);
BERR_Code BDBG_Fifo_Create(BDBG_Fifo_Handle *pFifo, const BDBG_Fifo_CreateSettings *createSettings);
void BDBG_Fifo_Destroy(BDBG_Fifo_Handle fifo);
void *BDBG_Fifo_GetBuffer(BDBG_Fifo_Handle fifo, BDBG_Fifo_Token *token);
void BDBG_Fifo_CommitBuffer(const BDBG_Fifo_Token *token);
BERR_Code BDBG_FifoReader_Create(BDBG_FifoReader_Handle *pReader, BDBG_Fifo_Handle fifo);
void BDBG_FifoReader_Destroy(BDBG_FifoReader_Handle fifo);
BERR_Code BDBG_FifoReader_Read(BDBG_FifoReader_Handle fifo, void *buffer, size_t buffer_size);
BERR_Code BDBG_FifoReader_Resync(BDBG_FifoReader_Handle fifo);

#ifdef __cplusplus
}
#endif

#endif  /* BDBG_FIFO_H */
