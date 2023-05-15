/**
 * Created by shlee7 on 5/7/14.
 */

// 7.2.1.1 Constants
export var WIDGET_INSTALLATION_STARTED = 0; // The Widget installation has started
export var WIDGET_INSTALLATION_COMPLETED = 1; // The Widget installation has completed successfully
export var WIDGET_INSTALLATION_FAILED = 2; // The Widget installation has failed
export var WIDGET_UNINSTALLATION_STARTED = 3; // The Widget uninstallation has started
export var WIDGET_UNINSTALLATION_COMPLETED = 4; // The Widget uninstallation has completed successfully
export var WIDGET_UNINSTALLATION_FAILED = 5; // The Widget uninstallation has failed
export var WIDGET_ERROR_STORAGE_AREA_FULL = 10; // The local storage device is full
export var WIDGET_ERROR_DOWNLOAD = 11; // The Widget cannot be downloaded
export var WIDGET_ERROR_INVALID_ZIP_ARCHIVE = 12; // The Widget package is corrupted or is an Invalid Zip Archive(as defined in [Widgets - Packaging])
export var WIDGET_ERROR_INVALID_SIGNATURE = 13; // Widget's Signature Validation failed
export var WIDGET_ERROR_GENERIC = 14; // Other reason
export var WIDGET_ERROR_SIZE_EXCEEDED = 15; // The Widget exceeded the maximum size for a single widget allowed by the platform, as defined in section 9.1.

// 7.3.2.1 Properties
// Configuration.pvrPolicy
export var PVR_POLICY_NOT_DELETED = 0; // Indicates a recording management policy where no recordings are to be deleted.
export var PVR_POLICY_WATCHED_DELETED = 1; // Indicates a recording management policy where only watched recordings MAY be deleted.
export var PVR_POLICY_THRESHOLD_DELETED = 2; // Indicates a recording management policy where only recordings older than the specified threshold(given by the pvrSaveDaysand pvrSaveEpisodesproperties) MAY be deleted.

// 7.3.3.1 Constants

/*
 The OITF is in the off state and no power is
 consumed. This is the case of a power outage or if the
 OITF has the ability to be completely turned off.
 Scheduled recording is not expected to work.
 */
export var POWER_OFF = 0;

/*
 The OITF is in normal working mode with user
 interaction. The DAE applications may render any
 presentation graphically.
 */
export var POWER_ON = 1;

/*
 The OITF is in the lowestpossible power consumption
 state (meeting regulationsand certifications). The
 OITF may support wake-up from a passive standby in
 order, for example, to perform a scheduled recording.
 */
export var POWER_PASSIVE_STANDBY = 2;

/*
 The OITF is in an intermediate power consumption
 state. The output to the display shall be inactive. In
 this state DAE applications may continue to operate.
 */
export var POWER_ACTIVE_STANDBY = 3;

/*
 The OITF is in the lowestpossible power consumption
 state (meeting regulations and certifications). If the
 platform supports hibernate mode then the OITF
 stores all applications in volatile memory to allow for
 quick startup.
 */
export var POWER_PASSIVE_STANDBY_HIBERNATE = 4;

/*
 tvStandard
 */
export var TVSTANDARD_NTSC = 1; // Indicates platform support for the NTSC TV standard.
export var TVSTANDARD_PAL = 2; // Indicates platform support for the PAL TV standard.
export var TVSTANDARD_SECAM = 4; // Indicates platform support for the SECAM TV standard.

/*
 pvrSupport
 */
export var PVR_SUPPORTED = 0;
export var PVR_NOT_SUPPORTED = 1;

/*
 7.4.4.1 Constants
 */
export var DOWNLOAD_COMPLETED = 1; // The download has completed.
export var DOWNLOAD_IN_PROGRESS = 2; // The download is in progress.
export var DOWNLOAD_PAUSED = 4; // The download has been paused(either by an application or automatically by the platform)
export var DOWNLOAD_FAILED = 8; // The download has failed.
export var DOWNLOAD_NOT_STARTED = 16; // The download is queued but has not yet started.
export var DOWNLOAD_STALLED = 32; // The download has stalled due to a transient failure and the Download Manager is attempting to recuperate and re - establish the download.

