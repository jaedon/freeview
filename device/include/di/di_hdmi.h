

#ifndef __DI_HDMI_H__
#define __DI_HDMI_H__


/**
 * @brief Example showing how to document a function with Doxygen.
 *
 * Description of what the function does. This part may refer to the parameters
 * of the function, like @p param1 or @p param2. A word of code can also be
 * inserted like @c this which is equivalent to <tt>this</tt> and can be useful
 * to say that the function returns a @c void or an @c int. If you want to have
 * more than one word in typewriter font, then just use @<tt@>.
 * We can also include text verbatim,
 * @verbatim like this@endverbatim
 * Sometimes it is also convenient to include an example of usage:
 * @code
 * BoxStruct *out = Box_The_Function_Name(param1, param2);
 * printf("something...\n");
 * @endcode
 * Or,
 * @code{.py}
 * pyval = python_func(arg1, arg2)
 * print pyval
 * @endcode
 * when the language is not the one used in the current source file (but
 * <b>be careful</b> as this may be supported only by recent versions
 * of Doxygen). By the way, <b>this is how you write bold text</b> or,
 * if it is just one word, then you can just do @b this.
 * @param param1 Description of the first parameter of the function.
 * @param param2 The second one, which follows @p param1.
 * @return Describe what the function returns.
 * @see Box_The_Second_Function
 * @see Box_The_Last_One
 * @see http://website/
 * @note Something to note.
 * @warning Warning.
 */

/*******************************************************************/
/**************************** Header Files**************************/
/*******************************************************************/
#include "htype.h"
#include "di_err.h"

/*******************************************************************/
/************************ Extern variables *************************/
/*******************************************************************/

/*******************************************************************/
/************************ Macro Definition *************************/
/*******************************************************************/

/*******************************************************************/
/****************************** typedef ****************************/
/*******************************************************************/


/**
 * @brief typedef of enum DI_HDMI_EVENT
 *
 * DI will callback these events to Middleware.
 *
 * @see  DI_HDMI_RegisterEventNotify()
 */

typedef enum
{
	DH_EVENT_HOTPLUG_CONNECT=0,
	DH_EVENT_HOTPLUG_DISCONNECT,
	DH_EVENT_HOTPLUG_POWER_DOWN,
	DH_EVENT_HDCP_SUCCESS,
	DH_EVENT_HDCP_FAIL,
	DH_EVENT_CEC_MSG_RECEIVED,
	DH_EVENT_MAX
} DI_HDMI_EVENT;


/**
 * @brief typedef of enum HDMI_RECEIVER_INFO_OPTION
 *
 * Middleware can question di what is the state of XXX.
 *
 * IS_CONNECTED : whether HDMI device is connected or not.
 * IS_HDMI_DEVICE : whether connected device is HDMI device or not.
 * IS_AUTHENCATED : whether connected device is HDCP authenticated or not.
 * IS_HDCP22_SINK : whether connected device is HDCP 2.2 authentication protocol or not
 * @see  DI_HDMI_GetState()
 */

typedef enum
{
	IS_CONNECTED=0x01,
	IS_HDMI_DEVICE=0x02,
	IS_AUTHENCATED=0x04,
	IS_HDCP22_SINK=0x08
} HDMI_RECEIVER_INFO_OPTION;

/**
 * @brief typedef of enum DI_HDMI_AUDIO_FS_e
 *
 *
 * @see  DI_HDMI_GetState()
 */

typedef enum
{
	DI_HDMI_FS_32KHz  =  0x1,
	DI_HDMI_FS_44KHz  =  0x2,
	DI_HDMI_FS_48KHz  =  0x4,
	DI_HDMI_FS_88KHz  =  0x8,
	DI_HDMI_FS_96KHz  = 0x10,
	DI_HDMI_FS_176KHz = 0x20,
	DI_HDMI_FS_192KHz = 0x40,
	DI_HDMI_FS_MAX = 0x80
}DI_HDMI_AUDIO_FS_e;


