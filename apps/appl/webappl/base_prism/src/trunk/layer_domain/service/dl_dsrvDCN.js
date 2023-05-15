/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvTemplate.js
//    DCN: ['layer_domain/service/dl_dsrvDCN.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLDCN = (function (_super) {
        __extends(cDLDCN, _super);
        function cDLDCN(aServiceName) {
            _super.call(this, aServiceName);
            hx.log("dsvc", "initializer service - hx.svc.DCN");
            this.init();
        }
        /*********** Internal Function Start ****************/
        // PRIVATE FUNCTION FOR CONTENT MAKING
        cDLDCN.prototype.fnGetMatchedListInCache = function (aNum, aGroup, aList) {
            var newlist = [], i, it = aList.length;
            var item, numString, dcn, ni, nit;
            if (aNum[0] !== "0") {
                for (i = 0; i < it; i += 1) {
                    item = aList[i];
                    if (item) {
                        numString = (item.major).toString();
                        if (numString.indexOf(aNum) > -1) {
                            newlist.push(item);
                        }
                    }
                }
            }
            else {
                for (i = 0; i < it; i += 1) {
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
            for (ni = 0; ni < nit; ni += 1) {
                aList[aList.length] = newlist[ni];
            }
        };
        cDLDCN.prototype.fnAnotherGroup = function (aLCN, aGroup, aTargetList) {
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
        };
        cDLDCN.prototype.fnGetMatchListInChannelList = function (aNum, aUiGroup, aFx) {
            var retObj, chlist, c, len, filterlist = [], ci, dcn, cNum;
            var retLSvcObj = hx.svc.WEB_STORAGE.getLastService();
            var group = aUiGroup || retLSvcObj.group;
            var disposableIndex = -1;
            retObj = hx.svc.CH_LIST.getChlist({
                group: group
            });
            chlist = retObj.chList;
            len = chlist.length;
            if (hx.il.castNumber(aNum)) {
                for (c = 0; c < len; c += 1) {
                    ci = chlist[c];
                    cNum = (ci.majorChannel).toString();
                    if (cNum.indexOf(aNum) > -1) {
                        // TODO : We have to read from real Channel Object.
                        filterlist.push({
                            major: ci.majorChannel,
                            ccid: ci.ccid,
                            name: ci.name,
                            digiNum: ci._dcn,
                            group: group,
                            hidden: ci.hidden,
                            selectable: ci.getField('selectable')
                        });
                    }
                }
            }
            else {
                for (c = 0; c < len; c += 1) {
                    ci = chlist[c];
                    dcn = hx.il.makeDigitNumber(ci.majorChannel);
                    if (dcn.indexOf(aNum) > -1) {
                        // TODO : We have to read from real Channel Object.
                        filterlist.push({
                            major: ci.majorChannel,
                            ccid: ci.ccid,
                            name: ci.name,
                            digiNum: dcn,
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
        };
        cDLDCN.prototype.fnSetChannel = function (aChannel) {
            var param = {
                ccid: aChannel.ccid
            };
            if (ENV.listOP.UKDTT === ENV.op) {
                param.group = aChannel.group;
            }
            var retObj = hx.svc.CH_LIST.getChannel(param);
            if (!retObj.changeable) {
                return false;
            }
            param = {
                channel: retObj.channel
            };
            if (ENV.listOP.UKDTT === ENV.op) {
                param.group = aChannel.group;
            }
            hx.svc.AV_CTRL.channelChange(param);
            return true;
        };
        cDLDCN.prototype.init = function () {
            hx.log("dsvc", "hx.svc.DCN's init() called");
        };
        cDLDCN.prototype._setCache = function (aList, aNumber, aGroup, aAnotherGroupEqualChannelIndex) {
            if (aAnotherGroupEqualChannelIndex === void 0) { aAnotherGroupEqualChannelIndex = -1; }
            var ret;
            aList.sort(function (a, b) {
                return a.major - b.major;
            });
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
        };
        cDLDCN.prototype.create = function (aParam) {
            var _this = this;
            //        var cache = {
            //            number : aParam.startnum,
            //            list : [],
            //            group : aParam.group
            //        };
            //        this.mOCache = cache;
            var cache = this._setCache([], aParam.startnum, aParam.group);
            this.fnGetMatchListInChannelList(cache.number, cache.group, function (list, aAnotherGroupEqualChannelIndex) {
                if (aAnotherGroupEqualChannelIndex === void 0) { aAnotherGroupEqualChannelIndex = -1; }
                if (ENV.op === ENV.listOP.UKDTT) {
                    cache = _this._setCache(list, null, null);
                }
                else {
                    cache = _this._setCache(list, null, null, aAnotherGroupEqualChannelIndex);
                }
            });
            return cache;
        };
        cDLDCN.prototype.putNumber = function (aParam) {
            var _this = this;
            var cache = this.mOCache;
            cache.number += aParam.number;
            if (cache.list.length) {
                this.fnGetMatchedListInCache(cache.number, cache.group, cache.list);
            }
            else {
                this.fnGetMatchListInChannelList(cache.number, cache.group, function (list, aAnotherGroupEqualChannelIndex) {
                    if (aAnotherGroupEqualChannelIndex === void 0) { aAnotherGroupEqualChannelIndex = -1; }
                    if (ENV.op === ENV.listOP.UKDTT) {
                        cache = _this._setCache(list, null, null);
                    }
                    else {
                        cache = _this._setCache(list, null, null, aAnotherGroupEqualChannelIndex);
                    }
                });
            }
            return cache;
        };
        cDLDCN.prototype.setchannel = function (aParam) {
            var _this = this;
            var ret = false;
            var cache = this.mOCache;
            if (cache === null) {
                ret = false;
            }
            else {
                if (cache.list[aParam.index]) {
                    ret = this.fnSetChannel(cache.list[aParam.index]);
                }
                else {
                    if (ENV.listOP.UKDTT === ENV.op) {
                        this.fnGetMatchListInChannelList(cache.number, null, function (list, aAnotherGroupEqualChannelIndex) {
                            if (aAnotherGroupEqualChannelIndex === void 0) { aAnotherGroupEqualChannelIndex = -1; }
                            var channel = list[aAnotherGroupEqualChannelIndex];
                            if (channel) {
                                ret = _this.fnSetChannel(channel);
                            }
                        });
                    }
                }
            }
            return ret;
        };
        return cDLDCN;
    })(__KERNEL__);
    return cDLDCN;
});
