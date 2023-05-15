/***************************************************************************
 *     Copyright (c) 2002-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: mvd_test.c $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 7/10/09 5:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/portinginterface/vdc/mvd_test.c $
 * 
 * Hydra_Software_Devel/5   7/10/09 5:55p rpan
 * PR55188: Convert scanf() to BTST_P_Scanf().
 * 
 * Hydra_Software_Devel/4   5/18/09 11:49a tdo
 * PR53239: Remove usage of private defintions in vdc unittest.
 *
 * Hydra_Software_Devel/3   10/16/07 2:46p rpan
 * PR34857: First version after merging the latest xvd_vdc_test and
 * mvd_vdc_test as of Oct. 16, 2007, 12pm.
 *
 *
 * Hydra_Software_Devel/1   9/27/07 4:13p rpan
 * PR34857: Copied over as is from mvd_vdc_test.
 *
 ***************************************************************************/

#include "stdio.h"
#include "string.h"
#include <stdlib.h>

#include "mvd_test.h"
#if 0
#include "itb_dump.h"
#include "cdb_dump.h"
#endif
#include <sys/stat.h>
#include <sys/resource.h>
/* base modules */
#include "bstd.h"           /* standard types */
#include "berr.h"           /* error code */
#include "breg_mem.h"       /* register access */
#include "bmem.h"           /* memory heap */
#include "bkni.h"           /* kernel interface */
#include "btst_kni.h"       /* to handle task */

/* interrupt manager */
#include "bint.h"
#include "bint_plat.h"      /* platform specific */


#include "systeminit.h"
#include "framework.h"
#include "bmem_debug.h"
#include "bchp_mtp_1.h"

#include "bmvd_mtpmv_proc_reg.h"
#include "bchp_sun_top_ctrl.h"
#include "bchp_sun_gisb_arb.h"
#include "bchp_bvnf_intr2_4.h"

#include "bavc.h"

#include "bchp_xpt_dpcr0.h"
#include "bchp_xpt_dpcr1.h"
#include "bmvd_userdata.h"
#include "bxpt.h"
#include "bxpt_rave.h"
#include "bxpt_rsbuf.h"
#include "bxpt_pcr.h"
#include "bxpt_pcr_offset.h"
#include "bchp_xpt_bus_if.h"




#define USER_DATA_PACKET_SIZE	1024
#define TOTAL_USER_DATA_BUFFER_SIZE	10240


/*
 * global handles
 */
BCHP_Handle g_hChip;
BREG_Handle g_hRegister;
BMEM_Handle g_hMemory;
BINT_Handle g_hInterrupt;
BTMR_Handle g_hTmr;
/*
static BSystem_Info         sysInfo;
*/
static bool s_bInited = false;

static BMVD_Handle         g_hMvd ;
static BMVD_ChannelHandle  g_hMvdCh_0 = NULL ;
BKNI_EventHandle		g_hUserDataEvent;
BMVD_USERDATA_Handle	g_hUserData;

BKNI_EventHandle		 g_hBufferReadyEvent,g_TaskComplete;
BTST_TaskHandle         hTaskBuffReady= NULL;

static BAVC_MVD_Field	   MVD_to_VDC_info;

BRDC_Handle     g_hRdc;

static BVDC_Window_Handle  s_hMpegWindow0;


static BVDC_Handle         s_hVdc;
static BFMT_VideoInfo      s_stVideoFmtInfo;
bool            g_bInterceptFrame;
bool			g_ProcessUserData = true ;

uint32_t        g_uiLumaBufSize, g_uiChromaBufSize;
void           *g_pvLumaStartAddress, *g_pvChromaStartAddress;
static BVDC_AspectRatioMode s_eARCmode = BVDC_AspectRatioMode_eBypass;

typedef void (*pFnTESTRecord)(BTST_TaskHandle task, void* data);
BERR_Code FrameDumpFn(void );

#define BYTES_MB  256
BAVC_YCbCrType ChromaType ;



BDBG_MODULE(mvd_test);
#ifdef MVD_TEST
int fnDramImgToPPM (int addr_luma_start,
					int addr_chroma_start,
					int horiz_size,
					int vert_size,
					int NMBx,
					int picstruct,
					int fieldtype,
					int nImageFileFormat,
					FILE *fpPPM,
					FILE *fpY,
					FILE *fpU,
					FILE *fpV,
					BMEM_Handle MemHandle
					);
#endif
int SimCaptureFrames(
	BMEM_Handle MemHandle,
	char *pImageFileName,
	int nImageFileFormat,
	int luma_start,
	int chroma_start,
	int width,
	int height,
	int nmbx,
	int picstruct,
	int fieldtype,
	int eChromaType
	) ;

void fnYUV2PPM(FILE *fp, unsigned char Y0, unsigned char Y1, unsigned char U, unsigned char V);

void TEST_UserData_isr(void *pParm1, int parm2, void *pMVD_data)
{
	BSTD_UNUSED(pParm1) ;
	BSTD_UNUSED(parm2) ;
	BSTD_UNUSED(pMVD_data) ;
	BKNI_SetEvent( g_hUserDataEvent ) ;

}


void TEST_UserDataCapture(BTST_TaskHandle task, void *data)
{
	BMVD_USERDATA_info stUserDataInfo ;
	unsigned int i;
	BERR_Code result ;
	FILE *fpUserData = fopen("UDOut.dat", "w");
	if(fpUserData)
		printf("file has opened\n");
	else
	{
		printf("File is not opend\n");
		exit(1);
	}
	BSTD_UNUSED(task);
	BSTD_UNUSED(data);
	printf("In TEST_UserDataCapture ");
	g_ProcessUserData = true ;
	while (g_ProcessUserData)
	{
		/* Sleep Forever */
		result = BKNI_WaitForEvent(g_hUserDataEvent, BKNI_INFINITE);
		if (!result)
		{
			while( BMVD_USERDATA_Read(g_hUserData, &stUserDataInfo) != BMVD_USERDATA_ERR_BUFFER_EMPTY)
			{
				BDBG_MSG(("Got User Data"));
				fprintf(fpUserData, "%s", stUserDataInfo.bErrorBufferOverflow ? "Error !\n" : "") ;
				fprintf(fpUserData, "%s", (stUserDataInfo.eUserDataType == BAVC_USERDATA_Type_eSeq) ? "--- user data for Sequence ---\n" :
											(stUserDataInfo.eUserDataType == BAVC_USERDATA_Type_eGOP) ? "--- user data for GOP ---\n" : "") ;
				if(stUserDataInfo.eUserDataType == BAVC_USERDATA_Type_ePicture)
				{
					fprintf(fpUserData, "--- user data for Picture. Coding=%c(%d) ---\n",
							(stUserDataInfo.ePicCodingType==1)?'I':(stUserDataInfo.ePicCodingType==2)?'P':(stUserDataInfo.ePicCodingType==3)?'B':'D',
							stUserDataInfo.ePicCodingType) ;
				}

				for(i=0; i<(stUserDataInfo.ui32UserDataBufSize/4); i++)
				{
					uint32_t data = ((uint32_t *)stUserDataInfo.pUserDataBuffer)[i] ;
					fprintf(fpUserData,"%02x%02x%02x%02x\n",(int) (data&0xFF), (int)((data>>8)&0xFF), (int)((data>>16)&0xFF),(int)((data>>24)&0xFF)) ;
				}
				BMVD_USERDATA_FlagReadDone(g_hUserData) ;
			}
			/* Save into a file */
		}
		BKNI_Sleep(1);
	}
	fclose(fpUserData);
}

void bsettop_p_dec_buf_ready_isr(void *pParm1, int parm2, void *mvd_vdc_struct)
{
	BAVC_MVD_Field	   *pMVD_to_VDC_info;

	BSTD_UNUSED(pParm1) ;
	BSTD_UNUSED(parm2) ;
	printf("Direct Feed Buffer Ready ISR\n");
	pMVD_to_VDC_info = (BAVC_MVD_Field*)mvd_vdc_struct;
	MVD_to_VDC_info = *(pMVD_to_VDC_info) ;

	BKNI_SetEvent( g_hBufferReadyEvent ) ;
	fprintf(stderr, "*");
	fprintf(stderr, "Got DecodeBufferReady Interrupt\n");
}

static void bsettop_p_dec_buf_ready(BTST_TaskHandle task, void *data)
{

	uint32_t ui32LuminanceFrameBufferAddress, ui32ChrominanceFrameBufferAddress ;
	BERR_Code result;
	FILE *fpPPM ;


	BSTD_UNUSED(task);
	BSTD_UNUSED(data);

	/*printf("waiting for set event: g_hBufferReadyEvent\n");*/

	for(;;)
	{

	/* Sleep Forever */
	result = BKNI_WaitForEvent(g_hBufferReadyEvent, BKNI_INFINITE);


	if(!(fpPPM = fopen("image.ppm", "w")))
	{
		printf("Unable to open image file %s", "image.ppm");
	}


	BDBG_ERR(("Decoded Buffer Ready callback : Uploading image"));

	BMEM_ConvertAddressToOffset(g_hMemory,MVD_to_VDC_info.pLuminanceFrameBufferAddress,&ui32LuminanceFrameBufferAddress);
	BMEM_ConvertAddressToOffset(g_hMemory,MVD_to_VDC_info.pChrominanceFrameBufferAddress,&ui32ChrominanceFrameBufferAddress);

	fprintf(fpPPM, "P6\n%d %d\n255\n", MVD_to_VDC_info.ulSourceHorizontalSize, MVD_to_VDC_info.ulSourceVerticalSize);

	fclose(fpPPM);

	fpPPM = fopen("image.ppm", "ab") ;

#ifdef MVD_TEST
	BDBG_ERR(("Decoded Buffer Ready callback : Uploading image6"));
	fnDramImgToPPM (ui32LuminanceFrameBufferAddress,
					ui32ChrominanceFrameBufferAddress,
					MVD_to_VDC_info.ulSourceHorizontalSize,
					MVD_to_VDC_info.ulSourceVerticalSize,
					(MVD_to_VDC_info.ulRowStride>>8),
					1,
					0,
					0,
					fpPPM,
					NULL,
					NULL,
					NULL,
					g_hMemory
					);
#endif
	BDBG_ERR(("Done : Uploading image"));
	BKNI_SetEvent( g_TaskComplete ) ;
	}
	return;
}



