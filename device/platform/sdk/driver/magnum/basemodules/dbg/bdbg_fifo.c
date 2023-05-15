/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdbg_fifo.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 6/6/11 4:16p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dbg/bdbg_fifo.c $
 * 
 * Hydra_Software_Devel/5   6/6/11 4:16p vsilyaev
 * SW7405-4477: Resolved another warning
 * 
 * Hydra_Software_Devel/4   6/6/11 3:46p vsilyaev
 * SW7405-4477: Fixed warning
 * 
 * Hydra_Software_Devel/3   6/6/11 3:27p vsilyaev
 * SW7405-4477: Routed all debug output through buffer and use external
 * application to extract and print debug output
 * 
 * Hydra_Software_Devel/2   4/13/11 7:33p vsilyaev
 * SW7405-5221: Verify fifo status after copying data out
 * 
 * Hydra_Software_Devel/1   4/4/11 6:09p vsilyaev
 * SW7405-5221: Implementation of lock-free multi-writer/multi-reader FIFO
 *
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bdbg_fifo.h"

#if defined(__mips__) && defined(__GNUC__)
#define BDBG_P_LOG_SUPPORTED    1
#endif


#if BDBG_P_LOG_SUPPORTED
#define BDBG_P_INLINE extern __inline__ __attribute__((always_inline))
#else 
#define BDBG_P_INLINE static
#endif

BDBG_P_INLINE void 
BDBG_P_Atomic_Set(BDBG_P_Atomic *a, long val) {
	a->atomic = val;
    return;
}

BDBG_P_INLINE long 
BDBG_P_Atomic_Get(const BDBG_P_Atomic *a) {
	return a->atomic;
}

BDBG_P_INLINE long
BDBG_P_Atomic_AddReturnOld(BDBG_P_Atomic *a, long val)
{
	unsigned long result;
#if BDBG_P_LOG_SUPPORTED && defined(__mips__)
	unsigned long temp;
	__asm__ __volatile__(
		"	.set	mips32					\n"
		"1:	ll	%1, %2		# BDBG_P_Atomic_AddReturn \n"
		"	addu	%0, %1, %3				\n"
		"	sc	%0, %2					\n"
		"	beqz	%0, 1b					\n"
		"	move    %0, %1				    \n"
		"	.set	mips0					\n"
		: "=&r" (result), "=&r" (temp), "=m" (a->atomic)
		: "Ir" (val), "m" (a->atomic)
		: "memory");
#else
	a->atomic += val;
	result = a->atomic;
#endif
	return result;
}


#define BDBG_P_LOG_TAG_EMPTY    0
#define BDBG_P_LOG_TAG_ALLOCATED    1
#define BDBG_P_LOG_TAG_COMPLETED    2


struct BDBG_Fifo {
    size_t element_size;
    unsigned nelements;
    bool buffer_allocated;
    bool enabled;
    BDBG_P_Atomic write_counter;
    BDBG_OBJECT(BDBG_Fifo)
    unsigned buffer[1]; /* variable size array */
} BDBG_Fifo;

BDBG_OBJECT_ID(BDBG_Fifo);

struct BDBG_FifoReader  {
    BDBG_OBJECT(BDBG_FifoReader)
    BDBG_Fifo_CHandle writer;
    long read_counter;
};
BDBG_OBJECT_ID(BDBG_FifoReader);



void 
BDBG_Fifo_GetDefaultCreateSettings(BDBG_Fifo_CreateSettings *createSettings)
{
    BDBG_ASSERT(createSettings);
    BKNI_Memset(createSettings, 0, sizeof(*createSettings));
    createSettings->nelements = 32;
    return;
}

#if BDBG_P_LOG_SUPPORTED  
/* acual element size should be power of 2 */
static BERR_Code
BDBG_Fifo_P_NearPow2(unsigned a, unsigned *result)
{
    unsigned n;
    unsigned i;

    for(i=0,n=1;n<a;i++,n*=2) {
        if(i>=30) {
            return BERR_TRACE(BERR_NOT_SUPPORTED);
        }
    }
    *result = n;
    return BERR_SUCCESS;
}
#endif