/*
 reason
 */
export var DOWNLOAD_FAILED_FULL = 0; // The local storage device is full.
export var DOWNLOAD_FAILED_NOT_PURCHASED = 1; // The item cannot be downloaded(e.g.because it has not been purchased).
export var DOWNLOAD_FAILED_NOT_AVAIL = 2; // The item is no longer available for download.
export var DOWNLOAD_FAILED_CHECKSUM = 3; // The item is invalid due to bad checksum or length.
export var DOWNLOAD_FAILED_UNKNOWN = 4; // Other reason.

/*
 Return value of
 Integer setParentalControlStatus( String pcPIN, Boolean enable )
 Integer setParentalControlPIN( String oldPcPIN, String newPcPIN )
 Integer unlockWithParentalControlPIN( String pcPIN, Object target )
 Integer verifyParentalControlPIN( String pcPIN )
 Integer setBlockUnrated( String pcPIN, Boolean block )
 */
export var ERR_PIN_CORRECT = 0; // The PIN is correct.
export var ERR_PIN_INCORRECT = 1; // The PIN is incorrect.
export var ERR_PIN_LOCKED = 2; // PIN entry is locked.
export var ERR_INVALID_OBJECT = 4; // Invalid object

/*
 ParentalRating.labels
 */
export var PARENTAL_ADVISORY_SEXUAL_DIALOG = 1; // Indicates that a contentitem features sexual suggestive dialog.
export var PARENTAL_ADVISORY_STRONG_LANGUAGE = 2; // Indicates that a content item features strong language.
export var PARENTAL_ADVISORY_SEXUAL_SITUATIONS = 4; // Indicates that a contentitem features sexual situations.
export var PARENTAL_ADVISORY_VIOLENCE = 8; // Indicates that a content item features violence.
export var PARENTAL_ADVISORY_FANTASY_VIOLENCE = 16; // Indicates that a content item features fantasy violence.
export var PARENTAL_ADVISORY_DISTURBING_SCENES = 32; // Indicates that a content item features disturbing scenes.
export var PARENTAL_ADVISORY_DISCRIMINATION = 64; // Indicates that a content item features portrayals of discrimination.
export var PARENTAL_ADVISORY_ILLEGAL_DRUG_USE = 128; // Indicates that a contentitem features scenes of illegal drug use.
export var PARENTAL_ADVISORY_PHOTOSENSITIVE_EPILEPSY = 256; // Indicates that a contentitem features strobing that could impact viewers suffering from Photosensitive epilepsy

/*
 7.10.1.1 Methods
 */

export var REC_REPEAT_SUNDAY = 0x01; // (i.e. 00000001)
export var REC_REPEAT_MONDAY = 0x02; // (i.e. 00000010)
export var REC_REPEAT_TUESDAY = 0x04; // (i.e. 00000100)
export var REC_REPEAT_WEDNESDAY = 0x08; // (i.e. 00001000)
export var REC_REPEAT_THURSDAY = 0x10; // (i.e. 00010000)
export var REC_REPEAT_FRIDAY = 0x20; // (i.e. 00100000)
export var REC_REPEAT_SATURDAY = 0x40; // (i.e. 01000000)

/*
 7.10.2.1 Constants
 */

/*
 Used in the programmeIDTypeproperty to indicate that the programme
 is identified by its TV - Anytime CRID(Content Reference Identifier).
 */
export var ID_TVA_CRID = 0;

/*
 Used in the programmeIDTypeproperty to indicate that the programme
 is identified by a DVB URL referencing a DVB - SI event as enabled by
 Section 4.1.3 of[OIPF_META2].Support for this constant is
 OPTIONAL.
 */
export var ID_DVB_EVENT = 1;

/*
 Used in the programmeIDTypeproperty to indicate that the
 Programme object represents a group of programmes identified by a
 TV-Anytime group CRID.
 */
export var ID_TVA_GROUP_CRID = 2;

/*
 7.10.5.1 Constants
 */
