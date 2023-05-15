#include "bstd.h"
#include "bxpt_priv.h"
#include "bxpt.h"
#include <stdio.h>

#include "btst_kni.h"
#include "btst.h"
#include "bkni_multi.h"
#include "bkni.h"
#include "bchp_int_id_xpt_pb0.h"
#include "bchp_int_id_xpt_pb1.h"
#include "bint.h"
#include "btmr.h"
#include "raaga_test_video.h"

BDBG_MODULE(raaga_test_data_feeder);

/* Following things should go in proper place */
#define BDSP_RAAGA_P_FIFO_BASE_OFFSET    0
#define BDSP_RAAGA_P_FIFO_END_OFFSET     4
#define BDSP_RAAGA_P_FIFO_READ_OFFSET    12 /*8*/
#define BDSP_RAAGA_P_FIFO_WRITE_OFFSET   8  /*12*/

#define BDSP_RAAGA_DSP_P_FIFO_PDQ   15
#define BDSP_RAAGA_DSP_P_FIFO_PRQ   16
#define BDSP_RAAGA_DSP_P_FIFO_DSQ   14

#define BUFFER_LEN		200000

BRAAGA_TEST_FeederStruct	Feeder0;
bool						gStopFeeder = false;

static void DataFeederTask( BTST_TaskHandle Task, void *Parm );
static uint32_t FeedAndDumpData( BRAAGA_TEST_Feeder_Handle hFeeder );
#if 0
static void PictureDoneIsr( void *Parm1, int Parm2 );
#endif 

void BRAAGA_TEST_GetBufferInfo(BRAAGA_TEST_Feeder_Handle hFeeder, BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, BRAAGA_TEST_CircBfrInfo *pCircBfrInfo);

