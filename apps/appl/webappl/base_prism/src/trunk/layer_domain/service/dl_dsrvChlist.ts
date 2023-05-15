/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvChlist.js
/*jslint sloppy:true, nomen:true, vars:true */
//    CH_LIST: ['layer_domain/service/dl_dsrvChlist.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __MODEL__ = require("../../model/model");


enum eSVC_SECTION {
    SECTION_DEFAULT = 1,
    SECTION_LCN = 2,
    SECTION_NONE_LCN = 16
}

class cDLCHList extends __KERNEL__ implements ifSVCChlist {
    //ENUM

    // Channel Database
    channel_db = [];
    channelConfig;
    editTvList;
    editRadioList;
    _newTvList;
    constructor(aServiceName: string) {
        super(aServiceName);
        var that = this;
        that.defineEvents(['ChannelListUpdate']);
        that.channelConfig = hx.stbObject.channelConfig;
        var favouriteLists = that.channelConfig.favouriteLists;
        //hx.logadd('dsrvChlist');
        hx.log("dsrvChlist", "hx.svc.CH_LIST's _init() called");
        that._initEventHandler();
        that.channel_db = that._ChReload(favouriteLists);
        that._loadFavLists(that.channel_db, favouriteLists);
        favouriteLists = undefined;
    }
    _initEventHandler() {
        // Apply Channel List Listener.
        var that = this;
        that.channelConfig.onChannelListUpdate = function(updatedChannelList) {
            var favouriteLists = that.channelConfig.favouriteLists;
            hx.log('debug', 'DSVC onChannelListUpdate called!!!!');
            if (that.channel_db) {
                that.channel_db = [];
            }
            that.channel_db = that._ChReload(favouriteLists);
            that._loadFavLists(that.channel_db, favouriteLists);
            that.fireEvent('ChannelListUpdate');
            favouriteLists = undefined;
        };
    }

    /*********** External API Start ****************/
    /*
     *
     * getZapTime -- (deprecated!!)
     * getAllList() -- (deprecated!!)
     * **************************
     * getChannel(ccid, group) -- return result, channel, group name, changeable (for edit Channel)
     * getNextChannel(cur channel) -- return result, channel, group name, changeable (for live)
     * getPrevChannel(cur channel) -- return result, channel, group name, changeable (for live)
     * getCurrentChannel() -- return result, channel, group name (for epg, smartsearch, live)
     * getChannelCount(group) -- return group name, channelCount (for channelSearch, live, installWizard)
     * getChlist(group) -- return group name, channel list, favID (for list draw, chlist & epg)
     * getCurrentGrpInfo(group) -- return channel list & group name (for list draw, chlist & epg)
     * checkChannelChangable(channel) -- return isChChangable (for epg)
     * checkChannelsChangable(channels) -- return channel, isChChangable (for epg)
     * getAvailableFirstChannel -- return channel
     * getWholeChDB -- return channelDB (for epg)
     * forceChListUpdate -- Reload ChannelList and fireEvent('ChannelListUpdate')
     * getChannels(ccid, group, offset, count) -- return channels (for epg)
     * removeWholeChannelList -- remove all channelList (for channelSearch)
     * removeChannel(group, ccidArray) -- remove channel (for edit Channel)
     * updateFavList() -- update FavList and return FavListIds (for chlist)
     * addToFavList(ccid, favtype, cb) -- add Ch to FavList and return isSuccess
     */
    checkChannelChangable(data) {
        var a = this;
        return a._checkChannelChangableCL(data.channel);
    }
    checkChannelsChangable(channels) {
        var a = this;
        var len = channels.length;
        var i;
        var p = [];

        for ( i = 0; i < len; i += 1) {
            var ret = {
                channel : channels[i],
                changeable : (channels[i].ccid) ? a._checkChannelChangableCL(channels[i]) : false
            };
            p.push(ret);
        }
        return p;
    }

