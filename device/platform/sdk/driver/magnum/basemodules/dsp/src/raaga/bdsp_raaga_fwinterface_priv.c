/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_fwinterface_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/23 $
 * $brcm_Date: 1/10/13 10:59a $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_fwinterface_priv.c $
 * 
 * Hydra_Software_Devel/23   1/10/13 10:59a skalapur
 * FWRAA-474:iLBC code merging to main latest
 * 
 * Hydra_Software_Devel/22   1/3/13 5:21p ananthan
 * SW7425-3628: Changes for cached access (Merge to mainline)
 * 
 * Hydra_Software_Devel/SW7425-3628/1   8/29/12 5:57p jgarrett
 * SW7425-3628: Converting BMEM accesses to cached memory
 * 
 * Hydra_Software_Devel/21   7/16/12 4:14p dlimaye
 * SW7425-2805: Add AMR-NB and AMR-WB codecs support Added new codecs: AMR
 * and AMRWB decoder/encoder
 * 
 * Hydra_Software_Devel/20   3/30/12 12:34a gprasad
 * SW7425-2786: BDSP changes for adding MLP Passthru support
 * 
 * Hydra_Software_Devel/19   3/23/12 11:55a gprasad
 * FWRAA-399: Adding the missing break to resolve coverity issue
 * 
 * Hydra_Software_Devel/18   3/22/12 6:48p gprasad
 * FWRAA-399: DTS LBR SUPPORT
 * 
 * Hydra_Software_Devel/17   3/19/12 2:34p gprasad
 * SW7425-2378: Adding support for Monkey audio codec
 * 
 * Hydra_Software_Devel/16   3/1/12 8:48p ashoky
 * SW7231-585: Changing the RDQ implementation from DRAM based to RDB
 * based.
 * 
 * Hydra_Software_Devel/15   2/3/12 5:18p ramanatm
 * SW7425-1752:[7425] Add support for FLAC media file format
 * 
 * Hydra_Software_Devel/14   2/3/12 11:53a ashoky
 * FWRAA-362: Raaga dsp sram optimization. Providing memory to swap
 * resident data.
 * 
 * Hydra_Software_Devel/13   1/27/12 1:19p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/12   1/13/12 9:50p gprasad
 * FWRAA-342: Adding support for SPEEX AEC
 * 
 * Hydra_Software_Devel/11   12/29/11 5:55p gprasad
 * FWRAA-370: G723.1 codec development on Raaga platform
 * 
 * Hydra_Software_Devel/10   11/8/11 1:37p ramanatm
 * FWRAA-348:[7425] Vorbis bringup
 * 
 * Hydra_Software_Devel/9   8/6/11 1:56p gprasad
 * FWRAA-327: Add support for G.711/G.726/G.729 codecs - Fixing the
 * warning reported by coverity
 * 
 * Hydra_Software_Devel/8   8/1/11 3:11p gprasad
 * FWRAA-327: Add support for G.711/G.726/G.729 codecs
 * 
 * Hydra_Software_Devel/7   7/12/11 4:44p gprasad
 * SW7422-370: Add changes for bdsp_types.h based on code review feedback
 * 
 * Hydra_Software_Devel/6   6/8/11 4:43p gautamk
 * SW7425-572: [7425] Adding DM related API support for VP6  in BDSP
 * 
 * Hydra_Software_Devel/5   5/16/11 10:08p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/4   5/11/11 5:31p srajapur
 * SW7422-374 : [7425] Incorporate changes to BDSP "details" structures
 * from code review feedback.
 * 
 * Hydra_Software_Devel/3   4/15/11 2:10p srajapur
 * SW7425-363 : [7425] Adding AMR algorithm support in BDSP
 * 
 * Hydra_Software_Devel/2   4/14/11 6:23p srajapur
 * SW7425-291: [7425] BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   4/6/11 2:27p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/13   3/15/11 12:28p jgarrett
 * SW7422-146: Removing externs from C files
 * 
 * Hydra_Software_Devel/12   3/8/11 9:12a srajapur
 * SW7422-294 : [7422]Adding watchdog recovery support in BASE DSP
 * 
 * Hydra_Software_Devel/11   2/26/11 12:39a srajapur
 * SW7422-293:[7422] Adding support for encode algorithms
 * 
 * Hydra_Software_Devel/10   2/10/11 7:00a srajapur
 * SW7422-229:[7422]Added support for follow audio codec algorithms
 * ========== WMA - tested WMAPRO - tested DRA - tested DTSHD - tested
 * DTSBROADCAST - tested LPCMDVD - tested PCMWAV PCM REALAUDIOLBR ADPCM
 * ==========
 * 
 * Hydra_Software_Devel/9   2/5/11 4:23a srajapur
 * SW7422-247 : [7422] Adding Generic passthrul audio processing support.
 * 
 * Hydra_Software_Devel/8   1/29/11 5:01a srajapur
 * SW7422-229 :[7422]Adding support for AAC, AACLOAS, AACSBR and
 * AACSBRADTS algorithm.
 * 
 * Hydra_Software_Devel/7   1/21/11 5:37a gautamk
 * SW7422-191: Fixing bug in Writemsg_isr and StopTask.
 * 
 * Hydra_Software_Devel/6   1/20/11 8:10a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/5   1/19/11 2:14a gautamk
 * SW7422-191: putting all the Details of Audio codec in a const array and
 * then using it across the code.
 * 
 * Hydra_Software_Devel/4   1/18/11 5:57a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/3   1/18/11 4:21a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/2   1/15/11 3:32a gautamk
 * SW7422-191:[7422] CLeaning up unwanted debug message
 * 
 * Hydra_Software_Devel/1   1/13/11 5:35a gautamk
 * SW7422-191:[7422]Adding DSP support for Audio decode in Basemodule  It
 * includes - DSP Booting - Open time Download :  - Start Time Audio
 * Decode algorithm download  - Message buffer interaction with DSP:  -
 * Synchronized Interrupt:  - Async interrupts:  - Config Commands for
 * Decode/TSM/IDS: - Status Buffer for Decode/TSM/IDS:  - Start AC3 Audio
 * Decode:  - Pause/Resume Command:  - Frame Advance command:  - Device
 * level interrupt
 * 
 * 
 ***************************************************************************/
#include "bdsp_raaga_fwinterface_priv.h"
#include "bdsp_raaga_priv.h"




BDBG_MODULE(bdsp_raaga_fwinterface);

BERR_Code BDSP_Raaga_P_CreateMsgQueue(
    BDSP_Raaga_P_MsgQueueParams    *psMsgQueueParams , /* [in]*/
    BMEM_Handle                 hHeap,              /* [in] */
    BREG_Handle                 hRegister,          /* [in] */
    uint32_t                    ui32DspOffset,      /* [in] */
    BDSP_Raaga_P_MsgQueueHandle    *hMsgQueue,        /* [out]*/
    bool bWdgRecovery /* [in] */ 
    )
{

    BERR_Code err=BERR_SUCCESS;

    uint32_t    ui32BaseAddrOffset=0;
    uint32_t    ui32EndAddrOffset=0;
    BDSP_Raaga_P_MsgQueueHandle  hHandle = NULL;
    uint32_t    ui32RegOffset = 0;
    void        *pCached;

    BDBG_ENTER(BDSP_Raaga_P_CreateMsgQueue);
    
    BDBG_ASSERT(psMsgQueueParams);
    BDBG_ASSERT(hHeap);
    BDBG_ASSERT(hRegister);
    BDBG_ASSERT(hMsgQueue);
    BDBG_ASSERT(psMsgQueueParams->ui32FifoId != BDSP_RAAGA_FIFO_INVALID);

    BDBG_MSG(("psMsgQueueParams->ui32FifoId > %d", 
                psMsgQueueParams->ui32FifoId));   
    
    if(false == bWdgRecovery)
    {
    
        /* Allocate memory for the Message Queue */
        hHandle =(BDSP_Raaga_P_MsgQueueHandle)
                  BKNI_Malloc(sizeof(struct BDSP_Raaga_P_MsgQueue));
        
        if(hHandle == NULL)
        {
            err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto exit;
        }
        BKNI_Memset (hHandle, 0, sizeof(struct BDSP_Raaga_P_MsgQueue));              
    }
    else
    {
        hHandle = *hMsgQueue;
    }


	/* Conversion of address from virtual to physical*/	    
    err = BMEM_ConvertAddressToOffset(
            hHeap,(void *)psMsgQueueParams->uiBaseAddr,&ui32BaseAddrOffset);
    if(BERR_SUCCESS != err )
    {
        BDBG_ERR(("Conversion Failed"));
        if(hHandle)
        {
            BKNI_Free(hHandle);
        }
        goto exit;
    }
    err = BMEM_Heap_ConvertAddressToCached(hHeap, (void *)psMsgQueueParams->uiBaseAddr, &pCached);
    if(BERR_SUCCESS != err )
    {
        BDBG_ERR(("Conversion Failed"));
        if(hHandle)
        {
            BKNI_Free(hHandle);
        }
        goto exit;
    }    
	BKNI_Memset(pCached, 0, psMsgQueueParams->uiMsgQueueSize);
    BMEM_Heap_FlushCache(hHeap, pCached, psMsgQueueParams->uiMsgQueueSize);
    
	/*Initializing attributes of message queue in DRAM (device memory)*/

    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
                    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;
	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * psMsgQueueParams->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_BASE_OFFSET + ui32DspOffset, 
        ui32BaseAddrOffset); /* base */

	ui32EndAddrOffset = ui32BaseAddrOffset + (psMsgQueueParams->uiMsgQueueSize);	
    
	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * psMsgQueueParams->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_END_OFFSET + ui32DspOffset, 
        ui32EndAddrOffset); /* end */

	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * psMsgQueueParams->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_READ_OFFSET + ui32DspOffset, 
        ui32BaseAddrOffset); /* read */

	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * psMsgQueueParams->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_WRITE_OFFSET + ui32DspOffset, 
        ui32BaseAddrOffset); /* write */

	/* Initializes attributes in the local copy(handle) in system memory*/
	
    hHandle->hHeap          = hHeap;
    hHandle->hRegister      = hRegister;
    hHandle->uiBaseAddr     = ui32BaseAddrOffset ;
    hHandle->uiEndAddr      = ui32EndAddrOffset;
    hHandle->uiReadPtr      = ui32BaseAddrOffset;
    hHandle->uiWritePtr     = ui32BaseAddrOffset;
    hHandle->ui32FifoId     = psMsgQueueParams->ui32FifoId;
    hHandle->ui32DspOffset  = ui32DspOffset;
       
    if(false == bWdgRecovery)
    {        
        *hMsgQueue = hHandle;
    }