/**
 * @brief typedef of enum DI_HDMI_AUDIO_FORMAT
 *
 * Defines audio format which is supported by peer device.
 *
 * @see  P_HDMI_GetCapability()
 */

typedef enum
{
	DI_HDMI_AFORMAT_PCM     = 0x0001,
	DI_HDMI_AFORMAT_AC3     = 0x0002,
	DI_HDMI_AFORMAT_MPEG1   = 0x0004,
	DI_HDMI_AFORMAT_MP3     = 0x0008,
	DI_HDMI_AFORMAT_MPEG2   = 0x0010,
	DI_HDMI_AFORMAT_AAC     = 0x0020,
	DI_HDMI_AFORMAT_DTS     = 0x0040,

	// The followings are newly added to 7405 bavc.h
	DI_HDMI_AFORMAT_AVS     = 0x0080,
	DI_HDMI_AFORMAT_ATRAC   = 0x0100,
	DI_HDMI_AFORMAT_OneBit  = 0x0200,
	DI_HDMI_AFORMAT_AC3PLUS = 0x0400,
	DI_HDMI_AFORMAT_DTSHD   = 0x0800,
	DI_HDMI_AFORMAT_MATMLP  = 0x1000,
	DI_HDMI_AFORMAT_DST     = 0x2000,
	DI_HDMI_AFORMAT_WMAPro  = 0x4000,
	DI_HDMI_AFORMAT_UNKNOWN = 0x8000
} DI_HDMI_AUDIO_FORMAT;


/**
 * @brief typedef of enum DI_HDMI_VIDEO_FORMAT
 *
 * Defines video format which is supported by peer device.
 * This is deprecated so don't use this enum. only used in DI 1.0
 * Use DI_HDMI_EDID_VFORMAT please.
 *
 * @see  P_HDMI_GetCapability()
 * @see DI_HDMI_EDID_VFORMAT
 */

typedef enum
{
	DI_HDMI_VFORMAT_480I        = 0x00000001,
	DI_HDMI_VFORMAT_480I_J      = 0x00000002,
 	DI_HDMI_VFORMAT_480P        = 0x00000004,
 	DI_HDMI_VFORMAT_576I        = 0x00000008,
	DI_HDMI_VFORMAT_576P        = 0x00000010,
	DI_HDMI_VFORMAT_720P        = 0x00000020,
	DI_HDMI_VFORMAT_1080I       = 0x00000040,
	DI_HDMI_VFORMAT_720P_50HZ   = 0x00000080,
	DI_HDMI_VFORMAT_1080I_50HZ  = 0x00000100,
	DI_HDMI_VFORMAT_1080P_50HZ  = 0x00000200,
	DI_HDMI_VFORMAT_480I_4_3    = 0x00000400,
	DI_HDMI_VFORMAT_480P_4_3    = 0x00000800,
	DI_HDMI_VFORMAT_576I_4_3    = 0x00001000,
	DI_HDMI_VFORMAT_576P_4_3    = 0x00002000,
	DI_HDMI_VFORMAT_1080P_24HZ  = 0x00004000,
	DI_HDMI_VFORMAT_1080P_30HZ  = 0x00008000,
	DI_HDMI_VFORMAT_1080P       = 0x00010000,
	DI_HDMI_VFORMAT_2160P_24HZ  = 0x00020000,
	DI_HDMI_VFORMAT_2160P_25HZ  = 0x00040000,
	DI_HDMI_VFORMAT_2160P_30HZ  = 0x00080000,
	DI_HDMI_VFORMAT_2160P_50HZ  = 0x00100000,
	DI_HDMI_VFORMAT_2160P_60HZ  = 0x00200000,
	DI_HDMI_VFORMAT_1080P_25HZ	= 0x00400000,
	DI_HDMI_VFORMAT_DUMMY       = 0x40000000
} DI_HDMI_VIDEO_FORMAT;


/**
 * @brief typedef of enum DI_HDMI_VIDEO_RESOLUTION
 *
 * Defines supported video resolution.
 *
 * @see  P_HDMI_GetCapability()
 */