    getChlist(param: any = {}, aIsHiddenChannelOnly?: boolean) {
        var chList = [];
        var retObj: any = {};
        var favID = null, name = null;
        var group = param.group || "ALL";

        hx.log("dsrvChlist", "getChlist() -- group : " + group);
        var findedChList = this._findChList(group);
        if (findedChList) {
            chList = findedChList.list;
            favID = findedChList.favID;
            name = findedChList.name;
            findedChList = undefined;
        }

        retObj.group = group;
        retObj.chList = chList;
        retObj.name = name;
        if (favID !== null) {
            retObj.favID = favID;
        }
//        if (aIsHiddenChannelOnly) {
//            var listForHiddenCH = [];
//            var ch;
//            for (var i = 0, len = chList.length; i < len; i++) {
//                ch = chList[i];
//                if (ch.hidden) {
//                    listForHiddenCH.push(ch);
//                }
//            }
//            retObj.chList = listForHiddenCH;
//        }
//        if (ENV.listOP.UKDTT === ENV.op) {
//            var listForHiddenCH = [];
//            var ch;
//            for (var i = chList.length - 1; i >= 0; i--) {
//                ch = chList[i];
//                if (ch.hidden) {
//                    listForHiddenCH.push(ch);
//                    chList.splice(i, 1);
//                }
//            }
//            retObj.hidden = listForHiddenCH;
//        }
        hx.log("dsrvChlist", "getChlist() -- retObj.chList.length : " + retObj.chList.length);
        chList = [];
        if (param.cb !== undefined) {
            setTimeout(param.cb(retObj), 0);
        }
        return retObj;
    }
    getCurrentGrpInfo(group) {
        var len_grp, a = this, retObj: any = {}, retGrp;
        var i;

        hx.log("dsrvChlist", "getCurrentGrpInfo() -- group : " + group);

        retGrp = hx.svc.WEB_STORAGE.getLastGroup();

        retObj.group = retGrp;

        var chList = a._findChList(group);
        if (chList) {
            retObj.chlist = chList.list;
        }
        return retObj;
    }
    getAvailableFirstChannel(param?) {
        var i, len, a = this, tempParam: any = {}, retObj: any = {}, channel = null, group;

        hx.log("dsrvChlist", "getAvailableFirstChannel()");

        if (param) {
            if (param.group !== undefined) {
                tempParam.group = param.group;
                retObj = a.getChlist(tempParam);
                len = retObj.chList.length;
                if (retObj.chList.length) {
                    for ( i = 0; i < len; i += 1) {
                        channel = retObj.chList[i];
                        if (true === a._checkChannelChangableCL(channel)) {
                            group = tempParam.group;
                            break;
                        }
                    }
                    retObj = undefined;
                    return {
                        'channel' : channel,
                        'group' : group
                    };
                }
            }
        }

        tempParam.group = 'TV';
        retObj = a.getChlist(tempParam);
        len = retObj.chList.length;
        if (retObj.chList.length) {
            for ( i = 0; i < len; i += 1) {
                channel = retObj.chList[i];
                if (true === a._checkChannelChangableCL(channel)) {
                    group = tempParam.group;
                    break;
                }
            }
        } else {
            tempParam.group = 'RADIO';
            retObj = a.getChlist(tempParam);
            len = retObj.chList.length;
            if (retObj.chList.length) {
                for ( i = 0; i < len; i += 1) {
                    channel = retObj.chList[i];
                    if (true === a._checkChannelChangableCL(channel)) {
                        group = tempParam.group;
                        break;
                    }
                }
            }
        }
        retObj = undefined;
        return {
            'channel' : channel,
            'group' : group
        };
    }
    getAvailableChannelWithGroup(group) {
        var i, len, a = this, param: any = {}, retObj: any = {}, channel = null;

        hx.log("dsrvChlist", "getAvailableChannelWithGroup() Group : " + group);
        param.group = group;
        retObj = a.getChlist(param);
        len = retObj.chList.length;
        if (retObj.chList.length) {
            for ( i = 0; i < len; i += 1) {
                channel = retObj.chList[i];
                if (true === a._checkChannelChangableCL(channel)) {
                    group = param.group;
                    break;
                }
            }
        }
        retObj = undefined;
        return {
            'channel' : channel,
            'group' : group
        };
    }
    getAvailableGroupForChannel(data) {
        var i, len, a = this, param: any = {}, retObj: any = {}, group = null, channel = data.channel;

        hx.log("dsrvChlist", "getAvailableGroupForChannel()");

        param.group = 'TV';
        retObj = a.getChlist(param);
        len = retObj.chList.length;
        if (retObj.chList.length) {
            for ( i = 0; i < len; i += 1) {
                if (channel.ccid === retObj.chList[i].ccid) {
                    group = param.group;
                    break;
                }
            }
        }

        if (group === null) {
            param.group = 'RADIO';
            retObj = a.getChlist(param);
            len = retObj.chList.length;
            if (retObj.chList.length) {
                for ( i = 0; i < len; i += 1) {
                    if (channel.ccid === retObj.chList[i].ccid) {
                        group = param.group;
                        break;
                    }
                }
            }
        }
        retObj = undefined;
        return {
            'channel' : channel,
            'group' : group
        };
    }
    getAvailableGroupForCcid(data) {
        var i, len, a = this, param: any = {}, retObj: any = {}, group = null, ccid = data.ccid;

        hx.log("dsrvChlist", "getAvailableGroupForCcid()");

        param.group = 'TV';
        retObj = a.getChlist(param);
        len = retObj.chList.length;
        if (retObj.chList.length) {
            for ( i = 0; i < len; i += 1) {
                if (ccid === retObj.chList[i].ccid) {
                    group = param.group;
                    break;
                }
            }
        }

        if (group === null) {
            param.group = 'RADIO';
            retObj = a.getChlist(param);
            len = retObj.chList.length;
            if (retObj.chList.length) {
                for ( i = 0; i < len; i += 1) {
                    if (ccid === retObj.chList[i].ccid) {
                        group = param.group;
                        break;
                    }
                }
            }
        }
        retObj = undefined;
        return {
            'ccid' : ccid,
            'group' : group
        };
    }
    getChannelCount(aParam: any = {}) {
        var len_grp, a = this, group = '', chCnt = 0, retObj: any = {};

        if (aParam.group === undefined) {
            group = 'ALL';
        } else {
            group = aParam.group;
        }

        hx.log("dsrvChlist", "getChannelCount() -- group : " + group);
        var chList = a._findChList(group);
        if (chList) {
            chCnt = chList.list.length;
        }
        if (chCnt > 0 && ENV.op === ENV.listOP.UKDTT) {
            var list = chList.list;
            for (var i = 0; i < list.length; i++) {
                if (list[i].hidden) {
                    if (--chCnt === 0) {
                        break;
                    }
                }
            }
        }
        chList = undefined;
        retObj.group = group;
        retObj.chCnt = chCnt;
        hx.log("dsrvChlist", "getChannelCount() -- chCnt : " + chCnt);
        if (aParam.cb !== undefined) {
            setTimeout(aParam.cb(retObj), 0);
        }
        return retObj;
    }
    getWholeChDB(param) {
        var a = this;
        if (param) {
            hx.log("dsrvChlist", "getWholeChDB() -- param.cb : " + param.cb);
            if (param.cb !== undefined) {
                setTimeout(param.cb(a.channel_db), 0);
            }
        }
        return a.channel_db;
    }
    forceChListUpdate() {
        var a = this;
        var favouriteLists = a.channelConfig.favouriteLists;
        hx.log("dsrvChlist", "forceChListUpdate()");

        if (a.channel_db) {
            a.channel_db = [];
        }
        a.channel_db = a._ChReload(favouriteLists);
        a._loadFavLists(a.channel_db, favouriteLists);
        a.fireEvent('ChannelListUpdate', null);
        favouriteLists = undefined;
    }
    getChannel(param) {
        var that = this;
        var chObj, retObj: any = {};

        retObj.result = false;

        if (param.group === undefined) {
            var group;
            var ccid = param.ccid;
            function findChannelGroup(chList, ccid){
                if (chList) {
                    var i,listCh;
                    for ( i = 0; i < chList.list.length; i += 1) {
                        listCh = chList.list[i];
                        if (listCh.ccid === ccid) {
                            return true;
                        }
                    }
                }
                return false;
            }

            //1. find channel in the lastGroup
            //2. search channel in the each group
            if(findChannelGroup(that._findChList(hx.svc.WEB_STORAGE.getLastGroup()) , ccid)){
                group = hx.svc.WEB_STORAGE.getLastGroup();
            }else if(findChannelGroup(that._findChList('TV') , ccid)){
                group = 'TV';
            }else if(findChannelGroup(that._findChList('RADIO') , ccid)){
                group = 'RADIO';
            }else{
                group = 'ALL';
            }
            param.group = group;
        }

        hx.log("dsrvChlist", "getChannel() -- param.group : " + param.group);

        if (param.lcn) {
            chObj = that._getChannelByLcn(param);
        } else {
            chObj = that._getChannelByCcid(param);
        }
        if (chObj) {
            retObj.result = true;
            retObj.channel = chObj;
            retObj.group = param.group;
            retObj.changeable = this._checkChannelChangableCL(chObj);
        } else {
            retObj.group = param.group;
        }
        chObj = undefined;

        if (param.cb !== undefined) {
            setTimeout(param.cb(retObj), 0);
        }

        return retObj;
    }
    getChannels(param) {
        var that = this, i, j, index, start, end, length, channel, //
            channel_db, ccid, lastServiceCcid, group, lastGroup, //
            offset, count, channels = [], result = [];

        offset = param.offset;
        count = param.count;

        lastGroup = hx.svc.WEB_STORAGE.getLastGroup();
        lastServiceCcid = hx.svc.WEB_STORAGE.getLastService().ccid;

        group = param.group || lastGroup;
        if (!group) {
            group = 'TV';
        }

        hx.log("dsrvChlist", "getChannels() -- group : " + group);

        var chList = this._findChList(group);
        if (chList) {
            for(i = 0 ; i < chList.list.length ; i++) {
                if(chList.list[i].ccid === lastServiceCcid) {
                    lastServiceCcid = chList.list[i].hidden ? hx.svc.CH_LIST.getAvailableFirstChannel().channel.ccid : lastServiceCcid;
                    break;
                }
            }
            if (ENV.op === ENV.listOP.UKDTT) {
                var list = chList.list;
                for (i = 0; i < list.length; i++) {
                    if (!list[i].hidden) {
                        channels.push(list[i]);
                    }
                }
                list = [];
            } else {
                channels = chList.list;
            }
        }
        length = channels.length;
        if (length) {
            ccid = param.ccid;
            if (!ccid) {
                ccid = (lastGroup !== group) ? channels[0].ccid : (param.ccid || lastServiceCcid);
            }
            if (offset === 'first') {
                ccid = channels[0].ccid;
                offset = 0;
            } else if (offset === 'last') {
                ccid = channels[length - 1].ccid;
                offset = length - 1;
            }
            length = channels.length;
            count = (count > length) ? length : count;
            for ( i = 0; i < length; i += 1) {
                channel = channels[i];
                if (ccid === channel.ccid) {
                    index = i;

                    if ((index === length - 1) && offset > 0) {
                        index = -1;
                    }

                    start = index + offset;
                    end = start < 0 ? (length + start + count) : (start + count);

                    result = channels.slice(start, end);

                    if (result.length < count) {
                        start = 0;
                        end = Math.min(count - result.length, channels.length - result.length);
                        result = result.concat(channels.slice(start, end));
                    }
                }
            }
        }
        chList = undefined;
        channels = [];
        return result;
    }
    getNextChannel(param) {
        var a = this, i, ccid, group, //
            channel, len_grp, len_list, ret_channel, //
            grpInfo, retObj: any = {};

        ccid = param.ccid;
        group = param.group;
        retObj.result = false;
        grpInfo = a._findChList(group);

        if (grpInfo) {
            len_list = grpInfo.list.length;
            for ( i = 0; i < len_list; i += 1) {
                channel = a._getNextChannel(param, +1, grpInfo);
                if (!channel) {
                    break;
                }
                if (true === a._checkChannelChangableCL(channel)) {
                    retObj.result = true;
                    retObj.channel = channel;
                    retObj.group = group;
                    retObj.changeable = true;
                    break;
                }
                param.ccid = channel.ccid;
            }
            channel = undefined;
            grpInfo = undefined;
        }

        if (param.cb !== undefined) {
            setTimeout(param.cb(retObj), 0);
        }

        return retObj;
    }
    getPrevChannel(param) {
        var a = this, ccid, group, //
            channel, len_grp, len_list, ret_channel, //
            grpInfo, retObj: any = {};
        var i;

        ccid = param.ccid;
        group = param.group;
        retObj.result = false;
        grpInfo = a._findChList(group);

        if (grpInfo) {
            len_list = grpInfo.list.length;
            for ( i = 0; i < len_list; i += 1) {
                channel = a._getNextChannel(param, -1, grpInfo);
                if (!channel) {
                    break;
                }
                if (true === a._checkChannelChangableCL(channel)) {
                    retObj.result = true;
                    retObj.channel = channel;
                    retObj.group = group;
                    retObj.changeable = true;
                    break;
                }
                param.ccid = channel.ccid;
            }
            channel = undefined;
            grpInfo = undefined;
        }

        if (param.cb !== undefined) {
            setTimeout(param.cb(retObj), 0);
        }
        return retObj;
    }
    getBackChannel(param) {
        var a = this;
        var that = this;
        var retObj: any = {};
        var o1 = hx.svc.WEB_STORAGE.getBackService();

        retObj.result = false;

        if (o1.ccid !== null && o1.group !== null) {
            var o2 = this.getChannel({
                ccid : o1.ccid,
                group : o1.group
            });

            if (true === a._checkChannelChangableCL(o2.channel)) {
                retObj.result = true;
                retObj.channel = o2.channel;
                retObj.group = o2.group;
                retObj.changeable = true;
            }
            o2 = undefined;
        }
        o1 = undefined;

        if (param.cb !== undefined) {
            setTimeout(param.cb(retObj), 0);
        }
        return retObj;
    }
    getCurrentChannel(param?) {
        var that = this;
        var ccid = null;
        var group = '';
        var retObj: any = {}, retSvc: any = {};

        retSvc = hx.svc.WEB_STORAGE.getLastService();

        var cnt = this.getChannelCount(retSvc);
        if (!cnt.chCnt) {
            retSvc.group = 'ALL';
        }

        ccid = retSvc.ccid;
        group = retSvc.group;

        if (ccid === null) {
            retObj.result = false;
        } else {
            var chObj;

            chObj = this._getChannelByCcid(retSvc);
            if (!chObj) {
                retObj.result = false;
            } else {
                retObj.result = true;
                retObj.channel = chObj;
                if (group === 'ALL') {
                    retObj.group = (chObj.channelType === 1) ? 'RADIO' : 'TV';
                } else {
                    retObj.group = group;
                    retObj.groupName = chObj.groupName || '';
                }
            }
            chObj = undefined;
        }

        if (param) {
            if (param.cb !== undefined) {
                setTimeout(param.cb(retObj), 0);
            }
        }

        return retObj;
    }
    removeWholeChannelList() {
        var a = this;
        var channelList = a.channelConfig.channelList;
        var len = channelList.length;

        hx.log('dsrvChlist', '!!!!!!!!!!! Remove Whole Channel List [' + len + ']  @!!!!!!!!!!!!!');

        if (a.channel_db) {
            a.channel_db = [];
        }

        a.channelConfig.removeChannelList(channelList);
        channelList = undefined;
    }
    removeChannel(param) {
        //TODO : have to change to handle Array
        var that = this;
        hx.log('dsrvChlist', '###[removeChannel] - ccid : ' + param.channel.ccid);

        //that._removeChannelByCcid(param.group, param.channel.ccid);
        that.channelConfig.removeChannel(param.channel);
    }
    updateChannel(param) {
        var that = this, chObj, retObj: any = {}, findObj: any = {};
        hx.log('dsrvChlist', '###[updateChannel] - ccid : ' + param.ccid);

        findObj.ccid = param.ccid;
        findObj.group = param.group;
        chObj = that._getChannelByCcid(findObj);

        if (!chObj) {
            retObj.result = false;
        } else {
            retObj.result = true;
            retObj.channel = chObj;
            //retObj.group = param.group;
            //retObj.changeable = this._checkChannelChangableCL(chObj);
        }

        if (retObj.result === true) {//&& retObj.changeable === true
            hx.log('dsrvChlist', '\tparam.isRemoved : ' + param.isRemoved);
            if (param.isRemoved) {
                this.removeFromFavList({
                    favtype : param.group,
                    ccid : param.ccid
                });
            } else {
                if (param.isLockChange) {
                    chObj.manualBlock = param.lock;
                }
                if (param.isNameChange) {
                    chObj.name = param.name;
                }
            }
        }
        chObj = undefined;
        retObj = undefined;
    }
    // Favourite List area.....
    updateFavList(p) {
        var that = this;
        var FavListIds;
        var favouriteLists = that.channelConfig.favouriteLists;
        /*
         if (p !== null) {
         that._favIDs[0] = p.favID1;
         that._favIDs[1] = p.favID2;
         that._favIDs[2] = p.favID3;
         that._favIDs[3] = p.favID4;
         that._favIDs[4] = p.favID5;
         }
         */
        hx.log("dsrvChlist", "updateFavList() called");
        //that.forceChListUpdate();

        that._loadFavLists(that.channel_db, favouriteLists);
        that.fireEvent('ChannelListUpdate', null);
        favouriteLists = undefined;

        FavListIds = hx.svc.WEB_STORAGE.getFavListID();
        return FavListIds;
    }
    addToFavList(params): any {
        var that = this;
        var ccid = params.ccid;
        var favtype = params.favtype;
        var favList = null;
        var InternalParam: any = {};
        var n;
        var len = 0;
        var i;

        hx.log('dsrvChlist', 'addToFavList ccid=' + ccid);
        if ((!favtype) || (!ccid)) {
            return false;
        }
        InternalParam.group = favtype;
        favList = that.getChlist(InternalParam);
        hx.log('dsrvChlist', 'addToFavList web array favList.favID=' + favList.favID + 'favList.chList.length=' + favList.chList.length);
        if (favList.favID === undefined) {
            return false;
        }

        len = favList.chList.length;

        // limit 200
        if (len >= 200) {
            hx.log("error", favtype + " is full!!!");
            return len;
        }

        // not allow same service
        for ( n = 0; n < len; n += 1) {
            hx.log('dsrvChlist', 'favList.chList[n].name=' + favList.chList[n].name + ',ccid=' + favList.chList[n].ccid);
            if (favList.chList[n].ccid === ccid) {
                hx.log('dsrvChlist', 'it is already favourite!!!!');
                if (params.cb !== undefined) {
                    setTimeout(params.cb(true), 0);
                }
                return true;
            }
        }

        var favouriteLists = that.channelConfig.favouriteLists;
        if (!favouriteLists) {
            return false;
        }
        var userFav = favouriteLists.getFavouriteList(favList.favID);
        hx.log('dsrvChlist', 'addToFavList userFav.length=' + userFav.length);
        if (userFav) {
            var fResult = userFav.insertBefore(len, ccid);
            if (fResult === true) {
                userFav.commit();
                hx.log('dsrvChlist', 'addToFavList commit!!!!');
            } else {
                userFav = undefined;
                favouriteLists = undefined;
                return false;
            }
        } else {
            userFav = undefined;
            favouriteLists = undefined;
            return false;
        }
        // update fav. list
        // temporary we have to add
        // InternalParam = {};
        // InternalParam.ccid = ccid;
        // InternalParam.group = favtype;
        // var OipfChannel = this._getChannelByCcid(InternalParam);
        // if (OipfChannel) {
        // favList.push(OipfChannel);
        // } else {
        // // TODO : Force Update!!!!!!!
        // //that.updateFavList();
        // }
        InternalParam = {};
        InternalParam.ccid = ccid;
        InternalParam.group = "ALL";
        var OipfChannel = this._getChannelByCcid(InternalParam);
        favList.chList.push(OipfChannel);
        hx.log('dsrvChlist', 'addToFavList web array push ???');
        //if (that.channel_db) {
        //    var len_grp = that.channel_db.length;
        //    for ( i = 0; i < len_grp; i += 1) {
        //        if (that.channel_db[i].favID === userFav.favID) {
        //            that.channel_db[i].list = favList.chList;
        //            hx.log('dsrvChlist', 'addToFavList web array push !!!');
        //            break;
        //        }
        //    }
        //}
        OipfChannel = undefined;
        favList = undefined;
        userFav = undefined;
        favouriteLists = undefined;
        if (params.cb !== undefined) {
            setTimeout(params.cb(true), 0);
        }
        return true;
    }
    getFavListName(param) {
        var that = this, favtype = param.favtype, favList = null, InternalParam: any = {};
        var nameFav;

        hx.log("dsrvChlist", "getFavListName() called - favtype : " + favtype);

        if (!favtype) {
            return false;
        }

        InternalParam.group = favtype;
        favList = that.getChlist(InternalParam);
        if (favList.favID === undefined) {
            return false;
        }

        hx.log('dsrvChlist', 'name of group : ' + favList.name);
        nameFav = favList.name;
        favList = undefined;
        return nameFav;
    }
    removeFromFavList(params) {
        var that = this;
        var ccid = params.ccid;
        var favtype = params.favtype;
        var favList = null, index = -1;
        var InternalParam: any = {};
        var n;
        var len = 0;
        var i;

        hx.log('dsrvChlist', 'removeFromFavList ccid=' + ccid);
        if ((!favtype) || (!ccid)) {
            return false;
        }
        InternalParam.group = favtype;

        favList = that.getChlist(InternalParam);
        hx.log('dsrvChlist', 'removeFromFavList web array favList.favID=' + favList.favID + 'favList.chList.length=' + favList.chList.length);
        if (favList.favID === undefined) {
            return false;
        }
        len = favList.chList.length;

        // find same service
        for ( n = 0; n < len; n += 1) {
            hx.log('dsrvChlist', 'favList.chList[n].name=' + favList.chList[n].name + ',ccid=' + favList.chList[n].ccid);
            if (favList.chList[n].ccid === ccid) {
                index = n;
                break;
            }
        }
        hx.log('dsrvChlist', 'removeFromFavList web array index=' + index);
        if (-1 === index) {
            return false;
        }

        var favouriteLists = that.channelConfig.favouriteLists;
        if (!favouriteLists) {
            return false;
        }
        var userFav = favouriteLists.getFavouriteList(favList.favID);
        hx.log('dsrvChlist', 'removeFromFavList userFav.length=' + userFav.length);
        if (userFav) {
            var fResult = userFav.remove(index);
            if (fResult === true) {
                userFav.commit();
                hx.log('dsrvChlist', 'removeFromFavList userFav commit!!');
            } else {
                userFav = undefined;
                favouriteLists = undefined;
                return false;
            }
        } else {
            userFav = undefined;
            favouriteLists = undefined;
            return false;
        }

        // update fav. list
        // temporary we have to add
        // TODO : Force Update!!!!!!!
        favList.chList.splice(index, 1);
        hx.log('dsrvChlist', 'removeFromFavList web array splice???');
        //that.updateFavList();
        //if (that.channel_db) {
        //    var len_grp = that.channel_db.length;
        //    for ( i = 0; i < len_grp; i += 1) {
        //        if (that.channel_db[i].favID === userFav.favID) {
        //            that.channel_db[i].list = favList.chList;
        //            hx.log('dsrvChlist', 'removeFromFavList web array splice !!!');
        //            break;
        //        }
        //    }
        //}
        favList = undefined;
        userFav = undefined;
        favouriteLists = undefined;

        if (params.cb !== undefined) {
            setTimeout(params.cb(true), 0);
        }

        return fResult;
    }
    renewFavList(params) {
        // this function for settings>edit favourite list
        // 1. remove all list
        // 2. add all list that from parmas
        hx.log("dsrvChlist", "renewFavList() called");
        var that = this;
        var favListArrayLen = params.updateParamArray.length;
        var updateParam = {
            favType : null,
            isNameChange : null,
            favListName : null,
            isListChange : null,
            ccids : null
        };
        var InternalParam = {
            group : null
        };
        var favList = null, favListlen = 0;
        var delFavChlist = [], userFav;
        var i, j, n, ccidlen, len_grp, fResult;
        var curObj = that.getCurrentChannel();
        var favouriteLists = that.channelConfig.favouriteLists;
        if (!favouriteLists) {
            return false;
        }

        for (i = 0; i < favListArrayLen; i+=1) {
            updateParam = params.updateParamArray[i];
            if (!updateParam.favType) {
                return false;
            }
            //get favList from favType
            if (updateParam.isNameChange || updateParam.isListChange) {
                InternalParam.group = updateParam.favType;
                favList = that.getChlist(InternalParam);
                if (favList.favID === undefined) {
                    favouriteLists = undefined;
                    return false;
                }
                userFav = favouriteLists.getFavouriteList(favList.favID);
                favListlen = favList.chList.length;
            }
            //check change and set favListName
            if (updateParam.isNameChange) {
                if (userFav) {
                    userFav.name = updateParam.favListName;
                }
                if (that.channel_db) {
                    len_grp = that.channel_db.length;
                    for ( j = 0; j < len_grp; j += 1) {
                        if (that.channel_db[j].favID === favList.favID) {
                            that.channel_db[j].name = updateParam.favListName;
                            hx.log('dsrvChlist', 'setFavListName web array !!!');
                            break;
                        }
                    }
                }
            }
            //check change and set favList
            if (updateParam.isListChange) {
                if (userFav) {
                    ccidlen = updateParam.ccids.length;
                    // Get deleted service list
                    for ( j = 0; j < favListlen; j += 1) {
                        var chObj = userFav.item(j);
                        for ( n = 0; n < ccidlen; n += 1) {
                            if (chObj) {
                                if (chObj.ccid === updateParam.ccids[n]) {
                                    break;
                                }
                            }
                        }

                        if (n === ccidlen) {
                            delFavChlist.push(chObj);
                        }
                    }
                    // remove all
                    for ( n = favListlen; n > 0; n -= 1) {
                        fResult = userFav.remove(n - 1);
                        // to avoid emulator problem
                    }
                    // add all
                    for ( n = 0; n < ccidlen; n += 1) {
                        fResult = userFav.insertBefore(n, updateParam.ccids[n]);
                    }
                } else {
                    favouriteLists = null;
                    return false;
                }
            }
            //commit changed favList
            if (updateParam.isNameChange || updateParam.isListChange) {
                if (userFav) {
                    userFav.commit();
                }
            }
            //update web cached data and current service
            if (updateParam.isListChange) {
                // limit
                if (ccidlen > 200) {
                    ccidlen = 200;
                }
                // remove all
                favList.chList = [];
                // add all
                // TODO : Force Update!!!!!!!
                //that.updateFavList();
                var favIdx;
                switch (updateParam.favType) {
                    case 'Favourite1':
                        favIdx = 1;
                        break;
                    case 'Favourite2':
                        favIdx = 2;
                        break;
                    case 'Favourite3':
                        favIdx = 3;
                        break;
                    case 'Favourite4':
                        favIdx = 4;
                        break;
                    case 'Favourite5':
                        favIdx = 5;
                        break;
                    default:
                        favIdx = 1;
                        break;
                }
                var result = that._loadFavList(favList.favID, favIdx, favouriteLists);
                if (that.channel_db) {
                    len_grp = that.channel_db.length;
                    for ( j = 0; j < len_grp; j += 1) {
                        if (userFav.favID === that.channel_db[j].favID) {
                            that.channel_db[j].list = result.userFavList;
                            break;
                        }
                    }
                }
                // deleted service = current service stop & next service play
                if (curObj !== undefined && curObj.channel !== null && delFavChlist.length > 0) {
                    hx.log('dsrvChlist', 'curSvcCcid=' + curObj.channel.ccid);
                    for ( j = 0; j < delFavChlist.length; j += 1) {
                        if (curObj.channel.ccid === delFavChlist[j].ccid) {
                            hx.log('dsrvChlist', 'found delete service');
                            var nextChObj = hx.svc.CH_LIST.getAvailableChannelWithGroup(updateParam.favType);
                            if (nextChObj === undefined || nextChObj.channel === null) {
                                hx.log('dsrvChlist', 'the last channel....');
                                nextChObj = hx.svc.CH_LIST.getAvailableFirstChannel();
                            }

                            hx.log('dsrvChlist', 'found next service=' + nextChObj.channel.ccid);
                            if (nextChObj.channel !== null && nextChObj.channel.ccid !== curObj.channel.ccid) {
                                hx.log('dsrvChlist', 'stop deleted service=' + curObj.channel.ccid + ' and start new service=' + nextChObj.channel.ccid);
                                hx.svc.AV_CTRL.stopCurChannel({
                                    stopAV : 1,
                                    stopTsr : 0
                                });
                                hx.svc.AV_CTRL.channelChange({
                                    channel : nextChObj.channel,
                                    group : nextChObj.group
                                });
                            }
                            break;
                        }
                    }
                }
            }
        }
        favList = undefined;
        nextChObj = undefined;
        favouriteLists = undefined;
        delFavChlist = [];
        userFav = undefined;
        result = undefined;
        if (params.cb !== undefined) {
            setTimeout(params.cb(true), 0);
        }
        return fResult;
    }
    renewPreferList(params) {
        // this function for settings>edit favourite list
        // 1. remove all list
        // 2. add all list that from parmas
        var that = this;
        var ccids = params.ccids;
        // array
        var favtype = params.favtype;
        var favList = null, index = -1;
        var InternalParam: any = {};
        var n;
        var len = 0;
        var ccidlen = ccids.length;
        var i;
        var fResult;

        //hx.logadd("dsrvChlist");
        hx.log("dsrvChlist", "renewPreferList() called");

        if (!favtype) {
            return false;
        }
        InternalParam.group = favtype;

        favList = that.getChlist(InternalParam);
        if (favList.favID === undefined) {
            return false;
        }

        len = favList.chList.length;

        // limit
        // if (ccidlen > 200) {
        // ccidlen = 200;
        // }

        hx.log('dsrvChlist', 'renewFavList channelConfig.favouriteLists!!!!');
        var favouriteLists = that.channelConfig.favouriteLists;
        if (!favouriteLists) {
            return false;
        }

        var userFav = favouriteLists.getFavouriteList(favList.favID);
        var userFavArray = [];
        var favIdx;

        switch (favtype) {
            case 'TV':
                favIdx = 1;
                break;
            case 'RADIO':
                favIdx = 2;
                break;
            default:
                favIdx = 1;
                break;
        }

        if (userFav) {
            // remove all
            for ( n = len; n > 0; n -= 1) {
                fResult = userFav.remove(n - 1);
                // to avoid emulator problem
            }
            // add all

            for ( n = 0; n < ccidlen; n += 1) {
                fResult = userFav.insertBefore2(n, ccids[n], params.majorChannels[n]);
            }

            // userFavArray = this._conv_oipf_to_list(userFav);
            //
            // userFavArray.slice(0, ccidlen);
            // for ( n = 0; n < ccidlen; n += 1) {
            // userFavArray.pop() = params.majorChannels[i];
            // }

            // commit
            if (fResult === true) {
                userFav.commit2(favIdx);
            } else {
                userFav = undefined;
                favouriteLists = undefined;
                return false;
            }
        } else {
            userFav = undefined;
            favouriteLists = undefined;
            return false;
        }

        // remove all
        favList.chList = [];

        var result = that._loadFavList(favList.favID, favtype, favouriteLists);

        if (that.channel_db) {

            var len_grp = result.userFavList.length;
            var idx;

            // for ( idx = 0; idx < len_grp; idx += 1) {
            // result.userFavList[i].majorChannel = params.majorChannels[i];
            // hx.log('dsrvChlist', 'update refreshed channel list array');
            // }
            for ( idx = 0; idx < len_grp; idx += 1) {
                if (that.channel_db[idx].name === params.favtype) {
                    that.channel_db[idx].list = params.updatedChLists;
                    hx.log('dsrvChlist', 'update refreshed channel list array');
                    break;
                }
            }
        }
        favList = undefined;
        userFav = undefined;
        favouriteLists = undefined;
        result = undefined;

        if (params.cb !== undefined) {
            setTimeout(params.cb(true), 0);
        }
    }
    moveChListItem(params): any {
        var that = this;
        var ccid = params.ccid;
        var favtype = params.favtype;
        var favList = null, index = -1, insertIdx = -1;
        var InternalParam: any = {};
        var n;
        var len = 0;
        var isFindMoveItem = false, isFindTargetItem = false;
        var userFav;
        var fResult;

        hx.log('dsrvChlist', 'moveChListItems ccid=' + ccid);
        if ((!favtype) || (!ccid)) {
            return false;
        }
        InternalParam.group = favtype;

        favList = that.getChlist(InternalParam);

        hx.log('dsrvChlist', 'moveChListItems web array favList.favID=' + favList.favID + 'favList.chList.length=' + favList.chList.length);
        /*
         if (favList.favID === undefined) {
         //return false;
         }*/

        len = favList.chList.length;

        // find same service
        for ( n = 0; n < len; n += 1) {
            hx.log('dsrvChlist', 'favList.chList[n].name=' + favList.chList[n].name + ',ccid=' + favList.chList[n].ccid);
            if (!isFindMoveItem) {
                if (favList.chList[n].ccid === ccid) {
                    index = n;
                    isFindMoveItem = true;
                }
            }
            if (!isFindTargetItem) {
                if (favList.chList[n].ccid === params.insertTargetccid) {
                    insertIdx = n;
                    isFindTargetItem = true;
                }
            }
            if (isFindMoveItem && isFindTargetItem) {
                break;
            }
        }

        if (index < insertIdx) {
            insertIdx -= 1;
        }

        hx.log('dsrvChlist', 'moveChListItems web array index=' + index);
        if (-1 === index) {
            return false;
        }

        var favouriteLists;
        if (favtype === 'TV') {
            if (that.editTvList) {
                userFav = that.editTvList;
            } else {
                favouriteLists = that.channelConfig.favouriteLists;
                userFav = favouriteLists.getFavouriteList(favList.favID);
                that.editTvList = userFav;
            }
        } else if (favtype === 'RADIO') {
            if (that.editRadioList) {
                userFav = that.editRadioList;
            } else {
                favouriteLists = that.channelConfig.favouriteLists;
                userFav = favouriteLists.getFavouriteList(favList.favID);
                that.editRadioList = userFav;
            }
        }

        if (userFav) {
            var len_grp;
            fResult = userFav.remove(index);
            hx.log('dsrvChlist', 'moveChListItems remove fResult=' + fResult);
            if (fResult !== true) {
                favList = undefined;
                userFav = undefined;
            }
            favList.chList.splice(index, 1);
            hx.log('dsrvChlist', 'removeFromFavList web array splice???');
            //that.updateFavList();
            //if (that.channel_db) {
            //    len_grp = that.channel_db.length;
            //    for (var i = 0; i < len_grp; i += 1) {
            //        if (that.channel_db[i].name === favtype) {
            //            that.channel_db[i].list = favList.chList;
            //            hx.log('dsrvChlist', 'removeFromFavList web array splice !!!');
            //            break;
            //        }
            //    }
            //}

            fResult = userFav.insertBefore(insertIdx, ccid);
            hx.log('dsrvChlist', 'moveChListItems ccid=' + ccid);
            hx.log('dsrvChlist', 'moveChListItems params.insertIdx=' + insertIdx);
            hx.log('dsrvChlist', 'moveChListItems insertBefore fResult=' + fResult);
            if (fResult !== true) {
                favList = undefined;
                userFav = undefined;
                return false;
            }
            InternalParam = {};
            InternalParam.ccid = ccid;
            InternalParam.group = "ALL";
            var OipfChannel = this._getChannelByCcid(InternalParam);
            favList.chList.splice(insertIdx, 0, OipfChannel);
            hx.log('dsrvChlist', 'addToFavList web array push ???');
            //if (that.channel_db) {
            //    len_grp = that.channel_db.length;
            //    for (var i = 0; i < len_grp; i += 1) {
            //        if (that.channel_db[i].name === favtype) {
            //            that.channel_db[i].list = favList.chList;
            //            hx.log('dsrvChlist', 'addToFavList web array push !!!');
            //            break;
            //        }
            //    }
            //}
        } else {
            return false;
        }
        favList = undefined;
        userFav = undefined;
        favouriteLists = undefined;

        if (params.cb !== undefined) {
            setTimeout(params.cb(true), 0);
        }

        if (fResult) {
            fResult = undefined;
            return insertIdx;
        }
    }
    moveChListItemWithTargetIdx(params): any {
        var that = this;
        var ccid = params.ccid;
        var favtype = params.favtype;
        var favList = null, index = -1, insertIdx = -1;
        var InternalParam: any = {};
        var n;
        var len = 0;
        var len_grp;
        var fResult;
        var userFav;

        hx.log('dsrvChlist', 'moveChListItems ccid=' + ccid);
        if ((!favtype) || (!ccid)) {
            return false;
        }
        InternalParam.group = favtype;

        favList = that.getChlist(InternalParam);
        /*
         if (favList.favID === undefined) {
         //return false;
         }*/

        len = favList.chList.length;

        // find same service
        for ( n = 0; n < len; n += 1) {
            if (favList.chList[n].ccid === ccid) {
                index = n;
                break;
            }
        }

        insertIdx = params.targetIdx + params.offset;

        if (index < insertIdx) {
            //insertIdx -= 1;
            insertIdx = params.targetIdx;
        }

        hx.log('dsrvChlist', 'moveChListItems web array index=' + index);
        if (-1 === index) {
            return false;
        }

        var favouriteLists;
        if (favtype === 'TV') {
            if (that.editTvList) {
                userFav = that.editTvList;
            } else {
                favouriteLists = that.channelConfig.favouriteLists;
                userFav = favouriteLists.getFavouriteList(favList.favID);
                that.editTvList = userFav;
            }
        } else if (favtype === 'RADIO') {
            if (that.editRadioList) {
                userFav = that.editRadioList;
            } else {
                favouriteLists = that.channelConfig.favouriteLists;
                userFav = favouriteLists.getFavouriteList(favList.favID);
                that.editRadioList = userFav;
            }
        }

        if (userFav) {
            fResult = userFav.remove(index);
            hx.log('dsrvChlist', 'moveChListItems remove fResult=' + fResult);
            if (fResult !== true) {
                return false;
            }
            favList.chList.splice(index, 1);
            //that.updateFavList();
            //if (that.channel_db) {
            //    len_grp = that.channel_db.length;
            //    for ( i = 0; i < len_grp; i += 1) {
            //        if (that.channel_db[i].name === favtype) {
            //            that.channel_db[i].list = favList.chList;
            //            hx.log('dsrvChlist', 'removeFromFavList web array splice !!!');
            //            break;
            //        }
            //    }
            //}

            fResult = userFav.insertBefore(insertIdx, ccid);
            hx.log('dsrvChlist', 'moveChListItems ccid=' + ccid);
            hx.log('dsrvChlist', 'moveChListItems params.insertIdx=' + insertIdx);
            hx.log('dsrvChlist', 'moveChListItems insertBefore fResult=' + fResult);
            if (fResult !== true) {
                return false;
            }
            InternalParam = {};
            InternalParam.ccid = ccid;
            InternalParam.group = "ALL";
            var OipfChannel = this._getChannelByCcid(InternalParam);
            favList.chList.splice(insertIdx, 0, OipfChannel);
            //if (that.channel_db) {
            //    len_grp = that.channel_db.length;
            //    for (var  i = 0; i < len_grp; i += 1) {
            //        if (that.channel_db[i].name === favtype) {
            //            that.channel_db[i].list = favList.chList;
            //            hx.log('dsrvChlist', 'addToFavList web array push !!!');
            //            break;
            //        }
            //    }
            //}
        } else {
            favList = undefined;
            favouriteLists = undefined;
            return false;
        }
        OipfChannel = undefined;
        favList = undefined;
        userFav = undefined;
        favouriteLists = undefined;

        if (params.cb !== undefined) {
            setTimeout(params.cb(true), 0);
        }

        if (fResult) {
            fResult = undefined;
            return insertIdx;
        }
    }
    editChListDoCommit(param) {
        hx.log("dsrvChlist", "editChListDoCommit() called - param.group : " + param.group);
        if (param.group === 'TV') {
            if (this.editTvList) {
                this.editTvList.commit();
            }
        } else if (param.group === 'RADIO') {
            if (this.editRadioList) {
                this.editRadioList.commit();
            }
        }
    }
    outFromEditChannels() {
        var that = this;
        that.editTvList = undefined;
        that.editRadioList = undefined;
    }
    getChannelByTriplet(params) {
        var onid = params.onid;
        var tsid = params.tsid;
        var sid = params.sid;
        hx.log('dsrvChlist', 'onid = ' + onid + ', tsid = ' + tsid + ', sid = ' + sid);

        var i, j, a = this, channel;

        for ( i = 0; i < a.channel_db.length; i += 1) {
            for ( j = 0; j < a.channel_db[i].list.length; j += 1) {
                channel = a.channel_db[i].list[j];
                if (channel.onid === onid && channel.tsid === tsid && channel.sid === sid) {
                    return {
                        channel : channel,
                        group : a.channel_db[i].name
                    };
                }
            }
        }
        hx.log('dsrvChlist', 'getChannelByTriplet return null!!!');
        return null;
    }
    getChannelByTripletExceptALL(params) {
        var onid = params.onid;
        var tsid = params.tsid;
        var sid = params.sid;
        hx.log('dsrvChlist', 'onid = ' + onid + ', tsid = ' + tsid + ', sid = ' + sid);

        var i, j, a = this, channel;

        for ( i = 0; i < a.channel_db.length; i += 1) {
            for ( j = 0; j < a.channel_db[i].list.length; j += 1) {
                channel = a.channel_db[i].list[j];
                if (a.channel_db[i].name !== 'ALL' && channel.onid === onid && channel.tsid === tsid && channel.sid === sid) {
                    return {
                        channel : channel,
                        group : a.channel_db[i].name
                    };
                }
            }
        }
        hx.log('dsrvChlist', 'getChannelByTripletExceptALL return null!!!');
        return null;
    }
    getChannelByCcidExceptALL(param) {
        var i, j, a = this, channel;

        for ( i = 0; i < a.channel_db.length; i += 1) {
            for ( j = 0; j < a.channel_db[i].list.length; j += 1) {
                channel = a.channel_db[i].list[j];
                if (a.channel_db[i].name !== 'ALL' && channel.ccid === param.ccid) {
                    return {
                        channel : channel,
                        group : a.channel_db[i].name
                    };
                }
            }
        }

        return null;
    }
    getChannelByCcidInFav(param) {
        var i, j, a = this, channel;

        for ( i = 0; i < a.channel_db.length; i += 1) {
            for ( j = 0; j < a.channel_db[i].list.length; j += 1) {
                channel = a.channel_db[i].list[j];
                if (a.channel_db[i].name.indexOf('Favourite') >= 0 && channel.ccid === param.ccid) {
                    return {
                        channel : channel,
                        group : a.channel_db[i].name
                    };
                }
            }
        }

        return null;
    }
    getChannelGroupList () {
        var i, count, length, id, name, channelGroupList = [];
        var groupIds = hx.config.CPgInfoHub.chGroup.groupDB;
        var groupNames = hx.config.CPgInfoHub.chGroup.groupName;
        length = groupIds.length;
        for ( i = 0; i < length; i += 1) {
            id = groupIds[i];
            if (id === 'A-Z') {
                id = 'AGALL';
            }
            count = this.getChannelCount({
                group : id
            }).chCnt;
            if (count) {
                if (id.indexOf('Favourite') >= 0) {
                    name = this.getFavListName({
                        favtype : id
                    });
                    switch (name) {
                        case 'Favourite1':
                            name = hx.l('LOC_FAVOURITE1_ID');
                            break;
                        case 'Favourite2':
                            name = hx.l('LOC_FAVOURITE2_ID');
                            break;
                        case 'Favourite3':
                            name = hx.l('LOC_FAVOURITE3_ID');
                            break;
                        case 'Favourite4':
                            name = hx.l('LOC_FAVOURITE4_ID');
                            break;
                        case 'Favourite5':
                            name = hx.l('LOC_FAVOURITE5_ID');
                            break;
                        default:
                            name = name;
                            break;
                    }
                } else {
                    name = hx.l(groupNames[i]);
                }
                channelGroupList.push({
                    id : id,
                    name : name
                });
            }
        }
        return channelGroupList;
    }
    /*
     * It is just for channel list update function via OIFP favourite API..
     */
    removeChannels(params) {
        var that = this, chObj, chItem, idx, len_of_list, findObj: any = {};

        len_of_list = params.deleteChLists.length;
        hx.log('dsrvChlist', '###[removeChannels] - params.deleteChLists.length : ' + len_of_list);

        findObj.group = params.group;
        var favList = that.getChlist(findObj);

        if (favList.favID === undefined) {
            return false;
        }

        var foundIdxArray = [], idxFav, len_of_favlist = favList.chList.length;
        var favouriteLists = that.channelConfig.favouriteLists;

        for ( idx = 0; idx < len_of_list; idx += 1) {
            chItem = params.deleteChLists[idx];

            for ( idxFav = 0; idxFav < len_of_favlist; idxFav += 1) {
                if (favList.chList[idxFav].ccid === chItem.ccid) {
                    hx.log('dsrvChlist', ' del [' + idx + '].name=' + favList.chList[idxFav].name + ',ccid=' + favList.chList[idxFav].ccid + ' , favIdx : ' + idxFav);
                    foundIdxArray.push(idxFav);
                    break;
                }
            }
        }

        // sort numerically and descending order.
        // Because, if we called remove() in favourite api,
        // that favourite item will be delete in favourite list object, actually.
        foundIdxArray.sort(function(a, b) {
            return b - a;
        });

        var favOipfObj = favouriteLists.getFavouriteList(favList.favID);
        hx.log('dsrvChlist', 'removeFromFavList favOipfObj.length=' + favOipfObj.length);
        if (favOipfObj) {
            for ( idxFav = 0; idxFav < foundIdxArray.length; idxFav += 1) {
                // Ths is just for debugging, START
                /*
                 {
                 var chObj = favOipfObj.item(foundIdxArray[idxFav]);
                 if (chObj) {
                 hx.log('dsrvChlist', ' FavItem [' + foundIdxArray[idxFav] + '].name=' + chObj.name + ' , ccid=' + chObj.ccid);
                 } else {
                 hx.log('dsrvChlist', ' FavItem [' + foundIdxArray[idxFav] + '] is invalid');
                 }

                 }
                 */
                // Ths is just for debugging, END
                favOipfObj.remove(foundIdxArray[idxFav]);
                favList.chList.splice(foundIdxArray[idxFav], 1);
            }
            favOipfObj.commit();
            hx.log('dsrvChlist', 'removeFromFavList favOipfObj commit!!');
        }

        //if (that.channel_db) {
        //    var len_grp = that.channel_db.length;
        //    for ( idx = 0; idx < len_grp; idx += 1) {
        //        if (that.channel_db[idx].name === params.group) {
        //            that.channel_db[idx].list = favList.chList;
        //            hx.log('dsrvChlist', 'update refreshed channel list array');
        //            break;
        //        }
        //    }
        //}
        favList = undefined;
        foundIdxArray = [];
        favOipfObj = undefined;
        favouriteLists = undefined;
        params.deleteChLists = [];
    }
    /*
     * It is just for channel list update function via OIFP favourite API..
     */
    updateChannels(params) {
        var that = this, len_of_list, findObj: any = {};

        len_of_list = params.updatedChLists.length;
        hx.log('dsrvChlist', '###[updateChannels] - updated channel list count : ' + len_of_list);

        findObj.group = params.group;
        var favList = that.getChlist(findObj);
        if (favList.favID === undefined) {
            return false;
        }

        var chItem, idxFav, idx, len_of_favlist = favList.chList.length;

        for ( idx = 0; idx < len_of_list; idx += 1) {
            chItem = params.updatedChLists[idx];

            for ( idxFav = 0; idxFav < len_of_favlist; idxFav += 1) {
                if (favList.chList[idxFav].ccid === chItem.ccid) {
                    hx.log('dsrvChlist', ' modify [' + idx + '].name=' + favList.chList[idxFav].name + ',ccid=' + favList.chList[idxFav].ccid);

                    if (chItem.isLockChange) {
                        favList.chList[idxFav].manualBlock = chItem.lock;
                        hx.log('dsrvChlist', ' \tmodify Lock :' + favList.chList[idxFav].manualBlock);
                    }
                    if (chItem.isNameChange) {
                        favList.chList[idxFav].name = chItem.name;
                        hx.log('dsrvChlist', ' \tmodify Name :' + favList.chList[idxFav].name);
                    }
                    break;
                }
            }
        }
        favList = undefined;
        params.updatedChLists = [];
    }
    /*********** External API End ****************/

