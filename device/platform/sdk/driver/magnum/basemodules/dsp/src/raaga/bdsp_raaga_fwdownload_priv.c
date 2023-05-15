/***************************************************************************
 *     Copyright (c) 2006-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bdsp_raaga_fwdownload_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/24 $
 * $brcm_Date: 1/25/13 2:27p $
 *
 * Module Description: Host DSP Interface
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/basemodules/dsp/src/raaga/bdsp_raaga_fwdownload_priv.c $
 * 
 * Hydra_Software_Devel/24   1/25/13 2:27p ananthan
 * FWRAA-558: Incorrect free sequence while trying to passthrough MLP
 * [Free resources was having algorithm type enumeration hardcoded for
 * Generic Passthrough while the same was correct at allocation.
 * Correcting the passthrough type based on the usage case]
 * 
 * Hydra_Software_Devel/23   12/5/12 7:50a kavitha
 * SW7425-4242: Removing unwanted debug logs. Cleanup. Logs got checked in
 * during SCM3 checkin
 * 
 * Hydra_Software_Devel/22   11/26/12 1:20p ananthan
 * SW7425-4242: Add SCM3 interface and merge SCM to mainline.
 * 
 * Hydra_Software_Devel/21   10/8/12 11:40a ananthan
 * FWRAA-495: Remove trivial MLP passthrough download messages.
 * 
 * Hydra_Software_Devel/20   8/20/12 7:14p ananthan
 * SW7425-3322: Enabling conditional download of MLP Passthrough when Open
 * Time Downloading is enabled.
 * 
 * Hydra_Software_Devel/19   7/10/12 6:25p ananthan
 * SW7425-3182: Correcting passthrough type assignment. [Fixing unittests
 * failure]
 * 
 * Hydra_Software_Devel/18   7/10/12 4:46p ananthan
 * SW7425-3182: Correcting passthrough type assignment. [Fixing coverity
 * issue]
 * 
 * Hydra_Software_Devel/17   7/4/12 5:24p ananthan
 * SW7425-3182: Correcting passthrough type assignment. [Fixing coverity
 * issue]
 * 
 * Hydra_Software_Devel/16   7/3/12 2:59p ananthan
 * SW7425-3182: Correcting passthrough type assignment.
 * 
 * Hydra_Software_Devel/15   6/6/12 12:58p ananthan
 * FWRAA-443: Add MAT Encoder support in Raaga
 * 
 * Hydra_Software_Devel/14   3/27/12 2:37p ashoky
 * SW7231-363: Update correct mit entries for passthru code/table.
 * 
 * Hydra_Software_Devel/13   1/27/12 1:19p ashoky
 * FWRAA-377: Merging h264 soft dsp video encoder to mainline.
 * 
 * Hydra_Software_Devel/12   7/12/11 4:44p gprasad
 * SW7422-370: Add changes for bdsp_types.h based on code review feedback
 * 
 * Hydra_Software_Devel/11   6/16/11 7:02p gautamk
 * SW7422-369: [7422] fixing debug message
 * 
 * Hydra_Software_Devel/10   6/16/11 2:38p gautamk
 * SW7425-732: [7425] Handling return code from
 * BDSP_Raaga_P_CopyFWImageToMem
 * 
 * Hydra_Software_Devel/9   6/16/11 1:50p gautamk
 * SW7422-357 : [7425] checking for index boundness.
 * 
 * Hydra_Software_Devel/8   5/28/11 10:53p srajapur
 * SW7422-358 : [7425] Added support for MS11 DDRE and DV258 to BDSP
 * 
 * Hydra_Software_Devel/7   5/16/11 10:08p srajapur
 * SW7425-572: [7425] Adding VP6 support in basemodules
 * 
 * Hydra_Software_Devel/6   5/11/11 5:30p srajapur
 * SW7422-374 : [7425] Incorporate changes to BDSP "details" structures
 * from code review feedback.
 * 
 * Hydra_Software_Devel/5   4/26/11 7:12p srajapur
 * SW7425-392: [7425] Freed the memory allocation for encode.
 * 
 * Hydra_Software_Devel/4   4/20/11 4:25p jgarrett
 * SW7425-392: Fixing warning
 * 
 * Hydra_Software_Devel/3   4/20/11 1:10p jgarrett
 * SW7425-392: Removing BDSP_ProcessingType enum
 * 
 * Hydra_Software_Devel/2   4/13/11 8:22p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/1   4/6/11 2:27p srajapur
 * SW7425-291: [7425]BDSP directory structure changes
 * 
 * Hydra_Software_Devel/15   3/15/11 12:28p jgarrett
 * SW7422-146: Removing externs from C files
 * 
 * Hydra_Software_Devel/14   3/9/11 6:30a srajapur
 * SW7422-293:[7422] Fixing encode download problem.
 * 
 * Hydra_Software_Devel/13   3/5/11 4:51a srajapur
 * SW7422-314 : [7422] Adding RDB Independence Feature in BASE DSP
 * Codebase
 * 
 * Hydra_Software_Devel/12   2/26/11 12:39a srajapur
 * SW7422-293:[7422] Adding support for encode algorithms
 * 
 * Hydra_Software_Devel/11   2/5/11 4:23a srajapur
 * SW7422-247 : [7422] Adding Generic passthrul audio processing support.
 * 
 * Hydra_Software_Devel/10   2/3/11 6:23a srajapur
 * SW7422-229 : [7422]Adding support for DDP decode.
 * 
 * Hydra_Software_Devel/9   1/26/11 12:53a gautamk
 * SW7422-191: Adding error recovery code for create task.
 * 
 * Hydra_Software_Devel/8   1/20/11 8:09a gautamk
 * SW7422-191: Implementing code review comments
 * 
 * Hydra_Software_Devel/7   1/19/11 7:14a gautamk
 * SW7422-191: Various DSP Basmodules features and review comment
 * implementation
 * 
 * Hydra_Software_Devel/6   1/19/11 2:14a gautamk
 * SW7422-191: putting all the Details of Audio codec in a const array and
 * then using it across the code.
 * 
 * Hydra_Software_Devel/5   1/18/11 5:57a gautamk
 * SW7422-191: Implementing review comments.
 * 
 * Hydra_Software_Devel/4   1/18/11 4:22a gautamk
 * SW7125-766: Added code for review comments.
 * 
 * Hydra_Software_Devel/2   1/15/11 3:31a gautamk
 * SW7422-191:[7422] CLeaning up unwanted debug message
 * 
 * Hydra_Software_Devel/1   1/13/11 5:36a gautamk
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
#include "bdsp_raaga_fwdownload_priv.h"
#include "bdsp_raaga_priv.h"


BDBG_MODULE(bdsp_raaga_fwdownload);

/******************************************************************************
Summary:
	This Function Copies the FW exec from Image interface to buffer in the DRAM.
*******************************************************************************/

BERR_Code BDSP_Raaga_P_CopyFWImageToMem(
		const BIMG_Interface *iface,
		void *pImgContext,
		uint32_t ui32MemAddr,	/* 0 , If its a offline utility */
		BDSP_Img_Id firmware_id,
		BMEM_Handle hHeap
		)
{
	void *image = NULL, *cacheAddr = NULL;	
	const void *data = NULL;	
	void *context = pImgContext;

	uint32_t ui32Size = 0, ui32numOfChunks = 0,ui32ChunkLen = 0;
	uint32_t ui32Count = 0;
       uint32_t uiSizeCopied=0;

	BERR_Code rc = BERR_SUCCESS;	
	BDBG_ENTER(BDSP_Raaga_P_CopyFWImageToMem);
	BDBG_ASSERT(iface);
	BDBG_ASSERT(pImgContext);

	rc = iface->open(context, &image, firmware_id);
	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("Error in Opening the Image Interface"));
		return BERR_TRACE(rc);
	}

	rc = iface->next(image, 0, &data, 8);
	if (rc != BERR_SUCCESS) 
	{		
		BDBG_ERR(("\nError in fetching next chunk in Image Interface\n"));
		iface->close(image);
		return BERR_TRACE(rc);
	}

	ui32Size =((uint32_t *) data)[0];
	ui32numOfChunks = ((uint32_t *) data)[1];

        BDBG_MSG(("Total Size = %d",ui32Size));
	for (ui32Count = 1;ui32Count <= ui32numOfChunks; ui32Count++)
	{
		/* The implementation of image interface has to be such that there is 
		one header array and then there are firmware binary	arrays each having
		bytes of size BDSP_IMG_CHUNK_SIZE. But the last array most probably will
		not have a size equal to exactly BDSP_IMG_CHUNK_SIZE. So we are testing 
		for the last chunk here and getting the size based on the total firmware
		binary size and number of chunks*/

		ui32ChunkLen = (ui32Count == ui32numOfChunks) ?  \
		(ui32Size - ((ui32Count - 1)*BDSP_IMG_CHUNK_SIZE)): BDSP_IMG_CHUNK_SIZE;

        BDBG_ASSERT(ui32ChunkLen <= BDSP_IMG_CHUNK_SIZE);  
        BDBG_MSG(("ui32Count = %d, ui32numOfChunks = %d , ui32ChunkLen =%d , "
        "ui32MemAddr = %#x",ui32Count,ui32numOfChunks,ui32ChunkLen,ui32MemAddr));

		rc = iface->next(image, ui32Count, &data, ui32ChunkLen);
		if (rc != BERR_SUCCESS) 
		{		
			BDBG_ERR(("\nError in fetching next chunk in Image Interface\n"));;	
			iface->close(image);
			return BERR_TRACE(rc);
		}


            rc = BMEM_Heap_ConvertAddressToCached(hHeap,
                    (void *) ui32MemAddr,(void **) &cacheAddr);

            if (rc!=BERR_SUCCESS)
             {
                BDBG_ERR(("BMEM_Heap_ConvertAddressToCached returned Error %d",rc));
                return BERR_TRACE(rc);
            }

            BKNI_Memcpy((void *)(volatile void *)cacheAddr,data,ui32ChunkLen);	
            BMEM_Heap_FlushCache(hHeap,(void *)cacheAddr,ui32ChunkLen);   
            ui32MemAddr +=ui32ChunkLen;
            uiSizeCopied +=  ui32ChunkLen;
	}	

        if(uiSizeCopied != ui32Size)
        {
            BDBG_ERR(("FW Image (Id =%#x) not downloaded properly",firmware_id));
        }

	iface->close(image);

	BDBG_LEAVE(BDSP_Raaga_P_CopyFWImageToMem);
	return BERR_SUCCESS;
}


static BERR_Code BDSP_Raaga_P_DownloadResidentCode(
	void *pDeviceHandle
)
{
    const BIMG_Interface *iface = NULL;
    void *pImgContext = NULL;
    uint32_t ui32MemAddr = 0,ui32Size = 0;
    BDSP_Img_Id eFirmwareId = BDSP_Img_Id_eInvalid;

    BERR_Code rc = BERR_SUCCESS;
    uint32_t ui32physAddress = 0;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    

    BDBG_ENTER(BDSP_Raaga_P_DownloadResidentCode);	

    iface = pDevice->settings.pImageInterface;
    pImgContext = pDevice->settings.pImageContext;
	
    /* Download System Codes , Includes uCOS OS, CPT and ATM resident tasks*/
    eFirmwareId = BDSP_Img_Id_eSystemCode;
    ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Addr;
    rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
    if(rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto end;
    }

	if(ui32Size > pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download System code"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sSystemCodeMem.ui32Size,eFirmwareId,
				ui32Size
				));
            rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, 
        (void *)(ui32MemAddr), &ui32physAddress);   
	BDBG_MSG(("Downloading System code(ID = %#x) at address "
                "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
                ui32physAddress,ui32Size));
    
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    

    eFirmwareId = BDSP_Img_Id_eScm_Task_Code;
    ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Addr;
    rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
    if(rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto end;
    }

	if(ui32Size > pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download SCM Task code"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Size,eFirmwareId,
				ui32Size
				));
            rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, 
        (void *)(ui32MemAddr), &ui32physAddress);   
	BDBG_MSG(("Downloading SCM Task code(ID = %#x) at address "
                "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
                ui32physAddress,ui32Size));
    
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    

        BDSP_Raaga_P_UpdateMitEntry (pDeviceHandle, BDSP_AF_P_AlgoId_eScmTask, ui32MemAddr, 
            pDevice->memInfo.sDwnldMemInfo.sScmTaskCodeMem.ui32Size, 0,0);
    /* Download SCM1 Digest */
    eFirmwareId = BDSP_Img_Id_eScm1_Digest;
    ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Addr;
    rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
    if(rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto end;
    }

	if(ui32Size > pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download SCM1 Digest"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sScm1DigestMem.ui32Size,eFirmwareId,
				ui32Size
				));
            rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, 
        (void *)(ui32MemAddr), &ui32physAddress);   
	BDBG_MSG(("Downloading SCM1 Digest(ID = %#x) at address "
                "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
                ui32physAddress,ui32Size));
    
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }


    /* Download SCM2 Digest */
    eFirmwareId = BDSP_Img_Id_eScm2_Digest;
    ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Addr;
    rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
    if(rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto end;
    }

	if(ui32Size > pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download SCM2 Digest"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sScm2DigestMem.ui32Size,eFirmwareId,
				ui32Size
				));
            rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, 
        (void *)(ui32MemAddr), &ui32physAddress);   
	BDBG_MSG(("Downloading SCM2 Digest(ID = %#x) at address "
                "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
                ui32physAddress,ui32Size));
    
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }


	/* Download System Data, Includes uCOS OS, CPT and ATM resident tasks */
	eFirmwareId = BDSP_Img_Id_eSystemData; 
	ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Addr;
	rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    
	
	if(ui32Size >pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download System Data"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sSystemDataMem.ui32Size,eFirmwareId,
				ui32Size
				));
		rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, 
        (void *)(ui32MemAddr), &ui32physAddress);   
	BDBG_MSG(("Downloading System Data(ID = %#x) at address "
                "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
                ui32physAddress,ui32Size));
    
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    

    /* Download System Rdb Vars */	
	eFirmwareId = BDSP_Img_Id_eSystemRdbvars; 
	ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Addr;
	rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    
	
	if(ui32Size > pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download System RDB Vars"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sSystemRdbVarsMem.ui32Size,eFirmwareId,
				ui32Size
				));
		rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, 
        (void *)(ui32MemAddr), &ui32physAddress);   
	BDBG_MSG(("Downloading System RDB Vars(ID = %#x) at address "
                "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
                ui32physAddress,ui32Size));
    
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    

	/* Download Syslib Code */
	eFirmwareId = BDSP_Img_Id_eSyslibCode; 
	ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Addr;
	rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    
	
	if(ui32Size > pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download Syslib Code"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Size,eFirmwareId,
				ui32Size
				));
		rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, 
        (void *)(ui32MemAddr), &ui32physAddress);   
	BDBG_MSG(("Downloading Syslib Code(ID = %#x) at address "
        "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
        ui32physAddress,ui32Size));
    
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    
    
        BDSP_Raaga_P_UpdateMitEntry (pDeviceHandle, BDSP_AF_P_AlgoId_eSysLib, ui32MemAddr, 
            pDevice->memInfo.sDwnldMemInfo.sSystemLibMem.ui32Size, 0,0);

	/* Download Algolib Data */
	eFirmwareId = BDSP_Img_Id_eAlgolibCode; 
	ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Addr;
	rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    
	
	if(ui32Size > pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download Algolib Size"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Size,eFirmwareId,
				ui32Size
				));
		rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, 
        (void *)(ui32MemAddr), &ui32physAddress);
    
	BDBG_MSG(("Downloading Algolib (ID = %#x) at address "
                "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
                ui32physAddress,ui32Size));
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    
    
    BDSP_Raaga_P_UpdateMitEntry (pDeviceHandle, BDSP_AF_P_AlgoId_eAlgoLib, ui32MemAddr, 
        pDevice->memInfo.sDwnldMemInfo.sAlgoLibMem.ui32Size, 0,0);

	/* Download CommonIds Code */
	eFirmwareId = BDSP_Img_Id_eCommon_Ids_Code;
	ui32MemAddr = pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Addr;
	rc = BDSP_Raaga_P_GetFWSize (pDeviceHandle, eFirmwareId, &ui32Size);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }
	
	if(ui32Size > pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Size)
	{
		BDBG_ERR(("Size available (%d) to download CommonIds code"
				"(%#x) is less than required (%d)",
				pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Size,eFirmwareId,
				ui32Size
				));
		rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
            goto end;
	}
	
	BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(ui32MemAddr), &ui32physAddress);	
	BDBG_MSG(("Downloading CommonIds code(ID = %#x) at address "
                "0x%x (Physical = %#x), size = 0x%x",eFirmwareId,ui32MemAddr,
                ui32physAddress,ui32Size));
    
	rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,ui32MemAddr,
        eFirmwareId,pDevice->memHandle);
        if(rc != BERR_SUCCESS)
        {
            rc = BERR_TRACE(rc);
            goto end;
        }    
    
    BDSP_Raaga_P_UpdateMitEntry (pDeviceHandle, BDSP_AF_P_AlgoId_eIDSCommonLib, ui32MemAddr, 
        pDevice->memInfo.sDwnldMemInfo.sCommonIdsCodeMem.ui32Size, 0,0);

