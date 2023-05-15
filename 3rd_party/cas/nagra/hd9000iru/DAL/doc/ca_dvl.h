/**
** @file ca_dvl.h
**
** @brief
**   DVR/VOD Library external definitions.
**
**   The syntax of the comments of this file is compliant to the format used by
**   Doxygen to generate html documentation from source and header files.
**   Doxygen can be downloaded from www.doxygen.org and is provided under GNU
**   General Public License.
**
** $Revision:  $
**
** Copyright:
**   2006-2010 Nagra France S.A.
**
**
*/

#ifndef CA_DVL_H
#define CA_DVL_H

/******************************************************************************/
/*                                                                            */
/*                               INCLUDE FILES                                */
/*                                                                            */
/******************************************************************************/

#include "ca_defs.h"
#include "ca_defsx.h"

/******************************************************************************/
/*                                                                            */
/*                              GROUPS DEFINITION                             */
/*                                                                            */
/******************************************************************************/

/**
 *  @defgroup groupInitTerm Initialization and termination 
*/

/**
 *  @defgroup groupSystemInfo System and Chipset Information
*/

/**
 *  @defgroup groupHomeNetwork Home Network Information
*/

/**
 *  @defgroup groupRecord Content Recording or Migrating
*/

/**
 *  @defgroup groupPlayback Content Playback
*/

/**
 *  @defgroup groupCredInfo Credentials Information
*/

/**
 *  @defgroup groupPMM PRM Management Messages
*/

/**
 *  @defgroup groupMetadata LCM Metadata Update
*/

/**
 *  @defgroup groupMemory Memory Management
*/

/**
 *  @defgroup groupODM Opaque Data Management
*/

/******************************************************************************/
/*                                                                            */
/*                                     TYPES                                  */
/*                                                                            */
/******************************************************************************/
/**
 *  @brief
 *    Definition of the DVR/VOD Library Handle that uniquely identifies a 
 *    recording or playback session.
 *
 *  @remark
 *    -# The type TDvlHandle is opaque to client applications to prevent 
 *    external access. A pointer on a TDvlHandle is returned when starting a 
 *    record or a playback session and shall be returned when stopping the 
 *    session later. This pointer on a TDvlHandle is also given by the DVR/VOD 
 *    library in all the error call-backs. The type TdvlHandle should always 
 *    be used as a pointer by a client application.
*/
typedef struct SDvlHandle TDvlHandle;

/**
 *  @brief
 *    Definition of the Handle that uniquely identifies an 
 *    Opaque Data Object.
 *
 *  @remark
 *    -# The type TOdmHandle is opaque to client applications to prevent 
 *    external access. A pointer on a TOdmHandle is returned when calling 
 *    dvlOpaqueDataMessageCreate and shall be returned when releasing the 
 *    Opaque Data Message later. 
 *    The type TOdmHandle should always be used as a pointer by a client
 *    application.
*/
typedef struct SOdmHandle TOdmHandle;


/**
 *  @brief
 *    This enumeration defines the error statuses returned by
 *    the DVR/VOD Library functions.
*/
typedef enum
{
  DVL_NO_ERROR = 0,
    /**<
     *    The function terminated successfully.
    */
  DVL_ERROR,
    /**<
     *    An unspecified error happened during the function execution.
    */
  DVL_ERROR_BAD_PARAMETER,
    /**<
     *    One or more function arguments are missing. The request could not be 
     *    successfully processed.
    */
  DVL_ERROR_DVL_NOT_RUNNING,
    /**<  The Nagra DVR/VOD Library is currently not running and the requested
     *    operation cannot be executed.
    */
  LAST_DVL_STATUS
    /**<
     *    Last DVL status.
     *    This status is never returned by the DVR/VOD Library.
    */
} TDvlStatus;


/**
 *  @ingroup groupRecord
 *  @brief
 *    This enumeration defines the possible status returned by a recording 
 *    request.<br>
 *    For easy usage, each DENIED value is greater than DVL_RECORD_DENIED. 
 *    The client application is encouraged to use this comparison to be
 *    compatible with the future recording status Nagravision could add.
*/
typedef enum
{
  DVL_RECORD_GRANTED = 0,
    /**<
     *    The recording request has successfully terminated.
     *    The content key is now set in the chipset stream cipher and the STB 
     *    application can start storing the content on the HDD.
    */
  DVL_RECORD_DENIED = 100,
    /**<
     *    The usage rules associated to this content deny recording it.
     *    No key has been set into the chipset stream cipher and the STB
     *    application shall not store the content on the HDD.
     */
  DVL_RECORD_DENIED_APPENDING_DENIED,
    /**<  The DVR/VOD Library could not append the current recording with the
     *    content related to the input credentials.
     *    The DVR/VOD Library did NOT set any key into the chipset and the STB
     *    application shall not store the content on the HDD.
    */
  DVL_RECORD_DENIED_BAD_PROFILE,
    /**<
     *    Deprecated.
     *    The input Profile of the request is invalid. No key has been set into
     *    the chipset stream cipher and the STB application shall not store the
     *    content on the HDD.
     */
  DVL_RECORD_DENIED_BAD_INPUT_CREDENTIALS,
    /**<
     *    The input Credentials of the request provided for appending purpose
     *    is invalid. No key has been set into the chipset stream cipher and 
     *    the STB application shall not store the content on the HDD.
     */
  DVL_RECORD_DENIED_WRONG_TSID,
    /**<
     *    The transport session identifier is unknown by the SCR at that time. 
     *    No key has been set into the chipset stream cipher and the STB 
     *    application shall not store the content on the HDD.
     */
  DVL_RECORD_DENIED_LIMIT_EXCEEDED,
    /**<
     *    Deprecated.
     *    This status is not returned by the DVL.
     */
  DVL_RECORD_DENIED_INVALID_TIME,
    /**<
     *    The internal time reference is not valid. The DVR/VOD Library is not 
     *    authorized to record any content. No key has been set into the 
     *    chipset stream cipher and the STB application shall not store the 
     *    content on the HDD.
     */
  DVL_RECORD_DENIED_INVALID_EMI,
    /**<
     *    The EMI planned to encrypt the content is not supported by the SCR. 
     *    No key has been set into the chipset stream cipher and the STB 
     *    application shall not store the content on the HDD.
     */
   LAST_DVL_RECORD_STATUS
    /**<
     *    Last DVR/VOD Library Record status value. This value is never 
     *    returned by the DVR/VOD Library.
     */
} TDvlRecordStatus;

/**
 *  @ingroup groupCredInfo groupPlayback
 *  @brief
 *    Possible access statuses associated to a provided content.<br>
 *    For easy usage, each DENIED value is greater than DVL_ACCESS_DENIED. 
 *    The client application is encouraged to use this comparison to be
 *    compatible with the future access status Nagravision could add.
*/
typedef enum
{
  DVL_ACCESS_GRANTED = 0,
    /**<
     *    The playback of the content is granted.
    */
  DVL_ACCESS_DENIED = 100,
    /**<
     *    The playback of the content is denied.
     */
  DVL_ACCESS_DENIED_CONTENT_EXPIRED,
    /**<
     *    The content's usage rules deny its playback because it is expired.
     */
  DVL_ACCESS_DENIED_INVALID_CREDENTIALS,
    /**<
     *    The provided content Credentials are invalid.
     */
  DVL_ACCESS_DENIED_WRONG_TSID,
    /**<
     *    The transport session identifier is unknown by the SCR at that time. 
     *    This status only applies to a playback request.
     */
  DVL_ACCESS_DENIED_LIMIT_EXCEEDED,
    /**<
     *    Deprecated.
     *    This status is not returned by the DVL.
     */
  DVL_ACCESS_DENIED_INVALID_TIME,
    /**<
     *    The internal time reference is not valid. The DVR/VOD Library is not 
     *    authorized to play back any content.
     */
  DVL_ACCESS_DENIED_UNKNOWN_STB,
    /**<
     *    The access to the content is denied because it has been 
     *    recorded by an unknown set-top box.
     */
  DVL_ACCESS_DENIED_UNKNOWN_HOME_NETWORK,
    /**<
     *    The access to the content is denied because it has been
     *    recorded by a set-top box belonging to an unknow Home Network.
     */
  DVL_ACCESS_DENIED_UNKNOWN_NUID,
    /**<
     *    The access to the content is denied because it has been 
     *    recorded by a set-top box with an unknown NUID.
     */
  DVL_ACCESS_DENIED_INVALID_EMI,
    /**<
     *    The EMI used to protect this content is not supported by the SCR 
     *    implementation.
     */
  DVL_ACCESS_DENIED_INVALID_KEY_PROTECTION,
    /**<
     *    The keys needed to decipher the content have been protected using a 
     *    mechanism not supported by the SCR implementation.
     */
  LAST_DVL_ACCESS_STATUS
    /**<
     *    Last DVL Playback status value.
     *    This value is never returned by the DVR/VOD Library.
     */
} TDvlAccessStatus;

/**
 *  @ingroup groupSystemInfo
 *  @brief
 *    defines the length of the string version of the DVR/VOD Library.
 *
 *  @see  TDvlVersionString
*/
#define DVL_VERSION_STRING_LENGTH  20
/**
 *  @ingroup groupSystemInfo
 *  @brief
 *    This type is used for NULL-terminated printable string containing the 
 *    version of the DVR/VOD Library.
*/
typedef TChar TDvlVersionString[DVL_VERSION_STRING_LENGTH];


/**
 *  @ingroup groupSystemInfo
 *  @brief
 *    defines the length of the unique ID string.
 *
 *  @see  TDvlUniqueIdString
*/
#define DVL_UNIQUE_ID_STRING_LENGTH  16
/**
 *  @brief
 *    This type is used for NULL-terminated printable strings containing a Nagra
 *    Unique identifier, such as the chipset unique ID (the NUID) or the IRD S/N.
 *    It respects the following format: "xx xxxx xxxx xx". 
*/
typedef TChar TDvlUniqueIdString[DVL_UNIQUE_ID_STRING_LENGTH];


/**
 *  @ingroup groupSystemInfo
 *  @brief
 *    defines the length of the chipset type string.
 *
 *  @see  TDvlChipsetTypeString
*/
#define DVL_CHIPSET_TYPE_STRING  40
/**
 *  @ingroup groupSystemInfo
 *  @brief
 *    This type is used for NULL-terminated printable strings containing a 
 *    chipset type. It respects the following format:
 *    "xxxx xxxx xxxx xxxx xx".
*/
typedef TChar TDvlChipsetTypeString[DVL_CHIPSET_TYPE_STRING];

/**
 *  @ingroup groupHomeNetwork
 *  @brief
 *    This enumeration gives information about the type of secret.
*/
typedef enum
{
  DVL_SECRET_TYPE_SEC = 0,
    /**<
     *    The secrets are chipset based.
   */
  DVL_SECRET_TYPE_OTP,
    /**<
     *    The secrets are OTP based.
   */
  LAST_DVL_SECRET_TYPE
    /**<
     *    Last DVL key type.
     *    This key type is never returned by the DVR/VOD Library.
   */
} TDvlSecretsType;

/**
 *  @ingroup groupHomeNetwork
 *  @brief
 *    This structure contains information about the secrets that
 *    the DVR/VOD Library possesses and that allows it to
 *    playback content recorded by a neighbourhood Set-Top Box.
*/
typedef struct
{
  TUnsignedInt32  irdSerialNumber;
    /**<
     *    This field contains the Nagra unique Identifier of the Set-Top Box
     *    of which the DVR/VOD Library possesses some secrets.
   */
  TDvlSecretsType secretsType;
    /**<
     *    This field contains the type of the secrets.
   */
} TDvlNeighbourhoodInfo;

/**
 *  @ingroup groupCredInfo
 *  @brief
 *    The output format of the Credentials information request depends on the 
 *    type of Credentials. Current known Credentials types are:
 *    - VOD EMM: Credentials generated by the Head End and containing the key 
 *      protecting the consumption rules associated to a VOD content. VOD EMM 
 *      can be processed only by the decoder to which they are intended.
 *    - VOD ECM: Credentials generated by the Head End and containing the key 
 *      protecting the VOD content.
 *    - LCM: Credentials generated by the DVR/VOD Library at recording time. 
 *      It contains the key protecting the recorded content and its 
 *      consumption rules. LCM can be used for playback only by the decoder 
 *      that recorded the content.
*/
typedef enum
{
  CREDENTIALS_TYPE_PROFILE = 0,
    /**<  Deprecated.
     *    The Credentials correspond to a Profile.
    */
  CREDENTIALS_TYPE_VOD_EMM,
    /**<  The Credentials correspond to a VOD EMM.
    */
  CREDENTIALS_TYPE_VOD_ECM,
    /**<  The Credentials correspond to a VOD content.
    */
  CREDENTIALS_TYPE_LCM,
    /**<  The Credentials correspond to a LCM.
    */
  CREDENTIALS_TYPE_HOME_NETWORK_LCM,
    /**<  Deprecated.
     *    The Credentials correspond to a Home Network LCM. This type is not 
     *    yet available.
    */
  CREDENTIALS_TYPE_UNKNOWN,
    /**<  The Credentials are of unknown type.
    */
  LAST_CREDENTIALS_TYPE
    /**<  Last Credentials type value.
     *    This value is never returned by the DVR/VOD Library.
    */
} TCredentialsType;

