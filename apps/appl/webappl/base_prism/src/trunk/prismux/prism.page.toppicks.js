/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'model/model', 'pages/util/controls', './prism.page'], function (require, exports, __MODEL__, __CONTROLS__, __PRISM_CPAGE__) {
    var mPrism;
    (function (mPrism) {
        var ETopPicksCtrl;
        (function (ETopPicksCtrl) {
            ETopPicksCtrl[ETopPicksCtrl["eON_NOW"] = 0] = "eON_NOW";
            ETopPicksCtrl[ETopPicksCtrl["eNEXT"] = 1] = "eNEXT";
            ETopPicksCtrl[ETopPicksCtrl["eRECORDINGS"] = 2] = "eRECORDINGS";
            ETopPicksCtrl[ETopPicksCtrl["eON_DEMAND"] = 3] = "eON_DEMAND";
            ETopPicksCtrl[ETopPicksCtrl["eMAX"] = 4] = "eMAX";
        })(ETopPicksCtrl || (ETopPicksCtrl = {}));
        var TTopPicksIndexByPath = {
            '/onnow': 0 /* eON_NOW */,
            '/next': 1 /* eNEXT */,
            '/recordings': 2 /* eRECORDINGS */,
            '/ondemand': 3 /* eON_DEMAND */
        };
        var ERecordings;
        (function (ERecordings) {
            ERecordings[ERecordings["eRECENTLY"] = 0] = "eRECENTLY";
            ERecordings[ERecordings["eCONTINUE"] = 1] = "eCONTINUE";
            ERecordings[ERecordings["eMAX"] = 2] = "eMAX";
        })(ERecordings || (ERecordings = {}));
        var ENext;
        (function (ENext) {
            ENext[ENext["eDONTMISSIT"] = 0] = "eDONTMISSIT";
            ENext[ENext["ePOPULARTVSERIES"] = 1] = "ePOPULARTVSERIES";
            ENext[ENext["eMAX"] = 2] = "eMAX";
        })(ENext || (ENext = {}));
        var CTopPicksCtrl = (function (_super) {
            __extends(CTopPicksCtrl, _super);
            function CTopPicksCtrl(parent, dataAPI, $el) {
                var that = this;
                _super.call(this);
                that._parent = parent;
                that._dataAPI = dataAPI;
                that.NORMAL_THUMBNAIL_WIDTH = 267;
                that.NORMAL_CHANNEL_LOGO_WIDTH = 60;
            }
            CTopPicksCtrl.prototype.destroy = function () {
                var that = this;
                if (that._categoryCtrl) {
                    that._categoryCtrl.destroy();
                    delete that._categoryCtrl;
                    that._categoryCtrl = undefined;
                }
                if (that._contentsCtrl) {
                    that._contentsCtrl.destroy();
                    delete that._contentsCtrl;
                    that._contentsCtrl = undefined;
                }
                if (that._contentsView) {
                    that._contentsView.destroy();
                    delete that._contentsView;
                    that._contentsView = undefined;
                }
                if (that._dataAPI) {
                    that._dataAPI = undefined;
                }
                if (that._$noData) {
                    that._$noData.remove();
                    that._$noData = null;
                }
                _super.prototype.destroy.call(that);
            };
            CTopPicksCtrl.prototype.init = function ($el) {
                var that = this;
                var ctrls;
                _super.prototype.init.call(this, $el);
                that.setId('toppicksCtrl');
                ctrls = that.makeChildCtrls();
                if (ctrls) {
                    that.setChildControl(ctrls);
                }
                if (that._contentsView) {
                    that._contentsView.$self().append($('<div>', {
                        'class': 'title' // for no data
                    }));
                    that._$noData = that._contentsView.$self().children('.title');
                }
            };
            CTopPicksCtrl.prototype.makeChildCtrls = function () {
                throw "this function must override!!!";
            };
            CTopPicksCtrl.prototype.getContentsCtrl = function () {
                return this._contentsCtrl;
            };
            CTopPicksCtrl.prototype.getContentsView = function () {
                return this._contentsView;
            };
            CTopPicksCtrl.prototype.doDraw = function (param) {
                var that = this;
                if (!that.getFocusedChild()) {
                    // initialising
                    that.processAsyncData();
                }
                return _super.prototype.doDraw.call(that, param);
            };
            CTopPicksCtrl.prototype.doKeyOk = function () {
                var that = this;
                var consumed = false;
                var focusCtrl = that.getFocusedChild();
                if (focusCtrl && focusCtrl.doKeyOk) {
                    consumed = focusCtrl.doKeyOk();
                }
                return consumed;
            };
            CTopPicksCtrl.prototype.doKeyRec = function () {
                var that = this;
                var consumed = false;
                var focusCtrl = that.getFocusedChild();
                if (focusCtrl && focusCtrl.doKeyRec) {
                    consumed = focusCtrl.doKeyRec();
                }
                return consumed;
            };
            CTopPicksCtrl.prototype.doKeyStop = function () {
                var that = this;
                var consumed = false;
                var focusCtrl = that.getFocusedChild();
                if (focusCtrl && focusCtrl.doKeyStop) {
                    consumed = focusCtrl.doKeyStop();
                }
                return consumed;
            };
            CTopPicksCtrl.prototype.doKeyOption = function () {
                var that = this;
                var consumed = false;
                var focusCtrl = that.getFocusedChild();
                if (focusCtrl && focusCtrl.doKeyOption) {
                    consumed = focusCtrl.doKeyOption();
                }
                return consumed;
            };
            CTopPicksCtrl.prototype.processAsyncData = function () {
                // TODO : use this by overriding if needed
            };
            return CTopPicksCtrl;
        })(__CONTROLS__.CVerticalGroupControl);
        var COnDemandCtrl = (function (_super) {
            __extends(COnDemandCtrl, _super);
            function COnDemandCtrl(parent, dataAPI, $el) {
                _super.call(this, parent, dataAPI, $el);
                var that = this;
                that._noChannelHTML = 'No channels are available.<div class=\'description\'>Go to Channel Settings to search for channels.</div>';
                that._noInternetHTML = 'Connect to the internet and agree to the Terms and Conditions on Settings before using service.';
                that._notAvailableHTML = 'It is not available at the moment.<div class=\'description\'>Please try again later.</div>';
                that._processingHTML = 'We\'re loading some great recommendations for you.<div class=\'description\'>Please wait.</div>';
                if (arguments.length) {
                    that.init($el);
                }
            }
            COnDemandCtrl.prototype.destroy = function () {
                var that = this;
                _super.prototype.destroy.call(that);
            };
            COnDemandCtrl.prototype.init = function ($el) {
                var that = this;
                _super.prototype.init.call(that, $el);
                that.addClass('ondemand');
                if (that._dataAPI.getChannelCount() === 0) {
                    that._$noData.html(that._noChannelHTML);
                    that._contentsView.$self().addClass('notAvailable');
                    return;
                }
                if (!hx.svc.SETTING_UTIL.getInternetAvailable() || !hx.svc.TV_PORTAL.isTermsAndConditionsAccepted()) {
                    that._$noData.html(that._noInternetHTML);
                    that._contentsView.$self().addClass('notAvailable');
                    return;
                }
                that._dataAPI.getOnDemandCategories(function (categories) {
                    if (!categories || !categories.length) {
                        hx.log('debug', '[getOnDemandGroup] FAIL/ !categories || !categories.length');
                        that._$noData.html(that._notAvailableHTML);
                        that._contentsView.$self().addClass('notAvailable');
                        return;
                    }
                    var groupId = categories[0].groupId;
                    that._categoryCtrl.setData(categories);
                    that._categoryCtrl.draw();
                    that._ondemandReqId = groupId;
                }, function () {
                    that.drawWaitingView();
                });
            };
            COnDemandCtrl.prototype.processAsyncData = function () {
                var that = this;
                that._dataAPI.getOnDemandGroup(that._ondemandReqId, function (groupId, contents) {
                    if (that._ondemandReqId !== groupId) {
                        hx.log('debug', '[getOnDemandGroup] cannot init this category (that._ondemandReqId !== groupId)');
                        return;
                    }
                    if (!contents || !contents.length) {
                        hx.log('debug', '[getOnDemandGroup] FAIL/ !contents || !contents.length');
                        that._$noData.html(that._notAvailableHTML);
                        that._contentsView.$self().addClass('notAvailable');
                        return;
                    }
                    that._contentsView.$self().removeClass('notAvailable');
                    that._contentsCtrl.setDataProvider(new CDataProvider(contents));
                    that._contentsView.draw();
                    if ((that._parent.getStartIndex() === 3 /* eON_DEMAND */) && (that._parent.getFocusCtrlIndex() === 3 /* eON_DEMAND */)) {
                        that._parent.doKey(CONTROLS_CONFIG.KEY_DOWN);
                        that._parent.doKey(CONTROLS_CONFIG.KEY_DOWN);
                    }
                });
            };
            COnDemandCtrl.prototype.drawWaitingView = function () {
                var that = this;
                that._$noData.html(that._processingHTML);
                that._contentsView.$self().addClass('notAvailable');
            };
            COnDemandCtrl.prototype.makeChildCtrls = function () {
                var that = this;
                var categoryCtrl = new CCarouselControl(null);
                var contentsCtrl = new CGridControl(null);
                var contentsView = new CViewGroupControl(null);
                var TIMEOUT_UPDATE_DATA = 1000; // 1 sec
                categoryCtrl.setId('categories');
                categoryCtrl.setData([]);
                categoryCtrl.setDrawParam({
                    viewCount: 5,
                    anchorIndex: 2,
                    itemWidth: 149,
                    anchorWidth: 149,
                    transparentAnchor: false,
                    align: 'horizontal',
                    animation: true,
                    animationInterval: 0.3,
                    maxKeyQueueCount: 0
                });
                categoryCtrl.setDataDrawer(function (param) {
                    var item = param ? param.item : null;
                    var $item = param ? param.$item : null;
                    if (item && $item) {
                        $item.addClass('categoryItem');
                        $item.text(item.title);
                    }
                });
                categoryCtrl.onFocusStartToChange = function (param) {
                    var item = param.item;
                    if (item) {
                        var groupId = item.groupId;
                        that._ondemandReqId = groupId;
                        if (that._ondemandRequestTimeout) {
                            clearTimeout(that._ondemandRequestTimeout);
                            that._ondemandRequestTimeout = null;
                        }
                        that._ondemandRequestTimeout = setTimeout(function () {
                            that._contentsView.$self().css('opacity', '0');
                            that._dataAPI.getOnDemandGroup(groupId, function (groupId, contents) {
                                if (that._ondemandReqId !== groupId) {
                                    hx.log('debug', '[getOnDemandGroup] cannot update this category (that._ondemandReqId !== groupId)');
                                    return;
                                }
                                if (!contents || !contents.length) {
                                    hx.log('debug', '[getOnDemandGroup] FAIL// !contents || !contents.length');
                                    that._contentsCtrl.setDataProvider(new CDataProvider([]));
                                    that._$noData.html(that._notAvailableHTML);
                                    that._contentsView.$self().addClass('notAvailable');
                                    that._contentsView.$self().css('opacity', '1');
                                    return;
                                }
                                that._contentsCtrl.setDataProvider(new CDataProvider(contents));
                                that._contentsView.$self().removeClass('notAvailable');
                                that._contentsView.$self().css('opacity', '1');
                                that._contentsView.setTop(0, true); // including draw()
                                //that._contentsView.draw();
                            });
                        }, TIMEOUT_UPDATE_DATA);
                    }
                };
                categoryCtrl.setActiveFocus = function (activeFocus) {
                    if (activeFocus) {
                        this.$self().addClass('-afd');
                    }
                };
                categoryCtrl.doKeyPageDown = function () {
                    return false;
                };
                categoryCtrl.doKeyPageUp = function () {
                    return false;
                };
                contentsCtrl.setDrawParam({
                    maxColCount: 3,
                    itemWidth: 275,
                    itemHeight: 158,
                    scrollByItemSize: true,
                    scrollPageSizeByPageUpDownKey: true
                });
                contentsCtrl.setDataProvider(new CDataProvider([]));
                contentsCtrl.onItemSelected = function (param) {
                    var item = param ? param.item : null;
                    if (item) {
                        that._dataAPI.openDeepLink(item);
                    }
                };
                contentsCtrl.setDataDrawer(function (param) {
                    var $item = param.$item;
                    var item = param.item;
                    var $thumbnail, $title, $secondary, $owingLogo;
                    if (!$item.hasClass('contentItem')) {
                        $item.addClass('contentItem');
                        $item.append($('<div>', {
                            'id': 'thumbnail'
                        }));
                        $item.append($('<div>', {
                            'id': 'dim'
                        }));
                        $item.append($('<div>', {
                            'id': 'title'
                        }));
                        $item.append($('<div>', {
                            'id': 'secondary'
                        }));
                        $item.append($('<div>', {
                            'id': 'owingLogo'
                        }));
                    }
                    if (item) {
                        $thumbnail = $item.children('#thumbnail');
                        if ($thumbnail) {
                            if (item.thumbnailURL) {
                                $thumbnail.css('background', 'url("' + item.thumbnailURL + '") no-repeat center/100% 100%');
                            }
                            else {
                                $thumbnail.css('background', 'url("images/614_OD_Default01.png") no-repeat center/100% 100%');
                                item.getThumbnail({
                                    updateCb: function (receivedUrl) {
                                        if (receivedUrl) {
                                            item.thumbnailURL = receivedUrl;
                                            $thumbnail.css('background', 'url("' + item.thumbnailURL + '") no-repeat center/100% 100%');
                                        }
                                    },
                                    width: that.NORMAL_THUMBNAIL_WIDTH
                                });
                            }
                        }
                        $title = $item.children('#title');
                        if ($title) {
                            $title.text(item.name);
                        }
                        $secondary = $item.children('#secondary');
                        if ($secondary) {
                            $secondary.text(item.secondary);
                        }
                        $owingLogo = $item.children('#owingLogo');
                        if ($owingLogo) {
                            if (item.logoURL) {
                                $owingLogo.css('background', 'url("' + item.logoURL + '") no-repeat center/100% 100%');
                            }
                            else {
                                item.getContentOwningLogo({
                                    updateCb: function (receivedUrl) {
                                        if (receivedUrl) {
                                            item.logoURL = receivedUrl;
                                            $owingLogo.css('background', 'url("' + item.logoURL + '") no-repeat center/100% 100%');
                                        }
                                    },
                                    width: that.NORMAL_CHANNEL_LOGO_WIDTH
                                });
                            }
                        }
                    }
                });
                contentsView.setChildControl([contentsCtrl]);
                contentsView.doKeyOk = function () {
                    return this.getFocusedChild().doKeyOk();
                };
                contentsView.setId('contentsView');
                that._categoryCtrl = categoryCtrl;
                that._contentsCtrl = contentsCtrl;
                that._contentsView = contentsView;
                return [categoryCtrl, contentsView];
            };
            COnDemandCtrl.prototype.getOnDemandReqId = function () {
                return this._ondemandReqId;
            };
            COnDemandCtrl.prototype.getCategoryCtrl = function () {
                return this._categoryCtrl;
            };
            COnDemandCtrl.prototype.doKeyPageDown = function () {
                var that = this;
                if (that._contentsView.$self().hasClass('notAvailable')) {
                    return true;
                }
                return false;
            };
            COnDemandCtrl.prototype.doKeyDown = function () {
                var that = this;
                if (that._contentsView.$self().hasClass('notAvailable')) {
                    return true;
                }
                return false;
            };
            return COnDemandCtrl;
        })(CTopPicksCtrl);
        var CRecordingsCtrls = (function (_super) {
            __extends(CRecordingsCtrls, _super);
            function CRecordingsCtrls(parent, dataAPI, $el) {
                _super.call(this, parent, dataAPI, $el);
                var that = this;
                that._noRecentlyHTML = 'There are no new recordings.<div class=\'description\'>Go to TV Guide and set recording schedules for the programmes<br>you want to record.</div>';
                that._noWatchingHTML = 'There are no recordings to continue.';
                if (arguments.length) {
                    that.init($el);
                }
            }
            CRecordingsCtrls.prototype.destroy = function () {
                var that = this;
                _super.prototype.destroy.call(that);
            };
            CRecordingsCtrls.prototype.init = function ($el) {
                var that = this;
                _super.prototype.init.call(that, $el);
                that.addClass('recordings');
                that._selectedIndex = 0 /* eRECENTLY */;
                if (!that._contentsCtrl.getDataProvider().getCount()) {
                    that._$noData.html(that._noRecentlyHTML);
                    that._contentsView.$self().addClass('noRecording');
                }
            };
            CRecordingsCtrls.prototype.makeChildCtrls = function () {
                var that = this;
                var categoryCtrl = new CListControl(null);
                var contentsCtrl = new CGridControl(null);
                var contentsView = new CViewGroupControl(null);
                categoryCtrl.setId('categories');
                categoryCtrl.setDrawParam({
                    align: 'horizontal',
                    itemWidth: 160,
                    itemHeight: 42
                });
                categoryCtrl.setDataProvider(new CDataProvider(['Recently Added', 'Continue Watching']));
                categoryCtrl.setDataDrawer(function (param) {
                    var $item = param ? param.$item : null;
                    var item = param ? param.item : null;
                    var index = param ? param.index : 0;
                    if ($item && item) {
                        $item.text(item);
                        if (index === this.getSelectedIndex()) {
                            $item.addClass('-sel');
                        }
                    }
                });
                categoryCtrl.onFocusChanged = function ($old, $new) {
                    that._selectedIndex = $new.index();
                    that._contentsView.$self().css('opacity', '0');
                    $old.removeClass('-sel');
                    $new.addClass('-sel');
                    var dataList = null;
                    var dataProvider = null;
                    if (that._selectedIndex === 0 /* eRECENTLY */) {
                        dataList = that._dataAPI.getRecentlyRecordings();
                        if (dataList && dataList.length) {
                            that._contentsView.$self().removeClass('noRecording');
                            dataProvider = new CDataProvider(dataList);
                        }
                        else {
                            that._$noData.html(that._noRecentlyHTML);
                            that._contentsView.$self().addClass('noRecording');
                            dataProvider = new CDataProvider([]);
                        }
                        that._contentsView.$self().removeClass('-continue');
                        that._contentsView.$self().addClass('-recently');
                    }
                    else if (that._selectedIndex === 1 /* eCONTINUE */) {
                        dataList = that._dataAPI.getContinueWatchRecordings();
                        if (dataList && dataList.length) {
                            that._contentsView.$self().removeClass('noRecording');
                            dataProvider = new CDataProvider(dataList);
                        }
                        else {
                            that._$noData.html(that._noWatchingHTML);
                            that._contentsView.$self().addClass('noRecording');
                            dataProvider = new CDataProvider([]);
                        }
                        that._contentsView.$self().removeClass('-recently');
                        that._contentsView.$self().addClass('-continue');
                    }
                    else {
                        hx.log('error', "category change index error");
                        dataProvider = new CDataProvider([]);
                    }
                    that._contentsCtrl.setDataProvider(dataProvider);
                    that._contentsView.$self().css('opacity', '1');
                    that._contentsView.setTop(0, true); // including draw()
                    //that._contentsView.draw();
                };
                contentsCtrl.setDrawParam({
                    maxColCount: 3,
                    itemWidth: 275,
                    itemHeight: 158,
                    scrollByItemSize: true,
                    scrollPageSizeByPageUpDownKey: true
                });
                contentsCtrl.setDataProvider(new CDataProvider(that._dataAPI.getRecentlyRecordings()));
                contentsCtrl.onItemSelected = function (param) {
                    var item = param ? param.item : null;
                    if (item) {
                        if (item.type === 'more') {
                            that._dataAPI.seeMore(2 /* eRECORDINGS */);
                        }
                        else {
                            that._dataAPI.requestPlayback(item);
                        }
                    }
                };
                contentsCtrl.setDataDrawer(function (param) {
                    var $item = param.$item;
                    var item = param.item;
                    var $thumbnail, $title, $secondary, $durationBar, $channelLogo, $icon;
                    if (item && item.type == 'more') {
                        $item.addClass('contentMore');
                        $item.text('SEE ALL RECORDINGS');
                    }
                    else {
                        if (!$item.hasClass('contentItem')) {
                            $item.addClass('contentItem');
                            $item.append($('<div>', {
                                'id': 'thumbnail'
                            }));
                            $item.append($('<div>', {
                                'id': 'dim'
                            }));
                            $item.append($('<div>', {
                                'id': 'title'
                            }));
                            $item.append($('<div>', {
                                'id': 'secondary'
                            }));
                            $item.append($('<div>', {
                                'id': 'channelLogo'
                            }));
                            $item.append($('<div>', {
                                'id': 'icon'
                            }));
                            $item.append($('<div>', {
                                'id': 'durationBg'
                            }).append($('<div>', {
                                'id': 'durationBar'
                            })));
                        }
                        if (item) {
                            $thumbnail = $item.children('#thumbnail');
                            if ($thumbnail) {
                                $thumbnail.css('background', 'url("' + (item.getThumbnailUrl() || __MODEL__.TDefaultThumbnailList.RECORDING) + '") no-repeat center/100% 100%');
                            }
                            //if (item.isNew()) {
                            //    $thumbnail.addClass('icoNew');
                            //} else {
                            //    $thumbnail.removeClass('icoNew');
                            //}
                            $icon = $item.children('#icon');
                            if ($icon) {
                                $icon.removeClass('icoLocked icoRecording icoGuidance');
                                if (item.isLocked()) {
                                    $icon.addClass('icoLocked');
                                }
                                else if (item.hasGuidance()) {
                                    $icon.addClass('icoGuidance');
                                }
                                else if (item.isRecordingOnGoing()) {
                                    $icon.addClass('icoRecording');
                                }
                            }
                            $title = $item.children('#title');
                            if ($title) {
                                var title = item.getTitle ? item.getTitle() : "";
                                $title.text(title || "");
                            }
                            $secondary = $item.children('#secondary');
                            if ($secondary) {
                                // MON 01 MAR 09:20-10:30
                                var dispDate = dateFormat(item.getStartTime(), 'ddd dd mmm') + ' <span>|</span> ';
                                var dispTime = dateFormat(item.getStartTime(), 'HH:MM') + ' - ' + dateFormat(item.getEndTime(), 'HH:MM');
                                $secondary.html(dispDate + dispTime);
                            }
                            if (that._selectedIndex === 1 /* eCONTINUE */) {
                                $durationBar = $item.find('#durationBar');
                                if ($durationBar) {
                                    var lastPos = item.getLastPlayPosition ? item.getLastPlayPosition() : 0;
                                    var duration = item.getDuration ? parseInt(item.getDuration()) : 0;
                                    var percent = 0;
                                    if (duration) {
                                        percent = parseInt((lastPos / duration * 100) + "", 10);
                                    }
                                    $durationBar.width(percent + "%");
                                }
                            }
                            $channelLogo = $item.children('#channelLogo');
                            if ($channelLogo) {
                                if (item.logoURL) {
                                    $channelLogo.append($('<img>', {
                                        'class': 'logo',
                                        'src': item.logoURL,
                                        'alt': ''
                                    }));
                                }
                                else {
                                    that._dataAPI.getChannelLogo(item.getChannelId(), that.NORMAL_CHANNEL_LOGO_WIDTH, function (receivedUrl) {
                                        if (receivedUrl) {
                                            item.logoURL = receivedUrl;
                                            $channelLogo.append($('<img>', {
                                                'class': 'logo',
                                                'src': item.logoURL,
                                                'alt': ''
                                            }));
                                        }
                                        else {
                                            $channelLogo.text((item.getChannelName() || '').toUpperCase());
                                        }
                                    });
                                }
                            }
                        }
                    }
                });
                contentsView.setChildControl([contentsCtrl]);
                contentsView.doKeyOk = function () {
                    return this.getFocusedChild().doKeyOk();
                };
                contentsView.setId('contentsView');
                contentsView.addClass('-recently');
                that._categoryCtrl = categoryCtrl;
                that._contentsCtrl = contentsCtrl;
                that._contentsView = contentsView;
                return [categoryCtrl, contentsView];
            };
            CRecordingsCtrls.prototype.getCategoryCtrl = function () {
                return this._categoryCtrl;
            };
            CRecordingsCtrls.prototype.doKeyPageDown = function () {
                var that = this;
                if (that._contentsView.$self().hasClass('noRecording')) {
                    return true;
                }
                return false;
            };
            CRecordingsCtrls.prototype.doKeyDown = function () {
                var that = this;
                if (that._contentsView.$self().hasClass('noRecording')) {
                    return true;
                }
                return false;
            };
            return CRecordingsCtrls;
        })(CTopPicksCtrl);
        var CNextCtrls = (function (_super) {
            __extends(CNextCtrls, _super);
            function CNextCtrls(parent, dataAPI, $el) {
                _super.call(this, parent, dataAPI, $el);
                var that = this;
                that._noNextHTML = 'There are no recommendations to record.';
                that._noChannelHTML = 'No channels are available.<div class=\'description\'>Go to Channel Settings to search for channels.</div>';
                that._noInternetHTML = 'Connect to the internet and agree to the Privacy Policy on Settings before using service.';
                that._notAvailableHTML = 'It is not available at the moment.<div class=\'description\'>Please try again later.</div>';
                that._processingHTML = 'We\'re loading some great recommendations for you.<div class=\'description\'>Please wait.</div>';
                if (arguments.length) {
                    that.init($el);
                }
            }
            CNextCtrls.prototype.destroy = function () {
                var that = this;
                _super.prototype.destroy.call(that);
            };
            CNextCtrls.prototype.init = function ($el) {
                var that = this;
                _super.prototype.init.call(that, $el);
                that.addClass('next');
                that._selectedIndex = 0 /* eDONTMISSIT */;
                if (that._dataAPI.getChannelCount() === 0) {
                    that._$noData.html(that._noChannelHTML);
                    that._contentsView.$self().addClass('notAvailable');
                    return;
                }
                if (!hx.svc.SETTING_UTIL.getInternetAvailable() || !hx.svc.TV_PORTAL.isPrivacyPolicyAccepted()) {
                    that._$noData.html(that._noInternetHTML);
                    that._contentsView.$self().addClass('notAvailable');
                    return;
                }
                that._$noData.html(that._processingHTML);
                that._contentsView.$self().addClass('notAvailable');
            };
            CNextCtrls.prototype.processAsyncData = function () {
                var that = this;
                that._dataAPI.getHumaxRecommendation(3 /* EDontMissIt */, function (contents, type) {
                    that.drawContentView(contents, type);
                }, function (type) {
                    that.drawWaitingView(type);
                });
                that._dataAPI.getHumaxRecommendation(4 /* EPopularTvSeries */, function (contents, type) {
                    that.drawContentView(contents, type);
                }, function (type) {
                    that.drawWaitingView(type);
                });
            };
            CNextCtrls.prototype.drawContentView = function (aContents, aType) {
                var that = this;
                var targetType = 0 /* ENone */;
                switch (that._selectedIndex) {
                    case 0 /* eDONTMISSIT */:
                        targetType = 3 /* EDontMissIt */;
                        break;
                    case 1 /* ePOPULARTVSERIES */:
                        targetType = 4 /* EPopularTvSeries */;
                        break;
                    default:
                        break;
                }
                if (targetType !== aType) {
                    hx.log('debug', '[drawContentView] targetType !== aType');
                    return;
                }
                if (!aContents || !aContents.length) {
                    hx.log('debug', '[drawContentView] FAIL/ !contents || !contents.length');
                    if (that._$noData) {
                        that._$noData.html(that._notAvailableHTML);
                    }
                    if (that._contentsView) {
                        that._contentsView.$self().addClass('notAvailable');
                    }
                    return;
                }
                if (that._contentsView) {
                    that._contentsView.$self().removeClass('notAvailable');
                    that._contentsCtrl.setDataProvider(new CDataProvider(aContents));
                    that._contentsView.setTop(0, true); // including draw()
                }
            };
            CNextCtrls.prototype.drawWaitingView = function (aType) {
                var that = this;
                var targetType = 0 /* ENone */;
                switch (that._selectedIndex) {
                    case 0 /* eDONTMISSIT */:
                        targetType = 3 /* EDontMissIt */;
                        break;
                    case 1 /* ePOPULARTVSERIES */:
                        targetType = 4 /* EPopularTvSeries */;
                        break;
                    default:
                        break;
                }
                if (targetType !== aType) {
                    hx.log('debug', '[NEXT][drawWaitingView] targetType !== aType');
                    return;
                }
                if (that._$noData) {
                    that._$noData.html(that._processingHTML);
                }
                if (that._contentsView) {
                    that._contentsView.$self().addClass('notAvailable');
                }
            };
            CNextCtrls.prototype.makeChildCtrls = function () {
                var that = this;
                var categoryCtrl = new CListControl(null);
                var contentsCtrl = new CGridControl(null);
                var contentsView = new CViewGroupControl(null);
                categoryCtrl.setId('categories');
                categoryCtrl.setDrawParam({
                    align: 'horizontal',
                    itemWidth: 160,
                    itemHeight: 42
                });
                categoryCtrl.setDataProvider(new CDataProvider(['Don\'t miss it', 'Popular TV series']));
                categoryCtrl.setDataDrawer(function (param) {
                    var $item = param ? param.$item : null;
                    var item = param ? param.item : null;
                    var index = param ? param.index : 0;
                    if ($item && item) {
                        $item.text(item);
                        if (index === this.getSelectedIndex()) {
                            $item.addClass('-sel');
                        }
                    }
                });
                categoryCtrl.onFocusChanged = function ($old, $new) {
                    that._selectedIndex = $new.index();
                    that._contentsCtrl.clearDrawnItems();
                    $old.removeClass('-sel');
                    $new.addClass('-sel');
                    var targetType = 0 /* ENone */;
                    switch (that._selectedIndex) {
                        case 0 /* eDONTMISSIT */:
                            targetType = 3 /* EDontMissIt */;
                            break;
                        case 1 /* ePOPULARTVSERIES */:
                            targetType = 4 /* EPopularTvSeries */;
                            break;
                        default:
                            break;
                    }
                    if (that._dataAPI.getChannelCount() === 0) {
                        hx.log('error', '[NEXT][onFocusChanged] fail!! getChannelCount() === 0');
                        that._$noData.html(that._noChannelHTML);
                        that._contentsView.$self().addClass('notAvailable');
                        return;
                    }
                    if (targetType === 0 /* ENone */) {
                        hx.log('error', '[NEXT][onFocusChanged] invalid selectedIndex');
                        that._contentsView.$self().addClass('notAvailable');
                        that._contentsCtrl.setDataProvider(new CDataProvider([]));
                        that._contentsView.setTop(0, true); // including draw()
                        return;
                    }
                    if (!hx.svc.SETTING_UTIL.getInternetAvailable() || !hx.svc.TV_PORTAL.isPrivacyPolicyAccepted()) {
                        hx.log('error', '[NEXT][onFocusChanged] fail!! by internet or PP problem');
                        that._$noData.html(that._noInternetHTML);
                        that._contentsView.$self().addClass('notAvailable');
                        return;
                    }
                    that._dataAPI.getHumaxRecommendation(targetType, function (contents, type) {
                        that.drawContentView(contents, type);
                    }, function (type) {
                        that.drawWaitingView(type);
                    });
                };
                contentsCtrl.setDrawParam({
                    maxColCount: 3,
                    itemWidth: 275,
                    itemHeight: 158,
                    scrollByItemSize: true,
                    scrollPageSizeByPageUpDownKey: true
                });
                contentsCtrl.setDataProvider(new CDataProvider([]));
                contentsCtrl.onItemSelected = function (param) {
                    var item = param ? param.item : null;
                    if (item) {
                        that._dataAPI.showProgrammeDetail(item);
                    }
                };
                contentsCtrl.setDataDrawer(function (param) {
                    var $item = param.$item;
                    var item = param.item;
                    var $thumbnail, $title, $secondary, $channelLogo, $icon;
                    if (item && item.type == 'more') {
                        $item.addClass('contentMore');
                        $item.text('SEE ALL RECORDINGS');
                    }
                    else {
                        if (!$item.hasClass('contentItem')) {
                            $item.addClass('contentItem');
                            $item.append($('<div>', {
                                'id': 'thumbnail'
                            }));
                            $item.append($('<div>', {
                                'id': 'dim'
                            }));
                            $item.append($('<div>', {
                                'id': 'title'
                            }));
                            $item.append($('<div>', {
                                'id': 'secondary'
                            }));
                            $item.append($('<div>', {
                                'id': 'channelLogo'
                            }));
                            $item.append($('<div>', {
                                'id': 'icon'
                            }));
                        }
                        if (item) {
                            $title = $item.children('#title');
                            if ($title) {
                                $title.text(item.getName() || "");
                            }
                            $secondary = $item.children('#secondary');
                            if ($secondary) {
                                // MON 01 MAR 09:20-10:30
                                var dispDate = dateFormat(item.getStartTime() * 1000, 'ddd dd mmm') + ' <span>|</span> ';
                                var dispTime = dateFormat(item.getStartTime() * 1000, 'HH:MM') + ' - ' + dateFormat(item.getEndTime() * 1000, 'HH:MM');
                                $secondary.html(dispDate + dispTime);
                            }
                            $icon = $item.children('#icon');
                            if ($icon) {
                                $icon.removeClass('icoRec icoRemind icoSeries icoGuidance');
                                if (item.hasSchedule()) {
                                    $icon.addClass('icoRec');
                                }
                                else if (item.hasReminder()) {
                                    $icon.addClass('icoRemind');
                                }
                                else if (item.hasSeriesSchedule()) {
                                    $icon.addClass('icoSeries');
                                }
                                else if (item.hasGuidance()) {
                                    $icon.addClass('icoGuidance');
                                }
                            }
                            $thumbnail = $item.children('#thumbnail');
                            if (item.thumbnail) {
                                $thumbnail.css('background', 'url("' + item.thumbnail + '") no-repeat center/100% 100%');
                            }
                            else {
                                that._dataAPI.getThumbnail(item, that.NORMAL_THUMBNAIL_WIDTH, function (receivedUrl) {
                                    $thumbnail.css('background', 'url("' + (item.thumbnail = (receivedUrl || __MODEL__.TDefaultThumbnailList.DEFAULT)) + '") no-repeat center/100% 100%');
                                });
                            }
                            $channelLogo = $item.children('#channelLogo');
                            if ($channelLogo) {
                                if (item.logoURL) {
                                    $channelLogo.append($('<img>', {
                                        'class': 'logo',
                                        'src': item.logoURL,
                                        'alt': ''
                                    }));
                                }
                                else {
                                    that._dataAPI.getChannelLogo(item.getChannelId(), that.NORMAL_CHANNEL_LOGO_WIDTH, function (receivedUrl) {
                                        if (receivedUrl) {
                                            item.logoURL = receivedUrl;
                                            $channelLogo.append($('<img>', {
                                                'class': 'logo',
                                                'src': item.logoURL,
                                                'alt': ''
                                            }));
                                        }
                                        else {
                                            $channelLogo.text((item.getChannelName() || '').toUpperCase());
                                        }
                                    });
                                }
                            }
                        }
                    }
                });
                contentsView.setChildControl([contentsCtrl]);
                contentsView.doKeyOk = function () {
                    return this.getFocusedChild().doKeyOk();
                };
                contentsView.doKeyRec = function () {
                    var bConsumed = false;
                    var targetItem = that._contentsCtrl.getFocusedItem();
                    if (targetItem && targetItem.item) {
                        bConsumed = that._dataAPI.requestReservation(targetItem.item);
                    }
                    return bConsumed;
                };
                contentsView.setId('contentsView');
                //contentsView.addClass('-recently');
                that._categoryCtrl = categoryCtrl;
                that._contentsCtrl = contentsCtrl;
                that._contentsView = contentsView;
                return [categoryCtrl, contentsView];
            };
            CNextCtrls.prototype.getCategoryCtrl = function () {
                return this._categoryCtrl;
            };
            CNextCtrls.prototype.doKeyPageDown = function () {
                if (this._contentsView.$self().hasClass('notAvailable')) {
                    return true;
                }
                return false;
            };
            CNextCtrls.prototype.doKeyDown = function () {
                if (this._contentsView.$self().hasClass('notAvailable')) {
                    return true;
                }
                return false;
            };
            return CNextCtrls;
        })(CTopPicksCtrl);
        var COnNowCtrl = (function (_super) {
            __extends(COnNowCtrl, _super);
            function COnNowCtrl(parent, dataAPI, $el) {
                _super.call(this, parent, dataAPI, $el);
                var that = this;
                that._noChannelHTML = 'No channels are available.<div class=\'description\'>Go to Channel Settings to search for channels.</div>';
                that.ON_NOW_MAX = 9;
                if (arguments.length) {
                    that.init($el);
                }
            }
            COnNowCtrl.prototype.destroy = function () {
                var that = this;
                if (that._$edit) {
                    that._$edit.remove();
                    delete that._$edit;
                    that._$edit = undefined;
                }
                if (that._editDlg) {
                    that._editDlg.close();
                    that._editDlg = undefined;
                }
                if (that._onNowChannels) {
                    delete that._onNowChannels;
                    that._onNowChannels = undefined;
                }
                if (that._channels) {
                    delete that._channels;
                    that._channels = undefined;
                }
                _super.prototype.destroy.call(that);
            };
            COnNowCtrl.prototype.init = function ($el) {
                var that = this;
                that._onNowChannels = that._dataAPI.getOnNowChannels();
                that._channels = that._dataAPI.getChannels();
                _super.prototype.init.call(that, $el);
                that.addClass('onNow');
                if (that._dataAPI.getChannelCount() === 0) {
                    that._$noData.html(that._noChannelHTML);
                    that._contentsView.$self().addClass('noChannel');
                    return;
                }
                if (!that._onNowChannels || !that._onNowChannels.length) {
                    that._$noData.html(that._noChannelHTML);
                    that._contentsView.$self().addClass('noChannel');
                }
            };
            COnNowCtrl.prototype.makeChildCtrls = function () {
                var that = this;
                var contentsCtrl = new CGridControl(null);
                var contentsView = new CViewGroupControl(null);
                var $edit, $editIcon, $editText;
                contentsCtrl.setDrawParam({
                    maxColCount: 3,
                    itemWidth: 275,
                    itemHeight: 158
                });
                contentsCtrl.setDataProvider(new CDataProvider(that._onNowChannels));
                contentsCtrl.onItemSelected = function (param) {
                    var item = param ? param.item : null;
                    if (item) {
                        that.setChannel(that._onNowChannels[param.itemKey]);
                    }
                };
                contentsCtrl.setDataDrawer(function (param) {
                    var item = param.item;
                    var $item = param.$item;
                    var focusedIndex = param.index;
                    if (!item || !$item) {
                        hx.log('debug', '[COnNowCtrl][setDataDrawer] Fail!! cos (!item || !$item)');
                        return;
                    }
                    $item.addClass('contentItem');
                    $item.append($('<div>', {
                        'id': 'thumbnail'
                    })).append($('<div>', {
                        'id': 'dim'
                    })).append($('<div>', {
                        'id': 'title'
                    })).append($('<div>', {
                        'id': 'icon'
                    })).append($('<div>', {
                        'id': 'secondary'
                    })).append($('<div>', {
                        'id': 'channelLogo'
                    })).append($('<div>', {
                        'id': 'durationBg'
                    }).append($('<div>', {
                        'id': 'durationBar'
                    })));
                    var ccid = item.getId ? item.getId() : null;
                    if (!ccid) {
                        hx.log('debug', '[COnNowCtrl][setDataDrawer] Fail!! cos (!ccid)');
                        return;
                    }
                    that._dataAPI.searchOnNowEPGByCcid(ccid, function (param) {
                        var onNowEvent = param.result;
                        var onNowChannel = param.channel;
                        var isChannelLock = onNowChannel ? onNowChannel.manualBlock : false;
                        if (onNowEvent) {
                            var fnGetThumbnail = param.getThumbnail;
                            var $title = $item.children('#title');
                            var $secondary = $item.children('#secondary');
                            var $durationBar = $item.find('#durationBar');
                            var $icon = $item.children('#icon');
                            var $thumbnail = $item.children('#thumbnail');
                            if (onNowEvent.getProgrammeName) {
                                $title.text(onNowEvent.getProgrammeName());
                            }
                            if (onNowEvent.getShortDescription) {
                                $secondary.text(onNowEvent.getShortDescription());
                            }
                            if (onNowEvent.getPercentage) {
                                $durationBar.width(onNowEvent.getPercentage() + '%');
                            }
                            $icon.removeClass('icoRecording', 'icoGuidance', 'icoLocked');
                            if (hx.svc.RECORDINGS.getCurrentRecordingByCcid(item.getId())) {
                                $icon.addClass('icoRecording');
                            }
                            else if (isChannelLock) {
                                $icon.addClass('icoLocked');
                            }
                            else if (onNowEvent.hasGuidance()) {
                                $icon.addClass('icoGuidance');
                            }
                            if (isChannelLock) {
                                if (that._onNowChannels[focusedIndex].isRadio()) {
                                    $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.RADIO + '") no-repeat center/100% 100%');
                                }
                                else {
                                    $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.TV + '") no-repeat center/100% 100%');
                                }
                            }
                            else {
                                fnGetThumbnail(onNowEvent, that.NORMAL_THUMBNAIL_WIDTH, function (receivedUrl) {
                                    if (receivedUrl) {
                                        $thumbnail.css('background', 'url("' + receivedUrl + '") no-repeat center/100% 100%');
                                    }
                                    if (onNowEvent.getSecondary()) {
                                        $secondary.text(onNowEvent.getSecondary());
                                    }
                                });
                            }
                            if (param.needUpdateChannel) {
                                if (onNowEvent.getChannelId()) {
                                    that._dataAPI.updateOnNowChannel(focusedIndex, onNowEvent.getChannelId());
                                    that._onNowChannels = that._dataAPI.getOnNowChannels();
                                    that._dataAPI.getChannelLogo(onNowEvent.getChannelId(), that.NORMAL_CHANNEL_LOGO_WIDTH, function (receivedUrl) {
                                        var $channelLogo = $item.children('#channelLogo');
                                        if (receivedUrl) {
                                            $channelLogo.append($('<img>', {
                                                'class': 'logo',
                                                'src': receivedUrl,
                                                'alt': ''
                                            }));
                                        }
                                        else {
                                            $channelLogo.text((that._onNowChannels[focusedIndex].getName()).toUpperCase());
                                        }
                                    });
                                }
                            }
                        }
                        else {
                            var $thumbnail = $item.children('#thumbnail');
                            if (isChannelLock) {
                                $item.children('#icon').addClass('icoLocked');
                            }
                            if (that._onNowChannels[focusedIndex].isRadio()) {
                                $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.RADIO + '") no-repeat center/100% 100%');
                            }
                            else {
                                $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.TV + '") no-repeat center/100% 100%');
                            }
                        }
                    });
                    //request channel logo
                    that._dataAPI.getChannelLogo(ccid, that.NORMAL_CHANNEL_LOGO_WIDTH, function (receivedUrl) {
                        var $channelLogo = $item.children('#channelLogo');
                        if (receivedUrl) {
                            $channelLogo.append($('<img>', {
                                'class': 'logo',
                                'src': receivedUrl,
                                'alt': ''
                            }));
                        }
                        else {
                            $channelLogo.text((that._onNowChannels[focusedIndex].getName()).toUpperCase());
                        }
                    });
                });
                contentsCtrl.setActiveFocus = function (activeFocus) {
                    if (activeFocus) {
                        contentsCtrl.$self().addClass('-afd');
                        that._$edit.addClass('-show');
                    }
                    else {
                        contentsCtrl.$self().removeClass('-afd');
                        that._$edit.removeClass('-show');
                    }
                    CGridControl.prototype.setActiveFocus.call(this, activeFocus);
                };
                contentsCtrl.isFocusable = function () {
                    return true;
                };
                if (that._onNowChannels && that._onNowChannels.length) {
                    $edit = $('<div>', {
                        'id': 'edit'
                    });
                    $editIcon = $('<div>', {
                        'id': 'editIcon'
                    });
                    $editText = $('<div>', {
                        'id': 'editText'
                    }).text('REPLACE CHANNEL');
                    $edit.append($editIcon).append($editText);
                    that.$self().append($edit);
                }
                contentsView.setChildControl([contentsCtrl]);
                contentsView.doKeyOption = function () {
                    that._editDlg = prism.createListDlg(that._parent, {
                        groupItems: that._channels,
                        textHtml: 'Replace the selected channel.',
                        cls: 'chGrid'
                    }, function (param) {
                        that.updateOnNowChannel(param.id);
                    });
                    that._editDlg.open({
                        curGroupIndex: that.findFocusIndex(),
                        itemWidth: 360,
                        itemHeight: 40,
                        openClass: 'grid',
                        dimm: true,
                        onClosed: function (param) {
                            that._editDlg = null;
                        }
                    });
                    that._editDlg.doKeyEscape = function () {
                        return that._editDlg.doKeyBack();
                    };
                    return true;
                };
                contentsView.doKeyOk = function () {
                    return this.getFocusedChild().doKeyOk();
                };
                contentsView.doKeyRec = function () {
                    var bConsumed = false;
                    var targetItem = that._contentsCtrl.getFocusedItem();
                    if (targetItem && targetItem.item) {
                        bConsumed = that._dataAPI.requestRecord(targetItem.item.getId());
                    }
                    return bConsumed;
                };
                contentsView.doKeyStop = function () {
                    var bConsumed = false;
                    var targetItem = that._contentsCtrl.getFocusedItem();
                    if (targetItem && targetItem.item) {
                        bConsumed = that._dataAPI.requestStopRecord(targetItem.item.getId());
                    }
                    return bConsumed;
                };
                contentsView.setId('contentsView');
                that._contentsView = contentsView;
                that._contentsCtrl = contentsCtrl;
                that._$edit = $edit;
                return [contentsView];
            };
            COnNowCtrl.prototype.findFocusIndex = function () {
                var that = this;
                var focusedIndex = that._contentsCtrl.getCurrentIndex();
                if (focusedIndex < 0) {
                    return 0;
                }
                var focusedItemCcid = that._onNowChannels[focusedIndex].getId();
                var channel = null;
                var length = that._channels ? that._channels.length : 0;
                for (var i = 0; i < length; i++) {
                    channel = that._channels[i];
                    if (channel.id === focusedItemCcid) {
                        return i;
                    }
                }
                return 0;
            };
            COnNowCtrl.prototype.updateOnNowChannel = function (ccid) {
                var that = this;
                var focusedItem = that._contentsCtrl.getFocusedItem();
                var $item = focusedItem ? focusedItem.$item : null;
                var focusedIndex = focusedItem ? focusedItem.itemKey : 0;
                if (!$item) {
                    hx.log('debug', '[COnNowCtrl][updateOnNowChannel] Fail!! cos (!$item)');
                    return;
                }
                that._dataAPI.updateOnNowChannel(focusedIndex, ccid);
                that._onNowChannels = that._dataAPI.getOnNowChannels();
                that._dataAPI.searchOnNowEPGByCcid(ccid, function (param) {
                    var onNowEvent = param.result;
                    var onNowChannel = param.channel;
                    var fnGetThumbnail = param.getThumbnail;
                    var $title = $item.children('#title');
                    var $secondary = $item.children('#secondary');
                    var $durationBar = $item.find('#durationBar');
                    var $thumbnail = $item.children('#thumbnail');
                    var $icon = $item.children('#icon');
                    if (onNowEvent) {
                        if (onNowEvent.getProgrammeName) {
                            $title.text(onNowEvent.getProgrammeName());
                        }
                        if (onNowEvent.getShortDescription) {
                            $secondary.text(onNowEvent.getShortDescription());
                        }
                        if (onNowEvent.getPercentage) {
                            $durationBar.width(onNowEvent.getPercentage() + '%');
                        }
                        if (onNowChannel && onNowChannel.manualBlock) {
                            $icon.addClass('icoLocked');
                            if (that._onNowChannels[focusedIndex].isRadio()) {
                                $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.RADIO + '") no-repeat center/100% 100%');
                            }
                            else {
                                $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.TV + '") no-repeat center/100% 100%');
                            }
                        }
                        else {
                            $icon.removeClass('icoLocked');
                            fnGetThumbnail(onNowEvent, that.NORMAL_THUMBNAIL_WIDTH, function (receivedUrl) {
                                if (receivedUrl) {
                                    $thumbnail.css('background', 'url("' + receivedUrl + '") no-repeat center/100% 100%');
                                }
                                if (onNowEvent.getSecondary()) {
                                    $secondary.text(onNowEvent.getSecondary());
                                }
                            });
                        }
                    }
                    else {
                        $title.text('');
                        $secondary.text('');
                        $durationBar.width('0%');
                        if (onNowChannel && onNowChannel.manualBlock) {
                            $item.children('#icon').addClass('icoLocked');
                        }
                        else {
                            $item.children('#icon').removeClass('icoLocked');
                        }
                        if (that._onNowChannels[focusedIndex].isRadio()) {
                            $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.RADIO + '") no-repeat center/100% 100%');
                        }
                        else {
                            $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.TV + '") no-repeat center/100% 100%');
                        }
                    }
                });
                //request channel logo
                that._dataAPI.getChannelLogo(ccid, that.NORMAL_CHANNEL_LOGO_WIDTH, function (receivedUrl) {
                    var $channelLogo = $item.children('#channelLogo');
                    $channelLogo.html('');
                    if (receivedUrl) {
                        $channelLogo.append($('<img>', {
                            'class': 'logo',
                            'src': receivedUrl,
                            'alt': ''
                        }));
                    }
                    else {
                        $channelLogo.text((that._onNowChannels[focusedIndex].getName()).toUpperCase());
                    }
                });
            };
            COnNowCtrl.prototype.setChannel = function (targetChannel) {
                if (!targetChannel) {
                    hx.log('debug', '[toppicks][setChannel] Fail!! cos !targetChannel');
                    return;
                }
                this._dataAPI.setChannelByCcid(targetChannel.getId());
            };
            COnNowCtrl.prototype.doKeyPageDown = function () {
                var that = this;
                if (that._contentsView.$self().hasClass('noChannel')) {
                    return true;
                }
                return false;
            };
            COnNowCtrl.prototype.doKeyDown = function () {
                var that = this;
                if (that._contentsView.$self().hasClass('noChannel')) {
                    return true;
                }
                return false;
            };
            return COnNowCtrl;
        })(CTopPicksCtrl);
        var CTopPicksPage = (function (_super) {
            __extends(CTopPicksPage, _super);
            function CTopPicksPage($el, startPath) {
                _super.call(this, $el);
                this._startIndex = TTopPicksIndexByPath[startPath] || 0 /* eON_NOW */;
            }
            CTopPicksPage.prototype.destroy = function () {
                var that = this;
                if (that._topPicksCtrls) {
                    var length = that._topPicksCtrls.length;
                    for (var i = 0; i < length; i++) {
                        that._topPicksCtrls[i].destroy();
                        that._topPicksCtrls[i] = undefined;
                    }
                    delete that._topPicksCtrls;
                    that._topPicksCtrls = undefined;
                }
                if (that._contentsSlider) {
                    that._contentsSlider.destroy();
                    delete that._contentsSlider;
                    that._contentsSlider = undefined;
                }
                if (that._menuSlider) {
                    that._menuSlider.destroy();
                    delete that._menuSlider;
                    that._menuSlider = undefined;
                }
                if (that._dataView) {
                    that._dataView.destroy();
                    delete that._dataView;
                    that._dataView = undefined;
                }
                _super.prototype.destroy.call(that);
            };
            CTopPicksPage.prototype.init = function ($el) {
                var that = this;
                _super.prototype.init.call(that, $el);
                that.setId('toppicksPage');
                that._topPicksCtrls = new Array(4 /* eMAX */);
                that._focusCtrlIndex = 0 /* eON_NOW */;
            };
            CTopPicksPage.prototype.setDataAPI = function (dataAPI) {
                var that = this;
                that._dataAPI = dataAPI;
            };
            CTopPicksPage.prototype.makeMenuSlider = function () {
                var that = this;
                var menuSlider = new CCarouselControl(null);
                menuSlider.setId('menuSlider');
                menuSlider.setData(['ON NOW', 'NEXT', 'RECORDINGS', 'ON DEMAND']);
                menuSlider.setDrawParam({
                    viewCount: 4,
                    anchorIndex: 1,
                    itemWidth: 897,
                    anchorWidth: 897,
                    transparentAnchor: false,
                    align: 'horizontal',
                    animation: false,
                    maxKeyQueueCount: 0,
                    visibleAfterDraw: false,
                    setActiveFocus: true
                });
                menuSlider.setDataDrawer(function (param) {
                    var item = param.item;
                    var $item = param.$item;
                    var $menu = $('<div>', {
                        'id': 'menu'
                    }).text(item);
                    var $arrowL = $('<div>', {
                        'id': 'arrowLeft'
                    });
                    var $arrowR = $('<div>', {
                        'id': 'arrowRight'
                    });
                    $item.append($arrowL).append($menu).append($arrowR);
                });
                menuSlider.doKey = function (keyCode) {
                    var consumed = false;
                    switch (keyCode) {
                        case CONTROLS_CONFIG.KEY_LEFT:
                            that._focusCtrlIndex = (that._focusCtrlIndex === 0) ? 3 /* eON_DEMAND */ : that._focusCtrlIndex - 1;
                            CCarouselControl.prototype.doKey.call(this, keyCode);
                            consumed = that._contentsSlider.doKey(keyCode);
                            break;
                        case CONTROLS_CONFIG.KEY_RIGHT:
                            that._focusCtrlIndex = (that._focusCtrlIndex === 3 /* eON_DEMAND */) ? 0 /* eON_NOW */ : that._focusCtrlIndex + 1;
                            CCarouselControl.prototype.doKey.call(this, keyCode);
                            consumed = that._contentsSlider.doKey(keyCode);
                            break;
                        case CONTROLS_CONFIG.KEY_DOWN:
                            var focusedContentView = that._topPicksCtrls[that._focusCtrlIndex].getContentsView();
                            if (focusedContentView.$self().hasClass('noChannel') || focusedContentView.$self().hasClass('noOndemand')) {
                                return true;
                            }
                            break;
                        default: break;
                    }
                    return consumed;
                };
                that._menuSlider = menuSlider;
                return menuSlider;
            };
            CTopPicksPage.prototype.makeTopPicksCtrl = function () {
                var that = this;
                that._topPicksCtrls[3 /* eON_DEMAND */] = new COnDemandCtrl(that, that._dataAPI);
                that._topPicksCtrls[0 /* eON_NOW */] = new COnNowCtrl(that, that._dataAPI);
                that._topPicksCtrls[1 /* eNEXT */] = new CNextCtrls(that, that._dataAPI);
                that._topPicksCtrls[2 /* eRECORDINGS */] = new CRecordingsCtrls(that, that._dataAPI);
            };
            CTopPicksPage.prototype.makeContentsSlider = function () {
                var _this = this;
                var that = this;
                var contentsSlider = new CCarouselControl(null);
                contentsSlider.setId('contentsSlider');
                contentsSlider.setData(that._topPicksCtrls);
                contentsSlider.setDrawParam({
                    viewCount: 4,
                    anchorIndex: 1,
                    itemWidth: 897,
                    anchorWidth: 897,
                    transparentAnchor: false,
                    align: 'horizontal',
                    animation: false,
                    //animationInterval : 0.8,
                    maxKeyQueueCount: 0,
                    visibleAfterDraw: false
                });
                contentsSlider.setDataDrawer(function (param) {
                    var item = param.item;
                    var $item = param.$item;
                    $item.append(item.$self());
                    setTimeout(function () {
                        item.draw({});
                    }, 1);
                });
                contentsSlider.onFocusGained = function () {
                    var that = _this;
                    var topPicksCtrl = that._topPicksCtrls[that._focusCtrlIndex];
                    if (topPicksCtrl) {
                        topPicksCtrl.setActiveFocus(true);
                    }
                };
                contentsSlider.onFocusLost = function () {
                    var that = _this;
                    var topPicksCtrl = that._topPicksCtrls[that._focusCtrlIndex];
                    if (topPicksCtrl) {
                        topPicksCtrl.setActiveFocus(false);
                    }
                };
                contentsSlider.doKey = function (keyCode) {
                    var consumed = false;
                    switch (keyCode) {
                        case CONTROLS_CONFIG.KEY_LEFT:
                        case CONTROLS_CONFIG.KEY_RIGHT:
                            if (that._dataView.getFocusedChildIndex() === 0) {
                                consumed = CCarouselControl.prototype.doKey.call(this, keyCode);
                            }
                            else {
                                consumed = that._topPicksCtrls[that._focusCtrlIndex].doKey(keyCode);
                            }
                            break;
                        case CONTROLS_CONFIG.KEY_UP:
                        case CONTROLS_CONFIG.KEY_DOWN:
                            consumed = that._topPicksCtrls[that._focusCtrlIndex].doKey(keyCode);
                            break;
                        case CONTROLS_CONFIG.KEY_BACK:
                            var topPicksCtrl = that._topPicksCtrls[that._focusCtrlIndex];
                            var index = topPicksCtrl.getFocusedChildIndex();
                            if (index == 0) {
                                topPicksCtrl.doKey(keyCode);
                                that._dataView.setFocusedChld(that._menuSlider);
                            }
                            else {
                                if (topPicksCtrl.getCategoryCtrl) {
                                    topPicksCtrl.setFocusedChld(topPicksCtrl.getCategoryCtrl());
                                }
                            }
                            consumed = true;
                            break;
                        case CONTROLS_CONFIG.KEY_PAGEUP:
                        case CONTROLS_CONFIG.KEY_PAGEDOWN:
                            consumed = that._topPicksCtrls[that._focusCtrlIndex].doKey(keyCode);
                            break;
                        default: break;
                    }
                    return consumed;
                };
                contentsSlider.doKeyOption = function () {
                    return that._topPicksCtrls[that._focusCtrlIndex].doKeyOption();
                };
                contentsSlider.doKeyOk = function () {
                    return that._topPicksCtrls[that._focusCtrlIndex].doKeyOk();
                };
                contentsSlider.doKeyRec = function () {
                    return that._topPicksCtrls[that._focusCtrlIndex].doKeyRec();
                };
                that._contentsSlider = contentsSlider;
                return contentsSlider;
            };
            CTopPicksPage.prototype.draw = function (param) {
                var that = this;
                if (!that.$layer()) {
                    that.createLayer({
                        fnAfterCreate: function () {
                            // draw logo
                            var $logo = $('<div>', {
                                'id': 'freeviewLogo'
                            });
                            var $line = $('<div>', {
                                'id': 'line'
                            });
                            that.$layer().append($logo).append($line);
                            var menuSlider = that.makeMenuSlider();
                            that.makeTopPicksCtrl();
                            var contentsSlider = that.makeContentsSlider();
                            var view = new CVerticalGroupControl(null);
                            view.setChildControl([menuSlider, contentsSlider], 0);
                            that._dataView = view;
                            that.setChildControl([view], 0);
                            that.draw({
                                setActiveFocus: true
                            });
                            if (that._startIndex === 3 /* eON_DEMAND */) {
                                menuSlider.doKey(CONTROLS_CONFIG.KEY_LEFT);
                            }
                            else if (that._startIndex === 2 /* eRECORDINGS */) {
                                menuSlider.doKey(CONTROLS_CONFIG.KEY_RIGHT);
                            }
                            else {
                            }
                        }
                    });
                }
                else {
                    _super.prototype.draw.call(that, param);
                }
            };
            CTopPicksPage.prototype.doKeyOk = function () {
                var that = this;
                var consumed = false;
                if (that.getFocusedChild() === that._dataView) {
                    if (that._dataView.getFocusedChildIndex() === 1) {
                        consumed = that._topPicksCtrls[that._focusCtrlIndex].doKeyOk();
                    }
                }
                return consumed;
            };
            CTopPicksPage.prototype.doKeyRec = function () {
                var that = this;
                var consumed = false;
                if (that.getFocusedChild() === that._dataView) {
                    if (that._dataView.getFocusedChildIndex() === 1) {
                        consumed = that._topPicksCtrls[that._focusCtrlIndex].doKeyRec();
                    }
                }
                return consumed;
            };
            CTopPicksPage.prototype.doKeyStop = function () {
                var that = this;
                var consumed = false;
                if (that.getFocusedChild() === that._dataView) {
                    if (that._dataView.getFocusedChildIndex() === 1) {
                        consumed = that._topPicksCtrls[that._focusCtrlIndex].doKeyStop();
                    }
                }
                return consumed;
            };
            CTopPicksPage.prototype.doKeyOption = function () {
                var that = this;
                var consumed = false;
                if (that.getFocusedChild() === that._dataView) {
                    if (that._dataView.getFocusedChildIndex() === 1) {
                        consumed = that._topPicksCtrls[that._focusCtrlIndex].doKeyOption();
                    }
                }
                return consumed;
            };
            CTopPicksPage.prototype.getStartIndex = function () {
                return this._startIndex;
            };
            CTopPicksPage.prototype.getFocusCtrlIndex = function () {
                return this._focusCtrlIndex;
            };
            CTopPicksPage.prototype.getEnumTopPicks = function () {
                return ETopPicksCtrl;
            };
            CTopPicksPage.prototype.updateItemByEvent = function (targetType, eventItem, comparison) {
                var targetProvider = this._topPicksCtrls[targetType].getContentsCtrl().getDataProvider();
                if (targetProvider) {
                    var key = (function (aList, aItem) {
                        var i, ti, index = -1;
                        for (i = 0, ti = aList.getCount(); i < ti; i += 1) {
                            if (aList.getItem(i).getProgrammeID) {
                                if (aList.getItem(i).getProgrammeID() === comparison) {
                                    index = i;
                                    break;
                                }
                            }
                            if (aList.getItem(i).getId) {
                                if (aList.getItem(i).getId() === comparison) {
                                    index = i;
                                    break;
                                }
                            }
                        }
                        return index;
                    })(targetProvider, eventItem);
                    if (key !== -1) {
                        targetProvider.updateItem(key, (targetType === 2 /* eRECORDINGS */) ? eventItem : targetProvider.getItem(key));
                    }
                }
            };
            //onRecordingItemUpdated(aItem) {
            //    this.updateItemByEvent(ETopPicksCtrl.eRECORDINGS, aItem);
            //}
            CTopPicksPage.prototype.onRecordingItemAdded = function (aItem) {
                this.updateItemByEvent(0 /* eON_NOW */, aItem, aItem.getChannelId());
            };
            CTopPicksPage.prototype.onRecordingItemRemoved = function (aItem) {
                this.updateItemByEvent(0 /* eON_NOW */, aItem, aItem.getChannelId());
            };
            CTopPicksPage.prototype.onScheduledItemAdded = function (aItem) {
                this.updateItemByEvent(1 /* eNEXT */, aItem, aItem.getProgrammeID());
            };
            CTopPicksPage.prototype.onScheduledItemRemoved = function (aItem) {
                this.updateItemByEvent(1 /* eNEXT */, aItem, aItem.getProgrammeID());
            };
            return CTopPicksPage;
        })(__PRISM_CPAGE__.CPage);
        if (!window.prism) {
            window.prism = {};
        }
        prism.createTopPicksPage = function ($parent, startPath) {
            var $page = $('<div>', {
                'class': 'prism-page'
            });
            $parent.append($page);
            var page = new CTopPicksPage($page, startPath);
            return page;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
