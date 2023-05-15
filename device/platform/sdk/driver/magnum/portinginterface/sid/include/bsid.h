/******************************************************************************
*     Copyright (c) 2004-2012, Broadcom Corporation
*     All Rights Reserved
*     Confidential Property of Broadcom Corporation
*
*  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
*  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
*  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
*
* $brcm_Workfile: bsid.h $
* $brcm_Revision: Hydra_Software_Devel/4 $
* $brcm_Date: 6/13/12 3:06p $
*
* Module Description:
*   See Module Overview below.
*
* Revision History:
*
* $brcm_Log: /magnum/portinginterface/sid/7425/bsid.h $
* 
* Hydra_Software_Devel/4   6/13/12 3:06p fbasso
* SW7425-3239: fixed calculation of memory region for SID boot and text
* segments to be authenticated during secure boot
*
* Hydra_Software_Devel/3   3/19/12 3:52p fbasso
* SW7425-2449: added BSID_Standby and BSID_Resume to use during power
* down and up sequence.
*
* Hydra_Software_Devel/2   2/21/12 2:20p fbasso
* SW7425-2112: added support for MJPEG video decode. Merging new feature
* to Hydra_Software_Devel.
*
* Hydra_Software_Devel/SW7425-2112/5   2/20/12 3:50p fbasso
* SW7425-2112: remove run time option to configure channel change output
* behavior; rather than sid, the behavior upon channel change is decided
* by xdm configuration; sid will simply follow xdm.
*
* Hydra_Software_Devel/SW7425-2112/4   2/16/12 4:51p fbasso
* SW7425-2112: API clean-up; added mechanism to share SID FW/HW between
* still channels and motion channel; added support for channel change
* output mode (hold last picture)
*
* Hydra_Software_Devel/SW7425-2112/3   2/15/12 11:32a fbasso
* SW7425-2112: configure ePulldown variable in unified picture structure
* to frame progressive to resolve incorrect cropping at display time.
* Introduced new function BSID_GetRaveItbCbdConfigInfo to retrieve
* default RAVE configuration to use during video decode.
*
* Hydra_Software_Devel/SW7425-2112/2   2/10/12 10:54a fbasso
* SW7425-2112: added support for motion jpeg decode using XPT RAVE to
* provide input data to SID fw/hw and Xdm interface to supply SID output
* pictures to VDC.
*
* Hydra_Software_Devel/SW7425-2112/1   1/26/12 5:46p fbasso
* SW7425-2112: rave support first draft
*
* Hydra_Software_Devel/1   1/20/12 2:05p fbasso
* SW7425-1619: release SID multi channel interface to
* Hydra_Software_Devel
*
* Hydra_Software_Devel/sid_channel_interface_devel/4   1/20/12 1:29p fbasso
* SW7425-2112: in BSID_DataMapType added new enum for RAVE input feeding
* type
*
* Hydra_Software_Devel/sid_channel_interface_devel/3   1/3/12 4:53p fbasso
* SW7425-1619: added isr version of BSID_SetDmaChunkInfo; added support
* for 7346
*
* Hydra_Software_Devel/sid_channel_interface_devel/2   1/2/12 4:52p fbasso
* SW7425-674: modified handling ofinput buffer in streaming mode.
* Extended to support up to sixteen channels
*
* Hydra_Software_Devel/sid_channel_interface_devel/1   10/27/11 12:43p fbasso
* SW7425-1619: add queue based interface and multi channel support
* /main/$branch: m/d/y h:m fbasso
* - Created this file
******************************************************************************/


#ifndef BSID_H__
#define BSID_H__

#include "bchp.h"
#include "bmem.h"
#include "bint.h"
#include "bpxl.h"
#include "breg_mem.h"
#include "bsid_err.h"
#include "bavc_xpt.h"

#include "bxdm_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
Summary: number of SID hw instance in the SOC.
******************************************************************************/
#define BSID_NUM_DEVICES 1
#define BSID_MAX_CHANNELS 16

/******************************************************************************
Summary:
    BSID_Handle - the handle for SID (Still Image Decoder) pi.

Description:
    Opaque handle returned by BSID_Open and required to be provided for non
    channel related operations.

See also:
    - BSID_Open
    - BSID_Close
    - BSID_GetRevisionInfo
******************************************************************************/
typedef struct BSID_P_Handle *BSID_Handle;


/******************************************************************************
Summary:
    BSID_HandleHandle - the channel handle for SID (Still Image Decoder) pi.

Description:
    Opaque handle returned by BSID_OpenChannel and required to be provided
    channel related operations.

See also:
    - BSID_OpenChannel
    - BSID_CloseChannel
    - BSID_DecodeImage
    - BSID_GetStreamInfo
    - BSID_GetStatus
    - BSID_SetDMAChunkInfo
******************************************************************************/
typedef struct BSID_P_Channel *BSID_ChannelHandle;


/******************************************************************************
Summary:
    BSID_RevisionInfo - returns information about sid firmware and hardware.

Description:
    This structure is used to supply the caller module with information about
    about SID such as SID hardware limitations or unsupported functionality.

    ui32_PIVersion:
    . version of porting interface.

    ui32_FWVersion:
    . version of firmare.

    ui32_RleLegacyMaxWidth:
    . max width SID can decode for RLE images as per DVD Video specification.

    ui32_RleHdDvdRleMaxWidth:
    . max width SID can decode for RLE format as per HD-DVD Video specification.

    ui32_RleBdMaxWidth:
    . max width SID can decode for RLE format as per BD (Bluray Disk) ROM
      specification.

    ui32_JpegMaxWidth:
    . max width SID can decode for JPEG format:

    ui32_PngPaletteMaxWidth:
    . max width SID can decode for PNG format, palettized sub type.

    ui32_PngRgbaMaxWidth:
    .max width SID can decode for PNG format, with ARGB pixel format.

    ui32_Png16BppMaxWidth:
    .max width SID can decode for PNG format, with 16 bit per pixel format.

    ui32_GifMaxWidth:
    . max width SID can decode for GIF format.

    ui32_NumChannelsSupported:
    . max number of channels that can be operated simultaneously;


See also:
    - BSID_GetRevisionInfo
******************************************************************************/
typedef struct BSID_RevisionInfo {
    uint32_t                      ui32_PIVersion;
    uint32_t                      ui32_FWVersion;
    uint32_t                      ui32_RleLegacyMaxWidth;
    uint32_t                      ui32_RleHdDvdRleMaxWidth;
    uint32_t                      ui32_RleBdMaxWidth;
    uint32_t                      ui32_JpegMaxWidth;
    uint32_t                      ui32_PngPaletteMaxWidth;
    uint32_t                      ui32_PngRgbaMaxWidth;
    uint32_t                      ui32_Png16BppMaxWidth;
    uint32_t                      ui32_GifMaxWidth;
} BSID_RevisionInfo;