/**
 *  @ingroup groupCredInfo
 *  @brief
 *    This enum defines the two different types of viewing windows:
 *    one relative to the recording date and the other one relative
 *    to the first visualization date.
*/
typedef enum
{
  VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE = 0,
    /**<
     *    The viewing window is relative to the creation date.
     *    In case of a LCM, it corresponds to the recording date.
    */
  VIEWING_WINDOW_RELATIVE_TO_VISUALIZATION,
    /**<
     *    The viewing window is relative to the first visualization date.
    */
  LAST_VIEWING_WINDOW_RELATIVITY
    /**<
     *    Last viewing window relativity.
     *    This value is never returned, nor used by the DVR/VOD Library.
    */
} TViewingWindowRelativity;

/**
 *  @deprecated
 *  @ingroup groupCredInfo
 *  @brief
 *    This structure contains all public information contained into Credentials
 *    of type CREDENTIALS_TYPE_PROFILE.
*/
typedef struct
{
  TUnsignedInt32            profileId;
    /**<
     *    Unique identifier of the Profile.
    */
  TUnsignedInt16            profileVersion;
    /**<
     *    Version of the Profile.
    */
  TUnixDate                 credentialsCreationDate;
    /**<
     *    Date at which the Profile has been created.
    */
  TUnixDate                 profileExpirationDate;
    /**<
     *    Date at which the Profile expires. Corresponds to the date after 
     *    which no content can anymore be recorded using that Profile.
    */
  TViewingWindowRelativity  contentViewingWindowRelativity;
    /**<
     *    Relativity of the viewing window.
     *    @see ::TViewingWindowRelativity
    */
  TCaDuration               contentViewingWindowDuration;
    /**<
     *    Number of seconds the content recorded with help of this Profile can
     *    be played back.
     *    The viewing window can be relative to the recording date or to the
     *    content first visualization date.
    */
  TCaDuration               contentRetentionDuration;
    /**<
     *    Number of seconds the content recorded with help of this Profile can
     *    be stored on the HDD. It corresponds to the number of seconds after
     *    which the content expires.
    */
} TProfileData;

/**
 *  @ingroup groupCredInfo
 *  @brief
 *    This structure contains all public information contained into Credentials
 *    of type CREDENTIALS_TYPE_VOD_EMM.
 *    Once \c contentViewingWindowEndDate is reached, the STB application shall 
 *    remove the related content from the HDD.
 *    If the viewing window is relative to the recording date (the 
 *    \c credentialsCreationDate), the \c contentViewingWindowEndDate 
 *    corresponds to the \c contentExpirationDate. If the viewing window is 
 *    relative to the first content visualization, the 
 *    \c contentViewingWindowEndDate is smaller or equal to the 
 *    \c contentExpirationDate.
*/
typedef struct
{
  TUnsignedInt32            irdSerialNumber;
    /**<
     *    Unique identifier of the STB for which the credentials is intended. 
    */
  TUnsignedInt32            contentId;
    /**<
     *    Unique identifier of the VOD content.
    */
  TUnsignedInt32            vodEmmId;
    /**<
     *    Unique identifier of the VOD EMM.
    */
  TUnixDate                 credentialsCreationDate;
    /**<
     *    Date at which the VOD EMM has been created. That date corresponds to 
     *    the purchase date.
    */
  TUnixDate                 contentExpirationDate;
    /**<
     *    Date at which the VOD content expires.
     *    Corresponds to the date after which the related content can no more
     *    be played back, regardless of the \c viewingWindowEndDate.
    */
  TCaDuration               contentViewingWindowDuration;
    /**<
     *    Number of seconds the content can be played back.
     *    The viewing window can be relative to the purchase date or to the 
     *    content first visualization date.
    */
  TUnixDate                 contentFirstVisualizationDate;
    /**<
     *    Date of the first visualization of the VOD content.
     *    If the related VOD content has not yet been watched,
     *    \c contentFirstVisualizationDate is set to 0xFFFFFFFF.
     *    If the related VOD content is not encrypted (contentIsCiphered is
     *    set to FALSE), \c contentFirstVisualizationDate is not relevant and is
     *    also set to 0xFFFFFFFF.
    */
  TUnixDate                 contentViewingWindowEndDate;
    /**<
     *    Date after which the VOD content can no more be viewed.
    */
  TViewingWindowRelativity  contentViewingWindowRelativity;
    /**<
     *    Relativity of the viewing window.
     *    @see ::TViewingWindowRelativity
    */
  TBoolean                  contentIsCiphered;
    /**<
     *    TRUE:   The VOD content is protected and encrypted.
     *    FALSE:  The VOD content is not encrypted (it's a free content).
    */
  TBoolean                  contentKeyIsProtected;
    /**<
     *    TRUE:   The content key is protected.
     *    FALSE:  The content key is not protected.
    */
  TUnsignedInt32            nuidValue;
    /**<
     *    NUId of the entity to which the VOD EMM is intended.
    */
} TVodEmmData;

/**
 *  @ingroup groupCredInfo
 *  @brief
 *    This structure contains all public information contained into Credentials
 *    of type CREDENTIALS_TYPE_VOD_ECM.
 *    Once \c contentExpirationDate is reached, the STB application shall remove 
 *    the related content from the HDD.
*/
typedef struct
{
  TUnsignedInt32            contentId;
    /**<
     *    Unique identifier of the VOD content.
    */
  TUnixDate                 contentCreationDate;
    /**<
     *    Date at which the VOD content has been created.
    */
  TUnixDate                 contentExpirationDate;
    /**<
     *    Date at which the content expires.
     *    Corresponds to the date after which the related content can no more
     *    be played back, regardless of the \c viewingWindowEndDate.
     *    Once this date is reached, the STB application shall remove the 
     *    related content from the HDD.
    */
  TUnsignedInt16            contentEmi;
    /**<
     *    Encryption Method Indicator describing the encryption format used for
     *    the VOD content.
    */
  TBoolean                  contentKeyIsProtected;
    /**<
     *    TRUE:   The content key is protected.
     *    FALSE:  The content key is not protected.
    */
} TVodEcmData;

/**
 *  @ingroup groupCredInfo
 *  @brief
 *    This structure contains all public information contained into Credentials
 *    of type CREDENTIALS_TYPE_LCM.
 *    Once \c contentViewingWindowEndDate is reached, the STB application shall
 *    remove the related content from the HDD.
*/
typedef struct
{
  TUnsignedInt32            irdSerialNumber;
    /**<
     *    Unique identifier of the STB that created the LCM.
    */
  TUnsignedInt32            profileId;
    /**<  Deprecated.
     *    Unique identifier of the Profile that was used to create the LCM.
    */
  TUnsignedInt32            collectionId;
    /**<  Unique identifier of the collection to which belongs the content
     *    protected by this LCM. In order to be able to append two contents, 
     *    they must belong to the same collection.
    */
  TUnixDate                 credentialsCreationDate;
    /**<
     *    Date at which the LCM has been created.
     *    Corresponds to the content recording date.
    */
  TUnixDate                 contentExpirationDate;
    /**<
     *    Date at which the content expires.
     *    Corresponds to the date after which the related content can no longer
     *    be played back, regardless of the \c contentViewingWindowEndDate.
     *    Once this date is reached, the STB application shall remove the related
     *    content from the HDD.
    */
  TCaDuration               contentViewingWindowDuration;
    /**<
     *    Number of seconds the content can be played back.
     *    The viewing window can be relative to the record date or to the 
     *    content first visualization date.
    */
  TUnixDate                 contentFirstVisualizationDate;
    /**<
     *    Date of the first visualization of the content.
     *    If the related content has not yet been watched,
     *    \c contentFirstVisualizationDate is set to 0xDEADBEEF.
    */
  TUnixDate                 contentViewingWindowEndDate;
    /**<
     *    Date after which the content can no more be viewed with this LCM.
    */
  TUnixDate                 contentAppendEndDate;
    /**<
     *    Deprecated.
     *    Date until another content can be appended to this content. In order 
     *    to be able to append two contents, they must belong to the same 
     *    collection.
    */
  TViewingWindowRelativity  contentViewingWindowRelativity;
    /**<
     *    Relativity of the viewing window.
     *    @see ::TViewingWindowRelativity
    */
  TDvlAccessStatus          contentAccessStatus;
    /**< 
     *  \c DVL_ACCESS_GRANTED:
     *        The access to the related content is granted at the current time.
     *        If the \c irdSerialNumber of the LCM differs from the IRD S/N
     *        of the Set-Top Box previously returned by 
     *        \c dvlSystemInformationRequest(), it means the Set-Top Box is 
     *        able to access content recorded by the decoder identified by the 
     *        LCM address.<br>
     *  \c DVL_ACCESS_DENIED_CONTENT_EXPIRED:
     *        The access to the related content is expired.<br>
     *  \c DVL_ACCESS_DENIED_UNKNOWN_STB:
     *        The access to the related content is denied at the current time,
     *        because it has been recorded by an unknown set-top box, identified
     *        by the address of the LCM.<br>
     *  \c DVL_ACCESS_DENIED_UNKNOWN_NUID:
     *        The access to the related content is denied at the current time,
     *        because it has been recorded by an unknown chipset, identified
     *        by the address of the LCM.<br>
     *  \c DVL_ACCESS_DENIED_INVALID_EMI:
     *        The access to the related content is denied at the current time,
     *        because it has been recorded with an EMI unsupported by the SCR 
     *        implementation.<br>
     *  \c DVL_ACCESS_DENIED_INVALID_KEY_PROTECTION:
     *        The access to the related content is denied at the current time,
     *        because its key has been protected with an mechanism not 
     *        supported by the SCR implementation.
     */
  TUnsignedInt16            contentEmi;
    /**<
     *    Encryption Method Indicator describing the encryption of the content.
     */
  TUnsignedInt32            nuidValue;
    /**<
     *    NUId of the entity that created the LCM.
    */
  TDvlChipsetTypeString     chipsetType;
    /**<
     *    Type of the chipset that created the LCM.
    */
} TLcmData;

/** @cond APA_NO_EXPORT */

/**
 *  @deprecated
 *  @ingroup groupCredInfo
 *  @brief
 *    This structure contains all public information contained into Credentials
 *    of type CREDENTIALS_TYPE_HOME_NETWORK_LCM.
*/
typedef struct
{
  TUnsignedInt32            irdSerialNumber;
    /**<
     *    Unique identifier of the STB that created the HLCM.
    */
  TUnsignedInt32            homeNetworkId;
    /**<
     *    Unique identifier of the Home Network to which the HLCM belongs.
    */
  TUnsignedInt32            profileId;
    /**<
     *    Unique identifier of the Profile that was used to create the HLCM.
    */
  TUnsignedInt32            collectionId;
    /**<
     *    Unique identifier of the collection to which belongs the content
     *    protected by this HLCM. In order to be able to append two contents, 
     *    they must belong to the same collection.
    */
  TUnixDate                 credentialsCreationDate;
    /**<
     *    Date at which the HLCM has been created.
     *    Corresponds also to the recording date.
    */
  TUnixDate                 contentExpirationDate;
    /**<
     *    Date at which the content expires.
     *    Corresponds to the date after which the related content can no more
     *    be played back, regardless of the \c viewingWindowEndDate.
     *    Once this date is reached, the STB application shall remove the 
     *    related content from the HDD.
    */
  TCaDuration               contentViewingWindowDuration;
    /**<
     *    Number of seconds the content can be played back.
     *    The viewing window can be relative to the record date or to the 
     *    content first visualization date.
    */
  TUnixDate                 contentFirstVisualizationDate;
    /**<
     *    Date of the first visualization of the content.
     *    If the related content has not yet been watched,
     *    \c contentFirstVisualizationDate is set to 0xFFFFFFFF.
    */
  TUnixDate                 contentViewingWindowEndDate;
    /**<
     *    Date after which the content can no more be viewed.
     *    If the viewing window is relative to the recording date (as to know
     *    the \c credentialsCreationDate), the \c viewingWindowEndDate 
     *    corresponds to \c credentialsCreationDate + \c viewingWindowDuration.
     *    If the viewing window is relative to the first content visualization,
     *    the \c viewingWindowEndDate is equal to
     *    \c firstVisualizationDate + \c viewingWindowDuration.
    */
  TUnixDate                 contentAppendEndDate;
    /**<
     *    Date until another content can be appended to this content. In order 
     *    to be able to append two contents, they must belong to the same 
     *    collection.
    */
  TViewingWindowRelativity  contentViewingWindowRelativity;
    /**<
     *    Relativity of the viewing window.
    */
  TDvlAccessStatus          contentAccessStatus;
    /**<
     *    \c DVL_ACCESS_GRANTED:
     *        The access to the related content is granted at the current time.
     *        If the \c irdSerialNumber of the LCM differs from the IRD S/N
     *        of the Set-Top Box previously returned by \c dvlInitialziation(),
     *        that means the Set-Top Box is granted to access content recorded
     *        by the decoder identifier by the LCM \c irdSerialNumber.<br>
     *    \c DVL_ACCESS_DENIED_CONTENT_EXPIRED:
     *        The access to the related content is expired.<br>
     *    \c DVL_ACCESS_DENIED_UNKNOWN_HOME_NETWORK:
     *        The access to the related content is denied at the current time,
     *        because it has been recorded by a set-top box belonging to an
     *        unknown Home Network, identified by the \c homeNetworkId of the 
     *        HLCM.
     */
} THomeNetworkLcmData;

