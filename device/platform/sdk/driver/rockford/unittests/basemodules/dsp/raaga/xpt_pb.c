/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: xpt_pb.c $
 * $brcm_Revision: Hydra_Software_Devel/13 $
 * $brcm_Date: 10/10/12 7:09a $
 *
 * Playback testing library. 
 * This code is referred from pb_lib.c of XPT unit test.
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/basemodules/dsp/raaga/xpt_pb.c $
 * 
 * Hydra_Software_Devel/13   10/10/12 7:09a jgarrett
 * SW7584-27: Removing use of BXPT_P_MAX_PLAYBACKS - all 40nm chips use
 * BXPT_NUM_PLAYBACKS
 * 
 * Hydra_Software_Devel/12   8/31/12 3:01p gprasad
 * SW7425-3628: Converting memory accesses in raaga test to cached access
 * 
 * Hydra_Software_Devel/11   4/3/12 11:22a gprasad
 * FWRAA-382: Updating the error messages to logging messages
 * 
 * Hydra_Software_Devel/10   1/16/12 12:38p ashoky
 * SW7435-5: Merging raaga_test changes to mainline.
 * 
 * Hydra_Software_Devel/9   1/13/12 3:34p ashoky
 * SW7360-2: Addng changes in raaga_test.
 * 
 * Hydra_Software_Devel/8   11/29/11 12:17p ashoky
 * SW7429-18: Support for 7429.
 * 
 * Hydra_Software_Devel/7   8/10/11 11:08a jgarrett
 * SW7422-370: Fixing XPT configuration issues
 * 
 * Hydra_Software_Devel/6   8/10/11 5:00p gprasad
 * SW7422-370: Fixing the build issue seen after latest updates to XPT
 * 
 * Hydra_Software_Devel/5   5/19/11 4:11p srajapur
 * SWDTV-6868 : [35125] Adding raaga test app support for 35125
 * 
 * Hydra_Software_Devel/4   5/19/11 11:51a srajapur
 * SWDTV-6746 : [35230] Adding support for 35230 and 35233 to raaga_test
 * (fmm_config.c)
 * 
 * Hydra_Software_Devel/3   3/16/11 9:42a srajapur
 * SW7425-90 : [7425] Added raaga test app support for 7425
 * 
 * Hydra_Software_Devel/2   1/28/11 8:28a srajapur
 * SW7422-191:[7422] Adding different test modes.
 * 
 * Hydra_Software_Devel/1   1/14/11 3:27p gdata
 * SW7422-191:[7422] Adding the raaga test application
 * 
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"
#include <stdio.h>
#include "bxpt_rave.h"
#if (BCHP_CHIP != 35125)
#include "bxpt_directv.h"
#include "bxpt_directv_playback.h"
#endif
#include "btst_kni.h"
#include "btst.h"
#include "bkni_multi.h"
#include "bkni.h"
#include "bchp_int_id_xpt_pb0.h"
#include "bchp_int_id_xpt_pb1.h"
#include "bint.h"
#include "xpt_pb.h"

BDBG_MODULE( raaga_test );

#define BUFFER_LEN	( 1 * ( 512 * 1024 ) )

typedef struct BRAAGA_TEST_PbLib_Vars
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
}BRAAGA_TEST_PbLib_Vars;

static BRAAGA_TEST_PbLib_Vars PlaybackVars[ BXPT_NUM_PLAYBACKS ];

bool	gStopTask;

static void FeedDataTask( BTST_TaskHandle Task, void *Parm );
static void FeedData( BRAAGA_TEST_PbLib_Handle hPbLib );
static BINT_Id GetDoneIntId( unsigned ChannelNum );
static void DescDoneIsr( void *Parm1, int Parm2 );

BRAAGA_TEST_PbLib_Handle BRAAGA_TEST_PbLib_Init( 
	BXPT_Handle hXpt,
	unsigned PbChannelNum 
	)
{
	BXPT_Playback_ChannelSettings DefaultPbSettings;

	BRAAGA_TEST_PbLib_Handle hPbLib = &PlaybackVars[ PbChannelNum ];

	hPbLib->hXpt = hXpt;
	hPbLib->ChannelNum = PbChannelNum;
	hPbLib->Running = false;
	hPbLib->Repeat = false;
	hPbLib->Buffer = ( unsigned char * ) BMEM_AllocAligned( hXpt->hMemory, BUFFER_LEN, 2, 0 );
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
	BDBG_LOG(("BINT_EnableCallback:hPbLib->DescDoneIntC=%x ",hPbLib->DescDoneIntCb));

	BINT_EnableCallback( hPbLib->DescDoneIntCb );
		
	return( hPbLib );
}

