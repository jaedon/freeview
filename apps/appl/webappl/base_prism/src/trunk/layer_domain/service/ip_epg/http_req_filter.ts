/**
 * Created by ksjung on 2015-06-03.
 */
/// <reference path="../../../def/q.d.ts" />
/// <reference path="../../../def/jquery.d.ts" />
/// <reference path="../../../def/sugar.d.ts" />
/// <reference path="./adapter.d.ts" />
import __cache_controller__ = require('./cache_controller');

declare var hx: ifSugar;

enum TimeSec{
    T24h = 86400,
    T20h = 72000,
    T6h = 21600,
    T1h = 3600
}

export class Category{
    static AUTH : string ='auth';
    static META : string ='meta';
    static AIT : string ='ait';
    static IMAGE : string ='image';
    static SEARCH : string ='search';
    static RECOMMENDATION : string ='recommendation';
};

export class Query{
    static AUTH : string = 'auth';
    static REAUTH : string = 're_auth';
    static RETRYAFTER : string = 'retryAfter';
    static BACKOFF : string = 'backoff';
}

export interface IHttpReqMDSParam{
    senderF : (p?) => Q.Promise<any>; // function implementing req
    query : string; // hash for cacheControl
    param? : any; //parameter for sender
    category : Category; //auth, meta, image
    resDef? : Q.Deferred<any>; //If Req hava this for response, This call must return value by resDef.resolve call.
}

export interface IHttpProxyMDS{
    reqToAuth(sender:IHttpReqMDSParam): Q.Promise<any>|any;
    reqToMeta(sender:IHttpReqMDSParam): Q.Promise<any>|any;
    reqToImage(sender:IHttpReqMDSParam): Q.Promise<any>|any;
    reqToMetaList(senderList:IHttpReqMDSParam[]): Q.Promise<any>|any;
    setEnableProxy(enable : boolean): Q.Promise<any>;
    isAvailableToUseAuth(): boolean;
}

interface IHttpProxyResponse{
    _reqSuccess(res: IDeferResolved, sender : IHttpReqMDSParam): Q.Promise<any>|any; //req callback for DONE
    _reqFail(res: IJQueryXHRWithQuery, sender : IHttpReqMDSParam): Q.Promise<any>|any; //req callback for FAIL
}

class CCategoryMDSRes implements IHttpProxyResponse{
    public _cacheController : __cache_controller__.CCacheController;
    public _IhttpProxyMDS : IHttpProxyMDS;

    constructor(cacheController : __cache_controller__.CCacheController,_IhttpControl : IHttpProxyMDS){
        this._cacheController = cacheController;
        this._IhttpProxyMDS =  _IhttpControl;
    }
    __reTryAfter(res:IJQueryXHRWithQuery,sender:IHttpReqMDSParam) : Q.Promise<any>{
        var category : any = sender.category;
        var hash = sender.query;
        var self = this;
        var def = Q.defer();
        var status = res.status;

        var retry_After = parseFloat(res.getResponseHeader('Retry-After'));

        if(!retry_After){
            retry_After = 0;
        }

        var waitTime = retry_After*1000;
        hx.log('mds','__reTryAfter status('+status+'), retry-After : '+retry_After);

        setTimeout(function() {
            var re_authQuery:IHttpReqMDSParam = {
                senderF: null, query: Query.RETRYAFTER, param: null, category: Category.AUTH
            };
            self._IhttpProxyMDS.reqToAuth(re_authQuery).then(
                function (reAuthRet) {
                    hx.log('mds','Auth Success');
                    def.resolve(reAuthRet);
                }, function (ret) {
                    hx.log('mds','Auth retry Fail');
                    switch (status){
                        case 403:
                        case 404:
                            hx.log('mds','Auth retry Fail set 24 hours .........');
                            //set cache for 24 hour
                            var waitTime = TimeSec.T24h*1000;
                            setTimeout(function() {
                                self._IhttpProxyMDS.reqToAuth(re_authQuery).then(function(ret){
                                        hx.log('mds','Auth 24hours retry Fail set 24 hours');
                                        def.resolve(ret);
                                    },
                                    function(ret){
                                        hx.log('mds','Auth 24hours retry Fail set 24 hours');
                                        def.reject(ret);
                                    }
                                );
                            },waitTime);
                            break;
                        default :
                            self._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, null);
                            def.reject(res);
                            break;
                    }
                }
            )
        },waitTime+1000);
        return def.promise;
    }

    _reqSuccess(res:IDeferResolved, sender:IHttpReqMDSParam):any {
        return null;
    }

    _reqFail(res:IJQueryXHRWithQuery, sender:IHttpReqMDSParam):any {
        return null;
    }
}

class CAuthRes extends CCategoryMDSRes{
    constructor(cacheController : __cache_controller__.CCacheController,_IhttpControl : IHttpProxyMDS){
        super(cacheController,_IhttpControl);
    }

    __backOff(res:IJQueryXHRWithQuery,sender:IHttpReqMDSParam) : Q.Promise<any>{
        var category : any = sender.category;
        var hash = sender.query;
        var self = this;
        var retDef = Q.defer();
        var retryCount : number = 0;

        var cache = this._cacheController.get(category, hash);
        var paramKey = __cache_controller__.ICacheQueryParamKey;

        if(cache && (cache.status==500 || cache.status==502 || cache.status==503 ||cache.status==504) && cache.param && cache.param[paramKey.ReTryCount]){
            cache.param[paramKey.ReTryCount]+=1;
            retryCount = cache.param[paramKey.ReTryCount];
        }else{
            if(!cache.param){
                cache.param=[];
            }
            cache.param[paramKey.ReTryCount]=1;
            retryCount = 1;
        }

        this._cacheController.set(category, hash, null, res.status, cache.data,cache.param);


        var nextDelayTime = Math.pow(4,retryCount)*100;//until 104857600
        if(retryCount>10){
            hx.log('mds','Auth Retry time over ... ');
            retDef.reject(res);
            return retDef.promise;
        }
        hx.log('mds','Auth 500,504 next delay :'+nextDelayTime);
        setTimeout(function(){
            var backOffQuery:IHttpReqMDSParam = {
                senderF: null, query: Query.BACKOFF, param: null, category: Category.AUTH
            };
            self._IhttpProxyMDS.reqToAuth(backOffQuery).then(
                function (res) {
                    hx.log('mds','Auth Success from 500,504');
                    self._cacheController.set(category, hash, res.header, res.status, res.data);
                    return retDef.resolve(res);
                }, function () {
                    hx.log('mds','500 Auth retry from 500,504');
                    if(retryCount>=10){
                        return retDef.reject(res);
                    }
                }
            )
        },nextDelayTime+1000);
        return retDef.promise;
    }

    _reqSuccess(res:IDeferResolved, sender:IHttpReqMDSParam):any {
        var status = res.status;
        var category : string = <string>sender.category;
        var query = sender.query;
        var cache =this._cacheController.get(category, query);
        var paramKey = __cache_controller__.ICacheQueryParamKey;
        hx.log('mds', '[CAuthRes] _reqSuccess category('+category+'), status('+res.status+')');

        if(cache.param[paramKey.ReqPromise]){
            hx.log('mds','###### Auth Success BackOff promise : '+cache.param[paramKey.ReqPromise]+ ' count : '+cache.param[paramKey.ReTryCount]);
            cache.param[paramKey.ReqPromise] = null;//until status changed
            cache.param[paramKey.ReTryCount] = 0;//until status changed
        }

        this._cacheController.set(category, sender.query, res.header, res.status, res.data).get(category, sender.query);
//        this._cacheController.setDb(JSON.parse(localStorage.getItem(__cache_controller__.CCacheController.CACHE_DB_NAME)));
//        localStorage.setItem(__cache_controller__.CCacheController.CACHE_DB_NAME, JSON.stringify(this._cacheController.getDb()));



        return Q.resolve(res);
    }

    _reqFail(res:IJQueryXHRWithQuery, sender:IHttpReqMDSParam):any {
        var category : any = sender.category;
        var hash = sender.query;
        var self = this;
        hx.log('mds', '[CAuthRes] _reqFail category('+category+'), status('+(res?(res.status||res):'')+')');

        if (typeof res === 'string') {
            hx.log('debug', '[CAuthRes] _reqFail!! res(string) : ' + res);
            self._cacheController.set(category, hash, null, 0, null); //set cache
            return Q.reject(res);
        }

        var cache = this._cacheController.get(category,hash);
        if(cache){
            //check Last Status
            switch(cache.status){
                case 403:
                case 404:
                    return Q.reject(res);
                default :
                    break;
            }
        }

        this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, null); //set cache

