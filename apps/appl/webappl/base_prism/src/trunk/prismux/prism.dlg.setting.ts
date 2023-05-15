/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.setting.js
/*jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
declare var notify: Function;

import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    var KItemHeight = 58;
    var KItemHeightFull = 70;
    var KExpendWidth = 350;
    var KMargin = 50;
    var KListTop = 100;
    var mHidingTimer = null;
    var findChannel = function (ccid, channelGroupList) {
        var i, leni, j, lenj, group, channelList, channel;
        for (i = 0, leni = channelGroupList.length; i < leni; i++) {
            group = channelGroupList[i];
            channelList = group.channelList;
            for (j = 0, lenj = channelList.length; j < lenj; j++) {
                channel = channelList[j];
                if (channel.ccid == ccid) {
                    return {
                        channel: channel,
                        groupIndex: i,
                        channelIndex: j
                    };
                }
            }
        }
    };
    var findChannelInGroup = function (ccid, channelList) {
        var j, lenj, channel;
        for (j = 0, lenj = channelList.length; j < lenj; j++) {
            channel = channelList[j];
            if (channel.ccid == ccid) {
                return {
                    channel: channel,
                    channelIndex: j
                };
            }
        }
    };
    var fnMatchDate = function (date1, date2) {
        var temp1 = new Date(date1);
        var temp2 = new Date(date2);
        temp1.setMilliseconds(0);
        temp1.setSeconds(0);
        temp1.setMinutes(0);
        temp1.setHours(0);
        temp2.setMilliseconds(0);
        temp2.setSeconds(0);
        temp2.setMinutes(0);
        temp2.setHours(0);
        return temp1.getTime() == temp2.getTime();
    };
    var fnPropertyToString = function (item, textPropertyName) {
        var propertyList = [], toString;
        if (textPropertyName instanceof Array) {
            var i, ti, property;
            for (i = 0, ti = textPropertyName.length; i < ti; i += 1) {
                property = textPropertyName[i];
                if (property === 'no') {
                    propertyList.push(prism.makeChannelNumberString(item[property]));
                } else {
                    propertyList.push(item[property]);
                }
            }
            toString = propertyList.join(' ');
        } else {
            toString = item[textPropertyName];
        }
        return toString;
    };
    var fnDrawCarousel = function (a, param, data, startIndex, textPropertyName, handler) {
        var $target = param.$item;
        var settingItemInfo = param.item;
        var settingItem = settingItemInfo.settingItem;
        var offset = $target.offset();
        var dialogTop = a._$self.offset().top;
        var settingListViewTop = a._settingListView._$self.position().top
        var top = dialogTop + settingListViewTop - a._itemHeight;
        var left = offset.left + $target.width();
        var carousel = new CCarouselControl(null);
        var $selectionName = null;
        var $selectionAnchorName = null;
        carousel.addClass('selectionListView');
        carousel.addClass(param.item.settingItem.cls || '');
        carousel.setData(data);
        carousel.setDataDrawer(function (param) {
            var item = param.item;
            var $item = param.$item;
            var index = param.key;
            var $selectionItem = $item.children('.selectionItem');
            if (!$selectionItem.length) {
                $selectionItem = $('<div>', {'class': 'selectionItem'})
                    .append($('<span>', {'class': 'selectionName'}))
                    .appendTo($item);
            }
            $selectionName = $selectionItem.children('.selectionName').text(fnPropertyToString(item, textPropertyName));
            if (settingItemInfo.changed) {
                if (index === startIndex) {
                    $selectionItem.addClass('active');
                }
            }
            $selectionName.addClass(item.cls || '');
        });
        carousel.setAnchorDrawer(function (param) {
            var item = param.currentItem.data;
            var $item = param.$anchor;
            var $selectionAnchorItem = $item.children('.selectionItem');
            if (!$selectionAnchorItem.length) {
                $selectionAnchorItem = $('<div>', {'class': 'selectionItem'})
                    .append($('<span>', {'class': 'selectionName'}))
                    .appendTo($item);
            }
            $selectionAnchorName = $selectionAnchorItem.children('.selectionName').text(fnPropertyToString(item, textPropertyName));
            $selectionAnchorName.addClass(item.cls || '');
        });
        carousel.setDrawParam({
            viewCount: a._viewCount + 2,
            anchorIndex: Math.round((offset.top - dialogTop - settingListViewTop) / a._itemHeight) + 1, //
            itemHeight: a._itemHeight,
            animationInterval: 0.2,
            maxKeyQueueCount: 3,
            anchorHeight: a._itemHeight,
            transparentAnchor: true,
            applyOnce: {
                drawEffect: 'spreadOut',
                visibleAfterDraw: true
            }
        });
        carousel.onFocusStartToChange = function (param) {
            $selectionAnchorName.removeClass().addClass('selectionName ' + param.item.cls);
            $selectionAnchorName.text(fnPropertyToString(param.item, textPropertyName));
        };
        carousel.onItemSelected = handler;
        carousel.setCurrentIndex(startIndex === undefined ? 0 : startIndex);
        carousel._$self.css({
            'width': '320px',
        });
        a.setChildControl([carousel]);
        if (a._fullPage) {
            a.setDrawParam({
                childAlign: null,
                padding: 0,
                margins: [100, 0, 60, a._prevWidth]
            });
        } else {
            a.setDrawParam({
                align: 'hcenter',
                childAlign: null,
                padding: 0,
                margins: [100, 0, 60, a._prevWidth]
            });
        }
        var i, len, $carousel, width;
        for (i = 0, len = a._selectionList.length; i < len; i++) {
            a._selectionList[i].destroy();
        }
        a._selectionList = [];
        a.draw({
            setActiveFocus: true
        });
        carousel._$self.css({
            'position': 'fixed',
            'top': top + 'px',
            'left': left + 'px',
            'width': '320px',
        });
        a._selectionList.push(carousel);
        a._childControlReplaced = true;
    };
    var fnDrawSelectionList = function (a, param) {
        var settingItemInfo = param.item;
        var settingItem = settingItemInfo.settingItem;
        var startIndex = settingItemInfo.changed ? settingItemInfo.changedValue : settingItem.config.activeIndex;
        fnDrawCarousel(a, param, //
            settingItem.config.selectionList, //
            startIndex,
            'text',
            function (param) {
                fnApplyChanged(a, settingItemInfo, settingItem.config.activeIndex == param.key, param.key);
            });
    };
    var fnDrawChannelList = function (a, param) {
        'use strict';
        var settingItemInfo = param.item;
        var settingItem = settingItemInfo.settingItem;
        fnDrawCarousel(a, param, settingItem.config.channelGroupList, undefined, 'groupName', function (groupSelectedParam) {
            var ccid = settingItemInfo.changed ? settingItemInfo.changedValue : settingItem.config.ccid;
            var startIndex = 0, channel;
            if (ccid) {
                channel = findChannelInGroup(ccid, groupSelectedParam.item.channelList);
                if (channel) {
                    startIndex = channel.channelIndex;
                }
            }
            fnDrawCarousel(a, param, groupSelectedParam.item.channelList, startIndex, ['no', 'name'], function (param) {
                fnApplyChanged(a,
                    settingItemInfo, //
                        settingItem.config.ccid == param.item.ccid, //
                    param.item.ccid);
            });
        });
    };
    var fnDrawDateList = function (a, param) {
        var settingItemInfo = param.item;
        var settingItem = settingItemInfo.settingItem;
        var i, dateList = [];
        var date = new Date();
        var startIndex = 0;
        for (i = 0; i < 30 * 6; i++) {
            dateList.push({
                date: date,
                dateText: prism.getDateText(date)
            });
            if (settingItemInfo.changed && fnMatchDate(settingItemInfo.changedValue, date)) {
                startIndex = i;
            }
            date = new Date(date.getTime() + 24 * 60 * 60 * 1000);
        }
        fnDrawCarousel(a, param, dateList, startIndex, 'dateText', function (dateListItem) {
            fnApplyChanged(a,
                settingItemInfo,
                fnMatchDate(settingItem.config.date, dateListItem.item.date),
                dateListItem.item.date
            );
        });
    };
    var fnHandleSettingItemSelection = function (a, param) {
        var type = param.item.settingItem.type;
        if (type === 'custom') {
            var $item = param.$item;
            var $settingItem = $item.children('.settingItem');
            if (a.onSettingsItemSelected) {
                a.onSettingsItemSelected(param, function (changedValue) {
                    $settingItem.children('.settingValue').text(changedValue).addClass('changed');
                });
            }
            return;
        }
        if (type) {
            var fnDraw = function () {
                a._$self.append($('<div>').addClass('dimm'));
                var prepareNewControls = function () {
                    param.$item.css('z-index', 1);
                    // back up main control key map context
                    a._mainKeyContext = {
                        controls: a._controls,
                        currentIndex: a._currentIndex,
                        keyMap: a._keyMap
                    };
                };
                var handler = {
                    'custom': function () {
                    },
                    'multiSelection': function () {
                        prepareNewControls();
                        fnDrawSelectionList(a, param);
                    },
                    'channel': function () {
                        prepareNewControls();
                        fnDrawChannelList(a, param);
                    },
                    'date': function () {
                        prepareNewControls();
                        fnDrawDateList(a, param);
                    },
                    'time': function () {
                        var timeQueryDlg = prism.createQueryTimeDlg(a._parent);
                        timeQueryDlg.setTextHtml(tr('LOC_REC_ENTERSTARTTIME_ID'));
                        timeQueryDlg.onTimeEntered = function (hours, minutes) {
                            var item = param.item;
                            var originalTime = item.settingItem.config.time;
                            if (originalTime.getHours() == hours && originalTime.getMinutes() == minutes) {
                                item.changed = false;
                                item.changedValue = undefined;
                                timeQueryDlg.close();
                            } else {
                                var changedTime = new Date(item.settingItem.config.time);
                                changedTime.setHours(hours);
                                changedTime.setMinutes(minutes);
                                item.changed = true;
                                item.changedValue = changedTime;
                                timeQueryDlg.close();
                            }
                        };
                        timeQueryDlg.open({
                            onClosed: function () {
                                fnClearSelectionList(a, true, param.itemKey);
                            }
                        });
                    },
                    'number': function () {
                        a._settingOnGoing = true;
                        var item = param.item;
                        var numberQueryDlg = prism.createQueryNumberDlg(a._parent, item.settingItem.config);
                        numberQueryDlg.setTextHtml(item.settingItem.string);
                        numberQueryDlg.setTrailText(item.settingItem.config.unitText);
                        numberQueryDlg.onNumberEntered = function (number) {
                            if (a.onInputChanged) {
                                if (!a.onInputChanged(number)) {
                                    return;
                                }
                            }
                            var originalDuration = item.settingItem.config.duration;
                            if (originalDuration == number) {
                                item.changed = false;
                                item.changedValue = undefined;
                            } else {
                                item.changed = true;
                                item.changedValue = number;
                            }
                            if (a._onItemSelected) {
                                a._onItemSelected({
                                    index: item.index,
                                    settingItem: item.settingItem,
                                    changedValue: item.changedValue
                                });
                            }
                            numberQueryDlg.close();
                        };
                        numberQueryDlg.open({
                            onClosed: function () {
                                fnClearSelectionList(a, true, param.itemKey);
                                if (param.item.settingItem.cls === 'userinputsymbolRate') {
                                    if (param.item.changedValue) {
                                        a._$self.find('.settingItem .settingValue').eq(param.targetItemKey).text(param.item.changedValue).addClass('changed');
                                    }
                                }
                            }
                        });
                        numberQueryDlg.setFocusRight();
                    }
                }[type]();
                a._settingOnGoing = true;
                a.setTransition(false);
            };
            a._prevLeft = a._$self.position().left;
            a._prevWidth = a._$self.width();
            if (a._fullPage) {
                a._$self.css({
                    'width': 1280 + 'px'
                });
                fnDraw();
            } else {
                a.addClass('transition');
                a.setTransition(true);
                setTimeout(function () {
                    a._$self.css({
                        'left': (a._prevLeft - KExpendWidth / 2) + 'px',
                        'width': a._prevWidth + KExpendWidth + 'px'
                    }).afterTransitionOneShot(fnDraw);
                }, 1);
            }
        }
    };
    var fnApplyChanged = function (a, settingItemInfo, changed, changedValue) {
        var refresh = false;
        if (settingItemInfo.settingItem.updateAlways) {
            if (a._onItemSelected) {
                a._onItemSelected({
                    index: settingItemInfo.index,
                    settingItem: settingItemInfo.settingItem,
                    changedValue: changedValue
                });
            }
            settingItemInfo.changed = true;
            settingItemInfo.changedValue = changedValue;
            refresh = true;
        } else {
            if (changed) {
                settingItemInfo.changed = false;
                settingItemInfo.changedValue = undefined;
                refresh = true;
            } else {
                if (a.onItemChanged) {
                    a.onItemChanged(settingItemInfo.index, settingItemInfo.settingItem, changedValue);
                }
                settingItemInfo.changed = true;
                settingItemInfo.changedValue = changedValue;
                refresh = true;
            }
        }
        fnClearSelectionList(a, refresh, settingItemInfo.index);
    };
    var fnClearSelectionList = function (a, refresh?, index?) {
        if (a._settingOnGoing) {
            a._settingList.getFocusedItem().$item.css('z-index', '');
            if (a._childControlReplaced) {
                a.destroyChildControl();
                delete a._childControlReplaced;
            }
            var fnDraw = function () {
                a._$self.children('.dimm').remove();
                if (a._mainKeyContext) {
                    a._controls = a._mainKeyContext.controls;
                    a._currentIndex = a._mainKeyContext.currentIndex;
                    a._keyMap = a._mainKeyContext.keyMap;
                    delete a._mainKeyContext;
                }
                if (a._fullPage) {
                    a.setDrawParam({
                        childAlign: 'hcenter',
                        padding: 20,
                        margins: [33, 50, 60]

                    });
                } else {
                    a.setDrawParam({
                        align: 'hcenter',
                        childAlign: 'hcenter',
                        padding: 33,
                        margins: [33, 50, 60]
                    });
                }
                a._settingListItemInfoProvider.updateItem(index);
                a._settingOnGoing = false;
                a.setTransition(false);
            };
            if (a._fullPage) {
                a._$self.css({
                    'width': a._prevWidth + 'px'
                });
                fnDraw();
            } else {
                a.setTransition(true);
                a._$self.css({
                    'left': a._prevLeft + 'px',
                    'width': a._prevWidth + 'px'
                }).afterTransitionOneShot(fnDraw);
            }
            return true;
        }
        return false;
    };
    var fnGetAudioLanguage = function (item, index) {
        var szAudioLang = null;
        if (item !== null) {
            szAudioLang = hx.il.iso639toString(item);
            if (szAudioLang.length <= 3) {
                szAudioLang = hx.l('LOC_SOUND_ID') + ' ' + index;
            }
        } else {
            szAudioLang = hx.l('LOC_SOUND_ID') + ' ' + index;
        }
        return szAudioLang;
    };
    var fnGetChannelText = function (channel) {
        return prism.makeChannelNumberString(channel.no) + ' ' + channel.name;
    };
    export class CSettingDlg extends __PRISM_CDIALOG__.CDialog {
        private _statusInfoList: any[];
        private _itemHeight: number;
        private _selectionList: any[];
        private _fullPage: any;
        private _settingOnGoing: boolean;
        private _settingList: __CONTROLS__.CListControl;
        private _settingListView: any;
        private _$date: JQuery;
        private _$time: JQuery;
        private _$title: JQuery;
        private _$titleArea: JQuery;
        private _$infoArea: JQuery;
        private handleSettingItemSelection: Function;
        private _settingListItemInfoProvider: __CONTROLS__.CDataProvider;
        private _viewPage;
        private _viewCount: number;
        _timeout: number;
        _settingListItemInfos: any;
        _onItemSelected: Function;
        onSettingsFocusChanged: Function;
        onOk: Function;

        constructor(layeredGroupControl) {
            super(layeredGroupControl);
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl);
            a.addClass('settingDlg');
            a._buttonInfoList = [
                {
                    text: tr('LOC_OK_ID')
                }
            ];
            a._statusInfoList = [];
            a._onItemSelected = null;
            a._itemHeight = KItemHeight;
            a._selectionList = [];
            a.handleSettingItemSelection = function (a, param) {
                fnHandleSettingItemSelection(a, param);
            };
        }
        destroy() {
            var a = this;
            a._timeout = null;
            a._buttonInfoList = null;
            a._statusInfoList = null;
            a._onItemSelected = null;
            super.destroy.call(a);
        }
        setFullPage() {
            var a = this;
            a._fullPage = true;
            a._$self.addClass('fullPage');
            a.setDrawParam('width', 880);
            a._itemHeight = KItemHeightFull;
        }
        setViewPage() {
            var a = this;
            a.setFullPage();
            a._$self.addClass('view');
            a._viewPage = true;
        }
        setTitleHtml(titleHtml) {
            var a = this;
            if (a._viewPage) {
                if (!$titleArea) {
                    var $title = $('<div>', {
                        'class': 'title'
                    });
                    var $date = $('<div>', {
                        'class': 'date'
                    });
                    var $time = $('<div>', {
                        'class': 'time'
                    });
                    var $titleArea = $('<div>', {
                        'class': 'titleArea'
                    }).append($title).append($('<div>', {
                        'class': 'dateTime'
                    }).append($date).append($time));
                    $title.html(titleHtml);
                    a._$date = $date;
                    a._$time = $time;
                    a._$title = $title;
                    a._$titleArea = $titleArea;
                    a.showDateTime();
                }
                else {
                    a._$title.html(titleHtml);
                }
            }
            else {
                var $titleArea = $('<div>', {
                    'html': titleHtml
                });
                a._$titleArea = $titleArea;
            }
        }
        showDateTime() {
            var a = this;
            var now = new Date();
            a.setDateText(prism.getDateText(now));
            a.setTimeText(prism.getTimeText(now));
            prism.clock.addEventHandler('UpdateDateText', a, function (dateText) {
                a.setDateText(dateText);
            });
            prism.clock.addEventHandler('UpdateTimeText', a, function (timeText) {
                a.setTimeText(timeText);
            });
        }
        setDateText(text) {
            var a = this;
            a._$date.text(text);
        }
        setTimeText(text) {
            var a = this;
            a._$time.text(text);
        }
        setStatusInfoList(statusInfoList) {
            var a = this;
            a._statusInfoList = statusInfoList;
        }
        setOnItemSelected(fn) {
            var a = this;
            a._onItemSelected = fn;
        }
        setButtonInfoList(buttonInfoList) {
            var a = this;
            a._buttonInfoList = buttonInfoList;
        }
        setInfoHtml(infoHtml) {
            var a = this;
            if (!a._$infoArea) {
                var $infoArea = $('<div>', {
                    'class': 'infoArea'
                });
                $infoArea.html(infoHtml);
                a._$self.append($infoArea);
                a._$infoArea = $infoArea;
            }
            else {
                a._$infoArea.html(infoHtml);
            }
        }
        setSettingListItems(settingListItems) {
            var a = this;
            var i, len;
            var settingListItemInfos = [];
            for (i = 0, len = settingListItems.length; i < len; i++) {
                settingListItemInfos.push({
                    settingItem: settingListItems[i],
                    changed: false,
                    changedValue: undefined,
                    index: i
                });
            }
            a._settingListItemInfos = settingListItemInfos;
        }
        setTvOptionListItemsByData(data) {
            var a = this;
            var i, len = 0, tempIdx = 0, audioLen = 0, subtitleLen = 0, supportedSubtitleList = [], supportedAudioList = [], supportedResList = [], displayList = [];
            var displayIdx = 0, audioIdx = 0, subtitleIdx = 0, resIdx = 0;
            var avInfo = data.avInfo;
            var pictureInfo = data.pictureInfo;
            var supportedSubtitles = data.supportedSubtitles;
            var language = data.language;
            var AD = data.AD;
            var bMedia = data.bMedia;
            var bDisplaySubttl = data.bSubttlDisp;
            var repeatIdx = data.repeatOption;
            // --------------- resolution --------------- //
            var fnMakeResolutionItem = function () {
                var resLen = pictureInfo.supportedResolutions.length;
                for (i = 0; i < resLen; i += 1) {
                    var data = {
                        cls: '',
                        text: pictureInfo.supportedResolutions[i],
                        value: pictureInfo.supportedResolutions[i]
                    };
                    supportedResList.push(data);
                    if (pictureInfo.curResolution === pictureInfo.supportedResolutions[i]) {
                        resIdx = i;
                    }
                }
            };
            // --------------- display format --------------- //
            var fnMakeDisplayFormatItem = function () {
                pictureInfo.tvAspectRatio = (pictureInfo.tvAspectRatio !== '4:3' && pictureInfo.tvAspectRatio !== '16:9') ? '16:9' : pictureInfo.tvAspectRatio;
                switch (pictureInfo.videoDisplayFormat) {
                    case 'Auto':
                        displayIdx = 0;
                        pictureInfo.videoDisplayFormat = hx.l('LOC_AUTO_ID');
                        break;
                    case 'Sqeeze':
                        displayIdx = 1;
                        pictureInfo.videoDisplayFormat = (pictureInfo.tvAspectRatio === '16:9') ? hx.l('LOC_PILLARBOX_ID') : hx.l('LOC_LETTERBOX_ID');
                        break;
                    case 'CenterCutOut':
                        displayIdx = 2;
                        pictureInfo.videoDisplayFormat = (pictureInfo.tvAspectRatio === '16:9') ? hx.l('LOC_ZOOM_ID') : hx.l('LOC_CENTER_ID');
                        break;
                }
            };
            // --------------- audio --------------- //
            var fnMakeAudioItem = function () {
                var audio = avInfo.audio;
                var szIsoLang = null;
                var szIsoLang2 = null;
                var szAudioLang = null;
                var al = null;
                if (audio.list !== null) {
                    audioLen = audio.list.length;
                    for (i = 0; i < audioLen; i += 1) {
                        al = audio.list[i];
                        tempIdx = i + 1;
                        if (al.language2 === null) {
                            szIsoLang = fnGetAudioLanguage(al.language, tempIdx)
                            szAudioLang = szIsoLang;
                        } else {
                            szIsoLang = fnGetAudioLanguage(al.language, tempIdx)
                            szIsoLang2 = fnGetAudioLanguage(al.language2, tempIdx)
                            szAudioLang = szIsoLang + ' / ' + szIsoLang2;
                        }
                        var dataA = {
                            cls: '',
                            text: szAudioLang,
                            value: al
                        };
                        if (al.encoding === "E-AC3") {
                            dataA.cls = 'ac3p';
                        } else if (al.encoding === "AC3") {
                            dataA.cls = 'ac3';
                        }
                        supportedAudioList.push(dataA);
                        prism.log(hx.util.format('[doCreateTvOption]------------------Audio component : {0}', i));
                        prism.log(hx.util.format('[doCreateTvOption] componentTag : {0}', al.componentTag));
                        prism.log(hx.util.format('[doCreateTvOption] encoding : {0}', al.encoding));
                        prism.log(hx.util.format('[doCreateTvOption] pid : {0}', al.pid));
                        prism.log(hx.util.format('[doCreateTvOption] subType : {0}', al.subType));
                        prism.log(hx.util.format('[doCreateTvOption] type : {0}', al.type));
                        prism.log(hx.util.format('[doCreateTvOption] language : {0}', al.language));
                    }
                }
                if (audio.index !== null) {
                    audioIdx = audio.index;
                }
            };
            // --------------- Audio Description --------------- //
            var fnMakeAudioDescriptionItem = function () {
                supportedAudioList.push({
                    text: tr('LOC_OFF_ID'),
                    value: 0
                });
                supportedAudioList.push({
                    text: tr('LOC_ON_ID'),
                    value: 1
                });
                audioIdx = AD? 1 : 0;
            };
            // --------------- subtitle OnOff--------------- //
            var fnMakeSubtitleOnOffItem = function () {
                supportedSubtitleList.push({
                    text: tr('LOC_OFF_ID'),
                    value: 0
                });
                supportedSubtitleList.push({
                    text: tr('LOC_ON_ID'),
                    value: 1
                });
                subtitleIdx = supportedSubtitles? 1 : 0;

            };
            // --------------- subtitle --------------- //
            var fnMakeSubtitleItem = function () {
                if (bMedia) {
                    supportedSubtitleList.push({
                        text: tr('LOC_OFF_ID'),
                        value: '-1'
                    });
                    if (supportedSubtitles) {
                        subtitleLen = supportedSubtitles.length;
                        var lang;
                        for (i = 0; i < subtitleLen; i += 1) {
                            lang = supportedSubtitles.item(i);
                            if (lang === language) {
                                subtitleIdx = i + 1;
                            }
                            supportedSubtitleList.push({
                                text: hx.il.getSubtitleLanguageText(lang, i + 1),
                                value: lang
                            });
                        }
                    }
                } else {
                    var subtitle = avInfo.subtitle;
                    if (subtitle.list !== null && subtitle.list.length > 0) {
                        subtitle.list.splice(0, 0, null);
                        // add off
                        if (subtitle.index !== null) {
                            subtitle.index += 1;
                        }
                    }
                    if (subtitle.selected !== null) {
                        var selLang = subtitle.selected.language;
                        tempIdx = subtitle.index + 1;
                    }
                    if (subtitle.list !== null) {
                        len = subtitle.list.length;
                        for (i = 0; i < len; i += 1) {
                            var sl = subtitle.list[i];
                            var langS;
                            if (sl === null) {
                                langS = hx.l('LOC_OFF_ID');
                                sl = '';
                            } else {
                                tempIdx = i + 1;
                                langS = (sl.language !== null) ? hx.il.iso639toString(sl.language) : hx.l('LOC_SUBTITLE_ID') + ' ' + tempIdx;
                                prism.log(hx.util.format('[doCreateTvOption]------------------subtitle component : {0}', i));
                                prism.log(hx.util.format('[doCreateTvOption] componentTag : {0}', sl.componentTag));
                                prism.log(hx.util.format('[doCreateTvOption] pid : {0}', sl.pid));
                                prism.log(hx.util.format('[doCreateTvOption] hearingImpaired : {0}', sl.hearingImpaired));
                                prism.log(hx.util.format('[doCreateTvOption] subType : {0}', sl.subType));
                                prism.log(hx.util.format('[doCreateTvOption] type : {0}', sl.type));
                                prism.log(hx.util.format('[doCreateTvOption] language : {0}', sl.language));
                            }
                            // DVB: 201, EBU: 202
                            var dataS = {
                                cls: '',
                                text: langS,
                                value: sl
                            };
                            if (sl) {
                                if (sl.subType === 201) {
                                    dataS.cls = 'subDVB';
                                } else if (sl.subType === 202) {
                                    dataS.cls = 'subEBU';
                                }
                                // support Hard of Hearing
                                if (sl.hearingImpaired === true) {
                                    dataS.cls = 'subHardofHearing';
                                }
                            }
                            supportedSubtitleList.push(dataS);
                        }
                    }
                    if (subtitle.index !== null) {
                        subtitleIdx = subtitle.index;
                    }
                }
            };

            if(ENV.op === ENV.listOP.UKDTT){
                if(bMedia == false)  {
                    fnMakeResolutionItem();
                    fnMakeDisplayFormatItem();
                    fnMakeAudioDescriptionItem();
                    fnMakeSubtitleOnOffItem();
                } else{
                    fnMakeResolutionItem();
                    fnMakeDisplayFormatItem();
                    fnMakeAudioDescriptionItem();
                    fnMakeSubtitleItem();
                }
            } else{
                fnMakeResolutionItem();
                fnMakeDisplayFormatItem();
                fnMakeAudioItem();
                fnMakeSubtitleItem();
            }

            var settingListItems = [
                {
                    type: 'multiSelection',
                    text: tr('LOC_RESOLUTION_ID'),
                    cls: 'resolution',
                    config: {
                        selectionList: supportedResList.length === 0 ? [
                            {
                                text: '',
                                value: null
                            }
                        ] : supportedResList,
                        activeIndex: resIdx
                    }
                },
                {
                    type: 'multiSelection',
                    text: tr('LOC_DISPLAYFORMAT_ID'),
                    cls: (pictureInfo.tvAspectRatio === '16:9') ? 'dp169' : 'dp43',
                    config: {
                        selectionList: [
                            {
                                cls: (pictureInfo.tvAspectRatio === '16:9') ? 'auto169' : 'auto43',
                                text: hx.l('LOC_AUTO_ID'),
                                value: 'Auto'
                            },
                            {
                                cls: (pictureInfo.tvAspectRatio === '16:9') ? 'pill' : 'lbox',
                                text: (pictureInfo.tvAspectRatio === '16:9') ? hx.l('LOC_PILLARBOX_ID') : hx.l('LOC_LETTERBOX_ID'),
                                value: 'Sqeeze'
                            },
                            {
                                cls: (pictureInfo.tvAspectRatio === '16:9') ? 'zoom' : 'cent',
                                text: (pictureInfo.tvAspectRatio === '16:9') ? hx.l('LOC_ZOOM_ID') : hx.l('LOC_CENTER_ID'),
                                value: 'CenterCutOut'
                            }
                        ],
                        activeIndex: displayIdx
                    }
                },
                {
                    type: 'multiSelection',
                    text: tr('LOC_AUDIO_LANGUAGE_ID'),
                    cls: 'audioLang',
                    config: {
                        selectionList: supportedAudioList.length === 0 ? [
                            {
                                text: tr('LOC_OFF_ID'),
                                value: null
                            }
                        ] : supportedAudioList,
                        activeIndex: audioIdx
                    }
                },
                {
                    type: 'multiSelection',
                    text: tr('LOC_SUBTITLE_DISPLAY_ID'),
                    cls: 'subtitleLang',
                    config: {
                        selectionList: supportedSubtitleList.length === 0 ? [
                            {
                                text: tr('LOC_OFF_ID'),
                                value: null
                            }
                        ] : supportedSubtitleList,
                        activeIndex: subtitleIdx
                    }
                }
            ];
            // --------------- repeatOption --------------- //
            if (bMedia) {
                settingListItems.push({
                    type: 'multiSelection',
                    text: tr('LOC_REPEAT_OPTION_ID'),
                    cls: 'repeatOpt',
                    config: {
                        selectionList: [
                            {
                                text: tr('LOC_REPEAT_ALL_ID'),
                                value: 0
                            },
                            {
                                text: tr('LOC_ONCE_ID'),
                                value: 1
                            },
                            {
                                text: tr('LOC_REPEAT_1_ID'),
                                value: 2
                            }
                        ],
                        activeIndex: repeatIdx
                    }
                });
            }
            if(ENV.op === ENV.listOP.UKDTT){
                settingListItems[2].text = tr('LOC_AUDIO_DESCRIPTION_ID');
            }

            var settingListItemInfos = [];
            for (i = 0, len = settingListItems.length; i < len; i++) {
                settingListItemInfos.push({
                    settingItem: settingListItems[i],
                    changed: false,
                    changedValue: undefined,
                    index: i
                });
            }
            a._settingListItemInfos = settingListItemInfos;
        }
        doOpen(param?) {
            var a = this;
            var childControl = [];
            var initialFocus = 0;
            var width = a._drawParam.width ? a._drawParam.width : 600;
            var childWidth = width - KMargin * 2;
            var verticalLayout: __CONTROLS__.CVerticalLayoutControl;
            a._$self.css('width', width + 'px');
            if (a._$titleArea) {
                verticalLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                verticalLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.removeClass('-f');
                        $item.css('width', childWidth + 'px');
                        $item.append(a._$titleArea);
                        return false;
                    }]);
                childControl.push(verticalLayout);
                initialFocus = 1;
            }
            var listControl = new __CONTROLS__.CListControl(null);
            listControl.setDrawParam({
                itemHeight: a._itemHeight
            });
            listControl.setDataDrawer(function (param) {
                var settingItemInfo = param.item;
                var settingItem = settingItemInfo.settingItem;
                var type = settingItem.type;
                var $item = param.$item;
                var $settingItem = $item.children('.settingItem');
                if ($settingItem.length == 0) {
                    $settingItem = $('<div>', {'class': 'settingItem'})
                        .append($('<span>', {'class': 'settingName'}))
                        .append($('<span>', {'class': 'settingValue'}));
                    $item.append($settingItem);
                }
                var $settingName = $settingItem.children('.settingName');
                var $settingValue = $settingItem.children('.settingValue');
                $item.css('z-index', '');
                $settingItem.children('.settigTitle').text(settingItem.text);
                var config = settingItem.config;
                if (type) {
                    var valueDrawer = {
                        'custom': function () {
                            $settingName.text(settingItem.text);
                            $settingName.addClass(settingItem.cls || '');
                            if (settingItemInfo.changed) {
                                $settingValue.text(settingItem.value).addClass('changed');
                            } else {
                                $settingValue.text(settingItem.value);
                            }
                        },
                        'multiSelection': function () {
                            $settingName.text(settingItem.text);
                            $settingName.addClass(settingItem.cls || '');
                            if (config.selectionList[config.activeIndex].value === null) {
                                $item.removeClass('-f').addClass('dis');
                            }
                            if (settingItemInfo.changed) {
                                if (config.selectionList[settingItemInfo.changedValue].value !== 'userdefine') {
                                    $settingValue.text(config.selectionList[settingItemInfo.changedValue].text).addClass('changed');
                                }
                            } else {
                                $settingValue.text(config.selectionList[config.activeIndex].text).removeClass('changed');
                            }
                        },
                        'channel': function () {
                            var channel, channelText;
                            $settingName.text(settingItem.text);
                            $settingName.addClass(settingItem.cls || '');
                            if (settingItemInfo.changed) {
                                $settingValue.text(fnGetChannelText(findChannel(settingItemInfo.changedValue, config.channelGroupList).channel)).addClass('changed');
                            } else {
                                if (config.ccid) {
                                    channel = findChannel(config.ccid, config.channelGroupList);
                                    if (channel) {
                                        channelText = fnGetChannelText(channel.channel);
                                    }
                                }
                                if (!channelText) {
                                    channelText = fnGetChannelText(config.channelGroupList[0].channelList[0]);
                                }
                                $settingValue.text(channelText).removeClass('changed');
                            }
                        },
                        'date': function () {
                            $settingName.text(settingItem.text);
                            $settingName.addClass(settingItem.cls || '');
                            if (settingItemInfo.changed) {
                                $settingValue.text(prism.getDateText(settingItemInfo.changedValue)).addClass('changed');
                            } else {
                                $settingValue.text(prism.getDateText(config.date)).removeClass('changed');
                            }
                        },
                        'time': function () {
                            $settingName.text(settingItem.text);
                            $settingName.addClass(settingItem.cls || '');
                            if (settingItemInfo.changed) {
                                $settingValue.text(settingItemInfo.changedValue.format("HH:MM")).addClass('changed');
                            } else {
                                $settingValue.text(config.time.format("HH:MM")).removeClass('changed');
                            }
                        },
                        'number': function () {
                            $settingName.text(settingItem.text);
                            $settingName.addClass(settingItem.cls || '');
                            var unitText = ' ' + (config.unitText ? config.unitText : '');
                            if (settingItemInfo.changed) {
                                $settingValue.text(settingItemInfo.changedValue + unitText).addClass('changed');
                            } else {
                                $settingValue.text(config.value + unitText).removeClass('changed');
                            }
                        },
                    }[type]();
                }
            });
            a._settingListItemInfoProvider = new __CONTROLS__.CDataProvider(a._settingListItemInfos);
            listControl.setDataProvider(a._settingListItemInfoProvider);
            listControl.onItemSelected = function (param) {
                fnHandleSettingItemSelection(a, param);
            };
            listControl.onFocusChanged = function (param) {
                if (a.onSettingsFocusChanged) {
                    var focusedItem = listControl.getFocusedItem();
                    a.onSettingsFocusChanged(focusedItem);
                }
            };
            a._settingList = listControl;
            var view = new __CONTROLS__.CViewGroupControl(null);
            view.addClass('settingListView');
            a._viewCount = (!a._fullPage && a._settingListItemInfos.length < 7) ? a._settingListItemInfos.length : 7;
            view._$self.css({
                'width': width - KMargin * 2 + 'px',
                'height': a._viewCount * a._itemHeight + 'px'
            });
            view.setDrawParam({
                scrollByItemSize: true
            });
            view.setChildControl([listControl]);
            childControl.push(view);
            a._settingListView = view;
            if (a._statusInfoList.length) {
                var statusDrawer = function (index, $item) {
                    var typeClass = '';
                    if (index === 0) {
                        typeClass = ' type1';
                    } else if (index === 1) {
                        typeClass = ' type2';
                    }
                    $item.addClass('statusItem');
                    $item.removeClass('-f');
                    $item.append($('<span>', {'class': 'statusName' + typeClass}).html(a._statusInfoList[index].text));
                    $item.append($('<span>', {'class': 'statusBar ' + typeClass}));
                };
                var statusDrawers = [];
                var i;
                for (i = 0; i < a._statusInfoList.length; i += 1) {
                    statusDrawers.push(statusDrawer);
                }
                verticalLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                verticalLayout.addClass('statusListView');
                verticalLayout._$self.css({
                    'width': width - KMargin * 2 + 'px',
                });
                verticalLayout.setDrawParam({
                    scrollByItemSize: true
                });
                verticalLayout.setItemDrawers(statusDrawers);
                childControl.push(verticalLayout);
            }
            var buttonDrawer = function (index, $item) {
                $item.addClass('btn').html(a._buttonInfoList[index].text);
            };
            var drawers = [];
            if (a._buttonInfoList) {
                var i;
                for (i = 0; i < a._buttonInfoList.length; i += 1) {
                    drawers.push(buttonDrawer);
                }
            }
            var horizontalLayout = new __CONTROLS__.CHorizontalLayoutControl(null);
            horizontalLayout.addClass('buttonArea');
            horizontalLayout.setDrawParam({
                padding: 15,
                align: 'hcenter',
            });
            horizontalLayout.setItemDrawers(drawers);
            horizontalLayout.onItemSelected = function (index, $item) {
                var fn = null;
                if (a._buttonInfoList) {
                    fn = a._buttonInfoList[index].fn;
                }
                if (fn) {
                    setTimeout(function () {
                        fn();
                    }, 1);
                }
                a.close('internal');
            };
            childControl.push(horizontalLayout);
            a.setChildControl(childControl, initialFocus);
            if (a._fullPage) {
                a.setDrawParam({
                    applyOnce: {
                        visibleAfterDraw: true
                    },
                    childAlign: 'hcenter',
                    padding: 20,
                    margins: [33, (a._fullPage ? 60 : 50), 60],
                    setFocusRolling: true
                });
            } else {
                a.setDrawParam({
                    applyOnce: {
                        visibleAfterDraw: true
                    },
                    align: 'hcenter',
                    childAlign: 'hcenter',
                    padding: 33,
                    margins: [33, 50, 60],
                    setFocusRolling: true
                });
            }
            a.resetHidingTimer();
        }
        resetHidingTimer() {
            var a = this;
            if (a._timeout) {
                clearTimeout(mHidingTimer);
                mHidingTimer = setTimeout(function () {
                    if (a._timeout) {
                        if (a instanceof CSettingDlg) {
                            a.close();
                        }
                    }
                }, a._timeout);
            }
        }
        doKey(keyCode?) {
            var a = this;
            var consumed = false;
            switch (keyCode) {
                case CONTROLS_CONFIG.KEY_UP:
                case CONTROLS_CONFIG.KEY_DOWN:
                case CONTROLS_CONFIG.KEY_LEFT:
                case CONTROLS_CONFIG.KEY_RIGHT:
                case CONTROLS_CONFIG.KEY_PAGEUP:
                case CONTROLS_CONFIG.KEY_PAGEDOWN:
                    CVerticalGroupControl.prototype.doKey.call(a, keyCode);
                    consumed = true;
                    break;
                default:
                    consumed = CVerticalGroupControl.prototype.doKey.call(a, keyCode);
                    break;
            }
            a.resetHidingTimer();
            return consumed;
        }
        doKeyRight(keyCode) {
            var a = this;
            if (a._settingListView.hasFocus() && !a._settingOnGoing) {
                fnHandleSettingItemSelection(a, a._settingList.getFocusedItem());
            }
            return false;
        }
        doKeyLeft(keyCode) {
            var a = this;
            return fnClearSelectionList(a);
        }
        doKeyBack(keyCode) {
            var a = this;
            if (!fnClearSelectionList(a)) {
                a.close();
            }
            return true;
        }
        doKeyRecord(keyCode) {
            return true;
        }
        doKeyOption(keyCode?) {
            var a = this;
            a.close();
            return true;
        }
    }
    prism.CSettingDlg = CSettingDlg;
    prism.createSettingDlg = function (layeredGroupControl, settingInfo, callback) {
        if (layeredGroupControl) {
            var settingDlg = new CSettingDlg(layeredGroupControl);
            settingDlg.setButtonInfoList([
                {
                    text: tr('LOC_OK_ID'),
                    fn: function () {
                        var i, len;
                        var changedInfos = [];
                        for (i = 0, len = settingDlg._settingListItemInfos.length; i < len; i++) {
                            var settingListItemInfo = settingDlg._settingListItemInfos[i];
                            if (settingListItemInfo.changed) {
                                changedInfos.push({
                                    index: settingListItemInfo.index,
                                    settingItem: settingListItemInfo.settingItem,
                                    prevValue: settingListItemInfo.settingItem.config.activeIndex,
                                    changedValue: settingListItemInfo.changedValue
                                });
                            }
                        }
                        if (settingDlg.onOk) {
                            settingDlg.onOk(changedInfos);
                        } else {
                            notify('notifyButtonSelected', {
                                type: "OK",
                                changedInfoList: changedInfos
                            }, this._stackIndex);
                        }
                    }
                },
                {
                    text: tr('LOC_CANCEL_ID'),
                    fn: function () {
                    }
                }
            ]);
            if (settingInfo) {
                if (settingInfo.width) {
                    settingDlg.setDrawParam('width', settingInfo.width);
                }
                if (settingInfo.title) {
                    settingDlg.setTitleHtml(settingInfo.title);
                }
                if (settingInfo.settingListItems) {
                    settingDlg.setSettingListItems(settingInfo.settingListItems);
                }
                if (settingInfo.statusInfoList) {
                    settingDlg.setStatusInfoList(settingInfo.statusInfoList);
                }
                if (settingInfo.timeout) {
                    settingDlg._timeout = settingInfo.timeout;
                }
                if (settingDlg.onItemSelected) {
                    settingDlg._onItemSelected = settingDlg.onItemSelected;
                } else {
                    var itemSelectedHandler = function (param) {
                        notify('notifyItemSelected', {
                            type: "ItemSelected",
                            param: param
                        }, this._stackIndex);
                    };
                    settingDlg.setOnItemSelected(itemSelectedHandler);
                }
            }
            return settingDlg;
        } else {
            var globalDlg = prism.createGlobalDialog('createSettingDlg', settingInfo, callback);
            return globalDlg;
        }
    };
    prism.createFullSettingDlg = function (layeredGroupControl) {
        var settingDlg = new CSettingDlg(layeredGroupControl);
        settingDlg.setFullPage();
        settingDlg.setButtonInfoList([
            {
                text: tr('LOC_OK_ID'),
                fn: function () {
                    var i, len;
                    var changedInfos = [];
                    for (i = 0, len = settingDlg._settingListItemInfos.length; i < len; i++) {
                        var settingListItemInfo = settingDlg._settingListItemInfos[i];
                        if (settingListItemInfo.changed) {
                            changedInfos.push({
                                index: settingListItemInfo.index,
                                settingItem: settingListItemInfo.settingItem,
                                changedValue: settingListItemInfo.changedValue
                            });
                        }
                    }
                    if (settingDlg.onOk) {
                        settingDlg.onOk(changedInfos);
                    }
                }
            },
            {
                text: tr('LOC_CANCEL_ID'),
                fn: function () {
                }
            }
        ]);
        return settingDlg;
    };

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createSettingView = function (layeredGroupControl) {
        var settingDlg = new CSettingDlg(layeredGroupControl);
        settingDlg.setButtonInfoList([]);
        return settingDlg;
    };
}
export = mPrism;