exit:
	
   BDBG_LEAVE(BDSP_Raaga_P_CreateMsgQueue);
   return err;

}


BERR_Code BDSP_Raaga_P_DestroyMsgQueue(
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue, 
    BREG_Handle                 hRegister,          /* [in] */
    uint32_t                    ui32DspOffset      /* [in] */    
    )
{
    BERR_Code   err = BERR_SUCCESS;
    uint32_t    ui32RegOffset = 0;

    BDBG_ENTER(BDSP_Raaga_P_DestroyMsgQueue);

    BDBG_ASSERT(hMsgQueue);

    /*Reseting the FIFO buffers to invalid dram address*/
    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
                    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;

	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * hMsgQueue->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_BASE_OFFSET + ui32DspOffset, 
        BDSP_RAAGA_INVALID_DRAM_ADDRESS); /* base */
    
	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * hMsgQueue->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_END_OFFSET + ui32DspOffset, 
        BDSP_RAAGA_INVALID_DRAM_ADDRESS); /* end */
    
	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * hMsgQueue->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_READ_OFFSET + ui32DspOffset, 
        BDSP_RAAGA_INVALID_DRAM_ADDRESS); /* read */
    
	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * hMsgQueue->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_WRITE_OFFSET + ui32DspOffset, 
        BDSP_RAAGA_INVALID_DRAM_ADDRESS); /* write */

    BKNI_Free(hMsgQueue);
	BDBG_LEAVE	(BDSP_Raaga_P_DestroyMsgQueue);
	return err;

}


BERR_Code BDSP_Raaga_P_CreateRdbQueue(
    BDSP_Raaga_P_RdbQueueParams    *psMsgQueueParams , /* [in]*/
    BMEM_Handle                 hHeap,              /* [in] */
    BREG_Handle                 hRegister,          /* [in] */
    uint32_t                    ui32DspOffset,      /* [in] */
    BDSP_Raaga_P_MsgQueueHandle    *hMsgQueue,        /* [out]*/
    bool bWdgRecovery /* [in] */ 
    )
{

    BERR_Code err=BERR_SUCCESS;

    uint32_t    ui32BaseAddrOffset=0;
    uint32_t    ui32EndAddrOffset=0;
    BDSP_Raaga_P_MsgQueueHandle  hHandle = NULL;
    uint32_t    ui32RegOffset = 0;

	
    BDBG_ENTER(BDSP_Raaga_P_CreateRdbQueue);
    
    BDBG_ASSERT(psMsgQueueParams);
    BDBG_ASSERT(hHeap);
    BDBG_ASSERT(hRegister);
    BDBG_ASSERT(hMsgQueue);
    BDBG_ASSERT(psMsgQueueParams->ui32FifoId != BDSP_RAAGA_FIFO_INVALID);

    BDBG_MSG(("psMsgQueueParams->ui32FifoId > %d", 
                psMsgQueueParams->ui32FifoId));   
    
    if(false == bWdgRecovery)
    {
    
        /* Allocate memory for the Message Queue */
        hHandle =(BDSP_Raaga_P_MsgQueueHandle)
                  BKNI_Malloc(sizeof(struct BDSP_Raaga_P_MsgQueue));
        
        if(hHandle == NULL)
        {
            err = BERR_TRACE(BERR_OUT_OF_SYSTEM_MEMORY);
            goto exit;
        }
        BKNI_Memset (hHandle, 0, sizeof(struct BDSP_Raaga_P_MsgQueue));              
    }
    else
    {
        hHandle = *hMsgQueue;
    }


	/* Base address is the actual Fifo Index from where RDBs are free. */
	ui32BaseAddrOffset = BCHP_PHYSICAL_OFFSET + BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + \
		psMsgQueueParams->startIndexOfFreeFifo*(BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1-BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0)+\
		ui32DspOffset;
	
	/*Initializing attributes of message queue in DRAM (device memory)*/

    ui32RegOffset = BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_1 - \
                    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0;
	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
        (ui32RegOffset * psMsgQueueParams->ui32FifoId) + 
        BDSP_RAAGA_P_FIFO_BASE_OFFSET + ui32DspOffset, 
        ui32BaseAddrOffset); /* base */

	ui32EndAddrOffset = ui32BaseAddrOffset + (psMsgQueueParams->uiMsgQueueSize);	
    
	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * psMsgQueueParams->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_END_OFFSET + ui32DspOffset, 
        ui32EndAddrOffset); /* end */

	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * psMsgQueueParams->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_READ_OFFSET + ui32DspOffset, 
        ui32BaseAddrOffset); /* read */

	BDSP_Write32(
        hRegister,
        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
            (ui32RegOffset * psMsgQueueParams->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_WRITE_OFFSET + ui32DspOffset, 
        ui32BaseAddrOffset); /* write */

	/* Initializes attributes in the local copy(handle) in system memory*/
	
    hHandle->hHeap          = hHeap;
    hHandle->hRegister      = hRegister;
    hHandle->uiBaseAddr     = ui32BaseAddrOffset ;
    hHandle->uiEndAddr      = ui32EndAddrOffset;
    hHandle->uiReadPtr      = ui32BaseAddrOffset;
    hHandle->uiWritePtr     = ui32BaseAddrOffset;
    hHandle->ui32FifoId     = psMsgQueueParams->ui32FifoId;
    hHandle->ui32DspOffset  = ui32DspOffset;
       
    if(false == bWdgRecovery)
    {        
        *hMsgQueue = hHandle;
    }

exit:
	
   BDBG_LEAVE(BDSP_Raaga_P_CreateRdbQueue);
   return err;

}

BERR_Code BDSP_Raaga_P_WriteMsg_isr(
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue   /*[in]*/,
    void                        *pMsgBuf,   /*[in]*/
    unsigned int                uiBufSize   /*[in]*/
    )
{	
	BERR_Code err = BERR_SUCCESS;
	unsigned int i,uiFreeSpace=0;
	uint32_t ui32chunk1=0,ui32chunk2=0;
	uint32_t ui32dramReadPtr=0;
	uint32_t ui32dramWritePtr=0;
	uint32_t ui32maskReadPtr=0;
	uint32_t ui32maskWritePtr=0; 
	void *pvMsgQueueWriteAddr=NULL;
	
		
	BDBG_ENTER(BDSP_Raaga_P_WriteMsg_isr);

	BDBG_ASSERT(hMsgQueue);
	BDBG_ASSERT(pMsgBuf);

	ui32dramReadPtr = BDSP_Read32_isr(
	                    hMsgQueue->hRegister,
	                    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
	                        (4 * 4 * hMsgQueue->ui32FifoId) + 
                            BDSP_RAAGA_P_FIFO_READ_OFFSET + 
                            hMsgQueue->ui32DspOffset);
	
	ui32dramWritePtr = BDSP_Read32_isr(
                        hMsgQueue->hRegister,
                        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
                        (4 * 4 * hMsgQueue->ui32FifoId) + 
                        BDSP_RAAGA_P_FIFO_WRITE_OFFSET + 
                        hMsgQueue->ui32DspOffset);

    ui32maskReadPtr  = ui32dramReadPtr;
    ui32maskWritePtr = ui32dramWritePtr;

    /*Sanity check*/
	/* Checking boundness of read pointer - 
	if((readptr>endaddr) OR (readptr<baseaddr)) read ptr not within bound */
	
	if ( (ui32maskReadPtr > hMsgQueue->uiEndAddr)||
         (ui32maskReadPtr<hMsgQueue->uiBaseAddr))
	{
		BDBG_ERR(("Read pointer not within bounds in Message Queue, Fifo ID =%d , DSPOffset = %d, ui32dramReadPtr = %d, hMsgQueue->uiEndAddr = %d, hMsgQueue->uiBaseAddr =%d"
            ,hMsgQueue->ui32FifoId,hMsgQueue->ui32DspOffset,ui32dramReadPtr,hMsgQueue->uiEndAddr,hMsgQueue->uiBaseAddr));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
	}
	
	/*Checking boundness of write pointer - 
	if((writeptr>endaddr) OR (writeptr<baseaddr))  write ptr not within bound */
	
	if ( (ui32maskWritePtr>hMsgQueue->uiEndAddr)||
         (ui32maskWritePtr<hMsgQueue->uiBaseAddr))
	{
		BDBG_ERR(("Write pointer not within bounds in Message Queue"));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
	}

	/*update read ptr */
	hMsgQueue->uiReadPtr= ui32maskReadPtr;

	/* checking write ptrs */
	if((hMsgQueue->uiWritePtr)!=ui32maskWritePtr)
	{
		BDBG_ERR(("Write pointer corrupted in the Message Queue, hMsgQueue->uiWritePtr=%x , ui32dramWritePtr = %x"
            ,hMsgQueue->uiWritePtr,ui32dramWritePtr));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);	
	}
    /* End of Sanity Check */
    
    /* --------------------------------------------------------------------
    
    Different cases: 

    If maskwriteptr > maskreadptr
        ReadPtrMSB  WritePtrMSB     Freespace
        0   	    0               ((endaddr-writeptr)+(readptr-baseaddr));           		
        0   		1   	        Impossible Condition
        1   		0   	        Impossible Condition
        1   		1   	        ((endaddr-writeptr)+(readptr-baseaddr));	


    If maskreadptr > maskwriteptr
        ReadptrMSB 	WritePtrMSB	    Freespace 
        0   		0   		    Impossible Condition	
        0   		1   		    read-write
        1   		0   		    read-write
        1   		1   		    Impossible Condition

    If maskreadptr == maskwriteptr
        If the toggle bits are the same,then the buffer is empty
        If the toggle bits are different,the buffer is full
        
    --------------------------------------------------------------------- */

	
	/* Calculate the free space in the message queue */

	/* Case1: if(maskwriteptr>maskreadptr) */
	if(ui32maskWritePtr > ui32maskReadPtr)
	{	
		uiFreeSpace=((hMsgQueue->uiEndAddr-ui32maskWritePtr)+
                     (ui32maskReadPtr-hMsgQueue->uiBaseAddr))-uiBufSize;
	}
	
	/* Case2: if(maskreadptr>maskwriteptr) */
	if(ui32maskReadPtr>ui32maskWritePtr)
	{
		uiFreeSpace=(ui32maskReadPtr-ui32maskWritePtr)-uiBufSize;
	}
	
	/* Case3: if(maskreadptr==maskwriteptr) */
	if(ui32maskReadPtr==ui32maskWritePtr)
	{
		/* The buffer is empty */
		uiFreeSpace=(hMsgQueue->uiEndAddr-hMsgQueue->uiBaseAddr)-uiBufSize;
	}

	/* Generate BUFFER_FULL error when there is no space for the message to be 
	    written into the message queue*/

	if(uiFreeSpace <= 0)
	{
		BDBG_ERR(("No Free space in the buffer.No new messages can be written"));
		return BERR_TRACE(BDSP_ERR_BUFFER_FULL);
	}
	
	/* Writing Messages into the Message Queue */

	/*Assume: The Message buffer always has only one message */
	
	BDBG_MSG(("Buffer size should be a multiple of 4"));
	BDBG_ASSERT(!(uiBufSize%4));
    BDBG_MSG(("uiBufSize > %d", uiBufSize));
	
	/* This conversion is done to pass a virtual address as the second argument 
	    of BDSP_Raaga_P_MemWrite32 */
    err = BMEM_ConvertOffsetToAddress_isr(
            hMsgQueue->hHeap, ui32maskWritePtr, (void **)&pvMsgQueueWriteAddr);
    
    if (BERR_SUCCESS!= err)
    {   
    	return BERR_TRACE(err);
    }
	
	/* Writing data in two chunks taking wrap-around into consideration */
    if ( (ui32maskWritePtr > ui32maskReadPtr)||
         (ui32maskWritePtr == ui32maskReadPtr))
    {
        if(uiBufSize > (hMsgQueue->uiEndAddr-ui32maskWritePtr))
        {
            ui32chunk1 = hMsgQueue->uiEndAddr-ui32maskWritePtr;
			ui32chunk2 = uiBufSize - ui32chunk1;
        }
        else
        {
            ui32chunk1 = uiBufSize;
			ui32chunk2 = 0;
        }
    }
    else
    {
        ui32chunk1 = uiBufSize;
		ui32chunk2 = 0;
    }
    
	/* Writing into chunk1 */
	for (i=0; i<(ui32chunk1/4); i++)
	{
	    BDBG_MSG(("*((uint32_t *)pMsgBuf+i) > %x", *((uint32_t *)pMsgBuf+i)));

        BDSP_Raaga_P_MemWrite32_isr( hMsgQueue->hHeap,
            (uint32_t)pvMsgQueueWriteAddr+(i*4), *((uint32_t *)pMsgBuf+i));
        
		ui32dramWritePtr=ui32dramWritePtr+4;
	}

	/* Toggling the write pointer to wrap around */
	if((ui32maskWritePtr + ui32chunk1) == hMsgQueue->uiEndAddr ) 
	{   
        ui32dramWritePtr = hMsgQueue->uiBaseAddr;
        ui32maskWritePtr = ui32dramWritePtr;
	}
	
	/* Writing into chunk 2 */
	if ( ui32chunk2 > 0 )
	{
	    err = BMEM_ConvertOffsetToAddress_isr(
                hMsgQueue->hHeap,ui32maskWritePtr,
                (void **)&pvMsgQueueWriteAddr
                );
        
        if (BERR_SUCCESS != err)
        {      
			return BERR_TRACE(err);
        }
        
		for (i=0; i<(ui32chunk2/4); i++)
		{
            BDBG_MSG(("-->*((uint32_t *)pMsgBuf+i) > %x", 
                       *((uint32_t *)pMsgBuf+(ui32chunk1/4)+i)));      
            
			BDSP_Raaga_P_MemWrite32_isr(hMsgQueue->hHeap,
                (uint32_t)pvMsgQueueWriteAddr+(i*4), 
                *((uint32_t *)pMsgBuf+(ui32chunk1/4)+i)
                );
            
			ui32dramWritePtr=ui32dramWritePtr+4;
		}
	}

	/* Updating write ptr in the Queue Attribute Structure */
	BDSP_Write32_isr(
	    hMsgQueue->hRegister,
	    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
	        (4 * 4 * hMsgQueue->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_WRITE_OFFSET + hMsgQueue->ui32DspOffset, 
        ui32dramWritePtr); /* write */

	BDBG_MSG(("ui32dramReadPtr > %x",  ui32dramReadPtr));
	BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));
	
	/* Updating write ptr in the handle */
	hMsgQueue->uiWritePtr = ui32dramWritePtr;
    
	BDBG_LEAVE(BDSP_Raaga_P_WriteMsg_isr);
    
	return err;
		
}