end: 
    BDBG_LEAVE(BDSP_Raaga_P_DownloadResidentCode);	
	return rc;
}


/*******************************************************************************
Summary: 
	This Function will download all the supported encode algorithms.
	The entries in the MIT will be updated with the Mem. pointers and its size.
	This function is called at device open time.

Pseudo code:
DownloadEncodeExecs:
{
	-For All Supported Encode algorithm
		-Download Code+Table
		BDSP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pDeviceHandle:	RAAGA Handle (Device Handle)

*******************************************************************************/
static BERR_Code BDSP_Raaga_P_DownloadEncodeExecs(
	void *pDeviceHandle
)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t codePtr = 0,ui32CodeSize = 0,tblPtr = 0,tblSize = 0;
	BDSP_Img_Id eCodeFirmwareId = BDSP_Img_Id_eInvalid;
	BDSP_Img_Id eTblFirmwareId = BDSP_Img_Id_eInvalid ;
	BDSP_AF_P_AlgoId eCodeExecId,eTempExecId;
	unsigned int count = 0,i = 0;
	bool bDownloaded= false;	

	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    

    BDBG_ENTER(BDSP_Raaga_P_DownloadEncodeExecs);		

    iface = pDevice->settings.pImageInterface;
    pImgContext = pDevice->settings.pImageContext;

	codePtr= pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32BaseAddr;	
	ui32EndAddr = codePtr+ pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;	    

	for(count = 0; count < BDSP_AudioEncode_eMax; count++)
	{
		if(false == BDSP_Raaga_P_IsAudioEncodeSupported(count))
		{
			continue;
		}

		eCodeExecId = BDSP_sEncAlgoExecInfo[count][0].eAlgoIds[0];
            if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
            {
                continue;
            }                  
        
		eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];
		if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
		{
			BDBG_MSG(("Invalid Image ID for Framesync of Encode Algorithm %#x",count));
			continue;
		}

            if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
            {
                continue;
            }
            else
            {
                pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
            }

		ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];
		if((codePtr + ui32CodeSize) > ui32EndAddr)
		{
			BDBG_ERR(("Size available (%d) to download Framesync (IMG ID = %#x) of Encode Algorithm (%#x)"
					"is less than required (%d)",
					(ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
					ui32CodeSize
					));
			rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;
		}

		BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

		BDBG_MSG(("Downloading Framesync(IMG ID = %#x) of Encode Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
						eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
		rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
                if(rc != BERR_SUCCESS)
                {
                    rc = BERR_TRACE(rc);
                    goto end;
                }        
		BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,0,0);		
		codePtr += ui32CodeSize;
	}
    
		/*Download all encode Code+Table for supported algorithm*/	
		ui32CodeSize = 0;
		codePtr= pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32BaseAddr;	
		ui32EndAddr = codePtr + pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[0].ui32Size;	
    
	for(count = 0; count < BDSP_AudioEncode_eMax; count++)
	{
		if(false == BDSP_Raaga_P_IsAudioEncodeSupported(count))
		{
			continue;
		}
		
		for(i = 1;i<BDSP_sEncAlgoExecInfo[count][0].NumNodes;i++)
		{

		/*Download encode Code */
			eCodeExecId = BDSP_sEncAlgoExecInfo[count][0].eAlgoIds[i];
                    if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }         
			eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

			if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				BDBG_MSG(("Invalid Image ID for  code of Encode algorithm %#x Stage %d",count,i));
				break;
			}
            
                    if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
                    }            

			ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];
			if((codePtr+ ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download  code (IMG ID = %#x) of Encode Algorithm (%#x)"
						" is less than required (%d)",
						(ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
						ui32CodeSize
						));
				rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                            goto end;
			}

			BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

			BDBG_MSG(("Downloading code (IMG ID = %#x) of Encode Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
			rc =  BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
                    if(rc != BERR_SUCCESS)
                    {
                        rc = BERR_TRACE(rc);
                        goto end;
                    }            

			tblPtr = codePtr + ui32CodeSize;

			/*Download encode table */
			eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

			if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
			{
				BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,0,0);	
				codePtr += ui32CodeSize;	                
				BDBG_MSG(("Invalid Image ID for  table of Encode algorithm %#x Stage %d",count,i));
				continue;
			}
			bDownloaded = false;
			BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eEncode,eCodeExecId,&bDownloaded,&eTempExecId);
			
			if(!bDownloaded)
			{
				tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

				if((tblPtr+ tblSize) > ui32EndAddr)
				{
					BDBG_ERR(("Size available (%d) to download  table (IMG ID = %#x) of Encode Algorithm (%#x)"
							" is less than required (%d)",
							(ui32EndAddr-(tblPtr+ tblSize)),eTblFirmwareId,count,
							tblSize
							));
					return	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
				}
				
				BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
				BDBG_MSG(("Downloading table (IMG ID = %#x) of Encode Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
				rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
                            if(rc != BERR_SUCCESS)
                            {
                                rc = BERR_TRACE(rc);
                                goto end;
                            }                
			}
			else
			{
				tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
				tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

				BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
				BDBG_MSG(("Table (IMG ID = %#x) of Encode Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
			}
			BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);	

			/* updating codePtr variable, used to track the used size */ 		
			if(false == bDownloaded)
			{
				codePtr += ui32CodeSize + tblSize;
			}
			else
			{
				codePtr += ui32CodeSize;				
			}
		}
	}
end:    
    BDBG_LEAVE(BDSP_Raaga_P_DownloadEncodeExecs);	
    return rc;

}



/*******************************************************************************
Summary:
	This Function will be called only if the OpenTimeDownload is set. It will 
	download the Passthru executables.
	The entries of MIT wiil be updated as well.

Input Parameter:
	void *pDeviceHandle:	RAAGA Handle (Device Handle)

*******************************************************************************/
static BERR_Code BDSP_Raaga_P_DownloadPassthruExecs(
	void *pDeviceHandle
)
{
    const BIMG_Interface *iface = NULL;
    void *pImgContext = NULL;
    uint32_t codePtr = 0,ui32CodeSize = 0,tblPtr = 0,tblSize = 0;
    BDSP_Img_Id eCodeFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_Img_Id eTblFirmwareId = BDSP_Img_Id_eInvalid ;

    uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
    								for downloading Supported codec */
    uint32_t		ui32physAddress	= 0;
    BERR_Code 			rc = BERR_SUCCESS;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    


    BDBG_ENTER(BDSP_Raaga_P_DownloadPassthruExecs);		

    iface = pDevice->settings.pImageInterface;
    pImgContext = pDevice->settings.pImageContext;


    codePtr= pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32BaseAddr;	
    ui32EndAddr = codePtr+ pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;	

    /*Download passthru Code */
    eCodeFirmwareId = BDSP_Img_Id_ePassthruCode;
    ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[BDSP_AF_P_AlgoId_ePassThru];

    if((codePtr + ui32CodeSize) > ui32EndAddr)
    {
    	BDBG_ERR(("Size available (%d) to download passthru code (IMG ID = %#x) "
    			" is less than required (%d)",
    			(ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,
    			ui32CodeSize
    			));
    	rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;
    }

    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

    BDBG_MSG(("Downloading passthru code (IMG ID = %#x)  at address 0x%x(Physical = %#x), size = 0x%x",
    				eCodeFirmwareId,codePtr,ui32physAddress,ui32CodeSize));
    rc =BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
    if(rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto end;
    }    


    /*Download passthru table */
    eTblFirmwareId = BDSP_Img_Id_ePassthruTable;

    tblPtr = codePtr + ui32CodeSize;
    tblSize = pDevice->imgIdMappingArrays.tableSize[BDSP_AF_P_AlgoId_ePassThru];

    if((tblPtr + tblSize) > ui32EndAddr)
    {
    	BDBG_ERR(("Size available (%d) to download passthru table (IMG ID = %#x) of Audio Algorithm (Passthru)"
    			" is less than required (%d)",
    			(ui32EndAddr-(tblPtr + tblSize)),eTblFirmwareId,
    			tblSize
    			));
    	rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;
    }

    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);			
    BDBG_MSG(("Downloading passthru table (IMG ID = %#x) of Audio Algorithm (Passthru) at address 0x%x(Physical = %#x), size = 0x%x",
    				eTblFirmwareId,tblPtr,ui32physAddress,tblSize));
    rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
    if(rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto end;
    }    

    BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,BDSP_AF_P_AlgoId_ePassThru,codePtr,ui32CodeSize,tblPtr,tblSize);		

    if (BDSP_sAudioProcessingDetails[BDSP_AudioProcessing_eMlpPassThru]->processingSupported == true)
    {
    /*Download MLP passthru Code */
	BDBG_ERR(("Downloading MLP PassThrough Code ... "));
    codePtr= pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32BaseAddr;	
    ui32EndAddr = codePtr+ pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[0].ui32Size;	
    eCodeFirmwareId = BDSP_Img_Id_eMLP_Passthrough_Code;
    ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[BDSP_AF_P_AlgoId_eMLPPassThru];

    if((codePtr + ui32CodeSize) > ui32EndAddr)
    {
    	BDBG_ERR(("Size available (%d) to download passthru code (IMG ID = %#x) "
    			" is less than required (%d)",
    			(ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,
    			ui32CodeSize
    			));
    	rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;
    }

    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

    BDBG_MSG(("Downloading MLP passthru code (IMG ID = %#x)  at address 0x%x(Physical = %#x), size = 0x%x",
    				eCodeFirmwareId,codePtr,ui32physAddress,ui32CodeSize));
    rc =BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
    if(rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto end;
    }    


    /*Download MLP passthru table */
    eTblFirmwareId = BDSP_Img_Id_eMLP_Passthrough_Table;

    tblPtr = codePtr + ui32CodeSize;
    tblSize = pDevice->imgIdMappingArrays.tableSize[BDSP_AF_P_AlgoId_eMLPPassThru];

    if((tblPtr + tblSize) > ui32EndAddr)
    {
    	BDBG_ERR(("Size available (%d) to download MLP passthru table (IMG ID = %#x) of Audio Algorithm (Passthru)"
    			" is less than required (%d)",
    			(ui32EndAddr-(tblPtr + tblSize)),eTblFirmwareId,
    			tblSize
    			));
    	rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
        goto end;
    }

    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);			
    BDBG_MSG(("Downloading MLP passthru table (IMG ID = %#x) of Audio Algorithm (Passthru) at address 0x%x(Physical = %#x), size = 0x%x",
    				eTblFirmwareId,tblPtr,ui32physAddress,tblSize));
    rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
    if(rc != BERR_SUCCESS)
    {
        rc = BERR_TRACE(rc);
        goto end;
    }    

    BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,BDSP_AF_P_AlgoId_eMLPPassThru,codePtr,ui32CodeSize,tblPtr,tblSize);		
    }



end:
    BDBG_LEAVE(BDSP_Raaga_P_DownloadPassthruExecs);	
    return rc;
}