/*==============================================================*/
/*						Init Function							*/
/*==============================================================*/
BRAAGA_TEST_Feeder_Handle BRAAGA_TEST_Feeder_Init(
						BCHP_Handle					hChp,
						BMEM_Handle					hMem,
						BINT_Handle                 hInt,
						BREG_Handle                 hReg
						)
{
	uint32_t	ui32RegOffset;
	BRAAGA_TEST_Feeder_Handle	hFeeder	= &Feeder0;

	/* Initialize handles */
	hFeeder->hReg	= hReg;
	hFeeder->hMem	= hMem;
	hFeeder->hChp	= hChp;
	hFeeder->hInt	= hInt;

	hFeeder->Buffer = ( unsigned char * ) BMEM_AllocAligned( hMem, BUFFER_LEN, 2, 0 );
	BDBG_ASSERT( hFeeder->Buffer );

	/* No Repeat */
	hFeeder->Repeat = false;

	/* Init PDQ and PRQ buffers */
	/* Nonsense but this is the way it is done as of now */
	ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;
    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + (ui32RegOffset * BDSP_RAAGA_DSP_P_FIFO_PDQ);

    hFeeder->sPDQ.ui32BaseAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;
    hFeeder->sPDQ.ui32EndAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;
    hFeeder->sPDQ.ui32WriteAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
    hFeeder->sPDQ.ui32ReadAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
    /* This is not used by F/W */
    hFeeder->sPDQ.ui32WrapAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + (ui32RegOffset * BDSP_RAAGA_DSP_P_FIFO_PRQ);

    hFeeder->sPRQ.ui32BaseAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;
    hFeeder->sPRQ.ui32EndAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;
    hFeeder->sPRQ.ui32WriteAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
    hFeeder->sPRQ.ui32ReadAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
    /* This is not used by F/W */
    hFeeder->sPRQ.ui32WrapAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

	/* Initialize CDB and ITB buffers here */
	ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;
    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + (ui32RegOffset * H264_ENCODE_OUTPUT_CDB_FIFO_NUM);

	hFeeder->sCdbBuffer.ui32BaseAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;
    hFeeder->sCdbBuffer.ui32EndAddr			= ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;
    hFeeder->sCdbBuffer.ui32WriteAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
    hFeeder->sCdbBuffer.ui32ReadAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
    /* This is not used by F/W */
    hFeeder->sCdbBuffer.ui32WrapAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

	BDBG_MSG(("O/P CDB addresses BaseAddr = 0x%x, EndAddr = 0x%x, WriteAddr = 0x%x, ReadAddr = 0x%x",
		hFeeder->sCdbBuffer.ui32BaseAddr, hFeeder->sCdbBuffer.ui32EndAddr, 
		hFeeder->sCdbBuffer.ui32WriteAddr, hFeeder->sCdbBuffer.ui32ReadAddr));
	
	/*===========================================================================================
					ITB buffer details 
	============================================================================================*/
	ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;
    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + (ui32RegOffset * H264_ENCODE_OUTPUT_ITB_FIFO_NUM);

	hFeeder->sItbBuffer.ui32BaseAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_BASE_OFFSET ;
    hFeeder->sItbBuffer.ui32EndAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;
    hFeeder->sItbBuffer.ui32WriteAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_WRITE_OFFSET;
    hFeeder->sItbBuffer.ui32ReadAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_READ_OFFSET;
    /* This is not used by F/W */
    hFeeder->sItbBuffer.ui32WrapAddr		= ui32RegOffset + BDSP_RAAGA_P_FIFO_END_OFFSET;

	/* Create event for sync */
	/*
	BKNI_CreateEvent( &( hFeeder->FeederStartEvent ) );
	*/
	BKNI_CreateEvent( &( hFeeder->PictureDoneEvent ) );

#if 0
	/* Create timers to use time based reading */
    /* ready to setup the BTMR */
    BTMR_GetDefaultSettings(&hFeeder->TmrDefSettings);

    /* Make any changes required from the default values */
    hFeeder->TmrDefSettings.timerMask = 0; /* defines the timers to reserve for external use (none) */

	/* Open Timer device */
    BTMR_Open(&hFeeder->hTimerDevice, hChp, hReg, hInt, &hFeeder->TmrDefSettings);

    /* Timers are now ready for use. */

    /* Create and start a timer. */
    {
    	BTMR_Settings Settings = { BTMR_Type_ePeriodic, (BTMR_CallbackFunc)PictureDoneIsr, ( void * ) hFeeder, 0, false };
        BTMR_CreateTimer(hFeeder->hTimerDevice, &hFeeder->hTimer, &Settings);
    }
#endif 

	/* Need to create callback on some interrupt. Should be PRQ/PDQ */
	/*
	BINT_CreateCallback( &hFeeder->PictureDoneIntCb, hFeeder->hXpt->hInt, PRQ_DONE, 
		PictureDone, ( void * ) hFeeder, 0 );
	BDBG_MSG(("BINT_EnableCallback:hFeeder->PictureDoneIntCb=%x ",hFeeder->PictureDoneIntCb));
	*/
	return (hFeeder);
}

#if 0
void PictureDoneIsr(	void *Parm1, 	int Parm2 )
{
	BRAAGA_TEST_Feeder_Handle hFeeder	= Parm1;

	BSTD_UNUSED( Parm2 );

	/* Set the event so that next picture can be put into pic buffer */
	BKNI_SetEvent( hFeeder->PictureDoneEvent );

}
#endif