/** @endcond */

/**
 *  @ingroup groupCredInfo
 *  @brief
 *    The output format of the Credentials information request depends on the 
 *    type of Credentials. The information of the Credentials is therefore 
 *    returned in the form of a union. The content of the union returned by 
 *    \c dvlCredentialsInformationRequest() depends on the output 
 *    \c *pxCredentialsType parameter.
*/
typedef union
{
  TProfileData          profileData;
    /**<  Deprecated
     *    If the Credentials are of type \c CREDENTIALS_TYPE_PROFILE,
     *    their information will be stored into the \c profileData.
    */
  TVodEmmData           vodEmmData;
    /**<
     *    If the Credentials are of type \c CREDENTIALS_TYPE_VOD_EMM,
     *    their information will be stored into the \c vodEmmData.
    */
  TVodEcmData           vodEcmData;
    /**<
     *    If the Credentials are of type \c CREDENTIALS_TYPE_VOD_ECM,
     *    their information will be stored into the \c vodEcmData.
    */
  TLcmData              lcmData;
    /**<
     *    If the Credentials are of type \c CREDENTIALS_TYPE_LCM,
     *    their information will be stored into the \c lcmData.
    */
  THomeNetworkLcmData   homeNetworkLcmData;
    /**<  Deprecated
     *    If the Credentials are of type \c CREDENTIALS_TYPE_HOME_NETWORK_LCM,
     *    their information will be stored into the \c homeNetworkLcmData.
    */
} TCredentialsUnion;

/**
 *  @ingroup groupSystemInfo
 *  @brief
 *    This structure contains information specific to the chipset.
*/
typedef struct
{
  TUnsignedInt32            nuidValue;
    /**<
     *    NUId of the chipset.
    */
  TDvlUniqueIdString        nuidString;
    /**<
     *    NULL-terminated and printable string containing the Nagra unique 
     *    identifier of the chipset.
    */
  TDvlChipsetTypeString     type;
    /**<
     *    This field contains the chipset type as a NULL-terminated string.
   */
} TDvlChipsetInfo;


/**
 * @ingroup groupInitTerm 
 * @brief
 *   UTC time importation callback.
 *
 *   This decoder manufacturer's function returns the current UTC time and date
 *   to the DVR VOD library. It will be called each time the DVR VOD Library
 *   needs this information.
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param[out] pxUtcTime
 *    UTC Time.
 *
 *  @remark
 *    -# This function is called in the DVR/VOD Library context. Therefore,
 *       its execution should be as short as possible. It must not block the
 *       calling task.
 *    -# As consequence of previous remark, no other function of this API
 *       shall be called directly inside the notification callback.
*/
typedef void (*TUtcUnixTimeImportationCB)
(
  TUnixDate* pxUtcTime
);

/**
 * @ingroup groupInitTerm 
 * @brief
 *   Pairing data importation callback.
 *
 *   This decoder manufacturer's function gets the pairing data
 *   (reconstruction), stored in the protected memory area of the decoder. The
 *   buffer is provided by this function. It should remain valid until the
 *   DVR/VOD Library shuts down.
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param[out] ppxNaspPairingData
 *            Pointer to the first byte of Nagravision-provided pairing
 *            data.
 *
 *  @remark
 *    -# This function is called in the DVR/VOD Library context. Therefore,
 *       its execution should be as short as possible. It must not block the
 *       calling task.
 *    -# As consequence of previous remark, no other function of this API
 *       shall be called directly inside the notification callback.
 *
*/
typedef void (*TPairingImportationCB)
(
  const TUnsignedInt8** ppxNaspPairingData
);

/**
 * @ingroup groupRecord
 * @brief
 *    Record error callback.
 *
 *    This callback is called by the DVR/VOD Library if an error occurs during
 *    a record session. In that case, the DVR/VOD Library sets the error status
 *    in the parameter \c xRecordStatus.
 *
 *    If the callback is called by the DVR/VOD Library, the STB Application
 *    shall consider the recording has failed. It shall initiate a message that
 *    will then stop the recording and return from the callback. Once the STB
 *    application runs in its context again, it shall call \c dvlStopRecord().
 *
 *    This callback can only be called by the DVR/VOD Library as long as the
 *    recording handle is valid (i.e. as long as the STB Application has not
 *    called \c dvlStopRecord() with that recording handle).
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param[in] pxRecordHandle
 *            Pointer to the handle of the concerned recording session.
 *  @param[in] xRecordStatus
 *            DVL Record status returned by the DVR/VOD Library.
 *
 *  @remark
 *    -# This function is called in the DVR/VOD Library context. Therefore,
 *       its execution should be as short as possible. It must not block the
 *       calling task.
 *    -# As consequence of previous remark, no other function of this API
 *       shall be called directly inside the notification callback.
 *
*/
typedef void (*TDvlRecordErrorCB)
(
  TDvlHandle*      pxRecordHandle,
  TDvlRecordStatus  xRecordStatus
);

/**
 *  @ingroup groupRecord
 *  @brief
 *    This structure contains the details of the recording parameters used by 
 *    the DVL. To ease the management of default values and API compatibility, 
 *    this structure SHALL only be allocated by the DVL by calling 
 *    dvlCreateRecordSessionParameters and never by the application itself.
 *    When this structure is released by calling dvlReleaseBuffer, the memory
 *    pointed by pInputCredentials and pSpecificMetadata is untouched.
*/
typedef struct 
{
  TTransportSessionId       tsId;
    /**<
     *    Identifier of transport stream to be recorded.
    */
  TDvlRecordErrorCB         pErrorCB;
    /**<
     *    Callback used by the DVR/VOD Library to report a recording error.
    */
  TUnsignedInt32            collectionId;
    /**<
     *    Unique identifier of the collection to which the content belongs.
     *    The collection identifier is used for recorded content appending
    */
  TSize                     inputCredentialsSize;
    /**<
     *    Size of the optional input Credentials.
     *    Shall be set to zero in case no input Credentials is pased to
     *    the DVR/VOD Library.
    */
  TUnsignedInt8*            pInputCredentials;
    /**<
     *    Optional Credentials of the content to which the current recording
     *    should be appended.
     *    Should be set to \c NULL if the current recording is not intended
     *    to be appended to any other content.
    */
  TSize                     specificMetadataSize;
    /**<
     *    Size of the optional specific metadata to be stored in the output 
     *    Credentials. Shall be set to zero in case no specific related 
     *    metadata are passed to the DVR/VOD Library.
    */
  TUnsignedInt8*            pSpecificMetadata;
    /**<
     *    Optional specific metadata to be stored in the output Credentials.
     *    Shall be set to NULL in case no specific metadata are passed to 
     *    the DVR/VOD Library.
    */
  TViewingWindowRelativity  viewingWindowRelativity;
    /**<
     *    Relativity of the viewing window.
     *    @see ::TViewingWindowRelativity
    */
  TCaDuration               viewingWindowDuration;
    /**<
     *    Number of seconds the content recorded can be played back.
     *    The viewing window can be relative to the recording date or to the
     *    content first visualization date.
    */
  TCaDuration               retentionDuration;
    /**<
     *    Number of seconds the content recorded can stay stored on the HDD. It 
     *    corresponds to the number of seconds after which the content expires.
    */
   TUnsignedInt16            emi;
    /**<
     *    EMI used for content encryption 
    */
} TDvlRecordSessionParameters;



/**
 * @ingroup groupPlayback
 * @deprecated
 *   This structure is used with deprecated functions dvlStartPlayback and dvlStartVodPlayback
 * @brief
 *    Playback error callback.
 *
 *    This callback is called by the DVR/VOD Library if an error occurs during
 *    a playback session. In that case, the DVR/VOD Library sets the
 *    corresponding error status in the parameter \c xPlaybackStatus.
 *
 *    If the callback is called by the DVR/VOD Library, the STB Application
 *    shall consider the playback has no more allowed. It shall initiate a
 *    message that will then stop the concerned playback and return from the
 *    callback.
 *    Once the STB application runs in its context again, it shall call
 *    \c dvlStopPlayback() with the corresponding playback handle.
 *
 *    This callback can only be called by the DVR/VOD Library as long as the
 *    playback handle is valid (i.e. as long as the STB Application has not
 *    called \c dvlStopPlayback() with that playback handle).
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param[out] pxPlaybackHandle
 *            Pointer to the handle of the concerned playback session.
 *  @param[out] xPlaybackStatus
 *            DVL playback status returned by the DVR/VOD Library.
 *
 *  @remark
 *    -# This function is called in the DVR/VOD Library context. Therefore,
 *       its execution should be as short as possible. It must not block the
 *       calling task.
 *    -# As consequence of previous remark, no other function of this API
 *       shall be called directly inside the notification callback.
 *
*/
typedef void (*TDvlPlaybackErrorCB)
(
  TDvlHandle*      pxPlaybackHandle,
  TDvlAccessStatus  xPlaybackStatus
);


/**
 * @ingroup groupPlayback
 * @brief
 *    Playback Notification callback on an Entitlement.
 *
 *    This callback is called by the DVR/VOD Library if an error occurs during
 *    a playback session. In that case, the DVR/VOD Library sets the
 *    corresponding error status in the parameter \c xPlaybackStatus.
 *    It also set the privateData pointer with the value supplied when calling dvlPlaybackSessionSetEntitlements().
 *
 *    If the callback is called by the DVR/VOD Library, the STB Application
 *    shall consider the playback has no more allowed. It shall initiate a
 *    message that will then take proper action of the concerned playback and return from the
 *    callback.
 *    Once the STB application runs in its context again, it should call
 *    \c dvlPlaybackCloseSession() with the corresponding playback handle.
 *
 *    This callback can only be called by the DVR/VOD Library as long as the
 *    playback handle is valid (i.e. as long as the STB Application has not
 *    called \c dvlPlaybackCloseSession() with that playback handle).
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @param[out] pxPlaybackHandle
 *            Pointer to the handle of the concerned playback session.
 *  @param[out] pxPrivateData
 *            Pointer to Application Private Data allowing Application to store 
 *             any contextual information
 *  @param[out] xPlaybackStatus
 *            DVL playback status returned by the DVR/VOD Library.
 *
 *  @remark
 *    -# This function is called in the DVR/VOD Library context. Therefore,
 *       its execution should be as short as possible. It must not block the
 *       calling task.
 *    -# As consequence of previous remark, no other function of this API
 *       shall be called directly inside the notification callback.
 *
*/
typedef void (*TDvlPlaybackNotificationCB)
(
  TDvlHandle*      pxPlaybackHandle,
  void *           pxPrivateData,
  TDvlAccessStatus  xPlaybackStatus
);



/**
 *  @ingroup groupPlayback
 *  @brief
 *    Structure used when calling the dvlPlaybackSetEntitlements() request.
 *    It contains input credentials and Application Private Data that will be 
 *    returned in dvlPlaybackNotificationCallback. <br>
 *    Current known Entitlement format for Playback are:
 *    - One Credentials buffer required to perform the playback
 *    - Two Credentials buffer required to perform the playback
*/
typedef struct
{
	TSize  firstCredentialsSize;
	/**<  1st credentials size.<br>
	  * Size in bytes of the mandatory first input credential
    */
  TUnsignedInt8*  firstCredentials;
  /**<  1st credentials buffer.<br>
    *    Pointer to the  first input Credentials related to the content to play back.
    *    During a call to dvlPlaybackSetEntitlements(), the DVR/VOD Library won't modify that buffer. But the STB
    *      application shall replace on the content storage this input
    *    Credentials by the output Credentials.<br>
    */
  TSize  secondCredentialsSize;
  /**<  2nd credentials size.<br>
    *   Size in bytes of the second input credential.
    *   This field is only used when 2 credentials buffer are required, else it shall be 0.
    */
  TUnsignedInt8*  secondCredentials;
  /**<  2nd credentials buffer.<br>
    *    Pointer to the second input Credentials (type ECM)
    *      related to the content to play back.
    *    During a call to dvlPlaybackSetEntitlements(), the DVR/VOD Library won't modify that structure. But the STB
    *      application shall replace on the content storage the input
    *    Credentials by the output Credentials.<br>
    *    This field is only used when 2 credentials are needed, else it shall be NULL.
    */ 
  TDvlPlaybackNotificationCB   playbackNotificationCB;
  /**<  Entitlement Notification Callback.<br>
    *      Callback used by the DVR/VOD Library to report any playback error on the related Entitlement.
    */
  void* privateData;
  /**<  Application private data for NotificationCallback.<br>
    *   The Application can put any information at this address 
    *   in order to be able to manage correctly the callback.
    */
} TDvlEntitlementsParameters;



