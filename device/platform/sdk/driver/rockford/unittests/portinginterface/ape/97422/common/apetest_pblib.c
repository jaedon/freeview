/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: apetest_pblib.c $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 8/30/12 4:47p $
 *
 * Playback testing library. 
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/ape/97422/common/apetest_pblib.c $
 * 
 * Hydra_Software_Devel/2   8/30/12 4:47p jgarrett
 * SW7425-3629: Fixes for cached access
 * 
 * Hydra_Software_Devel/1   7/10/12 9:25a jgarrett
 * SW7425-3457: Adding ape_decode application
 * 
 * Hydra_Software_Devel/4   12/18/10 4:09p gmullen
 * SW7425-15: Updated PI testing app
 * 
 * Hydra_Software_Devel/3   11/12/10 3:27p gmullen
 * SW7425-15: Added emulation support
 * 
 * Hydra_Software_Devel/2   11/2/10 1:42p gmullen
 * SW7425-15: Fixed compile issue
 * 
 * Hydra_Software_Devel/2   11/2/10 1:23p gmullen
 * SW7422-20: Fixed compile issues.
 * 
 * Hydra_Software_Devel/1   10/25/10 10:28a gmullen
 * SW7422-20: Added testapp files
 * 
 * Hydra_Software_Devel/3   12/6/07 1:25p gmullen
 * PR36900: Added support for 7325
 * 
 * Hydra_Software_Devel/2   8/31/07 10:28a gmullen
 * PR15309: Updated tests.
 * 
 * Hydra_Software_Devel/7   6/15/07 8:11a gmullen
 * PR29691: Added test code to recreate this error condition.
 * 
 * Hydra_Software_Devel/6   5/15/07 8:23a gmullen
 * PR29510: Updated tests for PR 29510
 * 
 * Hydra_Software_Devel/5   5/10/07 3:33p gmullen
 * PR15309: Updated for new RAVE and playback tests.
 * 
 * Hydra_Software_Devel/4   3/8/07 5:47p gmullen
 * PR15309: Added playback with timestamp test.
 * 
 * Hydra_Software_Devel/3   11/30/06 2:18p gmullen
 * PR15309: Added support for PB 2 and 3.
 * 
 * Hydra_Software_Devel/2   11/23/05 9:22a gmullen
 * PR15309: Added verification support,.
 * 
 * Hydra_Software_Devel/1   9/21/05 2:20p gmullen
 * PR15309: Added tests for playback and changed RAVE tests for new PI.
 * 
 *
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"
#include <stdio.h>
#include "apetest_pblib.h"
#include "btst_kni.h"
#include "btst.h"
#include "bkni_multi.h"
#include "bkni.h"
#include "bchp_int_id_xpt_pb0.h"
#include "bchp_int_id_xpt_pb1.h"
#if BCHP_CHIP == 7325 
#else
    #include "bchp_int_id_xpt_pb2.h"
    #include "bchp_int_id_xpt_pb3.h"
#endif
#include "bint.h"

BDBG_MODULE( aptest_pblib );

#define BUFFER_LEN	( 1 * ( 512 * 1024 ) )  

typedef struct PbLib_Vars
{
	BXPT_Handle hXpt;
	unsigned ChannelNum;
	bool Running;
	void *Buffer;
	BTST_TaskHandle hPbTask;						
	BKNI_EventHandle PbStartEvent;	
	BKNI_EventHandle PbDoneEvent;	
	BKNI_EventHandle DescriptorDoneEvent;	
	BXPT_Playback_Handle hPb;
	BXPT_PvrDescriptor *Descriptor;
	size_t BufferSize;
	FILE *InFilePtr;
	BINT_CallbackHandle DescDoneIntCb;
	bool Repeat;
    bool Quit;
}
PbLib_Vars;

static PbLib_Vars PlaybackVars[ BXPT_NUM_PLAYBACKS ];

static void FeedDataTask( BTST_TaskHandle Task, void *Parm );
static void FeedData( APETEST_PbLib_Handle hPbLib );
static BINT_Id GetDoneIntId( unsigned ChannelNum );
static void DescDoneIsr( void *Parm1, int Parm2 );