/***************************************************************************
 * intercepts the callback between MVD and VDC (for debugging only)
 *
 */
void MpegDataReady_isr(
	void                            *pvSourceHandle,
	int                              iParm2,
	void                            *pvFieldData
	)
{
	BAVC_MVD_Field *pFieldData = (BAVC_MVD_Field *)pvFieldData;

	static uint32_t uiCnt = 0;
	uint32_t        uiAvcFieldSize = sizeof(BAVC_MVD_Field);
	uint32_t        bytesPerRow = (( pFieldData->ulSourceHorizontalSize + 15 ) >> 4) << 8;/* a row contains one frame's row */
	uint32_t        totalRows = (pFieldData->ulSourceVerticalSize + 15) >> 4;
	uint32_t        uiLumaBufSize = pFieldData->ulRowStride * (totalRows - 1) + bytesPerRow;/* interleaved frames buffer */
	uint32_t        uiChromaBufSize = pFieldData->ulRowStride * ((totalRows + 1) / 2 - 1) + bytesPerRow;/* interleaved frames buffer */

	/* if requested => intercept a frame to be dumped later on when exiting */
	if(g_bInterceptFrame)
	{
		BDBG_MSG(("Source Width  = %d\n", pFieldData->ulSourceHorizontalSize));
		BDBG_MSG(("Source Height = %d\n", pFieldData->ulSourceVerticalSize));
		BDBG_MSG(("size of AVC field data structure = %d\n", uiAvcFieldSize));
		BDBG_MSG(("Calculated luma buffer size = %d\n", uiLumaBufSize));
		BDBG_MSG(("Calculated chroma buffer size = %d\n", uiChromaBufSize));
		BDBG_ASSERT(((uiLumaBufSize + uiAvcFieldSize) < g_uiLumaBufSize));
		BDBG_ASSERT((uiChromaBufSize < g_uiChromaBufSize));
		if(++uiCnt == 1)
		{/* only capture one frame */
			BKNI_Memcpy(g_pvLumaStartAddress, pFieldData, uiAvcFieldSize);
			BKNI_Memcpy(
				(void*)((uint32_t)g_pvLumaStartAddress + uiAvcFieldSize),
				pFieldData->pLuminanceFrameBufferAddress,
				uiLumaBufSize);
			BKNI_Memcpy(
				g_pvChromaStartAddress,
				pFieldData->pChrominanceFrameBufferAddress,
				uiChromaBufSize);
		}
	}

	/* user-defined ARC */
	switch(s_eARCmode)
	{
	case BVDC_AspectRatioMode_eUseAllSource:
		/* Use all source     : only modify the destination. */
		if((BFMT_AspectRatio_e4_3  == pFieldData->eAspectRatio) &&
		   (BFMT_AspectRatio_e16_9 == s_stVideoFmtInfo.eAspectRatio))
		{
			BVDC_Window_SetDstRect_isr( s_hMpegWindow0,
				s_stVideoFmtInfo.ulWidth * 1 / 8,
				0,
                s_stVideoFmtInfo.ulWidth * 3 / 4,
				s_stVideoFmtInfo.ulHeight);
			BVDC_Window_SetScalerOutput_isr( s_hMpegWindow0,
				0, 0,
				s_stVideoFmtInfo.ulWidth * 3 / 4,
				s_stVideoFmtInfo.ulHeight);
		}
		else if((BFMT_AspectRatio_e16_9 == pFieldData->eAspectRatio) &&
		        (BFMT_AspectRatio_e4_3  == s_stVideoFmtInfo.eAspectRatio))
		{
			BVDC_Window_SetDstRect_isr( s_hMpegWindow0,
				0,
				s_stVideoFmtInfo.ulHeight * 1 / 8,
				s_stVideoFmtInfo.ulWidth,
				s_stVideoFmtInfo.ulHeight      * 3 / 4);
			BVDC_Window_SetScalerOutput_isr( s_hMpegWindow0,
				0, 0,
				s_stVideoFmtInfo.ulWidth,
				s_stVideoFmtInfo.ulHeight * 3 / 4);
		}

		/* clear source clipping */
		BVDC_Window_SetSrcClip_isr( s_hMpegWindow0,
			0, 0,
			0, 0);
		break;
	case BVDC_AspectRatioMode_eUseAllDestination:
		/* Use all dest     : only modify the source. */
		if((BFMT_AspectRatio_e4_3  == pFieldData->eAspectRatio) &&
			(BFMT_AspectRatio_e16_9 == s_stVideoFmtInfo.eAspectRatio))
		{
			/* remove 1/8 top/bottom */
			BVDC_Window_SetSrcClip_isr( s_hMpegWindow0,
				0, 0,
				pFieldData->ulSourceVerticalSize * 1/8,
				pFieldData->ulSourceVerticalSize * 1/8);
		}
		else if((BFMT_AspectRatio_e16_9 == pFieldData->eAspectRatio) &&
			    (BFMT_AspectRatio_e4_3  == s_stVideoFmtInfo.eAspectRatio))
		{
			/* remove 1/8 left/right. */
			BVDC_Window_SetSrcClip_isr( s_hMpegWindow0,
				pFieldData->ulSourceHorizontalSize * 1/8,
				pFieldData->ulSourceHorizontalSize * 1/8,
				0, 0);
		}

		/* restore original scaler output and dest rectangles. */
		BVDC_Window_SetDstRect_isr( s_hMpegWindow0,
			0,
			0,
            s_stVideoFmtInfo.ulWidth,
			s_stVideoFmtInfo.ulHeight);
		BVDC_Window_SetScalerOutput_isr( s_hMpegWindow0,
			0, 0,
			s_stVideoFmtInfo.ulWidth,
			s_stVideoFmtInfo.ulHeight);
		break;
	case BVDC_AspectRatioMode_eBypass:
		/* The user-defined ARC mode should be
		   differentiated from Bypass mode;
		   temporary workaround: set illegal rectangles to avoid user thread
		   rectangles settings being mistakenly overridden. */
		BVDC_Window_SetSrcClip_isr( s_hMpegWindow0,
			0, 0,
			0, 0);
		BVDC_Window_SetDstRect_isr( s_hMpegWindow0,
			0, 0,
			0, 0);
		BVDC_Window_SetScalerOutput_isr( s_hMpegWindow0,
			0, 0,
			0, 0);
		break;
	default:
		break;
	}

	/* Temporary fixed for 1088 stream. */
	if(pFieldData->ulDisplayVerticalSize > BFMT_1080I_HEIGHT)
	{
		pFieldData->ulDisplayVerticalSize = BFMT_1080I_HEIGHT;
	}

	if(pFieldData->ulSourceVerticalSize > BFMT_1080I_HEIGHT)
	{
		pFieldData->ulSourceVerticalSize = BFMT_1080I_HEIGHT;
	}

	/* If we have bad MVD field just tell VDC to display fixed color */
	if(!pFieldData->ulDisplayVerticalSize ||
		!pFieldData->ulDisplayHorizontalSize ||
		!pFieldData->ulSourceVerticalSize ||
		!pFieldData->ulSourceHorizontalSize)
	{
		pFieldData->bMute = true;
	}

	/* call VDC callback routine */
	BDBG_MSG(("\n--------------------------------------------"));
	BDBG_MSG(("pFieldData->bMute                         : %d", pFieldData->bMute));
	BDBG_MSG(("pFieldData->eSourcePolarity               : %d", pFieldData->eSourcePolarity));
	BDBG_MSG(("pFieldData->eInterruptPolarity            : %d", pFieldData->eInterruptPolarity));
	BDBG_MSG(("pFieldData->ulDisplayHorizontalSize       : %d", pFieldData->ulDisplayHorizontalSize));
	BDBG_MSG(("pFieldData->ulDisplayVerticalSize         : %d", pFieldData->ulDisplayVerticalSize));
	BDBG_MSG(("pFieldData->ulSourceHorizontalSize        : %d", pFieldData->ulSourceHorizontalSize));
	BDBG_MSG(("pFieldData->ulSourceVerticalSize          : %d", pFieldData->ulSourceVerticalSize));
	BDBG_MSG(("i32_HorizontalPanScan                     : 0x%x", pFieldData->i32_HorizontalPanScan));
	BDBG_MSG(("i32_VerticalPanScan                       : 0x%x", pFieldData->i32_VerticalPanScan));

	BVDC_Source_MpegDataReady_isr(pvSourceHandle, iParm2, pvFieldData);
}

static int b_filesize(const char *filename)
{
	struct stat st;
	if (stat(filename, &st))
		return -1;
	return st.st_size;
}


void fill_cdb(char *filename, uint32_t *pCDBAddr, uint32_t ui32CDBSize,uint32_t *ptotalbytes)
{
	uint32_t i=0, j=0, data, totalbytes, mod=0;
	int size;
	FILE *fp_es;

	printf("Inside fill_cdb\n");
	fp_es = fopen(filename, "rb");
	if (!fp_es)
		goto FileFailed;

	size = b_filesize(filename);
	if (size == -1)
	{
		printf("Unable to read filesize");
		fclose (fp_es);
		goto FileFailed;
	}

	if ((uint32_t)size < ui32CDBSize)
	{
		totalbytes = (uint32_t)size;
		/* check for dword alignment */
		if ((mod=totalbytes%4))
		{
			totalbytes-=mod;
		}
	}
	else
	{
		totalbytes=ui32CDBSize;

		printf("ES file %s too large to fit in CDB of size %0x Defaulting to max size", filename, ui32CDBSize);
		/*goto FileFailed;*/
	}

	printf("file opened \n");

	for (i=0,j=0; i<=totalbytes; i+=4,j++)
	{
		data = (uint32_t)(((fgetc(fp_es)<<24))|((fgetc(fp_es)<<16)) | ((fgetc(fp_es)<<8)) | ((fgetc(fp_es))));
		if(i<=20)
			printf("%0x\t", data);
		*((volatile uint32_t *)(pCDBAddr)+j) = data;
	}

	if (mod)
	{
		printf("has remainder\n");
		data = 0;
		for (i=0; i<mod; i++)
		{
			data|=(uint32_t)(fgetc(fp_es)<<(24-(i*8)));
		}
		printf("mod=%d, data=%0x",mod,data);
		*((volatile uint32_t *)(pCDBAddr)+(j)) = data;
		totalbytes += 4;
	}
	*ptotalbytes = totalbytes;
	printf("Total bytes in CDB  = %d \n",totalbytes);
	fclose (fp_es);
	return;

FileFailed :
	if(fp_es)
		fclose (fp_es);
	printf("Unable to open file %s\n",filename);
	return;
}