/******************************************************************************
Summary:
    BSID_BootInfo - embedded processor firmare code location and size.

Description:
    This structure is used to communicate firmare code location and size to
    security module to perform authentication and boot SID fw.

    ui32_Address:
    . virtual address of start of SID fw code section.

    ui32_Size:
    . size of code section in bytes.

See also:
    - BSID_Open
    - BSID_Settings
********************************************************************************/
typedef struct BSID_BootInfo
{
    uint32_t ui32_Address;
    uint32_t ui32_Size;
} BSID_BootInfo;


/******************************************************************************
Summary:
    BSID_Settings - structure to use to configure SID porting interface,
    firmware and hardware.

Description:
    This structure provides a set of paramters the user can use to configure
    the SID module (pi/firmare/hardware) according to system properties, or
    software utilization of the module.

    ui32_SidInstance:
    . instance number: this number is used by the PI to select which hw
      instance of the SID hw block it wants to use. The maximum number of
      instances is returned by BSID_GetNumberOfHwInstances().

    ui8_JPEGHorizAndVerFilt:
    . horizontal filter - upper 4 bits: specify the filter ID of the filter
      used for the 4:2:2 to 4:4:4 chroma up conversions.
      Similarly for the vertical filtering where the ID is specified in the
      lower 4 bits.
      Please refer "Still Image Decoder (SID) Specification" hw document for
      details.

    ui8_AlphaValue:
    . in case of 24bit output pixel format (RGB, YUV4:4:4), this value of
      alpha will be placed in the alpha channel to create 32 bits pixels
      (ARGB, AYUV4:4:4). Default value is opaque 0xFF.

    b_EndianessSwap:
    . specify the Endianess convention used by the SID ARC to interpret the
      data in the input and output buffers. "True" for Big Endian, "False"
      (default) for Little Endian.

    b_SelfTest:
    . when "True" (default) SID PI will perform a sanity check every time
      the SID ARC firmware is written to memory before the SID ARC is started.
      when "False" SID PI will not perform this check.

    p_ExternalBootCallback:
    . fw authentication mechanism. When this function is installed SID does
      not boot the firmware. Instead it will call this callback to let the
      security module know where the SID firmware is located in bmem. Once
      the security module has determine that the SID firmware is secured, it
      will also proceed to boot the SID ARC using the authenticated firmware.

    pv_ExternalBootCallbackData:
      caller callback data pointer returned as part of callback call.

See also:
    - BSID_Open.
******************************************************************************/
typedef struct BSID_Settings {
    uint32_t    ui32_SidInstance;
    uint16_t    ui16_JPEGHorizAndVerFilt;
    uint8_t     ui8_AlphaValue;
    bool        b_EndianessSwap;
    bool        b_SelfTest;
    BERR_Code   (*p_ExternalBootCallback)(void *pv_CallbackData, const BSID_BootInfo *ps_BootInfo);
    void        *pv_ExternalBootCallbackData;
} BSID_Settings;


/******************************************************************************
Summary:
    BSID_NotificationEvent - notification events.

Description:
    Callback notification events. Caller can choose which events it likes to
    be notified upon by OR-ing them when installing a notification callback.
    Following are the events supported:

    DecodeDone:
    . notify on BSID_DecodeImage done with success.

    DecodeError:
    . notify on BSID_DecodeImage done with error.

    GetInfoDone:
    . notify on BSID_GetStreamInfo done with success.

    GetInfoDone:
    . notify on BSID_GetStreamInfo done with error.

    WaitingForMoreInputData:
    . notify on input data undeflow when input data are provided using
      streaming model mechanism.

See also:
    - BSID_DecodeImage
    - BSID_GetStreamInfo
    - BSID_SetDMAChunkInfo
******************************************************************************/
typedef enum BSID_NotificationEvent {
    BSID_NotificationEvent_eDecodeDone                      = 0x00000001,
    BSID_NotificationEvent_eDecodeError                     = 0x00000002,
    BSID_NotificationEvent_eGetInfoDone                     = 0x00000004,
    BSID_NotificationEvent_eGetInfoError                    = 0x00000008,
    BSID_NotificationEvent_eWaitingForMoreInputData         = 0x00000010,
    BSID_NotificationEvent_eFlushDone                       = 0x00000020,
    BSID_NotificationEvent_eFlushError                      = 0x00000040
} BSID_NotificationEvent;


/******************************************************************************
Summary:
    BSID_ResourceStatus - status of channel.

Description:
    It reports the status of the channel:
    . busy during a decoding or when retrieving header information.
    . idle otherwise.

See also:
    - BSID_GetStatus
******************************************************************************/
typedef enum BSID_ResourceStatus {
    BSID_ResourceStatus_eIdle,
    BSID_ResourceStatus_eBusy,
    BSID_ResourceStatus_eLast
} BSID_ResourceStatus;