/*----------------------------------------------------------------------------*/
BERR_Code BDSP_Raaga_P_SendCommand( 
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue   /*[in]*/, 
    const BDSP_Raaga_P_Command         *psCommand  /*[in]*/ ,
    void *pTaskHandle      /*[in] Task handle */
    )
{
    BERR_Code   rc = BERR_SUCCESS;
    BDBG_ENTER(BDSP_Raaga_P_SendCommand);

	BKNI_EnterCriticalSection();  
	rc = BDSP_Raaga_P_SendCommand_isr(hMsgQueue, psCommand,pTaskHandle);
	BKNI_LeaveCriticalSection();

    BDBG_LEAVE(BDSP_Raaga_P_SendCommand);
    return rc;
}


/*----------------------------------------------------------------------------*/
BERR_Code BDSP_Raaga_P_SendCommand_isr( 
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue   /*[in]*/, 
    const BDSP_Raaga_P_Command         *psCommand  /*[in]*/,
    void    *pTaskHandle       /*[in] Task handle */)
{
    BERR_Code err = BERR_SUCCESS;
    unsigned int uiCommandSize;    
    BDSP_RaagaTask *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;    
    
    BDBG_ENTER( BDSP_Raaga_P_SendCommand_isr );

    BDBG_ASSERT( hMsgQueue );
    BDBG_ASSERT( psCommand );

    
    if ( (psCommand->sCommandHeader.ui32CommandID != BDSP_RAAGA_PING_COMMAND_ID)
		&& (psCommand->sCommandHeader.ui32CommandID != BDSP_RAAGA_GET_SYSTEM_SWAP_MEMORY_COMMAND_ID))
    {
        /* When isStopped is true at that instance STOP/START commands can come 
            and should be processed */
        
        if( (pRaagaTask->isStopped == true) &&
            (psCommand->sCommandHeader.ui32CommandID != \
                BDSP_RAAGA_START_TASK_COMMAND_ID)
            &&(psCommand->sCommandHeader.ui32CommandID != \
                BDSP_RAAGA_GET_VOM_TABLE_COMMAND_ID)
          )
        {
            BDBG_MSG(("Task is in stop state, Can't accept Command %#x",
                        psCommand->sCommandHeader.ui32CommandID ));
            return BERR_SUCCESS;        
        }
    }
    else
    {
        BSTD_UNUSED(pRaagaTask);
    }
    
    uiCommandSize = sizeof(BDSP_Raaga_P_Command);
    
    BDBG_MSG(("psCommand->sCommandHeader.ui32CommandSizeInBytes > %d", 
                psCommand->sCommandHeader.ui32CommandSizeInBytes));
    
	err = BDSP_Raaga_P_WriteMsg_isr( hMsgQueue,(void *) psCommand,uiCommandSize);
	if(BERR_SUCCESS != err)
	{
        return BERR_TRACE(err);
	}

    if(psCommand->sCommandHeader.ui32CommandID == \
            BDSP_RAAGA_STOP_TASK_COMMAND_ID)
    {
        pRaagaTask->isStopped = true;
    }
    
	BDBG_LEAVE( BDSP_Raaga_P_SendCommand_isr );
    
	return err;
	
}

/*----------------------------------------------------------------------------*/
BERR_Code BDSP_Raaga_P_GetMsg(
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue,  /*[in]*/
    void                        *pMsgBuf,   /*[in]*/
    BDSP_Raaga_P_MsgType              eMgsType
    )
{
    BERR_Code   rc = BERR_SUCCESS;
    BDBG_ENTER(BDSP_Raaga_P_GetMsg);

	BKNI_EnterCriticalSection();
	rc = BDSP_Raaga_P_GetMsg_isr(hMsgQueue, pMsgBuf,eMgsType);
	BKNI_LeaveCriticalSection();

    BDBG_LEAVE(BDSP_Raaga_P_GetMsg);
    return rc;
}


