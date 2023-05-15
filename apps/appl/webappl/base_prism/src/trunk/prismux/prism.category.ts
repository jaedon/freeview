/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
/// <reference path="dep.d.ts" />
// file name : prismux/prism.category.js
/*jslint sloppy:true, nomen:true, vars:true */
/// <reference path="../layer_domain/service/ip_epg/engine/interface_freeview_play.d.ts" />
declare var hx: ifSugar;
import __PROVIDER__ = require("pages/util/provider")
import __CONTROLS__ = require('pages/util/controls');
import __CTRL_BASE__ = require('prismux/controls/ctrlbase');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    var TViewType = {
        ESuggestion: 0,
        EResult: 1,
        EHistory: 2
    };
    var TSearchType = {
        ESmart: 0,
        EGlobal: 1
    };
    export interface ICategoryItem {
        targetType : number;
        text : string;
    }
    enum EOPTION_MENU {
        SORT_BY_TIME = 1,
        SORT_BY_ALPHABET,
        SORT_BY_CHANNEL
    }
    var DClassForIconOnThumb = {
        vod : 'vod',
        comingsoon : 'comingsoon',
        recording : 'recording',
        schedule : 'schedule',
        series : 'series',
        reminder : 'reminder',
    }
    var CATEGORY_THUMBNAIL_WIDTH = 123;
    var CATEGORY_LOGO_WIDTH = 50;
    class CSortOptionListControl extends __CTRL_BASE__.CListControl {
        constructor(aElement: HTMLElement) {
            super(aElement);
            this.setClassToGroupViewElement("-sort-option");
            this.setItemWidth(250);
            this.setItemHeight(58);
            this.setOrientation(__CTRL_BASE__.TParamOrientation.EVertical);
            this.setListData([
                tr('LOC_SORT_BY_TIME_ID').toUpperCase(),
                tr('LOC_SORT_BY_ALPHABET_ID').toUpperCase(),
                tr('LOC_SORT_BY_CHANNEL_ID').toUpperCase()
            ]);
            this.setDataDrawer((aKey: any, aItem: any, aEl: HTMLElement): __CTRL_BASE__.TFocusInfo => {
                var doc = document;
                var elOption = doc.createElement("div");
                elOption.innerHTML = aItem;
                aEl.appendChild(elOption);
                return __CTRL_BASE__.TFocusInfo.KFocusAble;
            });
            this._doKeyRightLatent = function() { return true; };
            this._doKeyLeftLatent = function() { return true; };
            this._doKeyUpLatent = function() { return true; };
            this._doKeyDownLatent = function() { return true; };
            this._doKeyChannelUpLatent = function() { return true; };
            this._doKeyChannelDownLatent = function() { return true; };
        }
    }
    class COptionDlg extends __CTRL_BASE__.CLayeredGroupControl {
        static ID = "option-dlg";
        private _ctrlGroup;
        constructor($aParent: JQuery, startIndex?: EOPTION_MENU) {
            super($("<div>", {"id": COptionDlg.ID}));
            var $self = this.$self();
            this.createLayer({
                addClass: "layer-main",
                transition: {
                    newLayer: "fadeIn",
                    fnAfterTransition: () => {
                        var doc = document;
                        this._ctrlGroup = new __CTRL_BASE__.CGroupControl(this.getLayoutElement("-genre-ctrl-group"));
                        var ctrlOption = new CSortOptionListControl(doc.createElement("div"));
                        this._ctrlGroup.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EHorizontal);
                        this._ctrlGroup.setOwnedChildControls([ctrlOption]);
                        this.addControl(this._ctrlGroup);
                        this._ctrlGroup.draw();

                    }
                }
            });
            $aParent.append($self);
        }
        getFocusedOption(): EOPTION_MENU {
            var index:EOPTION_MENU = -1;
            var ctrl: CSortOptionListControl = this._ctrlGroup.getFocusedChild();
            index = ctrl.getFocusedElementIndex();
            return index + 1;
        }
    }
    class CCategoryItem {
        private _metadata: any;//__PROVIDER__.CMetadataProvider;
        private _childCount: number;
        private _subItemList: Array<any>;// __CONTROLS__.CDataProvider;
        private _type: string;
        private _category: Object;
        constructor(metadata?, category?, childCount?) {
            var a = this;
            a._metadata = metadata || new __PROVIDER__.CMetadataProvider();
            a._type = category ? category.targetType : null;
            a._category = category || null;
            a._childCount = childCount || 0;
            a._subItemList = [];
        }
        addSubItem(item) {
            var a = this;
            a._subItemList.push(item);
        }
        getMetadata() {
            return this._metadata;
        }
        getSubItemList() {
            var a = this;
            return a._subItemList;
        }
        getType() {
            return this._type ? this._type : this._metadata.getType();
        }
        getCategory() {
            return this._category || null;
        }
        getMediaType() {
            return this._metadata.getMediaType ? this._metadata.getMediaType() : '';
        }
        getMetadataType() {
            if (this._metadata.getType) {
                return this._metadata.getType();
            } else if (this._metadata.channel) {
                return this._metadata.channel.channelType;
            } else {
                return null;
            }
        }
        getName() {
            return this._metadata.getName ? this._metadata.getName() : this._metadata.name;
        }
        getAddition() {
            return this._metadata.getAddition ? this._metadata.getAddition() : null;
        }
        getChannelNo() {
            var a = this;
            var metadata = a._metadata, channelNo = '';
            if (metadata) {
                if (metadata.getChannelNum) {
                    channelNo = metadata.getChannelNum();
                } else if (metadata.channel) {
                    channelNo = metadata.channel.majorChannel;
                }
            }
            return channelNo;
        }
        getChannelName() {
            var a = this;
            var metadata = a._metadata, channel = null, channelName = '';
            if (metadata) {
                channel = metadata.channel;
                if (channel) {
                    channelName = channel ? channel.name : null;
                }
            }
            return channelName;
        }
        getThumbnail(aUpdateCb) {
            var a = this;
            if (a._metadata.getMdsSchedule) {
                a._metadata.getMdsSchedule((function (aProg) {
                    return function() {
                        aProg.getThumbnail({
                            updateCb : aUpdateCb,
                            width : CATEGORY_THUMBNAIL_WIDTH
                        });
                    }
                })(a._metadata));
                return a._metadata.getThumbnail();
            }
            return a._metadata.getThumbnail ? a._metadata.getThumbnail({
                updateCb : aUpdateCb,
                width : CATEGORY_THUMBNAIL_WIDTH
            }) : '';
        }
        getPlayerLogo(updateCb) {
            return this._metadata.getPlayerLogo ? this._metadata.getPlayerLogo(updateCb) : '';
        }
        getChannelLogo(updateCb) {
            return this._metadata.getChannelLogo ? this._metadata.getChannelLogo({
                updateCb : updateCb,
                width : CATEGORY_LOGO_WIDTH
            }) : '';
        }
        getContentOwningLogo(updateCb) {
            return this._metadata.getContentOwningLogo ? this._metadata.getContentOwningLogo({
                updateCb : updateCb,
                width : CATEGORY_LOGO_WIDTH
            }) : '';
        }
        getStartTime() {
            return this._metadata.getStartTime ? this._metadata.getStartTime() : this._metadata.startTime;
        }
        getEndTime() {
            return this._metadata.getEndTime ? this._metadata.getEndTime() : this._metadata.endTime;
        }
        getProgrammeCridMds() {
            return this._metadata.programmeId;
        }
        getProgrammeID() {
            return this._metadata.getProgrammeID ? this._metadata.getProgrammeID() : (this._metadata.programmeID || null);
        }
        getChildCount() {
            return this._childCount;
        }
        setChildCount(count) {
            this._childCount = count;
        }
        isPlayAvailable() {
            var a = this;
            if (a._metadata.isPlayAvailable) {
                return a._metadata.isPlayAvailable();
            }
            return false;
        }
        isComingSoon() {
            var a = this;
            return a._metadata.getAvailability ? !!(a._metadata.getAvailability() === __PROVIDER__.EOndemandAvailable.EComingSoon) : false;
        }
        getClassThumbIcon(from?) {
            var a = this;
            var classStr = '';
            if ((function (channelId, isLive) {
                    if (!channelId || !isLive) {
                        return false;
                    }
                    var i, l, isRecording = false, item = null;
                    var currentRecList = hx.svc.RECORDINGS.getCurrentRecordings();
                    for (i = 0, l = currentRecList.length; i < l; i += 1) {
                        item = currentRecList[i];
                        if (item) {
                            if (item.getChannelId() === channelId) {
                                isRecording = true;
                                break;
                            }
                        }
                    }
                    return isRecording;
                }(a._metadata.ccid, a._metadata.isLive ? a._metadata.isLive() : false))) {
                classStr = DClassForIconOnThumb.recording;
            } else if (a._metadata.hasSchedule && a._metadata.hasSchedule()) {
                classStr = DClassForIconOnThumb.schedule;
            } else if (a._metadata.hasReminder && a._metadata.hasReminder()) {
                classStr = DClassForIconOnThumb.reminder;
            } else if (a._metadata.hasSeriesSchedule && a._metadata.hasSeriesSchedule()) {
                classStr = DClassForIconOnThumb.series;
            } else if (a.isPlayAvailable() && !(from === 'search')) {
                classStr = DClassForIconOnThumb.vod;
            } else if (a.isComingSoon() && !(from === 'search')) {
                classStr = DClassForIconOnThumb.comingsoon;
            }
            return classStr;
        }
        isDummy() {
            return this._metadata.isDummy ? this._metadata.isDummy() : false;
        }
        isHD() {
            return this._metadata.isHd ? this._metadata.isHd() : this._metadata.isHD;
        }
    }
    class CCategoryItemList {
        private _categoryData: {};
        private _categoryOrder: Array<any>;
        constructor(categorySet?) {
            this._categoryData = {};
            this._categoryOrder = [];
            if (categorySet) {
                for (var i in categorySet) {
                    this._categoryOrder.push(categorySet[i].targetType);
                }
            }
            this.clear();
        }
        push(item, numOfItems?) {
            var that = this;
            var type = item.getType();
            var category = item.getCategory();
            if (that._categoryData[type]) {
                that._categoryData[type].addSubItem(item);
            } else {
                that._categoryData[type] = new CCategoryItem(null, category, numOfItems || null);  // parent item
                that._categoryData[type].addSubItem(item);
            }
        }
        initParentItem(parentItem) {
            var that = this;
            that._categoryData[parentItem.getType()] = parentItem;  // parent item
        }
        getDataList() {
            var that = this;
            var key;
            var dataList = [];
            for (key in that._categoryData) {
                dataList.push(that._categoryData[key]);
            }
            var fnSortByOrder : Function = (list) => {
                list.sort((a, b) => {
                    a = a instanceof Array ? a[0] : a;
                    b = b instanceof Array ? b[0] : b;
                    return that._categoryOrder.indexOf(a.getType()) - that._categoryOrder.indexOf(b.getType());
                });
            };
            fnSortByOrder(dataList);
            return dataList;
        }
        getParentItem(type) {
            return this._categoryData[type] || null;
        }
        clear() {
            this._categoryData = {};
        }
    }
    export class CCategoryControl extends __CONTROLS__.CLayeredGroupControl {
        private _$title: JQuery;
        private _$index: JQuery;
        private _$indicator: JQuery;
        private _$next: JQuery;
        private _$prev: JQuery;
        private _$first: JQuery;
        private _$last: JQuery;
        private _$smt: JQuery;
        private _$opt: JQuery;
        private _$optTitle: JQuery;
        private _groupItemList: CCategoryItemList;
        private _currentDetailProvider: __CONTROLS__.CDataProvider;
        private _currentType;
        private _resultView;
        private _suggestionView;
        private _historyView;
        private _detailView;
        private _groupView;
        private _parentLayer;
        private _mainControl;
        private _searchControl;
        private _searchView;
        private _oldText;
        private _$input: JQuery;
        private _keyword: string;
        private _defaultStringOnSearchControl : string;
        private _showKeyboardTimer: any;
        private _lastSuggestion: IParsedSuggestion;
        private _paginations: any;
        private MAX_DETAIL_VIEW_ITEM = 10;
        private MAX_ONDEMAND_ITEM_PER_REQUEST = 30;
        private REMAINING_PAGE_COUNT_TO_UPDATE = 1;
        private MAX_SEARCH_HISTORY_ITEM = 10;
        private STR_PREFIX_PARTIAL_SEARCH = "Search by";
        private STR_DELETE_HISTORY = "DELETE HISTORY";
        private _isWithoutKeyboard = false; // a crucial variable to separate 2 kinds of category UX
        private SCategory: ICategoryItem[];
        private TSearchTarget;
        private STR_TITLE: string;
        private sortDescent: boolean = false;
        private _optionMenu;
        private _selectedOptText = hx.l('LOC_SORT_BY_TIME_ID').toUpperCase();
        private _sortOption: EOPTION_MENU = EOPTION_MENU.SORT_BY_TIME;
        private _shownFocus;

        onTextChanged: Function;
        onSearch: Function;
        onPlayRequest: Function;
        onClosed: Function;
        onCheckPrivacyPolicyAccepted: Function;
        onRecordRequest: Function;

        constructor(param) {
            if (param) {
                if (param.withoutKeyboard) {
                    this._isWithoutKeyboard = param.withoutKeyboard;
                    this.MAX_DETAIL_VIEW_ITEM = 12;
                }
                if (param.category) {
                    this.SCategory = param.category;
                }
                if (param.target) {
                    this.TSearchTarget = param.target;
                }
                if (param.title) {
                    this.STR_TITLE = param.title;
                }
                if (param.dataDrawer) {
                    this.setDataDrawer = param.dataDrawer;
                }
                if (param.sortDescent) {
                    this.sortDescent = param.sortDescent;
                }
            }
            super(param ? param.$el : null);
        }
        init($el) {
            hx.logadd('CCategoryControl');
            var a = this;
            var $title = $('<h1>');
            var $indicator = $('<div>', {
                'class': 'indicator'
            });
            var $next = $('<div>', {
                'class': 'next'
            });
            var $prev = $('<div>', {
                'class': 'prev'
            });
            var $first = $('<div>', {
                'class': 'first',
                'text' : "Go to First Page"
            });
            var $last = $('<div>', {
                'class': 'last',
                'text' : "Go to Last Page"
            });
            var $index = $('<div>', {
                'class': 'pagination',
                'html': '0 / 0'
            });
            var $opt = $('<section>', {
                'id' : 'opt',
                'class' : 'category'
            });
            var $optTitle = $('<p>', {
                'class' : 'tit',
                'text' : a._selectedOptText
            });
            var $smt = $('<div>', {
                'id': 'smt2'
            }).append($title)
                .append($indicator
                    .append($next)
                    .append($prev)
                    .append($first)
                    .append($last)
                    .append($index)
                    .append($opt
                        .append($optTitle)));
            if (a._isWithoutKeyboard) {
                $next.addClass('top');
                $prev.addClass('top');
                $opt.css('opacity', '1');
                $opt.css('z-index', '5');
            }
            a._$title = $title;
            a._$index = $index;
            a._$next = $next;
            a._$prev = $prev;
            a._$first = $first;
            a._$last = $last;
            a._$indicator = $indicator;
            a._$smt = $smt;
            a._$opt = $opt;
            a._$optTitle = $optTitle;
            a._groupItemList = new CCategoryItemList(a.SCategory);
            a._currentDetailProvider = new __CONTROLS__.CDataProvider([]);
            CLayeredGroupControl.prototype.init.call(a, $smt);

            // Main
            var mainControl = new __CONTROLS__.CLayeredGroupControl($('<div>', {
                id: 'layers'
            }));
            var resultView = a.createView(TViewType.EResult);
            a._resultView = resultView;
            mainControl.createLayer({
                fnAfterCreate: function () {
                    var self = this;
                    self.setChildControl([resultView]);
                    self.draw();
                }
            });
            a._mainControl = mainControl;
            // Search
            var searchView = new __CONTROLS__.CVerticalGroupControl(null);
            if (!a._isWithoutKeyboard) {
                var searchControl = new __CONTROLS__.CVerticalLayoutControl($('<div>', {
                    id: 'search'
                }));
                a._defaultStringOnSearchControl = "";
                searchControl.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('window');
                        $item.html(hx.util.format('<form method="post" style="display:block">' + '<input type="text" name="search" id="inputSch" value="{0} \'{1}\' {2}" ' + 'style="input-format:simple" />' + '</form>', tr('LOC_PRESS'), tr('LOC_OK_ID'), tr('LOC_TO_ENTER_A_KEYWORD')));
                        //$item.html(hx.util.format('<form method="post" style="display:block">' + '<input type="text" name="search" id="inputSch"' + 'style="input-format:simple" />' + '</form>'));
                    }
                ]);
                searchControl.draw({
                    setActiveFocus: true
                });
                a._searchControl = searchControl;
                searchView.setChildControl([searchControl, mainControl], 0);
            } else {
                a._$title.addClass('title');
                a._$title.text(a.STR_TITLE);
                searchView.setChildControl([mainControl], 0);
            }
            searchView.onFocusChanged = function(pOld, pNew) {
                var $h1 = a._$smt.find('h1');
                if (!a.isShown()) {
                    hx.log('CCategoryControl', '[searchView.onFocusChanged] do not create NATIVE KEYBOARD! at this' +
                        ' time. (hiding search)');
                    return;
                }
                if (pNew === searchControl) {   // to input tag
                    if (a._$input) {
                        a._$input.val(a._oldText);
                        a._$input.focus();
                        if (a._oldText == '') {
                            a.updateHistory();
                        }
                    }
                    $h1.addClass('in');
                } else if (pNew === mainControl) {    // to result
                    if ((a._suggestionView && a._suggestionView.$self().hasClass('show')) ||
                        (a._historyView && a._historyView.$self().hasClass('show'))) {
                        $h1.removeClass('in');
                    } else {
                        //a.drawDetail(false);
                    }
                }
            };
            searchView.draw({
                setActiveFocus: true
            });
            a._searchView = searchView;
            a.createLayer({
                fnAfterCreate: function () {
                    var self = this;
                    self.setChildControl([searchView]);
                }
            });
            a.$self().css('z-index', '4');
            // Parent
            a._parentLayer = new __CONTROLS__.CLayeredGroupControl($el);
            a._parentLayer.createLayer({
                fnAfterCreate: function () {
                    var self = this;
                    self.setChildControl([a]);
                    self.draw();
                }
            });
            if (a._isWithoutKeyboard) {
                // initial categories
                for (var i in a.SCategory) {
                    a._groupItemList.initParentItem(new CCategoryItem(null, a.SCategory[i]));
                }
                a.drawResult(true, null);
            } else {
                // Input Box
                var $input = $smt.find('#inputSch');
                a._oldText = '';
                $input.bind('input', function () {
                    var text = $(this).val();
                    hx.log('CCategoryControl', hx.util.format('onchange: text = {0}', text));
                    if (text !== a._oldText) {
                        if (text == '') {
                            a.updateHistory();
                        } else {
                            if (a.onTextChanged) {
                                a.onTextChanged(text, TSearchType.EGlobal, false);
                            }
                        }
                    } else {
                        if (text == '') {
                            if (a._$input) {
                                a._$input.val(a._defaultStringOnSearchControl);
                                a._$input.removeClass('normal');
                            }
                        }
                    }
                    a._oldText = text;
                });
                a._$input = $input;
            }
            a._shown = false;
        }
        destroy() {
            var a = this;
            a.clear();
            if (a.onClosed) {
                a.onClosed();
            }
            if (a._currentDetailProvider) {
                a._currentDetailProvider.destroy();
                a._currentDetailProvider = null;
            }
            super.destroy();
        }
        doShow(param?) {
            var a = this;
            var $smt = a._$smt;
            var $h1 = $smt.find('h1');
            var searchType = param ? param.searchType : TSearchType.EGlobal;
            var dimm = param && param.dimm;
            if (dimm) {
                $smt.addClass(dimm);
            } else {
                $smt.removeClass('-full-dimm');
                $smt.removeClass('-full-black');
            }
            $h1.addClass('in');
            $smt.removeClass('out');
            if (!a._isWithoutKeyboard) {
                if (a._showKeyboardTimer) {
                    clearTimeout(a._showKeyboardTimer);
                    a._showKeyboardTimer = null;
                }
                if (searchType === TSearchType.EGlobal) {
                    a._showKeyboardTimer = setTimeout(function() {
                        if (!a.isShown()) {
                            hx.log('CCategoryControl', '[doShow] do not create NATIVE KEYBOARD! at this time.' +
                                ' (hiding' +
                                ' search)');
                            return;
                        }
                        if (a._$input) {
                            a._$input.val("");
                            a._$input.focus();
                            if (a._oldText == '') {
                                a.updateHistory();
                            }
                        }
                    }, 1000);
                    a._oldText = '';
                }
            }
            $smt.addClass('in');
            if (hx.emulMode) {
                if (a.onTextChanged) {
                    a.onTextChanged('American', TSearchType.EGlobal, false);
                }
            }
            if (a._$input) {
                a._$input.val(a._defaultStringOnSearchControl);
                a._$input.removeClass('normal');
            }
            a._oldText = '';
            if (!a._isWithoutKeyboard) {
                a._searchView.setFocusedChld(a._searchControl);
            }
        }
        doHide(param?) {
            var a = this;
            var bKeepData = param ? param.bKeepData : false;
            var $smt = a._$smt;
            var $h1 = $smt.find('h1');
            $h1.removeClass('in');
            $smt.removeClass('in');
            if (!bKeepData) {
                a._oldText = '';
                a.clear();
                a._isTransitioning = false;
                a._lastSuggestion = null;
            }
        }
        doKey(keyCode) {
            var a = this;
            if (a._optionMenu) {
                if (a._optionMenu.doKey(keyCode)) {
                    return true;
                }
            }
            if (keyCode === CONTROLS_CONFIG.KEY_DOWN)/* || keyCode === CONTROLS_CONFIG.KEY_BACK)*/ {
                if (a._suggestionView) {
                    var suggestionProvider = a._suggestionView.list.getDataProvider() || null;
                    var suggestionCount = suggestionProvider ? suggestionProvider.getCount() : 0;
                    if (suggestionCount === 0) {
                        return false;
                    }
                } else if (a._historyView) {
                    var historyProvider = a._historyView.list.getDataProvider() || null;
                    var historyCount = historyProvider ? historyProvider.getCount() : 0;
                    if (historyCount === 0) {
                        return false;
                    }
                } else {
                    var detailCount = 0;
                    var resultProvider = a._groupItemList ? a._groupItemList.getDataList() : null;
                    var resultCount = resultProvider ? resultProvider.length : 0;
                    if (resultCount === 0) {
                        return false;
                    } else {
                        detailCount = a._detailView.list.getDataProvider().getCount();
                        if (detailCount === 0) {
                            return false;
                        }
                    }
                }
            } else if (keyCode === CONTROLS_CONFIG.KEY_PLAY_SPEED_DOWN || keyCode === CONTROLS_CONFIG.KEY_PLAY_SPEED_UP
                || keyCode === CONTROLS_CONFIG.KEY_REWIND || keyCode === CONTROLS_CONFIG.KEY_FAST_FWD) {
                var focusedView = a._mainControl.getFocusedChild();
                if (focusedView.type === TViewType.EResult) {
                    if (focusedView.getFocusedChild() === a._detailView) {
                        var moveLeft = 0;
                        if (a._groupView.getFocusedChild().getFocusedItem().item.getType() !== a.TSearchTarget.EOnDemand) {
                            // remove jumping to last page on ondemand result
                            if (keyCode === CONTROLS_CONFIG.KEY_PLAY_SPEED_UP || keyCode === CONTROLS_CONFIG.KEY_FAST_FWD) {
                                var listWidth = a._detailView.list.getWidth();
                                var viewWidth = a._detailView.getWidth();
                                moveLeft = (Math.ceil(listWidth / viewWidth) - 1) * viewWidth;
                            }
                        }
                        a._detailView.setLeft(moveLeft);
                        return true;
                    }
                }
            }
            return CLayeredGroupControl.prototype.doKey.call(a, keyCode);
        }
        doKeySearch() {
            var a = this;
            var $search = a._$smt.find('#search');
            var $input = $search.find('#inputSch');
            var inputStr = '';
            var bConsumed = false;
            if ($search.hasClass('-fd')) {
                inputStr = $input.val();
                if (inputStr === a._defaultStringOnSearchControl) {
                    $input.val("");
                    $input.focus();
                    a._oldText = '';
                    $input.addClass('normal');
                } else {
                    if (a.onSearch) {
                        a.onSearch(inputStr, TSearchType.EGlobal, false, false);
                    }
                    a._$smt.find('h1').addClass('in');
                }
                bConsumed = true;
            }
            return bConsumed;
        }
        doKeyEscape() {
            return this.doKeyBack();
        }
        doKeyOption() {
            var a = this;
            if (!a._isWithoutKeyboard) {
                hx.log('CCategoryControl', '[doKeyOption] !a._isWithoutKeyboard');
                return;
            }
            if (a._currentDetailProvider.getCount() === 0) {
                hx.log('CCategoryControl', '[doKeyOption] a._currentDetailProvider.getCount() === 0');
                return;
            }
            if (a._optionMenu) {
                a._optionMenu.destroy();
                delete a._optionMenu;
                a._optionMenu = null;
                a.showFocus();
            } else {
                a.hideFocus();
                a._optionMenu = new COptionDlg(a.$self(), a._sortOption);
            }
            return true;
        }
        doKeyOk() {
            var a = this;
            var metadata, focusedView, focusedItem, keyword, $keyword;
            var $search = a._$smt.find('#search');
            var $input = $search.find('#inputSch');
            var inputStr = '';
            var consumed = false;
            if ($search.hasClass('-fd')) {
                inputStr = $input.val();
                if (inputStr === a._defaultStringOnSearchControl) {
                    $input.val("");
                    $input.focus();
                    a._oldText = '';
                    $input.addClass('normal');
                } else {
                    if (a.onSearch) {
                        a.onSearch(inputStr, TSearchType.EGlobal, false, false);
                    }
                    a._$smt.find('h1').addClass('in');
                }
                consumed = true;
            } else {
                if (a._optionMenu) {
                    a.showFocus();
                    a.drawDetail(false, true, true);
                    a._resultView.setFocusedChld(a._detailView);
                    a._optionMenu.destroy();
                    delete a._optionMenu;
                    a._optionMenu = null;
                    return true;
                }
                focusedView = a._mainControl.getFocusedChild();
                if (!focusedView) {
                    return true;
                }
            }
            return consumed;
        }
        doKeyRec() {
            var a = this;
            var consumed = false;
            var currentProg = a.getCurrentProgramme();
            if (currentProg && currentProg instanceof __PROVIDER__.CProgrammeProvider) {
                if (a.onRecordRequest) {
                    consumed = a.onRecordRequest({
                        item: currentProg
                    });
                }
            }
            return consumed;
        }
        doKeyBack() {
            var a = this;
            var consumed = false;
            if (a._optionMenu) {
                a._optionMenu.destroy();
                delete a._optionMenu;
                a._optionMenu = null;
                a.showFocus();
                consumed = true;
            }
            return consumed;
        }
        doKeyLeftIfNotHandled() {
            return true;
        }
        doKeyRightIfNotHandled() {
            return true;
        }
        doTransition(param) {
            var a = this;
            var before, after;
            var $view = param.$view;
            var transition = param.transition;
            var cb = function () {
                $view.removeClass('ani');
                param.cb();
            };
            switch (transition) {
                case 'FadeOut':
                    before = {
                        opacity: 1
                    };
                    after = {
                        opacity: 0
                    };
                    break;
                case 'MoveOutFromLeft':
                    before = {
                        opacity: 1,
                        left: 0
                    };
                    after = {
                        opacity: 0,
                        left: 70
                    };
                    break;
                case 'MoveOutFromRight':
                    before = {
                        opacity: 1,
                        left: 0
                    };
                    after = {
                        opacity: 0,
                        left: -70
                    };
                    break;
                case 'MoveInFromLeft':
                    before = {
                        opacity: 0,
                        left: -70 // -$view.width() + 'px'
                    };
                    after = {
                        opacity: 1,
                        left: 0
                    };
                    break;
                case 'MoveInFromRight':
                    before = {
                        opacity: 0,
                        left: 70 // $view.width() + 'px'
                    };
                    after = {
                        opacity: 1,
                        left: 0
                    };
                    break;
            }
            $view.cssBefore(before);
            $view.addClass('ani');
            $view.anim(after, cb);
        }
        getFocusedItem() {
            var a = this;
            var focusedItem;
            var focusedChild = a._mainControl.getFocusedChild();
            while (focusedChild && focusedChild.getFocusedChild) {
                focusedChild = focusedChild.getFocusedChild();
            }
            if (focusedChild) {
                focusedItem = focusedChild.getFocusedItem();
            }
            if (focusedItem && focusedItem.item) {
                return focusedItem.item;
            }
            return null;
        }
        setKeyword(keyword) {
            this._keyword = keyword;
        }
        setTitleText(keyword) {
            var a = this;
            if (a._isWithoutKeyboard) {
                //hx.log('CCategoryControl', '[setTitleText] do not update title (this is not keyword at this time)');
                return;
            }
            var text = (keyword && keyword.length) ? '"' + keyword.replace('<br />', ' ') + '"' : '';
            if (false) {//hx.config.useAnimation) {
                a._$title.fadeOut(300, function () {
                    a._$title.text(text);
                    a._$title.fadeIn();
                });
            } else {
                a._$title.text(text);
            }
        }
        setGroupCount(type, count) {
            var a = this;
            var parentItem = a._groupItemList.getParentItem(type);
            var groupCount = count || parentItem.getSubItemList().length;
            var i = 0;
            var $category = null;
            var $categorys = $('#smt2').find('.category');
            var length = $categorys.length;
            if (length) {
                var category = parentItem.getCategory();
                var typeStr = category.text;
                for (i = 0; i < length; i += 1) {
                    $category = $($categorys[i]);
                    if ($category.text().indexOf(typeStr) > -1) {
                        $category.text(hx.util.format('{0} ({1})', typeStr, groupCount));
                        break;
                    }
                }
            }
        }
        setIndexText(aList?) {
            var a = this;
            if (aList) {
                var totalCount = 0, childCount = 0;
                var dataProvider = aList.getDataProvider();
                var focusedItem = aList.getFocusedItem();
                if (dataProvider && focusedItem) {
                    a.showIndicator(true);
                    totalCount = dataProvider.getCount();
                    childCount = a._groupItemList.getParentItem(focusedItem.item.getType()).getChildCount();
                    if ((childCount > 0) && (totalCount !== childCount)) {
                        totalCount = childCount;
                    }
                    var totalPageCount = Math.ceil(totalCount / a.MAX_DETAIL_VIEW_ITEM);
                    var currentPageIdx = Math.floor(Number(focusedItem.$item.attr('data')) / a.MAX_DETAIL_VIEW_ITEM) + 1;
                    if (totalCount > 0) {
                        a._$index.html(hx.util.format('{0} / {1}', currentPageIdx, totalPageCount));
                        if (currentPageIdx === 1) {
                            a._$prev.removeClass('on');
                        } else {
                            a._$prev.addClass('on');
                        }
                        if (currentPageIdx === totalPageCount) {
                            a._$next.removeClass('on');
                        } else {
                            a._$next.addClass('on');
                        }
                    }
                    if (totalCount !== dataProvider.getCount()) {
                        // e.g.
                        // totalPageCount 10 (numOfItems : 100)
                        // realPageCount 3 (dataProvider.getCount() : 30)
                        // currentPageIdx 2
                        // => It sholud be updated at this time! (not 1 page)
                        // => Thus, the next currentPageIdx to update is 5!
                        var realPageCount = Math.ceil(dataProvider.getCount() / a.MAX_DETAIL_VIEW_ITEM);
                        if ((realPageCount - currentPageIdx) <= a.REMAINING_PAGE_COUNT_TO_UPDATE) {
                            if (a._paginations && a._paginations.next) {
                                hx.log('debug', '[requestSearchMore] next : ' + a._paginations.next);
                                if (a.onSearch) {
                                    a.onSearch(a._keyword, TSearchType.EGlobal, true, false, {
                                        target : a.TSearchTarget.EOnDemand,
                                        numOfItems : childCount,
                                        pageNum : Number(a._paginations.next.split('page=')[1])
                                    });
                                }
                                a._paginations = null;
                            }
                        }
                    }
                } else {
                    a.showIndicator(false);
                }
            } else {
                a._$next.removeClass('on');
                a._$prev.removeClass('on');
                a._$index.text('');
            }
        }
        showIndicator(bOn) {
            var a = this;
            if (bOn) {
                a._$indicator.addClass('on');
                if (a._groupView.getFocusedChild().getFocusedItem().item.getType() === a.TSearchTarget.EOnDemand) {
                    a._$last.css('opacity', '0');
                } else {
                    a._$last.css('opacity', '1');
                }
            } else {
                a._$indicator.removeClass('on');
            }
        }
        clearHistory() {
            var a = this;
            hx.svc.WEB_STORAGE.setSearchHistory(null);  // remove history data
            a.clear();
            a._searchView.setFocusedChld(a._searchControl);
        }
        updateHistory(aKeyword?) {
            var a = this;
            var searchHistory = hx.svc.WEB_STORAGE.getSearchHistory() || [];
            if (aKeyword) { // update!
                hx.log('CCategoryControl', '[updateHistory] : ' + aKeyword);
                var existedIdx = searchHistory.indexOf(aKeyword);
                if (existedIdx > -1) {
                    searchHistory.splice(existedIdx, 1);
                }
                searchHistory.unshift(aKeyword);
                if (searchHistory.length > a.MAX_SEARCH_HISTORY_ITEM) {
                    searchHistory.length = a.MAX_SEARCH_HISTORY_ITEM;
                }   // keep max(10) items
                hx.svc.WEB_STORAGE.setSearchHistory(searchHistory);
            } else {    // draw & show!
                hx.log('CCategoryControl', '[updateHistory] show & draw');
                if (searchHistory.length) {
                    searchHistory.push(a.STR_DELETE_HISTORY);
                }
                var dataProvider = new __CONTROLS__.CDataProvider(searchHistory);
                if (!a._historyView) {
                    a.clear();
                    a._historyView = a.createView(TViewType.EHistory);
                    a._historyView.list.setDataProvider(dataProvider);
                    a._mainControl.createLayer({
                        fnAfterCreate: function () {
                            var self = this;
                            self.setChildControl([a._historyView]);
                            self.draw();
                        }
                    });
                } else {
                    a._historyView.list.setDataProvider(dataProvider);
                    a._mainControl.setChildControl([a._historyView]);
                    a._historyView.draw();
                }
                a._historyView.$self().addClass('show');
            }
        }
        updateSuggestions(aSuggestion:IParsedSuggestion) {
            var a = this;
            if (!aSuggestion || !aSuggestion.keyword || !aSuggestion.suggestions) {
                hx.log('CCategoryControl', '[updateSuggestions] fail!! invalid suggestions from MDS');
                return;
            }
            a._lastSuggestion = aSuggestion;
            if (a._resultView) {
                hx.log('CCategoryControl', '[updateSuggestions] fail!! already search this string : ' + a._keyword);
                return;
            }
            if (a._historyView) {
                hx.log('CCategoryControl', '[updateSuggestions] fail!! already history list is shown');
                return;
            }
            var keyword = aSuggestion.keyword;
            var suggestions = aSuggestion.suggestions.slice(0);
            if (suggestions && keyword.length >= 3) {
                suggestions.unshift(a.STR_PREFIX_PARTIAL_SEARCH + ' ' + keyword);
            }
            var dataProvider = new __CONTROLS__.CDataProvider(suggestions);
            if (!a._suggestionView) {
                a._suggestionView = a.createView(TViewType.ESuggestion);
                a._suggestionView.list.setDataProvider(dataProvider);
                a._mainControl.createLayer({
                    fnAfterCreate: function () {
                        var self = this;
                        self.setChildControl([a._suggestionView]);
                        self.draw();
                    }
                });
            } else {
                a._suggestionView.list.setDataProvider(dataProvider);
                a._mainControl.setChildControl([a._suggestionView]);
                a._suggestionView.draw();
            }
            a._suggestionView.$self().addClass('show');
            suggestions = null;
        }
        update(data) {
            var a = this;
            if (a._suggestionView) {
                hx.log('CCategoryControl', '[update] fail!! already back to suggestions');
                return;
            }
            if (a._historyView) {
                hx.log('CCategoryControl', '[update] fail!! already back to history');
                return;
            }
            if (!a.SCategory) {
                hx.log('CCategoryControl', '[update] fail!! must set a.SCategory before update');
                return;
            }
            if (!data || !data.type) {
                hx.log('CCategoryControl', '[update] fail!! invalid data');
                return;
            }
            var i, item;
            var bAutoUpdate = true;
            var result = data.items;
            var numOfItems = data.numOfItems || 0;
            var pageNum = data.pageNum || 0;
            var type = data.type;
            var fnFindItemByType = function(arr, type) {
                for (var i in arr) {
                    if (arr[i].targetType === type) {
                        return arr[i];
                    }
                }
            };
            a._paginations = data.paginations || a._paginations;
            if (!a._groupItemList) {
                a._groupItemList = new CCategoryItemList(a.SCategory);
            }
            if (a._isWithoutKeyboard) {
                bAutoUpdate = true;
            } else {
                if (a._groupItemList.getParentItem(type)) {
                    bAutoUpdate = false;
                }
                if (type === a.TSearchTarget.EMore) {
                    bAutoUpdate = true;
                }
            }
            if (result) {
                for (i in result) {
                    item = new CCategoryItem(result[i], fnFindItemByType(a.SCategory, type));
                    a._groupItemList.push(item, numOfItems);
                }
                if (item || a._isWithoutKeyboard) {
                    // check a._isWithoutKeyboard to be clear even if result is empty
                    if (a._groupItemList.getParentItem(type).getSubItemList().length < a.MAX_DETAIL_VIEW_ITEM) {
                        if (a._paginations && a._paginations.next) {
                            hx.log('debug', '[requestSearchMore] next : ' + a._paginations.next);
                            if (a.onSearch) {
                                a.onSearch(a._keyword, TSearchType.EGlobal, true, false, {
                                    target : a.TSearchTarget.EOnDemand,
                                    numOfItems : a._groupItemList.getParentItem(type).getChildCount(),
                                    pageNum : Number(a._paginations.next.split('page=')[1])
                                });
                            }
                            a._paginations = null;
                        };
                    }
                    a.drawResult(bAutoUpdate, type);
                }
                if (a._groupItemList.getParentItem(type)) {
                    if (a._paginations && !a._paginations.next) {
                        // this is the last response for ondemand
                        // to satisfy MDS_504
                        numOfItems = a._groupItemList.getParentItem(type).getSubItemList().length;
                        a._groupItemList.getParentItem(type).setChildCount(numOfItems);
                    }
                    a.setGroupCount(type, numOfItems);
                    a.setIndexText(a._detailView.list);
                }
            }
        }
        drawResult(bAutoUpdate, type) {
            var a = this;
            var focusedGroupItem = null;
            var bInit = true;
            if (!a._resultView) {
                a._resultView = a.createView(TViewType.EResult);
                a._mainControl.createLayer({
                    fnAfterCreate: function () {
                        this.setChildControl([a._resultView]);
                        this.draw({
                            setActiveFocus: true
                        });
                    }
                });
                a._searchView.setFocusedChld(a._mainControl);
            } else {
                if (bAutoUpdate) {
                    if (a._isWithoutKeyboard && type) {
                        // just to update detail view
                        // nothing to do for group view
                    } else {
                        a._groupView.list.setDataProvider(new __CONTROLS__.CDataProvider(a._groupItemList.getDataList() || []));
                        a._groupView.list.setActiveIndex(0);
                        a._mainControl.setChildControl([a._resultView]);
                        a._resultView.draw({
                            setActiveFocus: true
                        });
                    }
                }
            }
            focusedGroupItem = a._groupView.list.getFocusedItem();
            if (focusedGroupItem) {
                if (!a._isWithoutKeyboard) {
                    focusedGroupItem.$item.addClass('selected');
                    bInit = false;
                    // keep the previous view even if data is changed (on Search)
                }
                if (focusedGroupItem.item.getType() === type) {
                    if (type === a.TSearchTarget.EOnDemand) {
                        if (a._detailView && a._detailView.list) {
                            if (a._detailView.list.getDrawnItems().getCount() < a.MAX_DETAIL_VIEW_ITEM) {
                                bAutoUpdate = true;
                            }
                        }
                    }
                    // update detail view if this result comes from focused item(group)
                    a.drawDetail(bInit, bAutoUpdate, false, focusedGroupItem.item);
                }
            }
        }
        drawDetail(bInit, bReDraw, keepFocus?, groupItem?) {
            var a = this;
            hx.log('CCategoryControl', 'drawDetail');
            if (a._isTransitioning) {
                hx.log('CCategoryControl', '[drawDetail] fail!! due to isTransitioning');
                return false;
            }
            var focusedDataItem;
            var $search = a._$smt.find('#search');
            if (groupItem || !$search.hasClass('-fd')) {
                if (a._mainControl.stackSize() === 0) {
                    if (groupItem) {
                        focusedDataItem = groupItem;
                    } else if (a._optionMenu) {
                        focusedDataItem = a._groupItemList.getParentItem(a._currentType);
                    } else {
                        focusedDataItem = a.getFocusedItem();
                    }
                    if (focusedDataItem) {
                        if (focusedDataItem.isDummy()) {
                            return false;
                        }
                        if (focusedDataItem.getCategory() === null) {
                            return false;
                        }
                        a.doSort(focusedDataItem.getType());
                        a._currentDetailProvider = new __CONTROLS__.CDataProvider(focusedDataItem.getSubItemList() || []);
                        a._detailView.list.setDataProvider(a._currentDetailProvider, !bReDraw);
                        if (bReDraw) {
                            a._detailView.draw({
                                left: bInit ? 0 : (a._detailView.getClientRect().left || 0),
                                setActiveFocus: bInit ? false : (a._detailView.getClientRect().left ? true : (keepFocus || false))
                            });
                        } else {
                            a._detailView.list.updateContentAvail({});
                        }
                        //a.setIndexText(a._detailView.list);
                        a.setTitleText(a._keyword);
                        a.showIndicator(true);
                        return true;
                    }
                }
            }
            return false;
        }
        addSubItems(subItems) {
            var a = this;
            var i, length;
            var subItem, dataProvider;
            var focusedView = a._mainControl.getFocusedChild();
            if (focusedView && focusedView.type === TViewType.EResult) {
                dataProvider = focusedView.list._dataProvider;
                var item = dataProvider.getItem(0);
                var category = item ? item.getCategory() : null;
                if (category) {
                    length = subItems.length;
                    for (i = 0; i < length; i += 1) {
                        subItem = subItems[i];
                        if (subItem.getCategory() === category) {
                            dataProvider.appendItem(new CCategoryItem(subItem));
                        }
                    }
                }
            } else {
                a.update(subItems);
            }
        }
        clear() {
            var a = this;
            if (a._groupItemList) {
                a._groupItemList.clear();
                a._groupItemList = null;
            }
            if (a._mainControl && a._mainControl.$layer()) {
                a._mainControl.removeLayer();
            }
            if (a._suggestionView) {
                a._suggestionView.destroy();
                a._suggestionView = null;
            }
            if (a._historyView) {
                a._historyView.destroy();
                a._historyView = null;
            }
            if (a._resultView) {
                a._resultView.destroy();
                a._resultView = null;
            }
            if (a._groupView) {
                a._groupView.destroy();
                if (a._groupView.list) {
                    a._groupView.list.destroy();
                    a._groupView.list = null;
                }
                a._groupView = null;
            }
            if (a._detailView) {
                a._detailView.destroy();
                if (a._detailView.list) {
                    a._detailView.list.destroy();
                    a._detailView.list = null;
                }
                a._detailView = null;
            }
            if (a._lastSuggestion) {
                //a._lastSuggestion = null;
            }
            if (a._optionMenu) {
                a._optionMenu.destroy();
                delete a._optionMenu;
                a._optionMenu = null;
            }
            a._currentType = null;
            a._paginations = null;
            a.setTitleText('');
            a.setIndexText();
            a.showIndicator(false);
        }
        setDataDrawer(param) {
            // TODO: make this on page
        }
        createView(viewType) {
            var a = this;
            var view = [
                function () {
                    var suggestionList, suggestionView;
                    suggestionList = new __CONTROLS__.CListControl(null);
                    suggestionList.setDrawParam({
                        itemHeight: 47,
                        scrollByItemSize: true
                    });
                    suggestionList.setDataDrawer(function (param) {
                        if (!param) {
                            return;
                        }
                        var drawItem = param ? param.item : null;
                        var drawItemKey = param ? param.index : 0;
                        var $drawItem = param ? param.$item : null;
                        if (drawItem && $drawItem) {
                            var highlightStr = hx.il.highlightInnerHTML(drawItem, a._lastSuggestion ? a._lastSuggestion.keyword : a._keyword);
                            if (a._keyword.length >= 3) {
                                if (($drawItem.attr('data') === "0") && (drawItem.indexOf(a.STR_PREFIX_PARTIAL_SEARCH) === 0)) {
                                    $drawItem.css('top', '-25px');
                                    $drawItem.addClass('searchby');
                                    suggestionView.$self().find('.title').css('top', '48px');
                                    highlightStr = a.STR_PREFIX_PARTIAL_SEARCH + ' ' + hx.il.highlightInnerHTML(a._lastSuggestion ? a._lastSuggestion.keyword : a._keyword, a._lastSuggestion ? a._lastSuggestion.keyword : a._keyword);
                                }
                            }
                            $drawItem.html(highlightStr);
                            $drawItem.addClass('suggestion');
                        }
                    });
                    suggestionList.draw = function (param) {
                        var self = this;
                        CListControl.prototype.draw.call(self, param);
                        if (a._mainControl) {
                            var focusedView = a._mainControl.getFocusedChild();
                            if (focusedView && focusedView.type === TViewType.ESuggestion) {
                                //a.setIndexText(suggestionList);
                            }
                        }
                    };
                    suggestionList.onFocusChanged = function () {
                        //a.setIndexText(suggestionList);
                    };
                    suggestionList.doKeyBack = function() {
                        //a.clear();
                        if (!a._isWithoutKeyboard) {
                            a._searchView.setFocusedChld(a._searchControl);
                        }
                        return true;
                    };
                    suggestionList.onItemSelected = function(param) {
                        var focusedItem = param ? param.item : null;
                        var focusedItemKey = param ? param.itemKey : 0;
                        var $focusedItem = param ? param.$item : null;
                        if (focusedItem) {
                            if ((focusedItemKey === 0) && (focusedItem.indexOf(a.STR_PREFIX_PARTIAL_SEARCH) === 0)) {
                                focusedItem = focusedItem.split(a.STR_PREFIX_PARTIAL_SEARCH + ' ')[1];
                            }
                            if (a.onSearch) {
                                a.onSearch(focusedItem, TSearchType.EGlobal, false, true);
                            }
                            a._$smt.find('h1').addClass('in');
                        }
                    };
                    suggestionView = new __CONTROLS__.CViewGroupControl($('<div>', {
                        id: 'suggestionList'
                    }));
                    suggestionView.setChildControl([suggestionList]);
                    suggestionView.list = suggestionList;
                    suggestionView.type = TViewType.ESuggestion;
                    suggestionView.draw();
                    suggestionView.$self().prepend($('<div>', {
                        'class': 'title',
                    }).text("SEARCH SUGGESTIONS"));
                    return suggestionView;
                },
                function () {
                    var groupList, detailList, groupView, detailView, resultView;
                    groupList = new __CONTROLS__.CListControl(null);
                    groupList.setDrawParam({
                        align: 'horizontal',
                        itemWidth: 227,
                        scrollByItemSize: true
                    });
                    groupList.setDataDrawer(function (param) {
                        param.drawType = 'group';
                        a.setDataDrawer(param);
                    });
                    groupList.draw = function (param) {
                        var self = this;
                        CListControl.prototype.draw.call(self, param);
                    };
                    groupList.onFocusChanged = function ($old, $new) {
                        if (!a._isWithoutKeyboard) {
                            if ($old !== $new) {    // remove from FocusedGained first
                                if (Number($new.attr('data')) === groupList.getCurrentIndex()) {
                                    // currentIndex from using setFocusedItemBy is different from when using doNavi.
                                    // As a result, it should be updated by only doNavi action here.
                                    a.drawDetail(true, true);
                                }
                            }
                            $old.removeClass('selected');
                            $new.addClass('selected');
                        }
                    };
                    groupList.onActiveFocusedGained = function($target) {
                        //$target.addClass('selected');
                    };
                    groupList.onActiveFocusedLost = function($target) {
                        //$target.removeClass('selected');
                    };
                    groupList.setDataProvider(new __CONTROLS__.CDataProvider(a._groupItemList.getDataList() || []));
                    groupList.onItemSelected = function(param) {
                        var focusedItem = param ? param.item : null;
                        var focusedItemKey = param ? param.itemKey : 0;
                        var $focusedItem = param ? param.$item : null;
                        if (a._isWithoutKeyboard && focusedItem && $focusedItem) {
                            if (focusedItem.getSubItemList().length === 0) {
                                if (a.onSearch) {
                                    a.onSearch(a._keyword, TSearchType.EGlobal, true, false, {
                                        target : focusedItem.getType()
                                    });
                                }
                            } else {
                                a.drawDetail(true, true);
                            }
                            $.each(groupList.getDrawnItems().items, function(i, aItem) {
                                aItem.removeClass('selected');
                            });
                            $focusedItem.addClass('selected');
                        }
                    };
                    groupList.doKeyBack = function() {
                        if (a._lastSuggestion) {
                            a.clear();
                            a.updateSuggestions(a._lastSuggestion);
                        }
                        if (!a._isWithoutKeyboard) {
                            a._searchView.setFocusedChld(a._searchControl);
                            return true;
                        }
                    };
                    //a._groupList = groupList;
                    groupView = new __CONTROLS__.CViewGroupControl($('<div>', {id: 'group'}));
                    groupView.setDrawParam({
                        showHidden: true
                    });
                    groupView.setChildControl([groupList]);
                    groupView.list = groupList;
                    a._groupView = groupView;

                    detailList = new CHorizontalGridControl(null);
                    detailList.setDrawParam({
                        itemHeight : 75,
                        itemWidth : 570,
                        maxRowCount: a._isWithoutKeyboard ? 6 : 5,
                        scrollByViewSize : true
                    });
                    detailList.setDataDrawer(function (param) {
                        param.drawType = 'detail';
                        a.setDataDrawer(param);
                    });
                    detailList.setDataProvider(a._currentDetailProvider);
                    detailList.doKeyBack = function() {
                        resultView.setFocusedChld(groupView);
                        return true;
                    };
                    detailList.onItemSelected = function(param) {
                        var bConsumed = false;
                        var focusedItem = param ? param.item : null;
                        var focusedItemKey = param ? param.itemKey : 0;
                        var $focusedItem = param ? param.$item : null;
                        if (focusedItem) {
                            var metadata = focusedItem.getMetadata();
                            if (metadata instanceof __PROVIDER__.CApplicationProvider || metadata instanceof __PROVIDER__.CWebContentProvider) {
                                if (a.onCheckPrivacyPolicyAccepted && !a.onCheckPrivacyPolicyAccepted()) {
                                    var buttonDlg = prism.createButtonDlg(null, {
                                        textHtml: tr('LOC_NEED_PRIVACY_POLICY_ID')
                                    });
                                    buttonDlg.open();
                                    bConsumed = true;
                                }
                            }
                            if (!bConsumed) {
                                if (a.onPlayRequest) {
                                    a.onPlayRequest({
                                        item: focusedItem
                                    });
                                }
                            }
                        }
                    };
                    detailList.draw = function (param) {
                        var self = this;
                        CHorizontalGridControl.prototype.draw.call(self, param);
                        if (a._mainControl) {
                            var focusedView = a._mainControl.getFocusedChild();
                            if (focusedView && focusedView.type === TViewType.EResult) {
                                a.setIndexText(detailList);
                            }
                        }
                    };
                    detailView = new __CONTROLS__.CViewGroupControl($('<div>', {id: 'detail'}));
                    detailView.setDrawParam({
                        withoutAnim : true
                    });
                    detailView.setChildControl([detailList]);
                    detailView.list = detailList;
                    a._detailView = detailView;

                    resultView = new __CONTROLS__.CVerticalGroupControl($('<div>', {id: 'result', 'class': (a._isWithoutKeyboard ? 'bgThum' : 'keyboard')}));
                    resultView.setChildControl([groupView, detailView]);
                    resultView.type = TViewType.EResult;
                    resultView.draw();
                    resultView.addClass('in');
                    return resultView;
                },
                function () {
                    var historyList, historyView;
                    historyList = new __CONTROLS__.CListControl(null);
                    historyList.setDrawParam({
                        itemHeight: 47,
                        scrollByItemSize: true
                    });
                    historyList.setDataDrawer(function (param) {
                        if (!param) {
                            return;
                        }
                        var drawItem = param ? param.item : null;
                        var drawItemKey = param ? param.index : 0;
                        var $drawItem = param ? param.$item : null;
                        if (drawItem && $drawItem) {
                            $drawItem.html(drawItem);
                            $drawItem.addClass('history');
                            var historyProvider = historyList.getDataProvider() || null;
                            var historyCount = historyProvider ? historyProvider.getCount() : 0;
                            if ((historyCount === (drawItemKey + 1)) && (drawItem === a.STR_DELETE_HISTORY)) {
                                $drawItem.addClass('delete');
                            }
                        }
                    });
                    historyList.onFocusChanged = function () {
                        //a.setIndexText(suggestionList);
                    };
                    historyList.onItemSelected = function(param) {
                        var selectedItem = param ? param.item : null;
                        var selectedItemKey = param ? param.itemKey : 0;
                        var $selectedItem = param ? param.$item : null;
                        if (selectedItem) {
                            var historyProvider = historyList.getDataProvider() || null;
                            var historyCount = historyProvider ? historyProvider.getCount() : 0;
                            if ((historyCount === (selectedItemKey + 1)) && (selectedItem === a.STR_DELETE_HISTORY)) {
                                a.clearHistory();
                            } else {
                                if (a.onSearch) {
                                    a.onSearch(selectedItem, TSearchType.EGlobal, false, true);
                                }
                            }
                            a._$smt.find('h1').addClass('in');
                        }
                    };
                    historyList.doKeyBack = function() {
                        //a.clear();
                        if (!a._isWithoutKeyboard) {
                            a._searchView.setFocusedChld(a._searchControl);
                        }
                        return true;
                    };
                    historyView = new __CONTROLS__.CViewGroupControl($('<div>', {
                        id: 'history'
                    }));
                    historyView.setChildControl([historyList]);
                    historyView.list = historyList;
                    historyView.type = TViewType.EHistory;
                    historyView.draw();
                    historyView.$self().prepend($('<div>', {
                        'class': 'title',
                    }).text("SEARCH HISTORY"));
                    return historyView;
                }][viewType]();
            return view;
        }
        getViewType() {
            return TViewType;
        }
        setCategory(categories) {
            var a = this;
            a.SCategory = categories;
        }
        getSearchType() {
            return TSearchType;
        }
        getParentLayer() {
            return this._parentLayer;
        }
        getCurrentProgramme() {
            var a = this;
            var focusedView = a._mainControl.getFocusedChild();
            if (focusedView.type === TViewType.EResult) {
                if (focusedView.getFocusedChild() === a._detailView) {
                    return a._detailView.getFocusedChild().getFocusedItem().item.getMetadata();
                }
            }
            return null;
        }
        getGroupTitleStr(aType) {
            if (aType === undefined) {
                hx.log('CCategoryControl', 'getGroupTitleStr : undefined "aType"');
                return '';
            }
            var a = this;
            var retStr = '';
            switch (aType) {
                case a.TSearchTarget.EOnDemand:
                    retStr = "ON DEMAND";
                    break;
                case a.TSearchTarget.EChannel:
                    retStr = "TV/RADIO CH";
                    break;
                case a.TSearchTarget.EProgramme:
                case a.TSearchTarget.EScheduled:
                    retStr = "BROADCAST";
                    break;
                case a.TSearchTarget.ERecording:
                    retStr = "RECORDING";
                    break;
                case a.TSearchTarget.EApplication:
                case a.TSearchTarget.EAppContent:
                case a.TSearchTarget.EDlnaContent:
                case a.TSearchTarget.EFile:
                case a.TSearchTarget.EMore:
                    retStr = "MORE";
                    break;
                default:
                    retStr = aType;
                    break;
            }
            return retStr;
        }
        showFocus() {
            var a = this;
            if (a._shownFocus) {
                a._shownFocus.addClass('-afd-leaf');
                a._shownFocus = null;
            }
        }
        hideFocus() {
            var a = this;
            var shownFocus = a._$smt.find('.-afd-leaf');
            if (shownFocus && shownFocus.length) {
                shownFocus.removeClass('-afd-leaf');
                a._shownFocus = shownFocus;
            }
        }
        getOptionDlg() {
            return this._optionMenu;
        }
        doSort(aTargetType?) {
            if (!this._isWithoutKeyboard) {
                hx.log('CCategoryControl', '[doSort] !this._isWithoutKeyboard');
            }
            if (this._optionMenu) {
                this._sortOption = this._optionMenu.getFocusedOption();
            }
            if (aTargetType) {
                this._currentType = aTargetType;
            }
            if (!this._currentType) {
                hx.log('CCategoryControl', '[doSort] !this._currentType');
                return;
            }
            var groupItem = this._groupItemList.getParentItem(this._currentType);
            var contentList = groupItem.getSubItemList();
            switch (this._sortOption) {
                case EOPTION_MENU.SORT_BY_TIME:
                    this._selectedOptText = hx.l('LOC_SORT_BY_TIME_ID').toUpperCase();
                    if (this.sortDescent) {
                        contentList.sort(function (a, b) {
                            return b.getStartTime() - a.getStartTime();
                        });
                    } else {
                        contentList.sort(function (a, b) {
                            return a.getStartTime() - b.getStartTime();
                        });
                    }
                    break;
                case EOPTION_MENU.SORT_BY_ALPHABET:
                    this._selectedOptText = hx.l('LOC_SORT_BY_ALPHABET_ID').toUpperCase();
                    contentList.sort(function (a, b) {
                        var A = a.getName().toLowerCase();
                        var B = b.getName().toLowerCase();
                        if (A < B) {
                            return -1;
                        } else if (A > B) {
                            return 1;
                        } else {
                            return 0;
                        }
                    });
                    break;
                case EOPTION_MENU.SORT_BY_CHANNEL:
                    this._selectedOptText = hx.l('LOC_SORT_BY_CHANNEL_ID').toUpperCase();
                    contentList.sort(function (a, b) {
                        return a.getChannelNo() - b.getChannelNo();
                    });
                    break;
                default:
                    throw "sort error";
            }
            this._$optTitle.text(this._selectedOptText);
        }
        updateScheduleItem(item, fnUpdate) {
            var a = this;
            var drawnItems = a._detailView ? a._detailView.list.getDrawnItems() : null;
            if (drawnItems && drawnItems.getCount()) {
                var classStr = '';
                var key = (function(drawnList, providerList, eventItem) {
                    var metadataItem, index = -1;
                    for (var i in drawnList) {
                        metadataItem = providerList.getItem(i);
                        if (metadataItem.getProgrammeID() === eventItem.getProgrammeID()) {
                            index = i;
                            break;
                        }
                    }
                    return index;
                })(drawnItems.items, a._currentDetailProvider, item);
                if (key !== -1) {
                    if (item.isReminder()) {
                        classStr = DClassForIconOnThumb.reminder;
                    } else if (item.isSeries()) {
                        classStr = DClassForIconOnThumb.series;
                    } else {
                        classStr = DClassForIconOnThumb.schedule;
                    }
                    if (fnUpdate) {
                        fnUpdate(drawnItems.getItem(key), classStr);
                    }
                }
            }
        }
        updateRecordingItem(item, fnUpdate) {
            var a = this;
            var groupList = a._groupView ? a._groupView.getFocusedChild() : null;
            if (groupList) {
                var focusedDataItem = a._groupItemList.getParentItem(groupList.getFocusedItem().item.getType());
                if (focusedDataItem && (focusedDataItem.getType() === a.TSearchTarget.ERecording)) {
                    var key = (function(aList, eventItem) {
                        var i, ti, item, index = -1;
                        for (i = 0, ti = aList.length; i < ti; i += 1) {
                            item = aList[i].getMetadata();
                            if (item.getRecordingID() === eventItem.getId()) {
                                index = i;
                                break;
                            }
                        }
                        return index;
                    })(focusedDataItem.getSubItemList(), item);
                    if (key !== -1) {
                        if (fnUpdate) {
                            fnUpdate(focusedDataItem, key);
                        }
                        // TODO? refresh all results
                        //groupList.setActiveFocus(0);
                        //focusedDataItem = groupList.getFocusedItem().item;
                        //a.drawResult(bAutoUpdate = true, focusedDataItem.getType());
                    }
                }
            }
        }
        onRecordingItemRemoved(item) {
            this.updateRecordingItem(item, (focusedItem, key) => {
                focusedItem.getSubItemList().splice(key, 1);
                this.drawDetail(false, true, true, focusedItem);
                this.setGroupCount(this.TSearchTarget.ERecording, focusedItem.getSubItemList().length);
                if (focusedItem.getSubItemList().length == 0) {
                    this._resultView.setFocusedChld(this._groupView);
                }
            });

        }
        onScheduledItemAdded(item) {
            this.updateScheduleItem(item, ($drawnItem, classStr) => {
                $drawnItem.addClass(classStr);
            });
        }
        onScheduledItemRemoved(item) {
            this.updateScheduleItem(item, ($drawnItem, classStr) => {
                $drawnItem.removeClass(classStr);
            });
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createCategoryControl = function (param) {
        return new CCategoryControl(param);
    };
}
export = mPrism;