/******************************************************************************
Summary:
    BSID_CallbackFunc - callback used to notify the completion of an operation
    or other events.

Description:
    BSID_DecodeImage, BSID_GetStreamInfo can be executed as non blocking call.
    In this case the user may specify a callback to be notified upon completion
    of the operation.

See also:
    - BSID_DecodeImage
    - BSID_GetStreamInfo
******************************************************************************/
typedef void (*BSID_CallbackFunc)(BSID_ChannelHandle hSidChannel,
                                  BSID_NotificationEvent e_EventType,
                                  void *pv_CallbackData);


/******************************************************************************
Summary:
    BSID_MemoryBounds: structure used to verify that the memory provided to SID
    (input and output) is contained within the memory limits specified. When
    offset and size are set to zero, no memory bounds check shall be performed.

Description:

    ui32_Offset:
    . starting memory offset. If zero no bounds check shall be performed.

    ui32_Size:
    . memory size.

See also:
    - BSID_OpenChannel.
********************************************************************************/
typedef struct BSID_MemoryBounds {
    uint32_t ui32_Offset;
    uint32_t ui32_Size;
} BSID_MemoryBounds;


/******************************************************************************
Summary:
    BSID_ChannelType - type of channel.

Description:
    Select whether the channel is a Still picture (default) or Motion picture one.

See also:
    - BSID_GetStatus
******************************************************************************/
typedef enum BSID_ChannelType {
    BSID_ChannelType_eStill,
    BSID_ChannelType_eMotion,
    BSID_ChannelType_eLast
} BSID_ChannelType;


/******************************************************************************
Summary:
    BSID_OpenChannelSettings - structure to use to configure SID at open
    channel time.

Description:
    This structure provides a set of paramters the user can use to configure
    a channel.

    e_ChannelType:
    . channel type: Still (default) or Motion.

    ui32_ChannelPriority:
    . channel priority.

    ui32_QueueDepth: still channel only.
    . maximum number of operations queue-able by the caller.

    p_CallbackFunc_Isr: still channel only.
    . if not NULL and b_WaitForCompletion is false, this function will be
      called when the events specified in ui32_CallbackEvents are verified.

    ui3i2_CallbackEvents: still channel only.
    . events upon which pvCallback shall be called. One or more
      BSID_NotificationEvents shall be OR-ed together.

    pv_CallbackData: still channel only.
    . private data returned to the caller on callback. It can be changed at
	  run time.

    s_MemoryBounds:  still channel only.
    . structure used to verify that the memory provided to SID (input and output)
      is contained within the memory limits specified. When offset and size are
      set to zero, no memory bounds check shall be performed.

    ui32_OutputBuffersNumber: motion channel only.
    . number of buffer exchanged between SID and XDM.

    ui32_OutputMaxWidth: motion channel only.
    . max support video width.

    ui32_OutputMaxHeight: motion channel only.
    . max support video height.

    hOutputBuffersHeap: motion channel only.
    . heap to use to allocated channel related bmem memory.

See also:
    - BSID_Open.
******************************************************************************/
typedef struct BSID_OpenChannelSettings {
    BSID_ChannelType        e_ChannelType;
    uint32_t                ui32_ChannelPriority;
    union {
        struct {
            uint32_t            ui32_QueueDepth;
            BSID_MemoryBounds   s_MemoryBounds;
            BSID_CallbackFunc   p_CallbackFunc_Isr;
            uint32_t            ui32_CallbackEvents;
            void                *pv_CallbackData;
        } still;
        struct {
            uint32_t                        ui32_OutputBuffersNumber;
            uint32_t                        ui32_OutputMaxWidth;
            uint32_t                        ui32_OutputMaxHeight;
            BMEM_Handle                     hOutputBuffersHeap;
        } motion;
    } u_ChannelSpecific;
} BSID_OpenChannelSettings;


/******************************************************************************
Summary:
    BSID_ChannelSettings - structure to use to configure SID channel runtime.

Description:
    This structure provides a set of paramters the user can use to configure
    a channel.

    ui32_ChannelPriority:
    . channel priority.

    p_CallbackFunc_Isr: still channel
    . if not NULL, this function will be called when the events specified in
      ui32_CallbackEvents are verified.

    ui3i2_CallbackEvents: still channel
    . events upon which pvCallback shall be called. One or more
      BSID_NotificationEvents shall be OR-ed together.

    pv_CallbackData: still channel
    . private data returned to the caller on callback.

See also:
    - BSID_Open.
******************************************************************************/
typedef struct BSID_ChannelSettings {
    uint32_t ui32_ChannelPriority;
    union {
        struct {
            BSID_CallbackFunc   p_CallbackFunc_Isr;
            uint32_t            ui32_CallbackEvents;
            void                *pv_CallbackData;
        } still;
        struct {
            void                *unused;
        } motion;
    } u_ChannelSpecific;
} BSID_ChannelSettings;


/******************************************************************************
Summary:
    BSID_ImageFormat - image format supported by SID.

Description:
    RLE_Legacy_DVD, RLE_HD_DVD, RLE_BD:
    . support for 2 bits/pixel (DVD) and 8 bits/pixel (HD-DVD and Blu-Ray).

    JPEG:
    . baseline, 8 bits per pixel, sequential and progressive:
      o Greyscale and YUV 4:4:4, 4:2:2, 4:2:0.
      o Image width: up to 8K.

    PNG:
    . version 1.2
      o All color types & bit depths (16-bit data is truncated to 8-bit).
      o Greyscale (1,2,4,8,16 bpp), Pseudo-color (1,2,4,8 bpp), RGB,
        RGB + Alpha.
      o Deflate version 1.3.
      o Sequential and interlaced support (re-interlacing done by the ARC).
      o Images up to 8K pixels wide (8 bpp or less), 2K wide (RGBA), and
        1K for 16-bit.

    GIF:
    . basic pixel + run encoding:
      o Support for 2 bits/pixel (DVD), 8 bits/pixel (DVD and Blu-Ray).

    MPEG-2:
    . Intra frame decode only..

See also:
    - BSID_GetStreamInfo.
******************************************************************************/
typedef enum BSID_ImageFormat {
    BSID_ImageFormat_eRLE_Legacy_DVD,
    BSID_ImageFormat_eRLE_HD_DVD,
    BSID_ImageFormat_eRLE_BD,
    BSID_ImageFormat_eJPEG,
    BSID_ImageFormat_ePNG,
    BSID_ImageFormat_eGIF,
    BSID_ImageFormat_eMPEG_IFRAME,
    BSID_ImageFormat_eLast
} BSID_ImageFormat;