/******************************************************************************/
/*                          DVR/VOD API LIBRARY                               */
/******************************************************************************/
/******************************************************************************/
/*                                                                            */
/*                            FUNCTIONS PROTOTYPES                            */
/*                                                                            */
/******************************************************************************/
/**
 *  @ingroup groupInitTerm 
 *  @brief
 *    Initialize the DVR/VOD Library. This call may be done only once. No 
 *    function of the DVR/VOD Library API shall be called before the end of the 
 *    initialization stage. If the DVR/VOD Library is shutdown by a call to 
 *    \c dvlTerminate(), it can be initialized again by calling 
 *    \c dvlInitialize().
 *
 *  @pre
 *    The DVR/VOD Library must not be running.
 *
 *  @post
 *    The DVR/VOD Library may be started.
 *
 *  @param[in] xPairingImportationCB
 *            Callback function used by the DVR/VOD Library to get its pairing
 *            data block from the decoder OTP memory.
 *  @param[in] xUtcUnixTimeImportationCB
 *            Callback function used by the DVR/VOD Library to get the UTC time.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *            Either a callback pointer or the mandatory output pointers NULL.
 *  @retval DVL_ERROR
 *            The function execution failed.
*/
NAGRA_CA_API TDvlStatus dvlInitialize
(
  TPairingImportationCB       xPairingImportationCB,
  TUtcUnixTimeImportationCB   xUtcUnixTimeImportationCB
);

/**
 *  @ingroup groupInitTerm 
 *  @brief
 *   Terminate the DVR/VOD Library. This call may be made only once. No other 
 *   function of the DVR/VOD Library API shall be called after the termination 
 *   stage, except the initialization function \c dvlInitialize().
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    The DVR/VOD Library may be stopped.
 *
 *  @remark
 *    -# This function is synchronous. It means that the caller will be
 *       suspended as long as all the task have not performed their cleanup and
 *       have not terminated.
 *    -# During the \c dvlTerminate() call execution, no other functions of the
 *       DVR/VOD Library API may be called.
*/
NAGRA_CA_API void dvlTerminate
(
  void
);

/**
 *  @ingroup groupSystemInfo
 *  @brief
 *    This function allows the STB application to request information about
 *    DVR/VOD Library.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @param[out] xVersionString
 *            NULL-terminated string of maximum length 
 *            DVL_VERSION_STRING_LENGTH Bytes, containing the project name, 
 *            project version and DVR/VOD Library version.
 *  @param[out] pxIrdSerialNumberValue
 *            Value of the Nagra unique identifier of the Set-Top Box on which
 *            runs the DVR/VOD Library.
 *  @param[out] xIrdSerialNumberString
 *            NULL-terminated and printable string containing the Nagra unique
 *            identifier of the Set-Top Box on which runs the DVR/VOD Library.
 *  @param[out] pxNuidValue
 *            Value of the Nagra unique identifier of the chipset.
 *  @param[out] xNuidString
 *            NULL-terminated and printable string containing the Nagra unique
 *            identifier of the chipset.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One of the input pointers of the function is invalid.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlSystemInformationRequest
(
  TDvlVersionString   xVersionString,
  TUnsignedInt32*    pxIrdSerialNumberValue,
  TDvlUniqueIdString  xIrdSerialNumberString,
  TUnsignedInt32*    pxNuidValue,
  TDvlUniqueIdString  xNuidString
);

/**
 *  @ingroup groupSystemInfo
 *  @brief
 *    This function allows the STB application to request information about
 *    the chipset the DVR/VOD Library was compiled for.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @param[out] ppxChipsetInformation
 *            Pointer on a static structure of the DVR/VOD Library containing 
 *            the chipset information.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One of the input pointers of the function is invalid.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlChipsetInformationRequest
(
  TDvlChipsetInfo**   ppxChipsetInformation
);



/**
 *  @ingroup groupHomeNetwork
 *  @brief
 *    This function allows the STB application to request information about
 *    the Home Networking.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @param[out] pxHomeNetworkIdValue
 *            Nagra unique identifier of the Home Network to which the STB
 *            belongs.
 *            If the Set-Top Box does not belong to any Home Network,
 *            the DVR/VOD Library returns 0xFFFFFFFF as 
 *            \c *pxHomeNetworkIdValue.
 *  @param[out] xHomeNetworkIdString
 *            NULL-terminated and printable string containing the Nagra unique
 *            Home Network Identifier.
 *            If the Set-Top Box doesn't belong to any Home Network,
 *            the string will contain "42 9496 7295 96".
 *  @param[out] pxHomeNetworkVersion
 *            Version of the Home Network to which the STB belongs.
 *            If the STB doesn't belong to any Home Network, the DVR/VOD
 *            Library returns 0xFF as \c pxHomeNetworkVersion.
 *  @param[out] pxNumberOfNeighbourhoodInfoItems
 *            Number of neighbourhood information items contained in
 *            \c ppxNeighbourhoodInfoItems.
 *  @param[out] ppxNeighbourhoodInfoItems
 *            Array containing information about neighbourhood Set-Top Boxes
 *            secrets.
 *            The DVR/VOD Library possesses some secrets allowing to playback
 *            the content recorded by any of the neighbourhood Set-Top Boxes
 *            listed in this array. This buffer shall be released by calling
 *            the function \c dvlReleaseBuffer().
 *            This array might be empty, as is does not contain the identifier 
 *            of the local Set-Top Box on which runs this particular instance 
 *            of DVR/VOD Library. This array may contain more than once the 
 *            information item related to the same neightbourhood Set-Top Box, 
 *            once per secret type (see \c TDvlSecretsType).
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One of the input pointers of the function is invalid.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @see ::dvlReleaseBuffer()
*/
NAGRA_CA_API TDvlStatus dvlHomeNetworkInformationRequest
(
        TUnsignedInt32*          pxHomeNetworkIdValue,
        TDvlUniqueIdString        xHomeNetworkIdString,
        TUnsignedInt8*           pxHomeNetworkVersion,
        TUnsignedInt8*           pxNumberOfNeighbourhoodInfoItems,
  const TDvlNeighbourhoodInfo** ppxNeighbourhoodInfoItems
);

/**
 *  @ingroup groupRecord
 *  @brief
 *    This function allocates a TDvlRecordSessionParameters structure, 
 *    initializes it with default values and returns it to the application. 
 *    At this time and if the result of the function is DVL_NO_ERROR, the 
 *    structure contains all the input parameters of a 
 *    recording session with their default values hardcoded in the DVR/VOD 
 *    Library. The application SHALL then modify those values according to 
 *    content usage rules and then start recording sessions by calling 
 *    \c dvlStartRecordEx.<br>
 *    When no more used, the structure needs to be released by calling 
 *    \c dvlReleaseBuffer().<br>
 *    Usage of this function to create the structure is mandatory to ensure 
 *    backward compatibility with future evolutions of the DVL.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[out] ppxRecordParameters
 *            Returned pointer to the structure allocated by the DVL and 
 *            containing the default recording parameters. 
 *            If the method fails, the retuned pointer is NULL.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function was not able to allocate the structure to return.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            the function was called with a NULL pointer.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @remark
 *    -# The TTransportSessionId attribute of the structure is initialized by 
 *      default to TRANSPORT_SESSION_INVALID and SHALL be configured by the 
 *      application.
 *    -# The TDvlRecordErrorCB attribute of the structure is initialized by
 *      default to NULL and SHALL be configured by the application.
 *    -# The rest of the attributes of the structure CAN be modified by the 
 *      application and have the following default values: 
 *     (collectionId : 0xFFFFFFFF), (inputCredentialsSize : 0), 
 *     (pInputCredentials : NULL), (specificMetadataSize : 0), 
 *     (pSpecificMetadata : 0), (viewingWindowRelativity : VIEWING_WINDOW_RELATIVE_TO_CREATION_DATE),
 *     (viewingWindowDuration : 0xFFFFFFFF), (retentionDuration : 0xFFFFFFFF) 
 *     (emi : default value set at DVL compilation time)
*/
NAGRA_CA_API TDvlStatus dvlCreateRecordSessionParameters
(
        TDvlRecordSessionParameters** ppxRecordParameters
);

/**
 *  @ingroup groupRecord
 *  @brief
 *    This function sends a record session request to the DVR/VOD Library.
 *    When this function sets \c DVL_RECORD_GRANTED into \c *pxRecordingStatus, 
 *    the corresponding content key has been properly set into the chipset 
 *    stream cipher and the STB application can start storing the content on 
 *    the HDD.<br> 
 *    This function also allows appending the current record to a previous 
 *    recorded content, by passing as argument the Credentials related to 
 *    the previous recorded content to which the current recording shall be 
 *    appended. In append mode, the function ignores the specific metadata,
 *    viewing windows and retention duration specified in the the 
 *    TDvlRecordSessionParameters.<br>
 *    If the append is authorized, the current content will be recorded 
 *    with same content key as the event to which it is appended. The output 
 *    Credentials then apply to both contents, the previously recorded one and 
 *    the currently recorded one; the STB application may then consider the 
 *    two records as only one.<br>
 *    If the append fails (record status \c DVL_RECORD_DENIED_APPENDING_DENIED 
 *    is returned in \c *pxRecordingStatus), no content key is set into the 
 *    stream cipher of the chipset. In that case, the STB application may 
 *    still try a standalone recording (without passing any input 
 *    Credentials).<br>
 *    If the function returned with the record status DVL_RECORD_GRANTED, the 
 *    STB application shall copy/store the content of the output Credentials 
 *    buffer on the HDD and then release it by calling the function 
 *    \c dvlReleaseBuffer().<br>
 *    Once the function returned with the record status \c DVL_RECORD_GRANTED, 
 *    the DVR/VOD Library may call the passed record error callback at any 
 *    time. The main reason is that the recording may be subject to a time 
 *    limit and the recording is suddenly denied. In that case, the STB 
 *    application shall stop the current recording session. When the function 
 *    returns with error status \c DVL_NO_ERROR, it does not necessarily mean 
 *    the recording is successful. The STB Application still needs to check the 
 *    value set in the record status \c *pxRecordStatus parameter.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] pxRecordParameters
 *            Pointer on a record parameters structure allocated by the 
 *            dvlCreateRecordSessionParameters function.
 *  @param[out] pxOutputCredentialsSize
 *            Pointer to the length of the output Credentials.
 *            This size is set to zero by the DVR/VOD Library in case the
 *            returned status is not equal to \c DVL_RECORD_GRANTED.
 *  @param[out] ppxOutputCredentials
 *            This pointer will contain the address of the memory where the 
 *            output Credentials will be stored if the function returns 
 *            DVL_RECORD_GRANTED. In case the function returns any other 
 *            status, the returned address will be set to \c NULL. The memory 
 *            pointed by \c *ppxOutputCredentials shall be released by the STB 
 *            application by calling the dedicated function 
 *            \c dvlReleaseBuffer().
 *  @param[out] pxRecordingStatus
 *            Pointer to the memory where the DVR/VOD Library will store the
 *            recording status.<br>
 *            If pxRecordingStatus is greater or equal to \c DVL_RECORD_DENIED,
 *            the content key has not been set into the chipset stream cipher 
 *            and the content cannot be recorded.
 *  @param[out] ppxRecordHandle
 *            Pointer where the DVR/VOD Library will store the recording handle.
 *            The input pointer shall not be \c NULL.
 *            The value set by the DVR/VOD Library is significant only if
 *            \c *pxRecordingStatus is \c DVL_RECORD_GRANTED.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @remark
 *    -# If the function returns \c DVL_NO_ERROR and \c *pxRecordStatus contains
 *      \c DVL_RECORD_GRANTED, the STB application shall copy/store the content
 *      of the output Credentials buffer on the HDD and then release it by 
 *      calling the function \c dvlReleaseBuffer().
 *    -# The DVL itself supports several recording sessions at the same time, 
 *      the underlying STB drivers might not. When they do, the 
 *      TTransportSessionId used to start each record SHALL be configured 
 *      accordingly.
*/
NAGRA_CA_API TDvlStatus dvlStartRecordEx
(
        TDvlRecordSessionParameters*  pxRecordParameters,
        TSize*                        pxOutputCredentialsSize,
  const TUnsignedInt8**              ppxOutputCredentials,
        TDvlRecordStatus*             pxRecordingStatus,
        TDvlHandle**                 ppxRecordHandle
);