APETEST_PbLib_Handle APETEST_PbLib_Init( 
	BXPT_Handle hXpt,
	unsigned PbChannelNum 
	)
{
	BXPT_Playback_ChannelSettings DefaultPbSettings;

	APETEST_PbLib_Handle hPbLib = &PlaybackVars[ PbChannelNum ];

	hPbLib->hXpt = hXpt;
	hPbLib->ChannelNum = PbChannelNum;
	hPbLib->Running = false;
	hPbLib->Repeat = false;
/*chz 	hPbLib->Buffer = ( unsigned char * ) BMEM_AllocAligned( hXpt->hMemory, BUFFER_LEN, 2, 0 ); */
	hPbLib->Buffer = ( unsigned char * ) BMEM_AllocAligned( hXpt->hMemory, BUFFER_LEN, 4, 0 ); 
	BDBG_ASSERT( hPbLib->Buffer );

	/* 
	 * Create semaphores and tasks.  Make sure the playback tasks are in a stopped state
	 * (don't allow them to acquire the semaphore until a playback is actually started).
	 * To do this, the event must be created BEFORE the playback task is 
	 * created (started).
	 */
	BKNI_CreateEvent( &( hPbLib->PbStartEvent ) );
	BKNI_CreateEvent( &( hPbLib->PbDoneEvent ) );
	BKNI_CreateEvent( &( hPbLib->DescriptorDoneEvent ) );
	BTST_CreateTask( &( hPbLib->hPbTask ), FeedDataTask, ( void * ) &( PlaybackVars[ PbChannelNum ] ) );

	/* Get a playback channel handle. */
	BXPT_Playback_GetChannelDefaultSettings( hXpt, PbChannelNum, &DefaultPbSettings );
	BXPT_Playback_OpenChannel( hXpt, &( hPbLib->hPb ), PbChannelNum, &DefaultPbSettings );
	
	hPbLib->Descriptor = BMEM_AllocAligned( hXpt->hMemory, sizeof( BXPT_PvrDescriptor ), 4, 0 );
	BDBG_ASSERT( hPbLib->Descriptor );

	BINT_CreateCallback( &hPbLib->DescDoneIntCb, hPbLib->hXpt->hInt, GetDoneIntId( hPbLib->ChannelNum ), 
		DescDoneIsr, ( void * ) hPbLib, 0 );
	BINT_EnableCallback( hPbLib->DescDoneIntCb );
		
	return( hPbLib );
}

void APETEST_PbLib_Shutdown( 
	APETEST_PbLib_Handle hPbLib 
	)
{
	BMEM_Free( hPbLib->hXpt->hMemory, hPbLib->Buffer );
	BMEM_Free( hPbLib->hXpt->hMemory, hPbLib->Descriptor );
	BXPT_Playback_CloseChannel( hPbLib->hPb );
	BINT_DestroyCallback( hPbLib->DescDoneIntCb );
    hPbLib->Quit = true;
    BKNI_SetEvent(hPbLib->PbStartEvent);
    BTST_DestroyTask(hPbLib->hPbTask);
    BKNI_DestroyEvent(hPbLib->PbStartEvent);
    BKNI_DestroyEvent(hPbLib->PbDoneEvent);
    BKNI_DestroyEvent(hPbLib->DescriptorDoneEvent);
    BKNI_Memset(hPbLib, 0, sizeof(&hPbLib));
}

BERR_Code APETEST_PbLib_DoPlay( 
	APETEST_PbLib_Handle hPbLib, 
	char *FileName,
	unsigned int SyncMode  
	)
{
	BXPT_Playback_ChannelSettings ChannelSettings;

	BERR_Code ExitCode = BERR_SUCCESS;

	hPbLib->InFilePtr = fopen( FileName, "rb" );
	BDBG_ASSERT( hPbLib->InFilePtr );

	hPbLib->Repeat = false;
	BXPT_Playback_GetChannelSettings( hPbLib->hPb, &ChannelSettings );
  	ChannelSettings.SyncMode = SyncMode;
	switch( SyncMode )
	{
		case BXPT_PB_SYNC_MPEG_BLIND:
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eMpeg );
		ChannelSettings.PacketLength = 188;
		break;

		case BXPT_PB_SYNC_DIRECTV:
		case BXPT_PB_SYNC_DIRECTV_BLIND:
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eDirecTv );
		ChannelSettings.PacketLength = 130;
		break;
													
		default:
		break;
	}
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
 	BKNI_WaitForEvent( hPbLib->PbDoneEvent, BKNI_INFINITE );
	BXPT_Playback_StopChannel( hPbLib->hPb );

	fclose( hPbLib->InFilePtr );

	return( ExitCode );
}

