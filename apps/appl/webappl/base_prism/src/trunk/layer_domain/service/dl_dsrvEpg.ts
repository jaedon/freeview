/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvEpg.js
/*jslint vars:true */
/*jslint nomen:true */
//    EPG: ['layer_domain/service/dl_dsrvEpg.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");
import __PROVIDER__ = require("../../pages/util/provider");


class cDLSvcEpgCache {
    private tables = [];
    constructor() {
    }
    isHit(channel, condition) {
        var that = this;
        var i, j, cLen, rLen, count = 0, startTime, endTime;
        var table;

        cLen = channel.length;

        for ( i = 0; i < cLen; i += 1) {
            table = that.tables[channel[i].ccid];

            if (table) {
                rLen = table.ranges.length;

                if (rLen) {
                    for ( j = 0; j < rLen; j += 1) {
                        startTime = table.ranges[j].startTime;
                        endTime = table.ranges[j].endTime;

                        if (condition.startTime >= startTime && condition.endTime <= endTime) {
                            count += 1;
                        }
                    }
                }
            }
        }

        return count === cLen ? true : false;
    }
    setRange(channel, condition) {
        var that = this, i, j, cLen, rLen, range, table, ccid;
        var startTime = condition.startTime, endTime = condition.endTime;
        var garbage = [];

        for ( i = 0, cLen = channel.length; i < cLen; i += 1) {
            ccid = channel[i].ccid;

            if (!that.tables[ccid]) {
                that.tables[ccid] = TAFFY([]);
                that.tables[ccid].ranges = [];
            }

            table = that.tables[ccid];

            if (table.ranges.length) {
                for ( j = 0, rLen = table.ranges.length; j < rLen; j += 1) {
                    range = table.ranges[j];

                    if (range.startTime >= startTime && range.endTime < endTime) {
                        garbage.push(i);
                    }
                }

                if (garbage.length) {
                    rLen = garbage.length;
                    for ( j = rLen - 1; j >= 0; j -= 1) {
                        table.ranges.splice(garbage[j], 1);
                    }
                }

                if (table.ranges.length) {
                    for ( j = 0, rLen = table.ranges.length; j < rLen; j += 1) {
                        range = table.ranges[j];

                        if (range.startTime < startTime) {
                            range.startTime = startTime;

                            if (table.ranges[j + 1]) {
                                range.endTime = endTime;
                                table.ranges.splice(j + 1, 1);
                            }
                        }
                    }
                } else {
                    table.ranges.push({
                        startTime : startTime,
                        endTime : endTime
                    });
                }
            } else {
                table.ranges.push({
                    startTime : startTime,
                    endTime : endTime
                });
            }

            that.tables[ccid] = table;
        }
    }
    insert(programme) {
        var that = this, table;
        var id = programme.programmeID;
        var ccid = programme.channel.ccid;
        var startTime = programme.startTime;
        var endTime = startTime + programme.duration;

        if (!that.tables[ccid]) {
            that.tables[ccid] = TAFFY([]);
            that.tables[ccid].ranges = [];
        }

        table = that.tables[ccid];

        table.insert({
            id : id,
            startTime : startTime,
            endTime : endTime,
            pluginObject : programme
        });

        that.tables[ccid] = table;
    }
    update(programme) {
        var that = this;
        var table = that.tables[programme.channel.ccid];
        var startTime = programme.startTime;
        var endTime = startTime + programme.duration;

        if (table) {
            table({
                id : programme.programmeID
            }).update({
                startTime : startTime,
                endTime : endTime,
                pluginObject : programme
            });
        }
    }
    remove(ccid, condition) {
        var that = this;
        var table = that.tables[ccid];

        if (table) {
            table(condition).remove();
        }
    }
    select(channel, condition) {
        var that = this;
        var programmesCollection = [];
        var table;
        var i, cLen;

        cLen = channel.length;

        function select(table, condition, collection) {
            table(condition).order('startTime').each(function(programme) {
                collection.push(new __PROVIDER__.CProgrammeProvider({oipfData : programme.pluginObject}));
            });

            return collection;
        }

        for ( i = 0; i < cLen; i += 1) {
            table = that.tables[channel[i].ccid];
            if (table) {
                programmesCollection[i] = select(table, condition, []);
            }
        }

        return programmesCollection;
    }
    truncate() {
        this.tables = [];
    }
}

