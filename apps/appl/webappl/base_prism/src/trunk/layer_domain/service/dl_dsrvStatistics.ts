/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvStatistics.js
/*jslint nomen:true, vars:true */
//    STATISTICS: ['layer_domain/service/dl_dsrvStatistics.js'],

declare var hx:ifSugar;

import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");

var dsrvRecordings2 = hx.svc.RECORDINGS;
var dsrvBridge = hx.svc.BRIDGE;
var dsrvSettingUtil = hx.svc.SETTING_UTIL;
var dsrvTVPortal = hx.svc.TV_PORTAL;
var dsrvStorageMass = hx.svc.STORAGE_MASS;
var dsrvStorageUtil = hx.svc.STORAGE_UTIL;
var dsrvAppMgr = hx.svc.APP_MGR;
var dsrvWebStorage = hx.svc.WEB_STORAGE;
if(hx.config.netflix) {
    var dsrvNetflix = hx.svc.NETFLIX;
}
var mTimerInterval: number;
var mTimerAlive: number;
//interval time
var TINTERVAL: number = 60 * 60 * 1000;
// 1 hour
//alive time
var TALIVE: number = 5 * 60 * 1000;
//5 minutes
var mOperateStatus: boolean = false;
var mPolicyStatus: boolean = false;
var mNetworkAvailableStatus: boolean = false;
var mStatisticsLog = [];
var mStatisticsErrorLog = [];
var mErrorLogFlag = false;
var mSysInfo: any;
var mNetConnectedInfo: any;
var mLogCount: number = 1;
var mOperationMode: string = dsrvSettingUtil.getTVPortalUrl() === "http://test.humaxtvportal.com" ? "debug" : "release";
var mUrl: string;
var mSettingsTransTime = dsrvWebStorage.getStatisticsLastSendingTimeOfSettingInfo();
var mCheckUploadTransTime = dsrvWebStorage.getStatisticsLastSendingTimeOfCheckUpload();
var bChlist: boolean = false;
var bReclist: boolean = false;
var bUsblist: boolean = false;
var bHdmiInfo: boolean = false;
var powerValueObj = dsrvSettingUtil.getPowerStateValues();
var isSstart = false;
var isIpEpgDownStart: boolean = false;
var isIpEpgDownEnd: boolean = false;
var mIsIpEpgDownloadStart: any = dsrvWebStorage.getStatisticsIpEpgDownloadStartEnable();
var mLastUpdateIpEpgTime: any = dsrvWebStorage.getStatisticsLastUpdateIpEpgTime();
var mExternalIpAddr: string = '';

enum TPowrOnInfoRepeatDayType {
    ONCE = 0,
    DAILY = 1,
    WEEKLY = 2
};

var TPhysicalStorageMaxCount = 2;

enum TPhysicalStorageItem {
    First = 0,
    Second = 1
};

enum TPhysicalStorageType {
    ETypeUnknown = 0,
    ETypeInternal = 1,
    ETypeExternal = 2
};
enum TPhysicalStorageKind {
    EKindUnknown = 0,
    EKindHDD = 1,
    EKindUSBMemory = 2,
    EKindSDMemory = 3
};

enum TMediaContentType {
    ETypeVideo = 0,
    ETypeMusic = 1,
    ETypePhoto = 2,
    ETypeUnknown = 3
};

enum TMediaFileType {
    ETypeFile = 0,
    ETypeFolder = 1,
    ETypeGroup = 2,
    ETypeUnknown = 3
};

enum TMediaWatchedType {
    ETypeNotWatched = 0,
    ETypeWatchedHalf = 1,
    ETypeWatchedFull = 2
};

enum TMediaStorageType {
    ETypeSATA = 0,
    ETypeUSB = 1,
    ETypeSD = 2,
    ETypeSAMBA = 3,
    ETypeUnknown = 4
};

enum TRsvType {
    EReserved = 0,
    EWatching,
    ERecordEBR,
    ERecordTBR
}

var TMajorGenreType =
    ['Movie', 'News', 'Shows', 'Sports', 'Children', 'Music', 'Arts', 'Social', 'Education', 'Leisure', 'Special Characteristics'];
var TMinorGenreType =
    [['Movie/Drama', 'Detective/Thriller', 'Adventure/Western/War', 'Science Fiction/Fantasy/Horror', 'Comedy', 'Soap/Melodrama/Folkloric', 'Romance', 'Serious/Classical/Religious/Historical Movie/Drama', 'Adult Movie/Drama'],
        ['News/Current Affairs', 'News/Weather Report', 'News Magazine', 'Documentary', 'Discussion/Interview/Debate'],
        ['Show/Game Show', 'Game show/Quiz/Contest', 'Variety Show', 'Talk Show'],
        ['Special Events', 'Sports Magazines', 'Football/Soccer', 'Tennis/Squash', 'Team Sports', 'Athletics', 'Motor Sport', 'Water Sport', 'Winter Sports', 'Equestrian', 'Martial Sports'],
        ['Children\'s/Youth Programmes', 'Pre-school Children\'s Programmes', 'Entertainment Programmes for 6 to 14', 'Entertainment Programmes for 10 to 16', 'Informational/Educational/School Programmes', 'Cartoons/Puppets'],
        ['Music/Ballet/Dance', 'Rock/Pop', 'Serious Music/Classical Music', 'Folk/Traditional Music', 'Jazz', 'Musical/Opera', 'Ballet'],
        ['Arts/Culture', 'Performing Arts', 'Fine Arts', 'Religion', 'Popular Culture/Traditional Arts', 'Literature', 'Film/Cinema', 'Experimental Film/Video', 'Broadcasting/Press', 'New Media', 'Arts/Culture Magazines', 'Fashion'],
        ['Social/Political Issues/Economics', 'Magazines/Reports/Documentary', 'Economics/Social Advisory', 'Remarkable People'],
        ['Education/Science/Factual Topics', 'Nature/Animals/Environment', 'Technology/Natural Sciences', 'Medicine/Physiology/Psychology', 'Foreign Countries/Expeditions', 'Social/Spiritual Sciences', 'Further Education', 'Languages'],
        ['Leisure Hobbies', 'Tourism/Travel', 'Handicraft', 'Motoring', 'Fitness and Health', 'Cooking', 'Advertisement/Shopping', 'Gardening'],
        ['Original Language', 'Black and White', 'Unpublished', 'Live Broadcast']];