typedef enum
{
	DI_HDMI_RESOLUTION_Unknown			=0x00000000,
	DI_HDMI_RESOLUTION_720x240P			=0x00000001,
	DI_HDMI_RESOLUTION_2880x240P		=0x00000002,
	DI_HDMI_RESOLUTION_640x480P			=0x00000004,
	DI_HDMI_RESOLUTION_720x480I			=0x00000008,
	DI_HDMI_RESOLUTION_720x480P			=0x00000010,
	DI_HDMI_RESOLUTION_1440x480P		=0x00000020,
	DI_HDMI_RESOLUTION_2880x480I		=0x00000040,
	DI_HDMI_RESOLUTION_2880x480P		=0x00000080,
	DI_HDMI_RESOLUTION_720x288P			=0x00000100,
	DI_HDMI_RESOLUTION_2880x288P 		=0x00000200,
	DI_HDMI_RESOLUTION_720x576I			=0x00000400,
	DI_HDMI_RESOLUTION_720x576P			=0x00000800,
	DI_HDMI_RESOLUTION_1440x576P		=0x00001000,
	DI_HDMI_RESOLUTION_2880x576I 		=0x00002000,
	DI_HDMI_RESOLUTION_2880x576P		=0x00004000,
	DI_HDMI_RESOLUTION_1080x720P		=0x00008000,
	DI_HDMI_RESOLUTION_1280x720P		=0x00010000,
	DI_HDMI_RESOLUTION_1920x1080I		=0x00020000,
	DI_HDMI_RESOLUTION_1920x1080P		=0x00040000,
	DI_HDMI_RESOLUTION_1680x720P		=0x00080000,
	DI_HDMI_RESOLUTION_2560x1080P		=0x00100000,
	DI_HDMI_RESOLUTION_3840x2160P		=0x00200000,
	DI_HDMI_RESOLUTION_4096x2160P		=0x00400000,
	DI_HDMI_RESOLUTION_DUMMY			=0x0FFFFFFF
} DI_HDMI_VIDEO_RESOLUTION;


/**
 * @brief typedef of enum DI_HDMI_VIDEO_FIELD_RATE
 *
 * Defines type of field rate.
 *
 */

typedef enum
{
	DI_HDMI_FIELD_RATE_Unknown		=0x0000,
	DI_HDMI_FIELD_RATE_24HZ			=0x0001,
	DI_HDMI_FIELD_RATE_25HZ 		=0x0002,
	DI_HDMI_FIELD_RATE_30HZ 		=0x0004,
	DI_HDMI_FIELD_RATE_50HZ 		=0x0008,
	DI_HDMI_FIELD_RATE_60HZ 		=0x0010,
	DI_HDMI_FIELD_RATE_100HZ 		=0x0020,
	DI_HDMI_FIELD_RATE_120HZ 		=0x0040,
	DI_HDMI_FIELD_RATE_200HZ		=0x0080,
	DI_HDMI_FIELD_RATE_240HZ		=0x0100,
	DI_HDMI_FIELD_RATE_DUMMY		=0xFFFF
} DI_HDMI_VIDEO_FIELD_RATE;


/**
 * @brief typedef of enum DI_HDMI_VIDEO_AR
 *
 * Defines type of aspect ratio.
 *
 * @see  DI_HDMI_SetAuxVideoInfo()
 */

typedef enum
{
	DI_HDMI_VIDEO_AR_eNoData = 0,
	DI_HDMI_VIDEO_AR_e4_3,
	DI_HDMI_VIDEO_AR_e16_9
}DI_HDMI_VIDEO_AR;


/**
 * @brief typedef of enum DI_HDMI_VIDEO_AFD
 *
 * Defines type of active format descriptor.
 *
 * @see  DI_HDMI_SetAuxVideoInfo()
 */