BERR_Code BDSP_Raaga_P_DownloadVideoDecodeExecs(void *pDeviceHandle)
{
    const BIMG_Interface *iface = NULL;
    void *pImgContext = NULL;
    uint32_t codePtr = 0,ui32CodeSize = 0,tblPtr = 0,tblSize = 0;
    BDSP_Img_Id eCodeFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_Img_Id eTblFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_AF_P_AlgoId eCodeExecId ,eTempExecId;
    unsigned int count = 0,i = 0;
    bool bDownloaded= false;	

    uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
    for downloading Supported codec */
    uint32_t		ui32physAddress	= 0;
    BERR_Code 			rc = BERR_SUCCESS;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    


    BDBG_ENTER(BDSP_Raaga_P_DownloadVideoDecodeExecs);		

    iface = pDevice->settings.pImageInterface;
    pImgContext = pDevice->settings.pImageContext;

    codePtr= pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32BaseAddr;	
    ui32EndAddr = codePtr+ pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[0].ui32Size;	


    for(count = 0; count < BDSP_VideoType_eMax; count++)
    {
        if(false == BDSP_Raaga_P_IsVideoTypeSupported(count))
        {
            continue;
        }

        for(i = 0;i<BDSP_sVideoDecAlgoExecInfo[count][BDSP_DecodeMode_eDecode].NumNodes;i++)
        {

            /*Download decode Code */
            eCodeExecId = BDSP_sVideoDecAlgoExecInfo[count][BDSP_DecodeMode_eDecode].eAlgoIds[i];
            if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
            {
                continue;
            }         

            eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];
            if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
            {
                BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",count,i));
                break;
            }

            if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
            {
                continue;
            }
            else
            {
                pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
            }

            ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

            if((codePtr+ ui32CodeSize) > ui32EndAddr)
            {
                BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of Video Algorithm (%#x)"
                                " is less than required (%d)",
                                (ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
                                ui32CodeSize
                                ));
                rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto end;
            }

            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

            BDBG_MSG(("Downloading decode code (IMG ID = %#x) of Video Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
            eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
            rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
            if(rc != BERR_SUCCESS)
            {
                rc = BERR_TRACE(rc);
                goto end;
            }            

            tblPtr = codePtr + ui32CodeSize;

            /*Download decode table */
            eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

            if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
            {
                BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,0,0);	
                codePtr += ui32CodeSize;	                
                BDBG_MSG(("Invalid Image ID for decode table of algorithm %#x Stage %d",count,i));
                continue;
            }
            bDownloaded = false;
            BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eVideoDecode,eCodeExecId,&bDownloaded,&eTempExecId);

            if(!bDownloaded)
            {
                tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                if((tblPtr+ tblSize) > ui32EndAddr)
                {
                    BDBG_ERR(("Size available (%d) to download decode  table (IMG ID = %#x) of video Algorithm (%#x)"
                                        " is less than required (%d)",
                                        (ui32EndAddr-(tblPtr+ tblSize)),eTblFirmwareId,count,
                                        tblSize
                                        ));
                    rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;
                }

                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
                BDBG_MSG(("Downloading decode table (IMG ID = %#x) of video Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
                eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
                rc =BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
                if(rc != BERR_SUCCESS)
                {
                    rc = BERR_TRACE(rc);
                    goto end;
                }                
            }
            else
            {
                tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
                BDBG_MSG(("Decode Table (IMG ID = %#x) of video Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
                eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
            }
            BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);	

            /* updating codePtr variable, used to track the used size */ 		
            if(false == bDownloaded)
            {
                codePtr += ui32CodeSize + tblSize;
            }
            else
            {
                codePtr += ui32CodeSize;				
            }
        }
    }

end:
    BDBG_LEAVE(BDSP_Raaga_P_DownloadVideoDecodeExecs);	
    return rc;

}

BERR_Code BDSP_Raaga_P_DownloadScmExecs(void *pDeviceHandle)
{
    const BIMG_Interface *iface = NULL;
    void *pImgContext = NULL;
    uint32_t codePtr = 0,ui32CodeSize = 0,tblPtr = 0,tblSize = 0;
    BDSP_Img_Id eCodeFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_Img_Id eTblFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_AF_P_AlgoId eCodeExecId ,eTempExecId;
    unsigned int count = 0,i = 0;
    bool bDownloaded= false;
	
	/* End Address of the chunk allocated for downloading Supported codec */
    uint32_t		ui32EndAddr = 0;
    uint32_t		ui32physAddress	= 0;
    BERR_Code 			rc = BERR_SUCCESS;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    


    BDBG_ENTER(BDSP_Raaga_P_DownloadScmExecs);		

    iface = pDevice->settings.pImageInterface;
    pImgContext = pDevice->settings.pImageContext;

    codePtr= pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[0].ui32BaseAddr;	
    ui32EndAddr = codePtr+ pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[0].ui32Size;	


    for(count = 0; count < BDSP_ScmType_eMax; count++)
    {
        if(false == BDSP_Raaga_P_IsScmTypeSupported(count))
        {
		BDBG_ERR (("ERROR in downloadscmexecs "));
            continue;
        }

        for(i = 0;i<BDSP_sScmAlgoExecInfo[count].NumNodes;i++)
        {

            /*Download SCM Code */
            eCodeExecId = BDSP_sScmAlgoExecInfo[count].eAlgoIds[i];
            if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
            {
		BDBG_ERR (("ERROR 1 in downloadscmexecs "));
                continue;
            }         

            eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];
            if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
            {
                BDBG_ERR(("Invalid Image ID for SCM code of algorithm %#x Stage %d",count,i));
                break;
            }

            if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
            {
		BDBG_ERR (("ERROR 2 in downloadscmexecs "));
                continue;
            }
            else
            {
                pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
            }

            ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

            if((codePtr+ ui32CodeSize) > ui32EndAddr)
            {
                BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of SCM Algorithm (%#x)"
                                " is less than required (%d)",
                                (ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
                                ui32CodeSize
                                ));
                rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto end;
            }

            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

            BDBG_MSG(("Downloading decode code (IMG ID = %#x) of SCM Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
            eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
            rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
            if(rc != BERR_SUCCESS)
            {
                rc = BERR_TRACE(rc);
                goto end;
            }            

            tblPtr = codePtr + ui32CodeSize;

            /*Download SCM table */
            eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

            if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
            {
                BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,0,0);	
                codePtr += ui32CodeSize;	                
                BDBG_MSG(("Invalid Image ID for decode table of algorithm %#x Stage %d",count,i));
                continue;
            }
            bDownloaded = false;
            BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eScm,eCodeExecId,&bDownloaded,&eTempExecId);

            if(!bDownloaded)
            {
                tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                if((tblPtr+ tblSize) > ui32EndAddr)
                {
                    BDBG_ERR(("Size available (%d) to download decode  table (IMG ID = %#x) of SCM Algorithm (%#x)"
                                        " is less than required (%d)",
                                        (ui32EndAddr-(tblPtr+ tblSize)),eTblFirmwareId,count,
                                        tblSize
                                        ));
                    rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;
                }

                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
                BDBG_MSG(("Downloading encode table (IMG ID = %#x) of SCM Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
                eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
                rc =BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
                if(rc != BERR_SUCCESS)
                {
                    rc = BERR_TRACE(rc);
                    goto end;
                }                
            }
            else
            {
                tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
                BDBG_MSG(("Encode Table (IMG ID = %#x) of SCM Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
                eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
            }
            BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);	

            /* updating codePtr variable, used to track the used size */ 		
            if(false == bDownloaded)
            {
                codePtr += ui32CodeSize + tblSize;
            }
            else
            {
                codePtr += ui32CodeSize;				
            }
        }
    }

end:
    BDBG_LEAVE(BDSP_Raaga_P_DownloadScmExecs);	
    return rc;

}
/*******************************************************************************
Summary:
	This Function will be called only if the OpenTimeDownload is set. It will 
	download the executables for all supported 	video encoders.
	The entries of MIT wiil be updated as well.

Pseudo code:
DownloadAllSupportedExecs:
{
	
	For each Supported video encode Type
		- Download Framesync Exec
			- There are can be option to choose framesync from various
			  framesync execs available for an algorithm. Sodownload all
			  the framesync options available for algorithms.
			  
		- Download encode Code+Table Exec
		- Download Other required Execs.

	BDSP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pDeviceHandle:	RAAGA Handle (Device Handle)

*******************************************************************************/

BERR_Code BDSP_Raaga_P_DownloadVideoEncodeExecs(void *pDeviceHandle)
{
    const BIMG_Interface *iface = NULL;
    void *pImgContext = NULL;
    uint32_t codePtr = 0,ui32CodeSize = 0,tblPtr = 0,tblSize = 0;
    BDSP_Img_Id eCodeFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_Img_Id eTblFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_AF_P_AlgoId eCodeExecId ,eTempExecId;
    unsigned int count = 0,i = 0;
    bool bDownloaded= false;
	
	/* End Address of the chunk allocated for downloading Supported codec */
    uint32_t		ui32EndAddr = 0;
    uint32_t		ui32physAddress	= 0;
    BERR_Code 			rc = BERR_SUCCESS;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    


    BDBG_ENTER(BDSP_Raaga_P_DownloadVideoEncodeExecs);		

    iface = pDevice->settings.pImageInterface;
    pImgContext = pDevice->settings.pImageContext;

    codePtr= pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32BaseAddr;	
    ui32EndAddr = codePtr+ pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[0].ui32Size;	


    for(count = 0; count < BDSP_VideoEncode_eMax; count++)
    {
        if(false == BDSP_Raaga_P_IsVideoEncodeTypeSupported(count))
        {
            continue;
        }

        for(i = 0;i<BDSP_sVideoEncAlgoExecInfo[count][BDSP_EncodeMode_eNonRealtime].NumNodes;i++)
        {

            /*Download encode Code */
            eCodeExecId = BDSP_sVideoEncAlgoExecInfo[count][BDSP_EncodeMode_eNonRealtime].eAlgoIds[i];
            if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
            {
                continue;
            }         

            eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];
            if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
            {
                BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",count,i));
                break;
            }

            if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
            {
                continue;
            }
            else
            {
                pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
            }

            ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

            if((codePtr+ ui32CodeSize) > ui32EndAddr)
            {
                BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of Video Algorithm (%#x)"
                                " is less than required (%d)",
                                (ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
                                ui32CodeSize
                                ));
                rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto end;
            }

            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

            BDBG_MSG(("Downloading decode code (IMG ID = %#x) of Video Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
            eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
            rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
            if(rc != BERR_SUCCESS)
            {
                rc = BERR_TRACE(rc);
                goto end;
            }            

            tblPtr = codePtr + ui32CodeSize;

            /*Download encode table */
            eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

            if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
            {
                BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,0,0);	
                codePtr += ui32CodeSize;	                
                BDBG_MSG(("Invalid Image ID for decode table of algorithm %#x Stage %d",count,i));
                continue;
            }
            bDownloaded = false;
            BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eVideoEncode,eCodeExecId,&bDownloaded,&eTempExecId);

            if(!bDownloaded)
            {
                tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                if((tblPtr+ tblSize) > ui32EndAddr)
                {
                    BDBG_ERR(("Size available (%d) to download decode  table (IMG ID = %#x) of video Algorithm (%#x)"
                                        " is less than required (%d)",
                                        (ui32EndAddr-(tblPtr+ tblSize)),eTblFirmwareId,count,
                                        tblSize
                                        ));
                    rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;
                }

                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
                BDBG_MSG(("Downloading encode table (IMG ID = %#x) of video Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
                eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
                rc =BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
                if(rc != BERR_SUCCESS)
                {
                    rc = BERR_TRACE(rc);
                    goto end;
                }                
            }
            else
            {
                tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
                BDBG_MSG(("Encode Table (IMG ID = %#x) of video Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
                eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
            }
            BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);	

            /* updating codePtr variable, used to track the used size */ 		
            if(false == bDownloaded)
            {
                codePtr += ui32CodeSize + tblSize;
            }
            else
            {
                codePtr += ui32CodeSize;				
            }
        }
    }

end:
    BDBG_LEAVE(BDSP_Raaga_P_DownloadVideoEncodeExecs);	
    return rc;

}
/*******************************************************************************
Summary:
	This Function will be called only if the OpenTimeDownload is set. It will 
	download the executables for all supported 	audio codecs.
	The entries of MIT wiil be updated as well.

Pseudo code:
DownloadAllSupportedExecs:
{
	
	For each Supported Audio Type
		- Download Framesync Exec
			- There are can be option to choose framesync from various
			  framesync execs available for an algorithm. Sodownload all
			  the framesync options available for algorithms.
			  
		- Download Decode Code+Table Exec
		- Download Passthru Code+Table Exec
		- Download Other required Execs.

	BDSP_DWN_CopyFWImageToMem();
}

Input Parameter:
	void *pDeviceHandle:	RAAGA Handle (Device Handle)

*******************************************************************************/
static  BERR_Code BDSP_Raaga_P_DownloadSupportedCodecExecs(
	void *pDeviceHandle
)
{
	const BIMG_Interface *iface = NULL;
	void *pImgContext = NULL;
	uint32_t codePtr = 0,ui32CodeSize = 0,tblPtr = 0,tblSize = 0;
	BDSP_Img_Id eCodeFirmwareId = BDSP_Img_Id_eInvalid;
	BDSP_Img_Id eTblFirmwareId = BDSP_Img_Id_eInvalid;
	BDSP_AF_P_AlgoId eCodeExecId ,eTempExecId;
	unsigned int count = 0,i = 0;
	bool bDownloaded= false;	

	uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
											for downloading Supported codec */
	uint32_t		ui32physAddress	= 0;
	BERR_Code 			rc = BERR_SUCCESS;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    


    BDBG_ENTER(BDSP_Raaga_P_DownloadSupportedCodecExecs);		

    iface = pDevice->settings.pImageInterface;
    pImgContext = pDevice->settings.pImageContext;

	codePtr= pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[0].ui32BaseAddr;	
	ui32EndAddr = codePtr+ pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[0].ui32Size;	

	for(count = 0; count < BDSP_AudioType_eMax; count++)
	{
            if(!(BDSP_Raaga_P_IsPassthruSupportedWithoutLicense(count)))
            {
                if(!BDSP_Raaga_P_IsAudioTypeSupported(count))
                {
                     continue;
                }
		}

		eCodeExecId = BDSP_sDecAlgoExecInfo[count][BDSP_DecodeMode_eDecode].eAlgoIds[0];                        

                if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                } 
		eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

		if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
		{
			BDBG_MSG(("Invalid Image ID for Framesync of Audio Algorithm %#x",count));
                    continue;
		}

            if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
            {
                continue;
            }
            else
            {
                pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
            }


		ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];
		if((codePtr + ui32CodeSize) > ui32EndAddr)
		{
			BDBG_ERR(("Size available (%d) to download Framesync (IMG ID = %#x) of Audio Algorithm (%#x)"
					"is less than required (%d)",
					(ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
					ui32CodeSize
					));
			rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;
		}

		BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

		BDBG_MSG(("Downloading Framesync(IMG ID = %#x) of Audio Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
						eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
		rc =BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
                if(rc != BERR_SUCCESS)
                {
                    rc = BERR_TRACE(rc);
                    goto end;
                }                
        
		BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,0,0);		
		codePtr += ui32CodeSize;
	}

		/*Download all decode Code+Table for supported algorithm*/	
		ui32CodeSize = 0;
		codePtr= pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32BaseAddr;	
		ui32EndAddr = codePtr + pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[0].ui32Size;	

	for(count = 0; count < BDSP_AudioType_eMax; count++)
	{
		if(false == BDSP_Raaga_P_IsAudioTypeSupported(count))
		{
			continue;
		}
		
		for(i = 1;i<BDSP_sDecAlgoExecInfo[count][BDSP_DecodeMode_eDecode].NumNodes;i++)
		{

		/*Download decode Code */
			eCodeExecId = BDSP_sDecAlgoExecInfo[count][BDSP_DecodeMode_eDecode].eAlgoIds[i];
                    if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }         
			eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

			if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
			{
				BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",count,i));
				break;
			}

                    if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
                    }

			ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

			if((codePtr+ ui32CodeSize) > ui32EndAddr)
			{
				BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of Audio Algorithm (%#x)"
						" is less than required (%d)",
						(ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
						ui32CodeSize
						));
				rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                            goto end;
			}

			BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

			BDBG_MSG(("Downloading decode code (IMG ID = %#x) of Audio Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
							eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
			rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
                    if(rc != BERR_SUCCESS)
                    {
                        rc = BERR_TRACE(rc);
                        goto end;
                    }                
            

			tblPtr = codePtr + ui32CodeSize;

			/*Download decode table */
			eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

			if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
			{
				BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,0,0);	
				codePtr += ui32CodeSize;	                
				BDBG_MSG(("Invalid Image ID for decode table of algorithm %#x Stage %d",count,i));
				continue;
			}
			bDownloaded = false;
			BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);
			
			if(!bDownloaded)
			{
				tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

				if((tblPtr+ tblSize) > ui32EndAddr)
				{
					BDBG_ERR(("Size available (%d) to download decode  table (IMG ID = %#x) of Audio Algorithm (%#x)"
							" is less than required (%d)",
							(ui32EndAddr-(tblPtr+ tblSize)),eTblFirmwareId,count,
							tblSize
							));
					rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                                    goto end;
				}
				
				BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
				BDBG_MSG(("Downloading decode table (IMG ID = %#x) of Audio Algorithm (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
				rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
                            if(rc != BERR_SUCCESS)
                            {
                                rc = BERR_TRACE(rc);
                                goto end;
                            }                
                
			}
			else
			{
				tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
				tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

				BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
				BDBG_MSG(("Decode Table (IMG ID = %#x) of Audio Algorithm (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
								eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
			}
			BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);	

			/* updating codePtr variable, used to track the used size */ 		
			if(false == bDownloaded)
			{
				codePtr += ui32CodeSize + tblSize;
			}
			else
			{
				codePtr += ui32CodeSize;				
			}
		}
	}

end:
    BDBG_LEAVE(BDSP_Raaga_P_DownloadSupportedCodecExecs);	
    return rc;
}



BERR_Code BDSP_Raaga_P_DownloadAudProcessingExecs(
	void *pDeviceHandle
)
{
    const BIMG_Interface *iface = NULL;
    void *pImgContext = NULL;
    uint32_t codePtr = 0,ui32CodeSize = 0,tblPtr = 0,tblSize = 0;
    BDSP_Img_Id eCodeFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_Img_Id eTblFirmwareId = BDSP_Img_Id_eInvalid ;
    BDSP_AF_P_AlgoId eCodeExecId,eTempExecId;
    unsigned int count = 0,i = 0;
    bool bDownloaded= false;	
    BDSP_AF_P_sALGO_EXEC_INFO sExecID;    

    uint32_t		ui32EndAddr = 0; /* End Address of the chunk allocated 
    										for downloading Supported codec */
    uint32_t		ui32physAddress	= 0;
    BERR_Code 			rc = BERR_SUCCESS;

    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;

    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);    

    BDBG_ENTER(BDSP_Raaga_P_DownloadAudProcessingExecs);		
	
    iface = pDevice->settings.pImageInterface;
    pImgContext = pDevice->settings.pImageContext;

    /*Download all Processing Code+Table for supported algorithm*/	
    ui32CodeSize = 0;
    codePtr= pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32BaseAddr;	
    ui32EndAddr = codePtr + pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[0].ui32Size;	

    for(count = 0; count < BDSP_AudioProcessing_eMax; count++)
    {
        if(false == BDSP_Raaga_IsAudioProcessingSupported(count))
        {
            continue;
        }

        if(count == BDSP_AudioProcessing_eFWMixer)
        {
            BDSP_Raaga_P_GetAudProcExecIds((BDSP_AudioProcessing)count,&sExecID);

            eCodeExecId = sExecID.eAlgoIds[0];
            if(eCodeExecId != BDSP_AF_P_AlgoId_eInvalid)
            {
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];
                if(eCodeFirmwareId != BDSP_Img_Id_eInvalid)
                {        		
                    if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
                    {
                        continue;
                    }
                    else
                    {
                        pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
                    }

                    ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

                    if((codePtr+ ui32CodeSize) > ui32EndAddr)
                    {
                        BDBG_ERR(("Size available (%d) to Framesync ID(IMG ID = %#x) of Audio Processing (%#x)"
                        		" is less than required (%d)",
                        		(ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
                        		ui32CodeSize
                        		));
                        rc =BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto end;
                    }

                    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

                    BDBG_MSG(("Downloading  Framesync (IMG ID = %#x) of Audio ProcessiIMGng (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
                    			eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
                    rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
                    if(rc != BERR_SUCCESS)
                    {
                        rc = BERR_TRACE(rc);
                        goto end;
                    }                
                    
                    BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,0,0);
        			codePtr = codePtr + ui32CodeSize;

                }
            }
        
        }

        
        BDBG_MSG(("eCodeExecId > %d", BDSP_sPpAlgoExecInfo[count][0].eAlgoIds[1]));

        for(i = 1;i<BDSP_sPpAlgoExecInfo[count][0].NumNodes;i++)
        {

            /*Download Processing Code */
            eCodeExecId = BDSP_sPpAlgoExecInfo[count][0].eAlgoIds[i];
            if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
            {
                continue;
            }         
            eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

            if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
            {
                BDBG_MSG(("Invalid Image ID for Audio Processing %#x Stage %d",count,i));
                break;
            }

            if(true == pDevice->mit.isExecutablesDownloaded[eCodeExecId])
            {
                continue;
            }
            else
            {
                pDevice->mit.isExecutablesDownloaded[eCodeExecId] = true;
            }

            ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

            if((codePtr+ ui32CodeSize) > ui32EndAddr)
            {
                BDBG_ERR(("Size available (%d) to download code (IMG ID = %#x) of Audio Processing (%#x)"
                		" is less than required (%d)",
                		(ui32EndAddr-(codePtr + ui32CodeSize)),eCodeFirmwareId,count,
                		ui32CodeSize
                		));
                rc =BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                if(rc != BERR_SUCCESS)
                {
                    rc = BERR_TRACE(rc);
                    goto end;
                }                                
            }

            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);

            BDBG_MSG(("Downloading  code (IMG ID = %#x) of Audio Processing (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
            			eCodeFirmwareId,count,codePtr,ui32physAddress,ui32CodeSize));
            rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
            if(rc != BERR_SUCCESS)
            {
                rc = BERR_TRACE(rc);
                goto end;
            }                
            

            tblPtr = codePtr + ui32CodeSize;

            /*Download encode table */
            eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

            if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
            {
                BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,0,0);	
                /* updating codePtr variable, used to track the used size */ 		
                codePtr += ui32CodeSize;				
                BDBG_MSG(("Invalid Image ID for  table of Audio Processing %#x Stage %d",count,i));
                continue;
            }
            bDownloaded = false;
            BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eProcessingAlgo,eCodeExecId,&bDownloaded,&eTempExecId);
            if(!bDownloaded)
            {
                tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                if((tblPtr+ tblSize) > ui32EndAddr)
                {
                	BDBG_ERR(("Size available (%d) to download  table (IMG ID = %#x) of Audio Processing (%#x)"
                			" is less than required (%d)",
                			(ui32EndAddr-(tblPtr+ tblSize)),eTblFirmwareId,count,
                			tblSize
                			));
                	rc =	BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto end;                    
                }

                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
                BDBG_MSG(("Downloading table (IMG ID = %#x) of Audio Processing (%#x) at address 0x%x(Physical = %#x), size = 0x%x",
                				eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
                rc = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
                if(rc != BERR_SUCCESS)
                {
                    rc = BERR_TRACE(rc);
                    goto end;
                }                
                
            }
            else
            {
                tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;

                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);
                BDBG_MSG((" Table (IMG ID = %#x) of Audio Processing (%#x) is already downloaded at address 0x%x(Physical = %#x), size = 0x%x",
                				eTblFirmwareId,count,tblPtr,ui32physAddress,tblSize));
            }
            BDSP_Raaga_P_UpdateMitEntry(pDeviceHandle,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);	

            /* updating codePtr variable, used to track the used size */ 		
            if(false == bDownloaded)
            {
                codePtr += ui32CodeSize + tblSize;
            }
            else
            {
                codePtr += ui32CodeSize;				
            }
        }
    }

end:
    BDBG_LEAVE(BDSP_Raaga_P_DownloadAudProcessingExecs);	
    return rc;

}

/******************************************************************************
Summary:
	This function downloads following at device open time.
			-- Resident Code
			-- TSM Exec
			-- Audio Processing Algo
			-- Decode algorithm Exec

Pseudo Code:			
DownloadFwExec:
{
	DownloadResidentCode();

	DownloadAllTSMExec();

	If(DownloadAudProcAtOpentime) 
	{
	 	DownloadPPAlgoAtOpentime will be decided based on some logic if
	 	it is beneficial to download the PP algos at start time to save 
	 	the memory.
	 	
		DownloadProcessingExecs();
	}
	If(OpenTimeDownload)
	{

		DownloadAllSupportedExecs();
	}
}

Input Parameter:
	void *pDeviceHandle:	RAAGA Handle (Device Handle)

*******************************************************************************/
BERR_Code BDSP_Raaga_P_DownloadFwExec(
	void *pDeviceHandle
)
{
    bool	bOpenTimeFwDownload;
    BERR_Code ret = BERR_SUCCESS;
    BDSP_Raaga *pDevice = (BDSP_Raaga *)pDeviceHandle;    
	
    BDBG_ENTER(BDSP_Raaga_P_DownloadFwExec);
    
    BDBG_OBJECT_ASSERT(pDevice, BDSP_Raaga);   

    bOpenTimeFwDownload = pDevice->settings.preloadImages;
    
    ret = BDSP_Raaga_P_DownloadResidentCode(pDeviceHandle);

    if(bOpenTimeFwDownload)
    { 
        BDSP_Raaga_P_DownloadAudProcessingExecs(pDeviceHandle);
        BDSP_Raaga_P_DownloadEncodeExecs(pDeviceHandle);	
        BDSP_Raaga_P_DownloadSupportedCodecExecs(pDeviceHandle);
        BDSP_Raaga_P_DownloadPassthruExecs(pDeviceHandle);
		BDSP_Raaga_P_DownloadVideoDecodeExecs(pDeviceHandle);        
		BDSP_Raaga_P_DownloadScmExecs(pDeviceHandle);
		BDSP_Raaga_P_DownloadVideoEncodeExecs(pDeviceHandle);
        
    }
    BDBG_LEAVE(BDSP_Raaga_P_DownloadFwExec);
    return ret;
}


/*******************************************************************************
Summary:
	This returns if the audio processing needs to be downloaded at Device open 
	time or Channel Start time.
*******************************************************************************/
void BDSP_Raaga_P_IfOpenTimeProcDownload(bool *bOpenTimeAudProcDownload)
{
    BDBG_ENTER(BDSP_Raaga_P_IfOpenTimeAudProcDownload);	

	*bOpenTimeAudProcDownload = true;

    BDBG_LEAVE(BDSP_Raaga_P_IfOpenTimeAudProcDownload);	
	return;
}
/******************************************************************************
Summary:
	This Function returns true, If the Encode algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsAudioEncodeSupported(unsigned int AlgoId)
{
    if(AlgoId >= BDSP_AudioEncode_eMax)
         return false;
     else
	return	BDSP_sAudioEncodeDetails[AlgoId]->EncodeSupported;
}

/******************************************************************************
Summary:
	This Function returns true, If the Processing algorithm having AlgoId passed as 
	argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsAudioProcessingSupported(unsigned int AlgoId)
{
    if(AlgoId >= BDSP_AudioProcessing_eMax)
        return false;
    else
	return	BDSP_sAudioProcessingDetails[AlgoId]->processingSupported;
}


/******************************************************************************
Summary:
	This Function returns true, If the audio processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsAudioTypeSupported(unsigned int AlgoId)
{

    if(AlgoId >= BDSP_AudioType_eMax)
        return false;
    else
        return BDSP_sAudioCodecDetails[AlgoId]->codecSupported;
	
}

/******************************************************************************
Summary:
	This Function returns true, If the video processing algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsVideoTypeSupported(unsigned int AlgoId)
{

    if(AlgoId >= BDSP_VideoType_eMax)
        return false;
    else
	return BDSP_sVideoCodecDetails[AlgoId]->codecSupported;
	
}

/******************************************************************************
Summary:
	This Function returns true, If theSCM algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsScmTypeSupported(unsigned int AlgoId)
{

    if(AlgoId >= BDSP_ScmType_eMax)
   	{
        return false;
   	}
    else
    {
		return BDSP_sScmCodecDetails[AlgoId]->scmSupported;
    }
	
}
/******************************************************************************
Summary:
	This Function returns true, If the video encode algorithm having AlgoId 
	passed as argument is supported or not.
*******************************************************************************/
bool BDSP_Raaga_P_IsVideoEncodeTypeSupported(unsigned int AlgoId)
{

    if(AlgoId >= BDSP_VideoEncode_eMax)
   	{
        return false;
   	}
    else
    {
		return BDSP_sVideoEncodeCodecDetails[AlgoId]->EncodeSupported;
    }
	
}

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Decode Mode and Decode algo type.
*******************************************************************************/
void BDSP_Raaga_P_GetVideoExecIds(
						BDSP_DecodeMode decMode, 
						BDSP_VideoType vidType, 
						BDSP_AF_P_sALGO_EXEC_INFO *pExecID /* [out] */)
{

    BDBG_ENTER(BDSP_Raaga_P_GetExecIds);

    if((decMode < BDSP_DecodeMode_eMax) &&(vidType < BDSP_VideoType_eMax))
    {
    	*pExecID = BDSP_sVideoDecAlgoExecInfo[vidType][decMode];
    }
    else
    {
        BDBG_ERR(("Decode mode or Video algorithm type is wrong"));
    }
    
    BDBG_LEAVE(BDSP_Raaga_P_GetExecIds);
	return ;
}
/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Scm algorithm type.
*******************************************************************************/
void  BDSP_Raaga_P_GetScmExecIds(
				BDSP_ScmType scmType,	/* [in]Scm processing algorithm */
				BDSP_AF_P_sALGO_EXEC_INFO *pScmExecID /* [out] */)

{

    BDBG_ENTER(BDSP_Raaga_P_GetScmExecIds);

    if((scmType < BDSP_ScmType_eMax))
    {
    	*pScmExecID = BDSP_sScmAlgoExecInfo[scmType];
    }
    else
    {
        BDBG_ERR(("SCM algorithm type is wrong"));
    }
    
    BDBG_LEAVE(BDSP_Raaga_P_GetScmExecIds);
	return ;
}

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular encode Mode and video encode algo type.
*******************************************************************************/
void BDSP_Raaga_P_GetVideoEncodeExecIds(
						BDSP_EncodeMode encMode, 
						BDSP_VideoEncode vidEncType, 
						BDSP_AF_P_sALGO_EXEC_INFO *pExecID /* [out] */)
{

    BDBG_ENTER(BDSP_Raaga_P_GetVideoEncodeExecIds);

    if((encMode < BDSP_EncodeMode_eSimulMode) &&(vidEncType < BDSP_VideoEncode_eMax))
    {
    	*pExecID = BDSP_sVideoEncAlgoExecInfo[vidEncType][encMode];
    }
    else
    {
        BDBG_ERR(("Encode mode or Video algorithm type is wrong"));
    }
    
    BDBG_LEAVE(BDSP_Raaga_P_GetVideoEncodeExecIds);
	return ;
}

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Decode Mode and Decode algo type.
*******************************************************************************/
void BDSP_Raaga_P_GetExecIds(
						BDSP_DecodeMode decMode, 
						BDSP_AudioType audType, 
						BDSP_AF_P_sALGO_EXEC_INFO *pExecID /* [out] */)
{

    BDBG_ENTER(BDSP_Raaga_P_GetExecIds);

	*pExecID = BDSP_sDecAlgoExecInfo[audType][decMode];

    BDBG_LEAVE(BDSP_Raaga_P_GetExecIds);
	return ;
}

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Decode Mode and audio processing algorithm type.
*******************************************************************************/
void  BDSP_Raaga_P_GetAudProcExecIds(
				BDSP_AudioProcessing audProcAlgo,	/* [in]Audio processing algorithm */
				BDSP_AF_P_sALGO_EXEC_INFO *pAudProcExecID /* [out] */)
{
	*pAudProcExecID = BDSP_sPpAlgoExecInfo[audProcAlgo][0];
	return;
}

/*******************************************************************************
Summary:
	This function returns an structure which contains all the executable to 
	download for a particular Encode algorithm type.
*******************************************************************************/
void  BDSP_Raaga_P_GetEncodeExecIds(
				BDSP_AudioEncode encodeAlgo,	/* [in]Audio processing algorithm */
				BDSP_AF_P_sALGO_EXEC_INFO *pEncodeExecID /* [out] */)
{
	*pEncodeExecID = BDSP_sEncAlgoExecInfo[encodeAlgo][0];
	return;
}

/*******************************************************************************
Summary:
	This function returns if the Algo table is already downloaded or not. If its
	downloaded, it sets bDownloaded=true and eCodeFirmwareId to the Algo Code 
	with which the table was associated in the MIT when it was downloaded earlier.
*******************************************************************************/
void BDSP_Raaga_P_IsTableDownloaded(
					BDSP_Raaga_P_ExecType	execType,	
					BDSP_AF_P_AlgoId	 eCodeExecId,
					bool		*bDownloaded,
					BDSP_AF_P_AlgoId *eExecId
					)
{
    BDBG_ENTER(BDSP_Raaga_P_IsTableDownloaded);
    BSTD_UNUSED(eCodeExecId);    
    BSTD_UNUSED(eExecId);	
		if(BDSP_Raaga_P_ExecType_eDecode == execType)
		{
			*bDownloaded = false;		

			if(0)
			{
				
			}
#ifdef BDSP_MPEG_SUPPORT
#endif
		}
		else if(BDSP_Raaga_P_ExecType_ePassthru == execType)
		{
				*bDownloaded = false;							
		}
		else if(BDSP_Raaga_P_ExecType_eProcessingAlgo == execType)
		{
			*bDownloaded = false;				
#ifdef BDSP_MP3_ENCODE_SUPPORT		
#endif
		}
		else
		{
				*bDownloaded = false;				
		}
		
	    BDBG_LEAVE(BDSP_Raaga_P_IsTableDownloaded);	
		return;
}

BERR_Code BDSP_Raaga_P_FreeFwCtxtBuf(
	void *pContextHandle,
	uint32_t codecType,	    /* For PostProcessing it will be BDSP_AudioProcessing
	                           For Decode it will be BDSP_AudioType 
                               For Encode it will be BDSP_AudioEncode */
	BDSP_Raaga_P_ExecType execType)
{
    unsigned int	i = 0;
    BDSP_Raaga_P_CtxtFwBufInfo	*psTempBufInfo;
    bool	bOpenTimeAudProcDownload = false;    
    BDSP_RaagaContext   *pRaagaContext = (BDSP_RaagaContext  *)pContextHandle;    
    BDSP_Raaga  *pDevice = (BDSP_Raaga  *)pRaagaContext->pDevice;
    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);
    
	
    BDBG_ENTER(BDSP_Raaga_P_FreeFwCtxtBuf);

    if(true == pDevice->settings.preloadImages)
    {/* If Execs Downloaded at opentime, No need to decrement the usage count */
        goto end; 
    }
    
	if(BDSP_Raaga_P_ExecType_eFrameSync == execType)
	{
		for(i = 0; i < pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDatasyncDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eAlgoType == (BDSP_AudioType)codecType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Fs Buffer [%d] allocted" 
						"for audio type %#x, Current usage count = %#x",
						i,codecType,psTempBufInfo->numUser));
					break;
					
				}
				else
				{
					BDBG_ERR(("Usage count of Fs Buffer [%d] is already 0."
						"This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}
	}
	else if(BDSP_Raaga_P_ExecType_eDecode == execType)
	{		
		for(i = 0; i < pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDecodeDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eAlgoType == (BDSP_AudioType)codecType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Decode codec Buffer [%d] allocted" 
						"for audio type %#x, Current usage count = %#x",
						i,codecType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Decode codec Buffer [%d] is already 0."
						"This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}
	}
	else if(BDSP_Raaga_P_ExecType_ePassthru== execType)
	{	
		for(i = 0; i < pRaagaContext->settings.audioDownloadBuffer.maxNumAudioPassthruDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BDSP_AudioProcessing)codecType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Passthru codec Buffer [%d] allocted" 
						"for audio type %#x, Current usage count = %#x",
						i,codecType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Passthru codec Buffer [%d] is already 0."
						"This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}	
	}
	else if(BDSP_Raaga_P_ExecType_eProcessingAlgo== execType)
	{
            BDSP_Raaga_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);
            if(true == bOpenTimeAudProcDownload)
            {/* If Execs Downloaded at opentime, No need to decrement the usage count */
                goto end; 
            }	    
		for(i = 0; i < pRaagaContext->settings.audioDownloadBuffer.maxNumAudioProcessingDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BDSP_AudioProcessing)codecType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Audio Processing codec " 
						"Buffer [%d] allocted for audio type %#x, Current usage count = %#x",
						i,codecType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Audio Processing codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}	
	else if(BDSP_Raaga_P_ExecType_eEncode== execType)
	{   
		for(i = 0; i < pRaagaContext->settings.audioDownloadBuffer.maxNumAudioEncodeDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eEAlgoType == (BDSP_AudioEncode)codecType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Encode codec " 
						"Buffer [%d] allocted for audio type %#x, Current usage count = %#x",
						i,codecType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Encode codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}	    
	else if(BDSP_Raaga_P_ExecType_eVideoDecode== execType)
	{   
		for(i = 0; i < pRaagaContext->settings.videoDownloadBuffer.maxNumVideoDecodeDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eVideoAlgoType == (BDSP_VideoType)codecType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Video codec " 
						"Buffer [%d] allocted for Video type %#x, Current usage count = %#x",
						i,codecType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Video codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}	    
	else if(BDSP_Raaga_P_ExecType_eScm== execType)
	{   
		for(i = 0; i < pRaagaContext->settings.scmDownloadBuffer.maxNumScmDecodeDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eScmAlgoType == (BDSP_ScmType)codecType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of SCM codec " 
						"Buffer [%d] allocted for SCM type %#x, Current usage count = %#x",
						i,codecType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of SCM codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}
	else if(BDSP_Raaga_P_ExecType_eVideoEncode== execType)
	{   
		for(i = 0; i < pRaagaContext->settings.videoEncodeDownloadBuffer.maxNumVideoEncodeDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eVideoEncAlgoType == (BDSP_VideoEncode)codecType)
			{
				if(psTempBufInfo->numUser > 0)
				{
					psTempBufInfo->numUser--;
					BDBG_MSG(("Decreasing usage count of Video codec " 
						"Buffer [%d] allocted for Video type %#x, Current usage count = %#x",
						i,codecType,psTempBufInfo->numUser));
					break;
				}
				else
				{
					BDBG_ERR(("Usage count of Video codec Buffer [%d] ."
						"is already 0. This is an Error condition.",i));
					return BERR_TRACE(BERR_UNKNOWN);
				}
			}
		}		
	}
    

end:
    BDBG_LEAVE(BDSP_Raaga_P_FreeFwCtxtBuf);
    return BERR_SUCCESS;
}

BERR_Code BDSP_Raaga_P_AllocateFwCtxtBuf(
    void *pContextHandle, 
    uint32_t codecType,	    /* For PostProcessing it will be BDSP_AudioProcessing
                               For Decode it will be BDSP_AudioType 
                               For Encode it will be BDSP_AudioEncode */
    uint32_t *pui32FwCtxIndx,
    bool *bDownload,
    BDSP_Raaga_P_ExecType execType)
{
    unsigned int	i = 0;
    uint32_t	ui32FirstAvailIdx = BDSP_RAAGA_INVALID_INDEX, ui32SecondAvailIdx = BDSP_RAAGA_INVALID_INDEX;
    BDSP_Raaga_P_CtxtFwBufInfo	*psTempBufInfo=NULL;
    BDSP_RaagaContext   *pRaagaContext = (BDSP_RaagaContext  *)pContextHandle;
    BDSP_Raaga  *pDevice = (BDSP_Raaga  *)pRaagaContext->pDevice;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);

    BDBG_ENTER(BDSP_Raaga_P_AllocateFwCtxtBuf);
    
    *pui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
	
    if(BDSP_Raaga_P_ExecType_eFrameSync == execType)
    {
    	for(i = 0; i < pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDatasyncDownloadBuffer;i++)
    	{
    		psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[i]);
    		if(psTempBufInfo->uExecAlgoType.eAlgoType == (BDSP_AudioType)codecType)
    		{
    			*pui32FwCtxIndx = i;
    			*bDownload = false;
#if 0    			
    			if(false == bWatchDogRecovery)
#endif                    
    			{
    				psTempBufInfo->numUser++;
    				psTempBufInfo->uExecAlgoType.eAlgoType =(BDSP_AudioType)codecType;							
    			}
    			break;
    		}
    		else if((ui32FirstAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
    			&& (psTempBufInfo->numUser == -1))
    				ui32FirstAvailIdx = i;
    		else if((ui32SecondAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
    			&& (psTempBufInfo->numUser == 0))
    				ui32SecondAvailIdx = i;
    	}

    	BDBG_MSG(("pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
    	
    	if(BDSP_RAAGA_INVALID_INDEX != *pui32FwCtxIndx)
    		goto exit;

    	if(ui32FirstAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
    	{
    			*pui32FwCtxIndx = ui32FirstAvailIdx;
    			*bDownload = true;
#if 0    			
                
    			if(false == bWatchDogRecovery)
#endif                    
    			{
    				pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[*pui32FwCtxIndx].numUser = 1;
    				pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[*pui32FwCtxIndx].uExecAlgoType.eAlgoType = (BDSP_AudioType)codecType;					
    			}
    			goto exit;
    	}
    	else if(ui32SecondAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
    	{
    			*pui32FwCtxIndx = ui32SecondAvailIdx;
    			*bDownload = true;
#if 0    			                
    			if(false == bWatchDogRecovery)
#endif                    
    			{
    				pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[*pui32FwCtxIndx].numUser = 1;
    				pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[*pui32FwCtxIndx].uExecAlgoType.eAlgoType = (BDSP_AudioType)codecType;					
    			}
    			goto exit;
    	}
    }
	else if(BDSP_Raaga_P_ExecType_eDecode == execType)
	{
		for(i = 0; i <pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDecodeDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eAlgoType == (BDSP_AudioType)codecType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
#if 0    			                				
				if(false == bWatchDogRecovery)
#endif                    
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eAlgoType = (BDSP_AudioType)codecType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(BDSP_RAAGA_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
#if 0    			                                
				if(false == bWatchDogRecovery)
#endif                    
				{
					pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eAlgoType = (BDSP_AudioType)codecType;
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eAlgoType = (BDSP_AudioType)codecType;
				}
				goto exit;
		}
	}
	else if(BDSP_Raaga_P_ExecType_ePassthru == execType)
	{
		for(i = 0; i < pRaagaContext->settings.audioDownloadBuffer.maxNumAudioPassthruDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BDSP_AudioProcessing)codecType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.ePAlgoType = (BDSP_AudioProcessing)codecType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(BDSP_RAAGA_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;

		if(ui32FirstAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType = (BDSP_AudioProcessing)codecType;					
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType =(BDSP_AudioProcessing)codecType;					
				}
				goto exit;
		}

	}
	else if(BDSP_Raaga_P_ExecType_eProcessingAlgo == execType)
	{	
		for(i = 0; i < pRaagaContext->settings.audioDownloadBuffer.maxNumAudioProcessingDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.ePAlgoType == (BDSP_AudioProcessing)codecType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.ePAlgoType = (BDSP_AudioProcessing)codecType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(BDSP_RAAGA_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType = (BDSP_AudioProcessing)codecType;	
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sProcessingAlgoBufInfo[*pui32FwCtxIndx].uExecAlgoType.ePAlgoType = (BDSP_AudioProcessing)codecType;	
				}
				goto exit;
		}
	}
	else if(BDSP_Raaga_P_ExecType_eEncode== execType)
	{	
		for(i = 0; i <pRaagaContext->settings.audioDownloadBuffer.maxNumAudioEncodeDownloadBuffer;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eEAlgoType == (BDSP_AudioEncode)codecType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
				
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eEAlgoType = (BDSP_AudioEncode)codecType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(BDSP_RAAGA_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;

		if(ui32FirstAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eEAlgoType = (BDSP_AudioEncode)codecType;	
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
#if 0    			                                
                                if(false == bWatchDogRecovery)
#endif
				{
					pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eEAlgoType = (BDSP_AudioEncode)codecType;	
				}
				goto exit;
		}
	}
	else if(BDSP_Raaga_P_ExecType_eVideoDecode == execType)
	{
		for(i = 0; i < BDSP_RAAGA_MAX_VIDEODECODE_CTXT;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eVideoAlgoType == (BDSP_VideoType)codecType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
#if 0				
				if(false == bWatchDogRecovery)
#endif                    
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eVideoAlgoType = (BDSP_VideoType)codecType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",
            __LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(BDSP_RAAGA_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
#if 0                
				if(false == bWatchDogRecovery)
#endif                    
				{
					pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eVideoAlgoType = (BDSP_VideoType)codecType;
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
#if 0                
				if(false == bWatchDogRecovery)
#endif                    
				{
					pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eVideoAlgoType = (BDSP_VideoType)codecType;
				}
				goto exit;
		}
	}    
	else if(BDSP_Raaga_P_ExecType_eScm == execType)
	{
		for(i = 0; i < BDSP_RAAGA_MAX_SCM_CTXT;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eScmAlgoType == (BDSP_ScmType)codecType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
#if 0				
				if(false == bWatchDogRecovery)
#endif                    
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eScmAlgoType = (BDSP_ScmType)codecType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",
			__LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(BDSP_RAAGA_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
#if 0                
				if(false == bWatchDogRecovery)
#endif                    
				{
					pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eScmAlgoType = (BDSP_ScmType)codecType;
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
#if 0                
				if(false == bWatchDogRecovery)
#endif                    
				{
					pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eScmAlgoType = (BDSP_ScmType)codecType;
				}
				goto exit;
		}
	}
	else if(BDSP_Raaga_P_ExecType_eVideoEncode == execType)
	{
		for(i = 0; i < BDSP_RAAGA_MAX_VIDEOENCODE_CTXT;i++)
		{
			psTempBufInfo = &(pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[i]);
			if(psTempBufInfo->uExecAlgoType.eVideoEncAlgoType == (BDSP_VideoEncode)codecType)
			{
				*pui32FwCtxIndx = i;
				*bDownload = false;
#if 0				
				if(false == bWatchDogRecovery)
#endif                    
				{
					psTempBufInfo->numUser++;
					psTempBufInfo->uExecAlgoType.eVideoEncAlgoType = (BDSP_VideoEncode)codecType;		
				}
				break;
			}
			else if((ui32FirstAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == -1))
					ui32FirstAvailIdx = i;
			else if((ui32SecondAvailIdx ==BDSP_RAAGA_INVALID_INDEX) 
				&& (psTempBufInfo->numUser == 0))
					ui32SecondAvailIdx = i;
		}
		BDBG_MSG(("LINE: - %d pui32FwCtxIndx = %d, ui32FirstAvailIdx = %d, ui32SecondAvailIdx =%d , psTempBufInfo.numUser =%d",
            __LINE__,*pui32FwCtxIndx,ui32FirstAvailIdx,ui32SecondAvailIdx,psTempBufInfo->numUser));
		if(BDSP_RAAGA_INVALID_INDEX != *pui32FwCtxIndx)
			goto exit;
		if(ui32FirstAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32FirstAvailIdx;
				*bDownload = true;
#if 0                
				if(false == bWatchDogRecovery)
#endif                    
				{
					pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eVideoEncAlgoType = (BDSP_VideoEncode)codecType;
				}
				goto exit;
		}
		else if(ui32SecondAvailIdx !=BDSP_RAAGA_INVALID_INDEX)
		{
				*pui32FwCtxIndx = ui32SecondAvailIdx;
				*bDownload = true;
#if 0                
				if(false == bWatchDogRecovery)
#endif                    
				{
					pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].numUser = 1;
					pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[*pui32FwCtxIndx].uExecAlgoType.eVideoEncAlgoType = (BDSP_VideoEncode)codecType;
				}
				goto exit;
		}
	}

exit:
	
	BDBG_LEAVE(BDSP_Raaga_P_AllocateFwCtxtBuf);
	return BERR_SUCCESS;
}


BERR_Code BDSP_Raaga_P_DownloadStartTimeFWExec(
                    void *pContextHandle, 
                    const BDSP_TaskCreateSettings *pSettings
)
{
    BDSP_DecodeMode decMode = BDSP_DecodeMode_eMax; 
    BDSP_AudioType audType = BDSP_AudioType_eMax;
    BDSP_VideoType vidType = BDSP_VideoType_eMax; 
	BDSP_PassThruType passthruType = BDSP_PassThruType_eMax;
	BDSP_VideoEncode vidEncType = BDSP_VideoEncode_eMax;
    BDSP_EncodeMode encMode = BDSP_EncodeMode_eInvalid; 
    BDSP_AudioEncode encType = BDSP_AudioEncode_eMax;
	BDSP_ScmType scmType = BDSP_ScmType_eMax;
    BDSP_Img_Id eCodeFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_Img_Id eTblFirmwareId = BDSP_Img_Id_eInvalid;
    BDSP_AF_P_AlgoId eTempExecId = BDSP_AF_P_AlgoId_eMax;
    BDSP_AF_P_AlgoId eCodeExecId = BDSP_AF_P_AlgoId_eMax;
    bool	bDownload = false;
    uint32_t	ui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
    uint32_t codePtr = 0,ui32CodeSize = 0,tblPtr = 0,tblSize = 0,ui32physAddress;
    uint32_t ui32SizeAvail = 0,i = 0,j = 0, l = 0;
    const BIMG_Interface *iface = NULL;	
    void *pImgContext = NULL;
    bool	bOpenTimeAudProcDownload = true;
    bool bDownloaded= false;	
    BDSP_Raaga_P_CtxtFwBufInfo	sBufInfo;
    BDSP_AF_P_sALGO_EXEC_INFO sExecID;
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pContextHandle;
    BDSP_Raaga  *pDevice= pRaagaContext->pDevice;

    BERR_Code   ret= BERR_SUCCESS;

    BDBG_ENTER(BDSP_Raaga_P_DownloadStartTimeFWExec);
    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);

    iface = pDevice->settings.pImageInterface;	
    pImgContext = pDevice->settings.pImageContext;

    if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
    {
        if(pSettings->pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eDecode)
        {
            decMode = pSettings->pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eDecAudioMode;
            audType = pSettings->pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eDecAudioAlgo;

            if(!((BDSP_Raaga_P_IsPassthruSupportedWithoutLicense(audType))&&
                (BDSP_DecodeMode_ePassThru == decMode)))
            {
                if(!BDSP_Raaga_P_IsAudioTypeSupported((uint32_t)audType))
                {
                    BDBG_ERR(("Audio type %#x is not supported for this platform",audType));
                    ret = BERR_TRACE(BERR_NOT_SUPPORTED);
                    goto err_audiotype_notsupported;
                }
            }

            BDSP_Raaga_P_GetExecIds(decMode,audType,&sExecID);
			if (sExecID.eAlgoIds[1] == BDSP_AF_P_AlgoId_ePassThru)
				passthruType = BDSP_PassThruType_eGeneric;
			else if (sExecID.eAlgoIds[1] == BDSP_AF_P_AlgoId_eMLPPassThru)
				passthruType = BDSP_PassThruType_eMlp;
            
			/* Download FS Exec */
            eCodeExecId = sExecID.eAlgoIds[0];
            if(eCodeExecId != BDSP_AF_P_AlgoId_eInvalid)
            {
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];
                if(eCodeFirmwareId != BDSP_Img_Id_eInvalid)
                {        		
                    BDSP_Raaga_P_AllocateFwCtxtBuf(pContextHandle, audType, &ui32FwCtxIndx,&bDownload,BDSP_Raaga_P_ExecType_eFrameSync);

                    /* If its Watchdog recovery, And its exec is not already in the DRAM, 
                    then this is a BAS DEVICE STATE */
                    /* If no Free Buf to download FS */
                    if((BDSP_RAAGA_INVALID_INDEX == ui32FwCtxIndx)
                        || ui32FwCtxIndx >= pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDatasyncDownloadBuffer)
                    {
                        BDBG_ERR(("ERROR!!! -- All FS context buffers are in use. ui32FwCtxIndx =%d",ui32FwCtxIndx));
                        ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto err_allocate_fscontext;                            
                    }

                    sBufInfo = pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[ui32FwCtxIndx];
                    codePtr = sBufInfo.ui32BaseAddr;

                    ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];
                    sBufInfo = pDevice->memInfo.sDwnldMemInfo.sFsBufInfo[ui32FwCtxIndx];

                    if(ui32CodeSize> sBufInfo.ui32Size)
                    {
                        BDBG_ERR(("Size available (%d) to download Framesync IMG ID (%#x) of"
                                        "Audio Algorithm (%#x) is less than required (%d)",
                                        sBufInfo.ui32Size,eCodeFirmwareId,audType,
                                        ui32CodeSize));
                        ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);                            
                        goto err_fscontext_size;                            
                    }

                    if(true == bDownload)
                    {
                        BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);	
                        BDBG_MSG(("Downloading Framesync (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eCodeFirmwareId,audType,codePtr,ui32physAddress,ui32CodeSize));

                        ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
                        if(ret != BERR_SUCCESS)
                        {
                            ret = BERR_TRACE(ret);
                            goto err_fscontext_size;
                        }                
                    }
                    BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,0,0);
                }
            }

            if((BDSP_DecodeMode_eDecode == decMode))
            {
                BDSP_Raaga_P_AllocateFwCtxtBuf((void *)pContextHandle, audType, &ui32FwCtxIndx,&bDownload,BDSP_Raaga_P_ExecType_eDecode);

                /* If its Watchdog recovery, And its exec is not already in the DRAM, 
                then this is a BAS DEVICE STATE */
                /* If no Free Buf to download algo code */
                if((BDSP_RAAGA_INVALID_INDEX == ui32FwCtxIndx)
                    || ui32FwCtxIndx >= pRaagaContext->settings.audioDownloadBuffer.maxNumAudioDecodeDownloadBuffer)
                {
                    BDBG_ERR(("ERROR!!! -- All decode context buffers are in use."));
                    ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto err_allocate_deccontext;                                                
                }

                sBufInfo = pDevice->memInfo.sDwnldMemInfo.sDecCodecBufInfo[ui32FwCtxIndx];
                codePtr = sBufInfo.ui32BaseAddr;
                ui32SizeAvail = sBufInfo.ui32Size;


                /*Download all decode Code+Table for supported algorithm*/  
                for(i = BDSP_CIT_P_DECODE_NODE_INFO_INDEX; i < sExecID.NumNodes; i++)
                {
                    /*Download decode Code */
                    eCodeExecId = sExecID.eAlgoIds[i];
                    if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
                    {
                        continue;
                    }                   
                    eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

                    if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
                    {
                        BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",audType,i));
                        continue;
                    }

                    ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

                    if(ui32CodeSize > ui32SizeAvail)
                    {
                        BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of Audio Algorithm (%#x)"
                                            " is less than required (%d)",  
                                            ui32SizeAvail,eCodeFirmwareId,audType,ui32CodeSize
                                            ));
                        ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto err_deccontext_size;                                                                        
                    }
                    /* Code is already downloaded, just update the MIT entries */
                    if(true == bDownload)
                    {                           
                        BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);   

                        BDBG_MSG(("Downloading decode code (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eCodeFirmwareId,audType,codePtr,ui32physAddress,ui32CodeSize));
                        ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);  
                        if(ret != BERR_SUCCESS)
                        {
                            ret = BERR_TRACE(ret);
                            goto err_deccontext_size;
                        }                          
                    }

                    ui32SizeAvail-= ui32CodeSize;

                    tblPtr = codePtr + ui32CodeSize;

                    /*Download decode table */
                    eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

                    if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
                    {
                        BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,0,0);   
                        codePtr += ui32CodeSize;                                          
                        BDBG_MSG(("Invalid Image ID for decode table of algorithm %#x Stage %d",audType,i));
                        continue;
                    }

                    bDownloaded = false;
                    BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eDecode,eCodeExecId,&bDownloaded,&eTempExecId);

                    if(!bDownloaded)
                    {
                        tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                        if( tblSize > ui32SizeAvail)
                        {
                            BDBG_ERR(("Size available (%d) to download decode table (IMG ID = %#x) of Audio Algorithm (%#x)"
                                                "is less than required (%d)",
                                                ui32SizeAvail,eTblFirmwareId,audType,tblSize
                                                ));
                            ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                            goto err_deccontext_size;                                                                                                    
                        }

                        /* Table is already downloaded, just update the MIT entries */
                        if(true == bDownload)
                        {    
                            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);    
                            BDBG_MSG(("Downloading decode table (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                                eTblFirmwareId,audType,tblPtr,ui32physAddress,tblSize));
                            ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);    
                            if(ret != BERR_SUCCESS)
                            {
                                ret = BERR_TRACE(ret);
                                goto err_deccontext_size;
                            }                            
                        }
                        /* updating codePtr variable, used to track the used size */        
                        ui32SizeAvail-=tblSize;
                    }
                    else
                    {
                        tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                        tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;
                        BDBG_MSG(("decode Table (IMG ID = %#x) of Audio Algorithm (%#x) is already downloaded at address 0x%x, size = 0x%x",
                                        eTblFirmwareId,audType,tblPtr,tblSize));
                    }
                    BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);    
                    if(false == bDownloaded)
                    {
                        codePtr += ui32CodeSize + tblSize;
                    }
                    else
                    {
                        codePtr += ui32CodeSize;                
                    }
                }/*for*/

                ui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
                codePtr = 0;
                ui32CodeSize = 0;
                ui32SizeAvail = 0;
                bDownload = 0;
            }
            else if((BDSP_DecodeMode_ePassThru== decMode))
            {
                /* Download Passthru Mode Execs */         
                ui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
                codePtr = 0;
                ui32CodeSize = 0;
                ui32SizeAvail = 0;
                bDownload = 0;        

				if (passthruType == BDSP_PassThruType_eGeneric)
				{
                BDSP_Raaga_P_AllocateFwCtxtBuf((void *)pContextHandle,BDSP_AudioProcessing_eGenericPassThru, &ui32FwCtxIndx,&bDownload,BDSP_Raaga_P_ExecType_ePassthru);
                /* If no Free Buf to download FS */
                if((BDSP_RAAGA_INVALID_INDEX == ui32FwCtxIndx)
                    || ui32FwCtxIndx >= pRaagaContext->settings.audioDownloadBuffer.maxNumAudioPassthruDownloadBuffer)
                {
                    BDBG_ERR(("ERROR!!! -- All Passthru context buffers are in use."));
                    ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto err_allocate_passthrucontext;                                                                                            
                }

                sBufInfo = pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[ui32FwCtxIndx];
                codePtr = sBufInfo.ui32BaseAddr;
                ui32SizeAvail = sBufInfo.ui32Size;

                eCodeFirmwareId = BDSP_Img_Id_ePassthruCode;
                ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[BDSP_AF_P_AlgoId_ePassThru];

                if(ui32CodeSize > ui32SizeAvail)
                {
                    BDBG_ERR(("Size available (%d) to download passthru code (IMG ID = %#x) of Audio Algorithm (%#x)"
                                        "is less than required (%d)",	
                                        ui32SizeAvail,eCodeFirmwareId,audType,ui32CodeSize
                                        ));
                    ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto err_passthrucontext_size;              
                }
				
                if(true == bDownload)
                {
                    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);	
                    BDBG_MSG(("Downloading passthru code (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eCodeFirmwareId,audType,codePtr,ui32physAddress,ui32CodeSize));
                    ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
                    if(ret != BERR_SUCCESS)
                    {
                        ret = BERR_TRACE(ret);
                        goto err_passthrucontext_size;
                    }                    
                }
				/* Code is already downloaded. Just update the MIT entries */
				else
				{
					BDBG_MSG(("Passthru code already downloaded (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x, size = 0x%x",
										eCodeFirmwareId,audType,codePtr,ui32CodeSize));
				}
                ui32SizeAvail-= ui32CodeSize;
                tblPtr = codePtr + ui32CodeSize;

                /*Download passthru table */
                eTblFirmwareId = BDSP_Img_Id_ePassthruTable;

                tblSize = pDevice->imgIdMappingArrays.tableSize[BDSP_AF_P_AlgoId_ePassThru];

                if( tblSize > ui32SizeAvail)
                {
                    BDBG_ERR(("Size available (%d) to download passthru table (IMG ID = %#x) of Audio Algorithm (%#x)"
                                    "(%#x) is less than required (%d)",
                                    ui32SizeAvail,eTblFirmwareId,audType,tblSize
                                    ));
                    ret =BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto err_passthrucontext_size;                                      
                }
				if(true == bDownload)
				{
                    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);	

                    BDBG_MSG(("Downloading passthru Table (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                        eTblFirmwareId,audType,tblPtr,ui32physAddress,tblSize));
                    ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
                    if(ret != BERR_SUCCESS)
                    {
                        ret = BERR_TRACE(ret);
                        goto err_passthrucontext_size;
                    }                      
				}
                BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,BDSP_AF_P_AlgoId_ePassThru,codePtr,ui32CodeSize,tblPtr,tblSize);	
				}
				else if (passthruType == BDSP_PassThruType_eMlp)
				{

						BDSP_Raaga_P_AllocateFwCtxtBuf((void *)pContextHandle,BDSP_AudioProcessing_eMlpPassThru, &ui32FwCtxIndx,&bDownload,BDSP_Raaga_P_ExecType_ePassthru);
						/* If no Free Buf to download FS */
						if((BDSP_RAAGA_INVALID_INDEX == ui32FwCtxIndx)
							|| ui32FwCtxIndx >= pRaagaContext->settings.audioDownloadBuffer.maxNumAudioPassthruDownloadBuffer)
						{
							BDBG_ERR(("ERROR!!! -- All Passthru context buffers are in use."));
							ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
							goto err_allocate_passthrucontext;																							  
						}
					
						sBufInfo = pDevice->memInfo.sDwnldMemInfo.sPtCodecBufInfo[ui32FwCtxIndx];
						codePtr = sBufInfo.ui32BaseAddr;
						ui32SizeAvail = sBufInfo.ui32Size;
					
						eCodeFirmwareId = BDSP_Img_Id_eMLP_Passthrough_Code;
						ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[BDSP_AF_P_AlgoId_eMLPPassThru];
					
						if(ui32CodeSize > ui32SizeAvail)
						{
							BDBG_ERR(("Size available (%d) to download MLP passthru code (IMG ID = %#x) of Audio Algorithm (%#x)"
												"is less than required (%d)",	
												ui32SizeAvail,eCodeFirmwareId,audType,ui32CodeSize
												));
							ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
							goto err_passthrucontext_size;				
						}
						
						if(true == bDownload)
						{
							BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);	
							BDBG_MSG(("Downloading MLP passthru code (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
												eCodeFirmwareId,audType,codePtr,ui32physAddress,ui32CodeSize));
							ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
							if(ret != BERR_SUCCESS)
							{
								ret = BERR_TRACE(ret);
								goto err_passthrucontext_size;
							}					 
						}
						/* Code is already downloaded. Just update the MIT entries */
						else
						{
							BDBG_MSG(("MLP Passthru code already downloaded (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x, size = 0x%x",
												eCodeFirmwareId,audType,codePtr,ui32CodeSize));
						}
						ui32SizeAvail-= ui32CodeSize;
						tblPtr = codePtr + ui32CodeSize;
					
						/*Download passthru table */
						eTblFirmwareId = BDSP_Img_Id_eMLP_Passthrough_Table;
					
						tblSize = pDevice->imgIdMappingArrays.tableSize[BDSP_AF_P_AlgoId_eMLPPassThru];
					
						if( tblSize > ui32SizeAvail)
						{
							BDBG_ERR(("Size available (%d) to download MLP passthru table (IMG ID = %#x) of Audio Algorithm (%#x)"
											"(%#x) is less than required (%d)",
											ui32SizeAvail,eTblFirmwareId,audType,tblSize
											));
							ret =BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
							goto err_passthrucontext_size;										
						}
						if(true == bDownload)
						{
							BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);	
					
							BDBG_MSG(("Downloading MLP passthru Table (IMG ID = %#x) of Audio Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
								eTblFirmwareId,audType,tblPtr,ui32physAddress,tblSize));
							ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);	
							if(ret != BERR_SUCCESS)
							{
								ret = BERR_TRACE(ret);
								goto err_passthrucontext_size;
							}					   
						}
						BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,BDSP_AF_P_AlgoId_eMLPPassThru,codePtr,ui32CodeSize,tblPtr,tblSize); 
					}
				else
				{
					BDBG_ERR(("Passthru type %#x is not supported for this platform",passthruType));
					ret = BERR_TRACE(BERR_NOT_SUPPORTED);
					goto err_passthrutype_notsupported;
				}

            }/*if*/
        }
        /* Download PostProcessing Algorithms if not downloaded at init time. */
        BDSP_Raaga_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);

        ui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
        codePtr = 0;
        ui32CodeSize = 0;
        ui32SizeAvail = 0;
        bDownload = 0;

        for (i = 0; i < pSettings->numBranches; i++)
        {
            for(j = 0; j < pSettings->pBranchInfo[i]->ui32NumStages; j++)
            {
                if(pSettings->pBranchInfo[i]->sFwStgInfo[j].eStageType == BDSP_CIT_P_StageType_eEncode)
                {
                    encMode = pSettings->pBranchInfo[i]->sFwStgInfo[j].uAudioMode.eEncAudioMode;
                    encType = pSettings->pBranchInfo[i]->sFwStgInfo[j].uAlgorithm.eEncAudioAlgo;

                    if(!BDSP_Raaga_P_IsAudioEncodeSupported((uint32_t)encType))
                    {
                        BDBG_ERR(("Encode type %#x is not supported for this platform",encType));
                        ret = BERR_TRACE(BERR_NOT_SUPPORTED);
                        goto err_encodetype_notsupported;
                    }

                    BDSP_Raaga_P_GetEncodeExecIds(encType,&sExecID);

                    BDSP_Raaga_P_AllocateFwCtxtBuf(pContextHandle, encType, &ui32FwCtxIndx,&bDownload,BDSP_Raaga_P_ExecType_eEncode);
                    sBufInfo = pDevice->memInfo.sDwnldMemInfo.sEncodeCodecBufInfo[ui32FwCtxIndx];
                    codePtr = sBufInfo.ui32BaseAddr;

                    ui32SizeAvail = sBufInfo.ui32Size;                                


                    /* Download FS Exec */
                    eCodeExecId = sExecID.eAlgoIds[0];
                    if(eCodeExecId != BDSP_AF_P_AlgoId_eInvalid)
                    {
                        eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];
                        if(eCodeFirmwareId != BDSP_Img_Id_eInvalid)
                        {        		
                            /* If its Watchdog recovery, And its exec is not already in the DRAM, 
                            then this is a BAS DEVICE STATE */
                            /* If no Free Buf to download FS */
                            if((BDSP_RAAGA_INVALID_INDEX == ui32FwCtxIndx)
                                || ui32FwCtxIndx >= pRaagaContext->settings.audioDownloadBuffer.maxNumAudioEncodeDownloadBuffer)
                            {
                                BDBG_ERR(("ERROR!!! -- All FS context buffers are in use. ui32FwCtxIndx =%d",ui32FwCtxIndx));
                                ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                                goto err_allocate_fscontext;                            
                            }

                            ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

                            if(ui32CodeSize> ui32SizeAvail)
                            {
                                BDBG_ERR(("Size available (%d) to download Framesync IMG ID (%#x) of"
                                                "Encode Algorithm (%#x) is less than required (%d)",
                                                sBufInfo.ui32Size,eCodeFirmwareId,encType,
                                                ui32CodeSize));
                                ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);                            
                                goto err_fscontext_size;                            
                            }

                            if(true == bDownload)
                            {
                                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);	

                                BDBG_MSG(("Downloading Framesync (IMG ID = %#x) of Encode Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                    eCodeFirmwareId,encType,codePtr,ui32physAddress,ui32CodeSize));

                                ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);	
                                if(ret != BERR_SUCCESS)
                                {
                                    ret = BERR_TRACE(ret);
                                    goto err_fscontext_size;
                                }                                      
                            }
                            BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,0,0);
                            codePtr += ui32CodeSize;
                            ui32SizeAvail-= ui32CodeSize;  
                        }
                    }

                    /*Download all encode Code+Table for supported algorithm*/	                    
                    
                    for(l = BDSP_CIT_P_ENCODE_NODE_INFO_INDEX; l < sExecID.NumNodes; l++)
                    {
                        /*Download encode Code */
                        eCodeExecId = sExecID.eAlgoIds[l];
                        if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
                        {
                            continue;
                        }                               
                        eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

                        if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
                        {
                            BDBG_MSG(("Invalid Image ID for  code of encode processing algorithm %#x Stage %d",encType,l));
                            continue;
                        }

                        ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

                        if(ui32CodeSize > ui32SizeAvail)
                        {
                            BDBG_ERR(("Size available (%d) to download encode code (IMG ID = %#x) of Encode Algorithm (%#x)"
                                            "is less than required (%d)",	
                                            ui32SizeAvail,eCodeFirmwareId,encType,ui32CodeSize
                                            ));
                            ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                            goto err_fscontext_size;
                        }

                        /* Code is already downloaded, just update the MIT entries */
                        if(true == bDownload)
                        {                           
                            BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);   

                            BDBG_MSG(("Downloading encode code (IMG ID = %#x) of encode code (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                eCodeFirmwareId,encType,codePtr,ui32physAddress,ui32CodeSize));
                            ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);  
                            if(ret != BERR_SUCCESS)
                            {
                                ret = BERR_TRACE(ret);
                                goto err_fscontext_size;
                            }                                  
                        }

                        ui32SizeAvail-= ui32CodeSize;            
                        tblPtr = codePtr + ui32CodeSize;

                        /*Download encode table */
                        eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

                        if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
                        {
                            BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,0,0);   
                            codePtr += ui32CodeSize;                                          
                            BDBG_MSG(("Invalid Image ID for encode table of algorithm %#x Stage %d",encType,i));
                            continue;
                        }

                        bDownloaded = false;
                        BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eEncode,eCodeExecId,&bDownloaded,&eTempExecId);

                        if(!bDownloaded)
                        {
                            tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                            if( tblSize > ui32SizeAvail)
                            {
                                BDBG_ERR(("Size available (%d) to download encode table (IMG ID = %#x) of Audio Algorithm (%#x)"
                                                "is less than required (%d)",
                                                ui32SizeAvail,eTblFirmwareId,encType,tblSize
                                                ));
                                ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                                goto err_encodecontext_size;
                            }

                            /* Table is already downloaded, just update the MIT entries */
                            if(true == bDownload)
                            {    
                                BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);    
                                BDBG_MSG(("Downloading encode table (IMG ID = %#x) of encode Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                                eTblFirmwareId,encType,tblPtr,ui32physAddress,tblSize));
                                ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);    
                                if(ret != BERR_SUCCESS)
                                {
                                    ret = BERR_TRACE(ret);
                                    goto err_encodecontext_size;
                                }                                         
                            }
                            /* updating codePtr variable, used to track the used size */        
                            ui32SizeAvail-=tblSize;
                        }
                        else
                        {
                            tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                            tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;
                            BDBG_MSG(("decode Table (IMG ID = %#x) of encode Algorithm (%#x) is already downloaded at address 0x%x, size = 0x%x",
                            eTblFirmwareId,encType,tblPtr,tblSize));
                        }
                        BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);    
                        if(false == bDownloaded)
                        {
                            codePtr += ui32CodeSize + tblSize;
                        }
                        else
                        {
                            codePtr += ui32CodeSize;                
                        }							
                    }
                }                    
            }
        }

        ui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
        codePtr = 0;
        ui32CodeSize = 0;

    }
    else if(pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        if(pSettings->pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eVideoDecode)
        {       
            vidType = pSettings->pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoAlgo;

            if(!BDSP_Raaga_P_IsVideoTypeSupported((uint32_t)vidType))
            {
                BDBG_ERR(("Video type %#x is not supported for this platform",vidType));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
                goto err_audiotype_notsupported;
            }

            BDSP_Raaga_P_GetVideoExecIds(BDSP_DecodeMode_eDecode,vidType,&sExecID);

            BDSP_Raaga_P_AllocateFwCtxtBuf((void *)pContextHandle, vidType, &ui32FwCtxIndx,&bDownload,BDSP_Raaga_P_ExecType_eVideoDecode);

            /* If its Watchdog recovery, And its exec is not already in the DRAM, 
            then this is a BAS DEVICE STATE */
            BDBG_MSG(("ui32FwCtxIndx = %d max buffer = %d",ui32FwCtxIndx,
                            pRaagaContext->settings.videoDownloadBuffer.maxNumVideoDecodeDownloadBuffer));
            /* If no Free Buf to download algo code */
            if((BDSP_RAAGA_INVALID_INDEX == ui32FwCtxIndx)
                || ui32FwCtxIndx >= pRaagaContext->settings.videoDownloadBuffer.maxNumVideoDecodeDownloadBuffer)
            {
                BDBG_ERR(("ERROR!!! -- All decode context buffers are in use."));
                ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto err_allocate_deccontext;                                                
            }

            sBufInfo = pDevice->memInfo.sDwnldMemInfo.sVideoDecCodecBufInfo[ui32FwCtxIndx];
            codePtr = sBufInfo.ui32BaseAddr;
            ui32SizeAvail = sBufInfo.ui32Size;
            BDBG_MSG(("Video ui32CodePtr = %x,ui32EndAddr=%x,ui32Size =%d",
                            codePtr,(codePtr+ui32SizeAvail),ui32SizeAvail));


            /*Download all decode Code+Table for supported algorithm*/  
            for(i = 0; i < sExecID.NumNodes; i++)
            {
                /*Download decode Code */
                eCodeExecId = sExecID.eAlgoIds[i];
                if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                   
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

                if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
                {
                    BDBG_MSG(("Invalid Image ID for decode code of algorithm %#x Stage %d",vidType,i));
                    continue;
                }

                ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

                if(ui32CodeSize > ui32SizeAvail)
                {
                    BDBG_ERR(("Size available (%d) to download decode code (IMG ID = %#x) of video Algorithm (%#x)"
                                        " is less than required (%d)",  
                                        ui32SizeAvail,eCodeFirmwareId,vidType,ui32CodeSize
                                        ));
                    ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto err_deccontext_size;                                                                        
                }
                /* Code is already downloaded, just update the MIT entries */
                if(true == bDownload)
                {                           
                    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);   

                    BDBG_MSG(("Downloading decode code (IMG ID = %#x) of Video Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eCodeFirmwareId,vidType,codePtr,ui32physAddress,ui32CodeSize));
                    ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);  
                    if(ret != BERR_SUCCESS)
                    {
                        ret = BERR_TRACE(ret);
                        goto err_deccontext_size;
                    }                        
                }

                ui32SizeAvail-= ui32CodeSize;

                tblPtr = codePtr + ui32CodeSize;

                /*Download decode table */
                eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

                if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
                {
                    BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,0,0);   
                    codePtr += ui32CodeSize;                                          
                    BDBG_MSG(("Invalid Image ID for video decode table of algorithm %#x Stage %d",vidType,i));
                    continue;
                }

                bDownloaded = false;
                BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eVideoDecode,eCodeExecId,&bDownloaded,&eTempExecId);

                if(!bDownloaded)
                {
                    tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                    if( tblSize > ui32SizeAvail)
                    {
                        BDBG_ERR(("Size available (%d) to download decode table (IMG ID = %#x) of video Algorithm (%#x)"
                                        "is less than required (%d)",
                                        ui32SizeAvail,eTblFirmwareId,vidType,tblSize
                                        ));
                        ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto err_deccontext_size;                                                                                                    
                    }

                    /* Table is already downloaded, just update the MIT entries */
                    if(true == bDownload)                    
                    {    
                        BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);    
                        BDBG_MSG(("Downloading video decode table (IMG ID = %#x) of video Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eTblFirmwareId,vidType,tblPtr,ui32physAddress,tblSize));
                        ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);    
                        if(ret != BERR_SUCCESS)
                        {
                            ret = BERR_TRACE(ret);
                            goto err_deccontext_size;
                        }                         
                    }
                    /* updating codePtr variable, used to track the used size */        
                    ui32SizeAvail-=tblSize;
                }
                else
                {
                    tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                    tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;
                    BDBG_ERR(("decode Table (IMG ID = %#x) of video Algorithm (%#x) is already downloaded at address 0x%x, size = 0x%x",
                                    eTblFirmwareId,vidType,tblPtr,tblSize));
                }
                BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);    
                if(false == bDownloaded)
                {
                    codePtr += ui32CodeSize + tblSize;
                }
                else
                {
                    codePtr += ui32CodeSize;                
                }
            }/*for*/

            ui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
            codePtr = 0;
            ui32CodeSize = 0;
            ui32SizeAvail = 0;
            bDownload = 0;
        }
    }
	else if(BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType)
	{
        if(pSettings->pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eVideoEncode)
        {
            vidEncType = pSettings->pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoEncAlgo;

            if(!BDSP_Raaga_P_IsVideoEncodeTypeSupported((uint32_t)vidEncType))
            {
                BDBG_ERR(("Video Encode type %#x is not supported for this platform",vidEncType));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
				/* Nothing to free here just exit */
                goto err_audiotype_notsupported;
            }

            BDSP_Raaga_P_GetVideoEncodeExecIds(BDSP_EncodeMode_eRealtime,vidEncType,&sExecID);

            BDSP_Raaga_P_AllocateFwCtxtBuf((void *)pContextHandle, vidEncType, &ui32FwCtxIndx,&bDownload, BDSP_Raaga_P_ExecType_eVideoEncode);

            /* If its Watchdog recovery, And its exec is not already in the DRAM, 
           		 then this is a BAD DEVICE STATE */
            BDBG_MSG(("ui32FwCtxIndx = %d max buffer = %d",ui32FwCtxIndx,
                            pRaagaContext->settings.videoEncodeDownloadBuffer.maxNumVideoEncodeDownloadBuffer));
            /* If no Free Buf to download algo code */
            if((BDSP_RAAGA_INVALID_INDEX == ui32FwCtxIndx)
                || ui32FwCtxIndx >= pRaagaContext->settings.videoEncodeDownloadBuffer.maxNumVideoEncodeDownloadBuffer)
            {
                BDBG_ERR(("ERROR!!! -- All Encode context buffers are in use."));
                ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto err_allocate_deccontext;                                                
            }

            sBufInfo = pDevice->memInfo.sDwnldMemInfo.sVideoEncodeCodecBufInfo[ui32FwCtxIndx];
            codePtr = sBufInfo.ui32BaseAddr;
            ui32SizeAvail = sBufInfo.ui32Size;
            BDBG_MSG(("Video ui32CodePtr = %x,ui32EndAddr=%x,ui32Size =%d",
                            codePtr,(codePtr+ui32SizeAvail),ui32SizeAvail));


            /*Download all decode Code+Table for supported algorithm*/  
            for(i = 0; i < sExecID.NumNodes; i++)
            {
                /*Download decode Code */
                eCodeExecId = sExecID.eAlgoIds[i];
                if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                   
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

                if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
                {
                    BDBG_MSG(("Invalid Image ID for Encode code of algorithm %#x Stage %d", vidEncType, i));
                    continue;
                }

                ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

                if(ui32CodeSize > ui32SizeAvail)
                {
                    BDBG_ERR(("Size available (%d) to download encode code (IMG ID = %#x) of video Algorithm (%#x)"
                                        " is less than required (%d)",  
                                        ui32SizeAvail,eCodeFirmwareId, vidEncType, ui32CodeSize
                                        ));
                    ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto err_deccontext_size;                                                                        
                }
                /* Code is already downloaded, just update the MIT entries */
                if(true == bDownload)
                {                           
                    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);   

                    BDBG_MSG(("Downloading Encode code (IMG ID = %#x) of Video Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eCodeFirmwareId, vidEncType, codePtr, ui32physAddress, ui32CodeSize));
                    ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);  
                    if(ret != BERR_SUCCESS)
                    {
                        ret = BERR_TRACE(ret);
                        goto err_deccontext_size;
                    }                        
                }

                ui32SizeAvail-= ui32CodeSize;

                tblPtr = codePtr + ui32CodeSize;

                /*Download decode table */
                eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

                if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
                {
                    BDSP_Raaga_P_UpdateMitEntry((void*)pDevice, eCodeExecId, codePtr, ui32CodeSize,0,0);   
                    codePtr += ui32CodeSize;                                          
                    BDBG_MSG(("Invalid Image ID for video Encode table of algorithm %#x Stage %d",vidEncType,i));
                    continue;
                }

                bDownloaded = false;
                BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eVideoEncode,eCodeExecId,&bDownloaded,&eTempExecId);

                if(!bDownloaded)
                {
                    tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                    if( tblSize > ui32SizeAvail)
                    {
                        BDBG_ERR(("Size available (%d) to download decode table (IMG ID = %#x) of video Algorithm (%#x)"
                                        "is less than required (%d)",
                                        ui32SizeAvail, eTblFirmwareId, vidEncType, tblSize
                                        ));
                        ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto err_deccontext_size;                                                                                                    
                    }

                    /* Table is already downloaded, just update the MIT entries */
                    if(true == bDownload)                    
                    {    
                        BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);    
                        BDBG_MSG(("Downloading video Encode table (IMG ID = %#x) of video Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eTblFirmwareId, vidEncType, tblPtr, ui32physAddress, tblSize));
                        ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);    
                        if(ret != BERR_SUCCESS)
                        {
                            ret = BERR_TRACE(ret);
                            goto err_deccontext_size;
                        }                         
                    }
                    /* updating codePtr variable, used to track the used size */        
                    ui32SizeAvail-=tblSize;
                }
                else
                {
                    tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                    tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;
                    BDBG_ERR(("decode Table (IMG ID = %#x) of video Algorithm (%#x) is already downloaded at address 0x%x, size = 0x%x",
                                    eTblFirmwareId,vidType,tblPtr,tblSize));
                }
                BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);    
                if(false == bDownloaded)
                {
                    codePtr += ui32CodeSize + tblSize;
                }
                else
                {
                    codePtr += ui32CodeSize;                
                }
            }/*for*/

            ui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
            codePtr = 0;
            ui32CodeSize = 0;
            ui32SizeAvail = 0;
            bDownload = 0;
        }
    }
    else if(pRaagaContext->settings.contextType == BDSP_ContextType_eGraphics)
    {
    /*TBD*/
    }
    else if(pRaagaContext->settings.contextType == BDSP_ContextType_eSecurity)
    {
    /*TBD*/
    }
	else if(BDSP_ContextType_eScm == pRaagaContext->settings.contextType)
	{
        if(pSettings->pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eScm)
        {
            scmType = pSettings->pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eScmAlgo; 

            if(!BDSP_Raaga_P_IsScmTypeSupported((uint32_t)scmType))
            {
                BDBG_ERR(("SCM type %#x is not supported for this platform",scmType));
                ret = BERR_TRACE(BERR_NOT_SUPPORTED);
				/* Nothing to free here just exit */
                goto err_audiotype_notsupported; 
            }

            BDSP_Raaga_P_GetScmExecIds(scmType,&sExecID);

            BDSP_Raaga_P_AllocateFwCtxtBuf((void *)pContextHandle, scmType, &ui32FwCtxIndx,&bDownload, BDSP_Raaga_P_ExecType_eScm);

            BDBG_MSG(("ui32FwCtxIndx = %d max buffer = %d",ui32FwCtxIndx,
                            pRaagaContext->settings.scmDownloadBuffer.maxNumScmDecodeDownloadBuffer));
            /* If no Free Buf to download algo code */
            if((BDSP_RAAGA_INVALID_INDEX == ui32FwCtxIndx)
                || ui32FwCtxIndx >= pRaagaContext->settings.scmDownloadBuffer.maxNumScmDecodeDownloadBuffer)
            {
                BDBG_ERR(("ERROR!!! -- All SCM context buffers are in use."));
                ret = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                goto err_allocate_deccontext;                                               
            }

            sBufInfo = pDevice->memInfo.sDwnldMemInfo.sScmCodecBufInfo[ui32FwCtxIndx];
            codePtr = sBufInfo.ui32BaseAddr;
            ui32SizeAvail = sBufInfo.ui32Size;
            BDBG_MSG(("SCM ui32CodePtr = %x,ui32EndAddr=%x,ui32Size =%d",
                            codePtr,(codePtr+ui32SizeAvail),ui32SizeAvail));


            /*Download all decode Code+Table for supported algorithm*/  
            for(i = 0; i < sExecID.NumNodes; i++)
            {
                /*Download decode Code */
                eCodeExecId = sExecID.eAlgoIds[i];
                if(eCodeExecId == BDSP_AF_P_AlgoId_eInvalid)
                {
                    continue;
                }                   
                eCodeFirmwareId = pDevice->imgIdMappingArrays.codeImgId[eCodeExecId];

                if(BDSP_Img_Id_eInvalid == eCodeFirmwareId)
                {
                    BDBG_MSG(("Invalid Image ID for Encode code of algorithm %#x Stage %d", scmType, i));
                    continue;
                }

                ui32CodeSize = pDevice->imgIdMappingArrays.ui32CodeSize[eCodeExecId];

                if(ui32CodeSize > ui32SizeAvail)
                {
                    BDBG_ERR(("Size available (%d) to download SCM code (IMG ID = %#x) of SCM Algorithm (%#x)"
                                        " is less than required (%d)",  
                                        ui32SizeAvail,eCodeFirmwareId, scmType, ui32CodeSize
                                        ));
                    ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                    goto err_deccontext_size;                                                                     
                }
                /* Code is already downloaded, just update the MIT entries */
                if(true == bDownload)
                {                           
                    BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(codePtr), &ui32physAddress);   

                    BDBG_MSG(("Downloading SCM code (IMG ID = %#x) of SCM Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eCodeFirmwareId, scmType, codePtr, ui32physAddress, ui32CodeSize));
                    ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,codePtr,eCodeFirmwareId,pDevice->memHandle);  
                    if(ret != BERR_SUCCESS)
                    {
                        ret = BERR_TRACE(ret);
                        goto err_deccontext_size; 
                    }                        
                }

                ui32SizeAvail-= ui32CodeSize;

                tblPtr = codePtr + ui32CodeSize;

                /*Download decode table */
                eTblFirmwareId = pDevice->imgIdMappingArrays.tableImgId[eCodeExecId];

                if(BDSP_Img_Id_eInvalid == eTblFirmwareId)
                {
                    BDSP_Raaga_P_UpdateMitEntry((void*)pDevice, eCodeExecId, codePtr, ui32CodeSize,0,0);   
                    codePtr += ui32CodeSize;                                          
                    BDBG_MSG(("Invalid Image ID for SCM table of algorithm %#x Stage %d",scmType,i));
                    continue;
                }

                bDownloaded = false;
                BDSP_Raaga_P_IsTableDownloaded(BDSP_Raaga_P_ExecType_eScm,eCodeExecId,&bDownloaded,&eTempExecId);

                if(!bDownloaded)
                {
                    tblSize = pDevice->imgIdMappingArrays.tableSize[eCodeExecId];

                    if( tblSize > ui32SizeAvail)
                    {
                        BDBG_ERR(("Size available (%d) to download decode table (IMG ID = %#x) of SCM Algorithm (%#x)"
                                        "is less than required (%d)",
                                        ui32SizeAvail, eTblFirmwareId, scmType, tblSize
                                        ));
                        ret =  BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
                        goto err_deccontext_size;                                                                                                    
                    }

                    /* Table is already downloaded, just update the MIT entries */
                    if(true == bDownload)                    
                    {    
                        BMEM_ConvertAddressToOffset(pDevice->memHandle, (void *)(tblPtr), &ui32physAddress);    
                        BDBG_MSG(("Downloading SCM table (IMG ID = %#x) of video Algorithm (%#x) at address %#x (Physical = %#x), size = 0x%x",
                                        eTblFirmwareId, scmType, tblPtr, ui32physAddress, tblSize));
                        ret = BDSP_Raaga_P_CopyFWImageToMem(iface,pImgContext,tblPtr,eTblFirmwareId,pDevice->memHandle);    
                        if(ret != BERR_SUCCESS)
                        {
                            ret = BERR_TRACE(ret);
                            goto err_deccontext_size;
                        }                         
                    }
                    /* updating codePtr variable, used to track the used size */        
                    ui32SizeAvail-=tblSize;
                }
                else
                {
                    tblPtr = pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataPtr;
                    tblSize= pDevice->mit.sAlgoDwnldDetail[eTempExecId].ui32AlgoDataSize;
                    BDBG_ERR(("decode Table (IMG ID = %#x) of SCM Algorithm (%#x) is already downloaded at address 0x%x, size = 0x%x",
                                    eTblFirmwareId,vidType,tblPtr,tblSize));
                }
                BDSP_Raaga_P_UpdateMitEntry((void*)pDevice,eCodeExecId,codePtr,ui32CodeSize,tblPtr,tblSize);    
                if(false == bDownloaded)
                {
                    codePtr += ui32CodeSize + tblSize;
                }
                else
                {
                    codePtr += ui32CodeSize;                
                }
            }/*for*/

            ui32FwCtxIndx = BDSP_RAAGA_INVALID_INDEX;
            codePtr = 0;
            ui32CodeSize = 0;
            ui32SizeAvail = 0;
            bDownload = 0;
        }
    }

    goto end;

    err_encodecontext_size:
    if(BDSP_EncodeMode_eInvalid != encMode)
    {
        BDSP_Raaga_P_FreeFwCtxtBuf((void *)pContextHandle, encType,BDSP_Raaga_P_ExecType_eEncode);
    }    
    err_passthrucontext_size:
    if(BDSP_DecodeMode_ePassThru== decMode)
    {    
        if (passthruType == BDSP_PassThruType_eMlp)
		{
			BDSP_Raaga_P_FreeFwCtxtBuf((void *)pContextHandle,BDSP_AudioProcessing_eMlpPassThru,BDSP_Raaga_P_ExecType_ePassthru);
		}
		else if (passthruType == BDSP_PassThruType_eGeneric)
		{
        	BDSP_Raaga_P_FreeFwCtxtBuf((void *)pContextHandle,BDSP_AudioProcessing_eGenericPassThru,BDSP_Raaga_P_ExecType_ePassthru);
    	}
    }

    err_deccontext_size:
    if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)    
    {
        if(BDSP_DecodeMode_eDecode== decMode)
        {
            BDSP_Raaga_P_FreeFwCtxtBuf((void *)pContextHandle, audType,BDSP_Raaga_P_ExecType_eDecode);
        }
    }
    else if(pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)    
    {
        BDSP_Raaga_P_FreeFwCtxtBuf((void *)pContextHandle, vidType,BDSP_Raaga_P_ExecType_eVideoDecode);
    }
	else if(pRaagaContext->settings.contextType == BDSP_ContextType_eScm)    
    {
        BDSP_Raaga_P_FreeFwCtxtBuf((void *)pContextHandle, scmType,BDSP_Raaga_P_ExecType_eScm);
    }
    else if(pRaagaContext->settings.contextType == BDSP_ContextType_eVideoEncode)    
    {
        BDSP_Raaga_P_FreeFwCtxtBuf((void *)pContextHandle, vidEncType,BDSP_Raaga_P_ExecType_eVideoEncode);
    }
	
    err_allocate_deccontext:  
    err_allocate_passthrucontext:   
    err_fscontext_size:
    BDSP_Raaga_P_FreeFwCtxtBuf((void *)pContextHandle, audType,BDSP_Raaga_P_ExecType_eFrameSync);

    err_allocate_fscontext:
    err_audiotype_notsupported:
    err_encodetype_notsupported:
	err_passthrutype_notsupported:
    end:
    BDBG_LEAVE(BDSP_Raaga_P_DownloadStartTimeFWExec);

    return ret;
}

