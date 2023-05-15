/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvTVPortal.js
/*jslint sloppy:true, nomen:true, vars:true */
//    TV_PORTAL: ['layer_domain/service/dl_dsrvTVPortal.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

enum eKERROR {
    KErrNone = 0, //
    KErrUnKnown = 1, //
    KErrAborted = 2, //
    KErrParseError = 3, //
    KErrTimedOut = 4, //
    KErrNotAuthorized = 10, //
    KErrNotAvailable = 11, //
    KErrServerFail = 12, //
    KErrPrivacyPolicyUpdated = 11,
    KErrPinError = 100, //
};

class cDLTV_PORTAL extends __KERNEL__ implements ifSVCTVPortal {
    private mLocalSystemOipfObj: any = this.getPluginInstance('LocalSystem');
    private mConfigurationOipfObj: any = this.getPluginInstance('Configuration');
    p : any = {};
    //mPortalUrl : string = 'http://test.humaxtvportal.com';
    mPortalUrl : string = hx.svc.SETTING_UTIL.getTVPortalUrl();
    mLanguage : string = 'en';
    mPrivacyInfo : any = hx.svc.WEB_STORAGE.getPrivacyPolicy();
    mTermsInfo : any = hx.svc.WEB_STORAGE.getTermsAndConditions();
    mSystemInfo : any = hx.svc.SETTING_UTIL.getSystemInfo();
    mSoftwareVersion : any = this.mSystemInfo.softwareVersion.substr(7, 13);
    mRespLogIn : any = null;
    mAuthTicket : any = null;
    mPrivacyAccepted : boolean = !!(this.mPrivacyInfo.state === "true");
    mTermsAccepted : boolean = !!(this.mTermsInfo.state === "true");
    mNewPrivacyVersion : string = this.mPrivacyInfo.version || '';
    mNewTermsVersion : string = this.mTermsInfo.version || '';
    mDeviceInfo : any = null;
    mProfiles : any = null;
    mActiveProfile : any = null;
    mRegistrationCode : any = null;
    mInitialized : boolean = false;
    mLogged : boolean = false;
    mLogging : boolean = false;
    mCheckCrashLog : boolean = false;
    mAppList : any = {
        featured : null, //
        available : null, //
        ott : null, //
        def : null, //
        installed : null, //
        pre : null
    }; //
    mRegionInfo : any = {
        ip : '',
        country : '',
        countryname : '',
        city : ''
    };
    mSmartSearchAppList : any = null;
    mDsrvAppMgr : any = null;
    mActiveAppInfo : any = null;
    mLastErrorCode : number = eKERROR.KErrNone;
    mDisableHumaxServer : boolean = false;
    mLoginQueue = [];

    /*
     * HUMAX TV Portal open API information
     */
    fnGetCommonLanParam = () => {
        return {
            'Lang' : this.mLanguage
        };
    };

    fnGetCommonAuthParam = () => {
        return {
            'authticket' : this.mAuthTicket
        };
    };

    fnGetCommonAvailAppParam = (aType) => {
        return {
            'authticket' : this.mAuthTicket,
            'appsType' : aType
        };
    };