/*
 Unrealized: user / application has not
 requested timeshift or recordNow functionality
 for the channel shown.No timeshift or
 recording resources are claimed in this state.
 */
export var RECORDING_UNREALIZED = 0;

/*
 Recording has been newly scheduled.
 RECORDING_DEL_SCHEDULED  The recording has been deleted (for complete
 or in-progress recordings) or removed from
 the schedule (for scheduled recordings).
 */
export var RECORDING_SCHEDULED = 1;

/*
 Recording is about to start.
 */
export var RECORDING_REC_PRESTART = 2;

/*
 Acquiring recording resources (incl.media connection).
 */
export var RECORDING_REC_ACQUIRING_RESOURCES = 3;

/*
 Recording has started.
 */
export var RECORDING_REC_STARTED = 4;

/*
 Recording has been updated.
 */
export var RECORDING_REC_UPDATED = 5;

/*
 Recording has successfully completed.
 */
export var RECORDING_REC_COMPLETED = 6;

/*
 The recording has only partially completed
 due to insufficient storage space, a clash or
 hardware failure.
 There are three possible conditions for this:
 1) The end of the recording is missed.
 2) The start of the recording is missed.
 3) A piece from the centre of the recording is
 missed (e.g.due to the receiver rebooting or
 a transient failure of the network connection).
 */
export var RECORDING_REC_PARTIALLY_COMPLETED = 7;

/*
 Acquiring timeshift resources(incl.media
 connection).
 */
export var RECORDING_TS_ACQUIRING_RESOURCES = 8;

/*
 Timeshift mode has started.
 */
export var RECORDING_TS_STARTED = 9;

/*
 An error has been encountered.Refer to
 detailed error codes for details on the error.
 */
export var RECORDING_ERROR = 10;

/*
 The recording sub - system is unable to record
 due to resource limitations.
 */
export var ERROR_REC_RESOURCE_LIMITATION = 0;

/*
 There is insufficient storage space available.
 (Some of the recording may be available).
 */
export var ERROR_INSUFFICIENT_STORAGE = 1;

/*
 Tuner conflict(e.g.due to conflicting
 scheduled recording).
 */
export var ERROR_TUNER_CONFLICT = 2;

/*
 Recording not allowed due to DRM
 restrictions.
 */
export var ERROR_REC_DRM_RESTRICTION = 3;

/*
 Recording has stopped before completion due
 to unknown(probably hardware) failure.
 */
export var ERROR_REC_UNKNOWN = 4;

/*
 Timeshift not possible due to resource
 limitations.
 */
export var ERROR_TS_RESOURCE_LIMITATION = 5;

/*
 Timeshift not allowed due to DRM restrictions.
 */
export var ERROR_TS_DRM_RESTRICTION = 6;

/*
 Timeshift ended due to unknown failure.
 */
export var ERROR_TS_UNKNOWN = 7;

/*
 showType
 */

export var RECORDING_SHOWTYPE_LIVE = 0; // The show is live.
export var RECORDING_SHOWTYPE_FIRST_RUN = 1; // The show is a first-run show.
export var RECORDING_SHOWTYPE_RERUN = 2; // The show is a rerun.

/*
 audioType
 */
export var AUDIO_TYPE_MONO = 1;
export var AUDIO_TYPE_STEREO = 2;
export var AUDIO_TYPE_MULTI_CHANNEL = 4;

/*
 7.12.1.1 Properties
 function onMetadataUpdate( Integer action, Integer info, Object object)
 */

/*
 A new version of metadata is available (see clause 4.1.2.1.2 of
 [META]) and applications SHOULD discard all references to
 Programme objects immediately and re-acquire them.
 */
export var UPDATE_ACTION_DISCARD_ALL = 1;

/*
 A change to the parental control flags for a content item has
 occurred (e.g. the user has unlocked the parental control features
 of the receiver, allowing a blocked item to be played).
 */
export var UPDATE_ACTION_PARENTAL_FLAG_CHANGED = 2;

/**
 * A flag affecting the filtering criteria of a channel has changed.
 * Applications MAY listen for events with this action code to update
 * lists of favourite channels, for instance.
 */