BERR_Code APETEST_PbLib_DoPlayNonBlocking( 
	APETEST_PbLib_Handle hPbLib, 
	char *FileName,
	unsigned int SyncMode  
	)
{
	BXPT_Playback_ChannelSettings ChannelSettings;

	BERR_Code ExitCode = BERR_SUCCESS;

	hPbLib->InFilePtr = fopen( FileName, "rb" );
	BDBG_ASSERT( hPbLib->InFilePtr );

	hPbLib->Repeat = false;
	BXPT_Playback_GetChannelSettings( hPbLib->hPb, &ChannelSettings );
  	ChannelSettings.SyncMode = SyncMode;
	switch( SyncMode )
	{
		case BXPT_PB_SYNC_MPEG_BLIND:                         
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eMpeg );
		ChannelSettings.PacketLength = 188;
		break;

		case BXPT_PB_SYNC_DIRECTV:
		case BXPT_PB_SYNC_DIRECTV_BLIND:
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eDirecTv );
		ChannelSettings.PacketLength = 130;
		break;
													
		default:
		break;
	}
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
    return ExitCode;
}
      
bool APETEST_PbLib_IsPlayComplete(
    APETEST_PbLib_Handle hPbLib
    )
{
	BXPT_Playback_ChannelStatus Status;

 	bool IsComplete = false;

 	BXPT_Playback_GetChannelStatus( hPbLib->hPb, &Status );
 	if( Status.Run == false || BKNI_WaitForEvent( hPbLib->PbDoneEvent, 0 ) == BERR_SUCCESS )
    {
        IsComplete = true;

        /*
        ** The event was consumed by BKNI_WaitForEvent. Set it again in
        ** case they call PbLib_WaitForPlayComplete after calling this API.
        */
        BKNI_SetEvent( hPbLib->PbDoneEvent );
    }

 	return IsComplete;
}

BERR_Code APETEST_PbLib_WaitForPlayComplete(
    APETEST_PbLib_Handle hPbLib
    )
{
 	return BKNI_WaitForEvent( hPbLib->PbDoneEvent, BKNI_INFINITE );
}

BERR_Code APETEST_PbLib_StopPlay(
    APETEST_PbLib_Handle hPbLib
    )
{
	BERR_Code ExitCode = BXPT_Playback_StopChannel( hPbLib->hPb );
	fclose( hPbLib->InFilePtr );
	return( ExitCode );
}

BERR_Code APETEST_PbLib_DoRepeatingPlay( 
	APETEST_PbLib_Handle hPbLib, 
	char *FileName,
	unsigned int SyncMode  
	)
{
	BXPT_Playback_ChannelSettings ChannelSettings;

	BERR_Code ExitCode = BERR_SUCCESS;

	hPbLib->InFilePtr = fopen( FileName, "rb" );
	BDBG_ASSERT( hPbLib->InFilePtr );

	hPbLib->Repeat = true;
	BXPT_Playback_GetChannelSettings( hPbLib->hPb, &ChannelSettings );
  	ChannelSettings.SyncMode = SyncMode;
	switch( SyncMode )
	{
		case BXPT_PB_SYNC_MPEG_BLIND:
		ChannelSettings.PacketLength = 188;
		break;

		case BXPT_PB_SYNC_DIRECTV:
		case BXPT_PB_SYNC_DIRECTV_BLIND:
		ChannelSettings.PacketLength = 130;
		break;
													
		default:
		break;
	}
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
 	BKNI_WaitForEvent( hPbLib->PbDoneEvent, BKNI_INFINITE );
	BXPT_Playback_StopChannel( hPbLib->hPb );

	fclose( hPbLib->InFilePtr );

	return( ExitCode );
}