    mPrimitivesInfo : any = {
        'getPrivacyVersion' : {
            needSystemInfo : true,
            api : 'GetLegalDocVer',
            getParam : (aParam?) => {
                var ret = {
                    'SystemID' : this.mSystemInfo.system_id
                };
                if (aParam) {
                    for (var i in aParam) {
                        if (aParam.hasOwnProperty(i)) {
                            ret[i] = aParam[i];
                        }
                    }
                }
                return ret;
            },
            handleData : (data)=> {
                if (data) {
                    var list = data.list;
                    var newVersion = '';
                    for (var i = 0, len = list.length; i < len; i += 1) {
                        if (list[i].docType === 'privacy') {
                            newVersion = list[i].key + '_' + list[i].seq;
                        }
                    }
                }
                if (newVersion !== this.mNewPrivacyVersion) {
                    this.mNewPrivacyVersion = newVersion;
                }
            }
        },
        'getTermsVersion' : {
            needSystemInfo : true,
            api : 'GetLegalDocVer',
            getParam : (aParam?) => {
                var ret = {
                    'SystemID' : this.mSystemInfo.system_id
                };
                if (aParam) {
                    for (var i in aParam) {
                        if (aParam.hasOwnProperty(i)) {
                            ret[i] = aParam[i];
                        }
                    }
                }
                return ret;
            },
            handleData : (data)=> {
                if (data) {
                    var list = data.list;
                    var newVersion = '';
                    for (var i = 0, len = list.length; i < len; i += 1) {
                        if (list[i].docType === 'terms') {
                            newVersion = list[i].key + '_' + list[i].seq;
                        }
                    }
                }
                if (newVersion !== this.mNewTermsVersion) {
                    this.mNewTermsVersion = newVersion;
                }
            }
        },
        'getPrivacy' : {
            needSystemInfo : true,
            api : 'GetLegalDoc',
            getParam : (aParam?) => {
                var ret = {
                    'doctype' : 'privacy',
                    'SystemID' : this.mSystemInfo.system_id,
                    'Lang' : this.mLanguage
                };
                if (aParam) {
                    for (var i in aParam) {
                        if (aParam.hasOwnProperty(i)) {
                            ret[i] = aParam[i];
                        }
                    }
                }
                return ret;
            },
            handleData : (data)=> {
                this.mPrivacyInfo.text = data.text;
                hx.svc.WEB_STORAGE.setPrivacyPolicyText(data.text);
            }
        },
        'getTermsAndConditions' : {
            needSystemInfo : true,
            api : 'GetLegalDoc',
            getParam : (aParam?) => {
                var ret = {
                    'doctype' : 'terms',
                    'SystemID' : this.mSystemInfo.system_id,
                    'Lang' : this.mLanguage
                };
                if (aParam) {
                    for (var i in aParam) {
                        if (aParam.hasOwnProperty(i)) {
                            ret[i] = aParam[i];
                        }
                    }
                }
                return ret;
            },
            handleData : (data)=> {
                this.mTermsInfo.text = data.text;
                hx.svc.WEB_STORAGE.setTermsAndConditionsText(data.text);
            }
        },
        'acceptPrivacyPolicy' : {
            needAuthTicket : true,
            needUserParam : ['agree'],
            api : 'AcceptPrivacy',
            getParam : (userParam)=> {
                return {
                    'authticket' : this.mAuthTicket,
                    'agree' : userParam.agree ? 'Y' : 'N'
                };
            },
            handleData : (data, userParam)=> {
                if (data.status) {
                    if (this.mRespLogIn) {
                        this.mRespLogIn.AcceptPrivacy = userParam.agree;
                    }
                    this.mPrivacyAccepted = userParam.agree;
                }
            }
        },
        'acceptTermsAndConditions' : {
            needAuthTicket : true,
            needUserParam : ['agree'],
            api : 'AcceptTerms',
            getParam : (userParam)=> {
                return {
                    'authticket' : this.mAuthTicket,
                    'agree' : userParam.agree ? 'Y' : 'N'
                };
            },
            handleData : (data, userParam)=> {
                if (data.status) {
                    if (this.mRespLogIn) {
                        this.mRespLogIn.AcceptTerms = userParam.agree;
                    }
                    this.mTermsAccepted = userParam.agree;
                    //this._updateTermsAndConditions(userParam.agree);
                }
            }
        },
        'acceptPrivacyTerms' : {
            needAuthTicket : true,
            needUserParam : ['privacy', 'terms'],
            api : 'AcceptPrivacyTerms',
            getParam : (userParam)=> {
                return {
                    'authticket' : this.mAuthTicket,
                    'privacy' : userParam.privacy ? 'Y' : 'N',
                    'terms' : userParam.terms ? 'Y' : 'N',
                };
            },
            handleData : (data, userParam)=> {
                if (data.status) {
                    if (this.mRespLogIn) {
                        this.mRespLogIn.AcceptPrivacy = userParam.privacy;
                        this.mRespLogIn.AcceptTerms = userParam.terms;
                    }
                    this.mPrivacyAccepted = userParam.privacy;
                    this.mTermsAccepted = userParam.terms;
                    //this._updateTermsAndConditions(userParam.terms);
                }
            }
        },
        'deviceReset' : {
            needSystemInfo : true,
            api : 'DeviceReset',
            getParam : (aParam?)=> {
                var now : any = new Date();
                var timezoneOffset : any = now.getTimezoneOffset();
                if (timezoneOffset > 0) {
                    timezoneOffset = '-' + timezoneOffset;
                } else {
                    timezoneOffset = timezoneOffset * -1;
                }
                var ret = {
                    'deviceID' : this.mSystemInfo.wiredMacAddress,
                    'MACAddress' : this.mSystemInfo.wiredMacAddress,
                    'SystemID' : this.mSystemInfo.system_id,
                    'SWVer' : this.mSoftwareVersion,
                    'serialNumber' : this.mSystemInfo.serialNumber,
                    'timeZone' : '' + timezoneOffset
                };
                if (aParam) {
                    for (var i in aParam) {
                        if (aParam.hasOwnProperty(i)) {
                            ret[i] = aParam[i];
                        }
                    }
                }
                return ret;
            },
            handleData : (data)=> {
                if (data) {
                    this.mRespLogIn = data;
                    this.mAuthTicket = data.AuthTicket;
                    //this.mPrivacyAccepted = data.AcceptPrivacy;
                    //this.mTermsAccepted = data.AcceptTerms;
                    //hx.svc.WEB_STORAGE.setPrivacyPolicyState(this.mPrivacyAccepted);
                    //hx.svc.WEB_STORAGE.setTermsAndConditionsState(this.mTermsAccepted);
                }
                //this._updateTermsAndConditions(data.AcceptTerms);
            }
        },
        'logIn' : {
            needSystemInfo : true,
            api : 'Login',
            getParam : (aParam?)=> {
                var now : any = new Date();
                var timezoneOffset : any = now.getTimezoneOffset();
                if (timezoneOffset > 0) {
                    timezoneOffset = '-' + timezoneOffset;
                } else {
                    timezoneOffset = timezoneOffset * -1;
                }
                var ret = {
                    'deviceID' : this.mSystemInfo.wiredMacAddress,
                    'MACAddress' : this.mSystemInfo.wiredMacAddress,
                    'SystemID' : this.mSystemInfo.system_id,
                    'SWVer' : this.mSoftwareVersion,
                    'serialNumber' : this.mSystemInfo.serialNumber,
                    'timeZone' : '' + timezoneOffset
                };
                if (aParam) {
                    for (var i in aParam) {
                        if (aParam.hasOwnProperty(i)) {
                            ret[i] = aParam[i];
                        }
                    }
                }
                return ret;
            },
            handleData : (data)=> {
                if (data) {
                    this.mRespLogIn = data;
                    this.mAuthTicket = data.AuthTicket;
                    this.mPrivacyAccepted = data.AcceptPrivacy;
                    this.mTermsAccepted = data.AcceptTerms;
                    hx.svc.WEB_STORAGE.setPrivacyPolicyState(this.mPrivacyAccepted);
                    hx.svc.WEB_STORAGE.setTermsAndConditionsState(this.mTermsAccepted);
                }
                //this._updateTermsAndConditions(data.AcceptTerms);
            }
        },
        'logOut' : {
            api : 'Logout',
            getParam : () => {
                return {};
            },
            handleData : (data) => {
                if (data && data.status) {
                    this.mRespLogIn = null;
                    this.mAuthTicket = null;
                }
            }
        },
        // TODO: ReLogin
        'getProfileList' : {
            needAuthTicket : true,
            api : 'GetProfileList',
            getParam : ()=> {
                return {
                    'authticket' : this.mAuthTicket
                };
            },
            handleData : (data)=> {
                var i : number;
                for ( i = 0; i < data.length; i += 1) {
                    var profileInfo : any = data[i];
                    if (profileInfo.UID === this.mDeviceInfo.DefaultUID) {
                        profileInfo.isDefault = true;
                    }
                }
                this.mProfiles = data;
            }
        },
        'setActiveProfile' : {
            needAuthTicket : true,
            needUserParam : ['deviceId, userId, pinCode'],
            api : 'LoginProfile',
            getParam : function(userParam) {
                return {
                    'did' : userParam.deviceId,
                    'uid' : userParam.userId,
                    'pincode' : userParam.pinCode
                };
            },
            handleData : (data)=> {
                this.mAuthTicket = data.AuthTicket;
            }
        },
        'getActiveProfile' : {
            needAuthTicket : true,
            api : 'GetProfileInfo',
            getParam : this.fnGetCommonAuthParam,
            handleData : (data)=> {
                this.mActiveProfile = data;
                this.mPrivacyAccepted = data.AcceptPrivacy;
            }
        },
        'getDeviceInfo' : {
            needAuthTicket : true,
            api : 'GetDeviceInfo',
            getParam : this.fnGetCommonAuthParam,
            handleData : (data)=> {
                this.mDeviceInfo = data;
            }
        },
        // TODO: GetCurrentRegionInfo
        'getRegistrationCode' : {
            needAuthTicket : true,
            api : 'GetRegCode',
            getParam : this.fnGetCommonAuthParam,
            handleData : (data)=> {
                this.mRegistrationCode = data;
            }
        },
        // TODO: UpdateUsingCountry
        'getApplicationUrl' : {
            needAuthTicket : true,
            needUserParam : ['applicationId'],
            api : 'RunApp',
            getParam : (userParam)=> {
                return {
                    'id' : userParam.applicationId,
                    'authticket' : this.mAuthTicket
                };
            }
        },
        'deleteApplication' : {
            needAuthTicket : true,
            needUserParam : ['applicationId'],
            api : 'DeleteDeviceApp',
            getParam : (userParam)=> {
                return {
                    'id' : userParam.applicationId,
                    'authticket' : this.mAuthTicket
                };
            }
        },
        'updateApplicationList' : {
            needAuthTicket : true,
            needUserParam : ['applicationList'],
            api : 'UploadDeviceApp',
            getParam : (aUserParam)=> {
                return {
                    'authticket' : this.mAuthTicket
                };
            },
            getPostMessage : function(aUserParam) {
                var i : number, list : any = [], appInfo : any;
                hx.log('dsrvTVPortal', 'updateApplicationList len=' + aUserParam.applicationList.length);
                for ( i = 0; i < aUserParam.applicationList.length; i += 1) {
                    appInfo = aUserParam.applicationList[i];
                    if (appInfo.appid) {
                        hx.log('dsrvTVPortal', 'updateApplicationList "' + appInfo.name + '"');
                        list.push({
                            'AppID' : appInfo.appid,
                            'PackageName' : appInfo.name,
                            'Version' : (appInfo.widgetDescriptor) ? appInfo.widgetDescriptor.version : appInfo.appInfo,
                            'HitCount' : 0,
                            'LastHitDate' : '',
                            'CreateDate' : '',
                            'ModDate' : '',
                            'InstallUID' : ''
                        });
                    }
                }
                appInfo = null;
                aUserParam.applicationList = [];
                return JSON.stringify(list);
            }
        },
        'getFeaturedAppList' : {
            needAuthTicket : true,
            api : 'getModelAppsList',
            getParam : ()=> {
                return this.fnGetCommonAvailAppParam('FEATUREDAPP');
            },
            handleData : (data)=> {
                this.mAppList.featured = data;
            }
        },
        'getAvailableAppList' : {
            needAuthTicket : true,
            api : 'GetModelAppsList',
            getParam : ()=> {
                return this.fnGetCommonAvailAppParam('MODELONLYAPP');
            },
            handleData : (data)=> {
                this.mAppList.available = data;
            }
        },
        'getOttAppList' : {
            needAuthTicket : true,
            api : 'GetModelAppsList',
            getParam : ()=> {
                return this.fnGetCommonAvailAppParam('OTTLANUNCHER');
            },
            handleData : (data)=> {
                this.mAppList.ott = data;
            }
        },
        'getDefaultAppList' : {
            needAuthTicket : true,
            api : 'GetModelAppsList',
            getParam : ()=> {
                return this.fnGetCommonAvailAppParam('DEFAULTAPP');
            },
            handleData : (data)=> {
                this.mAppList.def = data;
            }
        },
        'getPreInstalledAppList' : {
            needAuthTicket : true,
            api : 'GetModelAppsList',
            getParam : ()=> {
                return this.fnGetCommonAvailAppParam('PREINSTALL');
            },
            handleData : (data)=> {
                this.mAppList.pre = data;
            }
        },
        'getInstalledAppList' : {
            needAuthTicket : true,
            api : 'GetMyAppsList', // get locally installed application list
            getParam : this.fnGetCommonAuthParam,
            handleData : (data)=> {
                var i : number, len : number;
                for ( i = 0, len = data.length; i < len; i += 1) {
                    hx.log('dsrvTVPortal', 'getInstalledAppList: [' + data[i].name + ']');
                }
                this.mAppList.installed = data;
            }
        },
        'getSmartSearchPlayers' : {
            needAuthTicket : true,
            api : 'GetModelAppsList', // get locally installed application list
            getParam : ()=> {
                return this.fnGetCommonAvailAppParam('STBLAUNCHER');
            },
            handleData : (data)=> {
                var i : number, len : number;
                for ( i = 0, len = data.length; i < len; i += 1) {
                    hx.log('dsrvTVPortal', 'getModelAppList: [' + data[i].name + ']');
                    //this.fnGetApplicationUrl(data[i].appid, function(result) {
                        // merong
                    //});
                }
            }
        },
        'getSmartSearchAppList' : {
            needAuthTicket : true,
            api : 'GetModelAppsList',
            getParam : this.fnGetCommonAuthParam,
            handleData : (data)=> {
                this.mSmartSearchAppList = data;
            }
        },
        'setStatisticsLog' : {
            needAuthTicket : true,
            api : 'UploadStatistics',
            getParam : this.fnGetCommonAuthParam,
            getPostMessage : (userParam)=> {
                return JSON.stringify(userParam.statisticsLog);
            }
        },
        'findProgrammes' : {
            needAuthTicket : true,
            getParam : function(userParam) {
                var url : string = '';
                var authticket : string = '';
                var text : string = '?authticket=';
                var index : number = userParam.url.indexOf(text);
                if (index > -1) {
                    url = userParam.url.substring(0, index);
                    authticket = userParam.url.substring(index + text.length, userParam.url.length);
                }
                return {
                    'url' : url,
                    'authticket' : authticket,
                    'searchText' : userParam.searchText,
                    'offset' : userParam.offset,
                    'limit' : userParam.limit
                };
            }
        },
        'putCrashLog' : {
            api : 'UploadCrashLog',
            getParam : ()=> {
                var now : any = new Date();
                var time : number = Math.floor(now.getTime() / 1000);
                return {
                    'crashDate' : time,
                    'deviceID' : this.mSystemInfo.wiredMacAddress,
                    'SystemID' : this.mSystemInfo.system_id,
                    'SWVer' : this.mSoftwareVersion,
                    'country' : this.mConfigurationOipfObj.countryId
                };
            },
            putData : ()=> {
                return this.mLocalSystemOipfObj.hcrdata;
            }
        },
        'putExceptionLog' : {
            api : 'UploadCrashLog',
            getParam : ()=> {
                return {
                    'crashDate' : Math.floor(Date.now() / 1000),
                    'deviceID' : this.mSystemInfo.wiredMacAddress,
                    'SystemID' : this.mSystemInfo.system_id,
                    'SWVer' : this.mSoftwareVersion,
                    'country' : this.mConfigurationOipfObj.countryId
                };
            },
            putData : (userData)=> {
                return userData;
            }
        },
        'checkNewSoftware' : {
            getParam : ()=> {
                var systemId : string = this.mSystemInfo.system_id;
                var ipOtaUrl : string = hx.svc.SETTING_UTIL.getIpOtaUrl();
                return {
                    'dataType': 'xml',
                    'url': ipOtaUrl + '/SWUpdate/CheckNewSW',
                    'SystemID': systemId,
                    'SWVersion': this.mSoftwareVersion,
                    'bForced': true
                };
            }
        },
        'getCurrentRegionInfo' : {
            api : 'GetCurrentRegionInfo',
            getParam : function() {
                return {};
            },
            handleData : (data) => {
                this.mRegionInfo.ip = data.ip;
                this.mRegionInfo.country = data.country;
                this.mRegionInfo.countryname = data.countryname;
                this.mRegionInfo.city = data.city;
            }
        },
        'getCheckUploadStatistics' : {
            api : 'CheckUploadStatistics',
            getParam : (userParam) => {
                return {
                    'authticket' : this.mAuthTicket
                };
            },
            handleData : function(data) {
            }
        },
        'getRecommendations' : {
            needAuthTicket : true,
            api : 'GetIPEPGRecommend', // get recommendations to record
            getParam : (userParam)=> {
                return {
                    'deviceID' : this.mSystemInfo.wiredMacAddress,
                    'type' : userParam.type,
                    'repeat' : userParam.repeat
                };
            },
            handleData : (data)=> {
                //var i : number, len : number;
            }
        },
    };