/**
 *  @ingroup groupRecord
 *  @brief
 *   This function stops the current recording session handled by 
 *   \c pxRecordHandle. The session has been started by \c dvlStartRecordEx().
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *    A valid recording session must be running, handled by the parameter
 *    \c pxRecordHandle.
 *
 *  @post
 *    None
 *
 *  @param[in] pxRecordHandle
 *            Recording session handle.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            The input argument of the function is invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlStopRecord
(
  TDvlHandle*  pxRecordHandle
);



/**
 *  @ingroup groupPlayback
 *  @brief
 *    This function opens a Playback Session for the specified TSId.<br>
 *    The application can use this session to set keys embedded in Entitlements.<br>
 *    
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xTSid
 *            Identifier of transport session to be played back.
 *  @param[out] ppxPlaybackHandle
 *            Pointer where the DVR/VOD Library will store the playback session
 *            handler.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
*/
NAGRA_CA_API TDvlStatus dvlPlaybackOpenSession
(
        TTransportSessionId   xTSid,
        TDvlHandle**        ppxPlaybackHandle
);



/**
 *  @ingroup groupPlayback
 *  @brief
 *    This function gives Entitlements to the DVR/VOD library.<br>
 *    When this function returns with the status \c DVL_ACCESS_GRANTED set in 
 *    \c *pxPlaybackStatus, that means that the corresponding content key has 
 *    been correctly set in the chipset stream cipher and the STB application 
 *    can start playing back the content.<br>
 *    If the function returns with the playback status \c DVL_ACCESS_GRANTED, 
 *    the STB application shall copy/store the content of the output 
 *    Credentials buffer on the HDD and then release it by calling the function 
 *    \c dvlReleaseBuffer(). That output Credentials should replace any one 
 *    already existing for that content.<br>
 *    Once the function returned with the playback status \c DVL_ACCESS_GRANTED,
 *    the DVR/VOD Library may call the playback error callback supplied in 
 *    pxEntitlementsParameters at any time. The main reason is that the playback may be subject to a time limit 
 *    and the playback is suddenly denied. In that case, the STB application 
 *    shall stop the current playback session.<br>
 *    When the function returns with error status \c DVL_NO_ERROR, it does not 
 *    necessarily mean the playback is successful. The STB Application still 
 *    needs to check the value set in the playback status \c *pxPlaybackStatus 
 *    parameter.<br>
 *    Note that this function can be called several times during a Playback session
 *    in order to load several keys. It enables to manage key change smoothly. The keys
 *    table is managed in circular buffer mode, that means that when the table is full,
 *    the oldest key is replaced by the new key.
 *    If several Entitlements are set with the same KeyId, the Callback will not be called for former Entitlements
 *    since the key is overriden.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] pxPlaybackHandle
 *            Identifier of the playback session previously created with dvlPlaybackOpenSession().
 *  @param[in] pxEntitlementsParameters
 *            Structure containing Credentials and associated Notification Callback.
 *  @param[out] pxPlaybackStatus
 *            Pointer where the DVR/VOD Library will store the playback status.
 *            If \c pxPlaybackStatus is greater or equal to 
 *            \c DVL_ACCESS_DENIED, the content key has not been set into the 
 *            chipset stream cipher and the content cannot be played back.
 *  @param[out] pxOutputCredentialsSize
 *            Pointer where the DVR/VOD Library will store the size in bytes of the 
 *            buffer containing the output credential.
 *  @param[out] ppxOutputCredentials
 *            Pointer where the DVR/VOD Library will store the pointer to the
 *            memory that will contain the output Credentials.<br>
 *            If the input pointer is \c NULL, the DVR/VOD library will not fill this field.<br>
 *            Else, the STB application shall replace all input Credentials on the
 *            content storage by the output Credentials and then release the
 *            buffer by calling \c dvlReleaseBuffer().
 *  @param[out] pxKeyIdSize
 *            Pointer where the DVR/VOD Library will store the Size in Bytes of
 *            the buffer containing the output keyId.
 *            The input pointer shall not be NULL if ppxKeyId is not NULL. 
 *            If no KeyId is present in the Entitlment, the KeyId size is set to zero.
 *  @param[out] ppxKeyId
 *            Pointer where the DVR/VOD Library will store the pointer to the
 *            memory that will contain the KeyId. <br>
 *            This buffer can be \c NULL if the caller is not interested in retrieving KeyId.<br>
 *            Else:
 *            - If no KeyId is present in the Entitlement, *ppxKeyId will be set to NULL.
 *            - else the STB application shall release the buffer by calling \c dvlReleaseBuffer().
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
*/
NAGRA_CA_API TDvlStatus dvlPlaybackSetEntitlements
(
        TDvlHandle*                 pxPlaybackHandle,
        TDvlEntitlementsParameters* pxEntitlementsParameters,
        TDvlAccessStatus*           pxPlaybackStatus,
        TSize*                      pxOutputCredentialsSize,
  const TUnsignedInt8**            ppxOutputCredentials,
        TSize*                      pxKeyIdSize,
  const TUnsignedInt8**            ppxKeyId
);


/**
 *  @ingroup groupPlayback
 *  @brief
 *    This function is responsible for closing the playback session, which
 *    consists in the cleanup of the session's resources.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *    A valid playback session created with dvlPlaybackOpenSession 
 *    must be open, handled by the parameter pxPlaybackHandle.
 *
 *  @post
 *    None
 *
 *  @param[in] pxPlaybackHandle
 *            Handle of the playback session to close.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            The playback handle is invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlPlaybackCloseSession
(
        TDvlHandle*        pxPlaybackHandle
);



/**
 *  @ingroup groupCredInfo
 *  @brief
 *    This function allows the STB application to request information about
 *    DVR/VOD Library Credentials.
 *    This information returned by this request depends on the type of
 *    Credentials.
 *
 *    Some Credentials may contain operator specific data. As these data are
 *    operator dependant and are not interpreted by the DVR/VOD Library, these
 *    specific data are returned by this request in the form of a buffers of
 *    Bytes pointed by \c *ppxGenericMetadata  and \c *ppxSpecificMetadata. 
 *    The STB manufacturer shall refer to the operator for the format and 
 *    meaning of the information contained in these metadata.
 *    All output pointers shall be released by the function 
 *    \c dvlReleaseBuffer().
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xCredentialsSize
 *            Size in Bytes of the buffer containing the Credentials.
 *  @param[in] pxCredentials
 *            Pointer to the buffer containing the Credentials.
 *  @param[out] pxCredentialsType
 *            Pointer where the DVR/VOD Library will store the Credentials
 *            type.
 *  @param[out] ppxCredentialsUnion
 *            Pointer where the DVR/VOD Library will store the Credentials
 *            information.
 *            The input pointer shall not be NULL.
 *  @param[out] pxGenericMetadataSize
 *            Pointer where the DVR/VOD Library will store the size of the 
 *            optional generic metadata. The input pointer shall not be NULL 
 *            if ppxGenericMetadata is not NULL.
 *  @param[out] ppxGenericMetadata
 *            Pointer where the DVR/VOD Library will store the pointer to the 
 *            buffer containing the optional generic metadata. This buffer can 
 *            be NULL if the caller is not interested in retrieving generic 
 *            metadata.
 *            Once the function returns, \c *ppxGenericMetadata may be NULL if 
 *            the input Credentials contain no generic metadata or if they are 
 *            not valid (function returns \c DVL_ERROR_BAD_PARAMETER).
 *            If returned, the buffer pointed by \c *ppxGenericMetadata shall be 
 *            released once no longer used by the STB application, by calling 
 *            \c dvlReleaseBuffer().
 *  @param[out] pxSpecificMetadataSize
 *            Pointer where the DVR/VOD Library will store the size of the 
 *            optional specific metadata. The input pointer shall not be \c NULL 
 *            if \c ppxSpecificMetadata is not \c NULL.
 *  @param[out] ppxSpecificMetadata
 *            Pointer where the DVR/VOD Library will store the pointer to the 
 *            buffer containing the optional specific metadata. This buffer can 
 *            be NULL if the caller is not interested in retrieving specific 
 *            metadata.
 *            Once the function returns, \c *ppxSpecificMetadata may be NULL if 
 *            the input Credentials contain no specific metadata or if they are 
 *            not valid (function returns \c DVL_ERROR_BAD_PARAMETER).
 *            The buffer pointed by \c *ppxSpecificMetadata shall be released once 
 *            no longer used by the STB application, by calling 
 *            \c dvlReleaseBuffer().
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @see ::dvlReleaseBuffer()
*/
NAGRA_CA_API TDvlStatus dvlCredentialsInformationRequest
(
        TSize                 xCredentialsSize,
  const TUnsignedInt8*       pxCredentials,
        TCredentialsType*    pxCredentialsType,
  const TCredentialsUnion** ppxCredentialsUnion,
        TSize*               pxGenericMetadataSize,
  const TUnsignedInt8**     ppxGenericMetadata,
        TSize*               pxSpecificMetadataSize,
  const TUnsignedInt8**     ppxSpecificMetadata
);


