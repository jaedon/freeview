/**
 * Created by shlee7 on 5/7/14.
 */
define(["require", "exports"], function (require, exports) {
    // 7.2.1.1 Constants
    exports.WIDGET_INSTALLATION_STARTED = 0; // The Widget installation has started
    exports.WIDGET_INSTALLATION_COMPLETED = 1; // The Widget installation has completed successfully
    exports.WIDGET_INSTALLATION_FAILED = 2; // The Widget installation has failed
    exports.WIDGET_UNINSTALLATION_STARTED = 3; // The Widget uninstallation has started
    exports.WIDGET_UNINSTALLATION_COMPLETED = 4; // The Widget uninstallation has completed successfully
    exports.WIDGET_UNINSTALLATION_FAILED = 5; // The Widget uninstallation has failed
    exports.WIDGET_ERROR_STORAGE_AREA_FULL = 10; // The local storage device is full
    exports.WIDGET_ERROR_DOWNLOAD = 11; // The Widget cannot be downloaded
    exports.WIDGET_ERROR_INVALID_ZIP_ARCHIVE = 12; // The Widget package is corrupted or is an Invalid Zip Archive(as defined in [Widgets - Packaging])
    exports.WIDGET_ERROR_INVALID_SIGNATURE = 13; // Widget's Signature Validation failed
    exports.WIDGET_ERROR_GENERIC = 14; // Other reason
    exports.WIDGET_ERROR_SIZE_EXCEEDED = 15; // The Widget exceeded the maximum size for a single widget allowed by the platform, as defined in section 9.1.
    // 7.3.2.1 Properties
    // Configuration.pvrPolicy
    exports.PVR_POLICY_NOT_DELETED = 0; // Indicates a recording management policy where no recordings are to be deleted.
    exports.PVR_POLICY_WATCHED_DELETED = 1; // Indicates a recording management policy where only watched recordings MAY be deleted.
    exports.PVR_POLICY_THRESHOLD_DELETED = 2; // Indicates a recording management policy where only recordings older than the specified threshold(given by the pvrSaveDaysand pvrSaveEpisodesproperties) MAY be deleted.
    // 7.3.3.1 Constants
    /*
     The OITF is in the off state and no power is
     consumed. This is the case of a power outage or if the
     OITF has the ability to be completely turned off.
     Scheduled recording is not expected to work.
     */
    exports.POWER_OFF = 0;
    /*
     The OITF is in normal working mode with user
     interaction. The DAE applications may render any
     presentation graphically.
     */
    exports.POWER_ON = 1;
    /*
     The OITF is in the lowestpossible power consumption
     state (meeting regulationsand certifications). The
     OITF may support wake-up from a passive standby in
     order, for example, to perform a scheduled recording.
     */
    exports.POWER_PASSIVE_STANDBY = 2;
    /*
     The OITF is in an intermediate power consumption
     state. The output to the display shall be inactive. In
     this state DAE applications may continue to operate.
     */
    exports.POWER_ACTIVE_STANDBY = 3;
    /*
     The OITF is in the lowestpossible power consumption
     state (meeting regulations and certifications). If the
     platform supports hibernate mode then the OITF
     stores all applications in volatile memory to allow for
     quick startup.
     */
    exports.POWER_PASSIVE_STANDBY_HIBERNATE = 4;
    /*
     tvStandard
     */
    exports.TVSTANDARD_NTSC = 1; // Indicates platform support for the NTSC TV standard.
    exports.TVSTANDARD_PAL = 2; // Indicates platform support for the PAL TV standard.
    exports.TVSTANDARD_SECAM = 4; // Indicates platform support for the SECAM TV standard.
    /*
     pvrSupport
     */
    exports.PVR_SUPPORTED = 0;
    exports.PVR_NOT_SUPPORTED = 1;
    /*
     7.4.4.1 Constants
     */
    exports.DOWNLOAD_COMPLETED = 1; // The download has completed.
    exports.DOWNLOAD_IN_PROGRESS = 2; // The download is in progress.
    exports.DOWNLOAD_PAUSED = 4; // The download has been paused(either by an application or automatically by the platform)
    exports.DOWNLOAD_FAILED = 8; // The download has failed.
    exports.DOWNLOAD_NOT_STARTED = 16; // The download is queued but has not yet started.
    exports.DOWNLOAD_STALLED = 32; // The download has stalled due to a transient failure and the Download Manager is attempting to recuperate and re - establish the download.
    /*
     reason
     */
    exports.DOWNLOAD_FAILED_FULL = 0; // The local storage device is full.
    exports.DOWNLOAD_FAILED_NOT_PURCHASED = 1; // The item cannot be downloaded(e.g.because it has not been purchased).
    exports.DOWNLOAD_FAILED_NOT_AVAIL = 2; // The item is no longer available for download.
    exports.DOWNLOAD_FAILED_CHECKSUM = 3; // The item is invalid due to bad checksum or length.
    exports.DOWNLOAD_FAILED_UNKNOWN = 4; // Other reason.
    /*
     Return value of
     Integer setParentalControlStatus( String pcPIN, Boolean enable )
     Integer setParentalControlPIN( String oldPcPIN, String newPcPIN )
     Integer unlockWithParentalControlPIN( String pcPIN, Object target )
     Integer verifyParentalControlPIN( String pcPIN )
     Integer setBlockUnrated( String pcPIN, Boolean block )
     */
    exports.ERR_PIN_CORRECT = 0; // The PIN is correct.
    exports.ERR_PIN_INCORRECT = 1; // The PIN is incorrect.
    exports.ERR_PIN_LOCKED = 2; // PIN entry is locked.
    exports.ERR_INVALID_OBJECT = 4; // Invalid object
    /*
     ParentalRating.labels
     */
    exports.PARENTAL_ADVISORY_SEXUAL_DIALOG = 1; // Indicates that a contentitem features sexual suggestive dialog.
    exports.PARENTAL_ADVISORY_STRONG_LANGUAGE = 2; // Indicates that a content item features strong language.
    exports.PARENTAL_ADVISORY_SEXUAL_SITUATIONS = 4; // Indicates that a contentitem features sexual situations.
    exports.PARENTAL_ADVISORY_VIOLENCE = 8; // Indicates that a content item features violence.
    exports.PARENTAL_ADVISORY_FANTASY_VIOLENCE = 16; // Indicates that a content item features fantasy violence.
    exports.PARENTAL_ADVISORY_DISTURBING_SCENES = 32; // Indicates that a content item features disturbing scenes.
    exports.PARENTAL_ADVISORY_DISCRIMINATION = 64; // Indicates that a content item features portrayals of discrimination.
    exports.PARENTAL_ADVISORY_ILLEGAL_DRUG_USE = 128; // Indicates that a contentitem features scenes of illegal drug use.
    exports.PARENTAL_ADVISORY_PHOTOSENSITIVE_EPILEPSY = 256; // Indicates that a contentitem features strobing that could impact viewers suffering from Photosensitive epilepsy
    /*
     7.10.1.1 Methods
     */
    exports.REC_REPEAT_SUNDAY = 0x01; // (i.e. 00000001)
    exports.REC_REPEAT_MONDAY = 0x02; // (i.e. 00000010)
    exports.REC_REPEAT_TUESDAY = 0x04; // (i.e. 00000100)
    exports.REC_REPEAT_WEDNESDAY = 0x08; // (i.e. 00001000)
    exports.REC_REPEAT_THURSDAY = 0x10; // (i.e. 00010000)
    exports.REC_REPEAT_FRIDAY = 0x20; // (i.e. 00100000)
    exports.REC_REPEAT_SATURDAY = 0x40; // (i.e. 01000000)
    /*
     7.10.2.1 Constants
     */
    /*
     Used in the programmeIDTypeproperty to indicate that the programme
     is identified by its TV - Anytime CRID(Content Reference Identifier).
     */
    exports.ID_TVA_CRID = 0;
    /*
     Used in the programmeIDTypeproperty to indicate that the programme
     is identified by a DVB URL referencing a DVB - SI event as enabled by
     Section 4.1.3 of[OIPF_META2].Support for this constant is
     OPTIONAL.
     */
    exports.ID_DVB_EVENT = 1;
    /*
     Used in the programmeIDTypeproperty to indicate that the
     Programme object represents a group of programmes identified by a
     TV-Anytime group CRID.
     */
    exports.ID_TVA_GROUP_CRID = 2;
    /*
     7.10.5.1 Constants
     */
    /*
     Unrealized: user / application has not
     requested timeshift or recordNow functionality
     for the channel shown.No timeshift or
     recording resources are claimed in this state.
     */
    exports.RECORDING_UNREALIZED = 0;
    /*
     Recording has been newly scheduled.
     RECORDING_DEL_SCHEDULED  The recording has been deleted (for complete
     or in-progress recordings) or removed from
     the schedule (for scheduled recordings).
     */
    exports.RECORDING_SCHEDULED = 1;
    /*
     Recording is about to start.
     */
    exports.RECORDING_REC_PRESTART = 2;
    /*
     Acquiring recording resources (incl.media connection).
     */
    exports.RECORDING_REC_ACQUIRING_RESOURCES = 3;
    /*
     Recording has started.
     */
    exports.RECORDING_REC_STARTED = 4;
    /*
     Recording has been updated.
     */
    exports.RECORDING_REC_UPDATED = 5;
    /*
     Recording has successfully completed.
     */
    exports.RECORDING_REC_COMPLETED = 6;
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
    exports.RECORDING_REC_PARTIALLY_COMPLETED = 7;
    /*
     Acquiring timeshift resources(incl.media
     connection).
     */
    exports.RECORDING_TS_ACQUIRING_RESOURCES = 8;
    /*
     Timeshift mode has started.
     */
    exports.RECORDING_TS_STARTED = 9;
    /*
     An error has been encountered.Refer to
     detailed error codes for details on the error.
     */
    exports.RECORDING_ERROR = 10;
    /*
     The recording sub - system is unable to record
     due to resource limitations.
     */
    exports.ERROR_REC_RESOURCE_LIMITATION = 0;
    /*
     There is insufficient storage space available.
     (Some of the recording may be available).
     */
    exports.ERROR_INSUFFICIENT_STORAGE = 1;
    /*
     Tuner conflict(e.g.due to conflicting
     scheduled recording).
     */
    exports.ERROR_TUNER_CONFLICT = 2;
    /*
     Recording not allowed due to DRM
     restrictions.
     */
    exports.ERROR_REC_DRM_RESTRICTION = 3;
    /*
     Recording has stopped before completion due
     to unknown(probably hardware) failure.
     */
    exports.ERROR_REC_UNKNOWN = 4;
    /*
     Timeshift not possible due to resource
     limitations.
     */
    exports.ERROR_TS_RESOURCE_LIMITATION = 5;
    /*
     Timeshift not allowed due to DRM restrictions.
     */
    exports.ERROR_TS_DRM_RESTRICTION = 6;
    /*
     Timeshift ended due to unknown failure.
     */
    exports.ERROR_TS_UNKNOWN = 7;
    /*
     showType
     */
    exports.RECORDING_SHOWTYPE_LIVE = 0; // The show is live.
    exports.RECORDING_SHOWTYPE_FIRST_RUN = 1; // The show is a first-run show.
    exports.RECORDING_SHOWTYPE_RERUN = 2; // The show is a rerun.
    /*
     audioType
     */
    exports.AUDIO_TYPE_MONO = 1;
    exports.AUDIO_TYPE_STEREO = 2;
    exports.AUDIO_TYPE_MULTI_CHANNEL = 4;
    /*
     7.12.1.1 Properties
     function onMetadataUpdate( Integer action, Integer info, Object object)
     */
    /*
     A new version of metadata is available (see clause 4.1.2.1.2 of
     [META]) and applications SHOULD discard all references to
     Programme objects immediately and re-acquire them.
     */
    exports.UPDATE_ACTION_DISCARD_ALL = 1;
    /*
     A change to the parental control flags for a content item has
     occurred (e.g. the user has unlocked the parental control features
     of the receiver, allowing a blocked item to be played).
     */
    exports.UPDATE_ACTION_PARENTAL_FLAG_CHANGED = 2;
    /**
     * A flag affecting the filtering criteria of a channel has changed.
     * Applications MAY listen for events with this action code to update
     * lists of favourite channels, for instance.
     */
    exports.UPDATE_ACTION_FILTER_FLAG_CHANGED = 3;
    exports.UPDATE_INFO_BLOCKED_CHANNELLIST_CHANGED = 1; // The list of blocked channels has changed.
    exports.UPDATE_INFO_FAVOURITE_CHANNELLIST_CHANGED = 2; // A list of favourite channels has changed
    exports.UPDATE_INFO_HIDDEN_CHANNELLIST_CHANGED = 4; // The list of hidden channels has changed.
    /**
     * Search has finished.This event SHALL be generated when a
     * search has completed or been aborted.
     */
    exports.SEARCH_FINISHED = 0;
    /*
     * More search results are available.Calling update() on the
     * SearchResultsobject SHALL update the list of results to
     * include the newly - retrieved data.
     */
    exports.SEARCH_MORE_RESULTS = 1;
    /*
     * The data returned by the search is no longer valid, e.g.because
     * of a change in the metadata.Applications that still require the
     * data SHALL repeat the search.
     */
    exports.SEARCH_NO_LONGER_VALID = 2;
    /*
     * 7.12.1.3 Methods
     */
    exports.SEARCHTARGET_SCHEDULED = 1; //Metadata relating to scheduled content shall be  searched.
    exports.SEARCHTARGET_COD = 2; // Metadata relating to content on demand shall be searched.
    exports.COMPARISON_EQUAL = 0; // True if the specified value is equal to the value of the specified field.
    exports.COMPARISON_NOT_EQUAL = 1; // True if the specified value is not equal to the value of the specified field.
    exports.COMPARISON_GREATER_THAN = 2; // True if the value of the specified field is greater than the specified value.
    exports.COMPARISON_GREATER_THAN_OR_EQUAL = 3; // True if the value of the specified field is greater than or equal to the specified value.
    exports.COMPARISON_LESS_THAN = 4; // True if the value of the specified field is less than the specified value.
    exports.COMPARISON_LESS_THAN_OR_EQUAL = 5; // True if the value of the specified field is less than or equal to the specified value.
    exports.COMPARISON_CONTAINS = 6; // True if the string value of the specified field contains the specified value.
    exports.COMPARISON_FIELD_EXISTS = 7; // True if the specified field exists.
    /*
     7.13.1.2 Properties
     */
    exports.ERROR_CHANNEL_NOT_SUPPORTED_BY_TUNER = 0; // Channel not supported by tuner.
    exports.ERROR_CANNOT_TUNE_TO_GIVEN_TRANSPORT_STREAM = 1; // Cannot tune to given transport stream(e.g.no signal)
    exports.ERROR_TUNER_LOCKED_BY_OTHER_OBJECT = 2; // Tuner locked by other object.
    exports.ERROR_PARENTAL_LOCK_ON_CHANNEL = 3; // Parental lock on channel.
    exports.ERROR_ENCRYPTED_CHANNEL = 4; // Encrypted channel, key / module missing.
    exports.ERROR_UNKNOWN_CHANNEL = 5; // Unknown channel(e.g.can��t resolve DVB or ISDB triplet).
    exports.ERROR_CHANNEL_SWITCH_INTERRUPTED = 6; // Channel switch interrupted(e.g.because another channel switch was activated before the previous one completed).
    exports.ERROR_CHANNEL_CANNOT_BE_CHANGED_WHILE_RECORDING = 7; // Channel cannot be changed, because it is currently being recorded.
    exports.ERROR_CANNOT_RESOLVE_URI = 8; // Cannot resolve URI of referenced IP channel.
    exports.ERROR_INSUFFICIENT_BANDWIDTH = 9; // Insufficient bandwidth.
    exports.ERROR_NO_CHANNEL_LIST = 10; // Channel cannot be changed by the nextChannel() /prevChannel() methods
    exports.ERROR_INSUFFICIENT_RESOURCES = 11; // Insufficient resources are available to present the given channel (e.g.a lack of available codec resources).
    exports.ERROR_CHANNEL_NOT_FOUND = 12; // Specified channel not found in transport stream.
    exports.ERROR_UNIDENTIFIED = 100; // Unidentified error.
    /*
     VideoBroadcast.playState
     */
    /*
     Unrealized; the user (or application) has not made a request to start presenting a channel or
     has stopped presenting a channel and released any resources. The content of the
     video/broadcast object SHALL be an opaque black rectangle.
     */
    exports.PLAYSTATE_UNREALIZED = 0;
    /*
     Connecting; the receiver is connecting to the media source in order to begin playback.
     Objects in this state may be buffering data in order to start playback. The content of the
     video/broadcast object SHALL be either the last frame of decoded video (e.g. in the case of
     transient errors or changing channels), or an opaque black rectangle.
     */
    exports.PLAYSTATE_CONNECTING = 1;
    /*
     Presenting; the media is currently being presented to the user. The object is in this state
     regardless of whether the media is playing atnormal speed, paused, or playing in a trick
     mode (e.g. at a speed other than normal speed).
     */
    exports.PLAYSTATE_PRESENTING = 2;
    /*
     Stopped; the terminal is not presenting media, either inside the video/broadcast object or in
     the logical video plane. The logical video plane is disabled. The content of the
     video/broadcast object SHALL be opaque black rectangle. Control of media presentation is
     under the control of the application, as defined in Section 8.4
     */
    exports.PLAYSTATE_STOPPED = 3;
    /*
     VideoBroadcast.timeShiftMode
     */
    exports.TIMESHIFTMODE_OFF = 0;
    exports.TIMESHIFTMODE_LOCAL_RESOURCE = 1;
    exports.TIMESHIFTMODE_NETWORK_RESOURCE = 2;
    exports.TIMESHIFTMODE_LOCAL_OR_NETWORK_RESOURCE = 3;
    /*
     7.13.10.1 Properties
     ChannelConfig.onChannelScan
     type
     */
    exports.CHANNEL_SCAN_TYPE_STARTED = 0; // A channel scan has started.
    exports.CHANNEL_SCAN_TYPE_PROGRESSING = 1; // Indicates the current progress of the scan.
    exports.CHANNEL_SCAN_TYPE_CHANNEL_FOUND = 2; // A new channel has been found.
    exports.CHANNEL_SCAN_TYPE_TRANSPONDER_FOUND = 3; // A new transponder has been found.
    exports.CHANNEL_SCAN_TYPE_COMPLETED = 4; // A channel scan has completed.
    exports.CHANNEL_SCAN_TYPE_ABORTED = 5; // A channel scan has been aborted.
    /*
     7.13.12.1 Constants
     */
    /*
     Used in the channelTypeproperty to indicate a TV channel.
     */
    exports.TYPE_TV = 0;
    /*
     Used in the channelTypeproperty to indicate a radio channel.
     */
    exports.TYPE_RADIO = 1;
    /*
     Used in the channelTypeproperty to indicate that the type of the channel
     is unknown, or known but not of type TV or radio.
     */
    exports.TYPE_OTHER = 2;
    /*
     Used in the idTypeproperty to indicate an analogue channel identified by
     the property freqand optionally cnior name.
     */
    exports.ID_ANALOG = 0;
    /*
     Used in the idTypeproperty to indicate a DVB - C channel identified by the
     three properties onid, tsid, sid.
     */
    exports.ID_DVB_C = 10;
    /*
     Used in the idTypeproperty to indicate a DVB - S channel identified by the
     three properties onid, tsid, sid.
     */
    exports.ID_DVB_S = 11;
    /*
     Used in the idTypeproperty to indicate a DVB - T channel identified by the
     three properties onid, tsid, sid.
     */
    exports.ID_DVB_T = 12;
    /*
     Used in the idTypeproperty to indicate a channel that is identified through
     its delivery system descriptor as defined by DVB - SI[EN 300 468]section
     6.2.13.
     */
    exports.ID_DVB_SI_DIRECT = 13;
    /*
     Used in the idTypeproperty to indicate a DVB - C or DVB - C2 channel
     identified by the three properties onid, tsid, sid.
     */
    exports.ID_DVB_C2 = 14;
    /*
     Used in the idTypeproperty to indicate a DVB - S or DVB - S2 channel
     identified by the three properties onid, tsid, sid.
     */
    exports.ID_DVB_S2 = 15;
    /*
     Used in the idTypeproperty to indicate a DVB - T or DVB - T2 channel
     identified by the three properties onid, tsid, sid.
     */
    exports.ID_DVB_T2 = 16;
    /*
     Used in the idTypeproperty to indicate an ISDB - C channel identified by
     the three properties: onid, tsid, sid.
     */
    exports.ID_ISDB_C = 20;
    /*
     Used in the idTypeproperty to indicate an ISDB - S channel identified by
     the three properties onid, tsid, sid.
     */
    exports.ID_ISDB_S = 21;
    /*
     Used in the idTypeproperty to indicate an ISDB - T channel identified by
     the three properties onid, tsid, sid.
     */
    exports.ID_ISDB_T = 22;
    /*
     Used in the idTypeproperty to indicate a terrestrial ATSC channel
     identified by the property sourceID.
     */
    exports.ID_ATSC_T = 30;
    /*
     Used in the idTypeproperty to indicate an IP broadcast channel identified
     through SD & S by a DVB textual service identifier specified in the format
     ��ServiceName.DomainName�� as value for property ipBroadcastIDwith
     ServiceNameand DomainNameas defined in[DVB-IPTV].This idType
     SHALL be used to indicate Scheduled content service defined by
     [OIPF_PROT2]
     */
    exports.ID_IPTV_SDS = 40;
    /*
     Used in the idTypeproperty to indicate an IP broadcast channel identified
     by a DVB MCAST URI (i.e.dvb-mcast://) or by a HAS URI (i.e.
     http://), as value for property ipBroadcastID.
     */
    exports.ID_IPTV_URI = 41;
    /*
     7.13.18.1 Properties
     ofdm
     */
    exports.DVBT_OFDM_MODE_1K = "MODE_1K"; // OFDM mode 1K
    exports.DVBT_OFDM_MODE_2K = "MODE_2K"; // OFDM mode 2K
    exports.DVBT_OFDM_MODE_4K = "MODE_4K"; // OFDM mode 4K
    exports.DVBT_OFDM_MODE_8K = "MODE_8K"; // OFDM mode 8K
    exports.DVBT_OFDM_MODE_16K = "MODE_16K"; // OFDM mode 16K
    exports.DVBT_OFDM_MODE_32K = "MODE_32K"; // OFDM mode 32K
    exports.DVBT_MODULATION_QPSK = 1; // QPSK modulation
    exports.DVBT_MODULATION_QAM16 = 4; // QAM16 modulation
    exports.DVBT_MODULATION_QAM32 = 8; // QAM32 modulation
    exports.DVBT_MODULATION_QAM64 = 16; // QAM64 modulation
    exports.DVBT_MODULATION_QAM128 = 32; // QAM128 modulation
    exports.DVBT_MODULATION_QAM256 = 64; // QAM256 modulation
    exports.DVBT_BANDWIDTH_1_7M = "BAND_1.7MHZ"; // 1.7 MHz bandwidth
    exports.DVBT_BANDWIDTH_5M = "BAND_5MHz"; // 5 MHz bandwidth
    exports.DVBT_BANDWIDTH_6M = "BAND_6MHz"; // 6 MHz bandwidth
    exports.DVBT_BANDWIDTH_7M = "BAND_7MHz"; // 7 MHz bandwidth
    exports.DVBT_BANDWIDTH_8M = "BAND_8MHz"; // 8 MHz bandwidth
    exports.DVBT_BANDWIDTH_10M = "BAND_10MHz"; // 10 MHz bandwidth
    /*
     7.13.19.1 Properties
     */
    exports.DVBS_MODULATION_QPSK = 1; // QPSK modulation
    exports.DVBS_MODULATION_8PSK = 2; // 8PSK modulation
    exports.DVBS_MODULATION_QAM16 = 4; // QAM16 modulation
    exports.DVBS_POLARISATION_HORIZONTAL = 1; // Horizontal polarisation
    exports.DVBS_POLARISATION_VERTICAL = 2; // Vertical polarisation
    exports.DVBS_POLARISATION_CLOCKWISE = 4; // Right - handed / clockwise circular polarisation
    exports.DVBS_POLARISATION_COUNTER_CLOCKWISE = 8; // Left - handed / counter - clockwise circular polarization
    /*
     7.13.20.1 Properties
     */
    exports.DVBC_modulation_QAM16 = 4; // QAM16 modulation
    exports.DVBC_modulation_QAM32 = 8; // QAM32 modulation
    exports.DVBC_modulation_QAM64 = 16; // QAM64 modulation
    exports.DVBC_modulation_QAM128 = 32; // QAM128 modulation
    exports.DVBC_modulation_QAM256 = 64; // QAM256 modulation
    exports.DVBC_modulation_QAM1024 = 128; // QAM1024 modulation
    exports.DVBC_modulation_QAM4096 = 256; // QAM4096 modulation
    /*
     Programme.showType
     */
    /*
     The programme is live; indicated by the presence of a Liveelement
     with a value attribute set to true.
     */
    exports.PROGRAMME_SHOWTYPE_LIVE = 1;
    /*
     The programme is a first - run show; indicated by the presence of a
     FirstShowingelement with a value attribute set to true.
     */
    exports.PROGRAMME_SHOWTYPE_FIRST = 2;
    /*
     The programme is a rerun; indicated by the presence of a Repeat
     element with a value attribute set to true.
     */
    exports.PROGRAMME_SHOWTYPE_RERUN = 3;
    /*
     7.16.5.1.1 Constants
     */
    /*
     Represents a video component.This constant is used for all
     video components regardless of encoding.
     */
    exports.COMPONENT_TYPE_VIDEO = 0;
    /*
     Represents an audio component.This constant is used for all
     audio components regardless of encoding.
     */
    exports.COMPONENT_TYPE_AUDIO = 1;
    /*
     Represents a subtitle component.This constant is used for all
     subtitle components regardless of subtitle format.NOTE: A
     subtitle component may also be related to closed captioning as
     part of a video stream.
     */
    exports.COMPONENT_TYPE_SUBTITLE = 2;
    /*
     HUMAX EXTENSION
     */
    /*
     physical storage interface
     */
    exports.EInterfaceUnknown = 0;
    exports.EInterfaceSATA = 1;
    exports.EInterfaceUSB = 2;
    /*
     physical storage type
     */
    exports.ETypeUnknown = 0;
    exports.ETypeInternal = 1;
    exports.ETypeExternal = 2;
    /*
     physical stroage kind
     */
    exports.EKindUnknown = 0;
    exports.EKindHDD = 1;
    exports.EKindUSBMemory = 2;
    /*
     physical stroage usage
     */
    exports.EUsageUnkown = 0;
    exports.EUsagePVR = 1;
    exports.EUsageStorage = 2;
    /*
     HContentManager
     */
    exports.E1DEPTH_VIEW = 0;
    exports.EFOLDER_VIEW = 1;
    exports.BROWSE_START = 0;
    exports.BROWSE_ADD = 1;
    exports.BROWSE_COMPLETE = 2;
    exports.BROWSE_ERROR = 3;
    /*
     RCT
     */
    (function (eTermID) {
        eTermID[eTermID["RECORD THIS PROGRAMME"] = 2] = "RECORD THIS PROGRAMME";
        eTermID[eTermID["RECORD WHOLE SERIES"] = 5] = "RECORD WHOLE SERIES";
    })(exports.eTermID || (exports.eTermID = {}));
    var eTermID = exports.eTermID;
    (function (eMhegTuneInfo) {
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_NO_IPLATE"] = 0x01] = "TUNEMODE_NO_IPLATE";
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_PINCODE_ON_TOP"] = 0x02] = "TUNEMODE_PINCODE_ON_TOP";
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_PINCODE_PASSED"] = 0x04] = "TUNEMODE_PINCODE_PASSED";
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_KEEP_PINPASS"] = 0x08] = "TUNEMODE_KEEP_PINPASS";
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_FORCE_TUNE"] = 0x10] = "TUNEMODE_FORCE_TUNE";
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_NDT_TUNE"] = 0x20] = "TUNEMODE_NDT_TUNE";
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_CA_ALTER_TUNE"] = 0x40] = "TUNEMODE_CA_ALTER_TUNE";
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_EWS_TUNE"] = 0x80] = "TUNEMODE_EWS_TUNE";
        eMhegTuneInfo[eMhegTuneInfo["TUNEMODE_MAX"] = 129] = "TUNEMODE_MAX";
    })(exports.eMhegTuneInfo || (exports.eMhegTuneInfo = {}));
    var eMhegTuneInfo = exports.eMhegTuneInfo;
});
