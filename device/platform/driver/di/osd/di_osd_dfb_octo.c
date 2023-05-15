/*
 * $Header:   $
 */
/*********************************************************************
 * $Workfile:   di_osd.c  $
 * $Modtime:   Jul 16 2007 01:07:04  $
 *
 * Author:
 * Description:
 *
 *                                 Copyright (c) 2008 HUMAX Co., Ltd.
 *                                               All rights reserved.
 ********************************************************************/

/*********************************************************************
 * Header Files
 *********************************************************************/
#include <directfb.h>
#include "platform_init.h"

#include "linden_trace.h"

#include "htype.h"
#include "di_err.h"
#include "di_uart.h"
#include "di_video.h"
#include "drv_video.h"
#include "di_osd.h"
#include "drv_osd.h"
#include "vkernel.h"
#include "taskdef.h"

#include "nexus_platform.h"
#include "nexus_surface.h"
#include "nexus_graphics2d.h"
#include "nexus_display.h"

#define DEBUG_MODULE				TRACE_MODULE_DI_OSD

#define UNUSED(x) 					(void)(x)
#define DI_OSD_FB_MAX_NUM			(1000)

NEXUS_Graphics2DHandle 		nexus_gfx_2DHandle;

IDirectFB			   		*dfb;
IDirectFBScreen		 		*primary_screen;
static unsigned long 		s_ulOsdSemId;
static DI_OSD_INSTANCE 		s_aOsdFBInstance[DI_OSD_FB_MAX_NUM];
static BKNI_EventHandle		s_hOsdGfxEvent;

STATIC DI_ERR_CODE _di_osd_InitDirectFB( int argc, char *argv[] )
{
	DFBResult 				err;

	err = DirectFBInit( &argc, &argv );
	if (err) {
		DirectFBError( "DirectFBInit", err );
		return DI_ERR_ERROR;
	}

	/* create the super interface */
	err = DirectFBCreate( &dfb );
	if (err) {
		DirectFBError( "DirectFBCreate", err );
		return DI_ERR_ERROR;
	}
	

	return DI_ERR_OK;
}