/*----------------------------------------------------------------------------*/
BERR_Code BDSP_Raaga_P_GetMsg_isr(
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue,  /*[in]*/
    void                        *pMsgBuf,/*[in]*/
    BDSP_Raaga_P_MsgType              eMgsType
    )
{
	BERR_Code err=BERR_SUCCESS;
	
	uint32_t ui32dramReadPtr=0;
	uint32_t ui32dramWritePtr=0;
	uint32_t ui32maskReadPtr=0;
	uint32_t ui32maskWritePtr=0;
    uint32_t ui32chunk1=0,ui32chunk2=0,i;
	int32_t  i32BytesToBeRead=0;
    uint32_t ui32ResponseSize = 0;
	void *   pvMsgQueueReadAddr=NULL;
	
	BDBG_ENTER(BDSP_Raaga_P_GetMsg_isr);
    
	BDBG_ASSERT(hMsgQueue);
	BDBG_ASSERT(pMsgBuf);

	ui32dramReadPtr = BDSP_Read32_isr(
	                    hMsgQueue->hRegister,
	                    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
	                        (4 * 4 * hMsgQueue->ui32FifoId) + 
                            BDSP_RAAGA_P_FIFO_READ_OFFSET + 
                            hMsgQueue->ui32DspOffset);

	ui32dramWritePtr = BDSP_Read32_isr(
                        hMsgQueue->hRegister,
                        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
                            (4 * 4 * hMsgQueue->ui32FifoId) + 
                            BDSP_RAAGA_P_FIFO_WRITE_OFFSET + 
                            hMsgQueue->ui32DspOffset);

	BDBG_MSG(("ui32dramReadPtr > %x",  ui32dramReadPtr));
	BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));
    
	ui32maskReadPtr  = ui32dramReadPtr;
	ui32maskWritePtr = ui32dramWritePtr;
	   
	 /*Sanity check*/
     
	/* checking write ptr boundness- if((writeptr>endaddr)|(writeptr<baseaddr)) 
	    write ptr is not within bound*/	
	if( (ui32maskWritePtr>hMsgQueue->uiEndAddr)||
        (ui32maskWritePtr<hMsgQueue->uiBaseAddr))
	{
		BDBG_ERR(("Write pointer not within bounds in Message Queue"));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
	}

	
	/* checking read ptr boundness- if((readptr>endaddr)|(readptr<baseaddr)) 
	    read ptr is not within bound*/
	if( (ui32maskReadPtr>hMsgQueue->uiEndAddr)||
        (ui32maskReadPtr<hMsgQueue->uiBaseAddr))
	{
		BDBG_ERR(("Read pointer not within bounds in Message Queue"));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
	}
	
	/*Updating write ptr */
	hMsgQueue->uiWritePtr= ui32maskWritePtr;

	/* checking read ptrs to see if they are the same */
	if((hMsgQueue->uiReadPtr)!=ui32maskReadPtr)
	{
		BDBG_ERR(("Read pointer corrupted in the Message Queue"));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE );		
	}

	/* End of Sanity Check */

    /* --------------------------------------------------------------------
    
    Different cases: 

    If maskwriteptr > maskreadptr
        ReadPtrMSB  WritePtrMSB     Freespace
        0   	    0               write-read           		
        0   		1   	        Impossible Condition
        1   		0   	        Impossible Condition
        1   		1   	        write-read	


    If maskreadptr > maskwriteptr
        ReadptrMSB 	WritePtrMSB	    Freespace 
        0   		0   		    Impossible Condition	
        0   		1   		    (end-read)+(write-base)
        1   		0   		    (end-read)+(write-base)
        1   		1   		    Impossible Condition

    If maskreadptr == maskwriteptr
        If the toggle bits are the same,then there is no message to be read
        If the toggle bits are different, all the messages have to be read
        
    --------------------------------------------------------------------- */
    
	/* Condition for reading messages from the message queue into the message 
	    buffer */

	/* If no msg is to be read, generate a BDSP_ERR_BUFFER_EMPTY */
    if(BDSP_Raaga_P_MsgType_eSyn == eMgsType)
    {
        ui32ResponseSize = BDSP_RAAGA_RESPONSE_SIZE_IN_BYTES;
    }
    else if(BDSP_Raaga_P_MsgType_eAsyn == eMgsType)
    {
        ui32ResponseSize = BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES;
    }

	/* Checking if a msg is present */
	
	/* Case 1: if(readptr > writeptr)*/
	if (ui32maskReadPtr > ui32maskWritePtr)
	{	
		i32BytesToBeRead=(hMsgQueue->uiEndAddr-ui32maskReadPtr)+
                (ui32maskWritePtr-hMsgQueue->uiBaseAddr);
	}

	 /* Case 2:if(writeptr>readptr) */
	if (ui32maskWritePtr>ui32maskReadPtr)
	{
		i32BytesToBeRead=ui32maskWritePtr-ui32maskReadPtr;
	}

    /* Case 3: if readptr == writeptr */
    if (ui32maskWritePtr == ui32maskReadPtr)
    {	
        /*All messages have to be read*/
        i32BytesToBeRead = hMsgQueue->uiEndAddr-hMsgQueue->uiBaseAddr;
    }
	 if (i32BytesToBeRead < 0)
	 {
		BDBG_ERR(("The Message Queue is empty.No message is present."));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BUFFER_EMPTY);
	 }
	 
	
	/* Reading Message from the message queue into the message buffer */

    err = BMEM_ConvertOffsetToAddress_isr(
            hMsgQueue->hHeap,ui32maskReadPtr,(void **)&pvMsgQueueReadAddr);
	if( BERR_SUCCESS != err)
    {   
		return BERR_TRACE(err);
    }
	
	/* Reading data in two chunks taking wrap-around into consideration  */
	if ( (ui32maskReadPtr > ui32maskWritePtr)||
         (ui32maskReadPtr == ui32maskWritePtr))
	{
		if(ui32ResponseSize> (hMsgQueue->uiEndAddr-ui32maskReadPtr))
		{
			ui32chunk1=hMsgQueue->uiEndAddr-ui32maskReadPtr;
			ui32chunk2=i32BytesToBeRead-ui32chunk1;
		}
		else
		{
			ui32chunk1=ui32ResponseSize;
			ui32chunk2=0;	
		}
	}
	else
	{
		ui32chunk1=ui32ResponseSize;
		ui32chunk2=0;
	}
		

	/* Reading from chunk1 */
	for(i=0;i<(ui32chunk1/4);i++)
	{
		*((uint32_t *)pMsgBuf+i) = BDSP_Raaga_P_MemRead32_isr(hMsgQueue->hHeap,
                                    (uint32_t )pvMsgQueueReadAddr+(i*4));
		ui32dramReadPtr=ui32dramReadPtr+4;
			
	}

    /* Toggling the read pointer */
	if(ui32maskReadPtr + ui32chunk1 == hMsgQueue->uiEndAddr)
	{
		ui32dramReadPtr=hMsgQueue->uiBaseAddr;
		ui32maskReadPtr=ui32dramReadPtr;
	}
	
		
	/* Reading from chunk2 */
	if(ui32chunk2>0)
	{
	    err = BMEM_ConvertOffsetToAddress_isr(
                hMsgQueue->hHeap,ui32maskReadPtr,(void **)&pvMsgQueueReadAddr);
		if (BERR_SUCCESS != err)
        {      
			return BERR_TRACE(err);
        }
		
		for(i=0;i<(ui32chunk2/4);i++)
		{
			*((uint32_t *)pMsgBuf+i) = BDSP_Raaga_P_MemRead32_isr(hMsgQueue->hHeap,
                (uint32_t) pvMsgQueueReadAddr+(i*4));
			ui32dramReadPtr=ui32dramReadPtr+4;
		}
	}
	
	/* Updating read ptr in the Queue Attribute Structure */
	BDSP_Write32_isr(
	    hMsgQueue->hRegister,
	    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
	        (4 * 4 * hMsgQueue->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_READ_OFFSET + hMsgQueue->ui32DspOffset, 
        ui32dramReadPtr); 

	BDBG_MSG(("ui32dramReadPtr > %x",  ui32dramReadPtr));
	BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));
		
	/* Updating read ptr in the handle */
	hMsgQueue->uiReadPtr = ui32dramReadPtr;
	
	BDBG_LEAVE(BDSP_Raaga_P_GetMsg_isr);
	return err;
 
}

