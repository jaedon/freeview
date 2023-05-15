///<reference path="../../def/sugar.d.ts" />
// file name : pages/msgbox/cChUpdate.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var chupdate = (function (_super) {
        __extends(chupdate, _super);
        function chupdate() {
            _super.call(this);
            this.MAX_CH_SHOW_NUM = 4;
            this.FIRST_SCROLL_POSITION = 10;
            this.LAST_SCROLL_POSITION = 130;
            this.NEW_TYPE_TOP_POSITION = 118;
            this.REPLACED_TYPE_TOP_POSITION = 197;
            this.BOX_ELEMENT_HEIGHT = 183;
            this.SCROLL_ELEMENT_LEFT = 649;
            this.TEXT_FONT_SIZE = 27;
        }
        chupdate.prototype.init = function () {
            // TODO:
        };
        chupdate.prototype.create = function (aParam) {
            var that = this;
            $.extend(that, aParam, {
                _btn_title_arr: aParam.btn_title_arr,
                _msg_type: aParam.msg_type,
                _chInfos: aParam.chInfos,
                _callback_fn: aParam.callback_fn,
                _titleEl: document.createElement('h1'),
                _textEl: document.createElement('p'),
                _boxEl: document.createElement('aside'),
                _descriptEl: null,
                _btnElList: [],
                _boxDlEl: null,
                _scrollEl: null,
                _focusElList: []
            });
            var topPosition;
            if (that._msg_type === 'new') {
                topPosition = that.NEW_TYPE_TOP_POSITION + 'px';
            }
            else if (that._msg_type === 'replaced') {
                topPosition = that.REPLACED_TYPE_TOP_POSITION + 'px';
            }
            that._create({
                'tag': 'div',
                'attr': {
                    'class': 'con chUpdate',
                    'style': "top: " + topPosition
                }
            });
            that.chListManager = {
                'chList': that._chInfos,
                'curIdx': 0,
                'inc': function () {
                    this.curIdx += 1;
                },
                'dec': function () {
                    this.curIdx -= 1;
                }
            };
            that._titleEl.innerHTML = aParam.title_string;
            that._textEl.innerHTML = aParam.text_string;
            that._textEl.style.fontSize = that.TEXT_FONT_SIZE + 'px';
            that._boxEl.classList.add('box');
            that._boxEl.style.height = that.BOX_ELEMENT_HEIGHT + 'px';
            that._show_chList();
            that.$[0].appendChild(that._titleEl);
            that.$[0].appendChild(that._textEl);
            that.$[0].appendChild(that._boxEl);
            if (that._msg_type === 'new') {
                that._descriptEl = document.createElement('p');
                that._descriptEl.innerHTML = aParam.descript_string;
                that.$[0].appendChild(that._descriptEl);
            }
            var i, len = that._btn_title_arr.length;
            var btnEl = document.createElement('p');
            for (i = 0; i < len; i += 1) {
                var btnCloneEl = btnEl.cloneNode();
                btnCloneEl.innerHTML = that._btn_title_arr[i];
                btnCloneEl.classList.add('button');
                btnCloneEl.classList.add('double');
                that.$[0].appendChild(btnCloneEl);
                that._btnElList.push(btnCloneEl);
                that._focusElList.push(btnCloneEl);
            }
            that._btnElList[0].classList.add('on');
        };
        chupdate.prototype.destroy = function () {
            this.$.blur();
            this.$.remove();
            this._titleEl = undefined;
            this._textEl = undefined;
            this._boxEl = undefined;
            this._descriptEl = undefined;
            this._boxDlEl = undefined;
            this._scrollEl = undefined;
            this.chListManager = undefined;
            this._focusElList = undefined;
            this._btnElList = undefined;
            this._btn_title_arr = undefined;
            this._callback_fn = undefined;
        };
        chupdate.prototype._show_chList = function () {
            var that = this;
            that._boxEl.style.display = 'none';
            //remove boxDlEl child
            var i;
            if (that._boxDlEl) {
                var elLen = that._boxDlEl.children.length;
                for (i = 0; i < elLen; i += 1) {
                    that._boxDlEl.removeChild(that._boxDlEl.firstChild);
                }
            }
            //get lastShowIdx
            var lastShowIdx = that.chListManager.chList.length - that.chListManager.curIdx;
            if (lastShowIdx >= that.MAX_CH_SHOW_NUM) {
                lastShowIdx = that.MAX_CH_SHOW_NUM + that.chListManager.curIdx;
            }
            //if not made boxEl and scrollEl before, do make
            if (!that._boxDlEl) {
                that._boxDlEl = document.createElement('dl');
                that._boxEl.appendChild(that._boxDlEl);
                if (that.chListManager.chList.length <= that.MAX_CH_SHOW_NUM) {
                    that._boxDlEl.classList.add('noScroll');
                }
                else {
                    if (!that._scrollEl) {
                        that._scrollEl = document.createElement('p');
                        that._scrollEl.classList.add('scr');
                        that._scrollEl.style.top = that.FIRST_SCROLL_POSITION + 'px';
                        that._scrollEl.style.left = that.SCROLL_ELEMENT_LEFT + 'px';
                        that._boxEl.appendChild(that._scrollEl);
                        that._focusElList.push(that._scrollEl);
                    }
                }
            }
            //set scroll position
            if (that._scrollEl) {
                that._set_scrollPosition();
            }
            //make dtEl and ddEl which get chInfo
            var dtEl = document.createElement('dt');
            var ddEl = document.createElement('dd');
            for (i = that.chListManager.curIdx; i < lastShowIdx; i += 1) {
                var dtCloneEl = dtEl.cloneNode();
                dtCloneEl.innerHTML = that.chListManager.chList[i].name;
                var ddCloneEl = ddEl.cloneNode();
                ddCloneEl.innerHTML = that.chListManager.chList[i].lcn;
                that._boxDlEl.appendChild(dtCloneEl);
                that._boxDlEl.appendChild(ddCloneEl);
            }
            that._boxEl.style.display = 'block';
        };
        chupdate.prototype._set_scrollPosition = function () {
            if (this.chListManager.curIdx === 0) {
                this._scrollEl.style.top = this.FIRST_SCROLL_POSITION + 'px';
            }
            else {
                var scrollMoveSize = (this.LAST_SCROLL_POSITION - this.FIRST_SCROLL_POSITION) / (this.chListManager.chList.length - this.MAX_CH_SHOW_NUM);
                this._scrollEl.style.top = this.chListManager.curIdx * scrollMoveSize + 'px';
            }
        };
        chupdate.prototype._send_event = function (aParam) {
            aParam = (aParam) || {};
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.input_title = aParam.focusedEl.innerHTML;
            aParam.alData.me = this;
            aParam.alData.callback_fn = this._callback_fn;
            this.sendEventToParent(aParam);
        };
        // EVENT HANDLER
        chupdate.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var kc = aParam.alKey;
            var chListLen = this.chListManager.chList.length;
            var i, len = this._focusElList.length;
            var focusedEl, focusedIdx;
            for (i = 0; i < len; i += 1) {
                if (this._focusElList[i].classList.contains('on')) {
                    focusedIdx = i;
                    focusedEl = this._focusElList[i];
                    break;
                }
            }
            var newFocusIdx;
            switch (kc) {
                case hx.key.KEY_UP:
                    if (focusedEl === this._scrollEl) {
                        if (this.chListManager.curIdx > 0) {
                            focusedEl.classList.remove('on');
                            this.chListManager.dec();
                            this._show_chList();
                            this._scrollEl.classList.add('on');
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_DOWN:
                    if (focusedEl === this._scrollEl) {
                        if (chListLen > this.chListManager.curIdx + this.MAX_CH_SHOW_NUM) {
                            focusedEl.classList.remove('on');
                            this.chListManager.inc();
                            this._show_chList();
                            this._scrollEl.classList.add('on');
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_LEFT:
                    newFocusIdx = focusedIdx - 1;
                    if (newFocusIdx < 0) {
                        newFocusIdx = len - 1;
                    }
                    focusedEl.classList.remove('on');
                    this._focusElList[newFocusIdx].classList.add('on');
                    bConsumed = true;
                    break;
                case hx.key.KEY_CHANNEL_UP:
                    if (focusedEl === this._scrollEl) {
                        if (this.chListManager.curIdx >= this.MAX_CH_SHOW_NUM) {
                            focusedEl.classList.remove('on');
                            for (i = 0; i < this.MAX_CH_SHOW_NUM; i += 1) {
                                this.chListManager.dec();
                            }
                            this._show_chList();
                            this._scrollEl.classList.add('on');
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_CHANNEL_DOWN:
                    if (focusedEl === this._scrollEl) {
                        if (chListLen >= this.chListManager.curIdx + this.MAX_CH_SHOW_NUM + this.MAX_CH_SHOW_NUM) {
                            focusedEl.classList.remove('on');
                            for (i = 0; i < this.MAX_CH_SHOW_NUM; i += 1) {
                                this.chListManager.inc();
                            }
                            this._show_chList();
                            this._scrollEl.classList.add('on');
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_RIGHT:
                    newFocusIdx = focusedIdx + 1;
                    if (newFocusIdx >= len) {
                        newFocusIdx = 0;
                    }
                    focusedEl.classList.remove('on');
                    this._focusElList[newFocusIdx].classList.add('on');
                    bConsumed = true;
                    break;
                case hx.key.KEY_OK:
                    if (focusedEl === this._btnElList[0] || focusedEl === this._btnElList[1]) {
                        this._send_event({
                            'focusedEl': focusedEl
                        });
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_POWER:
                case hx.key.KEY_ESCAPE:
                    return false;
                    break;
            }
            return bConsumed;
        };
        return chupdate;
    })(__COMPONENT_KERNEL__);
    exports.chupdate = chupdate;
    var selectchlist = (function (_super) {
        __extends(selectchlist, _super);
        function selectchlist() {
            _super.call(this);
            this.MAX_CH_SHOW_NUM = 6;
        }
        selectchlist.prototype.init = function () {
            // TODO:
        };
        selectchlist.prototype.create = function (aParam) {
            var that = this;
            $.extend(that, aParam, {
                _callback_fn: aParam.callback_fn,
                _listInfos: aParam.listInfos,
                _titleEl: document.createElement('p'),
                _wrapChEl: document.createElement('p'),
                _notiEl: document.createElement('p'),
                _dimEl: null,
                _focusElList: []
            });
            that._create({
                'tag': 'div',
                'attr': {
                    'class': 'con vaBot btn2'
                }
            });
            that.chListManager = {
                'chList': that._listInfos,
                'curIdx': 0,
                'inc': function () {
                    this.curIdx += 1;
                    if (this.chList.length <= this.curIdx) {
                        this.curIdx = 0;
                    }
                },
                'dec': function () {
                    this.curIdx -= 1;
                    if (this.curIdx < 0) {
                        this.curIdx = this.chList.length - 1;
                    }
                },
                'setCur': function (index) {
                    this.curIdx = index;
                },
                'isFirst': function () {
                    if (this.curIdx === 0) {
                        return true;
                    }
                    else {
                        return false;
                    }
                },
                'isLast': function () {
                    if (this.curIdx === this.chList.length - 1) {
                        return true;
                    }
                    else {
                        return false;
                    }
                }
            };
            that._titleEl.innerHTML = aParam.title_string;
            that._notiEl.classList.add('noti');
            that._notiEl.innerHTML = aParam.text_string;
            that.$[0].appendChild(that._titleEl);
            that.$[0].appendChild(that._wrapChEl);
            that.$[0].appendChild(that._notiEl);
            //append dimEl
            if (that.chListManager.chList.length >= that.MAX_CH_SHOW_NUM) {
                that._dimEl = document.createElement('div');
                that._dimEl.classList.add('dimBg');
                that._dimEl.style.display = 'block';
                that.$[0].appendChild(that._dimEl);
            }
            that._show_chList(0);
        };
        selectchlist.prototype.destroy = function () {
            this.$.blur();
            this.$.remove();
            this._titleEl = undefined;
            this._wrapChEl = undefined;
            this._notiEl = undefined;
            this._dimEl = undefined;
            this.chListManager = undefined;
            this._focusElList = undefined;
            this._callback_fn = undefined;
        };
        selectchlist.prototype._show_chList = function (aFocusedIdx) {
            var i;
            this._wrapChEl.style.display = 'none';
            //empty focusElList
            this._focusElList = [];
            //remove chListEl
            var elLen = this._wrapChEl.querySelectorAll('.button').length;
            for (i = 0; i < elLen; i += 1) {
                this._wrapChEl.removeChild(this._wrapChEl.querySelector('.button'));
            }
            //get firstShowIdx
            var firstShowIdx = this.chListManager.curIdx;
            for (i = 0; i < aFocusedIdx; i += 1) {
                firstShowIdx -= 1;
                if (firstShowIdx < 0) {
                    firstShowIdx = this.chListManager.chList.length - 1;
                }
            }
            //get drawChannelCount
            var channelCount;
            if (this.chListManager.chList.length >= this.MAX_CH_SHOW_NUM) {
                channelCount = this.MAX_CH_SHOW_NUM;
            }
            else {
                channelCount = this.chListManager.chList.length;
            }
            //make chListEl which get chInfo
            var curIdx = this.chListManager.curIdx; //store curIdx
            this.chListManager.setCur(firstShowIdx);
            var chListEl = document.createElement('p');
            chListEl.classList.add('button');
            for (i = 0; i < channelCount; i += 1) {
                var chListCloneEl = chListEl.cloneNode();
                chListCloneEl.innerHTML = this.chListManager.chList[this.chListManager.curIdx].name;
                this._wrapChEl.appendChild(chListCloneEl);
                this.chListManager.inc();
                if (i < this.MAX_CH_SHOW_NUM - 1) {
                    this._focusElList.push(chListCloneEl);
                }
            }
            this.chListManager.setCur(curIdx); //return to curIdx
            this._focusElList[aFocusedIdx].classList.add('on');
            this._wrapChEl.style.display = 'block';
        };
        selectchlist.prototype._send_event = function (aParam) {
            aParam = (aParam) || {};
            aParam.alEvent = 'alClicked';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.input_title = aParam.focusedEl.innerHTML;
            aParam.alData.index = aParam.index;
            aParam.alData.me = this;
            aParam.alData.callback_fn = this._callback_fn;
            this.sendEventToParent(aParam);
        };
        // EVENT HANDLER
        selectchlist.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var kc = aParam.alKey;
            var chListLen = this.chListManager.chList.length;
            var i, len = this._focusElList.length;
            var focusedEl, focusedIdx;
            for (i = 0; i < len; i += 1) {
                if (this._focusElList[i].classList.contains('on')) {
                    focusedIdx = i;
                    focusedEl = this._focusElList[i];
                    break;
                }
            }
            switch (kc) {
                case hx.key.KEY_UP:
                    if (focusedEl !== this._focusElList[0] || chListLen < this.MAX_CH_SHOW_NUM) {
                        focusedEl.classList.remove('on');
                        this.chListManager.dec();
                        if (focusedEl === this._focusElList[0]) {
                            this._focusElList[len - 1].classList.add('on');
                        }
                        else {
                            this._focusElList[focusedIdx - 1].classList.add('on');
                        }
                    }
                    else {
                        this.chListManager.dec();
                        this._show_chList(focusedIdx);
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_DOWN:
                    if (focusedEl !== this._focusElList[len - 1] || chListLen < this.MAX_CH_SHOW_NUM) {
                        focusedEl.classList.remove('on');
                        this.chListManager.inc();
                        if (focusedEl === this._focusElList[len - 1]) {
                            this._focusElList[0].classList.add('on');
                        }
                        else {
                            this._focusElList[focusedIdx + 1].classList.add('on');
                        }
                    }
                    else {
                        this.chListManager.inc();
                        this._show_chList(focusedIdx);
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_CHANNEL_UP:
                    for (i = 0; i < this.MAX_CH_SHOW_NUM; i += 1) {
                        this.chListManager.dec();
                        this._show_chList(focusedIdx);
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_CHANNEL_DOWN:
                    for (i = 0; i < this.MAX_CH_SHOW_NUM; i += 1) {
                        this.chListManager.inc();
                        this._show_chList(focusedIdx);
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_LEFT:
                    bConsumed = true;
                    break;
                case hx.key.KEY_RIGHT:
                    bConsumed = true;
                    break;
                case hx.key.KEY_OK:
                    for (i = 0; i < len; i += 1) {
                        if (focusedEl === this._focusElList[i]) {
                            this._send_event({
                                'focusedEl': focusedEl,
                                'index': this.chListManager.chList[this.chListManager.curIdx].index
                            });
                        }
                    }
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                case hx.key.KEY_POWER:
                case hx.key.KEY_ESCAPE:
                    return false;
                    break;
            }
            return bConsumed;
        };
        return selectchlist;
    })(__COMPONENT_KERNEL__);
    exports.selectchlist = selectchlist;
});