        var procedure = {
            0 : (res) =>{
                //Browser Network Exception
                this._cacheController.set(category, hash, res.header, res.status, null); //set cache
                return Q.reject(res);
            },
            400: (res) => {
                this._cacheController.set(category, hash, res.header, res.status, null); //set cache
                return Q.reject(res);
            },
            401: (res) => {
                /*
                * If a 401 (Authentication Required) HTTP response code is received on a request to the authentication Service (see Section 4.1),
                * the device SHALL re-acquire the base_broadcast_auth_url from the broadcast URI Linkage Descriptor as required by [FVPC_TECH] Section 5.3 before attempting to authenticate again.
                * The device SHALL honour the Retry-After header in the response to determine when to re-attempt the authentication request.
                * */
                return self.__reTryAfter(res,sender);
            },
            403: (res) => {
                /*
                * If a 403 (Forbidden) HTTP response code is received on a request to the authentication service the device SHALL re-acquire the basebroadcast_auth_url
                * from the broadcast URI Linkage Descriptor as required by [FVPC_TECH] Section 5.3. The device SHALL then retry the request.
                * In the event that this retry also fails with a 403 response the device SHALL wait 24 hours before retrying.
                * The device SHALL retry the request once during any 24-hour period and the timeout MAY not be persisted across device reboots.
                * */
                return self.__reTryAfter(res,sender);
            },
            404: (res) => {
                /*
                 * If a 404 (Not Found) HTTP response code is received on a request to the authentication service the device SHALL re-acquire
                 * the basebroadcast_auth_url from the broadcast URI Linkage Descriptor as required by [FVPC_TECH] Section 5.3.
                 * The device SHALL then retry the request. In the event that this retry also fails with a 404 response the device SHALL wait 24 hours before retrying.
                 * The device SHALL retry the request once during any 24-hour period and the timeout MAY not be persisted across device reboots.
                 * */
                return self.__reTryAfter(res,sender);
            },
            406: (res) => {
                this._cacheController.set(category, hash, res.header, res.status, null);
                return Q.reject(res);
            },
            500 : (res) => {
                /*
                * If a 500 (System Error), 502 (Bad Gateway) or 504 (Gateway Timeout) HTTP response code is received on any request to the authentication service,
                * the device SHALL employ the back-off mechanism as described for these codes in Annex K.1.1Section B.2.4.6.3 of [DBOOK7_PARTB8].
                * The device MAY continue to retry until powered off, however the retry period SHALL only be increased for the initial 10 retries (up to a maximum of 104857600 milliseconds).//29hours
                * The device SHALL re-acquire the broadcast_authase_url from the broadcast URI Linkage Descriptor as required by [FVPC_TECH] Section 5.3 for each retry
                *
                * The retry count SHALL be reset when the device is powered off and does not need to be persisted.
                * */
                return self.__backOff(res,sender);
            },
            502 : (res) => {
                return self.__backOff(res,sender);
            },
            503 : (res) => {
                /*
                * If a 503 (Service Unavailable) HTTP response code is received on any request to either the metadata service, or authentication service or image service
                * (for instance due to rate limiting, see Section 5.4), the device SHALL perform as defined in Annex K.1.1Section B.2.4.6.3 of [DBOOK7_PARTB8]
                * with the exception that the device MAY continue to retry until powered off,
                * if no Retry-After header is specified the retry period SHALL only be increased for the initial 10 retries (up to a maximum of 104857600 milliseconds).
                 The retry count SHALL be reset when the device is powered off and does not need to be persisted.
                */
                var retry_After = parseFloat(res.getResponseHeader('Retry-After'));
                if(!retry_After){
                    return self.__backOff(res,sender);
                }else{
                    return self.__reTryAfter(res,sender);
                }
            },
            504 : (res) => {
                return self.__backOff(res,sender);
            }
        }[res.status];
        console.error('[' + res.status + '][' + res.statusText + '] ' + res.responseText);
        if (!procedure) {
            return Q.reject(null);
        }
        return procedure(res);
    }
}

class CMetaRes extends CCategoryMDSRes{
    constructor(cacheController : __cache_controller__.CCacheController, IhttpControl : IHttpProxyMDS){
        super(cacheController,IhttpControl);
    }

    __backOff(res:IJQueryXHRWithQuery,sender:IHttpReqMDSParam) : Q.Promise<any>{
        var category = <string>sender.category;
        var hash = sender.query;
        var self = this;
        var retDef = Q.defer();
        var retryCount : number = 0;

        var cache = this._cacheController.get(category, hash);
        var paramKey = __cache_controller__.ICacheQueryParamKey;

        if(cache && (cache.status==500 || cache.status==502 || cache.status==503 || cache.status==504) && cache.param && cache.param[paramKey.ReTryCount]){
            cache.param[paramKey.ReTryCount]+=1;
            retryCount = cache.param[paramKey.ReTryCount];
        }else{
            if(!cache.param){
                cache.param=[];
            }
            cache.param[paramKey.ReTryCount]=1;
            retryCount = 1;
        }

        this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, cache.data,cache.param);

        var re_authQuery:IHttpReqMDSParam = {
            senderF: null, query: Query.REAUTH, param: null, category: Category.AUTH
        };

        if(retryCount>10){
            retDef.reject(res);
            return retDef.promise;
        }

