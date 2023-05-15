/**
 * @file        uapi_params.h
 * @brief       This header file contains definitions for the various configuration parameters
 *              for UAPI
 * 
 */


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


#ifndef __UAPI_PARAMS_H__
#define __UAPI_PARAMS_H__
#ifdef __cplusplus
extern "C"{
#endif


/* List of all configurable parameters for each feature */

/**
 * @ brief This enumeration defines the various parameters that are configurable for Relative Motion in a UAPI enabled remote control
 */
typedef enum
{
    /** This field enables or disables Relative Motion. This field is 8 bits */
    RelMotionEnable                               = 0x00,
    /** This field enables or disables Relative Motion Acceleration. This field is 8 bits */
    RelMotionAccelEnable                          = 0x01,
    /** This field enables or disables Relative Motion Deceleration. This field is 8 bits */
    RelMotionDecelEnable                          = 0x02,
    /** This field controls the number of physical steps required to map to logical steps. This field is 16 bits */
    RelMotionPhysicalToLogical                    = 0x03, /* 2 byte data */
    /** This field controls the Relative Motion Traverse Time. This field is 16 bits */
    RelMotionTraverseTime                         = 0x04,
    /** This field controls the Relative Motion Deceleration Sample Time. This field is 16 bits */
    RelMotionDecelSampleTime                      = 0x05, /* 2 byte data */
    /** This field controls the Relative Motion Transmit Interval (ms). This field is 16 bits */
    RelMotionTxIntervalMs                         = 0x06, /* 2 byte data */
    /** This field controls the Relative Motion Deceleration Coeffieicent. This field is 16 bits */
    RelMotionDecelCoefficient                     = 0x07, /* 2 byte data */
    /** This field enables or disables Relative Motion Finger Presence Notification. This field is 8 bits */
    RelMotionFingerPresenceNotifyEnable           = 0x08,  /*1 byte data */
    /** This field specifies the finger duration that will be interpreted as a tap (ms) This field is 16 bits*/
    RelMotionTapDuration                          = 0x09,
    /** This field enables or disables relative orientation. This field is 8 bits */
    RelMotionRelativeOrientation                  = 0x0A,
    /** This field enables or disables sending of click as left mouse. This field is 8 bits*/
    RelMotionSendMouseClick                       = 0x0B,
    /** This field enables or disables sending of click as URC select/ok key. This field is 8 bits */
    RelMotionSendSelectKey                        = 0x0C,
    /** This field is the acceleration scale factor. This field is 16 bits */
    RelMotionAccelCoefficient                     = 0x0D,
    /** This field enables or disables the sending of a click as an enter key TextEntry. This field is 8 bits */
    RelMotionSendEnterKey                         = 0x0E,
    /** This field controls the physical to logical conversion for first touch. This field is 16 bits */
    RelMotionFirstPhysicalToLogical 		  = 0x0F,
    /** This field controls the physical to logical conversion for deceleration. This field is 16 bits */
    RelMotionDecelPhysicalToLogical		  = 0x10

} RelativeMotionConfigParameter_t;

/**
 * @ brief This enumeration defines the various parameters that are configurable for Absolute Motion in a UAPI enabled remote control
 */
typedef enum
{
    /** This field enables or disasbles Absolute Position. This field is 8 bits. It is RW */
    AbsMotionEnable	                              = 0, /*1 byte data*/
    /** This field controls the transmit interval (in ms) of absolute position data. This field is 16 bits. It is RW */
    AbsMotionTxIntervalMs                         = 1, /*2 byte data*/
    /** This field controls the Orientation X Inversion state. This field is 16 bits. It is RW */
	AbsMotionOrientationXInvState				  = 6  /*2 byte data*/

} AbsolutePositionConfigParameter_t;

/**
 * @ brief This enumeration defines the various parameters that are configurable for Remote API in a UAPI enabled remote control
 */
typedef enum
{
    /** This field enables or disables Remote API functionality. It is an 8 bit field. It is RW */
    RemoteAPIEnable				                  = 0 /*1 byte data*/

} RemoteAPIConfigParameter_t;

/**
 * @ brief This enumeration defines the various parameters that are configurable for orientation on a UAPI enabled remote control
 */
typedef enum
{
    /** This field enables or disables orientation reporting. It is an 8 bit field. It is RW */
    OrientationEnable			                  = 0 /*1 byte data*/

} OrientationConfigParameter_t;

/**
 * @ brief This enumeration defines the various parameters that are configurable for URC key presses on a UAPI enabled remote control
 */
typedef enum
{
    /** This field enables or disables User Activity reporting. It is an 8 bit field. It is RW */
    UserActivityEnable			                  = 0 /*1 byte data*/

} UserActivityConfigParameter_t;

/**
 * @ brief This enumeration defines the various parameters that are configurable for URC key presses on a UAPI enabled remote control
 */
typedef enum
{
    /** This field enables or disables URC key reporting. It is an 8 bit field. It is RW */
    URCKeyEnable				                  = 0 /*1 byte data*/

} URCKeyConfigParameter_t;

/**
 * @ brief This enumeration defines the various parameters that are configurable for battery status on a UAPI enabled remote control
 */
typedef enum
{
    /** This field enables or disables battery status notification. It is an 8 bit field. It is RW */
    BatteryStatusEnable		                 = 0, /*1 byte data*/
    BatteryReportingMask                         = 1,/*1 byte data*/
    BatteryCriticalThreshold                     = 2, /*2 byte data*/
    BatteryWarningThreshold                      = 3,/*2 byte data*/
    BatteryFullThreshold                         = 4 /*2 byte data*/

} BatteryStatusConfigParameter_t;

/**
 * @ brief This enumeration defines the various parameters that are configurable for text entry on a UAPI enabled remote control
 */
typedef enum
{
    /** This field enables or disables Text Entry on the remote control keyboard, if equipped. It is an 8 bit field. it is RW */
	TextEntryEnable				                  = 0, /*1 byte data    */
    /** This field enables or disables Symbol Entry on the remote control keyboard, if equipped. It is an 8 bit field. It is RW */
    SymShiftModeEnable                            = 1, /*1 byte data*/
    /** This field enables or disables Caps Lock Mode on the remote control keyboard, if equipped. It is an 8 bit field. It is RW */
    CapsModeEnable                                = 2, /*1 byte data*/
    /** This field enables or disables Alternate Mode on the remote contorl keyboard, if equipped. It is an 8 bit field. It is RW */
	AltModeEnable								  = 3  /*1 byte data*/

} TextEntryConfigParameter_t;

/**
 * @brief This enumeration defines the various parameters that are configurable for the remote finder feature on a UAPI enabled remote control
 * */
typedef enum {
    /** This field enables or disables Remote finder, if equipped. It is an 8 bit field, it is RW */
    RemoteFinderEnable                            = 0,
    /** This field sets the generic timeout value in units of 62.5ms. It is a 16 bit field, it is RW */
    GenericTimeout                                = 1,
    /** This field sets the Finder timeout value in units of 62.5ms. It is a 16 bit field, it is RW */
    FinderTimeout                                 = 2,
    /** This field sets the LED timeout value in units of 62.5ms. It is a 16 bit field, it is RW */
    LEDTimeout                                    = 3,
    /** This field sets the Tone timeout value in units of 62.5ms. IT is a 16 bit field, it is RW */
    ToneTimeout                                   = 4

} RemoteFinderConfigParameter_t;

/** 
 * @brief These parameters contain information regarding UAPI 
 **/
typedef enum {
    /** This field indicates if UAPI is enabled in the remote control. It is an 8 bit field. it is READ ONLY */
    UapiEnbState								  = 0, /*1 byte data READ ONLY*/
    /** This field contains the UAPI version information. It is a 32 bit field. It is READ ONLY */
    UapiVersionInfo								  = 1, /*4 byte data READ ONLY*/
    /** This field contains a string of the name of the product. It is a 16 byte null terminated (max) string. it is READ ONLY*/
    UapiProductInfo								  = 2, /*4-16 byte data READ ONLY*/
    /** This field controls the polling rate of the remote control. It is a 32 bit value. This field is RW */
    UapiPollingRate								  = 3, /*4 byte data*/
    /** This field contains HW information. It is a 32 bit field. It is READ ONLY */
    UapiCPUHwInfo								  = 4, /*4 byte data READ ONLY*/
    /** This field enables or disables auto poll in the remote control. It is an 8 bit field. It is RW */
    UapiAutoPollState							  = 5 /*1 byte data*/

} UAPIAttributeID_t;


/*! \def MAX_UAPI_PARAM_DATA
 *   The maximum uapi parameter data length is defined as 16
 */
#define MAX_UAPI_PARAM_DATA 16

/*! \def MAX_PRODUCT_NAME
 *  *   The maximum product name length is defined as 10
 *   */
#define MAX_PRODUCT_NAME 10

/**
 * @ brief This structure contains both the remote control firmware version and the product name of the remote control */
typedef struct UAPIProductInfo_s{
	/** software version of remote firmware */
    unsigned int swVersion;
    /** product name of remote control */
	char productName[MAX_PRODUCT_NAME];/*10 byte name of remote*/
}UAPI_productInfo_t;

/**
 * @ brief This structure is used to store information for a single parameter
 */
typedef struct configParam_s
{
	/** action to be taken for each parameter. set/get/reset */
    unsigned char action;
    /** attribute to be modified */
    unsigned char attributeId;
    union
	{
        /** product information */
		UAPI_productInfo_t productInfo;
        /** 1 byte data storage for parameters with 8 bit data */
        unsigned char   UINT8_data;  /*1 byte data*/
        /** 2 byte data storage for parameters with 16 bit data */
        unsigned short  UINT16_data; /*2 byte data*/
        /** 4 byte data storage for parameters with 32 bit data */
        unsigned int    UINT32_data; /*4 byte data*/
        /** 16 byte data storage for bytestream data */
		unsigned char   bytestring_data[MAX_UAPI_PARAM_DATA]; /*byte string max 16 bytes*/
        /** 16 byte data storage for string data */
        char            charstring_data[MAX_UAPI_PARAM_DATA]; /*character string max 16 bytes*/
    }value;
}UAPI_configParam_t;

/**
 * @ brief This structure is used to store parameters for Raw Sensor Data 
 * */
typedef enum {
    RawSensorDataEnable                           = 0,
    RawSensorDataSuspend                          = 1,
    RawSensorDataNumSensors                       = 2
} RawSensorDataParameter_t;

/**
 * @ brief This enum is used to define storage parameters 
 **/
typedef enum{
    /* byte defining if storage feature is enabled */
    StorageEnable                                 = 0,
    /* byte defining if storage area requires erase prior to writing */
    StorageEraseRequired                          = 1,
    /* 2 bytes defining page size in bytes of storage area */
    StoragePageSize                               = 2,
    /* 2 bytes defining number of pages in storage area */
    StorageLength                                 = 3
} UAPI_Storage_Params_t;
/**
 * @ brief This structure is used to store a parameter list for a specific feature specified by featureId. It should contain numParams parameters
 */
typedef struct configParamStruct_s{
    /** feature this configuration structure is intended for */
    unsigned char featureId;
    /** the number of parameters in the structure */
    unsigned char numParams;
    /** pointer to UAPI configuration parameter structure */
    UAPI_configParam_t* param;
}UAPI_configParamStruct_t;

/**
 * @brief This structure is used to store a parameter list for a given remote Id, connected with devId network interface
 * */
typedef struct remoteParam_s{
    /** The network interface these parameters are received from */
    unsigned char devId;
    /** The remote id these parameters are received from */
    unsigned char remoteId;
    /** The parameter list received from the device and remote id specified above */
    UAPI_configParamStruct_t* remoteParams;
}UAPI_remoteParams_t;

#ifdef __cplusplus
}
#endif


#endif