BERR_Code BDSP_Raaga_P_FreeStartTimeFWCtxt(
    void *pTaskHandle    
)
{
    BDSP_AudioType audType = BDSP_AudioType_eMax;
    BDSP_VideoType vidType = BDSP_VideoType_eMax;    
    bool	bOpenTimeAudProcDownload = true;
    BDSP_DecodeMode decMode = BDSP_DecodeMode_eMax;
    BDSP_AudioEncode    encType = BDSP_AudioEncode_eMax;    
    BDSP_AudioProcessing    procType = BDSP_AudioProcessing_eMax;    
    BDSP_RaagaTask  *pRaagaTask = (BDSP_RaagaTask *)pTaskHandle;    
    BDSP_RaagaContext *pRaagaContext = (BDSP_RaagaContext *)pRaagaTask->pContext;
	BDSP_AF_P_sALGO_EXEC_INFO sExecID;
	BDSP_AudioProcessing  passthruType = BDSP_AudioProcessing_eInvalid;
    uint32_t i = 0,j = 0;

    BDBG_OBJECT_ASSERT(pRaagaContext, BDSP_RaagaContext);

    if(pRaagaContext->settings.contextType == BDSP_ContextType_eAudio)
    {
        if(pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eDecode)
        {
            decMode = pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAudioMode.eDecAudioMode;
            audType = pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eDecAudioAlgo;
            
			BDSP_Raaga_P_GetExecIds(decMode,audType,&sExecID);
			if (sExecID.eAlgoIds[1] == BDSP_AF_P_AlgoId_ePassThru)
				passthruType = BDSP_AudioProcessing_eGenericPassThru;
			else if (sExecID.eAlgoIds[1] == BDSP_AF_P_AlgoId_eMLPPassThru)
				passthruType = BDSP_AudioProcessing_eMlpPassThru;
            
            BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, audType, BDSP_Raaga_P_ExecType_eFrameSync);  
            if(BDSP_DecodeMode_eDecode == decMode)
            {
                BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, audType, BDSP_Raaga_P_ExecType_eDecode);          
            }
            else if(BDSP_DecodeMode_ePassThru == decMode)
            {
                BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, passthruType, BDSP_Raaga_P_ExecType_ePassthru);
            }             
        }

        for(i = 0; i < pRaagaTask->settings.numBranches ; i++)
        {
            for(j = 0; j < pRaagaTask->settings.pBranchInfo[i]->ui32NumStages; j++)
            {    
                if(pRaagaTask->settings.pBranchInfo[i]->sFwStgInfo[j].eStageType == BDSP_CIT_P_StageType_eEncode)
                {
                    encType = pRaagaTask->settings.pBranchInfo[i]->sFwStgInfo[j].uAlgorithm.eEncAudioAlgo;
                    BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, encType, BDSP_Raaga_P_ExecType_eEncode);                                
                }
            }
        }

        BDSP_Raaga_P_IfOpenTimeProcDownload(&bOpenTimeAudProcDownload);

        if(BDSP_DecodeMode_eDecode == decMode)
        {
            for(i = 0; i < pRaagaTask->settings.numBranches ; i++)
            {
                for(j = 0; j < pRaagaTask->settings.pBranchInfo[i]->ui32NumStages; j++)
                {    
                    if(pRaagaTask->settings.pBranchInfo[i]->sFwStgInfo[j].eStageType == BDSP_CIT_P_StageType_ePostProc)
                    {
                        procType = pRaagaTask->settings.pBranchInfo[i]->sFwStgInfo[j].uAlgorithm.eProcAudioAlgo;
                        BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, procType, BDSP_Raaga_P_ExecType_eProcessingAlgo);
                    }
                }
            }   
        }
    }
    else if(pRaagaContext->settings.contextType == BDSP_ContextType_eVideo)
    {
        if(pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eVideoDecode)
        {
            decMode = BDSP_DecodeMode_eDecode;
            vidType = pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoAlgo;
            
            BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, vidType, BDSP_Raaga_P_ExecType_eFrameSync);  
            if(BDSP_DecodeMode_eDecode == decMode)
            {
                BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, vidType, BDSP_Raaga_P_ExecType_eDecode);          
            }
        }        
    }
    else if(BDSP_ContextType_eVideoEncode == pRaagaContext->settings.contextType )
    {
        if(pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eVideoEncode)
        {
            BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eVideoEncAlgo, BDSP_Raaga_P_ExecType_eVideoEncode);
        }        
    }
    else if(pRaagaContext->settings.contextType == BDSP_ContextType_eGraphics)
    {
        /*TBD*/
    }
    else if(pRaagaContext->settings.contextType == BDSP_ContextType_eSecurity)
    {
        /*TBD*/
    }      
	else if(BDSP_ContextType_eScm == pRaagaContext->settings.contextType )
    {
        if(pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].eStageType == BDSP_CIT_P_StageType_eScm)
        {
            BDSP_Raaga_P_FreeFwCtxtBuf(pRaagaContext, pRaagaTask->settings.pBranchInfo[0]->sFwStgInfo[0].uAlgorithm.eScmAlgo, BDSP_Raaga_P_ExecType_eScm);
        }        
    }
    return BERR_SUCCESS;
}