void fill_itb(char *filename, uint32_t *pITBAddr, uint32_t ui32ITBSize,uint32_t *ptotalbytes, uint16_t *Picture_Count)
{
	uint32_t i=0, j=0, data, totalbytes, mod=0,temp_code;
	int size;
	FILE *fp_es;
	printf("Inside fill_itb\n");
	fp_es = fopen(filename, "rb");
	if (!fp_es)
		goto FileFailed;

	size = b_filesize(filename);
	if (size == -1)
	{
		printf("Unable to read filesize");
		fclose (fp_es);
		goto FileFailed;
	}

	if ((uint32_t)size < ui32ITBSize)
	{
		totalbytes = (uint32_t)size;
		/* check for dword alignment */
		if ((mod=totalbytes%4))
		{
			totalbytes-=mod;
		}
	}
	else
	{
	totalbytes=ui32ITBSize;
		printf("ES file %s too large to fit in CDB of size %0x", filename, ui32ITBSize);
		/*goto FileFailed;*/
	}

	printf("file opened \n");

	for (i=0,j=0,data=0; i<=totalbytes; i+=4,j++)
	{
		temp_code=(uint32_t)fgetc(fp_es);
		if((temp_code==0x00) ||(temp_code==0xB7))
		(*Picture_Count)++;
		data=(temp_code<<24);
		temp_code=(uint32_t)fgetc(fp_es);
		if((temp_code==0x00) ||(temp_code==0xB7))
		(*Picture_Count)++;
		data|= (temp_code<<16);
		temp_code=(uint32_t)fgetc(fp_es);
		if((temp_code==0x00) ||(temp_code==0xB7))
		(*Picture_Count)++;
		data|=( temp_code<<8);
		temp_code=(uint32_t)fgetc(fp_es);
		if((temp_code==0x00) ||(temp_code==0xB7))
		(*Picture_Count)++;
		data|= temp_code;

		*((volatile uint32_t *)(pITBAddr)+j) = data;

	}

	if (mod)
	{
		printf("has remainder\n");
		data = 0;
		for (i=0; i<mod; i++)
		{
			temp_code=(uint32_t)fgetc(fp_es);
			if((temp_code==0x00) ||(temp_code==0xB7))
			(*Picture_Count)++;
			data|=(uint32_t)(temp_code<<(24-(i*8)));
		}
		printf("mod=%d, data=%0x",mod,data);
		*((volatile uint32_t *)(pITBAddr)+(j)) = data;
		totalbytes += 4;

	}
	*ptotalbytes = totalbytes;
	printf("Total bytes in CDB  = %d \n",totalbytes);
	fclose (fp_es);
	return;

FileFailed :
	if(fp_es)
		fclose (fp_es);
	printf("Unable to open file %s\n",filename);
	return;
}

#if 0
typedef struct
{
	unsigned char SCvalue[16];

}START_CODE_TABLE;


FILE *ESFilePtr=NULL;
int count=2;
START_CODE_TABLE STable;
uint32_t ITBCount=0;

BERR_Code WritetoITBTable(int *Max_Offset, int *File_Offset,uint32_t ui32CDBPhysOffset, uint32_t *pITBPhysOffset,
					uint32_t ui32ITBSize, uint32_t *itb_totalbytes, uint32_t *Picture_Count)
{
	int rc,i;
	static int flag=1;
	static int initial_offset=0;
	uint32_t ui32BaseAddress=0;
	uint32_t ui32AdrValue[2];

	rc = fgetc(ESFilePtr);

	if(flag ==0)
		*Max_Offset = *File_Offset-initial_offset;

	if((rc==0x00) ||(rc==0xB7))
		(*Picture_Count)++;

	/* Write SCEntry (G3)to the file*/

/*	if((count >15)|| (*Max_Offset) > 250)		*/
	if((count >15)|| ((*Max_Offset) > 250) || ((*File_Offset) > 250))
	{
		if( ((ITBCount+4)*4) <= ui32ITBSize )
		{
			uint32_t data;
			uint32_t i;
			for(i=0; i<16; i+=4)
			{
				data = (uint32_t)(((STable.SCvalue[i]<<24))|((STable.SCvalue[i+1]<<16)) | ((STable.SCvalue[i+2]<<8)) | ((STable.SCvalue[i+3])));
				*((volatile uint32_t *)(pITBPhysOffset)+ITBCount) = data;
				ITBCount+=1;
			}
			*itb_totalbytes+=1;

		}
		else
		{
			printf("ITB data too large to fit in ITB buffer of size %0x\n", ui32ITBSize);
			return BERR_OUT_OF_DEVICE_MEMORY;
		}

		for(i=2;i<16;i++)
			STable.SCvalue[i]|=0xFF;

		if((*Max_Offset) > 250)
		{
			flag = 1;
			*Max_Offset = 0;
		}

		count =2;
	}


	/* Write  SCValue and offset to array*/
	STable.SCvalue[count] = rc;
	count++;
	STable.SCvalue[count] = *Max_Offset;
	count++;


	/* Write CDB address (G0)to the file*/
	if(flag ==1)
	{
		initial_offset = *File_Offset;
		ui32BaseAddress = ui32CDBPhysOffset + (*File_Offset);
		ui32AdrValue[0]= 0x00 | ((ui32BaseAddress&0xffff0000) >>16)  ;
		ui32AdrValue[1]= 0x00 | ((ui32BaseAddress&0x0000ffff) <<16) ;

		*((volatile uint32_t *)(pITBPhysOffset)+ITBCount) =ui32AdrValue[0];
		ITBCount+=1;

		*((volatile uint32_t *)(pITBPhysOffset)+ITBCount) = ui32AdrValue[1];
		ITBCount+=1;

		*((volatile uint32_t *)(pITBPhysOffset)+ITBCount) = 0;
		ITBCount+=1;

		*((volatile uint32_t *)(pITBPhysOffset)+ITBCount) = 0;
		ITBCount+=1;

		flag =0;
		*itb_totalbytes+=1;

	}

	(*File_Offset)++;



/*	if((*Max_Offset) > 250)
		(*Max_Offset) = 0;*/
	return BERR_SUCCESS;

}


void ConvertEStoITB(char *es_file, uint32_t ui32CDBPhysOffset, uint32_t *pITBPhysOffset, uint32_t ui32ITBSize,
					uint32_t *itb_totalbytes, uint32_t *Pic_Count)
{

	int rc3,rc1,rc2,i,data;
	int File_Offset=0,Max_Offset=0;

	printf("file name= %s\n",es_file);
	printf("CDBPhsOffset = 0x%x  %d\n",ui32CDBPhysOffset, ui32CDBPhysOffset);

	ESFilePtr = fopen(es_file,"rb");
	if(ESFilePtr==NULL)
	{
		printf("Failed to open EStoITB.es file\n");
		exit(0);
	}

	rc2 = 0xFF;
	rc3 = 0xFF;
	ITBCount=0;

	printf("Inside ConvertEStoITB \n");
	STable.SCvalue[0] |= 0x00;
	STable.SCvalue[1] |= 0x03;
	for (i=2;i<16 ;i++ )
	{
		STable.SCvalue[i] |= 0xff;
	}


	while (1)
	{
		rc1 = fgetc(ESFilePtr);
		File_Offset++;

		if((rc1 ==0x01) && (!(rc2 |rc3)) && (rc1 != EOF))
		{
			WritetoITBTable(&Max_Offset,&File_Offset,ui32CDBPhysOffset,pITBPhysOffset, ui32ITBSize, itb_totalbytes,Pic_Count);
		}

		rc2 = fgetc(ESFilePtr);
		File_Offset++;
		if((rc2 ==0x01) && (!(rc1 |rc3)) && (rc2 != EOF))
		{
			WritetoITBTable(&Max_Offset,&File_Offset,ui32CDBPhysOffset,pITBPhysOffset, ui32ITBSize, itb_totalbytes,Pic_Count);
		}

		rc3 = fgetc(ESFilePtr);
		File_Offset++;
		if((rc3 ==0x01) && (!(rc1 |rc2)) && (rc3 != EOF))
		{
			WritetoITBTable(&Max_Offset,&File_Offset,ui32CDBPhysOffset,pITBPhysOffset, ui32ITBSize, itb_totalbytes,Pic_Count);
		}

		if((rc1 == EOF) || (rc2 == EOF) || (rc3== EOF))
		{
			STable.SCvalue[count] |= 0xFF;
			for(i=0,data=0; i<16; i=i+4)
			{
				data = (uint32_t)(((STable.SCvalue[i]<<24))|((STable.SCvalue[i+1]<<16)) | ((STable.SCvalue[i+2]<<8)) | ((STable.SCvalue[i+3])));
				*((volatile uint32_t *)(pITBPhysOffset)+ITBCount) = data;
				ITBCount++;
			}

			printf("End of File\n");
			break;
		}

	}
	*itb_totalbytes = ((*itb_totalbytes)+1)*16;

	fclose(ESFilePtr);

}
uint32_t TerminateTestApp(void)
{
	if( !s_bInited ) return 0;

	printf("\nTEST: Clean Up!\n");

	/* This is where anything that needs to get freed or terminated
	 * gets done. (i.e. things allocated in the INIT function.) */
	/* release all the opened resources */

	BINT_Close(g_hInterrupt);
	BMEM_Dbg_DumpHeap(g_hMemory);
	BMEM_DestroyHeap(g_hMemory);
	BCHP_Close(g_hChip);
	BREG_Close(g_hRegister);
	BDBG_Uninit();
	BKNI_Uninit();
	BSystem_Uninit(&sysInfo);
	s_bInited = false;

	return ( 1 );
}
#endif


