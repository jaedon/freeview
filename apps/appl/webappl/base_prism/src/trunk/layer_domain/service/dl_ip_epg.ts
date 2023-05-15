/**
 * Created by bglee on 2015-03-28.
 */
/// <reference path="../../def/sugar.d.ts"/>
/// <reference path="ip_epg/adapter.d.ts" />
/// <reference path="ip_epg/engine/interface_freeview_play.d.ts" />

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __ADAPTER__ = require('./ip_epg/engine/adapter_freeview_play');
import __cache_controller__ = require('./ip_epg/cache_controller');
import __HTTP_PROXY__ = require('./ip_epg/http_req_filter');

var _join = Array.prototype.join;

class CDLIpEpg extends __KERNEL__ implements IAdaptorIpEpg,IQueryMethodMDS {
    private _adapter = new __ADAPTER__(function() {
        return hx.svc.SETTING_UTIL.getPrefixUrl();
    });
    private _IhttpHeaderControl : IHttpControl = this._adapter;
    private _cache_controller = new __cache_controller__.CCacheController([
        //'channel', 'schedule', 'detailInfo', 'thumbnail', 'channelLogo', 'application',
        __HTTP_PROXY__.Category.META,__HTTP_PROXY__.Category.IMAGE,
        'moved', __HTTP_PROXY__.Category.AUTH
    ]);
    private _httpProxy : __HTTP_PROXY__.IHttpProxyMDS = new __HTTP_PROXY__.CHttpProxyMDS(this._cache_controller,this._IhttpHeaderControl);
    parse = this._adapter.parse;
    convert =  this._adapter.convert;
    private MdsUrl : string;
    private _UriMode : string = 'NIT';//'NIT' or 'USB'
    private isTermsAndConditionAccepted : boolean = false;
    constructor(aServiceName: string) {
        super(aServiceName);
        this.MdsUrl = null;
        hx.logadd("mds");
        this.initEventListener();
        this.defineEvents(['getAuthToken']);
    }
    private _getHeaderFromPrevRequest(cache, param?) {
        var hLastModified='';
        var hRetryAfter='';
        var hIfModifiedSince='';
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
        ret={
            'Retry-After' : hRetryAfter,
            'If-Modified-Since' : hIfModifiedSince || hLastModified,
            'URL' :hURL,
            'Accept' : hAccept
        };
        return ret;
    }
    initEventListener() {
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
        hx.svc.TV_PORTAL.addEventCb('TermsAndConditionsUpdated',this,(bAccepted)=>{
            hx.log('error','## TermsAndConditionsUpdated bAccepted : '+bAccepted);
            if (this.isTermsAndConditionAccepted!=bAccepted) {
                this.isTermsAndConditionAccepted = bAccepted;
                this.setEnableHttp(bAccepted);
            }
        });
        //AP Update check
        hx.svc.SETTING_UTIL.addEventCb('NetworkAvailableChanged', this, (networkAvailable) => {
            hx.log('error','## NetworkAvailableChanged networkAvailable : '+networkAvailable);
            if (!this.MdsUrl){
                var dbURL = hx.svc.SETTING_UTIL.getPrefixUrl();
                hx.log('error','## NetworkAvailableChanged networkAvailable MDS URL NULL , use DB URL : '+dbURL);
                this.MdsUrl = dbURL;
            }
            if (networkAvailable && this.MdsUrl && hx.svc.TV_PORTAL.isTermsAndConditionsAccepted()) {
                this.getAuthToken(0, this.MdsUrl);
            } else {
                hx.log('error','############## NetworkAvailableChanged MDS URL FAIL :  '+this.MdsUrl+ ' , termsAndCondition : '+hx.svc.TV_PORTAL.isTermsAndConditionsAccepted());
            }
            if (!networkAvailable) {
                // clear prev Auth info to reset if Internet is available to use again!
                var db = <__cache_controller__.ICacheQueryObject>{
                    'auth' : {}
                };
                this._cache_controller.setDb(db);
            }
        });
        hx.svc.AV_CTRL.addEventCb('NetworkUriChanged', this, (param) => {
            var uri ='';
            if (!param){
                this.MdsUrl = null;
                hx.log('error', '################ network uri Null');
                return;
            }
            if (hx.svc.TV_PORTAL.isTermsAndConditionsAccepted()) {
                this._httpProxy.setEnableProxy(true);//Do not authenticate here.
            }
            if (typeof param === 'string' && this._UriMode=='NIT') {
                uri = param;
                hx.log('error', '################ network uri changed : ' + uri + ' (prev uri : ' + this.MdsUrl + ')');
                if (hx.il.checkHttpUrl(uri)) {  //check url with http or https
                    if (uri != this.MdsUrl) {
                        hx.log('debug', '################ network uri updated ##### uri != this.MdsUrl');
                        this.getAuthToken(0, uri, true);    // forced regardless of auth before
                    } else {
                        this.getAuthToken(0, uri);
                    }
                } else {
                    hx.log('error', '######### network uri need to check URI : ' + uri);
                }
            } else if (typeof param != 'string') {
                //From USB
                uri = param['uri'];
                if (uri && uri != this.MdsUrl) {//check url with http or https
                    this.MdsUrl = uri;
                    this._UriMode = param['mode'];
                    hx.log('error', '################ network uri changed from USB: ' + uri);
                    var db = <__cache_controller__.ICacheQueryObject>{
                        'auth' : {},
                        'meta' : {},
                        'image' : {},
                        'moved' : {}
                    };
                    this._cache_controller.removeAll();
                    this._cache_controller.setDb(db);
                    this._adapter.setBaseUrl(null);
                    this.getAuthToken(-1, uri);//reset All cached data on HTTP and reset Auth
                }
            } else if(this._UriMode == 'USB') {
                hx.log('error', '################ network uri changed in USB MODE : ' + param);
            }
        });
    }
    getChannels(prevHeader?): Q.Promise<any>|any {
        hx.log('mds', '######### getChannels #########');
        var category : string = __HTTP_PROXY__.Category.META;
        var hash = _join.call(arguments, ':');
        var networkIds = hx.svc.SETTING_UTIL.getNetworkId();
        var reqParamList : __HTTP_PROXY__.IHttpReqMDSParam[] = [];
        var self = this;
        // [FVP ? Business-to-Consumer Interface Specification v1.3] - 6.1.1 Linear Service List by Network ID
        // The client device MAY send a single request for the complete list of services on a particular network.
        // This request SHALL use a single device-determined Network ID in order to ensure that only relevant services are returned.
        // In cases where multiple Network IDs are visible to the client device, a separate request SHALL be made for each.
        // Combining the results is the responsibility of the client device and the client device MAY filter out any duplicate services and services it cannot receive or that the user has deleted.
        networkIds.forEach(function(aNetworkId : number){
            var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
            param.senderF = function(aParam?) {
                var cache = self._cache_controller.get(category, hash);
                return self._adapter.getChannels(aNetworkId, self._getHeaderFromPrevRequest(cache, aParam));
            };
            param.query = hash + ':channel:' + aNetworkId;
            param.category = category;
            param.resDef = Q.defer();
            param.resDef.promise.then((aData) => {
                self._adapter.setChannelLogoUrls(aData);
            });
            reqParamList.push(param);
        });
        return this._httpProxy.reqToMetaList(reqParamList);
    }
    getContentOwningServices(prevHeader?) {
        hx.log('mds', '######### getContentOwningServices #########');
        var category : string = __HTTP_PROXY__.Category.META;
        var query = 'contentOwningServices';
        var self = this;
        function req(aParam?) {
            var cache = self._cache_controller.get(category, query);
            return self._adapter.getContentOwningServices("content_owning", self._getHeaderFromPrevRequest(cache, aParam));
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = query;
        param.category = category;
        return this._httpProxy.reqToMeta(param).then((aData) => {
            self._adapter.setContentOwningLogoUrls(aData);
        });
    }
    getSchedules(serviceId, startTime: number, hours: number, catchupOnly?: boolean, prevHeader?) {
        hx.log('mds', '######### getSchedules #########');
        var category = __HTTP_PROXY__.Category.META;
        var hash = this._adapter.getSchedulesQuery(serviceId, startTime, hours, catchupOnly);
        var self = this;
        function req(aParam?) {
            var cache = self._cache_controller.get(category, hash);
            return self._adapter.getXmlByQuery(hash, self._getHeaderFromPrevRequest(cache, aParam));
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = hash;
        param.category = category;
        return this._httpProxy.reqToMeta(param);
    }
    getScheduleList(scheduleQueryList:IQuerySchedule[]):Q.Promise<any> {
        hx.log('mds', '######### getScheduleList #########');
        var reqParamList : __HTTP_PROXY__.IHttpReqMDSParam[] = [];
        var self = this;
        var category = __HTTP_PROXY__.Category.META;
        scheduleQueryList.forEach(function(scheduleQuery : IQuerySchedule){
            var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
            var hash = self._adapter.getSchedulesQuery(scheduleQuery.serviceId, scheduleQuery.startTime, scheduleQuery.hours);
            param.senderF = function(aParam?) {
                var cache = self._cache_controller.get(category, hash);
                return self._adapter.getXmlByQuery(hash, self._getHeaderFromPrevRequest(cache, aParam));
            };
            param.query = hash;
            param.category = category;
            param.resDef = scheduleQuery.retDef;
            reqParamList.push(param);
        });
        return this._httpProxy.reqToMetaList(reqParamList);
    }
    getAITList(IQueryAIT:IQueryAIT[]):Q.Promise<any> {
        hx.log('mds', '######### getAITList #########');
        if (!IQueryAIT) {
            hx.log('debug', '######### getAITList ####### FAIL!! !IQueryAIT');
            return Q.reject(null);
        }
        var reqParamList : __HTTP_PROXY__.IHttpReqMDSParam[] = [];
        var self = this;
        var category = __HTTP_PROXY__.Category.AIT;
        IQueryAIT.forEach(function(aitQuery : IQueryAIT){
            var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
            var hash = aitQuery.url;
            param.senderF = function(aParam?) {
                var cache = self._cache_controller.get(category, hash);
                return self._adapter.getXmlByQuery(hash, self._getHeaderFromPrevRequest(cache, aParam));
            };
            param.query = hash;
            param.category = category;
            param.resDef = aitQuery.retDef;
            param.param ={
                'URL' : aitQuery.url
            };
            reqParamList.push(param);
        });
        return this._httpProxy.reqToMetaList(reqParamList);
    }
    getAIT(url: string):Q.Promise<any> {
        hx.log('mds', '######### getAIT #########');
        if (!url) {
            hx.log('debug', '######### getAIT ####### FAIL!! !url');
            return Q.reject(null);
        }
        var self = this;
        var category = __HTTP_PROXY__.Category.AIT;
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        var hash = url;
        param.senderF = function(aParam?) {
            var cache = self._cache_controller.get(category, hash);
            return self._adapter.getXmlByQuery(hash, self._getHeaderFromPrevRequest(cache, aParam));
        };
        param.query = hash;
        param.category = category;
        param.param ={
            'URL' : url
        };
        return this._httpProxy.reqToMeta(param);
    }

    getDetailInfo(programmeId, encode = true, prevHeader?) {
        hx.log('mds', '######### getDetailInfo #########');
        var category = __HTTP_PROXY__.Category.META;
        var hash = _join.call(arguments, ':');
        var self = this;
        function req(aParam?) {
            var cache = self._cache_controller.get(category, hash);
            return self._adapter.getDetailInfo(programmeId, encode, self._getHeaderFromPrevRequest(cache, aParam));
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = hash;
        param.category = category;
        return this._httpProxy.reqToMeta(param);
    }
    getThumbnail(url: string, width = 400, prevHeader?, type = '') {
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
        function req(type?) {
            var cache = self._cache_controller.get(category, hash);
            var aParam = {};
            aParam['Accept'] = contentType;
            return self._adapter.getThumbnail(url, width, self._getHeaderFromPrevRequest(cache, aParam), type);
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = hash;
        param.category = category;
        param.param = type || null;
        return this._httpProxy.reqToImage(param);
    }
    getChannelLogo(serviceId, width = 80, prevHeader?) {
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
        function req(aParam?) {
            var cache = self._cache_controller.get(category, hash);
            var bParam = aParam || {};
            bParam['Accept'] = contentType;
            return self._adapter.getChannelLogo(serviceId, width, self._getHeaderFromPrevRequest(cache, bParam));
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = hash;
        param.category = category;
        return this._httpProxy.reqToImage(param);
    }
    getApplication(image_variant?) {
        hx.log('mds', '######### getApplication #########');
        var category = __HTTP_PROXY__.Category.META;
        var hash = _join.call(arguments, ':');
        var query = hash + ':' + 'application:' + hx.svc.SETTING_UTIL.getNetworkId();
        var self = this;
        function req(aParam?) {
            var cache = self._cache_controller.get(category, query);
            return self._adapter.getApplication(hx.svc.SETTING_UTIL.getNetworkId(), self._getHeaderFromPrevRequest(cache, aParam), image_variant);
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = query;
        param.category = category;
        return this._httpProxy.reqToMeta(param);
    }
    getOnDemandSuggestions(inputString) {
        hx.log('mds', '######### getOnDemandSuggestions #########');
        var category = __HTTP_PROXY__.Category.SEARCH;
        var query = 'ondemand:suggestions:' + inputString;
        var self = this;
        function req(aParam?) {
            var cache = self._cache_controller.get(category, query);
            return self._adapter.getOnDemandSuggestions(inputString, self._getHeaderFromPrevRequest(cache, aParam));
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = query;
        param.category = category;
        return this._httpProxy.reqToMeta(param);
    }
    getOnDemandResults(searchTerm: String, pageNum?) {
        hx.log('mds', '######### getOnDemandResults #########');
        var category = __HTTP_PROXY__.Category.SEARCH;
        var query = 'ondemand:results:' + searchTerm + (pageNum || '');
        var self = this;
        function req(aParam?) {
            var cache = self._cache_controller.get(category, query);
            return self._adapter.getOnDemandResults(searchTerm, hx.svc.SETTING_UTIL.getNetworkId(), self._getHeaderFromPrevRequest(cache, aParam), pageNum);
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = query;
        param.category = category;
        return this._httpProxy.reqToMeta(param);
    }
    getContentOwningLogo(serviceId, width, prevHeader?) {
        hx.log('mds', '######### getContentOwningLogo #########');
        var category : string = __HTTP_PROXY__.Category.IMAGE;
        var query = 'contentOwningLogo:' + serviceId + ':' + width;
        var self = this;
        function req(aParam) {
            var cache = self._cache_controller.get(category, query);
            return self._adapter.getContentOwningLogo(serviceId, width, self._getHeaderFromPrevRequest(cache, aParam));
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = query;
        param.category = category;
        return this._httpProxy.reqToImage(param);
    }
    getRecommendationCategories() {
        hx.log('mds', '######### getRecommendationCategories #########');
        var category = __HTTP_PROXY__.Category.RECOMMENDATION;
        var type = 'ondemand';
        var query = 'recommendation:categories:' + type;
        var self = this;
        function req(aParam?) {
            var cache = self._cache_controller.get(category, query);
            return self._adapter.getRecommendationCategories(type, self._getHeaderFromPrevRequest(cache, aParam));
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = query;
        param.category = category;
        return this._httpProxy.reqToMeta(param);
    }
    getRecommendationResults(data) {
        hx.log('mds', '######### getRecommendationResults #########');
        var category = __HTTP_PROXY__.Category.RECOMMENDATION;
        var type = 'ondemand';
        var groupId = data ? data.groupId : '';
        var serviceId = data ? data.serviceId : '';
        var networkIds = hx.svc.SETTING_UTIL.getNetworkId();
        var query = 'recommendation:results:' + (groupId || serviceId) + ':' + networkIds;
        var self = this;
        function req(aParam?) {
            var cache = self._cache_controller.get(category, query);
            return self._adapter.getRecommendationResults(type, networkIds, groupId, serviceId, self._getHeaderFromPrevRequest(cache, aParam));
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = query;
        param.category = category;
        return this._httpProxy.reqToMeta(param);
    }
    //if retryAfter 0, immediately call request
    getAuthToken(retryAfter?: number|string, aMdsUrl? : string, bForced? : boolean) {
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
        function req(re_retry?:number|string) {
            if (mdsUrl) {
                return self._adapter.getAuthToken(re_retry, mdsUrl);
            } else {
                return self._adapter.getAuthToken();
            }
        }
        var param = <__HTTP_PROXY__.IHttpReqMDSParam>{};
        param.senderF = req;
        param.query = hash;
        param.category = category;
        //param.param =  {retryAfter : retryAfter, mdsUrl : mdsUrl};
        param.param = retryAfter;
        return this._httpProxy.reqToAuth(param).then((res:__cache_controller__.ICacheQuery) => {
            hx.log('mds', 'reqToAuth Success call getChannels : '+res);
            //Auth Suceess
            if (retryAfter != 0) {
                self.getChannels(self._getHeaderFromPrevRequest(cache));
                self.getContentOwningServices(self._getHeaderFromPrevRequest(cache));
            } else {
                self.getChannels();
                self.getContentOwningServices();
            }
            self.fireEvent('getAuthToken');
        }, (ret) => {
            hx.log('mds', 'reqToAuth Fail ');
            var res;
            if (ret && ret.reason) {
                res = ret.reason.exception;
            } else if (ret && ret.status!=null) {
                res = ret;
            }
            if (res && res.status != null) {
                hx.log('mds', 'reqToAuth Fail res.status : '+res.status);
                switch (res.status){
                    case 401:
                        break;
                    case 403:
                        break;
                    case 0:
                        self.MdsUrl = null; // waiting NIT Updated. It would be Network problem on Browser.
                        self._UriMode = 'NIT';
                        break;
                    default :
                        break;
                }
            } else if (res == null) {
                hx.log('error', 'Auth Fail check httpStatus');
                self._UriMode = 'NIT';
            }
            return Q.reject(res);
        });
    }
    setEnableHttp(enable:boolean):Q.Promise<any> {
        hx.log('mds', '######### setEnableHttp #########');
        var self = this;
        return self._httpProxy.setEnableProxy(enable).then(function() {
            if (enable){
                if (self.MdsUrl) {
                    return self.getAuthToken(0, self.MdsUrl, true);
                }
            }
        });
    }
    getCachedBackward(): any[] {
        var now = new Date().getTime();
        var cache = this._cache_controller.getDb();
        var metaCache = cache['meta'];
        var backwards = [];
        for (var query in metaCache) {
            if (query.indexOf('schedules') !== -1) {
                query.replace(/start=([0-9]*)/g, () => {
                    var start = arguments[1] * 1000;
                    if (start < now) {
                        backwards.push(this._cache_controller.get('meta', query));
                    }
                });
            }
        }
        return backwards;
    }
    isAvailableToUseAuth(): boolean {
        return this._httpProxy.isAvailableToUseAuth();
    }
}
export = CDLIpEpg;