/******************************************************************************
Summary:
    BSID_DataMapType - enum used to specify the nature of the compressed data
    as stored in memory.

Description:
    BSID_DataMapType_eLinear:
    . the bitstream is guaranteed to be contiguous in memory.

    BSID_DataMapType_eScatter:
    . the bitstream is not contiguous in memory and a list of pointers to each
      chunk will be provided. Currently not supported.

    BSID_DataMapType_eRave:
    . the bitstream is provided as per RAVE format. BAVC_XptContextMap will be
      used for pv_InputDataMap.

See also:
    - BSID_DecodeImage,
    - BSID_GetStreamInfo.
******************************************************************************/
typedef enum BSID_DataMapType {
    BSID_DataMapType_eLinear,
    BSID_DataMapType_eScatter,
    BSID_DataMapType_eRave,
    BSID_DataMapType_eLast
} BSID_DataMapType;


/******************************************************************************
Summary:
    BSID_JpegSubType - jpeg subtype

Description:
    - It describe the chroma format of the native data. This parameter shall not
      be confused with the sid decode output format. sid output format is 444 if
      the native data is 444. For all the other format sid does always output in
      422 format.

See also:
    - BSID_GetStreamInfo
******************************************************************************/
typedef enum BSID_JpegSubType {
    BSID_JpegSubType_eGrayScale,
    BSID_JpegSubType_e444,
    BSID_JpegSubType_e422,
    BSID_JpegSubType_e420,
    BSID_JpegSubType_e422r,
    BSID_JpegSubType_e411
} BSID_JpegSubType;


/******************************************************************************
Summary:
    BSID_LinDataMap - linear data map specification.

Description:
    Provide a data structure for compressed data stored contiguosly in memory.

    ui32_Offset:
    . physical address of the bitstream data. No special alignment is required.

    ui32_Size:
    . size of the bistream.

See also:
    - BSID_DecodeImage
    - BSID_StreamInfo
******************************************************************************/
typedef struct BSID_LinDataMap {
    uint32_t ui32_Offset;
    uint32_t ui32_Size;
} BSID_LinDataMap;


/******************************************************************************
Summary:
    BSID_ImageBuffer - information about the SID output buffer.

Description:
    This data structure serves a dual purpose according to the image format type:
    1) JPEG, PNG, GIF: this structure is used to explicitly state the geometry
    and pixel format of the SID output surface allocated using information from
    BSID_ImageHeader.
    2) RLE formats: this structure is used to provide information on how much
    RLE data need to be decoded. Width and height will be used to define the
    geometry of the RLE object that the SID needs to decode. They are not
    related to the buffer geometry.

    ui32_Offset:
    . physical address of the output surface.

    ui32_Width:
    . case 1: output surface width.
    . case 2) width of the RLE objects to decode.

    ui32_Height:
    . case 1: output surface height.
    . case 2) height of the RLE objects to decode.

    ui32_Pitch:
    . output surface pitch.

    e_Format:
    . output surface pixel format.

See also:
    - BSID_ImageHeader
    - BSID_DecodeImage
******************************************************************************/
typedef struct BSID_ImageBuffer {
    uint32_t    ui32_Offset;
    uint32_t    ui32_Width;
    uint32_t    ui32_Height;
    uint32_t    ui32_Pitch;
    BPXL_Format e_Format;
} BSID_ImageBuffer;


/******************************************************************************
Summary:
    BSID_DecodeImage - decode Image data structure.

Description:
    This structure provides information on where to find the bitstream, where
    to store the decode image and whether the execution of the operation shall
    be blocking or non blocking.

    e_ImageFormat:
    . the format of the image.

    e_DataMapType:
    . organization of the bistream in memory.

    pv_InputDataMap:
    . according with eDataMapType it will point to the right data structure
      that describe the position of the bistream data in memory.

    s_ImageBuffer:
    . this structure contains the output surface information.

    pui8_IntraQuantTable:
    . pointer to quant-table (required by fw to decode MPEG2 I-pictures) if
      seq. header had one. If seq. header did not have a quant table, then
      this must be set to NULL.

    b_LastDmaChunk:
    . last dma.

    ui32_MultiScanBufAddr:
    . physical Address of the auxiliary buffer that SID will use to decode
      multi-scan images such as Progressive JPEG and PNG interlaced images.
      If zero is specified SID will not perform multi scan decoding.

    ui32_MultiScanBufSize:
    . size of the multi-scan decoding buffer.

See also:
    - BSID_DecodeImage
******************************************************************************/
#define BSID_MPEG2IFRAME_INTRA_QUANT_ENRIES     64
#define BSID_GAMMA_CORRECTION_TABLE_ENTRIES     256
typedef struct BSID_DecodeImage {
    BSID_ImageFormat  e_ImageFormat;
    BSID_DataMapType  e_DataMapType;
    void             *pv_InputDataMap;
    BSID_ImageBuffer  s_ImageBuffer;
    uint8_t           *pui8_IntraQuantTable;
    bool              b_LastDmaChunk;
    uint32_t          ui32_MultiScanBufAddr;
    uint32_t          ui32_MultiScanBufSize;
} BSID_DecodeImage;