uint32_t InitializeMvdTestApp(void)
{
	BERR_Code            err;
	s_bInited = true;

	/* PR13177: support RGBHV output */
	{
#if (BCHP_CHIP==7038)
#if defined(BCHP_REV_A0) || defined(BCHP_REV_B0) || defined(BCHP_REV_B1) || (BCHP_VER < BCHP_VER_C0)
		uint32_t ulValPinMux8 = BREG_Read32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8);
		uint32_t ulValPinMux9 = BREG_Read32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9);
		/* set up pin mux */
		ulValPinMux8 &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_45);
		ulValPinMux8 |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_8, gpio_45, 1);/* hsync_0 */
		BREG_Write32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_8, ulValPinMux8);

		ulValPinMux9 &= ~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_46);
		ulValPinMux9 |= BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_9, gpio_46, 1);/* vsync_0 */
		BREG_Write32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_9, ulValPinMux9);
#else
		uint32_t ulValPinMux7 = BREG_Read32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7);
		/* set up pin mux */
		ulValPinMux7 &= ~(
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_25) |
			BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_24));
		ulValPinMux7 |=
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_25, 3) |/* hsync_0 */
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_7, gpio_24, 4); /* vsync_0 */
		BREG_Write32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_7, ulValPinMux7);
#endif
#elif (BCHP_CHIP==3560)
		/* set up pinmux for 656 output */
		uint32_t ulValPinMux4 = BREG_Read32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4);
		uint32_t ulValPinMux0 = BREG_Read32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0);
		/* set up pin mux */
		ulValPinMux4 &=
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_40) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_41);
		ulValPinMux4 |=
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_40, 1) |/* hsync */
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_4, gpio_41, 1); /* vsync */
		BREG_Write32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_4, ulValPinMux4);

		ulValPinMux0 &=
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_clk_pos) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_23) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_22) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_21) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_20) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_19) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_18) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_17) &
			~BCHP_MASK(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_16);
		ulValPinMux0 |=
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_clk_pos, 1) | /* vo_656_clk */
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_23, 1) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_22, 1) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_21, 1) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_20, 1) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_19, 1) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_18, 1) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_17, 1) |
			BCHP_FIELD_DATA(SUN_TOP_CTRL_PIN_MUX_CTRL_0, dvo_16, 1);
		BREG_Write32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, ulValPinMux0);
#endif
	}

	TestError( BKNI_CreateEvent( &g_TaskComplete ),
				"ERROR : BKNI_CreateEvent") ;
	BKNI_SetEvent( g_TaskComplete ) ;

	/* Event for Buffer ready / decoded buffer ready for Direct Feed*/
	TestError( BKNI_CreateEvent( &g_hBufferReadyEvent ),
			"ERROR : BKNI_CreateEvent") ;

	/* create a task for handling  JPEG DecodedBufferReady interrupts */
	BTST_CreateTask( &hTaskBuffReady, bsettop_p_dec_buf_ready, NULL);

Done:
	return ( err == BERR_SUCCESS );
}



#if BDBG_DEBUG_BUILD
#ifdef B_SYSTEM_linuxkernel
#include <asm/string.h>
#else
#include <string.h>
#endif
static void set_module_debuglevel(const char *modulelist, BDBG_Level level)
{
    while (modulelist && *modulelist) {
        const char *end = strchr(modulelist, ',');
        if (!end) {
            BDBG_SetModuleLevel(modulelist, level);
            break;
        }
        else {
            int n = end-modulelist;
            char *buf = (char *)BKNI_Malloc(n+1);
            /* NOTE: memory leak, but this is debug */
            strncpy(buf, modulelist, n);
            buf[n] = 0;
            BDBG_SetModuleLevel(buf, level);
        }
        modulelist = ++end;
    }
}
#else
static void set_module_debuglevel(const char *modulelist, BDBG_Level level)
{
	BSTD_UNUSED(level);
	BSTD_UNUSED(modulelist);
}
#endif

/* Get a user-configurable setting. This is useful for development. */
const char *bsettop_get_config(const char *name)
{
#if defined(__vxworks) || defined(B_SYSTEM_linuxkernel)
    int i;
    for (i=0;i<g_config_total;i++) {
        if (!strcmp(g_config[i].name, name))
            return g_config[i].value;
    }
    return NULL;
#else
    /* linux user - get a value from the environment */
    return getenv(name);
#endif
}

#if 0
void fill_itb_file(uint32_t *pITBAddr, uint32_t ui32ITBSize,uint32_t *ptotalbytes)
{
	uint32_t i,j,data=0;
	uint32_t totalbytes=size_itb;
	uint32_t mod;

	if(ui32ITBSize<size_itb)
	{
		printf("ITB file too large to fit in ITB of size ");
		goto FileFailed;
	}
	printf("ITB fill start \n");
	if((mod=size_itb%4))
		totalbytes=size_itb-mod;
	for(i=0,j=0;i<totalbytes;i+=4,j++,data=0)
	{
		data|=(((*(itb_dump+i)&0xff)<<24) | ((*(itb_dump+i+1)&0xff)<<16) | ((*(itb_dump+i+2)&0xff)<<8) | (*(itb_dump+i+3)&0xff));
		*((volatile uint32_t *)(pITBAddr)+j) = data;
	}

	if (mod)
	{
		data = 0;
		for (i=0; i<mod; i++)
		{
			data|=((*(itb_dump+i+totalbytes))<<(24-(i*8)));
		}
		*((volatile uint32_t *)(pITBAddr)+(j)) = data;
		totalbytes += 4;
	}
	*ptotalbytes=totalbytes;
	printf("ITB fill done \n");
	return;

FileFailed :
	printf("Unable to write in ITB");
	return;

}



void fill_cdb_file(uint32_t *pcdbAddr, uint32_t ui32cdBSize,uint32_t *ptotalbytes)
{
	uint32_t i,j,data=0;
	uint32_t totalbytes=size_cdb;
	uint32_t mod;

	if(ui32cdBSize<size_cdb)
	{
		printf("CDB file too large to fit in CDB of size ");
		goto FileFailed;
	}

	printf("CDB fill start \n");

	if((mod=size_cdb%4))
		totalbytes=size_cdb-mod;
	for(i=0,j=0;i<totalbytes;i+=4,j++,data=0)
	{
		data|=(((*(cdb_dump+i)&0xff)<<24) | ((*(cdb_dump+i+1)&0xff)<<16) | ((*(cdb_dump+i+2)&0xff)<<8) | (*(cdb_dump+i+3)&0xff));
		*((volatile uint32_t *)(pcdbAddr)+j) = data;
	}

	if (mod)
	{
		data = 0;
		for (i=0; i<mod; i++)
		{
			data|=((*(cdb_dump+i+totalbytes))<<(24-(i*8)));
		}
		*((volatile uint32_t *)(pcdbAddr)+(j)) = data;
		totalbytes += 4;
	}
	*ptotalbytes=totalbytes;
	printf("CDB fill done \n");
	return;

FileFailed :
	printf("Unable to write in CDB");
	return;

}
#endif
void dump_mvd_bin(int channel, const char *mpegfile)
{
	unsigned int i, iCount;

	uint32_t data;
	uint32_t buffer_addr, buffer_size;
	void *pBufferAddr;
	FILE *mpeg = fopen(mpegfile, "wb");

	BDBG_ASSERT(mpeg);
	BSTD_UNUSED(channel) ;


	printf("Enter buffer start address(physical)\n");
	BTST_P_Scanf(pContext, "%x", &buffer_addr);

	printf("Enter buffer size in bytes \n");
	BTST_P_Scanf(pContext, "%u", &buffer_size);

	BKNI_EnterCriticalSection();
	BMEM_ConvertOffsetToAddress(
			g_hMvdCh_0->hCurHeap,
			buffer_addr,
			&pBufferAddr);
	printf("buffer_addr = %x, buffer_size = %d, pBufferAddr = %p\n",buffer_addr, buffer_size, pBufferAddr);

	for(i=0; i<buffer_size/4; i++)
	{
		data = ((volatile uint32_t *)pBufferAddr)[i] ;
		/*fwrite(&data,sizeof(data),1,mpeg);*/
		iCount = fputc((data>>24)&0xFF, mpeg);
		iCount = fputc((data>>16)&0xFF, mpeg);
		iCount = fputc((data>>8)&0xFF, mpeg);
		iCount = fputc((data)&0xFF, mpeg);

		printf("data=%0x\t", data);
		printf(".");
	}


	BKNI_LeaveCriticalSection();

	fclose(mpeg);

}

void dump_mvd(int channel, const char *mpegfile)
{
	unsigned int i, iCount;

	uint32_t data;
	uint32_t buffer_addr, buffer_size;
	void *pBufferAddr;
	FILE *mpeg = fopen(mpegfile, "w");

	BDBG_ASSERT(mpeg);
	BSTD_UNUSED(channel) ;


	printf("Enter buffer start address(physical)\n");
	BTST_P_Scanf(pContext, "%x", &buffer_addr);

	printf("Enter buffer size in bytes \n");
	BTST_P_Scanf(pContext, "%u", &buffer_size);
/*
	buffer_addr = 0x2088AE0;
	buffer_size = 500;
*/
	BKNI_EnterCriticalSection();
	BMEM_ConvertOffsetToAddress(
			g_hMvdCh_0->hCurHeap,
			buffer_addr,
			&pBufferAddr);
	printf("buffer_addr = %x, buffer_size = %d, pBufferAddr = %p\n",buffer_addr, buffer_size, pBufferAddr);
	for(i=0; i<buffer_size/4; i++)
	{
		data = ((volatile uint32_t *)pBufferAddr)[i] ;
		printf("data=%0x\t", data);
		iCount = fputc((data>>24)&0xFF, mpeg);
		iCount = fputc((data>>16)&0xFF, mpeg);
		iCount = fputc((data>>8)&0xFF, mpeg);
		iCount = fputc((data)&0xFF, mpeg);

	}


	BKNI_LeaveCriticalSection();

	fclose(mpeg);

}

