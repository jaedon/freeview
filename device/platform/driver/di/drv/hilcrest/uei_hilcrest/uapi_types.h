/**
 * @file        uapi_types.h
 * @brief       This file contains the structure definitions used by UAPI
 **/

/*===================================================================
 COPYRIGHT 2010 UNIVERSAL ELECTRONICS INC (UEI).
 These materials are provided under license by UEI.  UEI licenses
 this file to you under the Universal Remote Control API (UAPI)
 License accompanying the UEI libuapi Software (the "License").
 You may not use this file except in compliance with the License.
 You may obtain a copy of the License from UEI. Unless required by
 applicable law or agreed to in writing, all materials distributed
 under the License is distributed on an "AS IS" BASIS, WITHOUT
 WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions
 and limitations under the License.
===================================================================*/

/******************************************************************************
* Filename:     UAPI.h
* Description: 
*******************************************************************************
*/

#ifndef __UAPI_INTERFACE_H__
#define __UAPI_INTERFACE_H__
#ifdef __cplusplus
extern "C"{
#endif

#include "uapi_defs.h"


/*! \def UAPI_VERSION_SIZE
 *  *   The maximum size of UAPI version information is 4 bytes
 *   */
#define UAPI_VERSION_SIZE       0x04

/*! \def UAPI_PRODUCT_NAME_SIZE
 *  *   The maximum size of UAPI product name information is 10 bytes
 *   */
#define UAPI_PRODUCT_NAME_SIZE  10

/*! \def UAPI_RF4CE_NIF
 *   The UAPI RF4CE Network Interface is defined as 0x00
 */
#define UAPI_RF4CE_NIF			0x00
/*! \def MAX_REMOTEAPI_DATA_SIZE                                   
 *   The maximum remote API data size is 1040
 */
#define MAX_REMOTEAPI_DATA_SIZE 1040
/**
 * @brief       defines enum of all features available in UAPI
 */
typedef enum {
    UAPIFeatureIDGeneralServices        = 0x00,
    UAPIFeatureIDOTA                    = 0x01,
    UAPIFeatureIDRemoteAPI              = 0x11,
    UAPIFeatureIDUserActivity           = 0x20,
    UAPIFeatureIDUrcKey                 = 0x21,
    UAPIFeatureIDText                   = 0x22,
    UAPIFeatureIDRemoteFinder           = 0x25,
    UAPIFeatureIDOrientation            = 0x31,
    UAPIFeatureIDRelativeMotion         = 0x32,
    UAPIFeatureIDHighResRelativeMotion  = 0x33,
    UAPIFeatureIDAbsolutePosition       = 0x34,
    UAPIFeatureIDHighResAbsolutePosition= 0x35,
    UAPIFeatureIDStorage                = 0x37,
    UAPIFeatureIDBatteryStatus          = 0x43,
    UAPIFeatureIDRawSensorData          = 0x44

} UAPIFeatureID_t;
/**
 * @brief       defines structure provided when keys are pressed on remote
 */
typedef struct {
    /** release (0), press (1), or repeat (2) */
	UINT8 eventType;
    /** USB HID Usage table */
	UINT8 usagePage;
    /** ID of key */
	UINT16 keyID;
}UAPI_URCKeyPress_t;
/**
 * @brief       defines structure provided when text entry data is generated
 */
typedef struct {
    /** press (0), release (1) */
    UINT8 eventType;
    /** UTF8 (1), UTF16 (2), UTF32 (3) */
	UINT8 charType;
	union{
        /** UTF8 character valid if charType is 1 */
		UINT8 UTF8char[1];
        /** UTF16 character valid if charType is 2 */
		UINT8 UTF16char[2];
        /** UTF32 character valid if charType is 3 */
		UINT8 UTF32char[4];
	}value;
}UAPI_TextEntry_t;
/**
 * @brief       defines structure showing button mapping
 *            
 */
typedef struct {
    /** 1 bit for button 1 */
    INT8 button1;
    /** 1 bit for button 2 */
    INT8 button2;
    /** 1 bit for button 3 */
    INT8 button3;
    /** 4 bits reserved */
	INT8 reserved;
    /** 1 bit for finger present */
	INT8 fingerPresent;
}UAPI_ButtonInfo_t;
/**
 * @brief       defines structure provided when relative motion data is generated
 *            
 */

typedef struct {
    /** button mapping */
	UAPI_ButtonInfo_t buttonInfo;
    /** displacement relative to direction of travel on X axis */
	INT8 xDisplacement;
    /** displacement relative to direction of travel on Y axis */
	INT8 yDisplacement;
    /** vertical scroll value */
	INT8 verticalScroll;
    /** horizontal scroll value */
	INT8 horizontalPan;
}UAPI_RelativeMotion_t;

/**
 * @brief       defines structure provided when high resolution relative motion data is generated
 */
typedef struct {
    /** button mapping info */
    UAPI_ButtonInfo_t buttonInfo;
    /** displacement relative to direction of travel on X axis */
    INT16 xDisplacement;
    /** displacement relative to direction of travel on Y axis */
    INT16 yDisplacement;
    /** vertical scroll value */
    INT8 verticalScroll;
    /** horizontal scroll value */
    INT8 horizontalPan;
}UAPI_HighResRelativeMotion_t;

/**
 * @brief       defines structure provided when absolute motion data is generated
 *            
 */
typedef struct {
    /** button mapping info */
	UAPI_ButtonInfo_t buttonInfo;
    /** absolute position of finger on X axis */
	UINT8 xPosition;
    /** absolute position of finger on Y axis */
	UINT8 yPosition;
}UAPI_AbsoluteMotion_t;

/**
 * @brief       defines structure provided when high resolution absolute motion is generated
 */
typedef struct {
    /** button mapping info */
    UAPI_ButtonInfo_t buttonInfo;
    /** absolute position of finger on X axis */
    UINT16 xPosition;
    /** absolute position of finger on Y axis */
    UINT16 yPosition;
}UAPI_HighResAbsoluteMotion_t;

/**
 * @brief       defines structure provided when orientation data is generated
 *        
 */
typedef struct {
    /** download facing */
    INT32 Down:1;
    /** upward facing */
    INT32 Up:1;
    /** portrait facing down */
    INT32 PortraitDown:1;
    /** portrait facing up */
    INT32 PortraitUp:1;
    /** landscape facing right */
    INT32 LandscapeRight:1;
    /** landscape facing left */
    INT32 LandscapeLeft:1;
    /** reserved */
	INT32 Reserved:2;
} UAPI_Orientation_t;
/**
 * @brief       defines structure provided when user activity data is generated
 * 
 */
typedef struct {
    /** feature ID generating the wakeup */
	UINT16 featureID;
}UAPI_UserActivity_t;
/**
 * @brief       defines structure provided when Remote API data is generated
 *
 */
typedef struct {
    /** record length of Remote API data */
	UINT16 recordLength;
    /** remote API data */
	UINT8 remoteAPIdata[MAX_REMOTEAPI_DATA_SIZE];
}UAPI_RemoteAPI_t;
/**
 * @brief       defines structure provided when battery status data is generated
 *
 */
typedef struct {
    /** OK, Warning, Critical, Unknown (0x00, 0x01, 0x02, 0xFF) */
    UINT8 status;
    /** level, in percentage */
    UINT8 level;
    /** voltage/2048 = actual voltage */
    UINT16 voltage;
}UAPI_BatteryStatus_t;
/**
 * @brief       defines structure provided when UAPI data is generated from the remote
 *              
 */
typedef struct{
    /** logical ID of remote generating data */
	UINT8 remoteId;
    /** communication interface data was received over */
	UINT8 devId;
    /** feature generating the data */
	UAPIFeatureID_t featureId;
    /** the data from the remote control */
	VOID* featureData;
} UAPIFeatureData_t;

/** 
 * @brief UAPI Header structure
 * **/
typedef struct{
    /** UAPI Options */
    UINT8    UAPIOptions;
    /** sequence number */
    UINT8    seqNum;
    /** feature ID */
    UINT8    appID;
    /** UAPI code */
    UINT8    opCode;
} UAPIHdr_t;

/** 
 * @ brief      UAPI Data packet structure
 * **/
typedef struct {
    /** communication interface data is communicated over */
    UINT16              deviceID;
    /** logical ID of remote */
    UINT8				remoteID;
    /** length of the data */
    UINT16              dataLength;
    /** UAPI header */
    UAPIHdr_t	        uapiHdr;
    /** pointer to data */
    UINT8               *data;
} UAPIDataPkt;

/** 
 * @brief      XMP1 data packet structure
 *
 * **/
typedef struct {
    /** 4 byte registry packet */
    UINT8               registryPacket[4];
    /** 4 byte data packet */
    UINT8               XMP1DataPacket[4];

} XMP1Data_t;
/**
 * @brief      Data structure containing detailed information regarding remote platform
 * **/
typedef struct{
    /** remote control uapi version */
    UINT8 rcu_uapi_version[UAPI_VERSION_SIZE];
    /** remote control software version */
    UINT8 rcu_sw_version[UAPI_VERSION_SIZE];
    /** remote control product name */
    UINT8 rcu_product_name[UAPI_PRODUCT_NAME_SIZE];
    /** remote control hardware revision */
    UINT8 rcu_hw_info;
    /** remote control IR support */
    UINT8 rcu_ir_info;
    /** remote control RF support */
    UINT8 rcu_rf_info;
    /** remote control CPU speed */
    UINT8 rcu_cpu_info;
}UAPI_RemoteInfo_t;

/** UAPI Error codes **/
typedef enum {
    /** no error */
    UAPI_SUCCESS			= 0x00, 
    /** permissions error */
    UAPI_EPERM              = 0x01,
    /** invalid feature error */
    UAPI_INVALID_FEATURE,
    /** no valid handler error */
    UAPI_ESRCH              = 0x03,
    /** invalid parameter */
	UAPI_INVALID_PARAMETER,
    /** out of memory error */
	UAPI_OUT_OF_MEMORY,
    /** not null error */
	UAPI_NOT_NULL,
    /** too big error */
    UAPI_E2BIG              = 0x07,
    /** no application  error */
    UAPI_ENOEXEC            = 0x08,
    /** invalid opcode error */
	UAPI_INVALID_OPCODE     = 0x09,  
    /** invalid index error */
	UAPI_INVALID_INDEX,
    /** try again error */
    UAPI_EAGAIN             = 0x0B,
    /** error no memory error */
    UAPI_ENOMEM             = 0x0C,
    /** error access permissions error */
    UAPI_EACCES             = 0x0D,
    /** broken error */
    UAPI_EFAULT             = 0x0E,
    /** empty parameter list error */
	UAPI_EMPTY_PARAMETER_LIST,
    /** busy error */
    UAPI_EBUSY              = 0x10,
    /** already exists error */
    UAPI_EEXIST             = 0x11,    
    /** attribute not available error */
	UAPI_ATTRIBUTE_NOT_FOUND,
    UAPI_ENODEV             = 0x13,
	UAPI_INVALID_PARAMETER_VALUE,
    UAPI_EINVAL             = 0x16,
    UAPI_ENOSPC             = 0x1C,
    UAPI_ENAMETOOLONG       = 0x24,
    UAPI_ENOSYS             = 0x26,
	UAPI_ENOMSG             = 0x2A,
    UAPI_EBADRQC            = 0x38,
    UAPI_ENODATA            = 0x3D,
    UAPI_ETIME              = 0x3E,
    UAPI_ECOMM              = 0x46,
    UAPI_EPROTO             = 0x47,
	UAPI_EBADMSG            = 0x4A,
    UAPI_EOVERFLOW          = 0x4B,
    UAPI_EILSEQ             = 0x54,
    UAPI_FAILURE,
    UAPI_UNSUPPORTED_XMP_FEATURE,
    UAPI_XMP1_BAD_CHECKSUM,
    UAPI_UNSUPPORTED_NIF,
    UAPI_FILE_ERROR,
    UAPI_EMSGSIZE           = 0x5A,
    UAPI_ENOPROTOOPT        = 0x5C,
    UAPI_EPROTONOSUPPORT    = 0x5D,
    UAPI_EPFNOSUPPORT       = 0x60,
    UAPI_ENETDOWN           = 0x64,
    UAPI_ENETUNREACH        = 0x65,
    UAPI_ENETRESET          = 0x66,
    UAPI_ECONNRESET         = 0x68,
    UAPI_ENOBUFS            = 0x69,
    UAPI_ETIMEDOUT          = 0x6E,
    UAPI_ECONNREFUSED       = 0x6F,
    UAPI_EHOSTDOWN          = 0x70,
    UAPI_STORAGE_FILE_NOT_FOUND,
    UAPI_STORAGE_BAD_CHECKSUM,
    UAPI_STORAGE_OFFSET_MISMATCH,
    UAPI_STORAGE_FULL,
    UAPI_INVALID_REMOTE


} uapi_status_t;


/**
 * @brief        UAPI error structure 
 *
 * */
typedef struct {
    /** feature error pertains to*/
    UINT8 featureId;
    /** remoteId generating error */
    UINT8 remoteId;
    /** network interface remote communicated over */
    UINT8 devId;    
    /** error code generated */
    uapi_status_t error_code;
    /** datalength of data if there is additional data */
    UINT8 dataLength;
    /** contents of error message */
    UINT8 *data;
} UAPIError_t;

/** 
 *
 * @brief UAPI feature list structure 
 *
 * */
typedef struct {
  /** if set to true, Core UAPI features supported bye remote */
    UINT8 feature_Core;
  /** if set to true, QuickSet feature is supported by remote */
    UINT8 feature_QuickSet;
  /** if set to true, User Activity wakeup is supported by remote*/
    UINT8 feature_UserActivity;
  /** if set to true, URC Key is supported by remote*/
    UINT8 feature_UrcKey;
  /** if set to true, text entry is supported by remote */
    UINT8 feature_Text;
  /** if set to true, orientation is supported by remote */
    UINT8 feature_Orientation;
  /** if set to true, relative motion is supported by remote */
    UINT8 feature_RelativeMotion;
  /** if set to true, absolute position is supported by remote */
    UINT8 feature_AbsolutePosition;
  /** if set to true, battery status is supported by remote */
    UINT8 feature_BatteryStatus;
  /** if set to true, Over the Air Update is supported by remote */
    UINT8 feature_OTA;
  /** if set to true, remote finder is supported by remote */
    UINT8 feature_RemoteFinder;
  /** if set to true, high resolution relative motion is supported by remote */
    UINT8 feature_HighResRelativeMotion;
  /** if set to true, high resolution absolute position is supported by remote*/
    UINT8 feature_HighResAbsolutePosition;
  /** if set to true, raw sensor data is supported by remote*/
    UINT8 feature_RawSensorData;
  /** if set to true, Storage is supported by remote */
    UINT8 feature_Storage;

} UAPIRemoteFeatureList_t;

/**
 * @brief UAPI Core Data report 
 ***/
typedef struct{
    /** ID of remote control generating report*/
    UINT8 remoteId;
    /** network interface generating report */
    UINT8 deviceId;
    /** feature list report */
    UAPIRemoteFeatureList_t features;
} UAPICoreData_t;

/** 
 *
 * @brief remote finder timeout conditions 
 *
 * */
typedef enum UAPI_RemoteFinderCond_s{
    /** remote finder will cease based on timeout*/
    UAPI_REMOTEFINDER_TIMEOUT =1,
    /** remote finder will cease based on keypress*/
    UAPI_REMOTEFINDER_KEYPRESS = 2,
    /** remote finder will cease based on any motion */
    UAPI_REMOTEFINDER_ANY_MOTION = 4,
    /** remote finder will cease based on timeout or keypress */
    UAPI_REMOTEFINDER_TIMEOUT_KEYPRESS = 3,
    /** remote finder will cease based on timeout or any motion */
    UAPI_REMOTEFINDER_TIMEOUT_MOTION = 5,
    /** remote finder will cease based on keypress or any motion*/
    UAPI_REMOTEFINDER_KEYPRESS_MOTION = 6, 
    /** remote finder will cease based on all conditions */
    UAPI_REMOTEFINDER_ALL_COND = 7
}UAPIRemoteFinderCond_t;

/** 
 *
 * @brief       over the air update information 
 *
 * */
typedef struct UAPI_OTAUpdateParams_s{
    /** specify the file type, binary, image */
    UINT16  fileType;
    /** absolute path to hex file upgrade */
    INT8    hexFileName[512];
    /** size of upgrade file */
    UINT32  hexFileSz;
    /** major version number of upgrade image*/
    UINT8   majorVersionNum;
    /** minor version number of upgrade image */
    UINT8   minorVersionNum;
    /** release number of upgrade image */
    UINT16 swReleaseNum;
    /** hardware revision of remote targetted */
    char    hwRev;
    /** CRC of file */
    INT16   CRC;
    /** product name of remote targetted */
    char productName[UAPI_PRODUCT_NAME_SIZE];
}UAPI_OTAUpdateParams_t;

/**
 *  * @brief       defines structure showing button mapping
 *   *            
 *    */
typedef struct {
    /** 6 bits reserved */
    INT32 reserved:6;
    /** 1 bit for button released */
    INT32 button_released:1;
    /** 1 bit for button pressed */
    INT32 button_pressed:1;

}UAPI_RawSensorButtonInfo_t;

/**
 *
 * @brief   Raw Sensor Data Packet
 *
 * */
typedef struct UAPI_RawSensorData_s{
    UINT16 seqNum;
    UAPI_RawSensorButtonInfo_t button;
    UINT16 length;
    UINT8* sensorData;
}UAPI_RawSensorData_t;
#define UAPI_MAX_CAL_NAME 8
typedef struct UAPI_RawSensorCalibration_s{
    
    INT8 CalibrationName[UAPI_MAX_CAL_NAME];
    UINT8* CalibrationData;
}UAPI_RawSensorCalibration_t;


typedef struct{
    /** File Identifier maximum of 254 */
    UINT8 ID;
    /** length of file */
    INT16 length;
}UAPI_Storage_BAT_entry_t;



#ifdef __cplusplus
}
#endif


#endif
