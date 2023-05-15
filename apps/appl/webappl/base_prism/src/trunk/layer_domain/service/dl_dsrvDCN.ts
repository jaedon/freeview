/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvTemplate.js
//    DCN: ['layer_domain/service/dl_dsrvDCN.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

class cDLDCN extends __KERNEL__ implements ifSVCDCN {
    private mOCache: any;
    constructor(aServiceName: string) {
        super(aServiceName);
        hx.log("dsvc", "initializer service - hx.svc.DCN");
        this.init();
    }
    /*********** Internal Function Start ****************/
    // PRIVATE FUNCTION FOR CONTENT MAKING
    private fnGetMatchedListInCache(aNum, aGroup, aList) {
        var newlist : any = [], i : number, it : number = aList.length;
        var item : any, numString : string, dcn : string, ni : number, nit : number;

        if (aNum[0] !== "0") {
            for ( i = 0; i < it; i += 1) {
                item = aList[i];
                if (item) {
                    numString = (item.major).toString();
                    if (numString.indexOf(aNum) > -1) {
                        newlist.push(item);
                    }
                }
            }
        } else {
            for ( i = 0; i < it; i += 1) {
                item = aList[i];
                if (item) {
                    dcn = hx.il.makeDigitNumber(item.major);
                    if (dcn.indexOf(aNum) > -1) {
                        newlist.push(item);
                    }
                }
            }
        }

        aList.length = 0;
        nit = newlist.length;
        for ( ni = 0; ni < nit; ni += 1) {
            aList[aList.length] = newlist[ni];
        }
    }

    private fnAnotherGroup(aLCN, aGroup, aTargetList): number {
        var disposableIndex = -1;
        if (aGroup === "TV" || aGroup === "RADIO") {
            aGroup = aGroup === "TV" ? "RADIO" : "TV";
            var chlist = hx.svc.CH_LIST.getChlist({
                group: aGroup
            }).chList;
            var len = chlist.length;
        }
        var result = /[^0][0-9]*/.exec(aLCN);
        var num = result ? result[0] : null;
        var channel;
        if (num) {
            for (var i = 0; i < len; i++) {
                channel = chlist[i];
                if (channel.majorChannel.toString().indexOf(num) > -1) {
                    disposableIndex = aTargetList.push({
                        major: channel.majorChannel,
                        cchanneld: channel.cchanneld,
                        ccid: channel.ccid,
                        name: channel.name,
                        digiNum: channel._dcn,
                        group: aGroup,
                        hidden: channel.hidden,
                        selectable: channel.getField('selectable')
                    }) - 1;
                }
            }
        }
        return disposableIndex;
    }

