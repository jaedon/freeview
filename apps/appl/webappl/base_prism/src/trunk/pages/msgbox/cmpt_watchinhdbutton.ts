///<reference path="../../def/sugar.d.ts" />
// file name : pages/msgbox/cmpt_watchinHDButton.js
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
export class watchinHDButton extends __COMPONENT_KERNEL__ {
    public $: JQuery = null;

    private _$watchinHDBtn: JQuery = null;

    private _elMinipop: HTMLElement = null;
    private _elMiniMessage: HTMLElement = null;
    private _elText: HTMLElement = null;

    private _updownKeyManager: any = null;
    private _callback_fn: any = null;
    //private _programmes: any = null;
    private _linkage: any = null;

    constructor () {
        super();
    }
    create (aParam: any): any {
        this.$ = $('<section>');
        this._$watchinHDBtn = $('<section>');
        this._$watchinHDBtn[0].id = 'trailer';
        this._$watchinHDBtn.hide();

        this.$[0].appendChild(this._$watchinHDBtn[0]);

        //this._programmes = aParam.programmes;
        this._callback_fn = aParam.callback_fn;
        this._linkage = aParam.linkage;
        this._showWatchinHDBtn();
        return this;
    }

    resetTimeout (): void {
        this.__send_timer_refresh();
    }

    destroy (): void {
        this.$.remove();
        this.$ = null;
        this._callback_fn = null;
        this._$watchinHDBtn = null;

        this._elMinipop = null;
        this._elMiniMessage = null;
        this._elText = null;

        this._updownKeyManager = null;
        //this._programmes = null;
        this._linkage = null;
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

        if (this._$watchinHDBtn[0].style.display === "none") { //detailDlg

        } else {  //greenBtn
            switch (kc) {
                case hx.key.KEY_OK:
                    this.__send_event(__MSG_DEF__.eWatchinHDBtnCommand.GOTO_THE_HD_CHANNEL);
                    bConsumed = true;
                    break;
            }
        }
        this.__send_timer_refresh(aParam);
        return bConsumed;
    }

    private _showWatchinHDBtn (): void {
        this._$watchinHDBtn.hide();

        if (!this._elMinipop) {
            this._elMinipop = document.createElement('section');
            this._elMinipop.id = 'minipop';
            this._elMinipop.className = 'dimTrailer';
            this._$watchinHDBtn[0].appendChild(this._elMinipop);
        }
        if (!this._elMiniMessage) {
            this._elMiniMessage = document.createElement('section');
            this._elMiniMessage.className = 'minimessage';
            this._elMinipop.appendChild(this._elMiniMessage);
        }
        if (!this._elText) {
            this._elText = document.createElement('p');
            this._elText.className = 'txt';
            this._elText.innerHTML = "<em class='OK'></em>" + hx.l('LOC_WATCH_IN_HD_ID');
            this._elMiniMessage.appendChild(this._elText);
        }
        this._$watchinHDBtn.show();
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

    private __send_event (aInput: __MSG_DEF__.eWatchinHDBtnCommand): void {
        var aParam = {
            alEvent: null,
            alData: {
                me: null,
                callback_fn: null,
                input_title: null,
                linkage: null
            }
        };
        aParam.alEvent = 'alClicked';
        aParam.alData.me = this;
        aParam.alData.callback_fn = this._callback_fn;
        aParam.alData.input_title = aInput;
        aParam.alData.linkage = this._linkage;
        this.sendEventToParent(aParam);
    }
}