var _addMacInfo = function() {
    var mac, i;

    if (mNetConnectedInfo.wireDevState) {
        mac = mSysInfo.wiredMacAddress;
    } else if (mNetConnectedInfo.wirelessDevState) {
        mac = mSysInfo.wirelessMacAddress;
    } else {
        mac = "";
    }

    for ( i = 0; i < mStatisticsLog.length; i += 1) {
        mStatisticsLog[i].MAC = mac;
    }
};
var _currentDate = function() {
    var now = new Date();
    var date;

    date = now.format('yyyy-mm-dd');

    hx.log('dsrvStatistics', 'Current Date :' + date);
    return date;
};
var _currentTime = function() {
    var now = new Date();
    var time = '';

    //time = now.format('HH:MM:ss');

    time += now.getHours() < 10 ? '0'+ now.getHours()  : now.getHours();
    time += ":";
    time += now.getMinutes() < 10 ? '0'+ now.getMinutes()  : now.getMinutes();
    time += ":";
    time += now.getSeconds() < 10 ? '0'+ now.getSeconds()  : now.getSeconds();
    time += ".";
    time += now.getMilliseconds() < 10 ? '00'+ now.getMilliseconds() : (now.getMilliseconds() >= 10 && now.getUTCMilliseconds() < 100) ? '0'+ now.getUTCMilliseconds() : now.getUTCMilliseconds();

    hx.log('dsrvStatistics', 'Current UTC Time :' + time);

    return time;
};
var _convertLocalTimeOffset = function(param) {
    var convLto, offSet = param;

    offSet = offSet / -60;
    offSet = offSet < 10 ? "0" + offSet : offSet;
    convLto = offSet + ":00";

    return convLto;
};
var _getLocalTimeOffset = function() {
    var time = new Date();
    var lto = time.getTimezoneOffset();		// return -540 in case of GMT +9
    //var lto = dsrvSettingUtil.getoffsetGMT();
    lto = _convertLocalTimeOffset(lto);
    hx.log('dsrvStatistics', 'Current LTO :' + lto);

    return lto;
};