    constructor(aServiceName: string) {
        super(aServiceName);
        hx.logadd('dsrvTVPortal');
        this.defineEvents(['privacyPolicyChange', 'TermsAndConditionsUpdated']);
        this.require('APP_MGR', (dsrvAppMgr)=> {
            this.mDsrvAppMgr = dsrvAppMgr;
        });
        this.require('SETTING_UTIL', (dsrvSettingUtil)=> {
            dsrvSettingUtil.addEventCb('LanguageChanged', this.p, (languageInfo)=> {
                hx.log('dsrvTVPortal', 'onLanguageChanged"' + languageInfo.LangMenu + '"');
                this.fnSetLanguage(languageInfo.LangMenu);
            });
            dsrvSettingUtil.addEventCb('NetworkAvailableChanged', this.p, (networkAvailable)=> {
                if (!networkAvailable) {
                    this.mLogged = false;
                    this.mRespLogIn = null;
                    this.mInitialized = false;
                } else {
                    this.fnExecutePrimitive('getCurrentRegionInfo');
                    hx.log('dsrvTVPortal', 'mRegionInfo.ip: ' + this.mRegionInfo.ip);
                }
            });
        });

        this.addEventCb('privacyPolicyChange', this, (policyValue) => {
            hx.log('dsrvTVPortal', 'privacyPolicyChange policyValue : ' + policyValue);
            if (!policyValue) {
                hx.svc.SETTING_UTIL.setEnableRemoteRec(false);
                hx.log('dsrvTVPortal', 'privacyPolicyChange : set enableRemoteRecord to false');
            }
        });

        window.addEventListener('error', (exception) => {
            if (this.isPrivacyPolicyAccepted()) {
                var ret = {
                    type: "",
                    message: "",
                    stack: "",
                    filename: "",
                    lineno: -1,
                    colno: -1,
                    typestamp: -1
                };
                if (exception instanceof ErrorEvent) {
                    for (var key in ret) {
                        ret[key] = exception[key];
                    }
                } else if(exception instanceof Error) {
                    ret.message = (<any>exception).message;
                    ret.stack = (<any>exception).stack;
                } else {
                    ret.message = <any>exception;
                }
                hx.log('warn', 'exception!!');
                hx.log('warn', 'exception!!');
                hx.log('warn', 'exception!!');
                hx.log('warn', 'exception!!');
                this.putExceptionLogData(JSON.stringify(ret));
            }
        });
    }


    /**********************************************************
     * Utilities
     **********************************************************/

