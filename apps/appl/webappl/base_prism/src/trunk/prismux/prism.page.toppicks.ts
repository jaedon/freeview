/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />

declare var hx: ifSugar;
import __MODEL__ = require('model/model');
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CPAGE__ = require('./prism.page');

module mPrism {
    enum ETopPicksCtrl {
        eON_NOW = 0,
        eNEXT,
        eRECORDINGS,
        eON_DEMAND,
        eMAX
    }
    var TTopPicksIndexByPath = {
        '/onnow' : ETopPicksCtrl.eON_NOW,
        '/next' : ETopPicksCtrl.eNEXT,
        '/recordings' : ETopPicksCtrl.eRECORDINGS,
        '/ondemand' : ETopPicksCtrl.eON_DEMAND
    };
    enum ERecordings {
        eRECENTLY = 0,
        eCONTINUE,
        eMAX
    }
    enum ENext {
        eDONTMISSIT = 0,
        ePOPULARTVSERIES,
        eMAX
    }

    class CTopPicksCtrl extends __CONTROLS__.CVerticalGroupControl {
        _categoryCtrl;
        _contentsCtrl;
        _contentsView;
        _selectedIndex;
        _dataAPI;
        _parent;
        _$noData;
        NORMAL_THUMBNAIL_WIDTH: number;
        NORMAL_CHANNEL_LOGO_WIDTH: number;

        constructor(parent, dataAPI, $el?) {
            var that = this;

            super();
            that._parent = parent;
            that._dataAPI = dataAPI;
            that.NORMAL_THUMBNAIL_WIDTH = 267;
            that.NORMAL_CHANNEL_LOGO_WIDTH = 60;
        }
        destroy() {
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
            super.destroy.call(that);
        }

        init($el) {
            var that = this;
            var ctrls;

            super.init.call(this, $el);
            that.setId('toppicksCtrl');

            ctrls = that.makeChildCtrls();
            if (ctrls) {
                that.setChildControl(ctrls);
            }
            if (that._contentsView) {
                that._contentsView.$self().append($('<div>', {
                    'class': 'title'    // for no data
                }));
                that._$noData = that._contentsView.$self().children('.title');
            }
        }

        makeChildCtrls(): Array<any> {
            throw "this function must override!!!";
        }

        getContentsCtrl() {
            return this._contentsCtrl;
        }

        getContentsView() {
            return this._contentsView;
        }
        doDraw(param?) {
            var that = this;
            if (!that.getFocusedChild()) {
                // initialising
                that.processAsyncData();
            }
            return super.doDraw.call(that, param);
        }
        doKeyOk() {
            var that = this;
            var consumed = false;
            var focusCtrl = that.getFocusedChild();
            if (focusCtrl && focusCtrl.doKeyOk) {
                consumed = focusCtrl.doKeyOk();
            }
            return consumed;
        }
        doKeyRec() {
            var that = this;
            var consumed = false;
            var focusCtrl = that.getFocusedChild();
            if (focusCtrl && (<any>focusCtrl).doKeyRec) {
                consumed = (<any>focusCtrl).doKeyRec();
            }
            return consumed;
        }
        doKeyStop() {
            var that = this;
            var consumed = false;
            var focusCtrl = that.getFocusedChild();
            if (focusCtrl && (<any>focusCtrl).doKeyStop) {
                consumed = (<any>focusCtrl).doKeyStop();
            }
            return consumed;
        }
        doKeyOption() {
            var that = this;
            var consumed = false;
            var focusCtrl = that.getFocusedChild();
            if (focusCtrl && (<any>focusCtrl).doKeyOption) {
                consumed = (<any>focusCtrl).doKeyOption();
            }
            return consumed;
        }
        processAsyncData() {
            // TODO : use this by overriding if needed
        }
    }
    class COnDemandCtrl extends CTopPicksCtrl {
        _ondemandReqId;
        _ondemandRequestTimeout;
        _noChannelHTML;
        _noInternetHTML;
        _notAvailableHTML;
        _processingHTML;