    private fnGetMatchListInChannelList(aNum: string, aUiGroup: string, aFx) {
        var retObj : any, chlist : any, c : number, len : number, filterlist = [], ci : any, dcn : string, cNum : string;
        var retLSvcObj : any = hx.svc.WEB_STORAGE.getLastService();
        var group : string = aUiGroup || retLSvcObj.group;
        var disposableIndex = -1;

        retObj = hx.svc.CH_LIST.getChlist({
            group : group
        });
        chlist = retObj.chList;
        len = chlist.length;
        if (hx.il.castNumber(aNum)) {
            for ( c = 0; c < len; c += 1) {
                ci = chlist[c];
                cNum = (ci.majorChannel).toString();
                if (cNum.indexOf(aNum) > -1) {
                    // TODO : We have to read from real Channel Object.
                    filterlist.push({
                        major : ci.majorChannel,
                        ccid : ci.ccid,
                        name : ci.name,
                        digiNum : ci._dcn,
                        group: group,
                        hidden: ci.hidden,
                        selectable: ci.getField('selectable')
                    });
                }
            }
        } else {
            for ( c = 0; c < len; c += 1) {
                ci = chlist[c];
                dcn = hx.il.makeDigitNumber(ci.majorChannel);
                if (dcn.indexOf(aNum) > -1) {
                    // TODO : We have to read from real Channel Object.
                    filterlist.push({
                        major : ci.majorChannel,
                        ccid : ci.ccid,
                        name : ci.name,
                        digiNum : dcn,
                        group: group,
                        hidden: ci.hidden,
                        selectable: ci.getField('selectable')
                    });
                }
            }
        }
        if (ENV.listOP.UKDTT === ENV.op) {
            disposableIndex = this.fnAnotherGroup(aNum, group, filterlist);
        }
        ci = undefined;
        chlist = [];
        retObj = undefined;
        aFx(filterlist, disposableIndex);
    }
    private fnSetChannel (aChannel) {
        var param: any = {
            ccid: aChannel.ccid
        };
        if (ENV.listOP.UKDTT === ENV.op) {
            param.group = aChannel.group;
        }
        var retObj : any = hx.svc.CH_LIST.getChannel(param);

        if (!retObj.changeable) {
            return false;
        }

        param = {
            channel : retObj.channel
        };
        if (ENV.listOP.UKDTT === ENV.op) {
            param.group = aChannel.group;
        }
        hx.svc.AV_CTRL.channelChange(param);
        return true;
    }
    init () {
        hx.log("dsvc", "hx.svc.DCN's init() called");
    }
    private _setCache(aList: any[], aNumber: string, aGroup, aAnotherGroupEqualChannelIndex: number = -1) {
        var ret;
        aList.sort(function(a, b) { return a.major - b.major; });
        if (!this.mOCache) {
            this.mOCache = {};
        }
        ret = this.mOCache;
        if (aNumber) {
            this.mOCache.number = aNumber;
        }
        if (aGroup) {
            this.mOCache.group = aGroup;
        }
        if (aList) {
            this.mOCache.list = aList;
            if (aList[aAnotherGroupEqualChannelIndex]) {
                ret = $.extend(true, {}, this.mOCache);
                aList.splice(aAnotherGroupEqualChannelIndex, 1);
            }
            this.mOCache.list = aList;
        }
        return ret;
    }
    create(aParam) {
//        var cache = {
//            number : aParam.startnum,
//            list : [],
//            group : aParam.group
//        };
//        this.mOCache = cache;
        var cache = this._setCache([], aParam.startnum, aParam.group);
        this.fnGetMatchListInChannelList(cache.number, cache.group, (list, aAnotherGroupEqualChannelIndex = -1) => {
            if (ENV.op === ENV.listOP.UKDTT) {
                cache = this._setCache(list, null, null);
            } else {
                cache = this._setCache(list, null, null, aAnotherGroupEqualChannelIndex);
            }
        });

        return cache;
    }
    putNumber (aParam) {
        var cache: any= this.mOCache;
        cache.number += aParam.number;

        if (cache.list.length) {
            this.fnGetMatchedListInCache(cache.number, cache.group, cache.list);
        } else {
            this.fnGetMatchListInChannelList(cache.number, cache.group, (list, aAnotherGroupEqualChannelIndex = -1) => {
                if (ENV.op === ENV.listOP.UKDTT) {
                    cache = this._setCache(list, null, null);
                } else {
                    cache = this._setCache(list, null, null, aAnotherGroupEqualChannelIndex);
                }
            });
        }

        return cache;
    }
    setchannel (aParam) {
        var ret = false;
        var cache: any = this.mOCache;

        if (cache === null) {
            ret = false;
        } else {
            if (cache.list[aParam.index]) {
                ret = this.fnSetChannel(cache.list[aParam.index]);
            } else {
                if (ENV.listOP.UKDTT === ENV.op) {
                    this.fnGetMatchListInChannelList(cache.number, null, (list, aAnotherGroupEqualChannelIndex = -1) => {
                        var channel = list[aAnotherGroupEqualChannelIndex];
                        if (channel) {
                            ret = this.fnSetChannel(channel);
                        }
                    });
                }
            }
        }

        return ret;
    }
}
export =  cDLDCN;