typedef enum
{
	DI_HDMI_VIDEO_AFD_ePicture    =  8,
	DI_HDMI_VIDEO_AFD_e4_3Center  =  9,
	DI_HDMI_VIDEO_AFD_e16_9Center = 10,
	DI_HDMI_VIDEO_AFD_e14_9Center = 11,
	DI_HDMI_VIDEO_AFD_e4_3_SNP_14_9Center	= 13,
	DI_HDMI_VIDEO_AFD_e16_9_SNP_14_9Center	= 14,
	DI_HDMI_VIDEO_AFD_e16_9_SNP_4_3Center	= 15
}DI_HDMI_VIDEO_AFD;


/**
 * @brief typedef of enum DI_HDMI_VIDEO_COLORSPACE
 *
 * Defines type of color space.
 *
 * @see  DI_HDMI_SetAuxVideoInfo()
 */

typedef enum
{
    DI_HDMI_VIDEO_ColorSpace_eRgb = 0,
    DI_HDMI_VIDEO_ColorSpace_eYCbCr422,
    DI_HDMI_VIDEO_ColorSpace_eYCbCr444,
    DI_HDMI_VIDEO_ColorSpace_eAuto,
	DI_HDMI_VIDEO_ColorSpace_eYCbCr420,
    DI_HDMI_VIDEO_ColorSpace_eMax
} DI_HDMI_VIDEO_COLORSPACE;


/**
 * @brief typedef of enum DI_HDMI_3D_FORMAT
 *
 * Defines 3d video format of mine.
 * This enum is only used by DI 2.0
 *
 * @see  DI_HDMI_Set3DFormat()
 */

typedef enum
{
	DI_HDMI_3D_FORMAT_NONE 		= 0x0000,
	DI_HDMI_3D_FORMAT_SIDEBYSIDE	= 0x0001,
	DI_HDMI_3D_FORMAT_TOPANDBOTTOM 	= 0x0002,
	DI_HDMI_3D_FORMAT_FRAMEPACKING	= 0x0004
} DI_HDMI_3D_FORMAT;


/**
 * @brief typedef of enum DI_HDMI_AUTOLIPSYNC_STATE
 *
 * Defines auto lip sync state.
 * This is used by TR069 api
 *
 * @see  DI_HDMI_GetAutoLipSyncInfo()
 */

typedef enum
{
	DI_HDMI_AUTOLIPSYNC_OFF		= 0,
	DI_HDMI_AUTOLIPSYNC_ON
} DI_HDMI_AUTOLIPSYNC_STATE;


/**
 * @brief typedef of enum DI_HDMI_CEC_STATE
 *
 * Defines CEC state.
 * This is used by TR069 api
 *
 * @see  DI_HDMI_GetCECInfo()
 */

typedef enum
{
	DI_HDMI_CEC_NO_DEVICE		= 0,
	DI_HDMI_CEC_DEVICE
} DI_HDMI_CEC_STATE;


/**
 * @brief typedef of enum DI_HDMI_HDCP_VERSION
 *
 * Defines version of HDCP.
 * This is used by TR069 api
 *
 * @see  DI_HDMI_GetHdcpVersion()
 */

typedef enum
{
	DI_HDMI_HDCP_VERSION_1_0	= 0,
	DI_HDMI_HDCP_VERSION_1_1,
	DI_HDMI_HDCP_VERSION_1_2,
	DI_HDMI_HDCP_VERSION_1_3,
	DI_HDMI_HDCP_VERSION_1_4,
	DI_HDMI_HDCP_VERSION_2_0,
	DI_HDMI_HDCP_VERSION_2_1,
	DI_HDMI_HDCP_VERSION_2_2,
	DI_HDMI_HDCP_VERSION_MAX,
} DI_HDMI_HDCP_VERSION;


/**
 * @brief typedef of enum DI_HDMI_INFO_FRAME
 *
 * Control and Configuration frame info
 *
 * @see  DI_HDMI_INFO_FRAME()
 */

typedef enum
{
	DI_HDMI_AVI_INFO = 0x0000,
	DI_HDMI_COLOR_SPACE_INFO = 0x0001,
	DI_HDMI_3D_INFO	= 0x0002
} DI_HDMI_INFO_FRAME;