BERR_Code 
BDBG_Fifo_Create(BDBG_Fifo_Handle *pFifo, const BDBG_Fifo_CreateSettings *createSettings)
{
    BDBG_Fifo_Handle fifo;
    BERR_Code rc;
    unsigned i;
    size_t element_size;
    unsigned nelements=0;

    BDBG_ASSERT(pFifo);
    BDBG_ASSERT(createSettings);
    if(createSettings->elementSize == 0) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if(createSettings->bufferSize == 0 && createSettings->nelements == 0) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if(createSettings->bufferSize !=0 && createSettings->buffer == NULL) {
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }

    element_size = createSettings->elementSize + 2*sizeof(BDBG_P_Atomic) - 1;
    element_size = element_size - (element_size % sizeof(BDBG_P_Atomic));

#if BDBG_P_LOG_SUPPORTED  
    if(createSettings->buffer) {
        unsigned nelementsLimit;
        unsigned bufferLeft = createSettings->bufferSize;
        if(bufferLeft<sizeof(*fifo)) {rc=BERR_TRACE(BERR_INVALID_PARAMETER);goto err_alloc;}
        bufferLeft -= sizeof(fifo);
        nelementsLimit = bufferLeft / element_size;
        rc = BDBG_Fifo_P_NearPow2(nelementsLimit, &nelements);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_nelements; }
        if(nelements>nelementsLimit) {
            nelements /=2;
            BDBG_ASSERT(nelements<=nelementsLimit);
            if(nelements==0) {rc = BERR_TRACE(BERR_NOT_SUPPORTED); goto err_nelements;}
        }
        fifo = createSettings->buffer;
        BDBG_OBJECT_INIT(fifo, BDBG_Fifo);
        fifo->buffer_allocated = false;
     } else {
        rc = BDBG_Fifo_P_NearPow2(createSettings->nelements, &nelements);
        if(rc!=BERR_SUCCESS) { rc = BERR_TRACE(rc); goto err_nelements; }
        fifo = BKNI_Malloc(sizeof(*fifo) + nelements*element_size);
        if(fifo==NULL) {rc=BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);goto err_alloc;}
        BDBG_OBJECT_INIT(fifo, BDBG_Fifo);
        fifo->buffer_allocated = true;
    }
#else
    /* short circuit execution */
    fifo = NULL; 
    rc = BERR_TRACE(BERR_NOT_SUPPORTED);goto err_alloc;
#endif
    BDBG_ASSERT(nelements>0);
    fifo->enabled = true;
    fifo->nelements = nelements;
    fifo->element_size = element_size;

    BDBG_P_Atomic_Set(&fifo->write_counter, 0);
    for(i=0;i<fifo->nelements;i++) {
        BDBG_P_Atomic *atomic;

        atomic = (void *)((uint8_t *)fifo->buffer + i*fifo->element_size);
        BDBG_P_Atomic_Set(atomic, BDBG_P_LOG_TAG_EMPTY);
    }
    *pFifo = fifo;
    return BERR_SUCCESS;

    /* BKNI_Free(fifo); */
err_alloc:
#if BDBG_P_LOG_SUPPORTED  
err_nelements:
#endif
    return rc;
}

void 
BDBG_Fifo_Destroy(BDBG_Fifo_Handle fifo)
{
    BDBG_OBJECT_ASSERT(fifo, BDBG_Fifo);
    if(fifo->buffer_allocated) {
        BDBG_OBJECT_DESTROY(fifo, BDBG_Fifo);  
        BKNI_Free(fifo);
    }
    return;
}


static void *
BDBG_Fifo_P_GetElementPointer(BDBG_Fifo_CHandle fifo, unsigned long counter)
{
    unsigned offset;
    void *buf;

    offset = (unsigned) (counter % fifo->nelements);
    offset = offset * fifo->element_size;
    buf = (uint8_t *)fifo->buffer + offset;
    return buf;
}


void *
BDBG_Fifo_GetBuffer(BDBG_Fifo_Handle fifo, BDBG_Fifo_Token *token)
{
    void *buf;
    
    BDBG_OBJECT_ASSERT(fifo, BDBG_Fifo);
    BDBG_ASSERT(token);
    if(fifo->enabled) {
        buf = BDBG_Fifo_P_GetElementPointer(fifo, BDBG_P_Atomic_AddReturnOld(&fifo->write_counter, 1));
        token->marker = buf;
        BDBG_P_Atomic_Set(token->marker, BDBG_P_LOG_TAG_EMPTY);
        return (uint8_t *)buf + sizeof(token->marker);
    } else {
        return NULL;
    }
}

