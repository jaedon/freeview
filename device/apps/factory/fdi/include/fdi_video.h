/******************************************************************************
* $Workfile:$
* $Modtime:$
* Author :
* description :		 Factory Error Code Definition
*
* Copyright (c) 2008 by Humax Co., Ltd.
* All right reserved
******************************************************************************/
#ifndef _FDI_VIDEO_H_
#define _FDI_VIDEO_H_

/******************************************************************************
* header and definitions
******************************************************************************/
#include "fdi_err.h"
#include "ftype.h"

/******************************************************************************
* Definitions, typdef & const data
******************************************************************************/
typedef enum
{
	FDI_VIDEO_FORMAT_MPEG1			= 0x00000001,	/**< MPEG1 Video */
	FDI_VIDEO_FORMAT_MPEG2			= 0x00000002,	/**< MPEG2 Video */
	FDI_VIDEO_FORMAT_MPEG2DTV		= 0x00000004,	/**< MPEG2DTV Video */
	FDI_VIDEO_FORMAT_MPEG2DTV_PES	= 0x00000008,	/**< MPEG2DTV_PES Video */
	FDI_VIDEO_FORMAT_MPEG4Part2		= 0x00000010,	/**< MPEG4 Part2 Video */
	FDI_VIDEO_FORMAT_H261				= 0x00000020,	/**< H.261 Video */
	FDI_VIDEO_FORMAT_H263				= 0x00000040,	/**< H.263 Video */
	FDI_VIDEO_FORMAT_H264				= 0x00000080,	/**< H.264 Video */
	FDI_VIDEO_FORMAT_VC1SimpleMain		= 0x00000100,	/**< VC1 Simple Main Video */
	FDI_VIDEO_FORMAT_VC1 				= 0x00000200,	/**< VC1 Video */
	FDI_VIDEO_FORMAT_AVS 				= 0x00000400,	/**< AVS Video */
	FDI_VIDEO_FORMAT_H265				= 0x00000800	/**< H.265 Video */
} FDI_VIDEO_FORMAT;

typedef enum {
	FDI_VIDEO_RESOLUTION_480I = 0,
	FDI_VIDEO_RESOLUTION_480P,
	FDI_VIDEO_RESOLUTION_576I,
	FDI_VIDEO_RESOLUTION_576P,
	FDI_VIDEO_RESOLUTION_720P,
	FDI_VIDEO_RESOLUTION_1080I,
	FDI_VIDEO_RESOLUTION_720P_50HZ,
	FDI_VIDEO_RESOLUTION_1080I_50HZ,
	FDI_VIDEO_RESOLUTION_480I_J,
	FDI_VIDEO_RESOLUTION_1080P,
	FDI_VIDEO_RESOLUTION_1080P50,
	FDI_VIDEO_RESOLUTION_MAX
} FDI_VIDEO_RESOLUTION;

typedef enum
{
	FDI_VIDEO_ASPECTRATIO_4_3 = 0,
	FDI_VIDEO_ASPECTRATIO_4_3_LETTER,
	FDI_VIDEO_ASPECTRATIO_16_9,
	FDI_VIDEO_ASPECTRATIO_14_9,
	FDI_VIDEO_ASPECTRATIO_1_1,
	FDI_VIDEO_ASPECTRATIO_MAX
} FDI_VIDEO_AspectRatio;

typedef	enum
{
	FDI_VIDEO_NOTIFY_EVT_SEQUENCE_HEADER_INFO_CHANGED,		/**< Video sequence info event is changed */
	FDI_VIDEO_NOTIFY_EVT_FORMAT_CHANGED,						/**< Video format event is changed */
	FDI_VIDEO_NOTIFY_EVT_RUNNING_STATE_CHANGED,				/**< Video running state is changed */
	FDI_VIDEO_NOTIFY_EVT_AFD_CHANGED,						/**< Video afd info event is changed */
	FDI_VIDEO_NOTIFY_EVT_END
} FDI_VIDEO_NOTIFY_EVT;

typedef enum
{
	FDI_VIDEO_RUNNING_STATE_UNKNOWN = 0,				/**< unknown video state */
	FDI_VIDEO_RUNNING_STATE_DECODING ,				/**< decoding video state */
	FDI_VIDEO_RUNNING_STATE_NODATA					/**< nodata video state */
} FDI_VIDEO_RUNNING_STATE;

typedef struct
{
	FDI_VIDEO_RUNNING_STATE eState;
} FDI_VIDEO_RUNNING_STATE_EVT_t;

typedef	struct
{
	unsigned long			ulDecId;
	FDI_VIDEO_NOTIFY_EVT	eEvent;
	void					*pvEventData; /* Sequence Header Changed, Format Changed, Running State Changed */
} FDI_VIDEO_CALLBACK_DATA_t;

typedef struct
{
	BOOL bCVBS;				// composite
	BOOL bSVHS;				// s-video
	BOOL bRGB;				// component
	BOOL bYPbPr;
	BOOL bYPbPr_Downscale;
} FDI_VIDEO_OUT_MODE;

typedef struct
{
	unsigned short usX;
	unsigned short usY;
	unsigned short usWidth;
	unsigned short usHeight;
} FDI_VIDEO_RECT;

typedef FDI_ERR_CODE (*FDI_VIDEO_EventCallback)(FDI_VIDEO_NOTIFY_EVT eEvent, FDI_VIDEO_RUNNING_STATE eState);

/******************************************************************************
* Global variables and structures
******************************************************************************/
/* global variables and structures */

/******************************************************************************
* External variables and functions
******************************************************************************/
/* external variables  and functions */

/******************************************************************************
* Static variables and structures
******************************************************************************/
/* static variables and structures */

/******************************************************************************
* Static function prototypes
******************************************************************************/
/* static function prototypes */

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_GetNumOfDevice(unsigned long *pulNumOfDevice);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_GetCapability();

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_Open(unsigned long ulDeviceID);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_Start(unsigned long ulDeviceID, FDI_VIDEO_FORMAT eVideoFormat);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_Stop(unsigned long ulDeviceID);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_Show(unsigned long ulDeviceID, BOOL bShow);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_DisplayColorbar(unsigned long ulDeviceID, BOOL bOnOff);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_SetOutWindow(unsigned long ulDeviceID, FDI_VIDEO_RECT *pstOutRect);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_SetAnalogOutput(unsigned long ulDeviceID, FDI_VIDEO_OUT_MODE tVideoOuputMode);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_GetAnalogOutput(unsigned long ulDeviceID, FDI_VIDEO_OUT_MODE *tVideoOuputMode);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_SetDisplayStandard(unsigned long ulDeviceID, FDI_VIDEO_RESOLUTION eVideoResolution);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_ResetDisplayStandard(void);


/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_RegisterEventCallback(unsigned long ulDeviceId,
											 FDI_VIDEO_EventCallback fnVideoCallback);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_ControlDPort(unsigned long ulDeviceId, FDI_VIDEO_RESOLUTION eHdResolution, FDI_VIDEO_AspectRatio eAspectRatio);

/*******************************************************************************
* function :
* description :
* input :
* output :
* return :
* 기타 참고자료 및 파일
*******************************************************************************/
FDI_ERR_CODE FDI_VIDEO_SetSaspectVoltage();

#endif