/******************************************************************************
Summary:
    BSID_ImageHeader - image header.

Description:
    This structure is filled up when BSID_GetStreamInfo is called.

    e_PixelFormat:
    . pixel format of the decoded data.

    ui32_GrayScale:
    . indicate if the pixel format is palettized gray scale. For gray scale
      images SID does not return a valid CLUT, ui16_NumClutEntries will be
      zero. The user shall build its own CLUT.

    ui32_BitPerPixel:
    . number of bits per pixel.

    ui32_ImgWidth:
    . real image width.

    ui32_ImgHeight:
    . real image height.

    ui32_SurPitch:
    . number of bytes between two consecutive row start addresses.

    ui32_SurWidth:
    . in case of block-based images like JPEG, this number corresponds to
      ui16_ImgWidth rounded up to the nearest block-aligned width. For non
      block-based images, ui16_SurWidth and ui16_ImgWidth are the same.

    ui32_SurHeight:
    . in case of block-based images like JPEG, this number corresponds to
      ui16_ImgHeight rounded up to the nearest block-aligned height.For non
      block-based images, ui16_SurHeight and ui16_ImgHeight are the same.

    ui32_MultiScan:
    . indicates if the image requires multiple scan (pass) to be decoded.
      This type of decoding will make use of the multiscan buffer specified
      at BSID_Open time. If one is not available, SID will return with failure.

    ui32_NumClutEntries:
    . the number of valid entries in the CLUT.

    aui32_Clut:
    . CLUT entries.

    e_ClutPixelFormat:
    . CLUT pixel format.

    e_JpegSubtype:
    . chroma sub type for jpeg images.

    ui32_IsIntraQuantTablePresent:
    . set by fw if the seq. header had a quant table.

    ui8_IntraQuantTable[];
    . valid only if ui32_isIntraQuantTablePresent (above) is true. Contains
      the quant table of the seq. header.

    ui32_PicStruct:
    . 1: Top.
    . 2: Bottom.
    . 3: Frame.

    ui32_Gamma

    ui32_HasTransparentData:
    . when not zero, indicates that transparent data (TransparentColorIndex
      and TransparentColorRGB) are available. This variable can be applied
      to GIF and PNG image format with some restriction, see below.

    ui32_TransparentColorIndex:
    . when ui8_HasTransparentData is not zero, it indicates the index of
      the entry in the CLUT that that shall be made transparent. This
      variable has only meaning for GIF image format.

    ui32_TransparentColorRGB:
    . when ui8_HasTransparentData is not zero it indicates the explicit
      color (RGB format) that shall be made transparent. This variable has
      meaning for GIF and PNG image format.

See also:
    - BSID_GetStreamInfo
******************************************************************************/
#define BSID_CLUT_MAX_ENTRIES        256
typedef struct BSID_ImageHeader {
    BPXL_Format         e_PixelFormat;
    uint32_t            ui32_GrayScale;
    uint32_t            ui32_BitPerPixel;
    uint32_t            ui32_ImgWidth;
    uint32_t            ui32_ImgHeight;
    uint32_t            ui32_SurPitch;
    uint32_t            ui32_SurWidth;
    uint32_t            ui32_SurHeight;
    uint32_t            ui32_MultiScan;
    uint32_t            ui32_NumClutEntries;
    uint32_t            aui32_Clut[BSID_CLUT_MAX_ENTRIES];
    BPXL_Format         e_ClutPixelFormat;
    BSID_JpegSubType    e_JpegSubtype;
    uint32_t            ui32_IsIntraQuantTablePresent;
    uint8_t             ui8_IntraQuantTable[BSID_MPEG2IFRAME_INTRA_QUANT_ENRIES];
    uint32_t            ui32_PicStruct;
    uint32_t            ui32_Gamma;
    uint32_t            ui32_HasTransparentData;
    uint32_t            ui32_TransparentColorIndex;
    uint32_t            ui32_TransparentColorRGB;
} BSID_ImageHeader;


/******************************************************************************
Summary:
    BSID_StreamInfo - get stream info data structure.

Description:
    This structure provides information on where to find the bitstream, where to
    store the image header info and whether the execution of the operation shall
    be blocking or non blocking.

    e_ImageFormat:
    . the format of the image.

    e_DataMapType:
    . organization of the bitstream data in memory.

    pv_InputDataMap:
    . according with eDataMapType it will point to a data structure that provide
      information on where the bistream is in memory.

    ps_OutImageInfo:
    . this structure will be filled up with the image header information.

    b_LastDmaChunk:
    . last dma.


See also:
    - BSID_StreamInfo
******************************************************************************/
typedef struct BSID_StreamInfo {
    BSID_ImageFormat    e_ImageFormat;
    BSID_DataMapType    e_DataMapType;
    void                *pv_InputDataMap;
    BSID_ImageHeader    *ps_OutImageInfo;
    bool                b_LastDmaChunk;
} BSID_StreamInfo;


