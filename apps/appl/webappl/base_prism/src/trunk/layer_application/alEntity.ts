///<reference path="../def/require.d.ts" />
///<reference path="../def/sugar.d.ts" />
// file name : fw/common/entry/alEntity.js
/*jslint sloppy:true, nomen:true, vars:true */
/* ENTRY POINT */
declare var hx: ifSugar;
import __AL_COMPOSER__ = require("./alComposer");
module mAL {
    var parse = {
        url: /^([^\/?#:]*)(\/[\w\/]*)?(\?)?([\w]+)?(=)?([\w:.\/]+)?([\w=.,]+)?/,
        url_param: /^(,)?([\w]+)?(=)?([\w:.\/]+)?([\w=.,]+)?/,
        wrap: {
            _urlTable: {
                chr: ["%", "-"],
                code: ["__p__", "__h__"]
            },
            url: function (url) {
                url = encodeURIComponent(url);
                var a = this._urlTable, l = a.chr.length, n;
                for (n = 0; n < l; n += 1) {
                    url = url.replace(new RegExp(a.chr[n], 'gi'), a.code[n]);
                }
                return url;
            },
            code2url: function (str) {
                var a = this._urlTable, l = a.chr.length, n;
                for (n = 0; n < l; n += 1) {
                    str = str.replace(new RegExp(a.code[n], 'gi'), a.chr[n]);
                }
                return decodeURIComponent(str);
            }
        }
    };

    function parsePageUrl(url) {
        var ret = parse.url.exec(url);
        var data = {
            page: ret[1],
            path: ret[2],
            query: ret[4],
            param: ret[6],
            data: {}
        };

        function _addData(tag, value) {
            if (tag) {
                data.data[tag] = value;
            }
        }

        _addData(ret[4], ret[6]);
        var p = ret[7];
        var ar;
        while (p !== undefined) {
            ar = parse.url_param.exec(p);
            p = ar[5];
            _addData(ar[2], ar[4]);
        }
        return data;
    }

    class cALFactoryKernel {
        private _name: string = '';
//        private _loadCompleteList: any = {};
        _DB: any = {};

        constructor(aName?, aParam?) {
            this._name = aName;
            this._DB = aParam || {};
        }

        _makeObject(aKey, aConstructorOrModules?): any {
            return this.doMakeObject(aKey, aConstructorOrModules);
        }

        have(aKey) {
            return this._DB.hasOwnProperty(aKey);
        }

        create(aKey, aCallback) {
            var path = this._DB[aKey];
            if (!path) {
                hx.log("error", "Not defined key in page-factory");
                throw ("Undefined key in page-factory");
            }
            require([path], (aConstructor) => {
                if (!aConstructor) {
                    throw "page export error";
                }
                if (aCallback) {
                    aCallback(this._makeObject(aKey, aConstructor), aKey);
                }
            });
        }

        doMakeObject(aKey, aConstructor?): any {
            throw "do implement";
        }
    }
    class cALFactoryPage extends cALFactoryKernel {
        constructor(aPages: ifPAGE) {
            super('pages', aPages);
        }

        doMakeObject(aKey, aConstructor?) {
            var ret = new aConstructor(aKey);
            ret.setName(aKey);
            return ret;
        }
    }
    class cALFactoryComponent extends cALFactoryKernel {
        constructor(aComponents: ifOBJKeyString) {
            super('components', aComponents);
        }

        create(aKey, aCallback) {
            var path = aKey;
            if (!path) {
                hx.log("error", "Not defined key in page-factory");
                throw ("Undefined key in page-factory");
            }
            //if (!ENV.isModeRelease) {
            //    path += "?" + new Date().getTime();
            //    hx.log("info", path);
            //    hx.log("info", "ALFactoryKernel will add random digit to path for dynamic reload");
            //}
            ((aKeys:string[]) => {
                require([path], (aConstructor) => {
                    if (!aConstructor) {
                        throw "page export error";
                    }
                    if (aCallback) {
                        for (var i = 0, len = aKeys.length; i < len; i++) {
                            aCallback(this._makeObject(aKeys[i], aConstructor), aKeys[i]);
                        }
                    }
                });
            })(this._DB[aKey]);
        }

        doMakeObject(aKey, aModule?) {
            if (!aModule || !aModule[aKey]) {
                throw 'something wrong';
            }
            return aModule[aKey];
        }
    }
    class cHistory {
        _reg: any[] = [];

        _add(aData) {
            var stack = this._reg;
            stack.push({
                data: aData
            });
            return stack[stack.length - 1];
        }

        _pop() {
            return this._reg.pop();
        }

        add(aData) {
            return this._add(aData).data;
        }

        pop() {
            var data = this._pop();
            if (data) {
                return data.data;
            }
            return data;
        }

        addwithMark(aData, aMark) {
            var data = this._add(aData);
            data.mark = aMark;
            return data.data;
        }

        popwithMark(aMark) {
            var data = this._pop();
            while (data === undefined || data.mark !== aMark) {
                data = this._pop();
            }
            if (data) {
                return data.data;
            }
            return data;
        }
    }
    class cALPageManager {
        private parent: cALEntity = null;
        private _all_p = [];
        private _top_p = [];
        private _center_p = [];
        private _bottom_p = [];
        private _currentCenterPageName: string = "";
        private _focusedPage = ["", ""];
        private _onId;
        private _tsId;
        private _sId;
        private _chName;
        private __basePage__ = false;
        private _install_p;
        private _all_p_;
        //private _visibleIDSysKernel: number = 1;
        //private _visibleIDKernel: number;
        private _pageLayout: ifPageLayout;
        private _pageEntry: string;
        private _repo: cALRepository;
        private _queueCallbackCenterPagesCahnged: Function[] = [];

        constructor(aParent: cALEntity, aPageLayout: ifPageLayout, aPageEntry: string) {
            this.parent = aParent;
            this._currentCenterPageName = "";
            this._focusedPage = ["", ""];
            this._pageLayout = aPageLayout;
            this._pageEntry = aPageEntry;
            this._repo = aParent.getRepository();
            return this;
        }

        private setPageAll(aPages: any[]) {
            this._all_p = aPages;
            this.parent._on_alCheckPageVisible();
        }

        private _checkUsingKey(page, name, key) {
            var t = page._keyeventTable[name + 't'];
            if (t.indexOf(undefined) >= 0) {
                return true;
            }
            var n, l = t.length, ex = false;
            for (n = 0; n < l; n += 1) {
                ex = ex || !!t[n][key];
            }
            return ex;
        }

        private _destroyPages(a) {
            var r = this._repo;
            var l = a.length, n;
            for (n = 0; n < l; n += 1) {
                if (a[n].destroy) {
                    a[n].__created__ = undefined;
                    a[n].destroy();
                }
                decompose(a[n]);
                r.remove(a[n].name);
            }
        }
        _checkURL(pageUrl) {
            var parsedUrl = parsePageUrl(pageUrl);
            return parsedUrl.page;
        }
        private _createPageList(arr, aPageURLs: string[], cb, eachCb?) {
            var pageNames = [];
            var n;
            var len = aPageURLs.length;
            for (n = 0; n < len; n += 1) {
                pageNames[n] = parsePageUrl(aPageURLs[n]).page;
            }
            function _push(page, add_obj) {
                if (!page) {
                    throw "fix it";
                }
                arr[pageNames.indexOf(page.name)] = page;
                if (eachCb) {
                    eachCb(add_obj);
                }
                len -= 1;
                if (len === 0) {
                    cb();
                }
            }

            for (n = 0; n < aPageURLs.length; n += 1) {
                this._createPage(aPageURLs[n], _push);
            }
        }

        private _createCenterPageList(p) {
            var page = this._checkURL(p);
            var pages = [p];
            var childPages = this._pageLayout.pages_center[page];
            if (childPages !== undefined) {
                pages = childPages.concat(pages);
            }
            this._center_p.length = 0;
            this._createPageList(this._center_p, pages, () => {
                this._all_p.length = 0;
                if (this._center_p.length) {
                    this._center_p.unshift(this._center_p.pop());
                }
                this.setPageAll([].concat(this._bottom_p, this._center_p, this._top_p));
                if (hx.svc.SETTING_UTIL.getFirstTimeBoot()) {
                    var nameInstallPages = _.pluck(this._install_p, "name");
                    this._all_p.forEach((page) => {
                        var index = nameInstallPages.indexOf(page.name);
                        if (index !== -1) {
                            this._install_p[index] = page;
                        }
                    });
                }
                var queue = this._queueCallbackCenterPagesCahnged;
                while (queue.length) {
                    queue.shift()();
                }
            });
        }

        private _createBasePages(cb) {
            var processCnt = 0;

            function _callCB() {
                if (processCnt === 0 && cb !== undefined) {
                    cb();
                }
            }

            function _check() {
                processCnt -= 1;
                _callCB();
            }

            this._all_p.length = 0;
            if (this._pageLayout.pages_top) {
                processCnt += 1;
                this._createPageList(this._top_p, this._pageLayout.pages_top, _check);
            }
            if (this._pageLayout.pages_bottom) {
                processCnt += 1;
                this._createPageList(this._bottom_p, this._pageLayout.pages_bottom, _check);
            }
            _callCB();
        }

        private _createPage(aPageURL, aCallback) {
            var pageInfo = parsePageUrl(aPageURL);
            var name = pageInfo.page;
            var pageInstance = this._repo.get(name);
            var __create = (aPageInstance, aALEntity, aPageInfo, aCallback) => {
                var ret = aPageInstance.create(aALEntity, aPageInfo);
                aPageInstance.__created__ = true;
                if (aCallback) {
                    aCallback(aPageInstance);
                    if (ret && ret !== aPageInstance) {
                        throw 'handle it another way'
                    }
                }
            };
            if (!pageInstance) {
                this.parent.getFactory().create(name, (aPageInstance) => {
                    this._repo.add(name, aPageInstance);
                    __create(aPageInstance, this.parent, pageInfo, aCallback);
                    var pagesRelated = this._pageLayout.pages_center[name];
                    if (pagesRelated) {
                        aPageInstance.setRelatedPages(pagesRelated);
                    }
                });
            } else {
                __create(pageInstance, this.parent, pageInfo, aCallback);
            }
        }

        setFocusedHistory(pg) {
            var a = this._focusedPage, lastPg = a[0];
            if (a[1] === pg) {
                return lastPg;
            }
            a[0] = a[1];
            a[1] = pg;
            return lastPg;
        }

        getLastFocusedPage() {
            return this._focusedPage[1];
        }

        getPrevFocusedPage() {
            return this._focusedPage[0];
        }

        print() {
            var a = this._all_p, l = a.length, n;
            for (n = 0; n < l; n += 1) {
                hx.log("warning", "PAGE:" + a[n].name);
            }
        }

        check(page) {
            return this._all_p.indexOf(page) >= 0;
        }

        top() {
            var p = this._all_p;
            return p[p.length - 1];
        }

        under(page, name, key) {
            var _recall = (next) => {
                if (next === undefined) {
                    return;
                }
                if (this.chkDisable(next, name, key)) {
                    next = this.under(next, name, key);
                }
                return next;
            };
            var a = this._all_p, n = a.indexOf(page);
            if (!n) {
                return;
            }
            return _recall(a[n - 1]);
        }

        chkDisable(page, name, key) {
            var ret = false;
            if (page._keyeventTable) {
                ret = ((!!page.isActive && !page.isActive()) || (!page._keyeventTable[name]) || !this._checkUsingKey(page, name, key));
            }
            return ret;
        }

        trigger(event, data) {
            var inst = this._repo.get(data.page);
            if (!inst) {
                hx.log("warning", data.page + " is not ready yet");
                return;
            }
            if (inst.__created__) {
                data.alEvent = event;
                inst.byEntity(data);
            //} else {
            //    console.log("%c message skipped %s %s %o", "font-size: 40px; color: yellow;", data.page, event, data);
            //    throw "call bglee";
            }
        }

        triggerCenter(event, aIsVisible) {
            var centerPages = this._center_p;
            var param = {
                visible: aIsVisible,
                alEvent: event
            };
            if (centerPages && centerPages.length) {
                if (!centerPages[0] || !centerPages[0].byEntity) {
                    this._queueCallbackCenterPagesCahnged.push(() => {
                        centerPages[0].byEntity(param);
                    });
                } else {
                    centerPages[0].byEntity(param);
                }
            }
        }
        change(page) {
            if (!page) {
                throw 'page undefined';
            }
            var prevCenterPageName = this._currentCenterPageName;
            var _destoryPages = () => {
                var a = this._center_p;
                var n;
                if (a[0].name !== "CPgStart" && a[0].name !== "CPgLiveController") {
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE": "OSDOUT",
                            "TRS": "I",
                            "OSD": a[0].name
                        });
                    }
                }
                for (n = 0; n < a.length; n += 1) {
                    if (a[n].destroy) {
                        a[n].__created__ = undefined;
                        a[n].destroy();
                    }
                    decompose(a[n]);
                }
            };
            var _checkChangeToSamePage = () => {
                var data = this._center_p[0];
                if (!data) {
                    return false;
                }
                //TODO : need to check parameter in the page name
                var p = parsePageUrl(page);
                var ret = data.name === p.page;
                if (ret) {
                    if ((<any>p).data.refresh) {
                        ret = false;
                    }
                }
                return ret;
            };
            var _createPages = () => {
                if (_checkChangeToSamePage()) {
                    this._currentCenterPageName = prevCenterPageName;
                    return;
                }
                _destoryPages();
                this._createCenterPageList(page);
                if (this._currentCenterPageName !== "CPgLiveController") {
                    if (hx.svc.STATISTICS) {
                        hx.svc.STATISTICS.statisticsLog({
                            "CODE": "OSDIN",
                            "TRS": "I",
                            "OSD": this._currentCenterPageName
                        });
                    }
                } else {
                    var curChannel = hx.svc.CH_LIST.getCurrentChannel().channel;
                    if (curChannel) {
                        this._onId = curChannel.onid;
                        this._tsId = curChannel.tsid;
                        this._sId = curChannel.sid;
                        this._chName = curChannel.name;

                        hx.svc.EPG.getProgrammes({
                            isOnAir: true,
                            filter: "P/F",
                            channel: curChannel,
                            listener: this,
                            notUI: true,
                            cb: (result) => {
                                if (result[0] && result[0][0]) {
                                    var programmeID = result[0][0].programmeID, tmp, eventID;
                                    var evtName = result[0][0].name;
                                    if (programmeID) {
                                        tmp = programmeID.split(';');
                                        if (tmp.length > 1) {
                                            eventID = parseInt(tmp[1], 16).toString(10);
                                        }
                                    }
                                    if (hx.svc.STATISTICS) {
                                        hx.svc.STATISTICS.statisticsLog({
                                            "CODE": "ELWS", //EVENT_LIVE_WATCH_START
                                            "TRS": "R",
                                            "ONID": this._onId,
                                            "TSID": this._tsId,
                                            "SID": this._sId,
                                            "EID": eventID,
                                            "CHNAME" : this._chName,
                                            "EVTNAME" : evtName
                                        });
                                    }
                                }
                                result = undefined;
                            }
                        });
                    }
                }
            };
            this._currentCenterPageName = this._checkURL(page);
            if (!this.__basePage__) {
                this._destroyPages(this._all_p);
                this._createBasePages(_createPages);
                this.__basePage__ = true;
            } else {
                _createPages();
            }
            var centerPage = this._repo.get(this.getCurrentCenterPageName());
            if (centerPage) {
                hx.svc.SETTING_UTIL.doingKeyGrab(centerPage.visible());
            }
        }

