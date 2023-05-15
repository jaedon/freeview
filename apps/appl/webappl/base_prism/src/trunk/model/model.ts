/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 4. 2..
 */
declare var hx: ifSugar;

module mPrismModel {
    export enum EHumaxRecommendationType {
        ENone = 0,
        EPeopleLikeThese,
        EPopularThisWeek,
        EDontMissIt,
        EPopularTvSeries
    }
    export enum EHomeDataType {
        EContent = 1,
        EApp,
        EMoreContent
    }
    export enum EImageVariant {
        EColour_16_9  = 1,
        EColour_1_1,
        EWhite_16_9,
        EWhite_1_1
    }
    export enum eTChannelType {
        ETV = 0,
        ERADIO = 1,
        EOTHER = 2
    }
    export enum eTRecordingShowType {
        ELIVE = 0,
        EFIRST_RUN = 1,
        ERERUN = 2
    }
    export enum eTScheduledFactoryType {
        ERECORDING = 0,
        EREMIND = 1,
        EHTTP_LIVE_STREAMING = 4,
        EPOWER_ON_TIMER = 6,
        EREGULAR_OTA = 7,
        ESATRECORD = 8,
        ESCHEDULED_RECORDING_AOS_NOTIFY = 9,
        ESERIES_RECORDING = 10,
        EALTERNATIVE_RECORDING = 11,
        EALTERNATIVE_SERIES_RECORDING = 12
    }
    export enum eTStorageType {
        EMANUAL = 0,
        EINTHDD = 1,
        EINTSUBHDD = 2,
        EEXTHDD = 3,
        ERECSTORAGE = 5
    }
    export enum eTRecordingState {
        ERECORDING_UNREALIZED = 0,
        ERECORDING_SCHEDULED = 1,
        ERECORDING_DEL_SCHEDULED = 2,
        ERECORDING_REC_PRESTART = 3,
        ERECORDING_REC_ACQUIRING_RESOURCES = 4,
        ERECORDING_REC_STARTED = 5,
        ERECORDING_REC_UPDATED = 6,
        ERECORDING_REC_COMPLETED = 7,
        ERECORDING_REC_PARTIALLY_COMPLETED = 8,
        ERECORDING_TS_ACQUIRING_RESOURCES = 9,
        ERECORDING_TS_STARTED = 10,
        ERECORDING_ERROR = 11,
        ERECORDING_REC_START_ERROR = 12,
        ERECORDING_REC_START_CONFLICT = 13,
        ERECORDING_REC_LOAD = 14
    }
    export enum eTRecordingError {
        ERROR_REC_RESOURCE_LIMITATION = 0,
        ERROR_INSUFFICIENT_STORAGE = 1,
        ERROR_TUNER_CONFLICT = 2,
        ERROR_REC_DRM_RESTRICTION = 3,
        ERROR_REC_UNKNOWN = 4,
        ERROR_TS_RESOURCE_LIMITATION = 5,
        ERROR_TS_DRM_RESTRICTION = 6,
        ERROR_HAVE_ALTERNATIVE = 7,
        ERROR_TS_UNKNOWN = 8,
        ERROR_RFOTA = 9,
        ERROR_NO_AV = 10,
        ERROR_POWER_OFF = 11,
        ERROR_REC_CAS = 12,
        ERROR_HIGHER_PRIORITY = 13,
        ERROR_TRACKING_FAIL = 14,
        ERROR_OFF_AIR = 15,
        ERROR_SATIP_FAILED = 16,
        ERROR_MAX_RECORD_LIST = 17
    }
    export enum eTRecordingCopyControl {
        ERECORDING_COPYCONTROL_COPYOK = 0,
        ERECORDING_COPYCONTROL_NOFURTHERCOPY = 1,
        ERECORDING_COPYCONTROL_COPYONCE = 2,
        ERECORDING_COPYCONTROL_COPYPROHIBITED = 3
    }
    export enum eTRecordingFileOperationState {
        ERECORDING_FILEOP_STATE_ING = 0,
        ERECORDING_FILEOP_STATE_COMPLETE = 1,
        ERECORDING_FILEOP_STATE_ERROR = 2,
        ERECORDING_FILEOP_STATE_UNKNOWN = 3,
        ERECORDING_FILEOP_STATE_STOP = 4,
        ERECORDING_FILEOP_NOT_ENOUGH_SPACE = 5,
        ERECORDING_FILEOP_STATE_SIZE_LIMIT_4G = 6
    }
    export class IChannelGroupItem {
        getId = null;
        getName = null;
        getChannelItemList = null;
    }
    export class IChannelItem {
        getId = null;
        getNo = null;
        getName = null;
        getThumbnailUrl = null;
        isTunnable = null;
    }
    export class IProgrammeItem {
        getId = null;
        getStartTime = null;
        getTitle = null;
        getThumbnailUrl = null;
    }
    export class IProgrammeDetailItem {
        static CHANNEL_LOGO_WIDTH = 80;
        static PROVIDER_LOGO_WIDTH = 100;
        static THUMB_WIDTH = 400;
        static type = {
            programme: 'programme',
            recording: 'recording',
            scheduled: 'scheduled',
            ondemand: 'ondemand'
        };
        getThumbWidth() {
            return IProgrammeDetailItem.THUMB_WIDTH;
        }
        getLogoWidth() {
            return IProgrammeDetailItem.CHANNEL_LOGO_WIDTH;
        }
//        getSourceItem = null;
//        getType: () => string = null;
//        getId = null;
//        getStartTime = null;
//        getEndTime = null;
//        getTitle = null;
//        getThumbnailUrl = null;
//        getChannel = null;
//        getChannelId = null;
//        getChannelNo = null;
//        getChannelName = null;
//        getLogoUrl = null;
//        getLongDescription = null;
//        getGuidance = null;
//        getSeriesId = null;
//        getSeasonNo = null;
//        getEpisodeNo = null;
//        getGenreList = null;
//        getParentalRating = null;
//        getCasIDs = null;
//        getLastPlayPosition = null;
//        hasSchedule = null;
//        hasSeriesSchedule = null;
//        isReminder = null;
//        isOnAir = null;
//        isSeries = null;
//        isPlayable = null;
//        isNew = null;
//        isHD = null;
//        isDRM = null;
//        isLocked = null;
//        isRecordingOnGoing = null;
//        hasRecording = null;
//        isTimebased = null;
//        isScrambled = null;
//        getGroupCRIDs = [];
    }
    export class IPdeProgrammeItem extends IProgrammeDetailItem {
        getType() {
            return IProgrammeDetailItem.type.programme;
        }
    }
    export class IPdeRecordingItem extends IProgrammeDetailItem {
        getType() {
            return IProgrammeDetailItem.type.recording;
        }
    }
    export class IPdeScheduledItem extends IProgrammeDetailItem {
        getType() {
            return IProgrammeDetailItem.type.scheduled;
        }
    }
    export class IPdeOndemandItem extends IProgrammeDetailItem {
        getType() {
            return IProgrammeDetailItem.type.ondemand;
        }
        getLogoWidth() {
            return IProgrammeDetailItem.PROVIDER_LOGO_WIDTH;
        }
    }
    export class IScheduledItem {
        mPvrStorageInfo;

        getId(): any {
            return 1;
        }

        getFullPath(): string {
            return "";
        }

        getStartTime(): any {
            return 1;
        }

        getDuration(): any {
            return 1;
        }

        getTitle(): string {
            return "";
        }

        getThumbnailUrl(): string {
            return "";
        }

        getRepeat(): any {
            return 1;
        }

        getChannelInstance(): any {
            return 1;
        }

        getChannelId(): any {
            return 1;
        }

        getChannelNo(): any {
            return 1;
        }

        getChannelName(): string {
            return "";
        }

        getLogoUrl(param?): string {
            return "";
        }

        getSeriesId(): any {
            return 1;
        }

        getGroupCRIDs(): any[] {
            return [];
        }

        getProgrammeCRID(): any {
            return 1;
        }

        getVolume(): number {
            return 0;
        }

        getLCN(): any {
            return 1;
        }

        setThumbnailUrl(aURL): void {
        }

        getSeriesTitle(): string {
            return '';
        }

        getStorageId(): any {
            return 1;
        }

        // TODO: clarify necessity of storage information for scheduled item
        getStorageType(): any {
            return 1;
        }

        // internal, detachable, external
        getStorageName(): string {
            return "";
        }

        getProgrammeID(): any {
            return 1;
        }

        getCasIDs(): any {
            return 1;
        }

        isPendingSeriesSchedule(): boolean {
            return false;
        }

        isEqual(b): boolean {
            return false;
        }

        isReminder(): boolean {
            return false;
        }

        isNCDChanged(): boolean {
            return false;
        }

        isSeries(): boolean {
            return false;
        }

        isRadio(): boolean {
            return false;
        }

        isLocked(): boolean {
            return false;
        }

        isScrambled(): boolean {
            return false;
        }

        isStreaming(): boolean {
            return false;
        }

        isRecordingOnGoing(): boolean {
            return false;
        }

        setRecordingOnGoing(): void {
        }

        isSeriesButNotRecoredAsSeries(): boolean {
            return false;
        }
    }
    export class IRecordingItem extends IScheduledItem {
        mRecStorageInfo;

        getError(): any {
        }

        getLastPlayPosition(): any {
        }

        getRecordingDuration(): any {
        }

        isNew(): boolean {
            return false;
        }