    fnRequest (aType, aApiName, aParam, aFn, aFnError, aRetryCount? : number) {
        var that = this;
        if (!hx.svc.SETTING_UTIL.getInternetAvailable()) {
            hx.log('dsrvTVPortal', 'cannot make a request cos network(internet) is not available');
            if (aFnError) {
                aFnError(eKERROR.KErrUnKnown);
            }
            return;
        }
        if (this.mDisableHumaxServer) {
            hx.log('dsrvTVPortal', 'Humax Portal is disabled for TEST');
            if (aFnError) {
                aFnError(eKERROR.KErrUnKnown);
            }
            return;
        }
        if (aParam && aParam.data && aParam.data.bForced) {
            hx.log('debug', '[fnRequest] bypass!! aParam.data.bForced : ' + aApiName);
            delete aParam.data.bForced;
        } else if (!that.isPrivacyPolicyAccepted()) {
            hx.log('debug', '[fnRequest] FAIL!! cos !that.isPrivacyPolicyAccepted()');
            if (aFnError) {
                aFnError(eKERROR.KErrUnKnown);
            }
            return;
        }
        aRetryCount = aRetryCount || 1;
        var urlString : string = (!!aParam.data.url) ? aParam.data.url : this.mPortalUrl + '/api/' + aApiName;
        hx.log('dsrvTVPortal', 'init: serialNumber: ' + this.mSystemInfo.serialNumber + ' urlString ' + urlString);
        var dataTypeString : string = aParam.data.dataType ? aParam.data.dataType : 'json';
        var data : any = {}, i;

        if ((aType === 'post' && aParam.urlParam) || (aType === 'put' && aParam.urlParam)) {
            urlString += '?' + $.param(aParam.urlParam);
        }

        if (aType === 'post') {
            data = aParam.data;
        } else if (aType === 'put') {
            dataTypeString = 'text';
            data = aParam.data;
        } else {
            for (i in aParam.data) {
                if (aParam.data.hasOwnProperty(i)) {
                    data[i] = aParam.data[i];
                }
            }
        }
        delete data['url'];
        var settings : any = {
            type : aType,
            url : urlString,
            data : data,
            dataType : dataTypeString,
            async : true,
            timeout : 10000,
            success : aFn,
            error : function(jqXHR, textStatus, errorThrown) {
                if (textStatus === 'timeout' && aRetryCount < 3) {
                    hx.log('error', '[ajax]Timed out retry ' + aRetryCount);
                    that.fnRequest(aType, aApiName, aParam, aFn, aFnError, aRetryCount + 1);
                } else {
                    var errorCode : number = eKERROR.KErrUnKnown;
                    if (aFnError) {
                        if (textStatus === 'timeout') {
                            errorCode = eKERROR.KErrTimedOut;
                        } else if (textStatus === 'abort') {
                            errorCode = eKERROR.KErrAborted;
                        } else if (textStatus === 'parsererror') {
                            errorCode = eKERROR.KErrParseError;
                        } else if (jqXHR.status === 401) {
                            errorCode = eKERROR.KErrNotAuthorized;
                        } else if (jqXHR.status === 403) {
                            errorCode = eKERROR.KErrNotAvailable;
                        } else if (jqXHR.status >= 500) {
                            errorCode = eKERROR.KErrServerFail;
                        }
                        hx.log('dsrvTVPortal', 'jqXHR.status=' + jqXHR.status + ' textStatus=' + textStatus);
                        that.mLastErrorCode = errorCode;
                        aFnError(errorCode);
                    } else {
                        hx.log('error', 'CTVPortal service error: ' + textStatus);
                    }
                    // for P/P updated on portal server
                    if (jqXHR.status === 451) {
                        that.fnCheckPrivacyVersion();
                    }
                }
            }
        };
        $.ajax(settings);
    }

    fnExecutePrimitive (aPrimitive, aFn?, aFnError?, aUserParam?) {
        var info = this.mPrimitivesInfo[aPrimitive];
        if (info.needSystemInfo && !this.mSystemInfo) {
            hx.log('debug', '"' + aPrimitive + '" requires system info.');
            if (aFnError) {
                aFnError();
            }
            return;
        }
        if (info.needAuthTicket && !this.mAuthTicket) {
            hx.log('debug', '"' + aPrimitive + '" requires authentication ticket.');
            if (aFnError) {
                aFnError();
            }
            return;
        }
        if (info.needUserParam) {
            var  i : number, userParamKeys : any = info.needUserParam;
            for ( i = 0; i < info.needUserParam; i += 1) {
                var key : any = userParamKeys[i];
                if (aUserParam[key] === undefined) {
                    throw '"' + aPrimitive + '" requires "' + userParamKeys.join(',') + '"';
                }
            }
        }
        if (info.getPostMessage) {
            this.fnRequest('post', info.api, {
                urlParam : info.getParam(aUserParam),
                data : info.getPostMessage(aUserParam)
            }, function(data) {
                hx.log('dsrvTVPortal', 'Executing "' + aPrimitive + '" OK');
                if (info.handleData) {
                    info.handleData(data, aUserParam);
                }
                if (aFn) {
                    aFn(data);
                }
            }, function(errorCode) {
                hx.log('error', 'Executing "' + aPrimitive + '" FAILED WITH: ' + errorCode);
                if (aFnError) {
                    aFnError();
                }
            });
        } else if (info.putData) {
            this.fnRequest('put', info.api, {
                urlParam : info.getParam(aUserParam),
                data : info.putData(aUserParam)
            }, function(data) {
                hx.log('dsrvTVPortal', 'Executing "' + aPrimitive + '" OK');
                if (info.handleData) {
                    info.handleData(data, aUserParam);
                }
                if (aFn) {
                    aFn(data);
                }
            }, function(errorCode) {
                hx.log('error', 'Executing "' + aPrimitive + '" FAILED WITH: ' + errorCode);
                if (aFnError) {
                    aFnError();
                }
            });
        } else {
            this.fnRequest('get', info.api, {
                data : info.getParam(aUserParam)
            }, function(data) {
                hx.log('dsrvTVPortal', 'Executing "' + aPrimitive + '" OK');
                if (info.handleData) {
                    info.handleData(data, aUserParam);
                }
                if (aFn) {
                    aFn(data);
                }
            }, function(errorCode) {
                hx.log('error', 'Executing "' + aPrimitive + '" FAILED WITH: ' + errorCode);
                if (aFnError) {
                    aFnError();
                }
            });
        }
    }

    fnExecutePrimitives (aPrimitives, aFn, aFnError, aParam?) {
        if (aPrimitives.length) {
            var p = aPrimitives.shift();
            this.fnExecutePrimitive(p, ()=> {
                this.fnExecutePrimitives(aPrimitives, aFn, aFnError, aParam);
            }, aFnError, aParam);
        } else {
            if (aFn) {
                aFn();
            }
        }
    }

    fnInstallApps (aDownloadAppInfo, aFnInstallFinished) {
        var that = this;
        var i : number, installed : boolean;
        var appInfo = aDownloadAppInfo.shift();
        if (appInfo) {
            installed = hx.svc.APP_MGR.isWidgetInstalled({
                id : appInfo.appid
            });
            if (!installed) {
                hx.log('dsrvTVPortal', 'Auto installing "' + appInfo.name + '"');
                hx.svc.APP_MGR.installWidget({
                    url : appInfo.downloadurl,
                    onCompleted : function(success) {
                        hx.log('dsrvTVPortal', 'Install finished "' + appInfo.name + '" result=' + success);
                        that.fnInstallApps(aDownloadAppInfo, aFnInstallFinished);
                    }
                });
            } else {
                hx.log('dsrvTVPortal', 'Already installed "' + appInfo.name + '"');
                that.fnInstallApps(aDownloadAppInfo, aFnInstallFinished);
            }
        } else {
            aFnInstallFinished();
        }
    }

    /**********************************************************
     * Functions
     **********************************************************/

    fnInit (aFn, aFnError) {
        var that = this;
        this.require('SETTING_UTIL', (dsrvSettingUtil)=> {
            var portalUrl : any = dsrvSettingUtil.getTVPortalUrl();
            var systemInfo : any = dsrvSettingUtil.getSystemInfo();
            var languageInfo : any = dsrvSettingUtil.getLanguage();
            hx.log('dsrvTVPortal', 'init: URL: ' + portalUrl);
            hx.log('dsrvTVPortal', 'init: CID : ' + systemInfo.chipId);
            hx.log('dsrvTVPortal', 'init: MAC: ' + systemInfo.wiredMacAddress);
            hx.log('dsrvTVPortal', 'init: LAN: ' + languageInfo.LangMenu);
            if (that.mInitialized) {
                that.fnGetActiveProfile(function() {
                    hx.log('dsrvTVPortal', 'GetActiveProfile succeed');
                    aFn(true);
                }, function() {
                    hx.log('error', 'GetActiveProfile Failed');
                    aFn(false);
                });
            } else {
                that.fnInitSecond(portalUrl, systemInfo, languageInfo.LangMenu, function() {
                    hx.log('dsrvTVPortal', 'create: init completed');
                    aFn(true);
                }, function() {
                    hx.log('error', 'create: init failed');
                    aFn(false);
                });
            }
        });
    }