var _setLog = function(param) {
    var input, input2, input3, date, time, localTimeOffset, mac;

    date = _currentDate();
    time = _currentTime();
    localTimeOffset = String(_getLocalTimeOffset());
    mNetConnectedInfo = dsrvSettingUtil.getNetConnectedInfo();
    if (mNetConnectedInfo.wireDevState) {
        mac = mSysInfo.wiredMacAddress;
    } else if (mNetConnectedInfo.wirelessDevState) {
        mac = mSysInfo.wirelessMacAddress;
    } else {
        mac = "";
    }

    mExternalIpAddr = dsrvTVPortal.getExternalIp();
    hx.log('dsrvStatistics', 'ExternalIpAddr : ' + mExternalIpAddr);

    input = {
        "TIMESTAMP" : date + " " + time + "+" + localTimeOffset,
        "MAC" : mac,
        "SYSTEM_ID" : mSysInfo.system_id,
        "NO" : mLogCount++,
        "IP_ADDR" : mExternalIpAddr,
        "MODE" : mOperationMode
    };

    input = JSON.stringify(input);
    input = input.substr(0, input.length-1);
    input2 = JSON.stringify(param);
    input2 = input2.substr(1, input2.length-1);

    input3 = input + "," + input2;
    input3 = JSON.parse(input3);

    if (param.CODE === 'FAIL_SEND') {
        mStatisticsErrorLog.push(input3);
    } else {
        mStatisticsLog.push(input3);
    }
};
var _classifyLog = function(param) {
    var logType;
    logType = param.TRS;

    if (logType === "I") {
        _setLog(param);
    } else if (logType === "R") {
        _setLog(param);
    } else {
        hx.log('dsrvStatistics', 'This logType(' + logType + ') is not found. ');
    }
};
var _startLog = function() {
    dsrvTVPortal.getCurrentRegionInfo({
        fn : (result) => {
            mExternalIpAddr = result.ip;
            hx.log('dsrvStatistics', 'mExternalIpAddr : ' + mExternalIpAddr);
            _classifyLog({
                "CODE" : "SSTART",
                "TRS" : "I",
                "SW_VER" : mSysInfo.softwareVersion,
                "LOADER_VER" : '',//mSysInfo.serialoaderVersionlNumber,
                "MODEL_NAME" : mSysInfo.modelName,
                "SN" : mSysInfo.serialNumber,
                "UPDATE_DATE" : mSysInfo.update_date,
                "WAKEUP" : powerValueObj.wakeupReason
            });

            isSstart = true;

            hx.log('dsrvStatistics', '_startLog === Wakeup Reason :' + powerValueObj.wakeupReason);
            hx.log('dsrvStatistics', '_startLog fn === result :' + result);
        },
        fnError : (result) => {
            dsrvTVPortal.getCurrentRegionInfo({
                fn : (result) => {
                    mExternalIpAddr = result.ip;
                    hx.log('dsrvStatistics', 'mExternalIpAddr : ' + mExternalIpAddr);

                    _classifyLog({
                        "CODE" : "SSTART",
                        "TRS" : "I",
                        "SW_VER" : mSysInfo.softwareVersion,
                        "LOADER_VER" : mSysInfo.serialoaderVersionlNumber,
                        "MODEL_NAME" : mSysInfo.modelName,
                        "SN" : mSysInfo.serialNumber,
                        "UPDATE_DATE" : mSysInfo.update_date,
                        "WAKEUP" : powerValueObj.wakeupReason
                    });

                    isSstart = true;

                    hx.log('dsrvStatistics', '_startLog === Wakeup Reason :' + powerValueObj.wakeupReason);
                    hx.log('dsrvStatistics', '_startLog fn === result :' + result);
                },
                fnError : (result) => {
                    hx.log('dsrvStatistics', '_startLog fnError === result :' + result);
                }
            });

            hx.log('dsrvStatistics', '_startLog fnError === result :' + result);
        }
    });

};
var _conv_updatePeriod_to_string = function(updatePeriod) {
    var value = updatePeriod, startTimeHour, startTimeMinute, endTimeHour, endTimeMinute, updatePeriodValue;

    startTimeHour = parseInt(value.start_time, 10)/3600;
    if(startTimeHour < 10) {
        startTimeHour = '0' + parseInt(value.start_time, 10)/3600;
    }

    startTimeMinute = (parseInt(value.start_time, 10)%3600)/60;
    if(startTimeMinute < 10) {
        startTimeMinute = '0' + (parseInt(value.start_time, 10)%3600)/60;
    }

    endTimeHour = parseInt(value.end_time, 10)/3600;
    if(endTimeHour < 10) {
        endTimeHour = '0' + parseInt(value.end_time, 10)/3600;
    }

    endTimeMinute = (parseInt(value.end_time, 10)%3600)/60;
    if(endTimeMinute < 10) {
        endTimeMinute = '0' + (parseInt(value.end_time, 10)%3600)/60;
    }

    updatePeriodValue = {
        'startTime' : startTimeHour + ':' + startTimeMinute,
        'endTime' : endTimeHour + ':' + endTimeMinute
    };

    return updatePeriodValue;
};
var _logSettingVal = function() {

    var maturity = hx.svc.PARENTAL_CTRL .getParentalRatingValue();

    var LangObj = dsrvSettingUtil.getLanguage();

    var picInfo = dsrvSettingUtil.getPictureInfo();
    var audioInfo = dsrvSettingUtil.getPictureInfo();

    var displayTime = dsrvSettingUtil.getInfoDisplayTimeout();

    var subtitle = dsrvSettingUtil.getSubtitleFont();

    var networkServer = dsrvSettingUtil.IsNetServerWork();
    var serverActTime = hx.svc.WEB_STORAGE.getServerActivationTime();

    var sambaServer = dsrvSettingUtil.getEnableSambaServer();
    var ftpServer = dsrvSettingUtil.getEnableFtpServer();

    var ipepg = dsrvSettingUtil.getEnableIPEPG();

    var remoteRec = dsrvSettingUtil.getEnableRemoteRec();
    var recUpdatePeriod = dsrvSettingUtil.getUpdatePeriodRemoteRec();
    recUpdatePeriod = _conv_updatePeriod_to_string(recUpdatePeriod);
    var recUpdateInterval = dsrvSettingUtil.getUpdateIntervalRemoteRec();

    var startPadding = dsrvSettingUtil.getPvrStartPadding();
    var endPadding = dsrvSettingUtil.getPvrEndPadding();

    var seriesPlay = dsrvSettingUtil.getEnableSeriesPlayback();

    var instantReplayTime = dsrvSettingUtil.getInstantReplayTime();
    var skipForwardTime = dsrvSettingUtil.getSkipForwardTime();

    var autoPowerDown = hx.svc.WEB_STORAGE.getAutoPowerDown();
    var powerOnInfo =  dsrvSettingUtil.getPowerOnInfo();
    var privacyPolicyVal = dsrvTVPortal.isPrivacyPolicyAccepted();

    if (powerOnInfo) {
        powerOnInfo.time = dateFormat(powerOnInfo.time, "HH:MM");

        switch (powerOnInfo.repeatDays) {
            case TPowrOnInfoRepeatDayType.ONCE :
                powerOnInfo.repeatDays = 'once';
                break;
            case TPowrOnInfoRepeatDayType.DAILY :
                powerOnInfo.repeatDays = 'daily';
                break;
            case TPowrOnInfoRepeatDayType.WEEKLY :
                powerOnInfo.repeatDays = 'weekly';
                break;
            default :
                powerOnInfo.repeatDays = 'Off';
                break;
        }
    } else{
        powerOnInfo = {
            channel : 'Off',
            time : 'Off',
            volume : 'Off',
            repeatDays : 'Off'
        }
    }

    var powerOffInfo =  dsrvWebStorage.getPowerOffTimer();

    if (powerOffInfo.hour) {
        powerOffInfo = powerOffInfo.hour + ':' + powerOffInfo.min;
    } else {
        powerOffInfo = 'Off';
    }

    var powerSaving = dsrvSettingUtil.getStandbyPowerMode();

    var systemInfo = dsrvSettingUtil.getSystemInfo();

    var switchTypeObj = dsrvWebStorage.getLastSearchType();

    var netConnectedInfo = dsrvSettingUtil.getNetConnectedInfo();
    var netAllAddr = dsrvSettingUtil.getallAddress();

    _classifyLog({
        "CODE" : "SETINFO",
        "TRS" : "I",
        "MATURITY" : maturity,
        "SYS_LANG" : LangObj.LangMenu,
        "AUD_LANG" : LangObj.LangAudio,
        "SUB_LANG" : LangObj.LangSubTtl,
        "SCREEN_RATIO" : picInfo.tvAspectRatio,
        "DIS_FORMAT" : picInfo.videoDisplayFormat,
        "RESOLUTION" : picInfo.curResolution,
        "S/PDIF" : audioInfo.audioModePcm,
        "HDMI" : audioInfo.audioModeHdmi,
        "AUD_TYPE" : audioInfo.audioType,
        "LIP-SYNC_DELAY" : audioInfo.lipSyncDelay,
        "INFO_DISPLAY_TIME" : displayTime,
        "SUBTITLE" : subtitle,
        "CONFIGURE_IP" : netAllAddr.type,
        "IP_ADDRESS" : netAllAddr.ipAddress,
        "WIRED_CONNECTION" : netConnectedInfo.wireDevState,
        "WIRELESS_CONNECTION" : netConnectedInfo.wirelessDevState,
        "NETWORK_SERVER" : networkServer,
        "SERVER_ACT_TIME_START" : serverActTime.startHour + ':' + serverActTime.startMin,
        "SERVER_ACT_TIME_END" : serverActTime.endHour + ':' + serverActTime.endMin,
        "SAMBA_SERVER" : sambaServer,
        "FTP_SERVER" : ftpServer,
        "IP_EPG" : ipepg,
        "REMOTE_REC" : remoteRec,
        "REC_UPDATE_PERIOD_START" : recUpdatePeriod.startTime,
        "REC_UPDATE_PERIOD_END" : recUpdatePeriod.endTime,
        "REC_UPDATE_INTERVAL" : recUpdateInterval,
        "START_PADDING_TIME" : startPadding / 60,
        "END_PADDING_TIME" : endPadding / 60,
        "SERIES_PLAY" : seriesPlay,
        "INSTANT_REPLAY_TIME" : instantReplayTime,
        "SKIP_FORWARD_TIME" : skipForwardTime,
        "POWER_ON_TIME" : powerOnInfo.time,
        "POWER_ON_TIME_CH" : powerOnInfo.channel.name,
        "POWER_ON_TIME_VOL" : powerOnInfo.volume,
        "POWER_ON_TIME_REPEAT" : powerOnInfo.repeatDays,
        "POWER_OFF_TIME" : powerOffInfo,
        "AUTO_POWER_DOWN" : autoPowerDown.isOn,
        "POWER_SAVING_IN_STANDBY" : powerSaving,
        "PRIVACY_POLICY" : privacyPolicyVal,
        "WLAN_MAC" : systemInfo.wirelessMacAddress,
        "SWITCH_TYPE" : switchTypeObj.searchType
    });
};
var _logChanneList = function() {
    var tvList, radioList, retObj, favList1, favList2, favList3, favList4, favList5, favName1, favName2, favName3, favName4, favName5;
    var chList = "", favList = "", i;

    retObj = hx.svc.CH_LIST.getChlist({
            group : 'TV'
    });
    tvList = retObj.chList;

    retObj = hx.svc.CH_LIST.getChlist({
            group : 'RADIO'
    });
    radioList = retObj.chList;

    for ( i = 0; i < tvList.length; i += 1) {
        chList += tvList[i].name;
        if (i !== tvList.length - 1) {
            chList += ",";
        }
    }

    for ( i = 0; i < radioList.length; i += 1) {
        if (tvList.length > 0) {
            chList += ",";
        }
        chList += radioList[i].name;
        if (i !== radioList.length - 1) {
            chList += ",";
        }
    }

    //get favourite List
    favList1 = hx.svc.CH_LIST.getChlist({
            group : 'Favourite1'
    });
    favList2 = hx.svc.CH_LIST.getChlist({
            group : 'Favourite2'
    });
    favList3 = hx.svc.CH_LIST.getChlist({
            group : 'Favourite3'
    });
    favList4 = hx.svc.CH_LIST.getChlist({
            group : 'Favourite4'
    });
    favList5 = hx.svc.CH_LIST.getChlist({
            group : 'Favourite5'
    });

    for ( i = 0; i < favList1.chList.length; i += 1) {
        favList += favList1.chList[i].name;
        if (i !== favList1.chList.length - 1) {
            favList += ",";
        }
    }

    for ( i = 0; i < favList2.chList.length; i += 1) {
        if (favList1.chList.length > 0) {
            favList += ",";
        }
        favList += favList2.chList[i].name;
        if (i !== favList2.chList.length - 1) {
            favList += ",";
        }
    }

    for ( i = 0; i < favList3.chList.length; i += 1) {
        if (favList2.chList.length > 0) {
            favList += ",";
        }
        favList += favList3.chList[i].name;
        if (i !== favList3.chList.length - 1) {
            favList += ",";
        }
    }

    for ( i = 0; i < favList4.chList.length; i += 1) {
        if (favList3.chList.length > 0) {
            favList += ",";
        }
        favList += favList4.chList[i].name;
        if (i !== favList4.chList.length - 1) {
            favList += ",";
        }
    }

    for ( i = 0; i < favList5.chList.length; i += 1) {
        if (favList4.chList.length > 0) {
            favList += ",";
        }
        favList += favList5.chList[i].name;
        if (i !== favList5.chList.length - 1) {
            favList += ",";
        }
    }

    _classifyLog({
        "CODE" : "SALIVE",
        "TRS" : "R",
        "CHLIST" : chList,
        "FAVLIST" : favList
    });
};
var _logScheduleList = function() {
    hx.log('debug', 'start collecting schedule(for recording) list');
    var scheduleList = dsrvRecordings2.getScheduledList();
    var inputList = [];
    for (var i = 0, parsedId = null, length = scheduleList.length, scheduleItem = null; i < length; i += 1) {
        scheduleItem = scheduleList[i];
        if (scheduleItem.isRecording()) {   // only for scheduled recording.
            parsedId = hx.svc.IP_EPG.parse.util.programIdParser(scheduleItem.getProgrammeID());
            inputList.push({
                'RSV_INDEX' : i,
                'RSV_TYPE' : scheduleItem.getProgrammeID() ? TRsvType.ERecordEBR : TRsvType.ERecordTBR,
                'RSV_REPEAT' : scheduleItem.getRepeat(),
                'RSV_START_TIME' : scheduleItem.getStartTime().getTime() / 1000,
                'RSV_END_TIME' : scheduleItem.getEndTime().getTime() / 1000,
                'ONID' : scheduleItem.getChannelInstance() ? scheduleItem.getChannelInstance().onid : parsedId.onid,
                'TSID' : scheduleItem.getChannelInstance() ? scheduleItem.getChannelInstance().tsid : parsedId.tsid,
                'SID' : scheduleItem.getChannelInstance() ? scheduleItem.getChannelInstance().sid : parsedId.svcid,
                'EID' : parsedId.eventid ? (parseInt('0x' + parsedId.eventid) || '') : '',
                'EVENT_NAME' : scheduleItem.getTitle() || '',
                'CH_NUM' : scheduleItem.getChannelNo() || '',
                'CH_NAME' : scheduleItem.getChannelName() || '',
                'PROGRAMME_ID' : scheduleItem.getProgrammeID() || '',
                'PROGRAMME_CRID' : scheduleItem.getProgrammeCRID() || '',
                'SERIES_CRID' : scheduleItem.getSeriesId() || ''
            });
        }
    }
    _classifyLog({
        "CODE" : "SRSV",
        "TRS" : "R",
        "NUM_RSVLIST" : inputList.length,
        "RSVLIST" : inputList
    });
    hx.log('debug', 'end collecting schedule(for recording) list : ' + inputList.length);
};
var _logUsbStorageInfo = function () {
    var storageList = dsrvStorageMass.getDevice(), usbList = [], i, retObj, storageDetailInfo;

    for (i = 0; i < storageList.length; i++) {
        if(storageList[i].partitions[0]) {
            if (storageList[i].partitions[0].kind === TPhysicalStorageKind.EKindUSBMemory) {
                usbList.push(storageList[i].partitions[0]);
            }
        }
    }

    for (i = 0; i < usbList.length; i++) {
        storageDetailInfo = dsrvStorageMass.getDetailInfo(usbList[i]);
        if(storageDetailInfo) {
            usbList[i].totalSize = storageDetailInfo.totalSize;
        }
    }

    _classifyLog({
        "CODE" : "SUSB",
        "TRS" : "I",
        "USB1" : usbList[TPhysicalStorageItem.First] ? usbList[TPhysicalStorageItem.First].label : "",
        "USB1_AMOUNT" : usbList[TPhysicalStorageItem.First] ? usbList[TPhysicalStorageItem.First].totalSize + "GB": "",
        "USB1_LOCATION" : usbList[TPhysicalStorageItem.First] ? (usbList[TPhysicalStorageItem.First].id === TPhysicalStorageType.ETypeInternal ? "Front" : "REAR") : "",
        "USB2" : usbList[TPhysicalStorageItem.Second] ? usbList[TPhysicalStorageItem.Second].label : "",
        "USB2_AMOUNT" : usbList[TPhysicalStorageItem.Second] ? usbList[TPhysicalStorageItem.Second].totalSize + "GB" : "",
        "USB2_LOCATION" : usbList[TPhysicalStorageItem.Second] ? (usbList[TPhysicalStorageItem.Second].id === TPhysicalStorageType.ETypeInternal ? "Front" : "REAR") : "",
    });
};