/******************************************************************************
Summary:
   BSID_DecodeSegment - segmented decoding data structure. The segment geometry
   is ui16_SurWidth by ui32_SegmentsHeight, with pitch ui16_SurPitch and pixel
   format is ePixelFormat.

Description:
    This structure provides information on where to find the bitstream, where
    to store the segment and whether the execution of the operation shall
    be blocking or non blocking.

    e_ImageFormat:
    . the format of the image.

    e_DataMapType:
    . organization of the bistream in memory.

    pv_InputDataMap:
    . according with eDataMapType it will point to the right data structure
      that describe the position of the bistream data in memory.

    s_ImageBuffer:
    . this structure contains the output surface information.

    b_FirstSegment:
    . indicate whether this is the first segment of the image to be decoded.
      It shall be set to "true" on the very first segment to be decoded and
      "false" for the remaining ones.

    ui32_SegmentHeight:
    . indicate the height of the segment to decode. NOTE: ui32_SegmentsHeight
      shall always be a multiple of 16 lines even on the last segment when
      ui16_SurHeight is not a multiple of 16 lines. In this case the decode
      will only decode the reamining lines even if ui32_SegmentsHeight
      exceeds this number.

    b_LastDmaChunk:
    . last dma.

    ui32_MultiScanBufAddr:
    . physical Address of the auxiliary buffer that SID
      will use to decode multi-scan images such as Progressive JPEG and PNG
      interlaced images. If zero is specified SID will not perform multi scan
      decoding.

    - ui32_MultiScanBufSize:
    . size of the multi-scan decoding buffer.

See also:
    - BSID_DecodeImageSegment
******************************************************************************/
typedef struct BSID_DecodeSegment {
    BSID_ImageFormat    e_ImageFormat;
    BSID_DataMapType    e_DataMapType;
    void                *pv_InputDataMap;
    BSID_ImageBuffer    s_ImageBuffer;
    bool                b_FirstSegment;
    uint32_t            ui32_SegmentHeight;
    bool                b_LastDmaChunk;
    uint32_t            ui32_MultiScanBufAddr;
    uint32_t            ui32_MultiScanBufSize;
} BSID_DecodeSegment;


/******************************************************************************
Summary:
    BSID_DmaChunkInfo - compressed data chunk information used during streaming
    model.

Description:
    structure used to provide information about the next chunk of compressed
    input data to be read to continue on with the current operation.

    ui32_Offset:
    . physical address of new chunk of data to be provided.
    . this address must only be supplied when BSID_DataMapType is Scattered.

    ui32_Lenght:
    . Length in bytes. For optimal performance the dma chunk lenght shall be
      set to a reasonable large number to avoid running into underflow
      situation too often. A minimum of 4K should be guaranteed for all the
      chunks except the last one.

    ui32_LastDmaChunk:
    . True: no more input data to follow.
    . False: more input data to follow.

    ui32_AbortDma:
      True: causes SID to stop the current decode operation immediately

See also:
    - BSID_SetDmaChunkInfo
******************************************************************************/
typedef struct BSID_DmaChunkInfo {
    uint32_t            ui32_Offset;
    uint32_t            ui32_Size;
    uint32_t            ui32_LastDmaChunk;
    uint32_t            ui32_AbortDma;
} BSID_DmaChunkInfo;


/******************************************************************************
Summary:
    BSID_DecodeResult - result of the decoding operation

See also:
    - BSID_GetStatus
******************************************************************************/
typedef enum BSID_DecodeResult {
    BSID_DecodeResult_eSuccess,
    BSID_DecodeResult_eFailed,
    BSID_DecodeResult_eLast
} BSID_DecodeResult;


/******************************************************************************
Summary:
    BSID_Status - returned by BSID_GetStatus provide progress and result of the
    current operation, either BSID_DecodeImage or BSID_GetStreamInfo.

See also:
    - BSID_GetStatus
******************************************************************************/
typedef struct BSID_Status {
    BSID_DecodeResult   e_DecodeResult;
    BSID_ResourceStatus e_ResourceStatus;
    uint32_t            ui32_QueueDepth;
} BSID_Status;


/******************************************************************************
Summary:
    BSID_FlushSettings

See also:
    - BSID_FlushChannel
******************************************************************************/
typedef struct BSID_FlushSettings {
    unsigned int param1; /* TBD */
} BSID_FlushSettings;


/******************************************************************************
Summary:
    BSID_StartDecodeSettings

See also:
    - BSID_StartDecode, BSID_StopDecode
******************************************************************************/
typedef struct BSID_StartDecodeSettings {
     BAVC_XptContextMap *ps_RaveContextMap;
} BSID_StartDecodeSettings;


/******************************************************************************
Summary:
    BSID_GetRevisionInfo - firmware and hw information.

Description:
    This API returns the SID ARC firmware version in use and the chip id.

Parameters:
    - hSid: the SID handle.

    - pst_RevisionInfo: pointer to the structure that is used to return info
      about SID i.e. hw limitations and supported feaures.

Returns:
    - BERR_NOT_SUPPORTED: currently is not supported.

See also:
    - BSID_RevisionInfo
******************************************************************************/
BERR_Code BSID_GetRevisionInfo(
    BSID_Handle         hSid,
    BSID_RevisionInfo   *ps_RevisionInfo
    );


/******************************************************************************
Summary:
    BSID_GetDefaultSettings - gets default settings.

Description:
    This API returns default settings for SID.

Parameters:
    - ps_DefSettings: pointer to the default setting data structure.

Returns:
    -

See Also:
    - BSID_Settings
******************************************************************************/
void BSID_GetDefaultSettings(
    BSID_Settings   *ps_DefSettings
    );


/******************************************************************************
Summary:
    BSID_Open - creates an instance of the SID PI.

Description:
    This api open the SID PI and configure the low level firmware using the
    data from the default setting structure. SID ARC and HW will be reset as
    part of this operation.

Parameters:
    - p_hSid: used to return SID handle upon successful execution of the function.

    - hChp: system chip handle.

    - hReg: system register handle.

    - hMem: system memory heap that SID shall use to allocate internal buffers
      such as the one necessary for the SID ARC firmware and the Mailbox
      communication between MIPS and SID ARC.

    - hInt: system interrupt handle.

    - ps_DefSettings: pointer to the default setting data structure.

Returns:
    - BERR_SUCCESS: module was opened and valid handle was returned.

    - BERR_INVALID_PARAMETER: one of the input parameters was invalid.

    - BERR_OUT_OF_SYSTEM_MEMORY: not enough kernel memory to complete the
    operation.

See also:
    - BSID_Close
    - BSID_Settings
******************************************************************************/
BERR_Code BSID_Open(
    BSID_Handle         *phSid,
    BCHP_Handle         hChp,
    BREG_Handle         hReg,
    BMEM_Handle         hMem,
    BINT_Handle         hInt,
    const BSID_Settings *ps_DefSettings
    );