    fnInitSecond (aPortalUrl, aSystemInfo, aLanguage, aFn, aFnError) {
        var that = this;
        this.mPortalUrl = aPortalUrl;
        this.mSystemInfo = aSystemInfo;
        this.fnSetLanguage(aLanguage, function() {
            that.fnExecutePrimitives([
                //'deviceReset',
                'logIn', 'getDeviceInfo', 'getDefaultAppList', 'getPreInstalledAppList', 'getInstalledAppList', 'getSmartSearchAppList'], function() {
                that.mInitialized = true;
                aFn();
                //var downloadAppInfo : any = [], appInfo : any, preInstalled : boolean, i : number;
                //if (that.mAppList.def && that.mAppList.def.length) {
                //    for ( i = 0; i < that.mAppList.def.length; i += 1) {
                //        appInfo = that.mAppList.def[i];
                //        if (appInfo.downloadurl) {
                //            downloadAppInfo.push(appInfo);
                //        }
                //    }
                //}
                //preInstalled = hx.svc.WEB_STORAGE.isPreInstalledFinished();
                //if (hx.config.useWidget !== undefined) {
                //    if (hx.config.useWidget === false) {
                //        if (preInstalled === false) {
                //            hx.svc.WEB_STORAGE.setPreInstalledFinished({
                //                preInstalled: true
                //            });
                //            preInstalled = true;
                //        }
                //    }
                //}
                //if (!preInstalled) {
                //    if (that.mAppList.pre && that.mAppList.pre.length) {
                //        for ( i = 0; i < that.mAppList.pre.length; i += 1) {
                //            appInfo = that.mAppList.pre[i];
                //            if (appInfo.downloadurl) {
                //                downloadAppInfo.push(appInfo);
                //            }
                //        }
                //    }
                //}
                //
                //if (downloadAppInfo.length && !preInstalled) {
                //    that.fnInstallApps(downloadAppInfo, function() {
                //        var processFinished = function() {
                //            that.mInitialized = true;
                //            aFn();
                //        };
                //        if (!preInstalled) {
                //            var appList = that.mAppList.def.concat(that.mAppList.pre);
                //            hx.svc.WEB_STORAGE.setPreInstalledFinished({
                //                preInstalled : true
                //            });
                //            processFinished();
                //        } else {
                //            processFinished();
                //        }
                //    });
                //} else {
                //    that.mInitialized = true;
                //    aFn();
                //}
            }, aFnError);
        }, aFnError);
    }

    fnLogin (aFn, aFnError, aParam?) {
        var that = this;
        if (that.mLogged) {
            hx.log('debug', 'already logged in!!! no need to log in again!');
            if (aFn) {
                aFn(that.mRespLogIn);
            }
            return;
        }
        that.mLoginQueue.push({fn : aFn, fnError : aFnError});
        if (that.mLogging) {
            hx.log('debug', 'on logging in !!! callbacks will be executed after it.');
            return;
        }
        that.mLogging = true;
        var todo;
        this.require('SETTING_UTIL', (dsrvSettingUtil)=> {
            that.mPortalUrl = dsrvSettingUtil.getTVPortalUrl();
            that.mSystemInfo = dsrvSettingUtil.getSystemInfo();
            that.fnExecutePrimitive('logIn', () => {
                that.mLogging = false;
                that.mLogged = true;
                if (hx.svc.SETTING_UTIL.getInitHumaxTvPortal()) {
                    hx.log('dsrvTVPortal', 'HumaxTvPortal needs to be initialized');
                    that.fnDeviceReset(() => {
                        that.fnExecutePrimitive('acceptPrivacyTerms', null, null, {
                            'privacy' : that.mPrivacyAccepted,
                            'terms' : that.mTermsAccepted
                        });
                    }, null, aParam);
                    hx.svc.SETTING_UTIL.setInitHumaxTvPortal(false);
                } else {
                    hx.svc.IP_EPG.setEnableHttp(that.mTermsAccepted);
                }
                if (that.isPrivacyPolicyAccepted()) {
                    that.fnPutCrashLogData();
                }
                hx.svc.STATISTICS.startLog();
                while (that.mLoginQueue.length) {
                    todo = that.mLoginQueue.pop();
                    if (todo && todo.fn) {
                        todo.fn(that.mRespLogIn);
                    }
                }
            }, () => {
                that.mLogging = false;
                hx.svc.IP_EPG.setEnableHttp(that.isTermsAndConditionsAccepted());
                while (that.mLoginQueue.length) {
                    todo = that.mLoginQueue.pop();
                    if (todo && todo.fnError) {
                        todo.fn({
                            AcceptPrivacy : that.isPrivacyPolicyAccepted(),
                            AcceptTerms : that.isTermsAndConditionsAccepted()
                        });
                    }
                }
            }, aParam);
        });
    }
    fnLogout (aFn, aFnError) {
        var that = this;
        if (!that.mLogged) {
            hx.log('debug', 'already logged out!!! no need to log out again!');
            if (aFn) {
                aFn(that.mRespLogIn);
            }
            return;
        }
        that.fnExecutePrimitive('logOut', () => {
            that.mLogged = false;
            that.mInitialized = false;
            if (aFn) {
                aFn();
            }
        }, () => {
            if (aFnError) {
                aFnError({
                    AcceptPrivacy : that.isPrivacyPolicyAccepted(),
                    AcceptTerms : that.isTermsAndConditionsAccepted()
                });
            }
        });
    }
    fnDeviceReset (aFn?, aFnError?, aParam?) {
        var that = this;
        this.require('SETTING_UTIL', (dsrvSettingUtil)=> {
            that.mPortalUrl = dsrvSettingUtil.getTVPortalUrl();
            that.mSystemInfo = dsrvSettingUtil.getSystemInfo();
            that.fnExecutePrimitive('deviceReset', () => {
                hx.svc.IP_EPG.setEnableHttp(that.mTermsAccepted);
                if (aFn) {
                    aFn();
                }
            }, aFnError, aParam);
        });
    }
    fnSetLanguage (aLanguage, aFn?, aFnError?) {
        // http://technet.microsoft.com/ko-kr/library/dd744369(v=ws.10).aspx
        var langTable : any = {
            "alb" : "sq", //Albanian,  eLangID_Albanian
            "ara" : "ar", //Arabic,    eLangID_Arabic
            "aus" : null, //Australian,    eLangID_Australian
            "baq" : "eu-ES", //Basque,    eLangID_Basque
            "bas" : null, //Basaa, eLangID_Basaa
            "bul" : "bg", //Bulgarian, eLangID_Bulgarian
            "cat" : null, //Catalan,   eLangID_Catalan
            "cze" : "cs", //Czech, eLangID_Czech
            "dan" : "da", //Danish,    eLangID_Danish
            "dut" : "nl", //Dutch, eLangID_Dutch
            "eng" : "en", //English,   eLangID_English
            "est" : "et", //Estonian,  eLangID_Estonian
            "fao" : null, //Faroese,   eLangID_Faroese
            "fin" : "fi", //Finnish,   eLangID_Finnish
            "fre" : "fr", //French,    eLangID_French
            "ger" : "de", //German,    eLangID_German
            "gla" : null, //Scottish Gaelic,   eLangID_Gaelic
            "gle" : "ga", //Irish, eLangID_Irish
            "glg" : "gl", //Galician,  eLangID_Galician
            "gre" : "el", //Greek, eLangID_Greek
            "heb" : "he", //Hebrew,    eLangID_Hebrew
            "hin" : "hi", //Hindi, eLangID_Hindi
            "hun" : "hu", //Hungarian, eLangID_Hungarian
            "ice" : "is", //Icelandic, eLangID_Icelandic
            "ita" : "it", //Italian,   eLangID_Italian
            "jpn" : "ja", //Japanese,  eLangID_Japanese
            "kor" : "ko", //Korean,    eLangID_Korean
            "lav" : "lv", //Latvian,   eLangID_Latvian
            "lit" : "lt", //Lithuanian,    eLangID_Lithuanian
            //,"nar" //Audio Description, eLangID_Narration
            "nor" : "nb", //Norwegian, eLangID_Norwegian
            "per" : null, //Persian,   eLangID_Persian
            "pol" : "pl", //Polish,    eLangID_Polish
            "por" : "pt", //Portuguese,    eLangID_Portuguese
            //,"qaa" //Original Language, eLangID_OriginalAudio
            "rum" : "ro", //Romanian,  eLangID_Romanian
            "rus" : "ru", //Russian,   eLangID_Russian
            "scr" : "hr", //Croatian,  eLangID_Croatian
            "slo" : "sk", //Slovak,    eLangID_Slovak
            "slv" : "sl", //Slovenian, eLangID_Slovenian
            "spa" : "es", //Spanish,   eLangID_Spanish
            "srp" : "sr", //Serbian,   eLangID_Serbian
            "swe" : "sv", //Swedish,   eLangID_Swedish
            "tha" : "th", //Thai,  eLangID_Thai
            "tur" : "tr", //Turkish,   eLangID_Turkish
            "ukr" : "uk", //Ukrainian, eLangID_Ukrainian
            //,"und" //Undefine,  eLangID_Undefined
            "vie" : "vi", //Vietnamese,    eLangID_Vietnamese
            "wel" : null //Welsh, eLangID_Welsh
        };
        this.mLanguage = langTable[aLanguage] || 'en';
        hx.log('dsrvTVPortal', 'Language Code: "' + this.mLanguage + '"');
        this.fnExecutePrimitives(['getPrivacyVersion', 'getPrivacy', 'getTermsVersion', 'getTermsAndConditions'], function() {
            if (aFn) {
                aFn();
            }
        }, aFnError, {
            bForced : true
        });
    }