/*********************************************************************
 *
 * Function 	P_OSD_CheckHandle
 * Description	This function sets the alpha value of the specified OSD plane.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
static DI_ERR_CODE P_OSD_CheckHandle(DI_OSD_HANDLE hOsdHandle)
{
	if (hOsdHandle)	// null handle check
	{
		DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

		if( pOsdInstance->surface)
		{
			return DI_ERR_OK;
		}
	}

	return DI_ERR_ERROR;
}

static void P_OSD_gfxEventComplete(void *data, int unused)
{
    BSTD_UNUSED(unused);
    
    BKNI_SetEvent((BKNI_EventHandle)data);
}

static void P_OSD_gfxEngineSync(void)
{
	NEXUS_Error rc;
	BERR_Code err = BERR_SUCCESS;

	do
	{
		rc = NEXUS_Graphics2D_Checkpoint(nexus_gfx_2DHandle, NULL);
		if (rc == NEXUS_GRAPHICS2D_QUEUED)
		{
			rc = BKNI_WaitForEvent(s_hOsdGfxEvent, 1000);
			DONE_ERR((rc != BERR_SUCCESS));
		}
	} 
	while (rc == NEXUS_GRAPHICS2D_QUEUE_FULL);

	DONE_ERR((rc == NEXUS_GRAPHICS2D_BUSY));
done:
	return;
}

DI_ERR_CODE	DI_OSD_Init( int argc, char *argv[] )
{
	DI_ERR_CODE					dErr = DI_ERR_OK;
	DRV_Error					nRet = DRV_OK;
	NEXUS_Graphics2DSettings 	gfxSettings;

	dErr = _di_osd_InitDirectFB(argc, argv);
	if(dErr != DI_ERR_OK)
	{
		return DI_ERR_ERROR;
	}

	nexus_gfx_2DHandle = NEXUS_Graphics2D_Open(0, NULL);
	if( nexus_gfx_2DHandle == NULL)
	{
		PrintError("NEXUS_Graphics2D_Open error!!\n");
		return DRV_ERR;
	}

	nRet = VK_SEM_Create(&s_ulOsdSemId, "di_osd", DEFAULT_SUSPEND_TYPE);
	if( nRet != VK_OK )
	{
		PrintError("Error(0x%x) in VK_SEM_Create()\n\r", nRet);
		return DRV_ERR;
	}
	
	BKNI_CreateEvent(&s_hOsdGfxEvent);
	NEXUS_Graphics2D_GetSettings(nexus_gfx_2DHandle, &gfxSettings);
	gfxSettings.checkpointCallback.callback = P_OSD_gfxEventComplete;
	gfxSettings.checkpointCallback.context = s_hOsdGfxEvent;
	NEXUS_Graphics2D_SetSettings(nexus_gfx_2DHandle, &gfxSettings);

	return DI_ERR_OK;
}

void DI_OSD_SetDFB(void* directfb)
{
	dfb->Release(dfb);

	dfb = (IDirectFB *)directfb;
	
	return;
}

/*********************************************************************
 *
 * Function 	DI_OSD_CreateFrameBuffer
 * Description	This function makes a new screen on an specified plane with the specified
 *				position and size.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_CreateFrameBuffer( unsigned long width, unsigned long height,
										DI_OSD_PIXEL_FORMAT format,
										DI_OSD_HANDLE *pOsdHandle)
{
	int 				i;
	NEXUS_SurfaceCreateSettings createSettings;
	NEXUS_SurfaceHandle surface;
	NEXUS_SurfaceMemory frameBufferMem;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		for( i = 0; i < DI_OSD_FB_MAX_NUM; i++)
		{
			if( s_aOsdFBInstance[i].status == DI_OSD_DISABLED)
			{
				break;
			}
		}
		if( i == DI_OSD_FB_MAX_NUM)
		{
			PrintError("Not enough FB resource!!\n");
			nErr= DI_ERR_NO_RESOURCE;
			goto done;
		}

		/* Make Window */

		NEXUS_Surface_GetDefaultCreateSettings(&createSettings);
#if defined(CONFIG_DEVICE_MODE_KERNEL) || defined (CONFIG_OS_UCOS)
		createSettings.heap = NEXUS_Platform_GetFramebufferHeap(NEXUS_OFFSCREEN_SURFACE);
#endif
		switch( format)
		{
			case DI_OSD_PIXEL_FORMAT_ARGB_8888:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA8_R8_G8_B8;
				break;
			case DI_OSD_PIXEL_FORMAT_RGBA_8888:
				createSettings.pixelFormat = NEXUS_PixelFormat_eR8_G8_B8_A8;
				break;
			case DI_OSD_PIXEL_FORMAT_ARGB_4444:		// For MHEG Bitmap Pixel Format
				createSettings.pixelFormat = NEXUS_PixelFormat_eA4_R4_G4_B4;
				break;
			case DI_OSD_PIXEL_FORMAT_RGBA_4444:		// For MHEG Bitmap Pixel Format
				createSettings.pixelFormat = NEXUS_PixelFormat_eR4_G4_B4_A4;
				break;
			case DI_OSD_PIXEL_FORMAT_ARGB_1555:
				createSettings.pixelFormat = NEXUS_PixelFormat_eA1_R5_G5_B5;
				break;
			case DI_OSD_PIXEL_FORMAT_CLUT_8:
				createSettings.pixelFormat = NEXUS_PixelFormat_ePalette8;
				break;
			case DI_OSD_PIXEL_FORMAT_A_8:			// For MHEG Mask Pixel Format
				createSettings.pixelFormat = NEXUS_PixelFormat_eA8;
				break;
			default:
				PrintError("Not supported color format(%d)\n", format);
				nErr= DI_ERR_INVALID_PARAM;
				goto done;
				break;
		}

		createSettings.width = width;
		createSettings.height = height;

		surface = NEXUS_Surface_Create(&createSettings);
		if( surface == NULL)
		{
			PrintError("NEXUS_Surface_Create error!! (i:%d, width=%d, height=%d)\n", i, width, height);
			DRV_OSD_PrintInfo();
			nErr= DI_ERR_NO_RESOURCE;
			goto done;
		}

		NEXUS_Surface_GetMemory(surface, &frameBufferMem);

		s_aOsdFBInstance[i].status = DI_OSD_FB_ENABLED;
		s_aOsdFBInstance[i].rect.x = 0;
		s_aOsdFBInstance[i].rect.y = 0;
		s_aOsdFBInstance[i].rect.width = width;
		s_aOsdFBInstance[i].rect.height = height;
		s_aOsdFBInstance[i].surface = surface;
		s_aOsdFBInstance[i].format = format;
		s_aOsdFBInstance[i].pFBAddr = frameBufferMem.buffer;
		s_aOsdFBInstance[i].layer = DI_OSD_LAYER_0;

		*pOsdHandle = (DI_OSD_HANDLE)&s_aOsdFBInstance[i];

		PrintDebug("FB[%d]=Handle:0x%x, w(%d),h(%d), format(%d)\n", i, *pOsdHandle, width, height, format);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return nErr;
}