static void DumpMap1( BXPT_Handle hXpt, BAVC_XptContextMap *CtxMap )
{
	printf( "CDB_Read (0x%lX) is 0x%lX\n", (unsigned long) CtxMap->CDB_Read, (unsigned long) BREG_Read32( hXpt->hRegister, CtxMap->CDB_Read ) );
	printf( "CDB_Base (0x%lX) is 0x%lX\n", (unsigned long) CtxMap->CDB_Base, (unsigned long) BREG_Read32( hXpt->hRegister, CtxMap->CDB_Base ) );
	printf( "CDB_Wrap (0x%lX) is 0x%lX\n", (unsigned long) CtxMap->CDB_Wrap, (unsigned long) BREG_Read32( hXpt->hRegister, CtxMap->CDB_Wrap ) );
	printf( "CDB_Valid (0x%lX) is 0x%lX\n", (unsigned long) CtxMap->CDB_Valid, (unsigned long) BREG_Read32( hXpt->hRegister, CtxMap->CDB_Valid ) );

	printf( "ITB_Read (0x%lX) is 0x%lX\n", (unsigned long) CtxMap->ITB_Read, (unsigned long) BREG_Read32( hXpt->hRegister, CtxMap->ITB_Read ) );
	printf( "ITB_Base (0x%lX) is 0x%lX\n", (unsigned long) CtxMap->ITB_Base, (unsigned long) BREG_Read32( hXpt->hRegister, CtxMap->ITB_Base ) );
	printf( "ITB_Wrap (0x%lX) is 0x%lX\n", (unsigned long) CtxMap->ITB_Wrap, (unsigned long) BREG_Read32( hXpt->hRegister, CtxMap->ITB_Wrap ) );
	printf( "ITB_Valid (0x%lX) is 0x%lX\n", (unsigned long) CtxMap->ITB_Valid, (unsigned long) BREG_Read32( hXpt->hRegister, CtxMap->ITB_Valid ) );
}


static void RaveMpegVideoTest1( BXPT_Handle hXpt, BAVC_XptContextMap *Map, uint16_t pidnum, int frontend )
{
	BERR_Code Res;
	BXPT_Rave_ChannelSettings RaveDefaults;
	BXPT_Rave_Handle hRave;
	BXPT_RaveCx_Handle hCtx;
	BXPT_Rave_AvSettings AvCtxCfg;


	int RAVE_CHNL_NUM = 0;

	BAVC_CdbItbConfig CdbItb = { { 2457600, 5, false }, {131072, 5, false }, true }; /* original */



	printf( "RaveMpegVideoTest() called\n" );

	Res = BXPT_Rave_GetChannelDefaultSettings( hXpt, RAVE_CHNL_NUM, &RaveDefaults );
	BDBG_ASSERT( !Res );

	Res = BXPT_Rave_OpenChannel( hXpt, &hRave, RAVE_CHNL_NUM, &RaveDefaults );
	BDBG_ASSERT( !Res );

	Res = BXPT_Rave_AllocContext( hRave, BXPT_RaveCx_eAv, &CdbItb, &hCtx );
	BDBG_ASSERT( !Res );



	/* PID channel 0 carries PID 0x810 from parser band 1 */
	BXPT_ConfigurePidChannel( hXpt, 0, pidnum, 1 );
	BXPT_EnablePidChannel( hXpt, 0 );

	BXPT_Rave_AddPidChannel( hCtx, 0, false );
	if(frontend)
	{
		printf("frontend mode RaveMpegVideoTest1\n");
		/* Parser band 1 gets data from input band 2. New app code will configure JP2801 to feed IB2 */
		Res = BXPT_SetParserDataSource( hXpt, 1, BXPT_DataSource_eInputBand, 2 );
	}
	else
	{
		printf("streamer mode RaveMpegVideoTest1\n");
		/* Parser band 1 gets data from input band 4. for streamer */
		Res = BXPT_SetParserDataSource( hXpt, 1, BXPT_DataSource_eInputBand, 4 );
	}

	BDBG_ASSERT( !Res );

	Res = BXPT_SetParserEnable( hXpt, 1, true );
	BDBG_ASSERT( !Res );

	BXPT_Rave_GetAvConfig( hCtx, &AvCtxCfg );
	AvCtxCfg.InputFormat = BAVC_StreamType_eTsMpeg;
	AvCtxCfg.ItbFormat = BAVC_ItbEsType_eMpeg2Video;
	AvCtxCfg.OutputFormat = BAVC_StreamType_eEs;
	AvCtxCfg.EsRanges[ 0 ].RangeHi = 0xFE;
	AvCtxCfg.EsRanges[ 0 ].RangeLo = 0;
	AvCtxCfg.EsRanges[ 0 ].Enable = true;
	AvCtxCfg.EsRanges[ 1 ].Enable = false;
	AvCtxCfg.EsRanges[ 2 ].Enable = false;
	AvCtxCfg.EsRanges[ 3 ].Enable = false;
	AvCtxCfg.StreamIdHi = 0xEF;
	AvCtxCfg.StreamIdLo = 0xBD;
	Res = BXPT_Rave_SetAvConfig( hCtx, &AvCtxCfg );
	BDBG_ASSERT( !Res );
	/*BCHP_XPT_RAVE_CX0_AV_VID_SC_RANGE_AB
	BREG_Write32(g_hRegister, 0x21008c, 0x11);
	*/
	BREG_Write32(g_hRegister, 0x210084, 0xBFB0AF01);
	BREG_Write32(g_hRegister, 0x21005c, 0x0010002B);
	BREG_Write32(g_hRegister, 0x210088, 0x00000000);

	Res = BXPT_Rave_EnableContext( hCtx );
	BDBG_ASSERT( !Res );

	printf( "Start playback on streamer. Press <enter>.\n" );
	getchar();

	Res = BXPT_Rave_GetContextRegisters( hCtx, Map );
	BDBG_ASSERT( !Res );
	DumpMap1( hXpt, Map );


}




void Xpt_TimeStampManagement_Video1(
	BXPT_Handle hXpt,
	unsigned	PcrPid
	)
{
	BXPT_PCR_Handle hPcr;
	BXPT_PCR_DefSettings Defaults;
	BXPT_PcrOffset_Handle hPcrOff;
	BXPT_PCR_XptStreamPcrCfg PcrCfg;
	BXPT_PcrOffset_Settings PcrOffsetSettings;
	BXPT_PcrOffset_Defaults PcrOffsetDefaults;

	unsigned InputBand = 2;
	unsigned ParserBand = 1;
	unsigned PidChannelNum = 0;


	printf( "XPTTimeStampManagement_Video() called\n" );

	PcrCfg.Pid = PcrPid;				      					/* PCR Pid */
	PcrCfg.eStreamSelect = BXPT_DataSource_eInputBand;    	/* Pcr stream source. NOTE: only input bands are supported. */
	PcrCfg.WhichSource = InputBand;		  					/* Which instance of the source */
	PcrOffsetSettings.PidChannelNum = PidChannelNum;		/* PID channel carrying PCRs */
	PcrOffsetSettings.CountMode = BXPT_PcrOffset_StcCountMode_eMod300;		/* Count mode. Mod 300 for MPEG */
	PcrOffsetSettings.OffsetThreshold = 0;
	PcrOffsetSettings.UseHostPcrs = false;
  	BXPT_SetParserDataSource( hXpt, ParserBand, BXPT_DataSource_eInputBand, InputBand );
	BXPT_SetParserEnable( hXpt, ParserBand, true );
	BXPT_ConfigurePidChannel( hXpt, PidChannelNum, PcrPid, ParserBand );
	BXPT_EnablePidChannel( hXpt, PidChannelNum );

	BXPT_PCR_GetChannelDefSettings( hXpt, 0, &Defaults );
	PcrOffsetDefaults.UsePcrTimeBase = true;
	PcrOffsetDefaults.WhichPcr = 0;

	BXPT_PCR_Open( hXpt, 0, &Defaults, &hPcr );

	BXPT_PCR_SetStreamPcrConfig(hPcr, &PcrCfg );
	printf( "XPTTimeStampManagement_Video() called\n" );

	BXPT_PcrOffset_GetChannelDefSettings( hXpt, 0, &PcrOffsetDefaults );
	printf( "XPTTimeStampManagement_Video() called\n" );

   	BXPT_PcrOffset_Open( hXpt, 0, &PcrOffsetDefaults, &hPcrOff);
	printf( "XPTTimeStampManagement_Video() called\n" );

	BXPT_PcrOffset_SetSettings( hPcrOff, &PcrOffsetSettings );
	printf( "XPTTimeStampManagement_Video() called\n" );

}