/**
 *  @ingroup groupPMM
 *  @brief
 *    This function allows the STB application to send to the DVR/VOD Library 
 *    PRM Management Messages sent by the Head End.<br>
 *    The DVR/VOD Library processes the message in the function itself. That 
 *    means the STB application can release the buffer once the function 
 *    returns.<br>
 *    The buffer pointed by \c pxPrmManagementMessage shall remain valid 
 *    during the whole function processing. Once the function returns, the STB 
 *    application can release the pointed memory buffer.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xPrmManagementMessageSize
 *            Size of the PRM Management Message.
 *  @param[in] pxPrmManagementMessage
 *            Pointer to the buffer of Bytes containing the PRM Management
 *            message.
 *
 *  @retval DVL_NO_ERROR
 *            The message was successfully processed.
 *  @retval DVL_ERROR
 *            The function execution failed. The management message could not
 *            be processed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlPostPrmManagementMessage
(
        TSize             xPrmManagementMessageSize,
  const TUnsignedInt8*   pxPrmManagementMessage
);

/**
 *  @ingroup groupPMM
 *  @brief
 *    This function allows the STB application to retrieve private data that are
 *    contained in a ciphered message.
 *    The DVR/VOD Library decrypts the input message, verifies the signature
 *    that it contains and returns the private data in plaintext.<br>
 *    The function returns only authenticated data. If the decryption or the
 *    signature verification fails, it returns an error status and the pointer
 *    to the data is \c NULL.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xCipheredMessageSize
 *            Size of the input ciphered message.
 *  @param[in] pxCipheredMessage
 *            Pointer to the buffer of Bytes containing the ciphered message.
 *  @param[out] pxAuthenticatedDataSize
 *            Pointer where the DVR/VOD Library will store the size of the
 *            authenticated data contained in the ciphered message.
 *            The input pointer shall not be NULL.
 *  @param[out] ppxAuthenticatedData
 *            Pointer where the DVR/VOD Library will store the pointer to
 *            the buffer containing the authenticated data.
 *            This buffer cannot be NULL.
 *
 *  @retval DVL_NO_ERROR
 *            The message was successfully processed.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlGetAuthenticatedMessage
(
        TSize             xCipheredMessageSize,
  const TUnsignedInt8*   pxCipheredMessage,
        TSize*           pxAuthenticatedDataSize,
  const TUnsignedInt8** ppxAuthenticatedData
);



/**
 *  @ingroup groupODM
 *  @brief
 *    This function enables the STB application to request the creation of 
 *    an Opaque Data. This function supplies an Opaque Data Message Handle.
 * 
 *    The Application can use Opaque Data setters to add fields in the Opaque Data.
 *    The Opaque Data can then be used to convey information needed by the Head-End
 *    in order to generate an entitlement. 
 *
 *    The Application can retrieve the Opaque Data buffer thanks to dvlOpaqueDataMessageGet().
 *
 *    When the Application does not need the Opaque Data object anymore, it shall use the 
 *    dvlOpaqueDataMessageRelease() function to free resources allocated by DVL.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  
 *  @param[out]	ppxOpaqueDataMessageHandle	
 *             Pointer where the DVR/VOD Library will store the Opaque Data Message Handle. 
 *             The input pointer shall not be NULL.
 *
 *  @retval DVL_NO_ERROR
 *            The function was successfully processed.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlOpaqueDataMessageCreate
(
        TOdmHandle**     ppxOpaqueDataMessageHandle
);


/**
 *  @ingroup groupODM
 *  @brief
 *    This function allows to release a OpaqueDataMessage Handle when it is no
 *    more used by Application.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in]  pxOpaqueDataMessageHandle	
 *             Pointer to the OpaqueDataMessage Handle previously created with dvlOpaqueDataMessageCreate().
 *
 *  @retval DVL_NO_ERROR
 *            The message was successfully processed.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlOpaqueDataMessageRelease
(
          TOdmHandle*     pxOpaqueDataMessageHandle  
);

/**
 *  @ingroup groupODM
 *  @brief
 *    This function adds/updates Application Data inside the OpaqueDataMessage.
 *    The function dvlOpaqueDataMessageGet() allow to retrieve the 
 *    corresponding OpaqueDataMessage buffer.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in]  pxOpaqueDataMessageHandle	
 *             Pointer to the OpaqueDataMessage Handle previously created with dvlOpaqueDataMessageCreate().
 *  @param[in] xApplicationDataSize	
 *             Length of the Application Data          
 *  @param[in]	pxApplicationData	
 *             Pointer to the Application Data buffer. Note that Application Data shall be a
 *             base64 URL safe encoded string. 
 *
 *  @retval DVL_NO_ERROR
 *            The message was successfully processed.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlOpaqueDataMessageSetApplicationData
(
        TOdmHandle*     pxOpaqueDataMessageHandle,
        TSize	           xApplicationDataSize,
  const TUnsignedInt8* 	pxApplicationData
);


/**
 *  @ingroup groupODM
 *  @brief
 *    This function adds/updates PRM Signalization Data inside the Opaque Data Message.
 *    The function dvlOpaqueDataMessageGet() enable to retrieve the 
 *    corresponding Opaque Data Message buffer.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in]  pxOpaqueDataMessageHandle	
 *             Pointer to the Opaque Data Message Handle previously created with dvlOpaqueDataMessageCreate().
 *  @param[in] xPrmSignalizationSize	
 *             Length of the PRM Signalization Data   
 *  @param[in]	pxPrmSignalization	
 *             Pointer to the PRM Signalization Data buffer. 
 *             Note that this buffer shall be a base64 URL safe encoded string.
 *              
 *
 *  @retval DVL_NO_ERROR
 *            The message was successfully processed.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlOpaqueDataMessageSetPrmSignalization
(
        TOdmHandle*     pxOpaqueDataMessageHandle,
        TSize	           xPrmSignalizationSize,
  const TUnsignedInt8* 	pxPrmSignalization
);

/**
 *  @ingroup groupODM
 *  @brief
 *    This function supplies the address of the buffer containing the Opaque Data Message (base64).
 *    The DVR/VOD Library processes the message in the function itself. 
 *    That means the STB application can release the buffer once the function returns.
 *    The buffer pointed by pxOpaqueDataMessage shall remain valid during the 
 *    whole function processing. Once the function returns, the STB application 
 *    can release the pointed memory buffer.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in]  pxOpaqueDataMessageHandle	
 *             Pointer to the Opaque Data Message Handle previously created with dvlOpaqueDataMessageCreate().
 *  @param[out] pxOpaqueDataMessageSize	
 *             Pointer to the length of the output Message. This size is set to zero 
 *             by the DVR/VOD Library in case the returned status is not equal to DVL_NO_ERROR. 
 *  @param[out]	ppxOpaqueDataMessage	
 *             This pointer will contain the address of the memory where the 
 *             output Credentials will be stored if the function returns DVL_NO_ERROR. 
 *             In case the function returns any other status, the returned address 
 *             will be set to NULL. 
 *             The memory pointed by *ppxOpaqueDataMessage shall be released by the STB 
 *             application by calling the dedicated function dvlReleaseBuffer(). 
 *
 *  @retval DVL_NO_ERROR
 *            The message was successfully processed.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlOpaqueDataMessageGet
(
        TOdmHandle*      pxOpaqueDataMessageHandle,
        TSize* 	         pxOpaqueDataMessageSize,
  const TUnsignedInt8** ppxOpaqueDataMessage
);



/**
 *  @ingroup groupMemory
 *  @brief
 *    This function allows the DVR/VOD Library client application to release 
 *    an output buffer returned by the DVL. It shall be called on each output 
 *    buffer returned by the DVL.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] pxBuffer
 *            Pointer to the buffer to be released.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            The input argument of the function is invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlReleaseBuffer
(
  const TUnsignedInt8*     pxBuffer
);


/**
 *  @ingroup groupMetadata
 *  @brief
 *    This function allows the DVR/VOD Library client to replace the specific 
 *    metadata stored inside an LCM credential. The metadata can be read back 
 *    by using \c dvlCredentialsInformationRequest(). The update credential 
 *    returned by this function has to be released by calling 
 *    \c dvlReleaseBuffer().
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xInLcmCredentialsSize
 *            Size in Bytes of the buffer containing the LCM Credentials to 
 *            modify.
 *  @param[in] pxInLcmCredentials
 *            Pointer to the buffer containing the LCM Credentials to modified.
 *  @param[in] xSpecificMetadataSize
 *            Size of the specific metadata. The maximum size supported is 
 *            63 bytes.
 *  @param[in] pxSpecificMetadata
 *            Pointer on the specific metadata.
 *  @param[out] pxOutLcmCredentialsSize
 *            Returned size in Bytes of the buffer containing the updated 
 *            LCM Credentials.
 *  @param[out] ppxOutLcmCredentials
 *            Returned pointer to the buffer containing the updated 
 *            LCM Credentials.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            The input argument of the function is invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlUpdateSpecificLcmMetadata
(
        TSize                 xInLcmCredentialsSize,
  const TUnsignedInt8*       pxInLcmCredentials,
        TSize                 xSpecificMetadataSize,
  const TUnsignedInt8*       pxSpecificMetadata,
        TSize*               pxOutLcmCredentialsSize,
  const TUnsignedInt8**     ppxOutLcmCredentials
);

/**
 *  @ingroup groupMetadata
 *  @brief
 *    This function allows the DVR/VOD Library client to replace the generic 
 *    metadata stored inside an LCM credential. The metadata can be read back 
 *    by using \c dvlCredentialsInformationRequest(). The update credential returned
 *    by this function has to be released by calling \c dvlReleaseBuffer().
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xInLcmCredentialsSize
 *            Size in Bytes of the buffer containing the LCM Credentials to 
 *            modify.
 *  @param[in] pxInLcmCredentials
 *            Pointer to the buffer containing the LCM Credentials to modified.
 *  @param[in] xGenericMetadataSize
 *            Size of the generic metadata. The maximum size supported is 
 *            63 bytes.
 *  @param[in] pxGenericMetadata
 *            Pointer on the generic metadata.
 *  @param[out] pxOutLcmCredentialsSize
 *            Returned size in Bytes of the buffer containing the updated 
 *            LCM Credentials.
 *  @param[out] ppxOutLcmCredentials
 *            Returned pointer to the buffer containing the updated 
 *            LCM Credentials.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            The input argument of the function is invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlUpdateGenericLcmMetadata
(
        TSize                 xInLcmCredentialsSize,
  const TUnsignedInt8*       pxInLcmCredentials,
        TSize                 xGenericMetadataSize,
  const TUnsignedInt8*       pxGenericMetadata,
        TSize*               pxOutLcmCredentialsSize,
  const TUnsignedInt8**     ppxOutLcmCredentials
);

/**
 *  @ingroup groupRecord
 *  @brief
 *    This function creates a legacy LCM. This LCM does not contain any content
 *    key, instead it contains a special marker indicating, when it is played
 *    back by the DVL, that the SCR shall be set in legacy mode. This function
 *    is intended to be called during a legacy DVR migration phase and the
 *    returned LCM is expected to be stored persistently and associated to 
 *    legacy contents. Once this is done, the returned LCM buffer shall be
 *    released with the function \c dvlReleaseBuffer().
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xSpecificMetadataSize
 *            Size of the optional specific metadata to be stored in the output 
 *            LCM. Shall be set to zero in case no specific related 
 *            metadata are passed to the DVR/VOD Library.
 *  @param[in] pxSpecificMetadata
 *            Optional specific metadata to be stored in the output Credentials.
 *            Shall be set to NULL in case no specific metadata are passed to 
 *            the DVR/VOD Library.
 *  @param[out] pxOutputCredentialsSize
 *            Returned length of the output Credentials. This value shall be 
 *            considered valid only if the function returned DVL_NO_ERROR.
 *  @param[out] ppxOutputCredentials
 *            Returned address of the memory where the output Credentials will 
 *            be stored. This value shall be considered valid and released only
 *            if the  function returned DVL_NO_ERROR. To release this buffer, 
 *            the STB application shall call \c dvlReleaseBuffer().
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlCreateLegacyLcm
(
        TSize                 xSpecificMetadataSize,
  const TUnsignedInt8*       pxSpecificMetadata,
        TSize*               pxOutputCredentialsSize,
  const TUnsignedInt8**     ppxOutputCredentials
);

/******************************************************************************/
/*                          DVR/VOD Debug API LIBRARY                         */
/******************************************************************************/
/** @defgroup groupDebug Debugging
 * @{
*/

/**
 *  @brief
 *    DVL Debug module identifiers.
*/
typedef enum
{
  DVL_MOD_INIT = 1,       /**<  1: Initialization                  */
  DVL_MOD_MEM = 3,        /**<  3: Memory Allocator                */
  DVL_MOD_TLS = 5,        /**<  5: Tools (list, queue, ...)        */
  DVL_MOD_TLS_VTT,        /**<  6: Tools (vtt only)                */
  DVL_MOD_CRPT = 33,      /**< 33: Cryptographic algorithms        */
  DVL_MOD_UTC = 57,       /**< 57: UTCASH                          */
  DVL_MOD_DBG = 79,       /**< 79: Debug debugging                 */
  DVL_MOD_CASH = 82,      /**< 82: CA shell                        */
  DVL_MOD_DVL = 85,       /**< 85: DVR/VOD Library                 */
  DVL_MOD_SCE             /**< 86: Steady Clock Engine             */
} TDvlDebugModule;

/**
 * @brief
 *   Flag used to perform a control for every module
 *
 *  @see  TDvlDebugModule
*/
#define DVL_MOD_ALL    0xFFFFFFFF


/**
 * @brief
 *   DVL Verbosity level to control the amount of messages to display.
*/
typedef enum
{
  DVL_VERBOSITY_SILENT,
  /**< no output at all, except DBG_ALWAYS; default value */
  DVL_VERBOSITY_LOW,
  /**< global debug messages */
  DVL_VERBOSITY_MIDDLE,
  /**< global detailed debug messages */
  DVL_VERBOSITY_HIGH,
  /**< internal detailed debug messages */
  DVL_VERBOSITY_MAX
  /**< internal very detailed debug messages */
} TDvlDebugVerbosity;

/**
 * @brief
 *   DVL Debug classes.
*/
typedef TUnsignedInt32 TDvlDebugClass32;
#define DVL_CLASS_ALL  ((TDvlDebugClass32)(-1))
  /**< all classes */

/**
 * @brief
 *   DVL Debug role (functionality)
*/
typedef TUnsignedInt32 TDvlDebugRole;
#define DVL_DEBUG_ROLE_ALL   ((TDvlDebugRole)(-1))
  /**< all roles */

/**
 * @brief
 *   Defines the debug traces outputted by the module xModule.
 * @param   xModule
 *   Concerned module, or \c MOD_ALL to control all modules
 * @param   xVerbosity
 *   Verbosity that this module must use for debug
 *   (from \c VERBOSITY_LOW to \c VERBOSITY_MAX)
 * @param  xClass
 *   Class(es) that this module must output, or \c CLASS_ALL to
 *   output any class
 * @param   xRole
 *   Role(s) of output that this module must output, or
 *   \c DEBUG_ROLE_ALL to output any role
 * @remarks
 * -# Replaces the previous control performed for the concerned module(s).
 *   The previous control may have been hard-coded during the build,
 *   or done through a previous call to a \c dvlDebugControlRole() function.
*/
NAGRA_CA_API void dvlDebugControlRole
(
  TDvlDebugModule     xModule,
  TDvlDebugVerbosity  xVerbosity,
  TDvlDebugClass32    xClass,
  TDvlDebugRole       xRole
);

/** @} 
*/ /* End of groupDebug */

/******************************************************************************/
/*                                                                            */
/*                              OBSOLETE TYPES AND FUNCTIONS                  */
/*                                                                            */
/******************************************************************************/

/** @cond APA_NO_EXPORT */

/**
 * @deprecated
 *   Use dvlDebugControlRole() instead.
 * @brief
 *   Defines the debug traces outputted by the module xModule.
 *   Debug of any role is outputted. Use the function
 *   dvlDebugControlRole() to control also the debug role.
 * @param   xModule
 *   Concerned module, or MOD_ALL to control all modules
 * @param   xVerbosity
 *   Verbosity that this module must use for debug
 *   (from VERBOSITY_LOW to VERBOSITY_MAX)
 * @param  xClass
 *   Class(es) that this module must output, or CLASS_ALL to
 *   output any class
 * @remarks
 * -# Replaces the previous control performed for the concerned module(s).
 *   The previous control may have been hard-coded through dbginit file,
 *   or done through a previous call to a dvlDebugControlXxx() function.
*/
NAGRA_CA_API void dvlDebugControlX
(
  TDvlDebugModule  xModule,
  TDvlDebugVerbosity  xVerbosity,
  TDvlDebugClass32    xClass
);