BERR_Code APETEST_PbLib_DoEsPlay( 
	APETEST_PbLib_Handle hPbLib, 
	char *FileName
	)
{
	BXPT_Playback_ChannelSettings ChannelSettings;

	BERR_Code ExitCode = BERR_SUCCESS;

	hPbLib->InFilePtr = fopen( FileName, "rb" );
	BDBG_ASSERT( hPbLib->InFilePtr );

	hPbLib->Repeat = false;
	BXPT_Playback_GetChannelSettings( hPbLib->hPb, &ChannelSettings );
  	ChannelSettings.SyncMode = BXPT_PB_SYNC_MPEG_BLIND;
	ChannelSettings.PacketLength = 0xB8;
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
 	BKNI_WaitForEvent( hPbLib->PbDoneEvent, BKNI_INFINITE );
	BXPT_Playback_StopChannel( hPbLib->hPb );

	fclose( hPbLib->InFilePtr );

	return( ExitCode );
}

BERR_Code APETEST_PbLib_DoPesPlay( 
	APETEST_PbLib_Handle hPbLib, 
	char *FileName
	)
{
	BXPT_Playback_ChannelSettings ChannelSettings;

	BERR_Code ExitCode = BERR_SUCCESS;

	hPbLib->InFilePtr = fopen( FileName, "rb" );
    if(!hPbLib->InFilePtr) 
    {
        perror( "fopen" );
    }
	BDBG_ASSERT( hPbLib->InFilePtr );

	hPbLib->Repeat = false;
	BXPT_Playback_GetChannelSettings( hPbLib->hPb, &ChannelSettings );
  	ChannelSettings.SyncMode = BXPT_PB_SYNC_PES;
	ChannelSettings.PacketLength = 0xB8;
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
 	BKNI_WaitForEvent( hPbLib->PbDoneEvent, BKNI_INFINITE );
	BXPT_Playback_StopChannel( hPbLib->hPb );

	fclose( hPbLib->InFilePtr );

	return( ExitCode );
}

BERR_Code APETEST_PbLib_DoEsPlayNonBlocking( 
	APETEST_PbLib_Handle hPbLib, 
	char *FileName
	)
{
	BXPT_Playback_ChannelSettings ChannelSettings;

	BERR_Code ExitCode = BERR_SUCCESS;

	hPbLib->InFilePtr = fopen( FileName, "rb" );
	BDBG_ASSERT( hPbLib->InFilePtr );

	hPbLib->Repeat = false;
	BXPT_Playback_GetChannelSettings( hPbLib->hPb, &ChannelSettings );
  	ChannelSettings.SyncMode = BXPT_PB_SYNC_MPEG_BLIND;
	ChannelSettings.PacketLength = 0xB8;
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
	return( ExitCode );
}

BERR_Code APETEST_PbLib_DoPlayWithTimestamps( 
	APETEST_PbLib_Handle hPbLib, 
	char *FileName,
	unsigned int SyncMode  
	)
{
	BXPT_Playback_ChannelSettings ChannelSettings;

	BERR_Code ExitCode = BERR_SUCCESS;

	hPbLib->InFilePtr = fopen( FileName, "rb" );
	BDBG_ASSERT( hPbLib->InFilePtr );

	hPbLib->Repeat = false;
	BXPT_Playback_GetChannelSettings( hPbLib->hPb, &ChannelSettings );
  	ChannelSettings.SyncMode = SyncMode;
	switch( SyncMode )
	{
		case BXPT_PB_SYNC_MPEG_BLIND:
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eMpeg );
		ChannelSettings.PacketLength = 188 + 4;     /* Account for 4-byte timestamp. */
		break;

		case BXPT_PB_SYNC_DIRECTV:
		case BXPT_PB_SYNC_DIRECTV_BLIND:
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eDirecTv );
		ChannelSettings.PacketLength = 130 + 4;     /* Account for 4-byte timestamp */
		break;
													
		default:
		break;
	}
    ChannelSettings.TimestampEn = true;
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
 	BKNI_WaitForEvent( hPbLib->PbDoneEvent, BKNI_INFINITE );
	BXPT_Playback_StopChannel( hPbLib->hPb );

	fclose( hPbLib->InFilePtr );

	return( ExitCode );
}