    fnInitialized () {
        return this.mInitialized;
    }

    fnIsLogged () {
        return this.mLogged;
    }

    fnCheckPrivacyVersion (aFn?, aFnError?) {
        this.fnExecutePrimitive(['getPrivacyVersion'], () => {
            hx.log('debug', '[fnCheckPrivacyVersion] this.mNewPrivacyVersion : ' + this.mNewPrivacyVersion);
            hx.log('debug', '[fnCheckPrivacyVersion] this.mPrivacyInfo.version : ' + this.mPrivacyInfo.version);
            if (this.mNewPrivacyVersion === this.mPrivacyInfo.version) {
                if (aFn) {
                    aFn();
                }
            } else {
                // updated.
                this.mPrivacyInfo = {};
                this.mPrivacyAccepted = false;
                if (this.mRespLogIn) {
                    this.mRespLogIn.AcceptPrivacy = this.mPrivacyAccepted;
                }
                hx.svc.WEB_STORAGE.setPrivacyPolicyState(this.mPrivacyAccepted);
                //hx.svc.WEB_STORAGE.setPrivacyPolicyText(null);
                //hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(null);
                if (aFnError) {
                    aFnError();
                }
            }
        }, () => {
            // nothing
        }, {
            bForced : true
        });
    }

    fnCheckTermsVersion (aFn?, aFnError?) {
        this.fnExecutePrimitive(['getTermsVersion'], () => {
            hx.log('debug', '[fnCheckTermsVersion] this.mNewTermsVersion : ' + this.mNewTermsVersion);
            hx.log('debug', '[fnCheckTermsVersion] this.mTermsInfo.version : ' + this.mTermsInfo.version);
            if (this.mNewTermsVersion === this.mTermsInfo.version) {
                if (aFn) {
                    aFn();
                }
            } else {
                // updated.
                this.mTermsInfo = {};
                this.mTermsAccepted = false;
                if (this.mRespLogIn) {
                    this.mRespLogIn.AcceptTerms = this.mTermsAccepted;
                }
                hx.svc.WEB_STORAGE.setTermsAndConditionsState(this.mTermsAccepted);
                if (aFnError) {
                    aFnError();
                }
            }
        }, () => {
            // nothing
        }, {
            bForced : true
        });
    }

    fnGetPrivacyPolicyText (aFn?, aFnError?) {
        this.fnCheckPrivacyVersion(() => {
            // same version
            if (this.mPrivacyInfo && this.mPrivacyInfo.text) {
                if (aFn) {
                    aFn(this.mPrivacyInfo.text);
                }
            } else {
                this.fnExecutePrimitive('getPrivacy', () => {
                    if (aFn) {
                        aFn(this.mPrivacyInfo.text);
                    }
                }, aFnError, {
                    bForced : true
                });
            }
        }, () => {
            // new version
            this.fnExecutePrimitive('getPrivacy', () => {
                if (aFn) {
                    aFn(this.mPrivacyInfo.text);
                }
            }, aFnError, {
                bForced : true
            });
        });
    }

    fnGetTermsAndConditionsText (aFn?, aFnError?) {
        this.fnCheckTermsVersion(() => {
            // same version
            if (this.mTermsInfo && this.mTermsInfo.text) {
                if (aFn) {
                    aFn(this.mTermsInfo.text);
                }
            } else {
                this.fnExecutePrimitive('getTermsAndConditions', () => {
                    if (aFn) {
                        aFn(this.mTermsInfo.text);
                    }
                }, aFnError, {
                    bForced : true
                });
            }
        }, () => {
            // new version
            this.fnExecutePrimitive('getTermsAndConditions', () => {
                if (aFn) {
                    aFn(this.mTermsInfo.text);
                }
            }, aFnError, {
                bForced : true
            });
        });
    }

    fnGetProfileList (aFn, aFnError) {
        this.fnExecutePrimitive('getProfileList', aFn, aFnError);
    }

    fnGetActiveProfile (aFn, aFnError) {
        this.fnExecutePrimitive('getActiveProfile', aFn, () => {
            if (aFnError) {
                aFnError({
                    AcceptPrivacy : this.isPrivacyPolicyAccepted(),
                    AcceptTerms : this.isTermsAndConditionsAccepted()
                });
            }
        });
    }

    fnSetActiveProfile (aProfileInfo, aPinCode, aFn, aFnError) {
        var that = this;
        this.fnExecutePrimitive('setActiveProfile', function() {
            that.fnExecutePrimitive('getActiveProfile', function() {
                if (aProfileInfo.UID === that.mActiveProfile.UID) {
                    that.fnExecutePrimitives(['getInstalledAppList', 'getSmartSearchAppList'], aFn, aFnError);
                } else {
                    aFnError();
                }
            }, aFnError);
        }, aFnError, {
            'deviceId' : aProfileInfo.DID,
            'userId' : aProfileInfo.UID,
            'pinCode' : aPinCode
        });
    }

    fnGetRegistrationCode (aFn, aFnError) {
        this.fnExecutePrimitive('getRegistrationCode', aFn, aFnError);
    }

    fnGetFeaturedAppList () {
        return this.mAppList.featured;
    }

    fnGetAvailableAppList () {
        return this.mAppList.available;
    }

    fnGetOttAppList () {
        return this.mAppList.ott;
    }

    fnGetDefaultAppList () {
        return this.mAppList.def;
    }

    fnGetPreInstalledAppList () {
        return this.mAppList.pre;
    }

    fnRequestFeaturedAppList (fn, fnError) {
        this.fnExecutePrimitive('getFeaturedAppList', fn, fnError);
    }

    fnRequestFavouriteAppList (fn, fnError) {
        this.fnExecutePrimitive('getFavoriteAppsList', fn, fnError);
    }

    fnRequestDefaultAppList (fn, fnError) {
        this.fnExecutePrimitive('getDefaultAppList', fn, fnError);
    }