    /*********** Internal Function Start ****************/

        // TV / RADIO / HD / FAVOURITE 1,2,3,4,5 / CAS
        // TODO : Add functionality User Favourite functionality.
    _ChReload(favouriteLists) {
        var that = this;
        var name, result = [];
        var db;
        var groupIds;
        var i;
        var len;

        if (hx.emulMode) {
            db = {
                'ALL' : [],
                'TV' : [],
                'RADIO' : [],
                'HDTV' : [],
                'FREE' : [],
                'PAY' : []
            };
            groupIds = ['ALL', 'TV', 'RADIO', 'HDTV', 'FREE', 'PAY'];
        } else {
            db = {
                'ALL' : [],
                'TV' : [],
                'RADIO' : [],
                'HDTV' : [],
                'FREE' : [],
                'PAY' : [],
                'Favourite1' : [],
                'Favourite2' : [],
                'Favourite3' : [],
                'Favourite4' : [],
                'Favourite5' : []
            };
            groupIds = ['ALL', 'TV', 'RADIO', 'HDTV', 'FREE', 'PAY', 'Favourite1', 'Favourite2', 'Favourite3', 'Favourite4', 'Favourite5'];
        }
        var push = function(groupName, channel) {
            if (!db[groupName]) {
                db[groupName] = [];
            }
            db[groupName].push(channel);
        };

        var addChannelToDb = function(param) {
            // Add All
            push('ALL', c);

            // Add PAY/FREE
            if (c.casIDs && c.casIDs.length) {
                push('PAY', c);
            } else {
                push('FREE', c);
            }

            // Add TV / Radio / HD
            if (c.channelType === c.TYPE_TV) {
                push('TV', c);
                if (c.isHD) {
                    push('HDTV', c);
                }
            } else if (c.channelType === c.TYPE_RADIO) {
                push('RADIO', c);
            }

            if (hx.config.useAlphabetGroup === 1) {
                var ucAlphabetDB;

                // Add Alphabet list
                push('AGALL', c);

                ucAlphabetDB = that._getAlphabetGruop(c);
                push(ucAlphabetDB, c);
            }
        };

        if (hx.emulMode || !hx.config.useFavouriteCollection) {
            // use channelConfig's whole channel list.
            var channelList = that.channelConfig.channelList;
            len = channelList.length;
            for ( i = 0; i < len; i += 1) {
                var c = channelList.item(i);
                addChannelToDb(c);
            }
            if (db['ALL'].length === 0) {
                return result;
            }

            for (name in db) {
                if (db[name]) {
                    result.push({
                        'name' : name,
                        'list' : db[name],
                        'ext' : {
                            'groupid' : groupIds.indexOf(name)
                        }
                    });
                }
            }
            channelList = undefined;
        } else {
            // use OIPF favourite list for channel list.
            hx.log('dsrvChlist', '_ChReload channelConfig.favouriteLists!!!!');
            var list, chList = [];
            len = favouriteLists.length;
            for ( i = 0; i < len; i += 1) {
                list = favouriteLists.item(i);
                name = list.name;
                chList = that._conv_oipf_to_list(list);
                //db[name] = chList;
                // if (db['ALL'].length === 0) {
                // return result;
                // }
                hx.log('dsrvChlist', '[' + i + '] add channel list name : [' + name + ']' + ' favID : [' + list.favID + ']' + ' groupIds.indexOf(name) : ' + groupIds.indexOf(name));
                hx.log('dsrvChlist', '\t chList.length : ' + chList.length);
                if (db[name]) {
                    result.push({
                        'name' : name,
                        'list' : chList,
                        'favID' : list.favID,
                        'ext' : {
                            'groupid' : groupIds.indexOf(name)
                        }
                    });
                }
            }
            list = undefined;
            chList = undefined;
        }
        return result;
    }

