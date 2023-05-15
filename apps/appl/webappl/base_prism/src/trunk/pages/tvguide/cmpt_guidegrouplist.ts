//file name :  pages/tvguide/cGuideGroupList.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../def/sugar.d.ts" />
declare var hx;
import __KEYNAVIGATOR__ = require("pages/component/cmpt_keynavigator");

class cGuideGroupList extends __KEYNAVIGATOR__ {
    selectedInfo: any = {};
    _isOn: boolean = false;
    _FIRST_STEP: string = 'majorGroup';
    _SECOND_STEP: string = 'minorAlphabetGroup';
    _ROLLING_ROW_MAX: number = 12;
    $: JQuery;
    _divBox: any;
    _ulBox: any;
    _title: any;
    _firstGroupList: any;
    _secondAlphabetList: any;
    _channelDB: any;
    _langDB: any;
    _alphabetGroupDB: any;
    _strAlphabetGroupName: any;
    _contextList: any;
    _rowList: any;
    _dataState: any;
    _viewState: any;
    isActive: boolean;
    group: string;
    liveGroup: any;
    constructor() {
        super($('<section>', {
            'id' : 'opt',
            'class' : 'focusable',
            'name' : 'guideGroupList' // TODO find name (this.name)
        }), 'focusable', 'on');
        this.$ = this._$
        var i, j, l, ll;
        this._divBox = null;
        this._ulBox = null;
        this._title = null;
        this._firstGroupList = [];
        this._secondAlphabetList = [];
        this._channelDB = hx.config.CPgInfoHub.chGroup.groupDB;
        this._langDB = [];
        this._alphabetGroupDB = hx.config.CPgInfoHub.chGroup.alphabetGroupsDB;
        this._strAlphabetGroupName = [];
        this._init_Data();
        this._make_Data();
        this.create();
        //hx.logadd('guideGroupList');
    }
    create() {
        var divBox: HTMLElement, ulBox: HTMLElement, $title: JQuery;

        hx.log('guideGroupList', 'create()');

        this._prepare();
        this._getGroup();

        ulBox = document.createElement('ul');
        divBox = document.createElement('div');
        divBox.className = 'menu rol';

        $title = $('<p/>', {
            'id' : 'group',
            'class' : 'tit'
        });
        this.$.append($title);
        divBox.appendChild(ulBox);
        this.$[0].appendChild(divBox);

        this._divBox = divBox;
        this._ulBox = ulBox;
        this._title = $title;

        this._draw();
        this._setTitleInnerHTML();
    }
    destroy() {
        this.close();
        this._contextList = [];
        this._rowList = [];
        this._dataState = {
            prevRow : -1,
            prevContextList : []
        };
    }
    close() {
        this.selectedInfo = {};
        this._prepare();
        $(this._ulBox).empty();

        if (this._title.hasClass('on')) {
            this._title.removeClass('on');
        }
        this._draw();
        this.$.remove();
    }
    getCurrentViewState() {
        return (this._isOn) ? 'show' : 'hide';
    }
    showHideToggle() {
        if (this._isOn) {
            this.hide();
        } else {
            this.show();
        }
        this._isOn = !this._isOn;

        return this._isOn;
    }
    changeGroupForced(aGroup?) {
        var ulTvSvcCount: number = 0, ulRadioSvcCount: number = 0;
        var isGroupChange: boolean = false, szChangeGroup: string = '', param;

        if (aGroup && (aGroup !== this.group)) {
            isGroupChange = true;
            szChangeGroup = aGroup;
        } else {
            ulTvSvcCount = hx.svc.CH_LIST.getChannelCount({
                group : 'TV',
            }).chCnt;
            ulRadioSvcCount = hx.svc.CH_LIST.getChannelCount({
                group : 'RADIO',
            }).chCnt;

            if (this.group === 'RADIO') {
                if (ulTvSvcCount > 0) {
                    isGroupChange = true;
                    szChangeGroup = 'TV';
                }
            } else {
                if (ulRadioSvcCount > 0) {
                    isGroupChange = true;
                    szChangeGroup = 'RADIO';
                }
            }
        }

        if (isGroupChange === true) {// changeChannelGroup
            this._setGroupForced(szChangeGroup);
            this._setTitleInnerHTML();
            this._title.removeClass('on');
            param = {
                'action' : null,
                'group' : null
            }
            this._messageToParent(param, true);
        }
    }
    _setGroupForced(aGroup) {
        var i: number = 0, length: number = 0, j: number = 0, len: number = 0;
        var currentGroup = aGroup;

        for ( i = 0, length = this._channelDB.length; i < length; i += 1) {
            if (currentGroup === this._channelDB[i]) {
                for ( j = 0, len = this._firstGroupList.length; j < len; j += 1) {
                    if (this._firstGroupList[j] === this._langDB[i]) {
                        this._rowList[this._viewState.focusedRow].element.classList.remove('on');
                        this._rowList[j].element.classList.add('on');
                        this._viewState.focusedRow = j;
                        this._dataState.prevRow = j;
                        break;
                    }
                }
                this.group = currentGroup;
                break;
            }
        }
    }
    KEY_OK(param): boolean {
        var currentStep = this._viewState.step, focusedGroup = this._contextList[this._viewState.focusedRow], p, result = [];
        var bSupportAlpahbet = false;

        hx.log('guideGroupList', 'KEY_OK');
        hx.log('guideGroupList', hx.util.format('currentStep : {0}, focusedGroup : {1}', currentStep, focusedGroup));

        if (currentStep === this._FIRST_STEP) {
            hx.log('guideGroupList', hx.util.format('firstNibble[0] : {0}', this._firstGroupList[0]));

            if (this._dataState.prevRow !== this._viewState.focusedRow) {
                bSupportAlpahbet = this._selectAlphabetChGroup(this._viewState.focusedRow);
                if (bSupportAlpahbet === true) {
                    this._changeAlphabetGroupList();
                } else {
                    this._updateSelectedChannelGroup(this._viewState.focusedRow);
                    this._setTitleInnerHTML();
                    this._title.removeClass('on');
                    this._messageToParent(param, true);
                    this.showHideToggle();
                }
            } else {
                bSupportAlpahbet = this._selectAlphabetChGroup(this._viewState.focusedRow);
                if (bSupportAlpahbet === true) {
                    this._changeAlphabetGroupList();
                } else {
                    return this._actCloseByKEYInput(param);
                }
            }
        } else {
            if (hx.config.useAlphabetGroup === 1) {
                this._updateSelectedAlphabetChannelGroup(this._viewState.focusedRow);
                this._setTitleInnerHTML();
                this._title.removeClass('on');
                this._messageToParent(param, true);
                this.showHideToggle();
            } else {
                this._actCloseByKEYInput(param);
            }
        }

        return true;
    }
    _actCloseByKEYInput(aParam): boolean {
        var step, row, isConsumed: boolean;

        if (this._isOn) {
            step = this._viewState.step;
            if (step === this._FIRST_STEP) {
                this._messageToParent(aParam, false);
                this.showHideToggle();
            } else {
                this._viewState.step = this._FIRST_STEP;
                this._viewState.focusedRow = this._dataState.prevRow;
                this._contextList = this._dataState.prevContextList;

                if (this._title.hasClass('on')) {
                    this._title.removeClass('on');
                }

                $(this._ulBox).empty();
                this._draw();
            }

            isConsumed = true;
        }

        return isConsumed;
    }
    KEY_ESCAPE(aParam): boolean {
        return this._actCloseByKEYInput(aParam);
    }
    KEY_BACK(aParam): boolean {
        return this._actCloseByKEYInput(aParam);
    }
    KEY_UP(aParam): boolean {
        var p, l, newOne = [];

        if (!this.doNavi(aParam)) {
            p = {
                offset : -1,
                count : 1,
                anchorGroup : this._contextList[0]
            };

            newOne = this._getLoadGroupItem(p);
            l = this._contextList.length;
            this._contextList.splice(l - 1, 1);
            this._contextList = newOne.concat(this._contextList);

            $(this._ulBox).empty();
            this._draw();
        }

        return true;
    }
    KEY_DOWN(param): boolean {
        var p, l, newOne = [];

        if (!this.doNavi(param)) {
            p = {
                offset : 1,
                count : 1,
                anchorGroup : this._contextList[this._ROLLING_ROW_MAX - 1]
            };

            newOne = this._getLoadGroupItem(p);

            this._contextList.splice(0, 1);
            this._contextList = this._contextList.concat(newOne);

            $(this._ulBox).empty();
            this._draw();
        }

        return true;
    }
    KEY_OPTION(param): boolean {
        this._messageToParent(param, false);
        this.showHideToggle();

        return true;
    }
    KEY_CHANNEL_UP(param): boolean {
        return true;
    }
    KEY_CHANNEL_DOWN(param): boolean {
        return true;
    }
    KEY_LEFT(param): boolean {
        return true;
    }
    KEY_RIGHT(param): boolean {
        return true;
    }
    KEY_FAST_FWD(param): boolean {
        return true;
    }
    KEY_REWIND(param): boolean {
        return true;
    }
    KEY_PLAY_SPEED_UP(param): boolean {
        return true;
    }
    KEY_PLAY_SPEED_DOWN(param): boolean {
        return true;
    }
    onFocusChanged($old, $new) {
        this._viewState.focusedRow = Number($new.attr('row'));
    }
    // _ROLLING_ROW_MAX : 11
    _draw() {
        var i, l, li, focusedRow, datas, ulBox = this._ulBox, divBox = this._divBox;

        datas = this._contextList;
        l = datas.length;
        divBox.className = (l < this._ROLLING_ROW_MAX) ? 'menu' : 'menu rol';
        ulBox.style.marginTop = (l < this._ROLLING_ROW_MAX) ? '30px' : '-20px';
        this._viewState.defaultRow = (l < this._ROLLING_ROW_MAX) ? 0 : 1;
        focusedRow = this._viewState.focusedRow;

        for ( i = 0; i < l; i += 1) {
            li = document.createElement('li');

            if (l > this._ROLLING_ROW_MAX - 1) {
                if (i > 0 && i < (this._ROLLING_ROW_MAX - 1)) {
                    li.className = (i === focusedRow) ? 'focusable onFocus on' : 'focusable onFocus';
                }
            } else {
                li.className = (i === focusedRow) ? 'focusable onFocus on' : 'focusable onFocus';
            }

            li.style = 'width:auto;';
            li.setAttribute('row', i);
            li.setAttribute('col', 0);
            li.innerHTML = datas[i];

            ulBox.appendChild(li);
            this._rowList.push({
                'element' : li
            });
        }

        if (this._viewState.step === this._SECOND_STEP) {
            this._title.addClass('on');
        }

        this.build(this.upDownBuilder, l < this._ROLLING_ROW_MAX);
    }
    _prepare() {
        this._contextList = [];
        this._rowList = [];
        this._viewState = {
            step : this._FIRST_STEP,
            focusedRow : 0
        };
        this._dataState = {
            prevRow : -1,
            prevContextList : []
        };
        var p = {
            offset : -1,
            count : this._ROLLING_ROW_MAX,
            anchorGroup : null
        };

        this._contextList = this._getLoadGroupItem(p);
    }
    _getLoadGroupItem(aParam) {
        var i, l, start, end, result = [], datas;

        hx.log('guideGroupList', '_getLoadGroupItem');
        hx.log('guideGroupList', hx.util.format('step : {0}', this._viewState.step));

        if (this._viewState.step === this._FIRST_STEP) {
            datas = this._firstGroupList;
        } else {
            if (hx.config.useAlphabetGroup === 1) {
                datas = this._secondAlphabetList;
            } else {
                datas = this._firstGroupList;
            }
        }

        if ((typeof aParam.anchorGroup === undefined) || (aParam.anchorGroup === null) || (aParam.anchorGroup === undefined)) {
            aParam.anchorGroup = datas[0];
        }

        if (aParam.count > datas.length) {
            aParam.count = datas.length;
            aParam.offset = 0;
        }

        for ( i = 0, l = datas.length; i < l; i += 1) {
            if (aParam.anchorGroup === datas[i]) {
                start = i + aParam.offset;
                end = start < 0 ? (l + start + aParam.count) : (start + aParam.count);
                result = datas.slice(start, end);
                if (result.length < aParam.count) {
                    start = 0;
                    end = Math.min(aParam.count - result.length, datas.length - result.length);
                    result = result.concat(datas.slice(start, end));
                }
                break;
            }
        }

        return result;
    }
    show() {
        var $elem, focus;

        this.$.addClass('onFocus on');
        focus = document.getElementById('focus');
        if (focus) {
            focus.style.display = 'none';
        }
        this.isActive = true;
    }
    hide() {
        var focus;

        this.$.removeClass('onFocus on');
        this.isActive = false;

        focus = document.getElementById('focus');
        if (focus) {
            focus.style.display = 'block';
        }

        this._prepare();
        this._getLastGroup();
        $(this._ulBox).empty();
        this._draw();
    }
    _setTitleInnerHTML(genre?) {
        var data, row;
        var strGroup = hx.l('LOC_OPT_GROUP_ID');
        var currentStep = this._viewState.step
        var dispGroup;

        row = this._viewState.focusedRow;
        dispGroup = this._contextList[row];

        if (hx.config.useAlphabetGroup === 1) {
            if (currentStep === this._SECOND_STEP) {
                var currentGroup = this._checkAlphabetGroup(this.group);
                if (currentGroup === 'A-Z') {
                    dispGroup = hx.l('LOC_A_Z_ID');
                }
            }
        }

        this._title.html(strGroup + ' - <em>' + dispGroup + '</em>');
    }
    _messageToParent(aParam, aIsGenreChange: boolean) {
        aParam.action = 'closeChannelGroupOption';
        aParam.isGroupChange = aIsGenreChange;
        this.sendEventToParent(aParam);

        if (aIsGenreChange === false) {
            this._viewState.step = this.selectedInfo.step;
            this._viewState.focusedRow = this.selectedInfo.focusedRow;
            this._contextList = this.selectedInfo.contextList;
        } else if (aIsGenreChange === true) {
            this.selectedInfo.step = this._viewState.step;
            this.selectedInfo.focusedRow = this._viewState.focusedRow;
            this.selectedInfo.contextList = this._contextList;
        }
    }
    goBuild() {
        this.build(this.upDownBuilder, true);
    }
    _init_Data() {
        var i: number = 0;

        // Init. Group List Name
        for ( i = 0; i < hx.config.CPgInfoHub.chGroup.groupName.length; i++) {
            this._langDB[i] = hx.l(hx.config.CPgInfoHub.chGroup.groupName[i]);
        }

        // Init. Alphabet Group Name
        if (hx.config.useAlphabetGroup === 1) {
            for ( i = 0; i < hx.config.CPgInfoHub.chGroup.alphabetGroupName.length; i++) {
                if (hx.config.CPgInfoHub.chGroup.alphabetGroupsDB[i] === 'AGALL') {
                    this._strAlphabetGroupName[i] = hx.l(hx.config.CPgInfoHub.chGroup.alphabetGroupName[i]);
                } else {
                    this._strAlphabetGroupName[i] = hx.config.CPgInfoHub.chGroup.alphabetGroupName[i];
                }
            }
        }
    }
    _make_Data() {
        var i: number = 0, ulIdx: number = 0, count: number;

        // Group List
        ulIdx = 0
        for (i = 0, length = this._channelDB.length; i < length; i += 1) {
            count = hx.svc.CH_LIST.getChannelCount({
                    'group' : this._channelDB[i]
            }).chCnt;

            if ((hx.config.useAlphabetGroup === 1) && (this._channelDB[i] === 'A-Z')) {
                count = hx.svc.CH_LIST.getChannelCount({
                        group : 'AGALL',
                }).chCnt;
            }

            hx.log('guideGroupList', hx.util.format('===== [{0}] group : {1}, count : {2} ===== ', this.name, this._channelDB[i], count));

            var bIsFavroureGroup = this._checkFavourite(this._channelDB[i]);
            if (bIsFavroureGroup === true) {
                //if (this._channelDB[i].substr(0,9) == 'Favourite') {
                this._langDB[i] = hx.svc.CH_LIST.getFavListName({
                        'favtype' : this._channelDB[i]
                });
                hx.log('guideGroupList', '[' + i + '] index is founded Favourite Group : ' + this._langDB[i]);
            }

            if (count > 0) {
                this._firstGroupList[ulIdx] = this._langDB[i];
                ulIdx++;
            }
        }

        if (hx.config.useAlphabetGroup === 1) {
            ulIdx = 0;
            for ( i = 0, length = this._alphabetGroupDB.length; i < length; i += 1) {
                count = hx.svc.CH_LIST.getChannelCount({
                        group : this._alphabetGroupDB[i]
                }).chCnt;

                if (count > 0) {
                    this._secondAlphabetList[ulIdx] = this._strAlphabetGroupName[i];
                    ulIdx++;
                }
            }
        }
    }
    _getGroup() {
        var i = 0, length = 0, j = 0, len = 0;
        var currentGroup;

        this.liveGroup = hx.svc.CH_LIST.getCurrentChannel().group || '';

        if (hx.config.useAlphabetGroup === 1) {
            currentGroup = this._checkAlphabetGroup(this.liveGroup);
        } else {
            currentGroup = this.liveGroup;
        }

        for ( i = 0, length = this._channelDB.length; i < length; i += 1) {
            if (currentGroup === this._channelDB[i]) {
                for ( j = 0, len = this._firstGroupList.length; j < len; j += 1) {
                    if (this._firstGroupList[j] === this._langDB[i]) {
                        this._viewState.focusedRow = j;
                        this._dataState.prevRow = j;
                        break;
                    }
                }
                this.group = this.liveGroup;
                break;
            }
        }
    }
    _getLastGroup() {
        var i, length, j, len;
        var currentGroup;

        if (hx.config.useAlphabetGroup === 1) {
            currentGroup = this._checkAlphabetGroup(this.group);
        } else {
            currentGroup = this.group;
        }

        for ( i = 0, length = this._channelDB.length; i < length; i += 1) {
            if (currentGroup === this._channelDB[i]) {
                for ( j = 0, len = this._firstGroupList.length; j < len; j += 1) {
                    if (this._firstGroupList[j] === this._langDB[i]) {
                        this._viewState.focusedRow = j;
                        this._dataState.prevRow = j;
                        break;
                    }
                }
                break;
            }
        }
    }
    _updateSelectedChannelGroup(aRow) {
        var i, length;

        for ( i = 0, length = this._langDB.length; i < length; i += 1) {
            if (this._contextList[aRow] === this._langDB[i]) {
                this.group = this._channelDB[i];
                break;
            }
        }
    }
    _checkFavourite(aParam) {
        if (aParam.substr(0,9) == 'Favourite') {
            return true;
        } else {
            return false;
        }
    }
    // TODO : Alphabet Group Code
    _selectAlphabetChGroup(aParam): boolean {
        var bReturn: boolean = false;

        if (hx.config.useAlphabetGroup === 1) {
            if (this._contextList[aParam] === 'A-Z') {
                bReturn = true;
            } else {
                bReturn = false;
            }
        } else {
            bReturn = false;
        }

        return bReturn;
    }
    _checkAlphabetGroup(aParam) {
        var i: number = 0;

        for (i = 0; i < this._alphabetGroupDB.length; i++) {
            if (this._alphabetGroupDB[i] === aParam) {
                return	'A-Z';
            }
        }

        return	aParam;
    }
    _updateSelectedAlphabetChannelGroup(row) {
        var i: number = 0, length: number = 0;

        for ( i = 0, length = this._strAlphabetGroupName.length; i < length; i += 1) {
            if (this._contextList[row] === this._strAlphabetGroupName[i]) {
                this.group = this._alphabetGroupDB[i];
                break;
            }
        }
    }
    _changeAlphabetGroupList() {
        var p;

        this._dataState.prevRow = this._viewState.focusedRow;
        this._dataState.prevContextList = this._contextList;
        this._viewState.step = this._SECOND_STEP;
        p = {
            offset : -1,
            count : this._ROLLING_ROW_MAX,
            anchorGroup : null
        };

        this._contextList = this._getLoadGroupItem(p);
        this._viewState.focusedRow = (this._contextList.length < this._ROLLING_ROW_MAX) ? 0 : 1;
        $(this._ulBox).empty();
        this._draw();
    }
}
export = cGuideGroupList;
