/**
 * Created by bglee on 2015-04-25.
 */
export interface ICacheHeader {
    'Cache-Control'?: string;
    'Expires'?: string;
    'Last-Modified'?: string;
    'Date'?: string;
}

export class ICacheQueryParamKey{
    static ReTryCount : string ='reTryCount';
    static ReqPromise : string ='promiseRet';
    static ResIJQueryXHR : string = 'IJQueryXHRWithQuery';
    static ResDate : string = 'resDate';
}


export interface ICacheQuery {
    header: ICacheHeader;
    status: number;
    data: any;
    param?:ICacheQueryParamKey[];
}


export interface ICacheQueryObject {
    [query: string]: {[category: string]: ICacheQuery};
}

function __isExpiresExpired(cacheInfo: ICacheHeader) {
    var expires = cacheInfo['Expires'];
    var ret = !!expires && new Date(expires).valueOf() <= Date.now();
    console.info('__isExpiresExpired: ' + ret);
    return ret;
}
function __isMaxAgeExpired(cacheInfo: ICacheHeader) {
    var date = cacheInfo['Date'];
    var cacheControl = cacheInfo['Cache-Control'];
    if (!date || !cacheControl) {
        return false;
    }
    var maxAge = /^max-age=([0-9]+)/.exec(cacheControl);
    if (!(maxAge && maxAge[1])) {
        return false;
    }
    var ret = new Date(date).valueOf() + (parseInt(maxAge[1]) * 1000) <= Date.now();
    console.info('__isMaxAgeExpired: ' + ret);
    return ret;
}
function __stringToHeader(header: string) {
    var ret = {};
    if (header) {
        var splitted = header.split('\n');
        splitted.forEach(function(header) {
            var data = header.split(': ');
            if (data[0] && data[1]) {
                ret[data[0]] = data[1];
            }
        });
    }
    return ret;
}

export class CCacheController {
    private _cache: ICacheQueryObject = {};
    private _categories : string[];
    public static CACHE_DB_NAME : string = 'IP_EPG_CACHE_DB';

    constructor(categories: string[], prevCache?: ICacheQueryObject) {
        if (prevCache) {
            categories.forEach((category) => {
                this._cache[category] = prevCache[category] || {};
            });
        } else {
            categories.forEach((category) => {
                this._cache[category] = {};
            })
        }

        this._categories = categories;
    }

    set(category: string, hash: string, header: string, status:number, data: any, param?:ICacheQueryParamKey[] | any) {
        if(!this._cache[category]){
            this._cache[category]= {};
        }

        this._cache[category][hash] = {
            header: header? __stringToHeader(header) :  (this._cache[category][hash] ? this._cache[category][hash].header : null),
            status: status,
            data: data || (this._cache[category][hash] ? this._cache[category][hash].data : null),
            param : this._cache[category][hash] ? this._cache[category][hash].param : []
        };

        var p = this._cache[category][hash].param;
        if(p){
            var keyList = Object.keys(p);
            keyList.forEach((key) => {
                if(p[key] && param && param[key]){
                    p[key] = param[key] ? param[key] : p[key];
                }
            })
        }
        return this;
    }
    get(category: string, hash: string): ICacheQuery {
        if(!this._cache[category]){
            console.log('error cache set Error category '+category);
            this._cache[category] = {};
        }

        return this._cache[category][hash] || null;
    }

    setCache(category:string, hash:string,cache:ICacheQuery){
        if(!this._cache[category]){
            console.log('error cache set Error category '+category);
            this._cache[category] = {};
        }
        this._cache[category][hash] = cache;
    }

    del(category: string, hash: string): CCacheController {
        this._cache[category][hash] = null;
        delete this._cache[category][hash];
        return this;
    }

    delData(category: string, hash: string){
        if(this._cache[category] && this._cache[category][hash]){
            this._cache[category][hash].data = null;
        }
        return this;
    }

    isExpired(header: ICacheHeader): boolean {
        return __isMaxAgeExpired(header) || __isExpiresExpired(header);
    }
    getDb() {
        return this._cache;
    }
    setDb(db: ICacheQueryObject) {
        console.warn('cache db will be overrided');
        if (db) {
            this._categories.forEach((category) => {
                if(db[category]){
                    this._cache[category] = db[category];
                }
            })
        }
        return this;
    }

    removeAll(){
        if(this._categories){
            this._categories.forEach((category) => {
                if(this._cache[category]){
                    var cache = this._cache[category];
                    var dataQueryList = Object.keys(cache);
                    dataQueryList.forEach(function(query){
                        var data = cache[query];
                        var dataProperyList = Object.keys(data);
                        dataProperyList.forEach((property)=>{
                            if(data[property] instanceof XMLDocument){
                                //remove XML Root
                                data[property].removeChild(data[property].documentElement);
                                data[property] = null;
                                delete data[property];
                            }else{
                                data[property] = null;
                                delete data[property];
                            }
                        });
                        cache[query] = null;
                        delete cache[query];
                    });
                }
            });

            console.log('cache_controller removeAll done');
        }
    }

    util = {
        string2Header: __stringToHeader
    };
}