/**
 * @deprecated
 *   Use dvlReleaseBuffer() instead.
 *  @brief
 *    A call to this function releases the buffer allocated by the DVR/VOD
 *    Library and containing the authenticated data.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param  pxAuthenticatedData
 *            Pointer to the buffer to release, containing the authenticated
 *            data.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            The input argument of the function is invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlReleaseAuthenticatedData
(
  const TUnsignedInt8*     pxAuthenticatedData
);

/**
 * @deprecated
 *   Use dvlReleaseBuffer() instead.
 *  @brief
 *    A call to this function releases the buffer allocated by the DVR/VOD
 *    Library and containing the Credentials metadata related to the Profile and
 *    to the content.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param  pxCredentialsUnion
 *            Pointer to the Credentials union structure to release.
 *  @param  pxCredentialsGenericMetadata
 *            Pointer to the buffer to release, containing the Credentials 
 *            generic metadata.
 *  @param  pxCredentialsSpecificMetadata
 *            Pointer to the buffer to release, containing the Credentials 
 *            specific metadata.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlReleaseCredentialInformation
(
  const TCredentialsUnion* pxCredentialsUnion,
  const TUnsignedInt8*     pxCredentialsGenericMetadata,
  const TUnsignedInt8*     pxCredentialsSpecificMetadata
);

/**
 * @deprecated
 *   Use dvlReleaseBuffer() instead.
 *  @deprecated
 *    This function is obsolete, call dvlReleaseBuffer() instead.
 *  @brief
 *    A call to this function releases the buffer allocated by the DVR/VOD
 *    Library and containing the VOD playback output EMM and metadata. 
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param  pxVodPlaybackOutputEmm
 *            Pointer to the buffer containing the VOD playback output EMM
 *            to release.
 *  @param  pxGenericMetadata
 *            Pointer to the buffer to release, containing the generic metadata.
 *  @param  pxSpecificMetadata
 *            Pointer to the buffer to release, containing the specific metadata.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @see ::dvlStartVodPlayback()
 *  @see ::dvlReleaseBuffer()
 */
NAGRA_CA_API TDvlStatus dvlReleaseVodPlaybackOutputEmm
(
  const TUnsignedInt8* pxVodPlaybackOutputEmm,
  const TUnsignedInt8* pxGenericMetadata,
  const TUnsignedInt8* pxSpecificMetadata
);

/**
 * @deprecated
 *   Use dvlReleaseBuffer() instead.
 *  @brief
 *    A call to this function releases the array allocated by the DVR/VOD
 *    Library containing information about neighbourhood Set-Top Boxes secrets.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param  pxNeighbourhoodInfoItems
 *            Pointer to the buffer containing the array to release.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @see ::dvlHomeNetworkInformationRequest()
*/
NAGRA_CA_API TDvlStatus dvlReleaseNeighbourhoodInfoItems
(
  const TDvlNeighbourhoodInfo*  pxNeighbourhoodInfoItems
);

/**
 * @deprecated
 *   Use dvlReleaseBuffer() instead.
 *  @brief
 *    A call to this function releases the buffer allocated by the DVR/VOD
 *    Library and containing the recording output Credentials.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param  pxRecordOutputCredentials
 *            Pointer to the buffer containing the record output Credentials
 *            to release.
 *  @param  pxProfileMetadata
 *            Pointer to the buffer to release, containing the metadata related
 *            to the Profile.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @see ::dvlStartRecord()
*/
NAGRA_CA_API TDvlStatus dvlReleaseRecordOutputCredentials
(
  const TUnsignedInt8* pxRecordOutputCredentials,
  const TUnsignedInt8* pxProfileMetadata
);


/**
 * @deprecated
 *   Use dvlReleaseBuffer() instead.
 *  @brief
 *    A call to this function releases the buffer allocated by the DVR/VOD
 *    Library and containing the playback output Credentials.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param  pxPlaybackOutputCredentials
 *            Pointer to the buffer containing the playback output Credentials
 *            to release.
 *  @param  pxGenericMetadata
 *            Pointer to the buffer to release, containing the generic metadata
 *  @param  pxSpecificMetadata
 *            Pointer to the buffer to release, containing the specific metadata
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @see ::dvlStartPlayback()
*/
NAGRA_CA_API TDvlStatus dvlReleasePlaybackOutputCredentials
(
  const TUnsignedInt8* pxPlaybackOutputCredentials,
  const TUnsignedInt8* pxGenericMetadata,
  const TUnsignedInt8* pxSpecificMetadata
);

/**
 * @deprecated
 *   This callback was never called by the DVL.
 * @brief
 *    Home Networking Information change callback.
 *
 *    This callback is called by the DVR/VOD Library each time the Home Network
 *    changes.
 *
 *    The Home Networking information changes may be:
 *      - the Set-Top Box belongs to a new Home Network
 *      - the version of the Home Network has changed
 *      - the neighboorhood Set-Top Boxes list has changed
 *
 *  @pre
 *    None.
 *
 *  @post
 *    None.
 *
 *  @remark
 *    -# This function is called in the DVR/VOD Library context. Therefore,
 *       its execution should be as short as possible. It must not block the
 *       calling task.
 *    -# As consequence of previous remark, no other function of this API
 *       shall be called directly inside the notification callback.
 *
*/
typedef void (*TDvlHomeNetworkChangesCB)
(
  void
);

/**
 * @deprecated
 *   This function was never implemented.
 *  @brief
 *    A call to this function registers a callback that the DVR/VOD Library
 *    will call each time the Home Networking information changes.
 *
 *    The Home Networking information changes may be:
 *      - the Set-Top Box belongs to a new Home Network
 *      - the version of the Home Network has changed
 *      - the neighbourhood Set-Top Boxes list has changed
 *
 *    Once the Set-Top Box Application has been notified of change in the
 *    Home Networking, it may call ::dvlHomeNetworkInformationRequest() to
 *    get updated information.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param  xHomeNetworkChangesCB
 *            Function called by the DVR/VOD Library each time the Home
 *            Networking related information changes.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @see ::dvlHomeNetworkInformationRequest()
*/
NAGRA_CA_API TDvlStatus dvlRegisterHomeNetworkChangesNotification
(
  TDvlHomeNetworkChangesCB  xHomeNetworkChangesCB
);

/**
 *  @deprecated
 *  @ingroup groupRecord
 *  @brief
 *    This function sends a record session request to the DVR/VOD Library.
 *    When this function sets \c DVL_RECORD_GRANTED into \c *pxRecordingStatus, 
 *    the corresponding content key has been properly set into the chipset 
 *    stream cipher and the STB application can start storing the content on 
 *    the HDD.<br> 
 *    This function also allows appending the current record to a previous 
 *    recorded content, by passing as argument the Credentials related to 
 *    the previous recorded content to which the current recording shall be 
 *    appended.<br> 
 *    If the append is authorized , the current content will be recorded 
 *    with same content key as the event to which it is appended. The output 
 *    Credentials then apply to both contents, the previously recorded one and 
 *    the currently recorded one; the STB application may then consider the 
 *    two records as only one.<br>
 *    If the append fails (record status \c DVL_RECORD_DENIED_APPENDING_DENIED 
 *    is returned in \c *pxRecordingStatus), no content key is set into the 
 *    stream cipher of the chipset. In that case, the STB application may 
 *    still try a standalone recording (without passing any input Credentials).
 *    If the function returned with the record status DVL_RECORD_GRANTED, the 
 *    STB application shall copy/store the content of the output Credentials 
 *    buffer on the HDD and then release it by calling the function 
 *    \c dvlReleaseBuffer().<br>
 *    Once the function returned with the record status \c DVL_RECORD_GRANTED, 
 *    the DVR/VOD Library may call the passed record error callback at any 
 *    time. The main reason is that the recording may be subject to a time 
 *    limit and the recording is suddenly denied. In that case, the STB 
 *    application shall stop the current recording session. When the function 
 *    returns with error status \c DVL_NO_ERROR, it does not necessarily mean 
 *    the recording is successful. The STB Application still needs to check the 
 *    value set in the record status \c *pxRecordStatus parameter.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xTSid
 *            Identifier of transport stream to be recorded.
 *  @param[in] xProfileSize
 *            Size in Bytes of the input profile. Shall be set to 0 if the 
 *            default hard coded operator profile has to be used.
 *  @param[in] pxProfile
 *            Mandatory Profile containing the recording and usage rules of the 
 *            content. Shall be set to \c NULL if the default hard coded 
 *            operator profile has to be used.
 *  @param[in] xSpecificMetadataSize
 *            Size of the optional specific metadata to be stored in the output 
 *            Credentials. Shall be set to zero in case no specific related 
 *            metadata are passed to the DVR/VOD Library.
 *  @param[in] pxSpecificMetadata
 *            Optional specific metadata to be stored in the output Credentials.
 *            Shall be set to NULL in case no specific metadata are passed to 
 *            the DVR/VOD Library.
 *  @param[in] xCollectionId
 *            Unique identifier of the collection to which the content belongs.
 *            The collection identifier is used for recorded content appending.
 *  @param[in] xInputCredentialsSize
 *            Size of the optional input Credentials.
 *            Shall be set to zero in case no input Credentials is passed to
 *            the DVR/VOD Library.
 *  @param[in] pxInputCredentials
 *            Optional Credentials of the content to which the current recording
 *            should be appended.
 *            Should be set to \c NULL if the current recording is not intended
 *            to be appended to any other content.
 *  @param[in] xRecordErrorCB
 *            Callback used by the DVR/VOD Library to report a recording error.
 *  @param[out] pxOutputCredentialsSize
 *            Pointer to the length of the output Credentials.
 *            This size is set to zero by the DVR/VOD Library in case the
 *            returned status is not equal to \c DVL_RECORD_GRANTED.
 *  @param[out] ppxOutputCredentials
 *            This pointer will contain the address of the memory where the 
 *            output Credentials will be stored if the function returns 
 *            DVL_RECORD_GRANTED. In case the function returns any other 
 *            status, the returned address will be set to \c NULL. The memory 
 *            pointed by \c *ppxOutputCredentials shall be released by the STB 
 *            application by calling the dedicated function 
 *            \c dvlReleaseBuffer().
 *  @param[out] pxRecordingStatus
 *            Pointer to the memory where the DVR/VOD Library will store the
 *            recording status.<br>
 *            If pxRecordingStatus is greater or equal to \c DVL_RECORD_DENIED,
 *            the content key has not been set into the chipset stream cipher 
 *            and the content cannot be recorded.
 *  @param[out] pxGenericMetadataSize
 *            Pointer where the DVR/VOD Library will store the size of the 
 *            generic metadata. The input pointer shall not be \c NULL if 
 *            \c ppxGenericMetadata is not \c NULL.
 *  @param[out] ppxGenericMetadata
 *            Pointer where the DVR/VOD Library will store the pointer to the 
 *            buffer containing the optional generic metadata. This buffer can 
 *            be \c NULL if the caller is not interested in retrieving generic 
 *            metadata.
 *            Once the function returns, \c *ppxGenericMetadata may be NULL if 
 *            the Credentials contain no generic metadata or if the input 
 *            Credentials are not valid (function returns 
 *            \c DVL_ERROR_BAD_PARAMETER).
 *            The buffer pointed by *ppxGenericMetadata shall be released once no
 *            longer used by the STB application, by calling the dedicated 
 *            function \c dvlReleaseBuffer().
 *  @param[out] ppxRecordHandle
 *            Pointer where the DVR/VOD Library will store the recording handle.
 *            The input pointer shall not be \c NULL.
 *            The value set by the DVR/VOD Library is significant only if
 *            \c *pxRecordingStatus is \c DVL_RECORD_GRANTED.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @remark
 *    -# If the function returns \c DVL_NO_ERROR and \c *pxRecordStatus contains
 *      \c DVL_RECORD_GRANTED, the STB application shall copy/store the content
 *      of the output Credentials buffer on the HDD and then release it by 
 *      calling the function \c dvlReleaseBuffer().
 *    -# The appending result depends on various parameters, according to the
 *      operator's wishes. The Profile shall basically have an explicit append
 *      authorization and it may be subject to a certain time span after the
 *      first recording.
*/
NAGRA_CA_API TDvlStatus dvlStartRecord
(
        TTransportSessionId   xTSid,
        TSize                 xProfileSize,
  const TUnsignedInt8*       pxProfile,
        TSize                 xSpecificMetadataSize,
  const TUnsignedInt8*       pxSpecificMetadata,
        TUnsignedInt32        xCollectionId,
        TSize                 xInputCredentialsSize,
  const TUnsignedInt8*       pxInputCredentials,
        TDvlRecordErrorCB     xRecordErrorCB,
        TSize*               pxOutputCredentialsSize,
  const TUnsignedInt8**     ppxOutputCredentials,
        TDvlRecordStatus*    pxRecordingStatus,
        TSize*               pxGenericMetadataSize,
  const TUnsignedInt8**     ppxGenericMetadata,
        TDvlHandle**        ppxRecordHandle
);



