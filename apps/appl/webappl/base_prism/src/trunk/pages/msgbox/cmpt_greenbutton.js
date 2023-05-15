///<reference path="../../def/sugar.d.ts" />
// file name : pages/msgbox/cmpt_greenbutton.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel", "../../def/message.def"], function (require, exports, __COMPONENT_KERNEL__, __MSG_DEF__) {
    (function (direction) {
        direction[direction["NONE"] = 0] = "NONE";
        direction[direction["PREV"] = 1] = "PREV";
        direction[direction["NEXT"] = 2] = "NEXT";
        direction[direction["ALL"] = 3] = "ALL";
    })(exports.direction || (exports.direction = {}));
    var direction = exports.direction;
    var greenButton = (function (_super) {
        __extends(greenButton, _super);
        function greenButton() {
            _super.call(this);
            this.$ = null;
            this._$greenBtn = null;
            this._$detailDlg = null;
            this._elMinipop = null;
            this._elMiniMessage = null;
            this._elText = null;
            this._elWrapTitle = null;
            this._elTitle = null;
            this._elIcon = null;
            this._elWrapPaging = null;
            this._elWrapContent = null;
            this._elWrapDescript = null;
            this._elDescript = null;
            this._elWrapBtn = null;
            this._elUlBtn = null;
            this._elRecBtn = null;
            this._elRemindBtn = null;
            this._updownKeyManager = null;
            this._callback_fn = null;
            this._programmes = null;
            this._curDetailIdx = 0;
            this._noneIconClass = 'icon';
            this._recIconClass = 'icon rec';
            this._seriesRecIconClass = 'icon rec_series';
            this._remindIconClass = 'icon remind';
        }
        greenButton.prototype.create = function (aParam) {
            this.$ = $('<section>');
            this._$greenBtn = $('<section>');
            this._$greenBtn[0].id = 'trailer';
            this._$detailDlg = $('<section>');
            this._$detailDlg[0].id = 'trailer2';
            this._$greenBtn.hide();
            this._$detailDlg.hide();
            this.$[0].appendChild(this._$greenBtn[0]);
            this.$[0].appendChild(this._$detailDlg[0]);
            this._programmes = aParam.programmes;
            this._callback_fn = aParam.callback_fn;
            this._showGreenBtn();
            return this;
        };
        greenButton.prototype.resetTimeout = function () {
            this.__send_timer_refresh();
        };
        greenButton.prototype.destroy = function () {
            this.$.remove();
            this.$ = null;
            this._callback_fn = null;
            this._$greenBtn = null;
            this._$detailDlg = null;
            this._elMinipop = null;
            this._elMiniMessage = null;
            this._elText = null;
            this._elWrapTitle = null;
            this._elTitle = null;
            this._elIcon = null;
            this._elWrapPaging = null;
            this._elWrapContent = null;
            this._elWrapDescript = null;
            this._elDescript = null;
            this._elWrapBtn = null;
            this._elUlBtn = null;
            this._elRecBtn = null;
            this._elRemindBtn = null;
            this._updownKeyManager = null;
            this._programmes = null;
        };
        greenButton.prototype.show = function () {
            this.$.show();
        };
        greenButton.prototype.hide = function () {
            this.$.hide();
        };
        // EVENT HANDLER
        greenButton.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var kc = aParam.alKey;
            var curFocusEl = null;
            if (this._$greenBtn[0].style.display === "none") {
                switch (kc) {
                    case hx.key.KEY_UP:
                        this._updownKeyManager.doUp();
                        bConsumed = true;
                        break;
                    case hx.key.KEY_DOWN:
                        this._updownKeyManager.doDown();
                        bConsumed = true;
                        break;
                    case hx.key.KEY_LEFT:
                        curFocusEl = this._updownKeyManager.getCurrentFocusEl();
                        if (curFocusEl) {
                            if (curFocusEl === this._elWrapPaging) {
                                if (this._curDetailIdx > 0) {
                                    this._curDetailIdx -= 1;
                                    this._updateDetailInfo();
                                }
                            }
                        }
                        bConsumed = true;
                        break;
                    case hx.key.KEY_RIGHT:
                        curFocusEl = this._updownKeyManager.getCurrentFocusEl();
                        if (curFocusEl) {
                            if (curFocusEl === this._elWrapPaging) {
                                if (this._curDetailIdx < this._programmes.length - 1) {
                                    this._curDetailIdx += 1;
                                    this._updateDetailInfo();
                                }
                            }
                        }
                        bConsumed = true;
                        break;
                    case hx.key.KEY_OK:
                        curFocusEl = this._updownKeyManager.getCurrentFocusEl();
                        if (curFocusEl) {
                            if (curFocusEl === this._elRecBtn) {
                                if (this._isSeries(this._programmes[this._curDetailIdx])) {
                                    this.__send_event(1 /* SERIES_RECORD */);
                                    this._elIcon.className = this._seriesRecIconClass;
                                }
                                else {
                                    this.__send_event(0 /* RECORD */);
                                    this._elIcon.className = this._recIconClass;
                                }
                            }
                            else if (curFocusEl === this._elRemindBtn) {
                                this.__send_event(2 /* REMINDER */);
                                this._elIcon.className = this._remindIconClass;
                            }
                        }
                        bConsumed = true;
                        break;
                    case hx.key.KEY_BACK:
                    case hx.key.KEY_ESCAPE:
                        this._showGreenBtn();
                        bConsumed = true;
                        break;
                    case hx.key.KEY_MENU:
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
                        bConsumed = true;
                        break;
                }
            }
            else {
                switch (kc) {
                    case hx.key.KEY_GREEN:
                        this._showDetailDlg();
                        bConsumed = true;
                        break;
                }
            }
            this.__send_timer_refresh(aParam);
            return bConsumed;
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        greenButton.prototype._isSeries = function (programme) {
            var isSeries = false;
            if (programme) {
                if (programme.seriesId) {
                    if (programme.seriesId !== "") {
                        isSeries = true;
                    }
                }
            }
            return isSeries;
        };
        greenButton.prototype._isProgEmpty = function (programme) {
            var isEmpty = true;
            if (programme) {
                if (programme.programmeID) {
                    isEmpty = false;
                }
            }
            return isEmpty;
        };
        greenButton.prototype._showGreenBtn = function () {
            this._$greenBtn.hide();
            this._$detailDlg.hide();
            if (!this._elMinipop) {
                this._elMinipop = document.createElement('section');
                this._elMinipop.id = 'minipop';
                this._elMinipop.className = 'dimTrailer';
                this._$greenBtn[0].appendChild(this._elMinipop);
            }
            if (!this._elMiniMessage) {
                this._elMiniMessage = document.createElement('section');
                this._elMiniMessage.className = 'minimessage';
                this._elMinipop.appendChild(this._elMiniMessage);
            }
            if (!this._elText) {
                this._elText = document.createElement('p');
                this._elText.className = 'txt';
                this._elText.innerHTML = "<em class='greenbtn'></em>";
                this._elMiniMessage.appendChild(this._elText);
            }
            this._$greenBtn.show();
        };
        greenButton.prototype._showDetailDlg = function () {
            var title = this._programmes[this._curDetailIdx].name;
            var description = this._programmes[this._curDetailIdx].longDescription;
            var isSeries = this._isSeries(this._programmes[this._curDetailIdx]);
            var naviDirect = this._findNaviDirection_for_detailDlg();
            this._$greenBtn.hide();
            this._$detailDlg.hide();
            this._makeTitle_for_detailDlg(title);
            this._makePagingArrow_for_detailDlg(naviDirect);
            this._makeContent_for_detailDlg(description, isSeries);
            this._makeKeyManager_for_detailDlg();
            this._$detailDlg.show();
        };
        greenButton.prototype._updateDetailInfo = function () {
            var title = this._programmes[this._curDetailIdx].name;
            var description = this._programmes[this._curDetailIdx].longDescription;
            var isSeries = this._isSeries(this._programmes[this._curDetailIdx]);
            var naviDirect = this._findNaviDirection_for_detailDlg();
            this._$detailDlg.hide();
            this._makeTitle_for_detailDlg(title);
            this._makePagingArrow_for_detailDlg(naviDirect);
            this._makeContent_for_detailDlg(description, isSeries);
            this._$detailDlg.show();
        };
        greenButton.prototype._makeTitle_for_detailDlg = function (title) {
            if (!this._elWrapTitle) {
                this._elWrapTitle = document.createElement('hgroup');
                this._elWrapTitle.classList.add('wrap_title');
                this._$detailDlg[0].appendChild(this._elWrapTitle);
            }
            if (!this._elIcon) {
                this._elIcon = document.createElement('em');
                this._elWrapTitle.appendChild(this._elIcon);
            }
            if (!this._elTitle) {
                this._elTitle = document.createElement('h1');
                this._elTitle.classList.add('tit_tr');
                this._elWrapTitle.appendChild(this._elTitle);
            }
            this._elIcon.className = this._noneIconClass;
            this._elTitle.innerHTML = title;
        };
        greenButton.prototype._makePagingArrow_for_detailDlg = function (pagingDirection) {
            if (!this._elWrapPaging) {
                this._elWrapPaging = document.createElement('hgroup');
                this._elWrapPaging.classList.add('paging_tra');
                this._$detailDlg[0].appendChild(this._elWrapPaging);
            }
            if (pagingDirection === 1 /* PREV */) {
                this._elWrapPaging.innerHTML = "<span class='pagingPrv'></span><span class=''></span>";
            }
            else if (pagingDirection === 2 /* NEXT */) {
                this._elWrapPaging.innerHTML = "<span class=''></span><span class='pagingNxt'></span>";
            }
            else if (pagingDirection === 3 /* ALL */) {
                this._elWrapPaging.innerHTML = "<span class='pagingPrv'></span><span class='pagingNxt'></span>";
            }
            else {
                this._elWrapPaging.innerHTML = "";
            }
        };
        greenButton.prototype._makeContent_for_detailDlg = function (description, isSeries) {
            if (!this._elWrapContent) {
                this._elWrapContent = document.createElement('hgroup');
                this._elWrapContent.classList.add('wrap_epCont');
                this._$detailDlg[0].appendChild(this._elWrapContent);
            }
            if (!this._elWrapDescript) {
                this._elWrapDescript = document.createElement('article');
                this._elWrapDescript.classList.add('cont');
                this._elWrapContent.appendChild(this._elWrapDescript);
            }
            if (!this._elWrapBtn) {
                this._elWrapBtn = document.createElement('article');
                this._elWrapBtn.classList.add('ep');
                this._elWrapContent.appendChild(this._elWrapBtn);
            }
            this._makeDescipt_for_detailContent(description);
            this._makeBtn_for_detailContent(isSeries);
        };
        greenButton.prototype._makeDescipt_for_detailContent = function (description) {
            if (!this._elDescript) {
                this._elDescript = document.createElement('div');
                this._elDescript.classList.add('desp');
                this._elWrapDescript.appendChild(this._elDescript);
            }
            this._elDescript.innerHTML = description;
        };
        greenButton.prototype._makeBtn_for_detailContent = function (isSeries) {
            var isEmptyProg = true;
            isEmptyProg = this._isProgEmpty(this._programmes[this._curDetailIdx]);
            if (isEmptyProg) {
                if (this._elUlBtn) {
                    this._elUlBtn.removeChild(this._elRecBtn);
                    this._elUlBtn.removeChild(this._elRemindBtn);
                    this._elUlBtn = null;
                    this._elRecBtn = null;
                    this._elRemindBtn = null;
                }
            }
            else {
                if (!this._elUlBtn) {
                    this._elUlBtn = document.createElement('ul');
                    this._elWrapBtn.appendChild(this._elUlBtn);
                }
                //set Rec Button
                if (!this._elRecBtn) {
                    this._elRecBtn = document.createElement('li');
                    this._elRecBtn.classList.add('button');
                    this._elUlBtn.appendChild(this._elRecBtn);
                }
                if (isSeries) {
                    this._elRecBtn.innerHTML = "RECORD WHOLE SERIES";
                }
                else {
                    this._elRecBtn.innerHTML = "RECORD THIS PROGRAMME";
                }
                //set Remind Button
                if (!this._elRemindBtn) {
                    this._elRemindBtn = document.createElement('li');
                    this._elRemindBtn.classList.add('button');
                    this._elUlBtn.appendChild(this._elRemindBtn);
                }
                this._elRemindBtn.innerHTML = "SET REMINDER";
            }
        };
        greenButton.prototype._makeKeyManager_for_detailDlg = function () {
            var _this = this;
            this._updownKeyManager = {
                focusableElList: [],
                currentFocusIdx: 0,
                getCurrentFocusEl: function () {
                    return _this._updownKeyManager.focusableElList[_this._updownKeyManager.currentFocusIdx];
                },
                doUp: function () {
                    var curEl = _this._updownKeyManager.getCurrentFocusEl();
                    if (curEl) {
                        curEl.classList.remove('on');
                        _this._updownKeyManager.currentFocusIdx -= 1;
                        if (_this._updownKeyManager.currentFocusIdx < 0) {
                            _this._updownKeyManager.currentFocusIdx = _this._updownKeyManager.focusableElList.length - 1;
                        }
                        curEl = _this._updownKeyManager.getCurrentFocusEl();
                        curEl.classList.add('on');
                    }
                },
                doDown: function () {
                    var curEl = _this._updownKeyManager.getCurrentFocusEl();
                    if (curEl) {
                        curEl.classList.remove('on');
                        _this._updownKeyManager.currentFocusIdx += 1;
                        if (_this._updownKeyManager.currentFocusIdx === _this._updownKeyManager.focusableElList.length) {
                            _this._updownKeyManager.currentFocusIdx = 0;
                        }
                        curEl = _this._updownKeyManager.getCurrentFocusEl();
                        curEl.classList.add('on');
                    }
                }
            };
            this._registFocusableEl_for_keyManager(this._elRecBtn);
            this._registFocusableEl_for_keyManager(this._elRemindBtn);
            if (this._elWrapPaging) {
                if (this._elWrapPaging.innerHTML !== "") {
                    this._registFocusableEl_for_keyManager(this._elWrapPaging);
                }
            }
            var curEl = this._updownKeyManager.getCurrentFocusEl();
            if (curEl) {
                curEl.classList.add('on');
            }
        };
        greenButton.prototype._registFocusableEl_for_keyManager = function (focusableEl) {
            if (focusableEl) {
                this._updownKeyManager.focusableElList.push(focusableEl);
                focusableEl.classList.remove('on');
            }
        };
        greenButton.prototype._findNaviDirection_for_detailDlg = function () {
            var naviDirect = 0 /* NONE */;
            if (this._programmes.length <= 1) {
                naviDirect = 0 /* NONE */;
            }
            else if (this._curDetailIdx > 0) {
                if (this._curDetailIdx < this._programmes.length - 1) {
                    naviDirect = 3 /* ALL */;
                }
                else {
                    naviDirect = 1 /* PREV */;
                }
            }
            else {
                naviDirect = 2 /* NEXT */;
            }
            return naviDirect;
        };
        greenButton.prototype.__send_timer_refresh = function (aParam) {
            var index;
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        greenButton.prototype.__send_event = function (aInput) {
            var aParam = {
                alEvent: null,
                alData: {
                    me: null,
                    callback_fn: null,
                    input_title: null,
                    programme: null
                }
            };
            aParam.alEvent = 'alClicked';
            aParam.alData.me = this;
            aParam.alData.callback_fn = this._callback_fn;
            aParam.alData.input_title = aInput;
            aParam.alData.programme = this._programmes[this._curDetailIdx];
            this.sendEventToParent(aParam);
        };
        return greenButton;
    })(__COMPONENT_KERNEL__);
    exports.greenButton = greenButton;
});