class cDLSvcEpg extends __KERNEL__ implements ifSVCEPG {
    private EpgDatacache = null;
    private useEventDataCache = false;
    private listeners = [];
    private queue = [];
    private _currentSearchIdForUI = null;
    private searchManager = null;
    private searchTarget = {
        scheduled: 1,
        onDemand: 2,
        programme: 3,
        channel: 4,
        file: 5,
        recording: 6
    };
    private searchState = {
        finished: 0,
        available: 1
    };
    private updateState = {
        available: 1
    };
    private cached;

    private _workingQueue = [];
    private _cachedOnAirProgrammes = [];
    private _autoEmptyCacheTimerID = null;
    private _searchOngoing = false;
    private _liveData = TAFFY([]);
    private _liveSearchId = null;
    private name: string;

    constructor(aServiceName: string) {
        super(aServiceName);
        var that = this, evnt, args;

        //hx.logadd('dsrvEpg');
        //hx.logadd('dsrvEpg-getLiveProgrammes');
        //hx.logadd('dsrvEpg-SearchHandle');

        hx.log('dsrvEpg', 'init() called');

        that.searchManager = hx.stbObject.searchManager;
        that.searchManager.onMetadataSearch = function(search, state) {
            that.onMetadataSearch(search, state);
        };
        that.searchManager.onMetadataUpdate = function(action, info, object, type) {
            that.onMetadataUpdate(action, info, object, type);
        };

        if (that.useEventDataCache) {
            that.EpgDatacache = new cDLSvcEpgCache();
        }
        hx.svc.CH_LIST.addEventCb("ChannelListUpdate", this, () => {
            if (this.EpgDatacache) {
                this.EpgDatacache.truncate();
            }
            hx.log("dsrvEpg", "EPG cache truncated");
        })
    }
    // ####################################################################################################
    // Public Methods
    /*
     * findProgrammes is not intent to update Event.
     */
    findProgrammes(aParam: if_EPG_findProgrammes_0): any {
        hx.log('dsrvEpg', 'findProgrammes() called');
        aParam.notUI = true;
        return this._doSearch(aParam);
    }
    /*
     * getProgrammes is wait with update Event.
     */