void StartTransport(uint16_t ui16VideoPid, uint16_t ui16PcrPid,BAVC_XptContextMap *Map )
{
	BXPT_Handle hXpt;
	BXPT_DefaultSettings   xptSettings;
	BXPT_InputBandConfig Ib4Config;
	BERR_Code              	err = BERR_SUCCESS;
	int				frontend;
	BERR_Code Res;

	TestError(BXPT_GetDefaultSettings(&xptSettings, g_hChip), "BXPT_GetDefaultSettings");

	TestError(BXPT_Open(&hXpt, g_hChip, g_hRegister, g_hMemory, g_hInterrupt,
		                      &xptSettings), "BXPT_Open");
	printf("\nEnter 0 for streamer, 1 for frontend: ");
	BTST_P_Scanf(pContext, "%d", &frontend);

	if(frontend)
	{
		printf("frontend mode StartTransport\n");
		Res = BXPT_SetParserDataSource( hXpt, 0, BXPT_DataSource_eInputBand, 2 );
	   	/* Config IB2 to use frontend */
	    	BXPT_GetInputBandConfig( hXpt, 2, &Ib4Config );
	    	Ib4Config.ParallelInputSel = false;
	    	BXPT_SetInputBandConfig( hXpt, 2, &Ib4Config );
	}
	else
	{
		printf("STREAMER MODE StartTransport\n");
		/* Route parallel data from PCMCIA connector to IB4 */ /*streamer */
		BREG_Write32(g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_0, 0x2AAAC000 );
		BREG_Write32( g_hRegister, BCHP_SUN_TOP_CTRL_PIN_MUX_CTRL_1, 0x00000049 );

	   	/* Config IB4 to use parallel input */
	    	BXPT_GetInputBandConfig( hXpt, 4, &Ib4Config );
	    	Ib4Config.ParallelInputSel = true;
	    	BXPT_SetInputBandConfig( hXpt, 4, &Ib4Config );

	}
	Xpt_TimeStampManagement_Video1( hXpt,ui16PcrPid);
	RaveMpegVideoTest1( hXpt, Map, ui16VideoPid, frontend );
Done:
	printf("XPT start done\n");

}

