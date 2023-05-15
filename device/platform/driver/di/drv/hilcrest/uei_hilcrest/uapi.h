/**
 * @file        uapi.h
 * @brief       The main API header file for the UAPI library.
 * 
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
******************************************************************************/

#ifndef __UAPI_H__
#define __UAPI_H__

#ifdef __cplusplus
extern "C"{
#endif


#include "uapi_params.h"
#include "uapi_defs.h"
#include "uapi_types.h"

/*! \def UAPI_GENERALSERVICES_NAME                                                            
 *   The UAPI General Services Notification is GeneralServices
 */
#define UAPI_GENERALSERVICES_NAME "GeneralServices"
/*! \def UAPI_REMOTEAPI_NAME                                                            
 *   The UAPI Remote API Notification is RemoteAPI
 */
#define UAPI_REMOTEAPI_NAME "RemoteAPI"
/*! \def UAPI_USERACTIVITY_NAME
 *   The UAPI User Activity Notification is UserActivity
 */
#define UAPI_USERACTIVITY_NAME "UserActivity"
/*! \def UAPI_URCKEY_NAME                                                        
 *   The UAPI URC KEY Notification is UrcKey
 */
#define UAPI_URCKEY_NAME "UrcKey"
/*! \def UAPI_TEXTENTRY_NAME                                                  
 *   The UAPI Text Entry Notificadtion is TextEntry
 */
#define UAPI_TEXTENTRY_NAME "TextEntry"
/*! \def UAPI_ORIENTATION_NAME                                           
 *   The UAPI Orientation Notification is Orientation
 */
#define UAPI_ORIENTATION_NAME "Orientation"

/*! \def XMP_RELATIVEMOTION_SLIDER_NAME
 *  *   The XMP Relative Motion Slider Notification is XMPRelativeMotionSlider
 *   */
#define XMP_RELATIVEMOTION_Slider_NAME "XMPRelativeMotionSlider"

/*! \def UAPI_RELATIVEMOTION_NAME
 *   The UAPI Relative Motion Notification is RelativeMotion
 */
#define UAPI_RELATIVEMOTION_NAME "RelativeMotion"

/*! \def UAPI_HIGHRES_RELATIVEMOTION_NAME
 *  *   The UAPI High Resolution Relative Motion Notification is HighResRelativeMotion
 *   */
#define UAPI_HIGHRES_RELATIVEMOTION_NAME "HighResRelativeMotion"

/*! \def UAPI_ABSOLUTEPOSITION_NAME                                  
 *   The UAPI Absolute Position Notification is AbsolutePosition
 */
#define UAPI_ABSOLUTEPOSITION_NAME "AbsolutePosition"

/*! \def UAPI_HIGHRES_ABSOLUTEPOSITION_NAME                                  
 *  *   The UAPI High Resolution Absolute Position Notification is HighResAbsolutePosition
 *   */
#define UAPI_HIGHRES_ABSOLUTEPOSITION_NAME "HighResAbsolutePosition"

/*! \def UAPI_RAWSENSORDATA_NAME
 *  *  *   The UAPI Raw Sensor Data Notification is RawSensorData
 *   *   */
#define UAPI_RAWSENSORDATA_NAME "RawSensorData"


/*! \def UAPI_BATTERYSTATUS_NAME                                          
 *   The UAPI Battery Status Notification is BatteryStatus
 */
#define UAPI_BATTERYSTATUS_NAME "BatteryStatus"
/*! \def UAPI_CONFIGURATION_NAME                                     
 *   Ths UAPI Configuration Notification is Configuration
 */
#define UAPI_CONFIGURATION_NAME "Configuration"
/*! \def UAPI_ERROR_NAME                                     
 *   The UAPI Errors Notification is Errors
 */
#define UAPI_ERROR_NAME "Errors"
/*! \def UAPIFeature_Core                  
 *   The UAPI Core ID is 0x00
 */
#define UAPIFeature_Core 		 	 0x00
/*! \def UAPIFeature_RemoteAPI                                    
 *   The UAPI Feature Remote API ID is 0x11
 */
#define UAPIFeature_RemoteAPI        0x11
/*! \def UAPIFeature_UserActivity                               
 *   The UAPI Feature UserActivity ID is 0x20
 */
#define UAPIFeature_UserActivity 	 0x20
/*! \def UAPIFeature_UrcKey                            
 *   The UAPI Feature UrcKey ID is 0x21
 */
#define UAPIFeature_UrcKey		     0x21
/*! \def UAPIFeature_Text                            
 *   The UAPI Feature Text ID is 0x22
 */
#define UAPIFeature_Text		     0x22
/*! \def UAPIFeature_Orientation                                    
 *   The UAPI Feature Orientation ID is 0x31
 */
#define UAPIFeature_Orientation	     0x31
/*! \def UAPIFeature_RelativeMotion                             
 *   The UAPI Feature Relative Motion ID is 0x32
 */
#define UAPIFeature_RelativeMotion   0x32
/*! \def UAPIFeature_AbsolutePosition                         
 *   The UAPI Feature Absolute Position ID is 0x34
 */
#define UAPIFeature_AbsolutePosition 0x34

/*! \def UAPIFeature_Storage
 *   The UAPI Feature Storage ID is 0x37
 */
#define UAPIFeature_Storage          0x37

/*! \def UAPIFeature_BatteryStatus                        
 *   The UAPI Feature Battery Status ID is 0x43
 */
#define UAPIFeature_BatteryStatus    0x43

/*! \def MAX_UAPI_REMOTES
 *  *   The maximum number of supported remotes is 12
 *   */
#define MAX_UAPI_REMOTES             12 
/**
 * @brief        sends data via RF4CE to remote control
 *
 * @param        uapiDataPkt the packet to be translated to the underlying communication struct (if needed)
 *               and sent to the remote control
 *
 * @return       the return value provided by the underlying communication medium
 **/
INT16 UAPI_RF4CESend(UAPIDataPkt* uapiDataPkt);

/**
 * @brief        initializes UAPI library 
 * 
 * @return       uapi_success if UAPI library is initialized successfully
 *               
 **/
VOID UAPI_Init(VOID);

/**
 * @brief        deinitializes the UAPI library
 *
 * @return       uapi_success if UAPI library is de-initialized successfully
 **/
VOID UAPI_Stop(VOID);

/**
 * @brief        entry point for raw data into UAPI
 *
 * @param        msgData the wrapped UAPI data packet to be processed by the UAPI library
 *
 * @return       none
 **/
VOID UAPIDeMultiplexer(UAPIDataPkt *msgData);

/**
 * @brief        entry point for XMP1 data into UAPI
 *
 * @param        raw xmp1 data to be handled by UAPI library
 *
 * @return       uapi_status_t
 **/
uapi_status_t  ProcessXMP1Data(XMP1Data_t* xmp1Data);

/**
 * @brief       registration call to associate registry packets with XMP1 data
 *
 * @param       registry number of note, UAPI feature name associated with that particular feature
 *
 * @return      uapi_status_t
 **/
uapi_status_t  RegisterXMP1Registry(unsigned int registry, const char* featureName);

/**
 * @brief        creates a configuration parameter structure list
 *
 * @param        thing pointer to the data structure that needs to be created
 * @param        numParams the number of parameters to be stored in the parameter list
 * @param        featureId the UAPIFeature that should be configured
 *
 * @return       a valid pointer to the configuration parameter structure, NULL if
 *               the structure was unable to be created.
 *
 **/
UAPI_configParamStruct_t* UAPI_createParamStruct(UAPI_configParamStruct_t* thing, unsigned char numParams, unsigned char featureId);

/**
 * @brief        destroys a configuration parameter structure
 *
 * @param        params pointer to the configuration parameter structure to be destroyed
 *
 * @return       none
 **/
VOID UAPI_destroyParamStruct(UAPI_configParamStruct_t* params);

/**
 * @brief        sets a value in a configuration parameter structure list
 *
 * @param        paramStruct pointer to configuration parameter structure to populate
 * @param        index the index of the parameter in the structure
 * @param        attribute the ID of the attribute to be configured
 * @param        value pointer to the data to be stored in the configuration parameter structure
 * @param        size the sizeof of the value datatype
 *
 * @return       uapi_success if successful, error code detailing failure if not
 **/
uapi_status_t UAPI_setParam(UAPI_configParamStruct_t* paramStruct, unsigned char index, unsigned char attribute, void* value, int size);

/**
 * @brief        gets a value in a configuration parameter structure list
 *
 * @param        paramStruct point to configuration parameter structure list to retrieve value from
 * @param        attribute the ID of the attribute to retrieve
 * @param        value pointer to data for storage from the configuration parameter structure list
 *
 * @return       uapi_success if value is valid
 **/
uapi_status_t UAPI_getParam(UAPI_configParamStruct_t* paramStruct, unsigned char attribute, void* value);

/**
 * @brief        registers a callback to be notified when the appropriate data is received from the remote
 *
 * @param        featureName see defines for feature names
 * @param        FuncPtr function pointer to be called when notification arrives
 *
 * @return       uapi_success if registration succeeds
 **/
uapi_status_t UAPIRegisterCallback( const char* featureName, VOID (*FuncPtr)(VOID*) );

/**
 * @brief        sends configuration parameters to remote specified by remoteId and network interface specified by devId
 *
 * @param        params parameter list created by UAPI_createParamStruct
 * @param        devId the network interface to communicate with
 * @param        remoteId the remote to send the parameters to
 *
 * @return       uapi_success if the request is successfully queued up
 **/
uapi_status_t UAPIConfigureFeature(void* params,    UINT8 devId, UINT8 remoteId);

/**
 * @brief        get info of the specified feature from the remote connected via the specified devId
 * @param        featureId see defines of UAPIFeature_ IDs for featureIds
 * @param        devId the network interface the requested remote is attached with
 * @param        remoteId the ID of the desired remote 
 *
 * @return       returns uapi_success if request is successfully queued up
 **/
uapi_status_t UAPIGetInfo         (UINT8 featureId, UINT8 devId, UINT8 remoteId);

/**
 * @brief        retrieves the version of the UAPI library
 *
 * @param        libraryVersion pointer to a unsigned 32bit variable to store the library version
 * 
 * @return       uapi_success if the version request was successful, libraryVersion will be invalid if not.
 **/
uapi_status_t UAPIGetVersion	  (UINT32* libraryVersion);

/**
 * @brief        retrieves the amount of free space currently available for storage
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 * @param        FreeSpace pointer to a signed 32 bit integer to store the amount of free space
 *
 * @return       uapi_success if free space request was successful
 *
 */
uapi_status_t UAPI_Storage_GetFreeSpace( UINT8 devId, UINT8 remoteId, INT32* FreeSpace);

/**
 * @brief        retrieves the total size of the storage area in the remote control
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 * @param        TotalSpace pointer to a signed 32 bit integer to store the amount of totalspace
 *
 * @return       uapi_success if space request is successful
 */
uapi_status_t UAPI_Storage_GetTotalSize(UINT8 devId, UINT8 remoteId, INT32* TotalSpace);

/**
 * @brief        retrieves data from storage area based on File ID
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 * @param        ID requested ID of file to retrieve
 * @param        *data pointer to buffer to store retrieved data
 * @param        *length pointer to length variable to store size of retrieve data.
 *
 * @return       will return UAPI_SUCCESS when retrieval completes or times out. This is a blocking function
 *
 */
uapi_status_t UAPI_Storage_ReadData   (INT8 devId, INT8 remoteId, UINT8 ID, UINT8* data, INT16* length);

/**
 * @brief        retrieves data from storage area based on File ID
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 * @param        *entries pointer to entries list, will be created by this function and returned
 * @param        *num_entries the number of entries in the returned list
 *
 * @return       will return a valid pointer to the list of entries, it needs to be freed by the caller. Will be NULL if a failure occured.
 *               num_entries will only be valid if the return of this function is non NULL. 
 **/
UAPI_Storage_BAT_entry_t* UAPI_Storage_Get_BAT_List( UINT8 devId, UINT8 remoteId, UAPI_Storage_BAT_entry_t* entries, UINT8* num_entries);
/**
 * @brief        delete data from storage area based on File ID
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 * @param        ID requested ID of file to delete 
 *
 * @return       will return UAPI_SUCCESS when retrieval completes or times out. This is a blocking function
 * */
uapi_status_t UAPI_Storage_Delete_BAT_Entry( UINT8 devId, UINT8 remoteId, UINT8 ID);

/**
 * @brief        defragments the storage are of any "deleted" data and compacts storage area
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 *
 * @return       will return UAPI_SUCCESS when retrieval completes or times out. This is a blocking function
 **/
uapi_status_t UAPI_Storage_Defrag( UINT8 devId, UINT8 remoteId );

/**
 * @brief        writes data requested by user to remote control
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 * @param        *ID pointer to store ID after write is successful
 * @param        *data pointer to buffer with data to write
 * @param        length denoting the amount of data to be written
 *
 * @return       will return UAPI_SUCCESS when write completes or times out, will return UAPI_STORAGE_FULL if storage area becomes full
**/
uapi_status_t UAPI_Storage_WriteData  (UINT8 devId, UINT8 remoteId, UINT8 *ID, UINT8* data, INT16 length);


/**
 * @brief        queues request to get remote control feature list. When remote responds, user will be notified through callback
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 *
 * @return       UAPI_SUCCESS if request is successfully queued
 */
uapi_status_t UAPIGetRemoteFeatures(UINT8 devId, UINT8 remoteId);

/**
 * @brief        queues request to get remote control battery status.
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteId the simple ID of the desired remote
 *
 * @return       UAPI_SUCCESS if request is successfully queued
*/

uapi_status_t  UAPIBatteryStatusRequest(UINT8 devId, UINT8 remoteId );

/**
 * @brief        queues request to send QuickSet related data to remote control
 *
 * @param        *message buffer with QuickSet data to be sent to remote control
 * @param        size length of QuickSet data to be sent
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteID the simple ID of the desired remote
 *
 * @return       UAPI_SUCCESS if request is successfully queued
 */
uapi_status_t UAPIQSSendToRemote      (UINT8* message, INT16 size, UINT8 devId, UINT8 remoteID);

/**
 * @brief        announce remote has been paired. will clear remote settings for remoteId
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteID the simple ID of the desired remote
 *
 * @return       UAPI_SUCCESS
 *          */
uapi_status_t UAPI_AnnouncePairedRemote(UINT8 devId, UINT8 remoteId);
/**
 * @brief        announce remote has been unpaired. will clear remote settings for remoteId
 *
 * @param        devId the network interface the requested remote is connected with
 * @param        remoteID the simple ID of the desired remote
 *
 * @return       UAPI_SUCCESS 
 */
uapi_status_t UAPI_AnnounceUnpairedRemote(UINT8 devId, UINT8 remoteId);
uapi_status_t UAPIRawSensorCalibrationRequest( UINT8 devId, UINT8 remoteID );

#ifdef __cplusplus
}
#endif

#endif /* __UAPI_H__ */