void BRAAGA_TEST_PbLib_Shutdown( 
	BRAAGA_TEST_PbLib_Handle hPbLib 
	)
{
    BERR_Code err = BERR_SUCCESS;
    
    BMEM_Free( hPbLib->hXpt->hMemory, hPbLib->Buffer );
	BMEM_Free( hPbLib->hXpt->hMemory, hPbLib->Descriptor );
	BXPT_Playback_CloseChannel( hPbLib->hPb );
	BINT_DestroyCallback( hPbLib->DescDoneIntCb );

    BKNI_DestroyEvent(hPbLib->PbStartEvent);
    BKNI_DestroyEvent(hPbLib->PbDoneEvent);
    BKNI_DestroyEvent(hPbLib->DescriptorDoneEvent);
        
    err = BTST_DestroyTask(hPbLib->hPbTask);
    BDBG_ASSERT(err == BERR_SUCCESS);
}

BERR_Code BRAAGA_TEST_PbLib_DoPlay( 
	BRAAGA_TEST_PbLib_Handle hPbLib, 
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
#if (BCHP_CHIP != 35125)&&(BCHP_CHIP != 35233)
#if (BCHP_CHIP != 7425)&&(BCHP_CHIP != 7422)&&(BCHP_CHIP != 7358)&&(BCHP_CHIP == 7344)&&(BCHP_CHIP == 7346)&&(BCHP_CHIP != 7435)
		case BXPT_PB_SYNC_MPEG:
#endif            
		case BXPT_PB_SYNC_MPEG_BLIND:
#if (BCHP_CHIP != 35230)
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eMpeg );
		ChannelSettings.PacketLength = 188;
#endif
#endif
		break;

#if RAAGA_TEST_DIRECTV_SUPPORT
		case BXPT_PB_SYNC_DIRECTV:
		case BXPT_PB_SYNC_DIRECTV_BLIND:
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eDirecTv );
		ChannelSettings.PacketLength = 130;
#endif        
		break;
													
		default:
		break;
	}

	if(gStopTask == true)
	{
		BINT_EnableCallback( hPbLib->DescDoneIntCb );
		gStopTask = false;
	}
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
 	BKNI_WaitForEvent( hPbLib->PbDoneEvent, BKNI_INFINITE );

	return( ExitCode );
}

BERR_Code BRAAGA_TEST_PbLib_DoPlayNonBlocking( 
	BRAAGA_TEST_PbLib_Handle hPbLib, 
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
#if (BCHP_CHIP != 35125)&&(BCHP_CHIP != 35233)
#if ((BCHP_CHIP != 7422)&&(BCHP_CHIP != 7358)&&(BCHP_CHIP != 7344)&&(BCHP_CHIP != 7346)\
    &&(BCHP_CHIP != 7552)&&(BCHP_CHIP != 7425)&&(BCHP_CHIP != 7435)&&(BCHP_CHIP != 7231))
		case BXPT_PB_SYNC_MPEG:
#endif
		case BXPT_PB_SYNC_MPEG_BLIND:
#if (BCHP_CHIP != 35230)
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eMpeg );
		ChannelSettings.PacketLength = 188;
#endif        
#endif        
		break;

#if RAAGA_TEST_DIRECTV_SUPPORT
		case BXPT_PB_SYNC_DIRECTV:
		case BXPT_PB_SYNC_DIRECTV_BLIND:
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eDirecTv );
		ChannelSettings.PacketLength = 130;
#endif        
		break;
													
		default:
		break;
	}
	if(gStopTask == true)
	{
		BINT_EnableCallback( hPbLib->DescDoneIntCb );
		gStopTask = false;
	}
	BKNI_ResetEvent(hPbLib->DescriptorDoneEvent);
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );


	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
    return ExitCode;
}