/*********************************************************************
 *
 * Function 	DI_OSD_DeleteFrameBuffer
 * Description	This function destroys a screen created by DI_OSD_CreateScreen.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_DeleteFrameBuffer(DI_OSD_HANDLE hOsdHandle)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;
	DI_ERR_CODE	nErr = DI_ERR_OK;

	PrintEnter();

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
		{
			PrintError("P_OSD_CheckHandle error!!\n");
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		if( pOsdInstance->status != DI_OSD_FB_ENABLED)
		{
			PrintError("Status(%d) error!!\n", pOsdInstance->status);
			nErr = DI_ERR_INVALID_PARAM;
			goto done;
		}

		NEXUS_Surface_Destroy(pOsdInstance->surface);

		pOsdInstance->status = DI_OSD_DISABLED;
		pOsdInstance->rect.x = 0;
		pOsdInstance->rect.y = 0;
		pOsdInstance->rect.width = 0;
		pOsdInstance->rect.height = 0;
		pOsdInstance->surface = NULL;
		pOsdInstance->format = DI_OSD_PIXEL_FORMAT_ARGB_8888;
		pOsdInstance->layer = DI_OSD_LAYER_MAX;
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		nErr= DI_ERR_ERROR;
	}

done:
	VK_SEM_Release(s_ulOsdSemId);
	PrintExit();

	return DI_ERR_OK;
}


/*********************************************************************
 *
 * Function 	DI_OSD_GetFrameBuffer
 * Description
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DI_ERR_CODE	DI_OSD_GetFrameBuffer(DI_OSD_HANDLE hOsdHandle, void **pFrameBuffer)
{
	DI_OSD_INSTANCE	*pOsdInstance = (DI_OSD_INSTANCE *)hOsdHandle;

	PrintEnter();

	if( P_OSD_CheckHandle( hOsdHandle) != DI_ERR_OK)
	{
		PrintError("P_OSD_CheckHandle error!!\n");
		return DI_ERR_INVALID_PARAM;
	}

	*pFrameBuffer = pOsdInstance->pFBAddr;

	PrintExit();

	return DI_ERR_OK;
}


DRV_Error	DRV_OSD_Init( void )
{
	DI_ERR_CODE	dErr = DI_ERR_OK;

	dErr = DI_OSD_Init(0, NULL);
	if(dErr != DI_ERR_OK)
	{
		return DRV_ERR;
	}

	return DRV_OK;
}

DRV_Error DRV_OSD_GetGraphics2DHandle(NEXUS_Graphics2DHandle* pGraphics2DHandle)
{
	PrintEnter();

	if (nexus_gfx_2DHandle == 0)
	{
		PrintError("s_OsdList.nexus_gfx_2DHandle is NULL~~!!!\n");
		return DRV_ERR;
	}

	*pGraphics2DHandle=nexus_gfx_2DHandle;

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_OSD_GetDisplayHandle(HUINT32 ulDeviceId, NEXUS_DisplayHandle *dispHD, NEXUS_DisplayHandle *dispSD)
{
	DRV_Error err = DRV_OK;

	err = DRV_VIDEO_GetDisplayHandle(ulDeviceId, dispHD, dispSD);

	return err;
}

/*********************************************************************
 *
 * Function 	DRV_OSD_SetDisplayFormat
 * Description	This function set display format to osd (graphic) when display format change.
 * Inputs
 * Outputs
 * Returns
 *
 *********************************************************************/
