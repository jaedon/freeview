/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvTVPortal.js
/*jslint sloppy:true, nomen:true, vars:true */
//    TV_PORTAL: ['layer_domain/service/dl_dsrvTVPortal.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var eKERROR;
    (function (eKERROR) {
        eKERROR[eKERROR["KErrNone"] = 0] = "KErrNone";
        eKERROR[eKERROR["KErrUnKnown"] = 1] = "KErrUnKnown";
        eKERROR[eKERROR["KErrAborted"] = 2] = "KErrAborted";
        eKERROR[eKERROR["KErrParseError"] = 3] = "KErrParseError";
        eKERROR[eKERROR["KErrTimedOut"] = 4] = "KErrTimedOut";
        eKERROR[eKERROR["KErrNotAuthorized"] = 10] = "KErrNotAuthorized";
        eKERROR[eKERROR["KErrNotAvailable"] = 11] = "KErrNotAvailable";
        eKERROR[eKERROR["KErrServerFail"] = 12] = "KErrServerFail";
        eKERROR[eKERROR["KErrPrivacyPolicyUpdated"] = 11] = "KErrPrivacyPolicyUpdated";
        eKERROR[eKERROR["KErrPinError"] = 100] = "KErrPinError";
    })(eKERROR || (eKERROR = {}));
    ;
    var cDLTV_PORTAL = (function (_super) {
        __extends(cDLTV_PORTAL, _super);
        function cDLTV_PORTAL(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mLocalSystemOipfObj = this.getPluginInstance('LocalSystem');
            this.mConfigurationOipfObj = this.getPluginInstance('Configuration');
            this.p = {};
            //mPortalUrl : string = 'http://test.humaxtvportal.com';
            this.mPortalUrl = hx.svc.SETTING_UTIL.getTVPortalUrl();
            this.mLanguage = 'en';
            this.mPrivacyInfo = hx.svc.WEB_STORAGE.getPrivacyPolicy();
            this.mTermsInfo = hx.svc.WEB_STORAGE.getTermsAndConditions();
            this.mSystemInfo = hx.svc.SETTING_UTIL.getSystemInfo();
            this.mSoftwareVersion = this.mSystemInfo.softwareVersion.substr(7, 13);
            this.mRespLogIn = null;
            this.mAuthTicket = null;
            this.mPrivacyAccepted = !!(this.mPrivacyInfo.state === "true");
            this.mTermsAccepted = !!(this.mTermsInfo.state === "true");
            this.mNewPrivacyVersion = this.mPrivacyInfo.version || '';
            this.mNewTermsVersion = this.mTermsInfo.version || '';
            this.mDeviceInfo = null;
            this.mProfiles = null;
            this.mActiveProfile = null;
            this.mRegistrationCode = null;
            this.mInitialized = false;
            this.mLogged = false;
            this.mLogging = false;
            this.mCheckCrashLog = false;
            this.mAppList = {
                featured: null,
                available: null,
                ott: null,
                def: null,
                installed: null,
                pre: null
            }; //
            this.mRegionInfo = {
                ip: '',
                country: '',
                countryname: '',
                city: ''
            };
            this.mSmartSearchAppList = null;
            this.mDsrvAppMgr = null;
            this.mActiveAppInfo = null;
            this.mLastErrorCode = 0 /* KErrNone */;
            this.mDisableHumaxServer = false;
            this.mLoginQueue = [];
            /*
             * HUMAX TV Portal open API information
             */
            this.fnGetCommonLanParam = function () {
                return {
                    'Lang': _this.mLanguage
                };
            };
            this.fnGetCommonAuthParam = function () {
                return {
                    'authticket': _this.mAuthTicket
                };
            };
            this.fnGetCommonAvailAppParam = function (aType) {
                return {
                    'authticket': _this.mAuthTicket,
                    'appsType': aType
                };
            };
            this.mPrimitivesInfo = {
                'getPrivacyVersion': {
                    needSystemInfo: true,
                    api: 'GetLegalDocVer',
                    getParam: function (aParam) {
                        var ret = {
                            'SystemID': _this.mSystemInfo.system_id
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
                    handleData: function (data) {
                        if (data) {
                            var list = data.list;
                            var newVersion = '';
                            for (var i = 0, len = list.length; i < len; i += 1) {
                                if (list[i].docType === 'privacy') {
                                    newVersion = list[i].key + '_' + list[i].seq;
                                }
                            }
                        }
                        if (newVersion !== _this.mNewPrivacyVersion) {
                            _this.mNewPrivacyVersion = newVersion;
                        }
                    }
                },
                'getTermsVersion': {
                    needSystemInfo: true,
                    api: 'GetLegalDocVer',
                    getParam: function (aParam) {
                        var ret = {
                            'SystemID': _this.mSystemInfo.system_id
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
                    handleData: function (data) {
                        if (data) {
                            var list = data.list;
                            var newVersion = '';
                            for (var i = 0, len = list.length; i < len; i += 1) {
                                if (list[i].docType === 'terms') {
                                    newVersion = list[i].key + '_' + list[i].seq;
                                }
                            }
                        }
                        if (newVersion !== _this.mNewTermsVersion) {
                            _this.mNewTermsVersion = newVersion;
                        }
                    }
                },
                'getPrivacy': {
                    needSystemInfo: true,
                    api: 'GetLegalDoc',
                    getParam: function (aParam) {
                        var ret = {
                            'doctype': 'privacy',
                            'SystemID': _this.mSystemInfo.system_id,
                            'Lang': _this.mLanguage
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
                    handleData: function (data) {
                        _this.mPrivacyInfo.text = data.text;
                        hx.svc.WEB_STORAGE.setPrivacyPolicyText(data.text);
                    }
                },
                'getTermsAndConditions': {
                    needSystemInfo: true,
                    api: 'GetLegalDoc',
                    getParam: function (aParam) {
                        var ret = {
                            'doctype': 'terms',
                            'SystemID': _this.mSystemInfo.system_id,
                            'Lang': _this.mLanguage
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
                    handleData: function (data) {
                        _this.mTermsInfo.text = data.text;
                        hx.svc.WEB_STORAGE.setTermsAndConditionsText(data.text);
                    }
                },
                'acceptPrivacyPolicy': {
                    needAuthTicket: true,
                    needUserParam: ['agree'],
                    api: 'AcceptPrivacy',
                    getParam: function (userParam) {
                        return {
                            'authticket': _this.mAuthTicket,
                            'agree': userParam.agree ? 'Y' : 'N'
                        };
                    },
                    handleData: function (data, userParam) {
                        if (data.status) {
                            if (_this.mRespLogIn) {
                                _this.mRespLogIn.AcceptPrivacy = userParam.agree;
                            }
                            _this.mPrivacyAccepted = userParam.agree;
                        }
                    }
                },
                'acceptTermsAndConditions': {
                    needAuthTicket: true,
                    needUserParam: ['agree'],
                    api: 'AcceptTerms',
                    getParam: function (userParam) {
                        return {
                            'authticket': _this.mAuthTicket,
                            'agree': userParam.agree ? 'Y' : 'N'
                        };
                    },
                    handleData: function (data, userParam) {
                        if (data.status) {
                            if (_this.mRespLogIn) {
                                _this.mRespLogIn.AcceptTerms = userParam.agree;
                            }
                            _this.mTermsAccepted = userParam.agree;
                        }
                    }
                },
                'acceptPrivacyTerms': {
                    needAuthTicket: true,
                    needUserParam: ['privacy', 'terms'],
                    api: 'AcceptPrivacyTerms',
                    getParam: function (userParam) {
                        return {
                            'authticket': _this.mAuthTicket,
                            'privacy': userParam.privacy ? 'Y' : 'N',
                            'terms': userParam.terms ? 'Y' : 'N',
                        };
                    },
                    handleData: function (data, userParam) {
                        if (data.status) {
                            if (_this.mRespLogIn) {
                                _this.mRespLogIn.AcceptPrivacy = userParam.privacy;
                                _this.mRespLogIn.AcceptTerms = userParam.terms;
                            }
                            _this.mPrivacyAccepted = userParam.privacy;
                            _this.mTermsAccepted = userParam.terms;
                        }
                    }
                },
                'deviceReset': {
                    needSystemInfo: true,
                    api: 'DeviceReset',
                    getParam: function (aParam) {
                        var now = new Date();
                        var timezoneOffset = now.getTimezoneOffset();
                        if (timezoneOffset > 0) {
                            timezoneOffset = '-' + timezoneOffset;
                        }
                        else {
                            timezoneOffset = timezoneOffset * -1;
                        }
                        var ret = {
                            'deviceID': _this.mSystemInfo.wiredMacAddress,
                            'MACAddress': _this.mSystemInfo.wiredMacAddress,
                            'SystemID': _this.mSystemInfo.system_id,
                            'SWVer': _this.mSoftwareVersion,
                            'serialNumber': _this.mSystemInfo.serialNumber,
                            'timeZone': '' + timezoneOffset
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
                    handleData: function (data) {
                        if (data) {
                            _this.mRespLogIn = data;
                            _this.mAuthTicket = data.AuthTicket;
                        }
                        //this._updateTermsAndConditions(data.AcceptTerms);
                    }
                },
                'logIn': {
                    needSystemInfo: true,
                    api: 'Login',
                    getParam: function (aParam) {
                        var now = new Date();
                        var timezoneOffset = now.getTimezoneOffset();
                        if (timezoneOffset > 0) {
                            timezoneOffset = '-' + timezoneOffset;
                        }
                        else {
                            timezoneOffset = timezoneOffset * -1;
                        }
                        var ret = {
                            'deviceID': _this.mSystemInfo.wiredMacAddress,
                            'MACAddress': _this.mSystemInfo.wiredMacAddress,
                            'SystemID': _this.mSystemInfo.system_id,
                            'SWVer': _this.mSoftwareVersion,
                            'serialNumber': _this.mSystemInfo.serialNumber,
                            'timeZone': '' + timezoneOffset
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
                    handleData: function (data) {
                        if (data) {
                            _this.mRespLogIn = data;
                            _this.mAuthTicket = data.AuthTicket;
                            _this.mPrivacyAccepted = data.AcceptPrivacy;
                            _this.mTermsAccepted = data.AcceptTerms;
                            hx.svc.WEB_STORAGE.setPrivacyPolicyState(_this.mPrivacyAccepted);
                            hx.svc.WEB_STORAGE.setTermsAndConditionsState(_this.mTermsAccepted);
                        }
                        //this._updateTermsAndConditions(data.AcceptTerms);
                    }
                },
                'logOut': {
                    api: 'Logout',
                    getParam: function () {
                        return {};
                    },
                    handleData: function (data) {
                        if (data && data.status) {
                            _this.mRespLogIn = null;
                            _this.mAuthTicket = null;
                        }
                    }
                },
                // TODO: ReLogin
                'getProfileList': {
                    needAuthTicket: true,
                    api: 'GetProfileList',
                    getParam: function () {
                        return {
                            'authticket': _this.mAuthTicket
                        };
                    },
                    handleData: function (data) {
                        var i;
                        for (i = 0; i < data.length; i += 1) {
                            var profileInfo = data[i];
                            if (profileInfo.UID === _this.mDeviceInfo.DefaultUID) {
                                profileInfo.isDefault = true;
                            }
                        }
                        _this.mProfiles = data;
                    }
                },
                'setActiveProfile': {
                    needAuthTicket: true,
                    needUserParam: ['deviceId, userId, pinCode'],
                    api: 'LoginProfile',
                    getParam: function (userParam) {
                        return {
                            'did': userParam.deviceId,
                            'uid': userParam.userId,
                            'pincode': userParam.pinCode
                        };
                    },
                    handleData: function (data) {
                        _this.mAuthTicket = data.AuthTicket;
                    }
                },
                'getActiveProfile': {
                    needAuthTicket: true,
                    api: 'GetProfileInfo',
                    getParam: this.fnGetCommonAuthParam,
                    handleData: function (data) {
                        _this.mActiveProfile = data;
                        _this.mPrivacyAccepted = data.AcceptPrivacy;
                    }
                },
                'getDeviceInfo': {
                    needAuthTicket: true,
                    api: 'GetDeviceInfo',
                    getParam: this.fnGetCommonAuthParam,
                    handleData: function (data) {
                        _this.mDeviceInfo = data;
                    }
                },
                // TODO: GetCurrentRegionInfo
                'getRegistrationCode': {
                    needAuthTicket: true,
                    api: 'GetRegCode',
                    getParam: this.fnGetCommonAuthParam,
                    handleData: function (data) {
                        _this.mRegistrationCode = data;
                    }
                },
                // TODO: UpdateUsingCountry
                'getApplicationUrl': {
                    needAuthTicket: true,
                    needUserParam: ['applicationId'],
                    api: 'RunApp',
                    getParam: function (userParam) {
                        return {
                            'id': userParam.applicationId,
                            'authticket': _this.mAuthTicket
                        };
                    }
                },
                'deleteApplication': {
                    needAuthTicket: true,
                    needUserParam: ['applicationId'],
                    api: 'DeleteDeviceApp',
                    getParam: function (userParam) {
                        return {
                            'id': userParam.applicationId,
                            'authticket': _this.mAuthTicket
                        };
                    }
                },
                'updateApplicationList': {
                    needAuthTicket: true,
                    needUserParam: ['applicationList'],
                    api: 'UploadDeviceApp',
                    getParam: function (aUserParam) {
                        return {
                            'authticket': _this.mAuthTicket
                        };
                    },
                    getPostMessage: function (aUserParam) {
                        var i, list = [], appInfo;
                        hx.log('dsrvTVPortal', 'updateApplicationList len=' + aUserParam.applicationList.length);
                        for (i = 0; i < aUserParam.applicationList.length; i += 1) {
                            appInfo = aUserParam.applicationList[i];
                            if (appInfo.appid) {
                                hx.log('dsrvTVPortal', 'updateApplicationList "' + appInfo.name + '"');
                                list.push({
                                    'AppID': appInfo.appid,
                                    'PackageName': appInfo.name,
                                    'Version': (appInfo.widgetDescriptor) ? appInfo.widgetDescriptor.version : appInfo.appInfo,
                                    'HitCount': 0,
                                    'LastHitDate': '',
                                    'CreateDate': '',
                                    'ModDate': '',
                                    'InstallUID': ''
                                });
                            }
                        }
                        appInfo = null;
                        aUserParam.applicationList = [];
                        return JSON.stringify(list);
                    }
                },
                'getFeaturedAppList': {
                    needAuthTicket: true,
                    api: 'getModelAppsList',
                    getParam: function () {
                        return _this.fnGetCommonAvailAppParam('FEATUREDAPP');
                    },
                    handleData: function (data) {
                        _this.mAppList.featured = data;
                    }
                },
                'getAvailableAppList': {
                    needAuthTicket: true,
                    api: 'GetModelAppsList',
                    getParam: function () {
                        return _this.fnGetCommonAvailAppParam('MODELONLYAPP');
                    },
                    handleData: function (data) {
                        _this.mAppList.available = data;
                    }
                },
                'getOttAppList': {
                    needAuthTicket: true,
                    api: 'GetModelAppsList',
                    getParam: function () {
                        return _this.fnGetCommonAvailAppParam('OTTLANUNCHER');
                    },
                    handleData: function (data) {
                        _this.mAppList.ott = data;
                    }
                },
                'getDefaultAppList': {
                    needAuthTicket: true,
                    api: 'GetModelAppsList',
                    getParam: function () {
                        return _this.fnGetCommonAvailAppParam('DEFAULTAPP');
                    },
                    handleData: function (data) {
                        _this.mAppList.def = data;
                    }
                },
                'getPreInstalledAppList': {
                    needAuthTicket: true,
                    api: 'GetModelAppsList',
                    getParam: function () {
                        return _this.fnGetCommonAvailAppParam('PREINSTALL');
                    },
                    handleData: function (data) {
                        _this.mAppList.pre = data;
                    }
                },
                'getInstalledAppList': {
                    needAuthTicket: true,
                    api: 'GetMyAppsList',
                    getParam: this.fnGetCommonAuthParam,
                    handleData: function (data) {
                        var i, len;
                        for (i = 0, len = data.length; i < len; i += 1) {
                            hx.log('dsrvTVPortal', 'getInstalledAppList: [' + data[i].name + ']');
                        }
                        _this.mAppList.installed = data;
                    }
                },
                'getSmartSearchPlayers': {
                    needAuthTicket: true,
                    api: 'GetModelAppsList',
                    getParam: function () {
                        return _this.fnGetCommonAvailAppParam('STBLAUNCHER');
                    },
                    handleData: function (data) {
                        var i, len;
                        for (i = 0, len = data.length; i < len; i += 1) {
                            hx.log('dsrvTVPortal', 'getModelAppList: [' + data[i].name + ']');
                        }
                    }
                },
                'getSmartSearchAppList': {
                    needAuthTicket: true,
                    api: 'GetModelAppsList',
                    getParam: this.fnGetCommonAuthParam,
                    handleData: function (data) {
                        _this.mSmartSearchAppList = data;
                    }
                },
                'setStatisticsLog': {
                    needAuthTicket: true,
                    api: 'UploadStatistics',
                    getParam: this.fnGetCommonAuthParam,
                    getPostMessage: function (userParam) {
                        return JSON.stringify(userParam.statisticsLog);
                    }
                },
                'findProgrammes': {
                    needAuthTicket: true,
                    getParam: function (userParam) {
                        var url = '';
                        var authticket = '';
                        var text = '?authticket=';
                        var index = userParam.url.indexOf(text);
                        if (index > -1) {
                            url = userParam.url.substring(0, index);
                            authticket = userParam.url.substring(index + text.length, userParam.url.length);
                        }
                        return {
                            'url': url,
                            'authticket': authticket,
                            'searchText': userParam.searchText,
                            'offset': userParam.offset,
                            'limit': userParam.limit
                        };
                    }
                },
                'putCrashLog': {
                    api: 'UploadCrashLog',
                    getParam: function () {
                        var now = new Date();
                        var time = Math.floor(now.getTime() / 1000);
                        return {
                            'crashDate': time,
                            'deviceID': _this.mSystemInfo.wiredMacAddress,
                            'SystemID': _this.mSystemInfo.system_id,
                            'SWVer': _this.mSoftwareVersion,
                            'country': _this.mConfigurationOipfObj.countryId
                        };
                    },
                    putData: function () {
                        return _this.mLocalSystemOipfObj.hcrdata;
                    }
                },
                'putExceptionLog': {
                    api: 'UploadCrashLog',
                    getParam: function () {
                        return {
                            'crashDate': Math.floor(Date.now() / 1000),
                            'deviceID': _this.mSystemInfo.wiredMacAddress,
                            'SystemID': _this.mSystemInfo.system_id,
                            'SWVer': _this.mSoftwareVersion,
                            'country': _this.mConfigurationOipfObj.countryId
                        };
                    },
                    putData: function (userData) {
                        return userData;
                    }
                },
                'checkNewSoftware': {
                    getParam: function () {
                        var systemId = _this.mSystemInfo.system_id;
                        var ipOtaUrl = hx.svc.SETTING_UTIL.getIpOtaUrl();
                        return {
                            'dataType': 'xml',
                            'url': ipOtaUrl + '/SWUpdate/CheckNewSW',
                            'SystemID': systemId,
                            'SWVersion': _this.mSoftwareVersion,
                            'bForced': true
                        };
                    }
                },
                'getCurrentRegionInfo': {
                    api: 'GetCurrentRegionInfo',
                    getParam: function () {
                        return {};
                    },
                    handleData: function (data) {
                        _this.mRegionInfo.ip = data.ip;
                        _this.mRegionInfo.country = data.country;
                        _this.mRegionInfo.countryname = data.countryname;
                        _this.mRegionInfo.city = data.city;
                    }
                },
                'getCheckUploadStatistics': {
                    api: 'CheckUploadStatistics',
                    getParam: function (userParam) {
                        return {
                            'authticket': _this.mAuthTicket
                        };
                    },
                    handleData: function (data) {
                    }
                },
                'getRecommendations': {
                    needAuthTicket: true,
                    api: 'GetIPEPGRecommend',
                    getParam: function (userParam) {
                        return {
                            'deviceID': _this.mSystemInfo.wiredMacAddress,
                            'type': userParam.type,
                            'repeat': userParam.repeat
                        };
                    },
                    handleData: function (data) {
                        //var i : number, len : number;
                    }
                },
            };
            hx.logadd('dsrvTVPortal');
            this.defineEvents(['privacyPolicyChange', 'TermsAndConditionsUpdated']);
            this.require('APP_MGR', function (dsrvAppMgr) {
                _this.mDsrvAppMgr = dsrvAppMgr;
            });
            this.require('SETTING_UTIL', function (dsrvSettingUtil) {
                dsrvSettingUtil.addEventCb('LanguageChanged', _this.p, function (languageInfo) {
                    hx.log('dsrvTVPortal', 'onLanguageChanged"' + languageInfo.LangMenu + '"');
                    _this.fnSetLanguage(languageInfo.LangMenu);
                });
                dsrvSettingUtil.addEventCb('NetworkAvailableChanged', _this.p, function (networkAvailable) {
                    if (!networkAvailable) {
                        _this.mLogged = false;
                        _this.mRespLogIn = null;
                        _this.mInitialized = false;
                    }
                    else {
                        _this.fnExecutePrimitive('getCurrentRegionInfo');
                        hx.log('dsrvTVPortal', 'mRegionInfo.ip: ' + _this.mRegionInfo.ip);
                    }
                });
            });
            this.addEventCb('privacyPolicyChange', this, function (policyValue) {
                hx.log('dsrvTVPortal', 'privacyPolicyChange policyValue : ' + policyValue);
                if (!policyValue) {
                    hx.svc.SETTING_UTIL.setEnableRemoteRec(false);
                    hx.log('dsrvTVPortal', 'privacyPolicyChange : set enableRemoteRecord to false');
                }
            });
            window.addEventListener('error', function (exception) {
                if (_this.isPrivacyPolicyAccepted()) {
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
                    }
                    else if (exception instanceof Error) {
                        ret.message = exception.message;
                        ret.stack = exception.stack;
                    }
                    else {
                        ret.message = exception;
                    }
                    hx.log('warn', 'exception!!');
                    hx.log('warn', 'exception!!');
                    hx.log('warn', 'exception!!');
                    hx.log('warn', 'exception!!');
                    _this.putExceptionLogData(JSON.stringify(ret));
                }
            });
        }
        /**********************************************************
         * Utilities
         **********************************************************/
        cDLTV_PORTAL.prototype.fnRequest = function (aType, aApiName, aParam, aFn, aFnError, aRetryCount) {
            var that = this;
            if (!hx.svc.SETTING_UTIL.getInternetAvailable()) {
                hx.log('dsrvTVPortal', 'cannot make a request cos network(internet) is not available');
                if (aFnError) {
                    aFnError(1 /* KErrUnKnown */);
                }
                return;
            }
            if (this.mDisableHumaxServer) {
                hx.log('dsrvTVPortal', 'Humax Portal is disabled for TEST');
                if (aFnError) {
                    aFnError(1 /* KErrUnKnown */);
                }
                return;
            }
            if (aParam && aParam.data && aParam.data.bForced) {
                hx.log('debug', '[fnRequest] bypass!! aParam.data.bForced : ' + aApiName);
                delete aParam.data.bForced;
            }
            else if (!that.isPrivacyPolicyAccepted()) {
                hx.log('debug', '[fnRequest] FAIL!! cos !that.isPrivacyPolicyAccepted()');
                if (aFnError) {
                    aFnError(1 /* KErrUnKnown */);
                }
                return;
            }
            aRetryCount = aRetryCount || 1;
            var urlString = (!!aParam.data.url) ? aParam.data.url : this.mPortalUrl + '/api/' + aApiName;
            hx.log('dsrvTVPortal', 'init: serialNumber: ' + this.mSystemInfo.serialNumber + ' urlString ' + urlString);
            var dataTypeString = aParam.data.dataType ? aParam.data.dataType : 'json';
            var data = {}, i;
            if ((aType === 'post' && aParam.urlParam) || (aType === 'put' && aParam.urlParam)) {
                urlString += '?' + $.param(aParam.urlParam);
            }
            if (aType === 'post') {
                data = aParam.data;
            }
            else if (aType === 'put') {
                dataTypeString = 'text';
                data = aParam.data;
            }
            else {
                for (i in aParam.data) {
                    if (aParam.data.hasOwnProperty(i)) {
                        data[i] = aParam.data[i];
                    }
                }
            }
            delete data['url'];
            var settings = {
                type: aType,
                url: urlString,
                data: data,
                dataType: dataTypeString,
                async: true,
                timeout: 10000,
                success: aFn,
                error: function (jqXHR, textStatus, errorThrown) {
                    if (textStatus === 'timeout' && aRetryCount < 3) {
                        hx.log('error', '[ajax]Timed out retry ' + aRetryCount);
                        that.fnRequest(aType, aApiName, aParam, aFn, aFnError, aRetryCount + 1);
                    }
                    else {
                        var errorCode = 1 /* KErrUnKnown */;
                        if (aFnError) {
                            if (textStatus === 'timeout') {
                                errorCode = 4 /* KErrTimedOut */;
                            }
                            else if (textStatus === 'abort') {
                                errorCode = 2 /* KErrAborted */;
                            }
                            else if (textStatus === 'parsererror') {
                                errorCode = 3 /* KErrParseError */;
                            }
                            else if (jqXHR.status === 401) {
                                errorCode = 10 /* KErrNotAuthorized */;
                            }
                            else if (jqXHR.status === 403) {
                                errorCode = 11 /* KErrNotAvailable */;
                            }
                            else if (jqXHR.status >= 500) {
                                errorCode = 12 /* KErrServerFail */;
                            }
                            hx.log('dsrvTVPortal', 'jqXHR.status=' + jqXHR.status + ' textStatus=' + textStatus);
                            that.mLastErrorCode = errorCode;
                            aFnError(errorCode);
                        }
                        else {
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
        };
        cDLTV_PORTAL.prototype.fnExecutePrimitive = function (aPrimitive, aFn, aFnError, aUserParam) {
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
                var i, userParamKeys = info.needUserParam;
                for (i = 0; i < info.needUserParam; i += 1) {
                    var key = userParamKeys[i];
                    if (aUserParam[key] === undefined) {
                        throw '"' + aPrimitive + '" requires "' + userParamKeys.join(',') + '"';
                    }
                }
            }
            if (info.getPostMessage) {
                this.fnRequest('post', info.api, {
                    urlParam: info.getParam(aUserParam),
                    data: info.getPostMessage(aUserParam)
                }, function (data) {
                    hx.log('dsrvTVPortal', 'Executing "' + aPrimitive + '" OK');
                    if (info.handleData) {
                        info.handleData(data, aUserParam);
                    }
                    if (aFn) {
                        aFn(data);
                    }
                }, function (errorCode) {
                    hx.log('error', 'Executing "' + aPrimitive + '" FAILED WITH: ' + errorCode);
                    if (aFnError) {
                        aFnError();
                    }
                });
            }
            else if (info.putData) {
                this.fnRequest('put', info.api, {
                    urlParam: info.getParam(aUserParam),
                    data: info.putData(aUserParam)
                }, function (data) {
                    hx.log('dsrvTVPortal', 'Executing "' + aPrimitive + '" OK');
                    if (info.handleData) {
                        info.handleData(data, aUserParam);
                    }
                    if (aFn) {
                        aFn(data);
                    }
                }, function (errorCode) {
                    hx.log('error', 'Executing "' + aPrimitive + '" FAILED WITH: ' + errorCode);
                    if (aFnError) {
                        aFnError();
                    }
                });
            }
            else {
                this.fnRequest('get', info.api, {
                    data: info.getParam(aUserParam)
                }, function (data) {
                    hx.log('dsrvTVPortal', 'Executing "' + aPrimitive + '" OK');
                    if (info.handleData) {
                        info.handleData(data, aUserParam);
                    }
                    if (aFn) {
                        aFn(data);
                    }
                }, function (errorCode) {
                    hx.log('error', 'Executing "' + aPrimitive + '" FAILED WITH: ' + errorCode);
                    if (aFnError) {
                        aFnError();
                    }
                });
            }
        };
        cDLTV_PORTAL.prototype.fnExecutePrimitives = function (aPrimitives, aFn, aFnError, aParam) {
            var _this = this;
            if (aPrimitives.length) {
                var p = aPrimitives.shift();
                this.fnExecutePrimitive(p, function () {
                    _this.fnExecutePrimitives(aPrimitives, aFn, aFnError, aParam);
                }, aFnError, aParam);
            }
            else {
                if (aFn) {
                    aFn();
                }
            }
        };
        cDLTV_PORTAL.prototype.fnInstallApps = function (aDownloadAppInfo, aFnInstallFinished) {
            var that = this;
            var i, installed;
            var appInfo = aDownloadAppInfo.shift();
            if (appInfo) {
                installed = hx.svc.APP_MGR.isWidgetInstalled({
                    id: appInfo.appid
                });
                if (!installed) {
                    hx.log('dsrvTVPortal', 'Auto installing "' + appInfo.name + '"');
                    hx.svc.APP_MGR.installWidget({
                        url: appInfo.downloadurl,
                        onCompleted: function (success) {
                            hx.log('dsrvTVPortal', 'Install finished "' + appInfo.name + '" result=' + success);
                            that.fnInstallApps(aDownloadAppInfo, aFnInstallFinished);
                        }
                    });
                }
                else {
                    hx.log('dsrvTVPortal', 'Already installed "' + appInfo.name + '"');
                    that.fnInstallApps(aDownloadAppInfo, aFnInstallFinished);
                }
            }
            else {
                aFnInstallFinished();
            }
        };
        /**********************************************************
         * Functions
         **********************************************************/
        cDLTV_PORTAL.prototype.fnInit = function (aFn, aFnError) {
            var that = this;
            this.require('SETTING_UTIL', function (dsrvSettingUtil) {
                var portalUrl = dsrvSettingUtil.getTVPortalUrl();
                var systemInfo = dsrvSettingUtil.getSystemInfo();
                var languageInfo = dsrvSettingUtil.getLanguage();
                hx.log('dsrvTVPortal', 'init: URL: ' + portalUrl);
                hx.log('dsrvTVPortal', 'init: CID : ' + systemInfo.chipId);
                hx.log('dsrvTVPortal', 'init: MAC: ' + systemInfo.wiredMacAddress);
                hx.log('dsrvTVPortal', 'init: LAN: ' + languageInfo.LangMenu);
                if (that.mInitialized) {
                    that.fnGetActiveProfile(function () {
                        hx.log('dsrvTVPortal', 'GetActiveProfile succeed');
                        aFn(true);
                    }, function () {
                        hx.log('error', 'GetActiveProfile Failed');
                        aFn(false);
                    });
                }
                else {
                    that.fnInitSecond(portalUrl, systemInfo, languageInfo.LangMenu, function () {
                        hx.log('dsrvTVPortal', 'create: init completed');
                        aFn(true);
                    }, function () {
                        hx.log('error', 'create: init failed');
                        aFn(false);
                    });
                }
            });
        };
        cDLTV_PORTAL.prototype.fnInitSecond = function (aPortalUrl, aSystemInfo, aLanguage, aFn, aFnError) {
            var that = this;
            this.mPortalUrl = aPortalUrl;
            this.mSystemInfo = aSystemInfo;
            this.fnSetLanguage(aLanguage, function () {
                that.fnExecutePrimitives([
                    'logIn',
                    'getDeviceInfo',
                    'getDefaultAppList',
                    'getPreInstalledAppList',
                    'getInstalledAppList',
                    'getSmartSearchAppList'
                ], function () {
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
        };
        cDLTV_PORTAL.prototype.fnLogin = function (aFn, aFnError, aParam) {
            var that = this;
            if (that.mLogged) {
                hx.log('debug', 'already logged in!!! no need to log in again!');
                if (aFn) {
                    aFn(that.mRespLogIn);
                }
                return;
            }
            that.mLoginQueue.push({ fn: aFn, fnError: aFnError });
            if (that.mLogging) {
                hx.log('debug', 'on logging in !!! callbacks will be executed after it.');
                return;
            }
            that.mLogging = true;
            var todo;
            this.require('SETTING_UTIL', function (dsrvSettingUtil) {
                that.mPortalUrl = dsrvSettingUtil.getTVPortalUrl();
                that.mSystemInfo = dsrvSettingUtil.getSystemInfo();
                that.fnExecutePrimitive('logIn', function () {
                    that.mLogging = false;
                    that.mLogged = true;
                    if (hx.svc.SETTING_UTIL.getInitHumaxTvPortal()) {
                        hx.log('dsrvTVPortal', 'HumaxTvPortal needs to be initialized');
                        that.fnDeviceReset(function () {
                            that.fnExecutePrimitive('acceptPrivacyTerms', null, null, {
                                'privacy': that.mPrivacyAccepted,
                                'terms': that.mTermsAccepted
                            });
                        }, null, aParam);
                        hx.svc.SETTING_UTIL.setInitHumaxTvPortal(false);
                    }
                    else {
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
                }, function () {
                    that.mLogging = false;
                    hx.svc.IP_EPG.setEnableHttp(that.isTermsAndConditionsAccepted());
                    while (that.mLoginQueue.length) {
                        todo = that.mLoginQueue.pop();
                        if (todo && todo.fnError) {
                            todo.fn({
                                AcceptPrivacy: that.isPrivacyPolicyAccepted(),
                                AcceptTerms: that.isTermsAndConditionsAccepted()
                            });
                        }
                    }
                }, aParam);
            });
        };
        cDLTV_PORTAL.prototype.fnLogout = function (aFn, aFnError) {
            var that = this;
            if (!that.mLogged) {
                hx.log('debug', 'already logged out!!! no need to log out again!');
                if (aFn) {
                    aFn(that.mRespLogIn);
                }
                return;
            }
            that.fnExecutePrimitive('logOut', function () {
                that.mLogged = false;
                that.mInitialized = false;
                if (aFn) {
                    aFn();
                }
            }, function () {
                if (aFnError) {
                    aFnError({
                        AcceptPrivacy: that.isPrivacyPolicyAccepted(),
                        AcceptTerms: that.isTermsAndConditionsAccepted()
                    });
                }
            });
        };
        cDLTV_PORTAL.prototype.fnDeviceReset = function (aFn, aFnError, aParam) {
            var that = this;
            this.require('SETTING_UTIL', function (dsrvSettingUtil) {
                that.mPortalUrl = dsrvSettingUtil.getTVPortalUrl();
                that.mSystemInfo = dsrvSettingUtil.getSystemInfo();
                that.fnExecutePrimitive('deviceReset', function () {
                    hx.svc.IP_EPG.setEnableHttp(that.mTermsAccepted);
                    if (aFn) {
                        aFn();
                    }
                }, aFnError, aParam);
            });
        };
        cDLTV_PORTAL.prototype.fnSetLanguage = function (aLanguage, aFn, aFnError) {
            // http://technet.microsoft.com/ko-kr/library/dd744369(v=ws.10).aspx
            var langTable = {
                "alb": "sq",
                "ara": "ar",
                "aus": null,
                "baq": "eu-ES",
                "bas": null,
                "bul": "bg",
                "cat": null,
                "cze": "cs",
                "dan": "da",
                "dut": "nl",
                "eng": "en",
                "est": "et",
                "fao": null,
                "fin": "fi",
                "fre": "fr",
                "ger": "de",
                "gla": null,
                "gle": "ga",
                "glg": "gl",
                "gre": "el",
                "heb": "he",
                "hin": "hi",
                "hun": "hu",
                "ice": "is",
                "ita": "it",
                "jpn": "ja",
                "kor": "ko",
                "lav": "lv",
                "lit": "lt",
                //,"nar" //Audio Description, eLangID_Narration
                "nor": "nb",
                "per": null,
                "pol": "pl",
                "por": "pt",
                //,"qaa" //Original Language, eLangID_OriginalAudio
                "rum": "ro",
                "rus": "ru",
                "scr": "hr",
                "slo": "sk",
                "slv": "sl",
                "spa": "es",
                "srp": "sr",
                "swe": "sv",
                "tha": "th",
                "tur": "tr",
                "ukr": "uk",
                //,"und" //Undefine,  eLangID_Undefined
                "vie": "vi",
                "wel": null //Welsh, eLangID_Welsh
            };
            this.mLanguage = langTable[aLanguage] || 'en';
            hx.log('dsrvTVPortal', 'Language Code: "' + this.mLanguage + '"');
            this.fnExecutePrimitives(['getPrivacyVersion', 'getPrivacy', 'getTermsVersion', 'getTermsAndConditions'], function () {
                if (aFn) {
                    aFn();
                }
            }, aFnError, {
                bForced: true
            });
        };
        cDLTV_PORTAL.prototype.fnInitialized = function () {
            return this.mInitialized;
        };
        cDLTV_PORTAL.prototype.fnIsLogged = function () {
            return this.mLogged;
        };
        cDLTV_PORTAL.prototype.fnCheckPrivacyVersion = function (aFn, aFnError) {
            var _this = this;
            this.fnExecutePrimitive(['getPrivacyVersion'], function () {
                hx.log('debug', '[fnCheckPrivacyVersion] this.mNewPrivacyVersion : ' + _this.mNewPrivacyVersion);
                hx.log('debug', '[fnCheckPrivacyVersion] this.mPrivacyInfo.version : ' + _this.mPrivacyInfo.version);
                if (_this.mNewPrivacyVersion === _this.mPrivacyInfo.version) {
                    if (aFn) {
                        aFn();
                    }
                }
                else {
                    // updated.
                    _this.mPrivacyInfo = {};
                    _this.mPrivacyAccepted = false;
                    if (_this.mRespLogIn) {
                        _this.mRespLogIn.AcceptPrivacy = _this.mPrivacyAccepted;
                    }
                    hx.svc.WEB_STORAGE.setPrivacyPolicyState(_this.mPrivacyAccepted);
                    //hx.svc.WEB_STORAGE.setPrivacyPolicyText(null);
                    //hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(null);
                    if (aFnError) {
                        aFnError();
                    }
                }
            }, function () {
                // nothing
            }, {
                bForced: true
            });
        };
        cDLTV_PORTAL.prototype.fnCheckTermsVersion = function (aFn, aFnError) {
            var _this = this;
            this.fnExecutePrimitive(['getTermsVersion'], function () {
                hx.log('debug', '[fnCheckTermsVersion] this.mNewTermsVersion : ' + _this.mNewTermsVersion);
                hx.log('debug', '[fnCheckTermsVersion] this.mTermsInfo.version : ' + _this.mTermsInfo.version);
                if (_this.mNewTermsVersion === _this.mTermsInfo.version) {
                    if (aFn) {
                        aFn();
                    }
                }
                else {
                    // updated.
                    _this.mTermsInfo = {};
                    _this.mTermsAccepted = false;
                    if (_this.mRespLogIn) {
                        _this.mRespLogIn.AcceptTerms = _this.mTermsAccepted;
                    }
                    hx.svc.WEB_STORAGE.setTermsAndConditionsState(_this.mTermsAccepted);
                    if (aFnError) {
                        aFnError();
                    }
                }
            }, function () {
                // nothing
            }, {
                bForced: true
            });
        };
        cDLTV_PORTAL.prototype.fnGetPrivacyPolicyText = function (aFn, aFnError) {
            var _this = this;
            this.fnCheckPrivacyVersion(function () {
                // same version
                if (_this.mPrivacyInfo && _this.mPrivacyInfo.text) {
                    if (aFn) {
                        aFn(_this.mPrivacyInfo.text);
                    }
                }
                else {
                    _this.fnExecutePrimitive('getPrivacy', function () {
                        if (aFn) {
                            aFn(_this.mPrivacyInfo.text);
                        }
                    }, aFnError, {
                        bForced: true
                    });
                }
            }, function () {
                // new version
                _this.fnExecutePrimitive('getPrivacy', function () {
                    if (aFn) {
                        aFn(_this.mPrivacyInfo.text);
                    }
                }, aFnError, {
                    bForced: true
                });
            });
        };
        cDLTV_PORTAL.prototype.fnGetTermsAndConditionsText = function (aFn, aFnError) {
            var _this = this;
            this.fnCheckTermsVersion(function () {
                // same version
                if (_this.mTermsInfo && _this.mTermsInfo.text) {
                    if (aFn) {
                        aFn(_this.mTermsInfo.text);
                    }
                }
                else {
                    _this.fnExecutePrimitive('getTermsAndConditions', function () {
                        if (aFn) {
                            aFn(_this.mTermsInfo.text);
                        }
                    }, aFnError, {
                        bForced: true
                    });
                }
            }, function () {
                // new version
                _this.fnExecutePrimitive('getTermsAndConditions', function () {
                    if (aFn) {
                        aFn(_this.mTermsInfo.text);
                    }
                }, aFnError, {
                    bForced: true
                });
            });
        };
        cDLTV_PORTAL.prototype.fnGetProfileList = function (aFn, aFnError) {
            this.fnExecutePrimitive('getProfileList', aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.fnGetActiveProfile = function (aFn, aFnError) {
            var _this = this;
            this.fnExecutePrimitive('getActiveProfile', aFn, function () {
                if (aFnError) {
                    aFnError({
                        AcceptPrivacy: _this.isPrivacyPolicyAccepted(),
                        AcceptTerms: _this.isTermsAndConditionsAccepted()
                    });
                }
            });
        };
        cDLTV_PORTAL.prototype.fnSetActiveProfile = function (aProfileInfo, aPinCode, aFn, aFnError) {
            var that = this;
            this.fnExecutePrimitive('setActiveProfile', function () {
                that.fnExecutePrimitive('getActiveProfile', function () {
                    if (aProfileInfo.UID === that.mActiveProfile.UID) {
                        that.fnExecutePrimitives(['getInstalledAppList', 'getSmartSearchAppList'], aFn, aFnError);
                    }
                    else {
                        aFnError();
                    }
                }, aFnError);
            }, aFnError, {
                'deviceId': aProfileInfo.DID,
                'userId': aProfileInfo.UID,
                'pinCode': aPinCode
            });
        };
        cDLTV_PORTAL.prototype.fnGetRegistrationCode = function (aFn, aFnError) {
            this.fnExecutePrimitive('getRegistrationCode', aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.fnGetFeaturedAppList = function () {
            return this.mAppList.featured;
        };
        cDLTV_PORTAL.prototype.fnGetAvailableAppList = function () {
            return this.mAppList.available;
        };
        cDLTV_PORTAL.prototype.fnGetOttAppList = function () {
            return this.mAppList.ott;
        };
        cDLTV_PORTAL.prototype.fnGetDefaultAppList = function () {
            return this.mAppList.def;
        };
        cDLTV_PORTAL.prototype.fnGetPreInstalledAppList = function () {
            return this.mAppList.pre;
        };
        cDLTV_PORTAL.prototype.fnRequestFeaturedAppList = function (fn, fnError) {
            this.fnExecutePrimitive('getFeaturedAppList', fn, fnError);
        };
        cDLTV_PORTAL.prototype.fnRequestFavouriteAppList = function (fn, fnError) {
            this.fnExecutePrimitive('getFavoriteAppsList', fn, fnError);
        };
        cDLTV_PORTAL.prototype.fnRequestDefaultAppList = function (fn, fnError) {
            this.fnExecutePrimitive('getDefaultAppList', fn, fnError);
        };
        cDLTV_PORTAL.prototype.fnGetInstalledAppList = function (aFn, aFnError) {
            this.fnExecutePrimitive('getInstalledAppList', aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.fnGetApplicationUrl = function (aApplicationId, aFn, aFnError) {
            this.fnExecutePrimitive('getApplicationUrl', aFn, aFnError, {
                'applicationId': aApplicationId
            });
        };
        cDLTV_PORTAL.prototype.fnDeleteApplication = function (aApplicationId, aFn, aFnError) {
            this.fnExecutePrimitive('deleteApplication', aFn, aFnError, {
                'applicationId': aApplicationId
            });
        };
        cDLTV_PORTAL.prototype.fnUpdateApplicationList = function (aApplicationList, aFn, aFnError) {
            this.fnExecutePrimitive('updateApplicationList', aFn, aFnError, {
                'applicationList': aApplicationList
            });
        };
        cDLTV_PORTAL.prototype.fnAcceptPrivacyPolicy = function (aAccept, aFn, aFnError) {
            var _this = this;
            var prePrivacyAccepted = this.mPrivacyAccepted;
            if (aAccept) {
                this.fnLogin(function () {
                    _this.mPrivacyAccepted = aAccept;
                    _this.mPrivacyInfo.version = _this.mNewPrivacyVersion;
                    hx.svc.WEB_STORAGE.setPrivacyPolicyState(aAccept);
                    hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(_this.mNewPrivacyVersion);
                    hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(_this.mNewPrivacyVersion);
                    _this.fnExecutePrimitive('acceptPrivacyPolicy', aFn, aFnError, {
                        'agree': aAccept
                    });
                }, function () {
                    _this.mPrivacyAccepted = aAccept;
                    hx.svc.WEB_STORAGE.setPrivacyPolicyState(aAccept);
                    hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(null);
                    if (aFn) {
                        aFn();
                    }
                }, {
                    bForced: true
                });
            }
            else {
                this.fnExecutePrimitive('acceptPrivacyPolicy', aFn, aFnError, {
                    'agree': aAccept
                });
                this.fnLogout(function () {
                }, function () {
                });
                this.mPrivacyAccepted = aAccept;
                this.mPrivacyInfo.version = this.mNewPrivacyVersion;
                hx.svc.WEB_STORAGE.setPrivacyPolicyState(aAccept);
                hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(this.mNewPrivacyVersion);
                hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(null);
            }
            if (prePrivacyAccepted !== aAccept) {
                this.fireEvent('privacyPolicyChange', aAccept);
            }
        };
        cDLTV_PORTAL.prototype.fnAcceptTermsAndConditions = function (aAccept, aFn, aFnError) {
            this.fnExecutePrimitive('acceptTermsAndConditions', aFn, aFnError, {
                'agree': aAccept
            });
            this.mTermsAccepted = aAccept;
            this.mTermsInfo.version = this.mNewTermsVersion;
            hx.svc.WEB_STORAGE.setTermsAndConditionsState(aAccept);
            hx.svc.WEB_STORAGE.setTermsAndConditionsVersion(this.mNewTermsVersion);
        };
        cDLTV_PORTAL.prototype.fnAcceptPrivacyTerms = function (aPrivacyAccept, aTermsAccept, aFn, aFnError) {
            var _this = this;
            var prePrivacyAccepted = this.mPrivacyAccepted;
            if (aPrivacyAccept) {
                this.fnLogin(function () {
                    _this.mPrivacyAccepted = aPrivacyAccept;
                    _this.mPrivacyInfo.version = _this.mNewPrivacyVersion;
                    hx.svc.WEB_STORAGE.setPrivacyPolicyState(aPrivacyAccept);
                    hx.svc.WEB_STORAGE.setPrivacyPolicyVersion(_this.mNewPrivacyVersion);
                    hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(_this.mNewPrivacyVersion);
                    _this.fnExecutePrimitive('acceptPrivacyTerms', aFn, aFnError, {
                        'privacy': aPrivacyAccept,
                        'terms': aTermsAccept
                    });
                }, function () {
                    _this.mPrivacyAccepted = aPrivacyAccept;
                    hx.svc.WEB_STORAGE.setPrivacyPolicyState(aPrivacyAccept);
                    hx.svc.SETTING_UTIL.setPrivacyPolicyVersion(null);
                    if (aFn) {
                        aFn();
                    }
                }, {
                    bForced: true
                });
            }
            else {
                this.fnExecutePrimitive('acceptPrivacyTerms', aFn, aFnError, {
                    'privacy': aPrivacyAccept,
                    'terms': aTermsAccept
                });
                this.fnLogout(function () {
                }, function () {
                });
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
        };
        cDLTV_PORTAL.prototype.fnGetSmartSearchAppList = function (aFn, aFnError) {
            this.fnExecutePrimitive('getSmartSearchAppList', aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.fnGetSmartSearchPlayers = function (aFn, aFnError) {
            this.fnExecutePrimitive('getSmartSearchPlayers', aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.fnSetStatisticsLog = function (aStatisticsLog, aFn, aFnError) {
            this.fnExecutePrimitive('setStatisticsLog', aFn, aFnError, {
                'statisticsLog': aStatisticsLog
            });
        };
        cDLTV_PORTAL.prototype.fnFindProgrammes = function (aParam, aFn, aFnError) {
            this.fnExecutePrimitive('findProgrammes', aFn, aFnError, {
                'url': aParam.url,
                'searchText': aParam.searchText,
                'offset': aParam.offset,
                'limit': aParam.limit
            });
        };
        cDLTV_PORTAL.prototype.fnGetLastErrorCode = function () {
            return this.mLastErrorCode;
        };
        cDLTV_PORTAL.prototype.fnCreateWidget = function (aWd, aUrl) {
            var that = this;
            hx.log('dsrvTVPortal', 'fnCreateWidget');
            hx.svc.AV_CTRL.blockAV({
                cb: function () {
                    that.mDsrvAppMgr.createWidget({
                        name: aWd.name,
                        wd: aWd,
                        url: aUrl,
                        onLoaded: function (appInfo) {
                            that.mActiveAppInfo = appInfo;
                            hx.log('dsrvTVPortal', 'fnCreateWidget.onLoaded');
                            hx.svc.SETTING_UTIL.setFrontPanelString({
                                text: '-TV APPS-'
                            });
                        },
                        onKeyDown: function (appInfo, keyCode) {
                            hx.log('dsrvTVPortal', 'fnCreateWidget.onKeyDown: ' + keyCode);
                            if (that.mActiveAppInfo && keyCode === hx.key.KEY_ESCAPE) {
                                that.mDsrvAppMgr.destroyWidget(appInfo);
                            }
                        },
                        onUnloaded: function (appInfo) {
                            that.mActiveAppInfo = null;
                            hx.svc.AV_CTRL.unBlockAV();
                            var success = hx.svc.AV_CTRL.lastChannel();
                            if (!success) {
                                hx.svc.AV_CTRL.availableChannel();
                            }
                            hx.svc.SETTING_UTIL.setFrontPanelString({
                                text: ''
                            });
                        }
                    });
                }
            });
        };
        cDLTV_PORTAL.prototype.fnLaunchApp = function (aWidgetId, aFnError) {
            var that = this;
            hx.log('dsrvTVPortal', 'fnLaunchApp');
            var wd = this.mDsrvAppMgr.getWidgetDescriptor({
                widgetId: aWidgetId
            });
            if (aWidgetId.indexOf('linked://') === 0) {
                var appId = aWidgetId.substr(9, aWidgetId.length - 9);
                this.fnGetApplicationUrl(appId, function (result) {
                    if (result.Status) {
                        that.fnCreateWidget(wd, result.Url);
                    }
                    else {
                        aFnError(result);
                    }
                });
            }
            else {
                that.fnCreateWidget(wd);
            }
        };
        cDLTV_PORTAL.prototype.fnDestroyActiveApp = function () {
            hx.log('dsrvTVPortal', 'fnDestroyActiveApp');
            if (this.mActiveAppInfo) {
                this.mDsrvAppMgr.destroyWidget(this.mActiveAppInfo);
            }
        };
        cDLTV_PORTAL.prototype.fnPutCrashLogData = function () {
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
        };
        cDLTV_PORTAL.prototype.fnCheckNewSoftware = function (aFn, aFnError) {
            hx.log('dsrvTVPortal', 'fnCheckNewSoftware');
            this.fnExecutePrimitive('checkNewSoftware', aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.fnGetCurrentRegionInfo = function (fn, fnError) {
            hx.log('dsrvTVPortal', 'fnGetCurrentRegionInfo');
            this.fnExecutePrimitive('getCurrentRegionInfo', fn, fnError);
            hx.log('dsrvTVPortal', 'mRegionInfo.ip: ' + this.mRegionInfo.ip);
        };
        cDLTV_PORTAL.prototype.fnGetCheckUploadStatistics = function (fn, fnError) {
            hx.log('dsrvTVPortal', 'fnGetCheckUploadStatistics');
            this.fnExecutePrimitive('getCheckUploadStatistics', fn, fnError);
        };
        cDLTV_PORTAL.prototype.fnGetRecommendations = function (aFn, aFnError, aUserParam) {
            hx.log('dsrvTVPortal', 'fnGetRecommendations');
            this.fnExecutePrimitive('getRecommendations', aFn, aFnError, aUserParam);
        };
        cDLTV_PORTAL.prototype.init = function (aParam) {
            this.fnInit(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.login = function (aParam) {
            this.fnLogin(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.deviceReset = function (aParam) {
            this.fnDeviceReset(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.setLanguage = function (aParam) {
            this.fnSetLanguage(aParam.language, aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.isInitialized = function () {
            return this.fnInitialized();
        };
        cDLTV_PORTAL.prototype.isLogged = function () {
            return this.fnIsLogged();
        };
        cDLTV_PORTAL.prototype.checkPrivacyVersion = function (aFn, aFnError) {
            return this.fnCheckPrivacyVersion(aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.getPrivacyPolicyText = function (aFn, aFnError) {
            return this.fnGetPrivacyPolicyText(aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.getTermsAndConditionsText = function (aFn, aFnError) {
            return this.fnGetTermsAndConditionsText(aFn, aFnError);
        };
        cDLTV_PORTAL.prototype.getProfileList = function (aParam) {
            this.fnGetProfileList(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.getActiveProfile = function (aParam) {
            this.fnGetActiveProfile(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.setActiveProfile = function (aParam) {
            this.fnSetActiveProfile(aParam.profileInfo, aParam.pinCode, aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.getRegistrationCode = function (aParam) {
            this.fnGetRegistrationCode(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.getFeaturedAppList = function (aParam) {
            if (this.fnGetFeaturedAppList() && aParam.fn) {
                return aParam.fn(this.fnGetFeaturedAppList());
            }
            else {
                this.fnRequestFeaturedAppList(aParam.fn, aParam.fnError);
            }
        };
        cDLTV_PORTAL.prototype.getAvailableAppList = function () {
            return this.fnGetAvailableAppList();
        };
        cDLTV_PORTAL.prototype.getOttAppList = function () {
            return this.fnGetOttAppList();
        };
        cDLTV_PORTAL.prototype.getDefaultAppList = function (aParam) {
            return this.fnRequestDefaultAppList(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.getPreInstalledAppList = function () {
            return this.fnGetPreInstalledAppList();
        };
        cDLTV_PORTAL.prototype.getInstalledAppList = function (aParam) {
            this.fnGetInstalledAppList(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.getApplicationUrl = function (aParam) {
            this.fnGetApplicationUrl(aParam.applicationId, aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.deleteApplication = function (aParam) {
            this.fnDeleteApplication(aParam.applicationId, aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.updateApplicationList = function (aParam) {
            this.fnUpdateApplicationList(aParam.applicationList, aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.acceptPrivacyPolicy = function (aParam) {
            this.fnAcceptPrivacyPolicy(aParam.accept, aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.acceptTermsAndConditions = function (aParam) {
            var self = this;
            var preTermsAccepted = self.mTermsAccepted;
            var fn = function (data) {
                if (preTermsAccepted !== aParam.accept) {
                    hx.svc.IP_EPG.setEnableHttp(aParam.accept).then(function () {
                        if (aParam.fn) {
                            aParam.fn(data);
                        }
                    }, function () {
                        if (aParam.fnError) {
                            aParam.fnError();
                        }
                    });
                }
                else {
                    if (aParam.fn) {
                        aParam.fn(data);
                    }
                }
            };
            this.fnAcceptTermsAndConditions(aParam.accept, fn, fn);
        };
        cDLTV_PORTAL.prototype.acceptPrivacyTerms = function (aParam) {
            var self = this;
            var preTermsAccepted = self.mTermsAccepted;
            var fn = function (data) {
                if (preTermsAccepted !== aParam.terms) {
                    hx.svc.IP_EPG.setEnableHttp(aParam.terms).then(function () {
                        if (aParam.fn) {
                            aParam.fn(data);
                        }
                    }, function () {
                        if (aParam.fnError) {
                            aParam.fnError();
                        }
                    });
                }
                else {
                    if (aParam.fn) {
                        aParam.fn(data);
                    }
                }
            };
            this.fnAcceptPrivacyTerms(aParam.privacy, aParam.terms, fn, fn);
        };
        cDLTV_PORTAL.prototype.getSmartSearchAppList = function (aParam) {
            return this.fnGetSmartSearchAppList(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.getSmartSearchPlayers = function (aParam) {
            return this.fnGetSmartSearchPlayers(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.setStatisticsLog = function (aParam) {
            return this.fnSetStatisticsLog(aParam.statisticsLog, aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.findProgrammes = function (aParam) {
            this.fnFindProgrammes(aParam.param, aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.getLastErrorCode = function () {
            return this.fnGetLastErrorCode();
        };
        cDLTV_PORTAL.prototype.launchApp = function (aParam) {
            this.fnLaunchApp(aParam.widgetId, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.destroyActiveApp = function () {
            this.fnDestroyActiveApp();
        };
        cDLTV_PORTAL.prototype.isPrivacyPolicyAccepted = function () {
            return this.mPrivacyAccepted;
        };
        cDLTV_PORTAL.prototype.isTermsAndConditionsAccepted = function () {
            return this.mTermsAccepted;
        };
        cDLTV_PORTAL.prototype.putCrashLogData = function () {
            hx.log('dsrvTVPortal', 'putCrashLogData');
            return this.fnPutCrashLogData();
        };
        cDLTV_PORTAL.prototype.putExceptionLogData = function (exceptionData) {
            this.fnExecutePrimitive('putExceptionLog', null, null, exceptionData);
        };
        cDLTV_PORTAL.prototype.checkNewSoftware = function (aParam) {
            this.fnCheckNewSoftware(aParam.fn, aParam.fnError);
        };
        cDLTV_PORTAL.prototype.getCurrentRegionInfo = function (param) {
            this.fnGetCurrentRegionInfo(param.fn, param.fnError);
        };
        cDLTV_PORTAL.prototype.getExternalIp = function () {
            return this.mRegionInfo.ip;
        };
        cDLTV_PORTAL.prototype.getCheckUploadStatistics = function (param) {
            this.fnGetCheckUploadStatistics(param.fn, param.fnError);
        };
        cDLTV_PORTAL.prototype.getRecommendations = function (aParam) {
            return this.fnGetRecommendations(aParam.fn, aParam.fnError, aParam.userParam);
        };
        cDLTV_PORTAL.prototype.getAuthTicket = function () {
            return this.mAuthTicket;
        };
        cDLTV_PORTAL.prototype.getPortalUrl = function () {
            return this.mPortalUrl;
        };
        cDLTV_PORTAL.prototype.enableHumaxServerForTest = function () {
            this.mDisableHumaxServer = false;
        };
        cDLTV_PORTAL.prototype.disableHumaxServerForTest = function () {
            this.mDisableHumaxServer = true;
        };
        cDLTV_PORTAL.prototype._updateTermsAndConditions = function (bAccepted) {
            this.mTermsAccepted = bAccepted;
            this.fireEvent('TermsAndConditionsUpdated', bAccepted);
        };
        return cDLTV_PORTAL;
    })(__KERNEL__);
    return cDLTV_PORTAL;
});