/**
 *  @ingroup groupPlayback
 *  @deprecated
 *  @brief
 *    This function sends a playback request to the DVR/VOD library, given one 
 *    input Credentials.<br>
 *    When this function returns with the status \c DVL_ACCESS_GRANTED set in 
 *    \c *pxPlaybackStatus, that means that the corresponding content key has 
 *    been correctly set in the chipset stream cipher and the STB application 
 *    can start playing back the content.<br>
 *    If the function returns with the playback status \c DVL_ACCESS_GRANTED, 
 *    the STB application shall copy/store the content of the output 
 *    Credentials buffer on the HDD and then release it by calling the function 
 *    \c dvlReleaseBuffer(). That output Credentials should replace any one 
 *    already existing for that content.<br>
 *    Once the function returned with the playback status \c DVL_ACCESS_GRANTED,
 *    the DVR/VOD Library may call the passed playback error callback at any 
 *    time. The main reason is that the playback may be subject to a time limit 
 *    and the playback is suddenly denied. In that case, the STB application 
 *    shall stop the current playback session.<br>
 *    When the function returns with error status \c DVL_NO_ERROR, it does not 
 *    necessarily mean the playback is successful. The STB Application still 
 *    needs to check the value set in the playback status \c *pxPlaybackStatus 
 *    parameter.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xTSid
 *            Identifier of transport stream to be played back.
 *  @param[in] xInputCredentialsSize
 *            Size in Bytes of the mandatory input Credentials.
 *  @param[in] pxInputCredentials
 *            Pointer to the input Credentials related to the content
 *            to play back.
 *            The DVR/VOD Library won't modify that input buffer. But the STB
 *            application shall replace on the content storage the input
 *            Credentials by the output Credentials.
 *  @param[in] xPlaybackErrorCB
 *            Callback used by the DVR/VOD Library to report any playback error.
 *  @param[out] pxOutputCredentialsSize
 *            Pointer where the DVR/VOD Library will store the Size in Bytes of
 *            the buffer containing the output Credentials.
 *  @param[out] ppxOutputCredentials
 *            Pointer where the DVR/VOD Library will store the pointer to the
 *            memory that will contain the output Credentials.
 *            The input pointer shall not be \c NULL.
 *            The STB application shall replace the input Credentials on the
 *            content storage by this output Credentials and then release the
 *            buffer by calling \c dvlReleaseBuffer().
 *  @param[out] pxPlaybackStatus
 *            Pointer where the DVR/VOD Library will store the playback status.
 *            If \c pxPlaybackStatus is greater or equal to 
 *            \c DVL_ACCESS_DENIED, the content key has not been set into the 
 *            chipset stream cipher and the content cannot be played back.
 *  @param[out] pxGenericMetadataSize
 *            Pointer where the DVR/VOD Library will store the size of the 
 *            optional generic metadata. The input pointer shall not be \c NULL 
 *            if \c ppxGenericMetadata is not NULL.
 *  @param[out] ppxGenericMetadata
 *            Pointer where the DVR/VOD Library will store the pointer to the 
 *            buffer containing the optional generic metadata. This buffer can 
 *            be NULL if the caller is not interested in retrieving generic 
 *            metadata.
 *            Once the function returns, \c *ppxGenericMetadata may be NULL if
 *            the input Credentials contain no generic metadata or if they are
 *            not valid (function returns \c DVL_ERROR_BAD_PARAMETER).
 *            If returned, the buffer pointed by \c *ppxGenericMetadata shall 
 *            be released once no longer used by the STB application, by 
 *            calling \c ::dvlReleaseBuffer().
 *  @param[out] pxSpecificMetadataSize
 *            Pointer where the DVR/VOD Library will store the size of the 
 *            optional specific metadata. The input pointer shall not be \c NULL 
 *            if \c ppxSpecificMetadata is not \c NULL.
 *  @param[out] ppxSpecificMetadata
 *            Pointer where the DVR/VOD Library will store the pointer to the 
 *            buffer containing the optional specific metadata. This buffer can 
 *            be NULL if the caller is not interested in retrieving specific 
 *            metadata.
 *            Once the function returns, \c *ppxSpecificMetadata may be \c NULL
 *            if the input Credentials contain no specific metadata or if they 
 *            are not valid (function returns \c DVL_ERROR_BAD_PARAMETER).
 *            The buffer pointed by \c *ppxSpecificMetadata shall be released 
 *            once no longer used by the STB application, by calling 
 *            \c ::dvlReleaseBuffer().
 *  @param[out] ppxPlaybackHandle
 *            Pointer where the DVR/VOD Library will store the playback session
 *            handler.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @remark
 *    -# If the function returns \c DVL_NO_ERROR and \c *pxPlaybackStatus 
 *    contains \c DVL_ACCESS_GRANTED, the STB application shall copy/store 
 *    the content of the output Credentials buffer on the HDD and then release 
 *    it by calling the function \c dvlReleaseBuffer().
*/
NAGRA_CA_API TDvlStatus dvlStartPlayback
(
        TTransportSessionId   xTSid,
        TSize                 xInputCredentialsSize,
  const TUnsignedInt8*       pxInputCredentials,
        TDvlPlaybackErrorCB   xPlaybackErrorCB,
        TSize*               pxOutputCredentialsSize,
  const TUnsignedInt8**     ppxOutputCredentials,
        TDvlAccessStatus*    pxPlaybackStatus,
        TSize*               pxGenericMetadataSize,
  const TUnsignedInt8**     ppxGenericMetadata,
        TSize*               pxSpecificMetadataSize,
  const TUnsignedInt8**     ppxSpecificMetadata,
        TDvlHandle**        ppxPlaybackHandle
);

/**
 *  @ingroup groupPlayback
 *  @deprecated
 *  @brief
 *    This function is responsible for stopping the playback session, which
 *    consists in the cleanup of the session's resources and
 *    the removal of the content key from the chipset stream cipher.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *    A valid playback session must be running, handled by the parameter
 *    pxPlaybackHandle.
 *
 *  @post
 *    None
 *
 *  @param[in] pxPlaybackHandle
 *            Handle of the playback session to stop.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            The playback handle is invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlStopPlayback
(
  TDvlHandle*  pxPlaybackHandle
);


/**
 *  @ingroup groupPlayback
 *  @deprecated
 *  @brief
 *    This function sends a VOD playback request to the DVR/VOD library, given 
 *    two input Credentials, an EMM and an ECM.<br>
 *    When this function returns with the status \c DVL_ACCESS_GRANTED set in 
 *    \c *pxPlaybackStatus, that means that the corresponding content key has 
 *    been correctly set in the chipset stream cipher and the STB application 
 *    can start playing back the content.<br>
 *    If the function returns with the playback status \c DVL_ACCESS_GRANTED, 
 *    the STB application shall copy/store the content of the output 
 *    Credentials buffer containing the EMM on the HDD and then release it by 
 *    calling the function \c dvlReleaseBuffer(). That output EMM shall replace 
 *    any one already existing for that content.<br>
 *    Once the function returned with the playback status \c DVL_ACCESS_GRANTED,
 *    the DVR/VOD Library may call the passed playback error callback at any 
 *    time. The main reason is that the playback may be subject to a time limit 
 *    and the playback is suddenly denied. In that case, the STB application 
 *    shall stop the current VOD playback session.<br>
 *    When the function returns with error status \c DVL_NO_ERROR, it does not 
 *    necessarily mean the playback is successful. The STB Application still 
 *    needs to check the value set in the playback status \c *pxPlaybackStatus 
 *    parameter.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *
 *  @post
 *    None
 *
 *  @param[in] xTSid
 *            Identifier of transport stream to be played back.
 *  @param[in] xInputEmmSize
 *            Size in Bytes of the mandatory input EMM.
 *  @param[in] pxInputEmm
 *            Pointer to the input EMM related to the VOD content to play back.
 *            The DVR/VOD Library won't modify that input buffer. But the STB
 *            application shall replace on the content storage the input
 *            EMM by the output EMM contained in \c *ppxOutputEmm.
 *  @param[in] xEcmSize
 *            Size in Bytes of the mandatory ECM.
 *  @param[in] pxEcm
 *            Pointer to the ECM related to the VOD content to play back.
 *            The DVR/VOD Library won't modify that input buffer.
 *  @param[in] xPlaybackErrorCB
 *            Callback used by the DVR/VOD Library to report any playback error.
 *  @param[out] pxOutputEmmSize
 *            Pointer where the DVR/VOD Library will store the Size in Bytes of
 *            the buffer containing the output EMM.
 *  @param[out] ppxOutputEmm
 *            Pointer where the DVR/VOD Library will store the pointer to the
 *            memory that will contain the output EMM.
 *            The input pointer shall not be \c NULL.
 *            The STB application shall replace the input EMM on the
 *            content storage by this output EMM and then release the
 *            buffer by calling \c dvlReleaseBuffer().
 *  @param[out] pxVodPlaybackStatus
 *            Pointer where the DVR/VOD Library will store the playback status.
 *            If \c pxPlaybackStatus is lower than \c DVL_ACCESS_DENIED, the 
 *            content key has been correctly set into the chipset stream cipher 
 *            and the stream can be played back.
 *            If \c pxPlaybackStatus is greater or equal to \c DVL_ACCESS_DENIED, 
 *            the content key has not been set into the chipset stream cipher 
 *            and the content cannot be played back.
 *  @param[out] pxGenericMetadataSize
 *            Pointer where the DVR/VOD Library will store the size of the 
 *            optional generic metadata. The input pointer shall not be \c NULL
 *            if \c ppxGenericMetadata is not \c NULL.
 *  @param[out] ppxGenericMetadata
 *            Pointer where the DVR/VOD Library will store the pointer to the 
 *            buffer containing the optional generic metadata. This buffer can 
 *            be \c NULL if the caller is not interested in retrieving generic 
 *            metadata.
 *            Once the function returns, \c *ppxGenericMetadata may be \c NULL 
 *            if the input Credentials (ECM or EMM) contain no generic metadata 
 *            or if they are not valid (function returns 
 *            \c DVL_ERROR_BAD_PARAMETER).<br> 
 *            The buffer pointed by \c *ppxGenericMetadata shall be released 
 *            once no longer used by the STB application, by calling 
 *            \c dvlReleaseBuffer().
 *  @param[out] pxSpecificMetadataSize
 *            Pointer where the DVR/VOD Library will store the size of the 
 *            optional specific metadata. The input pointer shall not be 
 *            \c NULL if \c ppxSpecificMetadata is not \c NULL.
 *  @param[out] ppxSpecificMetadata
 *            Pointer where the DVR/VOD Library will store the pointer to the 
 *            buffer containing the optional specific metadata. This buffer can 
 *            be \c NULL if the caller is not interested in retrieving specific 
 *            metadata.<br>
 *            Once the function returns, \c *ppxSpecificMetadata may be \c NULL 
 *            if the input Credentials (ECM or EMM) contain no specific 
 *            metadata or if the input Credentials are not valid (function 
 *            returns \c DVL_ERROR_BAD_PARAMETER).
 *            The buffer pointed by \c *ppxSpecificMetadata shall be released 
 *            once no more used by the STB application, by calling the 
 *            dedicated function \c dvlReleaseBuffer().
 *  @param[out] ppxVodPlaybackHandle
 *            Pointer where the DVR/VOD Library will store the VOD playback
 *            session handler.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            One or more arguments of the function are invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
 *
 *  @remark
 *    -# If the function returns \c DVL_NO_ERROR and \c *pxPlaybackStatus 
 *    contains \c DVL_ACCESS_GRANTED, the STB application shall copy/store the 
 *    content of the output Credentials buffer on the HDD and then release it 
 *    by calling the function \c dvlReleaseBuffer().
*/
NAGRA_CA_API TDvlStatus dvlStartVodPlayback
(
        TTransportSessionId   xTSid,
        TSize                 xInputEmmSize,
  const TUnsignedInt8*       pxInputEmm,
        TSize                 xEcmSize,
  const TUnsignedInt8*       pxEcm,
        TDvlPlaybackErrorCB   xPlaybackErrorCB,
        TSize*               pxOutputEmmSize,
  const TUnsignedInt8**     ppxOutputEmm,
        TDvlAccessStatus*    pxVodPlaybackStatus,
        TSize*               pxGenericMetadataSize,
  const TUnsignedInt8**     ppxGenericMetadata,
        TSize*               pxSpecificMetadataSize,
  const TUnsignedInt8**     ppxSpecificMetadata,
        TDvlHandle**        ppxVodPlaybackHandle
);

/**
 *  @deprecated
 *  Use dvlStopPlayback() instead.
 *  @ingroup groupPlayback
 *  @brief
 *    This function is responsible for stopping the VOD playback session, which
 *    consists in the cleanup of the VOD playback session related resources and
 *    the removal of the VOD content key from the chipset stream cipher.
 *
 *  @pre
 *    The DVR/VOD Library must be running.
 *    A valid VOD playback session must be running, handled by the parameter
 *    pxVodPlaybackHandle.
 *
 *  @post
 *    None
 *
 *  @param[in] pxVodPlaybackHandle
 *            Handle of the playback session to stop.
 *
 *  @retval DVL_NO_ERROR
 *            The function execution was successful.
 *  @retval DVL_ERROR
 *            The function execution failed.
 *  @retval DVL_ERROR_BAD_PARAMETER
 *            The VOD handle is invalid or missing.
 *  @retval DVL_ERROR_DVL_NOT_RUNNING
 *            The function could not be executed as the DVR/VOD Library is not
 *            running.
*/
NAGRA_CA_API TDvlStatus dvlStopVodPlayback
(
  TDvlHandle*  pxVodPlaybackHandle
);

/** @endcond */

#endif /* CA_DVL_H */

/* END OF FILE */