DRV_Error DRV_OSD_SetDisplayFormatToHD(void)
{
	NEXUS_DisplaySettings 	displaySettings;
	DRV_Error				nRet = DRV_OK;
	DFBScreenEncoderConfig	encoderCfg;
	NEXUS_DisplayHandle 	HD_DispHandle;
	NEXUS_DisplayHandle 	SD_DispHandle;
	
	DFBResult				err;

	err = dfb->GetScreen( dfb, DSCID_PRIMARY, &primary_screen );

	nRet = DRV_OSD_GetDisplayHandle(0, &HD_DispHandle, &SD_DispHandle);
	if (nRet != DRV_OK)
	{
		PrintError("DRV_OSD_GetDisplayHandle error!!\n");
		return DRV_ERR;
	}

	if(HD_DispHandle != NULL)
	{

		// HD Display
		NEXUS_Display_GetSettings(HD_DispHandle, &displaySettings);

		PrintDebug("HD Disp format(0x%x)\n",displaySettings.format);

		err = primary_screen->GetEncoderConfiguration(primary_screen, 0, &encoderCfg);
		if (err) {
			DirectFBError( "DirectFBInit", err );
			return DRV_ERR;
		}
		
		encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS |
					DSECONF_SCANMODE | DSECONF_FREQUENCY | DSECONF_RESOLUTION );
		encoderCfg.tv_standard = DSETV_DIGITAL;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1			
		encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI | DSOC_COMPONENT);
#else
		encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif
		switch ( displaySettings.format )
		{
#if defined(CONFIG_HDOUT_2160P)
			case NEXUS_VideoFormat_e3840x2160p24hz:
				encoderCfg.frequency = DSEF_24HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1			
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif	
				break;
			case NEXUS_VideoFormat_e3840x2160p25hz:
				encoderCfg.frequency = DSEF_25HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1			
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif	
				break;
			case NEXUS_VideoFormat_e3840x2160p30hz:
				encoderCfg.frequency = DSEF_30HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1			
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif	
				break;			
			case NEXUS_VideoFormat_e3840x2160p50hz:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1			
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif				
				break;
			case NEXUS_VideoFormat_e3840x2160p60hz:			
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_3840_2160;
#if NEXUS_NUM_COMPONENT_OUTPUTS >= 1			
				encoderCfg.out_connectors = (DFBScreenOutputConnectors)(DSOC_HDMI);
#endif				
				break;
#endif		
			case NEXUS_VideoFormat_e1080p24hz:
				encoderCfg.frequency = DSEF_24HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080i50hz:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_INTERLACED;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080i:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_INTERLACED;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080p50hz:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e1080p:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1920_1080;
				break;
			case NEXUS_VideoFormat_e720p50hz:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1280_720;
				break;
			case NEXUS_VideoFormat_e720p:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_1280_720;
				break;
			case NEXUS_VideoFormat_ePal:
			case NEXUS_VideoFormat_ePalI:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_INTERLACED;
				encoderCfg.resolution = DSOR_720_576;
				encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
				encoderCfg.tv_standard = DSETV_PAL;
				break;
			case NEXUS_VideoFormat_e576p:
				encoderCfg.frequency = DSEF_50HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_720_576;
				break;
			case NEXUS_VideoFormat_eNtsc:
			case NEXUS_VideoFormat_eNtsc443:
			case NEXUS_VideoFormat_eNtscJapan:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_INTERLACED;
				encoderCfg.resolution = DSOR_640_480;
				encoderCfg.flags   = (DFBScreenEncoderConfigFlags)(DSECONF_TV_STANDARD | DSECONF_CONNECTORS);
				encoderCfg.tv_standard = DSETV_NTSC;
				break;
			case NEXUS_VideoFormat_e480p:
				encoderCfg.frequency = DSEF_60HZ;
				encoderCfg.scanmode = DSESM_PROGRESSIVE;
				encoderCfg.resolution = DSOR_640_480;
				break;
			default:
				PrintError("Not supported HD disp format(0x%x)\n", displaySettings.format);
				return DRV_ERR_INVALID_PARAMETER;
		}
		
		err = primary_screen->SetEncoderConfiguration(primary_screen, 0, &encoderCfg);
		if (err) {
			DirectFBError( "DirectFBInit", err );
			return DRV_ERR;
		}

		PrintDebug("encoderCfg %x, %x, %x, %x, %x, %x\n", encoderCfg.flags, encoderCfg.tv_standard, encoderCfg.out_connectors,
				encoderCfg.frequency, encoderCfg.scanmode, encoderCfg.resolution);
	}

	return DRV_OK;
}