static int	timeval_subtract (struct timeval *result,struct timeval  *x,struct timeval  *y)
{	  /* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec)
	{
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000)
	{
		int nsec = (y->tv_usec - x->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}
	/* Compute the time remaining to wait.
	tv_usec  is certainly positive. */
	result->tv_sec = x->tv_sec - y->tv_sec;
	result->tv_usec = x->tv_usec - y->tv_usec;
	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

static void print_stats(struct rusage r_usage_s, struct rusage r_usage_e)
{
	struct rusage r_usage_result;
	int rc ;
	printf("Stats:\n");
	rc=timeval_subtract(&r_usage_result.ru_utime, &r_usage_e.ru_utime, &r_usage_s.ru_utime);
	printf("1.user time = %.2f secs\n", (r_usage_result.ru_utime.tv_sec + ((float)r_usage_result.ru_utime.tv_usec/1000000)));
	rc=timeval_subtract(&r_usage_result.ru_stime, &r_usage_e.ru_stime, &r_usage_s.ru_stime);
	printf("2.system time = %.2f secs\n", (r_usage_result.ru_stime.tv_sec + ((float)r_usage_result.ru_stime.tv_usec/1000000)));
}

BERR_Code mvd_test
(	BREG_Handle 				hReg,
	BMEM_Handle 				hMem,
	BINT_Handle 				hInt,
	BMVD_Handle 			hMvd,
	BMVD_ChannelHandle		hMvdChan0,
	BVDC_Handle 			hVdc,
	BVDC_Display_Handle 		hDisplay0,
	BVDC_Window_Handle 		hMpegWindow0,
	BVDC_Source_Handle 		hMpegSource0,
	BCHP_Handle 				hChip)


{

	uint32_t cdb_totalbytes=0;
	uint32_t itb_totalbytes=0;

	BERR_Code               err = BERR_SUCCESS;
	BMVD_MemoryInfo mvd_meminfo;
	BMVD_VideoParams  VideoParams;
	BERR_Code		rc;
	BAVC_XptContextMap Map;
	BTST_TaskHandle         hTaskUserData = NULL;


	int Command;

	BFMT_VideoFmt eVideoFmt = BFMT_VideoFmt_eNTSC;
	uint16_t                Pic_Count=0;
	uint16_t			ui16PcrPid,ui16VideoPid;
	BMVD_USERDATA_Settings stUserDataSettings;

	BSTD_UNUSED(hDisplay0) ;
	BSTD_UNUSED(hMpegSource0) ;

	/* initialize */
	InitializeMvdTestApp();
	g_hRegister=hReg;
	g_hMemory=hMem;
	g_hInterrupt=hInt;
	g_hChip= hChip;


	BDBG_Init();
    	set_module_debuglevel(bsettop_get_config("trace_modules"), BDBG_eTrace);
    	set_module_debuglevel(bsettop_get_config("msg_modules"), BDBG_eMsg);
    	set_module_debuglevel(bsettop_get_config("wrn_modules"), BDBG_eWrn);

	g_hMvd = hMvd ;
	g_hMvdCh_0 = hMvdChan0 ;
	s_hMpegWindow0 = hMpegWindow0;
	s_hVdc = hVdc;



	BKNI_CreateEvent( &g_hUserDataEvent );
	BTST_CreateTask( &hTaskUserData, TEST_UserDataCapture, g_hMvdCh_0);
	BMVD_USERDATA_GetDefaultSettings( hMvdChan0, USER_DATA_PACKET_SIZE, &stUserDataSettings),
	stUserDataSettings.uTotalBufferSize = TOTAL_USER_DATA_BUFFER_SIZE ;
	BMVD_USERDATA_Open( hMvdChan0, &g_hUserData, &stUserDataSettings);
	BMVD_InstallInterruptCallback(
			hMvdChan0,
			BMVD_Interrupt_eUserData,
			TEST_UserData_isr,
			NULL,
			0 );
	printf("The Video Pid : ") ;
	BTST_P_Scanf(pContext, "%hx", &ui16VideoPid) ;
	printf("The PCR Pid : ") ;
	BTST_P_Scanf(pContext, "%hx", &ui16PcrPid) ;

	printf("Starting the XPT\n\n") ;
	StartTransport(ui16VideoPid,ui16PcrPid, &Map);


	while(1)
	{

		BERR_Code result = BKNI_WaitForEvent(g_TaskComplete, BKNI_INFINITE);
		if (!result)
		{
			printf("\nMpeg Video Decoder Menu: \n") ;
			printf("\t1. Directfeed Decoder:		1\n") ;
			printf("\t2. Single Decode or Dual Decode :	2\n") ;
			printf("\t3. Dump VBV:	  3\n") ;
			printf("\t4. Dump SCV:	4\n") ;
			printf("\t5. Frame Dump: 5\n") ;
			printf("\t6. Get Version Info: 6\n");
			printf("\t7. User Data menu: 7\n");
			printf("\t8. Decoder dead status: 8\n");
			printf("\t9. MTPT register dump menu: 9\n");/* 2 options all the register 1. select register */
			printf("\t10. MTPMV register dump menu: 10\n");/* 2 options all the register 1. select register*/
			printf("\t11. Dump DRAM: 11\n");
			printf("\t12. I picture found status: 12\n");
			printf("\t13. GetCDBOccupany: 13\n");
			printf("\t14. Register Dump (simdebug file): 14\n");
			printf("\t15. Dump DRAM: 15\n");
			printf("\t16. Directfeed Decoder using file:		16\n") ;
			printf("\t17. Dump DRAM in binary mode: 17\n");
			printf("\t18. Decoder dead status: 18\n");
			printf("\t   Press zero to quit:	0\n");

			printf("\nMain Menu Command = ") ;
			BTST_P_Scanf(pContext, "%d", &Command);

			if(Command== 0)
				break;



			switch(Command)
			{
				case 1 :
				{
						{
							BMVD_Handle MvdHandle = g_hMvd;
							BMVD_ChannelSettings MvdChannelSettings ;

							TestError( BMVD_GetChannelDefaultSettings(
									MvdHandle, /*MVD handle returned by BMVD_Open*/
									0,
									BMVD_ChannelConfig_eVideoHD,
									&MvdChannelSettings ),
								"ERROR:BMVD_GetChannelDefaultSettings" );

							MvdChannelSettings.eBufferScheme = BMVD_BufferScheme_eFourBufferScheme ;
							printf("DIRECT\n");
							MvdChannelSettings.bStillPictureMode = true; /*true;*/
							MvdChannelSettings.bStillDecodeDirectFeed = true; /*true; */

							TestError( BMVD_OpenChannel(
												MvdHandle,
												&g_hMvdCh_0,
												0,
												&MvdChannelSettings ),
								"ERROR:BMVD_OpenChannel" );
							hMvdChan0 = g_hMvdCh_0;
						}

						TestError( BMVD_InstallInterruptCallback(
					 	g_hMvdCh_0,
					 	BMVD_Interrupt_eDecodedBufferReady,
					 	bsettop_p_dec_buf_ready_isr, NULL, 0),
					 	"ERROR: BMVD_InstallInterruptCallback(UserData)");

						BMVD_GetMemoryInfo(hMvdChan0, &mvd_meminfo);
						printf("CDBSTART= %x, ITBSTART=%x\n", mvd_meminfo.ui32CDBStartPhysOffset, mvd_meminfo.ui32ITBStartPhysOffset);
						Pic_Count=2;
					#if 0
						fill_itb_file((uint32_t *)mvd_meminfo.pui32ITBStartAddress, mvd_meminfo.ui32ITBSize,  &itb_totalbytes);
						fill_cdb_file((uint32_t *)mvd_meminfo.pui32CDBStartAddress, mvd_meminfo.ui32CDBSize,  &cdb_totalbytes);
						/* do startdecode */
					#endif
						rc = BMVD_GetVideoParams(hMvdChan0, &VideoParams);
						if (rc!=BERR_SUCCESS) {
							BDBG_ERR(("Ignored error from BMVD_GetVideoParams"));
						}
						BMVD_SetStillDecodeCdbItbDepth (hMvdChan0, cdb_totalbytes, itb_totalbytes,Pic_Count);
						VideoParams.eStreamType =  BAVC_StreamType_eEs;
						printf("Calling Startdecode \n");
						rc = BMVD_StartDecode(hMvdChan0, &VideoParams);
						if (rc!=BERR_SUCCESS)
							BDBG_ERR(("Ignored error from BMVD_Startdecode"));
						BKNI_SetEvent( g_TaskComplete ) ;
						break;
				}
				case 2 :
				{
					struct rusage r_usage_start, r_usage_end;
					printf("Starting the MVD decode\n\n") ;
					BKNI_Memset( &VideoParams, 0, sizeof(VideoParams));
					VideoParams.eStreamType  = BAVC_StreamType_eTsMpeg;
					VideoParams.bEnableRMM = false;
					VideoParams.bEnableDropBOnOpenGOP = false;
					VideoParams.sXptContextMap = Map;
					VideoParams.ui16PcrPid=ui16PcrPid;
					VideoParams.ui16VideoPid=ui16VideoPid;


					/* Start Decode now */
					TestError( BMVD_StartDecode( hMvdChan0, &VideoParams),
								"ERROR:BMVD_StartDecode" );
					getrusage(RUSAGE_SELF, &r_usage_start);
					BMVD_FillDisplayBuffers(hMvdChan0,0,0,0);
					getrusage(RUSAGE_SELF, &r_usage_end);
					print_stats(r_usage_start, r_usage_end);

					/* save main display's format info */
					TestError( BFMT_GetVideoFormatInfo( eVideoFmt,
						&s_stVideoFmtInfo ),
						"ERROR:BFMT_GetVideoFormatInfo" );



			Done:
					/* return status */
					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				}
				case 3 :
				{
					uint32_t i,addr, data;
					FILE *fp = fopen("VBVout.dat","w") ;
					BMVD_MemoryInfo stMemoryInfo;
					BMVD_GetMemoryInfo(g_hMvdCh_0, &stMemoryInfo) ;
					BDBG_ERR(("VBV Address Virtual = %p", stMemoryInfo.pui32CDBStartAddress));
					BMEM_ConvertAddressToOffset(g_hMemory, stMemoryInfo.pui32CDBStartAddress, &addr) ;
					BDBG_ERR(("VBV Address Physical = %08lx", addr));

					for(i=0; i<stMemoryInfo.ui32CDBSize/4; i++)
					{
						if( i && (i%4==0) )
						{
							fprintf(fp, "\n");
						}
						data = ((volatile uint32_t *)stMemoryInfo.pui32CDBStartAddress)[i] ;
						fprintf(fp, "%02x %02x %02x %02x ", (data>>24)&0xFF, (data>>16)&0xFF, (data>>8)&0xFF, (data)&0xFF) ;
					}
					fclose(fp);

					printf("\nCDB dump completed\n");

					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				}
				case 4 :
				{
					uint32_t i, addr, data;
					FILE *fp = fopen("SCVout.dat","w") ;
					BMVD_MemoryInfo stMemoryInfo;
					BMVD_GetMemoryInfo(g_hMvdCh_0, &stMemoryInfo) ;
					BDBG_ERR(("SCV Address Virtual = %p", stMemoryInfo.pui32ITBStartAddress));
					BMEM_ConvertAddressToOffset(g_hMemory, stMemoryInfo.pui32ITBStartAddress, &addr) ;
					BDBG_ERR(("SCV Address Physical = %08lx", addr));

					for(i=0; i<stMemoryInfo.ui32ITBSize/4; i++)
					{
						if( i && (i%4==0) )
						{
							fprintf(fp, "\n");
						}
						data = ((volatile uint32_t *)stMemoryInfo.pui32ITBStartAddress)[i] ;
						fprintf(fp, "%02x %02x %02x %02x ", (data>>24)&0xFF, (data>>16)&0xFF, (data>>8)&0xFF, (data)&0xFF) ;
					}
					fclose(fp);

					printf("\nITB dump completed\n");

					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				}
				case 5 :
				{
					BERR_Code       Err = BERR_SUCCESS;
					Err = FrameDumpFn();
					if(Err != BERR_SUCCESS)
						printf("Frame Dump Failed\n");

					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				}
				case 6 :
				{
					BMVD_RevisionInfo stRevInfo ;
					BERR_Code simrc = BMVD_GetRevision(g_hMvd,
											 &stRevInfo);
					BDBG_ERR(("******************  Version Info *********************"));
					BDBG_ERR(("*  MVD Version - %02d.%02d Dated : %04hx %04hx",
						stRevInfo.sMVD_version.ui16MajorVersion,
						stRevInfo.sMVD_version.ui16MinorVersion,
						stRevInfo.sMVD_version.ui16Year,
						stRevInfo.sMVD_version.ui16Date ));
					BDBG_ERR(("*  MTPMV Version - %02x.%02x Dated : %04hx %04hx",
						stRevInfo.sMTPMV_version.ui16MajorVersion,
						stRevInfo.sMTPMV_version.ui16MinorVersion,
						stRevInfo.sMTPMV_version.ui16Year,
						stRevInfo.sMTPMV_version.ui16Date ));
					BDBG_ERR(("*  MTPSV Version - %02x.%02x Dated : %04hx %04hx",
						stRevInfo.sMTPSV_version.ui16MajorVersion,
						stRevInfo.sMTPSV_version.ui16MinorVersion,
						stRevInfo.sMTPSV_version.ui16Year,
						stRevInfo.sMTPSV_version.ui16Date ));
					BDBG_ERR(("******************************************************"));
					if(simrc != BERR_SUCCESS)
					{
						printf("\nError: BMVD_GetRevision...\n");
					}

					BKNI_SetEvent( g_TaskComplete ) ;
					break;

				}
				case 7 :
					printf("\nTo Be implemented\n\n");
					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				case 8 :
				{

					bool	pbVideoDecoderDead= true;
					uint32_t ui32DelayCount=10;
					BERR_Code err;
					if (g_hMvdCh_0 == NULL)
					{
						pbVideoDecoderDead = true;
					}
					else
					{
						err = BMVD_GetVideoDecoderDead (g_hMvdCh_0,
												&pbVideoDecoderDead, /*[out] decoder status*/
												 ui32DelayCount /* [in] delay count in ms */
												);
					}

					if (pbVideoDecoderDead == true )
					{
						printf("\nVIDEO DECODER DEAD\n");
					}
					else
					{
						printf("\nVIDEO DECODER NOT DEAD\n");
					}
					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				}
				case 9 :
					printf("\nTo Be implemented\n\n");
					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				case 10:
					{
					uint32_t ulRDBAddress, regl;
					printf("\nMTPMV reg read \n\n");
					BTST_P_Scanf(pContext, "%x", &ulRDBAddress);
					regl = BREG_Read32(g_hRegister, ulRDBAddress) ;
					BDBG_ERR(("%08lx",  regl));
					BKNI_SetEvent( g_TaskComplete ) ;
					break;
					}
				case 11 :
					printf("\nTo Be implemented\n\n");
					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				case 12 :
				{
					bool bIPicFound = 0;
					BERR_Code simrc = BMVD_GetIPictureFoundStatus( g_hMvdCh_0, &bIPicFound) ;

					if(simrc != BERR_SUCCESS)
					{
						printf("\nError: BMVD_GetIPictureFoundStatus...\n");
					}
					else
					{
						printf("%s",bIPicFound?"I picture found":"I picture not found\n");
					}
					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				}
				case 13 :
				{
					#if 0
					uint32_t CDBOccupancy = 0 ;
					BMVD_GetCDBOccupancy(g_hMvdCh_0, &CDBOccupancy) ;
					printf("CDBOccupancy = %08x\n",CDBOccupancy);
					BKNI_SetEvent( g_TaskComplete ) ;
					#endif
					printf("\ncommand not correct. Enter correct comman\n\n");
					break;
				}
				case 14:
				{
						FILE             *pFileDebugReg ;
						int nRetVal;
						uint32_t regl;
						uint32_t ulReg;
						int AccessMode;
						uint32_t ulRDBAddress;
						char pRDBName[256];


						/* Now read all the registers that have been entered in the file "simdebug" */
						pFileDebugReg = fopen("simdebug","r");
						if(pFileDebugReg != NULL )
						{

							for(;;)
							{
								nRetVal = fBTST_P_Scanf(pContext, pFileDebugReg, " %d %s 0x%x \n", &AccessMode, pRDBName, &ulRDBAddress ) ;
								if( (nRetVal < 3) || (nRetVal == EOF) )
								{
									break ;
								}
								if(AccessMode == 16)
								{
									ulReg = BREG_Read16(g_hRegister, ulRDBAddress) ;
									BDBG_WRN(("%s = %04hx", pRDBName, ulReg));
								}
								else if(AccessMode == 32)
								{
									regl = BREG_Read32(g_hRegister, ulRDBAddress) ;
									BDBG_WRN(("%s = %08lx", pRDBName, regl));
								}
								else
								{
									BDBG_ERR(("Invalid AccessMode - neither 16 nor 32"));
								}
							}
							fclose(pFileDebugReg);
						}
						else
						{
							printf("File open failed\n");
						}
						BKNI_SetEvent( g_TaskComplete ) ;
						break;
					}
				case 15 :
				{
					printf("\ndumping dump_file..\n");
					dump_mvd(0, "dump_file") ;
					BKNI_SetEvent( g_TaskComplete ) ;


					break;
				}
				case 16 :
				{
						char es_file[50]="context0_es.bin";
						{
							BMVD_Handle MvdHandle = g_hMvd;
							BMVD_ChannelSettings MvdChannelSettings ;

							TestError( BMVD_GetChannelDefaultSettings(
									MvdHandle, /*MVD handle returned by BMVD_Open*/
									0,
									BMVD_ChannelConfig_eVideoHD,
									&MvdChannelSettings ),
								"ERROR:BMVD_GetChannelDefaultSettings" );

							MvdChannelSettings.eBufferScheme = BMVD_BufferScheme_eFourBufferScheme ;
							printf("DIRECT\n");
							MvdChannelSettings.bStillPictureMode = true; /*true;*/
							MvdChannelSettings.bStillDecodeDirectFeed = true; /*true; */

							TestError( BMVD_OpenChannel(
												MvdHandle,
												&g_hMvdCh_0,
												0,
												&MvdChannelSettings ),
								"ERROR:BMVD_OpenChannel" );
							hMvdChan0 = g_hMvdCh_0;
						}


						printf("\nDirect Feed from HDD..\n");
						TestError( BMVD_InstallInterruptCallback(
					 	g_hMvdCh_0,
					 	BMVD_Interrupt_eDecodedBufferReady,
					 	bsettop_p_dec_buf_ready_isr, NULL, 0),
					 	"ERROR: BMVD_InstallInterruptCallback(UserData)");

						BMVD_GetMemoryInfo(hMvdChan0, &mvd_meminfo);
						printf("*****CDBSTART= %x, ITBSTART=%x\n*****", mvd_meminfo.ui32CDBStartPhysOffset, mvd_meminfo.ui32ITBStartPhysOffset);

						fill_cdb(es_file, (uint32_t *)mvd_meminfo.pui32CDBStartAddress, mvd_meminfo.ui32CDBSize,  &cdb_totalbytes);
						fill_itb("context0_itb.bin", (uint32_t *)mvd_meminfo.pui32ITBStartAddress, mvd_meminfo.ui32ITBSize,  &itb_totalbytes,&Pic_Count);
						Pic_Count=20;

						rc = BMVD_GetVideoParams(hMvdChan0, &VideoParams);
						if (rc!=BERR_SUCCESS) {
							BDBG_ERR(("Ignored error from BMVD_GetVideoParams"));
						}
						BMVD_SetStillDecodeCdbItbDepth (hMvdChan0, cdb_totalbytes, itb_totalbytes,Pic_Count);
						VideoParams.eStreamType =  BAVC_StreamType_eEs;
						rc = BMVD_StartDecode(hMvdChan0, &VideoParams);
						if (rc!=BERR_SUCCESS)
							BDBG_ERR(("Ignored error from BMVD_Startdecode"));


					BKNI_SetEvent( g_TaskComplete ) ;

					break;
				}
				case 17 :
					printf("\ndumping dump_file..\n");
					dump_mvd_bin(0, "dump_file.dat") ;
					BKNI_SetEvent( g_TaskComplete ) ;
				break;
				case 18 :
				{
					bool 	pbVideoDecoderStarted ;
					BMVD_GetVideoDecoderState (g_hMvdCh_0, &pbVideoDecoderStarted);
					if(pbVideoDecoderStarted)
					{
						printf("Decoder is in start state");
					}
					else
					{
						printf("Decoder is not started");
					}
					BKNI_SetEvent( g_TaskComplete ) ;
					break;
				}


				default:
					printf("\ncommand not correct. Enter correct comman\n\n");
					BKNI_SetEvent( g_TaskComplete ) ;
			}
		}
	}

	return 0;

}


/*----------------Frame Dump Code----------------------*/

BERR_Code FrameDumpFn( )
{
	BERR_Code               err = BERR_SUCCESS;
	uint32_t reg ;
	int autoAddress ;
	int i, numBuffers ;

	char pImageFileName[512], *pPtrImageFilename;
	int nImageFileFormat =0;
	int width;
	int height;
	int nmbx;
	int picstruct=1;
	int fieldtype=0;
	int eChromaType=1;
	uint32_t luma_start[4];
	uint32_t chroma_start[4];


	/***************************************************************/
	printf("********************************************************************\n\n\n") ;
	reg = BREG_Read32(g_hRegister, BCHP_SUN_TOP_CTRL_PROD_REVISION ) ;
	printf("BCHP_SUN_TOP_CTRL_PROD_REVISION=%x\n", reg) ;

	printf("Enter Filename : ");
	BTST_P_Scanf(pContext, "%s",pImageFileName);

	nmbx = BREG_Read32(g_hRegister,BCHP_MTP_MMTV_NMBxValue_0);
	printf("NMBX= %3x\n",nmbx);

	printf("Get Buffer Addresses Automatically ? 0=No, 1=yes ");
	BTST_P_Scanf(pContext, "%d", &autoAddress) ;

	if(autoAddress)
	{
		uint32_t addrPtr = BREG_Read32( g_hRegister, BCHP_MTP_MMTV_BuffAddrTblPtr_0);
		printf("addrPtr = %04x\n",addrPtr) ;
		numBuffers = 4 ;
		for(i=0;i<numBuffers;i++)
		{
			luma_start[i] = (BREG_Read32(g_hRegister, ConvertMTPMVtoCPUAdrs(addrPtr+i*2+1) )<<16) |
								BREG_Read32(g_hRegister, ConvertMTPMVtoCPUAdrs(addrPtr+i*2) )  ;
			luma_start[i] <<= 3;
			chroma_start[i] = luma_start[i] + (BREG_Read32(g_hRegister, BCHP_MTP_MMTV_32_BM_COMMON_CHROMA_OFFSET_LOW_0) << 3) ;
			printf("Luma[%d] = %08x \t|\t Chroma[%d] = %08x\n", i, luma_start[i], i, chroma_start[i]) ;
		}
	}
	else
	{
		printf("Enter Luma Base Address Hex and Physical Byte Address : ");
		BTST_P_Scanf(pContext, "%u",&luma_start[0]);
		printf("Enter Chroma Base Address Hex and Physical Byte Address : ");
		BTST_P_Scanf(pContext, "%u",&chroma_start[0]);
		numBuffers = 1 ;
	}

	printf("\nIs this picture HD ? 0=SD, 1=HD : ");
	BTST_P_Scanf(pContext, "%d", &autoAddress) ;

	if (autoAddress) /* HD */
	{
		width = 1920;
		height= 1088;
	}
	else /* SD */
	{
		width = 720;
		height= 480;
	}

	pPtrImageFilename = pImageFileName;

	for(i=0; i<numBuffers; i++)
	{
		if(numBuffers>1)
		{
			sprintf(pImageFileName+100, "%s%d",pImageFileName, i);
			printf("Filename = %s\n", pImageFileName+100);
			pPtrImageFilename = pImageFileName+100;
		}
		SimCaptureFrames(
			g_hMemory,
			pPtrImageFilename,
			nImageFileFormat,
			luma_start[i],
			chroma_start[i],
			width,
			height,
			nmbx,
			picstruct,
			fieldtype,
			eChromaType
			);
	}
	/* cleanup all modules */
	printf("End testapp. Shutting down...\n");

/*Done:*/
	/* return status */
	return err;
}


int SimCaptureFrames(
	BMEM_Handle MemHandle,
	char *pImageFileName,
	int nImageFileFormat,
	int luma_start,
	int chroma_start,
	int width,
	int height,
	int nmbx,
	int picstruct,
	int fieldtype,
	int eChromaType
	)
{
	FILE *fpPPM=0, *fpY=0, *fpU=0, *fpV=0 ;
	char PPMFileName[256] ;
	char YFileName[256], UFileName[256], VFileName[256] ;
	BSTD_UNUSED(MemHandle);

	fprintf(stderr,"\nInside SimCaptureFrames");
	BDBG_ERR(("Uploading the frame"));

	if(nImageFileFormat == 0)
	{
		sprintf(PPMFileName, "%s.ppm", pImageFileName) ;
	}
	else if(nImageFileFormat == 1)
	{
		sprintf(YFileName, "%s.Y", pImageFileName) ;
		sprintf(UFileName, "%s.U", pImageFileName) ;
		sprintf(VFileName, "%s.V", pImageFileName) ;
	}
	else if(nImageFileFormat == 2 || nImageFileFormat == 3)
	{
		sprintf(PPMFileName, "%s.YUV", pImageFileName) ;
	}
	else
	{
		BDBG_ERR(("Error Image File Format type"));
		return 2;
	}

	BDBG_ERR(("MTP: Luma Start %08lx Chroma Start %08lx Picstruct = %d fieldtype=%d\n", luma_start, chroma_start, picstruct, fieldtype));
	BDBG_ERR(("MTP: Sizes: Horiz = %d, Vert = %d\n", width, height));
	BDBG_ERR(("MTP: NMBx = %d\n", nmbx));
	ChromaType = eChromaType ;
	BDBG_ERR(("MTP: ChromaType = %d\n", ChromaType));

	/* Open the PPM file */
	if(nImageFileFormat == 0)
	{
		if ((fpPPM=fopen(PPMFileName, "w"))==NULL)
		{
			BDBG_ERR(("Unable to open file %s for writing", PPMFileName));
			return 2;
		}
		fprintf(fpPPM, "P6\n%d %d\n255\n", width, height/picstruct);
		fclose(fpPPM);
		fpPPM = fopen(PPMFileName, "ab");
		if(fpPPM == NULL)
		{
			BDBG_ERR(("Error Opening file %s for Write ", PPMFileName));
			return 2;
		}
	}
	else if(nImageFileFormat == 1)
	{
		if ((fpY=fopen(YFileName, "wb"))==NULL)
		{
			BDBG_ERR(("Unable to open file %s for writing", YFileName));
			return 2;
		}
		if ((fpU=fopen(UFileName, "wb"))==NULL)
		{
			BDBG_ERR(("Unable to open file %s for writing", UFileName));
			return 2;
		}
		if ((fpV=fopen(VFileName, "wb"))==NULL)
		{
			BDBG_ERR(("Unable to open file %s for writing", VFileName));
			return 2;
		}
	}
	else if(nImageFileFormat == 2 || nImageFileFormat == 3)
	{
		if ((fpPPM=fopen(PPMFileName, "wb"))==NULL)
		{
			BDBG_ERR(("Unable to open file %s for writing", PPMFileName));
			return 2;
		}
	}

	/* Capture the Previous frame using the old pointers */
#if defined(MVD_TEST)
	fnDramImgToPPM(luma_start,
				   chroma_start,
				   width,
				   height,
				   nmbx,
				   picstruct,
				   fieldtype,
				   nImageFileFormat,
				   fpPPM,
				   fpY,
				   fpU,
				   fpV,
				   MemHandle
				  );

#endif
	if(nImageFileFormat == 0)
	{
		fclose(fpPPM);
	}
	else if(nImageFileFormat == 1)
	{
		fclose(fpY);
		fclose(fpU);
		fclose(fpV);
	}
	else if(nImageFileFormat == 2 || nImageFileFormat == 3)
	{
		fclose(fpPPM);
	}

	return 0 ;
}


/* end of file */