void BRAAGA_TEST_GetBufferInfo(
							   BRAAGA_TEST_Feeder_Handle hFeeder,
							   BDSP_AF_P_sDRAM_CIRCULAR_BUFFER *pBuffer, 
							   BRAAGA_TEST_CircBfrInfo *pCircBfrInfo)
{
    uint32_t	ui32ReadAddr;
    uint32_t	ui32WriteAddr;
    uint32_t	ui32BaseAddr;
    uint32_t	ui32EndAddr;
	uint32_t	ui32Size, ui32DataNumBytes;
	void		*Addr;
	pCircBfrInfo->bWrapped	=	false;

	ui32ReadAddr = BREG_Read32( hFeeder->hReg, pBuffer->ui32ReadAddr );
	ui32BaseAddr = BREG_Read32( hFeeder->hReg, pBuffer->ui32BaseAddr );
	ui32WriteAddr = BREG_Read32( hFeeder->hReg, pBuffer->ui32WriteAddr );
	ui32EndAddr = BREG_Read32( hFeeder->hReg, pBuffer->ui32EndAddr );

	BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32ReadAddr, &Addr);
	ui32ReadAddr = (uint32_t)Addr;

	BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32BaseAddr, &Addr);
	ui32BaseAddr = (uint32_t)Addr;

	BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32WriteAddr, &Addr);
	ui32WriteAddr = (uint32_t)Addr;

	BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32EndAddr, &Addr);
	ui32EndAddr = (uint32_t)Addr;

	ui32Size  = ui32EndAddr - ui32BaseAddr;	
	if( ui32ReadAddr <= ui32WriteAddr )
	{
		ui32DataNumBytes = (ui32WriteAddr - ui32ReadAddr) ;
	}
	else
	{
		pCircBfrInfo->bWrapped = true;
		ui32DataNumBytes =	ui32Size + ui32WriteAddr - ui32ReadAddr;
	}

	pCircBfrInfo->uDepthTillEnd	= ui32EndAddr - ui32WriteAddr;

	pCircBfrInfo->uDataAvailableInBytes	= ui32DataNumBytes;
	
	/*** 4 this is added to maintain 
	one empty space b/wn Rd and Wr pointer , else
	Wr and Rd will get to same point , It will be difficult
	to find whether full or Empty incase of normal buffer **/
	pCircBfrInfo->uSpaceLeftInBytes	= ui32Size - ui32DataNumBytes - 4;
	
	return;

}

void BRAAGA_TEST_UpdateBuffer(
							   BRAAGA_TEST_Feeder_Handle		hFeeder,
							   BDSP_AF_P_sDRAM_CIRCULAR_BUFFER	*pBuffer, 
							   uint32_t							NumUpdateBytes)
{
    uint32_t	ui32ReadAddr;
    uint32_t	ui32WriteAddr;
    uint32_t	ui32BaseAddr;
    uint32_t	ui32EndAddr;
	uint32_t	ui32PhyAddr;
	void		*Addr;
	BERR_Code	bErr;
	
	ui32ReadAddr = BREG_Read32( hFeeder->hReg, pBuffer->ui32ReadAddr );
	ui32BaseAddr = BREG_Read32( hFeeder->hReg, pBuffer->ui32BaseAddr );
	ui32WriteAddr = BREG_Read32( hFeeder->hReg, pBuffer->ui32WriteAddr );
	ui32EndAddr = BREG_Read32( hFeeder->hReg, pBuffer->ui32EndAddr );

	BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32ReadAddr, &Addr);
	ui32ReadAddr = (uint32_t)Addr;

	BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32BaseAddr, &Addr);
	ui32BaseAddr = (uint32_t)Addr;

	BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32WriteAddr, &Addr);
	ui32WriteAddr = (uint32_t)Addr;

	BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32EndAddr, &Addr);
	ui32EndAddr = (uint32_t)Addr;

	if(ui32ReadAddr + NumUpdateBytes > ui32EndAddr)
	{
		ui32ReadAddr	= ui32BaseAddr + NumUpdateBytes - (ui32EndAddr - ui32ReadAddr);
	}
	else
	{
		ui32ReadAddr	+= NumUpdateBytes;
	}
	bErr = BMEM_ConvertAddressToOffset(hFeeder->hMem, (void *)ui32ReadAddr, &ui32PhyAddr);
    BREG_Write32(hFeeder->hReg,
			pBuffer->ui32ReadAddr, 
	        ui32PhyAddr);				

	return;
}

