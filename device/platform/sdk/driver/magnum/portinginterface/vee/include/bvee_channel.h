/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvee_channel.h $
 * $brcm_Revision: Hydra_Software_Devel/10 $
 * $brcm_Date: 5/4/12 7:08p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vee/7346/bvee_channel.h $
 * 
 * Hydra_Software_Devel/10   5/4/12 7:08p venkatr
 * SW7429-143 : [7429] Added elements in structure to turn on/off of
 * optimization in firmware and sub pixel mode
 * 
 * Hydra_Software_Devel/9   3/15/12 10:51a venkatr
 * SW7231-585: [7231] Add STC index element to VEE Channel start settings
 * 
 * Hydra_Software_Devel/8   3/7/12 10:13a venkatr
 * SW7231-585:[7231] Modified the channel start settings to pass the
 * external interrupt info
 * 
 * Hydra_Software_Devel/7   1/18/12 4:25p venkatr
 * SW7346-515:[7346] VEE PI code, code addition to get and set data sync
 * setting, inorder to get STC address to do a TSM based picture drop in
 * case of running in non real time mode
 * 
 * Hydra_Software_Devel/6   11/22/11 5:17p venkatr
 * SW7346-515:[7346] VEE PI code modification to get STC timestamp from
 * application
 * 
 * Hydra_Software_Devel/5   11/14/11 3:35p venkatr
 * SW7346-515:[7346] VEE PI code modification to retrieve picture buffer
 * address only rather than picture descriptor
 * 
 * Hydra_Software_Devel/4   11/11/11 3:27p venkatr
 * SW7346-515:[7346] VEE PI code code modifying VEE enums to AVC type
 * 
 * Hydra_Software_Devel/3   11/8/11 3:52p venkatr
 * SW7346-515:[7346] VEE PI code for allocating reference buffers, and
 * modified for setting task user config
 * 
 * Hydra_Software_Devel/2   10/27/11 2:01p venkatr
 * SW7346-515:[7346] Removing surface handle from structure
 * BVEE_PictureDescriptor and adding the required parameter
 * 
 * Hydra_Software_Devel/1   10/19/11 8:29p venkatr
 * SW7346-515 : [7346] VEE PI code initial check in
 * 
 *
 ***************************************************************************/
#ifndef _BVEE_CHANNEL_
#define _BVEE_CHANNEL_

#include "bavc.h"
#include "bsur.h"
#include "bsur_private.h"

/***************************************************************************
Summary:
VEE Channel Handle
***************************************************************************/
typedef struct BVEE_Channel *BVEE_ChannelHandle;


/***************************************************************************
Summary:
Live Transcode for video phone: 640x480 @ 15fps
Live Transcode for iPad: 440x224 @ 30fps
Offline Transcode: 1280x720 @ 30fps 1/6 X real time 
                    640x360 @ 30fps 1~0.7 X real time
***************************************************************************/
typedef struct BVEE_Resolution
{
   unsigned width;
   unsigned height;
} BVEE_Resolution;

typedef enum
{
   BVEE_ChromaSampling_e420,
   BVEE_ChromaSampling_e422,
   BVEE_ChromaSampling_e444,
   BVEE_ChromaSampling_eLast,
   BVEE_ChromaSampling_eInvalid               = 0x7FFFFFFF
}BVEE_ChromaSampling;

typedef enum
{
    BVEE_VideoH264Profile_eBaseline		                            = 66,
    BVEE_VideoH264Profile_eMain									    = 77,
    BVEE_VideoH264Profile_eExtended								    = 88,
    BVEE_VideoH264Profile_eFRExtProfileHigh						    = 100,							
    BVEE_VideoH264Profile_eFRExtProfileHigh10							= 110,	
    BVEE_VideoH264Profile_eFRExtProfileHigh422					    = 122,	
    BVEE_VideoH264Profile_eFRExtProfileHigh444					    = 244,	
    BVEE_VideoH264Profile_eFRExtProfileHighCavlc444					= 44	
}BVEE_VideoH264ProfileIdc;