    _isLoadedFavList(favID) {
        var a = this;
        var i, ret = false;
        for ( i = 0; i < a.channel_db.length; i += 1) {
            if (a.channel_db[i].favID === favID) {
                ret = true;
                break;
            }
        }
        return ret;
    }
    _saveFavID(idx, favID) {
        var favIdArray = ['favID1', 'favID2', 'favID3', 'favID4', 'favID5'];
        switch (favIdArray[idx]) {
            case 'favID1':
                hx.svc.WEB_STORAGE.setFavListID({
                    'favID1' : favID
                });
                break;
            case 'favID2':
                hx.svc.WEB_STORAGE.setFavListID({
                    'favID2' : favID
                });
                break;
            case 'favID3':
                hx.svc.WEB_STORAGE.setFavListID({
                    'favID3' : favID
                });
                break;
            case 'favID4':
                hx.svc.WEB_STORAGE.setFavListID({
                    'favID4' : favID
                });
                break;
            case 'favID5':
                hx.svc.WEB_STORAGE.setFavListID({
                    'favID5' : favID
                });
                break;
            default:
                break;
        }
    }
    _loadFavLists(channel_db, favouriteLists) {
        var that = this, result;
        var idx, favIdArray = ['favID1', 'favID2', 'favID3', 'favID4', 'favID5'];
        hx.log("dsrvChlist", "_loadFavLists() called");

        var FavListIds = hx.svc.WEB_STORAGE.getFavListID();

        for ( idx = 0; idx < favIdArray.length; idx += 1) {
            hx.log('dsrvChlist', 'get favouriteList id [' + FavListIds[favIdArray[idx]] + ']');
            if (hx.config.useFavouriteCollection) {
                // New Channel List via OIPF Favorutie...
                if (!that._isLoadedFavList(FavListIds[favIdArray[idx]])) {
                    result = that._loadFavList(FavListIds[favIdArray[idx]], (idx + 1), favouriteLists);
                    if (result !== undefined) {
                        that._saveFavID(idx, result.favID);
                        channel_db.push({
                            'name' : result.name,
                            'list' : result.userFavList,
                            'favID' : result.favID
                        });
                    }
                } else {
                    hx.log('dsrvChlist', '\tFAV id [' + FavListIds[favIdArray[idx]] + '] is already loaded!!');
                }
            } else {
                // OLD Channel List via ChannelConfig's Whole Channel List
                result = that._loadFavList(FavListIds[favIdArray[idx]], (idx + 1), favouriteLists);
                if (result !== undefined) {

                    channel_db.push({
                        'name' : ("Favourite1"),
                        'list' : result.userFavList,
                        'favID' : result.favID
                    });
                }
            }
        }
        FavListIds = undefined;
        result = undefined;
    }
    _conv_oipf_to_list(oChList) {
        var i, len, ChListArray = [];
        var service;

        if (!oChList) {
            return ChListArray;
        }

        len = oChList.length;
        for ( i = 0; i < len; i += 1) {
            service = oChList.item(i);
            if (service.removed !== true) {
                ChListArray.push(oChList.item(i));
            }
        }
        service = undefined;
        return ChListArray;
    }
    _loadFavList(p, idx, favouriteLists) {
        var that = this;
        var favID = p;
        hx.log('dsrvChlist', '_loadFavList channelConfig.favouriteLists!!!!');
        var userFav, userFavArray = [];
        hx.log('dsrvChlist', '###[_loadFavList] load favID : ' + favID);
        if (null === favID) {
            // just create dummy favourite list
            userFav = favouriteLists.createFavouriteList('Favourite' + idx);
            if (userFav) {
                userFavArray = this._conv_oipf_to_list(userFav);
                favID = userFav.favID;
                that._saveFavID((idx - 1), favID);
                return {
                    'favID' : favID,
                    'userFavList' : userFavArray,
                    'name' : userFav.name
                };
            }
        }

        userFav = favouriteLists.getFavouriteList(favID);
        if (userFav === null) {
            userFav = favouriteLists.createFavouriteList('Favourite' + idx);
            if (userFav) {
                userFavArray = this._conv_oipf_to_list(userFav);
                favID = userFav.favID;
                that._saveFavID((idx - 1), favID);
                return {
                    'favID' : favID,
                    'userFavList' : userFavArray,
                    'name' : userFav.name
                };
            }
        } else {
            favID = userFav.favID;
        }

        userFavArray = this._conv_oipf_to_list(userFav);
        hx.log('dsrvChlist', '###[_loadFavList] result favID : ' + favID);

        return {
            'favID' : favID,
            'userFavList' : userFavArray,
            'name' : userFav.name
        };
    }
    _findChList(group) {
        var i, a = this;
        var len_grp = a.channel_db.length;
        if (group === 'Favourite1' || group === 'Favourite2' || group === 'Favourite3' || group === 'Favourite4' || group === 'Favourite5') {
            var FavListIds = hx.svc.WEB_STORAGE.getFavListID();
            var targetFavId;
            switch(group) {
                case 'Favourite1':
                    targetFavId = FavListIds.favID1;
                    break;
                case 'Favourite2':
                    targetFavId = FavListIds.favID2;
                    break;
                case 'Favourite3':
                    targetFavId = FavListIds.favID3;
                    break;
                case 'Favourite4':
                    targetFavId = FavListIds.favID4;
                    break;
                case 'Favourite5':
                    targetFavId = FavListIds.favID5;
                    break;
            }
            for ( i = 0; i < len_grp; i += 1) {
                if (a.channel_db[i].favID !== undefined) {
                    if (targetFavId === a.channel_db[i].favID && a.channel_db[i]) {
                        return a.channel_db[i];
                    }
                }
            }
        } else {
            for ( i = 0; i < len_grp; i += 1) {
                if (group === a.channel_db[i].name) {
                    return a.channel_db[i];
                }
            }
        }
    }
    /*
     * We have to rebuild subAV
     */
    _getChannelByCcid(param) {
        var i, j, a = this, ccid, group, channel;

        ccid = param.ccid;
        group = ((!!param.group) ? param.group : 'ALL');

        var chList = a._findChList(group);
        if (chList) {
            for ( i = 0; i < chList.list.length; i += 1) {
                channel = chList.list[i];
                if (channel.ccid === ccid) {
                    channel.groupName = chList.name;
                    return channel;
                }
            }
        }
        return null;
    }
    _getChannelByLcn(param) {
        var i, j, a = this, channel;
        var lcn = param.lcn;
        var group = ((!!param.group) ? param.group : 'ALL');
        var chList = a._findChList(group);
        if (chList) {
            for ( i = 0; i < chList.list.length; i += 1) {
                channel = chList.list[i];
                if (channel.majorChannel === lcn) {
                    return channel;
                }
            }
        }
        return null;
    }
    _removeChannelByCcid(group, ccid) {
        var i, j, a = this, channel;

        for ( i = 0; i < a.channel_db.length; i += 1) {
            if (group === a.channel_db[i].name || 'ALL' === a.channel_db[i].name) {
                for ( j = 0; j < a.channel_db[i].list.length; j += 1) {
                    channel = a.channel_db[i].list[j];
                    if (channel.ccid === ccid) {
                        a.channel_db[i].list.splice(j, 1);
                    }
                }
            }
        }
    }
    _checkChannelChangableCL(channel) {
        var ret;
        if (channel) {
            ret = hx.stbObject.videoBroadcast.checkChannelChangable(channel);
            hx.log('dsrvChlist', '###[_checkChannelChangableCL]  ccid : [' + channel.ccid + '] name : [' + channel.name + '] result : [' + ret + ']');
            return ret;
        }
    }