/**
 * @brief typedef of struct DI_HDMI_EDID_VFORMAT
 *
 * Defines video format which can be known by parsing EDID.
 *
 * @see  P_HDMI_GetCapability()
 */

typedef struct
{
	HUINT32		ulVFormat;
	HUINT32		ulFieldRate;
	HUINT32 	ulAspectRatio;
} DI_HDMI_EDID_VFORMAT;


/**
 * @brief typedef of struct DI_HDMI_EDID_VFORMAT
 *
 * Defines available format which can be known by parsing EDID.
 * This will show capability of peer device.
 *
 * @see  P_HDMI_GetCapability()
 */

#define CEA_861_MAX_NUM_VIDEO_ID_CODE	107	/* defined in CEA-861-F */

typedef struct
{
	HUINT32 ulVideoFormats;	/* deprecated. used in DI 1.0. don't use this in DI 2.0 cause we can't represent required video formats */
	HUINT32 ulAudioFormats;	/* deprecated. use tAudioInfo please */
	HUINT32 ulLogicalAddr;	/* logical address of cec device. it is my logical address */

	HUINT32					ulNumOfVFormat;	/* number of available video format */
	DI_HDMI_EDID_VFORMAT	stVformats[CEA_861_MAX_NUM_VIDEO_ID_CODE];	/* available video format */

	struct
	{
		HUINT32 NodeA:4;
		HUINT32 NodeB:4;
		HUINT32 NodeC:4;
		HUINT32 NodeD:4;
	}sPhysicalAddr;	/* cec physical address */

	struct
	{
		HUINT32 ulAudioFormat;	/* audio format */
		HUINT32 unSupportedFs;	/* audio sampling rate */
	} tAudioInfo[16];	/* available audio codec / format */

	HINT32 ul3dFormats;	/* available 3d video format */
} DI_HDMI_AVAILABLE_FORMATS;


/**
 * @brief typedef of struct HDMI_RECEIVER_INFO
 *
 * Represents HDMI receiver's state.
 *
 * @see  DI_HDMI_GetState()
 */

typedef struct
{
	HBOOL bIsConnected;
	HBOOL bIsHdmiDevice;
	HBOOL bIsAuthencated;
	HBOOL bIsHDCP22Sink;/* Receiver supports HDCP 2.2 authentication protocol */
} HDMI_RECEIVER_INFO;


/**
 * @brief typedef of struct DI_HDMI_CEC_MSG_t
 *
 * If we use HDMI CED, this struct will be used.
 *
 * @see  DI_HDMI_SendCecMsg()
 */

typedef struct
{
	HUINT8  ucDestAddr;
	HUINT8  ucInitAddr;
	HUINT8  ucMessage[16];
	HUINT8  ucMegLength;
} DI_HDMI_CEC_MSG_t;


/**
 * @brief typedef of struct DI_HDMI_EDID_INFO
 *
 * Edid info wanted by middleware.
 *
 * @see  DI_HDMI_GetEdidInfo()
 */

typedef struct
{
	HUINT8 vendorID[2];		/* Vendor ID from Microsoft; compressed ASCII */
	HUINT8 productID[2];	/* Product ID assigned by Vendor */
	HUINT8 serialNum[4];	/* Serial # assigned by Vendor; may be undefined */
	HUINT8 monitorName[14];
	HUINT8 manufWeek;		/* Week of Manufture (1..53)			*/
	HINT16 manufYear;		/* Year of Manufacture + 1990 		*/
	HUINT8 edidVersion;		/* Version of Edid (version 1 or 2)	*/
	HUINT8 edidRevision;	/* Revision of Edid 1.3 or 2.0		*/
	HUINT8 maxHorizSize;	/* Max Horizontal Image size in cm		*/
	HUINT8 maxVertSize;		/* Max Vertical Image size in cm		*/
	HUINT8 extensions;		/* Number of 128 byte EDID extensions	*/
	HUINT32 firstVideoId;

	DI_HDMI_EDID_VFORMAT preferredVideoFormat;	/* peer device's preferred video format */
} DI_HDMI_EDID_INFO;