export var UPDATE_ACTION_FILTER_FLAG_CHANGED = 3;

export var UPDATE_INFO_BLOCKED_CHANNELLIST_CHANGED = 1; // The list of blocked channels has changed.
export var UPDATE_INFO_FAVOURITE_CHANNELLIST_CHANGED = 2; // A list of favourite channels has changed
export var UPDATE_INFO_HIDDEN_CHANNELLIST_CHANGED = 4; // The list of hidden channels has changed.

/**
 * Search has finished.This event SHALL be generated when a
 * search has completed or been aborted.
 */
export var SEARCH_FINISHED = 0;

/*
 * More search results are available.Calling update() on the
 * SearchResultsobject SHALL update the list of results to
 * include the newly - retrieved data.
 */
export var SEARCH_MORE_RESULTS = 1;

/*
 * The data returned by the search is no longer valid, e.g.because
 * of a change in the metadata.Applications that still require the
 * data SHALL repeat the search.
 */
export var SEARCH_NO_LONGER_VALID = 2;

/*
 * 7.12.1.3 Methods
 */
export var SEARCHTARGET_SCHEDULED = 1; //Metadata relating to scheduled content shall be  searched.
export var SEARCHTARGET_COD = 2; // Metadata relating to content on demand shall be searched.

export var COMPARISON_EQUAL = 0; // True if the specified value is equal to the value of the specified field.
export var COMPARISON_NOT_EQUAL = 1; // True if the specified value is not equal to the value of the specified field.
export var COMPARISON_GREATER_THAN = 2; // True if the value of the specified field is greater than the specified value.
export var COMPARISON_GREATER_THAN_OR_EQUAL = 3; // True if the value of the specified field is greater than or equal to the specified value.
export var COMPARISON_LESS_THAN = 4; // True if the value of the specified field is less than the specified value.
export var COMPARISON_LESS_THAN_OR_EQUAL = 5; // True if the value of the specified field is less than or equal to the specified value.
export var COMPARISON_CONTAINS = 6; // True if the string value of the specified field contains the specified value.
export var COMPARISON_FIELD_EXISTS = 7; // True if the specified field exists.

/*
 7.13.1.2 Properties
 */

export var ERROR_CHANNEL_NOT_SUPPORTED_BY_TUNER = 0; // Channel not supported by tuner.
export var ERROR_CANNOT_TUNE_TO_GIVEN_TRANSPORT_STREAM = 1; // Cannot tune to given transport stream(e.g.no signal)
export var ERROR_TUNER_LOCKED_BY_OTHER_OBJECT = 2; // Tuner locked by other object.
export var ERROR_PARENTAL_LOCK_ON_CHANNEL = 3; // Parental lock on channel.
export var ERROR_ENCRYPTED_CHANNEL = 4; // Encrypted channel, key / module missing.
export var ERROR_UNKNOWN_CHANNEL = 5; // Unknown channel(e.g.can��t resolve DVB or ISDB triplet).
export var ERROR_CHANNEL_SWITCH_INTERRUPTED = 6; // Channel switch interrupted(e.g.because another channel switch was activated before the previous one completed).
export var ERROR_CHANNEL_CANNOT_BE_CHANGED_WHILE_RECORDING = 7; // Channel cannot be changed, because it is currently being recorded.
export var ERROR_CANNOT_RESOLVE_URI = 8; // Cannot resolve URI of referenced IP channel.
export var ERROR_INSUFFICIENT_BANDWIDTH = 9; // Insufficient bandwidth.
export var ERROR_NO_CHANNEL_LIST = 10; // Channel cannot be changed by the nextChannel() /prevChannel() methods
export var ERROR_INSUFFICIENT_RESOURCES = 11; // Insufficient resources are available to present the given channel (e.g.a lack of available codec resources).
export var ERROR_CHANNEL_NOT_FOUND = 12; // Specified channel not found in transport stream.
export var ERROR_UNIDENTIFIED = 100; // Unidentified error.

/*
 VideoBroadcast.playState
 */