        function doNextReqMeta(){
            if(!sender.param){
                sender.param = {};
            }
            sender.param[Query.BACKOFF] = retryCount;
            var nextDelayTime = Math.pow(4,retryCount)*100;//until 104857600
            hx.log('mds','Meta 500,504 next delay :'+nextDelayTime);
            setTimeout(function(){
                self._IhttpProxyMDS.reqToMeta(sender).then(
                    function(res){
                        hx.log('mds','Meta success from 500,504');
                        retDef.resolve(res);
                    },
                    function(ret){
                        hx.log('mds','Meta Fail from 500,504');
                        self._cacheController.set(category, hash, ret.getAllResponseHeaders(), res.status, cache.data, cache.param);
                        if(retryCount>=10){
                            retDef.reject(res);
                        }
                    }
                );
            },nextDelayTime+1000);
        }
        switch(res.status) {
            case 500:
            case 502:
            case 504:
                if (category === Category.META) {
                    hx.log('mds','Meta trying Re-Auth from 500,502,504 ....');
                    self._IhttpProxyMDS.reqToAuth(re_authQuery).then(
                        function () {
                            doNextReqMeta();
                        }, function () {
                            if(retryCount>=10){
                                retDef.reject(res);
                            }
                        }
                    );
                } else {
                    hx.log('mds','Meta trying without Re-Auth with metadata endpoint....');
                    doNextReqMeta();
                }
                break;
            case 503:
                if (category === Category.SEARCH) {
                    hx.log('mds','NOT retry on SEARCH from 503');
                    retDef.reject(res);
                    break;
                }
                hx.log('mds','Meta trying without Re-Auth from 503 ....');
                doNextReqMeta();
                break;
            default:
                break;
        }
        return retDef.promise;
    }

    _reqSuccess(res:IDeferResolved, sender:IHttpReqMDSParam):any {
        var status = res.status;
        var category = <string>sender.category;
        var query = sender.query;
        var cache = this._cacheController.get(category, query);
        hx.log('mds', '[CMetaRes] _reqSuccess category('+category+'), status('+res.status+') , query : '+query);
        var ret = null;
        switch(res.status){
            case 304:
                ret = cache ? cache.data : null;
                break;
            default :
                var paramKey = __cache_controller__.ICacheQueryParamKey;
                if (!cache.param) {
                    cache.param = [];
                }
                if (cache.param[paramKey.ReqPromise]) {
                    hx.log('mds','###### Meta Success BackOff promise : '+cache.param[paramKey.ReqPromise]+ ' count : '+cache.param[paramKey.ReTryCount]);
                    cache.param[paramKey.ReqPromise] = null;//until status changed
                    cache.param[paramKey.ReTryCount] = 0;//until status changed
                }
                cache.param[paramKey.ResDate] = Date.now();//Use client time, not Server time.
                this._cacheController.set(category, query, res.header, res.status, res.data,cache.param);
                cache =this._cacheController.get(category, query);
                ret = cache.data;
                break;
        }
        return Q.resolve(ret);//only Data return
    }

    _reqFail(res:IJQueryXHRWithQuery, sender:IHttpReqMDSParam):any {
        var category = <string>sender.category;
        var hash = sender.query;
        var self = this;
        hx.log('mds', '[CMetaRes] _reqFail category('+category+'), status('+(res?(res.status||res):'')+') , query : '+hash);

        var cache = this._cacheController.get(category,hash);
        var paramKey = __cache_controller__.ICacheQueryParamKey;
        if(!cache.param){
            cache.param=[];
        }

        if (typeof res === 'string') {
            hx.log('debug', '[CMetaRes] _reqFail!! res(string) : ' + res);
            self._cacheController.set(category, hash, null, 0, null); //set cache
            /*
             network problem or image url does not be included.
            */
            return Q.reject(res);
        }

        cache.param[paramKey.ResIJQueryXHR] = res;
        cache.param[paramKey.ResDate] = Date.now();//Use client time, not Server time.
        this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, null,cache.param);

        function Meta500Error(res:IJQueryXHRWithQuery) : Q.Promise<any>{
            self._cacheController.set(category, hash, null, 500, null); //set cache
            return Q.reject(res);
        }
        var procedure = {
            400: (res) => {
                this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, null); //set cache
                return Q.reject(res);
            },
            401: (res) => {
                if (category === Category.AIT) {
                    //A 401 HTTP response code SHALL NOT be provided in response to requests to the image
                    //endpoint or an XML AIT service. If a 401 code is received, the client device SHALL
                    //assume that this is a system error and behave as defined for the 500 HTTP response code
                    //in Section 4.3.5.
                    hx.log('mds', '[CMetaRes] _reqFail : the device SHOULD not retry about XML AIT ######');
                    return Meta500Error(res);
                }
/*
* If a 401 HTTP response code is received on a request to the metadata service,
* the device SHALL wait the period defined by the Retry-After header of the response and then attempt to re-authenticate using the authentication service (see Section 4.1).
* If no Retry-After header is provided the device SHOULD attempt to re-authenticate immediately.
*/
                if(sender.resDef){
                    //for checking multiple call which need to re-auth once not every time.
                    hx.log('mds', '[CMetaRes] _reqFail Meta 401 Error ######');
                    return Q.reject(res);
                }

                var retDef = Q.defer();
                var promise = retDef.promise;
                promise.fin(function(){
                        var retry_After = parseFloat(res.getResponseHeader('Retry-After'));
                        if(!retry_After){
                            retry_After = 0;
                        }
                        var re_sender = sender;
                        if(re_sender.param){
                            re_sender.param['Retry-After'] = retry_After;
                        }else{
                            re_sender.param = {
                                'Retry-After' : retry_After
                            };
                        }

                        self._IhttpProxyMDS.reqToMeta(re_sender).then(function(res){
                                hx.log('mds','401 Meta Success ');
                                //retDef.resolve(res);
                            },
                            function(err){
                                hx.log('mds','401 Meta Fail');
                                //retDef.reject(err);
                            }
                        );
                });

                //Re-Auth
                self.__reTryAfter(res,sender).then(function(){
                        hx.log('mds','ReAuth Success');
                        retDef.reject(res);
                    }
                );
                return promise;
            },
            403: (res) => {
                if (category === Category.AIT) {
                    //A 403 HTTP response code SHALL NOT be provided in response to requests to the image
                    //endpoint or an XML AIT service. If a 403 code is received, the client device SHALL
                    //assume that this is a system error and behave as defined for the 500 HTTP response code
                    //in Section 4.3.5.
                    hx.log('mds', '[CMetaRes] _reqFail : the device SHOULD not retry about XML AIT ######');
                    return Meta500Error(res);
                }
            /*
            * If a 403 (Forbidden) HTTP response code is received on a request to the metadata service the device SHALL attempt to re-authenticate using the authentication service.
            * If after this re-authentication, calls to the metadata service still fail with a 403 response, the device SHALL wait 24 hours before retrying.
            * The device SHALL only retry the request once during any 24-hour period and the timeout MAY not be persisted across device reboots.
            * */
                if(sender.resDef){
                    //for checking multiple call which need to re-auth once not every time.
                    return Q.reject(res);
                }

                var retDef = Q.defer();
                var promise = retDef.promise;

                promise.fin(function(){
                    self._IhttpProxyMDS.reqToMeta(sender).then(function(ret){
                            hx.log('mds','403 Meta Success');
                            //retDef.resolve(ret);
                        },
                        function(){
                            hx.log('mds','403 set cache for 24 hours for retrying');
                            //set cache for 24 hour for retrying
                            var waitTime = TimeSec.T24h*1000;
                            //set 24 hours to Cache Data
                            //var category : string = Category.META;
                            var query = sender.query;
                            var cache = self._cacheController.get(category, query);
                            cache.header['Retry-After'] = TimeSec.T24h;
                            self._cacheController.setCache(category, hash,cache);

                            setTimeout(function() {
                                hx.log('mds','start 403 with 24 hours for retrying');
                                self._IhttpProxyMDS.reqToMeta(sender).then(function(ret){
                                        hx.log('mds','Last reqToMeta Success in 24 hours');
                                    },
                                    function(ret){
                                        hx.log('mds','Last reqToMeta Fail... in 24 hours');
                                    });
                            },waitTime+1000);
                        }
                    );
                });

                self.__reTryAfter(res,sender).then(
                    function(ret){
                        hx.log('mds','403 ReAuth Success ret reject previous result and then retry again');
                        retDef.reject(res);
                    },
                    function(err){
                        hx.log('mds','403 ReAuth Fail : '+err);
                        if(err && err.reason == 'getBroadcastURI'){
                            res.reason = 'getBroadcastURI'; //Retry After getting Broadcast URI
                            retDef.reject(res);
                        }
                        //Auth Fail. save 403 status to avoid re-request until 24 hours
                        this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, null);
                        retDef.reject(res);
                    }
                );
                return retDef.promise;
            },
            404: (res) => {
                if (category === Category.AIT) {
                    //If a 404 (Not Found) HTTP response code is received on a request to the image endpoint
                    //or an XML AIT service the client device SHALL NOT retry the request and deem it to have
                    //failed.
                    hx.log('mds', '[CMetaRes] _reqFail : the device SHOULD not retry about XML AIT ######');
                    return Q.reject(res);
                }
                /*
                * If a 404 (Not Found) HTTP response code is received on a request to the metadata service
                * the device SHALL attempt to re-authenticate using the authentication service.
                * If after this re-authentication calls to the metadata service still fail with a 404 response,
                * the device SHALL wait 24 hours before retrying. The device SHALL retry the request once during any 24-hour period and the timeout MAY not be persisted across device reboots.
                * */
                if(sender.resDef){
                    //for checking multiple call which need to re-auth once not every time.
                    return Q.reject(res);
                }

                var retDef = Q.defer();
                var promise = retDef.promise;
                promise.fin(function(){
                    self._IhttpProxyMDS.reqToMeta(sender).then(function(ret){
                            hx.log('mds','404 Meta Success');
                            //retDef.resolve(ret);
                        },
                        function(){
                            hx.log('mds','set cache for 24 hours for retrying');
                            //set cache for 24 hour for retrying
                            var waitTime = TimeSec.T24h*1000;
                            //set 24 hours to Cache Data
                            //var category : string = Category.META;
                            var query = sender.query;
                            var cache = self._cacheController.get(category, query);
                            cache.header['Retry-After'] = TimeSec.T24h;
                            self._cacheController.setCache(category, hash, cache);

                            setTimeout(function() {
                                hx.log('mds','24 hours for retrying awake up');
                                self._IhttpProxyMDS.reqToMeta(sender).then(function(ret){
                                        hx.log('mds','Last reqToMeta Success in 24 hours');
                                        //retDef.resolve(ret);
                                    },
                                    function(ret){
                                        hx.log('mds','Last reqToMeta Fail... in 24 hours');
                                        //retDef.reject(ret);
                                    });
                            },waitTime+1000);
                        }
                    );
                });
                self.__reTryAfter(res,sender).then(
                    function(ret){
                        hx.log('mds','404 ReAuth Success ret reject previous result and then retry again');
                        retDef.reject(res);
                    },
                    function(err){
                        hx.log('mds','403 ReAuth Fail : '+err);
                        if(err && err.reason == 'getBroadcastURI'){
                            res.reason = 'getBroadcastURI'; //Retry After getting Broadcast URI
                        }
                        //Auth Fail. save 403 status to avoid re-request until 24 hours
                        this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, null);
                        retDef.reject(res);
                    }
                );
                return retDef.promise;
            },
            406: (res) => {
                var param = [];
                param[paramKey.ResIJQueryXHR] = res;
                this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, null,param);
                return Q.reject(res);
            },
            500 : (res) => {
                if (category === Category.SEARCH) {
                    hx.log('mds', '[CMetaRes] _reqFail : the device SHOULD not retry about SEARCH ######');
                    return Q.reject(res);
                }
                /*
                 * If a 500 (System Error), 502 (Bad Gateway) or 504 (Gateway Timeout) HTTP response code is received on any request to the metadata service,
                 * the device SHALL employ the back-off mechanism as described for these codes in Annex K.1.1Section B.2.4.6.3 of [DBOOK7_PARTB8].
                 * The device MAY continue to retry until powered off, however the retry period SHALL only be increased for the initial 10 retries (up to a maximum of 104857600 milliseconds).
                 * The device SHALL attempt to re-authenticate using the authentication service before each retry.
                 *
                 * The retry count SHALL be reset when the device is powered off and does not need to be persisted.
                 * */
                if(sender.resDef){
                    //for checking multiple call which need to re-auth once not every time.
                    hx.log('mds','####### 500 From List');
                    return Q.reject(res);
                }

                return self.__backOff(res,sender);
            },
            502 : (res) => {
                if (category === Category.SEARCH) {
                    hx.log('mds', '[CMetaRes] _reqFail : the device SHOULD not retry about SEARCH ######');
                    return Q.reject(res);
                }
                if(sender.resDef){
                    //for checking multiple call which need to re-auth once not every time.
                    hx.log('mds','########## 502 From List');
                    return Q.reject(res);
                }
                return self.__backOff(res,sender);
            },
            504 : (res) => {
                if (category === Category.SEARCH) {
                    hx.log('mds', '[CMetaRes] _reqFail : the device SHOULD not retry about SEARCH ######');
                    return Q.reject(res);
                }
                if(sender.resDef){
                    //for checking multiple call which need to re-auth once not every time.
                    hx.log('mds','########## 504 From List');
                    return Q.reject(res);
                }
                return self.__backOff(res,sender);
            },
            503 : (res) => {
                if (category === Category.SEARCH) {
                    hx.log('mds', '[CMetaRes] _reqFail : the device SHOULD not retry about SEARCH ######');
                    return Q.reject(res);
                }
                /*
                 * If a 503 (Service Unavailable) HTTP response code is received on any request to either the metadata service, or authentication service or image service
                 * (for instance due to rate limiting, see Section 5.4), the device SHALL perform as defined in Annex K.1.1Section B.2.4.6.3 of [DBOOK7_PARTB8]
                 * with the exception that the device MAY continue to retry until powered off,
                 * if no Retry-After header is specified the retry period SHALL only be increased for the initial 10 retries (up to a maximum of 104857600 milliseconds).
                 The retry count SHALL be reset when the device is powered off and does not need to be persisted.
                 */
                if (sender.resDef) {
                    //for checking multiple call which need to re-auth once not every time.
                    return Q.reject(res);
                }
                var retry_After = parseFloat(res.getResponseHeader('Retry-After'));
                if (retry_After != undefined && retry_After >= 0) {
                    var retDef = Q.defer();
                    var waitTime = retry_After*1000;
                    hx.log('mds','503 reTryAfter without authentication status('+status+'), retry-After : '+retry_After);
                    var promise = retDef.promise;
                    retDef.reject(res);//for response to UI
                    setTimeout(function() {
                        self._IhttpProxyMDS.reqToMeta(sender).then(function(ret){
                                hx.log('mds','503 retryAfter reqToMeta success');
                                //retDef.resolve(ret);
                            },
                            function(res){
                                hx.log('mds','503 retryAfter reqToMeta Fail');
                                //retDef.reject(res);
                            }
                        );
                    }, waitTime+1000);
                    return promise;
                } else {
                    return self.__backOff(res, sender);
                }
            }
        }[res.status];
        if (!procedure) {
            return Q.reject(null);
        }
        return procedure(res);
    }
}