/**
 * @brief typedef of struct DI_HDMI_AUX_VIDEO_INFO
 *
 * Aux video info. This struct contains AR(Aspect Ratio), AFD(Active Format Descriptor), Color space.
 *
 * @see  DI_HDMI_SetAuxVideoInfo()
 */

typedef struct
{
	DI_HDMI_VIDEO_AR eAR;
 	DI_HDMI_VIDEO_AFD eAFD;
	DI_HDMI_VIDEO_COLORSPACE eColorSpace;
} DI_HDMI_AUX_VIDEO_INFO;


/**
 * @brief typedef of struct DI_HDMI_AUTOLIPSYNC_INFO
 *
 * Auto lipsync info. used by TR069.
 *
 * @see  DI_HDMI_GetAutoLipSyncInfo()
 */

typedef struct
{
	HUINT8		ucVideo;
	HUINT8		ucAudio;
	HUINT8 		ucInterlacedVideo;
	HUINT8 		ucInterlacedAudio;
	DI_HDMI_AUTOLIPSYNC_STATE 		eState;
} DI_HDMI_AUTOLIPSYNC_INFO;


/**
 * @brief typedef of struct DI_HDMI_EDID_RAW
 *
 * Raw edid data. used by TR069.
 *
 * @see  DI_HDMI_GetRawEdid()
 */

#define HDMI_EDID_MAX_EXTENSIONS 127
#define HDMI_EDID_BLOCKSIZE 128

typedef struct
{
    HUINT8 data[HDMI_EDID_BLOCKSIZE+HDMI_EDID_BLOCKSIZE*HDMI_EDID_MAX_EXTENSIONS]; /* basic block + extension block */
} DI_HDMI_EDID_RAW;

/**
Summary:
Vendor Specific Infoframe for HDMI output
**/
typedef struct DI_HDMI_VENDOR_SPECIFIC_INFO
{
	DI_HDMI_3D_FORMAT e3DFormat;
} DI_HDMI_VENDOR_SPECIFIC_INFO;


/**
 * @brief typedef of struct DI_HDMI_SPECIFIC_INFO
 *
 * Aux video info. This struct contains AR(Aspect Ratio), AFD(Active Format Descriptor), Color space.
 *
 * @see  DI_HDMI_SetAuxVideoInfo()
 */

typedef struct
{
	DI_HDMI_INFO_FRAME eFrame;

	DI_HDMI_VIDEO_AR eAR;
 	DI_HDMI_VIDEO_AFD eAFD;
	DI_HDMI_VIDEO_COLORSPACE eColorSpace;

	DI_HDMI_3D_FORMAT e3DFormat;
} DI_HDMI_SPECIFIC_INFO_t;

/**
 * @brief typedef of struct DI_HDMI_INPUT_OUTPUT_INFO
 *
 * hdmi input & output port info.
 *
 * @see  DI_HDMI_PORT_INFO_t()
 */

typedef struct
{
	HUINT32		ulInputPort;
	HUINT32		ulOutputPort;
} DI_HDMI_PORT_INFO_t;


/**
 * @brief typedef of function pointer DI_HDMI_CALLBACK
 *
 * Middleware use this to get call back by DI.
 *
 */

typedef void (*DI_HDMI_CALLBACK)(void*);


/*******************************************************************/
/******************** global function prototype ********************/
/*******************************************************************/


/**
 * @brief DI_HDMI_GetRawEdid
 *
 * Returns raw edid data. used by TR069.
 *
 * @param DI_HDMI_EDID_RAW* pEdid. [output]
 * @return DI_ERR_CODE
 * @see
 */
DI_ERR_CODE DI_HDMI_GetRawEdid(DI_HDMI_EDID_RAW* pEdid);