/*
 Unrealized; the user (or application) has not made a request to start presenting a channel or
 has stopped presenting a channel and released any resources. The content of the
 video/broadcast object SHALL be an opaque black rectangle.
 */
export var PLAYSTATE_UNREALIZED = 0;

/*
 Connecting; the receiver is connecting to the media source in order to begin playback.
 Objects in this state may be buffering data in order to start playback. The content of the
 video/broadcast object SHALL be either the last frame of decoded video (e.g. in the case of
 transient errors or changing channels), or an opaque black rectangle.
 */
export var PLAYSTATE_CONNECTING = 1;

/*
 Presenting; the media is currently being presented to the user. The object is in this state
 regardless of whether the media is playing atnormal speed, paused, or playing in a trick
 mode (e.g. at a speed other than normal speed).
 */
export var PLAYSTATE_PRESENTING = 2;

/*
 Stopped; the terminal is not presenting media, either inside the video/broadcast object or in
 the logical video plane. The logical video plane is disabled. The content of the
 video/broadcast object SHALL be opaque black rectangle. Control of media presentation is
 under the control of the application, as defined in Section 8.4
 */
export var PLAYSTATE_STOPPED = 3;

/*
 VideoBroadcast.timeShiftMode
 */

export var TIMESHIFTMODE_OFF = 0;
export var TIMESHIFTMODE_LOCAL_RESOURCE = 1;
export var TIMESHIFTMODE_NETWORK_RESOURCE = 2;
export var TIMESHIFTMODE_LOCAL_OR_NETWORK_RESOURCE = 3;

/*
 7.13.10.1 Properties
 ChannelConfig.onChannelScan
 type
 */
export var CHANNEL_SCAN_TYPE_STARTED = 0; // A channel scan has started.
export var CHANNEL_SCAN_TYPE_PROGRESSING = 1; // Indicates the current progress of the scan.
export var CHANNEL_SCAN_TYPE_CHANNEL_FOUND = 2; // A new channel has been found.
export var CHANNEL_SCAN_TYPE_TRANSPONDER_FOUND = 3; // A new transponder has been found.
export var CHANNEL_SCAN_TYPE_COMPLETED = 4; // A channel scan has completed.
export var CHANNEL_SCAN_TYPE_ABORTED = 5; // A channel scan has been aborted.

/*
 7.13.12.1 Constants
 */

/*
 Used in the channelTypeproperty to indicate a TV channel.
 */
export var TYPE_TV = 0;

/*
 Used in the channelTypeproperty to indicate a radio channel.
 */
export var TYPE_RADIO = 1;

/*
 Used in the channelTypeproperty to indicate that the type of the channel
 is unknown, or known but not of type TV or radio.
 */
export var TYPE_OTHER = 2;

/*
 Used in the idTypeproperty to indicate an analogue channel identified by
 the property freqand optionally cnior name.
 */
export var ID_ANALOG = 0;

/*
 Used in the idTypeproperty to indicate a DVB - C channel identified by the
 three properties onid, tsid, sid.
 */
export var ID_DVB_C = 10;

/*
 Used in the idTypeproperty to indicate a DVB - S channel identified by the
 three properties onid, tsid, sid.
 */
export var ID_DVB_S = 11;

/*
 Used in the idTypeproperty to indicate a DVB - T channel identified by the
 three properties onid, tsid, sid.
 */
export var ID_DVB_T = 12;

/*
 Used in the idTypeproperty to indicate a channel that is identified through
 its delivery system descriptor as defined by DVB - SI[EN 300 468]section
 6.2.13.
 */
export var ID_DVB_SI_DIRECT = 13;

/*
 Used in the idTypeproperty to indicate a DVB - C or DVB - C2 channel
 identified by the three properties onid, tsid, sid.
 */
export var ID_DVB_C2 = 14;

/*
 Used in the idTypeproperty to indicate a DVB - S or DVB - S2 channel
 identified by the three properties onid, tsid, sid.
 */
export var ID_DVB_S2 = 15;

/*
 Used in the idTypeproperty to indicate a DVB - T or DVB - T2 channel
 identified by the three properties onid, tsid, sid.
 */