class CImageRes extends CCategoryMDSRes{

    constructor(cacheController : __cache_controller__.CCacheController, IhttpControl : IHttpProxyMDS){
        super(cacheController,IhttpControl);
    }

    __backOff(res:IJQueryXHRWithQuery,sender:IHttpReqMDSParam) : Q.Promise<any>{
        var category = <string>sender.category;
        var hash = sender.query;
        var self = this;
        var retDef = Q.defer();
        var retryCount : number = 0;

        var cache = this._cacheController.get(category, hash);
        var paramKey = __cache_controller__.ICacheQueryParamKey;

        if(cache && (cache.status==503) && cache.param && cache.param[paramKey.ReTryCount]){
            cache.param[paramKey.ReTryCount]+=1;
            retryCount = cache.param[paramKey.ReTryCount];
        }else{
            if(!cache.param){
                cache.param=[];
            }
            cache.param[paramKey.ReTryCount]=1;
            retryCount = 1;
        }

        this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, cache.data,cache.param);

        if(retryCount>10){
            retDef.reject(res);
            return retDef.promise;
        }

        if(res.status == 503){
            if(!sender.param){
                sender.param = {};
            }
            hx.log('mds','Image trying without ReAuth from 503 ....');
            sender.param[Query.BACKOFF] = retryCount;
            var nextDelayTime = Math.pow(4,retryCount)*100;//until 104857600

            hx.log('mds','Image 503 timeout next delay :'+nextDelayTime);
            setTimeout(function(){
                self._IhttpProxyMDS.reqToImage(sender).then(
                    function(res){
                        hx.log('mds','Image success from 500,504');
                        retDef.resolve(res);
                    },
                    function(ret){
                        hx.log('mds','Image Fail from 500,504');
                        self._cacheController.set(category, hash, ret.getAllResponseHeaders(), res.status, cache.data, cache.param);
                        if(retryCount>=10){
                            retDef.reject(res);
                        }
                    }
                );
            },nextDelayTime+1000);
        }

        return retDef.promise;
    }

    _reqSuccess(res:IDeferResolved, sender:IHttpReqMDSParam):any {
        var status = res.status;
        var category = <string>sender.category;
        var query = sender.query;
        var cache =this._cacheController.get(category, query);
        hx.log('mds', '[CImageRes] _reqSuccess category('+category+'), status('+res.status+') , query : '+query);
        var ret = null;
        switch(res.status){
            case 304:
                ret = cache ? cache.data : null;
                break;
            default :
                var paramKey = __cache_controller__.ICacheQueryParamKey;
                if (!cache.param) {
                    cache.param = [];
                }
                cache.param[paramKey.ResDate] = Date.now();//Use client time, not Server time.
                if (cache.param[paramKey.ReqPromise]) {
                    cache.param[paramKey.ReqPromise] = null;//until status changed
                    cache.param[paramKey.ReTryCount] = 0;//until status changed
                }
                this._cacheController.set(category, query, res.header, res.status, res.data,cache.param);
                cache =this._cacheController.get(category, query);
                ret = cache.data;
                break;
        }
        return Q.resolve(ret);//only Data return
    }

    _reqFail(res:IJQueryXHRWithQuery, sender:IHttpReqMDSParam):any {
        var category : any = sender.category;
        var hash = sender.query;
        var self = this;

        hx.log('mds', '[CImageRes] _reqFail category('+category+'), status('+(res?(res.status||res):'')+') , query : '+hash);
        var cache = this._cacheController.get(category,hash);
        var paramKey = __cache_controller__.ICacheQueryParamKey;
        if(!cache.param){
            cache.param=[];
        }

        if (typeof res === 'string') {
            hx.log('debug', '[CImageRes] _reqFail!! res(string) : ' + res);
            self._cacheController.set(category, hash, null, 0, null); //set cache
            /*
            network problem or image url does not be included.
            * */
            return Q.reject(null);
        }

        cache.param[paramKey.ResIJQueryXHR] = res;
        cache.param[paramKey.ResDate] = Date.now();//Use client time, not Server time.
        this._cacheController.set(category, hash, res.getAllResponseHeaders(), res.status, null,cache.param);

        function Image500Error(res:IJQueryXHRWithQuery) : Q.Promise<any>{
            self._cacheController.set(category, hash, null, 500, null); //set cache
            return Q.reject(null);
        }

        var procedure = {
            400: (res) => {
                // Bad Request
                self._cacheController.set(category, hash, res.header, res.status, null); //set cache
                return Q.reject(null);
            },
            401: (res) => {
                // Authentication Required
            /*
            * A 401 HTTP response code SHALL NOT be provided in response to requests to the image service. If a 401 code is received,
            * the device SHOULD assume that this is a system error and behave as defined for the 500 HTTP response code in Section 4.3.5.
            * */
                return Image500Error(null);
            },
            403: (res) => {
                // Forbidden
            /*
            * A 403 HTTP response code SHALL NOT be provided in response to requests to the image service.
            * If a 403 code is received, the device SHOULD assume that this is a system error and behave as defined for the 500 HTTP response code in Section 4.3.5.
            * */
                return Image500Error(null);
            },
            404: (res) => {
                // Not Found
                /*
                * If a 404 (Not Found) HTTP response code is received on a request to the image service the device SHALL NOT retry the request and deem it to have failed.
                * */
                self._cacheController.set(category, hash, res.header, res.status, null);
                return Q.reject(null);
            },
            406: (res) => {
                // Not Acceptable
                self._cacheController.set(category, hash, res.header, res.status, null);
                return Q.reject(null);
            },
            500: (res) => {
                // System Error
                self._cacheController.set(category, hash, res.header, res.status, null);
                return Q.reject(null);
            },
            502: (res) => {
                // Bad Gateway
                self._cacheController.set(category, hash, res.header, res.status, null);
                return Q.reject(null);
            },
            504: (res) => {
                // Gateway Timeout
                self._cacheController.set(category, hash, res.header, res.status, null);
                return Q.reject(null);
            },
            503 : (res) => {
                /*
                 * If a 503 (Service Unavailable) HTTP response code is received on any request to either the metadata service, or authentication service or image service
                 * (for instance due to rate limiting, see Section 5.4), the device SHALL perform as defined in Annex K.1.1Section B.2.4.6.3 of [DBOOK7_PARTB8]
                 * with the exception that the device MAY continue to retry until powered off,
                 * if no Retry-After header is specified the retry period SHALL only be increased for the initial 10 retries (up to a maximum of 104857600 milliseconds).
                 The retry count SHALL be reset when the device is powered off and does not need to be persisted.
                 */
                if(sender.resDef){
                    //for checking multiple call which need to re-auth once not every time.
                    return Q.reject(res);
                }
                var retry_After = parseFloat(res.getResponseHeader('Retry-After'));
                if (!retry_After) {
                    return self.__backOff(res, sender);
                } else {
                    var retDef = Q.defer();
                    var waitTime = retry_After*1000;
                    hx.log('mds','__reTryAfter status('+status+'), retry-After : '+retry_After);
                    var promise = retDef.promise;
                    retDef.reject(res);
                    setTimeout(function() {
                        self._IhttpProxyMDS.reqToImage(sender).then(function(ret){
                                hx.log('mds','503 reqToMeta success');
                                //retDef.resolve(ret);
                            },
                            function(res){
                                hx.log('mds','503 reqToMeta Fail');
                                //retDef.reject(res);
                            }
                        );
                    },waitTime+1000);
                    return promise;
                }
            }
        }[res.status];
        if (!procedure) {
            return Q.reject(null);
        }
        return procedure(res);
    }
}

interface IHttpProxy extends IHttpProxyResponse{
    req(query : string, sender : IHttpReqMDSParam): Q.Promise<any>|any;
}

class CHttpProxy implements IHttpProxy{
    static _instance: CHttpProxy;

    constructor(){
        if (CHttpProxy._instance) {
            return CHttpProxy._instance;
        }
        CHttpProxy._instance = this;
    }

    req(query : string, sender:IHttpReqMDSParam){
        hx.log('mds','CHttpProxy req');
        if(sender.senderF){
            if(sender.param){
                return sender.senderF(sender.param);
            }else{
                return sender.senderF();
            }
        }else{
            return Q.reject(null);
        }
    }
    _reqSuccess(res: IDeferResolved, sender:IHttpReqMDSParam){return Q.reject(null);}
    _reqFail(res: IJQueryXHRWithQuery, sender:IHttpReqMDSParam){return Q.reject(null);}
}

export class CHttpProxyMDS extends CHttpProxy implements IHttpProxyMDS {
    private _cacheController : __cache_controller__.CCacheController;
    private _IhttpControl : IHttpControl;
    static _instance: CHttpProxyMDS;
    private _categoryResponses:IHttpProxyResponse[];
    private _promiseLoadingList : string[];
    private _isEnableProxy : boolean;

    constructor(cacheController : __cache_controller__.CCacheController,_IhttpControl : IHttpControl){
        super();
        if (CHttpProxyMDS._instance) {
            return CHttpProxyMDS._instance;
        }
        CHttpProxyMDS._instance = this;

        this._cacheController = cacheController;

        //this._cacheController.setDb(JSON.parse(localStorage.getItem(__cache_controller__.CCacheController.CACHE_DB_NAME)));
        this._IhttpControl = _IhttpControl;
        var _IHttpProxyMDS:IHttpProxyMDS = this;
        this._categoryResponses = [];
        this._categoryResponses[Category.AUTH]=new CAuthRes(this._cacheController,_IHttpProxyMDS);
        this._categoryResponses[Category.META]=new CMetaRes(this._cacheController,_IHttpProxyMDS);
        this._categoryResponses[Category.IMAGE]=new CImageRes(this._cacheController,_IHttpProxyMDS);
        this._promiseLoadingList = [];

        //chck if it is availble to Proxy using Terms and Conditions;
        this._isEnableProxy = false;
    }