void DataFeederTask( 
	BTST_TaskHandle Task, 
	void *Parm 
	)
{
	size_t						BytesRead;
	size_t						TotalPlayed;
	uint32_t					ui32BytesOp, ui32NumFrameDummy = 0;
	void						*BuffYAddr;
	BERR_Code					bErr = BERR_SUCCESS;
	BVENC_VF_sPicParamBuff		*pPpb = NULL;
	BDSP_TaskHandle				hDspTask;

	BRAAGA_TEST_Feeder_Handle	hFeeder = Parm;
	hDspTask					= hFeeder->hDspTask;

	BSTD_UNUSED( Task );

    uint32_t BaseAddr,EndAddr,RdPtrRdb,WrPtrRdb,WrPtr,RdPtr,Occupancy,FreeSpace;
    BaseAddr = BCHP_RAAGA_DSP_PERI_SW_MAILBOX2 + BCHP_PHYSICAL_OFFSET;
    EndAddr = BCHP_RAAGA_DSP_PERI_SW_MAILBOX5 + BCHP_PHYSICAL_OFFSET;
    RdPtrRdb = BCHP_RAAGA_DSP_PERI_SW_MAILBOX6;
    WrPtrRdb = BCHP_RAAGA_DSP_PERI_SW_MAILBOX7;
    
    BREG_Write32( hFeeder->hReg, RdPtrRdb,BaseAddr);
    BREG_Write32( hFeeder->hReg, WrPtrRdb,BaseAddr);

	while( 1 )
	{
		/*
		BDBG_ERR(("Coming to feeder module \n"));
		*/
		/* 
		Check PRQ here. If space is there, deliver pictures in PDQ. Update PDQ and 
		PRQ pointers and wait.
		*/
		bErr = BDSP_VideoEncode_getPictureBuffer_isr( (void *)hDspTask, &pPpb );
		BDBG_MSG(("Found PPB - %p", pPpb));

		/* Check if you got the buffer */
		if(pPpb != NULL)
		{

			/* Not sure what to put */
			pPpb->ui32CaptureTimeStamp						= 1000;
			pPpb->sPictureMetaData.ui32PicHeight			= hFeeder->sPicParams.ui32IpPicHeight;
			pPpb->sPictureMetaData.ui32PicWidth				= hFeeder->sPicParams.ui32IpPicWidth;
			pPpb->sPictureMetaData.eAspectRatio				= BVENC_VF_AspectRatio_e4_3;
			pPpb->sPictureMetaData.eEncodeFrameRate			= BDSP_VF_P_EncodeFrameRate_e30;
			pPpb->sPictureMetaData.eFrameType				= BVENC_VF_Frame_eProgressive;
			pPpb->sPictureMetaData.ui32OrignalPtsHigh		= 0xffffffff;
			pPpb->sPictureMetaData.ui32OrignalPtsLow		= 0xffffffff;
			pPpb->sPictureMetaData.eChromaSampling			= BVENC_VF_ChromaSampling_e422;
			
			/* Buffer addresses are here */
			BMEM_ConvertOffsetToAddress(hFeeder->hMem, pPpb->ui32BuffAddrY, &BuffYAddr);

			BDBG_MSG(("Writing %d*%d*2 in 0x%x/ DRAM address 0x%x from input file ", hFeeder->sPicParams.ui32IpPicHeight, hFeeder->sPicParams.ui32IpPicWidth, BuffYAddr, pPpb->ui32BuffAddrY));			

			BytesRead = fread( BuffYAddr, sizeof( char ), hFeeder->sPicParams.ui32IpPicHeight*hFeeder->sPicParams.ui32IpPicWidth*2, hFeeder->InFilePtr ); 


	 		if( !BytesRead )
			{
				BDBG_MSG(("EOF reached\n"));
				/* If not repeating, go back and wait for another start event */
				/*
				if( hFeeder->Repeat == false )
					continue;
				*/
				/* Otherwise, rewind to the beginning and try again. */
				/*
				fseek( hFeeder->InFilePtr, 0L, SEEK_SET );
				continue;
				*/
			}
			else
			{
               
				bErr = BDSP_VideoEncode_putPicture_isr( (void *)hDspTask, (uint32_t) pPpb);
                
                           /* Trigger the GO bit */
                BREG_Write32( hFeeder->hReg, BCHP_RAAGA_DSP_ESR_SI_MASK_CLEAR,0xffffffff ); /* Unmask interrupt */
                BREG_Write32( hFeeder->hReg, BCHP_RAAGA_DSP_ESR_SI_INT_SET,0x1 ); /* Set go bit interrupt */

                printf("PI triggering interrupt \n");

			}

			hFeeder->BufferSize = BytesRead;
			TotalPlayed += BytesRead;

			ui32BytesOp = FeedAndDumpData( hFeeder );
		}
		if(ui32BytesOp == 0)
		{
			ui32NumFrameDummy++;
		}
		else
		{
			ui32NumFrameDummy = 0;
		}
		if(ui32NumFrameDummy > 20)
		{
			gStopFeeder = true;
			hFeeder->Running = false;
			fclose(hFeeder->OutFilePtr[0]);
			fclose(hFeeder->OutFilePtr[1]);
			printf(("========= No data in encoders output for 600ms. Quitting..."));
		}
		/* Come again in 30 msec */
 		BKNI_WaitForEvent( hFeeder->PictureDoneEvent, 30 );
		if(gStopFeeder == true)
		{
			break;
		}
	}
		
}
void BRAAGA_TEST_mem_put_le16(void *vmem, uint32_t val)
{    
    uint8_t *mem = (uint8_t *)vmem;
	mem[0] = (val >>  0) & 0xff;
	mem[1] = (val >>  8) & 0xff;
}

