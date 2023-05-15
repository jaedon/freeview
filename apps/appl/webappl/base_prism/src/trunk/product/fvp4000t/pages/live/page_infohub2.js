var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "model/model", 'pages/util/controls'], function (require, exports, __PAGE_KERNEL__, __MODEL__, __CONTROLS__) {
    /// <reference path="../../../../def/sugar.d.ts" />
    /// <reference path="../../../../def/controls.d.ts" />
    ///<reference path="../../../../layer_domain/service/ip_epg/engine/interface_freeview_play.d.ts" />
    ///<reference path="../../../../layer_domain/service/ip_epg/adapter.d.ts" />
    var state = 'infoHub';
    var focusedChannel = null;
    var focusedProgramme = null;
    var createInfoHubIfNecessary = function (a) {
        if (!a._$infoHubPane) {
            var $infoHubPane = $('<div>', {
                'id': 'entryPane'
            }), $arrowUp = $('<div>', {
                'class': 'arrowUp'
            }).appendTo($infoHubPane), $highlight = $('<div>', {
                'class': 'highlight'
            }).appendTo($infoHubPane), $arrowRight = $('<div>', {
                'class': 'arrowRight'
            }).appendTo($highlight), $channelItem = $('<div>', {
                'class': 'chItem'
            }).appendTo($highlight), $channelNum = $('<div>', {
                'class': 'chNum'
            }).appendTo($channelItem), $channelName = $('<div>', {
                'class': 'chName'
            }).appendTo($channelItem), $channelItemDummy = $('<div>', {
                'class': 'chItem'
            }).appendTo($infoHubPane), $programmeName = $('<div>', {
                'class': 'progName'
            }).appendTo($infoHubPane), $programmeProgress = $('<div>', {
                'class': 'progProgress'
            }).appendTo($infoHubPane), $programmeStartTime = $('<div>', {
                'class': 'progStartTime'
            }).appendTo($programmeProgress), $programmeEndTime = $('<div>', {
                'class': 'progEndTime'
            }).appendTo($programmeProgress), $programmeBar = $('<div>', {
                'class': 'progBar'
            }).appendTo($programmeProgress), $programmePos = $('<div>', {
                'class': 'progPos'
            }).appendTo($programmeBar), $arrowDown = $('<div>', {
                'class': 'arrowDown'
            }).appendTo($infoHubPane);
            $infoHubPane.$arrows = $arrowUp.add($arrowDown).add($arrowRight);
            $infoHubPane.$channelNum = $channelNum;
            $infoHubPane.$channelName = $channelName;
            $infoHubPane.$programmeProgress = $programmeProgress;
            $infoHubPane.$programmeName = $programmeName;
            $infoHubPane.$programmeStartTime = $programmeStartTime;
            $infoHubPane.$programmeEndTime = $programmeEndTime;
            $infoHubPane.$programmePos = $programmePos;
            $infoHubPane.$programmeBar = $programmeBar;
            $infoHubPane.$highlight = $highlight;
            a._$infoHubPane = $infoHubPane;
        }
        a._$self.append(a._$infoHubPane);
    };
    var updateEntryPane = function (a) {
        if (a._$infoHubPane !== undefined) {
            if (state === 'channelList') {
                a._$infoHubPane.$programmeName.detach();
                a._$infoHubPane.$programmeProgress.detach();
                a._$infoHubPane.$arrows.removeClass('show');
                a._$infoHubPane.$highlight.removeClass('on');
            }
            else if (state === 'iPlate') {
                if (!a._$infoHubPane.find('.progName').length) {
                    a._$infoHubPane.$programmeName[0].innerHTML = '';
                    a._$infoHubPane.append(a._$infoHubPane.$programmeName);
                }
                if (!a._$infoHubPane.find('.progProgress').length) {
                    //a._$infoHubPane.$programmeProgress[0].innerHTML = '';
                    a._$infoHubPane.append(a._$infoHubPane.$programmeProgress);
                }
                a._$infoHubPane.$arrows.removeClass('show');
            }
            else {
                if (!a._$infoHubPane.find('.progName').length) {
                    a._$infoHubPane.append(a._$infoHubPane.$programmeName);
                }
                if (!a._$infoHubPane.find('.progProgress').length) {
                    a._$infoHubPane.append(a._$infoHubPane.$programmeProgress);
                }
                a._$infoHubPane.$arrows.addClass('show');
                a._$infoHubPane.$highlight.addClass('on');
            }
        }
    };
    var updateCurrentChannel = function (a) {
        if (a._currentChannel) {
            var channelNum = hx.il.makeDigitNumber(a._currentChannel.majorChannel);
            a._$infoHubPane.$channelNum.text(channelNum);
            a._$infoHubPane.$channelName.text(a._currentChannel.name);
        }
    };
    var checkChannelsChangeable = function (viewingItemInfos) {
        var i, channelChangable;
        viewingItemInfos = viewingItemInfos instanceof Array ? viewingItemInfos : [viewingItemInfos];
        for (i = 0; i < viewingItemInfos.length; i += 1) {
            var viewingItemInfo = viewingItemInfos[i];
            if (viewingItemInfo) {
                channelChangable = hx.svc.CH_LIST.checkChannelChangable({
                    channel: viewingItemInfo.item
                });
                if (viewingItemInfo.$item) {
                    if (channelChangable) {
                        viewingItemInfo.$item.removeClass('dis');
                    }
                    else {
                        viewingItemInfo.$item.addClass('dis');
                    }
                }
            }
        }
    };
    var $recIcon = $('<div>', {
        'class': 'chIconRec'
    });
    var TScheduledFactoryType;
    (function (TScheduledFactoryType) {
        TScheduledFactoryType[TScheduledFactoryType["ERECORDING"] = 0] = "ERECORDING";
        TScheduledFactoryType[TScheduledFactoryType["EREMIND"] = 1] = "EREMIND";
        TScheduledFactoryType[TScheduledFactoryType["ESERIES_RECORDING"] = 10] = "ESERIES_RECORDING";
    })(TScheduledFactoryType || (TScheduledFactoryType = {}));
    var CMainGroupControl = (function (_super) {
        __extends(CMainGroupControl, _super);
        function CMainGroupControl($el, parent) {
            _super.call(this, $el);
            this._parent = parent;
            this.createChannelList();
            this.setChildControl([]);
        }
        CMainGroupControl.prototype.destroy = function () {
            var a = this;
            if (a._channelList) {
                if (a._channelList._rollDataProvider) {
                    a._channelList._rollDataProvider.destroy();
                    delete a._channelList._rollDataProvider;
                    a._channelList._rollDataProvider = undefined;
                }
                a._channelList.destroy();
                delete a._channelList;
                a._channelList = undefined;
            }
            if (a._channelListData) {
                delete a._channelListData;
                a._channelListData = undefined;
            }
            if (a._channelListGroupOptBtn) {
                delete a._channelListGroupOptBtn;
                a._channelListGroupOptBtn = undefined;
            }
            if (a._parent) {
                delete a._parent;
                a._parent = undefined;
            }
            if (a._currentChannel) {
                delete a._currentChannel;
                a._currentChannel = undefined;
            }
            if (a._currentProgramme) {
                delete a._currentProgramme;
                a._currentProgramme = undefined;
            }
            if (a._updatedprogrammes) {
                delete a._updatedprogrammes;
                a._updatedprogrammes = undefined;
            }
            if (a._receivedProgrammes) {
                delete a._receivedProgrammes;
                a._receivedProgrammes = undefined;
            }
            if (a._receivedCatchUps) {
                delete a._receivedCatchUps;
                a._receivedCatchUps = undefined;
            }
            if (a._updatedCatchUps) {
                delete a._updatedCatchUps;
                a._updatedCatchUps = undefined;
            }
            if (a._receivedBackwards) {
                delete a._receivedBackwards;
                a._receivedBackwards = undefined;
            }
            if (a._updatedBackwards) {
                delete a._updatedBackwards;
                a._updatedBackwards = undefined;
            }
            if (focusedProgramme) {
                focusedProgramme = undefined;
            }
            if (focusedChannel) {
                focusedChannel = undefined;
            }
            if (a._$infoHubPane) {
                a._$infoHubPane.remove();
                delete a._$infoHubPane;
                a._$infoHubPane = undefined;
            }
            _super.prototype.destroy.call(a);
        };
        CMainGroupControl.prototype.createChannelList = function () {
            var a = this;
            var b = this._parent;
            var channelList = new __CONTROLS__.CCarouselControl(null);
            channelList.setId('channelList');
            channelList.setDrawParam({
                viewCount: 7,
                anchorIndex: 2,
                itemHeight: 54,
                anchorHeight: 270,
                align: 'vertical',
                animation: true,
                animationInterval: 0.15,
                maxKeyQueueCount: 1,
                transparentAnchor: true
            });
            channelList.setAnchorDrawer(function (param) {
                var that = this, bPrismLiteSMode = false;
                bPrismLiteSMode = a.isPrismLiteSModeOn();
                if (bPrismLiteSMode === true) {
                    that.drawPrismLiteSChannelList(param);
                }
                else {
                    that.drawPrismChannelList(param);
                }
            });
            channelList.setDataDrawer(function (param) {
                var channelNum = '', channelName = '';
                if (param.item) {
                    channelNum = hx.il.makeDigitNumber(param.item.majorChannel);
                    channelName = param.item.name;
                }
                param.$item.addClass('chItem');
                param.$item.append($('<div>', {
                    'class': 'chNum'
                }).text(channelNum));
                param.$item.append($('<div>', {
                    'class': 'chName'
                }).text(channelName));
                checkChannelsChangeable(param);
            });
            channelList.onFocusStartToChange = function (param) {
                if (!param.item) {
                    a.showChannelList();
                    // handle an execption
                    return;
                }
                var channelNum = hx.il.makeDigitNumber(param.item.majorChannel);
                if (a._$infoHubPane) {
                    a._$infoHubPane.$channelNum.text(channelNum);
                    a._$infoHubPane.$channelName.text(param.item.name);
                }
                focusedChannel = a._currentChannel = param.item;
                b._setCurrentChannel(focusedChannel);
                var bPrismLiteSMode = a.isPrismLiteSModeOn();
                if (bPrismLiteSMode === true) {
                    // initialize
                    a._channelList._$anchor.find('.chEvtName').html('');
                    a._channelList._$anchor.find('.chEvtTime').html('');
                    a._channelList._$anchor.find('.chThumbnail').html('');
                }
                else {
                    // initialize
                    var defaultThum = '<img src="' + a.getDefaultThumbnail() + '">';
                    a._channelList._$anchor.find('.chEvtName').html('');
                    a._channelList._$anchor.find('.chThumbnail').html(defaultThum);
                }
                b._resetTimer();
            };
            channelList.onFocusChanged = function (param) {
                if (!param.item) {
                    a.showChannelList();
                    // handle an execption
                    return;
                }
                var channelNum = hx.il.makeDigitNumber(param.item.majorChannel);
                if (a._$infoHubPane) {
                    a._$infoHubPane.$channelNum.text(channelNum);
                    a._$infoHubPane.$channelName.text(param.item.name);
                    if (hx.config.useSlideText) {
                        b._doSlideAnimation(a._$infoHubPane.$channelName, param.item.name);
                    }
                }
                focusedChannel = a._currentChannel = param.item;
                focusedProgramme = undefined;
                b._getChListEvt(focusedChannel);
            };
            channelList.onItemSelected = function (param) {
                var that = this;
                var channel = that._rollDataProvider.curItem();
                var curObj = hx.svc.CH_LIST.getCurrentChannel();
                if ((curObj.result === true) && (curObj.channel.ccid === channel.ccid) && (curObj.group === b._currentGroupDB)) {
                    // exit list
                    if (b._bActivate) {
                        b._hide();
                    }
                    return;
                }
                // check tunable
                var bChangeable = hx.svc.CH_LIST.checkChannelChangable({
                    channel: channel
                });
                if (bChangeable === false) {
                    if (hx.config.useTVStreaming === false) {
                        hx.msg('response', {
                            text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                    }
                    else {
                        hx.msg('response', {
                            text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                            auto_close: true,
                            close_time: 3000
                        });
                    }
                    return;
                }
                if (channel) {
                    hx.svc.AV_CTRL.channelChange({
                        channel: channel,
                        group: b._currentGroupDB
                    });
                    if (b._bActivate) {
                        b._hide();
                    }
                }
            };
            channelList.doKeyRec = function () {
                b._startRec(focusedChannel);
                return true;
            };
            channelList.drawPrismChannelList = function (param) {
                focusedChannel = a._currentChannel = param.currentItem.data;
                var channelNum = hx.il.makeDigitNumber(focusedChannel.majorChannel);
                var isCas, isLock;
                var programmeName = '', thumbnail = '', logo = '';
                var bRecording = false, thumbnailUrl = '';
                if (focusedChannel.casIDs) {
                    if (focusedChannel.casIDs.length > 0) {
                        isCas = true;
                    }
                }
                if (focusedChannel.manualBlock) {
                    isLock = true;
                }
                if (!!focusedProgramme) {
                    if (!!focusedProgramme.getTitle) {
                        programmeName = focusedProgramme.getTitle();
                    }
                    if (focusedProgramme.getThumbnailUrl) {
                        thumbnailUrl = focusedProgramme.getThumbnailUrl();
                    }
                    else {
                        thumbnailUrl = focusedProgramme.getDefaultThumbnail();
                    }
                    thumbnail = '<img src="' + thumbnailUrl + '" ' + 'onerror="this.src=\'' + focusedProgramme.getDefaultThumbnail() + '\';" />';
                }
                else {
                    thumbnail = '<img src="' + a.getDefaultThumbnail() + '">';
                }
                var $channelItem = $('<div>', {
                    'class': 'chItem'
                }).append($('<div>', {
                    'class': 'chEvtName'
                }).text(programmeName)).append($('<div>', {
                    'class': 'chThumOutline'
                })).append($('<div>', {
                    'class': 'chThumbnail'
                }).html(thumbnail));
                if (a.checkRecordings(focusedChannel.ccid)) {
                    $channelItem.find('.chThumbnail').append($recIcon);
                }
                if (ENV.op != ENV.listOP.UKDTT) {
                    if (focusedChannel.logoURL) {
                        logo = '<img src="' + focusedChannel.logoURL + '"onerror="this.src=\'\';" />';
                        $channelItem.find('.chThumbnail').append($('<div>', {
                            'class': 'chLogo'
                        }).html(logo));
                    }
                }
                if (isLock) {
                    $channelItem.find('.chEvtName').append($('<div>', {
                        'class': 'channelLock'
                    }));
                }
                if (isCas) {
                    var casInfo = a.getCasInformation();
                    $channelItem.find('.chEvtName').append($('<div>', {
                        'class': casInfo
                    }));
                }
                param.$anchor.append($channelItem);
            };
            channelList.drawPrismLiteSChannelList = function (param) {
                focusedChannel = a._currentChannel = param.currentItem.data;
                var channelNum = hx.il.makeDigitNumber(focusedChannel.majorChannel), isCas, isLock;
                if (focusedChannel.casIDs) {
                    if (focusedChannel.casIDs.length > 0) {
                        isCas = true;
                    }
                }
                if (focusedChannel.manualBlock) {
                    isLock = true;
                }
                var programmeName = '', programmeTime = '';
                var thumbnail = '', bRecording = false;
                var $channelItem = $('<div>', {
                    'class': 'chItem'
                }).append($('<div>', {
                    'class': 'chEvtName'
                }).text(programmeName)).append($('<div>', {
                    'class': 'chEvtTime'
                }).text(programmeTime)).append($('<div>', {
                    'class': 'chThumbnail'
                }).html(thumbnail));
                if (a.checkRecordings(focusedChannel.ccid)) {
                    $channelItem.find('.chThumbnail').append($recIcon);
                }
                if (isLock) {
                    $channelItem.find('.chEvtName').append($('<div>', {
                        'class': 'channelLock'
                    }));
                }
                if (isCas) {
                    var casInfo = a.getCasInformation();
                    $channelItem.find('.chEvtName').append($('<div>', {
                        'class': casInfo
                    }));
                }
                param.$anchor.append($channelItem);
                return true;
            };
            a._channelList = channelList;
            a._TYPE = ['TV', 'RADIO'];
        };
        CMainGroupControl.prototype.doShow = function () {
            var a = this;
            setTimeout(function () {
                if (a._$self) {
                    a._$self.addClass('show');
                }
            }, 1);
        };
        CMainGroupControl.prototype.doHide = function () {
            var a = this;
            var b = this._parent;
            state = 'channelList';
            a._$self.removeClass('show');
            if (hx.config.useSlideText) {
                b._stopSlideAnimation(null);
            }
        };
        CMainGroupControl.prototype.doDraw = function () {
            var _this = this;
            var a = this;
            var drawHandler = {
                'channelList': function () {
                    a.hideOptbtn();
                    createInfoHubIfNecessary(a);
                    updateEntryPane(a);
                    updateCurrentChannel(a);
                    a.displayOptbtn();
                    return _super.prototype.doDraw.call(a);
                }
            };
            return drawHandler[state]();
        };
        CMainGroupControl.prototype.displayOptbtn = function () {
            var a = this;
            var b = this._parent;
            if (a._channelListGroupOptBtn === undefined) {
                a._optIcon = $('<img>', {
                    'class': 'img',
                    'src': 'images/201_Option.png'
                });
                a._optText = $('<div>', {
                    'class': 'tit',
                    'html': hx.l('LOC_OPT_GROUP_ID') + ' - '
                });
                a._optData = $('<em>', {
                    'html': b._getLastGroupStr()
                });
                a._optText.append(a._optData);
                a._channelListGroupOptBtn = $('<div>', {
                    'id': 'channelListOptBtn'
                }).append(a._optIcon).append(a._optText);
                //$('#channelList').after(a._channelListGroupOptBtn);
                $('#-test-infoHub').append(a._channelListGroupOptBtn);
            }
            else {
                a._channelListGroupOptBtn.show();
            }
        };
        CMainGroupControl.prototype.hideOptbtn = function () {
            var a = this;
            if (a._channelListGroupOptBtn) {
                a._channelListGroupOptBtn.hide();
            }
        };
        CMainGroupControl.prototype.showChannelList = function (param) {
            var i, a = this, bPrismLiteSMode = false;
            a.setPrismAndPrismLiteMode();
            state = 'channelList';
            if (param) {
                focusedChannel = a._currentChannel = param;
            }
            for (i = 0; i < a._channelListData.length; i += 1) {
                var c = a._channelListData[i];
                if (c.ccid === a._currentChannel.ccid) {
                    a._channelList.setDrawParam('startIndex', i);
                    break;
                }
            }
            a._channelList.setDrawParam('drawEffect', 'spreadOut', true);
            a.setChildControl([a._channelList]);
            bPrismLiteSMode = a.isPrismLiteSModeOn();
            if (bPrismLiteSMode === true) {
                a.setChannelListPrismLiteSsetDrawParam();
            }
            else {
                a.setChannelListPrismsetDrawParam();
            }
            a.draw();
        };
        CMainGroupControl.prototype.keyBack = function () {
            var a = this;
            var b = this._parent;
            if (state === 'channelList') {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "channelList"
                    });
                }
                if (b._bActivate) {
                    b._hide();
                }
            }
        };
        CMainGroupControl.prototype.getCurrentState = function () {
            return state;
        };
        CMainGroupControl.prototype.setCurrentChannel = function (channel) {
            var a = this, c;
            var i;
            if (state === 'channelList') {
                for (i = 0; i < a._channelListData.length; i += 1) {
                    c = a._channelListData[i];
                    if (c.ccid === channel.ccid) {
                        a._channelList.setDrawParam('startIndex', i);
                        break;
                    }
                }
            }
            focusedChannel = a._currentChannel = channel;
        };
        CMainGroupControl.prototype.setCurrentGroup = function (group) {
            var a = this;
            a._currentGroup = group;
        };
        CMainGroupControl.prototype.setCurrentProgramme = function (programme) {
            var a = this;
            hx.log('infohub', '[setCurrentProgramme]');
            focusedProgramme = a._currentProgramme = programme;
        };
        CMainGroupControl.prototype.updateCurrentThumbnail = function (aThumbnailURL) {
            var a = this;
            var thumbnailHTML = '';
            if (!focusedChannel.manualBlock) {
                ;
                thumbnailHTML = '<img src="' + aThumbnailURL + '" ' + 'onerror="this.src=\'' + a.getDefaultThumbnail() + '\';" />';
            }
            else {
                thumbnailHTML = '<img src="' + a.getDefaultThumbnail() + '">';
            }
            a._channelList._$anchor.find('.chThumbnail').html(thumbnailHTML);
            if (a.checkRecordings(focusedChannel.ccid)) {
                a._channelList._$anchor.find('.chThumbnail').append($recIcon);
            }
        };
        CMainGroupControl.prototype.setFocusedChannelEvt = function (programme) {
            var a = this;
            var bPrismLiteSMode = a.isPrismLiteSModeOn();
            if (bPrismLiteSMode === true) {
                a.setChannelListPrismLiteSsetFocusEvent(programme);
            }
            else {
                a.setChannelListPrismsetFocusEvent(programme);
            }
        };
        CMainGroupControl.prototype.updateCurrentRecStatus = function (ccid, bStart) {
            var a = this;
            var b = this._parent;
            var bPrismLiteSMode = a.isPrismLiteSModeOn();
            if (!!focusedChannel) {
                if (state === 'channelList') {
                    if (ccid === focusedChannel.ccid) {
                        if (bPrismLiteSMode === true) {
                            if (bStart) {
                                b._getChListEvt(a._currentChannel);
                            }
                            else {
                                $('.chIconRecLite').detach();
                            }
                        }
                        else {
                            if (bStart) {
                                $('.chThumbnail').append($recIcon);
                            }
                            else {
                                $('.chIconRec').detach();
                            }
                        }
                    }
                    checkChannelsChangeable(a._channelList.getCurrentViewingItemInfos());
                }
            }
        };
        CMainGroupControl.prototype.updateRecordings = function (recordings) {
            var a = this;
            a._recordings = recordings;
        };
        CMainGroupControl.prototype.checkRecordings = function (ccid) {
            var a = this;
            if (a._recordings) {
                var len = a._recordings.length, i;
                for (i = 0; i < len; i += 1) {
                    if (ccid === a._recordings[i].getChannelId()) {
                        return true;
                    }
                }
            }
            return false;
        };
        CMainGroupControl.prototype.setChannelListData = function (channelList) {
            var a = this;
            a._channelListData = channelList;
            a._channelList.setData(channelList);
        };
        CMainGroupControl.prototype.getDefaultThumbnail = function () {
            var a = this;
            var channel = focusedChannel;
            var defaultThumbnail = (channel && channel.channelType === 1) ? __MODEL__.TDefaultThumbnailList.RADIO : __MODEL__.TDefaultThumbnailList.TV;
            hx.log("warning", "#### " + defaultThumbnail + "####");
            return defaultThumbnail;
        };
        CMainGroupControl.prototype.getCasInformation = function () {
            var that = this, i, casArr = [], length, casIDs;
            var b = this._parent;
            if (focusedChannel.ccid) {
                if (!!focusedChannel.casIDs) {
                    length = focusedChannel.casIDs.length;
                    for (i = 0; i < length; i += 1) {
                        casArr.push(focusedChannel.casIDs[i]);
                    }
                }
            }
            if (casArr.length > 0) {
                casIDs = 'cas ';
                if (casArr.length > 1) {
                    casIDs += 'multi';
                }
                else {
                    for (i = 0, length = casArr.length; i < length; i += 1) {
                        if (casArr[i] === b._strCasID.indexOf('CAS_NAGRA')) {
                            casIDs += 'na ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_IRDETO')) {
                            casIDs += 'ir ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_NDS')) {
                            casIDs += 'nd ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_VIACCESS')) {
                            casIDs += 'va ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_CONAX')) {
                            casIDs += 'cx ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_MEDIAGUARD')) {
                            casIDs += 'mg ';
                        }
                        else if (casArr[i] === b._strCasID.indexOf('CAS_CRYPTOWORKS')) {
                            casIDs += 'cr ';
                        }
                    }
                }
            }
            return casIDs;
        };
        // Used for PRISM Lite Simple
        CMainGroupControl.prototype.setPrismAndPrismLiteMode = function () {
            var a = this;
            var bIpEpgOn = false, bPrismLiteSmode = false;
            if ((hx.config.usePrismLiteS === undefined) || (hx.config.usePrismLiteS === false)) {
                bPrismLiteSmode = false;
            }
            else if (hx.config.usePrismLiteS === true) {
                if (ENV.listModel.IR4000HD === ENV.model || ENV.listModel.HDFOXIRCONNECT === ENV.model) {
                    bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
                    if (bIpEpgOn === true) {
                        bPrismLiteSmode = false;
                    }
                    else {
                        bPrismLiteSmode = true;
                    }
                }
                else if (ENV.listModel.HMS1000S === ENV.model) {
                    var countryId;
                    countryId = hx.svc.SETTING_UTIL.getCountryId();
                    bIpEpgOn = hx.svc.SETTING_UTIL.getEnableIPEPG();
                    switch (countryId) {
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_GERMANY_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_AUSTRIA_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_SWITZERLAND_ID')):
                        case hx.il.convertCountryId(hx.l('LOC_COUNTRY_ITALY_ID')):
                            bPrismLiteSmode = false;
                            break;
                        default:
                            bPrismLiteSmode = false;
                    }
                }
            }
            else {
                bPrismLiteSmode = false;
            }
            a.bPrismLiteSmode = bPrismLiteSmode;
        };
        CMainGroupControl.prototype.isPrismLiteSModeOn = function (ulIdx) {
            var a = this;
            return a.bPrismLiteSmode;
        };
        CMainGroupControl.prototype.setChannelListPrismLiteSsetFocusEvent = function (programme) {
            var a = this;
            focusedProgramme = a._currentProgramme = programme;
            var programmeName = '', programmeTime = '';
            var programmeDate = '', thumbnail = '', logo = '', casIcon = '';
            var sT, eT;
            var lockHtml = '', casHtml = '', progHtml = '', eventInfo, progTimeHtml = '';
            if (programme) {
                programmeName = programme.getName();
                sT = programme.getStartTime();
                eT = programme.getEndTime();
                programmeTime = dateFormat(new Date(sT * 1000), "HH:MM") + ' ~ ' + dateFormat(new Date(eT * 1000), "HH:MM");
            }
            if (a.checkRecordings(focusedChannel.ccid)) {
                thumbnail = '<div class="chIconRecLite"></div>';
            }
            var casInfo = a.getCasInformation();
            if (focusedChannel.manualBlock) {
                lockHtml = '<div class="channelLock"></div>';
            }
            if (casInfo) {
                casHtml = '<div class=\"' + casInfo + '\"></div>';
            }
            if (programmeName) {
                progHtml = programmeName;
            }
            else {
                progHtml = hx.l('LOC_NO_PROGRAMME_INFORMATION_ID');
            }
            if (programmeTime) {
                progTimeHtml = programmeTime;
            }
            eventInfo = lockHtml + casHtml + progHtml;
            a._channelList._$anchor.find('.chEvtName').html(eventInfo);
            a._channelList._$anchor.find('.chEvtTime').html(progTimeHtml);
            a._channelList._$anchor.find('.chThumbnail').html(thumbnail);
        };
        CMainGroupControl.prototype.setChannelListPrismsetFocusEvent = function (programme) {
            var a = this;
            if (focusedChannel && programme) {
                if (focusedChannel.ccid !== programme.ccid) {
                    return;
                }
            }
            if (focusedProgramme && focusedProgramme.load_thumbnail) {
                return;
            }
            focusedProgramme = a._currentProgramme = programme;
            var programmeName = '', thumbnail = '', logo = '', casIcon = '';
            if (programme && !focusedChannel.manualBlock) {
                programmeName = programme.getName();
            }
            if (a.checkRecordings(focusedChannel.ccid)) {
                thumbnail = thumbnail + '<div class="chIconRec"></div>';
            }
            if (ENV.op != ENV.listOP.UKDTT) {
                if (focusedChannel.logoURL) {
                    logo = '<img src="' + focusedChannel.logoURL + '"onerror="this.src=\'\';" />';
                    thumbnail = thumbnail + '<div class="chLogo">' + logo + '</div>';
                }
            }
            var casInfo = a.getCasInformation();
            var lockHtml = '', casHtml = '', progHtml = '', eventInfo;
            if (focusedChannel.manualBlock) {
                lockHtml = '<div class="channelLock"></div>';
            }
            if (casInfo) {
                casHtml = '<div class=\"' + casInfo + '\"></div>';
            }
            if (programmeName) {
                progHtml = programmeName;
            }
            eventInfo = lockHtml + casHtml + progHtml;
            if (a._channelList && a._channelList._$anchor) {
                if (programme) {
                    a._channelList._$anchor.find('.chEvtName').html(eventInfo);
                }
            }
        };
        CMainGroupControl.prototype.setChannelListPrismLiteSsetDrawParam = function () {
            var that = this;
            that._channelList._drawParam.viewCount = 10;
            that._channelList._drawParam.anchorHeight = 115;
        };
        CMainGroupControl.prototype.setChannelListPrismsetDrawParam = function () {
            var that = this;
            that._channelList._drawParam.viewCount = 7;
            that._channelList._drawParam.anchorHeight = 270;
        };
        CMainGroupControl.prototype.setPrismLiteSInitDataIdx = function (ulIdx) {
            var a = this;
            // Current Disp Event Index of iplate provider list
            a._ulDispEventIdx = ulIdx;
            // Event Count
            a._ulEventCount = 0;
            if (a._receivedProgrammes === true) {
                a._ulEventCount = a._updatedprogrammes.length;
            }
            // Catch ups Count
            //a._ulCatchUpsCount = 0;
            //if (a._receivedCatchUps === true) {
            //    a._ulCatchUpsCount = a._updatedCatchUps.length;
            //}
        };
        CMainGroupControl.prototype.setPrismLiteSUpdateDataIdx = function (ulEvent, ulCatchUps) {
            var a = this;
            a._ulEventCount = ulEvent;
            a._ulCatchUpsCount = ulCatchUps;
        };
        CMainGroupControl.prototype.actionReservaion = function (action) {
            var a = this;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var p = a._currentProgramme;
            var efactoryType = 0, bIsGetPvrHDD = false, bIsLiveEvt = false;
            var scheduledList = dsrvRecordings2.getScheduledList();
            if (a._ulEventCount < 1) {
                hx.log('infohub', 'actionReservaion() Error!! Does not has event data!!');
                return false;
            }
            function maxReservation() {
                // Check Max reservation count
                var ret = false;
                if (scheduledList.length >= 300) {
                    var now = new Date();
                    var ct = Math.floor(now.valueOf() / 1000);
                    if (p) {
                        // check live
                        if (p.catchup === undefined) {
                            if (ct >= p.getStartTime() && ct <= p.getEndTime()) {
                                bIsLiveEvt = true;
                            }
                            else {
                                bIsLiveEvt = false;
                            }
                        }
                    }
                    if (bIsLiveEvt === false) {
                        hx.log('infohub', 'actionReservaion() Error!! Does not action reservaion because max schedule count !!');
                        var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
                        hx.msg('response', {
                            text: msg,
                            auto_close: true,
                            close_time: 3000
                        });
                        ret = true;
                    }
                    else {
                        hx.log('infohub', 'actionReservaion() Error!! Does not action reservaion because max schedule count!! But live Event');
                        ret = false;
                    }
                }
                return ret;
            }
            function cancelReservation(data) {
                hx.log('infohub', 'actionReservaion() - cancelReservation()');
                var i, ti, scheduledItem, programmeID, foundItem;
                for (i = 0, ti = scheduledList.length; i < ti; i += 1) {
                    scheduledItem = scheduledList[i];
                    programmeID = scheduledItem.getProgrammeID();
                    if (programmeID && (programmeID === data.programmeID)) {
                        foundItem = scheduledItem;
                        break;
                    }
                }
                if (foundItem) {
                    dsrvRecordings2.removeScheduledRecording({
                        items: [scheduledItem],
                        finished: function () {
                            hx.log('infohub', 'actionReservaion() finished removeScheduledRecording');
                        }
                    });
                }
            }
            function isRecordingEvent(data) {
                var bIsRecordingEvent = false;
                var item = null, recCCid = 0, i, ti;
                var cRecList = dsrvRecordings2.getCurrentRecordings();
                var ulStartTimeValue = 0, ulEndTimeValue = 0;
                var ulstartTime = 0, ulendTime = 0;
                if (cRecList.length === 0) {
                    hx.log('infohub', 'isRecordingEvent() no recording channel');
                    bIsRecordingEvent = false;
                }
                for (i = 0, ti = cRecList.length; i < ti; i += 1) {
                    item = cRecList[i];
                    if (item && (item.getChannelId() === data.ccid)) {
                        hx.log('infohub', 'isRecordingEvent() find same recording channel : ' + item.getChannelId());
                        ulStartTimeValue = item.getStartTime();
                        ulEndTimeValue = item.getEndTime();
                        ulstartTime = Math.floor(ulStartTimeValue / 1000);
                        ulendTime = Math.floor(ulEndTimeValue / 1000);
                        if ((ulstartTime <= data.startTime) && (ulendTime >= data.startTime)) {
                            bIsRecordingEvent = true;
                        }
                        else {
                            bIsRecordingEvent = false;
                        }
                        break;
                    }
                }
                return bIsRecordingEvent;
            }
            function onRequestInstanceRecording(data) {
                var param = {};
                if (data) {
                    if (hx.config.useDelayedRec) {
                        param.tsrPosOffset = hx.svc.TSR.getPlaybackOffset();
                        param.tsrBufferSize = hx.svc.TSR.getMaxOffset();
                    }
                    else {
                        param.tsrPosOffset = 0;
                        param.tsrBufferSize = 0;
                    }
                    param.finished = function (result) {
                        hx.log('infohub', hx.util.format('onRequestInstanceRecording - result = {0}', result));
                    };
                    if (data.ccid) {
                        hx.log('infohub', hx.util.format('onRequestInstanceRecording - channel.ccid = {0}', data.ccid));
                        param.ccid = data.ccid;
                    }
                    var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                    if (bIsGetPvrHDD === true) {
                        hx.svc.RECORDINGS.requestInstanceRecording(param);
                    }
                    else {
                        this.sendEventToEntity({
                            alEvent: 'alMessageToPage',
                            target: 'CPgNotAvailable?show'
                        });
                    }
                }
            }
            if (p && (p.isLive() !== true)) {
                var ret = false;
                if (!p.hasSchedule() && !p.hasReminder()) {
                    var fullReserved = maxReservation();
                    if (fullReserved === true) {
                        return true;
                    }
                    bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
                    if (bIsGetPvrHDD === false) {
                        efactoryType = 1;
                    }
                    dsrvRecordings2.record({
                        programme: p.pluginObject,
                        factoryType: efactoryType,
                        finished: function () {
                            hx.log('infohub', 'actionReservaion() finished record');
                        }
                    });
                    ret = true;
                }
                else if (p.hasSchedule() || p.hasReminder()) {
                    var bIsRecording = isRecordingEvent(p);
                    if (bIsRecording === false) {
                        cancelReservation(p);
                        ret = true;
                    }
                    else {
                        onRequestInstanceRecording(p);
                        ret = true;
                    }
                }
                else {
                    ret = false;
                }
            }
            else {
                if (p && (p.isLive() === true)) {
                    onRequestInstanceRecording(p);
                    ret = true;
                }
                else {
                    ret = false;
                }
            }
            return ret;
        };
        return CMainGroupControl;
    })(__CONTROLS__.CGroupControl);
    var cCPgInfoHub = (function (_super) {
        __extends(cCPgInfoHub, _super);
        function cCPgInfoHub(aPageName) {
            _super.call(this, aPageName);
            this._responseObj = null;
            this._currentChannel = null;
            this._currentEvent = null;
            this._remChannel = null;
            this._recordings = [];
            this._groupContextMenu = null;
            this._strCurrentGroup = '';
            this._favlistname = [];
            this._strChannelGroupName = [];
            this._currentGroupDB = null;
            this._strCasID = hx.config.CPgInfoHub.chGroup.strCasID;
            this._groupDB = hx.config.CPgInfoHub.chGroup.groupDB;
            this._favGroupDB = hx.config.CPgInfoHub.chGroup.favGroupDB;
            // Alphabet Group
            this._alphabetgroupContextMenu = null;
            this._strAlphabetGroupName = [];
            this._alphabetGroupDB = hx.config.CPgInfoHub.chGroup.alphabetGroupsDB;
            this.TextAniValueOffset = 11;
            this.TextAniIntervalOffset = 15;
            this.THRESHOLD = 7 * 24 * 60 * 60 * 1000; // 7DAYS(ms)
            this.callbackIpEpgResultForChannelList = function (channel, res) {
                var _this = this;
                if (this._currentChannel !== channel) {
                    return;
                }
                var currentProgramme = this._infoHub._currentProgramme;
                if (!currentProgramme) {
                    return;
                }
                if (currentProgramme && currentProgramme.isWrapper) {
                    this._infoHub.setFocusedChannelEvt(currentProgramme);
                    return;
                }
                if (!res) {
                    this._infoHub.updateCurrentThumbnail(currentProgramme.getThumbnail());
                    return;
                }
                var THUMB_WIDTH = 263;
                var parsedId = hx.svc.IP_EPG.parse.util.programIdParser(currentProgramme.getProgrammeID());
                var legacyId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                var info = hx.svc.IP_EPG.parse.xml.schedule(res);
                var schedules = info.schedules;
                var data = null, eventId = null;
                var aEvent = null;
                for (var i = 0; i < schedules.length; i++) {
                    aEvent = hx.svc.IP_EPG.parse.element.scheduleEvent(schedules[i]);
                    parsedId = hx.svc.IP_EPG.parse.util.programIdParser(aEvent.programURL);
                    eventId = [parsedId.onid, parsedId.svcid, parsedId.eventid].join('.');
                    if (eventId === legacyId) {
                        data = info.crids[aEvent.crid];
                        data.schedule = schedules[i];
                        data.available = false;
                        currentProgramme.setMdsData({
                            mdsData: data,
                            launchLocation: 'epg'
                        });
                        currentProgramme.getThumbnail({
                            updateCb: function (url) {
                                _this._infoHub.updateCurrentThumbnail(url);
                            },
                            width: THUMB_WIDTH
                        });
                        data = null;
                        break;
                    }
                }
            };
        }
        cCPgInfoHub.prototype.create = function () {
            var _this = this;
            var that = this;
            that._bActivate = false;
            that._timerId = -1;
            hx.logadd('infohub');
            that._init_Data();
            // Main control: info hub control
            $.extend(this, {
                $container: $("#wrap")
            });
            //        var CInfoHubControl = that.CLASSES.CInfoHubControl;
            var $infoHub = $('<div>', {
                'id': '-test-infoHub',
                'class': '-afd'
            });
            var infoHub = new CMainGroupControl($infoHub, this);
            $('#ltv').after($infoHub);
            that._$infoHub = $infoHub;
            that._infoHub = infoHub;
            hx.al.compose(that);
            var fnUpdate = function () {
                var cRecList = hx.svc.RECORDINGS.getCurrentRecordings();
                if (!!cRecList) {
                    that._recordings = cRecList;
                    that._infoHub.updateRecordings(cRecList);
                }
                cRecList = undefined;
            };
            hx.svc.RECORDINGS.addEventCb('RecordingItemAdded', that, function (param) {
                var ccid = param.item.getChannelId();
                that._infoHub.updateCurrentRecStatus(ccid, true);
                fnUpdate();
            });
            hx.svc.RECORDINGS.addEventCb('RecordingCompleted', that, function (param) {
                var ccid = param.item.getChannelId();
                that._infoHub.updateCurrentRecStatus(ccid, false);
                fnUpdate();
            });
            hx.svc.CH_LIST.addEventCb('ChannelListUpdate', this, function () {
                _this._on_ChannelListUpdate();
            });
            if (ENV.op === ENV.listOP.UKDTT) {
                hx.svc.RECORDINGS.addEventCb('ShowHDAlternativeRecordWindow', this, function (aButtonInfo) {
                    if (_this.getPageVisible(_this.name) === true) {
                        var dlg = prism.createHDAlternativeButtonDlg(null, aButtonInfo);
                        dlg.open({
                            onClosed: function () {
                                _this.setKeyHookingControl(dlg);
                            }
                        });
                        _this.setKeyHookingControl(dlg, true);
                    }
                });
            }
        };
        cCPgInfoHub.prototype.destroy = function () {
            hx.svc.RECORDINGS.clearEventCb(this);
            // remove event listeners
            hx.svc.AV_CTRL.clearEventCb(this);
            hx.svc.CLOCK.clearEventCb(this);
            // remove subWrap
            if (this._bActivate === true) {
                this._hide();
            }
            if (this._$infoHub) {
                this._$infoHub.remove();
                delete this._$infoHub;
                this._$infoHub = undefined;
            }
            if (this._infoHub) {
                this._infoHub.destroy();
                delete this._infoHub;
                this._infoHub = undefined;
            }
            if (this._recordings) {
                delete this._recordings;
                this._recordings = undefined;
            }
            if (this._currentEvent) {
                delete this._currentEvent;
                this._currentEvent = undefined;
            }
            if (this._currentChannel) {
                delete this._currentChannel;
                this._currentChannel = undefined;
            }
            if (this._remChannel) {
                delete this._remChannel;
                this._remChannel = undefined;
            }
        };
        cCPgInfoHub.prototype.isActive = function () {
            return this._bActivate;
        };
        cCPgInfoHub.prototype.visible = function () {
            return this._bActivate;
        };
        cCPgInfoHub.prototype._on_alMessageToPage = function (aParam) {
            var that = this;
            var retObj, retObj2, retObj3;
            var state = that._infoHub.getCurrentState();
            var dsrvChList = hx.svc.CH_LIST;
            var param = '';
            hx.log('infohub', '[_on_alMessageToPage]');
            if (aParam.query === 'dcn') {
                if (state === 'channelList') {
                    if (aParam.param !== 'clear') {
                        var ch = dsrvChList.getCurrentChannel();
                        if (ch.group === "TV" || ch.group === "RADIO") {
                            var data = {};
                            data.ccid = aParam.param;
                            data = dsrvChList.getAvailableGroupForCcid(data);
                            this._infoHub._optData[0].innerHTML = this._getLastGroupStr(data.group);
                            retObj3 = dsrvChList.getChannel({
                                ccid: aParam.param,
                                group: data.group
                            });
                        }
                        else {
                            retObj3 = dsrvChList.getChannel({
                                ccid: aParam.param,
                                group: this._currentGroupDB
                            });
                        }
                        that._infoHub.setCurrentProgramme(undefined);
                        that._getChlist(retObj3);
                        that._infoHub.showChannelList(retObj3.channel);
                    }
                    that._resetTimer();
                }
                else if (state === 'iPlate') {
                    param = aParam.param ? '=' + aParam.param : '';
                    that.sendEventToEntity({
                        'alEvent': 'alMessageToPage',
                        'target': 'CPgIPlate?' + aParam.query + param
                    });
                }
                return;
            }
            retObj = dsrvChList.getCurrentChannel();
            if (true === retObj.result) {
                // for fast load
                if (aParam.query === 'backToParent') {
                    // TODO: status�� ���� iplate�̸� messageó��
                    // TODO: show iplate
                    if (state === 'iPlate') {
                        that.sendEventToEntity({
                            'alEvent': 'alMessageToPage',
                            'target': 'CPgIPlate?show'
                        });
                    }
                }
                else if (aParam.query === 'backToLive') {
                    that.sendEventToEntity({
                        'alEvent': 'alMessageToPage',
                        'target': 'CPgIPlate?hide'
                    });
                }
                else {
                    if (aParam.query === 'channelList') {
                        if (that._$infoHub[0].style.display === 'none') {
                            that._$infoHub[0].style.display = 'block';
                            that._bActivate = true;
                            that._hide();
                        }
                        that._getChlist(retObj);
                        that._draw_ChInfo(retObj, aParam.query, false);
                    }
                    else {
                        that.sendEventToEntity({
                            'alEvent': 'alMessageToPage',
                            'target': 'CPgIPlate?show'
                        });
                    }
                }
            }
        };
        cCPgInfoHub.prototype._draw_ChInfo = function (p, aMode, aIsUpdate) {
            var that = this;
            var newChannel = true;
            that._currentChannel = p.channel;
            that._currentGroupDB = p.group;
            // use cache ???
            if (that._currentChannel && that._remChannel) {
                if (that._currentChannel.ccid === that._remChannel.ccid) {
                    newChannel = false;
                }
            }
            if (aMode === 'channelList') {
                newChannel = false;
            }
            that._remChannel = that._currentChannel;
            var startTime = Math.floor(new Date().getTime() / 1000);
            if (hx.svc.TSR.isTsrOngoing() === true) {
                var VBChannel = hx.svc.AV_CTRL.getVideoBroadcastCurrentChannel();
                if (!!VBChannel && VBChannel.ccid === that._currentChannel.ccid) {
                    var TSRstartTime = hx.svc.TSR.getStartTime();
                    var TSRplaybackOffset = hx.svc.TSR.getPlaybackOffset();
                    hx.log('warning', 'StartTime : ' + TSRstartTime + ' offset : ' + TSRplaybackOffset);
                    startTime = TSRstartTime + TSRplaybackOffset;
                }
            }
            var endTime = startTime + 3600 * 12;
            if (aMode === 'iPlate') {
            }
            //////////////////
            //that._getEvt(that._currentChannel, newChannel, startTime, endTime, aMode);
            var cRecList = hx.svc.RECORDINGS.getCurrentRecordings();
            if (cRecList) {
                that._recordings = cRecList;
                that._infoHub.updateRecordings(cRecList);
            }
            if (that._bActivate === false || aIsUpdate === true) {
                that._show(aMode);
                $('#channelListOptBtn').find('em').text(that._getLastGroupStr(that._currentGroupDB));
                if (hx.config.useSlideText) {
                    that._doSlideAnimation($('#entryPane .highlight .chItem .chName'), that._currentChannel.name);
                }
            }
        };
        cCPgInfoHub.prototype._doSlideAnimation = function (target, text) {
            var that = this;
            that._stopSlideAnimation(target);
            var textAniValue = text.length * that.TextAniValueOffset;
            var textAniInterval = textAniValue * that.TextAniIntervalOffset;
            hx.il.slideTextAnimate(target, textAniValue, textAniInterval);
            that._textAnimate = setInterval(function () {
                hx.il.slideTextAnimate(target, textAniValue, textAniInterval);
            }, textAniInterval);
        };
        cCPgInfoHub.prototype._stopSlideAnimation = function (target) {
            var that = this;
            if (that._textAnimate) {
                clearInterval(that._textAnimate);
                that._textAnimate = null;
            }
            if (target) {
                target.stop();
                target[0].scrollLeft = 0;
            }
        };
        cCPgInfoHub.prototype._show = function (aMode) {
            // reset timer
            this._resetTimer();
            // hide pin
            this.sendEventToEntity({
                alEvent: 'alMessageToPage',
                target: 'CPgLiveController?hidepin=pin'
            });
            this._infoHub.setCurrentGroup(this._currentGroupDB);
            this._infoHub.setCurrentChannel(this._currentChannel);
            if (aMode === 'channelList') {
                this._infoHub.showChannelList();
                this._infoHub.show();
                // set flag
                this._bActivate = true;
                // show & animation .doShow
                this.sendEventToEntity({
                    alEvent: 'alCheckPageVisible'
                });
            }
            else if (aMode === 'iPlate') {
                this._infoHub.setPrismAndPrismLiteMode();
                state = 'iPlate';
                this.sendEventToEntity({
                    'alEvent': 'alMessageToPage',
                    'target': 'CPgIPlate?show'
                });
            }
        };
        cCPgInfoHub.prototype._resetTimer = function () {
            var _this = this;
            clearTimeout(this._timerId);
            this._timerId = setTimeout(function () {
                _this._hide();
            }, 30000);
        };
        cCPgInfoHub.prototype._hide = function () {
            var that = this;
            var state = that._infoHub.getCurrentState();
            if (state === "channelList") {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "channelList"
                    });
                }
            }
            else if (state === "iPlate") {
                if (hx.svc.STATISTICS) {
                    hx.svc.STATISTICS.statisticsLog({
                        "CODE": "OSDOUT",
                        "TRS": "I",
                        "OSD": "iPlate"
                    });
                }
            }
            that._destroyGroupListMenu();
            if (hx.config.useAlphabetGroup === 1) {
                that._ag_destroyAlphabetGroupListMenu();
            }
            that._infoHub.hide();
            if (that._infoHub._channelListGroupOptBtn) {
                that._infoHub._channelListGroupOptBtn.hide();
            }
            // kill timer
            if (that._timerId !== -1) {
                clearTimeout(that._timerId);
                that._timerId = -1;
            }
            if (that.getCenterPageName() === 'CPgLiveController') {
                that.sendEventToEntity({
                    'alEvent': 'alMessageToPage',
                    'target': 'CPgLiveController?hideInfoHub'
                });
                // show pin
                that.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgLiveController?showpin=pin'
                });
            }
            // un-set flag
            that._bActivate = false;
            that.sendEventToEntity({
                alEvent: 'alCheckPageVisible'
            });
        };
        cCPgInfoHub.prototype._on_alKey = function (aParam) {
            var that = this;
            var keyCode = aParam.alKey;
            var bConsumed = false, bActionPvr = false;
            var pra, bPrismLiteSMode = false;
            var ulTvSvcCount = 0, ulRadioSvcCount = 0;
            var state;
            hx.log('infohub', '[_on_alKey] = ' + keyCode);
            that._resetTimer();
            bConsumed = that._infoHub.doKey(keyCode);
            if (bConsumed) {
                return true;
            }
            state = that._infoHub.getCurrentState();
            if (hx.config.useSlideText) {
                if ((state === 'channelList') && ((keyCode === hx.key.KEY_UP) || (keyCode === hx.key.KEY_DOWN))) {
                    if (that._infoHub._$infoHubPane) {
                        that._stopSlideAnimation(that._infoHub._$infoHubPane.$channelName);
                    }
                }
            }
            switch (keyCode) {
                case hx.key.KEY_0:
                case hx.key.KEY_1:
                case hx.key.KEY_2:
                case hx.key.KEY_3:
                case hx.key.KEY_4:
                case hx.key.KEY_5:
                case hx.key.KEY_6:
                case hx.key.KEY_7:
                case hx.key.KEY_8:
                case hx.key.KEY_9:
                    that.sendEventToEntity({
                        alEvent: 'alMessageToPage',
                        target: 'CPgDcn?active=' + (keyCode - hx.key.KEY_0).toString()
                    });
                    clearTimeout(that._timerId);
                    bConsumed = true;
                    break;
                case hx.key.KEY_CHANNEL_UP:
                case hx.key.KEY_CHANNEL_DOWN:
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                    that._infoHub.keyBack();
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    if (that._bActivate) {
                        that._hide();
                        bConsumed = true;
                    }
                    break;
                case hx.key.KEY_OK:
                    bConsumed = true;
                    break;
                case hx.key.KEY_UP:
                case hx.key.KEY_DOWN:
                    if (state !== 'channelList') {
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE": "OSDOUT",
                                "TRS": "I",
                                "OSD": state
                            });
                        }
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE": "OSDIN",
                                "TRS": "I",
                                "OSD": "channelList"
                            });
                        }
                        pra = {
                            group: that._currentGroupDB,
                            channel: that._currentChannel
                        };
                        that._draw_ChInfo(pra, 'channelList', false);
                        that._infoHub.showChannelList();
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_LEFT:
                case hx.key.KEY_RIGHT:
                    bConsumed = true;
                    break;
                case hx.key.KEY_OPTION:
                    if (state === 'channelList') {
                        // channel group select
                        hx.ui.overlay.left($('#-test-infoHub'));
                        that._groupContextMenu = that._createOptListMenu();
                    }
                    //else if (state === 'iPlate') {
                    //    that.sendEventToEntity({
                    //        alEvent : 'alMessageToPage',
                    //        target : 'CPgNotAvailable?show'
                    //    });
                    //}
                    bConsumed = true;
                    break;
                case hx.key.KEY_PLAY:
                case hx.key.KEY_PAUSE:
                case hx.key.KEY_PLAY_SPEED_UP:
                case hx.key.KEY_PLAY_SPEED_DOWN:
                case hx.key.KEY_REWIND:
                case hx.key.KEY_FAST_FWD:
                    if (state === 'channelList') {
                        bConsumed = true;
                    }
                    else {
                        that._hide();
                        bConsumed = false;
                    }
                    break;
                case hx.key.KEY_STOP:
                    if (state === 'channelList') {
                        that._stopCurrentChannelRecord(that._currentChannel.ccid);
                        bConsumed = true;
                    }
                    break;
                case hx.key.KEY_RECORD:
                    break;
                case hx.key.KEY_TVRADIO_TOGGLE:
                    if (state === 'channelList') {
                        ulTvSvcCount = hx.svc.CH_LIST.getChannelCount({
                            group: 'TV'
                        }).chCnt;
                        ulRadioSvcCount = hx.svc.CH_LIST.getChannelCount({
                            group: 'RADIO'
                        }).chCnt;
                        if (that._currentGroupDB === 'RADIO') {
                            if (ulTvSvcCount > 0) {
                                that._changeChannelGroup('TV');
                            }
                        }
                        else {
                            if (ulRadioSvcCount > 0) {
                                that._changeChannelGroup('RADIO');
                            }
                        }
                        bConsumed = true;
                    }
                    break;
                case hx.key.KEY_RED:
                    if (hx.config.useLiveTuningInfoShow) {
                        if (state === 'channelList') {
                            if (that._currentChannel) {
                                var _tuningInfos = that._currentChannel.tuningInfo;
                                var _tuningInfoArr = [];
                                if (_tuningInfos) {
                                    _tuningInfoArr = _tuningInfos.split(',');
                                    //var _convName = hx.il.convertSatelliteNameToShow(_tuningInfoArr[0]);
                                    _tuningInfos = _tuningInfoArr[0] + ',' + _tuningInfoArr[1] + ',' + _tuningInfoArr[2] + ',' + _tuningInfoArr[3] + ',' + _tuningInfoArr[4];
                                    var msgKey = hx.msg('codeConfirm', {
                                        first_line: hx.l('LOC_CHANNEL_INFORMATION_ID'),
                                        thirdLineFont: true,
                                        nolinejump: true,
                                        second_line: that._currentChannel.majorChannel + ', ' + that._currentChannel.name + '<br>' + _tuningInfos,
                                        auto_close: false,
                                        btn_title_arr: [hx.l('LOC_OK_ID')]
                                    });
                                    setTimeout(function () {
                                        hx.msg.close('codeConfirm', msgKey);
                                    }, 5000);
                                }
                            }
                            bConsumed = true;
                        }
                    }
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cCPgInfoHub.prototype._stopCurrentChannelRecord = function (aData) {
            var item, recCCid = 0, findItem, i, ti;
            var dsrvRecordings = hx.svc.RECORDINGS;
            var currentRecordingList = dsrvRecordings.getCurrentRecordings();
            var ret = false;
            if (currentRecordingList.length === 0) {
                hx.log('infohub', '_stopCurrentChannelRecord() no recording channel');
                return false;
            }
            for (i = 0, ti = currentRecordingList.length; i < ti; i += 1) {
                item = currentRecordingList[i];
                if (item) {
                    if (item.getChannelId() === aData.ccid) {
                        hx.log('infohub', '_stopCurrentChannelRecord() find same recording channel : ' + item.getChannelId());
                        findItem = item;
                        break;
                    }
                }
            }
            if (findItem) {
                if (item instanceof __MODEL__.IRecordingItem) {
                    dsrvRecordings.stopRecord({
                        item: item
                    });
                }
                else if (item instanceof __MODEL__.IScheduledItem) {
                    dsrvRecordings.removeScheduledRecording({
                        items: [item]
                    });
                }
                ret = true;
            }
            else {
                ret = false;
            }
            return ret;
        };
        cCPgInfoHub.prototype._getEvt = function (aChannel, aRenew, aStartTime, aEndTime, aMode) {
            var _this = this;
            //chlist
            hx.svc.EPG.getProgrammes({
                isOnAir: false,
                channel: aChannel,
                startTime: aStartTime,
                endTime: aEndTime,
                listener: this,
                notUI: true,
                cb: function (result) {
                    if (result.length) {
                        _this._currentEvent = result[0][0];
                        if (_this._currentEvent) {
                            _this._infoHub.setCurrentProgramme(_this._currentEvent);
                            if (aMode === 'iPlate') {
                            }
                            else if (aMode === 'channelList') {
                                _this._infoHub.setFocusedChannelEvt(_this._currentEvent);
                                _this.updateWithIpEpg(aChannel, _this.callbackIpEpgResultForChannelList.bind(_this), _this._currentEvent.getStartTime() * 1000);
                            }
                            else {
                            }
                        }
                    }
                    result = undefined;
                }
            });
            // clear cached event
            if (aRenew === true) {
                this._currentEvent = null;
                this._infoHub.setCurrentProgramme(undefined);
                this._infoHub.setiPlateProgrammes(undefined);
            }
        };
        cCPgInfoHub.prototype._getChListEvt = function (aChannel) {
            var _this = this;
            this._currentChannel = aChannel;
            hx.svc.EPG.getProgrammes({
                isOnAir: true,
                filter: "P/F",
                channel: aChannel,
                listener: this,
                cb: function (result) {
                    if (result) {
                        var len = result[0].length;
                        if (len > 0) {
                            _this._infoHub.setCurrentProgramme(result[0][0]);
                            _this._infoHub.setFocusedChannelEvt(result[0][0]);
                            _this.updateWithIpEpg(aChannel, _this.callbackIpEpgResultForChannelList.bind(_this), result[0][0].getStartTime() * 1000);
                        }
                        else {
                            _this._infoHub.setFocusedChannelEvt(undefined);
                        }
                    }
                    else {
                        _this._infoHub.setFocusedChannelEvt(undefined);
                    }
                    result = undefined;
                }
            });
            //this.updateWithIpEpg(aChannel, this.callbackIpEpgResultForChannelList.bind(this), Date.now());
        };
        cCPgInfoHub.prototype.updateWithIpEpg = function (channel, callback, time, chunk, startIdx) {
            if (time === void 0) { time = Date.now(); }
            if (chunk === void 0) { chunk = 1; }
            if (startIdx === void 0) { startIdx = 0; }
            var HOUR6 = 60 * 60 * 6 * 1000;
            var i;
            if (channel) {
                for (i = 0; i < chunk; i += 1) {
                    hx.svc.IP_EPG.getSchedules(channel.sid, time + (HOUR6 * i), 6).then(function (res) {
                        callback(channel, res, startIdx);
                    }, function () {
                        callback(channel, null, startIdx);
                    });
                }
            }
        };
        cCPgInfoHub.prototype._getChlist = function (aCurrentChannel) {
            var retObj;
            retObj = hx.svc.CH_LIST.getChlist({
                group: aCurrentChannel.group
            });
            var list = [];
            if (ENV.op === ENV.listOP.UKDTT) {
                var listChannel = retObj.chList;
                var channel;
                for (var i = 0, len = listChannel.length; i < len; i++) {
                    channel = listChannel[i];
                    if (!channel.hidden) {
                        list.push(channel);
                    }
                }
                listChannel = undefined;
                channel = undefined;
            }
            else {
                list = retObj.chList;
            }
            this._infoHub.setChannelListData(list);
            list = undefined;
            retObj = undefined;
        };
        cCPgInfoHub.prototype._sortByNewerTime = function (a, b) {
            var aa = a.getStartTime();
            var bb = b.getStartTime();
            if (aa < bb) {
                return -1;
            }
            if (bb < aa) {
                return 1;
            }
            return 0;
        };
        cCPgInfoHub.prototype._startRec = function (aFocusedChannel) {
            var that = this;
            var dsrvRecordings = hx.svc.RECORDINGS;
            var bIsGetPvrHDD = hx.svc.STORAGE_MASS.getRecPVRHDD();
            if (bIsGetPvrHDD === false) {
                that.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgNotAvailable?show'
                });
                return;
            }
            if (aFocusedChannel) {
                var recordings = dsrvRecordings.getCurrentRecordings();
                var i, length, recording;
                if (recordings && recordings.length) {
                    length = recordings.length;
                    for (i = 0; i < length; i += 1) {
                        recording = recordings[i];
                        if (recording.getChannelId() === focusedChannel.ccid) {
                            dsrvRecordings.stopRecord({
                                item: recording
                            });
                            return;
                        }
                    }
                }
                hx.svc.BRIDGE.requestInstanceRecording(focusedChannel);
            }
        };
        cCPgInfoHub.prototype._doReservation = function (data) {
            var that = this;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var scheduledList = dsrvRecordings2.getScheduledList();
            // Check Max reservation count
            if (scheduledList.length >= 300) {
                var msg = hx.l('LOC_RESERVE_ONLY_300_ID');
                hx.msg('response', {
                    text: msg,
                    auto_close: true,
                    close_time: 3000
                });
                return;
            }
            var tmp, pluginObj;
            tmp = data.pluginObject || data._pluginObject;
            if (tmp) {
                pluginObj = tmp.pluginObject || tmp._pluginObject;
                if (pluginObj) {
                    tmp = pluginObj;
                    pluginObj = tmp.pluginObject || tmp._pluginObject;
                    if (!pluginObj) {
                        pluginObj = tmp;
                    }
                }
                else {
                    pluginObj = tmp;
                }
            }
            dsrvRecordings2.record({
                programme: pluginObj,
                factoryType: 0,
                finished: function () {
                }
            });
        };
        cCPgInfoHub.prototype._cancelReservation = function (data) {
            var that = this;
            var dsrvRecordings2 = hx.svc.RECORDINGS;
            var scheduledList = dsrvRecordings2.getScheduledList();
            var i, ti, scheduledItem, programmeID, foundItem;
            for (i = 0, ti = scheduledList.length; i < ti; i += 1) {
                scheduledItem = scheduledList[i];
                programmeID = scheduledItem.getProgrammeID();
                if (programmeID && (programmeID === data.programmeID)) {
                    foundItem = scheduledItem;
                    break;
                }
            }
            if (foundItem) {
                dsrvRecordings2.removeScheduledRecording({
                    items: [scheduledItem],
                    finished: function () {
                    }
                });
            }
        };
        cCPgInfoHub.prototype._changeChannelGroup = function (aGroup) {
            var retObj = hx.svc.CH_LIST.getChlist({
                group: aGroup
            });
            if (retObj.chList.length > 0) {
                var list = [];
                if (ENV.op === ENV.listOP.UKDTT) {
                    var listChannel = retObj.chList;
                    var channel = null;
                    for (var i = 0, len = listChannel.length; i < len; i++) {
                        channel = listChannel[i];
                        if (!channel.hidden) {
                            list.push(channel);
                        }
                    }
                }
                else {
                    list = retObj.chList;
                }
                this._infoHub.setChannelListData(list);
                var ch = (retObj.chList.length === 0) ? null : this._currentChannel;
                var pra = {
                    group: aGroup,
                    channel: ch
                };
                this._draw_ChInfo(pra, 'channelList', true);
            }
            retObj = null;
        };
        cCPgInfoHub.prototype._createOptListMenu = function () {
            var that = this, itemList = [], groupList = [], data, i, length, startIndex, groupName;
            var bIsFavroureGroup = false, ulCurGroupDB, count;
            for (i = 0, length = that._groupDB.length; i < length; i += 1) {
                count = hx.svc.CH_LIST.getChannelCount({
                    group: that._groupDB[i]
                }).chCnt;
                if ((hx.config.useAlphabetGroup === 1) && (that._groupDB[i] === 'A-Z')) {
                    count = hx.svc.CH_LIST.getChannelCount({
                        group: 'AGALL'
                    }).chCnt;
                }
                if (count > 0) {
                    bIsFavroureGroup = that._optListMenu_checkFavourite(that._groupDB[i]);
                    if (bIsFavroureGroup === true) {
                        groupName = hx.svc.CH_LIST.getFavListName({
                            favtype: that._groupDB[i]
                        });
                        if (groupName) {
                            that._strChannelGroupName[i] = groupName;
                        }
                    }
                    else {
                        groupName = that._strChannelGroupName[i];
                    }
                    data = {
                        text: groupName,
                        msg: '',
                        value: i
                    };
                    itemList.push(data);
                    groupList.push(that._groupDB[i]);
                }
            }
            ulCurGroupDB = that._currentGroupDB;
            if (hx.config.useAlphabetGroup === 1) {
                ulCurGroupDB = that._ag_checkAlphabetGroup(ulCurGroupDB);
            }
            startIndex = groupList.indexOf(ulCurGroupDB);
            var relationTarget = '#channelListOptBtn';
            var contextMenu = hx.ui.ContextMenu({
                compose: this,
                renderTo: $(that.$container),
                relation: $(relationTarget),
                direction: 'up',
                startIndex: startIndex,
                items: itemList,
                onOk: function (index, argObject) {
                    var dbIndex = that._strChannelGroupName.indexOf(argObject.text);
                    var bChgAlphabetGroup = that._ag_getSupportAlphabetChlist(dbIndex);
                    if (bChgAlphabetGroup === false) {
                        that._changeChannelGroup(that._groupDB[dbIndex]);
                        $('#channelListOptBtn').find('em').text(argObject.text);
                    }
                    this.onCancel();
                    if (bChgAlphabetGroup === true) {
                        // channel Alphabet group select
                        hx.ui.overlay.left($('#-test-infoHub'));
                        that._alphabetgroupContextMenu = that._ag_createOptAlphabetListMenu();
                        var $detach = $('#channelListOptBtn').detach();
                        $('#-test-infoHub').after($detach);
                    }
                },
                onCancel: function () {
                    this.destroy();
                    hx.ui.overlay.hide();
                    that._groupContextMenu = null;
                },
                onHideOption: function () {
                    this.destroy();
                    hx.ui.overlay.hide();
                    that._groupContextMenu = null;
                },
                onKeyUpdated: function () {
                    that._resetTimer();
                }
            });
            contextMenu.show({});
            return contextMenu;
        };
        cCPgInfoHub.prototype._destroyGroupListMenu = function () {
            if (this._groupContextMenu) {
                hx.ui.overlay.hide();
                this._groupContextMenu.destroy();
                this._groupContextMenu = undefined;
            }
        };
        cCPgInfoHub.prototype._getLastGroupStr = function (aGroup) {
            var that = this, i, lastGroup;
            if (aGroup) {
                lastGroup = aGroup;
            }
            else {
                var curObj = hx.svc.CH_LIST.getCurrentChannel();
                lastGroup = curObj.group;
            }
            for (i = 0; i < that._groupDB.length; i += 1) {
                if (lastGroup === that._groupDB[i]) {
                    return that._strChannelGroupName[i];
                }
            }
            if (hx.config.useAlphabetGroup === 1) {
                for (i = 0; i < that._alphabetGroupDB.length; i += 1) {
                    if (lastGroup === that._alphabetGroupDB[i]) {
                        var szAlphabetText = hx.l('LOC_A_Z_ID');
                        return szAlphabetText;
                    }
                }
            }
            return that._strChannelGroupName[0];
            // exception
        };
        cCPgInfoHub.prototype._setCurrentChannel = function (aChannel) {
            this._currentChannel = aChannel;
        };
        cCPgInfoHub.prototype._on_ChannelListUpdate = function () {
            if (this._bActivate) {
                this._hide();
            }
        };
        cCPgInfoHub.prototype._init_Data = function () {
            var that = this, i = 0, bIsFavGroup = false, groupName;
            for (i = 0; i < hx.config.CPgInfoHub.chGroup.groupName.length; i += 1) {
                that._strChannelGroupName[i] = hx.l(hx.config.CPgInfoHub.chGroup.groupName[i]);
            }
            for (i = 0, length = that._groupDB.length; i < length; i += 1) {
                bIsFavGroup = that._optListMenu_checkFavourite(that._groupDB[i]);
                if (bIsFavGroup) {
                    groupName = hx.svc.CH_LIST.getFavListName({
                        favtype: that._groupDB[i]
                    });
                    if (groupName) {
                        that._strChannelGroupName[i] = groupName;
                    }
                }
            }
            for (i = 0; i < hx.config.CPgInfoHub.chGroup.favlistName.length; i += 1) {
                that._favlistname[i] = hx.l(hx.config.CPgInfoHub.chGroup.favlistName[i]);
            }
            // Init. Alphabet List Name
            if (hx.config.useAlphabetGroup === 1) {
                for (i = 0; i < hx.config.CPgInfoHub.chGroup.alphabetGroupName.length; i += 1) {
                    if (hx.config.CPgInfoHub.chGroup.alphabetGroupsDB[i] === 'AGALL') {
                        that._strAlphabetGroupName[i] = hx.l(hx.config.CPgInfoHub.chGroup.alphabetGroupName[i]);
                    }
                    else {
                        that._strAlphabetGroupName[i] = hx.config.CPgInfoHub.chGroup.alphabetGroupName[i];
                    }
                }
            }
        };
        cCPgInfoHub.prototype._optListMenu_checkFavourite = function (aParam) {
            var ret = false;
            if (aParam === 'Favourite1') {
                ret = true;
            }
            else if (aParam === 'Favourite2') {
                ret = true;
            }
            else if (aParam === 'Favourite3') {
                ret = true;
            }
            else if (aParam === 'Favourite4') {
                ret = true;
            }
            else if (aParam === 'Favourite5') {
                ret = true;
            }
            else {
                ret = false;
            }
            return ret;
        };
        //------------------------------------------------------------------------------------------------------
        // Alphabet Group List Function
        //------------------------------------------------------------------------------------------------------
        cCPgInfoHub.prototype._ag_createOptAlphabetListMenu = function () {
            var that = this, itemList = [], itemListView = [], groupList = [], data;
            var i, length, startIndex, groupName;
            var bIsFavroureGroup = false, ulCurIndex = 0, curObject, count;
            for (i = 0, length = that._alphabetGroupDB.length; i < length; i += 1) {
                count = hx.svc.CH_LIST.getChannelCount({
                    group: that._alphabetGroupDB[i]
                }).chCnt;
                if (count > 0) {
                    groupName = that._strAlphabetGroupName[i];
                    data = {
                        text: groupName,
                        msg: '',
                        value: i
                    };
                    itemList.push(data);
                    groupList.push(that._alphabetGroupDB[i]);
                }
            }
            startIndex = groupList.indexOf(that._currentGroupDB);
            if (startIndex < 0) {
                startIndex = 0;
            }
            var contextMenu = hx.ui.ContextMenu({
                compose: this,
                renderTo: $(that.$container),
                relation: $('#channelListOptBtn'),
                direction: 'up',
                startIndex: startIndex,
                items: itemList,
                mode: 'scroll',
                column: 11,
                onOk: function (index, argObject) {
                    var dbIndex = that._strAlphabetGroupName.indexOf(argObject.text);
                    var szAlphabetText = hx.l('LOC_A_Z_ID');
                    that._ag_changeAlphabetChannelGroup(that._alphabetGroupDB[dbIndex]);
                    $('#channelListOptBtn').find('em').text(szAlphabetText);
                    this.onCancel();
                },
                onCancel: function () {
                    this.destroy();
                    hx.ui.overlay.hide();
                    that._ag_refreshOptAlphabetBtn();
                    that._alphabetgroupContextMenu = null;
                },
                onHideOption: function () {
                    this.destroy();
                    hx.ui.overlay.hide();
                    that._ag_refreshOptAlphabetBtn();
                    that._alphabetgroupContextMenu = null;
                },
                onKeyUpdated: function (param) {
                    that._resetTimer();
                }
            });
            contextMenu.show({});
            return contextMenu;
        };
        cCPgInfoHub.prototype._ag_destroyAlphabetGroupListMenu = function () {
            if (this._alphabetgroupContextMenu) {
                hx.ui.overlay.hide();
                this._ag_refreshOptAlphabetBtn();
                this._alphabetgroupContextMenu.destroy();
                this._alphabetgroupContextMenu = undefined;
            }
        };
        cCPgInfoHub.prototype._ag_refreshOptAlphabetBtn = function () {
            //var $detach = $('#channelListOptBtn').detach();
            //$('#channelList').after($detach);
        };
        cCPgInfoHub.prototype._ag_getSupportAlphabetChlist = function (aParam) {
            var bReturn = false;
            if (hx.config.useAlphabetGroup === 1) {
                if (this._groupDB[aParam] === 'A-Z') {
                    bReturn = true;
                }
                else {
                    bReturn = false;
                }
            }
            else {
                bReturn = false;
            }
            return bReturn;
        };
        cCPgInfoHub.prototype._ag_checkAlphabetGroup = function (aParam) {
            var i = 0;
            for (i = 0; i < this._alphabetGroupDB.length; i += 1) {
                if (this._alphabetGroupDB[i] === aParam) {
                    return 'A-Z';
                }
            }
            return aParam;
        };
        cCPgInfoHub.prototype._ag_changeAlphabetChannelGroup = function (aGroup) {
            var retObj;
            retObj = hx.svc.CH_LIST.getChlist({
                group: aGroup
            });
            if (retObj.chList.length > 0) {
                this._infoHub.setChannelListData(retObj.chList);
                var ch = (retObj.chList.length === 0) ? null : retObj.chList[0];
                var pra = {
                    group: aGroup,
                    channel: ch
                };
                this._draw_ChInfo(pra, 'channelList', true);
            }
            retObj = null;
        };
        return cCPgInfoHub;
    })(__PAGE_KERNEL__);
    hx.al.CPgInfoHub = cCPgInfoHub;
    return cCPgInfoHub;
});