        entry() {
            var ep = this._pageEntry;
            this._currentCenterPageName = this._checkURL(ep);
            this._createPage(ep, (inst) => {
                this._center_p.push(inst);
                this.setPageAll([].concat(this._center_p));
            });
        }

        installPage(pageUrl) {
            this._currentCenterPageName = this._checkURL(pageUrl);
            var _createBasePage = (cb) => {
                if (this._install_p !== undefined) {
                    cb();
                    return;
                }
                this._install_p = [];
                this._createPageList(this._install_p, this._pageLayout.pages_install_wizard, cb);
            };
            this._destroyPages(this._center_p);
            _createBasePage(() => {
                var pages = [];
                if (!$.isArray(pageUrl)) {
                    pageUrl = [pageUrl];
                }
                this._createPageList(pages, pageUrl, () => {
                    this._center_p = pages;
                    this.setPageAll([].concat(this._center_p, this._install_p));
                });
            });
        }

        moveTo(page) {
            var a = this._all_p;
            var n = a.indexOf(page);
            if (n < 0) {
                return;
            }
            this.setPageAll([].concat(a.slice(0, n), a.slice(n + 1, a.length), a[n]));
        }

        resetOrder() {
            if (this._bottom_p.length === 0 && this._top_p.length === 0 && this._install_p.length > 0) {
                this._all_p_ = [].concat(this._center_p, this._install_p);
            } else {
                this.setPageAll([].concat(this._bottom_p, this._center_p, this._top_p));
            }
        }