void BRAAGA_TEST_mem_put_le32(void *vmem, uint32_t val)
{
    uint8_t *mem = (uint8_t *)vmem;
	mem[0] = (val >>  0) & 0xff;
	mem[1] = (val >>  8) & 0xff;
	mem[2] = (val >> 16) & 0xff;
	mem[3] = (val >> 24) & 0xff;
}

uint32_t FeedAndDumpData( 
	BRAAGA_TEST_Feeder_Handle hFeeder
	)
{
	BRAAGA_TEST_CircBfrInfo		sBfrInfo;
	uint32_t					ui32ReadAddr, BytesDumped = 0;
	void						*Addr;
#ifdef 	OUPUT_IVF_FILE	
	uint8_t             header[12];
	static uint64_t            pts = 0;
#endif	
	/* Dump the CDB and ITB written by Raaga encoder and update the pointers */
	/* Do we need anything for the feeding side ??? */
	BRAAGA_TEST_GetBufferInfo(hFeeder, &hFeeder->sCdbBuffer, &sBfrInfo);
	BytesDumped = sBfrInfo.uDataAvailableInBytes;
	if(sBfrInfo.uDataAvailableInBytes > 0)
	{
#ifdef 	OUPUT_IVF_FILE
		BRAAGA_TEST_mem_put_le32(header, sBfrInfo.uDataAvailableInBytes);
		BRAAGA_TEST_mem_put_le32(header + 4, pts & 0xFFFFFFFF);
		BRAAGA_TEST_mem_put_le32(header + 8, pts >> 32);
		fwrite(header, 1, 12, hFeeder->OutFilePtr[0]);
		pts += 33;
#endif
		if(sBfrInfo.bWrapped == false)
		{
			ui32ReadAddr = BREG_Read32( hFeeder->hReg, hFeeder->sCdbBuffer.ui32ReadAddr );
			BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32ReadAddr, &Addr);
			fwrite(Addr, sizeof(char), sBfrInfo.uDataAvailableInBytes, hFeeder->OutFilePtr[0]);
		}
		else
		{
			ui32ReadAddr = BREG_Read32( hFeeder->hReg, hFeeder->sCdbBuffer.ui32ReadAddr );
			BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32ReadAddr, &Addr);
			fwrite(Addr, sizeof(char), sBfrInfo.uDepthTillEnd, hFeeder->OutFilePtr[0]);
			ui32ReadAddr = BREG_Read32( hFeeder->hReg, hFeeder->sCdbBuffer.ui32BaseAddr );
			BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32ReadAddr, &Addr);
			fwrite(Addr, sizeof(char), (sBfrInfo.uDataAvailableInBytes - sBfrInfo.uDepthTillEnd), hFeeder->OutFilePtr[0]);
		}
	}
	/* Update the pointers */
	BRAAGA_TEST_UpdateBuffer(hFeeder, &hFeeder->sCdbBuffer, sBfrInfo.uDataAvailableInBytes);

	/* Do the same for ITB buffer */
	BRAAGA_TEST_GetBufferInfo(hFeeder, &hFeeder->sItbBuffer, &sBfrInfo);
	if(sBfrInfo.uDataAvailableInBytes > 0)
	{
		if(sBfrInfo.bWrapped == false)
		{
			ui32ReadAddr = BREG_Read32( hFeeder->hReg, hFeeder->sItbBuffer.ui32ReadAddr );
			BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32ReadAddr, &Addr);
			fwrite(Addr, sizeof(char), sBfrInfo.uDataAvailableInBytes, hFeeder->OutFilePtr[1]);
		}
		else
		{
			ui32ReadAddr = BREG_Read32( hFeeder->hReg, hFeeder->sItbBuffer.ui32ReadAddr );
			BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32ReadAddr, &Addr);
			fwrite(Addr, sizeof(char), sBfrInfo.uDepthTillEnd, hFeeder->OutFilePtr[1]);
			ui32ReadAddr = BREG_Read32( hFeeder->hReg, hFeeder->sItbBuffer.ui32BaseAddr );
			BMEM_ConvertOffsetToAddress(hFeeder->hMem, ui32ReadAddr, &Addr);
			fwrite(Addr, sizeof(char), (sBfrInfo.uDataAvailableInBytes - sBfrInfo.uDepthTillEnd), hFeeder->OutFilePtr[1]);
		}
	}
	/* Update the pointers */
	BRAAGA_TEST_UpdateBuffer(hFeeder, &hFeeder->sItbBuffer, sBfrInfo.uDataAvailableInBytes);

	return BytesDumped;

}