    _getNextChannel(param, direction, grp, count = 0) {
        var i, j, a = this, //
            ccid, channel, len_grp, len_list, ret_channel;

        ccid = param.ccid;
        len_list = grp.list.length;
        if (count >= len_list) {
            return null;
        }
        for ( j = 0; j < len_list; j += 1) {
            channel = grp.list[j];
            if (channel.ccid === ccid) {
                if (direction > 0) {
                    // Next Channel
                    if ((j + direction) < len_list) {
                        ret_channel = grp.list[(j + direction)];
                    } else {
                        ret_channel = grp.list[0];
                    }
                } else {
                    // Prev Channel
                    if ((j + direction) >= 0) {
                        ret_channel = grp.list[(j + direction)];
                    } else {
                        ret_channel = grp.list[(len_list - 1)];
                    }
                }
                if (ret_channel.hidden) {
                    param.ccid = ret_channel.ccid;
                    return this._getNextChannel(param, direction, grp, ++count);
                } else {
                    return ret_channel;
                }
            }
        }

        return null;
    }
    _getAlphabetGruop(param) {
        var ucFistChat, i = 0, szAlphabetGroup = 'AG', szFindAlphabetGroup = 'AG', szDigitGroup = 'AGDIGHT', szEtcGroup = 'AGETC';
        var bFind = false;
        var aAlphabetlist = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'];
        var aDigitlist = ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9'];

        ucFistChat = param.name.charAt(0);

        // Check match with alphabet
        for ( i = 0; i < aAlphabetlist.length; i += 1) {
            if (ucFistChat.toUpperCase() === aAlphabetlist[i]) {
                bFind = true;
                szFindAlphabetGroup = szAlphabetGroup + aAlphabetlist[i];
                break;
            }
        }
        if (bFind === true) {
            return szFindAlphabetGroup;
        }

        // check match with digit
        for ( i = 0; i < aDigitlist.length; i += 1) {
            if (ucFistChat === aDigitlist[i]) {
                bFind = true;
                break;
            }
        }
        if (bFind === true) {
            return szDigitGroup;
        }

        // check match with etc
        return szEtcGroup;
    }
    /*********** Internal Function End ****************/
}
export = cDLCHList;