/******************************************************************************
Summary:
    BSID_Close - destroy the SID PI instance.

Description:
    This api waits for the SID ARC to become idle then initiate the closing
    procedure which will release all the allocated resource.

Parameters:
    - phSid: pointer to the SID handle.

Returns:
    - BERR_SUCCESS: module was closed.

    - BERR_UNKNOWN: SID failed to release some resources.

See also:
    - BSID_Open
******************************************************************************/
void BSID_Close(
    BSID_Handle hSid
    );


/******************************************************************************
Summary:
    BSID_GetOpenChannelSettings - gets open settings for a channel.

Description:
    This API returns default settings for a channel.

Parameters:
    - ps_OpenChannelSettings: pointer to the default setting data structure.

Returns:
    - BERR_SUCCESS: default settings returned.

See Also:
    - BSID_ChannelSettings
******************************************************************************/
void BSID_GetOpenChannelSettings(
    BSID_ChannelType e_ChannelType,
    BSID_OpenChannelSettings *ps_OpenChannelSettings
    );


/******************************************************************************
Summary:
    BSID_GetChannelSettings - gets current settings for the channel.

Description:
    This API returns current settings for a channel.

Parameters:
    - ps_ChannelSettings: pointer to the setting data structure.

Returns:
    - BERR_SUCCESS: default settings returned.

See Also:
    - BSID_ChannelSettings
******************************************************************************/
void BSID_GetChannelSettings(
    BSID_ChannelHandle hSidChannel,
    BSID_ChannelSettings *ps_ChannelSettings
    );


/******************************************************************************
Summary:
    BSID_SetChannelSettings - set run time settings for a channel.

Description:
    This API returns default settings for a channel.

Parameters:
    - hSidChannel: channel handle.
    - pChannelSettings: pointer to new settings to use.

Returns:
    - BERR_SUCCESS: default settings returned.

See Also:
    - BSID_ChannelSettings
******************************************************************************/
BERR_Code BSID_SetChannelSettings(
    BSID_ChannelHandle          hSidChannel,
    const BSID_ChannelSettings  *ps_ChannelSettings
    );


/******************************************************************************
Summary:
    BSID_OpenChannel - creates a SID channel.

Description:
    This api opens and configure a SID channel.

Parameters:
    - hSid: SID handle.

    - phSidChannel: used to return SID handle upon successful execution of
      the function.

    - ui32_ChannelId: channel unique identification number.

    - ps_OpenChannelSettings: pointer to the default channel configuration.

Returns:
    - BERR_SUCCESS: module was opened and valid handle was returned.

    - BERR_INVALID_PARAMETER: one of the input parameters was invalid.

    - BERR_OUT_OF_SYSTEM_MEMORY: not enough kernel memory to complete the
    operation.

See also:
    - BSID_CloseChannel
    - BSID_ChannelSettings
******************************************************************************/
BERR_Code BSID_OpenChannel(
    BSID_Handle                 hSid,
    BSID_ChannelHandle          *phSidChannel,
    uint32_t                    ui32_ChannelId,
    const BSID_OpenChannelSettings *ps_OpenChannelSettings
);


/******************************************************************************
Summary:
    BSID_CloseChannel - destroy the SID channel.

Description:
    This api release all the allocated resource for the channel.

Parameters:
    - hSidChannel: channel handle.

Returns:
    - BERR_SUCCESS: channel was closed.

    - BERR_UNKNOWN: SID failed to release some resources.

See also:
    - BSID_Open
******************************************************************************/
void BSID_CloseChannel(
    BSID_ChannelHandle  hSidChannel
);


/******************************************************************************
Summary:
    BSID_DecodeStillImage - issue decode command.

Description:
    This API request image decoding. This function can be blocking or not.
    When blocking a callback function can be registered to receive notification
    events about decoding such as "Decode done" or "Decode error" events.
    Please refer to the definition of BSID_DecodeImage for details on the
    parameters required by this function. Please note that the SID ARC/HW are
    not queue driven engine. Because of this, before posting a new decoding
    request it is mandatory to wait until the current operation is completed.

Parameters:
    - hSidChannel: channel handle.

    - ps_DecodeImage: pointer to the decode data structure required for the
      decoding operation.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    - BSID_DecodeImage
******************************************************************************/
BERR_Code BSID_DecodeStillImage(
    BSID_ChannelHandle  hSidChannel,
    const BSID_DecodeImage    *ps_DecodeImage
    );


/******************************************************************************
Summary:
    BSID_GetStreamInfo - issue get stream info command.

Description:
    This API parses the image header and return information about the geometry
    of the image contained in the bitstream as well as the output pixel format
    and any additional information that apply to the specific image format
    type. In case of JPEG, PNG and GIF, this API is used to determine the
    surface that shall be allocated for the decoded image. This API shall not
    be used for RLE formats. Please note that SID ARC/HW are not queue driven
    engine, which imply that before post a new "GetStreamInfo" request it is
    mandatory to wait until the current operation is completed.

Parameters:
    - hSidChannel: channel handle.

    - ps_StreamInfo: pointer to the data structure used to provide information
      on where to find the image bitstream and where to return the image header
      information.
Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    - BSID_StreamInfo
******************************************************************************/
BERR_Code BSID_GetStreamInfo(
    BSID_ChannelHandle  hSidChannel,
    BSID_StreamInfo     *ps_StreamInfo
);


/******************************************************************************
Summary:
    BSID_DecodeImageSegment - issue segmented decoding.

Description:
    This API is used to decode the next image segment. This function can be
    blocking or not. When blocking a callback function can be registered to
    receive notification events about decoding such as "Decode done" or
    "Decode error" events.
    Please refer to the definition of BSID_DecodeImageSegments for details on
    the parameters required by this function. Please note that the SID ARC/HW
    are not queue driven engine. Because of this, before posting a new decoding
    request it is mandatory to wait until the current operation is completed.

Parameters:
    - hSidChannel: channel handle.

    - ps_DecodeSegment: pointer to the decode data structure required for
      the decoding operation.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    - BSID_DecodeImage
******************************************************************************/
BERR_Code BSID_DecodeImageSegment(
    BSID_ChannelHandle  hSidChannel,
    const BSID_DecodeSegment  *ps_DecodeSegment
);