        constructor(parent, dataAPI, $el?) {
            super(parent, dataAPI, $el);
            var that = this;
            that._noChannelHTML = 'No channels are available.<div class=\'description\'>Go to Channel Settings to search for channels.</div>';
            that._noInternetHTML = 'Connect to the internet and agree to the Terms and Conditions on Settings before using service.';
            that._notAvailableHTML = 'It is not available at the moment.<div class=\'description\'>Please try again later.</div>';
            that._processingHTML = 'We\'re loading some great recommendations for you.<div class=\'description\'>Please wait.</div>';
            if (arguments.length) {
                that.init($el);
            }
        }

        destroy() {
            var that = this;
            super.destroy.call(that);
        }

        init($el) {
            var that = this;
            super.init.call(that, $el);
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
            that._dataAPI.getOnDemandCategories((categories) => {
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
            }, () => {
                that.drawWaitingView();
            });
        }

        processAsyncData() {
            var that = this;
            that._dataAPI.getOnDemandGroup(that._ondemandReqId, (groupId, contents) => {
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
                if ((that._parent.getStartIndex() === ETopPicksCtrl.eON_DEMAND) &&
                    (that._parent.getFocusCtrlIndex() === ETopPicksCtrl.eON_DEMAND)) {
                    that._parent.doKey(CONTROLS_CONFIG.KEY_DOWN);
                    that._parent.doKey(CONTROLS_CONFIG.KEY_DOWN);
                }
            });
        }

        drawWaitingView() {
            var that = this;
            that._$noData.html(that._processingHTML);
            that._contentsView.$self().addClass('notAvailable');
        }

