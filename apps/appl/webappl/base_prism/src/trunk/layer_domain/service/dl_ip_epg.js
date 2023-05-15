/**
 * Created by bglee on 2015-03-28.
 */
/// <reference path="../../def/sugar.d.ts"/>
/// <reference path="ip_epg/adapter.d.ts" />
/// <reference path="ip_epg/engine/interface_freeview_play.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", './ip_epg/engine/adapter_freeview_play', './ip_epg/cache_controller', './ip_epg/http_req_filter'], function (require, exports, __KERNEL__, __ADAPTER__, __cache_controller__, __HTTP_PROXY__) {
    var _join = Array.prototype.join;
    var CDLIpEpg = (function (_super) {
        __extends(CDLIpEpg, _super);
        function CDLIpEpg(aServiceName) {
            _super.call(this, aServiceName);
            this._adapter = new __ADAPTER__(function () {
                return hx.svc.SETTING_UTIL.getPrefixUrl();
            });
            this._IhttpHeaderControl = this._adapter;
            this._cache_controller = new __cache_controller__.CCacheController([
                __HTTP_PROXY__.Category.META,
                __HTTP_PROXY__.Category.IMAGE,
                'moved',
                __HTTP_PROXY__.Category.AUTH
            ]);
            this._httpProxy = new __HTTP_PROXY__.CHttpProxyMDS(this._cache_controller, this._IhttpHeaderControl);
            this.parse = this._adapter.parse;
            this.convert = this._adapter.convert;
            this._UriMode = 'NIT'; //'NIT' or 'USB'
            this.isTermsAndConditionAccepted = false;
            this.MdsUrl = null;
            hx.logadd("mds");
            this.initEventListener();
            this.defineEvents(['getAuthToken']);
        }
        CDLIpEpg.prototype._getHeaderFromPrevRequest = function (cache, param) {
            var hLastModified = '';
            var hRetryAfter = '';
            var hIfModifiedSince = '';
            var hURL = '';
            var hAccept = '';
            var ret;
            if (cache && cache.header) {
                hLastModified = cache.header['Last-Modified'];
            }
            if (param && param['Retry-After']) {
                hRetryAfter = param['Retry-After'];
            }
            if (param && param['If-Modified-Since']) {
                hIfModifiedSince = param['If-Modified-Since'];
            }
            if (param && param['URL']) {
                hURL = param['URL'];
            }
            if (param && param['Accept']) {
                hAccept = param['Accept'];
            }
            ret = {
                'Retry-After': hRetryAfter,
                'If-Modified-Since': hIfModifiedSince || hLastModified,
                'URL': hURL,
                'Accept': hAccept
            };
            return ret;
        };
        CDLIpEpg.prototype.initEventListener = function () {
            var _this = this;
            hx.svc.SETTING_UTIL.clearEventCb(this);
            /*
            hx.svc.SETTING_UTIL.addEventCb('PowerStateChanged', this, (status: number) => {
                //Do not save data on Local Storage
                if (status === 1) {
                    this._cache_controller.setDb(JSON.parse(localStorage.getItem(__cache_controller__.CCacheController.CACHE_DB_NAME)));
                } else {
                    localStorage.setItem(__cache_controller__.CCacheController.CACHE_DB_NAME, JSON.stringify(this._cache_controller.getDb()));
                }
            });
            */
            hx.svc.TV_PORTAL.addEventCb('TermsAndConditionsUpdated', this, function (bAccepted) {
                hx.log('error', '## TermsAndConditionsUpdated bAccepted : ' + bAccepted);
                if (_this.isTermsAndConditionAccepted != bAccepted) {
                    _this.isTermsAndConditionAccepted = bAccepted;
                    _this.setEnableHttp(bAccepted);
                }
            });
            //AP Update check
            hx.svc.SETTING_UTIL.addEventCb('NetworkAvailableChanged', this, function (networkAvailable) {
                hx.log('error', '## NetworkAvailableChanged networkAvailable : ' + networkAvailable);
                if (!_this.MdsUrl) {
                    var dbURL = hx.svc.SETTING_UTIL.getPrefixUrl();
                    hx.log('error', '## NetworkAvailableChanged networkAvailable MDS URL NULL , use DB URL : ' + dbURL);
                    _this.MdsUrl = dbURL;
                }
                if (networkAvailable && _this.MdsUrl && hx.svc.TV_PORTAL.isTermsAndConditionsAccepted()) {
                    _this.getAuthToken(0, _this.MdsUrl);
                }
                else {
                    hx.log('error', '############## NetworkAvailableChanged MDS URL FAIL :  ' + _this.MdsUrl + ' , termsAndCondition : ' + hx.svc.TV_PORTAL.isTermsAndConditionsAccepted());
                }
                if (!networkAvailable) {
                    // clear prev Auth info to reset if Internet is available to use again!
                    var db = {
                        'auth': {}
                    };
                    _this._cache_controller.setDb(db);
                }
            });
            hx.svc.AV_CTRL.addEventCb('NetworkUriChanged', this, function (param) {
                var uri = '';
                if (!param) {
                    _this.MdsUrl = null;
                    hx.log('error', '################ network uri Null');
                    return;
                }
                if (hx.svc.TV_PORTAL.isTermsAndConditionsAccepted()) {
                    _this._httpProxy.setEnableProxy(true); //Do not authenticate here.
                }
                if (typeof param === 'string' && _this._UriMode == 'NIT') {
                    uri = param;
                    hx.log('error', '################ network uri changed : ' + uri + ' (prev uri : ' + _this.MdsUrl + ')');
                    if (hx.il.checkHttpUrl(uri)) {
                        if (uri != _this.MdsUrl) {
                            hx.log('debug', '################ network uri updated ##### uri != this.MdsUrl');
                            _this.getAuthToken(0, uri, true); // forced regardless of auth before
                        }
                        else {
                            _this.getAuthToken(0, uri);
                        }
                    }
                    else {
                        hx.log('error', '######### network uri need to check URI : ' + uri);
                    }
                }
                else if (typeof param != 'string') {
                    //From USB
                    uri = param['uri'];
                    if (uri && uri != _this.MdsUrl) {
                        _this.MdsUrl = uri;
                        _this._UriMode = param['mode'];
                        hx.log('error', '################ network uri changed from USB: ' + uri);
                        var db = {
                            'auth': {},
                            'meta': {},
                            'image': {},
                            'moved': {}
                        };
                        _this._cache_controller.removeAll();
                        _this._cache_controller.setDb(db);
                        _this._adapter.setBaseUrl(null);
                        _this.getAuthToken(-1, uri); //reset All cached data on HTTP and reset Auth
                    }
                }
                else if (_this._UriMode == 'USB') {
                    hx.log('error', '################ network uri changed in USB MODE : ' + param);
                }
            });
        };
        CDLIpEpg.prototype.getChannels = function (prevHeader) {
            hx.log('mds', '######### getChannels #########');
            var category = __HTTP_PROXY__.Category.META;
            var hash = _join.call(arguments, ':');
            var networkIds = hx.svc.SETTING_UTIL.getNetworkId();
            var reqParamList = [];
            var self = this;
            // [FVP – Business-to-Consumer Interface Specification v1.3] - 6.1.1 Linear Service List by Network ID
            // The client device MAY send a single request for the complete list of services on a particular network.
            // This request SHALL use a single device-determined Network ID in order to ensure that only relevant services are returned.
            // In cases where multiple Network IDs are visible to the client device, a separate request SHALL be made for each.
            // Combining the results is the responsibility of the client device and the client device MAY filter out any duplicate services and services it cannot receive or that the user has deleted.
            networkIds.forEach(function (aNetworkId) {
                var param = {};
                param.senderF = function (aParam) {
                    var cache = self._cache_controller.get(category, hash);
                    return self._adapter.getChannels(aNetworkId, self._getHeaderFromPrevRequest(cache, aParam));
                };
                param.query = hash + ':channel:' + aNetworkId;
                param.category = category;
                param.resDef = Q.defer();
                param.resDef.promise.then(function (aData) {
                    self._adapter.setChannelLogoUrls(aData);
                });
                reqParamList.push(param);
            });
            return this._httpProxy.reqToMetaList(reqParamList);
        };
        CDLIpEpg.prototype.getContentOwningServices = function (prevHeader) {
            hx.log('mds', '######### getContentOwningServices #########');
            var category = __HTTP_PROXY__.Category.META;
            var query = 'contentOwningServices';
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, query);
                return self._adapter.getContentOwningServices("content_owning", self._getHeaderFromPrevRequest(cache, aParam));
            }
            var param = {};
            param.senderF = req;
            param.query = query;
            param.category = category;
            return this._httpProxy.reqToMeta(param).then(function (aData) {
                self._adapter.setContentOwningLogoUrls(aData);
            });
        };
        CDLIpEpg.prototype.getSchedules = function (serviceId, startTime, hours, catchupOnly, prevHeader) {
            hx.log('mds', '######### getSchedules #########');
            var category = __HTTP_PROXY__.Category.META;
            var hash = this._adapter.getSchedulesQuery(serviceId, startTime, hours, catchupOnly);
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, hash);
                return self._adapter.getXmlByQuery(hash, self._getHeaderFromPrevRequest(cache, aParam));
            }
            var param = {};
            param.senderF = req;
            param.query = hash;
            param.category = category;
            return this._httpProxy.reqToMeta(param);
        };
        CDLIpEpg.prototype.getScheduleList = function (scheduleQueryList) {
            hx.log('mds', '######### getScheduleList #########');
            var reqParamList = [];
            var self = this;
            var category = __HTTP_PROXY__.Category.META;
            scheduleQueryList.forEach(function (scheduleQuery) {
                var param = {};
                var hash = self._adapter.getSchedulesQuery(scheduleQuery.serviceId, scheduleQuery.startTime, scheduleQuery.hours);
                param.senderF = function (aParam) {
                    var cache = self._cache_controller.get(category, hash);
                    return self._adapter.getXmlByQuery(hash, self._getHeaderFromPrevRequest(cache, aParam));
                };
                param.query = hash;
                param.category = category;
                param.resDef = scheduleQuery.retDef;
                reqParamList.push(param);
            });
            return this._httpProxy.reqToMetaList(reqParamList);
        };
        CDLIpEpg.prototype.getAITList = function (IQueryAIT) {
            hx.log('mds', '######### getAITList #########');
            if (!IQueryAIT) {
                hx.log('debug', '######### getAITList ####### FAIL!! !IQueryAIT');
                return Q.reject(null);
            }
            var reqParamList = [];
            var self = this;
            var category = __HTTP_PROXY__.Category.AIT;
            IQueryAIT.forEach(function (aitQuery) {
                var param = {};
                var hash = aitQuery.url;
                param.senderF = function (aParam) {
                    var cache = self._cache_controller.get(category, hash);
                    return self._adapter.getXmlByQuery(hash, self._getHeaderFromPrevRequest(cache, aParam));
                };
                param.query = hash;
                param.category = category;
                param.resDef = aitQuery.retDef;
                param.param = {
                    'URL': aitQuery.url
                };
                reqParamList.push(param);
            });
            return this._httpProxy.reqToMetaList(reqParamList);
        };
        CDLIpEpg.prototype.getAIT = function (url) {
            hx.log('mds', '######### getAIT #########');
            if (!url) {
                hx.log('debug', '######### getAIT ####### FAIL!! !url');
                return Q.reject(null);
            }
            var self = this;
            var category = __HTTP_PROXY__.Category.AIT;
            var param = {};
            var hash = url;
            param.senderF = function (aParam) {
                var cache = self._cache_controller.get(category, hash);
                return self._adapter.getXmlByQuery(hash, self._getHeaderFromPrevRequest(cache, aParam));
            };
            param.query = hash;
            param.category = category;
            param.param = {
                'URL': url
            };
            return this._httpProxy.reqToMeta(param);
        };
        CDLIpEpg.prototype.getDetailInfo = function (programmeId, encode, prevHeader) {
            if (encode === void 0) { encode = true; }
            hx.log('mds', '######### getDetailInfo #########');
            var category = __HTTP_PROXY__.Category.META;
            var hash = _join.call(arguments, ':');
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, hash);
                return self._adapter.getDetailInfo(programmeId, encode, self._getHeaderFromPrevRequest(cache, aParam));
            }
            var param = {};
            param.senderF = req;
            param.query = hash;
            param.category = category;
            return this._httpProxy.reqToMeta(param);
        };
        CDLIpEpg.prototype.getThumbnail = function (url, width, prevHeader, type) {
            if (width === void 0) { width = 400; }
            if (type === void 0) { type = ''; }
            hx.log('mds', '######### getThumbnail #######');
            if (!hx.svc.SETTING_UTIL.getInternetAvailable()) {
                hx.log('debug', '######### getThumbnail ####### FAIL!! Internet is not available');
                return Q.reject(null);
            }
            var self = this;
            var category = __HTTP_PROXY__.Category.IMAGE;
            //var hash = _join.call(arguments, ':');
            var hash = url + ':' + width + type;
            var contentType = prevHeader ? prevHeader["content-type"] : null;
            if (!contentType) {
                contentType = 'image/*';
            }
            function req(type) {
                var cache = self._cache_controller.get(category, hash);
                var aParam = {};
                aParam['Accept'] = contentType;
                return self._adapter.getThumbnail(url, width, self._getHeaderFromPrevRequest(cache, aParam), type);
            }
            var param = {};
            param.senderF = req;
            param.query = hash;
            param.category = category;
            param.param = type || null;
            return this._httpProxy.reqToImage(param);
        };
        CDLIpEpg.prototype.getChannelLogo = function (serviceId, width, prevHeader) {
            if (width === void 0) { width = 80; }
            hx.log('mds', '######### getChannelLogo ######');
            if (!hx.svc.SETTING_UTIL.getInternetAvailable()) {
                hx.log('debug', '######### getChannelLogo ####### FAIL!! Internet is not available');
                return Q.reject(null);
            }
            var self = this;
            var category = __HTTP_PROXY__.Category.IMAGE;
            //var hash = _join.call(arguments, ':');
            var hash = serviceId + ':' + width;
            var contentType = prevHeader ? prevHeader["content-type"] : null;
            if (!contentType) {
                contentType = 'image/*';
            }
            function req(aParam) {
                var cache = self._cache_controller.get(category, hash);
                var bParam = aParam || {};
                bParam['Accept'] = contentType;
                return self._adapter.getChannelLogo(serviceId, width, self._getHeaderFromPrevRequest(cache, bParam));
            }
            var param = {};
            param.senderF = req;
            param.query = hash;
            param.category = category;
            return this._httpProxy.reqToImage(param);
        };
        CDLIpEpg.prototype.getApplication = function (image_variant) {
            hx.log('mds', '######### getApplication #########');
            var category = __HTTP_PROXY__.Category.META;
            var hash = _join.call(arguments, ':');
            var query = hash + ':' + 'application:' + hx.svc.SETTING_UTIL.getNetworkId();
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, query);
                return self._adapter.getApplication(hx.svc.SETTING_UTIL.getNetworkId(), self._getHeaderFromPrevRequest(cache, aParam), image_variant);
            }
            var param = {};
            param.senderF = req;
            param.query = query;
            param.category = category;
            return this._httpProxy.reqToMeta(param);
        };
        CDLIpEpg.prototype.getOnDemandSuggestions = function (inputString) {
            hx.log('mds', '######### getOnDemandSuggestions #########');
            var category = __HTTP_PROXY__.Category.SEARCH;
            var query = 'ondemand:suggestions:' + inputString;
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, query);
                return self._adapter.getOnDemandSuggestions(inputString, self._getHeaderFromPrevRequest(cache, aParam));
            }
            var param = {};
            param.senderF = req;
            param.query = query;
            param.category = category;
            return this._httpProxy.reqToMeta(param);
        };
        CDLIpEpg.prototype.getOnDemandResults = function (searchTerm, pageNum) {
            hx.log('mds', '######### getOnDemandResults #########');
            var category = __HTTP_PROXY__.Category.SEARCH;
            var query = 'ondemand:results:' + searchTerm + (pageNum || '');
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, query);
                return self._adapter.getOnDemandResults(searchTerm, hx.svc.SETTING_UTIL.getNetworkId(), self._getHeaderFromPrevRequest(cache, aParam), pageNum);
            }
            var param = {};
            param.senderF = req;
            param.query = query;
            param.category = category;
            return this._httpProxy.reqToMeta(param);
        };
        CDLIpEpg.prototype.getContentOwningLogo = function (serviceId, width, prevHeader) {
            hx.log('mds', '######### getContentOwningLogo #########');
            var category = __HTTP_PROXY__.Category.IMAGE;
            var query = 'contentOwningLogo:' + serviceId + ':' + width;
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, query);
                return self._adapter.getContentOwningLogo(serviceId, width, self._getHeaderFromPrevRequest(cache, aParam));
            }
            var param = {};
            param.senderF = req;
            param.query = query;
            param.category = category;
            return this._httpProxy.reqToImage(param);
        };
        CDLIpEpg.prototype.getRecommendationCategories = function () {
            hx.log('mds', '######### getRecommendationCategories #########');
            var category = __HTTP_PROXY__.Category.RECOMMENDATION;
            var type = 'ondemand';
            var query = 'recommendation:categories:' + type;
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, query);
                return self._adapter.getRecommendationCategories(type, self._getHeaderFromPrevRequest(cache, aParam));
            }
            var param = {};
            param.senderF = req;
            param.query = query;
            param.category = category;
            return this._httpProxy.reqToMeta(param);
        };
        CDLIpEpg.prototype.getRecommendationResults = function (data) {
            hx.log('mds', '######### getRecommendationResults #########');
            var category = __HTTP_PROXY__.Category.RECOMMENDATION;
            var type = 'ondemand';
            var groupId = data ? data.groupId : '';
            var serviceId = data ? data.serviceId : '';
            var networkIds = hx.svc.SETTING_UTIL.getNetworkId();
            var query = 'recommendation:results:' + (groupId || serviceId) + ':' + networkIds;
            var self = this;
            function req(aParam) {
                var cache = self._cache_controller.get(category, query);
                return self._adapter.getRecommendationResults(type, networkIds, groupId, serviceId, self._getHeaderFromPrevRequest(cache, aParam));
            }
            var param = {};
            param.senderF = req;
            param.query = query;
            param.category = category;
            return this._httpProxy.reqToMeta(param);
        };
        //if retryAfter 0, immediately call request
        CDLIpEpg.prototype.getAuthToken = function (retryAfter, aMdsUrl, bForced) {
            var self = this;
            var category = __HTTP_PROXY__.Category.AUTH;
            var hash = __HTTP_PROXY__.Query.AUTH;
            var mdsUrl = this.MdsUrl = aMdsUrl || this.MdsUrl || hx.svc.SETTING_UTIL.getPrefixUrl();
            hx.log('mds', '######### getAuthToken #########');
            if (!mdsUrl) {
                hx.log('error', 'The URL is not ready for Auth yet..');
                return;
            }
            if (!bForced && (retryAfter != -1)) {
                hx.log('mds', 'not forced.. check whether already auth or not.');
                if (self.isAvailableToUseAuth()) {
                    hx.log('error', 'Auth is already succeeded! therefore we do not need to auth again!');
                    return;
                }
            }
            var cache = this._cache_controller.get(category, hash);
            function req(re_retry) {
                if (mdsUrl) {
                    return self._adapter.getAuthToken(re_retry, mdsUrl);
                }
                else {
                    return self._adapter.getAuthToken();
                }
            }
            var param = {};
            param.senderF = req;
            param.query = hash;
            param.category = category;
            //param.param =  {retryAfter : retryAfter, mdsUrl : mdsUrl};
            param.param = retryAfter;
            return this._httpProxy.reqToAuth(param).then(function (res) {
                hx.log('mds', 'reqToAuth Success call getChannels : ' + res);
                //Auth Suceess
                if (retryAfter != 0) {
                    self.getChannels(self._getHeaderFromPrevRequest(cache));
                    self.getContentOwningServices(self._getHeaderFromPrevRequest(cache));
                }
                else {
                    self.getChannels();
                    self.getContentOwningServices();
                }
                self.fireEvent('getAuthToken');
            }, function (ret) {
                hx.log('mds', 'reqToAuth Fail ');
                var res;
                if (ret && ret.reason) {
                    res = ret.reason.exception;
                }
                else if (ret && ret.status != null) {
                    res = ret;
                }
                if (res && res.status != null) {
                    hx.log('mds', 'reqToAuth Fail res.status : ' + res.status);
                    switch (res.status) {
                        case 401:
                            break;
                        case 403:
                            break;
                        case 0:
                            self.MdsUrl = null; // waiting NIT Updated. It would be Network problem on Browser.
                            self._UriMode = 'NIT';
                            break;
                        default:
                            break;
                    }
                }
                else if (res == null) {
                    hx.log('error', 'Auth Fail check httpStatus');
                    self._UriMode = 'NIT';
                }
                return Q.reject(res);
            });
        };
        CDLIpEpg.prototype.setEnableHttp = function (enable) {
            hx.log('mds', '######### setEnableHttp #########');
            var self = this;
            return self._httpProxy.setEnableProxy(enable).then(function () {
                if (enable) {
                    if (self.MdsUrl) {
                        return self.getAuthToken(0, self.MdsUrl, true);
                    }
                }
            });
        };
        CDLIpEpg.prototype.getCachedBackward = function () {
            var _this = this;
            var now = new Date().getTime();
            var cache = this._cache_controller.getDb();
            var metaCache = cache['meta'];
            var backwards = [];
            for (var query in metaCache) {
                if (query.indexOf('schedules') !== -1) {
                    query.replace(/start=([0-9]*)/g, function () {
                        var start = arguments[1] * 1000;
                        if (start < now) {
                            backwards.push(_this._cache_controller.get('meta', query));
                        }
                    });
                }
            }
            return backwards;
        };
        CDLIpEpg.prototype.isAvailableToUseAuth = function () {
            return this._httpProxy.isAvailableToUseAuth();
        };
        return CDLIpEpg;
    })(__KERNEL__);
    return CDLIpEpg;
});