typedef enum
{
    BVEE_VideoEncodeMode_eHighDelay   				    = 0x0,      
    BVEE_VideoEncodeMode_eLowDelay    				    = 0x1,      
    BVEE_VideoEncodeMode_eAfap         				    = 0x2,
    BVEE_VideoEncodeMode_eInvalid		                = 0x7FFFFFFF
}BVEE_VideoEncodeMode;

typedef enum
{
    BVEE_VideoGopStruct_eIOnly		       			    = 0x0,      
    BVEE_VideoGopStruct_eIP			       			    = 0x1,      
    BVEE_VideoGopStruct_eIPB   							= 0x2,
    BVEE_VideoGopStruct_eIPBB			       			    = 0x3,
    BVEE_VideoGopStruct_eMax,
    BVEE_VideoGopStruct_eInvalid					        = 0x7FFFFFFF
}BVEE_VideoGopStruct;

typedef enum
{
   BVEE_AspectRatio_eUnknown = 0,
   BVEE_AspectRatio_eSquare,
   BVEE_AspectRatio_e12_11,
   BVEE_AspectRatio_e10_11,
   BVEE_AspectRatio_e16_11,
   BVEE_AspectRatio_e40_33,
   BVEE_AspectRatio_e24_11,
   BVEE_AspectRatio_e20_11,
   BVEE_AspectRatio_e32_11,
   BVEE_AspectRatio_e80_33,
   BVEE_AspectRatio_e18_11,
   BVEE_AspectRatio_e15_11,
   BVEE_AspectRatio_e64_33,
   BVEE_AspectRatio_e160_99,
   BVEE_AspectRatio_e4_3,
   BVEE_AspectRatio_e3_2,
   BVEE_AspectRatio_e2_1,
   BVEE_AspectRatio_e16_9,
   BVEE_AspectRatio_e221_1,
   BVEE_AspectRatio_eLast,
   BVEE_AspectRatio_eInvalid      = 0x7FFFFFFF
}BVEE_AspectRatio;
/***************************************************************************
Summary:
VEE channel open settings
***************************************************************************/
typedef struct BVEE_ChannelOpenSettings
{
    /* JDG: Capture buffers should not be allocated by the PI.  Only reference frame buffers. */
    BMEM_Handle bufferHeap;             /* Heap to allocate any reference buffers required by this channel.  
                                           If NULL, the heap passed to BVEE_Open will be used. */
    BAVC_VideoCompressionStd codec;   /* List of Video compression standards used by this channel */

    BVEE_Resolution resolution;         /* Maximum Encoder Resolution */

    unsigned maxQueuedPictures;         /* Maximum number of buffers that may be queued at any time.
                                           Requests to encode more buffers will result in an error returned
                                           from BVEE_Channel_EnqueuePicture */

    bool enableExternalTrigger;         /* If true, an external vsync trigger will be enabled.  When this is true,
                                           the register defined in BVEE_ChannelTriggerInfo must be written with the
                                           appropriate value each vsync.  If false (default), the encoder will automatically
                                           be triggered each time a BAVC_Channel_EnqueuePicture() is called. */
}BVEE_ChannelOpenSettings;

/***************************************************************************
Summary:
Get default open settings for a video channel
***************************************************************************/
 void BVEE_Channel_GetDefaultOpenSettings(
    BVEE_ChannelOpenSettings *pSettings             /* [out] */
    ); 

/***************************************************************************
Summary:
Open an instance of video encoding
***************************************************************************/
 BERR_Code BVEE_Channel_Open(
    BVEE_Handle deviceHandle,
    unsigned index,
    const BVEE_ChannelOpenSettings *pSettings,      /* Pass NULL for default settings */
    BVEE_ChannelHandle *pHandle                     /* [out] Returned Handle */
    );

