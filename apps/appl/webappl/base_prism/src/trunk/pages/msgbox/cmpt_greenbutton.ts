///<reference path="../../def/sugar.d.ts" />
// file name : pages/msgbox/cmpt_greenbutton.js
/* jslint sloppy:true, nomen:true, vars:true */

declare var hx: ifSugar;
declare var prism;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");
import __MSG_DEF__ = require("../../def/message.def");

export enum direction {
    NONE,
    PREV,
    NEXT,
    ALL
}
export class greenButton extends __COMPONENT_KERNEL__ {
    public $: JQuery = null;

    private _$greenBtn: JQuery = null;
    private _$detailDlg: JQuery = null;
    private _elMinipop: HTMLElement = null;
    private _elMiniMessage: HTMLElement = null;
    private _elText: HTMLElement = null;
    private _elWrapTitle: HTMLElement = null;
    private _elTitle: HTMLElement = null;
    private _elIcon: HTMLElement = null;
    private _elWrapPaging: HTMLElement = null;
    private _elWrapContent: HTMLElement = null;
    private _elWrapDescript: HTMLElement = null;
    private _elDescript: HTMLElement = null;
    private _elWrapBtn: HTMLElement = null;
    private _elUlBtn: HTMLElement = null;
    private _elRecBtn: HTMLElement = null;
    private _elRemindBtn: HTMLElement = null;
    private _updownKeyManager: any = null;
    private _callback_fn: any = null;
    private _programmes: any = null;
    private _curDetailIdx: number = 0;
    private _noneIconClass: string = 'icon';
    private _recIconClass: string = 'icon rec';
    private _seriesRecIconClass: string = 'icon rec_series';
    private _remindIconClass: string = 'icon remind';

    constructor () {
        super();
    }
    create (aParam: any): any {
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
    }

    resetTimeout (): void {
        this.__send_timer_refresh();
    }

    destroy (): void {
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
    }

    show (): void {
        this.$.show();
    }

    hide (): void {
        this.$.hide();
    }