    private _isExpired(cache : __cache_controller__.ICacheQuery, sender?:IHttpReqMDSParam): boolean {
        function __isMaxAgeExpired(cacheInfo: __cache_controller__.ICacheHeader) {
            var cacheControl = cacheInfo['Cache-Control'];
            if (!cacheControl) {
                return false;
            }

            var maxAge = /^max-age=([0-9]+)/.exec(cacheControl);

            if (!(maxAge && maxAge[1])) {
                return false;
            }


            var paramKey = __cache_controller__.ICacheQueryParamKey;
            var stampTime = cache.param[paramKey.ResDate];
            var maxAgeV = (parseFloat(maxAge[1]))*1000;//seconds
            var nowTime = new Date().getTime();
            var ret = stampTime + maxAgeV <= nowTime;
            return ret;
        }

        function __isExpiredAsAIT(cache : __cache_controller__.ICacheQuery, sender:IHttpReqMDSParam){
            //MDS921 AIT
            /*
             * The device SHALL respect the Expires header and/or Cache-Control: max-age header when retrieving Template XML AIT documents,
             * refreshing any cached Template AITs at the next opportunity once the timestamp/duration is reached.
             * If no Expires or max-age header is provided the device SHALL assume an expiry of 24 hours from retrieval.
             * If both an Expires and max-age header are present the device SHALL use the Cache-Control: max-age to determine when to refresh the Template XML AIT.
             * */
            if(!sender ||!sender.param || !sender.param['URL']){
                return false;
            }

            var cacheControl = cache.header['Cache-Control'];
            var hasMaxAge = cacheControl? (/^max-age=([0-9]+)/.exec(cacheControl)  ? true :false) : false;
            var maxAge = TimeSec.T24h;
            if(!hasMaxAge){
                var paramKey = __cache_controller__.ICacheQueryParamKey;
                var stampTime = cache.param[paramKey.ResDate];
                var maxAgeV = maxAge*1000;//seconds
                var nowTime = new Date().getTime();
                var ret = stampTime + maxAgeV <= nowTime;
                return ret;
            }
            return false
        }

        function __isExpiresExpired(cacheInfo: __cache_controller__.ICacheHeader, sender?:IHttpReqMDSParam) {
            /*
            * If both an Expires and max-age header are present the device SHALL use the Cache-Control: max-age
            * to determine when to refresh the Template XML AIT.
            * */
            if(sender && sender.param && sender.param['URL']){
                var cacheControl = cache.header['Cache-Control'];
                var hasMaxAge = cacheControl? (/^max-age=([0-9]+)/.exec(cacheControl)  ? true :false) : false;
                if(hasMaxAge){
                    return false;
                }
            }

            var expires = cacheInfo['Expires'];
            var ret = !!expires && new Date(expires.trim()).valueOf() <= new Date().getTime();
            return ret;
        }
        var header: __cache_controller__.ICacheHeader;
        if (cache) {
            header = cache.header;
        }
        if (!header){
            return false;
        }
        return __isMaxAgeExpired(header) || __isExpiresExpired(header,sender) || __isExpiredAsAIT(cache,sender);
    }

    _reqSuccess(res: IDeferResolved, sender:IHttpReqMDSParam) {
        return Q.resolve(res);
    }
    _reqFail(res: IJQueryXHRWithQuery, sender:IHttpReqMDSParam){
        return Q.reject(res);
    }

    req(query:string, sender:IHttpReqMDSParam):any {
        var responseCategory = <string>sender.category;
        if (sender.category === Category.AIT || sender.category === Category.SEARCH || sender.category === Category.RECOMMENDATION) {
            responseCategory = Category.META;
        }
        return super.req(query,sender).then((res: IDeferResolved) => {
            return this._categoryResponses[responseCategory]._reqSuccess(res,sender);
        }, (res) => {
            return this._categoryResponses[responseCategory]._reqFail(res,sender);
        });
    }

    __isAvailableToUseAuth() {
        var cache = this._cacheController.get(Category.AUTH, Query.AUTH);
        if (cache) {
            if (cache.status == 200) {
                hx.log('mds','__isAvailableToUseAuth status 200');
                return true;
            } else if (cache.status == 999){
                //check Auth loading...
                hx.log('mds','Auth Loading...');
                return false;
            } else {
                hx.log('mds','__isAvailableToUseAuth status Error : '+cache.status);
                return false;
            }
        }
        hx.log('mds','__isAvailableToUseAuth status Error It need to auth');
        return false;
    }

    private __lastAuthSender; // for implemnting re-auth
    private __authPromise; // for checking re-auth time

    reqToAuth(sender:IHttpReqMDSParam){
        hx.log('mds','reqToAuth');
        if (hx.emulMode) {
            return Q.reject(null); //request cancel
        }
        var category : string= Category.AUTH;
        var query = sender.query;
        var retDef = Q.defer();
        var self = this;
        var reqSender;

        if (!this._isEnableProxy){
            // removed checking _isEnableProxy with auth
            hx.log('mds','Not Availble Proxy');
            if(sender.resDef) {
                return Q.resolve(null); //return remove this call
            }else{
                return Q.reject(null);
            }
        }

        // {retryAfter : retryAfter, mdsUrl : mdsUrl};
        if(sender.param && sender.param.retryAfter == -1){
            //reset Auth Promise and other data
            hx.log('mds','####### reset All Data on Http Filter ############');
            self.__lastAuthSender = null;
            self.__authPromise = null;
            self._promiseLoadingList = [];
        }

        if(sender.query==Query.REAUTH || sender.query == Query.BACKOFF || sender.query ==Query.RETRYAFTER){
            if(this.__lastAuthSender){
                reqSender=this.__lastAuthSender;
                query = reqSender.query;
                hx.log('mds','reqToAuth - '+sender.query);
            }else{
                hx.log('mds','ReAuth - query Error');
                return Q.reject({reason : 'getBroadcastURI'});
            }
        }else{
            reqSender = sender;
        }

        var cache = this._cacheController.get(category, query);
        var isByBackOff = false;
        var isByRetryAfter = sender.query == Query.RETRYAFTER ? true : false;

        if (cache) {
            switch(cache.status){
                case 400:
                case 406:
                    hx.log('mds','previous status 400 || 406');
                    return Q.reject(null);
                    break;
                case 500:
                case 502:
                case 503:
                case 504:
                    //check retry or backoff?
                    if(!isByRetryAfter) {
                        //back-Off
                        var paramKey = __cache_controller__.ICacheQueryParamKey;
                        if (sender.query == Query.BACKOFF) {
                            hx.log('mds', 'senMetaReq by BackOff');
                            isByBackOff = true;
                        }
                        //check ReqPromise & count
                        if (cache.param && cache.param[paramKey.ReqPromise] && cache.param[paramKey.ReTryCount] && !isByBackOff) {
                            //UI request back-offing query
                            hx.log('mds', 'This req already has been working by BackOff. please wait until backoff has done');
                            return cache.param[paramKey.ReqPromise];
                        }
                    }
                    break;
                default:
                    hx.log('mds','###### Auth status : '+cache.status);
                    break;
            }
        }

        if(this.__authPromise){
            this.__authPromise.then(function(res){
                    retDef.resolve(res);},
                function(res){
                    retDef.reject(res);}
            );
            if(isByBackOff || isByRetryAfter){
                hx.log('mds','Auth Promise ing.... but pre-auth failed. try re-auth');
            }else{
                hx.log('mds','Auth Promise ing............');
                return retDef.promise;
            }
        }

        hx.log('mds','Auth Promise req..');
        this.__lastAuthSender = reqSender;

        if(cache&&cache.status===404){
            /*
             * If a 404 (Not Found) HTTP response code is received on a request to the authentication service the device SHALL re-acquire
             * the basebroadcast_auth_url from the broadcast URI Linkage Descriptor as required by [FVPC_TECH] Section 5.3.
             * The device SHALL then retry the request. In the event that this retry also fails with a 404 response the device SHALL wait 24 hours before retrying.
             * The device SHALL retry the request once during any 24-hour period and the timeout MAY not be persisted across device reboots.
             * */
        }else{
            self._cacheController.set(category, query, null, 999, null);//999 => Loading with re-authing...
        }


        this.__authPromise = this.req(query,reqSender);
        if(isByBackOff && cache){
            self._cacheController.set(category, query, null, 999, cache.data,cache.param);//999 => Loading with re-authing...
        }

        this.__authPromise.then(function(res){
                 self.__authPromise = null;
                 hx.log('mds','Auth Promise Done ....');
                 retDef.resolve(res);
         },
         function(res){
                self.__authPromise = null;
                hx.log('mds','Auth Promise Fail ....');
                retDef.reject(res);
            }
        );

        return retDef.promise;
    }

