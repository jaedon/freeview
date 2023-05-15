var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./alComposer"], function (require, exports, __AL_COMPOSER__) {
    ///<reference path="../def/require.d.ts" />
    ///<reference path="../def/sugar.d.ts" />
    var mAL;
    (function (mAL) {
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
        var cALFactoryKernel = (function () {
            function cALFactoryKernel(aName, aParam) {
                this._name = '';
                //        private _loadCompleteList: any = {};
                this._DB = {};
                this._name = aName;
                this._DB = aParam || {};
            }
            cALFactoryKernel.prototype._makeObject = function (aKey, aConstructorOrModules) {
                return this.doMakeObject(aKey, aConstructorOrModules);
            };
            cALFactoryKernel.prototype.have = function (aKey) {
                return this._DB.hasOwnProperty(aKey);
            };
            cALFactoryKernel.prototype.create = function (aKey, aCallback) {
                var _this = this;
                var path = this._DB[aKey];
                if (!path) {
                    hx.log("error", "Not defined key in page-factory");
                    throw ("Undefined key in page-factory");
                }
                require([path], function (aConstructor) {
                    if (!aConstructor) {
                        throw "page export error";
                    }
                    if (aCallback) {
                        aCallback(_this._makeObject(aKey, aConstructor), aKey);
                    }
                });
            };
            cALFactoryKernel.prototype.doMakeObject = function (aKey, aConstructor) {
                throw "do implement";
            };
            return cALFactoryKernel;
        })();
        var cALFactoryPage = (function (_super) {
            __extends(cALFactoryPage, _super);
            function cALFactoryPage(aPages) {
                _super.call(this, 'pages', aPages);
            }
            cALFactoryPage.prototype.doMakeObject = function (aKey, aConstructor) {
                var ret = new aConstructor(aKey);
                ret.setName(aKey);
                return ret;
            };
            return cALFactoryPage;
        })(cALFactoryKernel);
        var cALFactoryComponent = (function (_super) {
            __extends(cALFactoryComponent, _super);
            function cALFactoryComponent(aComponents) {
                _super.call(this, 'components', aComponents);
            }
            cALFactoryComponent.prototype.create = function (aKey, aCallback) {
                var _this = this;
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
                (function (aKeys) {
                    require([path], function (aConstructor) {
                        if (!aConstructor) {
                            throw "page export error";
                        }
                        if (aCallback) {
                            for (var i = 0, len = aKeys.length; i < len; i++) {
                                aCallback(_this._makeObject(aKeys[i], aConstructor), aKeys[i]);
                            }
                        }
                    });
                })(this._DB[aKey]);
            };
            cALFactoryComponent.prototype.doMakeObject = function (aKey, aModule) {
                if (!aModule || !aModule[aKey]) {
                    throw 'something wrong';
                }
                return aModule[aKey];
            };
            return cALFactoryComponent;
        })(cALFactoryKernel);
        var cHistory = (function () {
            function cHistory() {
                this._reg = [];
            }
            cHistory.prototype._add = function (aData) {
                var stack = this._reg;
                stack.push({
                    data: aData
                });
                return stack[stack.length - 1];
            };
            cHistory.prototype._pop = function () {
                return this._reg.pop();
            };
            cHistory.prototype.add = function (aData) {
                return this._add(aData).data;
            };
            cHistory.prototype.pop = function () {
                var data = this._pop();
                if (data) {
                    return data.data;
                }
                return data;
            };
            cHistory.prototype.addwithMark = function (aData, aMark) {
                var data = this._add(aData);
                data.mark = aMark;
                return data.data;
            };
            cHistory.prototype.popwithMark = function (aMark) {
                var data = this._pop();
                while (data === undefined || data.mark !== aMark) {
                    data = this._pop();
                }
                if (data) {
                    return data.data;
                }
                return data;
            };
            return cHistory;
        })();
        var cALPageManager = (function () {
            function cALPageManager(aParent, aPageLayout, aPageEntry) {
                this.parent = null;
                this._all_p = [];
                this._top_p = [];
                this._center_p = [];
                this._bottom_p = [];
                this._currentCenterPageName = "";
                this._focusedPage = ["", ""];
                this.__basePage__ = false;
                this._queueCallbackCenterPagesCahnged = [];
                this.parent = aParent;
                this._currentCenterPageName = "";
                this._focusedPage = ["", ""];
                this._pageLayout = aPageLayout;
                this._pageEntry = aPageEntry;
                this._repo = aParent.getRepository();
                return this;
            }
            cALPageManager.prototype.setPageAll = function (aPages) {
                this._all_p = aPages;
                this.parent._on_alCheckPageVisible();
            };
            cALPageManager.prototype._checkUsingKey = function (page, name, key) {
                var t = page._keyeventTable[name + 't'];
                if (t.indexOf(undefined) >= 0) {
                    return true;
                }
                var n, l = t.length, ex = false;
                for (n = 0; n < l; n += 1) {
                    ex = ex || !!t[n][key];
                }
                return ex;
            };
            cALPageManager.prototype._destroyPages = function (a) {
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
            };
            cALPageManager.prototype._checkURL = function (pageUrl) {
                var parsedUrl = parsePageUrl(pageUrl);
                return parsedUrl.page;
            };
            cALPageManager.prototype._createPageList = function (arr, aPageURLs, cb, eachCb) {
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
            };
            cALPageManager.prototype._createCenterPageList = function (p) {
                var _this = this;
                var page = this._checkURL(p);
                var pages = [p];
                var childPages = this._pageLayout.pages_center[page];
                if (childPages !== undefined) {
                    pages = childPages.concat(pages);
                }
                this._center_p.length = 0;
                this._createPageList(this._center_p, pages, function () {
                    _this._all_p.length = 0;
                    if (_this._center_p.length) {
                        _this._center_p.unshift(_this._center_p.pop());
                    }
                    _this.setPageAll([].concat(_this._bottom_p, _this._center_p, _this._top_p));
                    if (hx.svc.SETTING_UTIL.getFirstTimeBoot()) {
                        var nameInstallPages = _.pluck(_this._install_p, "name");
                        _this._all_p.forEach(function (page) {
                            var index = nameInstallPages.indexOf(page.name);
                            if (index !== -1) {
                                _this._install_p[index] = page;
                            }
                        });
                    }
                    var queue = _this._queueCallbackCenterPagesCahnged;
                    while (queue.length) {
                        queue.shift()();
                    }
                });
            };
            cALPageManager.prototype._createBasePages = function (cb) {
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
            };
            cALPageManager.prototype._createPage = function (aPageURL, aCallback) {
                var _this = this;
                var pageInfo = parsePageUrl(aPageURL);
                var name = pageInfo.page;
                var pageInstance = this._repo.get(name);
                var __create = function (aPageInstance, aALEntity, aPageInfo, aCallback) {
                    var ret = aPageInstance.create(aALEntity, aPageInfo);
                    aPageInstance.__created__ = true;
                    if (aCallback) {
                        aCallback(aPageInstance);
                        if (ret && ret !== aPageInstance) {
                            throw 'handle it another way';
                        }
                    }
                };
                if (!pageInstance) {
                    this.parent.getFactory().create(name, function (aPageInstance) {
                        _this._repo.add(name, aPageInstance);
                        __create(aPageInstance, _this.parent, pageInfo, aCallback);
                        var pagesRelated = _this._pageLayout.pages_center[name];
                        if (pagesRelated) {
                            aPageInstance.setRelatedPages(pagesRelated);
                        }
                    });
                }
                else {
                    __create(pageInstance, this.parent, pageInfo, aCallback);
                }
            };
            cALPageManager.prototype.setFocusedHistory = function (pg) {
                var a = this._focusedPage, lastPg = a[0];
                if (a[1] === pg) {
                    return lastPg;
                }
                a[0] = a[1];
                a[1] = pg;
                return lastPg;
            };
            cALPageManager.prototype.getLastFocusedPage = function () {
                return this._focusedPage[1];
            };
            cALPageManager.prototype.getPrevFocusedPage = function () {
                return this._focusedPage[0];
            };
            cALPageManager.prototype.print = function () {
                var a = this._all_p, l = a.length, n;
                for (n = 0; n < l; n += 1) {
                    hx.log("warning", "PAGE:" + a[n].name);
                }
            };
            cALPageManager.prototype.check = function (page) {
                return this._all_p.indexOf(page) >= 0;
            };
            cALPageManager.prototype.top = function () {
                var p = this._all_p;
                return p[p.length - 1];
            };
            cALPageManager.prototype.under = function (page, name, key) {
                var _this = this;
                var _recall = function (next) {
                    if (next === undefined) {
                        return;
                    }
                    if (_this.chkDisable(next, name, key)) {
                        next = _this.under(next, name, key);
                    }
                    return next;
                };
                var a = this._all_p, n = a.indexOf(page);
                if (!n) {
                    return;
                }
                return _recall(a[n - 1]);
            };
            cALPageManager.prototype.chkDisable = function (page, name, key) {
                var ret = false;
                if (page._keyeventTable) {
                    ret = ((!!page.isActive && !page.isActive()) || (!page._keyeventTable[name]) || !this._checkUsingKey(page, name, key));
                }
                return ret;
            };
            cALPageManager.prototype.trigger = function (event, data) {
                var inst = this._repo.get(data.page);
                if (!inst) {
                    hx.log("warning", data.page + " is not ready yet");
                    return;
                }
                if (inst.__created__) {
                    data.alEvent = event;
                    inst.byEntity(data);
                }
            };
            cALPageManager.prototype.triggerCenter = function (event, aIsVisible) {
                var centerPages = this._center_p;
                var param = {
                    visible: aIsVisible,
                    alEvent: event
                };
                if (centerPages && centerPages.length) {
                    if (!centerPages[0] || !centerPages[0].byEntity) {
                        this._queueCallbackCenterPagesCahnged.push(function () {
                            centerPages[0].byEntity(param);
                        });
                    }
                    else {
                        centerPages[0].byEntity(param);
                    }
                }
            };
            cALPageManager.prototype.change = function (page) {
                var _this = this;
                if (!page) {
                    throw 'page undefined';
                }
                var prevCenterPageName = this._currentCenterPageName;
                var _destoryPages = function () {
                    var a = _this._center_p;
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
                var _checkChangeToSamePage = function () {
                    var data = _this._center_p[0];
                    if (!data) {
                        return false;
                    }
                    //TODO : need to check parameter in the page name
                    var p = parsePageUrl(page);
                    var ret = data.name === p.page;
                    if (ret) {
                        if (p.data.refresh) {
                            ret = false;
                        }
                    }
                    return ret;
                };
                var _createPages = function () {
                    if (_checkChangeToSamePage()) {
                        _this._currentCenterPageName = prevCenterPageName;
                        return;
                    }
                    _destoryPages();
                    _this._createCenterPageList(page);
                    if (_this._currentCenterPageName !== "CPgLiveController") {
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE": "OSDIN",
                                "TRS": "I",
                                "OSD": _this._currentCenterPageName
                            });
                        }
                    }
                    else {
                        var curChannel = hx.svc.CH_LIST.getCurrentChannel().channel;
                        if (curChannel) {
                            _this._onId = curChannel.onid;
                            _this._tsId = curChannel.tsid;
                            _this._sId = curChannel.sid;
                            _this._chName = curChannel.name;
                            hx.svc.EPG.getProgrammes({
                                isOnAir: true,
                                filter: "P/F",
                                channel: curChannel,
                                listener: _this,
                                notUI: true,
                                cb: function (result) {
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
                                                "CODE": "ELWS",
                                                "TRS": "R",
                                                "ONID": _this._onId,
                                                "TSID": _this._tsId,
                                                "SID": _this._sId,
                                                "EID": eventID,
                                                "CHNAME": _this._chName,
                                                "EVTNAME": evtName
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
                }
                else {
                    _createPages();
                }
                var centerPage = this._repo.get(this.getCurrentCenterPageName());
                if (centerPage) {
                    hx.svc.SETTING_UTIL.doingKeyGrab(centerPage.visible());
                }
            };
            cALPageManager.prototype.entry = function () {
                var _this = this;
                var ep = this._pageEntry;
                this._currentCenterPageName = this._checkURL(ep);
                this._createPage(ep, function (inst) {
                    _this._center_p.push(inst);
                    _this.setPageAll([].concat(_this._center_p));
                });
            };
            cALPageManager.prototype.installPage = function (pageUrl) {
                var _this = this;
                this._currentCenterPageName = this._checkURL(pageUrl);
                var _createBasePage = function (cb) {
                    if (_this._install_p !== undefined) {
                        cb();
                        return;
                    }
                    _this._install_p = [];
                    _this._createPageList(_this._install_p, _this._pageLayout.pages_install_wizard, cb);
                };
                this._destroyPages(this._center_p);
                _createBasePage(function () {
                    var pages = [];
                    if (!$.isArray(pageUrl)) {
                        pageUrl = [pageUrl];
                    }
                    _this._createPageList(pages, pageUrl, function () {
                        _this._center_p = pages;
                        _this.setPageAll([].concat(_this._center_p, _this._install_p));
                    });
                });
            };
            cALPageManager.prototype.moveTo = function (page) {
                var a = this._all_p;
                var n = a.indexOf(page);
                if (n < 0) {
                    return;
                }
                this.setPageAll([].concat(a.slice(0, n), a.slice(n + 1, a.length), a[n]));
            };
            cALPageManager.prototype.resetOrder = function () {
                if (this._bottom_p.length === 0 && this._top_p.length === 0 && this._install_p.length > 0) {
                    this._all_p_ = [].concat(this._center_p, this._install_p);
                }
                else {
                    this.setPageAll([].concat(this._bottom_p, this._center_p, this._top_p));
                }
            };
            cALPageManager.prototype.visiblePageCount = function () {
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
                return ret;
            };
            cALPageManager.prototype.getCurrentCenterPageName = function () {
                return this._currentCenterPageName;
            };
            return cALPageManager;
        })();
        var cALFactory = (function () {
            function cALFactory(aPages, aComponents) {
                this._page = new cALFactoryPage(aPages);
                this._component = new cALFactoryComponent(aComponents);
            }
            cALFactory.prototype.create = function (aKey, aCallback) {
                if (this._component.have(aKey)) {
                    this._component.create(aKey, aCallback);
                }
                else if (this._page.have(aKey)) {
                    this._page.create(aKey, aCallback);
                }
            };
            return cALFactory;
        })();
        mAL.cALFactory = cALFactory;
        var cALRepository = (function () {
            function cALRepository() {
                this._repos = {};
            }
            cALRepository.prototype.get = function (aName) {
                return this._repos[aName];
            };
            cALRepository.prototype.add = function (aName, aObject) {
                return (this._repos[aName] = aObject);
            };
            cALRepository.prototype.remove = function (aName) {
                delete this._repos[aName];
            };
            cALRepository.prototype.list = function () {
                return this._repos;
            };
            return cALRepository;
        })();
        mAL.cALRepository = cALRepository;
        var cALEntity = (function () {
            function cALEntity(aPages, aPageLayout, aPageEntry, aComponents, aEvents) {
                var _this = this;
                this.name = "alEntity";
                hx.il.sequence([
                    function (aNext) {
                        _this._repos = new cALRepository();
                        _this._factory = new cALFactory(aPages, aComponents);
                        _this.$this = $(_this);
                        hx.svc.KEY_EVENT_BINDER.bind(document, _this);
                        hx.log("alEntity", "Initialize Language");
                        //Load Language for Direct service.
                        hx.svc.SETTING_UTIL.InitializeLanguage(aNext);
                    },
                    function (aNext) {
                        _composer = new __AL_COMPOSER__.cALComposer(_this, aComponents, aEvents, function () {
                            aNext();
                        });
                        _this.$composer = $(_composer);
                    },
                    function () {
                        _this._pageMgr.entry();
                    }
                ]);
                this.name = "alEntity";
                this._pageMgr = new cALPageManager(this, aPageLayout, aPageEntry);
                this._history = new cHistory();
            }
            cALEntity.prototype.destroy = function () {
            };
            //FROM PRESENTATION LAYER
            cALEntity.prototype._on_alMessageToPage = function (param) {
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
            };
            cALEntity.prototype._on_alChangePage = function (param) {
                this._pageMgr.change(param.target);
            };
            cALEntity.prototype._on_alChangeInstallPage = function (param) {
                this._pageMgr.installPage(param.target);
            };
            cALEntity.prototype._on_alCheckPageVisible = function () {
                var visiblePageCount = this._pageMgr.visiblePageCount();
                this._pageMgr.triggerCenter('alPagesVisible', visiblePageCount > 1);
            };
            cALEntity.prototype._on_alReFocus = function (param) {
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
            };
            cALEntity.prototype.getCenterPageName = function () {
                return this._pageMgr.getCurrentCenterPageName();
            };
            cALEntity.prototype.getPageVisible = function (name) {
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
            };
            cALEntity.prototype.getRepository = function () {
                return this._repos;
            };
            cALEntity.prototype.getFactory = function () {
                return this._factory;
            };
            cALEntity.prototype.onKey = function (aKeyCode, aRepeatCount, aEventType) {
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
            };
            cALEntity.prototype.byComposer = function (aParam) {
                var fx = this['_on_' + aParam.alEvent];
                if (fx) {
                    fx.call(this, aParam);
                }
            };
            cALEntity.prototype.pageFocused = function (page) {
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
            };
            return cALEntity;
        })();
        mAL.cALEntity = cALEntity;
        mAL.entity;
        var _composer;
        function component(aComponent, aParam) {
            return _composer.createComponent(aComponent, aParam);
        }
        mAL.component = component;
        function decomponent(aComponent) {
            _composer.destroyComponent(aComponent);
        }
        mAL.decomponent = decomponent;
        function compose(aPage) {
            return _composer.addPage(aPage);
        }
        mAL.compose = compose;
        function decompose(aPage) {
            _composer.removePage(aPage);
        }
        mAL.decompose = decompose;
    })(mAL || (mAL = {}));
    return mAL;
});