var _logIpEpgInfo = function() {
    var updateProgress = dsrvSettingUtil.getIpepgDownloadProgress(), lastUpdateTime, ipEpgDownloadSvcCount, ipEpgDownloadEvtCount;

    hx.log('dsrvStatistics', '_logIpEpgInfo === updateProgress :' + updateProgress);

    if (updateProgress > 0 && updateProgress < 100) {
        isIpEpgDownEnd = false;
        if ((!isIpEpgDownStart) && (!mIsIpEpgDownloadStart)){
            _classifyLog({
                "CODE" : "GES",
                "TRS" : "I"
            });

            dsrvWebStorage.setStatisticsIpEpgDownloadStartEnable(true);
            mIsIpEpgDownloadStart = true;
        }
        isIpEpgDownStart = true;
    } else if (updateProgress === 100) {
        isIpEpgDownStart = false;
        lastUpdateTime = dsrvSettingUtil.getIpepgLastUpdateTime();

        hx.log('dsrvStatistics', 'lastUpdateTime - dsrvSettingUtil.getIpepgLastUpdateTime() : ' + lastUpdateTime);

        if ((!isIpEpgDownEnd) && (mLastUpdateIpEpgTime !== lastUpdateTime) && (lastUpdateTime !== 0)) {
            ipEpgDownloadSvcCount = dsrvSettingUtil.getIpEpgSvcCount();
            ipEpgDownloadEvtCount = dsrvSettingUtil.getIpEpgEvtCount();

            _classifyLog({
                "CODE" : "GEE",
                "TRS" : "I",
                "GET_SVC_NUM" : ipEpgDownloadSvcCount,
                "GET_EVT_NUM" : ipEpgDownloadEvtCount
            });

            dsrvWebStorage.setStatisticsIpEpgDownloadStartEnable(false);
            mIsIpEpgDownloadStart = false;

            dsrvWebStorage.setStatisticsLastUpdateIpEpgTime(lastUpdateTime);
            mLastUpdateIpEpgTime = lastUpdateTime;
        }
        isIpEpgDownEnd = true;
    }
};

