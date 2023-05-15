/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');



class CMediaPlay extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Media.Play', '/Octopus/Appkit/Media/Play');
    }

    StartEvent(aCb: (msgInt: number, msgStr: string, param1: number, param2: number, param3: number) => void) {
        this._call( function (iface){
            iface.on('event', function(msgInt: number, msgStr: string, param1: number, param2: number, param3: number) {
                aCb(msgInt, msgStr, param1, param2, param3);
            });
        });
    }

    GetViewNumber(aCb: (aViewNum: number) => void) {
        this._call( function (iface){
            iface.GetViewNumber['finish'] = function(aViewNum: number) {
                aCb(aViewNum);
            };
            iface.GetViewNumber();
        });
    }

    SetMainViewId(aViewId: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetMainViewId['finish'] = function() {
                aCb();
            };
            iface.SetMainViewId(aViewId);
        });
    }

    GetMainViewId(aCb: (aViewId: number) => void) {
        this._call( function (iface){
            iface.GetMainViewId['finish'] = function(aViewId: number) {
                aCb(aViewId);
            };
            iface.GetMainViewId();
        });
    }

    GetPlayType(aViewId: number, aCb: (aPlayType: number) => void) {
        this._call( function (iface){
            iface.GetPlayType['finish'] = function(aPlayType: number) {
                aCb(aPlayType);
            };
            iface.GetPlayType(aViewId);
        });
    }

    GetNeedRadioBg(aViewId: number, aCb: (aIsNeed: number) => void) {
        this._call( function (iface){
            iface.GetNeedRadioBg['finish'] = function(aIsNeed: number) {
                aCb(aIsNeed);
            };
            iface.GetNeedRadioBg(aViewId);
        });
    }

    GetSessionId(aViewId: number, aCb: (aSessionId: number) => void) {
        this._call( function (iface){
            iface.GetSessionId['finish'] = function(aSessionId: number) {
                aCb(aSessionId);
            };
            iface.GetSessionId(aViewId);
        });
    }

    GetRequestId(aViewId: number, aPlaytype: number, aCb: (aRequestId: number) => void) {
        this._call( function (iface){
            iface.GetRequestId['finish'] = function(aRequestId: number) {
                aCb(aRequestId);
            };
            iface.GetRequestId(aViewId, aPlaytype);
        });
    }

    GetStreamAspectRatio(aViewId: number, aCb: (aAspectRatio: number) => void) {
        this._call( function (iface){
            iface.GetStreamAspectRatio['finish'] = function(aAspectRatio: number) {
                aCb(aAspectRatio);
            };
            iface.GetStreamAspectRatio(aViewId);
        });
    }

    GetMhegDisplayPoint(aViewId: number, aRefWidth: number, aRefHeight:number, aVideoPointX: number, aVideoPointY: number, aCb: (aScaledSizeW: number, aScaledSizeH: number) => void) {
        this._call( function (iface){
            iface.GetMhegDisplayPoint['finish'] = function(aScaledSize: any) {
                aCb(aScaledSize[0], aScaledSize[1]);
            };
            iface.GetMhegDisplayPoint(aViewId, aRefWidth, aRefHeight, aVideoPointX, aVideoPointY);
        });
    }

    StartAudioClip(aRequestId: number, aBuffer: ArrayBuffer, aBufferBytes: number, aAudioCodec: number, aLoopCount: number, aCb: () => void) {
        this._call( function (iface){
            iface.StartAudioClip['finish'] = function() {
                aCb();
            };
            iface.StartAudioClip(aRequestId, aBuffer, aBufferBytes, aAudioCodec, aLoopCount);
        });
    }

    PauseAudioClip(aRequestId: number, aCb: () => void) {
        this._call( function (iface){
            iface.PauseAudioClip['finish'] = function() {
                aCb();
            };
            iface.PauseAudioClip(aRequestId);
        });
    }

    ResumeAudioClip(aRequestId: number, aCb: () => void) {
        this._call( function (iface){
            iface.ResumeAudioClip['finish'] = function() {
                aCb();
            };
            iface.ResumeAudioClip(aRequestId);
        });
    }

    StopAudioClip(aRequestId: number, aCb: () => void) {
        this._call( function (iface){
            iface.StopAudioClip['finish'] = function() {
                aCb();
            };
            iface.StopAudioClip(aRequestId);
        });
    }

    SetVideoFreeze(aViewId: number, aFreeze: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetVideoFreeze['finish'] = function() {
                aCb();
            };
            iface.SetVideoFreeze(aViewId, aFreeze);
        });
    }

    SetAudioFreeze(aViewId: number, aFreeze: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAudioFreeze['finish'] = function() {
                aCb();
            };
            iface.SetAudioFreeze(aViewId, aFreeze);
        });
    }

    SetComponentIndex(aViewId: number, aCompType: number, aCompIndex: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetComponentIndex['finish'] = function() {
                aCb();
            };
            iface.SetComponentIndex(aViewId, aCompType, aCompIndex);
        });
    }

    GetComponentNum(aViewId: number, aCompType: number, aCb: (aCompCount: number) => void) {
        this._call( function (iface){
            iface.GetComponentNum['finish'] = function(aCompCount: number) {
                aCb(aCompCount);
            };
            iface.GetComponentNum(aViewId, aCompType);
        });
    }

    GetComponentIndex(aViewId: number, aCompType: number, aCb: (aCompIndex: number) => void) {
        this._call( function (iface){
            iface.GetComponentIndex['finish'] = function(aCompIndex: number) {
                aCb(aCompIndex);
            };
            iface.GetComponentIndex(aViewId, aCompType);
        });
    }

    StartLive(aViewId: number, aStartInfo: tdata.TStartInfoLive, aCb: (aSessionId: number) => void) {
        this._call( function (iface){
            iface.StartLive['finish'] = function(aSessionId: number) {
                aCb(aSessionId);
            };
            iface.StartLive(aViewId, aStartInfo);
        });
    }


    StartPvrPb(aViewId: number, aStartInfo: tdata.TStartInfoPvrPb, aCb: (aSessionId: number) => void) {
        this._call( function (iface){
            iface.StartPvrPb['finish'] = function(aSessionId: number) {
                aCb(aSessionId);
            };
            iface.StartPvrPb(aViewId, aStartInfo);
        });
    }

    StartTsrPb(aViewId: number, aStartInfo: tdata.TStartInfoTsrPb, aCb: (aSessionId: number) => void) {
        this._call( function (iface){
            iface.StartTsrPb['finish'] = function(aSessionId: number) {
                aCb(aSessionId);
            };
            iface.StartTsrPb(aViewId, aStartInfo);
        });
    }

    StartMedia(aViewId: number, aStartInfo: tdata.TStartInfoMediaPlay, aCb: (aSessionId: number) => void) {
        this._call( function (iface){
            iface.StartMedia['finish'] = function(aSessionId: number) {
                aCb(aSessionId);
            };
            iface.StartMedia(aViewId, aStartInfo);
        });
    }

    Stop(aViewId: number, aCb: () => void) {
        this._call( function (iface){
            iface.Stop['finish'] = function() {
                aCb();
            };
            iface.Stop(aViewId);
        });
    }

    SetPlaySpeed(aViewId: number, aSpeed: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPlaySpeed['finish'] = function() {
                aCb();
            };
            iface.SetPlaySpeed(aViewId, aSpeed);
        });
    }

    SetPlayPosition(aViewId: number, aPosition: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPlayPosition['finish'] = function() {
                aCb();
            };
            iface.SetPlayPosition(aViewId, aPosition);
        });
    }

    GetPlaySpeed(aViewId: number, aCb: (aSpeed: number) => void) {
        this._call( function (iface){
            iface.GetPlaySpeed['finish'] = function(aSpeed: number) {
                aCb(aSpeed);
            };
            iface.GetPlaySpeed(aViewId);
        });
    }

    GetPlayPosition(aViewId: number, aCb: (aPosition: number) => void) {
        this._call( function (iface){
            iface.GetPlayPosition['finish'] = function(aPosition: number) {
                aCb(aPosition);
            };
            iface.GetPlayPosition(aViewId);
        });
    }

    GetPlayState(aViewId: number, aCb: (aState: number) => void) {
        this._call( function (iface){
            iface.GetPlayState['finish'] = function(aState: number) {
                aCb(aState);
            };
            iface.GetPlayState(aViewId);
        });
    }

    GetPlayError(aViewId: number, aCb: (aError: number) => void) {
        this._call( function (iface){
            iface.GetPlayError['finish'] = function(aError: number) {
                aCb(aError);
            };
            iface.GetPlayError(aViewId);
        });
    }

    GetBufferedTime(aViewId: number, aCb: (aBufferred: number) => void) {
        this._call( function (iface){
            iface.GetBufferedTime['finish'] = function(aBufferred: number) {
                aCb(aBufferred);
            };
            iface.GetBufferedTime(aViewId);
        });
    }

    GetDurationTime(aViewId: number, aCb: (aDurationTime: number) => void) {
        this._call( function (iface){
            iface.GetDurationTime['finish'] = function(aDurationTime: number) {
                aCb(aDurationTime);
            };
            iface.GetDurationTime(aViewId);
        });
    }

    GetTsrStartTime(aViewId: number, aCb: (aTstStartTime: number) => void) {
        this._call( function (iface){
            iface.GetTsrStartTime['finish'] = function(aTstStartTime: number) {
                aCb(aTstStartTime);
            };
            iface.GetTsrStartTime(aViewId);
        });
    }

    GetSupportedSpeeds(aViewId: number, aMaxNumSpeed: number, aCb: (aSpeedList: ArrayBuffer) => void) {
        this._call( function (iface){
            iface.GetSupportedSpeeds['finish'] = function(aSpeedList: ArrayBuffer) {
                aCb(aSpeedList);
            };
            iface.GetSupportedSpeeds(aViewId);
        });
    }

    GetTrickRestrictMode(aViewId: number, aCb: (aTrickRestrictMode: number) => void) {
        this._call( function (iface){
            iface.GetTrickRestrictMode['finish'] = function(aTrickRestrictMode: number) {
                aCb(aTrickRestrictMode);
            };
            iface.GetTrickRestrictMode(aViewId);
        });
    }

    GetVideoSize(aViewId: number, aCb: (aVideoSize: ArrayBuffer) => void) {
        this._call( function (iface){
            iface.GetVideoSize['finish'] = function(aVideoSize: ArrayBuffer) {
                aCb(aVideoSize);
            };
            iface.GetVideoSize(aViewId);
        });
    }

    SetVideoSize(aViewId: number, aStartX: number, aStartY: number, aWidth: number, aHeight: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetVideoSize['finish'] = function() {
                aCb();
            };
            iface.SetVideoSize(aViewId, aStartX, aStartY, aWidth, aHeight);
        });
    }

    ResetVideoSize(aViewId: number, aCb: () => void) {
        this._call( function (iface){
            iface.ResetVideoSize['finish'] = function() {
                aCb();
            };
            iface.ResetVideoSize(aViewId);
        });
    }

    SetPigRect(aViewId: number, aZOrder: number, aIsX: number, aIsY: number, aIW: number, aIH: number, aOrX: number, aOrY: number, aOW: number, aOH: number, aOsdAspectRatio: number, aDFC: number, aPigAspectRatio: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPigRect['finish'] = function() {
                aCb();
            };
            iface.SetPigRect(aViewId, aZOrder, aIsX, aIsY, aIW, aIH, aOrX, aOrY, aOW, aOH, aOsdAspectRatio, aDFC, aPigAspectRatio);
        });
    }

    SetTSREnable(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetTSREnable['finish'] = function() {
                aCb();
            };
            iface.SetTSREnable(aEnable);
        });
    }

    SetSubtitleEnable(aViewId: number, aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetSubtitleEnable['finish'] = function() {
                aCb();
            };
            iface.SetSubtitleEnable(aViewId, aEnable);
        });
    }

    SetVideoHide(aViewId: number, aHide: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetVideoHide['finish'] = function() {
                aCb();
            };
            iface.SetVideoHide(aViewId, aHide);
        });
    }

    GetTSREnable(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetTSREnable['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetTSREnable();
        });
    }

    GetSubtitleEnable(aViewId: number, aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetSubtitleEnable['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetSubtitleEnable(aViewId);
        });
    }

    GetEventInfo(aViewId: number, aCb: (aData: ArrayBuffer) => void) {
        this._call( function (iface){
            iface.GetEventInfo['finish'] = function(aData: ArrayBuffer) {
                aCb(aData);
            };
            iface.GetEventInfo(aViewId);
        });
    }

    SaveThumbnail(aViewId: number, aFilename: string, aWidth: number, aHeight: number, aCb: () => void) {
        this._call( function (iface){
            iface.SaveThumbnail['finish'] = function() {
                aCb();
            };
            iface.SaveThumbnail(aViewId, aFilename, aWidth, aHeight);
        });
    }

    GetMajorChannel(aViewId: number, aCb: (aMajorCHNum: number) => void) {
        this._call( function (iface){
            iface.GetMajorChannel['finish'] = function(aMajorCHNum: number) {
                aCb(aMajorCHNum);
            };
            iface.GetMajorChannel(aViewId);
        });
    }

    CheckPlayChangable(aViewId: number, aPlayType: number, aStartInfo: ArrayBuffer, aCb: (aChangable: number) => void) {
        this._call( function (iface){
            iface.CheckPlayChangable['finish'] = function(aChangable: number) {
                aCb(aChangable);
            };
            iface.CheckPlayChangable(aViewId, aPlayType, aStartInfo);
        });
    }

    GetViewState(aViewId: number, aCb: (aViewState: number) => void) {
        this._call( function (iface){
            iface.GetViewState['finish'] = function(aViewState: number) {
                aCb(aViewState);
            };
            iface.GetViewState(aViewId);
        });
    }

    GetLockState(aViewId: number, aCb: (aLockState: number) => void) {
        this._call( function (iface){
            iface.GetLockState['finish'] = function(aLockState: number) {
                aCb(aLockState);
            };
            iface.GetLockState(aViewId);
        });
    }

    MemorizeMediaState(aViewId: number, aStopMedia: number, aCb: () => void) {
        this._call( function (iface){
            iface.MemorizeMediaState['finish'] = function() {
                aCb();
            };
            iface.MemorizeMediaState(aViewId, aStopMedia);
        });
    }

    RestoreMediaState(aViewId: number, aCb: () => void) {
        this._call( function (iface){
            iface.RestoreMediaState['finish'] = function() {
                aCb();
            };
            iface.RestoreMediaState(aViewId);
        });
    }

    GetThumbnail(aViewId: number, aPos: number, aCaptureTime: number, aCb: () => void) {
        this._call( function (iface){
            iface.GetThumbnail['finish'] = function() {
                aCb();
            };
            iface.GetThumbnail(aViewId, aPos, aCaptureTime);
        });
    }

    GetURI(aViewId: number, aCb: (aUri: string) => void) {
        this._call( function (iface){
            iface.GetURI['finish'] = function(aUri: string) {
                aCb(aUri);
            };
            iface.GetURI(aViewId);
        });
    }

    Probe(aViewId: number, aUri: string, aCb: () => void) {
        this._call( function (iface){
            iface.Probe['finish'] = function() {
                aCb();
            };
            iface.Probe(aViewId, aUri);
        });
    }

    SetSubtitleSyncTime(aViewId: number, aSubtitleSyncTime: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetSubtitleSyncTime['finish'] = function() {
                aCb();
            };
            iface.SetSubtitleSyncTime(aViewId, aSubtitleSyncTime);
        });
    }

    SetSubtitleFontSize(aViewId: number, aSubtitleFontSize: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetSubtitleFontSize['finish'] = function() {
                aCb();
            };
            iface.SetSubtitleFontSize(aViewId, aSubtitleFontSize);
        });
    }

    SetSubtitleTextPosition(aViewId: number, aSubtitleTextPosition: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetSubtitleTextPosition['finish'] = function() {
                aCb();
            };
            iface.SetSubtitleTextPosition(aViewId, aSubtitleTextPosition);
        });
    }

    GetSubtitleSyncTime(aViewId: number, aCb: (aSubtitleSyncTime: number) => void) {
        this._call( function (iface){
            iface.GetSubtitleSyncTime['finish'] = function(aSubtitleSyncTime: number) {
                aCb(aSubtitleSyncTime);
            };
            iface.GetSubtitleSyncTime(aViewId);
        });
    }

    GetSubtitleFontSize(aViewId: number, aCb: (aSubtitleFontSize: number) => void) {
        this._call( function (iface){
            iface.GetSubtitleFontSize['finish'] = function(aSubtitleFontSize: number) {
                aCb(aSubtitleFontSize);
            };
            iface.GetSubtitleFontSize(aViewId);
        });
    }

    GetSubtitleTextPosition(aViewId: number, aCb: (aSubtitleTextPosition: number) => void) {
        this._call( function (iface){
            iface.GetSubtitleTextPosition['finish'] = function(aSubtitleTextPosition: number) {
                aCb(aSubtitleTextPosition);
            };
            iface.GetSubtitleTextPosition(aViewId);
        });
    }

    GetSubtitleStatus(aViewId: number, aCb: (aStatusInfo: ArrayBuffer) => void) {
        this._call( function (iface){
            iface.GetSubtitleStatus['finish'] = function(aStatusInfo: ArrayBuffer) {
                aCb(aStatusInfo);
            };
            iface.GetSubtitleStatus(aViewId);
        });
    }

    SetBufferingScenario(aViewId: number, aIsAuto: number, aThreshold: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetBufferingScenario['finish'] = function() {
                aCb();
            };
            iface.SetBufferingScenario(aViewId, aIsAuto, aThreshold);
        });
    }

    GetBufferingChunkInfo(aViewId: number, aCb: (aChunkCount: number, aStartOffsets: ArrayBuffer, aEndOffsets: ArrayBuffer) => void) {
        this._call( function (iface){
            iface.GetBufferingChunkInfo['finish'] = function(aChunkCount: number, aStartOffsets: ArrayBuffer, aEndOffsets: ArrayBuffer) {
                aCb(aChunkCount, aStartOffsets, aEndOffsets);
            };
            iface.GetBufferingChunkInfo(aViewId);
        });
    }

    GetBufferingRemainedTime(aViewId: number, aCb: (aBufferRemaindTime: number) => void) {
        this._call( function (iface){
            iface.GetBufferingRemainedTime['finish'] = function(aBufferRemaindTime: number) {
                aCb(aBufferRemaindTime);
            };
            iface.GetBufferingRemainedTime(aViewId);
        });
    }

    GetBufferingPercent(aViewId: number, aCb: (aBufferPercent: number) => void) {
        this._call( function (iface){
            iface.GetBufferingPercent['finish'] = function(aBufferPercent: number) {
                aCb(aBufferPercent);
            };
            iface.GetBufferingPercent(aViewId);
        });
    }

    GetBufferState(aViewId: number, aCb: (aBufferState: number) => void) {
        this._call( function (iface){
            iface.GetBufferState['finish'] = function(aBufferState: number) {
                aCb(aBufferState);
            };
            iface.GetBufferState(aViewId);
        });
    }

    SetDrmLicenseData(aViewId: number, aData: ArrayBuffer, aCb: () => void) {
        this._call( function (iface){
            iface.SetDrmLicenseData['finish'] = function() {
                aCb();
            };
            iface.SetDrmLicenseData(aViewId, aData);
        });
    }

    SetBrowserUserAgentData(aViewId: number, aUAData: ArrayBuffer, aCb: () => void) {
        this._call( function (iface){
            iface.SetBrowserUserAgentData['finish'] = function() {
                aCb();
            };
            iface.SetBrowserUserAgentData(aViewId, aUAData);
        });
    }

    GetResourceMap(aCb: (aResourceMap: ArrayBuffer) => void) {
        this._call( function (iface){
            iface.GetResourceMap['finish'] = function(aResourceMap: ArrayBuffer) {
                aCb(aResourceMap);
            };
            iface.GetResourceMap();
        });
    }

    GetPumpIndex(aViewId: number, aCb: (aPumpId: number) => void) {
        this._call( function (iface){
            iface.GetPumpIndex['finish'] = function(aPumpId: number) {
                aCb(aPumpId);
            };
            iface.GetPumpIndex(aViewId);
        });
    }

    SetDuration(aViewId: number, aDuration: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetDuration['finish'] = function() {
                aCb();
            };
            iface.SetDuration(aViewId, aDuration);
        });
    }

    SetPlayReadyInfo(aViewId: number, aLaURL: string, aCustomData: ArrayBuffer, aControlFlag: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetPlayReadyInfo['finish'] = function() {
                aCb();
            };
            iface.SetPlayReadyInfo(aViewId, aLaURL, aCustomData, aControlFlag);
        });
    }

    GetVmxIdentifier(aCb: (aIdentifier: ArrayBuffer) => void) {
        this._call( function (iface){
            iface.GetVmxIdentifier['finish'] = function(aIdentifier: ArrayBuffer) {
                aCb(aIdentifier);
            };
            iface.GetVmxIdentifier();
        });
    }

    GetVmxStatus(aViewId: number, aCb: (aStatus: number) => void) {
        this._call( function (iface){
            iface.GetVmxStatus['finish'] = function(aStatus: number) {
                aCb(aStatus);
            };
            iface.GetVmxStatus();
        });
    }

    GetVmxError(aViewId: number, aCb: (aErrorCode: number) => void) {
        this._call( function (iface){
            iface.GetVmxError['finish'] = function(aErrorCode: number) {
                aCb(aErrorCode);
            };
            iface.GetVmxError();
        });
    }

    SetVmxCas(aViewId: number, aVCASUrl: string, aVendorName: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetVmxCas['finish'] = function() {
                aCb();
            };
            iface.SetVmxCas(aViewId, aVCASUrl, aVendorName);
        });
    }

    UnsetVmxCas(aViewId: number, aCb: () => void) {
        this._call( function (iface){
            iface.UnsetVmxCas['finish'] = function() {
                aCb();
            };
            iface.UnsetVmxCas(aViewId);
        });
    }
}

export = CMediaPlay