/******************************************************************************
Summary:
    BSID_DisableForFlush

Description:
    stop consuming RAVE data.

Parameters:
    - hSidChannel: channel handle.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    -
******************************************************************************/
BERR_Code BSID_DisableForFlush(
    BSID_ChannelHandle  hSidChannel
    );


/******************************************************************************
Summary:
    BSID_FlushChannel - abort the current operation and flushes all the pending
    ones. This is the default behavior when hFlushSettings is NULL.

Description:
    stop the current operation and flushes all the pending ones.

Parameters:
    - hSidChannel: channel handle.

    - hFlushSettings:
      . overwrite default behavior.
Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    -
******************************************************************************/
BERR_Code BSID_FlushChannel(
    BSID_ChannelHandle  hSidChannel,
    const BSID_FlushSettings    *ps_FlushSettings
    );


/******************************************************************************
Summary:
    BSID_SetDMAChunkInfo - used during streaming model, it provides information
    about the next chunk of compressed data SID will need to fetch to continue
    the current operation.

Description:
    Used during streaming model, it provides information about the next chunk of
    compressed data SID will need to fetch to continue the current operation.

Parameters:
    - hSidChannel: channel handle.

    - pDMAChunkInfo: pointer to the structure that is used to provide information
      about the next chunk of compressed data to dma in.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    - BSID_RevisionInfo
******************************************************************************/
BERR_Code BSID_SetDmaChunkInfo(
    BSID_ChannelHandle  hSidChannel,
    const BSID_DmaChunkInfo   *ps_DmaChunkInfo
    );


/******************************************************************************
Summary:
    BSID_SetDMAChunkInfo_Isr - isr safe version of BSID_SetDmaChunkInfo.

Description:
    Used during streaming model, it provides information about the next chunk of
    compressed data SID will need to fetch to continue the current operation.

Parameters:
    - hSidChannel: channel handle.

    - pDMAChunkInfo: pointer to the structure that is used to provide information
      about the next chunk of compressed data to dma in.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    - BSID_RevisionInfo
******************************************************************************/
BERR_Code BSID_SetDmaChunkInfo_Isr(
    BSID_ChannelHandle  hSidChannel,
    const BSID_DmaChunkInfo   *ps_DmaChunkInfo
    );


/******************************************************************************
Summary:
    BSID_GetStatus - status of an operation.

Description:
    This API returns progress and result for BSID_DecodeImage and
    BSID_GetStreamInfo API.

Parameters:
    - hSidChannel: channel handle.

    - ps_Status: pointer to the data structure used to return progress and
      result of the current operation.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    - BSID_Status
******************************************************************************/
BERR_Code BSID_GetStatus(
    BSID_ChannelHandle  hSidChannel,
    BSID_Status         *ps_Status
    );


/******************************************************************************
Summary:
    BSID_StartDecode - start decode command.

Description:
    - Start decode operation for the current channel.
    - Available on Motion Pictures channel only.

Parameters:
    - hSidChannel: channel handle.

    - ps_StartDecodeSettings: pointer to the start decode settings data
      structure.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

    - BERR_INVALID_PARAMETER: one of the parameter is invalid.

See Also:
    - BSID_DecodeImage
******************************************************************************/
BERR_Code BSID_StartDecode(
    BSID_ChannelHandle  hSidChannel,
    const BSID_StartDecodeSettings *ps_StartDecodeSettings
    );


/******************************************************************************
Summary:
    BSID_StopDecode - stop decode command.

Description:
    - Stop decode operation for the current channel.
    - Available on Motion Pictures channel only.

Parameters:
    - hSidChannel: channel handle.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

See Also:
    - BSID_DecodeImage
******************************************************************************/
void BSID_StopDecode(
    BSID_ChannelHandle  hSidChannel
);


/******************************************************************************
Summary:
    BSID_GetRaveItbCdbConfigInfo -

Description:
    - retrieve default ITB/CDB configuration to use for RAVE input during
      motion video decode.

Parameters:
    - ps_ConfigInfo: structure used to return defaults.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

See Also:
    - BSID_DecodeImage
******************************************************************************/
void BSID_GetRaveItbCdbConfigInfo(
    BAVC_CdbItbConfig  *ps_ConfigInfo
);


/******************************************************************************
Summary:
    BSID_GetXdmInterface - provide a mechanism to retrieve XDM interface.

Description:
    - provide a mechanism to retrieve XDM interface.

Parameters:
    - hSidChannel: channel handle.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

See Also:
    - BSID_DecodeImage
******************************************************************************/
void BSID_GetXdmInterface(
    BSID_ChannelHandle  hSidChannel,
    BXDM_Decoder_Interface *ps_XdmInterface,
    void **pCtx
);


/******************************************************************************
Summary:
    BSID_Standby - put SID in standby state.

Description:
    - This api put SID in standby state. During this state SID can be powered
      down.

Parameters:
    - hSid: sid.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

See Also:
    - BSID_DecodeImage
******************************************************************************/
BERR_Code BSID_Standby(
    BSID_Handle  hSid
);


/******************************************************************************
Summary:
    BSID_Resume - resume from standby, restore SID state before BSID_Standby
                  was called.

Description:
    - resume from standby, restore SID state before BSID_Standby was called.
Parameters:
    - hSid: sid.

Returns:
    - BERR_SUCCESS: the operation has been schedule with success.

See Also:
    - BSID_DecodeImage
******************************************************************************/
BERR_Code BSID_Resume(
    BSID_Handle  hSid
);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BSID_H__ */

/* end of file */