    fnGetInstalledAppList (aFn, aFnError) {
        this.fnExecutePrimitive('getInstalledAppList', aFn, aFnError);
    }

    fnGetApplicationUrl (aApplicationId, aFn, aFnError?) {
        this.fnExecutePrimitive('getApplicationUrl', aFn, aFnError, {
            'applicationId' : aApplicationId
        });
    }

    fnDeleteApplication (aApplicationId, aFn, aFnError) {
        this.fnExecutePrimitive('deleteApplication', aFn, aFnError, {
            'applicationId' : aApplicationId
        });
    }

    fnUpdateApplicationList (aApplicationList, aFn, aFnError) {
        this.fnExecutePrimitive('updateApplicationList', aFn, aFnError, {
            'applicationList' : aApplicationList
        });
    }
    fnAcceptPrivacyPolicy (aAccept, aFn, aFnError) {
        var prePrivacyAccepted = this.mPrivacyAccepted;
        if (aAccept) {
            this.fnLogin(() => {
                this.mPrivacyAccepted = aAccept;
                this.mPrivacyInfo.version = this.mNewPrivacyVersion;
                hx.svc.WEB_STORAGE.setPrivacyPolicyState(aAccept);
                hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(this.mNewPrivacyVersion);
                hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(this.mNewPrivacyVersion);
                this.fnExecutePrimitive('acceptPrivacyPolicy', aFn, aFnError, {
                    'agree' : aAccept
                });
            }, () => {
                this.mPrivacyAccepted = aAccept;
                hx.svc.WEB_STORAGE.setPrivacyPolicyState(aAccept);
                hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(null);
                if (aFn) {
                    aFn();
                }
            }, {
                bForced : true
            });
        } else {
            this.fnExecutePrimitive('acceptPrivacyPolicy', aFn, aFnError, {
                'agree' : aAccept
            });
            this.fnLogout(() => {}, () => {});
            this.mPrivacyAccepted = aAccept;
            this.mPrivacyInfo.version = this.mNewPrivacyVersion;
            hx.svc.WEB_STORAGE.setPrivacyPolicyState(aAccept);
            hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(this.mNewPrivacyVersion);
            hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(null);
        }
        if (prePrivacyAccepted !== aAccept) {
            this.fireEvent('privacyPolicyChange', aAccept);
        }
    }

    fnAcceptTermsAndConditions (aAccept, aFn, aFnError) {
        this.fnExecutePrimitive('acceptTermsAndConditions', aFn, aFnError, {
            'agree' : aAccept
        });
        this.mTermsAccepted = aAccept;
        this.mTermsInfo.version = this.mNewTermsVersion;
        hx.svc.WEB_STORAGE.setTermsAndConditionsState(aAccept);
        hx.svc.WEB_STORAGE.setTermsAndConditionsVersion(this.mNewTermsVersion);
    }

    fnAcceptPrivacyTerms (aPrivacyAccept, aTermsAccept, aFn, aFnError) {
        var prePrivacyAccepted = this.mPrivacyAccepted;
        if (aPrivacyAccept) {
            this.fnLogin(() => {
                this.mPrivacyAccepted = aPrivacyAccept;
                this.mPrivacyInfo.version = this.mNewPrivacyVersion;
                hx.svc.WEB_STORAGE.setPrivacyPolicyState(aPrivacyAccept);
                hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(this.mNewPrivacyVersion);
                hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(this.mNewPrivacyVersion);
                this.fnExecutePrimitive('acceptPrivacyTerms', aFn, aFnError, {
                    'privacy' : aPrivacyAccept,
                    'terms' : aTermsAccept
                });
            }, () => {
                this.mPrivacyAccepted = aPrivacyAccept;
                hx.svc.WEB_STORAGE.setPrivacyPolicyState(aPrivacyAccept);
                hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(null);
                if (aFn) {
                    aFn();
                }
            }, {
                bForced : true
            });
        } else {
            this.fnExecutePrimitive('acceptPrivacyTerms', aFn, aFnError, {
                'privacy' : aPrivacyAccept,
                'terms' : aTermsAccept
            });
            this.fnLogout(() => {}, () => {});
            this.mPrivacyAccepted = aPrivacyAccept;
            this.mPrivacyInfo.version = this.mNewPrivacyVersion;
            hx.svc.WEB_STORAGE.setPrivacyPolicyState(aPrivacyAccept);
            hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(this.mNewPrivacyVersion);
            hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(null);
        }
        this.mTermsAccepted = aTermsAccept;
        this.mTermsInfo.version = this.mNewTermsVersion;
        hx.svc.WEB_STORAGE.setTermsAndConditionsState(aTermsAccept);
        hx.svc.WEB_STORAGE.setTermsAndConditionsVersion(this.mNewTermsVersion);
        if (prePrivacyAccepted !== aPrivacyAccept) {
            this.fireEvent('privacyPolicyChange', aPrivacyAccept);
        }
    }

    fnGetSmartSearchAppList (aFn, aFnError) {
        this.fnExecutePrimitive('getSmartSearchAppList', aFn, aFnError);
    }

    fnGetSmartSearchPlayers(aFn, aFnError) {
        this.fnExecutePrimitive('getSmartSearchPlayers', aFn, aFnError);
    }

    fnSetStatisticsLog (aStatisticsLog, aFn, aFnError) {
        this.fnExecutePrimitive('setStatisticsLog', aFn, aFnError, {
            'statisticsLog' : aStatisticsLog
        });

    }

    fnFindProgrammes (aParam, aFn, aFnError) {
        this.fnExecutePrimitive('findProgrammes', aFn, aFnError, {
            'url' : aParam.url,
            'searchText' : aParam.searchText,
            'offset' : aParam.offset,
            'limit' : aParam.limit
        });
    }

    fnGetLastErrorCode () {
        return this.mLastErrorCode;
    }