DRV_Error DRV_OSD_CopyImage(NEXUS_SurfaceHandle hSrcNexusSurfHandle, NEXUS_SurfaceHandle hDstNexusSurfHandle)
{
	NEXUS_Error rc;
	NEXUS_Graphics2DBlitSettings blitSettings;

	PrintEnter();

	if( (hSrcNexusSurfHandle == NULL) || (hDstNexusSurfHandle == NULL) )
	{
		PrintError("Surface Handle is NULL~!\n");
		return DRV_ERR;
	}

	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
	{
		NEXUS_Graphics2D_GetDefaultBlitSettings(&blitSettings);
		blitSettings.source.surface = hSrcNexusSurfHandle;
		blitSettings.output.surface = hDstNexusSurfHandle;
		blitSettings.colorOp = NEXUS_BlitColorOp_eCopySource;
		blitSettings.alphaOp = NEXUS_BlitAlphaOp_eCopyConstant; /* YCrCb has no alpha, so we must set 0xFF */
		blitSettings.constantColor = 0xFF000000; /* alpha is opaque */

		rc = NEXUS_Graphics2D_Blit( nexus_gfx_2DHandle, &blitSettings );
		if(rc != NEXUS_SUCCESS) 
		{
			PrintError("NEXUS_Graphics2D_Blit ERROR code : 0x%x\n",rc);
			VK_SEM_Release(s_ulOsdSemId);
			return DRV_ERR;
		}
		
		P_OSD_gfxEngineSync();

		VK_SEM_Release(s_ulOsdSemId);
	}
	else
	{
		PrintError("VK_SEM_Get() Error~!\n");
		return DRV_ERR;
	}

	PrintExit();

	return DRV_OK;
}

DRV_Error DRV_OSD_BlendVisibleSurface(NEXUS_SurfaceHandle hDstNexusSurfHandle)
{
	PrintEnter();
	
	UNUSED(hDstNexusSurfHandle);
	
	PrintExit();
	return DRV_OK;
}

DRV_Error DRV_OSD_AcquireSemaphore(void)
{
	if(VK_SEM_Get(s_ulOsdSemId) == VK_OK)
		return DRV_OK;
	else
		return DRV_ERR;
}

DRV_Error DRV_OSD_ReleaseSemaphore(void)
{
	if(VK_SEM_Release(s_ulOsdSemId) == VK_OK)
		return DRV_OK;
	else
		return DRV_ERR;
}

void DRV_OSD_PrintInfo(void)
{
	/* OSD Plane Info */
	PrintDebug("\n\tOSD Infomation\n");

	return;
}

void DRV_OSD_Sync(void)
{
	P_OSD_gfxEngineSync();
}