/**
 * @brief DI_HDMI_GetAutoLipSyncInfo
 *
 * Returns auto libsync info. used by TR069.
 *
 * @param DI_HDMI_AUTOLIPSYNC_INFO* pstInfo. [output]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_GetAutoLipSyncInfo(DI_HDMI_AUTOLIPSYNC_INFO* pstInfo);


/**
 * @brief DI_HDMI_GetCECInfo
 *
 * Returns CEC device status.
 *
 * @param DI_HDMI_CEC_STATE* pState. [output]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_GetCECInfo(DI_HDMI_CEC_STATE* pState);


/**
 * @brief DI_HDMI_GetPortOutNum
 *
 * Returns the number of HDMI output.
 *
 * @param HUINT8* ucNum. [output]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_GetPortOutNum(HUINT8* ucNum);


/**
 * @brief DI_HDMI_GetHdcpVersion
 *
 * Returns version of HDCP which is used between peer device.
 *
 * @param DI_HDMI_HDCP_VERSION *eHdcpVersion. [output]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_GetHdcpVersion(DI_HDMI_HDCP_VERSION *eHdcpVersion);


/**
 * @brief DI_HDMI_EnableHDCP
 *
 * Enable HDCP Authentication process.
 *
 * @param HBOOL bEnable. [input]
 * @return DI_ERR_CODE
 * @see P_HDMI_EnableHDCP()
 */

DI_ERR_CODE DI_HDMI_EnableHDCP(HBOOL bEnable);


/**
 * @brief DI_HDMI_GetState
 *
 * Returns HDMI receiver state as input. Input is ReceiverInfoOption and middleware can or-ing these values.
 * For example, middleware can query HDMI receiver state by setting ReceiverInfoOption like
 * ReceiverInfoOption = IS_CONNECTED | IS_HDMI_DEVICE | IS_AUTHENCATED;
 *
 * @param HDMI_RECEIVER_INFO_OPTION ReceiverInfoOption. [input]
 * @param HDMI_RECEIVER_INFO* pReceiverInfo. [output]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_GetState(HDMI_RECEIVER_INFO_OPTION ReceiverInfoOption, HDMI_RECEIVER_INFO* pReceiverInfo);


/**
 * @brief DI_HDMI_RegisterEventNotify
 *
 * Middleware will register callback function by using this function.
 *
 * @param DI_HDMI_EVENT eHdmiEvent. [input]
 * @param DI_HDMI_CALLBACK pfnHdmiEventCallback. [input]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_RegisterEventNotify(DI_HDMI_EVENT eHdmiEvent, DI_HDMI_CALLBACK pfnHdmiEventCallback);


/**
 * @brief DI_HDMI_GetCapability
 *
 * Returns necessary information to control HDMI.
 *
 * @param DI_HDMI_AVAILABLE_FORMATS* pAvailable. [output]
 * @return DI_ERR_CODE
 * @see DI_HDMI_AVAILABLE_FORMATS
 */

DI_ERR_CODE DI_HDMI_GetCapability(DI_HDMI_AVAILABLE_FORMATS* pAvailable);


/**
 * @brief DI_HDMI_EnableOutput
 *
 * Enable HDMI output.
 * If bEnable is true, playback dummy audio(volume level 0 audio), add display output.
 * If bEnable is false, stop playback audio and remove display output.
 *
 * @param HBOOL bEnable. [input]
 * @return DI_ERR_CODE
 * @see P_HDMI_EnableHdmiMode
 */

DI_ERR_CODE DI_HDMI_EnableOutput (HBOOL bEnable);