    // EVENT HANDLER
    _on_alKey (aParam: any): boolean {
        var bConsumed: boolean = false;
        var kc: number = aParam.alKey;
        var curFocusEl: HTMLElement = null;

        if (this._$greenBtn[0].style.display === "none") { //detailDlg
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
                                this.__send_event(__MSG_DEF__.eGreenBtnCommand.SERIES_RECORD);
                                this._elIcon.className = this._seriesRecIconClass;
                            } else {
                                this.__send_event(__MSG_DEF__.eGreenBtnCommand.RECORD);
                                this._elIcon.className = this._recIconClass;
                            }
                        } else if (curFocusEl === this._elRemindBtn) {
                            this.__send_event(__MSG_DEF__.eGreenBtnCommand.REMINDER);
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
        } else {  //greenBtn
            switch (kc) {
                case hx.key.KEY_GREEN:
                    this._showDetailDlg();
                    bConsumed = true;
                    break;
            }
        }
        this.__send_timer_refresh(aParam);
        return bConsumed;
    }

    // PRIVATE FUNCTION FOR CONTENT MAKING
    private _isSeries (programme: any): boolean {
        var isSeries: boolean = false;
        if (programme) {
            if (programme.seriesId) {
                if (programme.seriesId !== "") {
                    isSeries = true;
                }
            }
        }
        return isSeries;
    }

    private _isProgEmpty (programme: any): boolean {
        var isEmpty: boolean = true;
        if (programme) {
            if (programme.programmeID) {
                isEmpty = false;
            }
        }
        return isEmpty;
    }

    private _showGreenBtn (): void {
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
    }

    private _showDetailDlg (): void {
        var title: string = this._programmes[this._curDetailIdx].name;
        var description: string = this._programmes[this._curDetailIdx].longDescription;
        var isSeries: boolean = this._isSeries(this._programmes[this._curDetailIdx]);
        var naviDirect: direction = this._findNaviDirection_for_detailDlg();

        this._$greenBtn.hide();
        this._$detailDlg.hide();
        this._makeTitle_for_detailDlg(title);
        this._makePagingArrow_for_detailDlg(naviDirect);
        this._makeContent_for_detailDlg(description, isSeries);
        this._makeKeyManager_for_detailDlg();
        this._$detailDlg.show();
    }

    private _updateDetailInfo (): void {
        var title: string = this._programmes[this._curDetailIdx].name;
        var description: string = this._programmes[this._curDetailIdx].longDescription;
        var isSeries: boolean = this._isSeries(this._programmes[this._curDetailIdx]);
        var naviDirect: direction = this._findNaviDirection_for_detailDlg();

        this._$detailDlg.hide();
        this._makeTitle_for_detailDlg(title);
        this._makePagingArrow_for_detailDlg(naviDirect);
        this._makeContent_for_detailDlg(description, isSeries);
        this._$detailDlg.show();
    }

    private _makeTitle_for_detailDlg (title: string): void {
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
    }

    private _makePagingArrow_for_detailDlg (pagingDirection: direction): void {
        if (!this._elWrapPaging) {
            this._elWrapPaging = document.createElement('hgroup');
            this._elWrapPaging.classList.add('paging_tra');
            this._$detailDlg[0].appendChild(this._elWrapPaging);
        }
        if (pagingDirection === direction.PREV) {
            this._elWrapPaging.innerHTML = "<span class='pagingPrv'></span><span class=''></span>";
        } else if (pagingDirection === direction.NEXT) {
            this._elWrapPaging.innerHTML = "<span class=''></span><span class='pagingNxt'></span>";
        } else if (pagingDirection === direction.ALL) {
            this._elWrapPaging.innerHTML = "<span class='pagingPrv'></span><span class='pagingNxt'></span>";
        } else {
            this._elWrapPaging.innerHTML = "";
        }
    }

    private _makeContent_for_detailDlg (description: string, isSeries: boolean): void {
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
    }

    private _makeDescipt_for_detailContent (description: string): void {
        if (!this._elDescript) {
            this._elDescript = document.createElement('div');
            this._elDescript.classList.add('desp');
            this._elWrapDescript.appendChild(this._elDescript);
        }
        this._elDescript.innerHTML = description;
    }

    private _makeBtn_for_detailContent (isSeries: boolean): void {
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
        } else {
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
            } else {
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
    }

    private _makeKeyManager_for_detailDlg (): void {
        this._updownKeyManager = {
            focusableElList: [],
            currentFocusIdx: 0,
            getCurrentFocusEl: (): HTMLElement => {
                return this._updownKeyManager.focusableElList[this._updownKeyManager.currentFocusIdx];
            },
            doUp: (): void => {
                var curEl: HTMLElement = this._updownKeyManager.getCurrentFocusEl();
                if (curEl) {
                    curEl.classList.remove('on');
                    this._updownKeyManager.currentFocusIdx -= 1;
                    if (this._updownKeyManager.currentFocusIdx < 0) {
                        this._updownKeyManager.currentFocusIdx = this._updownKeyManager.focusableElList.length - 1;
                    }
                    curEl = this._updownKeyManager.getCurrentFocusEl();
                    curEl.classList.add('on');
                }
            },
            doDown: (): void => {
                var curEl = this._updownKeyManager.getCurrentFocusEl();
                if (curEl) {
                    curEl.classList.remove('on');
                    this._updownKeyManager.currentFocusIdx += 1;
                    if (this._updownKeyManager.currentFocusIdx === this._updownKeyManager.focusableElList.length) {
                        this._updownKeyManager.currentFocusIdx = 0;
                    }
                    curEl = this._updownKeyManager.getCurrentFocusEl();
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
        var curEl: HTMLElement = this._updownKeyManager.getCurrentFocusEl();
        if (curEl) {
            curEl.classList.add('on');
        }
    }

    private _registFocusableEl_for_keyManager (focusableEl: HTMLElement): void {
        if (focusableEl) {
            this._updownKeyManager.focusableElList.push(focusableEl);
            focusableEl.classList.remove('on');
        }
    }

    private _findNaviDirection_for_detailDlg (): direction {
        var naviDirect = direction.NONE;
        if (this._programmes.length <= 1) {
            naviDirect = direction.NONE;
        } else if (this._curDetailIdx > 0) {
            if (this._curDetailIdx < this._programmes.length - 1) {
                naviDirect = direction.ALL;
            } else {
                naviDirect = direction.PREV;
            }
        } else {
            naviDirect = direction.NEXT;
        }
        return naviDirect;
    }

    private __send_timer_refresh (aParam?: any): void {
        var index: number;
        aParam = (aParam) || {};
        aParam.alEvent = 'alMessageToPage';
        if (aParam.alData === undefined) {
            aParam.alData = {};
        }
        aParam.alData.me = this;
        this.sendEventToParent(aParam);
    }

    private __send_event (aInput: __MSG_DEF__.eGreenBtnCommand): void {
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
    }
}