    reqToMeta(sender:IHttpReqMDSParam){
        if (hx.emulMode) {
            return Q.reject(null); //request cancel
        }
        hx.log('mds','reqToMeta');
        if (!sender) {
            hx.log('mds','FAIL! sender is null');
            return Q.reject(null); //request cancel
        }
        var category = <string>sender.category || Category.META;
        var query = sender.query;
        var cache = this._cacheController.get(category, query);
        var self = this;
        var isBackOff = false;

        if(!self.__isAvailableToUseAuth()){
            hx.log('mds','reqToMeta Auth Status Error');
            if(sender.resDef) {
                return Q.resolve(null); //return remove this call
            }else{
                return Q.reject(null);
            }
        }

        if(!this._isEnableProxy){
            hx.log('mds','Not Availble Proxy');
            if(sender.resDef) {
                return Q.resolve(null); //return remove this call
            }else{
                return Q.reject(null);
            }
        }
        if (cache) {
            hx.log('mds','reqToMeta Cache Status : '+cache.status);
            switch(cache.status) {
                case 200:
                case 304:
                    //var cacheControl = cache.header['Cache-Control'];
                    //var date = cache.header['Date'];
                    var isExpired = this._isExpired(cache, sender);
                    if (isExpired) {
                        //send req
                        var If_Modified_Since = cache.header['Last-Modified'];
                        if (!sender.param) {
                            sender.param = {};
                        }
                        sender.param['If-Modified-Since'] = If_Modified_Since;
                        if (self._promiseLoadingList[query]) {
                            hx.log('mds','previous expired call is working. wait this call.');
                            return self._promiseLoadingList[query];
                        }
                    } else {
                        return Q.resolve(cache.data);
                    }
                    break;
                case 400:
                case 406:
                    hx.log('mds','previous status 400 || 406');
                    if(sender.resDef){
                        return Q.resolve(null); //return remove this call
                    }else{
                        return Q.reject(null); //request single cancel
                    }
                    break;
                case 401:
                case 403:
                case 404:
                    if (category === Category.AIT) {
                        hx.log('mds', 'the device SHOULD not retry about XML AIT ######');
                        if (sender.resDef){
                            return Q.resolve(null); //return remove this call
                        } else{
                            return Q.reject(null); //request single cancel
                        }
                    }
                    if(cache.header){
                        //need to check re-try time
                        var retryAfter = cache.header['Retry-After'];
                        var paramKey = __cache_controller__.ICacheQueryParamKey;
                        if(!retryAfter){
                            retryAfterF = 0;
                        }else{
                            retryAfterF = parseFloat(retryAfter);
                        }
                        var stampTime = cache.param[paramKey.ResDate];
                        var lastExpiredTime = (stampTime + (retryAfterF*1000));
                        var now =  Date.now();
                        var isRetryAfter = (lastExpiredTime <= now);
                        if(!isRetryAfter){
                            if(sender.resDef){
                                hx.log('mds','retryAfter previous status retrying.. remove this call');
                                return Q.resolve(null); //return remove this call
                            }else{
                                hx.log('mds','retryAfter previous status retrying.. It maybe duplicated request.');
                                return Q.reject(cache.param[paramKey.ResIJQueryXHR]); //return IJQueryXHRWithQuery as Failed
                            }
                        }else{
                            isRetryAfter = true;
                        }
                    }
                    break;
                case 500:
                case 502:
                case 504:
                    if (category === Category.SEARCH) {
                        hx.log('mds', 'the device SHOULD not retry about SEARCH ######');
                        if (sender.resDef){
                            return Q.resolve(null); //return remove this call
                        } else{
                            return Q.reject(null); //request single cancel
                        }
                    }
                    var paramKey = __cache_controller__.ICacheQueryParamKey;
                    if(sender.param&&sender.param[Query.BACKOFF]){
                        hx.log('mds','senMetaReq by BackOff');
                        isBackOff = true;
                    }
                    //check ReqPromise & count
                    if(cache.param&& cache.param[paramKey.ReqPromise] && cache.param[paramKey.ReTryCount] && !isBackOff){
                        //UI request back-offing query
                        hx.log('mds','This Meta req already has been working by BackOff. please wait until backoff has done : '+cache.param[paramKey.ReTryCount]);
                        if(sender.resDef){
                            hx.log('mds','From List');
                            //From List Request
                            cache.param[paramKey.ReqPromise].then(function(ret){
                                sender.resDef.resolve(ret);
                            },function(ret){
                                sender.resDef.reject(ret);
                            });
                            return Q.resolve(null);
                        }else{
                            hx.log('mds','From single req');
                            return Q.resolve(null); //request cancel
                        }
                    }else if(!isBackOff && (cache.param[paramKey.ReqPromise]==null)){
                        hx.log('mds','From UI, request query (back-off need)');
                        isBackOff = true;
                    }
                    break;
                case 503:
                    if (category === Category.SEARCH) {
                        hx.log('mds', 'the device SHOULD not retry about SEARCH ######');
                        if (sender.resDef){
                            return Q.resolve(null); //return remove this call
                        } else{
                            return Q.reject(null); //request single cancel
                        }
                    }
                    //check retry or backoff?
                    if(cache.header){
                        //need to check re-try time
                        //Re-Auth
                        var retryAfter = cache.header['Retry-After'];
                        if(retryAfter){
                            var retryAfterF = parseFloat(retryAfter);
                            if(retryAfterF){
                                var paramKey = __cache_controller__.ICacheQueryParamKey;
                                var stampTime = cache.param[paramKey.ResDate];
                                var lastExpiredTime = (stampTime + (retryAfterF*1000));
                                var now =  Date.now();
                                var isRetryAfter = (lastExpiredTime <= now);
                                if(!isRetryAfter){
                                    if(sender.resDef){
                                        hx.log('mds','503 previous status retrying.. remove this call');
                                        return Q.resolve(null); //return remove this call
                                    }else{
                                        hx.log('mds','503 previous status retrying.. It maybe duplicated request.');
                                        return Q.reject(cache.param[paramKey.ResIJQueryXHR]); //return IJQueryXHRWithQuery as Failed
                                    }
                                }else{
                                    isRetryAfter = true;
                                }
                            }
                        }else{
                            var paramKey = __cache_controller__.ICacheQueryParamKey;
                            if(sender.param&&sender.param[Query.BACKOFF]){
                                hx.log('mds','sendAuth by BackOff');
                                isBackOff = true;
                            }
                            //check ReqPromise & count
                            if(cache.param&& cache.param[paramKey.ReqPromise] && cache.param[paramKey.ReTryCount] && !isBackOff){
                                //UI request back-offing query
                                hx.log('mds','This Meta 503 req already has been working by BackOff. please wait until backoff has done');
                                if(sender.resDef){
                                    //From List Request
                                    cache.param[paramKey.ReqPromise].then(function(ret){
                                        sender.resDef.resolve(ret);
                                    },function(ret){
                                        sender.resDef.reject(ret);
                                    });
                                    //return Q.reject(cache.param[paramKey.ResIJQueryXHR]);
                                    return Q.resolve(null); //request cancel in list call
                                }else{
                                    hx.log('mds','503 From single req');
                                    return Q.reject(null); //request single cancel
                                }
                            }else if(!isBackOff && (cache.param[paramKey.ReqPromise]==null)){
                                hx.log('mds','From UI, request query (back-off need)');
                                isBackOff = true;
                            }
                        }
                    }
                    break;
                case 999:
                    hx.log('mds','previous status 999 It maybe duplicated request.');
                    if(self._promiseLoadingList[query]){
                        return self._promiseLoadingList[query];
                    }else{
                        return Q.reject(null);
                    }
                    break;
                default:
                    hx.log('mds','######  status : '+cache.status);
                    break;
            }
        }else{
            hx.log('mds','reqToMeta NoCache');
            self._cacheController.set(category, query, null, 999, null);//999 => Loading with NoCache
        }

        hx.log('mds','reqToMeta query : '+sender.query);

        if(self._promiseLoadingList[query] && !isBackOff){
            hx.log('mds','This request would be UI request and previous Request to Http has not done... reject this call');
            return Q.resolve(null); //return remove this call
        }

        var retPromise = this.req(query,sender);
        var paramKey = __cache_controller__.ICacheQueryParamKey;

        if(isBackOff || (cache && cache.header && cache.header['Retry-After'])){
            if(!cache.param[paramKey.ReqPromise]){
                cache.param[paramKey.ReqPromise] = retPromise;//until status to be changed as success
                self._cacheController.set(category, query, null, cache.status, cache.data,cache.param);
            }
        }

        if(!self._promiseLoadingList[query] && !isBackOff){
            hx.log('mds','###### Add Meta Loading Promise ######');
            self._promiseLoadingList[query] = retPromise; //add loading to Query
        }

        retPromise.then(function(res){
            hx.log('mds','###### Meta Loading Promise DONE ######');
            self._promiseLoadingList[query] = null;
            return res;
        },function(res){
            hx.log('mds','###### Meta Loading Promise FAIL ######');
            self._promiseLoadingList[query] = null;
            return res;
        });

        return retPromise;
    }