/**
 * @brief DI_HDMI_SendCecMsg
 *
 * Send cec message to attached cec supporting device.
 *
 * @param DI_HDMI_CEC_MSG_t* pstCecMsg. [input]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_SendCecMsg(DI_HDMI_CEC_MSG_t* pstCecMsg);


/**
 * @brief DI_HDMI_GetLogicalAddress
 *
 * Returns logical address of cec.
 *
 * @param HUINT8* ucLogicalAddr. [output]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_GetLogicalAddress(HUINT8* ucLogicalAddr);


/**
 * @brief DI_HDMI_GetEdidInfo
 *
 * Returns EDID info of attached device.
 *
 * @param DI_HDMI_EDID_INFO* pEdidInfo. [output]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_GetEdidInfo(DI_HDMI_EDID_INFO* pEdidInfo);


/**
 * @brief DI_HDMI_SetAVMute
 *
 * Do not sent AV data on HDMI.
 *
 * @param HBOOL bEnable. [input]
 * @return DI_ERR_CODE
 * @warning Do not use this function. deprecated.
 * @see P_HDMI_SetAVMute
 */

DI_ERR_CODE DI_HDMI_SetAVMute(HBOOL bEnable);


/**
 * @brief DI_HDMI_SetAVBlank
 *
 * Do not sent AV data on HDMI.
 *
 * @param HBOOL bBlank. [input]
 * @return DI_ERR_CODE
 * @warning Do not use this function. deprecated.
 * @see P_HDMI_SetAVBlank
 */

DI_ERR_CODE DI_HDMI_SetAVBlank(HBOOL bBlank);


/**
 * @brief DI_HDMI_SetAuxVideoInfo
 *
 * Set Auxiliary Video Info Frame data.
 *
 * @param DI_HDMI_AUX_VIDEO_INFO stAVInfo. [input]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_SetAuxVideoInfo(DI_HDMI_AUX_VIDEO_INFO stAVInfo);


/**
 * @brief DI_HDMI_SetColorSpace_Info
 *
 * Set color space of HDMI output.
 *
 * @param DI_HDMI_AUX_VIDEO_INFO stAVInfo. [input]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_SetColorSpace_Info(DI_HDMI_AUX_VIDEO_INFO stAVInfo);


/**
 * @brief DI_HDMI_Set3DFormat
 *
 * Set 3D video format info to Vendor Specific Info Frame Data.
 *
 * @param DI_HDMI_3D_FORMAT e3dFormat. [input]
 * @return DI_ERR_CODE
 * @see
 */

DI_ERR_CODE DI_HDMI_Set3DFormat(DI_HDMI_3D_FORMAT e3dFormat);


/**
 * @brief DI_HDMI_SetPowerState
 *
 * If HDMI_POWER_CONTROL is defined, to control HDMI power, use this function.
 * In some case, disable tmds signal and in other case, just set device power to off in NEXUS.
 *
 * @param HBOOL bPowerOn. [input]
 * @return DI_ERR_CODE
 * @see NEXUS_HdmiOutput_SetPowerState
 * @see P_HDMI_SetTmdsSignal
 */

DI_ERR_CODE DI_HDMI_SetPowerState(HBOOL bPowerOn);

/**
 * @brief DI_HDMI_GetPreferredVideoFormat
 *
 * Returns sink device's preverred video format.
 * The first DTD(detailed timing descriptor) or SVD(short video descriptor) should be the preferred video format.
 * We consider the first SVD as preferred video format and this function will returns that.
 *
 * @param DI_HDMI_EDID_VFORMAT
 * @return DI_ERR_CODE
 */

DI_ERR_CODE DI_HDMI_GetPreferredVideoFormat(DI_HDMI_EDID_VFORMAT *pstVideoFormat);

/**
 * @brief DI_HDMI_HDCP22KeyProvisioning
 *
 * Provisioning and binding operation about HDCP22 key
 *
 * @param encrypted HDCP22 key & size
 * @return DI_ERR_CODE
 */
DI_ERR_CODE DI_HDMI_HDCP22KeyProvisioning(HUINT8 *pucInput, HUINT32 nKeySize, HUINT8 *pucOutput);

#if defined(CONFIG_HDMI_CEC)

/**
 * @brief DI_HDMI_CEC_Init
 *
 * Init Hdmi cec Core
 *
 * @return DI_ERR_CODE
 */
DI_ERR_CODE DI_HDMI_CEC_Init(void);

#endif

#endif /* !__DI_HDMI_H__ */

