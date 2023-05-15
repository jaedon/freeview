/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_capture.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/30/12 4:47p $
 *
 * Capture testing library. 
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_capture.c $
 * 
 * Hydra_Software_Devel/2   8/30/12 4:47p jgarrett
 * SW7425-3629: Fixes for cached access
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include <stdio.h>
#include "apetest_capture.h"
#include "apetest_common.h"
#include "btst_kni.h"
#include "btst.h"
#include "bkni_multi.h"
#include "bkni.h"
#include "bint.h"
#include "bkni_event_group.h"

BDBG_MODULE( aptest_Caplib );

typedef struct Capture_Vars
{   
    BAPE_Handle                 ape;
    BAPE_OutputCaptureHandle    outputCapture;
	bool                        running;
	bool                        exit; /* use this since BTST_Destroy() doesn't seem to work */
	BTST_TaskHandle             task;						
	BKNI_EventHandle            dataEvent;	
	FILE                      * file;
    long                        bytesCaptured;
	BAPE_MixerHandle            mixer;
    BAPE_OutputPort             outputPort;
    BMEM_Handle                 mem;
} Capture_Vars;



void CaptureDataTask( BTST_TaskHandle Task, void *parm )
{
	APETEST_Capture_Handle hCapture = parm;
	BSTD_UNUSED( Task );

	while( 1 )
	{
        BAPE_BufferDescriptor   bufferDescriptor;
        int16_t               * pBuffer;
        void                  * pCached;
        size_t                  bufferSize;
        size_t                  written;

        if (hCapture->exit)
        {
            return;
        }

        MSG_ERR( BAPE_OutputCapture_GetBuffer(hCapture->outputCapture, &bufferDescriptor) );

        /* We always use interleaved stereo, so take the first buffer pointer */
        (void)BMEM_ConvertAddressToCached(hCapture->mem, bufferDescriptor.buffers[0].pBuffer, &pCached);
        pBuffer = pCached;
        bufferSize = bufferDescriptor.bufferSize;

        /* Buffer is empty.  Wait for the next interrupt. */
        if ( bufferDescriptor.bufferSize == 0 )
        {
            BKNI_WaitForEvent( hCapture->dataEvent, 100 );
            continue;
        }

        if (hCapture->running)
        {
            /* could mod 4k for performance... */
            (void)BMEM_FlushCache(hCapture->mem, pBuffer, bufferSize);
            written = fwrite(pBuffer, sizeof( char ), bufferSize, hCapture->file);
            if (written != bufferSize)
            {
                printf("%s written(%d) != bufferSize(%d)\n", __FUNCTION__, written, bufferSize);
                /* for consumeData below */
                bufferSize = written; 
            }
            hCapture->bytesCaptured += written;
        }

        MSG_ERR( BAPE_OutputCapture_ConsumeData(hCapture->outputCapture, bufferSize) );
    }

    printf("%s leaving\n", __FUNCTION__);
}



static void capture_callback_isr(void *pParam1, int param2)
{
    APETEST_Capture_Handle hCapture = (APETEST_Capture_Handle) pParam1;
	BDBG_ASSERT( hCapture );
	BSTD_UNUSED( param2 );

    BKNI_SetEvent_isr(hCapture->dataEvent);
}



BERR_Code APETEST_Capture_Open(     
    BMEM_Handle mem,
	BAPE_Handle ape,
    APETEST_Capture_Handle * phCapture,
   	BAPE_MixerHandle hMixer
	)
{
    BAPE_OutputCaptureInterruptHandlers outputCaptureInterrupts;
	APETEST_Capture_Handle hCapture = (APETEST_Capture_Handle)BKNI_Malloc(sizeof(Capture_Vars));
	BDBG_ASSERT( hCapture );
	BDBG_ASSERT( hMixer );

    if (hCapture == NULL)
    {
        return BERR_OUT_OF_SYSTEM_MEMORY;
    }

    *phCapture = hCapture;
    BKNI_Memset(hCapture, 0, sizeof(*hCapture));

    hCapture->mem = mem;
	hCapture->ape = ape;
    hCapture->mixer = hMixer;
	/* 
	 * Create semaphores and tasks.  Make sure the Capture tasks are in a stopped state
	 * (don't allow them to acquire the semaphore until a Capture is actually started).
	 * To do this, the event must be created BEFORE the Capture task is 
	 * created (started).
	 */
	RET_ERR( BKNI_CreateEvent( &( hCapture->dataEvent )) );

    /* Open outputCapture */
    RET_ERR( BAPE_OutputCapture_Open( hCapture->ape, 0, NULL, &(hCapture->outputCapture)) );

    /* Register the outputCapture interrupts */
    BAPE_OutputCapture_GetInterruptHandlers( hCapture->outputCapture, &outputCaptureInterrupts);
    outputCaptureInterrupts.watermark.pCallback_isr = capture_callback_isr;
    outputCaptureInterrupts.watermark.pParam1 = hCapture;
    RET_ERR( BAPE_OutputCapture_SetInterruptHandlers(hCapture->outputCapture, &outputCaptureInterrupts) );

#if 1
    BAPE_OutputCapture_GetOutputPort(hCapture->outputCapture, &(hCapture->outputPort));
    RET_ERR( BAPE_Mixer_AddOutput(hCapture->mixer, hCapture->outputPort) );

#endif

    /* Create the task last after the handles are valid */
    RET_ERR( BTST_CreateTask( &( hCapture->task ), CaptureDataTask, ( void * ) hCapture ) );


	return BERR_SUCCESS;
}

void APETEST_Capture_Close( 
	APETEST_Capture_Handle hCapture 
	)
{
 	BDBG_ASSERT( hCapture );

    if (hCapture->running)
    {
        APETEST_Capture_Stop( hCapture );
    }

    if (hCapture->outputPort)
    {
        BAPE_Mixer_RemoveOutput(hCapture->mixer, hCapture->outputPort);
    }
    BAPE_OutputCapture_Close( hCapture->outputCapture );

    hCapture->exit=true;
    BKNI_SetEvent( hCapture->dataEvent );
    BKNI_Sleep(10);
    BTST_DestroyTask( hCapture->task ); 

    BKNI_DestroyEvent( hCapture->dataEvent );

    BKNI_Free( hCapture );
}



BERR_Code APETEST_Capture_Start( 
	APETEST_Capture_Handle hCapture, 
	const char *filename
	)
{
	BDBG_ASSERT( hCapture );
	BDBG_ASSERT( filename );
    
    hCapture->bytesCaptured = 0;
    hCapture->file = fopen(filename,"wb"); 
    if (!hCapture->file) 
    {
        printf("can't open file '%s'\n", filename);
        return BERR_UNKNOWN;
    }

   
    hCapture->running = true;
    return BERR_SUCCESS;
}


BERR_Code APETEST_Capture_Stop(
    APETEST_Capture_Handle hCapture
    )
{
	BDBG_ASSERT( hCapture );

    hCapture->running = false;

    if (hCapture->file)
    {
        fclose( hCapture->file );
        hCapture->file = NULL;
    }

	return BERR_SUCCESS;
}


long APETEST_Capture_BytesCaptured( APETEST_Capture_Handle hCapture )
{
	BDBG_ASSERT( hCapture );
    return (hCapture->bytesCaptured);
}