BERR_Code BDSP_Raaga_P_GetAudioDecodeStageSettings(
        BMEM_Handle         hHeap,
        BDSP_AudioType   eAudioType,         /* [in] Decode audio type */
        BDSP_DecodeMode  eDecodeMode,        /* [in] Decode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        void *pSettingsBuffer,                        /*[out]*/
        size_t settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
    
    BDBG_ENTER( BDSP_Raaga_P_GetAudioDecodeStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
        return err;

    if(eDecodeMode == BDSP_DecodeMode_ePassThru)
    {
        if(settingsBufferSize != sizeof(BDSP_Raaga_Audio_PassthruConfigParams))
        {
            BDBG_ERR(("Setting Buffer passed is not of Passthru user config struct. Size passed =%d , Actual Size =%d",settingsBufferSize,sizeof(BDSP_Raaga_Audio_PassthruConfigParams)));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        
        if (settingsBufferSize > uiConfigBufSize)
        {
            BDBG_ERR(("PassThru Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
            BDBG_ASSERT(0);
            return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        }
        BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
        
    }
    else  if(eDecodeMode == BDSP_DecodeMode_eDecode)
    {

        if(settingsBufferSize != BDSP_sAudioCodecDetails[eAudioType]->userConfigSize)
        {
            BDBG_ERR(("Decode Setting Buffer passed is not of Audio type =%s user config struct. Size passed =%d , Actual Size =%d",
                BDSP_sAudioCodecDetails[eAudioType]->audioTypeName,settingsBufferSize,BDSP_sAudioCodecDetails[eAudioType]->userConfigSize));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }             
        if (settingsBufferSize > uiConfigBufSize)
        {
            BDBG_ERR(("Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
            BDBG_ASSERT(0);
            return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        }                  
        BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
    }
    else
    {
            BDBG_ERR(("Invalid Decode Mode %d",eDecodeMode));
            BDBG_ASSERT(0);
    }

    BDBG_LEAVE( BDSP_Raaga_P_GetAudioDecodeStageSettings );
    return err;
}

BERR_Code BDSP_Raaga_P_GetVideoDecodeStageSettings(
        BMEM_Handle         hHeap,
        BDSP_VideoType   	eVideoType,         	/* [in] Decode Video type */
        BDSP_DecodeMode  	eDecodeMode,        	/* [in] Decode Mode */
        unsigned int        uiConfigBufAddr,    	/* [in] Config Buf Address */
        unsigned int        uiConfigBufSize,    	/* [in] Config Buf Size */
        void 				*pSettingsBuffer,       /*[out]*/
        size_t 				settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
    BSTD_UNUSED(eDecodeMode);
    BDBG_ENTER( BDSP_Raaga_P_GetVideoDecodeStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
        return err;


    if(settingsBufferSize != BDSP_sVideoCodecDetails[eVideoType]->userConfigSize)
    {
        BDBG_ERR(("Decode Setting Buffer passed is not of video type =%s user config struct. Size passed =%d , Actual Size =%d",
            BDSP_sVideoCodecDetails[eVideoType]->videoTypeName,settingsBufferSize,BDSP_sVideoCodecDetails[eVideoType]->userConfigSize));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }             
    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
        BDBG_ASSERT(0);
        return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
    }                  
        BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);

    BDBG_LEAVE( BDSP_Raaga_P_GetVideoDecodeStageSettings );
    return err;
}

BERR_Code BDSP_Raaga_P_GetVideoEncodeStageSettings(
        BMEM_Handle         	hHeap,
        BDSP_VideoEncode   		eVidEncType,        /* [in] Decode audio type */
        BDSP_EncodeMode  		eEncodeMode,        /* [in] Encode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        void 					*pSettingsBuffer,                        /*[out]*/
        size_t 					settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
    BSTD_UNUSED(eEncodeMode);
    BDBG_ENTER( BDSP_Raaga_P_GetVideoEncodeStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
        return err;


    if(settingsBufferSize != BDSP_sVideoEncodeCodecDetails[eVidEncType]->userConfigSize)
    {
        BDBG_ERR(("Encode Setting Buffer passed is not of video encode type =%s user config struct. Size passed =%d , Actual Size =%d",
            BDSP_sVideoEncodeCodecDetails[eVidEncType]->videoEncodeTypeName,settingsBufferSize,BDSP_sVideoEncodeCodecDetails[eVidEncType]->userConfigSize));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }             
    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Buffer size allocated(%d) is less than size of configuration parameters structure(%d).",uiConfigBufSize,settingsBufferSize));
        BDBG_ASSERT(0);
        return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
    }                  
    BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);

    BDBG_LEAVE( BDSP_Raaga_P_GetVideoEncodeStageSettings );
    return err;
}

BERR_Code BDSP_Raaga_P_GetAudioDecodeStageStatus(
        BMEM_Handle         hHeap,    
        BDSP_AudioType   eAudioType,         /* [in] Decode audio type */
        BDSP_DecodeMode  eDecodeMode,        /* [in] Decode Mode */
        unsigned int            uiStatusBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiStatusBufSize,    /* [in] Config Buf Size */
        void *pStatusBuffer,                        /*[out]*/
        size_t statusBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
    
    BDBG_ENTER( BDSP_Raaga_P_GetAudioDecodeStageStatus );

    if((pStatusBuffer == NULL) || (statusBufferSize ==0))
        return err;

    if(eDecodeMode == BDSP_DecodeMode_eDecode)
    {
        if(statusBufferSize != BDSP_sAudioCodecDetails[eAudioType]->statusBufferSize)
        {
            BDBG_ERR(("Status Buffer passed is not of Audio type =%s user config struct. Size passed =%d , Actual Size =%d",
                BDSP_sAudioCodecDetails[eAudioType]->audioTypeName,statusBufferSize,BDSP_sAudioCodecDetails[eAudioType]->statusBufferSize));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }             
        if (statusBufferSize > uiStatusBufSize)
        {
            BDBG_ERR(("Buffer size allocated(%d) is less than size of status buffer structure.(%d)",uiStatusBufSize,statusBufferSize));
            BDBG_ASSERT(0);
            return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        }                         
        BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pStatusBuffer,uiStatusBufAddr,statusBufferSize);
        
        
        switch( eAudioType )
        {
            case BDSP_AudioType_eAc3:
            case BDSP_AudioType_eAc3Plus:
            {
                if(0 != ((BDSP_Raaga_Audio_MultiStreamDDPStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Ac3/AC3plus Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                         
            }
            break;

            case BDSP_AudioType_eMpeg:
            {
                if(0 != ((BDSP_Raaga_Audio_MpegStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Mpeg Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                 
            }
            break;			
            
            case BDSP_AudioType_eAacAdts:
            case BDSP_AudioType_eAacLoas:                        
            case BDSP_AudioType_eAacSbrLoas:
            case BDSP_AudioType_eAacSbrAdts:
            {
#ifdef BDSP_MS10_SUPPORT
                if(0 != ((BDSP_Raaga_Audio_DolbyPulseStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: DolbyPulse Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                 
                break;
#else 
                if(0 != ((BDSP_Raaga_Audio_AacheStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Aache Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                 
                break;
#endif             
            }      	                
            case BDSP_AudioType_eWmaStd:
            {
                if(0 != ((BDSP_Raaga_Audio_WmaStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Wma Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            
            case BDSP_AudioType_eWmaPro:
            {
                if(0 != ((BDSP_Raaga_Audio_WmaProStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: WmaPro Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            
            case BDSP_AudioType_ePcm:
            {

            }
            break;     

            case BDSP_AudioType_eDtshd:
            {
                if(0 != ((BDSP_Raaga_Audio_DtsHdStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: DtsHd Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;

            case BDSP_AudioType_eDtsBroadcast:
            {
                if(0 != ((BDSP_Raaga_Audio_DtsHdStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: DtsBroadcast Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
			
			case BDSP_AudioType_eDtsLbr:
            {
                if(0 != ((BDSP_Raaga_Audio_DtslbrStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Mpeg Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }
            }
            break;
            
            case BDSP_AudioType_ePcmWav:
            {
                if(0 != ((BDSP_Raaga_Audio_PcmWavStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: PcmWav Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eAmr:
            {
                if(0 != ((BDSP_Raaga_Audio_AmrStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Amr Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;            
            case BDSP_AudioType_eDra:
            {
                if(0 != ((BDSP_Raaga_Audio_DraStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Dra Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eRealAudioLbr:
            {
                if(0 != ((BDSP_Raaga_Audio_RalbrStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: RealAudioLbr Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eLpcmDvd:
            {
                if(0 != ((BDSP_Raaga_Audio_LpcmStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: LpcmDvd Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eAdpcm:
            {
                if(0 != ((BDSP_Raaga_Audio_AdpcmStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Adpcm Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eG711G726:
            {
                if(0 != ((BDSP_Raaga_Audio_G726StreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: G.711/G.726 Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eG729:
            {
                if(0 != ((BDSP_Raaga_Audio_G729DecStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: G.729 Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eVorbis:
            {
                if(0 != ((BDSP_Raaga_Audio_VorbisDecStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: VORBIS Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eMlp:
            {
                if(0 != ((BDSP_Raaga_Audio_MlpStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: MLP Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eG723_1:
            {
                if(0 != ((BDSP_Raaga_Audio_G723_1_StreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: G.723.1 Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eFlac:
            {
                if(0 != ((BDSP_Raaga_Audio_FlacDecStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Flac Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
			case BDSP_AudioType_eMac:
            {
                if(0 != ((BDSP_Raaga_Audio_MacDecStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Mac Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
            case BDSP_AudioType_eAmrwb:
            {
                if(0 != ((BDSP_Raaga_Audio_AmrWbStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: Amrwb Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;            
            case BDSP_AudioType_eiLBC:
            {
                if(0 != ((BDSP_Raaga_Audio_iLBCStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: iLBC Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;
			case BDSP_AudioType_eiSAC:
            {
                if(0 != ((BDSP_Raaga_Audio_iSACStreamInfo *)pStatusBuffer)->ui32StatusValid)
                {
                    BDBG_MSG(("BDSP_Raaga_P_GetAudioDecodeStageStatus: iSAC Status buffer is not in valid status"));
                    err = (BDSP_ERR_BAD_DEVICE_STATE);
                    goto end;
                }                                 
            }
            break;                  

            default:
                break;
        }
    }
    else
    {
            BDBG_ERR(("Invalid Decode Mode %d",eDecodeMode));
            BDBG_ASSERT(0);
    }
end:
    BDBG_LEAVE( BDSP_Raaga_P_GetAudioDecodeStageStatus);
    return err;
}

BERR_Code BDSP_Raaga_P_SetVideoDecodeStageSettings(
        BMEM_Handle         hHeap,    
        BDSP_VideoType   eVideoType,         /* [in] Decode audio type */
        BDSP_DecodeMode  eDecodeMode,        /* [in] Decode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void *pSettingsBuffer,
        size_t settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
	BSTD_UNUSED(eDecodeMode);
    BDBG_ENTER( BDSP_Raaga_P_SetVideoDecodeStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
        return err;

        if(settingsBufferSize != BDSP_sVideoCodecDetails[eVideoType]->userConfigSize)
        {
            BDBG_ERR(("Setting Buffer passed is not of Video type =%s user config struct. Size passed =%d , Actual Size =%d",
                BDSP_sVideoCodecDetails[eVideoType]->videoTypeName,settingsBufferSize,BDSP_sVideoCodecDetails[eVideoType]->userConfigSize));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }             
        if (settingsBufferSize > uiConfigBufSize)
        {
            BDBG_ERR(("Decode Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
            BDBG_ASSERT(0);
            return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        }                  
        BDSP_Raaga_P_CopyDataToDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);

    BDBG_LEAVE( BDSP_Raaga_P_SetVideoDecodeStageSettings );
    return err;
    }             

BERR_Code BDSP_Raaga_P_SetVideoEncodeStageSettings(
        BMEM_Handle         hHeap,    
        BDSP_VideoEncode   	eVideoEncType,         	/* [in] video encode type */
        BDSP_EncodeMode		eEncodeMode,			/* [in] Encode mode type */
        unsigned int        uiConfigBufAddr,    	/* [in] Config Buf Address */
        unsigned int        uiConfigBufSize,   	 	/* [in] Config Buf Size */
        const void 			*pSettingsBuffer,
        size_t 				settingsBufferSize    
    )
    {
    BERR_Code err = BERR_SUCCESS;
	BSTD_UNUSED(eEncodeMode);
    BDBG_ENTER( BDSP_Raaga_P_SetVideoEncodeStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
        return err;

   
    if(settingsBufferSize != BDSP_sVideoEncodeCodecDetails[eVideoEncType]->userConfigSize)
    {
        BDBG_ERR(("Setting Buffer passed is not of Video Enc type =%s user config struct. Size passed =%d , Actual Size =%d",
            BDSP_sVideoEncodeCodecDetails[eVideoEncType]->videoEncodeTypeName,settingsBufferSize,BDSP_sVideoEncodeCodecDetails[eVideoEncType]->userConfigSize));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }             
    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Decode Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
        BDBG_ASSERT(0);
        return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
    }                  

    BDSP_Raaga_P_CopyDataToDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);        

    BDBG_LEAVE( BDSP_Raaga_P_SetVideoEncodeStageSettings );
    return err;
}

BERR_Code BDSP_Raaga_P_SetAudioDecodeStageSettings(
        BMEM_Handle         hHeap,    
        BDSP_AudioType   eAudioType,         /* [in] Decode audio type */
        BDSP_DecodeMode  eDecodeMode,        /* [in] Decode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void *pSettingsBuffer,
        size_t settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
    
    BDBG_ENTER( BDSP_Raaga_P_SetAudioDecodeStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
        return err;

    if(eDecodeMode == BDSP_DecodeMode_ePassThru)
    {
        if(settingsBufferSize != sizeof(BDSP_Raaga_Audio_PassthruConfigParams))
        {
            BDBG_ERR(("Setting Buffer passed is not of Passthru user config struct. Size passed =%d , Actual Size =%d",settingsBufferSize,sizeof(BDSP_Raaga_Audio_PassthruConfigParams)));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        
        if (settingsBufferSize > uiConfigBufSize)
        {
            BDBG_ERR(("PassThru Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
            BDBG_ASSERT(0);
            return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        }
        BDSP_Raaga_P_CopyDataToDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
        
    }
    else  if(eDecodeMode == BDSP_DecodeMode_eDecode)
    {
        if(settingsBufferSize != BDSP_sAudioCodecDetails[eAudioType]->userConfigSize)
        {
            BDBG_ERR(("Setting Buffer passed is not of Audio type =%s user config struct. Size passed =%d , Actual Size =%d",
                BDSP_sAudioCodecDetails[eAudioType]->audioTypeName,settingsBufferSize,BDSP_sAudioCodecDetails[eAudioType]->userConfigSize));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }             
        if (settingsBufferSize > uiConfigBufSize)
        {
            BDBG_ERR(("Decode Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
            BDBG_ASSERT(0);
            return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        }                  
        BDSP_Raaga_P_CopyDataToDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
    }             
    else
    {
        BDBG_ERR(("Invalid Decode Mode %d",eDecodeMode));
        BDBG_ASSERT(0);
    }                  

    BDBG_LEAVE( BDSP_Raaga_P_SetAudioDecodeStageSettings );
    return err;
}


BERR_Code BDSP_Raaga_P_GetAudioProcessingStageSettings(
        BMEM_Handle         hHeap,
        BDSP_AudioProcessing    eProcessingType,         /* [in] Processing type */
        BDSP_CIT_P_ProcessingMode  eProcessingMode,        /* [in] Processing Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        void *pSettingsBuffer,                        /*[out]*/
        size_t settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
    
    BDBG_ENTER( BDSP_Raaga_P_GetAudioProcessingStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
        return err;

    if(eProcessingMode == BDSP_DSPCHN_ProcessingMode_ePP)
    {
        if(settingsBufferSize != BDSP_sAudioProcessingDetails[eProcessingType]->userConfigSize)
        {
            BDBG_ERR(("Setting Buffer passed is not of Processing type =%s user config struct. Size passed =%d , Actual Size =%d",
            BDSP_sAudioProcessingDetails[eProcessingType]->audioProcessingTypeName,settingsBufferSize,BDSP_sAudioProcessingDetails[eProcessingType]->userConfigSize));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }             
        if (settingsBufferSize > uiConfigBufSize)
        {
            BDBG_ERR(("Post processing Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
            BDBG_ASSERT(0);
            return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        }                  
        err = BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);        
    }
    else
    {
        BDBG_ERR(("Invalid Decode Mode %d",eProcessingMode));
        BDBG_ASSERT(0);
    }

    BDBG_LEAVE( BDSP_Raaga_P_GetAudioProcessingStageSettings );
    return err;
}
            
BERR_Code BDSP_Raaga_P_SetAudioProcessingStageSettings(
        BMEM_Handle         hHeap,    
        BDSP_AudioProcessing    eProcessingType,         /* [in] Processing type */
        BDSP_CIT_P_ProcessingMode  eProcessingMode,        /* [in] Processing Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void *pSettingsBuffer,
        size_t settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
    BDBG_ENTER( BDSP_Raaga_P_SetAudioProcessingStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
    return err;

    if(eProcessingMode == BDSP_DSPCHN_ProcessingMode_ePP)
    {
        if(settingsBufferSize != BDSP_sAudioProcessingDetails[eProcessingType]->userConfigSize)
        {
            BDBG_ERR(("Setting Buffer passed is not of Processing type =%s user config struct. Size passed =%d , Actual Size =%d",
                BDSP_sAudioProcessingDetails[eProcessingType]->audioProcessingTypeName,settingsBufferSize,BDSP_sAudioProcessingDetails[eProcessingType]->userConfigSize));
            return BERR_TRACE(BERR_INVALID_PARAMETER);
        }
        if (settingsBufferSize > uiConfigBufSize)
        {
            BDBG_ERR(("Post processing Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
            BDBG_ASSERT(0);
            return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        }
        err = BDSP_Raaga_P_CopyDataToDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
    }
    else
    {
        BDBG_ERR(("Invalid eProcessingMode Mode %d",eProcessingMode));
        BDBG_ASSERT(0);
    }

    BDBG_LEAVE( BDSP_Raaga_P_SetAudioProcessingStageSettings );
    return err;
}

BERR_Code BDSP_Raaga_P_GetAudioEncodeStageSettings(
        BMEM_Handle         hHeap,
        BDSP_AudioEncode    eEncodeType,         /* [in] Encode type */
        BDSP_EncodeMode     eEncodeMode,        /* [in] Encode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        void *pSettingsBuffer,                        /*[out]*/
        size_t settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;

    BSTD_UNUSED(eEncodeMode);
    BDBG_ENTER( BDSP_Raaga_P_GetAudioEncodeStageSettings );

    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
        return err;

    if(settingsBufferSize != BDSP_sAudioEncodeDetails[eEncodeType]->userConfigSize)
    {
        BDBG_ERR(("Setting Buffer passed is not of Encode type =%s user config struct. Size passed =%d , Actual Size =%d",
        BDSP_sAudioEncodeDetails[eEncodeType]->audioEncodeTypeName,settingsBufferSize,BDSP_sAudioEncodeDetails[eEncodeType]->userConfigSize));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }    
    
    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Encode Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
        BDBG_ASSERT(0);
        return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
    }                  
    err = BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);        

    BDBG_LEAVE( BDSP_Raaga_P_GetAudioEncodeStageSettings );
    return err;
}
            
BERR_Code BDSP_Raaga_P_SetAudioEncodeStageSettings(
        BMEM_Handle         hHeap,    
        BDSP_AudioEncode    eEncodeType,         /* [in] Encode type */
        BDSP_EncodeMode     eEncodeMode,        /* [in] Encode Mode */
        unsigned int            uiConfigBufAddr,    /* [in] Config Buf Address */
        unsigned int            uiConfigBufSize,    /* [in] Config Buf Size */
        const void *pSettingsBuffer,
        size_t settingsBufferSize    
    )
{
    BERR_Code err = BERR_SUCCESS;
    BDBG_ENTER( BDSP_Raaga_P_SetAudioEncodeStageSettings );
    BSTD_UNUSED(eEncodeMode);


    if((pSettingsBuffer == NULL) || (settingsBufferSize ==0))
    return err;

    if(settingsBufferSize != BDSP_sAudioEncodeDetails[eEncodeType]->userConfigSize)
    {
        BDBG_ERR(("Setting Buffer passed is not of Encode type =%s user config struct. Size passed =%d , Actual Size =%d",
            BDSP_sAudioEncodeDetails[eEncodeType]->audioEncodeTypeName,settingsBufferSize,BDSP_sAudioEncodeDetails[eEncodeType]->userConfigSize));
        return BERR_TRACE(BERR_INVALID_PARAMETER);
    }
    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Encode Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
        BDBG_ASSERT(0);
        return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
    }
    err = BDSP_Raaga_P_CopyDataToDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);

    BDBG_LEAVE( BDSP_Raaga_P_SetAudioEncodeStageSettings );
    return err;
}

BERR_Code BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams(
    BMEM_Handle         hHeap,    
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    void *pSettingsBuffer    
)
{
    BERR_Code err=BERR_SUCCESS;    
    BDBG_ASSERT(pSettingsBuffer);
    BKNI_EnterCriticalSection();
    err =BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams_isr(hHeap,uiConfigBufAddr,uiConfigBufSize,pSettingsBuffer);    
    BKNI_LeaveCriticalSection();
    return err;
}

BERR_Code BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams(
    BMEM_Handle         hHeap,    
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    const void *pSettingsBuffer    
)
{
    BERR_Code err=BERR_SUCCESS;    
    BDBG_ASSERT(pSettingsBuffer);
    BKNI_EnterCriticalSection();
    err =BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams_isr(hHeap,uiConfigBufAddr,uiConfigBufSize,pSettingsBuffer);    
    BKNI_LeaveCriticalSection();
    return err;
}


BERR_Code BDSP_Raaga_P_GetFrameSyncTsmStageConfigParams_isr(
    BMEM_Handle         hHeap,    
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    void *pSettingsBuffer    
)
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned int settingsBufferSize=0;
    BDBG_ASSERT(pSettingsBuffer);

    settingsBufferSize = sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams);

    
    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
        ret = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;
    }
    
/*    BKNI_Memcpy((void *)(volatile void*)pSettingsBuffer,(void *)uiConfigBufAddr,settingsBufferSize);*/
    BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
    

end:
	return ret;

}


BERR_Code BDSP_Raaga_P_SetFrameSyncTsmStageConfigParams_isr(
    BMEM_Handle         hHeap,    
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    const void *pSettingsBuffer    
)
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned int settingsBufferSize=0;
    BDBG_ASSERT(pSettingsBuffer);

    settingsBufferSize = sizeof(BDSP_Raaga_Audio_FrameSyncTsmConfigParams);

    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Buffer size allocated(%d) is less than size of configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
        ret = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;
    }
    
/*    BKNI_Memcpy((void *)(volatile void*)uiConfigBufAddr,(void *)pSettingsBuffer,settingsBufferSize);*/
    BDSP_Raaga_P_CopyDataToDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
    
end:
	return ret;

}

/* Handle video encoder datasync cases here */
BERR_Code BDSP_Raaga_P_GetVideoEncodeFrameSyncStageConfigParams(
    BMEM_Handle         hHeap,    
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    void *pSettingsBuffer    
)
{
    BERR_Code err=BERR_SUCCESS;    
    BDBG_ASSERT(pSettingsBuffer);
    BKNI_EnterCriticalSection();
    err =BDSP_Raaga_P_GetVideoEncodeFrameSyncStageConfigParams_isr(hHeap,uiConfigBufAddr,uiConfigBufSize,pSettingsBuffer);    
    BKNI_LeaveCriticalSection();
    return err;
}

BERR_Code BDSP_Raaga_P_SetVideoEncodeFrameSyncStageConfigParams(
    BMEM_Handle         hHeap,    
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    const void *pSettingsBuffer    
)
{
    BERR_Code err=BERR_SUCCESS;    
    BDBG_ASSERT(pSettingsBuffer);
    BKNI_EnterCriticalSection();
    err =BDSP_Raaga_P_SetVideoEncodeFrameSyncStageConfigParams_isr(hHeap,uiConfigBufAddr,uiConfigBufSize,pSettingsBuffer);    
    BKNI_LeaveCriticalSection();
    return err;
}


BERR_Code BDSP_Raaga_P_GetVideoEncodeFrameSyncStageConfigParams_isr(
    BMEM_Handle         hHeap,    
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    void *pSettingsBuffer    
)
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned int settingsBufferSize=0;
    BDBG_ASSERT(pSettingsBuffer);

    settingsBufferSize = sizeof(BDSP_VideoEncodeTaskDatasyncSettings);

    
    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Buffer size allocated(%d) is less than size of video encoder's framesync config size.(%d)",uiConfigBufSize,settingsBufferSize));
        ret = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;
    }
    
/*    BKNI_Memcpy((void *)(volatile void*)pSettingsBuffer,(void *)uiConfigBufAddr,settingsBufferSize);*/
    BDSP_Raaga_P_CopyDataFromDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
    

end:
	return ret;

}


BERR_Code BDSP_Raaga_P_SetVideoEncodeFrameSyncStageConfigParams_isr(
    BMEM_Handle         hHeap,    
    unsigned int		uiConfigBufAddr,    /* [in] Config Buf Address */
    unsigned int		uiConfigBufSize,     /* [in] Config Buf Size */
    const void *pSettingsBuffer    
)
{
    BERR_Code ret = BERR_SUCCESS;
    unsigned int settingsBufferSize=0;
    BDBG_ASSERT(pSettingsBuffer);

    settingsBufferSize = sizeof(BDSP_VideoEncodeTaskDatasyncSettings);

    if (settingsBufferSize > uiConfigBufSize)
    {
        BDBG_ERR(("Buffer size allocated(%d) is less than size of Video encoder framesync configuration parameters structure.(%d)",uiConfigBufSize,settingsBufferSize));
        ret = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
        goto end;
    }
    
/*    BKNI_Memcpy((void *)(volatile void*)uiConfigBufAddr,(void *)pSettingsBuffer,settingsBufferSize);*/
    BDSP_Raaga_P_CopyDataToDram(hHeap,(uint32_t *)pSettingsBuffer,uiConfigBufAddr,settingsBufferSize);
    
end:
	return ret;

}

BERR_Code BDSP_Raaga_P_GetAsyncMsg_isr(
    BDSP_Raaga_P_MsgQueueHandle    hMsgQueue,  /*[in]*/
	void                        *pMsgBuf,   /*[in]*/
	unsigned int                *puiNumMsgs /*[out]*/
	)
{
    BERR_Code err=BERR_SUCCESS;

    uint32_t ui32dramReadPtr=0;
    uint32_t ui32dramWritePtr=0;
    uint32_t ui32maskReadPtr=0;
    uint32_t ui32maskWritePtr=0;
    uint32_t ui32chunk1=0,ui32chunk2=0,i = 0;
    int32_t  i32BytesToBeRead=0;
    uint32_t ui32ResponseSize = 0;
    void     *pvMsgQueueReadAddr=NULL;
    unsigned int uiMsgIndex = 0, uiContMsgs = 0, uiMoreMsgs = 0;

    BDBG_ENTER(BDSP_Raaga_P_GetMsg);

    BDBG_ASSERT(hMsgQueue);
    BDBG_ASSERT(pMsgBuf);
    BDBG_ASSERT(puiNumMsgs);
    BSTD_UNUSED(ui32chunk1);
    BSTD_UNUSED(ui32chunk2);

    *puiNumMsgs = 0;

	ui32dramReadPtr = BDSP_Read32(
	                    hMsgQueue->hRegister,
	                    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
	                        (4 * 4 * hMsgQueue->ui32FifoId) + 
                            BDSP_RAAGA_P_FIFO_READ_OFFSET + 
                            hMsgQueue->ui32DspOffset);

	ui32dramWritePtr = BDSP_Read32(
                        hMsgQueue->hRegister,
                        BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
                            (4 * 4 * hMsgQueue->ui32FifoId) + 
                            BDSP_RAAGA_P_FIFO_WRITE_OFFSET + 
                            hMsgQueue->ui32DspOffset);


	ui32maskReadPtr  = ui32dramReadPtr;
	ui32maskWritePtr = ui32dramWritePtr;
	   
	 /*Sanity check*/
	/* checking write ptr boundness - 
	    if((writeptr>endaddr)|(writeptr<baseaddr)) write ptr not within bound */	
	    
	if ( (ui32maskWritePtr>hMsgQueue->uiEndAddr)||
         (ui32maskWritePtr<hMsgQueue->uiBaseAddr))
	{
		BDBG_ERR(("Write pointer not within bounds in Message Queue"));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
	}

	
	/* checking read ptr boundness - 
	    if((readptr>endaddr)|(readptr<baseaddr)) read ptr is not within bound */
	if ( (ui32maskReadPtr>hMsgQueue->uiEndAddr)||
         (ui32maskReadPtr<hMsgQueue->uiBaseAddr))
	{
		BDBG_ERR(("Read pointer not within bounds in Message Queue"));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
	}
	
	/*Updating write ptr */
	hMsgQueue->uiWritePtr = ui32maskWritePtr;

	/* checking read ptrs to see if they are the same */
	if ( (hMsgQueue->uiReadPtr) != ui32maskReadPtr)
	{
		BDBG_ERR(("Read pointer corrupted in the Message Queue"));
		BDBG_ASSERT(0);        
		return BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE );		
	}

	/* End of Sanity Check */


    /* --------------------------------------------------------------------
    
    Different cases: 

    If maskwriteptr > maskreadptr
        ReadPtrMSB  WritePtrMSB     Freespace
        0   	    0               write-read           		
        0   		1   	        Impossible Condition
        1   		0   	        Impossible Condition
        1   		1   	        write-read	


    If maskreadptr > maskwriteptr
        ReadptrMSB 	WritePtrMSB	    Freespace 
        0   		0   		    Impossible Condition	
        0   		1   		    (end-read)+(write-base)
        1   		0   		    (end-read)+(write-base)
        1   		1   		    Impossible Condition

    If maskreadptr == maskwriteptr
        If the toggle bits are the same,then there is no message to be read
        If the toggle bits are different, all the messages have to be read
        
    --------------------------------------------------------------------- */

	/* Condition for reading messages from the message queue into the 
	    message buffer If no msg is to be read, generate a 
	    BDSP_ERR_BUFFER_EMPTY error */
    ui32ResponseSize = BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES;

	/* Checking if a message is present */
    
	/* Case1: if(readptr>writeptr) */
	if(ui32maskReadPtr > ui32maskWritePtr)
	{	

		i32BytesToBeRead=(hMsgQueue->uiEndAddr-ui32maskReadPtr)+
                (ui32maskWritePtr-hMsgQueue->uiBaseAddr);
        
        uiContMsgs = (hMsgQueue->uiEndAddr-ui32maskReadPtr)/
                        BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES;
        
        uiMoreMsgs = (ui32maskWritePtr-hMsgQueue->uiBaseAddr)/
                        BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES;
	}

	 /* Case2: if(writeptr>readptr) */
	if(ui32maskWritePtr>ui32maskReadPtr)
	{
		i32BytesToBeRead = ui32maskWritePtr-ui32maskReadPtr;
        uiContMsgs = i32BytesToBeRead / BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES;
        uiMoreMsgs = 0;
	}

	 if(i32BytesToBeRead <= 0)
	 {
		BDBG_MSG(("The Message Queue is empty.No message is present."));
		/*BDBG_ASSERT(0); */
		/* Removing the assert to take care of int timings from fw */
		/*return BERR_TRACE(BDSP_ERR_BUFFER_EMPTY);*/
		return BERR_TRACE(err);
	 }
     
    /* Revisit this if we make buffers a non-integral multiple of message size */
    *puiNumMsgs = i32BytesToBeRead/BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES;
	
	/*Reading Message from the message queue into the message buffer*/
    err = BMEM_ConvertOffsetToAddress(
            hMsgQueue->hHeap, ui32maskReadPtr, (void **)&pvMsgQueueReadAddr);
    if (BERR_SUCCESS != err)
    {
    	return BERR_TRACE(err);
    }
    
    for(uiMsgIndex = 0; uiMsgIndex < uiContMsgs; uiMsgIndex++)
    {
    	for(i=0; i<(ui32ResponseSize/4); i++)
    	{
    		*((uint32_t *)pMsgBuf+(uiMsgIndex * BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES/4)+i)
                = BDSP_Raaga_P_MemRead32(hMsgQueue->hHeap,
                    (uint32_t )pvMsgQueueReadAddr+(i*4) + 
                        (uiMsgIndex * BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES));
    	}
        
       ui32dramReadPtr +=  BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES;
       
	/*updating read ptr in the Queue Attribute Structure*/
	BDSP_Write32(
	    hMsgQueue->hRegister,
	    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
	        (4 * 4 * hMsgQueue->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_READ_OFFSET + hMsgQueue->ui32DspOffset, 
        ui32dramReadPtr
        );        
    }

    err = BMEM_ConvertOffsetToAddress(
            hMsgQueue->hHeap, 
            hMsgQueue->uiBaseAddr, (void **)&pvMsgQueueReadAddr);
        
	if (BERR_SUCCESS != err)
    {   
		return BERR_TRACE(err);
    }
    
    for(uiMsgIndex = 0; uiMsgIndex < uiMoreMsgs; uiMsgIndex++)
    {
    	for(i=0;i<(ui32ResponseSize/4);i++)
    	{
    		*((uint32_t *)pMsgBuf+((uiMsgIndex+uiContMsgs) * BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES/4)+i)
                =BDSP_Raaga_P_MemRead32(hMsgQueue->hHeap,(uint32_t )pvMsgQueueReadAddr+(i*4)+(uiMsgIndex * BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES));
    	}
        ui32dramReadPtr = hMsgQueue->uiBaseAddr + 
                            (uiMsgIndex+1)* 
                            BDSP_RAAGA_ASYNC_RESPONSE_SIZE_IN_BYTES;  
        
	/* Updating read ptr in the Queue Attribute Structure */
	BDSP_Write32(
	    hMsgQueue->hRegister,
	    BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 
	        (4 * 4 * hMsgQueue->ui32FifoId) + 
            BDSP_RAAGA_P_FIFO_READ_OFFSET + hMsgQueue->ui32DspOffset, 
        ui32dramReadPtr
        );         
    }
	/*updating read ptr in the handle*/
	hMsgQueue->uiReadPtr = ui32dramReadPtr;
	
	BDBG_LEAVE(BDSP_Raaga_P_GetMsg);
	return err;
 
}


BERR_Code BDSP_Raaga_P_GetVideoMsg_isr(BDSP_Raaga_P_MsgQueueHandle  hMsgQueue,/*[in]*/
									 uint32_t *pMsgBuf,bool bReadUpdate)
   
{
	BERR_Code err=BERR_SUCCESS;
	
	uint32_t ui32dramReadPtr=0;
	uint32_t ui32dramWritePtr=0;
	uint32_t ui32maskReadPtr=0;
	uint32_t ui32maskWritePtr=0;
	int32_t i32BytesToBeRead=0;
	void *pvMsgQueueReadAddr=NULL;
	
	BDBG_ASSERT(hMsgQueue);
	BDBG_ASSERT(pMsgBuf);

	 
	ui32dramReadPtr=BDSP_Read32_isr(hMsgQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * hMsgQueue->ui32FifoId + 
                    BDSP_RAAGA_P_FIFO_READ_OFFSET + hMsgQueue->ui32DspOffset);

	ui32dramWritePtr=BDSP_Read32_isr(hMsgQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * hMsgQueue->ui32FifoId + 
                    BDSP_RAAGA_P_FIFO_WRITE_OFFSET + hMsgQueue->ui32DspOffset);
       
	BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
	BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));
	ui32maskReadPtr=ui32dramReadPtr;
	ui32maskWritePtr=ui32dramWritePtr;
	   
	 /*Sanity check*/
	/* checking write ptr boundness- if((writeptr>endaddr)|(writeptr<baseaddr)) write ptr is not within bound*/	
	if((ui32maskWritePtr>hMsgQueue->uiEndAddr)||(ui32maskWritePtr<hMsgQueue->uiBaseAddr))
	{
            BDBG_ERR(("Write pointer not within bounds in Message Queue"));
            err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
            goto end;
	}

	
	/* checking read ptr boundness- if((readptr>endaddr)|(readptr<baseaddr)) read ptr is not within bound*/
	if((ui32maskReadPtr>hMsgQueue->uiEndAddr)||(ui32maskReadPtr<hMsgQueue->uiBaseAddr))
	{
            BDBG_ERR(("Read pointer not within bounds in Message Queue"));
            err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
            goto end;
	}
	
	/*Updating write ptr */
	hMsgQueue->uiWritePtr= ui32maskWritePtr;

	/* checking read ptrs to see if they are the same */
	if((hMsgQueue->uiReadPtr)!=ui32maskReadPtr)
	{
            BDBG_ERR(("Read pointer corrupted in the Message Queue"));
            err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
            goto end;
	}

	/* End of Sanity Check */

	/*******************************************************************
   Different cases: 

    If maskreadptr>maskwriteptr
     ReadPtrMSB   WritePtrMSB 		
        0   				0                   This condn. is not possible
        0   				1   	     		(end-read)+(write-base)
        1   				0   	              (end-read)+(write-base)
        1   				1   	     	       this condn. is not possible

  If maskwriteptr>maskreadptr
   ReadptrMSB 	WritePtrMSB	 
    0   				0   		  	write-read
    0   				1   		       this condn. not possible
    1   				0   		       this condn. not possible
    1   				1   		       write-read

  If maskreadptr==maskwriteptr
  If the toggle bits are the same,no message to read
  If the toggle bits are different all the messages have to be read
   

 ***********************************************************************/
	/*Condn. for reading messages from the message queue into the message buffer*/
	/* If no msg is to be read, generate a BRAP_ERR_BUFFER_EMPTY error(new error defined in brap.h)*/

	/* Checking if a msg is present */
	
	/* Case1:if(readptr>writeptr)*/
    if(ui32maskReadPtr > ui32maskWritePtr)
    {	
            i32BytesToBeRead=(hMsgQueue->uiEndAddr-ui32maskReadPtr)+
                                            (ui32maskWritePtr-hMsgQueue->uiBaseAddr);
    }

    /* Case2:if(writeptr>readptr) */
    if(ui32maskWritePtr>ui32maskReadPtr)
    {
        i32BytesToBeRead=ui32maskWritePtr-ui32maskReadPtr;
    }

    /*Case 3:if readptr == writeptr */
    if(ui32maskWritePtr ==ui32maskReadPtr)
    {	
        /*All messages have to be read*/
        i32BytesToBeRead=0;
    }
    if(i32BytesToBeRead == 0)
    {
        BDBG_ERR(("The Message Queue is empty.No message is present."));
        goto end;
    }

/*This conversion is done to pass a Phaysical address to BDSP_Raaga_P_MemRead32_isr*/
    if(BERR_SUCCESS!=(err=BMEM_ConvertOffsetToAddress_isr(hMsgQueue->hHeap,ui32maskReadPtr,(void **)&pvMsgQueueReadAddr)))
    {
        err = BERR_TRACE(err);    
        goto end;
    }


    /*Reading Message from the message queue into the message buffer*/
    *pMsgBuf=BDSP_Raaga_P_MemRead32_isr(hMsgQueue->hHeap,(uint32_t )pvMsgQueueReadAddr);

    BDBG_MSG(("In BRAP_P_GetMsg_isr *pMsgBuf = 0x%x\n",*pMsgBuf));

    if ((bReadUpdate == true)&&(i32BytesToBeRead!=0))
    {
        ui32dramReadPtr=ui32dramReadPtr+4;
        if(ui32dramReadPtr==hMsgQueue->uiEndAddr)
        ui32dramReadPtr=hMsgQueue->uiBaseAddr;        

        /*updating read ptr in the Queue Attribute Structure*/
        BDSP_Write32_isr(hMsgQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * hMsgQueue->ui32FifoId + 
        BDSP_RAAGA_P_FIFO_READ_OFFSET + hMsgQueue->ui32DspOffset, ui32dramReadPtr); 
        BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
        BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));

        /*updating read ptr in the handle*/
        hMsgQueue->uiReadPtr = ui32dramReadPtr;
    }
	
end:
	return err;
 
}


BERR_Code BDSP_Raaga_P_WriteVideoMsg_isr(BDSP_Raaga_P_MsgQueueHandle   hMsgQueue/*[in]*/,
									     void *pMsgBuf, /*[in]*/
									     unsigned int uiBufSize/*[in]*/)
{	
	BERR_Code err=BERR_SUCCESS;
	unsigned int uiFreeSpace=0;
	uint32_t ui32dramReadPtr=0;
	uint32_t ui32dramWritePtr=0;
	uint32_t ui32maskReadPtr=0;
	uint32_t ui32maskWritePtr=0; 
	void *pvMsgQueueWriteAddr=NULL;
			
	BDBG_ASSERT(hMsgQueue);
	BDBG_ASSERT(pMsgBuf);
	
	ui32dramReadPtr=BDSP_Read32_isr(hMsgQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * hMsgQueue->ui32FifoId + 
                    BDSP_RAAGA_P_FIFO_READ_OFFSET + hMsgQueue->ui32DspOffset);
	
	ui32dramWritePtr=BDSP_Read32_isr(hMsgQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * hMsgQueue->ui32FifoId + 
                    BDSP_RAAGA_P_FIFO_WRITE_OFFSET + hMsgQueue->ui32DspOffset);
	
        ui32maskReadPtr=ui32dramReadPtr;
        ui32maskWritePtr=ui32dramWritePtr;

        /*Sanity check*/
	/*Checking boundness of read pointer- if((readptr>endaddr) OR (readptr<baseaddr)) read ptr not within bound*/
	if((ui32maskReadPtr>hMsgQueue->uiEndAddr)||(ui32maskReadPtr<hMsgQueue->uiBaseAddr))
	{
            BDBG_ERR(("Read pointer not within bounds in Message Queue"));
            err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
            goto end;
	}
	
	/*Checking boundness of write pointer - if((writeptr>endaddr) OR (writeptr<baseaddr))  write ptr not within bound*/
	if((ui32maskWritePtr>hMsgQueue->uiEndAddr)||(ui32maskWritePtr<hMsgQueue->uiBaseAddr))
	{
            BDBG_ERR(("Write pointer not within bounds in Message Queue"));
            err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
            goto end;
	}

	/*update read ptr */
	hMsgQueue->uiReadPtr= ui32maskReadPtr;

	/* checking write ptrs */
	if((hMsgQueue->uiWritePtr)!=ui32maskWritePtr)
	{
            BDBG_ERR(("Write pointer corrupted in the Message Queue"));
            err = BERR_TRACE(BDSP_ERR_BAD_DEVICE_STATE);
            goto end;
	}
    
	
/* End of Sanity Check */
/*******************************************************************
   Different cases: 

    If maskwriteptr> maskreadptr
     ReadPtrMSB   WritePtrMSB 	freespace
        0   				0          freespace=((endaddr-writeptr)+(readptr-baseaddr));           		
        0   				1   	     this condn not possible	
        1   				0   	     this condn. not possible
        1   				1   	     freespace=((endaddr-writeptr)+(readptr-baseaddr));	

  If maskreadptr>maskwriteptr
   ReadptrMSB 	WritePtrMSB	 freespace 
    0   				0   		  this condn. not possible	
    0   				1   		  read-write
    1   				0   		  read-write
    1   				1   		  this condn. not possible

  If maskreadptr==maskwriteptr
  If the toggle bits are the same,then the buffer is empty
  If the toggle bits are different,the buffer is full
 ***********************************************************************/

	
	/*Calc free space in the message queue*/

	/* Case1:if(maskwriteptr>maskreadptr)*/
    if(ui32maskWritePtr > ui32maskReadPtr)
    {	
        uiFreeSpace=((hMsgQueue->uiEndAddr-ui32maskWritePtr)+
                            (ui32maskReadPtr-hMsgQueue->uiBaseAddr))-uiBufSize;
    }


    /* Case2:if(maskreadptr>maskwriteptr) */
    if(ui32maskReadPtr>ui32maskWritePtr)
    {
        uiFreeSpace=(ui32maskReadPtr-ui32maskWritePtr)-uiBufSize;
    }


    /* Case3:if(maskreadptr==maskwriteptr) */
    if(ui32maskReadPtr==ui32maskWritePtr)
    {
        /* The buffer is empty */
        uiFreeSpace=(hMsgQueue->uiEndAddr-hMsgQueue->uiBaseAddr)-uiBufSize;
    }

    /*Generate BUFFER_FULL error  when there is no space for the message to be written into the message queue*/
    /* BRAP_ERR_BUFFER_FULL is a new error that has been defined in brap.h */

    if(uiFreeSpace <= 0)/*Considering the buffer has only one message (i.e) uiBufSize=MESSAGE_SIZE */
    {
        BDBG_ERR(("No Free space in the buffer.No new messages can be written"));
        err =  BERR_TRACE(BDSP_ERR_BUFFER_FULL);
    }

    /*writing msgs into the message queue*/
    /*Assume:the Message buffer always has only one message*/

    BDBG_MSG(("Buffer size should be a multiple of 4"));
    BDBG_ASSERT(!(uiBufSize%4));
    BDBG_MSG(("uiBufSize > %d", uiBufSize));

    /*This conversion is done to pass a virtual address as the First argument of BRAP_MemWrite32*/
    if(BERR_SUCCESS!=(err=BMEM_ConvertOffsetToAddress_isr(hMsgQueue->hHeap,ui32maskWritePtr,(void **)&pvMsgQueueWriteAddr)))
    {
        err = BERR_TRACE(err);
        goto end;
    }

    /*Writing into Message queue*/
    BDBG_MSG(("In BRAP_P_WriteMsg_isr *(uint32_t *)pMsgBuf > 0x%x\n", *((uint32_t *)pMsgBuf)));
    BDSP_Raaga_P_MemWrite32_isr(hMsgQueue->hHeap,(uint32_t)pvMsgQueueWriteAddr, *((uint32_t *)pMsgBuf));
    ui32dramWritePtr=ui32dramWritePtr+4;

    /* Taking wrap-around into consideration*/
    if(ui32dramWritePtr==hMsgQueue->uiEndAddr)
    ui32dramWritePtr=hMsgQueue->uiBaseAddr;

    /*updating write ptr in the Queue Attribute Structure*/
    BDSP_Write32_isr(hMsgQueue->hRegister,BCHP_RAAGA_DSP_FW_CFG_BASE_ADDR_FIFO_0 + 4 * 4 * hMsgQueue->ui32FifoId + 
    BDSP_RAAGA_P_FIFO_WRITE_OFFSET + hMsgQueue->ui32DspOffset, ui32dramWritePtr); 
    BDBG_MSG(("ui32dramReadPtr > %x", ui32dramReadPtr));
    BDBG_MSG(("ui32dramWritePtr > %x", ui32dramWritePtr));

    /*updating write ptr in the handle*/
    hMsgQueue->uiWritePtr=ui32dramWritePtr;
    BDBG_LEAVE(BRAP_P_WriteMsg_isr);

    end:    
    return err;		
  }


