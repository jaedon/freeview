var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cPgInstallWelcome = (function (_super) {
        __extends(cPgInstallWelcome, _super);
        function cPgInstallWelcome() {
            _super.call(this);
            this._ulTimerId = 0;
        }
        cPgInstallWelcome.prototype.create = function () {
            var _this = this;
            hx.logadd('CPgInstall01');
            // fling do not have to work when Install Wizard
            var szMessage = '';
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
            this._ulTimerId = setTimeout(function () {
                _this._NextPage();
            }, 5000);
        };
        cPgInstallWelcome.prototype.destroy = function () {
            if (this._mContainerEl) {
                if (!this._bIsGoWizardPage) {
                    this._mContainerEl.parentElement.removeChild(this._mContainerEl);
                }
                else {
                    this._mContainerEl.removeChild(this._mP1El);
                }
                this._mContainerEl = undefined;
            }
            this._mP1El = undefined;
            // fling do not have to work when Install Wizard
            hx.svc.BRIDGE.enablePlayback(true);
        };
        cPgInstallWelcome.prototype._NextPage = function () {
            this._bIsGoWizardPage = true;
            this.sendEventToEntity({
                alEvent: 'alChangeInstallPage',
                target: hx.al.CPgInstall01.items.nextPage,
                me: this
            });
        };
        cPgInstallWelcome.prototype._on_alKey = function (param) {
            var bConsumed = false;
            switch (param.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
            }
            return bConsumed;
        };
        return cPgInstallWelcome;
    })(__PAGE_KERNEL__);
    hx.al.CPgInstall01 = cPgInstallWelcome;
    /********************      Static Variables     ********************/
    if (hx.config.wzd.page01) {
        hx.al.CPgInstall01.items = hx.config.wzd.page01;
    }
    else {
        hx.al.CPgInstall01.items = {
            itemStr: ['LOC_ENJOY_HUMAXWORLD_ID'],
            nextPage: 'CPgInstall02',
            prevPage: ''
        };
    }
    return cPgInstallWelcome;
});