void BRAAGA_TEST_Feeder_Shutdown(
	BRAAGA_TEST_Feeder_Handle hFeeder
	)
{
    BERR_Code err = BERR_SUCCESS;

    err = BTST_DestroyTask(hFeeder->hFeederTask);
	BDBG_ASSERT(err == BERR_SUCCESS);
	fclose(hFeeder->InFilePtr);
	fclose(hFeeder->OutFilePtr[0]);
	fclose(hFeeder->OutFilePtr[1]);
}

BERR_Code	BRAAGA_TEST_FeederStart( 
								BRAAGA_TEST_Feeder_Handle			hFeeder, 
								char								*FileName
								)
{
	BERR_Code	bErr = BERR_SUCCESS;
	uint8_t header[32];

	hFeeder->InFilePtr = fopen( FileName, "rb" );
	hFeeder->OutFilePtr[0] = fopen( "cdb.bin", "wb" );
	hFeeder->OutFilePtr[1] = fopen( "itb.bin", "wb" );
	BDBG_ASSERT( hFeeder->InFilePtr );
	BDBG_ASSERT( hFeeder->OutFilePtr[0] );
	BDBG_ASSERT( hFeeder->OutFilePtr[1] );

#define fourcc    0x30385056

	/* Create a feeder task */
	BTST_CreateTask( &( hFeeder->hFeederTask ), DataFeederTask, ( void * ) &( Feeder0) );
	hFeeder->Running = true;
#ifdef 	OUPUT_IVF_FILE
	header[0] = 'D';
	header[1] = 'K';
	header[2] = 'I';
	header[3] = 'F';
	BRAAGA_TEST_mem_put_le16(header + 4,  0);				  /* version */
	BRAAGA_TEST_mem_put_le16(header + 6,  32);				  /* headersize */
	BRAAGA_TEST_mem_put_le32(header + 8,  fourcc);			  /* headersize */
	BRAAGA_TEST_mem_put_le16(header + 12, hFeeder->sPicParams.ui32IpPicWidth);		  /* width */
	BRAAGA_TEST_mem_put_le16(header + 14, hFeeder->sPicParams.ui32IpPicHeight);		  /* height */
	BRAAGA_TEST_mem_put_le32(header + 16, 1000); /* rate */
	BRAAGA_TEST_mem_put_le32(header + 20, 1); /* scale */
	BRAAGA_TEST_mem_put_le32(header + 24, 0);		  /* length */
	BRAAGA_TEST_mem_put_le32(header + 28, 0);				  /* unused */
	
	fwrite(header,1,32,hFeeder->OutFilePtr[0]);
#endif

	/* interrupt in 30000 usec or 30 millisecond */
#if 0
	BTMR_StartTimer(hFeeder->hTimer, 30000); 
#endif 

	hFeeder->Repeat = false;
	return bErr;
}