export var ID_DVB_T2 = 16;

/*
 Used in the idTypeproperty to indicate an ISDB - C channel identified by
 the three properties: onid, tsid, sid.
 */
export var ID_ISDB_C = 20;

/*
 Used in the idTypeproperty to indicate an ISDB - S channel identified by
 the three properties onid, tsid, sid.
 */
export var ID_ISDB_S = 21;

/*
 Used in the idTypeproperty to indicate an ISDB - T channel identified by
 the three properties onid, tsid, sid.
 */
export var ID_ISDB_T = 22;

/*
 Used in the idTypeproperty to indicate a terrestrial ATSC channel
 identified by the property sourceID.
 */
export var ID_ATSC_T = 30;

/*
 Used in the idTypeproperty to indicate an IP broadcast channel identified
 through SD & S by a DVB textual service identifier specified in the format
 ��ServiceName.DomainName�� as value for property ipBroadcastIDwith
 ServiceNameand DomainNameas defined in[DVB-IPTV].This idType
 SHALL be used to indicate Scheduled content service defined by
 [OIPF_PROT2]
 */
export var ID_IPTV_SDS = 40;

/*
 Used in the idTypeproperty to indicate an IP broadcast channel identified
 by a DVB MCAST URI (i.e.dvb-mcast://) or by a HAS URI (i.e.
 http://), as value for property ipBroadcastID.
 */
export var ID_IPTV_URI = 41;

/*
 7.13.18.1 Properties
 ofdm
 */
export var DVBT_OFDM_MODE_1K = "MODE_1K"; // OFDM mode 1K
export var DVBT_OFDM_MODE_2K = "MODE_2K"; // OFDM mode 2K
export var DVBT_OFDM_MODE_4K = "MODE_4K"; // OFDM mode 4K
export var DVBT_OFDM_MODE_8K = "MODE_8K"; // OFDM mode 8K
export var DVBT_OFDM_MODE_16K = "MODE_16K"; // OFDM mode 16K
export var DVBT_OFDM_MODE_32K = "MODE_32K"; // OFDM mode 32K


export var DVBT_MODULATION_QPSK = 1; // QPSK modulation
export var DVBT_MODULATION_QAM16 = 4; // QAM16 modulation
export var DVBT_MODULATION_QAM32 = 8; // QAM32 modulation
export var DVBT_MODULATION_QAM64 = 16; // QAM64 modulation
export var DVBT_MODULATION_QAM128 = 32; // QAM128 modulation
export var DVBT_MODULATION_QAM256 = 64; // QAM256 modulation


export var DVBT_BANDWIDTH_1_7M = "BAND_1.7MHZ"; // 1.7 MHz bandwidth
export var DVBT_BANDWIDTH_5M = "BAND_5MHz"; // 5 MHz bandwidth
export var DVBT_BANDWIDTH_6M = "BAND_6MHz"; // 6 MHz bandwidth
export var DVBT_BANDWIDTH_7M = "BAND_7MHz"; // 7 MHz bandwidth
export var DVBT_BANDWIDTH_8M = "BAND_8MHz"; // 8 MHz bandwidth
export var DVBT_BANDWIDTH_10M = "BAND_10MHz"; // 10 MHz bandwidth

/*
 7.13.19.1 Properties
 */
export var DVBS_MODULATION_QPSK = 1; // QPSK modulation
export var DVBS_MODULATION_8PSK = 2; // 8PSK modulation
export var DVBS_MODULATION_QAM16 = 4; // QAM16 modulation

export var DVBS_POLARISATION_HORIZONTAL = 1; // Horizontal polarisation
export var DVBS_POLARISATION_VERTICAL = 2; // Vertical polarisation
export var DVBS_POLARISATION_CLOCKWISE = 4; // Right - handed / clockwise circular polarisation
export var DVBS_POLARISATION_COUNTER_CLOCKWISE = 8; // Left - handed / counter - clockwise circular polarization

/*
 7.13.20.1 Properties
 */

