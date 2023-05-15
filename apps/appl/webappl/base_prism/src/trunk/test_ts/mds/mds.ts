/**
 * Created by bglee on 2015-03-20.
 */
/// <reference path="./q.d.ts" />
/// <reference path="../../def/jquery.d.ts" />

interface IMDSReqHeader {
    "Accept"?: string;
    "x-auth-hash"?: string;
    "x-auth-timestamp"?: string;
}
interface IMDSResAuth {
    "x-baseurl": string;
    "x-auth-hash": string;
    "x-auth-timestamp": string;
}

function __getAuthToken(url ='https://auth-ctv.cp-test.digitaluk.co.uk') {
    var deferred = Q.defer();
    $.ajax({
        type: 'head',
        url: url,
        success: function(str, num, res) {
            console.log('success: auth ');
            deferred.resolve({
                "x-baseurl": res.getResponseHeader('x-baseurl'),
                "x-auth-hash": res.getResponseHeader('x-auth-hash'),
                "x-auth-timestamp": res.getResponseHeader('x-auth-timestamp')
            });
        },
        error: function(error) {
            console.log('fail: auth');
            deferred.reject(error);
        }
    });
    return deferred.promise;
}
interface IFileReader extends FileReader {
    __plantedIndex: number;
}
interface IPoolFileManager {
    deferred: Q.Deferred<any>;
    fr: IFileReader;
}
class CMDSPoolFileReader {
    private _pool: IPoolFileManager[] = [];
    private _poolAvailable: IPoolFileManager[] = [];
    constructor(count = 6) {
        var self = this;
        for (var i = 0; i < count; i++) {
            var fr = <IFileReader>new FileReader();
            fr.__plantedIndex = i;
            fr.onload = function() {
                var manager = self._pool[this.__plantedIndex];
                manager.deferred.resolve(this.result);
                manager.deferred = null;
                self._poolAvailable.push(manager);
            };
            this._pool[i] = {
                deferred: null,
                fr: fr
            };
            this._poolAvailable[i] = this._pool[i];
        }
    }
    getDataURL(blob: Blob): Q.Promise<any> {
        var manager = this._poolAvailable.shift();
        var deferred = Q.defer();
        if (!blob) {
            deferred.reject('arguments 0 must be blob object');
        } else if (!manager) {
            deferred.reject('file manager busy');
        } else {
            manager.deferred = deferred;
            manager.fr.readAsDataURL(blob);
        }
        return deferred.promise;
    }
}
class CMDSImageManager {
    private _cache = {};
    private _poolFr = new CMDSPoolFileReader();
    private _header;
    private _url_base;

    constructor() {
    }
    private _requestURL(query) {
        var xhr = new XMLHttpRequest();
        var deferred = Q.defer();
        var self = this;
        xhr.onload = function() {
            if (this.readyState === 4) {
                var plantedQuery = (<any>this).__plantedQuery;
                var urlCached = this._getUrl(plantedQuery);
                if (this.status === 200) {
                    self._poolFr.getDataURL(this.response).done((url) => {
                        if (urlCached) {
                            console.log('url cache updated');
                        }
                        this._setCacheURL(plantedQuery, url);
                        deferred.resolve(url);
                    }, deferred.reject);
                } else if (this.status === 304) {
                    deferred.resolve(urlCached);
                } else if (urlCached) {
                    deferred.resolve(urlCached)
                } else {
                    deferred.reject(this.status);
                }
            }
        };
        xhr.open('GET', this._url_base + query, true);
        for (var key in this._header) {
            xhr.setRequestHeader(key, this._header[key]);
        }
        (<any>xhr).__plantedQuery = query;
        xhr.responseType = 'blob';
        xhr.send();
        return deferred.promise;
    }
    private _setCacheURL(query: string, url: string) {
        this._cache[query] = url;
    }
    private _getURL(query: string) {
        return this._cache[query];
    }
    setHeader(header) {
        this._header = header;
    }
    setBaseURL(url: string) {
        this._url_base = url;
    }
    getImage(query: string) {
        if (!this._header) {
            this.onRequestHeader();
        }
        return this._requestURL(query);
    }
    truncate() {
        this._cache = {};
    }
    _requestHeader() {
        var header = this.onRequestHeader();
        if (header) {
            throw '_requestHeader fail';
        }
        this._header = header;
    }
    onRequestHeader: () => IMDSReqHeader;
}
class CMDSApi {
    private _header: IMDSReqHeader = {};
    private _url_base;
    private _url_auth;
    private _managerImage = new CMDSImageManager();
    constructor(urlAuth = 'https://auth-ctv.cp-test.digitaluk.co.uk') {
        this._url_auth = urlAuth;
        this.renewAuthInfo(urlAuth)
            .done(function() {
                console.log('success: renew auth info');
            }, function() {
                console.log('fail: renew auth info');
            });
        this._managerImage.onRequestHeader = () => {
            return this._getHeader();
        }
    }
    private _setBaseURL(url) {
        this._url_base = url;
    }
    private _getVersion() {
        if (!this._getBaseURL()) {
            var deferred = Q.defer();
            deferred.reject('need base url');
            return deferred.promise;
        }
        return this._call(this._getBaseURL() + '/version.json');
    }
    private _getBaseURL(): string {
        return this._url_base;
    }
    private _getHeader(): IMDSReqHeader {
        return this._header;
    }
    private _call(url, deferred?: Q.Deferred<any>) {
        var deferred = deferred || Q.defer();
        $.ajax({
            type: 'get',
            url: url,
            headers: <any>this._getHeader(),
            success: deferred.resolve,
            error: deferred.reject
        });
        return deferred.promise;
    }
    renewAuthInfo(urlAuth: string) {
        var deferred = Q.defer();
        __getAuthToken(urlAuth).done(
            (authInfo: IMDSResAuth) => {
                this._setBaseURL(authInfo['x-baseurl']);
                this.setHeader('x-auth-hash', authInfo['x-auth-hash']);
                this.setHeader('x-auth-timestamp', authInfo['x-auth-timestamp']);
                this._getVersion().done(
                    (json) => {
                        this.setHeader('Accept', 'application/vnd.fvc.v' + json.version[0] + '+xml');
                        this._managerImage.setHeader(this._getHeader());
                        deferred.resolve(authInfo);
                    },
                    deferred.reject
                );
                this._managerImage.setBaseURL(authInfo['x-baseurl'].replace('http://api', 'http://img'));
                this._managerImage.setHeader(this._getHeader());
            },
            deferred.reject
        );
        return deferred.promise;
    }
    setHeader(property: string, value: any) {
        console.log('setHeader: ' + property + ' | value: ' + value);
        this._header[property] = value;
    }
    getBaseURL() {
        return this._getBaseURL();
    }
    xml(query: string): Q.Promise<any> {
        if (!this._getBaseURL()) {
            var deferred = Q.defer();
            this.renewAuthInfo(this._url_auth).done(() => {
                this._call(this._getBaseURL() + query, deferred)
            });
            return deferred.promise;
        }
        return this._call(this._getBaseURL() + query);
    }
    image(query: string): Q.Promise<any> {
        return this._managerImage.getImage(query);
    }
}
(<any>window).CMdSApi = CMDSApi;
export = CMDSApi;