/***************************************************************************
Summary:
Close an instance of video encoding
***************************************************************************/
 BERR_Code BVEE_Channel_Close(
    BVEE_ChannelHandle handle
    );
/***************************************************************************
Summary: 
	The structure contains all information regarding soft external interrupts to DSP 

Description:
	This structure contains configuration info of soft external interrupts to DSP.

See Also:
	None.
****************************************************************************/
typedef struct BVEE_ExtIntrptConfig
{
	/* If the dsp task will be interrupted by external client */
	bool	                enableInterrupts;
	uint32_t				numInterrupts;
	/* only numInterrupts of following struct will be valid */
	struct
	{
		/* ESR_SI register address. Full 32bit address */
		uint32_t				interruptRegAddr;
	    /* Trigger bit in the above register. Bit count [0...31]*/
		uint32_t				interruptBit;
	}interruptInfo[BDSP_MAX_EXT_INTERRUPT_PER_TASK];

}BVEE_ExtIntrptConfig;
/***************************************************************************
Summary:
VEE Channel settings
***************************************************************************/
typedef struct BVEE_ChannelStartSettings
{
    BAVC_VideoCompressionStd codec;         
    const BAVC_XptContextMap *pContextMap;  /* What RAVE context should be written while encoding. */
    bool                    nonRealTime;    /* Encode Mode.  Set to true for offline transcode and false otherwise. */
    BVEE_AspectRatio		eAspectRatio;
    BAVC_FrameRateCode      frameRate;
    BAVC_YCbCrType          pxlformat;
    BVEE_VideoH264ProfileIdc    eProfileIDC;
    uint32_t					ui32LevelIdc;			/* ranges from 9 to 51. For SD it is 30 */
    BVEE_VideoEncodeMode	 	eMode;					/* (High Delay, Low Delay, AFAP)	 */
    uint32_t 					ui32TargetBitRate;			/* Number of bits per sec.	*/
    uint32_t 					ui32EncodPicWidth;
    uint32_t 					ui32EncodPicHeight;
    uint32_t					ui32IntraPeriod;
    uint32_t					ui32IDRPeriod;
    BVEE_VideoGopStruct			eGopStruct;
    bool						bDblkEnable;
    bool                        bSubPelMvEnable;
    bool                        bVarModeOptEnable;
    bool						bRateControlEnable;    
    uint32_t					ui32End2EndDelay;
    bool                        sendMetadata;
    bool                        sendEos;
    BVEE_ExtIntrptConfig        extIntCfg;
    int                         stcIndx;
}BVEE_ChannelStartSettings;

/***************************************************************************
Summary:
Get default start settings for a video channel
***************************************************************************/ 
 void BVEE_Channel_GetDefaultStartSettings(
    BVEE_ChannelStartSettings *settings
    ); 

/***************************************************************************
Summary:
Start video encoding
***************************************************************************/
 BERR_Code BVEE_Channel_Start(
    BVEE_ChannelHandle handle,
    const BVEE_ChannelStartSettings *psettings
    );

/***************************************************************************
Summary:
Stop Video encoding
***************************************************************************/
 void BVEE_Channel_Stop(
    BVEE_ChannelHandle handle
    );