bool BRAAGA_TEST_PbLib_IsPlayComplete(
    BRAAGA_TEST_PbLib_Handle hPbLib
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

BERR_Code BRAAGA_TEST_PbLib_WaitForPlayComplete(
    BRAAGA_TEST_PbLib_Handle hPbLib
    )
{
 	return BKNI_WaitForEvent( hPbLib->PbDoneEvent, BKNI_INFINITE );
}

BERR_Code BRAAGA_TEST_PbLib_StopPlay(
    BRAAGA_TEST_PbLib_Handle hPbLib
    )
{
	BERR_Code ExitCode;

	gStopTask = true;
	BKNI_SetEvent(hPbLib->DescriptorDoneEvent);
	BINT_DisableCallback(hPbLib->DescDoneIntCb);
	ExitCode = BXPT_Playback_StopChannel( hPbLib->hPb );

	fclose( hPbLib->InFilePtr );
	return( ExitCode );
}

BERR_Code BRAAGA_TEST_PbLib_DoRepeatingPlay( 
	BRAAGA_TEST_PbLib_Handle hPbLib, 
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
#if ((BCHP_CHIP != 7422)&&(BCHP_CHIP != 7358)&&(BCHP_CHIP != 7344)&&(BCHP_CHIP != 7346)\
    &&(BCHP_CHIP != 7552)&&(BCHP_CHIP != 7425)&&(BCHP_CHIP != 7435)&&(BCHP_CHIP != 7231))
		case BXPT_PB_SYNC_MPEG:
#endif
		case BXPT_PB_SYNC_MPEG_BLIND:
		ChannelSettings.PacketLength = 188;
		break;
#if RAAGA_TEST_DIRECTV_SUPPORT
		case BXPT_PB_SYNC_DIRECTV:
		case BXPT_PB_SYNC_DIRECTV_BLIND:
		ChannelSettings.PacketLength = 130;
		break;
#endif        
													
		default:
		break;
	}
	if(gStopTask == true)
	{
		BINT_EnableCallback( hPbLib->DescDoneIntCb );
		gStopTask = false;
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

BERR_Code BRAAGA_TEST_PbLib_DoEsPlay( 
	BRAAGA_TEST_PbLib_Handle hPbLib, 
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
	if(gStopTask == true)
	{
		BDBG_LOG(("BINT_EnableCallback:hPbLib->DescDoneIntC=%x ",hPbLib->DescDoneIntCb));
		BINT_EnableCallback( hPbLib->DescDoneIntCb );
		gStopTask = false;
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

BERR_Code BRAAGA_TEST_PbLib_DoPesPlay( 
	BRAAGA_TEST_PbLib_Handle hPbLib, 
	char *FileName
	)
{
	BXPT_Playback_ChannelSettings ChannelSettings;

	BERR_Code ExitCode = BERR_SUCCESS;

	hPbLib->InFilePtr = fopen( FileName, "rb" );
	BDBG_ASSERT( hPbLib->InFilePtr );

	hPbLib->Repeat = false;
	BXPT_Playback_GetChannelSettings( hPbLib->hPb, &ChannelSettings );
  	ChannelSettings.SyncMode = BXPT_PB_SYNC_PES;
	ChannelSettings.PacketLength = 0xB8;
	if(gStopTask == true)
	{
		BDBG_LOG(("BINT_EnableCallback:hPbLib->DescDoneIntC=%x ",hPbLib->DescDoneIntCb));
		BINT_EnableCallback( hPbLib->DescDoneIntCb );
		gStopTask = false;
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

BERR_Code BRAAGA_TEST_PbLib_DoEsPlayNonBlocking( 
	BRAAGA_TEST_PbLib_Handle hPbLib, 
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
	if(gStopTask == true)
	{
		BDBG_LOG(("BINT_EnableCallback:hPbLib->DescDoneIntC=%x ",hPbLib->DescDoneIntCb));
		BINT_EnableCallback( hPbLib->DescDoneIntCb );
		gStopTask = false;
	}
	BXPT_Playback_SetChannelSettings( hPbLib->hPb, &ChannelSettings );

	/* Start the feed task. Wait for it to finish before returning. */
    BKNI_ResetEvent( hPbLib->PbDoneEvent ); 
	BXPT_Playback_StartChannel( hPbLib->hPb );
	BKNI_SetEvent( hPbLib->PbStartEvent );
	return( ExitCode );
}

BERR_Code BRAAGA_TEST_PbLib_DoPlayWithTimestamps( 
	BRAAGA_TEST_PbLib_Handle hPbLib, 
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

#if ((BCHP_CHIP != 7422)&&(BCHP_CHIP != 7358)&&(BCHP_CHIP != 7344)&&(BCHP_CHIP != 7346)\
    &&(BCHP_CHIP != 7552)&&(BCHP_CHIP != 7425)&&(BCHP_CHIP != 7435)&&(BCHP_CHIP != 7231))
		case BXPT_PB_SYNC_MPEG:
#endif
		case BXPT_PB_SYNC_MPEG_BLIND:
#if (BCHP_CHIP != 35125) && (BCHP_CHIP != 35230)&&(BCHP_CHIP != 35233)
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eMpeg );
		ChannelSettings.PacketLength = 188 + 4;     /* Account for 4-byte timestamp. */
#endif        
		break;

#if RAAGA_TEST_DIRECTV_SUPPORT
		case BXPT_PB_SYNC_DIRECTV:
		case BXPT_PB_SYNC_DIRECTV_BLIND:
		BXPT_DirecTvPlayback_SetParserBandMode( hPbLib->hPb, BXPT_ParserMode_eDirecTv );
		ChannelSettings.PacketLength = 130 + 4;     /* Account for 4-byte timestamp */
#endif        
		break;
													
		default:
		break;
	}
    ChannelSettings.TimestampEn = true;
	if(gStopTask == true)
	{
		BDBG_LOG(("BINT_EnableCallback:hPbLib->DescDoneIntC=%x ",hPbLib->DescDoneIntCb));
		BINT_EnableCallback( hPbLib->DescDoneIntCb );
		gStopTask = false;
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

BXPT_Playback_Handle BRAAGA_TEST_PbLib_GetPbHandle( 
	BRAAGA_TEST_PbLib_Handle hPbLib 
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

	BRAAGA_TEST_PbLib_Handle hPbLib = Parm;	

	BSTD_UNUSED( Task );
	
	while( 1 )
	{
    	TotalPlayed = 0;
	 	BKNI_WaitForEvent( hPbLib->PbStartEvent, BKNI_INFINITE );
		/* Loop until the entire file has been played. */
		while( 1 )
		{
            void *pCached;
            /* Converting all memory accesses to cached */
            BMEM_ConvertAddressToCached(hPbLib->hXpt->hMemory, hPbLib->Buffer, &pCached);
            *(char *)pCached = 0;

			BytesRead = fread( pCached, sizeof( char ), BUFFER_LEN, hPbLib->InFilePtr );  		
	
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

	 		BKNI_WaitForEvent( hPbLib->DescriptorDoneEvent, BKNI_INFINITE );
			if(gStopTask == true)
				break;
		}
		

		/* Signal the calling task that playback has finished. */
		BKNI_SetEvent( hPbLib->PbDoneEvent );
	}
}

void FeedData( 
	BRAAGA_TEST_PbLib_Handle hPbLib
	)
{
	BXPT_Playback_ChannelStatus Status;


	BXPT_Playback_CreateDesc( hPbLib->hXpt, hPbLib->Descriptor, hPbLib->Buffer, hPbLib->BufferSize, 
		true, false, ( BXPT_PvrDescriptor * const ) NULL ); 
	BXPT_Playback_AddDescriptors( hPbLib->hPb, NULL, hPbLib->Descriptor );

	do
	{
		BKNI_Sleep( 10 );
		BXPT_Playback_GetChannelStatus( hPbLib->hPb, &Status );
	}
	while( Status.Finished == false && Status.Run == true );
}

void DescDoneIsr( 
	void *Parm1, 
	int Parm2 
	)
{
	BRAAGA_TEST_PbLib_Handle hPbLib	= Parm1;

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
#if 0
#if BCHP_CHIP == 7325 
#else
		case 2: 
		Id = BCHP_INT_ID_XPT_PB2_DONE_INT; 
		break;

		case 3: 
		Id = BCHP_INT_ID_XPT_PB3_DONE_INT; 
		break;
#endif
#endif
		default: 
		printf( "Unsupported PB channel number %u\n", ChannelNum ); 
		break;
	}

	return Id;
}