    __backOff(sender:IHttpReqMDSParam,res : IJQueryXHRWithQuery) : Q.Promise<any>{
        var category = <string>sender.category;
        var hash = sender.query;
        var self = this;
        var retDef = Q.defer();
        var retryCount : number = 0;

        var cache = this._cacheController.get(category, hash);
        var paramKey = __cache_controller__.ICacheQueryParamKey;

        //if (sender.param && sender.param[Query.BACKOFF] == null) { //if UI request
        //    if (!(cache.param && cache.param[paramKey.ReqPromise] && cache.param[paramKey.ReTryCount]) ){
        //        var backOffPromise = cache.param[paramKey.ReqPromise];
        //        backOffPromise.than(function(ret){
        //            retDef.resolve(ret);
        //        },function(ret){
        //            retDef.reject(ret);
        //        });
        //        return retDef.promise;
        //    } else {
        //        hx.log('mds','UI Request & no back-off running .... :  '+sender.query);
        //    }
        //}

        hx.log('mds','List Back Off query : '+sender.query);

        if(cache) {
            if (cache){
                switch (cache.status){
                    case 500:
                    case 502:
                    case 504:
                        if(cache.param && cache.param[paramKey.ReTryCount]){
                            cache.param[paramKey.ReTryCount] += 1;
                            retryCount = cache.param[paramKey.ReTryCount];
                        }else{
                            if (!cache.param) {
                                cache.param = [];
                            }
                            cache.param[paramKey.ReTryCount] = 1;
                            retryCount = 1;
                        }
                    break;
                    case 503:
                            var retry_After = res.getResponseHeader('Retry-After');
                            if(retry_After){
                                retDef.reject(null);
                                return retDef.promise;
                            }else{
                                if(cache.param && cache.param[paramKey.ReTryCount]){
                                    cache.param[paramKey.ReTryCount] += 1;
                                    retryCount = cache.param[paramKey.ReTryCount];
                                }else{
                                    if (!cache.param) {
                                        cache.param = [];
                                    }
                                    cache.param[paramKey.ReTryCount] = 1;
                                    retryCount = 1;
                                }
                            }
                    break;
                    default :
                        retDef.reject(null);
                        return retDef.promise;
                }
            }
        }else{
            hx.log('mds','No Cache Back Off Error ############');
            retDef.reject(null);
            return retDef.promise;
        }

        this._cacheController.set(category, hash, null, cache.status, cache.data,cache.param);

        function doNext(){
            var nextDelayTime = Math.pow(4,retryCount)*100;//until 104857600
            hx.log('mds','Do Next List status('+res.status+') next delay :'+nextDelayTime);

            setTimeout(function(){
                if(!sender.param){
                    sender.param = {};
                }
                sender.param[Query.BACKOFF] = retryCount;
                self.reqToMeta(sender).then(
                    function(res){
                        hx.log('mds','Do Next List status('+res.status+') query : '+res.query);
                        retDef.resolve(res);//data
                    },
                    function(ret){
                        if(ret == null){
                            hx.log('mds','Meta Fail from 500, 503, 504 ret null backOff Call ######');
                            self.__backOff(sender, res).then(function (ret) {
                                retDef.resolve(ret);
                            },function(ret){
                                retDef.reject(ret);
                            });
                            return;
                        }
                        hx.log('mds','Meta Fail from 500,503, 504 query : '+ret.query + ',  retryCount : '+retryCount);
                        if(retryCount<=10) {
                            hx.log('mds','Meta Fail from 500,503, 504 query reAuth : '+retryCount);
                            self.__backOff(sender, ret).then(function (ret) {
                                retDef.resolve(ret);
                            },function(ret){
                                retDef.reject(ret);
                            });
                        }else{
                            retDef.reject(ret);
                        }
                    }
                );
            },nextDelayTime+1000);
        }
        switch(res.status) {
            case 500:
            case 502:
            case 504:
                if (category === Category.META) {
                    hx.log('mds','Meta trying Re-Auth from 500,502,504 ....');
                    var re_authQuery : IHttpReqMDSParam = {
                        senderF : null, query : Query.REAUTH ,param : null, category : Category.AUTH
                    };
                    self.reqToAuth(re_authQuery).then(
                        function () {
                            doNext();
                        }, function () {
                            if(retryCount>=10){
                                retDef.reject(res);
                            }
                        }
                    );
                } else {
                    hx.log('mds','Meta trying without Re-Auth without metadata endpoint....');
                    doNext();
                }
                break;
            case 503:
                if (category === Category.SEARCH) {
                    hx.log('mds','NOT retry on SEARCH from 503');
                    retDef.reject(res);
                    break;
                }
                hx.log('mds','Meta trying without Re-Auth from 503 ....');
                doNext();
                break;
            default:
                break;
        }
        return retDef.promise;
    }

    reqToMetaList(senderList:IHttpReqMDSParam[]):any {
        if (hx.emulMode) {
            return Q.reject(null); //request cancel
        }
        var promiseList :Q.Promise<any>[] = [];
        var self = this;
        var category : string;
        var retDef = Q.defer();

        hx.log('mds','reqToMetaList');

        if(!self._isEnableProxy){
            hx.log('mds','reqToMetaList Not Availble Proxy');
            senderList.forEach(function(sender:IHttpReqMDSParam){
                sender.resDef.reject(null);
            });
            retDef.reject(null);
            return retDef.promise;
        }

        senderList.forEach(function(sender:IHttpReqMDSParam){
            category = <string>sender.category || Category.META;
            promiseList.push(self.reqToMeta(sender).then(function(res){
                sender.resDef.resolve(res);
            }));
        });

        function waitPromiseList(requestedPromises:Q.Promise<any>[]){
            function listRetry(res : IJQueryXHRWithQuery) : Q.Promise<any> { //IJQueryXHRWithQuery
                var retryDef = Q.defer();
                var retry_After = parseFloat(res.getResponseHeader('Retry-After'));
                if(!retry_After){
                    retry_After = 0;
                }
                var waitTime = retry_After*1000;
                hx.log('mds','listRetry retry-After : '+retry_After+ ' , query : '+res.query);
                function doNext(){
                    senderList.every(function(sender){
                        if(res.query == sender.query){
                            var re_sender = sender;
                            if(re_sender.param){
                                re_sender.param['Retry-After'] = retry_After;
                            }else{
                                re_sender.param = {
                                    'Retry-After' : retry_After
                                };
                            }
                            self.reqToMeta(re_sender).then(function (res) {
                                    retryDef.resolve(res);
                                    re_sender.resDef.resolve(res);
                                },
                                function (err) {
                                    retryDef.reject(err);
                                    re_sender.resDef.reject(err);
                                }
                            );
                            return false;
                        }else{
                            return true;
                        }
                    });
                }
                switch(res.status){
                    case 403:
                    case 404:
                        var re_authQuery : IHttpReqMDSParam = {
                            senderF : null, query : Query.REAUTH ,param : null, category : Category.AUTH
                        };
                        self.reqToAuth(re_authQuery).then(
                            function(){
                                hx.log('mds','ReAuth Success');
                                senderList.every(function(sender){
                                    if(res.query == sender.query){
                                        var re_sender = sender;
                                        if(re_sender.param){
                                            re_sender.param['Retry-After'] = retry_After;
                                        }else{
                                            re_sender.param = {
                                                'Retry-After' : retry_After
                                            };
                                        }

                                        self.reqToMeta(re_sender).then(function (res) {
                                                hx.log('mds','waitPromiseList 403 Meta Success');
                                            },
                                            function (err) {
                                                hx.log('mds','waitPromiseList 403,404 FAIL 24 hours waiting....');
                                                var waitTime = TimeSec.T24h*1000;
                                                //set 24 hours to Cache Data
                                                //var category : string = Category.META;
                                                var query = re_sender.query;
                                                var cache = self._cacheController.get(category, query);
                                                cache.header['Retry-After'] = TimeSec.T24h;
                                                self._cacheController.setCache(category, re_sender.query,cache);
                                                setTimeout(function() {
                                                    hx.log('mds','start 403 with 24 hours for retrying');
                                                    self.reqToMeta(re_sender).then(function(ret){
                                                            hx.log('mds','waitPromiseList reqToMeta Success in 24 hours');
                                                        },
                                                        function(ret){
                                                            hx.log('mds','waitPromiseList reqToMeta Fail... in 24 hours');
                                                        });
                                                },waitTime+1000);
                                            }
                                        );
                                        return false;
                                    }else{
                                        return true;
                                    }
                                });
                            }
                        );
                        break;
                    case 401:
                    case 503:
                        setTimeout(function(){
                            if(res.status==503){
                                doNext();
                            }else{
                                var re_authQuery : IHttpReqMDSParam = {
                                    senderF : null, query : Query.REAUTH ,param : null, category : Category.AUTH
                                };
                                self.reqToAuth(re_authQuery).then(
                                    function(){
                                        hx.log('mds','ReAuth Success');
                                        doNext();
                                    }
                                );
                            }
                        },waitTime+1000);
                        break;
                }
                return retryDef.promise;
            }

            function listBackOff(res : IJQueryXHRWithQuery) : Q.Promise<any>{
                var retryDef = Q.defer();
                senderList.every(function(sender){
                    if(res.query == sender.query){
                        hx.log('mds','List ReQuery : '+sender.query);
                        self.__backOff(sender,res).then(
                            function(ret){
                                hx.log('mds','List ReQuery done');
                                retryDef.resolve(ret);
                            },
                            function(ret){
                                hx.log('mds','List ReQuery fail : '+ret.query);
                                retryDef.reject(ret);
                            }
                        );
                        return false;
                    }else{
                        return true;
                    }
                });
                return retryDef.promise;
            }

            return Q.allSettled(requestedPromises).then(function (retPromises) {
                hx.log('mds','######## reqToMetaList then Done');
                var rejected : boolean = false;
                var reTryPromises :Q.Promise<any>[] = [];
                retPromises.every(function(retPromise){
                    hx.log('mds','reqToMetaList state : '+retPromise.state);
                    if(retPromise && retPromise.state == 'rejected' && retPromise.reason){
                        var res = retPromise.reason;
                        var status = res.status;
                        switch(status){
                            case 401:
                            case 403:
                            case 404:
                                if (category !== Category.AIT) {
                                    //ReTry-After
                                    reTryPromises.push(listRetry(res));
                                }
                                rejected = true;
                                return true;
                            case 500:
                            case 502:
                            case 504:
                                //BackOff
                                if (category !== Category.SEARCH) {
                                    reTryPromises.push(listBackOff(res));
                                }
                                rejected = true;
                                return true;
                            case 503:
                                var retry_After = parseFloat(res.getResponseHeader('Retry-After'));
                                if (category !== Category.SEARCH) {
                                    reTryPromises.push((retry_After>=0)?listRetry(res):listBackOff(res));
                                }
                                rejected = true;
                                return true;
                        }
                        return true;
                    }else if(retPromise && retPromise.state == 'rejected'){
                        rejected = true;
                    }
                    else if(retPromise.state == 'fulfilled'){
                        return true;
                    }
                    //loading ....
                    return true;
                });

                if(!rejected){
                    //each request would be done but It doesn't mean all
                    hx.log('mds','############ reqToMetaList Succeed ....');
                    retDef.resolve(retPromises);
                }else{
                    //may be auth fail... auth and than re-try
                    if(reTryPromises.length==0){
                        hx.log('mds','############ reqToMetaList rejected .... reTryPromises 0');
                        /*
                        var re_authQuery:IHttpReqMDSParam = {
                            senderF: null, query: Query.REAUTH, param: null, category: Category.AUTH
                        };
                        senderList.forEach(function(sender:IHttpReqMDSParam){
                            reTryPromises.push(self.reqToMeta(sender).then(function(res){
                                sender.resDef.resolve(res);
                            }));
                        });
                        self.reqToAuth(re_authQuery).then(function(){
                            waitPromiseList(reTryPromises);
                        });
                        */
                    }else{
                        hx.log('mds','############ reqToMetaList rejected .... reTryPromises');
                        waitPromiseList(reTryPromises);
                    }
                    retDef.reject(null);
                }
            },function (retPromises) {
                hx.log('mds','reqToMetaList Failed ....');
            }).fail(function(e){
                hx.log('mds','Promise get list error : '+e);
            });
        }
        waitPromiseList(promiseList);
        return retDef.promise;
    }