        makeChildCtrls(): Array<any> {
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
                    that._ondemandRequestTimeout = setTimeout(() => {
                        that._contentsView.$self().css('opacity', '0');
                        that._dataAPI.getOnDemandGroup(groupId, (groupId, contents) => {
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
                    //that._dataAPI.showProgrammeDetail(item);
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
                        } else {
                            $thumbnail.css('background', 'url("images/614_OD_Default01.png") no-repeat center/100% 100%');
                            item.getThumbnail({
                                updateCb : (receivedUrl) => {
                                    if (receivedUrl) {
                                        item.thumbnailURL = receivedUrl;
                                        $thumbnail.css('background', 'url("' + item.thumbnailURL + '") no-repeat center/100% 100%');
                                    }
                                },
                                width : that.NORMAL_THUMBNAIL_WIDTH
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
                        } else {
                            item.getContentOwningLogo({
                                updateCb: (receivedUrl) => {
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
            contentsView.doKeyOk = function() {
                return this.getFocusedChild().doKeyOk();
            };
            contentsView.setId('contentsView');

            that._categoryCtrl = categoryCtrl;
            that._contentsCtrl = contentsCtrl;
            that._contentsView = contentsView;

            return [categoryCtrl, contentsView];
        }

        getOnDemandReqId() {
            return this._ondemandReqId;
        }

        getCategoryCtrl() {
            return this._categoryCtrl;
        }

        doKeyPageDown() {
            var that = this;
            if (that._contentsView.$self().hasClass('notAvailable')) {
                return true;
            }
            return false;
        }
        doKeyDown() {
            var that = this;
            if (that._contentsView.$self().hasClass('notAvailable')) {
                return true;
            }
            return false;
        }
    }
    class CRecordingsCtrls extends CTopPicksCtrl {
        _categoryCtrl;
        _selectedIndex;
        _noRecentlyHTML;
        _noWatchingHTML;

        constructor(parent, dataAPI, $el?) {
            super(parent, dataAPI, $el);
            var that = this;
            that._noRecentlyHTML = 'There are no new recordings.<div class=\'description\'>Go to TV Guide and set recording schedules for the programmes<br>you want to record.</div>';
            that._noWatchingHTML = 'There are no recordings to continue.';
            if (arguments.length) {
                that.init($el);
            }
        }

        destroy() {
            var that = this;
            super.destroy.call(that);
        }

        init($el) {
            var that = this;
            super.init.call(that, $el);
            that.addClass('recordings');
            that._selectedIndex = ERecordings.eRECENTLY;
            if (!that._contentsCtrl.getDataProvider().getCount()) {
                that._$noData.html(that._noRecentlyHTML);
                that._contentsView.$self().addClass('noRecording');
            }
        }

        makeChildCtrls(): Array<any> {
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
            categoryCtrl.onFocusChanged = ($old, $new) => {
                that._selectedIndex = $new.index();
                that._contentsView.$self().css('opacity', '0');
                $old.removeClass('-sel');
                $new.addClass('-sel');
                var dataList = null;
                var dataProvider = null;
                if (that._selectedIndex === ERecordings.eRECENTLY) {
                    dataList = that._dataAPI.getRecentlyRecordings();
                    if (dataList && dataList.length) {
                        that._contentsView.$self().removeClass('noRecording');
                        dataProvider = new CDataProvider(dataList);
                    } else {
                        that._$noData.html(that._noRecentlyHTML);
                        that._contentsView.$self().addClass('noRecording');
                        dataProvider = new CDataProvider([]);
                    }
                    that._contentsView.$self().removeClass('-continue');
                    that._contentsView.$self().addClass('-recently');
                } else if (that._selectedIndex === ERecordings.eCONTINUE) {
                    dataList = that._dataAPI.getContinueWatchRecordings();
                    if (dataList && dataList.length) {
                        that._contentsView.$self().removeClass('noRecording');
                        dataProvider = new CDataProvider(dataList);
                    } else {
                        that._$noData.html(that._noWatchingHTML);
                        that._contentsView.$self().addClass('noRecording');
                        dataProvider = new CDataProvider([]);
                    }
                    that._contentsView.$self().removeClass('-recently');
                    that._contentsView.$self().addClass('-continue');
                } else {
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
                        that._dataAPI.seeMore(ETopPicksCtrl.eRECORDINGS);
                    } else {
                        that._dataAPI.requestPlayback(item);
                    }
                    //that._dataAPI.showProgrammeDetail(item);
                }
            };
            contentsCtrl.setDataDrawer(function (param) {
                var $item = param.$item;
                var item = param.item;
                var $thumbnail, $title, $secondary, $durationBar, $channelLogo, $icon;

                if (item && item.type == 'more') {
                    $item.addClass('contentMore');
                    $item.text('SEE ALL RECORDINGS');
                } else {
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
                            } else if (item.hasGuidance()) {
                                $icon.addClass('icoGuidance');
                            } else if (item.isRecordingOnGoing()) {
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
                        if (that._selectedIndex === ERecordings.eCONTINUE) {
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
                                    'class' : 'logo',
                                    'src' : item.logoURL,
                                    'alt' : ''
                                }));
                            } else {
                                that._dataAPI.getChannelLogo(item.getChannelId(), that.NORMAL_CHANNEL_LOGO_WIDTH, (receivedUrl) => {
                                    if (receivedUrl) {
                                        item.logoURL = receivedUrl;
                                        $channelLogo.append($('<img>', {
                                            'class' : 'logo',
                                            'src' : item.logoURL,
                                            'alt' : ''
                                        }));
                                    } else {
                                        $channelLogo.text((item.getChannelName() || '').toUpperCase());
                                    }
                                });
                            }
                        }
                    }
                }
            });

            contentsView.setChildControl([contentsCtrl]);
            contentsView.doKeyOk = function() {
                return this.getFocusedChild().doKeyOk();
            };
            contentsView.setId('contentsView');
            contentsView.addClass('-recently');

            that._categoryCtrl = categoryCtrl;
            that._contentsCtrl = contentsCtrl;
            that._contentsView = contentsView;

            return [categoryCtrl, contentsView];
        }

        getCategoryCtrl() {
            return this._categoryCtrl;
        }

        doKeyPageDown() {
            var that = this;
            if (that._contentsView.$self().hasClass('noRecording')) {
                return true;
            }
            return false;
        }
        doKeyDown() {
            var that = this;
            if (that._contentsView.$self().hasClass('noRecording')) {
                return true;
            }
            return false;
        }
    }

    class CNextCtrls extends CTopPicksCtrl {
        _categoryCtrl;
        _noNextHTML;
        _noChannelHTML;
        _noInternetHTML;
        _notAvailableHTML;
        _processingHTML;

        constructor(parent, dataAPI, $el?) {
            super(parent, dataAPI, $el);
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

        destroy() {
            var that = this;
            super.destroy.call(that);
        }

        init($el) {
            var that = this;
            super.init.call(that, $el);
            that.addClass('next');
            that._selectedIndex = ENext.eDONTMISSIT;
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
        }

        processAsyncData() {
            var that = this;
            that._dataAPI.getHumaxRecommendation(__MODEL__.EHumaxRecommendationType.EDontMissIt, (contents, type) => {
                that.drawContentView(contents, type);
            }, (type) => {
                that.drawWaitingView(type);
            });
            that._dataAPI.getHumaxRecommendation(__MODEL__.EHumaxRecommendationType.EPopularTvSeries, (contents, type) => {
                that.drawContentView(contents, type);
            }, (type) => {
                that.drawWaitingView(type);
            });
        }

        drawContentView(aContents, aType) {
            var that = this;
            var targetType = __MODEL__.EHumaxRecommendationType.ENone;
            switch (that._selectedIndex) {
                case ENext.eDONTMISSIT:
                    targetType = __MODEL__.EHumaxRecommendationType.EDontMissIt;
                    break;
                case ENext.ePOPULARTVSERIES:
                    targetType = __MODEL__.EHumaxRecommendationType.EPopularTvSeries;
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
        }

        drawWaitingView(aType) {
            var that = this;
            var targetType = __MODEL__.EHumaxRecommendationType.ENone;
            switch (that._selectedIndex) {
                case ENext.eDONTMISSIT:
                    targetType = __MODEL__.EHumaxRecommendationType.EDontMissIt;
                    break;
                case ENext.ePOPULARTVSERIES:
                    targetType = __MODEL__.EHumaxRecommendationType.EPopularTvSeries;
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
    }

        makeChildCtrls(): Array<any> {
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
            categoryCtrl.onFocusChanged = ($old, $new) => {
                that._selectedIndex = $new.index();
                that._contentsCtrl.clearDrawnItems();
                $old.removeClass('-sel');
                $new.addClass('-sel');
                var targetType = __MODEL__.EHumaxRecommendationType.ENone;
                switch (that._selectedIndex) {
                    case ENext.eDONTMISSIT:
                        targetType = __MODEL__.EHumaxRecommendationType.EDontMissIt;
                        break;
                    case ENext.ePOPULARTVSERIES:
                        targetType = __MODEL__.EHumaxRecommendationType.EPopularTvSeries;
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
                if (targetType === __MODEL__.EHumaxRecommendationType.ENone) {
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
                that._dataAPI.getHumaxRecommendation(targetType, (contents, type) => {
                    that.drawContentView(contents, type);
                }, (type) => {
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
                } else {
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
                            } else if (item.hasReminder()) {
                                $icon.addClass('icoRemind');
                            } else if (item.hasSeriesSchedule()) {
                                $icon.addClass('icoSeries');
                            } else if (item.hasGuidance()) {
                                $icon.addClass('icoGuidance');
                            }
                        }
                        $thumbnail = $item.children('#thumbnail');
                        if (item.thumbnail) {
                            $thumbnail.css('background', 'url("' + item.thumbnail + '") no-repeat center/100% 100%');
                        } else {
                            that._dataAPI.getThumbnail(item, that.NORMAL_THUMBNAIL_WIDTH, (receivedUrl) => {
                                $thumbnail.css('background', 'url("' + (item.thumbnail = (receivedUrl || __MODEL__.TDefaultThumbnailList.DEFAULT)) + '") no-repeat center/100% 100%');
                            });
                        }
                        $channelLogo = $item.children('#channelLogo');
                        if ($channelLogo) {
                            if (item.logoURL) {
                                $channelLogo.append($('<img>', {
                                    'class' : 'logo',
                                    'src' : item.logoURL,
                                    'alt' : ''
                                }));
                            } else {
                                that._dataAPI.getChannelLogo(item.getChannelId(), that.NORMAL_CHANNEL_LOGO_WIDTH, (receivedUrl) => {
                                    if (receivedUrl) {
                                        item.logoURL = receivedUrl;
                                        $channelLogo.append($('<img>', {
                                            'class' : 'logo',
                                            'src' : item.logoURL,
                                            'alt' : ''
                                        }));
                                    } else {
                                        $channelLogo.text((item.getChannelName() || '').toUpperCase());
                                    }
                                });
                            }
                        }
                    }
                }
            });

            contentsView.setChildControl([contentsCtrl]);
            contentsView.doKeyOk = function() {
                return this.getFocusedChild().doKeyOk();
            };
            contentsView.doKeyRec = function() {
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
        }

        getCategoryCtrl() {
            return this._categoryCtrl;
        }

        doKeyPageDown() {
            if (this._contentsView.$self().hasClass('notAvailable')) {
                return true;
            }
            return false;
        }

        doKeyDown() {
            if (this._contentsView.$self().hasClass('notAvailable')) {
                return true;
            }
            return false;
        }
    }

    class COnNowCtrl extends CTopPicksCtrl {
        _$edit: JQuery;
        _editDlg;
        _onNowChannels;
        _channels;
        _noChannelHTML;
        ON_NOW_MAX: number;

        constructor(parent, dataAPI, $el?) {
            super(parent, dataAPI, $el);
            var that = this;
            that._noChannelHTML = 'No channels are available.<div class=\'description\'>Go to Channel Settings to search for channels.</div>';
            that.ON_NOW_MAX = 9;
            if (arguments.length) {
                that.init($el);
            }
        }

        destroy() {
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
            super.destroy.call(that);
        }

        init($el) {
            var that = this;
            that._onNowChannels = that._dataAPI.getOnNowChannels();
            that._channels = that._dataAPI.getChannels();
            super.init.call(that, $el);
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
        }
        makeChildCtrls(): Array<any> {
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
                that._dataAPI.searchOnNowEPGByCcid(ccid, (param) => {
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
                        } else if (isChannelLock) {
                            $icon.addClass('icoLocked');
                        } else if (onNowEvent.hasGuidance()) {
                            $icon.addClass('icoGuidance');
                        }
                        if (isChannelLock) {
                            if (that._onNowChannels[focusedIndex].isRadio()) {
                                $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.RADIO + '") no-repeat center/100% 100%');
                            } else {
                                $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.TV + '") no-repeat center/100% 100%');
                            }
                        } else {
                            fnGetThumbnail(onNowEvent, that.NORMAL_THUMBNAIL_WIDTH, (receivedUrl) => {
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
                                that._dataAPI.getChannelLogo(onNowEvent.getChannelId(), that.NORMAL_CHANNEL_LOGO_WIDTH, (receivedUrl) => {
                                    var $channelLogo = $item.children('#channelLogo');
                                    if (receivedUrl) {
                                        $channelLogo.append($('<img>', {
                                            'class' : 'logo',
                                            'src' : receivedUrl,
                                            'alt' : ''
                                        }));
                                    } else {
                                        $channelLogo.text((that._onNowChannels[focusedIndex].getName()).toUpperCase());
                                    }
                                });
                            }
                        }
                    } else {
                        var $thumbnail = $item.children('#thumbnail');
                        if (isChannelLock) {
                            $item.children('#icon').addClass('icoLocked');
                        }
                        if (that._onNowChannels[focusedIndex].isRadio()) {
                            $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.RADIO + '") no-repeat center/100% 100%');
                        } else {
                            $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.TV + '") no-repeat center/100% 100%');
                        }
                    }
                });
                //request channel logo
                that._dataAPI.getChannelLogo(ccid, that.NORMAL_CHANNEL_LOGO_WIDTH, (receivedUrl) => {
                    var $channelLogo = $item.children('#channelLogo');
                    if (receivedUrl) {
                        $channelLogo.append($('<img>', {
                            'class' : 'logo',
                            'src' : receivedUrl,
                            'alt' : ''
                        }));
                    } else {
                        $channelLogo.text((that._onNowChannels[focusedIndex].getName()).toUpperCase());
                    }
                });
            });
            contentsCtrl.setActiveFocus = function (activeFocus) {
                if (activeFocus) {
                    contentsCtrl.$self().addClass('-afd');
                    that._$edit.addClass('-show');
                } else {
                    contentsCtrl.$self().removeClass('-afd');
                    that._$edit.removeClass('-show');
                }
                CGridControl.prototype.setActiveFocus.call(this, activeFocus);
            };
            contentsCtrl.isFocusable = function() {
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
            contentsView.doKeyOption = function() {
                that._editDlg = prism.createListDlg(that._parent, {
                    groupItems : that._channels,
                    textHtml : 'Replace the selected channel.',
                    cls : 'chGrid'
                }, function(param) {
                    that.updateOnNowChannel(param.id);
                });
                that._editDlg.open({
                    curGroupIndex: that.findFocusIndex(),
                    itemWidth : 360,
                    itemHeight : 40,
                    openClass : 'grid',
                    dimm : true,
                    onClosed: function(param) {
                        that._editDlg = null;
                    }
                });
                that._editDlg.doKeyEscape = function() {
                    return that._editDlg.doKeyBack();
                };
                return true;
            };
            contentsView.doKeyOk = function() {
                return this.getFocusedChild().doKeyOk();
            };
            contentsView.doKeyRec = function() {
                var bConsumed = false;
                var targetItem = that._contentsCtrl.getFocusedItem();
                if (targetItem && targetItem.item) {
                    bConsumed = that._dataAPI.requestRecord(targetItem.item.getId());
                }
                return bConsumed;
            };
            contentsView.doKeyStop = function() {
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
        }
        findFocusIndex() {
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
        }
        updateOnNowChannel(ccid) {
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
            that._dataAPI.searchOnNowEPGByCcid(ccid, (param) => {
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
                        } else {
                            $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.TV + '") no-repeat center/100% 100%');
                        }
                    } else {
                        $icon.removeClass('icoLocked');
                        fnGetThumbnail(onNowEvent, that.NORMAL_THUMBNAIL_WIDTH, (receivedUrl) => {
                            if (receivedUrl) {
                                $thumbnail.css('background', 'url("' + receivedUrl + '") no-repeat center/100% 100%');
                            }
                            if (onNowEvent.getSecondary()) {
                                $secondary.text(onNowEvent.getSecondary());
                            }
                        });
                    }
                } else {
                    $title.text('');
                    $secondary.text('');
                    $durationBar.width('0%');
                    if (onNowChannel && onNowChannel.manualBlock) {
                        $item.children('#icon').addClass('icoLocked');
                    } else {
                        $item.children('#icon').removeClass('icoLocked');
                    }
                    if (that._onNowChannels[focusedIndex].isRadio()) {
                        $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.RADIO + '") no-repeat center/100% 100%');
                    } else {
                        $thumbnail.css('background', 'url("' + __MODEL__.TDefaultThumbnailList.TV + '") no-repeat center/100% 100%');
                    }
                }

            });
            //request channel logo
            that._dataAPI.getChannelLogo(ccid, that.NORMAL_CHANNEL_LOGO_WIDTH, (receivedUrl) => {
                var $channelLogo = $item.children('#channelLogo');
                $channelLogo.html('');
                if (receivedUrl) {
                    $channelLogo.append($('<img>', {
                        'class' : 'logo',
                        'src' : receivedUrl,
                        'alt' : ''
                    }));
                } else {
                    $channelLogo.text((that._onNowChannels[focusedIndex].getName()).toUpperCase());
                }
            });
        }
        setChannel(targetChannel) {
            if (!targetChannel) {
                hx.log('debug', '[toppicks][setChannel] Fail!! cos !targetChannel');
                return;
            }
            this._dataAPI.setChannelByCcid(targetChannel.getId());
        }
        doKeyPageDown() {
            var that = this;
            if (that._contentsView.$self().hasClass('noChannel')) {
                return true;
            }
            return false;
        }
        doKeyDown() {
            var that = this;
            if (that._contentsView.$self().hasClass('noChannel')) {
                return true;
            }
            return false;
        }
    }

    class CTopPicksPage extends __PRISM_CPAGE__.CPage {
        _dataView;
        _menuSlider;
        _contentsSlider;
        _topPicksCtrls: Array<any>;
        _focusCtrlIndex;
        _dataAPI;
        _startIndex;

        constructor($el, startPath?) {
            super($el);
            this._startIndex = TTopPicksIndexByPath[startPath] || ETopPicksCtrl.eON_NOW;
        }

        destroy() {
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
            super.destroy.call(that);
        }

        init($el) {
            var that = this;

            super.init.call(that, $el);
            that.setId('toppicksPage');

            that._topPicksCtrls = new Array(ETopPicksCtrl.eMAX);
            that._focusCtrlIndex = ETopPicksCtrl.eON_NOW;
        }

        setDataAPI(dataAPI) {
            var that = this;
            that._dataAPI = dataAPI;
        }

        makeMenuSlider() {
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
            menuSlider.setDataDrawer(function(param) {
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
                //if (that._contentsSlider._onTransition) {
                //    return true;
                //}
                switch(keyCode) {
                    case CONTROLS_CONFIG.KEY_LEFT:
                        that._focusCtrlIndex = (that._focusCtrlIndex === 0) ? ETopPicksCtrl.eON_DEMAND : that._focusCtrlIndex - 1;
                        CCarouselControl.prototype.doKey.call(this, keyCode);
                        consumed = that._contentsSlider.doKey(keyCode);
                        break;
                    case CONTROLS_CONFIG.KEY_RIGHT:
                        that._focusCtrlIndex = (that._focusCtrlIndex === ETopPicksCtrl.eON_DEMAND) ? ETopPicksCtrl.eON_NOW : that._focusCtrlIndex + 1;
                        CCarouselControl.prototype.doKey.call(this, keyCode);
                        consumed = that._contentsSlider.doKey(keyCode);
                        break;
                    case CONTROLS_CONFIG.KEY_DOWN:
                        var focusedContentView = that._topPicksCtrls[that._focusCtrlIndex].getContentsView();
                        if (focusedContentView.$self().hasClass('noChannel') ||
                            focusedContentView.$self().hasClass('noOndemand')) {
                            return true;
                        }
                        break;
                    default: break;
                }
                return consumed;
            }
            that._menuSlider = menuSlider;

            return menuSlider;
        }
        makeTopPicksCtrl() {
            var that = this;
            that._topPicksCtrls[ETopPicksCtrl.eON_DEMAND] = new COnDemandCtrl(that, that._dataAPI);
            that._topPicksCtrls[ETopPicksCtrl.eON_NOW] = new COnNowCtrl(that, that._dataAPI);
            that._topPicksCtrls[ETopPicksCtrl.eNEXT] = new CNextCtrls(that, that._dataAPI);
            that._topPicksCtrls[ETopPicksCtrl.eRECORDINGS] = new CRecordingsCtrls(that, that._dataAPI);
        }
        makeContentsSlider() {
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

            contentsSlider.setDataDrawer((param) => {
                var item = param.item;
                var $item = param.$item;
                $item.append(item.$self());
                setTimeout(() => {
                    item.draw({});
                }, 1);
            });

            (<any>contentsSlider).onFocusGained = () => {
                var that = this;
                var topPicksCtrl = that._topPicksCtrls[that._focusCtrlIndex];
                if (topPicksCtrl) {
                    topPicksCtrl.setActiveFocus(true);
                }
            };
            (<any>contentsSlider).onFocusLost = () => {
                var that = this;
                var topPicksCtrl = that._topPicksCtrls[that._focusCtrlIndex];
                if (topPicksCtrl) {
                    topPicksCtrl.setActiveFocus(false);
                }
            };

            contentsSlider.doKey = function (keyCode) {
                var consumed = false;

                switch(keyCode) {
                    case CONTROLS_CONFIG.KEY_LEFT:
                    case CONTROLS_CONFIG.KEY_RIGHT:
                        if (that._dataView.getFocusedChildIndex() === 0) {
                            consumed = CCarouselControl.prototype.doKey.call(this, keyCode);
                        } else {
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
                        } else {
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
            contentsSlider.doKeyOption = function() {
                return that._topPicksCtrls[that._focusCtrlIndex].doKeyOption();
            };
            contentsSlider.doKeyOk = function() {
                return that._topPicksCtrls[that._focusCtrlIndex].doKeyOk();
            };
            contentsSlider.doKeyRec = function() {
                return that._topPicksCtrls[that._focusCtrlIndex].doKeyRec();
            };

            that._contentsSlider = contentsSlider;

            return contentsSlider;
        }
        draw(param?) {
            var that = this;
            if (!that.$layer()) {
                that.createLayer({
                    fnAfterCreate: function() {
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
                        if (that._startIndex === ETopPicksCtrl.eON_DEMAND) {
                            menuSlider.doKey(CONTROLS_CONFIG.KEY_LEFT);
                        } else if (that._startIndex === ETopPicksCtrl.eRECORDINGS) {
                            menuSlider.doKey(CONTROLS_CONFIG.KEY_RIGHT);
                        } else {
                            // keep the first page (ON NOW)
                        }
                    }
                });
            } else {
                super.draw.call(that, param);
            }
        }
        doKeyOk() {
            var that = this;
            var consumed = false;
            if (that.getFocusedChild() === that._dataView) {
                if (that._dataView.getFocusedChildIndex() === 1) {
                    consumed = that._topPicksCtrls[that._focusCtrlIndex].doKeyOk();
                }
            }
            return consumed;
        }
        doKeyRec() {
            var that = this;
            var consumed = false;
            if (that.getFocusedChild() === that._dataView) {
                if (that._dataView.getFocusedChildIndex() === 1) {
                    consumed = that._topPicksCtrls[that._focusCtrlIndex].doKeyRec();
                }
            }
            return consumed;
        }
        doKeyStop() {
            var that = this;
            var consumed = false;
            if (that.getFocusedChild() === that._dataView) {
                if (that._dataView.getFocusedChildIndex() === 1) {
                    consumed = that._topPicksCtrls[that._focusCtrlIndex].doKeyStop();
                }
            }
            return consumed;
        }
        doKeyOption() {
            var that = this;
            var consumed = false;
            if (that.getFocusedChild() === that._dataView) {
                if (that._dataView.getFocusedChildIndex() === 1) {
                    consumed = that._topPicksCtrls[that._focusCtrlIndex].doKeyOption();
                }
            }
            return consumed;
        }
        getStartIndex() {
            return this._startIndex;
        }
        getFocusCtrlIndex() {
            return this._focusCtrlIndex;
        }
        getEnumTopPicks() {
            return ETopPicksCtrl;
        }
        updateItemByEvent(targetType, eventItem, comparison) {
            var targetProvider = this._topPicksCtrls[targetType].getContentsCtrl().getDataProvider();
            if (targetProvider) {
                var key = (function(aList, aItem) {
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
                    targetProvider.updateItem(key, (targetType === ETopPicksCtrl.eRECORDINGS) ? eventItem : targetProvider.getItem(key));
                }
            }
        }
        //onRecordingItemUpdated(aItem) {
        //    this.updateItemByEvent(ETopPicksCtrl.eRECORDINGS, aItem);
        //}
        onRecordingItemAdded(aItem) {
            this.updateItemByEvent(ETopPicksCtrl.eON_NOW, aItem, aItem.getChannelId());
        }
        onRecordingItemRemoved(aItem) {
            this.updateItemByEvent(ETopPicksCtrl.eON_NOW, aItem, aItem.getChannelId());
        }
        onScheduledItemAdded(aItem) {
            this.updateItemByEvent(ETopPicksCtrl.eNEXT, aItem, aItem.getProgrammeID());
        }
        onScheduledItemRemoved(aItem) {
            this.updateItemByEvent(ETopPicksCtrl.eNEXT, aItem, aItem.getProgrammeID());
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createTopPicksPage = function($parent, startPath) {
        var $page = $('<div>', {
            'class': 'prism-page'
        });
        $parent.append($page);
        var page = new CTopPicksPage($page, startPath);
        return page;
    };
}
export = mPrism;