BXPT_Playback_Handle APETEST_PbLib_GetPbHandle( 
	APETEST_PbLib_Handle hPbLib 
	)
{
	return hPbLib->hPb;	
}

void FeedDataTask( 
	BTST_TaskHandle Task, 
	void *Parm 
	)
{
	size_t BytesRead;
	size_t TotalPlayed;

	APETEST_PbLib_Handle hPbLib = Parm;	

	BSTD_UNUSED( Task );
	
	while( 1 )
	{
    	TotalPlayed = 0;
	 	BKNI_WaitForEvent( hPbLib->PbStartEvent, BKNI_INFINITE );

        if ( hPbLib->Quit )
        {
            return;
        }
		/* Loop until the entire file has been played. */
		while( 1 )
		{
            void *pCached;
            char *ptr;
            (void)BMEM_ConvertAddressToCached(hPbLib->hXpt->hMemory, hPbLib->Buffer, &pCached);
            ptr = pCached;
            *ptr = 0;

			BytesRead = fread( pCached, sizeof( char ), BUFFER_LEN, hPbLib->InFilePtr );  	
/*            fprintf( stderr, "chz BytesRead=%d \n", BytesRead); */
/*            fprintf( stderr, "*"); */
		 	if( !BytesRead )
			{
				/* If not repeating, go back and wait for another start event */
				if( hPbLib->Repeat == false )
					break;

				/* Otherwise, rewind to the beginning and try again. */
				fseek( hPbLib->InFilePtr, 0L, SEEK_SET );
				continue;
			}

            BMEM_FlushCache(hPbLib->hXpt->hMemory, pCached, BytesRead);
			hPbLib->BufferSize = BytesRead;
		    TotalPlayed += BytesRead;
			FeedData( hPbLib );

	 		/* BKNI_WaitForEvent( hPbLib->DescriptorDoneEvent, BKNI_INFINITE ); */
		}
/*        fprintf( stderr, "\n" ); */

		/* Signal the calling task that playback has finished. */
		BKNI_SetEvent( hPbLib->PbDoneEvent );
	}
}

void FeedData( 
	APETEST_PbLib_Handle hPbLib
	)
{
	BXPT_Playback_ChannelStatus Status;

	BXPT_Playback_CreateDesc( hPbLib->hXpt, hPbLib->Descriptor, hPbLib->Buffer, hPbLib->BufferSize, 
		true, false, ( BXPT_PvrDescriptor * const ) NULL ); 
#if 0
	BXPT_Playback_AddDescriptors( hPbLib->hPb, hPbLib->Descriptor, hPbLib->Descriptor );  
#else
	BXPT_Playback_AddDescriptors( hPbLib->hPb, NULL, hPbLib->Descriptor ); 
#endif

	do
	{
		BKNI_Sleep( 10 );
		BXPT_Playback_GetChannelStatus( hPbLib->hPb, &Status );

/*        fprintf( stderr, "chz Status Busy=%d Finished=%d Run=%d OutOfSync=%d\n", 
            Status.Busy, Status.Finished, Status.Run, Status.OutOfSync);*/

	}
	while( Status.Finished == false && Status.Run == true );
}

void DescDoneIsr( 
	void *Parm1, 
	int Parm2 
	)
{
	APETEST_PbLib_Handle hPbLib	= Parm1;

	BSTD_UNUSED( Parm2 );
	BKNI_SetEvent( hPbLib->DescriptorDoneEvent );
}

BINT_Id GetDoneIntId(
	unsigned ChannelNum 
	)
{
	BINT_Id Id = 0;

	switch( ChannelNum )
	{
		case 0: 
		Id = BCHP_INT_ID_XPT_PB0_DONE_INT; 
		break;

		case 1: 
		Id = BCHP_INT_ID_XPT_PB1_DONE_INT; 
		break;

#if BCHP_CHIP == 7325 
#else
		case 2: 
		Id = BCHP_INT_ID_XPT_PB2_DONE_INT; 
		break;

		case 3: 
		Id = BCHP_INT_ID_XPT_PB3_DONE_INT; 
		break;
#endif

		default: 
		printf( "Unsupported PB channel number %u\n", ChannelNum ); 
		break;
	}

	return Id;
}