/***************************************************************************
Summary:
Description of a picture buffer
***************************************************************************/ 
typedef struct BVEE_PictureDescriptor
{    
    BAVC_FrameRateCode  frameRate;          /* Picture frame rate */
    BAVC_PictureCoding  pictureCoding;      /* Picture Type */
    BAVC_Polarity       polarity;           /* Picture Polarity */
    unsigned            sarHorizontal;      /* Sample Aspect Ratio horizontal value (SAR = sarHorizontal/sarVertical) */
    unsigned            sarVertical;        /* Sample Aspect Ratio vertical value (SAR = sarHorizontal/sarVertical) */
    bool                repeat;             /* This indicates the picture repeat known by the caller.  This flag applies 
                                               to both interlaced and progressive sequences.  For interlaced, repeated pictures
                                               have the same polarity as the previous picture.*/
    bool                ignore;             /* This flag indicates an unintentional picture repeat due to decoder underflow in 
                                               non-realtime transcode mode.  The encoder will drop any pictures with this flag set,
                                               timestamps will not be adjusted and it will have no effect in the coded stream.  */
    BAVC_PTSInfo        originalPts;        /* Original PTS value */
    uint32_t            id;                 /* Picture ID.  A 32-bit counter value generated by the caller to identify pictures. */
    void *              pvaddress;          /* Raw picture buffer address*/
    unsigned            height;             /* Encoded picture height*/
    unsigned            width;              /* Encoded picture width*/
    uint32_t            STC_Lo;             /* STC_Lo Timestamp when picture captured*/

} BVEE_PictureDescriptor;
/***************************************************************************
Summary:
Description of a Data Sync settings
***************************************************************************/ 
typedef struct BVEE_DatasyncSettings
{
    unsigned eEnableStc;		/*Set 0 to disable, 1 to enable*/
    uint32_t ui32StcAddress;
} BVEE_DatasyncSettings;
/***************************************************************************
Summary:
Initialize picture descriptor to default values
***************************************************************************/
void BVEE_Channel_InitPictureDescriptor(
    BVEE_ChannelHandle handle,
    BVEE_PictureDescriptor *pPicture    /* [out] */
    );

/***************************************************************************
Summary:
Add picture to encoder's queue. 
 
Description: 
The provided descriptor will be enqueued for the firmware.  If 
BVEE_ChannelOpenSettings.enableExternalTrigger is true, the register 
described in BVEE_ChannelTriggerInfo must also be written each vsync. 
Otherwise, the encoder will be automatically triggered. 
 
See Also: 
    BVEE_Channel_InitPictureDescriptor 
    BVEE_Channel_DequeuePicture_isr 
***************************************************************************/
BERR_Code BVEE_Channel_EnqueuePicture_isr(
    BVEE_ChannelHandle handle,
    const BVEE_PictureDescriptor *pPicture 
    );

/***************************************************************************
Summary:
Remove a picture that the encoder has finished encoding. 
 
Description: 
This call is non-blocking.  If no pictures are available, 
NULL will be returned. 
 
Returns: 
    BERR_SUCCESS - A frame was successfully dequeued.
    BVEE_ERR_QUEUE_EMPTY - No frames are available to dequeue.
 
See Also: 
    BVEE_Channel_EnqueuePicture_isr 
***************************************************************************/
void * BVEE_Channel_DequeuePicture_isr(
    BVEE_ChannelHandle handle
    );

/***************************************************************************
Summary:
External Trigger Information 
 
Description: 
This structure describes a register that must be written each vsync by 
an external source (e.g. BVN/VDC).  This is only used when the channel was 
opened with BVEE_ChannelOpenSettings. 
***************************************************************************/
typedef struct BVEE_ChannelTriggerInfo
{
    uint32_t address;       /* The register address to be written each vsync */
    uint32_t value;         /* The value to be written into the register each vsync */
} BVEE_ChannelTriggerInfo;

/***************************************************************************
Summary:
Get External Trigger Information 
***************************************************************************/
void BVEE_Channel_GetTriggerInfo(
    BVEE_ChannelHandle handle,
    BVEE_ChannelTriggerInfo *pInfo  /* [out] */
    );

