var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports"], function (require, exports) {
    /// <reference path="../def/sugar.d.ts" />
    /// <reference path="../def/prism.d.ts" />
    var mPrismModel;
    (function (mPrismModel) {
        (function (EHumaxRecommendationType) {
            EHumaxRecommendationType[EHumaxRecommendationType["ENone"] = 0] = "ENone";
            EHumaxRecommendationType[EHumaxRecommendationType["EPeopleLikeThese"] = 1] = "EPeopleLikeThese";
            EHumaxRecommendationType[EHumaxRecommendationType["EPopularThisWeek"] = 2] = "EPopularThisWeek";
            EHumaxRecommendationType[EHumaxRecommendationType["EDontMissIt"] = 3] = "EDontMissIt";
            EHumaxRecommendationType[EHumaxRecommendationType["EPopularTvSeries"] = 4] = "EPopularTvSeries";
        })(mPrismModel.EHumaxRecommendationType || (mPrismModel.EHumaxRecommendationType = {}));
        var EHumaxRecommendationType = mPrismModel.EHumaxRecommendationType;
        (function (EHomeDataType) {
            EHomeDataType[EHomeDataType["EContent"] = 1] = "EContent";
            EHomeDataType[EHomeDataType["EApp"] = 2] = "EApp";
            EHomeDataType[EHomeDataType["EMoreContent"] = 3] = "EMoreContent";
        })(mPrismModel.EHomeDataType || (mPrismModel.EHomeDataType = {}));
        var EHomeDataType = mPrismModel.EHomeDataType;
        (function (EImageVariant) {
            EImageVariant[EImageVariant["EColour_16_9"] = 1] = "EColour_16_9";
            EImageVariant[EImageVariant["EColour_1_1"] = 2] = "EColour_1_1";
            EImageVariant[EImageVariant["EWhite_16_9"] = 3] = "EWhite_16_9";
            EImageVariant[EImageVariant["EWhite_1_1"] = 4] = "EWhite_1_1";
        })(mPrismModel.EImageVariant || (mPrismModel.EImageVariant = {}));
        var EImageVariant = mPrismModel.EImageVariant;
        (function (eTChannelType) {
            eTChannelType[eTChannelType["ETV"] = 0] = "ETV";
            eTChannelType[eTChannelType["ERADIO"] = 1] = "ERADIO";
            eTChannelType[eTChannelType["EOTHER"] = 2] = "EOTHER";
        })(mPrismModel.eTChannelType || (mPrismModel.eTChannelType = {}));
        var eTChannelType = mPrismModel.eTChannelType;
        (function (eTRecordingShowType) {
            eTRecordingShowType[eTRecordingShowType["ELIVE"] = 0] = "ELIVE";
            eTRecordingShowType[eTRecordingShowType["EFIRST_RUN"] = 1] = "EFIRST_RUN";
            eTRecordingShowType[eTRecordingShowType["ERERUN"] = 2] = "ERERUN";
        })(mPrismModel.eTRecordingShowType || (mPrismModel.eTRecordingShowType = {}));
        var eTRecordingShowType = mPrismModel.eTRecordingShowType;
        (function (eTScheduledFactoryType) {
            eTScheduledFactoryType[eTScheduledFactoryType["ERECORDING"] = 0] = "ERECORDING";
            eTScheduledFactoryType[eTScheduledFactoryType["EREMIND"] = 1] = "EREMIND";
            eTScheduledFactoryType[eTScheduledFactoryType["EHTTP_LIVE_STREAMING"] = 4] = "EHTTP_LIVE_STREAMING";
            eTScheduledFactoryType[eTScheduledFactoryType["EPOWER_ON_TIMER"] = 6] = "EPOWER_ON_TIMER";
            eTScheduledFactoryType[eTScheduledFactoryType["EREGULAR_OTA"] = 7] = "EREGULAR_OTA";
            eTScheduledFactoryType[eTScheduledFactoryType["ESATRECORD"] = 8] = "ESATRECORD";
            eTScheduledFactoryType[eTScheduledFactoryType["ESCHEDULED_RECORDING_AOS_NOTIFY"] = 9] = "ESCHEDULED_RECORDING_AOS_NOTIFY";
            eTScheduledFactoryType[eTScheduledFactoryType["ESERIES_RECORDING"] = 10] = "ESERIES_RECORDING";
            eTScheduledFactoryType[eTScheduledFactoryType["EALTERNATIVE_RECORDING"] = 11] = "EALTERNATIVE_RECORDING";
            eTScheduledFactoryType[eTScheduledFactoryType["EALTERNATIVE_SERIES_RECORDING"] = 12] = "EALTERNATIVE_SERIES_RECORDING";
        })(mPrismModel.eTScheduledFactoryType || (mPrismModel.eTScheduledFactoryType = {}));
        var eTScheduledFactoryType = mPrismModel.eTScheduledFactoryType;
        (function (eTStorageType) {
            eTStorageType[eTStorageType["EMANUAL"] = 0] = "EMANUAL";
            eTStorageType[eTStorageType["EINTHDD"] = 1] = "EINTHDD";
            eTStorageType[eTStorageType["EINTSUBHDD"] = 2] = "EINTSUBHDD";
            eTStorageType[eTStorageType["EEXTHDD"] = 3] = "EEXTHDD";
            eTStorageType[eTStorageType["ERECSTORAGE"] = 5] = "ERECSTORAGE";
        })(mPrismModel.eTStorageType || (mPrismModel.eTStorageType = {}));
        var eTStorageType = mPrismModel.eTStorageType;
        (function (eTRecordingState) {
            eTRecordingState[eTRecordingState["ERECORDING_UNREALIZED"] = 0] = "ERECORDING_UNREALIZED";
            eTRecordingState[eTRecordingState["ERECORDING_SCHEDULED"] = 1] = "ERECORDING_SCHEDULED";
            eTRecordingState[eTRecordingState["ERECORDING_DEL_SCHEDULED"] = 2] = "ERECORDING_DEL_SCHEDULED";
            eTRecordingState[eTRecordingState["ERECORDING_REC_PRESTART"] = 3] = "ERECORDING_REC_PRESTART";
            eTRecordingState[eTRecordingState["ERECORDING_REC_ACQUIRING_RESOURCES"] = 4] = "ERECORDING_REC_ACQUIRING_RESOURCES";
            eTRecordingState[eTRecordingState["ERECORDING_REC_STARTED"] = 5] = "ERECORDING_REC_STARTED";
            eTRecordingState[eTRecordingState["ERECORDING_REC_UPDATED"] = 6] = "ERECORDING_REC_UPDATED";
            eTRecordingState[eTRecordingState["ERECORDING_REC_COMPLETED"] = 7] = "ERECORDING_REC_COMPLETED";
            eTRecordingState[eTRecordingState["ERECORDING_REC_PARTIALLY_COMPLETED"] = 8] = "ERECORDING_REC_PARTIALLY_COMPLETED";
            eTRecordingState[eTRecordingState["ERECORDING_TS_ACQUIRING_RESOURCES"] = 9] = "ERECORDING_TS_ACQUIRING_RESOURCES";
            eTRecordingState[eTRecordingState["ERECORDING_TS_STARTED"] = 10] = "ERECORDING_TS_STARTED";
            eTRecordingState[eTRecordingState["ERECORDING_ERROR"] = 11] = "ERECORDING_ERROR";
            eTRecordingState[eTRecordingState["ERECORDING_REC_START_ERROR"] = 12] = "ERECORDING_REC_START_ERROR";
            eTRecordingState[eTRecordingState["ERECORDING_REC_START_CONFLICT"] = 13] = "ERECORDING_REC_START_CONFLICT";
            eTRecordingState[eTRecordingState["ERECORDING_REC_LOAD"] = 14] = "ERECORDING_REC_LOAD";
        })(mPrismModel.eTRecordingState || (mPrismModel.eTRecordingState = {}));
        var eTRecordingState = mPrismModel.eTRecordingState;
        (function (eTRecordingError) {
            eTRecordingError[eTRecordingError["ERROR_REC_RESOURCE_LIMITATION"] = 0] = "ERROR_REC_RESOURCE_LIMITATION";
            eTRecordingError[eTRecordingError["ERROR_INSUFFICIENT_STORAGE"] = 1] = "ERROR_INSUFFICIENT_STORAGE";
            eTRecordingError[eTRecordingError["ERROR_TUNER_CONFLICT"] = 2] = "ERROR_TUNER_CONFLICT";
            eTRecordingError[eTRecordingError["ERROR_REC_DRM_RESTRICTION"] = 3] = "ERROR_REC_DRM_RESTRICTION";
            eTRecordingError[eTRecordingError["ERROR_REC_UNKNOWN"] = 4] = "ERROR_REC_UNKNOWN";
            eTRecordingError[eTRecordingError["ERROR_TS_RESOURCE_LIMITATION"] = 5] = "ERROR_TS_RESOURCE_LIMITATION";
            eTRecordingError[eTRecordingError["ERROR_TS_DRM_RESTRICTION"] = 6] = "ERROR_TS_DRM_RESTRICTION";
            eTRecordingError[eTRecordingError["ERROR_HAVE_ALTERNATIVE"] = 7] = "ERROR_HAVE_ALTERNATIVE";
            eTRecordingError[eTRecordingError["ERROR_TS_UNKNOWN"] = 8] = "ERROR_TS_UNKNOWN";
            eTRecordingError[eTRecordingError["ERROR_RFOTA"] = 9] = "ERROR_RFOTA";
            eTRecordingError[eTRecordingError["ERROR_NO_AV"] = 10] = "ERROR_NO_AV";
            eTRecordingError[eTRecordingError["ERROR_POWER_OFF"] = 11] = "ERROR_POWER_OFF";
            eTRecordingError[eTRecordingError["ERROR_REC_CAS"] = 12] = "ERROR_REC_CAS";
            eTRecordingError[eTRecordingError["ERROR_HIGHER_PRIORITY"] = 13] = "ERROR_HIGHER_PRIORITY";
            eTRecordingError[eTRecordingError["ERROR_TRACKING_FAIL"] = 14] = "ERROR_TRACKING_FAIL";
            eTRecordingError[eTRecordingError["ERROR_OFF_AIR"] = 15] = "ERROR_OFF_AIR";
            eTRecordingError[eTRecordingError["ERROR_SATIP_FAILED"] = 16] = "ERROR_SATIP_FAILED";
            eTRecordingError[eTRecordingError["ERROR_MAX_RECORD_LIST"] = 17] = "ERROR_MAX_RECORD_LIST";
        })(mPrismModel.eTRecordingError || (mPrismModel.eTRecordingError = {}));
        var eTRecordingError = mPrismModel.eTRecordingError;
        (function (eTRecordingCopyControl) {
            eTRecordingCopyControl[eTRecordingCopyControl["ERECORDING_COPYCONTROL_COPYOK"] = 0] = "ERECORDING_COPYCONTROL_COPYOK";
            eTRecordingCopyControl[eTRecordingCopyControl["ERECORDING_COPYCONTROL_NOFURTHERCOPY"] = 1] = "ERECORDING_COPYCONTROL_NOFURTHERCOPY";
            eTRecordingCopyControl[eTRecordingCopyControl["ERECORDING_COPYCONTROL_COPYONCE"] = 2] = "ERECORDING_COPYCONTROL_COPYONCE";
            eTRecordingCopyControl[eTRecordingCopyControl["ERECORDING_COPYCONTROL_COPYPROHIBITED"] = 3] = "ERECORDING_COPYCONTROL_COPYPROHIBITED";
        })(mPrismModel.eTRecordingCopyControl || (mPrismModel.eTRecordingCopyControl = {}));
        var eTRecordingCopyControl = mPrismModel.eTRecordingCopyControl;
        (function (eTRecordingFileOperationState) {
            eTRecordingFileOperationState[eTRecordingFileOperationState["ERECORDING_FILEOP_STATE_ING"] = 0] = "ERECORDING_FILEOP_STATE_ING";
            eTRecordingFileOperationState[eTRecordingFileOperationState["ERECORDING_FILEOP_STATE_COMPLETE"] = 1] = "ERECORDING_FILEOP_STATE_COMPLETE";
            eTRecordingFileOperationState[eTRecordingFileOperationState["ERECORDING_FILEOP_STATE_ERROR"] = 2] = "ERECORDING_FILEOP_STATE_ERROR";
            eTRecordingFileOperationState[eTRecordingFileOperationState["ERECORDING_FILEOP_STATE_UNKNOWN"] = 3] = "ERECORDING_FILEOP_STATE_UNKNOWN";
            eTRecordingFileOperationState[eTRecordingFileOperationState["ERECORDING_FILEOP_STATE_STOP"] = 4] = "ERECORDING_FILEOP_STATE_STOP";
            eTRecordingFileOperationState[eTRecordingFileOperationState["ERECORDING_FILEOP_NOT_ENOUGH_SPACE"] = 5] = "ERECORDING_FILEOP_NOT_ENOUGH_SPACE";
            eTRecordingFileOperationState[eTRecordingFileOperationState["ERECORDING_FILEOP_STATE_SIZE_LIMIT_4G"] = 6] = "ERECORDING_FILEOP_STATE_SIZE_LIMIT_4G";
        })(mPrismModel.eTRecordingFileOperationState || (mPrismModel.eTRecordingFileOperationState = {}));
        var eTRecordingFileOperationState = mPrismModel.eTRecordingFileOperationState;
        var IChannelGroupItem = (function () {
            function IChannelGroupItem() {
                this.getId = null;
                this.getName = null;
                this.getChannelItemList = null;
            }
            return IChannelGroupItem;
        })();
        mPrismModel.IChannelGroupItem = IChannelGroupItem;
        var IChannelItem = (function () {
            function IChannelItem() {
                this.getId = null;
                this.getNo = null;
                this.getName = null;
                this.getThumbnailUrl = null;
                this.isTunnable = null;
            }
            return IChannelItem;
        })();
        mPrismModel.IChannelItem = IChannelItem;
        var IProgrammeItem = (function () {
            function IProgrammeItem() {
                this.getId = null;
                this.getStartTime = null;
                this.getTitle = null;
                this.getThumbnailUrl = null;
            }
            return IProgrammeItem;
        })();
        mPrismModel.IProgrammeItem = IProgrammeItem;
        var IProgrammeDetailItem = (function () {
            function IProgrammeDetailItem() {
            }
            IProgrammeDetailItem.prototype.getThumbWidth = function () {
                return IProgrammeDetailItem.THUMB_WIDTH;
            };
            IProgrammeDetailItem.prototype.getLogoWidth = function () {
                return IProgrammeDetailItem.CHANNEL_LOGO_WIDTH;
            };
            IProgrammeDetailItem.CHANNEL_LOGO_WIDTH = 80;
            IProgrammeDetailItem.PROVIDER_LOGO_WIDTH = 100;
            IProgrammeDetailItem.THUMB_WIDTH = 400;
            IProgrammeDetailItem.type = {
                programme: 'programme',
                recording: 'recording',
                scheduled: 'scheduled',
                ondemand: 'ondemand'
            };
            return IProgrammeDetailItem;
        })();
        mPrismModel.IProgrammeDetailItem = IProgrammeDetailItem;
        var IPdeProgrammeItem = (function (_super) {
            __extends(IPdeProgrammeItem, _super);
            function IPdeProgrammeItem() {
                _super.apply(this, arguments);
            }
            IPdeProgrammeItem.prototype.getType = function () {
                return IProgrammeDetailItem.type.programme;
            };
            return IPdeProgrammeItem;
        })(IProgrammeDetailItem);
        mPrismModel.IPdeProgrammeItem = IPdeProgrammeItem;
        var IPdeRecordingItem = (function (_super) {
            __extends(IPdeRecordingItem, _super);
            function IPdeRecordingItem() {
                _super.apply(this, arguments);
            }
            IPdeRecordingItem.prototype.getType = function () {
                return IProgrammeDetailItem.type.recording;
            };
            return IPdeRecordingItem;
        })(IProgrammeDetailItem);
        mPrismModel.IPdeRecordingItem = IPdeRecordingItem;
        var IPdeScheduledItem = (function (_super) {
            __extends(IPdeScheduledItem, _super);
            function IPdeScheduledItem() {
                _super.apply(this, arguments);
            }
            IPdeScheduledItem.prototype.getType = function () {
                return IProgrammeDetailItem.type.scheduled;
            };
            return IPdeScheduledItem;
        })(IProgrammeDetailItem);
        mPrismModel.IPdeScheduledItem = IPdeScheduledItem;
        var IPdeOndemandItem = (function (_super) {
            __extends(IPdeOndemandItem, _super);
            function IPdeOndemandItem() {
                _super.apply(this, arguments);
            }
            IPdeOndemandItem.prototype.getType = function () {
                return IProgrammeDetailItem.type.ondemand;
            };
            IPdeOndemandItem.prototype.getLogoWidth = function () {
                return IProgrammeDetailItem.PROVIDER_LOGO_WIDTH;
            };
            return IPdeOndemandItem;
        })(IProgrammeDetailItem);
        mPrismModel.IPdeOndemandItem = IPdeOndemandItem;
        var IScheduledItem = (function () {
            function IScheduledItem() {
            }
            IScheduledItem.prototype.getId = function () {
                return 1;
            };
            IScheduledItem.prototype.getFullPath = function () {
                return "";
            };
            IScheduledItem.prototype.getStartTime = function () {
                return 1;
            };
            IScheduledItem.prototype.getDuration = function () {
                return 1;
            };
            IScheduledItem.prototype.getTitle = function () {
                return "";
            };
            IScheduledItem.prototype.getThumbnailUrl = function () {
                return "";
            };
            IScheduledItem.prototype.getRepeat = function () {
                return 1;
            };
            IScheduledItem.prototype.getChannelInstance = function () {
                return 1;
            };
            IScheduledItem.prototype.getChannelId = function () {
                return 1;
            };
            IScheduledItem.prototype.getChannelNo = function () {
                return 1;
            };
            IScheduledItem.prototype.getChannelName = function () {
                return "";
            };
            IScheduledItem.prototype.getLogoUrl = function (param) {
                return "";
            };
            IScheduledItem.prototype.getSeriesId = function () {
                return 1;
            };
            IScheduledItem.prototype.getGroupCRIDs = function () {
                return [];
            };
            IScheduledItem.prototype.getProgrammeCRID = function () {
                return 1;
            };
            IScheduledItem.prototype.getVolume = function () {
                return 0;
            };
            IScheduledItem.prototype.getLCN = function () {
                return 1;
            };
            IScheduledItem.prototype.setThumbnailUrl = function (aURL) {
            };
            IScheduledItem.prototype.getSeriesTitle = function () {
                return '';
            };
            IScheduledItem.prototype.getStorageId = function () {
                return 1;
            };
            // TODO: clarify necessity of storage information for scheduled item
            IScheduledItem.prototype.getStorageType = function () {
                return 1;
            };
            // internal, detachable, external
            IScheduledItem.prototype.getStorageName = function () {
                return "";
            };
            IScheduledItem.prototype.getProgrammeID = function () {
                return 1;
            };
            IScheduledItem.prototype.getCasIDs = function () {
                return 1;
            };
            IScheduledItem.prototype.isPendingSeriesSchedule = function () {
                return false;
            };
            IScheduledItem.prototype.isEqual = function (b) {
                return false;
            };
            IScheduledItem.prototype.isReminder = function () {
                return false;
            };
            IScheduledItem.prototype.isNCDChanged = function () {
                return false;
            };
            IScheduledItem.prototype.isSeries = function () {
                return false;
            };
            IScheduledItem.prototype.isRadio = function () {
                return false;
            };
            IScheduledItem.prototype.isLocked = function () {
                return false;
            };
            IScheduledItem.prototype.isScrambled = function () {
                return false;
            };
            IScheduledItem.prototype.isStreaming = function () {
                return false;
            };
            IScheduledItem.prototype.isRecordingOnGoing = function () {
                return false;
            };
            IScheduledItem.prototype.setRecordingOnGoing = function () {
            };
            IScheduledItem.prototype.isSeriesButNotRecoredAsSeries = function () {
                return false;
            };
            return IScheduledItem;
        })();
        mPrismModel.IScheduledItem = IScheduledItem;
        var IRecordingItem = (function (_super) {
            __extends(IRecordingItem, _super);
            function IRecordingItem() {
                _super.apply(this, arguments);
                this.audioType = null;
            }
            IRecordingItem.prototype.getError = function () {
            };
            IRecordingItem.prototype.getLastPlayPosition = function () {
            };
            IRecordingItem.prototype.getRecordingDuration = function () {
            };
            IRecordingItem.prototype.isNew = function () {
                return false;
            };
            IRecordingItem.prototype.isBroken = function () {
                return false;
            };
            IRecordingItem.prototype.isNoCopy = function () {
                return false;
            };
            IRecordingItem.prototype.getGroupCRIDs = function () {
                return [];
            };
            IRecordingItem.prototype.getProgrammeCRID = function () {
                return 1;
            };
            return IRecordingItem;
        })(IScheduledItem);
        mPrismModel.IRecordingItem = IRecordingItem;
        var fnCreateGuid = function () {
            return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
                var r = Math.random() * 16 | 0, v = c === 'x' ? r : (r & 0x3 | 0x8);
                return v.toString(16);
            });
        };
        var IMediaItem = (function () {
            function IMediaItem() {
            }
            //        getTitle: () => string;
            //        getThumbnailUrl: () => string;
            //        getFullPath: () => string;
            //        getPath: () => string;
            IMediaItem.TFileType = {
                EFile: 0,
                EFolder: 1,
                EGroup: 2
            };
            IMediaItem.TStorageType = {
                EUnknown: 0,
                ESata: 1,
                EUsb: 2,
                ESdCard: 3,
                ESamba: 4
            };
            IMediaItem.TContentType = {
                EVideo: 0,
                EMusic: 1,
                EPhoto: 2,
                EUnknown: 3
            };
            IMediaItem.TOperationState = {
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
            IMediaItem.ContentTypeCssList = ['video', 'music', 'photo'];
            IMediaItem.ExtensionList = [
                '3gp|3g2|mov|flv|swf|m2ts|rm|ts|mkv|avi|mp4|asf|wmv|tp|m2v|m4v|mts|mpg|vob|divx|mpeg|mpe|dat|ogm|ogg|rmvb',
                hx.config.CONFIG_HD_AUDIO ? 'mp3|wav|f4a|m4a|flac' : 'mp3|wav|f4a|m4a',
                'jpg|jpeg|bmp|gif|tif|png'
            ];
            return IMediaItem;
        })();
        mPrismModel.IMediaItem = IMediaItem;
        var TContentType = IMediaItem.TContentType;
        var CMediaItem = (function (_super) {
            __extends(CMediaItem, _super);
            function CMediaItem(aPluginObject) {
                _super.call(this);
                this._pluginObject = null;
                this._duration = 0;
                this._pluginObject = aPluginObject;
                this._duration = 0;
            }
            CMediaItem.prototype.getPluginObject = function () {
                return this._pluginObject;
            };
            CMediaItem.prototype.getContentType = function () {
                return this._pluginObject ? this._pluginObject.contentType : null;
            };
            CMediaItem.prototype.getFileType = function () {
                return this._pluginObject ? this._pluginObject.fileType : null;
            };
            CMediaItem.prototype.getFullPath = function () {
                return this._pluginObject ? this._pluginObject.fullPath : null;
            };
            CMediaItem.prototype.getDate = function () {
                return this._pluginObject ? this._pluginObject.date : null;
            };
            CMediaItem.prototype.getThumbnailResolution = function () {
                return this._pluginObject ? this._pluginObject.thumbResolution : null;
            };
            CMediaItem.prototype.getStartTime = function () {
                var a = this;
                return a._pluginObject ? (a._pluginObject.startTime || (new Date(1970, 1, 1, 0, 0, 0, 0)).toSeconds()) : (new Date(1970, 1, 1, 0, 0, 0, 0)).toSeconds();
            };
            CMediaItem.prototype.getDuration = function () {
                return this._pluginObject ? (this._pluginObject.duration || this._duration) : this._duration;
            };
            CMediaItem.prototype.setDuration = function (duration) {
                this._duration = duration;
            };
            CMediaItem.prototype.getLastPlayPosition = function () {
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
            };
            CMediaItem.prototype.getLargeThumbnailUrl = function () {
                return this._pluginObject ? (this._pluginObject.largeThumbnailUrl ? this._pluginObject.largeThumbnailUrl : '') : '';
            };
            CMediaItem.prototype.getThumbnailUrl = function () {
                return this._pluginObject ? this._pluginObject.thumbnailUrl : '';
            };
            CMediaItem.prototype.getResolution = function () {
                return this._pluginObject ? this._pluginObject.resolution : null;
            };
            CMediaItem.prototype.getPictureDate = function () {
                return this._pluginObject ? this._pluginObject.pictureDate : null;
            };
            CMediaItem.prototype.getTitle = function () {
                var a = this;
                var title = a._pluginObject ? a._pluginObject.title : '';
                var fullPath = a.getFullPath();
                var startIndex = fullPath.lastIndexOf('/') + 1;
                var path = fullPath.substring(startIndex);
                return (title === null || title === undefined || title === '') ? path : title;
            };
            CMediaItem.prototype.getGenre = function () {
                return this._pluginObject ? this._pluginObject.genre : '';
            };
            CMediaItem.prototype.getAlbumName = function () {
                return this._pluginObject ? this._pluginObject.album : '';
            };
            CMediaItem.prototype.getArtistName = function () {
                var artist = this._pluginObject ? this._pluginObject.artist : '';
                if (artist !== "" && !artist) {
                    artist = 0;
                }
                return artist;
            };
            CMediaItem.prototype.isWatched = function () {
                return this._pluginObject ? this._pluginObject.watched : null;
            };
            CMediaItem.prototype.getStorageType = function () {
                return this._pluginObject ? this._pluginObject.storageCategory : null;
            };
            CMediaItem.prototype.getPath = function () {
                return this.getFullPath();
            };
            CMediaItem.prototype.setParent = function (parent) {
                this._parent = parent;
            };
            CMediaItem.prototype.setLastPlayPosition = function (time) {
                this._pluginObject.lastViewTime = time;
            };
            CMediaItem.prototype.isOperationEnabled = function () {
                return true;
            };
            CMediaItem.prototype.getBitRate = function () {
                return this._pluginObject ? this._pluginObject.bitRate : null;
            };
            CMediaItem.prototype.getSampleRate = function () {
                return this._pluginObject ? this._pluginObject.sampleRate : null;
            };
            return CMediaItem;
        })(IMediaItem);
        mPrismModel.CMediaItem = CMediaItem;
        var CNetworkMediaItem = (function (_super) {
            __extends(CNetworkMediaItem, _super);
            function CNetworkMediaItem(pluginObject) {
                _super.call(this, pluginObject);
            }
            return CNetworkMediaItem;
        })(CMediaItem);
        mPrismModel.CNetworkMediaItem = CNetworkMediaItem;
        var CUpperFolderItem = (function (_super) {
            __extends(CUpperFolderItem, _super);
            function CUpperFolderItem(pluginObject) {
                _super.call(this, pluginObject);
            }
            CUpperFolderItem.prototype.getTitle = function () {
                return tr('LOC_STR_UPPER_FOLDER_ID');
            };
            CUpperFolderItem.prototype.getFileType = function () {
                return IMediaItem.TFileType.EFolder;
            };
            CUpperFolderItem.prototype.getFullPath = function () {
                var a = this;
                var fullPath = a._pluginObject.parentID;
                if (fullPath === undefined || fullPath === null) {
                    fullPath = CMediaItem.prototype.getFullPath.call(a);
                    fullPath = fullPath.substring(0, fullPath.lastIndexOf('/'));
                }
                return fullPath;
            };
            return CUpperFolderItem;
        })(CMediaItem);
        mPrismModel.CUpperFolderItem = CUpperFolderItem;
        var CMoreMediaItem = (function (_super) {
            __extends(CMoreMediaItem, _super);
            function CMoreMediaItem(pluginObject) {
                _super.call(this, pluginObject);
            }
            CMoreMediaItem.prototype.getId = function () {
                return mPrismModel.__recentGuid__;
            };
            CMoreMediaItem.prototype.getTitle = function () {
                return '';
            };
            CMoreMediaItem.prototype.getFileType = function () {
                return IMediaItem.TFileType.EFile;
            };
            CMoreMediaItem.prototype.getFullPath = function () {
                return '';
            };
            CMoreMediaItem.prototype.getThumbnailUrl = function () {
            };
            CMoreMediaItem.prototype.getArtistName = function () {
                return '';
            };
            return CMoreMediaItem;
        })(CMediaItem);
        mPrismModel.CMoreMediaItem = CMoreMediaItem;
        var IMediaServer = (function () {
            function IMediaServer() {
            }
            //        getId = null;
            //        getName = null;
            //        getStorageType = null;
            //        getStorageCssClass = null;
            //        getViewType = null;
            //        getLocationType = null;
            //        getPath = null;
            //        isMounted = null;
            IMediaServer.TStorageType = {
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
            IMediaServer.TViewType = {
                EOneDepthView: 0,
                EFolderView: 1
            };
            IMediaServer.TLocationType = {
                ELocal: 0,
                ENetwork: 1,
                EUser: 2
            };
            IMediaServer.StorageCssClassList = ['hdd', 'dtc', 'usb', 'sdm', 'dlna', 'smb', 'woon', 'recent', 'playlist', 'artist', 'album', 'shared'];
            return IMediaServer;
        })();
        mPrismModel.IMediaServer = IMediaServer;
        if (hx.config.useInternalPvr === 1) {
            IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EInternalHdd] = 'hdd';
        }
        else {
            IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EInternalHdd] = 'usbp';
        }
        var CNetworkServer = (function (_super) {
            __extends(CNetworkServer, _super);
            function CNetworkServer() {
                _super.call(this);
            }
            return CNetworkServer;
        })(IMediaServer);
        mPrismModel.CNetworkServer = CNetworkServer;
        //    CNetworkMediaItem.prototype.constructor = IMediaServer;
        mPrismModel.__recentGuid__ = fnCreateGuid();
        mPrismModel.__playlistGuid__ = fnCreateGuid();
        mPrismModel.__artistGuid__ = fnCreateGuid();
        mPrismModel.__albumGuid__ = fnCreateGuid();
        var CUserMediaServer = (function (_super) {
            __extends(CUserMediaServer, _super);
            function CUserMediaServer(pluginObject) {
                _super.call(this);
                this._pluginObject = pluginObject;
            }
            CUserMediaServer.prototype.getId = function () {
                return this._pluginObject.id;
            };
            CUserMediaServer.prototype.getName = function () {
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
            };
            CUserMediaServer.prototype.getStorageType = function () {
                return this._pluginObject.storageType;
            };
            CUserMediaServer.prototype.getStorageCssClass = function () {
                return this._pluginObject.storageCssClass;
            };
            CUserMediaServer.prototype.getViewType = function () {
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
            };
            CUserMediaServer.prototype.getLocationType = function () {
                return IMediaServer.TLocationType.EUser;
            };
            CUserMediaServer.prototype.getPath = function () {
                return '';
            };
            CUserMediaServer.prototype.isMounted = function () {
                return true;
            };
            return CUserMediaServer;
        })(IMediaServer);
        mPrismModel.CUserMediaServer = CUserMediaServer;
        var CMediaItemByUrl = (function (_super) {
            __extends(CMediaItemByUrl, _super);
            function CMediaItemByUrl(title, url) {
                _super.call(this, {});
                this.mUrl = url;
                this.mDuration = 0;
                this.mTitle = title;
            }
            CMediaItemByUrl.prototype.getFullPath = function () {
                return this.mUrl;
            };
            CMediaItemByUrl.prototype.setDuration = function (duration) {
                this.mDuration = Math.floor(duration / 1000);
            };
            CMediaItemByUrl.prototype.getDuration = function () {
                return this.mDuration;
            };
            CMediaItemByUrl.prototype.getTitle = function () {
                return this.mTitle;
            };
            CMediaItemByUrl.prototype.getContentType = function () {
                return IMediaItem.TContentType.EVideo;
            };
            return CMediaItemByUrl;
        })(CMediaItem);
        mPrismModel.CMediaItemByUrl = CMediaItemByUrl;
        var recentItem = new CUserMediaServer({
            id: mPrismModel.__recentGuid__,
            storageType: IMediaServer.TStorageType.ERecent,
            storageCssClass: IMediaServer.StorageCssClassList[IMediaServer.TStorageType.ERecent]
        });
        var playlistItem = new CUserMediaServer({
            id: mPrismModel.__playlistGuid__,
            storageType: IMediaServer.TStorageType.EPlaylist,
            storageCssClass: IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EPlaylist]
        });
        var artistItem = new CUserMediaServer({
            id: mPrismModel.__artistGuid__,
            storageType: IMediaServer.TStorageType.EArtist,
            storageCssClass: IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EArtist]
        });
        var albumItem = new CUserMediaServer({
            id: mPrismModel.__albumGuid__,
            storageType: IMediaServer.TStorageType.EAlbum,
            storageCssClass: IMediaServer.StorageCssClassList[IMediaServer.TStorageType.EAlbum]
        });
        mPrismModel.__photoUserMediaServerList__ = [recentItem, playlistItem];
        mPrismModel.__videoUserMediaServerList__ = [recentItem];
        mPrismModel.__musicUserMediaServerList__ = mPrismModel.__photoUserMediaServerList__.slice(0);
        //__musicUserMediaServerList__.push(artistItem);
        //__musicUserMediaServerList__.push(albumItem);
        // these are not supported at this time.
        //from dsrvRecordings2
        var COipfScheduledItem = (function (_super) {
            __extends(COipfScheduledItem, _super);
            function COipfScheduledItem(aProperty) {
                _super.call(this);
                this._recordingOnGoing = false;
                this._property = aProperty;
            }
            COipfScheduledItem.prototype.getId = function () {
                var that = this;
                if (that._id === undefined) {
                    that._id = that._property.id;
                }
                return that._id;
            };
            COipfScheduledItem.prototype.getFullPath = function () {
                return this.getId();
            };
            COipfScheduledItem.prototype.getStartTime = function () {
                var that = this;
                if (that._startTime === undefined) {
                    that._startTime = that._property.startTime;
                }
                return new Date(that._startTime * 1000);
            };
            COipfScheduledItem.prototype.getDuration = function () {
                var that = this;
                if (that._duration === undefined) {
                    that._duration = that._property.duration;
                }
                return that._duration;
            };
            COipfScheduledItem.prototype.getEndTime = function () {
                var that = this;
                if (that._startTime === undefined) {
                    that._startTime = that._property.startTime;
                }
                if (that._duration === undefined) {
                    that._duration = that._property.duration;
                }
                return new Date((that._startTime + that._duration) * 1000);
            };
            COipfScheduledItem.prototype.getTitle = function () {
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
            };
            COipfScheduledItem.prototype.getThumbnailUrl = function () {
                return this._thumbnailUrl;
            };
            COipfScheduledItem.prototype.getShortDescription = function () {
                return this._shortDescription;
            };
            COipfScheduledItem.prototype.getLongDescription = function () {
                return this._longDescription;
            };
            COipfScheduledItem.prototype.getGuidance = function () {
                return this._guidance;
            };
            COipfScheduledItem.prototype.getRepeat = function () {
                var that = this;
                if (that._repeatDays === undefined) {
                    that._repeatDays = that._property.repeatDays;
                }
                return that._repeatDays;
            };
            COipfScheduledItem.prototype.getChannelInstance = function () {
                var that = this;
                if (!that._channel) {
                    that._channel = that._property.channel;
                }
                return that._channel;
            };
            COipfScheduledItem.prototype.getChannelId = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().ccid : '';
            };
            COipfScheduledItem.prototype.getServiceId = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().sid : null;
            };
            COipfScheduledItem.prototype.getChannelType = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().channelType : 0;
            };
            COipfScheduledItem.prototype.getChannelNo = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().majorChannel : 0;
            };
            COipfScheduledItem.prototype.getChannelName = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().name : '';
            };
            COipfScheduledItem.prototype.getSeriesId = function () {
                var that = this;
                return that.getGroupCRIDs() ? that.getGroupCRIDs()[0] : null;
            };
            COipfScheduledItem.prototype.getGroupCRIDs = function () {
                var that = this;
                if (that._groupCRIDs === undefined) {
                    that._groupCRIDs = that._property.groupCRIDs;
                }
                return that._groupCRIDs || null;
            };
            COipfScheduledItem.prototype.getProgrammeCRID = function () {
                var that = this;
                if (that._programmeCRID === undefined) {
                    that._programmeCRID = that._property.programmeCRID;
                }
                return that._programmeCRID || null;
            };
            COipfScheduledItem.prototype.getSeriesTitle = function () {
                return this.getTitle();
            };
            COipfScheduledItem.prototype.getStorageId = function () {
                var that = this;
                if (that._discId === undefined) {
                    that._discId = that._property.discId;
                }
                return that._discId;
            };
            COipfScheduledItem.prototype.getStorageType = function () {
                return 'internal';
            };
            COipfScheduledItem.prototype.getStorageName = function () {
                var that = this, name;
                if (hx.config.useInternalPvr === 1) {
                    name = hx.l('LOC_INTERNALHDD_ID');
                }
                else {
                    name = hx.l('LOC_EXTERNAL_HDD_ID');
                }
                if (that._discId) {
                    var storageInfo = that._fnGetStorageInfo(that._discId);
                    if (storageInfo) {
                        switch (storageInfo.usage) {
                            case 1 /* EINTHDD */:
                                if (hx.config.useInternalPvr === 1) {
                                    name = hx.l('LOC_INTERNALHDD_ID');
                                }
                                else {
                                    //name = tr('LOC_EXTERNAL_HDD_ID');
                                    name = storageInfo.label;
                                }
                                break;
                            case 2 /* EINTSUBHDD */:
                                if (hx.config.useInternalPvr === 1) {
                                    name = hx.l('LOC_DETACHHDD_ID');
                                }
                                else {
                                    //name = tr('LOC_EXTERNAL_HDD_ID');
                                    name = storageInfo.label;
                                }
                                break;
                        }
                    }
                }
                return name;
            };
            COipfScheduledItem.prototype.getProgrammeID = function () {
                var that = this;
                if (that._programmeID === undefined) {
                    that._programmeID = that._property.programmeID;
                }
                return that._programmeID || null;
            };
            COipfScheduledItem.prototype.getCasIDs = function () {
                return [];
            };
            COipfScheduledItem.prototype.getFactoryType = function () {
                var that = this;
                if (that._factoryType === undefined) {
                    that._factoryType = that._property.factoryType;
                }
                return that._factoryType;
            };
            COipfScheduledItem.prototype.getFactoryTypeStr = function () {
                var that = this;
                switch (that.getFactoryType()) {
                    case 0 /* ERECORDING */:
                        return "Record";
                    case 1 /* EREMIND */:
                        return "Remind";
                    case 4 /* EHTTP_LIVE_STREAMING */:
                        return "Http Live Streaming";
                    case 6 /* EPOWER_ON_TIMER */:
                        return "Power On Timer";
                    case 8 /* ESATRECORD */:
                        return "Sat Record";
                    case 10 /* ESERIES_RECORDING */:
                        return "Series Record";
                    default:
                        return that.getFactoryType();
                }
            };
            COipfScheduledItem.prototype.getStartPadding = function () {
                var that = this;
                if (that._startPadding === undefined) {
                    that._startPadding = that._property.startPadding;
                }
                return that._startPadding;
            };
            COipfScheduledItem.prototype.getEndPadding = function () {
                var that = this;
                if (that._endPadding === undefined) {
                    that._endPadding = that._property.endPadding;
                }
                return that._endPadding;
            };
            COipfScheduledItem.prototype.isScheduledAsSeries = function () {
                var that = this;
                if (that._isScheduledAsSeries === undefined) {
                    that._isScheduledAsSeries = that._property.isScheduledAsSeries;
                }
                return that._isScheduledAsSeries;
            };
            COipfScheduledItem.prototype.isPendingSeriesSchedule = function () {
                var that = this;
                if (that._startTime === undefined) {
                    that._startTime = that._property.startTime;
                }
                var nowSec = hx.il.nowSec();
                return !!(((that._startTime + that.getDuration() + that.getEndPadding()) < nowSec) && (that.isScheduledAsSeries()));
            };
            COipfScheduledItem.prototype.isRecording = function () {
                return (this.getFactoryType() === 0 /* ERECORDING */) || false;
            };
            COipfScheduledItem.prototype.isReminder = function () {
                return (this.getFactoryType() === 1 /* EREMIND */) || false;
            };
            COipfScheduledItem.prototype.isNCDChanged = function () {
                return (this.getFactoryType() === 9 /* ESCHEDULED_RECORDING_AOS_NOTIFY */) || false;
            };
            COipfScheduledItem.prototype.isPowerOnTimer = function () {
                return (this.getFactoryType() === 6 /* EPOWER_ON_TIMER */) || false;
            };
            COipfScheduledItem.prototype.isRegularOTA = function () {
                return (this.getFactoryType() === 7 /* EREGULAR_OTA */) || false;
            };
            COipfScheduledItem.prototype.getLCN = function () {
                return this.getChannelNo();
            };
            COipfScheduledItem.prototype.GetVolume = function () {
                return this._property.volume;
            };
            COipfScheduledItem.prototype.hasGroupCRIDs = function () {
                var that = this;
                if (that.getGroupCRIDs() && that.getGroupCRIDs().length) {
                    return true;
                }
                return false;
            };
            COipfScheduledItem.prototype.isSeries = function () {
                return this.hasGroupCRIDs();
            };
            COipfScheduledItem.prototype.isRadio = function () {
                return (this.getChannelType() === 1 /* ERADIO */) || false;
            };
            COipfScheduledItem.prototype.isLocked = function () {
                return false;
            };
            COipfScheduledItem.prototype.isScrambled = function () {
                return false;
            };
            COipfScheduledItem.prototype.isStreaming = function () {
                return (this.getFactoryType() === 4 /* EHTTP_LIVE_STREAMING */) || false;
            };
            COipfScheduledItem.prototype.isRecordingOnGoing = function (bOnlyCheckFlag) {
                if (this._recordingOnGoing) {
                    return true;
                }
                if (bOnlyCheckFlag) {
                    return false;
                }
                var that = this;
                if (that.isPendingSeriesSchedule()) {
                    return false;
                }
                else {
                    if (that._startTime === undefined) {
                        that._startTime = that._property.startTime;
                    }
                    var currentSec = hx.il.nowSec();
                    return !!(currentSec >= ((that._startTime - that.getStartPadding())));
                }
            };
            COipfScheduledItem.prototype.isEqual = function (b) {
                var a = this;
                var bFoundSameSeries = false;
                var match = false;
                if (a.getFactoryType() === 4 /* EHTTP_LIVE_STREAMING */ && b.getFactoryType() === 4 /* EHTTP_LIVE_STREAMING */) {
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
                }
                else {
                    if (a.isPendingSeriesSchedule()) {
                        if (a.getGroupCRIDs() && b.getGroupCRIDs()) {
                            var len = b.getGroupCRIDs().length, i;
                            for (i = 0; i < len; i++) {
                                if ((Array.prototype.slice.call(a.getGroupCRIDs())).indexOf(b.getGroupCRIDs()[i]) > -1) {
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
                if (a.getUniqueID() === b.getUniqueID()) {
                    match = true;
                }
                return match;
            };
            COipfScheduledItem.prototype.getUniqueID = function () {
                var that = this;
                if (that._uid === undefined) {
                    that._uid = that._property.uid;
                }
                return that._uid;
            };
            COipfScheduledItem.prototype.getAudioType = function () {
                var that = this;
                if (that._audioType === undefined) {
                    that._audioType = that._property.audioType;
                }
                return that._audioType;
            };
            COipfScheduledItem.prototype.getLogoUrl = function (width) {
                var that = this;
                if (!that._logoURL) {
                    that._logoURL = {};
                }
                return that._logoURL[width] || '';
            };
            COipfScheduledItem.prototype.setLogoUrl = function (aUrl, width) {
                var that = this;
                if (!that._logoURL) {
                    that._logoURL = {};
                }
                that._logoURL[width] = aUrl;
            };
            COipfScheduledItem.prototype.setRecordingOnGoing = function () {
                this._recordingOnGoing = true;
            };
            COipfScheduledItem.prototype.setThumbnailUrl = function (url) {
                this._thumbnailUrl = url;
            };
            COipfScheduledItem.prototype.setShortDescription = function (desc) {
                this._shortDescription = desc || '';
            };
            COipfScheduledItem.prototype.setLongDescription = function (desc) {
                this._longDescription = desc || '';
            };
            COipfScheduledItem.prototype.setGuidance = function (desc) {
                this._guidance = desc;
            };
            COipfScheduledItem.prototype.getInstance = function () {
                return this._property;
            };
            COipfScheduledItem.prototype.getTuningInfo = function () {
                return {
                    diseqcInput: this._property.diseqcInput,
                    frequency: this._property.frequency,
                    polarisation: this._property.polarisation,
                    symbolRate: this._property.symbolRate,
                    satType: this._property.satType
                };
            };
            COipfScheduledItem.prototype._fnGetStorageInfo = function (uuid) {
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
            };
            return COipfScheduledItem;
        })(IScheduledItem);
        mPrismModel.COipfScheduledItem = COipfScheduledItem;
        var COipfRecordingItem = (function (_super) {
            __extends(COipfRecordingItem, _super);
            function COipfRecordingItem(aProperty) {
                _super.call(this);
                this._property = aProperty;
            }
            COipfRecordingItem.prototype.getId = function () {
                var that = this;
                if (that._id === undefined) {
                    that._id = that._property.id;
                }
                return that._id;
            };
            COipfRecordingItem.prototype.getEpisode = function () {
                return this._property.episode;
            };
            COipfRecordingItem.prototype.getSeason = function () {
                return this._property.season;
            };
            COipfRecordingItem.prototype.getFullPath = function () {
                return this.getId();
            };
            COipfRecordingItem.prototype.getStartTime = function () {
                var that = this;
                if (that._startTime === undefined) {
                    that._startTime = that._property.recordingStartTime;
                }
                return new Date(that._startTime * 1000);
            };
            COipfRecordingItem.prototype.getDuration = function () {
                var that = this;
                if (that._duration === undefined) {
                    that._duration = that._property.duration;
                }
                return that._duration;
            };
            COipfRecordingItem.prototype.getEndTime = function () {
                var that = this;
                if (that._startTime === undefined) {
                    that._startTime = that._property.recordingStartTime;
                }
                if (that._duration === undefined) {
                    that._duration = that._property.duration;
                }
                return new Date((that._startTime + that._duration) * 1000);
            };
            COipfRecordingItem.prototype.getRecordingDuration = function () {
                var that = this;
                if (that._recordingDuration === undefined) {
                    that._recordingDuration = that._property.recordingDuration;
                }
                return that._recordingDuration;
            };
            COipfRecordingItem.prototype.getTitle = function () {
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
            };
            COipfRecordingItem.prototype.getThumbnailUrl = function () {
                var that = this;
                if (!that._thumbnailURL) {
                    that._thumbnailURL = this._property.thumbnail;
                }
                var thumbnailUrl = that._thumbnailURL;
                if (that.isLocked()) {
                    thumbnailUrl = mPrismModel.TDefaultThumbnailList.RECORDING;
                }
                else if (that.isNew() && that.isBroken()) {
                    thumbnailUrl = mPrismModel.TDefaultThumbnailList.REC_FAIL;
                }
                else {
                }
                return thumbnailUrl;
            };
            COipfRecordingItem.prototype.getLogoUrl = function (width) {
                var that = this;
                if (!that._logoURL) {
                    that._logoURL = {};
                }
                return that._logoURL[width] || '';
            };
            COipfRecordingItem.prototype.setLogoUrl = function (aUrl, width) {
                var that = this;
                if (!that._logoURL) {
                    that._logoURL = {};
                }
                that._logoURL[width] = aUrl;
            };
            COipfRecordingItem.prototype.getRepeat = function () {
                var that = this;
                if (that._repeatDays === undefined) {
                    that._repeatDays = that._property.repeatDays;
                }
                return that._repeatDays;
            };
            COipfRecordingItem.prototype.getShortDescription = function () {
                var that = this;
                if (that._shortDescription === undefined) {
                    that._shortDescription = that._property.description;
                }
                return that._shortDescription;
            };
            COipfRecordingItem.prototype.getLongDescription = function () {
                var that = this;
                if (that._longDescription === undefined) {
                    that._longDescription = that._property.longDescription;
                }
                return that._longDescription;
            };
            COipfRecordingItem.prototype.getChannelInstance = function () {
                var that = this;
                if (that._channel === undefined) {
                    that._channel = that._property.channel;
                }
                return that._channel;
            };
            COipfRecordingItem.prototype.getChannelId = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().ccid : '';
            };
            COipfRecordingItem.prototype.getServiceId = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().sid : null;
            };
            COipfRecordingItem.prototype.getChannelType = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().channelType : 0;
            };
            COipfRecordingItem.prototype.getChannelNo = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().majorChannel : 0;
            };
            COipfRecordingItem.prototype.getChannelName = function () {
                var that = this;
                return that.getChannelInstance() ? that.getChannelInstance().name : '';
            };
            COipfRecordingItem.prototype.getSeriesId = function () {
                var that = this;
                return that.getGroupCRIDs() ? that.getGroupCRIDs()[0] : null;
            };
            COipfRecordingItem.prototype.getGroupCRIDs = function () {
                var that = this;
                if (that._groupCRIDs === undefined) {
                    that._groupCRIDs = that._property.groupCRIDs;
                }
                return that._groupCRIDs;
            };
            COipfRecordingItem.prototype.getProgrammeCRID = function () {
                var that = this;
                if (that._programmeCRID === undefined) {
                    that._programmeCRID = that._property.name;
                }
                return that._programmeCRID;
            };
            //        getRecommandationCRID(): any {
            //            return this._property.recommandationCRID;
            //        }
            COipfRecordingItem.prototype.getSeriesTitle = function () {
                var that = this;
                if (that._name === undefined) {
                    that._name = that._property.name;
                }
                return that._name;
            };
            COipfRecordingItem.prototype.getContentType = function () {
                return IMediaItem.TContentType.EVideo;
            };
            COipfRecordingItem.prototype.getStorageId = function () {
                var that = this;
                if (that._discId === undefined) {
                    that._discId = that._property.discId;
                }
                return that._discId;
            };
            COipfRecordingItem.prototype.getSubtitleTypes = function () {
                var that = this;
                if (that._subtitleTypes === undefined) {
                    that._subtitleTypes = that._property.subtitleTypes;
                }
                return that._subtitleTypes;
            };
            COipfRecordingItem.prototype.getStorageType = function () {
                var that = this;
                var type = 'internal';
                if (that._discId) {
                    var i, storageInfo = this._fnGetStorageInfo(that._discId);
                    if (storageInfo) {
                        switch (storageInfo.usage) {
                            case 1 /* EINTHDD */:
                                type = 'internal';
                                break;
                            case 2 /* EINTSUBHDD */:
                                type = 'detachable';
                                break;
                        }
                    }
                }
                return type;
            };
            COipfRecordingItem.prototype._fnGetStorageInfo = function (uuid) {
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
            };
            COipfRecordingItem.prototype._fnGetRecStorageInfo = function (uuid) {
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
            };
            COipfRecordingItem.prototype.getStorageName = function () {
                var a = this, name;
                if (hx.config.useInternalPvr === 1) {
                    name = hx.l('LOC_INTERNALHDD_ID');
                }
                else {
                    name = hx.l('LOC_EXTERNAL_HDD_ID');
                }
                if (this._property.discId) {
                    var i, storageInfo = this._fnGetRecStorageInfo(this._property.discId);
                    if (storageInfo) {
                        switch (storageInfo.usage) {
                            case 1 /* EINTHDD */:
                                if (hx.config.useInternalPvr === 1) {
                                    name = hx.l('LOC_INTERNALHDD_ID');
                                }
                                else {
                                    //name = tr('LOC_EXTERNAL_HDD_ID');
                                    name = storageInfo.label;
                                }
                                break;
                            case 2 /* EINTSUBHDD */:
                                if (hx.config.useInternalPvr === 1) {
                                    name = hx.l('LOC_DETACHHDD_ID');
                                }
                                else {
                                    //name = tr('LOC_EXTERNAL_HDD_ID');
                                    name = storageInfo.label;
                                }
                                break;
                            case 5 /* ERECSTORAGE */:
                                name = storageInfo.label;
                                break;
                        }
                    }
                }
                return name;
            };
            COipfRecordingItem.prototype.getCasIDs = function () {
                return [];
            };
            COipfRecordingItem.prototype.getGenre = function () {
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
                        }
                        else {
                            if (TMinorGenreType.length === 0) {
                                if (TMajorGenreType[major] !== '') {
                                    genres.push(hx.l(TMajorGenreType[major]));
                                }
                            }
                        }
                    }
                }
                return genres;
            };
            COipfRecordingItem.prototype.getProgrammeID = function () {
                var that = this;
                if (that._programmeID === undefined) {
                    that._programmeID = that._property.programmeID;
                }
                return that._programmeID;
            };
            COipfRecordingItem.prototype.getLastPlayPosition = function () {
                var that = this;
                if (that._lastPlayPosition === undefined) {
                    that._lastPlayPosition = that._property.lastPlayPosition;
                }
                return that._lastPlayPosition;
            };
            COipfRecordingItem.prototype.getError = function () {
                return this._property.error;
            };
            COipfRecordingItem.prototype.hasGroupCRIDs = function () {
                var that = this;
                if (that.getGroupCRIDs() && that.getGroupCRIDs().length) {
                    return true;
                }
                return false;
            };
            COipfRecordingItem.prototype.isHd = function () {
                var that = this;
                if (that._isHD === undefined) {
                    that._isHD = that._property.isHD;
                }
                return that._isHD;
            };
            COipfRecordingItem.prototype.isAd = function () {
                var that = this;
                if (that._isAD === undefined) {
                    that._isAD = that._property.getField("freeview/hasAudioDescription");
                }
                return !!(that._isAD === 'true');
            };
            COipfRecordingItem.prototype.getAudioType = function () {
                var that = this;
                if (that._audioType === undefined) {
                    that._audioType = that._property.audioType;
                }
                return that._audioType;
            };
            COipfRecordingItem.prototype.isSeries = function () {
                return this.hasGroupCRIDs();
            };
            COipfRecordingItem.prototype.isRadio = function () {
                return (this.getChannelType() === 1 /* ERADIO */) || false;
            };
            COipfRecordingItem.prototype.isNew = function () {
                var that = this;
                if (that._showType === undefined) {
                    that._showType = that._property.showType;
                }
                return (that._showType === 1 /* EFIRST_RUN */) || false;
            };
            COipfRecordingItem.prototype.isBroken = function () {
                var that = this;
                if (that._state === undefined) {
                    that._state = that._property.state;
                }
                return (that._state === 8 /* ERECORDING_REC_PARTIALLY_COMPLETED */) || false;
            };
            COipfRecordingItem.prototype.isNoCopy = function () {
                var that = this;
                if (that._copyControl === undefined) {
                    that._copyControl = that._property.copyControl;
                }
                return (that._copyControl !== 0 /* ERECORDING_COPYCONTROL_COPYOK */) || false;
            };
            COipfRecordingItem.prototype.isLocked = function () {
                var that = this;
                if (that._isManualLocked === undefined) {
                    that._isManualLocked = that._property.isManualLocked;
                }
                return that._isManualLocked || false;
            };
            COipfRecordingItem.prototype.isKept = function () {
                return this._property.doNotDelete || false;
            };
            COipfRecordingItem.prototype.setKeep = function (aKeep) {
                this._property.doNotDelete = aKeep;
            };
            COipfRecordingItem.prototype.isWillbedeleted = function () {
                return this.getWillBeDeleted() || false;
            };
            COipfRecordingItem.prototype.isScrambled = function () {
                var that = this;
                if (that._isScrambled === undefined) {
                    that._isScrambled = that._property.isScrambled;
                }
                return that._isScrambled || false;
            };
            COipfRecordingItem.prototype.isEncrypted = function () {
                var that = this;
                if (that._isEncrypted === undefined) {
                    that._isEncrypted = that._property.isEncrypted;
                }
                return that._isEncrypted || false;
            };
            COipfRecordingItem.prototype.isRecordingOnGoing = function () {
                var that = this;
                if (that._showType === undefined) {
                    that._showType = that._property.showType;
                }
                return (that._showType === 0 /* ELIVE */) || false;
            };
            COipfRecordingItem.prototype.isEqual = function (b) {
                var a = this;
                return !!(a.getId() && (a.getId() === b.getId()));
            };
            COipfRecordingItem.prototype.isScheduledAsSeries = function () {
                var that = this;
                if (that._isScheduledAsSeries === undefined) {
                    that._isScheduledAsSeries = that._property.isScheduledAsSeries;
                }
                return that._isScheduledAsSeries;
            };
            COipfRecordingItem.prototype.isRestrictedByDrm = function (b) {
                return (this._property.error === 3 /* ERROR_REC_DRM_RESTRICTION */) || false;
            };
            COipfRecordingItem.prototype.setManualLock = function (lock, oipfRecSchedulerObj) {
                var that = this;
                if (that._property.isManualLocked !== lock) {
                    that._property.isManualLocked = lock;
                    if (that.isSeries()) {
                        if (hx.config.useRfCRID === true) {
                            oipfRecSchedulerObj.setSeriesLock(that.getGroupCRIDs()[0], lock);
                        }
                        else {
                            oipfRecSchedulerObj.setSeriesLock(that.getSeriesId(), lock);
                        }
                    }
                    if (hx.emulMode) {
                        if (oipfRecSchedulerObj._PVREvent) {
                            oipfRecSchedulerObj._PVREvent(6 /* ERECORDING_REC_UPDATED */, that.getInstance());
                        }
                    }
                }
                else {
                    if (that.isSeries()) {
                        if (hx.config.useRfCRID === true) {
                            oipfRecSchedulerObj.setSeriesLock(that.getGroupCRIDs()[0], lock);
                        }
                        else {
                            oipfRecSchedulerObj.setSeriesLock(that.getSeriesId(), lock);
                        }
                    }
                }
            };
            COipfRecordingItem.prototype.getInstance = function () {
                return this._property;
            };
            COipfRecordingItem.prototype.getParentalRatings = function () {
                var that = this;
                if (that._parentalRatings === undefined) {
                    that._parentalRatings = that._property.parentalRatings;
                }
                return that._parentalRatings;
            };
            COipfRecordingItem.prototype.getWillBeDeleted = function () {
                var that = this;
                if (that._willBeDeleted === undefined) {
                    that._willBeDeleted = that._property.willBeDeleted;
                }
                return that._willBeDeleted;
            };
            COipfRecordingItem.prototype.getGuidance = function () {
                var that = this;
                if (that._guidance === undefined) {
                    that._guidance = that._property.getField("freeview/guidance");
                }
                if (that._guidance) {
                    var guidance = JSON.parse(that._guidance);
                    hx.log('debug', "[COipfRecordingItem] getGuidance---- text : " + (guidance.guidance ? guidance.guidance[0].text : ""));
                    hx.log('debug', "[COipfRecordingItem] getGuidance---- type : " + (guidance.guidance ? guidance.guidance[0].type : ""));
                    hx.log('debug', "[COipfRecordingItem] getGuidance---- mode : " + (guidance.guidance ? guidance.guidance[0].mode : ""));
                    return guidance.guidance ? guidance.guidance[0] : "";
                }
                else {
                    return "";
                }
            };
            COipfRecordingItem.prototype.hasGuidance = function () {
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
            };
            COipfRecordingItem.prototype.getEndPadding = function () {
                var that = this;
                if (that._endPadding === undefined) {
                    that._endPadding = that._property.endPadding;
                }
                return that._endPadding;
            };
            COipfRecordingItem.prototype.wasWatching = function () {
                var that = this;
                if (!that.isNew() && (that.getDuration() > that.getLastPlayPosition())) {
                    return true;
                }
                return false;
            };
            return COipfRecordingItem;
        })(IRecordingItem);
        mPrismModel.COipfRecordingItem = COipfRecordingItem;
        var cArray = (function () {
            function cArray() {
                var aArgs = [];
                for (var _i = 0; _i < arguments.length; _i++) {
                    aArgs[_i - 0] = arguments[_i];
                }
                Array.apply(this, arguments);
            }
            cArray.prototype.push = function (aItem) {
                return 0;
            };
            cArray.prototype.splice = function (aTargetIndex, aEA, a) {
                return 0;
            };
            cArray.prototype.unshift = function (aItem) {
                return 0;
            };
            cArray.prototype.slice = function (start, end) {
                return 0;
            };
            return cArray;
        })();
        mPrismModel.cArray = cArray;
        cArray.prototype = Array.prototype;
        var COipfScheduledItemList = (function (_super) {
            __extends(COipfScheduledItemList, _super);
            function COipfScheduledItemList(list, filter, maker) {
                _super.call(this);
                this.setList(list, filter, maker);
            }
            COipfScheduledItemList.prototype.setFilter = function (filter) {
                this._filter = filter;
            };
            COipfScheduledItemList.prototype.indexBy = function (aTarget, fnCondition) {
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
            };
            COipfScheduledItemList.prototype.add = function (item) {
                var that = this;
                if (that._filter(item)) {
                    return that.push(item);
                }
                return false;
            };
            COipfScheduledItemList.prototype.indexOf = function (item) {
                var idMap = this.map(function (aItem) {
                    var progID = aItem.getProgrammeID();
                    if (!progID) {
                        return '';
                    }
                    return hx.il.getCompareID(aItem);
                });
                return idMap.indexOf(hx.il.getCompareID(item));
            };
            COipfScheduledItemList.prototype.existOf = function (item) {
                return this.some(function (aItem) {
                    return hx.il.compareProgrammeID(item.getProgrammeID(), aItem.getProgrammeID());
                });
            };
            COipfScheduledItemList.prototype.remove = function (item) {
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
            };
            COipfScheduledItemList.prototype.setList = function (list, filter, maker) {
                var that = this;
                var i, ti;
                that.setFilter(filter);
                for (i = 0, ti = list.length; i < ti; i += 1) {
                    that.add(maker(list[i]));
                }
            };
            COipfScheduledItemList.prototype.update = function (item) {
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
            };
            return COipfScheduledItemList;
        })(cArray);
        mPrismModel.COipfScheduledItemList = COipfScheduledItemList;
        /**************************************************
         * CTsrItem
         **************************************************/
        var EMAX_DURATION = hx.config.durationTSR;
        var CTsrItem = (function (_super) {
            __extends(CTsrItem, _super);
            function CTsrItem(pluginObject) {
                _super.call(this, pluginObject || {});
                this._isChase = null;
            }
            CTsrItem.prototype.getStartTime = function () {
                var a = this;
                var now = new Date().toSeconds();
                var maxOffset = hx.svc.TSR.getMaxOffset();
                // a._pluginObject.startTime = mStartTime;
                var startTime = now - maxOffset;
                console.log(hx.util.format('getStartTime - startTime = {0}', startTime));
                return startTime;
            };
            CTsrItem.prototype.getStartOffset = function () {
                return hx.svc.TSR.getStartOffset();
            };
            CTsrItem.prototype.getDuration = function () {
                var a = this;
                var duration = 0;
                if (a.isChase() && !!a._recording) {
                    var chaseItem = a._recording.getInstance();
                    duration = a.getStartTime() - chaseItem.startTime + chaseItem.duration + 60;
                }
                else {
                    a._pluginObject.duration = EMAX_DURATION;
                    duration = _super.prototype.getDuration.call(a);
                }
                //hx.log(hx.util.format('getDuration - EMAX_DURATION = {0}, duration = {1}', EMAX_DURATION, duration));
                return duration;
            };
            CTsrItem.prototype.setDuration = function (duration) {
                var a = this;
                if (a.isChase() && !!a._recording) {
                    a._recording.getInstance().duration = duration;
                }
            };
            CTsrItem.prototype.getTitle = function () {
                var a = this;
                var title = '';
                var result;
                var currentChannel = hx.stbObject.videoBroadcast.currentChannel;
                if (currentChannel) {
                    result = hx.svc.EPG.getLiveProgrammes({
                        channel: currentChannel,
                        startTime: a.getStartTime() + hx.svc.TSR.getPlaybackOffset()
                    });
                }
                if (result) {
                    var len = result[0].length;
                    if (len > 0) {
                        title = result[0][0].getName();
                    }
                }
                return title;
            };
            CTsrItem.prototype.isChase = function () {
                var a = this;
                if (a._isChase === null) {
                    var i, length, item, recording, chase = false;
                    var channel = hx.svc.CH_LIST.getCurrentChannel().channel;
                    var recordings = hx.svc.RECORDINGS.getCurrentRecordings();
                    if (channel && recordings && recordings.length) {
                        length = recordings.length;
                        for (i = 0; i < length; i += 1) {
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
            };
            CTsrItem.prototype.getCasIDs = function () {
                var casIDs;
                var channel = hx.svc.CH_LIST.getCurrentChannel().channel;
                if (channel) {
                    casIDs = channel.casIDs;
                }
                else {
                    casIDs = [];
                }
                return casIDs;
            };
            return CTsrItem;
        })(CMediaItem);
        mPrismModel.CTsrItem = CTsrItem;
        // from dsrvHistory
        mPrismModel.TDefaultThumbnailList = {
            DEFAULT: 'images/614_Thumb_Genre.jpg',
            RECORDING: 'images/614_Thumb_REC.jpg',
            REC_FAIL: 'images/614_Thumb_REC_Fail.jpg',
            TV: 'images/614_Thumb_TV.jpg',
            RADIO: 'images/614_Thumb_Radio.jpg',
            FREEVIEW: 'images/614_OD_Default01.png',
            CONTENT: 'images/614_OD_Default02.png'
        };
        mPrismModel.THistoryType = {
            Channel: 'channel',
            OnNow: 'onNow',
            Recording: 'recentlyRecorded',
            Playback: 'recording',
            // 'recording' is already used for PVR playback in webstorage.
            Media: 'media',
            App: 'app',
            Unknown: 'unknown'
        };
        var cHistoryItem = (function () {
            function cHistoryItem() {
                this._type = '';
                this._id = '';
            }
            cHistoryItem.prototype.getType = function () {
                return this._type;
            };
            cHistoryItem.prototype.getId = function () {
                return this._id;
            };
            cHistoryItem.prototype.isEqual = function (historyItem) {
                return (this.getType() === historyItem.getType() && this.getId() === historyItem.getId());
            };
            cHistoryItem.prototype.serialize = function () {
                return {
                    type: this.getType(),
                    id: this._id
                };
            };
            cHistoryItem.prototype.deSerialize = function (obj) {
                this._type = obj.type;
                this._id = obj.id;
                return this;
            };
            return cHistoryItem;
        })();
        mPrismModel.cHistoryItem = cHistoryItem;
        var cChannelHistoryItem = (function (_super) {
            __extends(cChannelHistoryItem, _super);
            function cChannelHistoryItem(channel) {
                _super.call(this);
                if (channel) {
                    this._type = mPrismModel.THistoryType.Channel;
                    this._id = channel.ccid;
                    this._no = hx.il.makeDigitNumber(channel.majorChannel);
                    this._thumbnail = channel.logoURL;
                    this._title = channel.name;
                    this._group = hx.svc.WEB_STORAGE.getLastGroup();
                }
            }
            cChannelHistoryItem.prototype.serialize = function () {
                var obj = _super.prototype.serialize.call(this);
                obj.channelNo = this._no;
                obj.title = this._title;
                obj.thumbnail = this._thumbnail;
                obj.group = this._group;
                return obj;
            };
            cChannelHistoryItem.prototype.deSerialize = function (obj) {
                _super.prototype.deSerialize.call(this, obj);
                this._no = obj.channelNo;
                this._title = obj.title;
                this._thumbnail = obj.thumbnail;
                this._group = obj.group;
                return this;
            };
            cChannelHistoryItem.prototype.getNo = function () {
                return this._no;
            };
            cChannelHistoryItem.prototype.getName = function () {
                return this._title;
            };
            cChannelHistoryItem.prototype.getLogo = function () {
                return this._thumbnail;
            };
            return cChannelHistoryItem;
        })(cHistoryItem);
        mPrismModel.cChannelHistoryItem = cChannelHistoryItem;
        var cOnNowHistoryItem = (function (_super) {
            __extends(cOnNowHistoryItem, _super);
            function cOnNowHistoryItem(channel, index) {
                _super.call(this);
                if (channel) {
                    this._type = mPrismModel.THistoryType.OnNow;
                    this._id = channel.ccid;
                    this._title = channel.name;
                    this._no = hx.il.makeDigitNumber(channel.majorChannel);
                    this._channelType = channel.channelType;
                }
                this._index = index || 0;
            }
            cOnNowHistoryItem.prototype.serialize = function () {
                var obj = _super.prototype.serialize.call(this);
                obj.channelNo = this._no;
                obj.title = this._title;
                obj.index = this._index;
                obj.channelType = this._channelType;
                return obj;
            };
            cOnNowHistoryItem.prototype.deSerialize = function (obj) {
                _super.prototype.deSerialize.call(this, obj);
                this._no = obj.channelNo;
                this._title = obj.title;
                this._index = obj.index;
                this._channelType = obj.channelType;
                return this;
            };
            cOnNowHistoryItem.prototype.getIndex = function () {
                return this._index;
            };
            cOnNowHistoryItem.prototype.getName = function () {
                return this._title;
            };
            cOnNowHistoryItem.prototype.getLcn = function () {
                return this._no;
            };
            cOnNowHistoryItem.prototype.isRadio = function () {
                return (this._channelType === 1 /* ERADIO */) || false;
            };
            cOnNowHistoryItem.prototype.isEqual = function (historyItem) {
                return ((this.getType() === historyItem.getType()) && (this.getIndex() === historyItem.getIndex()));
            };
            return cOnNowHistoryItem;
        })(cHistoryItem);
        mPrismModel.cOnNowHistoryItem = cOnNowHistoryItem;
        var cRecordingHistoryItem = (function (_super) {
            __extends(cRecordingHistoryItem, _super);
            function cRecordingHistoryItem(recordingItem) {
                _super.call(this);
                if (recordingItem) {
                    if (recordingItem.id) {
                        this._type = mPrismModel.THistoryType.Recording;
                        this._id = recordingItem.id;
                        this._title = recordingItem.name;
                        this._startTime = recordingItem.startTime;
                        this._duration = recordingItem.duration;
                    }
                }
            }
            cRecordingHistoryItem.prototype.serialize = function () {
                var obj = _super.prototype.serialize.call(this);
                obj.title = this._title;
                obj.thumbnail = this._thumbnail;
                obj.startTime = this._startTime;
                obj.duration = this._duration;
                return obj;
            };
            cRecordingHistoryItem.prototype.deSerialize = function (obj) {
                _super.prototype.deSerialize.call(this, obj);
                this._title = obj.title;
                this._thumbnail = obj.thumbnail;
                this._startTime = obj.startTime;
                this._duration = obj.duration;
                return this;
            };
            cRecordingHistoryItem.prototype.getId = function () {
                return this._id;
            };
            cRecordingHistoryItem.prototype.getTitle = function () {
                return this._title;
            };
            cRecordingHistoryItem.prototype.getStartTime = function () {
                return this._startTime;
            };
            cRecordingHistoryItem.prototype.getDuration = function () {
                return this._duration;
            };
            return cRecordingHistoryItem;
        })(cHistoryItem);
        mPrismModel.cRecordingHistoryItem = cRecordingHistoryItem;
        var cPlaybackHistoryItem = (function (_super) {
            __extends(cPlaybackHistoryItem, _super);
            function cPlaybackHistoryItem(playbackItem) {
                _super.call(this);
                if (playbackItem) {
                    if (playbackItem.id) {
                        this._type = mPrismModel.THistoryType.Playback;
                        this._id = playbackItem.id;
                        this._title = playbackItem.name;
                        this._duration = playbackItem.duration;
                        this._lastPlayPosition = playbackItem.lastPlayPosition;
                        if (playbackItem.thumbnail) {
                            this._thumbnail = playbackItem.thumbnail.indexOf('file://') === 0 ? playbackItem.thumbnail : 'file://' + playbackItem.thumbnail;
                        }
                        else {
                            this._thumbnail = mPrismModel.TDefaultThumbnailList.RECORDING;
                        }
                    }
                }
            }
            cPlaybackHistoryItem.prototype.serialize = function () {
                var obj = _super.prototype.serialize.call(this);
                obj.title = this._title;
                obj.thumbnail = this._thumbnail;
                obj.duration = this._duration;
                obj.lastPlayPosition = this._lastPlayPosition;
                return obj;
            };
            cPlaybackHistoryItem.prototype.deSerialize = function (obj) {
                _super.prototype.deSerialize.call(this, obj);
                this._title = obj.title;
                this._thumbnail = obj.thumbnail;
                this._duration = obj.duration;
                this._lastPlayPosition = obj.lastPlayPosition;
                return this;
            };
            cPlaybackHistoryItem.prototype.getId = function () {
                return this._id;
            };
            cPlaybackHistoryItem.prototype.getTitle = function () {
                return this._title;
            };
            cPlaybackHistoryItem.prototype.getThumbnail = function () {
                return this._thumbnail;
            };
            cPlaybackHistoryItem.prototype.getDuration = function () {
                return this._lastPlayPosition;
            };
            cPlaybackHistoryItem.prototype.getLastPlayPosition = function () {
                return this._lastPlayPosition;
            };
            cPlaybackHistoryItem.prototype.setLastPlayPosition = function (time) {
                this._lastPlayPosition = time;
            };
            cPlaybackHistoryItem.prototype.wasWatching = function () {
                if (this._duration > this._lastPlayPosition) {
                    return true;
                }
                return false;
            };
            return cPlaybackHistoryItem;
        })(cHistoryItem);
        mPrismModel.cPlaybackHistoryItem = cPlaybackHistoryItem;
        var cMediaPlaybackHistoryItem = (function (_super) {
            __extends(cMediaPlaybackHistoryItem, _super);
            function cMediaPlaybackHistoryItem(mediaItem) {
                _super.call(this, mediaItem);
                if (mediaItem) {
                    this._pluginObject = mediaItem.getPluginObject();
                    this._type = mPrismModel.THistoryType.Media;
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
            cMediaPlaybackHistoryItem.prototype.serialize = function () {
                var obj = _super.prototype.serialize.call(this);
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
            };
            cMediaPlaybackHistoryItem.prototype.deSerialize = function (obj) {
                _super.prototype.deSerialize.call(this, obj);
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
            };
            cMediaPlaybackHistoryItem.prototype.getId = function () {
                return this._id;
            };
            cMediaPlaybackHistoryItem.prototype.getPluginObject = function () {
                return this._pluginObject;
            };
            cMediaPlaybackHistoryItem.prototype.getContentType = function () {
                return this._contentType;
            };
            cMediaPlaybackHistoryItem.prototype.getTitle = function () {
                return this._title;
            };
            cMediaPlaybackHistoryItem.prototype.getThumbnailUrl = function () {
                var duration = this._duration;
                var lastPlayPosition = this._lastPlayPosition;
                if (duration > 7) {
                    if (lastPlayPosition > duration - 60) {
                        lastPlayPosition = lastPlayPosition - 1;
                    }
                    else {
                        lastPlayPosition = (lastPlayPosition < 7) ? 7 : lastPlayPosition;
                    }
                    this._thumbnail = hx.svc.AV_CTRL.requestThumbnail({
                        'path': this._fullPath,
                        'position': lastPlayPosition
                    });
                    if (hx.svc.MEDIA_ITEM_MANAGER.isExist({ path: this._thumbnail })) {
                        return this._thumbnail;
                    }
                    lastPlayPosition = 7;
                }
                else {
                    lastPlayPosition = 0;
                }
                //this._lastPlayPosition = (this._lastPlayPosition === 0 ) ? 7 : this._lastPlayPosition;
                this._thumbnail = hx.svc.AV_CTRL.requestThumbnail({
                    'path': this._fullPath,
                    'position': lastPlayPosition
                });
                return this._thumbnail;
            };
            cMediaPlaybackHistoryItem.prototype.getLargeThumbnailUrl = function () {
                return this._largeThumbnailUrl ? this._largeThumbnailUrl : '';
            };
            cMediaPlaybackHistoryItem.prototype.getFullPath = function () {
                return this._fullPath;
            };
            cMediaPlaybackHistoryItem.prototype.getStartTime = function () {
                return this._startTime;
            };
            cMediaPlaybackHistoryItem.prototype.getDuration = function () {
                return this._duration;
            };
            cMediaPlaybackHistoryItem.prototype.getResolution = function () {
                return this._resolution;
            };
            cMediaPlaybackHistoryItem.prototype.getAlbumName = function () {
                return this._album;
            };
            cMediaPlaybackHistoryItem.prototype.getArtistName = function () {
                return this._artist;
            };
            cMediaPlaybackHistoryItem.prototype.getFileType = function () {
                return IMediaItem.TFileType.EFile;
            };
            cMediaPlaybackHistoryItem.prototype.getDate = function () {
                return this._time;
            };
            cMediaPlaybackHistoryItem.prototype.getStorageType = function () {
                return IMediaServer.TStorageType.ERecent;
            };
            cMediaPlaybackHistoryItem.prototype.getLastPlayPosition = function () {
                return this._lastPlayPosition;
            };
            cMediaPlaybackHistoryItem.prototype.setLastPlayPosition = function (time) {
                this._lastPlayPosition = (time === 0) ? 7 : time;
                this._thumbnail = hx.svc.AV_CTRL.requestThumbnail({
                    'path': this._fullPath,
                    'position': this._lastPlayPosition
                });
                //
            };
            cMediaPlaybackHistoryItem.prototype.isOperationEnabled = function () {
                return true;
            };
            cMediaPlaybackHistoryItem.prototype.getBitRate = function () {
                return this._bitRate;
            };
            cMediaPlaybackHistoryItem.prototype.getSampleRate = function () {
                return this._sampleRate;
            };
            return cMediaPlaybackHistoryItem;
        })(cPlaybackHistoryItem);
        mPrismModel.cMediaPlaybackHistoryItem = cMediaPlaybackHistoryItem;
        var cAppHistoryItem = (function (_super) {
            __extends(cAppHistoryItem, _super);
            function cAppHistoryItem(wd) {
                _super.call(this);
                this._type = mPrismModel.THistoryType.App;
                if (wd) {
                    this._id = wd.id;
                    this._title = wd.name;
                    this._thumbnail = wd.localURI + wd.customIcons[0];
                }
            }
            cAppHistoryItem.prototype.serialize = function () {
                var obj = _super.prototype.serialize.call(this);
                obj.title = this._title;
                obj.icon = this._icon;
                obj.thumbnail = this._thumbnail;
                return obj;
            };
            cAppHistoryItem.prototype.deSerialize = function (obj) {
                _super.prototype.deSerialize.call(this, obj);
                this._title = obj.title;
                this._icon = obj.icon;
                this._thumbnail = obj.thumbnail;
                return this;
            };
            return cAppHistoryItem;
        })(cHistoryItem);
        mPrismModel.cAppHistoryItem = cAppHistoryItem;
        // from dsrvPlaylist
        var cMediaPlaybackPlaylistItem = (function (_super) {
            __extends(cMediaPlaybackPlaylistItem, _super);
            function cMediaPlaybackPlaylistItem(mediaItem) {
                _super.call(this, mediaItem);
            }
            cMediaPlaybackPlaylistItem.prototype.getStorageType = function () {
                return IMediaServer.TStorageType.EPlaylist;
            };
            return cMediaPlaybackPlaylistItem;
        })(cMediaPlaybackHistoryItem);
        mPrismModel.cMediaPlaybackPlaylistItem = cMediaPlaybackPlaylistItem;
        // from mediacenter
        var cLocalServer = (function (_super) {
            __extends(cLocalServer, _super);
            function cLocalServer(pluginObject) {
                _super.call(this);
                if (pluginObject) {
                    hx.log('dsrvMediaItemManager', 'new CLocalServer()');
                    this._pluginObject = pluginObject;
                }
            }
            cLocalServer.prototype.getId = function () {
                return this._pluginObject.id;
            };
            cLocalServer.prototype.getName = function () {
                if (this._name === undefined) {
                    var serverName = '';
                    var label = this._pluginObject.label;
                    var TStorageType = IMediaServer.TStorageType;
                    var storageType = this.getStorageType();
                    switch (storageType) {
                        case TStorageType.EInternalHdd:
                            if (hx.config.useInternalPvr === 1) {
                                serverName = hx.l('LOC_INTERNALHDD_ID');
                            }
                            else {
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
            };
            cLocalServer.prototype.getStorageType = function () {
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
            };
            cLocalServer.prototype.getStorageCssClass = function () {
                return IMediaServer.StorageCssClassList[this.getStorageType()];
            };
            cLocalServer.prototype.getViewType = function () {
                return IMediaServer.TViewType.EFolderView;
            };
            cLocalServer.prototype.getPath = function (contentType) {
                var TContentType = IMediaItem.TContentType;
                var TStorageType = IMediaServer.TStorageType;
                var path = this._pluginObject.path;
                if ((this.getStorageType() === TStorageType.EInternalHdd) && (hx.config.useInternalPvr === 1)) {
                    switch (contentType) {
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
            };
            cLocalServer.prototype.getLocationType = function () {
                return IMediaServer.TLocationType.ELocal;
            };
            cLocalServer.prototype.setName = function (name) {
                this._name = name;
            };
            cLocalServer.prototype.isMounted = function () {
                return true;
            };
            return cLocalServer;
        })(IMediaServer);
        mPrismModel.cLocalServer = cLocalServer;
        var cSharedServer = (function (_super) {
            __extends(cSharedServer, _super);
            function cSharedServer(pluginObject) {
                _super.call(this, pluginObject);
                hx.log('dsrvMediaItemManager', 'new CSharedServer()');
            }
            cSharedServer.prototype.getName = function () {
                return hx.l('LOC_DOWNLOADED_ID');
            };
            cSharedServer.prototype.getPath = function () {
                return this._pluginObject.path + '/Media/Download';
            };
            cSharedServer.prototype.getStorageType = function () {
                return IMediaServer.TStorageType.EShared;
            };
            cSharedServer.prototype.getId = function () {
                return this._pluginObject.id + '_shared';
            };
            return cSharedServer;
        })(cLocalServer);
        mPrismModel.cSharedServer = cSharedServer;
        var cAirPlayItem = (function (_super) {
            __extends(cAirPlayItem, _super);
            function cAirPlayItem(url, contentType) {
                _super.call(this);
                this._url = url;
                this._duration = 0;
                this._contentType = contentType;
                this._title = '';
            }
            cAirPlayItem.prototype.getFullPath = function () {
                return this._url;
            };
            cAirPlayItem.prototype.setTitle = function (title) {
                this._title = title;
            };
            cAirPlayItem.prototype.getDuration = function () {
                return this._duration;
            };
            cAirPlayItem.prototype.getTitle = function () {
                return '';
            };
            cAirPlayItem.prototype.getContentType = function () {
                return this._contentType;
            };
            cAirPlayItem.prototype.isOperationEnabled = function () {
                var operationEnabled = true;
                if (this._contentType !== IMediaItem.TContentType.EVideo) {
                    operationEnabled = false;
                }
                return operationEnabled;
            };
            cAirPlayItem.prototype.getLargeThumbnailUrl = function () {
                return '';
            };
            cAirPlayItem.prototype.getThumbnailUrl = function () {
                return '';
            };
            return cAirPlayItem;
        })(CMediaItem);
        mPrismModel.cAirPlayItem = cAirPlayItem;
        var cDlnaItem = (function (_super) {
            __extends(cDlnaItem, _super);
            function cDlnaItem(pluginObject) {
                _super.call(this, pluginObject);
                this._pluginObject = pluginObject;
            }
            cDlnaItem.prototype.isByteSeekAvailable = function () {
                var byteSeekAble = this._pluginObject.byteSeekAble;
                hx.log('dsrvDlna', 'isByteSeekAvailable() = ' + byteSeekAble);
                return byteSeekAble;
            };
            cDlnaItem.prototype.getFullPath = function () {
                return this.getImportUrl();
            };
            cDlnaItem.prototype.getImportUrl = function () {
                var importUrl = this._pluginObject.importURI;
                hx.log('dsrvDlna', 'getImportUrl() = ' + importUrl);
                var fnParsePhotoPath = function (url) {
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
            };
            cDlnaItem.prototype.getObjectType = function () {
                var objectType = this._pluginObject.objectType;
                hx.log('dsrvDlna', 'getObjectType() = ' + objectType);
                return objectType;
            };
            cDlnaItem.prototype.getContentType = function () {
                var TContentType = IMediaItem.TContentType;
                var contentType, objectType;
                objectType = this.getObjectType();
                switch (objectType) {
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
            };
            cDlnaItem.prototype.getFileType = function () {
                var TFileType = IMediaItem.TFileType;
                var fileType = TFileType.EFile;
                var objectType = this.getObjectType();
                if (objectType === 'folder' || objectType === 'server') {
                    fileType = TFileType.EFolder;
                }
                return fileType;
            };
            cDlnaItem.prototype.getResolution = function () {
                var resolution = this._pluginObject.resolution;
                hx.log('dsrvDlna', 'getResolution() = ' + resolution);
                return resolution;
            };
            cDlnaItem.prototype.getTitle = function () {
                var title = this._pluginObject.title;
                hx.log('dsrvDlna', 'getTitle() = ' + title);
                return title;
            };
            cDlnaItem.prototype.getThumbnailUrl = function () {
                var thumbnailUrl = this._pluginObject.thumbnailUrl;
                hx.log('dsrvDlna', 'getThumbnailUrl() = ' + thumbnailUrl);
                return thumbnailUrl;
            };
            cDlnaItem.prototype.getPath = function () {
                return this._pluginObject.objectID;
            };
            cDlnaItem.prototype.getParentID = function () {
                return this._pluginObject.parentID;
            };
            return cDlnaItem;
        })(CNetworkMediaItem);
        mPrismModel.cDlnaItem = cDlnaItem;
    })(mPrismModel || (mPrismModel = {}));
    return mPrismModel;
});
