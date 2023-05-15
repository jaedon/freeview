/**
 * @file	di_video_enc.h
 * @brief	Video Encoder module DI layer header file
 * @version $Revision: 0.0.0 $
 * @date 	$Date: 2012/04/24 12:00:00 $
 * @author
 */

/** @defgroup	DI_VIDEO_Enc	VIDEO Encoder COMMON IF OF DI LAYER
 *  This is VIDEO Encoder module of DI layer.
 *  @{
 */

#ifndef __DI_VIDEO_ENC_H__
#define __DI_VIDEO_ENC_H__
/******************************************************************************
 * Include Files
 ******************************************************************************/
#include "htype.h"
#include "di_err.h" /* if di module is included, di_err.h is included first!Later this line will be removed.*/
#include "di_video.h" 

/******************************************************************************
 * Symbol and Macro Definitions
 ******************************************************************************/

/**
 * Define video encoder capability
 */
typedef struct
{
	HUINT32						ulGroupId;
	HUINT32						ulDecoderId;
	DI_VIDEO_FORMAT				eSupportedFormat;
} DI_VIDEO_ENC_CAP_t;

typedef struct
{
	DI_VIDEO_FORMAT eCodec;	
	DI_VIDEO_HD_RESOLUTION eRes;
	DI_VIDEO_FrameRate_t eFrameRate;

} DI_VIDEO_ENC_Set_t;

/******************************************************************************
 * Local(static) Data Types
 ******************************************************************************/

/******************************************************************************
 * Extern Variables
 ******************************************************************************/

/******************************************************************************
 * Global Variables
 ******************************************************************************/

/******************************************************************************
 * Local(static) Variables
 ******************************************************************************/

/******************************************************************************
 * Extern Function Prototypes
 ******************************************************************************/
DI_ERR_CODE  DI_VIDEO_EncGetCapability(HUINT32 *pulNumOfDevice, DI_VIDEO_ENC_CAP_t **ppCapInfo);
DI_ERR_CODE  DI_VIDEO_EncSettings(HUINT32 ulDeviceId, DI_VIDEO_HD_RESOLUTION eRes, DI_VIDEO_FrameRate_t eFrameRate);
DI_ERR_CODE  DI_VIDEO_EncStart(HUINT32 ulDeviceId, DI_VIDEO_ENC_Set_t *Settings);
DI_ERR_CODE  DI_VIDEO_EncStop(HUINT32 ulDeviceId);
#endif /* _DI_VIDEO_ENC_H_ */

/** @} */ // end of DI_VIDEO_ENC