var _logHdmiEdidInfo = function() {
    if (!hx.emulMode) {
        var hdmiEdidInfo = dsrvSettingUtil.getHdmiEdidInfo();
        _classifyLog({
            "CODE" : "SHDMI",
            "TRS" : "I",
            "VENDOR_ID" : hdmiEdidInfo.VENDOR_ID,
            "PRODUCT_ID" : hdmiEdidInfo.PRODUCT_ID,
            "SERIAL_NUM" : hdmiEdidInfo.SERIAL_NUM,
            "MONITOR_NAME" : hdmiEdidInfo.MONITOR_NAME,
            "MANUF_WEEK" : hdmiEdidInfo.MANUF_WEEK,
            "MANUF_YEAR" : hdmiEdidInfo.MANUF_YEAR,
            "EDID_VERSION" : hdmiEdidInfo.EDID_VERSION,
            "EDID_REVISION" : hdmiEdidInfo.EDID_REVISION,
            "FIRST_VIDEO_ID" : hdmiEdidInfo.FIRST_VIDEO_ID
        });
    }
};

var _getGenreList = function(param) {
    var i, length, major, minor, word, genre, genres = [], temp = param || [];

    function validate(value, min, max) {
        return (!isNaN(value) && value > min && value < max);
    }

    for ( i = 0, length = temp.length; i < length; i += 1) {
        genre = temp[i] || '';
        major = parseInt(genre.charAt(0), 16) - 1;
        minor = parseInt(genre.charAt(1), 16);

        if (validate(major, -1, TMajorGenreType.length)) {
            if (validate(minor, -1, TMinorGenreType.length)) {
                genres.push(TMinorGenreType[major][minor]);
            } else {
                if (TMinorGenreType.length === 0) {
                    genres.push(TMajorGenreType[major]);
                }
            }
        }
    }
    return genres[0];
};

var _logMediaInfo = function (a, param) {
    var item = param.item, code = param.code;
    var oipfObj = item.getPluginObject(), size = oipfObj ? oipfObj.size : 0;
    var contentType = item.getContentType(), fileType = item.getFileType(), isWatched = '', storageType = item.getStorageType();

    switch (contentType) {
        case TMediaContentType.ETypeVideo :
            contentType = "Video";
            break;
        case TMediaContentType.ETypeMusic :
            contentType = "Music";
            break;
        case TMediaContentType.ETypePhoto :
            contentType = "Photo";
            break;
        default :
            contentType = "Unknown";
            break;
    }

    switch (fileType) {
        case TMediaFileType.ETypeFile :
            fileType = "File";
            break;
        case TMediaFileType.ETypeFolder :
            fileType = "Folder";
            break;
        case TMediaFileType.ETypeGroup :
            fileType = "Group";
            break;
        default :
            fileType = "Unknown";
            break;
    }
    if (item.isWatched) {
        var watchedType = item.isWatched();
        switch (watchedType) {
            case TMediaWatchedType.ETypeNotWatched :
                isWatched = "Not_Watched";
                break;
            case TMediaWatchedType.ETypeWatchedHalf :
                isWatched = "Watched_Half";
                break;
            case TMediaWatchedType.ETypeWatchedFull :
                isWatched = "Watched_Full";
                break;
            default :
                isWatched = "Not_Watched";
                break;
        }
    }



    switch (storageType) {
        case TMediaStorageType.ETypeSATA :
            storageType = "SATA";
            break;
        case TMediaStorageType.ETypeUSB :
            storageType = "USB";
            break;
        case TMediaStorageType.ETypeSD :
            storageType = "SD";
            break;
        case TMediaStorageType.ETypeSAMBA :
            storageType = "SAMBA";
            break;
        default :
            storageType = "Unknown";
            break;
    }

    a.statisticsLog({
        "CODE" : code,
        "TRS" : "I",
        "MEDIA_FILE_NAME" : item.getTitle(),
        "MEDIA_CONTENT_TYPE" : contentType,
        "MEDIA_FILE_TYPE" : fileType,
        "MEDIA_FULL_PATH" : item.getFullPath ? item.getFullPath() : '',
        "MEDIA_HAS_SUB_ITEM" : '',//item.hasSubItem(),
        "MEDIA_LOCAL_FILE" : '',//item.isLocalMedia(),
        "MEDIA_SIZE" : size,
        "MEDIA_DATE" : item.getDate ? item.getDate() : 0,
        "MEDIA_DURATION" : item.getDuration ? item.getDuration() : 0,
        "MEDIA_BIT_RATE" : item.getBitRate ? item.getBitRate() : '',
        "MEDIA_SAMPLE_RATE" : item.getSampleRate ? item.getSampleRate() : '',
        "MEDIA_LAST_VIEW_TIME" : item.getLastPlayPosition ? item.getLastPlayPosition() : 0,
        "MEDIA_LARGE_THUMB_URL" : item.getLargeThumbnailUrl ? item.getLargeThumbnailUrl() : '',
        "MEDIA_THUMB_URL" : item.getThumbnailUrl ? item.getThumbnailUrl() : '',
        "MEDIA_GENRE" : item.getGenre ? item.getGenre() : '',
        "MEDIA_RESOLUTION" : item.getResolution ? item.getResolution() : null,
        "MEDIA_THUMB_RESOLUTION" : item.getThumbnailResolution ? item.getThumbnailResolution() : null,
        "MEDIA_PICTURE_DATE" : item.getPictureuDate ? item.getPictureDate() : '',
        "MEDIA_ALBUM" : item.getAlbumName ? item.getAlbumName() : '',
        "MEDIA_ARTIST" : item.getArtistName ? item.getArtistName() : '',
        "MEDIA_STORAGE_CATEGORY" : storageType,
        "MEDIA_WATCHED" : isWatched
    });
};


class cDLStatistics extends __KERNEL__ {
    _playItem: any;
    constructor(aServiceName:string) {
        super(aServiceName);
        //hx.logadd('dsrvStatistics');
        dsrvRecordings2.addEventCb('NotiRecordingStarted', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            var curChannel = hx.svc.CH_LIST.getCurrentChannel().channel;
            var onId, tsId, sId;
            var chName = '', evtName = '';

            if (curChannel) {
                chName = curChannel.name;
                onId = curChannel.onid;
                tsId = curChannel.tsid;
                sId = curChannel.sid;
            }
            evtName = e.item.getTitle();

            var pId = e.item.getId(), tmp;
            if (pId) {
                tmp = pId.split(';');
                if (tmp.length > 1) {
                    pId = parseInt(tmp[1], 16).toString(10);
                }
            }

            this.statisticsLog({
                "CODE": "ERECS", //EVENT_REC_START
                "TRS": "R",
                "ONID": onId,
                "TSID": tsId,
                "SID": sId,
                "EID": pId,
                "CHNAME": chName,
                "EVTNAME" : evtName
            });

            hx.log('dsrvStatistics', 'NotiRecordingStarted');
        });

