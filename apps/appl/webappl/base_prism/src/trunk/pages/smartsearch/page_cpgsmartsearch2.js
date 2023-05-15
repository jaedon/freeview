var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "model/model", "pages/util/provider", "pages/page_kernel"], function (require, exports, __MODEL__, __PROVIDER__, __PAGE_KERNEL__) {
    ///<reference path="../../def/sugar.d.ts" />
    var cCPgSmartSearch = (function (_super) {
        __extends(cCPgSmartSearch, _super);
        function cCPgSmartSearch() {
            _super.call(this);
            this.TMediaType = {
                EOtt: 'ott',
                EDefault: 'default'
            };
            this._isVisible = false;
            this._keyword = '';
            this.TSearchTarget = {
                EScheduled: 1,
                EOnDemand: 2,
                EProgramme: 3,
                EChannel: 4,
                EFile: 5,
                ERecording: 6,
                EApplication: 7,
                EAppContent: 8,
                EDlnaContent: 9,
                ESuggestion: 10,
                EMore: 11
            };
            this.SCategory = [
                {
                    targetType: this.TSearchTarget.EOnDemand,
                    text: 'ON DEMAND'
                },
                {
                    targetType: this.TSearchTarget.EChannel,
                    text: 'CHANNEL'
                },
                {
                    targetType: this.TSearchTarget.EScheduled,
                    text: 'BROADCAST'
                },
                {
                    targetType: this.TSearchTarget.ERecording,
                    text: 'RECORDING'
                },
                {
                    targetType: this.TSearchTarget.EMore,
                    text: 'MORE'
                }
            ];
        }
        /**************************************************
         * Public Members
         **************************************************/
        cCPgSmartSearch.prototype.create = function () {
            var a = this;
            hx.logadd('CPgSmartSearch');
            hx.al.compose(a);
            if (!a._smartSearchControl) {
                var fnDataDrawer = function (param) {
                    var drawType = param.drawType;
                    var item = param.item;
                    var type = item.getType();
                    var category = item.getCategory();
                    var name = item.getName();
                    var dummy = item.isDummy();
                    var childCount = item.getChildCount();
                    var subItemList = item.getSubItemList();
                    var figure = document.createElement('figure');
                    param.$item.children().remove();
                    switch (drawType) {
                        case 'group':
                            var categoryEl;
                            categoryEl = document.createElement('div');
                            categoryEl.className = 'category';
                            if (dummy) {
                                categoryEl.innerHTML = name;
                            }
                            else {
                                categoryEl.innerHTML = category.text;
                                if (type === a.TSearchTarget.EOnDemand) {
                                    categoryEl.innerHTML += ' (' + childCount + ')';
                                }
                                else if (subItemList.length) {
                                    categoryEl.innerHTML += ' (' + subItemList.length + ')';
                                }
                            }
                            param.$item.append($(categoryEl));
                            break;
                        case 'detail':
                            var firstRow, secondRow;
                            var updateThumbnail = function (receivedUrl) {
                                if (receivedUrl) {
                                    var $img = param.$item.find('img');
                                    if ($img && $img.length) {
                                        $img[0].src = receivedUrl;
                                    }
                                }
                            };
                            var updateLogo = function (receivedUrl) {
                                if (receivedUrl) {
                                    secondRow.innerHTML = hx.util.format('<img src="{0}" alt="" border="0"' + ' onload="this.style.display=\'inline\';" style="display:none;"/>', receivedUrl) + " " + addition;
                                }
                            };
                            var addition = item.getAddition();
                            var channelNo = item.getChannelNo();
                            var channelName = item.getChannelName();
                            var thumbnail = item.getThumbnail(updateThumbnail);
                            var channelLogo = item.getChannelLogo(updateLogo);
                            var contentOwningLogo = item.getContentOwningLogo(updateLogo);
                            var startTime = item.getStartTime() * 1000;
                            var endTime = item.getEndTime() * 1000;
                            var metaType = item.getMetadataType();
                            var classThumbIcon = item.getClassThumbIcon('search');
                            var defaultThumbnail = '';
                            switch (type) {
                                case a.TSearchTarget.EChannel:
                                case a.TSearchTarget.EScheduled:
                                    if (metaType === "TV" || metaType === "HDTV" || metaType === hx.l('LOC_TV_PROGRAMME_ID') || metaType === 0) {
                                        figure.className = 'thum tv';
                                        defaultThumbnail = __MODEL__.TDefaultThumbnailList.TV;
                                    }
                                    else if (metaType === "Radio" || metaType === hx.l('LOC_RADIO_PROGRAMME_ID') || metaType === 1) {
                                        figure.className = 'thum radio';
                                        defaultThumbnail = __MODEL__.TDefaultThumbnailList.RADIO;
                                    }
                                    else {
                                        figure.className = 'thum';
                                        defaultThumbnail = __MODEL__.TDefaultThumbnailList.TV;
                                    }
                                    break;
                                case a.TSearchTarget.ERecording:
                                    figure.className = 'thum';
                                    defaultThumbnail = __MODEL__.TDefaultThumbnailList.RECORDING;
                                    break;
                                case a.TSearchTarget.EOnDemand:
                                    figure.className = 'thum content';
                                    defaultThumbnail = __MODEL__.TDefaultThumbnailList.FREEVIEW;
                                    break;
                                default:
                                    figure.className = 'thum content';
                                    defaultThumbnail = __MODEL__.TDefaultThumbnailList.CONTENT;
                                    break;
                            }
                            thumbnail = thumbnail || defaultThumbnail;
                            figure.innerHTML = hx.util.format('<img src="{0}" alt="" border="0" onerror="this.src=\'{1}\';"/>', thumbnail, defaultThumbnail);
                            firstRow = document.createElement('p');
                            firstRow.className = 'tit';
                            secondRow = document.createElement('span');
                            secondRow.className = 'addition';
                            firstRow.innerHTML = dummy ? '' : name;
                            switch (type) {
                                case a.TSearchTarget.EChannel:
                                    //param.$item.addClass('vod');
                                    $(figure).addClass('channel');
                                    firstRow.innerHTML = hx.il.makeDigitNumber(channelNo) + ' ' + name;
                                    secondRow.innerHTML = metaType;
                                    break;
                                case a.TSearchTarget.ERecording:
                                case a.TSearchTarget.EScheduled:
                                    var dispTime = '', dispDate = '', channelInfo = '';
                                    if (startTime && endTime) {
                                        dispTime = dateFormat(startTime, 'HH:MM') + ' ~ ' + dateFormat(endTime, 'HH:MM');
                                        dispDate = ' ' + dateFormat(startTime, 'ddd dd mmm');
                                    }
                                    if (channelName && channelNo) {
                                        channelInfo = hx.il.makeDigitNumber(channelNo) + ' ' + channelName + ' / ';
                                    }
                                    addition = dispTime + dispDate;
                                    if (channelLogo) {
                                        updateLogo(channelLogo);
                                    }
                                    else {
                                        secondRow.innerHTML = channelInfo + addition;
                                    }
                                    break;
                                case a.TSearchTarget.EMore:
                                    $(figure).addClass('app');
                                    secondRow.innerHTML = metaType || '';
                                    break;
                                case a.TSearchTarget.EOnDemand:
                                    // should go throuth Programme detail to play
                                    if (contentOwningLogo) {
                                        updateLogo(contentOwningLogo);
                                    }
                                    else {
                                        secondRow.innerHTML = addition;
                                    }
                                    break;
                                default:
                                    break;
                            }
                            if (classThumbIcon) {
                                param.$item.addClass(classThumbIcon);
                            }
                            param.$item.append($(figure)).append($(firstRow)).append($(secondRow));
                            break;
                        default:
                            break;
                    }
                };
                a._smartSearchControl = prism.createCategoryControl({
                    $el: $('#wrap_smt'),
                    withoutKeyboard: false,
                    category: a.SCategory,
                    target: a.TSearchTarget,
                    dataDrawer: fnDataDrawer
                });
            }
            a._hide();
            a._setVisible(false);
            a._initEvent();
            return a;
        };
        cCPgSmartSearch.prototype.destroy = function () {
            var a = this;
            a._hide();
            hx.svc.SETTING_UTIL.clearEventCb(a);
            if (hx.config.useDLNAonSearch) {
                hx.svc.DLNA.clearEventCb(a);
            }
            hx.svc.RECORDINGS.clearEventCb(a);
            if (hx.config.netflix) {
                hx.svc.NETFLIX.clearEventCb(a);
            }
            if (a._smartSearchControl) {
                if (a._smartSearchControl.getParentLayer() && a._smartSearchControl.getParentLayer().$layer()) {
                    a._smartSearchControl.getParentLayer().removeLayer();
                }
                a._smartSearchControl.destroy();
                a._smartSearchControl = null;
            }
            a._setVisible(false);
        };
        cCPgSmartSearch.prototype.isActive = function () {
            return this._isVisible;
        };
        //visible () : boolean {
        //    return this._isVisible;
        //}
        /**************************************************
         * Private Members
         **************************************************/
        cCPgSmartSearch.prototype._setVisible = function (visible) {
            var a = this;
            a._isVisible = visible;
            a.sendEventToEntity({
                alEvent: 'alCheckPageVisible'
            });
        };
        cCPgSmartSearch.prototype._initEvent = function () {
            var a = this;
            if (!a._smartSearchControl) {
                hx.log('debug', '[CPgSmartSearch] _initEvent fail!!');
                return;
            }
            var TSearchType = a._smartSearchControl.getSearchType();
            var searchAction = a._createSearchAction();
            var doSearch = function (keyword, searchType, isMoreSearch, isUsingSuggestion, param) {
                if (!a._smartSearchControl) {
                    hx.log('debug', '[CPgSmartSearch] doSearch fail!!');
                    return;
                }
                var i, length;
                var transaction = [];
                a._keyword = keyword.toLowerCase();
                if (isMoreSearch && param) {
                    transaction.push(searchAction[param.target]);
                }
                else {
                    a._smartSearchControl.clear();
                    if (keyword.length > 0) {
                        a._smartSearchControl.setTitleText(keyword);
                        a._smartSearchControl.setKeyword(keyword);
                        a._smartSearchControl.updateHistory(keyword);
                    }
                    if (searchType === TSearchType.EGlobal) {
                        if (isUsingSuggestion) {
                            //a._smartSearchControl.update([null]);
                            if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                                transaction.push(searchAction[a.TSearchTarget.EOnDemand]);
                            }
                            transaction.push(searchAction[a.TSearchTarget.EChannel]);
                            transaction.push(searchAction[a.TSearchTarget.EScheduled]);
                            transaction.push(searchAction[a.TSearchTarget.ERecording]);
                            transaction.push(searchAction[a.TSearchTarget.EFile]);
                            if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                                transaction.push(searchAction[a.TSearchTarget.EApplication]);
                                transaction.push(searchAction[a.TSearchTarget.EAppContent]);
                            }
                        }
                        else {
                            if (!keyword || keyword.length <= 0) {
                                a._smartSearchControl.update([null]);
                            }
                            else if (keyword.length <= 2) {
                                transaction.push(searchAction[a.TSearchTarget.EChannel]);
                            }
                            else if (keyword.length > 2) {
                                a._smartSearchControl.update([null]);
                                if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                                    transaction.push(searchAction[a.TSearchTarget.EOnDemand]);
                                }
                                transaction.push(searchAction[a.TSearchTarget.EChannel]);
                                transaction.push(searchAction[a.TSearchTarget.EScheduled]);
                                transaction.push(searchAction[a.TSearchTarget.ERecording]);
                                transaction.push(searchAction[a.TSearchTarget.EFile]);
                                if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                                    transaction.push(searchAction[a.TSearchTarget.EApplication]);
                                    transaction.push(searchAction[a.TSearchTarget.EAppContent]);
                                }
                            }
                        }
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE": "ESK",
                                "TRS": "R",
                                "KEYWORD": keyword
                            });
                        }
                    }
                    else {
                        transaction.push(searchAction[a.TSearchTarget.EChannel]);
                        transaction.push(searchAction[a.TSearchTarget.EApplication]);
                        transaction.push(searchAction[a.TSearchTarget.EScheduled]);
                        transaction.push(searchAction[a.TSearchTarget.ERecording]);
                        transaction.push(searchAction[a.TSearchTarget.EFile]);
                        transaction.push(searchAction[a.TSearchTarget.EAppContent]);
                        if (hx.config.useDLNAonSearch) {
                            transaction.push(searchAction[a.TSearchTarget.EDlnaContent]);
                        }
                        if (hx.svc.STATISTICS) {
                            hx.svc.STATISTICS.statisticsLog({
                                "CODE": "ESI",
                                "TRS": "R",
                                "ITEM": keyword
                            });
                        }
                    }
                }
                if (a._transactionDefList) {
                    var i;
                    for (i = 0; i < a._transactionDefList.length; i++) {
                        a._transactionDefList[i].resolve(null);
                    }
                }
                a._transactionDefList = [];
                var trasactionPromises = [];
                length = transaction.length;
                var cbUpdate = function (result) {
                    if (result && a._smartSearchControl && a._smartSearchControl.isShown()) {
                        if (result.isArrayList && result.data) {
                            var index;
                            var length = result.data.length || 0;
                            for (index = 0; index < length; index += 1) {
                                a._smartSearchControl.update(result.data[index]);
                            }
                        }
                        else {
                            a._smartSearchControl.update(result);
                        }
                    }
                };
                var retDef;
                for (i = 0; i < length; i += 1) {
                    retDef = transaction[i](keyword, cbUpdate, param);
                    if (retDef) {
                        retDef.promise.done(cbUpdate);
                        trasactionPromises.push(retDef.promise);
                        a._transactionDefList.push(retDef);
                    }
                }
                return Q.allSettled(trasactionPromises);
            };
            var doSuggest = function (keyword) {
                var cbSuggestions = function (result) {
                    if (result && a._smartSearchControl && a._smartSearchControl.isShown()) {
                        a._smartSearchControl.updateSuggestions(result);
                    }
                };
                if (a._smartSearchControl) {
                    a._smartSearchControl.clear();
                    if (keyword.length > 0) {
                        a._smartSearchControl.setTitleText(keyword);
                        a._smartSearchControl.setKeyword(keyword);
                    }
                    var retDef = searchAction[a.TSearchTarget.ESuggestion](keyword);
                    if (retDef) {
                        retDef.promise.done(cbSuggestions);
                    }
                }
            };
            var doTextChanged = function (text, searchType, isMoreSearch) {
                var keyword = text.toLowerCase();
                a._keyword = keyword;
                if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                    doSuggest(keyword);
                }
                else {
                    var isUsingSuggestion = false;
                    doSearch(keyword, searchType, isMoreSearch, isUsingSuggestion);
                }
            };
            var doShowProgrammeDetail = function (aProgrammeDetailItem) {
                if (a._smartSearchControl) {
                    var parentLayer = a._smartSearchControl.getParentLayer();
                    parentLayer._$self.addClass('show');
                    a._detailDlg = hx.il.createProgrammeDetailForSugar({
                        layeredGroupControl: parentLayer,
                        sugarPage: a,
                        pdeItem: aProgrammeDetailItem,
                        fnDoHideOnOpen: function (dlg) {
                            dlg.doOpen = function (param) {
                                parentLayer._$self.find('.dlg').parent().remove();
                                $('#wrap').css('opacity', 0);
                                prism.CProgrammeDetailDlg.prototype.doOpen.call(dlg, param);
                            };
                            dlg.destroy = function () {
                                a._detailDlg = null;
                                $('#wrap').css('opacity', 1);
                                prism.CProgrammeDetailDlg.prototype.destroy.call(dlg);
                            };
                        },
                        fnDoShow: function () {
                            if (a._smartSearchControl) {
                                a._smartSearchControl.show();
                            }
                        },
                        fnDoHide: function () {
                            $('#wrap').css('opacity', 1);
                            if (a._smartSearchControl) {
                                a._smartSearchControl.hide();
                            }
                        }
                    });
                    a._detailDlg._$self.addClass('show');
                }
            };
            var doPlayRequest = function (param) {
                var type, programmeDetailItem, parentLayer, fn;
                var item = param.item;
                var metadata = item.getMetadata();
                var fnPlay = {
                    'programme': function () {
                        a.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: a
                        });
                    },
                    'channel': function () {
                        hx.svc.BRIDGE.requestStopPlayback();
                        metadata.watchNow(function () {
                            a.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgLiveController',
                                me: a
                            });
                        });
                    },
                    'recording': function () {
                        var recording;
                        var id = metadata.id;
                        recording = hx.svc.RECORDINGS.getRecordingById({ id: id });
                        hx.log('CPgSmartSearch', hx.util.format('doPlayRequest: id = {0}', id));
                        if (recording) {
                            //                        hx.al.pageMgr.destroyPage('CPgDetailInfo');
                            a.sendEventToEntity({
                                alEvent: 'alChangePage',
                                target: 'CPgLiveController',
                                me: a
                            });
                            hx.svc.BRIDGE.requestVideoPlayback([recording], 0);
                        }
                        else {
                            hx.log('error', 'doPlayRequest : no Reocrding data');
                        }
                    },
                    'video': function () {
                        var pluginObject = metadata.getPluginObject();
                        var mediaItem = (pluginObject instanceof __MODEL__.cDlnaItem) ? pluginObject : new __MODEL__.CMediaItem(pluginObject);
                        hx.svc.BRIDGE.requestVideoPlayback([mediaItem], 0);
                    },
                    'music': function () {
                        var pluginObject = metadata.getPluginObject();
                        var mediaItem = (pluginObject instanceof __MODEL__.cDlnaItem) ? pluginObject : new __MODEL__.CMediaItem(pluginObject);
                        hx.svc.BRIDGE.requestMusicPlayback([mediaItem], 0);
                    },
                    'photo': function () {
                        var pluginObject = metadata.getPluginObject();
                        var mediaItem = (pluginObject instanceof __MODEL__.cDlnaItem) ? pluginObject : new __MODEL__.CMediaItem(pluginObject);
                        hx.svc.BRIDGE.requestPhotoPlayback([mediaItem], 0);
                    },
                    'app': function () {
                        var name = metadata.name;
                        var app = metadata.pluginObject;
                        var id = app.appid;
                        hx.log('CPgSmartSearch', hx.util.format('doPlayRequest: name = {0}', name));
                        hx.log('CPgSmartSearch', hx.util.format('doPlayRequest: app.appid = {0}', id));
                        hx.il.launchApp(name, id, undefined, a);
                    },
                    'web': function () {
                        var i, media, url, videoId, params;
                        var medias = metadata.medias;
                        var name = metadata.name;
                        var TMediaType = a.TMediaType;
                        for (i = 0; i < medias.length; i += 1) {
                            media = medias[i];
                            if (media.MediaType === TMediaType.EDefault || media.MediaType.indexOf('stb') > -1) {
                                url = media.MediaURL;
                                hx.log('CPgSmartSearch', hx.util.format('doPlayRequest: url = {0}', url));
                                params = $.getUrlParams(url);
                                videoId = params.q || null;
                                // url = videoId ? (prefix + videoId) : url;
                                $.getJSON(url).done(function (aData) {
                                    hx.il.openDeepLink(name, aData.Url, undefined, undefined, undefined, a);
                                }).fail(function () {
                                    hx.il.openDeepLink(name, url, undefined, undefined, undefined, a);
                                });
                                break;
                            }
                        }
                    },
                    'ondemand': function () {
                        hx.il.openDeepLink(metadata.getName(), metadata.getDeeplinkUrl(), undefined, undefined, undefined, a);
                    }
                };
                if (metadata instanceof __PROVIDER__.COnDemandProvider) {
                    hx.svc.IP_EPG.getDetailInfo(metadata.getProgrammeCridMds()).then(function (info) {
                        metadata.updateMdsData('program', info.querySelector('ProgramInformation'));
                        doShowProgrammeDetail(hx.svc.DETAIL_INFO.createOnDemandItem(metadata));
                    }, function (err) {
                        doShowProgrammeDetail(hx.svc.DETAIL_INFO.createOnDemandItem(metadata));
                    });
                }
                else if (metadata instanceof __PROVIDER__.CProgrammeProvider) {
                    hx.svc.IP_EPG.getDetailInfo(metadata.getProgrammeCridMds()).then(function (info) {
                        metadata.updateMdsData('program', info.querySelector('ProgramInformation'));
                        doShowProgrammeDetail(hx.svc.DETAIL_INFO.createProgrammeItem(metadata));
                    }, function (err) {
                        doShowProgrammeDetail(hx.svc.DETAIL_INFO.createProgrammeItem(metadata));
                    });
                }
                else if (metadata instanceof __PROVIDER__.CRecordingProvider) {
                    doShowProgrammeDetail(hx.svc.DETAIL_INFO.createRecordingItem(metadata));
                }
                else {
                    if (metadata instanceof __PROVIDER__.CChannelProvider) {
                        type = 'channel';
                    }
                    else if (metadata instanceof __PROVIDER__.CHumaxContentProvider || metadata instanceof __PROVIDER__.CDlnaContentProvider) {
                        hx.log('CPgSmartSearch', hx.util.format('doPlayRequest: content.title = {0}', metadata.title));
                        hx.log('CPgSmartSearch', hx.util.format('doPlayRequest: content.fullPath = {0}', metadata.fullPath));
                        hx.log('CPgSmartSearch', hx.util.format('doPlayRequest: content.duration = {0}', metadata.duration));
                        // hx.svc.SMART_SEARCH.setPlayContent(metadata);
                        var mediaType = metadata.getMediaType();
                        if (mediaType.indexOf(hx.l('LOC_VIDEO_ID')) > -1) {
                            type = 'video';
                        }
                        else if (mediaType.indexOf(hx.l('LOC_PHOTO_ID')) > -1) {
                            type = 'photo';
                        }
                        else if (mediaType.indexOf(hx.l('LOC_GENRE_MUSIC_ID')) > -1) {
                            type = 'music';
                        }
                    }
                    else if (metadata instanceof __PROVIDER__.CApplicationProvider) {
                        type = 'app';
                    }
                    else if (metadata instanceof __PROVIDER__.CWebContentProvider) {
                        type = 'web';
                    }
                    a._hide();
                    fn = fnPlay[type];
                    if (fn) {
                        fn();
                    }
                }
            };
            var doRecordRequest = function (aParam) {
                var targetItem = aParam ? aParam.item : null;
                if (!targetItem || !targetItem.getPluginObject()) {
                    hx.log('error', '[Search] doRecordRequest() - (!targetItem || !targetItem.getPluginObject())');
                    return;
                }
                var fnCancelReservation = function (data) {
                    var scheduledList = hx.svc.RECORDINGS.getScheduledList();
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
                        hx.svc.RECORDINGS.removeScheduledRecording({
                            items: [scheduledItem],
                            finished: function () {
                                hx.log('CPgSmartSearch', 'finished removeScheduledRecording');
                            }
                        });
                    }
                };
                var fnCancelSeriesReservation = function (data) {
                    hx.log('CPgSmartSearch', 'cancelSeriesReservation()');
                    var scheduledList = hx.svc.RECORDINGS.getScheduledList();
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
                        hx.svc.RECORDINGS.removeWholeSeriesSchedule({
                            items: (function (targetItem) {
                                var i, j, k;
                                var ret = [];
                                var dsrvRecordings2 = hx.svc.RECORDINGS;
                                var schedulelist = dsrvRecordings2.getScheduledList();
                                for (i = 0; i < schedulelist.length; i += 1) {
                                    if (schedulelist[i].getGroupCRIDs() && targetItem.getGroupCRIDs()) {
                                        if (targetItem.getGroupCRIDs().length > 0 && schedulelist[i].getGroupCRIDs().length > 0) {
                                            for (j = 0; j < targetItem.getGroupCRIDs().length; j += 1) {
                                                for (k = 0; k < schedulelist[i].getGroupCRIDs().length; k++) {
                                                    if (targetItem.getGroupCRIDs()[j] === schedulelist[i].getGroupCRIDs()[k]) {
                                                        ret.push(schedulelist[i]);
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                return ret;
                            })(foundItem),
                            finished: function () {
                                hx.log('CPgSmartSearch', 'finished removeSeriesScheduledRecording');
                            }
                        });
                    }
                };
                var fnSelectRecordType = function (aProgramme) {
                    var itemList = [];
                    var fnMakeMenuItem = function (aName) {
                        itemList.push({
                            _one: aName,
                            _programme: aProgramme
                        });
                    };
                    fnMakeMenuItem(hx.l('LOC_TIMERREC_BTN1_ID'));
                    fnMakeMenuItem(hx.l('LOC_SERIESREC_BTN_ID'));
                    hx.msg('recordingConflict', {
                        'title': hx.l('LOC_RECORD_RECOMMENDATION_ID'),
                        'text': hx.l('LOC_GUIDE_RECORD_SELECT_ID'),
                        'list_arr': itemList,
                        'dia_class': 'osd',
                        'auto_close': true,
                        'timeout': 30000,
                        'callback_fn': function (_status, _data) {
                            if (_status === 'stop') {
                                var eType = __MODEL__.eTScheduledFactoryType;
                                var factoryType = 0 /* ERECORDING */;
                                if (_data._one === hx.l('LOC_TIMERREC_BTN1_ID')) {
                                    factoryType = 0 /* ERECORDING */;
                                }
                                else if (_data._one === hx.l('LOC_SERIESREC_BTN_ID')) {
                                    factoryType = 10 /* ESERIES_RECORDING */;
                                }
                                else {
                                    return;
                                }
                                if (_data && _data._programme) {
                                    fnDoReservation(_data._programme, factoryType);
                                }
                            }
                        }
                    });
                };
                var fnDoReservation = function (data, aFactoryType) {
                    var efactoryType = aFactoryType || 0 /* ERECORDING */;
                    var scheduledList = hx.svc.RECORDINGS.getScheduledList();
                    hx.log('CPgSmartSearch', 'doReservation()');
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
                    var _fnRecord = function (aProgramme) {
                        hx.svc.RECORDINGS.record({
                            programme: aProgramme,
                            factoryType: efactoryType,
                            finished: function () {
                                hx.log('CPgSmartSearch', 'finished record');
                            }
                        });
                    };
                    var linkage = JSON.parse(data.pluginObject.getField("freeview/linkage"));
                    if (linkage.length) {
                        var channel = hx.svc.CH_LIST.getChannel({
                            ccid: linkage[0].channel,
                            group: 'ALL'
                        });
                        if (channel.result) {
                            hx.svc.EPG.findProgrammes({
                                channel: channel.channel,
                                clause: 'programmeID == ' + linkage[0].event,
                                cb: function (aResult, b, c) {
                                    var programmes = aResult[0];
                                    if (programmes.length) {
                                        var hdProgramme = programmes[0];
                                        fnSelectSimulcast({
                                            hdProgramme: hdProgramme,
                                            sdProgramme: data,
                                            fnRecorder: _fnRecord
                                        });
                                    }
                                    else {
                                        hx.log('CPgSmartSearch', "linkaged detected but can't find HD programme");
                                        _fnRecord(data.pluginObject);
                                    }
                                }
                            });
                        }
                    }
                    else {
                        _fnRecord(data.pluginObject);
                    }
                };
                var fnSelectSimulcast = function (aData) {
                    var hdProgramme = aData.hdProgramme;
                    var sdProgramme = aData.sdProgramme;
                    var fnRecorder = aData.fnRecorder;
                    var dlg = prism.createHDAlternativeButtonDlg(null, [
                        {
                            text: hdProgramme.getName(),
                            fn: function () {
                                hx.log("debug", hdProgramme.getName() + " HD Programme recording");
                                fnRecorder(hdProgramme.getPluginObject());
                            }
                        },
                        {
                            text: sdProgramme.getName(),
                            fn: function () {
                                hx.log("debug", sdProgramme.getName() + " SD Programme recording");
                                fnRecorder(sdProgramme.getPluginObject());
                            }
                        }
                    ]);
                    a.setKeyHookingControl(dlg, true);
                    dlg.open({
                        onClosed: function () {
                            a.setKeyHookingControl(dlg);
                        }
                    });
                };
                if (targetItem.hasSchedule() || targetItem.hasReminder()) {
                    fnCancelReservation(targetItem);
                }
                else if (targetItem.hasSeriesSchedule()) {
                    fnCancelSeriesReservation(targetItem);
                }
                else {
                    if (targetItem.getGroupCRIDs() && targetItem.getGroupCRIDs().length) {
                        fnSelectRecordType(targetItem);
                    }
                    else {
                        fnDoReservation(targetItem);
                    }
                }
                return true;
            };
            a._smartSearchControl.onTextChanged = function (text, searchType, isMoreSearch) {
                doTextChanged(text, searchType, isMoreSearch);
            };
            a._smartSearchControl.onSearch = function (text, searchType, isMoreSearch, isUsingSuggestion, param) {
                doSearch(text, searchType, isMoreSearch, isUsingSuggestion, param);
            };
            a._smartSearchControl.onPlayRequest = function (param) {
                doPlayRequest(param);
            };
            a._smartSearchControl.onRecordRequest = function (param) {
                doRecordRequest(param);
            };
            a._smartSearchControl.onCheckPrivacyPolicyAccepted = function () {
                return hx.svc.TV_PORTAL.isPrivacyPolicyAccepted();
            };
            hx.svc.SETTING_UTIL.addEventCb('NetworkAvailableChanged', a, function (networkAvailable) {
                a.onNetworkAvailableChanged(networkAvailable);
            });
            if (hx.config.netflix) {
                hx.svc.NETFLIX.addEventCb('NetflixStatusChanged', a, function (status) {
                    if (hx.svc.NETFLIX.isRunning()) {
                        a._hide();
                    }
                });
            }
            if (hx.config.useDLNAonSearch) {
                hx.svc.DLNA.addEventCb('SearchedCdsAdded', a, function (dlnaCds) {
                    var i, length;
                    var dlnaItem, dlnaItems = [];
                    var searchedItem, searchedItems = dlnaCds.getSearchedItems();
                    length = searchedItems.length;
                    for (i = 0; i < length; i += 1) {
                        searchedItem = searchedItems[i];
                        dlnaItem = new __PROVIDER__.CDlnaContentProvider(searchedItem);
                        dlnaItem.setType(dlnaCds.getName());
                        dlnaItems.push(dlnaItem);
                    }
                    if (a._smartSearchControl.isShown()) {
                        a._smartSearchControl.update(dlnaItems);
                    }
                });
                hx.svc.DLNA.addEventCb('SearchedItemAdded', a, function (dlnaCds, searchedItems) {
                    var i, length;
                    var dlnaItem, dlnaItems = [], searchedItem;
                    length = searchedItems.length;
                    for (i = 0; i < length; i += 1) {
                        searchedItem = searchedItems[i];
                        dlnaItem = new __PROVIDER__.CDlnaContentProvider(searchedItem);
                        dlnaItem.setType(dlnaCds.getName());
                        dlnaItems.push(dlnaItem);
                    }
                    if (a._smartSearchControl.isShown()) {
                        a._smartSearchControl.addSubItems(dlnaItems);
                    }
                });
            }
            hx.svc.RECORDINGS.addEventCb('RecordingItemRemoved', a, function (param) {
                hx.log('CPgSmartSearch', 'RecordingItemRemoved');
                if (a._smartSearchControl) {
                    a._smartSearchControl.onRecordingItemRemoved(param.item);
                }
            });
            hx.svc.RECORDINGS.addEventCb('ScheduledItemAdded', this, function (param) {
                hx.log('CPgSmartSearch', 'ScheduledItemAdded');
                if (a._smartSearchControl) {
                    a._smartSearchControl.onScheduledItemAdded(param.item);
                }
            });
            hx.svc.RECORDINGS.addEventCb('ScheduledItemRemoved', this, function (param) {
                hx.log('CPgSmartSearch', 'ScheduledItemRemoved');
                if (a._smartSearchControl) {
                    a._smartSearchControl.onScheduledItemRemoved(param.item);
                }
            });
            hx.svc.VOICE_ASSISTANCE.addEventCb('SearchAndDisplayResults', this, function (param) {
                if (!param || !param.length) {
                    hx.log('CPgSmartSearch', '[Received SearchAndDisplayResults] Error, There is no param.');
                    return;
                }
                a._show(TSearchType.EGlobal);
                doSearch(param[0].value || '', TSearchType.EGlobal, false, false);
                hx.svc.VOICE_ASSISTANCE.publish();
            });
        };
        cCPgSmartSearch.prototype._createSearchAction = function () {
            var a = this;
            if (!a._smartSearchControl) {
                hx.log('debug', '[CPgSmartSearch] _createSearchAction fail!!');
                return;
            }
            var searchAction = [];
            var fnFindProgrammes = function (param) {
                var clause = param.clause;
                var cb = param.cb;
                hx.svc.EPG.findProgrammes({
                    clause: clause,
                    cb: function (result) {
                        cb(result);
                    }
                });
            };
            var fnExecuteQuery = function (param) {
                var cb = param.cb;
                var target = param.target;
                var query = param.query;
                hx.svc.EPG.executeQuery({
                    target: target,
                    query: query,
                    cb: function (result) {
                        var items = result && result[0] ? result[0] : [];
                        cb(items);
                    }
                });
            };
            var fnFindWidget = function (widgetList, uid) {
                var i = 0, wd;
                for (i = 0; i < widgetList.length; i += 1) {
                    wd = widgetList[i];
                    if (wd.id.indexOf(uid) !== -1) {
                        return wd;
                    }
                }
                return null;
            };
            var fnGetAppList = function (param) {
                var cb = param.cb;
                hx.svc.TV_PORTAL.getSmartSearchPlayers({
                    fn: function (installedAppList) {
                        var i, length, widgetList, appInfo, wd, appList = [];
                        if (installedAppList) {
                            //widgetList = hx.svc.APP_MGR.getWidgetList();
                            length = installedAppList.length;
                            for (i = 0; i < length; i += 1) {
                                appInfo = installedAppList[i];
                                //wd = fnFindWidget(widgetList, appInfo.appid);
                                //if (wd) {
                                appList.push(appInfo);
                            }
                        }
                        cb(appList);
                    }
                });
            };
            var fnDoFindWebContent = function (url, keyword, cb, offset) {
                hx.svc.TV_PORTAL.findProgrammes({
                    param: {
                        url: url,
                        searchText: keyword,
                        offset: offset === undefined ? 0 : offset,
                        limit: 50
                    },
                    fn: function (result) {
                        cb(result);
                        if (result.Items.length >= 50 && (offset === undefined || offset <= 1000)) {
                            fnDoFindWebContent(url, keyword, cb, offset === undefined ? 50 : offset + 50);
                        }
                    },
                    fnError: function () {
                        hx.log('CSmartSearchModel', 'fnErr: There is no programme.');
                    }
                });
            };
            var fnFindWebContent = function (param) {
                var cb = param.cb;
                var keyword = param.keyword;
                hx.svc.TV_PORTAL.getSmartSearchAppList({
                    fn: function (appList) {
                        var i, length, widgetList, app;
                        if (appList) {
                            length = appList.length;
                            for (i = 0; i < length; i += 1) {
                                app = appList[i];
                                fnDoFindWebContent(app.smartsearchurl, keyword, cb);
                            }
                        }
                    }
                });
            };
            if (hx.config.useDLNAonSearch) {
                var fnFindDlnaContent = function (param) {
                    var cb = param.cb;
                    var keyword = param.keyword;
                    hx.svc.DLNA.stopKeywordSearch();
                    hx.svc.DLNA.startKeywordSearch({
                        keyword: keyword
                    });
                };
            }
            var fnSearchOndemand = function (aKeyword, aNumOfItems, aPageNum, aFinished) {
                hx.svc.IP_EPG.getOnDemandResults(aKeyword, aPageNum).then(function (xml) {
                    var retObj = {};
                    var infos = hx.svc.IP_EPG.parse.xml.ondemand(xml);
                    var crids = infos.crids;
                    var paginations = infos.paginations;
                    var odProgs = [], i, odItem = null;
                    var queryList = [];
                    var numOfItems = aNumOfItems || infos.numOfItems;
                    var pageNum = aPageNum || 1;
                    // There could be any item to be hidden from user by checking AIT.
                    // In this case, numOfItems should be updated.
                    function addAITQueryToQueryList(aOndemand) {
                        var subRetDef = Q.defer();
                        subRetDef.promise.then(function (aXml) {
                            if (aOndemand.checkTemplateAIT(aXml)) {
                                odProgs.push(aOndemand);
                            }
                        });
                        var query = {
                            url: aOndemand.getTemplateAitUrl(),
                            retDef: subRetDef
                        };
                        queryList.push(query);
                    }
                    if (crids) {
                        for (i in crids) {
                            odItem = new __PROVIDER__.COnDemandProvider(crids[i], 'search');
                            if (odItem.isPlayAvailable()) {
                                addAITQueryToQueryList(odItem);
                            }
                        }
                        hx.log('debug', '[TSearchTarget.EOnDemand] queryList.length : ' + queryList.length);
                    }
                    if (paginations) {
                        retObj.paginations = paginations;
                    }
                    var processRetObj = function () {
                        if (queryList.length - odProgs.length) {
                            hx.log('debug', '[TSearchTarget.EOnDemand] missing AIT list length : ' + (queryList.length - odProgs.length));
                        }
                        if (odProgs.length) {
                            retObj.items = odProgs;
                            retObj.type = a.TSearchTarget.EOnDemand;
                            retObj.numOfItems = numOfItems -= (queryList.length - odProgs.length);
                            retObj.pageNum = pageNum;
                            aFinished(retObj);
                        }
                        else {
                            retObj.items = [];
                            retObj.type = a.TSearchTarget.EOnDemand;
                            aFinished(retObj);
                        }
                    };
                    hx.svc.IP_EPG.getAITList(queryList).then(function () {
                        hx.log('debug', '[TSearchTarget.EOnDemand] Success? odProgs.length : ' + odProgs.length);
                        processRetObj();
                    }, function () {
                        hx.log('debug', '[TSearchTarget.EOnDemand] Fail? odProgs.length : ' + odProgs.length);
                        processRetObj();
                    });
                }, function () {
                    aFinished(null);
                });
            };
            searchAction[a.TSearchTarget.EScheduled] = function (keyword, finished) {
                var retDef = Q.defer();
                var retObj = {};
                var now = new Date();
                var query = hx.util.format('name %% {0} OR person %% {0} AND endTime > {1}', keyword, Math.floor(now.getTime() / 1000)); //parseInt(now.getTime() / 1000, 10));
                fnFindProgrammes({
                    clause: query,
                    cb: function (result) {
                        var i, length;
                        var programme;
                        var programmes = result ? result[0] : [];
                        var fnCompareByTime = function (a, b) {
                            return a.startTime - b.startTime;
                        };
                        var fnSortByChannelNumber = function (list) {
                            list.sort(function (a, b) {
                                a = a instanceof Array ? a[0] : a;
                                b = b instanceof Array ? b[0] : b;
                                if (a.channel && b.channel) {
                                    if (a.channel.majorChannel === b.channel.majorChannel) {
                                        return fnCompareByTime(a, b);
                                    }
                                    else {
                                        return a.channel.majorChannel - b.channel.majorChannel;
                                    }
                                }
                                else {
                                    return fnCompareByTime(a, b);
                                }
                            });
                        };
                        fnSortByChannelNumber(programmes);
                        retObj.items = programmes;
                        retObj.type = a.TSearchTarget.EScheduled;
                        retDef.resolve(retObj);
                        programme = undefined;
                        programmes = undefined;
                        result = undefined;
                    }
                });
                hx.log('CPgSmartSearch', '##################################################');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '-----      TSearchTarget.EScheduled     ----------');
                hx.log('CPgSmartSearch', hx.util.format('search(programme): query = {0}', query));
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '##################################################');
                query = undefined;
                return retDef;
            };
            searchAction[a.TSearchTarget.EChannel] = function (keyword, finished) {
                var retDef = Q.defer();
                var retObj = {};
                var retArr = [], retCnt = 0;
                var query = "SELECT * FROM DxService_t WHERE svcType = {0} AND name LIKE '%{1}%' LIMIT 100";
                var i, channelTypeCount = 2;
                for (i = 0; i < channelTypeCount; i += 1) {
                    fnExecuteQuery({
                        target: a.TSearchTarget.EChannel,
                        query: hx.util.format(query, i + 1, keyword),
                        cb: function (result) {
                            retArr = retArr.concat(result);
                            retCnt += 1;
                            if (retCnt === channelTypeCount) {
                                var fnSortByChannelNumber = function (list) {
                                    list.sort(function (a, b) {
                                        a = a instanceof Array ? a[0] : a;
                                        b = b instanceof Array ? b[0] : b;
                                        return a.majorChannel - b.majorChannel;
                                    });
                                };
                                fnSortByChannelNumber(retArr);
                                retObj.items = retArr;
                                retObj.type = a.TSearchTarget.EChannel;
                                retDef.resolve(retObj);
                                retArr = null;
                                result = null;
                            }
                        }
                    });
                    hx.log('CPgSmartSearch', '##################################################');
                    hx.log('CPgSmartSearch', '--------------------------------------------------');
                    hx.log('CPgSmartSearch', '-----       TSearchTarget.EChannel      ----------');
                    hx.log('CPgSmartSearch', hx.util.format('search(channel): query = {0}', query, hx.util.format(query, i + 1, keyword)));
                    hx.log('CPgSmartSearch', '--------------------------------------------------');
                    hx.log('CPgSmartSearch', '##################################################');
                }
                query = undefined;
                return retDef;
            };
            searchAction[a.TSearchTarget.EFile] = function (keyword, finished) {
                var retDef = Q.defer();
                var retObj = {};
                var retArr = [], retCnt = 0;
                var query = "SELECT * FROM DxFileInfo_t WHERE " + "type <> '' AND name LIKE '%{0}%' " + " AND interface == '{1}' AND path <> '/mnt/hd2/Recordings' AND ext IN (" + " 'ts', 'mkv', 'avi', 'mp4', 'asf', 'wmv', 'tp', 'm2v', 'm4v', 'mts', 'mpg', 'vob', 'divx', 'mpeg', " + " 'jpg', 'jpeg', 'bmp', 'gif', 'tif', " + " 'mp3', 'wav') LIMIT 100"; // Music
                var i, storageTypeCount = 5;
                for (i = 0; i < storageTypeCount; i += 1) {
                    fnExecuteQuery({
                        target: a.TSearchTarget.EFile,
                        query: hx.util.format(query, keyword, i),
                        cb: function (result) {
                            retArr = retArr.concat(result);
                            retCnt += 1;
                            if (retCnt === storageTypeCount) {
                                retObj.items = retArr;
                                retObj.type = a.TSearchTarget.EMore;
                                retDef.resolve(retObj);
                                retArr = null;
                                result = null;
                            }
                        }
                    });
                    hx.log('CPgSmartSearch', '##################################################');
                    hx.log('CPgSmartSearch', '--------------------------------------------------');
                    hx.log('CPgSmartSearch', '-----        TSearchTarget.EFile        ----------');
                    hx.log('CPgSmartSearch', hx.util.format('search(file): query = {0}', hx.util.format(query, keyword, i)));
                    hx.log('CPgSmartSearch', '--------------------------------------------------');
                    hx.log('CPgSmartSearch', '##################################################');
                }
                query = undefined;
                return retDef;
            };
            searchAction[a.TSearchTarget.ERecording] = function (keyword, finished) {
                var retDef = Q.defer();
                var retObj = {};
                var query = hx.util.format("SELECT * FROM DxRecListData_t WHERE name LIKE '%{0}%' AND isLocked = 0 LIMIT 100", keyword);
                fnExecuteQuery({
                    target: a.TSearchTarget.ERecording,
                    query: query,
                    cb: function (result) {
                        var fnCompareByTimeReverse = function (a, b) {
                            return b.getStartTime() - a.getStartTime();
                        };
                        var fnSortByChannelNumber = function (list) {
                            list.sort(function (a, b) {
                                a = a instanceof Array ? a[0] : a;
                                b = b instanceof Array ? b[0] : b;
                                if (a.channel && b.channel) {
                                    if (a.channel.majorChannel === b.channel.majorChannel) {
                                        return fnCompareByTimeReverse(a, b);
                                    }
                                    else {
                                        return a.channel.majorChannel - b.channel.majorChannel;
                                    }
                                }
                                else {
                                    return fnCompareByTimeReverse(a, b);
                                }
                            });
                        };
                        fnSortByChannelNumber(result);
                        retObj.items = result;
                        retObj.type = a.TSearchTarget.ERecording;
                        retDef.resolve(retObj);
                        result = undefined;
                    }
                });
                hx.log('CPgSmartSearch', '##################################################');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '-----      TSearchTarget.ERecording     ----------');
                hx.log('CPgSmartSearch', hx.util.format('search(recording): query = {0}', query));
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '##################################################');
                query = undefined;
                return retDef;
            };
            searchAction[a.TSearchTarget.EApplication] = function (keyword, finished) {
                var retDef = Q.defer();
                var retObj = {};
                fnGetAppList({
                    cb: function (appList) {
                        var i, app;
                        var length = appList.length;
                        var apps = [];
                        for (i = 0; i < length; i += 1) {
                            app = appList[i];
                            // if (app.name.toLowerCase().indexOf(a._keyword) === 0) {
                            if (app.name.toLowerCase().indexOf(a._keyword) > -1) {
                                apps.push(new __PROVIDER__.CApplicationProvider(app));
                            }
                        }
                        retObj.items = apps;
                        retObj.type = a.TSearchTarget.EMore;
                        retDef.resolve(retObj);
                        app = undefined;
                        apps = [];
                        appList = null;
                    }
                });
                hx.log('CPgSmartSearch', '##################################################');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '-----     TSearchTarget.EApplication    ----------');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '##################################################');
                return retDef;
            };
            searchAction[a.TSearchTarget.EAppContent] = function (keyword, finished) {
                //var retDef = Q.defer();
                var retObj = {};
                fnFindWebContent({
                    cb: function (result) {
                        var i, length, metadata, item;
                        var items = [];
                        if (result.Items) {
                            length = result.Items.length;
                            for (i = 0; i < length; i += 1) {
                                metadata = result.Items[i];
                                if (metadata.Medias && metadata.Medias.length) {
                                    item = new __PROVIDER__.CWebContentProvider(metadata);
                                    item.setType(result.ProviderName.toUpperCase());
                                    items.push(item);
                                }
                            }
                        }
                        if (finished) {
                            retObj.items = items;
                            retObj.type = a.TSearchTarget.EMore;
                            finished(retObj);
                        }
                        //retDef.resolve(items);
                        items = [];
                        result = undefined;
                    },
                    keyword: keyword
                });
                hx.log('CPgSmartSearch', '##################################################');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '-----      TSearchTarget.EAppContent    ----------');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '##################################################');
                //return retDef;
            };
            searchAction[a.TSearchTarget.EOnDemand] = function (keyword, finished, param) {
                var pageNum = (param && param.pageNum) || 0;
                var numOfItems = (param && param.numOfItems) || 0;
                fnSearchOndemand(keyword, numOfItems, pageNum, finished);
                hx.log('CPgSmartSearch', '##################################################');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '-----      TSearchTarget.EOnDemand    ----------');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '##################################################');
            };
            searchAction[a.TSearchTarget.ESuggestion] = function (keyword, finished) {
                var retDef = Q.defer();
                var retObj;
                hx.svc.IP_EPG.getOnDemandSuggestions(keyword).then(function (xml) {
                    retObj = hx.svc.IP_EPG.parse.xml.suggestion(xml);
                    retDef.resolve(retObj);
                }, function () {
                    retDef.resolve([]);
                });
                hx.log('CPgSmartSearch', '##################################################');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '-----      TSearchTarget.ESuggestion    ----------');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '##################################################');
                return retDef;
            };
            if (hx.config.useDLNAonSearch) {
                searchAction[a.TSearchTarget.EDlnaContent] = function (keyword, finished) {
                    var retDef = Q.defer();
                    fnFindDlnaContent({
                        cb: function (dmsList) {
                            retDef.resolve(null);
                        },
                        keyword: keyword
                    });
                    return retDef;
                };
                hx.log('CPgSmartSearch', '##################################################');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '-----     TSearchTarget.EDlnaContent    ----------');
                hx.log('CPgSmartSearch', '--------------------------------------------------');
                hx.log('CPgSmartSearch', '##################################################');
            }
            return searchAction;
        };
        cCPgSmartSearch.prototype._show = function (searchType) {
            var a = this;
            if (a._smartSearchControl) {
                if (!a._smartSearchControl.isShown()) {
                    a.setSmartSearchOpened(true);
                    a._smartSearchControl.show(searchType);
                    a._setVisible(true);
                    if (hx.config.useDLNAonSearch) {
                        if (a.getCenterPageName() !== 'CPgMusic' && a.getCenterPageName() !== 'CPgVideo' && a.getCenterPageName() !== 'CPgPhoto') {
                            hx.svc.DLNA.startScan();
                        }
                    }
                }
            }
        };
        cCPgSmartSearch.prototype._hide = function () {
            var a = this;
            if (hx.config.useDLNAonSearch) {
                hx.svc.DLNA.stopKeywordSearch();
            }
            if (a._detailDlg) {
                a._detailDlg.close();
                delete a._detailDlg;
                a._detailDlg = null;
            }
            if (a._smartSearchControl) {
                if (a._smartSearchControl.isShown()) {
                    a.setSmartSearchOpened(false);
                    a._smartSearchControl.hide();
                    a._setVisible(false);
                    if (hx.config.useDLNAonSearch) {
                        if (a.getCenterPageName() !== 'CPgMusic' && a.getCenterPageName() !== 'CPgVideo' && a.getCenterPageName() !== 'CPgPhoto') {
                            hx.svc.DLNA.stopScan();
                        }
                    }
                }
            }
        };
        cCPgSmartSearch.prototype._toggle = function (searchType) {
            var a = this;
            if (a._smartSearchControl) {
                if (a._smartSearchControl.isShown()) {
                    a._hide();
                }
                else {
                    a._show(searchType);
                }
            }
        };
        cCPgSmartSearch.prototype._on_alMessageToPage = function (param) {
            var a = this;
            if (a._smartSearchControl) {
                a._toggle({
                    searchType: a._smartSearchControl.getSearchType().EGlobal,
                    dimm: '-full-dimm'
                });
            }
        };
        cCPgSmartSearch.prototype._on_alKey = function (param) {
            var a = this;
            var bconsumed = false, k = hx.key, kc = param.alKey;
            hx.log('CPgSmartSearch', '_on_alKey');
            if (a._smartSearchControl) {
                bconsumed = a._smartSearchControl.getParentLayer().doKey(kc);
                if (!bconsumed) {
                    switch (kc) {
                        case k.KEY_BACK:
                        case k.KEY_ESCAPE:
                            a._hide();
                            bconsumed = true;
                            break;
                        case k.KEY_OK:
                        case k.KEY_UP:
                        case k.KEY_DOWN:
                        case k.KEY_CHANNEL_UP:
                        case k.KEY_CHANNEL_DOWN:
                            bconsumed = true;
                            break;
                        case k.KEY_OPTION:
                            bconsumed = true;
                            break;
                        case k.KEY_SEARCH:
                            a._toggle();
                            bconsumed = true;
                            break;
                        case k.KEY_MENU:
                        case k.KEY_GUIDE:
                        case k.KEY_ON_DEMAND:
                        case k.KEY_PLAYER:
                        case k.KEY_POWER:
                            break;
                        default:
                            bconsumed = true;
                            break;
                    }
                }
                if (hx.config.netflix) {
                    switch (kc) {
                        case k.KEY_NETFLIX:
                            bconsumed = false;
                            break;
                    }
                }
            }
            hx.log('CPgSmartSearch', '_on_alKey: keyCode=' + kc + ' bconsumed=' + bconsumed);
            return bconsumed;
        };
        /**************************************************
         * Event Handlers
         **************************************************/
        cCPgSmartSearch.prototype.onNetworkAvailableChanged = function (networkAvailable) {
            hx.log('CPgSmartSearch', hx.util.format('NetworkAvailableChanged: networkAvailable={0}', networkAvailable));
            if (networkAvailable) {
            }
        };
        return cCPgSmartSearch;
    })(__PAGE_KERNEL__);
    hx.al.CPgSmartSearch = cCPgSmartSearch;
    return cCPgSmartSearch;
});