        isBroken(): boolean {
            return false;
        }

        isNoCopy(): boolean {
            return false;
        }

        audioType = null;

        getGroupCRIDs(): any[] {
            return [];
        }

        getProgrammeCRID(): any {
            return 1;
        }
    }
    var fnCreateGuid = function () {
        return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
            var r = Math.random() * 16 | 0, v = c === 'x' ? r : (r & 0x3 | 0x8);
            return v.toString(16);
        });
    };
    export class IMediaItem {
//        getTitle: () => string;
//        getThumbnailUrl: () => string;
//        getFullPath: () => string;
//        getPath: () => string;
        static TFileType = {
            EFile: 0,
            EFolder: 1,
            EGroup: 2
        };
        static TStorageType = {
            EUnknown: 0,
            ESata: 1,
            EUsb: 2,
            ESdCard: 3,
            ESamba: 4
        };
        static TContentType = {
            EVideo: 0,
            EMusic: 1,
            EPhoto: 2,
            EUnknown: 3
        };
        static TOperationState = {
            EIdle: 0,
            ECopying: 1,
            ECopyComplete: 2,
            ECopyError: 3,
            ERemoving: 4,
            ERemoveComplete: 5,
            ERemoveError: 6,
            EMoving: 7,
            EMoveComplete: 8,
            EMoveError: 9,
            ERenaming: 10,
            ERenameComplete: 11,
            ERenameError: 12,
            EMakingDir: 13,
            EMakeDirComplete: 14,
            EMakeDirError: 15,
            EStopping: 16,
            EStopComplete: 17,
            EStopError: 18,
            EScanning: 19,
            EScanComplete: 20,
            EScanError: 21,
            ERemovingDir: 22,
            ERemoveDirComplete: 23,
            ERemoveDirError: 24,
            EAutoScanStarted: 25,
            EAutoScanComplete: 26,
            EAutoScanError: 27,
            EInfoError: 80,
            E4GFat32: 81,
            ELackSpace: 82,
            EUnknownTicket: 238
        };
        static ContentTypeCssList = ['video', 'music', 'photo'];
        static ExtensionList =  [
            '3gp|3g2|mov|flv|swf|m2ts|rm|ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|mpeg|mpe|dat|ogm|ogg|rmvb',
            hx.config.CONFIG_HD_AUDIO ? 'mp3|wav|f4a|m4a|flac' :'mp3|wav|f4a|m4a',
            'jpg|jpeg|bmp|gif|tif|png'
        ];
    }
    var TContentType = IMediaItem.TContentType;
    export class CMediaItem extends IMediaItem {
        _parent: any;
        _pluginObject: any = null;
        _duration: number = 0;

        constructor(aPluginObject?) {
            super();
            this._pluginObject = aPluginObject;
            this._duration = 0;
        }

        getPluginObject() {
            return this._pluginObject;
        }

        getContentType() {
            return this._pluginObject ? this._pluginObject.contentType : null;
        }

        getFileType() {
            return this._pluginObject ? this._pluginObject.fileType : null;
        }

        getFullPath() {
            return this._pluginObject ? this._pluginObject.fullPath : null;
        }

        getDate() {
            return this._pluginObject ? this._pluginObject.date : null;
        }

        getThumbnailResolution() {
            return this._pluginObject ? this._pluginObject.thumbResolution : null;
        }

        getStartTime() {
            var a = this;
            return a._pluginObject ? (a._pluginObject.startTime || (new Date(1970, 1, 1, 0, 0, 0, 0)).toSeconds()) : (new Date(1970, 1, 1, 0, 0, 0, 0)).toSeconds();
        }

        getDuration() {
            return this._pluginObject ? (this._pluginObject.duration || this._duration) : this._duration;
        }

        setDuration(duration) {
            this._duration = duration;
        }

        getLastPlayPosition() {
            var a = this;
            var historyInfoList = hx.svc.HISTORY.getMediaList();
            var historyInfo;
            var length = historyInfoList.length;
            for (var i = 0; i < length; i += 1) {
                historyInfo = historyInfoList[i];
                if (historyInfo instanceof cMediaPlaybackHistoryItem) {
                    if (a.getFullPath() === historyInfo.getFullPath()) {
                        return historyInfo.getLastPlayPosition();
                    }
                }
            }
            return 0;
        }

        getLargeThumbnailUrl() {
            return this._pluginObject ? (this._pluginObject.largeThumbnailUrl ? this._pluginObject.largeThumbnailUrl : '') : '';
        }

        getThumbnailUrl() {
            return this._pluginObject ? this._pluginObject.thumbnailUrl : '';
        }

        getResolution() {
            return this._pluginObject ? this._pluginObject.resolution : null;
        }

        getPictureDate() {
            return this._pluginObject ? this._pluginObject.pictureDate : null;
        }

        getTitle() {
            var a = this;
            var title = a._pluginObject ? a._pluginObject.title : '';
            var fullPath = a.getFullPath();
            var startIndex = fullPath.lastIndexOf('/') + 1;
            var path = fullPath.substring(startIndex);
            return (title === null || title === undefined || title === '') ? path : title;
        }

        getGenre() {
            return this._pluginObject ? this._pluginObject.genre : '';
        }

        getAlbumName() {
            return this._pluginObject ? this._pluginObject.album : '';
        }

        getArtistName() {
            var artist = this._pluginObject ? this._pluginObject.artist : '';
            if (artist !== "" && !artist) {
                artist = 0;
            }
            return artist;
        }

        isWatched() {
            return this._pluginObject ? this._pluginObject.watched : null;
        }

        getStorageType() {
            return this._pluginObject ? this._pluginObject.storageCategory : null;
        }

        getPath() {
            return this.getFullPath();
        }

        setParent(parent) {
            this._parent = parent;
        }

        setLastPlayPosition(time) {
            this._pluginObject.lastViewTime = time;
        }

        isOperationEnabled() {
            return true;
        }

        getBitRate() {
            return this._pluginObject ? this._pluginObject.bitRate : null;
        }
        getSampleRate() {
            return this._pluginObject ? this._pluginObject.sampleRate : null;
        }
    }
    export class CNetworkMediaItem extends CMediaItem {
        constructor(pluginObject) {
            super(pluginObject);
        }
    }
    export class CUpperFolderItem extends CMediaItem {
        constructor(pluginObject) {
            super(pluginObject);
        }

        getTitle() {
            return tr('LOC_STR_UPPER_FOLDER_ID');
        }

        getFileType() {
            return IMediaItem.TFileType.EFolder;
        }

        getFullPath() {
            var a = this;
            var fullPath = a._pluginObject.parentID;
            if (fullPath === undefined || fullPath === null) {
                fullPath = CMediaItem.prototype.getFullPath.call(a);
                fullPath = fullPath.substring(0, fullPath.lastIndexOf('/'));
            }
            return fullPath;
        }
    }
    export class CMoreMediaItem extends CMediaItem {
        constructor(pluginObject?) {
            super(pluginObject);
        }

        getId() {
            return __recentGuid__;
        }

        getTitle() {
            return '';
        }

        getFileType() {
            return IMediaItem.TFileType.EFile;
        }

        getFullPath() {
            return '';
        }

        getThumbnailUrl() {
        }

        getArtistName() {
            return '';
        }
    }
    export class IMediaServer {
//        getId = null;
//        getName = null;
//        getStorageType = null;
//        getStorageCssClass = null;
//        getViewType = null;
//        getLocationType = null;
//        getPath = null;
//        isMounted = null;
        static TStorageType = {
            EInternalHdd: 0,
            EDetachableHdd: 1,
            EUsb: 2,
            ESdCard: 3,
            EDlna: 4,
            ESamba: 5,
            EWoon: 6,
            ERecent: 7,
            EPlaylist: 8,
            EArtist: 9,
            EAlbum: 10,
            EShared: 11
        };
        static TViewType = {
            EOneDepthView: 0,
            EFolderView: 1
        };
        static TLocationType = {
            ELocal: 0,
            ENetwork: 1,
            EUser: 2
        };
        static StorageCssClassList = ['hdd', 'dtc', 'usb', 'sdm', 'dlna', 'smb', 'woon', 'recent', 'playlist', 'artist', 'album', 'shared'];
    }
    if (hx.config.useInternalPvr === 1) {
        IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EInternalHdd] = 'hdd';
    } else {
        IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EInternalHdd] = 'usbp';
    }
    export class CNetworkServer extends IMediaServer {
        constructor() {
            super();
        }
    }