        dsrvRecordings2.addEventCb('RecordingCompleted', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false || !(e.item.isNew())) {
                return;
            }
            var curChannel = hx.svc.CH_LIST.getCurrentChannel().channel;
            var onId, tsId, sId;
            var chName = '', evtName = '';

            if (curChannel) {
                chName = curChannel.name;
                onId = curChannel.onid;
                tsId = curChannel.tsid;
                sId = curChannel.sid;
            }

            evtName = e.item.getTitle();

            var pId = e.item.getId(), tmp;
            if (pId) {
                tmp = pId.split(';');
                if (tmp.length > 1) {
                    pId = parseInt(tmp[1], 16).toString(10);
                }
            }

            this.statisticsLog({
                "CODE": "ERECE", //EVENT_REC_END
                "TRS": "R",
                "ONID": onId,
                "TSID": tsId,
                "SID": sId,
                "EID": pId,
                "CHNAME" : chName,
                "EVTNAME" : evtName
            });

            hx.log('dsrvStatistics', 'RecordingCompleted');
        });
        dsrvRecordings2.addEventCb('NotiLiveStreamingStart', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            var fnIsScheduled = function(item) {
                return (!item.getId()) ? true : false;
            };
            var item = e.item;

            hx.log('dsrvStatistics', 'NotiLiveStreamingStart === fnIsScheduled(item) : ' + fnIsScheduled(item));
            if (fnIsScheduled(item)) {
                if (item.isStreaming()) {
                    var onId = 0;
                    var tsId = 0;
                    var sId = 0;
                    var chName = '';

                    if (item._property) {
                        if (item._property.channel) {
                            chName = item._property.channel.name;
                            onId = item._property.channel.onid;
                            tsId = item._property.channel.tsid;
                            sId = item._property.channel.sid;

                            hx.svc.EPG.getProgrammes({
                                isOnAir : true,
                                filter: "P/F",
                                channel : item._property.channel,
                                listener : this,
                                notUI : true,
                                cb : (result) => {
                                    if (result[0][0]) {
                                        var programmeID = result[0][0].programmeID, tmp, eventID;
                                        var evtName = '';
                                        evtName = result[0][0].name;

                                        if (programmeID) {
                                            tmp = programmeID.split(';');
                                            if (tmp.length > 1) {
                                                eventID = parseInt(tmp[1], 16).toString(10);
                                            }
                                        }
                                        this.statisticsLog({
                                            "CODE" : "ELSS", //EVENT_LIVE_STREAMING_START
                                            "TRS" : "R",
                                            "ONID" : onId,
                                            "TSID" : tsId,
                                            "SID" : sId,
                                            "EID" : eventID,
                                            "CHNAME" : chName,
                                            "EVTNAME" : evtName
                                        });

                                        hx.log('dsrvStatistics', 'Live Streaming Start === onId : ' + onId + ',' + 'tsId : ' + tsId + ',' + 'sId : ' + sId);
                                    }
                                    result = undefined;
                                }
                            });
                        }
                    }
                }
            }
            hx.log('dsrvStatistics', 'NotiLiveStreamingStart');
        });

        dsrvRecordings2.addEventCb('NotiLiveStreamingStop', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            var item = e.item;

            hx.log('dsrvStatistics', 'NotiLiveStreamingStop');
            if (item.isStreaming()) {
                var onId = 0;
                var tsId = 0;
                var sId = 0;
                var chName = '';

                if (item._property) {
                    if (item._property.channel) {
                        chName = item._property.channel.name;
                        onId = item._property.channel.onid;
                        tsId = item._property.channel.tsid;
                        sId = item._property.channel.sid;

                        hx.svc.EPG.getProgrammes({
                            isOnAir : true,
                            filter: "P/F",
                            channel : item._property.channel,
                            listener : this,
                            notUI : true,
                            cb : (result) => {
                                if (result[0][0]) {
                                    var programmeID = result[0][0].programmeID, tmp, eventID;
                                    var evtName = '';
                                    evtName = result[0][0].name;

                                    if (programmeID) {
                                        tmp = programmeID.split(';');
                                        if (tmp.length > 1) {
                                            eventID = parseInt(tmp[1], 16).toString(10);
                                        }
                                    }
                                    this.statisticsLog({
                                        "CODE" : "ELSE", //EVENT_LIVE_STREAMING_END
                                        "TRS" : "R",
                                        "ONID" : onId,
                                        "TSID" : tsId,
                                        "SID" : sId,
                                        "EID" : eventID,
                                        "CHNAME" : chName,
                                        "EVTNAME" : evtName
                                    });

                                    hx.log('dsrvStatistics', 'Live Streaming END === onId :' + onId + ',' + 'tsId :' + tsId + ',' + 'sId :' + sId);
                                }
                                result = undefined;
                            }
                        });
                    }
                }
                hx.log('dsrvStatistics', '=== NotiLiveStreamingStop === Live Streaming END');
            }
        });

        dsrvRecordings2.addEventCb('NotiScheduledStartError', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            var curChannel = hx.svc.CH_LIST.getCurrentChannel().channel;
            var onId, tsId, sId;
            var chName = '', evtName = '';

            if (curChannel) {
                chName = curChannel.name;
                onId = curChannel.onid;
                tsId = curChannel.tsid;
                sId = curChannel.sid;
            }

            evtName = e.item.getTitle();

            var pId = e.item.getId(), tmp;
            if (pId) {
                tmp = pId.split(';');
                if (tmp.length > 1) {
                    pId = parseInt(tmp[1], 16).toString(10);
                }
            }

            this.statisticsLog({
                "COID": "EREMIND", //EVENT_REMIND
                "TRS": "R",
                "ONID": onId,
                "TSID": tsId,
                "SID": sId,
                "EID": pId,
                "CHNAME" : chName,
                "EVTNAME" : evtName
            });

            hx.log('dsrvStatistics', 'NotiScheduledStartError');
        });

        dsrvBridge.addEventCb('RequestMusicPlayback', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            /*
             items : items,
             startIndex : startIndex
             */
            var item = e.items[e.startIndex], code = "EMPLAYS";
            //EVENT_MUSIC_PLAYBACK_START
            this.fnSetMediaInfo(this, {
                'item' : item,
                'code' : code
            });
            hx.log('dsrvStatistics', 'RequestMusicPlayback');
        });

        dsrvBridge.addEventCb('RequestVideoPlayback', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            /*
             items : items,
             startIndex : startIndex
             */
            var CTsrItem = __MODEL__.CTsrItem;
            var item = e.items[e.startIndex];
            var code;

            if (item instanceof __MODEL__.IRecordingItem) {
                var chName = "";
                var progName = "";
                var programmeID = '', genre, eventID;

                if (item._property){
                    if (item._property.channel) {
                        var onId = item._property.channel.onid;
                        var tsId = item._property.channel.tsid;
                        var sId = item._property.channel.sid;
                        //chName = item._property.channel.name;
                        programmeID = item._property.programmeID;

                        if (programmeID) {
                            var tmp = programmeID.split(';');
                            if (tmp.length > 1) {
                                eventID = parseInt(tmp[1], 16).toString(10);
                            }
                        }

                        progName = item._property.name;
                        genre = this.fnGetGenreList(item._property.genres);
                    }
                }

                hx.log('dsrvStatistics', 'RequestRecordPlayback : IRecordingItem');
                code = "ERPLAYS";
                //EVENT_RECORDING_PLAYBACK_START
                this.statisticsLog({
                    "CODE" : code,
                    "TRS" : "I",
                    "ONID" : onId,
                    "TSID" : tsId,
                    "SID" : sId,
                    "EID" : eventID,
                    //"CH_NAME" : chName,
                    "PROG_NAME" : progName,
                    "GENRE" : genre
                });
            /*} else if (item instanceof CTsrItem) {
                //EVENT_TSR_PLAYBACK_START
                code = "ETSRPLAYS";*/
            } else {
                var a = this;
                a._playItem = item || a._playItem;
                code = "EVPLAYS";
                //EVENT_VIDEO_PLAYBACK_START
                this.fnSetMediaInfo(a, {
                    'item' : a._playItem,
                    'code' : code
                });
            }

            hx.log('dsrvStatistics', 'RequestVideoPlayback code : ' + code);
        });

        dsrvBridge.addEventCb('RequestPhotoPlayback', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            /*
             items : items,
             startIndex : startIndex
             */
            var a = this;
            var item = e.items[e.startIndex], code = "EPPLAYS";
            //EVENT_PHOTO_PLAYBACK_START
            this.fnSetMediaInfo(a, {
                'item': item,
                'code': code
            });

            hx.log('dsrvStatistics', 'RequestPhotoPlayback');
        });

        dsrvSettingUtil.addEventCb('NetworkAvailableChanged', this, (networkAvailable) => {
            if (!networkAvailable) {
                isSstart = false;
            }
            if (!isSstart) {
                this.setStatisticsMode(networkAvailable);
            }
            mNetworkAvailableStatus = networkAvailable;
            hx.log('dsrvStatistics', 'NetworkAvailableChanged networkAvailable : ' + networkAvailable + ', mOperateStatus : ' + mOperateStatus + ',  mPolicyStatus : ' + mPolicyStatus);
        });

        dsrvTVPortal.addEventCb('privacyPolicyChange', this, (policyValue) => {
            this.setStatisticsPolicy(policyValue);
            hx.log('dsrvStatistics', 'privacyPolicyChange policyValue : ' + policyValue);
        });

        dsrvStorageUtil.addEventCb('PhysicalStorageAttached', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            this.fnSetUsbStorageInfo();
            hx.log('dsrvStatistics', 'PhysicalStorageAttached');
        });

        dsrvStorageUtil.addEventCb('PhysicalStorageDetached', this, (e) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            this.fnSetUsbStorageInfo();
            hx.log('dsrvStatistics', 'PhysicalStorageDetached');
        });

        dsrvAppMgr.addEventCb('ApplicationStart', this, (appName) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            this.statisticsLog({
                "CODE" : "APPIN",
                "TRS" : "I",
                "APP_NAME" : appName
            });
            hx.log('dsrvStatistics', 'ApplicationStart');
        });

        dsrvAppMgr.addEventCb('ApplicationEnd', this, (appName) => {
            if (mOperateStatus === false || mPolicyStatus === false) {
                return;
            }
            this.statisticsLog({
                "CODE" : "APPOUT",
                "TRS" : "I",
                "APP_NAME" : appName
            });
            hx.log('dsrvStatistics', 'ApplicationEnd');
        });

        if (hx.config.netflix === true) {
            dsrvNetflix.addEventCb('NetflixStatusChanged', this, (status) => {
                if (status === __NETFLIX__.RUNNING_STATE.RUNNING) {
                    this.statisticsLog({
                        "CODE": "APPIN",
                        "TRS": "I",
                        "APP_NAME": "Netflix"
                    });
                    hx.log('dsrvStatistics', 'Netflix Start');
                } else {
                    this.statisticsLog({
                        "CODE": "APPOUT",
                        "TRS": "I",
                        "APP_NAME": "Netflix"
                    });
                    hx.log('dsrvStatistics', 'Netflix End');
                }
            });
        }
    }



    statisticsLog(param) {
        if (mOperateStatus === true && mPolicyStatus === true) {
            _classifyLog(param);
        }
    }
    startLog() {
        this._changeMode(dsrvSettingUtil.getNetworkAvailable());
        this.setStatisticsPolicy(dsrvTVPortal.isPrivacyPolicyAccepted());
    }
    setStatisticsMode(param: boolean) {
        this._changeMode(param);
    }
    setStatisticsPolicy(param) {
        if (!param) {
            param = false;
        }
        this._changePolicy(param);
    }
    transmitLog() {
        if (mOperateStatus === true && mPolicyStatus === true) {
            this._transAllLog();
        }
    }
    fnSetUsbStorageInfo() {
        bUsblist = true;
        _logUsbStorageInfo();
    }

    fnSetMediaInfo(a, param) {
        _logMediaInfo(a, param);
    }

    fnSetFailSendErrorLog() {
        _classifyLog({
            "CODE" : "FAIL_SEND",
            "TRS" : "I",
            "ERROR_CODE" : "Unknown"
        });
    }

    fnGetGenreList(param) {
        return _getGenreList(param);
    }
    //test function
    getLog() {
        return mStatisticsLog;
    }
    getSysInfo() {
        return mSysInfo;
    }
    getUrl() {
        return mUrl;
    }
    _transAllLog() {
        //var result;
        var i;
        var authTicket = hx.svc.TV_PORTAL.getAuthTicket();
        hx.log('dsrvStatistics', '_transAllLog start function');

        _logScheduleList();

        if (!authTicket) {
            hx.log('error', 'dsrvStatistics _transAllLog authTicket is ' + authTicket);
            mStatisticsLog = [];
            return;
        } else {
            hx.log('debug', 'dsrvStatistics _transAllLog authTicket is ' + authTicket);
        }

        if (mStatisticsLog.length === 0) {
            hx.log('dsrvStatistics', 'Log Count is 0');
        } else {
            var transLog = mStatisticsLog;
            for (i = 0; i < transLog.length; i += 1) {
                delete mStatisticsLog[i].TRS;
            }
            if (mNetworkAvailableStatus) {
                if (mStatisticsErrorLog.length !== 0) {
                    this.fnSetFailSendErrorLog();
                    for (i = 0; i < mStatisticsErrorLog.length; i++) {
                        delete mStatisticsErrorLog[i].TRS;
                        mStatisticsLog.push(mStatisticsErrorLog[i]);
                    }

                    mStatisticsErrorLog = [];
                    mErrorLogFlag = false;
                }
                transLog = mStatisticsLog;
            } else {
                if (!mErrorLogFlag) {
                    this.fnSetFailSendErrorLog();
                    //fnSetFailSendErrorLog();
                    mErrorLogFlag = true;
                }
                mStatisticsLog = [];
                transLog = mStatisticsLog;
            }
            var needCheck = false;
            var date = _currentDate();
            var day = dateFormat(date, "dd");

            if (mCheckUploadTransTime === null || mCheckUploadTransTime === undefined) {
                needCheck = true;
                dsrvWebStorage.setStatisticsLastSendingTimeOfCheckUpload(date);
                mCheckUploadTransTime = date;
                hx.log('dsrvStatistics', 'needCheck: ' + needCheck);
            } else {
                var tempCheckUploadTransTime = dateFormat(mCheckUploadTransTime, "dd");
                if (tempCheckUploadTransTime !== day) {
                    needCheck = true;
                    dsrvWebStorage.setStatisticsLastSendingTimeOfCheckUpload(date);
                    mCheckUploadTransTime = date;
                }
                hx.log('dsrvStatistics', 'needCheck: ' + needCheck);
            }
            dsrvTVPortal.login({
                fn : (logged) => {
                    hx.log('dsrvStatistics', 'login: ' + logged ? 'OK' : 'FAIL');
                    if (logged) {
                        if (needCheck) {
                            dsrvTVPortal.getCheckUploadStatistics({
                                fn : (result) => {
                                    var resultVal = result.status;
                                    this._checkUploadStatistics(resultVal, transLog);
                                },
                                fnError : (result) => {
                                    this._changeMode(false);
                                }
                            });
                        } else {
                            hx.log('dsrvStatistics', 'transLog: ' + JSON.stringify(transLog));
                            dsrvTVPortal.setStatisticsLog({
                                statisticsLog : transLog,
                                fn : (result) => {
                                    mUrl = result.message;
                                    hx.log('dsrvStatistics', 'return Url: ' + mUrl);

                                },
                                fnError : (result) => {
                                }
                            });
                        }
                    } else {
                        var errorCode = dsrvTVPortal.getLastErrorCode();
                        hx.log('dsrvStatistics', 'getLastErrorCode: ' + errorCode);
                    }
                }
            });
            mStatisticsLog = [];
        }
    }
    _intervalTimer() {
        this._transAllLog();
    }
    _aliveTimer() {
        var date = _currentDate();
        var day = dateFormat(date, "dd");

        if (mSettingsTransTime === null || mSettingsTransTime === undefined) {
            _logSettingVal();
            dsrvWebStorage.setStatisticsLastSendingTimeOfSettingInfo(date);
            mSettingsTransTime = date;
        } else {
            var tempSettingsTransTime = dateFormat(mSettingsTransTime, "dd");
            if (tempSettingsTransTime !== day) {
                _logSettingVal();
                dsrvWebStorage.setStatisticsLastSendingTimeOfSettingInfo(date);
                mSettingsTransTime = date;
            }
        }
        //_logIpEpgInfo();
        // not used

        if (!bUsblist) {
            _logUsbStorageInfo();
            bUsblist = true;
        }

        if (!bHdmiInfo) {
            _logHdmiEdidInfo();
            bHdmiInfo = true;
        }

        var localTimeOffset;
        var curChannel = hx.svc.CH_LIST.getCurrentChannel().channel;
        var chName = '';

        if (curChannel) {
            chName = curChannel.name;

            hx.svc.EPG.getProgrammes({
                isOnAir : true,
                filter: "P/F",
                channel : curChannel,
                listener : this,
                notUI : true,
                cb : (result) => {
                    if (result[0][0]) {
                        var programmeID = result[0][0].programmeID, tmp, eventID;
                        var evtName = '';
                        evtName = result[0][0].name;

                        if (programmeID) {
                            tmp = programmeID.split(';');
                            if (tmp.length > 1) {
                                eventID = parseInt(tmp[1], 16).toString(10);
                            }
                        }

                        _classifyLog({
                            "CODE": "SALIVE", //STATUS_ALIVE_LOG
                            "TRS": "R",
                            "ONID": curChannel.onid,
                            "TSID": curChannel.tsid,
                            "SID": curChannel.sid,
                            "EID": eventID,
                            "CHNAME": chName,
                            "EVTNAME": evtName
                        });
                        hx.log('dsrvStatistics', '_aliveTimer === EID :' + eventID + ' programmeID : ' + programmeID);
                        hx.log('dsrvStatistics', '_aliveTimer === ONID : ' + curChannel.onid + '  TSID : ' + curChannel.tsid + '  SID : ' + curChannel.sid);
                    }
                    result = undefined;
                }
            });
        }
    }
    _checkUploadStatistics (resultVal, transLog) {
        if (resultVal) {
            hx.log('dsrvStatistics', 'getCheckUploadStatistics : ' + resultVal);
            hx.log('dsrvStatistics', 'transLog: ' + JSON.stringify(transLog));
            dsrvTVPortal.setStatisticsLog({
                statisticsLog : transLog,
                fn : (result) => {
                    mUrl = result.message;
                    hx.log('dsrvStatistics', 'return Url: ' + mUrl);

                },
                fnError : (result) => {
                }
            });
        } else {
            this._changeMode(false);
        }
    }
    _changeMode(param: boolean) {
        mOperateStatus = param;
        mNetworkAvailableStatus = param;
        var that = this;
        if (mOperateStatus === true && mPolicyStatus === true) {
            mSysInfo = dsrvSettingUtil.getSystemInfo();
            _startLog();

            clearInterval(mTimerInterval);
            clearInterval(mTimerAlive);
            mTimerInterval = setInterval(() => {
                that._intervalTimer();
            }, TINTERVAL);
            mTimerAlive = setInterval(() => {
                that._aliveTimer();
            }, TALIVE);
        } else {
            clearInterval(mTimerInterval);
            clearInterval(mTimerAlive);
        }

        hx.log('dsrvStatistics', '_changeMode mOperateStatus : ' + mOperateStatus + '  mPolicyStatus : ' + mPolicyStatus);
    }
    _changePolicy(param: boolean) {
        mPolicyStatus = param;
        var that = this;
        if (mOperateStatus === true && mPolicyStatus === true) {
            mSysInfo = dsrvSettingUtil.getSystemInfo();
            _startLog();

            clearInterval(mTimerInterval);
            clearInterval(mTimerAlive);
            mTimerInterval = setInterval(() => {
                that._intervalTimer();
            }, TINTERVAL);
            mTimerAlive = setInterval(() => {
                that._aliveTimer();
            }, TALIVE);
        } else {
            clearInterval(mTimerInterval);
            clearInterval(mTimerAlive);
        }

        hx.log('dsrvStatistics', '_changePolicy mOperateStatus : ' + mOperateStatus + '  mPolicyStatus : ' + mPolicyStatus);
    }
}
export = cDLStatistics;