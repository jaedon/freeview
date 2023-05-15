/// <reference path="../../def/sugar.d.ts" />
// file name : pages/install/page01_welcome.js
/*jslint sloppy:true, nomen:true, vars:true */
/* DESIGN : design_result/02_WIZ/WIZ_1.html */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: ifSugar;
class cPgInstallWelcome extends __PAGE_KERNEL__ {
    private _ulTimerId: number = 0;
    private _mContainerEl: HTMLElement;
    private _mP1El: HTMLElement;
    private _bIsGoWizardPage: boolean;

    constructor() {
        super();
    }

    create() {
        hx.logadd('CPgInstall01');
        // fling do not have to work when Install Wizard
        var szMessage: String = '';
        hx.svc.BRIDGE.enablePlayback(false);
        $.extend(this, {
            _mContainerEl: document.createElement('section'),
            _mP1El: document.createElement('div')
        });
        this._mContainerEl.id = 'wiz';
        szMessage = hx.l(hx.al.CPgInstall01.items.itemStr[0]);
        this._mP1El.innerHTML = '<p>' + hx.l('LOC_WELCOME_ID') + '<em>' + hx.l('LOC_HMS_CHOICE_ID') + '!</em>' + '<em id="small">' + szMessage + '!</em>' + '</p>';
        this._mP1El.classList.add('wel');
        this._mContainerEl.appendChild(this._mP1El);
        document.querySelector('#wrap').appendChild(this._mContainerEl);
        hx.al.compose(this);
        hx.svc.SETTING_UTIL.setFrontPanelString({
            text: hx.al.CPgInstall01.items.fronttext
        });
        this._bIsGoWizardPage = false;
        clearTimeout(this._ulTimerId);
        this._ulTimerId = setTimeout(() => {
            this._NextPage();
        }, 5000);
    }

    destroy() {
        if (this._mContainerEl) {
            if (!this._bIsGoWizardPage) {
                this._mContainerEl.parentElement.removeChild(this._mContainerEl);
            } else {
                this._mContainerEl.removeChild(this._mP1El);
            }
            this._mContainerEl = undefined;
        }
        this._mP1El = undefined;
        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(true);
    }

    _NextPage() {
        this._bIsGoWizardPage = true;
        this.sendEventToEntity({
            alEvent: 'alChangeInstallPage',
            target: hx.al.CPgInstall01.items.nextPage,
            me: this
        });
    }

    _on_alKey(param) {
        var bConsumed: boolean = false;
        switch (param.alKey) {
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                bConsumed = true;
                break;
        }
        return bConsumed;
    }
}
hx.al.CPgInstall01 = cPgInstallWelcome;
/********************      Static Variables     ********************/
if (hx.config.wzd.page01) {
    hx.al.CPgInstall01.items = hx.config.wzd.page01;
} else {
    hx.al.CPgInstall01.items = {
        itemStr: ['LOC_ENJOY_HUMAXWORLD_ID'],
        nextPage: 'CPgInstall02',
        prevPage: ''
    };
}
export = cPgInstallWelcome;