//    CNetworkMediaItem.prototype.constructor = IMediaServer;
    export var __recentGuid__ = fnCreateGuid();
    export var __playlistGuid__ = fnCreateGuid();
    export var __artistGuid__ = fnCreateGuid();
    export var __albumGuid__ = fnCreateGuid();
    export class CUserMediaServer extends IMediaServer {
        _pluginObject: any;

        constructor(pluginObject) {
            super();
            this._pluginObject = pluginObject;
        }

        getId() {
            return this._pluginObject.id;
        }

        getName() {
            var storageType = this.getStorageType();
            var TStorageType = IMediaServer.TStorageType;
            var name = '';
            switch (storageType) {
                case TStorageType.ERecent:
                    name = tr('LOC_CRECENT_ID');
                    break;
                case TStorageType.EPlaylist:
                    name = tr('LOC_PLAYLIST_ID');
                    break;
                case TStorageType.EArtist:
                    name = tr('LOC_ARTIST_ID').toUpperCase();
                    break;
                case TStorageType.EAlbum:
                    name = tr('LOC_ALBUM_ID').toUpperCase();
                    break;
            }
            return name;
        }

        getStorageType() {
            return this._pluginObject.storageType;
        }

        getStorageCssClass() {
            return this._pluginObject.storageCssClass;
        }

        getViewType() {
            var storageType = this.getStorageType();
            var TStorageType = IMediaServer.TStorageType;
            var viewType = IMediaServer.TViewType.EFolderView;
            switch (storageType) {
                case TStorageType.EArtist:
                case TStorageType.EAlbum:
                    viewType = IMediaServer.TViewType.EOneDepthView;
                    break;
            }
            return viewType;
        }

        getLocationType() {
            return IMediaServer.TLocationType.EUser;
        }

        getPath() {
            return '';
        }

        isMounted() {
            return true;
        }
    }
    export class CMediaItemByUrl extends CMediaItem {
        mUrl;
        mDuration;
        mTitle;

        constructor(title, url) {
            super({});
            this.mUrl = url;
            this.mDuration = 0;
            this.mTitle = title;
        }

        getFullPath() {
            return this.mUrl;
        }

        setDuration(duration) {
            this.mDuration = Math.floor(duration / 1000);
        }

        getDuration() {
            return this.mDuration;
        }

        getTitle() {
            return this.mTitle;
        }

        getContentType() {
            return IMediaItem.TContentType.EVideo;
        }
    }
    var recentItem = new CUserMediaServer({
        id: __recentGuid__,
        storageType: IMediaServer.TStorageType.ERecent,
        storageCssClass: IMediaServer.StorageCssClassList[IMediaServer.TStorageType.ERecent]
    });
    var playlistItem = new CUserMediaServer({
        id: __playlistGuid__,
        storageType: IMediaServer.TStorageType.EPlaylist,
        storageCssClass: IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EPlaylist]
    });
    var artistItem = new CUserMediaServer({
        id: __artistGuid__,
        storageType: IMediaServer.TStorageType.EArtist,
        storageCssClass: IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EArtist]
    });
    var albumItem = new CUserMediaServer({
        id: __albumGuid__,
        storageType: IMediaServer.TStorageType.EAlbum,
        storageCssClass: IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EAlbum]
    });
    export var __photoUserMediaServerList__ = [recentItem, playlistItem];
    export var __videoUserMediaServerList__ = [recentItem];
    export var __musicUserMediaServerList__ = __photoUserMediaServerList__.slice(0);
    //__musicUserMediaServerList__.push(artistItem);
    //__musicUserMediaServerList__.push(albumItem);
    // these are not supported at this time.
    //from dsrvRecordings2

    export class COipfScheduledItem extends IScheduledItem {
        _property;
        _id;
        _uid;
        _name;
        _startTime;
        _duration;
        _channel;
        _groupCRIDs;
        _programmeID;
        _programmeCRID;
        _discId;
        _factoryType;
        _repeatDays;
        _endPadding;
        _startPadding;
        _isScheduledAsSeries;
        _thumbnailUrl: string;
        _recordingOnGoing: boolean = false;
        _shortDescription: string;
        _longDescription: string;
        _guidance: any;
        _audioType;
        _logoURL;

        constructor(aProperty) {
            super();
            this._property = aProperty;
        }

        getId(): any {
            var that = this;
            if (that._id === undefined) {
                that._id = that._property.id;
            }
            return that._id;
        }

        getFullPath(): string {
            return this.getId();
        }

        getStartTime(): any {
            var that = this;
            if (that._startTime === undefined) {
                that._startTime = that._property.startTime;
            }
            return new Date(that._startTime * 1000);
        }

        getDuration(): any {
            var that = this;
            if (that._duration === undefined) {
                that._duration = that._property.duration;
            }
            return that._duration;
        }

        getEndTime(): any {
            var that = this;
            if (that._startTime === undefined) {
                that._startTime = that._property.startTime;
            }
            if (that._duration === undefined) {
                that._duration = that._property.duration;
            }
            return new Date((that._startTime + that._duration) * 1000);
        }

        getTitle(): any {
            var that = this;
            if (that._name === undefined) {
                that._name = that._property.name;
            }
            var title = that._name;
            if (!title) {
                var channel = this.getChannelInstance();
                if (channel) {
                    title = this.getChannelName();
                }
            }
            return title || '';
        }

        getThumbnailUrl(): string {
            return this._thumbnailUrl;
        }

        getShortDescription(): string {
            return this._shortDescription;
        }

        getLongDescription(): string {
            return this._longDescription;
        }

        getGuidance(): any {
            return this._guidance;
        }

        getRepeat(): any {
            var that = this;
            if (that._repeatDays === undefined) {
                that._repeatDays = that._property.repeatDays;
            }
            return that._repeatDays;
        }

        getChannelInstance(): any {
            var that = this;
            if (!that._channel) {
                that._channel = that._property.channel;
            }
            return that._channel;
        }

        getChannelId(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().ccid : '';
        }

        getServiceId(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().sid : null;
        }

        getChannelType(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().channelType : 0;
        }

        getChannelNo(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().majorChannel : 0;
        }

        getChannelName(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().name : '';
        }

        getSeriesId(): any {
            var that = this;
            return that.getGroupCRIDs() ? that.getGroupCRIDs()[0] : null;
        }

        getGroupCRIDs(): any {
            var that = this;
            if (that._groupCRIDs === undefined) {
                that._groupCRIDs = that._property.groupCRIDs;
            }
            return that._groupCRIDs || null;
        }

        getProgrammeCRID(): any {
            var that = this;
            if (that._programmeCRID === undefined) {
                that._programmeCRID = that._property.programmeCRID;
            }
            return that._programmeCRID || null;
        }

        getSeriesTitle(): string {
            return this.getTitle();
        }

        getStorageId(): any {
            var that = this;
            if (that._discId === undefined) {
                that._discId = that._property.discId;
            }
            return that._discId;
        }

        getStorageType(): any {
            return 'internal';
        }

        getStorageName(): string {
            var that = this, name;
            if (hx.config.useInternalPvr === 1) {
                name = hx.l('LOC_INTERNALHDD_ID');
            } else {
                name = hx.l('LOC_EXTERNAL_HDD_ID');
            }
            if (that._discId) {
                var storageInfo = that._fnGetStorageInfo(that._discId);
                if (storageInfo) {
                    switch (storageInfo.usage) {
                        case eTStorageType.EINTHDD:
                            if (hx.config.useInternalPvr === 1) {
                                name = hx.l('LOC_INTERNALHDD_ID');
                            } else {
                                //name = tr('LOC_EXTERNAL_HDD_ID');
                                name = storageInfo.label;
                            }
                            break;
                        case eTStorageType.EINTSUBHDD:
                            if (hx.config.useInternalPvr === 1) {
                                name = hx.l('LOC_DETACHHDD_ID');
                            } else {
                                //name = tr('LOC_EXTERNAL_HDD_ID');
                                name = storageInfo.label;
                            }
                            break;
                    }
                }
            }
            return name;
        }

        getProgrammeID(): any {
            var that = this;
            if (that._programmeID === undefined) {
                that._programmeID = that._property.programmeID;
            }
            return that._programmeID || null;
        }

        getCasIDs(): any {
            return [];
        }

        getFactoryType(): any {
            var that = this;
            if (that._factoryType === undefined) {
                that._factoryType = that._property.factoryType;
            }
            return that._factoryType;
        }

        getFactoryTypeStr(): any {
            var that = this;
            switch(that.getFactoryType()) {
                case eTScheduledFactoryType.ERECORDING:
                    return "Record";
                case eTScheduledFactoryType.EREMIND:
                    return "Remind";
                case eTScheduledFactoryType.EHTTP_LIVE_STREAMING:
                    return "Http Live Streaming";
                case eTScheduledFactoryType.EPOWER_ON_TIMER:
                    return "Power On Timer";
                case eTScheduledFactoryType.ESATRECORD:
                    return "Sat Record";
                case eTScheduledFactoryType.ESERIES_RECORDING:
                    return "Series Record";
                default:
                    return that.getFactoryType();
            }
        }

        getStartPadding() {
            var that = this;
            if (that._startPadding === undefined) {
                that._startPadding = that._property.startPadding;
            }
            return that._startPadding;
        }

        getEndPadding() {
            var that = this;
            if (that._endPadding === undefined) {
                that._endPadding = that._property.endPadding;
            }
            return that._endPadding;
        }

        isScheduledAsSeries() {
            var that = this;
            if (that._isScheduledAsSeries === undefined) {
                that._isScheduledAsSeries = that._property.isScheduledAsSeries;
            }
            return that._isScheduledAsSeries;
        }
        isPendingSeriesSchedule() {
            var that = this;
            if (that._startTime === undefined) {
                that._startTime = that._property.startTime;
            }
            var nowSec = hx.il.nowSec();
            return !!(((that._startTime + that.getDuration() + that.getEndPadding()) < nowSec) && (that.isScheduledAsSeries()));
        }

        isRecording(): boolean {
            return (this.getFactoryType() === eTScheduledFactoryType.ERECORDING) || false;
        }

        isReminder(): boolean {
            return (this.getFactoryType() === eTScheduledFactoryType.EREMIND) || false;
        }

        isNCDChanged(): boolean {
            return (this.getFactoryType() === eTScheduledFactoryType.ESCHEDULED_RECORDING_AOS_NOTIFY) || false;
        }

        isPowerOnTimer(): boolean {
            return (this.getFactoryType() === eTScheduledFactoryType.EPOWER_ON_TIMER) || false;
        }

        isRegularOTA(): boolean {
            return (this.getFactoryType() === eTScheduledFactoryType.EREGULAR_OTA) || false;
        }

        getLCN(): any {
            return this.getChannelNo();
        }

        GetVolume(): number {
            return this._property.volume;
        }

        hasGroupCRIDs() {
            var that = this;
            if (that.getGroupCRIDs() && that.getGroupCRIDs().length) {
                return true;
            }
            return false;
        }

        isSeries(): boolean {
            return this.hasGroupCRIDs();
        }

        isRadio(): boolean {
            return (this.getChannelType() === eTChannelType.ERADIO) || false;
        }

        isLocked(): boolean {
            return false;
        }

        isScrambled(): boolean {
            return false;
        }

        isStreaming(): boolean {
            return (this.getFactoryType() === eTScheduledFactoryType.EHTTP_LIVE_STREAMING) || false;
        }

        isRecordingOnGoing(bOnlyCheckFlag?): boolean {
            if (this._recordingOnGoing) {
                return true;
            }
            if (bOnlyCheckFlag) {
                return false;
            }
            var that = this;
            if (that.isPendingSeriesSchedule()) {
                return false;
            } else {
                if (that._startTime === undefined) {
                    that._startTime = that._property.startTime;
                }
                var currentSec = hx.il.nowSec();
                return !!(currentSec >= ((that._startTime - that.getStartPadding())));
            }
        }

        isEqual(b: any): boolean {
            var a = this;
            var bFoundSameSeries = false;
            var match = false;
            if (a.getFactoryType() === eTScheduledFactoryType.EHTTP_LIVE_STREAMING && b.getFactoryType() === eTScheduledFactoryType.EHTTP_LIVE_STREAMING) {
                if (!a.getChannelInstance() || !b.getChannelInstance()) {
                    return match;
                }
                if (a.getChannelInstance().ccid === b.getChannelInstance().ccid) {
                    match = true;
                }
                return match;
            }
            if (a.isPendingSeriesSchedule() !== b.isPendingSeriesSchedule()) {
                return match;
            } else {
                if (a.isPendingSeriesSchedule()) {
                    if (a.getGroupCRIDs() && b.getGroupCRIDs()) {
                        var len = b.getGroupCRIDs().length, i;
                        for ( i = 0 ; i < len ; i++ ) {
                            if ( (Array.prototype.slice.call(a.getGroupCRIDs())).indexOf(b.getGroupCRIDs()[i]) > -1 ) {
                                bFoundSameSeries = true;
                                break;
                            }
                        }
                    }
                    if (bFoundSameSeries) {
                        if (a.getChannelInstance() && b.getChannelInstance()) {
                            if (a.getChannelId() === b.getChannelId()) {
                                match = true;
                            }
                        }
                    }
                    return match;
                }
            }
            if (a.getUniqueID() === b.getUniqueID()) { //for EBReservation & TBReservation, uid is unique
                match = true;
            }
            return match;
        }

        getUniqueID(): string {
            var that = this;
            if (that._uid === undefined) {
                that._uid = that._property.uid;
            }
            return that._uid;
        }

        getAudioType(): number {
            var that = this;
            if (that._audioType === undefined) {
                that._audioType = that._property.audioType;
            }
            return that._audioType;
        }

        getLogoUrl(width): string {
            var that = this;
            if (!that._logoURL) {
                that._logoURL = {};
            }
            return that._logoURL[width] || '';
        }

        setLogoUrl(aUrl, width) {
            var that = this;
            if (!that._logoURL) {
                that._logoURL = {};
            }
            that._logoURL[width] = aUrl;
        }

        setRecordingOnGoing(): void {
            this._recordingOnGoing = true;
        }

        setThumbnailUrl(url): void {
            this._thumbnailUrl = url;
        }

        setShortDescription(desc) {
            this._shortDescription = desc || '';
        }

        setLongDescription(desc) {
            this._longDescription = desc || '';
        }

        setGuidance(desc) {
            this._guidance = desc ;
        }

        getInstance(): any {
            return this._property;
        }

        getTuningInfo(): any {
            return {
                diseqcInput: this._property.diseqcInput,
                frequency: this._property.frequency,
                polarisation: this._property.polarisation,
                symbolRate: this._property.symbolRate,
                satType: this._property.satType
            };
        }

        _fnGetStorageInfo(uuid): any {
            var that = this;
            var i, ti, pStorage;
            if (!that.mPvrStorageInfo) {
                that.mPvrStorageInfo = [];
                var list = hx.svc.STORAGE_MASS.getRecPVRList();
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    pStorage = list[i];
                    if (pStorage) {
                        that.mPvrStorageInfo.push({
                            uuid: pStorage.uuid,
                            usage: pStorage.usage,
                            label: pStorage.label
                        });
                    }
                }
            }
            var storageInfo;
            for (i = 0, ti = that.mPvrStorageInfo.length; i < ti; i += 1) {
                if (that.mPvrStorageInfo[i].uuid === uuid) {
                    storageInfo = that.mPvrStorageInfo[i];
                    break;
                }
            }
            return storageInfo;
        }
    }
    export class COipfRecordingItem extends IRecordingItem {
        _property: any;
        _id;
        _name;
        _startTime;
        _duration;
        _recordingDuration;
        _channel;
        _groupCRIDs;
        _programmeID;
        _programmeCRID;
        _subtitleTypes;
        _discId;
        _guidance;
        _repeatDays;
        _shortDescription;
        _longDescription;
        _showType;
        _state;
        _copyControl;
        _parentalRatings;
        _isManualLocked;
        _willBeDeleted;
        _isScrambled;
        _isEncrypted;
        _lastPlayPosition;
        _isScheduledAsSeries;
        _isHD;
        _isAD;
        _audioType;
        _genres;
        _endPadding;
        _thumbnailURL;
        _logoURL;

        constructor(aProperty) {
            super();
            this._property = aProperty;
        }

        getId(): any {
            var that = this;
            if (that._id === undefined) {
                that._id = that._property.id;
            }
            return that._id;
        }

        getEpisode(): any {
            return this._property.episode;
        }

        getSeason(): any {
            return this._property.season;
        }

        getFullPath(): any {
            return this.getId();
        }

        getStartTime(): any {
            var that = this;
            if (that._startTime === undefined) {
                that._startTime = that._property.recordingStartTime;
            }
            return new Date(that._startTime * 1000);
        }

        getDuration(): any {
            var that = this;
            if (that._duration === undefined) {
                that._duration = that._property.duration;
            }
            return that._duration;
        }

        getEndTime(): any {
            var that = this;
            if (that._startTime === undefined) {
                that._startTime = that._property.recordingStartTime;
            }
            if (that._duration === undefined) {
                that._duration = that._property.duration;
            }
            return new Date((that._startTime + that._duration) * 1000);
        }

        getRecordingDuration(): any {
            var that = this;
            if (that._recordingDuration === undefined) {
                that._recordingDuration = that._property.recordingDuration;
            }
            return that._recordingDuration;
        }

        getTitle(): any {
            var that = this;
            if (that._name === undefined) {
                that._name = that._property.name;
            }
            var title = that._name;
            if (!title) {
                var channel = this.getChannelInstance();
                if (channel) {
                    title = this.getChannelName();
                }
            }
            return title || '';
        }

        getThumbnailUrl(): any {
            var that = this;
            if (!that._thumbnailURL) {
                that._thumbnailURL = this._property.thumbnail;
            }
            var thumbnailUrl = that._thumbnailURL;
            if (that.isLocked()) {
                thumbnailUrl = TDefaultThumbnailList.RECORDING;
            } else if (that.isNew() && that.isBroken()) {
                thumbnailUrl = TDefaultThumbnailList.REC_FAIL;
            } else {
                // TODO : should use default image on page(or upper layer)
            }
            return thumbnailUrl;
        }

        getLogoUrl(width): string {
            var that = this;
            if (!that._logoURL) {
                that._logoURL = {};
            }
            return that._logoURL[width] || '';
        }

        setLogoUrl(aUrl, width) {
            var that = this;
            if (!that._logoURL) {
                that._logoURL = {};
            }
            that._logoURL[width] = aUrl;
        }

        getRepeat(): any {
            var that = this;
            if (that._repeatDays === undefined) {
                that._repeatDays = that._property.repeatDays;
            }
            return that._repeatDays;
        }

        getShortDescription(): string {
            var that = this;
            if (that._shortDescription === undefined) {
                that._shortDescription = that._property.description;
            }
            return that._shortDescription;
        }

        getLongDescription(): string {
            var that = this;
            if (that._longDescription === undefined) {
                that._longDescription = that._property.longDescription;
            }
            return that._longDescription;
        }

        getChannelInstance(): any {
            var that = this;
            if (that._channel === undefined) {
                that._channel = that._property.channel;
            }
            return that._channel;
        }

        getChannelId(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().ccid : '';
        }

        getServiceId(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().sid : null;
        }

        getChannelType(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().channelType : 0;
        }

        getChannelNo(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().majorChannel : 0;
        }

        getChannelName(): any {
            var that = this;
            return that.getChannelInstance() ? that.getChannelInstance().name : '';
        }

        getSeriesId(): any {
            var that = this;
            return that.getGroupCRIDs() ? that.getGroupCRIDs()[0] : null;
        }

        getGroupCRIDs(): any {
            var that = this;
            if (that._groupCRIDs === undefined) {
                that._groupCRIDs = that._property.groupCRIDs;
            }
            return that._groupCRIDs;
        }

        getProgrammeCRID(): any {
            var that = this;
            if (that._programmeCRID === undefined) {
                that._programmeCRID = that._property.name;
            }
            return that._programmeCRID;
        }

//        getRecommandationCRID(): any {
//            return this._property.recommandationCRID;
//        }

        getSeriesTitle(): any {
            var that = this;
            if (that._name === undefined) {
                that._name = that._property.name;
            }
            return that._name;
        }

        getContentType(): any {
            return IMediaItem.TContentType.EVideo;
        }

        getStorageId(): any {
            var that = this;
            if (that._discId === undefined) {
                that._discId = that._property.discId;
            }
            return that._discId;
        }

        getSubtitleTypes(): any {
            var that = this;
            if (that._subtitleTypes === undefined) {
                that._subtitleTypes = that._property.subtitleTypes;
            }
            return that._subtitleTypes;
        }

        getStorageType(): any {
            var that = this;
            var type = 'internal';
            if (that._discId) {
                var i, storageInfo = this._fnGetStorageInfo(that._discId);
                if (storageInfo) {
                    switch (storageInfo.usage) {
                        case eTStorageType.EINTHDD:
                            type = 'internal';
                            break;
                        case eTStorageType.EINTSUBHDD:
                            type = 'detachable';
                            break;
                    }
                }
            }
            return type;
        }

        _fnGetStorageInfo(uuid): any {
            var i, ti, pStorage;
            if (!this.mPvrStorageInfo) {
                var list = hx.svc.STORAGE_MASS.getRecPVRList();
                this.mPvrStorageInfo = [];
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    pStorage = list[i];
                    if (pStorage) {
                        this.mPvrStorageInfo.push({
                            uuid: pStorage.uuid,
                            usage: pStorage.usage,
                            label: pStorage.label
                        });
                    }
                }
            }
            var storageInfo;
            for (i = 0, ti = this.mPvrStorageInfo.length; i < ti; i += 1) {
                if (this.mPvrStorageInfo[i].uuid === uuid) {
                    storageInfo = this.mPvrStorageInfo[i];
                    break;
                }
            }
            return storageInfo;
        }

        _fnGetRecStorageInfo(uuid): any {
            var i, ti, pStorage;
            if (!this.mRecStorageInfo) {
                var list = hx.svc.STORAGE_MASS.getRecStorageList();
                this.mRecStorageInfo = [];
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    pStorage = list[i];
                    if (pStorage) {
                        this.mRecStorageInfo.push({
                            uuid: pStorage.uuid,
                            usage: pStorage.usage,
                            label: pStorage.label
                        });
                    }
                }
            }
            var storageInfo;
            for (i = 0, ti = this.mRecStorageInfo.length; i < ti; i += 1) {
                if (this.mRecStorageInfo[i].uuid === uuid) {
                    storageInfo = this.mRecStorageInfo[i];
                    break;
                }
            }
            return storageInfo;
        }

        getStorageName(): any {
            var a = this, name;
            if (hx.config.useInternalPvr === 1) {
                name = hx.l('LOC_INTERNALHDD_ID');
            } else {
                name = hx.l('LOC_EXTERNAL_HDD_ID');
            }
            if (this._property.discId) {
                var i, storageInfo = this._fnGetRecStorageInfo(this._property.discId);
                if (storageInfo) {
                    switch (storageInfo.usage) {
                        case eTStorageType.EINTHDD:
                            if (hx.config.useInternalPvr === 1) {
                                name = hx.l('LOC_INTERNALHDD_ID');
                            } else {
                                //name = tr('LOC_EXTERNAL_HDD_ID');
                                name = storageInfo.label;
                            }
                            break;
                        case eTStorageType.EINTSUBHDD:
                            if (hx.config.useInternalPvr === 1) {
                                name = hx.l('LOC_DETACHHDD_ID');
                            } else {
                                //name = tr('LOC_EXTERNAL_HDD_ID');
                                name = storageInfo.label;
                            }
                            break;
                        case eTStorageType.ERECSTORAGE:
                            name = storageInfo.label;
                            break;
                    }
                }
            }
            return name;
        }

        getCasIDs(): any {
            return [];
        }

        getGenre(): any {
            var that = this;
            if (that._genres === undefined) {
                that._genres = that._property.genres || [];
            }
            var i, length, major, minor, word, genre, genres = [], temp = that._genres;
            var TMajorGenreType = hx.il.genreList.getGenreList('major', false);
            var TMinorGenreType = hx.il.genreList.getGenreList('minor', false);
            function validate(value, min, max) {
                return (!isNaN(value) && value > min && value < max);
            }
            for (i = 0, length = temp.length; i < length; i += 1) {
                genre = temp[i] || '';
                major = parseInt(genre.charAt(0), 16) - 1;
                minor = parseInt(genre.charAt(1), 16);

                if (validate(major, -1, TMajorGenreType.length)) {
                    if (validate(minor, -1, TMinorGenreType.length)) {
                        genres.push(hx.l(TMinorGenreType[major][minor]));
                    } else {
                        if (TMinorGenreType.length === 0) {
                            if(TMajorGenreType[major] !== '') {
                                genres.push(hx.l(TMajorGenreType[major]));
                            }
                        }
                    }
                }
            }
            return genres;
        }

        getProgrammeID(): any {
            var that = this;
            if (that._programmeID === undefined) {
                that._programmeID = that._property.programmeID;
            }
            return that._programmeID;
        }

        getLastPlayPosition(): any {
            var that = this;
            if (that._lastPlayPosition === undefined) {
                that._lastPlayPosition = that._property.lastPlayPosition;
            }
            return that._lastPlayPosition;
        }

        getError(): any {
            return this._property.error;
        }

        hasGroupCRIDs() {
            var that = this;
            if (that.getGroupCRIDs() && that.getGroupCRIDs().length) {
                return true;
            }
            return false;
        }

        isHd() {
            var that = this;
            if (that._isHD === undefined) {
                that._isHD = that._property.isHD;
            }
            return that._isHD;
        }

        isAd() {
            var that = this;
            if (that._isAD === undefined) {
                that._isAD = that._property.getField("freeview/hasAudioDescription");
            }
            return !!(that._isAD === 'true');
        }

        getAudioType() {
            var that = this;
            if (that._audioType === undefined) {
                that._audioType = that._property.audioType;
            }
            return that._audioType;
        }

        isSeries() {
            return this.hasGroupCRIDs();
        }

        isRadio() {
            return (this.getChannelType() === eTChannelType.ERADIO) || false;
        }

        isNew(): boolean {
            var that = this;
            if (that._showType === undefined) {
                that._showType = that._property.showType;
            }
            return (that._showType === eTRecordingShowType.EFIRST_RUN) || false;
        }

        isBroken(): boolean {
            var that = this;
            if (that._state === undefined) {
                that._state = that._property.state;
            }
            return (that._state === eTRecordingState.ERECORDING_REC_PARTIALLY_COMPLETED) || false;
        }

        isNoCopy(): boolean {
            var that = this;
            if (that._copyControl === undefined) {
                that._copyControl = that._property.copyControl;
            }
            return (that._copyControl !== eTRecordingCopyControl.ERECORDING_COPYCONTROL_COPYOK) || false;
        }

        isLocked() {
            var that = this;
            if (that._isManualLocked === undefined) {
                that._isManualLocked = that._property.isManualLocked;
            }
            return that._isManualLocked || false;
        }

        isKept() {
            return this._property.doNotDelete || false;
        }

        setKeep(aKeep) {
            this._property.doNotDelete = aKeep;
        }

        isWillbedeleted() {
            return this.getWillBeDeleted() || false;
        }

        isScrambled() {
            var that = this;
            if (that._isScrambled === undefined) {
                that._isScrambled = that._property.isScrambled;
            }
            return that._isScrambled || false;
        }

        isEncrypted() {
            var that = this;
            if (that._isEncrypted === undefined) {
                that._isEncrypted = that._property.isEncrypted;
            }
            return that._isEncrypted || false;
        }

        isRecordingOnGoing() {
            var that = this;
            if (that._showType === undefined) {
                that._showType = that._property.showType;
            }
            return (that._showType === eTRecordingShowType.ELIVE) || false;
        }

        isEqual(b) {
            var a = this;
            return !!(a.getId() && (a.getId() === b.getId()));
        }

        isScheduledAsSeries() {
            var that = this;
            if (that._isScheduledAsSeries === undefined) {
                that._isScheduledAsSeries = that._property.isScheduledAsSeries;
            }
            return that._isScheduledAsSeries;
        }

        isRestrictedByDrm(b) {
            return (this._property.error === eTRecordingError.ERROR_REC_DRM_RESTRICTION) || false;
        }

        setManualLock(lock, oipfRecSchedulerObj) {
            var that = this;
            if (that._property.isManualLocked !== lock) {
                that._property.isManualLocked = lock;
                if (that.isSeries()) {
                    if (hx.config.useRfCRID === true) {
                        oipfRecSchedulerObj.setSeriesLock(that.getGroupCRIDs()[0], lock);
                    } else {
                        oipfRecSchedulerObj.setSeriesLock(that.getSeriesId(), lock);
                    }
                }
                if (hx.emulMode) {
                    if (oipfRecSchedulerObj._PVREvent) {
                        oipfRecSchedulerObj._PVREvent(eTRecordingState.ERECORDING_REC_UPDATED, that.getInstance());
                    }
                }
            } else {
                if (that.isSeries()) {
                    if (hx.config.useRfCRID === true) {
                        oipfRecSchedulerObj.setSeriesLock(that.getGroupCRIDs()[0], lock);
                    } else {
                        oipfRecSchedulerObj.setSeriesLock(that.getSeriesId(), lock);
                    }
                }
            }
        }

        getInstance(): any {
            return this._property;
        }

        getParentalRatings(): any {
            var that = this;
            if (that._parentalRatings === undefined) {
                that._parentalRatings = that._property.parentalRatings;
            }
            return that._parentalRatings;
        }

        getWillBeDeleted(): boolean {
            var that = this;
            if (that._willBeDeleted === undefined) {
                that._willBeDeleted = that._property.willBeDeleted;
            }
            return that._willBeDeleted;
        }
        getGuidance() {
            var that = this;
            if (that._guidance === undefined) {
                that._guidance = that._property.getField("freeview/guidance");
            }
            if (that._guidance)  {
                var guidance = JSON.parse(that._guidance);
                hx.log('debug', "[COipfRecordingItem] getGuidance---- text : " + (guidance.guidance ? guidance.guidance[0].text : ""));
                hx.log('debug', "[COipfRecordingItem] getGuidance---- type : " + (guidance.guidance ? guidance.guidance[0].type : ""));
                hx.log('debug', "[COipfRecordingItem] getGuidance---- mode : " + (guidance.guidance ? guidance.guidance[0].mode : ""));
                return guidance.guidance ? guidance.guidance[0] : "";
            } else {
                return "";
            }
        }
        hasGuidance() {
            var that = this;
            var guidance = that.getGuidance();
            if (guidance && ((guidance.type === 0) || (guidance.type === 1 && guidance.mode === 1))) {
                // guidance_type | guidance_mode | Meaning
                // ----------------------------------------------
                //      0 1      |     n/a 1     | Unsuitable for viewing outside "watershed hours" without access controls.
                //       1       |       0       | Warnings/guidance (e.g. Strobe lighting), but suitable for viewing anytime.
                //      2 3      |      n/a      | Reserved for future use.
                // ----------------------------------------------
                return true;
            }
            return false;
        }
        getEndPadding() {
            var that = this;
            if (that._endPadding === undefined) {
                that._endPadding = that._property.endPadding;
            }
            return that._endPadding;
        }
        wasWatching() { // for continue watching feature
            var that = this;
            if (!that.isNew() && (that.getDuration() > that.getLastPlayPosition())) {
                return true;
            }
            return false;
        }
    }

    export class cArray<T> {
        length: number;
        constructor(...aArgs: T[]) {
            Array.apply(this, arguments);
        }
        push(aItem): number {
            return 0;
        }
        splice(aTargetIndex, aEA, a): any {
            return 0;
        }
        unshift(aItem?): number {
            return 0;
        }
        slice(start: number, end?: number): any {
            return 0;
        }
    }
    cArray.prototype = Array.prototype;
    export class COipfScheduledItemList extends cArray<any> {
        _filter;
        constructor(list, filter, maker) {
            super();
            this.setList(list, filter, maker);
        }
        setFilter(filter) {
            this._filter = filter;
        }
        indexBy(aTarget, fnCondition) {
            var that = this;
            if (!item || !fnCondition) {
                return;
            }
            for (var i = 0, ti = that.length; i < ti; i += 1) {
                if (fnCondition(that[i], aTarget)) {
                    return i;
                }
            }
            return -1;
        }
        add(item): any {
            var that = this;
            if (that._filter(item)) {
                return that.push(item);
            }
            return false;
        }
        indexOf(item) {
            var idMap = this.map(function(aItem) {
                var progID = aItem.getProgrammeID();
                if (!progID) {
                    return '';
                }
                return hx.il.getCompareID(aItem);
            });
            return idMap.indexOf(hx.il.getCompareID(item));
        }
        existOf(item) {
            return this.some(function(aItem) {
                return hx.il.compareProgrammeID(item.getProgrammeID(), aItem.getProgrammeID());
            });
        }
        remove(item) {
            var i, ti;
            var that = this;
            for (i = 0, ti = that.length; i < ti; i += 1) {
                if (item.isEqual(that[i])) {
                    delete that[i];
                    that.splice(i, 1);
                    break;
                }
            }
            return;
        }
        setList(list, filter, maker) {
            var that = this;
            var i, ti;
            that.setFilter(filter);
            for (i = 0, ti = list.length; i < ti; i += 1) {
                that.add(maker(list[i]));
            }
        }
        update(item) {
            var that = this;
            var i, ti, index = -1;
            if (that._filter(item)) {
                for (i = 0, ti = that.length; i < ti; i += 1) {
                    if (item.isEqual(that[i])) {
                        that[i] = item;
                        index = i;
                        hx.log('dsrvRecordings2', 'COipfScheduledItemList.updated ' + i);
                        break;
                    }
                }
            }
            return index;
        }
    }

    /**************************************************
     * CTsrItem
     **************************************************/
    var EMAX_DURATION = hx.config.durationTSR;
    export class CTsrItem extends CMediaItem {
        _isChase: any;
        _recording: any;
        constructor(pluginObject) {
            super(pluginObject || {});
            this._isChase = null;
        }
        getStartTime() {
            var a = this;
            var now = new Date().toSeconds();
            var maxOffset = hx.svc.TSR.getMaxOffset();
            // a._pluginObject.startTime = mStartTime;
            var startTime = now - maxOffset;
            console.log(hx.util.format('getStartTime - startTime = {0}', startTime));
            return startTime;
        }
        getStartOffset() {
            return hx.svc.TSR.getStartOffset();
        }
        getDuration() {
            var a = this;
            var duration = 0;
            if (a.isChase() && !!a._recording) {
                var chaseItem = a._recording.getInstance();
                duration = a.getStartTime() - chaseItem.startTime + chaseItem.duration + 60;
            } else {
                a._pluginObject.duration = EMAX_DURATION;
                duration = super.getDuration.call(a);
            }
            //hx.log(hx.util.format('getDuration - EMAX_DURATION = {0}, duration = {1}', EMAX_DURATION, duration));
            return duration;
        }
        setDuration(duration) {
            var a = this;
            if (a.isChase() && !!a._recording) {
                a._recording.getInstance().duration = duration;
            }
        }
        getTitle() {
            var a = this;
            var title = '';
            var result;
            var currentChannel = hx.stbObject.videoBroadcast.currentChannel;
            if (currentChannel) {
                result = hx.svc.EPG.getLiveProgrammes({
                    channel : currentChannel,
                    startTime : a.getStartTime() + hx.svc.TSR.getPlaybackOffset()
                });
            }
            if (result) {
                var len = result[0].length;
                if (len > 0) {
                    title = result[0][0].getName();
                }
            }
            return title;
        }
        isChase() {
            var a = this;
            if (a._isChase === null) {
                var i, length, item, recording, chase = false;
                var channel = hx.svc.CH_LIST.getCurrentChannel().channel;
                var recordings = hx.svc.RECORDINGS.getCurrentRecordings();
                if (channel && recordings && recordings.length) {
                    length = recordings.length;
                    for ( i = 0; i < length; i += 1) {
                        recording = recordings[i];
                        if (recording && recording.getChannelId() === channel.ccid) {
                            chase = true;
                            a._recording = recording;
                            break;
                        }
                    }
                }
                a._isChase = chase;
            }
            return a._isChase;
        }
        getCasIDs() {
            var casIDs;
            var channel = hx.svc.CH_LIST.getCurrentChannel().channel;
            if (channel) {
                casIDs = channel.casIDs;
            } else {
                casIDs = [];
            }
            return casIDs;
        }
    }

    // from dsrvHistory
    export var TDefaultThumbnailList = {
        DEFAULT : 'images/614_Thumb_Genre.jpg',
        RECORDING : 'images/614_Thumb_REC.jpg',
        REC_FAIL : 'images/614_Thumb_REC_Fail.jpg',
        TV : 'images/614_Thumb_TV.jpg',
        RADIO : 'images/614_Thumb_Radio.jpg',
        FREEVIEW : 'images/614_OD_Default01.png',
        CONTENT : 'images/614_OD_Default02.png'
    };
    export var THistoryType = {
        Channel: 'channel',
        OnNow: 'onNow',
        Recording: 'recentlyRecorded',
        Playback: 'recording',
        // 'recording' is already used for PVR playback in webstorage.
        Media: 'media',
        App: 'app',
        Unknown: 'unknown'
    };
    export class cHistoryItem {
        _type: any;
        _id: string;
        constructor() {
            this._type = '';
            this._id = '';
        }
        getType() {
            return this._type;
        }
        getId() {
            return this._id;
        }
        isEqual(historyItem) {
            return (this.getType() === historyItem.getType() && this.getId() === historyItem.getId());
        }
        serialize(): any{
            return {
                type : this.getType(),
                id : this._id
            };
        }
        deSerialize(obj): any {
            this._type = obj.type;
            this._id = obj.id;
            return this;
        }
    }
    export class cChannelHistoryItem extends cHistoryItem {
        _no: string;
        _thumbnail: any;
        _title: string;
        _group: any;
        constructor(channel) {
            super();
            if (channel) {
                this._type = THistoryType.Channel;
                this._id = channel.ccid;
                this._no = hx.il.makeDigitNumber(channel.majorChannel);
                this._thumbnail = channel.logoURL;
                this._title = channel.name;
                this._group = hx.svc.WEB_STORAGE.getLastGroup();
            }
        }
        serialize() {
            var obj = super.serialize();
            obj.channelNo = this._no;
            obj.title = this._title;
            obj.thumbnail = this._thumbnail;
            obj.group = this._group;
            return obj;
        }
        deSerialize(obj) {
            super.deSerialize(obj);
            this._no = obj.channelNo;
            this._title = obj.title;
            this._thumbnail = obj.thumbnail;
            this._group = obj.group;
            return this;
        }
        getNo() {
            return this._no;
        }
        getName() {
            return this._title;
        }
        getLogo() {
            return this._thumbnail;
        }
    }
    export class cOnNowHistoryItem extends cHistoryItem {
        _no: string;
        _title: string;
        _index: any;
        _channelType: any;
        constructor(channel, index) {
            super();
            if (channel) {
                this._type = THistoryType.OnNow;
                this._id = channel.ccid;
                this._title = channel.name;
                this._no = hx.il.makeDigitNumber(channel.majorChannel);
                this._channelType = channel.channelType;
            }
            this._index = index || 0;
        }
        serialize() {
            var obj = super.serialize();
            obj.channelNo = this._no;
            obj.title = this._title;
            obj.index = this._index;
            obj.channelType = this._channelType;
            return obj;
        }
        deSerialize(obj) {
            super.deSerialize(obj);
            this._no = obj.channelNo;
            this._title = obj.title;
            this._index = obj.index;
            this._channelType = obj.channelType;
            return this;
        }
        getIndex() {
            return this._index;
        }
        getName() {
            return this._title;
        }
        getLcn() {
            return this._no;
        }
        isRadio(): boolean {
            return (this._channelType === eTChannelType.ERADIO) || false;
        }
        isEqual(historyItem) {
            return ((this.getType() === historyItem.getType()) && (this.getIndex() === historyItem.getIndex()));
        }
    }
    export class cRecordingHistoryItem extends cHistoryItem {
        _title: any;
        _thumbnail: any;
        _startTime: any;
        _duration: any;
        constructor(recordingItem) {
            super();
            if (recordingItem) {
                if (recordingItem.id) {// OIPF Recording
                    this._type = THistoryType.Recording;
                    this._id = recordingItem.id;
                    this._title = recordingItem.name;
                    this._startTime = recordingItem.startTime;
                    this._duration = recordingItem.duration;
                }
            }
        }
        serialize() {
            var obj = super.serialize();
            obj.title = this._title;
            obj.thumbnail = this._thumbnail;
            obj.startTime = this._startTime;
            obj.duration = this._duration;
            return obj;
        }
        deSerialize(obj) {
            super.deSerialize(obj);
            this._title = obj.title;
            this._thumbnail = obj.thumbnail;
            this._startTime = obj.startTime;
            this._duration = obj.duration;
            return this;
        }
        getId() {
            return this._id;
        }
        getTitle() {
            return this._title;
        }
        getStartTime() {
            return this._startTime;
        }
        getDuration() {
            return this._duration;
        }
    }
    export class cPlaybackHistoryItem extends cHistoryItem {
        _title: any;
        _thumbnail: any;
        _duration: any;
        _lastPlayPosition: any;
        constructor(playbackItem) {
            super();
            if (playbackItem) {
                if (playbackItem.id) {// OIPF Recording
                    this._type = THistoryType.Playback;
                    this._id = playbackItem.id;
                    this._title = playbackItem.name;
                    this._duration = playbackItem.duration;
                    this._lastPlayPosition = playbackItem.lastPlayPosition;
                    if (playbackItem.thumbnail) {
                        this._thumbnail = playbackItem.thumbnail.indexOf('file://') === 0 ? playbackItem.thumbnail : 'file://' + playbackItem.thumbnail;
                    } else {
                        this._thumbnail = TDefaultThumbnailList.RECORDING;
                    }
                }
            }
        }
        serialize() {
            var obj = super.serialize();
            obj.title = this._title;
            obj.thumbnail = this._thumbnail;
            obj.duration = this._duration;
            obj.lastPlayPosition = this._lastPlayPosition;
            return obj;
        }
        deSerialize(obj) {
            super.deSerialize(obj);
            this._title = obj.title;
            this._thumbnail = obj.thumbnail;
            this._duration = obj.duration;
            this._lastPlayPosition = obj.lastPlayPosition;
            return this;
        }
        getId() {
            return this._id;
        }
        getTitle() {
            return this._title;
        }
        getThumbnail() {
            return this._thumbnail;
        }
        getDuration() {
            return this._lastPlayPosition;
        }
        getLastPlayPosition() {
            return this._lastPlayPosition;
        }
        setLastPlayPosition(time) {
            this._lastPlayPosition = time;
        }
        wasWatching() {
            if (this._duration > this._lastPlayPosition) {
                return true;
            }
            return false;
        }
    }
    export class cMediaPlaybackHistoryItem extends cPlaybackHistoryItem {
        _contentType: any;
        _fullPath: any;
        _startTime: any;
        _duration: any;
        _resolution: any;
        _artist: any;
        _album: any;
        _time: any;
        _lastPlayPosition: any;
        _largeThumbnailUrl: any;
        _localMedia: any;
        _sampleRate: any;
        _bitRate: any;
        _pluginObject;
        constructor(mediaItem) {
            super(mediaItem);
            if (mediaItem) {
                this._pluginObject = mediaItem.getPluginObject();
                this._type = THistoryType.Media;
                this._id = mediaItem.getFullPath();
                this._title = mediaItem.getTitle();
                this._localMedia = mediaItem._localMedia === undefined ? !(mediaItem instanceof CNetworkMediaItem) : mediaItem._localMedia;
                this._contentType = mediaItem.getContentType();
                this._fullPath = mediaItem.getFullPath();
                this._thumbnail = mediaItem.getThumbnailUrl();
                this._startTime = mediaItem.getStartTime();
                this._duration = mediaItem.getDuration();
                this._resolution = mediaItem.getResolution ? mediaItem.getResolution() : undefined;
                this._artist = mediaItem.getArtistName ? mediaItem.getArtistName() : undefined;
                this._album = mediaItem.getAlbumName ? mediaItem.getAlbumName() : '';
                this._time = mediaItem.getDate();
                this._lastPlayPosition = mediaItem.getLastPlayPosition();
                this._sampleRate = mediaItem.getSampleRate();
                this._bitRate = mediaItem.getBitRate();
            }
        }
        serialize(): any {
            var obj: any = super.serialize();
            obj.title = this._title;
            obj.thumbnail = this._thumbnail;
            obj.contentType = this._contentType;
            obj.fullPath = this._fullPath;
            obj.startTime = this._startTime;
            obj.duration = this._duration;
            obj.resolution = this._resolution;
            obj.artist = this._artist;
            obj.album = this._album;
            obj.largeThumbnailUrl = this._largeThumbnailUrl;
            obj.time = this._time;
            obj.lastPlayPosition = this._lastPlayPosition;
            obj.sampleRate = this._sampleRate;
            obj.bitRate = this._bitRate;
            return obj;
        }
        deSerialize(obj) {
            super.deSerialize(obj);
            this._title = obj.title;
            this._thumbnail = obj.thumbnail;
            this._contentType = obj.contentType;
            this._fullPath = obj.fullPath;
            this._startTime = obj.startTime;
            this._duration = obj.duration;
            this._resolution = obj.resolution;
            this._artist = obj.artist;
            this._album = obj.album;
            this._largeThumbnailUrl = obj.largeThumbnailUrl;
            this._time = obj.time;
            this._lastPlayPosition = obj.lastPlayPosition;
            this._sampleRate = obj.sampleRate;
            this._bitRate = obj.bitRate;
            return this;
        }
        getId() {
            return this._id;
        }
        getPluginObject() {
            return this._pluginObject;
        }
        getContentType() {
            return this._contentType;
        }
        getTitle() {
            return this._title;
        }
        getThumbnailUrl() {
            var duration = this._duration;
            var lastPlayPosition : number = this._lastPlayPosition;
            if (duration > 7) {
                if (lastPlayPosition > duration - 60) {
                    lastPlayPosition = lastPlayPosition - 1;
                } else {
                    lastPlayPosition = (lastPlayPosition < 7 ) ? 7 : lastPlayPosition;
                }
                this._thumbnail = hx.svc.AV_CTRL.requestThumbnail({
                    'path': this._fullPath,
                    'position': lastPlayPosition
                });
                if( hx.svc.MEDIA_ITEM_MANAGER.isExist({path: this._thumbnail}) ) {
                    return this._thumbnail;
                }
                lastPlayPosition = 7;
            } else {
                lastPlayPosition = 0;
            }

            //this._lastPlayPosition = (this._lastPlayPosition === 0 ) ? 7 : this._lastPlayPosition;
            this._thumbnail = hx.svc.AV_CTRL.requestThumbnail({
                'path': this._fullPath,
                'position': lastPlayPosition
            });
            return this._thumbnail;

        }
        getLargeThumbnailUrl() {
            return this._largeThumbnailUrl ? this._largeThumbnailUrl : '';
        }
        getFullPath() {
            return this._fullPath;
        }
        getStartTime() {
            return this._startTime;
        }
        getDuration() {
            return this._duration;
        }
        getResolution() {
            return this._resolution;
        }
        getAlbumName() {
            return this._album;
        }
        getArtistName() {
            return this._artist;
        }
        getFileType() {
            return IMediaItem.TFileType.EFile;
        }
        getDate() {
            return this._time;
        }
        getStorageType() {
            return IMediaServer.TStorageType.ERecent;
        }
        getLastPlayPosition() {
            return this._lastPlayPosition;
        }
        setLastPlayPosition(time) {
            this._lastPlayPosition = (time === 0) ? 7 : time;
            this._thumbnail = hx.svc.AV_CTRL.requestThumbnail({
                'path': this._fullPath,
                'position': this._lastPlayPosition
            });
            //
        }
        isOperationEnabled() {
            return true;
        }
        getBitRate() {
            return this._bitRate;
        }
        getSampleRate() {
            return this._sampleRate;
        }
    }
    export class cAppHistoryItem extends cHistoryItem {
        _title: string;
        _thumbnail: any;
        _icon: any;
        constructor(wd) {
            super();
            this._type = THistoryType.App;
            if (wd) {
                this._id = wd.id;
                this._title = wd.name;
                this._thumbnail = wd.localURI + wd.customIcons[0];
            }
        }
        serialize(): any {
            var obj: any = super.serialize();
            obj.title = this._title;
            obj.icon = this._icon;
            obj.thumbnail = this._thumbnail;
            return obj;
        }
        deSerialize(obj) {
            super.deSerialize(obj);
            this._title = obj.title;
            this._icon = obj.icon;
            this._thumbnail = obj.thumbnail;
            return this;
        }
    }
    // from dsrvPlaylist
    export class cMediaPlaybackPlaylistItem extends cMediaPlaybackHistoryItem {
        constructor(mediaItem) {
            super(mediaItem);
        }
        getStorageType() {
            return IMediaServer.TStorageType.EPlaylist;
        }
    }
    // from mediacenter
    export class cLocalServer extends IMediaServer {
        _pluginObject;
        _name;
        constructor(pluginObject?) {
            super();
            if (pluginObject) {
                hx.log('dsrvMediaItemManager', 'new CLocalServer()');
                this._pluginObject = pluginObject;
            }
        }
        getId(): any {
            return this._pluginObject.id;
        }
        getName(): string {
            if (this._name === undefined) {
                var serverName = '';
                var label = this._pluginObject.label;
                var TStorageType = IMediaServer.TStorageType;
                var storageType = this.getStorageType();
                switch (storageType) {
                    case TStorageType.EInternalHdd:
                        if (hx.config.useInternalPvr === 1) {
                            serverName = hx.l('LOC_INTERNALHDD_ID');
                        } else {
                            serverName = label || hx.l('LOC_USB_ID');
                        }
                        break;
                    case TStorageType.EUsb:
                        serverName = label || hx.l('LOC_USB_ID');
                        break;
                    case TStorageType.EDetachableHdd:
                        serverName = hx.l('LOC_DETACHHDD_ID');
                        break;
                    case TStorageType.ESdCard:
                        serverName = label || hx.l('LOC_SDCARD_ID');
                        break;
                    default:
                        serverName = '';
                        break;
                }
            }
            else {
                serverName = this._name;
            }
            return serverName;
        }
        getStorageType(): any {
            var storageType;
            var TStorageType = IMediaServer.TStorageType;
            var usage = this._pluginObject.usage;
            var kind = this._pluginObject.kind;
            if (kind === 1) {
                if (usage === 1) {
                    storageType = TStorageType.EInternalHdd;
                }
                else if (usage === 3) {
                    storageType = TStorageType.EUsb;
                }
                else {
                    storageType = TStorageType.EDetachableHdd;
                }
            }
            else if (kind === 2) {
                storageType = TStorageType.EUsb;
            }
            else if (kind === 3) {
                storageType = TStorageType.ESdCard;
            }
            return storageType;
        }
        getStorageCssClass(): string {
            return IMediaServer.StorageCssClassList[this.getStorageType()];
        }
        getViewType(): number {
            return IMediaServer.TViewType.EFolderView;
        }
        getPath(contentType): string {
            var TContentType = IMediaItem.TContentType;
            var TStorageType = IMediaServer.TStorageType;
            var path = this._pluginObject.path;
            if ((this.getStorageType() === TStorageType.EInternalHdd) && (hx.config.useInternalPvr === 1)) {
                switch(contentType) {
                    case TContentType.EVideo:
                        path += '/Media/Video';
                        break;
                    case TContentType.EMusic:
                        path += '/Media/Music';
                        break;
                    case TContentType.EPhoto:
                        path += '/Media/Photo';
                        break;
                }
            }
            return path;
        }
        getLocationType(): number {
            return IMediaServer.TLocationType.ELocal;
        }
        setName(name): void {
            this._name = name;
        }
        isMounted(): boolean {
            return true;
        }
    }
    export class cSharedServer extends cLocalServer {
        constructor(pluginObject) {
            super(pluginObject);
            hx.log('dsrvMediaItemManager', 'new CSharedServer()');
        }
        getName(): string {
            return hx.l('LOC_DOWNLOADED_ID');
        }
        getPath(): string {
            return this._pluginObject.path + '/Media/Download';
        }
        getStorageType(): number {
            return IMediaServer.TStorageType.EShared;
        }
        getId(): string {
            return this._pluginObject.id + '_shared';
        }
    }
    export class cAirPlayItem extends CMediaItem {
        _url;
        _duration: number;
        _contentType: number;
        _title: string;
        constructor(url, contentType: number) {
            super();
            this._url = url;
            this._duration = 0;
            this._contentType = contentType;
            this._title = '';
        }
        getFullPath() {
            return this._url;
        }
        setTitle(title) {
            this._title = title;
        }
        getDuration() {
            return this._duration;
        }
        getTitle() {
            return '';
        }
        getContentType() {
            return this._contentType;
        }
        isOperationEnabled() {
            var operationEnabled = true;
            if (this._contentType !== IMediaItem.TContentType.EVideo) {
                operationEnabled = false;
            }
            return operationEnabled;
        }
        getLargeThumbnailUrl() {
            return '';
        }

        getThumbnailUrl() {
            return '';
        }
    }
    export class cDlnaItem extends CNetworkMediaItem {
        _pluginObject;
        constructor(pluginObject) {
            super(pluginObject);
            this._pluginObject = pluginObject;
        }
        isByteSeekAvailable(): boolean {
            var byteSeekAble = this._pluginObject.byteSeekAble;
            hx.log('dsrvDlna', 'isByteSeekAvailable() = ' + byteSeekAble);
            return byteSeekAble;
        }
        getFullPath(): any {
            return this.getImportUrl();
        }
        getImportUrl(): any {
            var importUrl = this._pluginObject.importURI;
            hx.log('dsrvDlna', 'getImportUrl() = ' + importUrl);
            var fnParsePhotoPath = function(url) {
                var prefix = 'hdlna://';
                var postfix = '_Prot';
                var ret = url;
                if (url.indexOf(prefix) === 0) {
                    ret = url.substring(prefix.length);
                    var postfixIndex = ret.lastIndexOf(postfix);
                    if (postfixIndex !== -1) {
                        ret = ret.substring(0, postfixIndex);
                    }
                }
                return ret;
            };
            if (this.getObjectType() === 'photo') {
                importUrl = fnParsePhotoPath(importUrl);
            }
            return importUrl;
        }
        getObjectType(): any {
            var objectType = this._pluginObject.objectType;
            hx.log('dsrvDlna', 'getObjectType() = ' + objectType);
            return objectType;
        }
        getContentType(): any {
            var TContentType = IMediaItem.TContentType;
            var contentType, objectType;
            objectType = this.getObjectType();
            switch(objectType) {
                case 'video':
                    contentType = TContentType.EVideo;
                    break;
                case 'music':
                    contentType = TContentType.EMusic;
                    break;
                case 'photo':
                    contentType = TContentType.EPhoto;
                    break;
                default:
                    contentType = TContentType.EUnknown;
                    break;
            }
            return contentType;
        }
        getFileType(): any {
            var TFileType = IMediaItem.TFileType;
            var fileType = TFileType.EFile;
            var objectType = this.getObjectType();
            if (objectType === 'folder' || objectType === 'server') {
                fileType = TFileType.EFolder;
            }
            return fileType;
        }
        getResolution(): any {
            var resolution = this._pluginObject.resolution;
            hx.log('dsrvDlna', 'getResolution() = ' + resolution);
            return resolution;
        }
        getTitle(): any {
            var title = this._pluginObject.title;
            hx.log('dsrvDlna', 'getTitle() = ' + title);
            return title;
        }
        getThumbnailUrl(): any {
            var thumbnailUrl = this._pluginObject.thumbnailUrl;
            hx.log('dsrvDlna', 'getThumbnailUrl() = ' + thumbnailUrl);
            return thumbnailUrl;
        }
        getPath(): any {
            return this._pluginObject.objectID;
        }
        getParentID(): any {
            return this._pluginObject.parentID;
        }
    }
}
export = mPrismModel;
