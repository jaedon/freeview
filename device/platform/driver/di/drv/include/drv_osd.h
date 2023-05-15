/**
 * drv_osd.h
*/

#ifndef __DRV_OSD_H__
#define __DRV_OSD_H__

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "drv_err.h"
#include "di_osd.h"
#include "nexus_graphics2d.h"

#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_FACTORY)
#include "platform_init.h"
#include "directfb.h"
#endif

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/
#define OSD_WIDTH			1280
#define OSD_HEIGHT			720

#define OSD_SD_WIDTH		720
#define OSD_SD_HEIGHT		576

// BCM 은 GRAPHIC PLANE 이 "1" 개만 존재하나, 필요에 따라, virtual N 개 Plane (Layer) 를 개념적으로 사용가능하다.
// 단, 각 Layer 에 surface 는 "1" 개 씩 만 Create 할 수 있다.
#if		defined(CONFIG_OSD_LAYER_MAX_5)
#define	DI_OSD_LAYER_MAX 5
#elif	defined(CONFIG_OSD_LAYER_MAX_4)
#define	DI_OSD_LAYER_MAX 4
#elif	defined(CONFIG_PRODUCT_IMAGE_HWTEST)
//added by dsshin(01347)
#define DI_OSD_LAYER_MAX 4
#else
#define	DI_OSD_LAYER_MAX 3
#endif

#define	DI_OSD_SURFACE_MAX_NUM DI_OSD_LAYER_MAX

#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
#define	DI_OSD_BG_LAYER					(DI_OSD_LAYER_MAX-1)
#endif
/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/
typedef enum tagDI_OSD_STATUS
{
	DI_OSD_DISABLED = 0,
	DI_OSD_SURFACE_ENABLED,
	DI_OSD_FB_ENABLED
} DI_OSD_STATUS;

#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
typedef enum tagDI_OSD_BG_STATUS
{
	DI_OSD_BG_STATUS_NONE = 0,		// BG 위로 가려지는 plane 없음.
	DI_OSD_BG_STATUS_FULL,			// BG 위로 plane 이 가득 차서 가려짐. 즉, BG Plane clear
	DI_OSD_BG_STATUS_NONE_FULL		// BG 위로 plane 이 일정 영역만 가려짐. 즉, BG 영역 만큼 clear
} DI_OSD_BG_STATUS;
#endif

typedef struct tagDI_OSD_INSTANCE {
	DI_OSD_STATUS			status;
	DI_OSD_LAYER			layer;
	DI_OSD_RECT				rect;
	DI_OSD_PIXEL_FORMAT		format;
#if defined(CONFIG_DIRECTFB) && defined(CONFIG_PRODUCT_IMAGE_HWTEST) && !defined(CONFIG_PRODUCT_IMAGE_FACTORY)
	IDirectFBSurface		*surface;
	int						pitch;
	HBOOL					bIsLock;
#else
	NEXUS_SurfaceHandle 	surface;
#endif
	void 					*pFBAddr;
#if defined(CONFIG_SCALABLE_OSD_SURFACE) || defined(CONFIG_BACKGROUND_OSD_SURFACE)
	HBOOL					bOrgExist;
	HBOOL					bOrgDirty;
#endif
#if defined(CONFIG_SCALABLE_OSD_SURFACE)
	DI_OSD_RECT				In_rect;		// SetInOut 처리시 In(source) Window Rect
	DI_OSD_RECT				Out_rect;		// SetInOut 처리시 Out(destination) Window Rect
#endif
#if defined(CONFIG_BACKGROUND_OSD_SURFACE)
	DI_OSD_RECT				BG_rect;		// Background Plane 처리시, upper video plane 에 의해 가려지는 Rect
	HBOOL					bUseBG;
	DI_OSD_BG_STATUS		nBG_Status;
#endif
} DI_OSD_INSTANCE;

/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/
DRV_Error DRV_OSD_Init(void);
DRV_Error DRV_OSD_ResetDisplayFormatToHD(void);
DRV_Error DRV_OSD_SetDisplayFormatToHD(void);
DRV_Error DRV_OSD_SetDisplayFormatToHDForDFB(NEXUS_VideoFormat format);
DRV_Error DRV_OSD_ResetDisplayFormatToSD(void);
DRV_Error DRV_OSD_SetDisplayFormatToSD(void);
DRV_Error DRV_OSD_GetGraphics2DHandle(NEXUS_Graphics2DHandle* pGraphics2DHandle);
DRV_Error DRV_OSD_CopyImage(NEXUS_SurfaceHandle hSrcNexusSurfHandle, NEXUS_SurfaceHandle hDstNexusSurfHandle);
DRV_Error DRV_OSD_Refresh(DI_OSD_LAYER nOsdLayer);
DRV_Error DRV_OSD_GetOsdHandle(DI_OSD_LAYER nOsdLayer, DI_OSD_HANDLE *pOsdHandle);
DRV_Error DRV_OSD_AcquireSemaphore(void);
DRV_Error DRV_OSD_ReleaseSemaphore(void);
void DRV_OSD_PrintInfo(void);
DRV_Error DRV_OSD_GetFB(int nFB_Num, DI_OSD_HANDLE *pOsdHandle);
DRV_Error DRV_OSD_SetFilterCoeffs(int lFilterSelect);
DRV_Error DRV_OSD_BlendVisibleSurface(NEXUS_SurfaceHandle hDstNexusSurfHandle);
#if defined(CONFIG_DIRECTFB) && !defined(CONFIG_PRODUCT_IMAGE_FACTORY)
DRV_Error DRV_OSD_GetDisplayHandle(HUINT32 ulDeviceId, NEXUS_DisplayHandle *dispHD, NEXUS_DisplayHandle *dispSD);
#endif
void DRV_OSD_Sync(void);

#endif /* !__DRV_OSD_H__ */