        visiblePageCount() {
            var pagesAll = this._all_p, l = pagesAll.length;
            if (pagesAll === undefined || l < 1) {
                //hx.log("warning", "[alEntity]a : " + a + ", l : " + l);
                return false;
            }
            var ret = 0, i;
            for (i = 0; i < l; i += 1) {
                if (pagesAll[i].visible()) {
                    ret += 1;
                }
            }
            return <any>ret;
        }

        getCurrentCenterPageName(): string {
            return this._currentCenterPageName;
        }
    }
    export class cALFactory {
        private _page: cALFactoryPage;
        private _component: cALFactoryComponent;

        constructor(aPages: ifPAGE, aComponents: ifOBJKeyString) {
            this._page = new cALFactoryPage(aPages);
            this._component = new cALFactoryComponent(aComponents);
        }

        create(aKey, aCallback) {
            if (this._component.have(aKey)) {
                this._component.create(aKey, aCallback);
            } else if (this._page.have(aKey)) {
                this._page.create(aKey, aCallback);
            }
        }
    }
    export class cALRepository {
        private _repos: any = {};

        get(aName: string) {
            return this._repos[aName];
        }

        add(aName, aObject) {
            return (this._repos[aName] = aObject);
        }

        remove(aName) {
            delete this._repos[aName];
        }

        list() {
            return this._repos;
        }
    }
    export class cALEntity {
        private _repos: cALRepository;
        private _factory: cALFactory;
        private name: string = "alEntity";
        private _pageMgr: cALPageManager;
        private _history: cHistory;
        private _preProcess;
        private $this: JQuery;
        private $composer: JQuery;

        constructor(aPages: ifPAGE, aPageLayout: ifPageLayout, aPageEntry: string, aComponents: ifOBJKeyString, aEvents: any) {
            hx.il.sequence([
                (aNext) => {
                    this._repos = new cALRepository();
                    this._factory = new cALFactory(aPages, aComponents);
                    this.$this = $(this);
                    hx.svc.KEY_EVENT_BINDER.bind(document, this);
                    hx.log("alEntity", "Initialize Language");
                    //Load Language for Direct service.
                    hx.svc.SETTING_UTIL.InitializeLanguage(aNext);
                },
                (aNext) => {
                    _composer = new __AL_COMPOSER__.cALComposer(this, aComponents, aEvents, () => {
                        aNext();
                    });
                    this.$composer = $(_composer);
                },
                () => {
                    this._pageMgr.entry();
                }
            ]);
            this.name = "alEntity";
            this._pageMgr = new cALPageManager(this, aPageLayout, aPageEntry);
            this._history = new cHistory();
        }

        destroy() {
        }

        //FROM PRESENTATION LAYER
        _on_alMessageToPage(param) {
            if (param === undefined || param.target === undefined) {
                throw "It's error";
            }
            var a = param.target, p, n, l;
            if (!$.isArray(a)) {
                a = [a];
            }
            for (n = 0, l = a.length; n < l; n += 1) {
                p = parsePageUrl(a[n]);
                p.alStarter = param.alStarter;
                this._pageMgr.trigger('alMessageToPage', p);
            }
        }

        _on_alChangePage(param) {
            this._pageMgr.change(param.target);
        }

        _on_alChangeInstallPage(param) {
            this._pageMgr.installPage(param.target);
        }

        _on_alCheckPageVisible() {
            var visiblePageCount = this._pageMgr.visiblePageCount();
            this._pageMgr.triggerCenter('alPagesVisible', visiblePageCount > 1);
        }
        _on_alReFocus(param) {
            if (param.target === undefined) {
                param.target = [param.prev ? this._pageMgr.getPrevFocusedPage() : this._pageMgr.getLastFocusedPage()];
            }
            var r = this._repos, a = param.target, l = a.length, n, p;
            for (n = 0; n < l; n += 1) {
                p = r.get(a[n]);
                if (p && p.getLastFocusedElement && this._pageMgr.check(p)) {
                    var obj = p.getLastFocusedElement();
                    if (obj && obj.on) {
                        obj.on();
                    }
                }
            }
        }
        getCenterPageName(): string {
            return this._pageMgr.getCurrentCenterPageName();
        }
        getPageVisible(name) {
            var page = this._repos.get(name);
            if (!page) {
                hx.log('warning', name + '-page not loaded');
                return false;
            }
            if (!page.visible) {
                hx.log('warning', name + '.visible function not defined');
                return false;
            }
            return page.visible();
        }

        getRepository(): cALRepository {
            return this._repos;
        }

        getFactory(): cALFactory {
            return this._factory;
        }

        onKey(aKeyCode: number, aRepeatCount: number, aEventType: string) {
            console.log("[AL] onKey(" + aKeyCode + ")");
            var handled = false;
            var alType = 'alKey';
            var alKey = aKeyCode;
            //var DEV_setDebugKeySequence = (function() {
            //    var elKeyHelper: HTMLDivElement = <HTMLDivElement>document.getElementById("key-helper-sequence");
            //    if (!elKeyHelper) {
            //        elKeyHelper = document.createElement('div');
            //        var style = elKeyHelper.style;
            //        elKeyHelper.setAttribute("id", "key-helper-sequence");
            //        style.position = "absolute";
            //        style.left = "100px";
            //        style.top = "600px";
            //        style.padding = "15px";
            //        style.borderRadius = "8px";
            //        style.fontSize = "18px";
            //        style.backgroundColor = "rgba(0,0,0,.4)";
            //        style.width = "1080px";
            //        style.textAlign = "center";
            //        style.zIndex = "1000";
            //        style.display = "block";
            //    }
            //    document.body.appendChild(elKeyHelper);
            //    (<any>window).d = (<any>window).d || {};
            //    var key = (<any>window).d.key = [];
            //    return function(aPage: string) {
            //        if (aEventType === "keyup") {
            //            return;
            //        }
            //        key.push(aPage);
            //        elKeyHelper.innerText = key.join(" => ");
            //        (<any>window).d.key = key;
            //    }
            //})();
            if (aEventType === 'keyup') {
                alType = 'alEndOfRepeatKey';
            }
            if (this._preProcess) {
                this._preProcess();
                this._preProcess = undefined;
            }
            var mgr = this._pageMgr;
            var page = mgr.top();
            if (page) {
                if (mgr.chkDisable(page, alType, alKey)) {
                    page = mgr.under(page, alType, alKey);
                    if (!page) {
                        return;
                    }
                }
                handled = false;
                do {
                    if (page === mgr) {
                        throw "something wrong have to fix it";
                    }
                    //DEV_setDebugKeySequence(page.name);
                    handled = _composer.doCommand({
                        alType: alType,
                        alEvent: alType,
                        alKey: alKey,
                        alStarter: 'alEntity',
                        alRepeat: aRepeatCount,
                        alPage: page
                    }) || false;
                    page = this._pageMgr.under(page, alType, alKey);
                } while (page && !handled);
            }
        }

        byComposer(aParam) {
            var fx = this['_on_' + aParam.alEvent];
            if (fx) {
                fx.call(this, aParam);
            }
        }
        pageFocused(page) {
            var pgMgr = this._pageMgr, pagename = pgMgr._checkURL(page.name);
            if (pagename !== pgMgr.setFocusedHistory(pagename)) {
                var r = this._repos, p = r.get(pagename);
                if (p && p.getLastFocusedElement && this._pageMgr.check(p)) {
                    var obj = p.getLastFocusedElement();
                    if (obj && obj.hover) {
                        obj.hover();
                    }
                }
            }
            if (page.TOPMOST) {
                this._preProcess = function () {
                    pgMgr.resetOrder();
                    pgMgr.moveTo(page);
                };
            }
        }
    }
    export var entity: cALEntity;
    var _composer: __AL_COMPOSER__.cALComposer;

    export function component(aComponent: string, aParam?: any) {
        return _composer.createComponent(aComponent, aParam);
    }
    export function decomponent(aComponent: any) {
        _composer.destroyComponent(aComponent);
    }
    export function compose(aPage: any) {
        return _composer.addPage(aPage);
    }
    export function decompose(aPage: any) {
        _composer.removePage(aPage);
    }
}
export = mAL;