    getProgrammes(aParam: if_EPG_getProgrammes_0, range?: number): any {
        var that = this, searchId, now, channel, startTime, endTime;

        hx.log('dsrvEpg', 'getProgrammes() called -- param.isOnAir : ' + aParam.isOnAir);

        channel = aParam.channel;
        aParam.channel = channel instanceof Array ? channel : [channel];

        if (aParam.isOnAir) {
            now = new Date();
            startTime = now.toSeconds();
            endTime = startTime + 14400;
            // 4h : (60 * 60 * 4)
            //param.clause = 'endTime > ' + endTime;
            //XXX: #DTG #.2.15.4 #SI14_004 :: #EITsch full sub-table
            if (ENV.op === ENV.listOP.UKDTT) {
                aParam.clause = hx.util.format('endTime > {0}', startTime);
                aParam.startTime = startTime;
                aParam.endTime = endTime;
            } else {
                aParam.clause = hx.util.format('endTime > {0} AND startTime < {1}', startTime, endTime);
                aParam.startTime = startTime;
                aParam.endTime = endTime;
            }
        } else {
            startTime = aParam.startTime;
            endTime = aParam.endTime;
            aParam.clause = hx.util.format('endTime > {0} AND startTime < {1}', startTime, endTime);
        }

        hx.log("dsrvEpg", hx.util.format('++ ##### [dl_dsrvEpg][75] : {0} ##### ++ ', aParam.clause));
        hx.log("dsrvEpg", hx.util.format('++ ##### {0}: {1} ##### ++ ', dateFormat(startTime * 1000, 'HH:MM'), dateFormat(endTime * 1000, 'HH:MM')));

        // CACHE : select
        if (that.useEventDataCache) {
            var isHitParam = {
                startTime : startTime,
                endTime : endTime
            };
            if (that.EpgDatacache && (startTime !== endTime) && //
                aParam.info && (that.cached.indexOf(aParam.info) > -1) && //
                that.EpgDatacache.isHit(channel, isHitParam)) {

                hx.log('dsrvEpg', '\t HIT cached!!!!');

                var cbParam = {
                    startTime : {
                        lt : endTime
                    },
                    endTime : {
                        gt : startTime
                    }
                };

                aParam.cb(that.EpgDatacache.select(aParam.channel, cbParam));

                // CACHE : delete
                if (that.cached.length > 20) {
                    that.cached = [];
                    that.EpgDatacache.truncate();
                }

                searchId = aParam.info;
            } else {
                throw "not in epg cache";
            }
        } else {
            hx.log('dsrvEpg', '\t GOTO _doSearch()');
            searchId = that._doSearch(aParam, range);
        }
        channel = undefined;
        aParam = undefined;
        return searchId;
    }
    getAllProgrammesOrderByTime(view, param, range) {
        var that = this, searchId, now, channels, endTime, EpgDatacache, db, index;

        hx.log('dsrvEpg', 'getAllProgrammesOrderByTime() called');

        channels = param.channels;
        param.channels = channels instanceof Array ? channels : [channels];

        now = param.now.toSeconds();
        if (view === 'guideNext') {
            param.clause = hx.util.format('startTime > {0}', now);
        } else if (view === 'guideBackward') {
            param.clause = hx.util.format('endTime < {0}', now);
        }
        param.notUI = true;
        hx.log('dsrvEpg', '\t GOTO _doSearch()');
        searchId = that._doSearch(param, range);

        return searchId;
    }