void 
BDBG_Fifo_CommitBuffer(const BDBG_Fifo_Token *token)
{
    BDBG_P_Atomic_Set(token->marker, BDBG_P_LOG_TAG_COMPLETED);
    return;
}


BERR_Code 
BDBG_FifoReader_Create(BDBG_FifoReader_Handle *pReader, BDBG_Fifo_Handle fifo)
{
    BDBG_FifoReader_Handle reader;

    BDBG_ASSERT(pReader);
    /* BDBG_OBJECT_ASSERT(fifo, BDBG_Fifo); */

    reader = BKNI_Malloc(sizeof(*reader));
    if(!reader) {return BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);}

    BDBG_OBJECT_INIT(reader, BDBG_FifoReader);
    reader->writer = fifo;
    reader->read_counter = 0;
    *pReader = reader;

    return BERR_SUCCESS;
}

void
BDBG_FifoReader_Destroy(BDBG_FifoReader_Handle fifo)
{
    BDBG_OBJECT_DESTROY(fifo, BDBG_FifoReader);
    BKNI_Free(fifo);
    return;
}

BERR_Code 
BDBG_FifoReader_Read(BDBG_FifoReader_Handle fifo, void *buffer, size_t buffer_size)
{
    size_t element_size;
    long distance;
    BDBG_P_Atomic *marker;

    BDBG_OBJECT_ASSERT(fifo, BDBG_FifoReader);
    /* BDBG_OBJECT_ASSERT(fifo->writer, BDBG_Fifo); */
    BDBG_ASSERT(buffer);
    element_size = fifo->writer->element_size - sizeof(BDBG_P_Atomic);
    if(buffer_size < element_size) { return BERR_TRACE(BERR_INVALID_PARAMETER); }
    distance = BDBG_P_Atomic_Get(&fifo->writer->write_counter)-fifo->read_counter;
    if(distance == 0) {
        return BERR_FIFO_NO_DATA;
    }
    if(distance < 0 || distance >= (long)fifo->writer->nelements) {
        return BERR_FIFO_OVERFLOW;
    }
    marker = BDBG_Fifo_P_GetElementPointer(fifo->writer, fifo->read_counter);
    if(BDBG_P_Atomic_Get(marker)!=BDBG_P_LOG_TAG_COMPLETED) {
        if(distance==1) {
            return BERR_FIFO_BUSY;
        } else {
            return BERR_FIFO_NO_DATA;
        }
    }
    BKNI_Memcpy(buffer, marker+1, element_size);
    /* verify pointer after copying data */
    distance = BDBG_P_Atomic_Get(&fifo->writer->write_counter)-fifo->read_counter;
    if(distance < 0 || distance >= (long)fifo->writer->nelements) {
        return BERR_FIFO_OVERFLOW;
    }
    fifo->read_counter++;
    return BERR_SUCCESS;
}

BERR_Code 
BDBG_FifoReader_Resync(BDBG_FifoReader_Handle fifo)
{
    long distance;
    long write_counter;

    BDBG_OBJECT_ASSERT(fifo, BDBG_FifoReader);
    /* BDBG_OBJECT_ASSERT(fifo->writer, BDBG_Fifo); */
    write_counter = BDBG_P_Atomic_Get(&fifo->writer->write_counter);
    distance = write_counter-fifo->read_counter;
    if(distance == 0) {
        /* do nothing */
    } else if(distance < 0 || distance >= (long)fifo->writer->nelements) {
        fifo->read_counter = write_counter;
    } else {
        BDBG_P_Atomic *marker;
        marker = BDBG_Fifo_P_GetElementPointer(fifo->writer, fifo->read_counter);
        if(BDBG_P_Atomic_Get(marker)==BDBG_P_LOG_TAG_ALLOCATED) {
            fifo->read_counter++;
        }
    }
    return BERR_SUCCESS;
}