/***************************************************************************
Summary:
    Get Channel Buffer Descriptors

Description:
    Returns video buffer descriptors for CDB content in the
    BAVC_VideoBufferDescriptor array(s)
***************************************************************************/
BERR_Code BVEE_Channel_GetBufferDescriptors(
   BVEE_ChannelHandle handle,
   const BAVC_VideoBufferDescriptor **pBuffer, /* [out] pointer to BAVC_VideoBufferDescriptor structs */
   size_t *pSize, /* [out] size of pBuffer in bytes (not number of BAVC_VideoBufferDescriptor structs) */
   const BAVC_VideoBufferDescriptor **pBuffer2, /* [out] pointer to BAVC_VideoBufferDescriptor structs after wrap around */
   size_t *pSize2 /* [out] size of pBuffer2 in bytes (not number of BAVC_VideoBufferDescriptor structs) */
   );

/***************************************************************************
Summary:
    Consume Channel Buffer Descriptors

Description:
    Reclaims the specified number of video buffer descriptors
    The CDB read pointer is updated accordingly
***************************************************************************/
BERR_Code BVEE_Channel_ConsumeBufferDescriptors(
    BVEE_ChannelHandle handle,
    unsigned numBufferDecsriptors /* must be <= pSize+pSize2 returned by last BVEE_Channel_GetBufferDescriptors call. */
    );

/***************************************************************************
Summary:
    Get Channel Buffer Status

Description:
    Returns the output buffer status (e.g. the base virtual address)
***************************************************************************/
BERR_Code BVEE_Channel_GetBufferStatus(
   BVEE_ChannelHandle handle,
   BAVC_VideoBufferStatus *pBufferStatus    /* [out] */
   );

/***************************************************************************
Summary:
    Set Data Sync Settings

Description:
    Provide STC address to DSP to do a TSM based picture drop in case if it 
    is not running real time. This STC address should be the same which was
    used to generate time-stamp for captured picture and *not* the STC that
    is used by video/audio decoder
***************************************************************************/
BERR_Code BVEE_Channel_SetDataSyncSettings(
   BVEE_ChannelHandle handle,
   BVEE_DatasyncSettings DataSyncSettings
   );

/***************************************************************************
Summary:
    Get Data Sync Settings

Description:
    Returns STC address used to do a TSM based picture drop in case if it 
    is not running real time.
***************************************************************************/
BERR_Code BVEE_Channel_GetDataSyncSettings(
   BVEE_ChannelHandle handle,
   BVEE_DatasyncSettings *DataSyncSettings
   );

/***************************************************************************
Summary:
VEE External Interrupt Handle
***************************************************************************/
typedef struct BVEE_ExtInterrupt *BVEE_ExtInterruptHandle;

/***************************************************************************
Summary:
External Interrupt Information 
 
Description: 
This structure describes a register address and bit that must be set by 
an external source (e.g. BVN/VDC), when data is ready for encode.
***************************************************************************/
typedef struct BVEE_ExtInterruptInfo
{
    uint32_t address;  /* The register address to be written each vsync */
    uint32_t bit_num;  /* The value to be written into the register each vsync */
} BVEE_ExtInterruptInfo;

/***************************************************************************
Summary:
    Allocate the external interrupt.

Description:
    With this interrupt bdsp can be interrupted from an external source.
***************************************************************************/
BERR_Code BVEE_Channel_AllocExtInterrupt(
   BVEE_ChannelHandle handle,
   BVEE_ExtInterruptHandle *pIntHandle
   );

/***************************************************************************
Summary:
    Get external interrupt info

Description:
    Returns allocated register address and bit number which needs to be set
	to raise interrupt.
***************************************************************************/
BERR_Code BVEE_Channel_GetExtInterruptInfo(
   BVEE_ChannelHandle handle,
   BVEE_ExtInterruptHandle pIntHandle,
   BVEE_ExtInterruptInfo *pExtIntInfo 
   );

/***************************************************************************
Summary:
    Free external interrupt

Description:
    Free up the allocated external interrupt.
***************************************************************************/
BERR_Code BVEE_Channel_FreeExtInterrupt(
   BVEE_ChannelHandle handle,
   BVEE_ExtInterruptHandle pIntHandle
   );
#endif