    fnCreateWidget (aWd, aUrl?) {
        var that = this;
        hx.log('dsrvTVPortal', 'fnCreateWidget');
        hx.svc.AV_CTRL.blockAV({
            cb : function() {
                that.mDsrvAppMgr.createWidget({
                    name : aWd.name,
                    wd : aWd,
                    url : aUrl,
                    onLoaded : function(appInfo) {
                        that.mActiveAppInfo = appInfo;
                        hx.log('dsrvTVPortal', 'fnCreateWidget.onLoaded');
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text : '-TV APPS-'
                        });
                    },
                    onKeyDown : function(appInfo, keyCode) {
                        hx.log('dsrvTVPortal', 'fnCreateWidget.onKeyDown: ' + keyCode);
                        if (that.mActiveAppInfo && keyCode === hx.key.KEY_ESCAPE) {
                            that.mDsrvAppMgr.destroyWidget(appInfo);
                        }
                    },
                    onUnloaded : function(appInfo) {
                        that.mActiveAppInfo = null;
                        hx.svc.AV_CTRL.unBlockAV();
                        var success = hx.svc.AV_CTRL.lastChannel();
                        if (!success) {
                            hx.svc.AV_CTRL.availableChannel();
                        }
                        hx.svc.SETTING_UTIL.setFrontPanelString({
                            text : ''
                        });
                    }
                });
            }
        });
    }

    fnLaunchApp (aWidgetId, aFnError) {
        var that = this;
        hx.log('dsrvTVPortal', 'fnLaunchApp');
        var wd = this.mDsrvAppMgr.getWidgetDescriptor({
            widgetId : aWidgetId
        });
        if (aWidgetId.indexOf('linked://') === 0) {
            var appId = aWidgetId.substr(9, aWidgetId.length - 9);
            this.fnGetApplicationUrl(appId, function(result) {
                if (result.Status) {
                    that.fnCreateWidget(wd, result.Url);
                } else {
                    aFnError(result);
                }
            });
        } else {
            that.fnCreateWidget(wd);
        }
    }

    fnDestroyActiveApp () {
        hx.log('dsrvTVPortal', 'fnDestroyActiveApp');
        if (this.mActiveAppInfo) {
            this.mDsrvAppMgr.destroyWidget(this.mActiveAppInfo);
        }
    }

    fnPutCrashLogData () {
        if (this.mCheckCrashLog) {
            hx.log('dsrvTVPortal', 'fnPutCrashLogData - already checked HCR data');
            return;
        }
        hx.log('dsrvTVPortal', 'fnPutCrashLogData - going to check HCR data');
        this.mCheckCrashLog = true;
        if (this.mLocalSystemOipfObj.checkHcrData) {
            hx.log('dsrvTVPortal', 'fnPutCrashLogData - Found HCR send to server !!');
            this.fnExecutePrimitive('putCrashLog');
        }
    }

    fnCheckNewSoftware (aFn, aFnError) {
        hx.log('dsrvTVPortal', 'fnCheckNewSoftware');
        this.fnExecutePrimitive('checkNewSoftware', aFn, aFnError);
    }

    fnGetCurrentRegionInfo (fn, fnError) {
        hx.log('dsrvTVPortal', 'fnGetCurrentRegionInfo');
        this.fnExecutePrimitive('getCurrentRegionInfo', fn, fnError);
        hx.log('dsrvTVPortal', 'mRegionInfo.ip: ' + this.mRegionInfo.ip);
    }

    fnGetCheckUploadStatistics (fn, fnError) {
        hx.log('dsrvTVPortal', 'fnGetCheckUploadStatistics');
        this.fnExecutePrimitive('getCheckUploadStatistics', fn, fnError);
    }

    fnGetRecommendations(aFn, aFnError, aUserParam) {
        hx.log('dsrvTVPortal', 'fnGetRecommendations');
        this.fnExecutePrimitive('getRecommendations', aFn, aFnError, aUserParam);
    }

    init (aParam) {
        this.fnInit(aParam.fn, aParam.fnError);
    }
    login (aParam) {
        this.fnLogin(aParam.fn, aParam.fnError);
    }
    deviceReset (aParam) {
        this.fnDeviceReset(aParam.fn, aParam.fnError);
    }
    setLanguage (aParam) {
        this.fnSetLanguage(aParam.language, aParam.fn, aParam.fnError);
    }
    isInitialized () {
        return this.fnInitialized();
    }
    isLogged () {
        return this.fnIsLogged();
    }
    checkPrivacyVersion (aFn?, aFnError?) {
        return this.fnCheckPrivacyVersion(aFn, aFnError);
    }
    getPrivacyPolicyText (aFn?, aFnError?) {
        return this.fnGetPrivacyPolicyText(aFn, aFnError);
    }
    getTermsAndConditionsText (aFn?, aFnError?) {
        return this.fnGetTermsAndConditionsText(aFn, aFnError);
    }
    getProfileList (aParam) {
        this.fnGetProfileList(aParam.fn, aParam.fnError);
    }
    getActiveProfile (aParam) {
        this.fnGetActiveProfile(aParam.fn, aParam.fnError);
    }
    setActiveProfile (aParam) {
        this.fnSetActiveProfile(aParam.profileInfo, aParam.pinCode, aParam.fn, aParam.fnError);
    }
    getRegistrationCode (aParam) {
        this.fnGetRegistrationCode(aParam.fn, aParam.fnError);
    }
    getFeaturedAppList (aParam) {
        if (this.fnGetFeaturedAppList() && aParam.fn) {
            return aParam.fn(this.fnGetFeaturedAppList());
        } else {
            this.fnRequestFeaturedAppList(aParam.fn, aParam.fnError);
        }
    }
    getAvailableAppList () {
        return this.fnGetAvailableAppList();
    }
    getOttAppList () {
        return this.fnGetOttAppList();
    }
    getDefaultAppList (aParam) {
        return this.fnRequestDefaultAppList(aParam.fn, aParam.fnError);
    }
    getPreInstalledAppList () {
        return this.fnGetPreInstalledAppList();
    }
    getInstalledAppList (aParam) {
        this.fnGetInstalledAppList(aParam.fn, aParam.fnError);
    }
    getApplicationUrl (aParam) {
        this.fnGetApplicationUrl(aParam.applicationId, aParam.fn, aParam.fnError);
    }
    deleteApplication (aParam) {
        this.fnDeleteApplication(aParam.applicationId, aParam.fn, aParam.fnError);
    }
    updateApplicationList (aParam) {
        this.fnUpdateApplicationList(aParam.applicationList, aParam.fn, aParam.fnError);
    }
    acceptPrivacyPolicy (aParam) {
        this.fnAcceptPrivacyPolicy(aParam.accept, aParam.fn, aParam.fnError);
    }
    acceptTermsAndConditions (aParam) {
        var self = this;
        var preTermsAccepted = self.mTermsAccepted;
        var fn = (data) =>{
            if (preTermsAccepted !== aParam.accept) {
                hx.svc.IP_EPG.setEnableHttp(aParam.accept).then(() => {
                    if (aParam.fn) {
                        aParam.fn(data);
                    }
                }, () => {
                    if (aParam.fnError) {
                        aParam.fnError();
                    }
                });
            } else {
                if (aParam.fn) {
                    aParam.fn(data);
                }
            }
        };
        this.fnAcceptTermsAndConditions(aParam.accept, fn, fn);
    }
    acceptPrivacyTerms (aParam) {
        var self = this;
        var preTermsAccepted = self.mTermsAccepted;
        var fn = (data) => {
            if (preTermsAccepted !== aParam.terms) {
                hx.svc.IP_EPG.setEnableHttp(aParam.terms).then(() => {
                    if (aParam.fn) {
                        aParam.fn(data);
                    }
                }, () => {
                    if (aParam.fnError) {
                        aParam.fnError();
                    }
                });
            } else {
                if (aParam.fn) {
                    aParam.fn(data);
                }
            }
        };
        this.fnAcceptPrivacyTerms(aParam.privacy, aParam.terms, fn, fn);
    }
    getSmartSearchAppList (aParam) {
        return this.fnGetSmartSearchAppList(aParam.fn, aParam.fnError);
    }
    getSmartSearchPlayers(aParam) {
        return this.fnGetSmartSearchPlayers(aParam.fn, aParam.fnError);
    }
    setStatisticsLog (aParam) {
        return this.fnSetStatisticsLog(aParam.statisticsLog, aParam.fn, aParam.fnError);
    }
    findProgrammes (aParam) {
        this.fnFindProgrammes(aParam.param, aParam.fn, aParam.fnError);
    }
    getLastErrorCode () {
        return this.fnGetLastErrorCode();
    }
    launchApp (aParam) {
        this.fnLaunchApp(aParam.widgetId, aParam.fnError);
    }
    destroyActiveApp () {
        this.fnDestroyActiveApp();
    }
    isPrivacyPolicyAccepted () {
        return this.mPrivacyAccepted;
    }
    isTermsAndConditionsAccepted () {
        return this.mTermsAccepted;
    }
    putCrashLogData () {
        hx.log('dsrvTVPortal', 'putCrashLogData');
        return this.fnPutCrashLogData();
    }
    putExceptionLogData(exceptionData) {
        this.fnExecutePrimitive('putExceptionLog', null, null, exceptionData);
    }
    checkNewSoftware (aParam) {
        this.fnCheckNewSoftware(aParam.fn, aParam.fnError);
    }
    getCurrentRegionInfo (param) {
        this.fnGetCurrentRegionInfo(param.fn, param.fnError);
    }
    getExternalIp () {
        return this.mRegionInfo.ip;
    }
    getCheckUploadStatistics (param) {
        this.fnGetCheckUploadStatistics(param.fn, param.fnError);
    }
    getRecommendations(aParam) {
        return this.fnGetRecommendations(aParam.fn, aParam.fnError, aParam.userParam);
    }
    getAuthTicket () : any {
        return this.mAuthTicket;
    }
    getPortalUrl () : any {
        return this.mPortalUrl;
    }
    enableHumaxServerForTest() {
        this.mDisableHumaxServer = false;
    }
    disableHumaxServerForTest() {
        this.mDisableHumaxServer = true;
    }

    _updateTermsAndConditions(bAccepted : boolean){
        this.mTermsAccepted = bAccepted;
        this.fireEvent('TermsAndConditionsUpdated',bAccepted);
    }
}
export = cDLTV_PORTAL;
