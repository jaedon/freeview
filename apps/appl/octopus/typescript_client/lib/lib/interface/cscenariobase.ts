/**
 * Created by hsseo on 14. 5. 12.
 */


import tdata = require('./tdata');

import dbusConn = require('../util/dbus_connect');

class CScenarioBase extends dbusConn.CDBusInterface {
    constructor() {
        super('Octopus.Appkit.Scenario.Base', '/Octopus/Appkit/Scenario/Base');
    }

    GetMainLanguage(aCb: (aLanguage: string) => void) {
        this._call( function (iface){
            iface.GetMainLanguage['finish'] = function(aLanguage: string) {
                aCb(aLanguage);
            };
            iface.GetMainLanguage();
        });
    }

    SetMainLanguage(aLanguage: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetMainLanguage['finish'] = function() {
                aCb();
            };
            iface.SetMainLanguage(aLanguage);
        });
    }

    GetAudioLanguage(aIndex: number, aCb: (aLanguage: string) => void) {
        this._call( function (iface){
            iface.GetAudioLanguage['finish'] = function(aLanguage: string) {
                aCb(aLanguage);
            };
            iface.GetAudioLanguage(aIndex);
        });
    }

    SetAudioLanguage(aIndex: number, aLanguage: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetAudioLanguage['finish'] = function() {
                aCb();
            };
            iface.SetAudioLanguage(aIndex, aLanguage);
        });
    }

    GetSubtitleLanguage(aIndex: number, aCb: (aLanguage: string) => void) {
        this._call( function (iface){
            iface.GetSubtitleLanguage['finish'] = function(aLanguage: string) {
                aCb(aLanguage);
            };
            iface.GetSubtitleLanguage(aIndex);
        });
    }

    SetSubtitleLanguage(aIndex: number, aLanguage: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetSubtitleLanguage['finish'] = function() {
                aCb();
            };
            iface.SetSubtitleLanguage(aIndex, aLanguage);
        });
    }

    GetHardOfHearing(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetHardOfHearing['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetHardOfHearing();
        });
    }

    SetHardOfHearing(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetHardOfHearing['finish'] = function() {
                aCb();
            };
            iface.SetHardOfHearing(aEnable);
        });
    }

    GetMaturityRating(aCb: (aRating: number) => void) {
        this._call( function (iface){
            iface.GetMaturityRating['finish'] = function(aRating: number) {
                aCb(aRating);
            };
            iface.GetMaturityRating();
        });
    }

    SetMaturityRating(aRating: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetMaturityRating['finish'] = function() {
                aCb();
            };
            iface.SetMaturityRating(aRating);
        });
    }

    GetCountryCode(aCb: (aCountryCode: number) => void) {
        this._call( function (iface){
            iface.GetCountryCode['finish'] = function(aCountryCode: number) {
                aCb(aCountryCode);
            };
            iface.GetCountryCode();
        });
    }

    SetCountryCode(aCountryCode: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetCountryCode['finish'] = function() {
                aCb();
            };
            iface.SetCountryCode(aCountryCode);
        });
    }

    GetOperatorType(aCb: (aOpType: number) => void) {
        this._call( function (iface){
            iface.GetOperatorType['finish'] = function(aOpType: number) {
                aCb(aOpType);
            };
            iface.GetOperatorType();
        });
    }

    SetOperatorType(aOpType: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetOperatorType['finish'] = function() {
                aCb();
            };
            iface.SetOperatorType(aOpType);
        });
    }

    GetAntennaConnection(aCb: (aConnectionType: number) => void) {
        this._call( function (iface){
            iface.GetAntennaConnection['finish'] = function(aConnectionType: number) {
                aCb(aConnectionType);
            };
            iface.GetAntennaConnection();
        });
    }

    SetAntennaConnection(aConnectionType: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAntennaConnection['finish'] = function() {
                aCb();
            };
            iface.SetAntennaConnection(aConnectionType);
        });
    }

    GetStartPadding(aCb: (aPadding: number) => void) {
        this._call( function (iface){
            iface.GetStartPadding['finish'] = function(aPadding: number) {
                aCb(aPadding);
            };
            iface.GetStartPadding();
        });
    }

    SetStartPadding(aPadding: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetStartPadding['finish'] = function() {
                aCb();
            };
            iface.SetStartPadding(aPadding);
        });
    }

    GetEndPadding(aCb: (aPadding: number) => void) {
        this._call( function (iface){
            iface.GetEndPadding['finish'] = function(aPadding: number) {
                aCb(aPadding);
            };
            iface.GetEndPadding();
        });
    }

    SetEndPadding(aPadding: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetEndPadding['finish'] = function() {
                aCb();
            };
            iface.SetEndPadding(aPadding);
        });
    }

    GetAutoDelete(aCb: (aAutoDelete: number) => void) {
        this._call( function (iface){
            iface.GetAutoDelete['finish'] = function(aAutoDelete: number) {
                aCb(aAutoDelete);
            };
            iface.GetAutoDelete();
        });
    }

    SetAutoDelete(aAutoDelete: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAutoDelete['finish'] = function() {
                aCb();
            };
            iface.SetAutoDelete(aAutoDelete);
        });
    }

    GetPostCode(aCb: (aPostcode: string) => void) {
        this._call( function (iface){
            iface.GetPostCode['finish'] = function(aPostcode: string) {
                aCb(aPostcode);
            };
            iface.GetPostCode();
        });
    }

    SetPostCode(aPostcode: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetPostCode['finish'] = function() {
                aCb();
            };
            iface.SetPostCode(aPostcode);
        });
    }

    GetGMTOffset(aCb: (aGmtOffset: number) => void) {
        this._call( function (iface){
            iface.GetGMTOffset['finish'] = function(aGmtOffset: number) {
                aCb(aGmtOffset);
            };
            iface.GetGMTOffset();
        });
    }

    SetGMTOffset(aGmtOffset: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetGMTOffset['finish'] = function() {
                aCb();
            };
            iface.SetGMTOffset(aGmtOffset);
        });
    }

    GetChannelSetupPIN(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetChannelSetupPIN['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetChannelSetupPIN();
        });
    }

    SetChannelSetupPIN(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetChannelSetupPIN['finish'] = function() {
                aCb();
            };
            iface.SetChannelSetupPIN(aEnable);
        });
    }

    GetGuidancePolicy(aCb: (aGuidancePolicy: number) => void) {
        this._call( function (iface){
            iface.GetGuidancePolicy['finish'] = function(aGuidancePolicy: number) {
                aCb(aGuidancePolicy);
            };
            iface.GetGuidancePolicy();
        });
    }

    SetGuidancePolicy(aGuidancePolicy: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetGuidancePolicy['finish'] = function() {
                aCb();
            };
            iface.SetGuidancePolicy(aGuidancePolicy);
        });
    }

    GetSubtitleSetting(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetSubtitleSetting['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetSubtitleSetting();
        });
    }

    SetSubtitleSetting(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetSubtitleSetting['finish'] = function() {
                aCb();
            };
            iface.SetSubtitleSetting(aEnable);
        });
    }

    GetDayLightSaving(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetDayLightSaving['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetDayLightSaving();
        });
    }

    SetDayLightSaving(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetDayLightSaving['finish'] = function() {
                aCb();
            };
            iface.SetDayLightSaving(aEnable);
        });
    }

    GetTimeUpdate(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetTimeUpdate['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetTimeUpdate();
        });
    }

    SetTimeUpdate(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetTimeUpdate['finish'] = function() {
                aCb();
            };
            iface.SetTimeUpdate(aEnable);
        });
    }

    GetBouquetName(aCb: (aBouquet: string) => void) {
        this._call( function (iface){
            iface.GetBouquetName['finish'] = function(aBouquet: string) {
                aCb(aBouquet);
            };
            iface.GetBouquetName();
        });
    }

    SetBouquetName(aBouquet: string, aCb: () => void) {
        this._call( function (iface){
            iface.SetBouquetName['finish'] = function() {
                aCb();
            };
            iface.SetBouquetName(aBouquet);
        });
    }

    GetBouquetID(aCb: (aBouquetId: number) => void) {
        this._call( function (iface){
            iface.GetBouquetID['finish'] = function(aBouquetId: number) {
                aCb(aBouquetId);
            };
            iface.GetBouquetID();
        });
    }

    SetBouquetID(aBouquetId: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetBouquetID['finish'] = function() {
                aCb();
            };
            iface.SetBouquetID(aBouquetId);
        });
    }

    GetSIMode(aCb: (aSIMode: number) => void) {
        this._call( function (iface){
            iface.GetSIMode['finish'] = function(aSIMode: number) {
                aCb(aSIMode);
            };
            iface.GetSIMode();
        });
    }

    SetSIMode(aSIMode: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetSIMode['finish'] = function() {
                aCb();
            };
            iface.SetSIMode(aSIMode);
        });
    }

    GetRegionCode(aCb: (aRegionCode: number) => void) {
        this._call( function (iface){
            iface.GetRegionCode['finish'] = function(aRegionCode: number) {
                aCb(aRegionCode);
            };
            iface.GetRegionCode();
        });
    }

    SetRegionCode(aRegionCode: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetRegionCode['finish'] = function() {
                aCb();
            };
            iface.SetRegionCode(aRegionCode);
        });
    }

    GetAdultChannels(aCb: (aAdultChannel: number) => void) {
        this._call( function (iface){
            iface.GetAdultChannels['finish'] = function(aAdultChannel: number) {
                aCb(aAdultChannel);
            };
            iface.GetAdultChannels();
        });
    }

    SetAdultChannels(aAdultChannel: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetAdultChannels['finish'] = function() {
                aCb();
            };
            iface.SetAdultChannels(aAdultChannel);
        });
    }

    GetSWUpdateVersion(aCb: (aSwUpdateVersion: string) => void) {
        this._call( function (iface){
            iface.GetSWUpdateVersion['finish'] = function(aSwUpdateVersion: string) {
                aCb(aSwUpdateVersion);
            };
            iface.GetSWUpdateVersion();
        });
    }

    SetSatRecReceiverId(aReceiverId: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetSatRecReceiverId['finish'] = function() {
                aCb();
            };
            iface.SetSatRecReceiverId(aReceiverId);
        });
    }

    GetTVTVEnable(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetTVTVEnable['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetTVTVEnable();
        });
    }

    SetTVTVEnable(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetTVTVEnable['finish'] = function() {
                aCb();
            };
            iface.SetTVTVEnable(aEnable);
        });
    }

    GetTVTVEPGDownload_StandbyEnable(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetTVTVEPGDownload_StandbyEnable['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetTVTVEPGDownload_StandbyEnable();
        });
    }

    SetTVTVEPGDownload_StandbyEnable(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetTVTVEPGDownload_StandbyEnable['finish'] = function() {
                aCb();
            };
            iface.SetTVTVEPGDownload_StandbyEnable(aEnable);
        });
    }

    GetDescrambleOnOff(aCb: (aEnable: number) => void) {
        this._call( function (iface){
            iface.GetDescrambleOnOff['finish'] = function(aEnable: number) {
                aCb(aEnable);
            };
            iface.GetDescrambleOnOff();
        });
    }

    SetDescrambleOnOff(aEnable: number, aCb: () => void) {
        this._call( function (iface){
            iface.SetDescrambleOnOff['finish'] = function() {
                aCb();
            };
            iface.SetDescrambleOnOff(aEnable);
        });
    }
}

export = CScenarioBase