export var DVBC_modulation_QAM16 = 4; // QAM16 modulation
export var DVBC_modulation_QAM32 = 8; // QAM32 modulation
export var DVBC_modulation_QAM64 = 16; // QAM64 modulation
export var DVBC_modulation_QAM128 = 32; // QAM128 modulation
export var DVBC_modulation_QAM256 = 64; // QAM256 modulation
export var DVBC_modulation_QAM1024 = 128; // QAM1024 modulation
export var DVBC_modulation_QAM4096 = 256; // QAM4096 modulation

/*
 Programme.showType
 */

/*
 The programme is live; indicated by the presence of a Liveelement
 with a value attribute set to true.
 */
export var PROGRAMME_SHOWTYPE_LIVE = 1;

/*
 The programme is a first - run show; indicated by the presence of a
 FirstShowingelement with a value attribute set to true.
 */
export var PROGRAMME_SHOWTYPE_FIRST = 2;

/*
 The programme is a rerun; indicated by the presence of a Repeat
 element with a value attribute set to true.
 */
export var PROGRAMME_SHOWTYPE_RERUN = 3;

/*
 7.16.5.1.1 Constants
 */

/*
 Represents a video component.This constant is used for all
 video components regardless of encoding.
 */
export var COMPONENT_TYPE_VIDEO = 0;

/*
 Represents an audio component.This constant is used for all
 audio components regardless of encoding.
 */
export var COMPONENT_TYPE_AUDIO = 1;

/*
 Represents a subtitle component.This constant is used for all
 subtitle components regardless of subtitle format.NOTE: A
 subtitle component may also be related to closed captioning as
 part of a video stream.
 */
export var COMPONENT_TYPE_SUBTITLE = 2;

/*
 HUMAX EXTENSION
 */

/*
 physical storage interface
 */
export var EInterfaceUnknown = 0;
export var EInterfaceSATA = 1;
export var EInterfaceUSB = 2;

/*
 physical storage type
 */
export var ETypeUnknown = 0;
export var ETypeInternal = 1;
export var ETypeExternal = 2;

/*
 physical stroage kind
 */
export var EKindUnknown = 0;
export var EKindHDD = 1;
export var EKindUSBMemory = 2;

/*
 physical stroage usage
 */
export var EUsageUnkown = 0;
export var EUsagePVR = 1;
export var EUsageStorage = 2;

/*
 HContentManager
 */
export var E1DEPTH_VIEW = 0;
export var EFOLDER_VIEW = 1;

export var BROWSE_START = 0;
export var BROWSE_ADD = 1;
export var BROWSE_COMPLETE = 2;
export var BROWSE_ERROR = 3;

/*
 RCT
 */
export enum eTermID {
    "RECORD THIS PROGRAMME" = 2,
    "RECORD WHOLE SERIES" = 5
}

export enum eMhegTuneInfo {
    TUNEMODE_NO_IPLATE		= 0x01,	/**< i-plate display 없이 튜닝함. */
    TUNEMODE_PINCODE_ON_TOP	= 0x02,	/**< PIN code는 exit전까지 메뉴 위에 뜸. Exit하면 바닥에 깔림.*/
    TUNEMODE_PINCODE_PASSED	= 0x04,	/**< PIN code는 외부에서 pass 했음.. pin code pass로 간주. */
    TUNEMODE_KEEP_PINPASS	= 0x08,	/**< PIN code pass flag를 현재 session 대로 유지하라. (ex. subfeed끼리 이동... ) */
    TUNEMODE_FORCE_TUNE		= 0x10,	/**< 현재 튜닝중인 서비스 핸들이라도 강제로 재 튜닝 시키도록 한다. */
    TUNEMODE_NDT_TUNE		= 0x20,	/**< MHEG의 Non Destructive Tune을 위한 mode로 이전 Video와 Audio를 끊지 않고 변경될 redkey가 변경할 service handle 만 MHEG manager로 전달하기 위함. */
    TUNEMODE_CA_ALTER_TUNE	= 0x40,	/**< CA 대체 서비스에 의해 튜닝됨.	*/
    TUNEMODE_EWS_TUNE		= 0x80,	/**< EWS에의해 튜닝됨.	*/
    TUNEMODE_MAX,
}