    retrieveLiveProgrammes(param?): void {
        var that = this, channel, now, startTime, endTime, maxOffset;
        hx.log('dsrvEpg', 'retrieveLiveProgrammes() called');
        channel = ( param ? param.channel : null);
        now = new Date().toSeconds();
        maxOffset = hx.svc.TSR.getMaxOffset();
        startTime = now - maxOffset;
        endTime = startTime + 60 * 60 * 4;
        if (!channel) {
            channel = hx.svc.CH_LIST.getCurrentChannel().channel;
        }
        var firstCached = that._liveData().first();
        var lastCached = that._liveData().last();
        if (firstCached && lastCached && (firstCached.ccid === channel.ccid)) {
            if ((firstCached.startTime < startTime) && (lastCached.endTime > endTime)) {
                hx.log('debug', 'retrieveLiveProgrammes() do not need to search again(already cached on same channel)');
                return;
            }
        }
        that._searchOngoing = true;
        that._liveData().remove();
        // delete previous queue...
        // Is it need or not?
        /*
         if (that._liveSearchId) {
         that._deleteQueue(that._liveSearchId);
         }
         */
        that._liveSearchId = hx.svc.EPG.getProgrammes({
            'isOnAir' : false,
            'channel' : channel,
            'startTime' : startTime,
            'endTime' : endTime,
            'notUI' : true,
            'cb' : function(result) {
                var i, length;
                var programme, programmes = result[0];
                length = programmes ? programmes.length : 0;
                hx.log("dsrvEpg", hx.util.format('++ ##### retrieveLiveProgrammes - length : {0} ##### ++ ', length));
                if (length > 0) {
                    for ( i = 0; i < length; i += 1) {
                        programme = programmes[i];

                        that._liveData.insert({
                            ccid : channel.ccid,
                            startTime : programme.getStartTime(),
                            endTime : programme.getEndTime(),
                            programme : programme
                        });
                    }
                }
                channel = undefined;
                programmes = undefined;
                result = undefined;
                that._searchOngoing = false;
            }
        });

        if (!that._liveSearchId) {
            hx.log("dsrvEpg", hx.util.format('++ #####  retrieveLiveProgrammes - that._liveSearchId : {0} ##### ++ ', that._liveSearchId));
            that._searchOngoing = false;
        }
    }
    /*
     * It is not garentee EPG data existance...
     */
    getLiveProgrammes(p): __PROVIDER__.CProgrammeProvider[] {
        var that = this, now, param, channel, ccid, startTime, endTime, //
            programmes = [], result = [], i, length;
        hx.log('dsrvEpg', 'getLiveProgrammes() called');
        now = new Date();
        param = p || {};
        channel = param.channel || hx.svc.CH_LIST.getCurrentChannel().channel;
        ccid = channel ? channel.ccid : null;
        startTime = param.startTime || now.toSeconds();
        endTime = param.endTime || now.toSeconds();
        result[0] = [];
        function getVideobroadcastProgrammes() {
            var item, items = [], idx;
            var temp = hx.stbObject.videoBroadcast.programmes;
            length = temp.length;
            for ( idx = 0; idx < length; idx += 1) {
                item = temp.item(idx);
                if (item.startTime < endTime && (item.startTime + item.duration) > startTime) {
                    items.push(new __PROVIDER__.CProgrammeProvider({oipfData : item}));
                }
            }
            temp = undefined;
            return items;
        }
        programmes = that._liveData({
            ccid : ccid,
            startTime : {
                '<' : endTime
            },
            endTime : {
                '>' : startTime
            }
        }).order('startTime').get();
        length = programmes.length;
        if (length > 0) {
            for ( i = 0; i < length; i += 1) {
                if(programmes[i].programme){
                    result[0].push(programmes[i].programme);
                }
            }
        }
        that.retrieveLiveProgrammes();
        if (result[0].length === 0) {
            result[0] = getVideobroadcastProgrammes();
        }
        hx.log("dsrvEpg", hx.util.format('++ #####  getLiveProgrammes - result[0].length : {0} ##### ++ ', result[0].length));
        programmes = undefined;
        return result;
    }
    executeQuery(param): void {
        var that = this;
        var search, onGoing;
        var target = param.target;
        var query = param.query;
        var result;

        hx.log('dsrvEpg', 'executeQuery: target=' + target);

        // create a new search
        search = that.searchManager.createSearch(target);
        search.setQuery(search.createQuery(query));

        // start search
        result = search.result;
        result.getResults(0, 100);

        param.notUI = true;
        onGoing = {
            'result' : result,
            'search' : search,
            'param' : param,
            'cb' : param.cb
        };

        if (param && param.channel) {
            param.channel = _.compact(param.channel);
        }
        that.queue.push(onGoing);
        search = undefined;
        result = undefined;
        onGoing = undefined;
    }
    _deleteQueue(searchOipfObj): void {
        var idx, len, queue, token, index_of_slot = -1, that = this;
        queue = that.queue;
        len = queue.length;
        hx.log("dsrvEpg", '_deleteQueue() searchOipfObj.id : ' + searchOipfObj.id);
        hx.log("dsrvEpg", 'queue.length : ' + len);
        for ( idx = 0; idx < len; idx += 1) {
            token = queue[idx];
            hx.log("dsrvEpg", hx.util.format('\t idx : [{0}]:search id [{1}]', idx, token.search.id));
            if (searchOipfObj.id === token.search.id) {
                index_of_slot = idx;
                break;
            }
        }
        if (index_of_slot === -1) {
            hx.log("dsrvEpg", 'can not find search id : ' + searchOipfObj.id);
        } else {
            hx.log("dsrvEpg", 'delete search slot queue : ' + searchOipfObj.id + ' index_of_slot : ' + index_of_slot);
            queue.splice(index_of_slot, 1);
        }
    }
// ####################################################################################################
// Event Handlers
    onMetadataSearch(search, state): void {
        var that = this, programmes;
        hx.log('dsrvEpg', 'onMetadataSearch: state=' + state);
        var i, length, index = -1, queue = that.queue, token, data, cb, param, result, dataSearch, searchState = that.searchState;
        for ( i = 0, length = queue.length; i < length; i += 1) {
            token = queue[i];
            if (search.id === token.search.id) {
                index = i;
                break;
            }
        }
        if (index === -1) {
            hx.log("dsrvEpg", '++ ##### [dl_dsrvEpg][222] : Events that you do not need occurs. Ignore! ##### ++');
            return;
        }
        data = queue[index];
        cb = data.cb;
        param = data.param;
        result = data.result;
        dataSearch = data.search;
        if (!param.notUI && (this ._currentSearchIdForUI !== search.id)) {
            hx.log('debug', "onMetadataSearch: This UI result was already gone.");
        } else {
            if (state === searchState.finished || state === searchState.available) {
                result.update();
                queue.splice(index, 1);
                hx.log('debug', 'onMetadataSearch: search.searchTarget=' + dataSearch.searchTarget);
                hx.log('debug', 'onMetadataSearch: result.length=' + result.length);
                programmes = that._wrap(dataSearch.searchTarget, result, param);
                that._lazyExecute(param, programmes);
                cb(programmes);
                programmes = undefined;
                hx.log('dsrvEpg-SearchHandle', "");
                hx.log('dsrvEpg-SearchHandle', hx.util.format("@@@@@@@@@@@@@@@@@ Received Search Id : {0} [state: {2} ]", search.id, state));
                hx.log('dsrvEpg-SearchHandle', "");
            }
        }
        dataSearch = undefined;
        result = undefined;
        data = undefined;
        if (state === searchState.finished) {
            this._deleteQueue(search);
        }
    }
    onMetadataUpdate(action, info, object, type): void {
        hx.log('debug', hx.util.format('##### dsrvEpg.onMetadataUpdate ALL ##### +++++ action - {1}, info - {2}', action, info));
        if (this.updateState.available === action) {
            hx.log('debug', hx.util.format('##### dsrvEpg.onMetadataUpdate FireEvent!! ##### +++++ action - {1}, info - {2}', action, info));
            this.fireEvent_Legacy('MetadataUpdate', {
                'info' : info
            });
            if (this._liveSearchId === info) {
                this.retrieveLiveProgrammes();
            }

        }
    }
// ####################################################################################################
// Private Methods
    _doSearch(aParam: if_EPG_findProgrammes_0, range?: number): any {
        var i, c, length;
        var channel, clause;
        var search, result, onGoing;
        channel = aParam.channel;
        clause = aParam.clause;

        hx.log('dsrvEpg', '_doSearch() called');

        // create a new search
        search = this.searchManager.createSearch(this.searchTarget.scheduled);

        if (search.id === 0 || search.id === null) {
            hx.log('warning', hx.util.format(' ====== It is a very strange situation.! {0}.doSearch fail! ====== ', this.name));
            return;
        }

        // add a channel constraint
        if (channel) {
            channel = channel instanceof Array ? channel : [channel];
            for ( i = 0, length = channel.length; i < length; i += 1) {
                c = channel[i];
                if (c) {
                    if (c.pluginObject) {
                        search.addChannelConstraint(c.pluginObject);
                    } else {
                        search.addChannelConstraint(c);
                    }
                }
            }
            channel = undefined;
        }

        // create filter
        if (aParam.filter) {
            search.addConstraint('target', aParam.filter);
        }
        search.setQuery(this._createQuery(search, clause));

        // start search
        result = search.result;
        if (range) {
            result.getResults(0, range);
        } else {
            result.getResults(0, 100);
        }

        onGoing = {
            'result' : result,
            'search' : search,
            'param' : aParam,
            'cb' : aParam.cb
        };
        hx.log('dsrvEpg-SearchHandle', "@@@@@@@@@@@@@@@@@ New Search Id : " + search.id);

        if (aParam && aParam.channel) {
            aParam.channel = _.compact((<Channel[]>aParam.channel));
        }
        if (!aParam.notUI) {
            this._currentSearchIdForUI = search.id;
        }
        this.queue.push(onGoing);
        result = undefined;
        onGoing = undefined;
        return search;
    }
    _wrap(searchTarget, result, param): any {
        var that = this;
        var i, index, length, wrapObj, programme, programmes, programmesCollection = [], items = [], ccid, ccids = [], channel = param.channel;
        var startTime = param.startTime;
        var endTime = param.endTime;

        hx.log('dsrvEpg', '_wrap() called');

        if (searchTarget === that.searchTarget.scheduled) {
            // CACHE : set range
            if (that.useEventDataCache) {
                if (that.EpgDatacache) {
                    if (startTime !== endTime && !param.isOnAir) {
                        that.EpgDatacache.setRange(channel, {
                            startTime : startTime,
                            endTime : endTime
                        });
                    }
                }
            }

            if (channel) {
                for ( i = 0, length = channel.length; i < length; i += 1) {
                    ccid = channel[i].ccid;
                    ccids.push(ccid);
                    programmesCollection[i] = [];
                }
            }

            if (programmesCollection.length < 1) {
                programmesCollection[0] = [];
            }

            for ( i = 0, length = result.length; i < length; i += 1) {
                programme = result.item(i);
                wrapObj = new __PROVIDER__.CProgrammeProvider({oipfData : programme});
                ccid = wrapObj.ccid;
                index = ccids.indexOf(ccid);

                if (param.isBackward && param.isBackward === true) {
                    // mediaDeeplink
                    wrapObj.mediaDeeplinkExpireDate = programme.mediaDeeplinkExpireDate;
                    // DO NOT SET directly mediaDeeplinkUrl' may make a performance issue?
                    // that.mediaDeeplinkUrl = programme.mediaDeeplinkUrl;
                }

                if (index > -1) {

                    //hx.log('dsrvEpg', hx.util.format('##### _wrap #####
                    // ++++++++++++++++++++++++++++++++++++++++++++++++++', that.name));
                    //hx.log('dsrvEpg', hx.util.format('##### _wrap ##### +++++ index - {1}, programme.channel.majorChannel
                    // - {2}, programme.name - {3}', that.name, index, programme.channel.majorChannel, programme.name));
                    //hx.log('dsrvEpg', hx.util.format('##### _wrap #####
                    // ++++++++++++++++++++++++++++++++++++++++++++++++++', that.name));

                    programmesCollection[index].push(wrapObj);

                    // CACHE : insert a programme
                    if (that.useEventDataCache) {
                        if (that.EpgDatacache) {
                            if (startTime !== endTime && !param.isOnAir) {
                                that.EpgDatacache.remove(ccid, {
                                    id : programme.programmeID
                                });
                                that.EpgDatacache.insert(programme);
                            }
                        }
                    }

                } else {
                    programmesCollection[0].push(wrapObj);
                }
            }
            result = undefined;
            programme = undefined;
            wrapObj = undefined;
            return programmesCollection;
        }

        items[0] = [];

        if (searchTarget === that.searchTarget.channel) {
            for ( i = 0, length = result.length; i < length; i += 1) {
                items[0].push(new __PROVIDER__.CChannelProvider(result.item(i)));
            }
        } else if (searchTarget === that.searchTarget.file) {
            for ( i = 0, length = result.length; i < length; i += 1) {
                items[0].push(new __PROVIDER__.CHumaxContentProvider(result.item(i)));
            }
        } else if (searchTarget === that.searchTarget.recording) {
            var x;
            for ( i = 0, length = result.length; i < length; i += 1) {
                x = new __PROVIDER__.CRecordingProvider(result.item(i));
                hx.log('dsrvEpg', hx.util.format('##### _wrap ##### ++++++++++++++++++++++++++++++++++++++++++++++++++ uid = {0}', x.uid));
                items[0].push(x);
            }
            x = undefined;
        }
        result = undefined;
        return items;
    }
    _createQuery(search, clause): any {
        var that = this, i, j, query, index, last, formula, value, andOr, orderBy, ascending, isMatched = false;
        var field, fields = [], fLen;
        var comparison, comparisons;
        var operator, operators = ["==", "!=", ">", ">=", "<", "<=", "%%"], oLen = operators.length;
        var pattern = {
            'field' : /(.+?)(?:\s+(?:AND|OR)|$)/g,
            'comparison' : /(\s+(?:AND|OR))/g,
            'order' : /(\s+(?:ASC|DESC))/g
        };

        hx.log('dsrvEpg', '_createQuery() called');

        if (clause) {
            last = clause.indexOf('ORDER BY');
            if (last > -1) {
                orderBy = clause.substring(last, clause.length).replace(/ORDER BY/g, '');
                clause = clause.substring(0, last);
            }
            fields = clause.match(pattern.field);
            comparisons = clause.match(pattern.comparison);
        }

        for ( i = 0, fLen = fields.length; i < fLen; i += 1) {
            formula = fields[i].replace(pattern.comparison, '');
            isMatched = false;

            for ( j = 0; j < oLen; j += 1) {
                operator = operators[j];
                index = formula.indexOf(operator);

                if (index > -1) {
                    isMatched = true;
                    field = formula.substring(0, index).trim();
                    comparison = j;
                    value = formula.substring(index + operator.length, formula.length).trim();
                    break;
                }
            }

            if (isMatched) {
                if (!query) {
                    query = search.createQuery(field, comparison, value);
                } else {
                    andOr = comparisons[i - 1].toLowerCase().trim();
                    query = query[andOr](search.createQuery(field, comparison, value));
                }
            }
        }

        if (orderBy) {
            ascending = (orderBy.match(pattern.order)[0].toLowerCase().trim() === 'asc') ? true : false;
            search.orderBy(orderBy.substring(0, clause.indexOf(/*results[0]*/)).trim(), ascending);
        }

        return query;
    }
    _lazyExecute(param, programmes): void {
        var that = this, lastIndex, i;

        if (!param._isLive) {
            return;
        }

        hx.log('dsrvEpg', '_lazyExecute() called');

        hx.log("dsrvEpg-getLiveProgrammes", hx.util.format('+++++++ ####### onMetadataSearch, that._workingQueue.length : {0} ####### +++++++', that._workingQueue.length));

        for ( i = 0, length = that._workingQueue.length; i < length; i += 1) {
            if (param.channel[0].ccid === that._workingQueue[i].channel[0].ccid) {
                lastIndex = i;
                break;
            }
        }

        if (lastIndex) {
            that._workingQueue.splice(lastIndex, 1);

            //if (lastIndex === length - 1 && programmes[0].length) {
            //    that._cachedOnAirProgrammes = programmes;
            //    var name = that._cachedOnAirProgrammes[0][0].getName();
            //    hx.log("dsrvEpg-getLiveProgrammes", hx.util.format('+++++++ ####### onMetadataSearch, get CurrentProgrammes : {0} ####### +++++++', name));
            //    clearTimeout(that._autoEmptyCacheTimerID);
            //    that._autoEmptyCacheTimerID = setTimeout(function() {
            //        that._cachedOnAirProgrammes = [];
            //        clearTimeout(that._autoEmptyCacheTimerID);
            //        that._autoEmptyCacheTimerID = null;
            //        hx.log("dsrvEpg-getLiveProgrammes", hx.util.format('+++++++ ####### onMetadataSearch, auto Empty CurrentProgrammes : {0} ####### +++++++', name));
            //    }, 10000);
            //}
        }
    }
}
//hx.svc.EPG = new cDLSvcEpg();
//
export = cDLSvcEpg;