    private imgCachingPromise : Q.Deferred<any>;//promise checking cached expire time on image

    //TL request
    //Limit 300 numbers
    //delete expired images
    __checkImageCaching(delay? : number){
        var self = this;
        var LIMIT_CACHE = 300;
        var waitingTIme = 0;

        if(self.imgCachingPromise){
            return self.imgCachingPromise;
        }

        if(delay>=0){
            waitingTIme = delay;
        }else{
            waitingTIme = TimeSec.T1h*1000;
        }

        self.imgCachingPromise = Q.defer();

        setTimeout(function(){
            var imageCache = self._cacheController.getDb()[Category.IMAGE];
            var queryList = Object.keys(imageCache);
            var imgNumb = queryList.length;
            var expiredImgNum = 0;
            if(imgNumb>LIMIT_CACHE){
                hx.log('debug','free all cached images');
                hx.log('debug','free all cached images');
                hx.log('debug','free all cached images');
                hx.log('debug','free all cached images');
                //checking oldest data;
                var oldestOne;
                var oldestKey;
                queryList.forEach(function(key){
                    var cache = imageCache[key];
                    var paramKey = __cache_controller__.ICacheQueryParamKey;
                    if(oldestOne && oldestOne.param[paramKey.ResDate]>= cache.param[paramKey.ResDate]){
                        oldestOne = cache;
                        oldestKey = key;
                    }else{
                        oldestOne = cache;
                        oldestKey = key;
                    }
                });
                if(oldestKey){
                    hx.log('mds','__checkImageCaching image del :'+oldestKey);
                    self._cacheController.del(Category.IMAGE,oldestKey);
                }
                self.imgCachingPromise = null;
                self.__checkImageCaching(0);
            }

            queryList.forEach(function(key){
                var cache = imageCache[key];
                if(self._isExpired(cache)){
                    expiredImgNum++;
                    self._cacheController.delData(Category.IMAGE,key);
                }
            });

            if(expiredImgNum>0){
                hx.log('mds','__checkImageCaching image Total : '+imgNumb + ' expiredImgNum : '+expiredImgNum);
            }
            self.imgCachingPromise = null;
            self.__checkImageCaching();
        },waitingTIme+1000);

        return self.imgCachingPromise;
    }

    reqToImage(sender:IHttpReqMDSParam){
        if (hx.emulMode) {
            return Q.reject(null); //request cancel
        }
        hx.log('mds','reqToImage');
        var category : string = Category.IMAGE;
        var query = sender.query;
        var cache = this._cacheController.get(category, query);
        var self = this;
        var isBackOff = false;

        //call cache check
        self.__checkImageCaching();

        if(!self._isEnableProxy){
            hx.log('mds','reqToImage Not Availble Proxy');
            if(sender.resDef) {
                return Q.resolve(null); //return remove this call
            }else{
                return Q.reject(null);
            }
        }

        if(!self.__isAvailableToUseAuth()){
            hx.log('mds','reqToImage Auth Status Error');
            return Q.reject(null);
        }

//check before sending
        if (cache) {
            hx.log('mds','reqToImage cache status : '+cache.status);
            switch(cache.status){
                case 200:   // OK
                case 304:   // Not Modified
                    var isExpired = this._isExpired(cache,sender);
                    if (isExpired) {
                        //send req
                        var If_Modified_Since = cache.header['Last-Modified'];
                        if (!sender.param) {
                            sender.param = {};
                        }
                        sender.param['If-Modified-Since'] = If_Modified_Since;
                        //delete Cache data
                        self._cacheController.delData(category,query);
                    } else {
                        return Q.resolve(cache.data);
                    }
                    break;
                case 400:   // Bad Request
                case 401:   // Authentication Required
                case 403:   // Forbidden
                case 404:   // Not Found
                case 406:   // Not Acceptable
                case 500:   // System Error
                case 502:   // Bad Gateway
                case 504:   // Gateway Timeout
                    hx.log('mds','the client device SHALL NOT retry the request and deem it to have failed.');
                    return Q.reject(null);
                case 503:   // Service unavailable
                    //check retry or backoff?
                    if(cache.header){
                        //need to check re-try time
                        //Re-Auth
                        var retryAfter = cache.header['Retry-After'];
                        if(retryAfter){
                            var retryAfterF = parseFloat(retryAfter);
                            if(retryAfterF){
                                var paramKey = __cache_controller__.ICacheQueryParamKey;
                                var stampTime = cache.param[paramKey.ResDate];
                                var lastExpiredTime = (stampTime + (retryAfterF*1000));
                                var now =  Date.now();
                                var isRetryAfter = (lastExpiredTime <= now);
                                if(!isRetryAfter){
                                    if(sender.resDef){
                                        hx.log('mds','503 previous status retrying.. remove this call');
                                        return Q.resolve(null); //return remove this call
                                    }else{
                                        hx.log('mds','503 previous status retrying.. It maybe duplicated request.');
                                        return Q.reject(cache.param[paramKey.ResIJQueryXHR]); //return IJQueryXHRWithQuery as Failed
                                    }
                                }else{
                                    isRetryAfter = true;
                                }
                            }
                        }else{
                            var paramKey = __cache_controller__.ICacheQueryParamKey;
                            if(sender.param && sender.param[Query.BACKOFF]){
                                hx.log('mds','send Req Image by BackOff');
                                isBackOff = true;
                            }
                            //check ReqPromise & count
                            if(cache.param&& cache.param[paramKey.ReqPromise] && cache.param[paramKey.ReTryCount] && !isBackOff){
                                //UI request back-offing query
                                hx.log('mds','This Meta 503 req already has been working by BackOff. please wait until backoff has done');
                                if(sender.resDef){
                                    //From List Request
                                    cache.param[paramKey.ReqPromise].then(function(ret){
                                        sender.resDef.resolve(ret);
                                    },function(ret){
                                        sender.resDef.reject(ret);
                                    });
                                    //return Q.reject(cache.param[paramKey.ResIJQueryXHR]);
                                    return Q.resolve(null); //request cancel in list call
                                }else{
                                    hx.log('mds','503 From single req');
                                    return Q.reject(null); //request single cancel
                                }
                            }else if(!isBackOff && (cache.param[paramKey.ReqPromise]==null)){
                                hx.log('mds','From UI, request query (back-off need)');
                                isBackOff = true;
                            }
                        }
                    }
                    break;
                case 999:
                    hx.log('mds','Image previous status 999 It maybe duplicated request.');
                    if(self._promiseLoadingList[query]){
                        return self._promiseLoadingList[query];
                    }else{
                        return Q.reject(null);
                    }
                    break;
            }
        }else{
            hx.log('mds','reqToMeta NoCache');
            self._cacheController.set(category, query, null, 999, null);//999 => Loading with NoCache
        }

        if(self._promiseLoadingList[query] && !isBackOff){
            hx.log('mds','This Image request has not done... reject this call');
            return Q.resolve(null); //return remove this call
        }

        var retPromise = this.req(query,sender);
        var paramKey = __cache_controller__.ICacheQueryParamKey;

        if(isBackOff || (cache && cache.header && cache.header['Retry-After'])){
            if(!cache.param[paramKey.ReqPromise]){
                cache.param[paramKey.ReqPromise] = retPromise;//until status to be changed as success
                self._cacheController.set(category, query, null, cache.status, cache.data,cache.param);
            }
        }

        if(!self._promiseLoadingList[query] && !isBackOff){
            hx.log('mds','###### Add Image Loading Promise ######');
            self._promiseLoadingList[query] = retPromise; //add loading to Query
        }

        retPromise.then(function(res){
            hx.log('mds','###### Image Loading Promise DONE ######');
            if(self._promiseLoadingList[query]){
                self._promiseLoadingList[query] = null;
            }
            return res;
        },function(res){
            hx.log('mds','###### Image Loading Promise FAIL ######');
            if(self._promiseLoadingList[query]){
                self._promiseLoadingList[query] = null;
            }
            return res;
        });

        return retPromise;
    }

    setEnableProxy(enable:boolean):Q.Promise<any> {
        var ret = Q.defer();
        hx.log('mds','setEnableProxy : '+enable);
        this._isEnableProxy = enable;
        ret.resolve(true);

        return ret.promise;
    }
    isAvailableToUseAuth():boolean {
        return this.__isAvailableToUseAuth();
    }
}
