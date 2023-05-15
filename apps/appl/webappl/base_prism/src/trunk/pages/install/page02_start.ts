/// <reference path="../../def/sugar.d.ts" />
// file name : pages/install/page02_start.js
/*jslint sloppy:true, nomen:true, vars:true */
/* DESIGN : design_result/02_WIZ/WIZ_101.html */
import __PAGE_KERNEL__ = require("pages/page_kernel");
declare var hx: any;
declare var prism: any;


class cPgInstallStart extends __PAGE_KERNEL__ {
    _hiddenKeySeq: number = 0;
    _bIsGoWizardPage: boolean = false;

    containerEl: HTMLElement;
    hiddenEl: HTMLElement;
    serialEl: HTMLElement;
    languageEl: HTMLElement;
    strEl: HTMLElement;
    strEl2: HTMLElement;
    stepEl: HTMLElement;
    srcEl: HTMLElement;
    srcStrEl: HTMLElement;
    btnWrapEl: HTMLElement;
    btn: any;
    $article: HTMLElement;
    $hArea: HTMLElement;
    $title: HTMLElement;

    constructor() {
        super();
    }
    create() {
        var i: number = 0, ullen: number = 0;
        var bBoxFirstBootStatus: boolean = false;

        hx.logadd('CPgInstall02');

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(false);

        $.extend(this, {
            containerEl : document.querySelector('#wiz'),
            hiddenEl : document.createElement('div'),
            serialEl : document.createElement('td'),
            languageEl : document.createElement('td'),

            $article : $('<article class="cont">'),
            strEl : document.createElement('p'),
            strEl2 : document.createElement('p'),
            stepEl : document.createElement('ul'),
            srcEl : document.createElement('li'),
            srcStrEl : document.createElement('p'),
            $hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            $title : $('<span>', {
                'class' : 'tit',
                'html' : hx.l('LOC_INSTALLATION_WIZARD_ID')
            }),
            btnWrapEl : document.createElement('div'),
            btn : this._makebtn()
        });
        this.hiddenEl.classList.add('hidden');
        this.serialEl.classList.add('serial');
        this.languageEl.classList.add('language');
        this.strEl.innerHTML = "Let's get started.<br>We'll now take you through a few easy steps to get your recorder set up.";
        this.strEl2.innerHTML = "Select START NOW and press OK to continue.";
        this.strEl2.classList.add('str2')
        this.stepEl.classList.add('ins');
        this.btnWrapEl.classList.add('btnBo');

        /////////////////append To DOM
        if (!this.containerEl) {
            this.containerEl = document.createElement('section');
            this.containerEl.id = 'wiz';
            document.querySelector('#wrap').appendChild(this.containerEl);
        }

        ullen = hx.al.CPgInstall02.items.itemStr.length;
        if (ullen >= 4) {
            this.btn.$[0].style.width = "370px";
        }
        for ( i = 0; i < ullen; i += 1) {
            var self: any = this.srcEl.cloneNode();
            self.innerHTML = hx.l(hx.al.CPgInstall02.items.itemStr[i]).toUpperCase() + "<br>" +
                hx.l(hx.al.CPgInstall02.items.stepType[i]);
            this.stepEl.appendChild(self);
        }
        this.btnWrapEl.appendChild(this.btn.$[0]);
        this.$hArea[0].appendChild(this.$title[0]);
        this.$article[0].appendChild(this.strEl);
        this.$article[0].appendChild(this.strEl2);
        this.$article[0].appendChild(this.stepEl);
        this.$article[0].appendChild(this.btnWrapEl);
        this.containerEl.style.display = 'none';
        this.containerEl.appendChild(this.hiddenEl);
        this.containerEl.appendChild(this.$hArea[0]);
        this.containerEl.appendChild(this.$article[0]);
        this.containerEl.style.display = 'block';

        /////////////Do Initial Script
        hx.al.compose(this).append(this.btn);
        this.btn.on();
        this.btn.$.addClass('on');

        hx.svc.SETTING_UTIL.setFrontPanelString({
                text : hx.config.wzd.page01.fronttext
        });

        hx.il.ani.action({
            targets : [this.$hArea, this.$article],
            cls : 'aniFadeIn'
        });

        this._bIsGoWizardPage = false;

        bBoxFirstBootStatus = hx.svc.SETTING_UTIL.getBoxFirstBootStatus();
        if (bBoxFirstBootStatus) {
            if ((ENV.cas === ENV.listCAS.IR) || (ENV.cas === ENV.listCAS.IRUC)) {
                this._dispIRFusinInfo();
            } else {
                this._dispFusinInfo();
            }
            hx.svc.SETTING_UTIL.setBoxFirstBootStatus(false);
        }
        hx.il.isGoingOperationFromStandby = false;
    }

    destroy() {
        if (this.containerEl) {
            if (!this._bIsGoWizardPage) {
                this.containerEl.parentElement.removeChild(this.containerEl);
            } else {
                this.containerEl.style.display = 'none';
                this.containerEl.removeChild(this.hiddenEl);
                this.containerEl.removeChild(this.$hArea[0]);
                this.containerEl.removeChild(this.$article[0]);
                this.containerEl.style.display = 'block';
            }
            this.containerEl = undefined;
        }
        this.hiddenEl = undefined;
        this.serialEl = undefined;
        this.languageEl = undefined;
        this.$article = undefined;
        this.strEl = undefined;
        this.strEl2 = undefined;
        this.stepEl = undefined;
        this.srcEl = undefined;
        this.srcStrEl = undefined;
        this.$hArea = undefined;
        this.$title = undefined;
        this.btnWrapEl = undefined;
        this.btn = undefined;

        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(true);
    }
    _on_alClicked(aParam) {
        var bConsumed: boolean = false, key: any = aParam.alKey;
        if (aParam.alData.me === this.btn && key === hx.key.KEY_OK) {
            bConsumed = true;
            this._bIsGoWizardPage = true;
            hx.al.decompose(this);
            this._fadeOutAni(hx.al.CPgInstall02.items.nextPage);
        }
        return bConsumed;
    }
    _on_alKey(aParam) {
        var bConsumed: boolean = false, bEnable: boolean = false;
        switch(aParam.alKey) {
            case hx.key.KEY_1:
            case hx.key.KEY_2:
            case hx.key.KEY_3:
            case hx.key.KEY_4:
            case hx.key.KEY_5:
            case hx.key.KEY_6:
            case hx.key.KEY_7:
            case hx.key.KEY_8:
            case hx.key.KEY_9:
                bEnable = this._checkHiddenMenukey(aParam.alKey);
                if (bEnable === true) {
                    this._showHiddenMenu();
                    bConsumed = true;
                }
                break;
            case hx.key.KEY_MENU:
            case hx.key.KEY_GUIDE:
            case hx.key.KEY_ON_DEMAND:
                //bConsumed = true;
                break;
            case hx.key.KEY_BACK:
                break;
            default:
                this._hiddenKeySeq = 0;
                break;
        }
        return bConsumed;
    }
    _makebtn() {
        return hx.al.component('button', {
            attr : {
                'class' : 'button',
                'data-langID' : 'LOC_START_ID'
            },
            usePointerKey : true
        });
    }
    _checkHiddenMenukey(aKey) {
        switch(aKey) {
            case hx.key.KEY_4:
                if (this._hiddenKeySeq === 0) {
                    this._hiddenKeySeq += 1;
                } else if (this._hiddenKeySeq === 2) {
                    this._hiddenKeySeq = 0;
                    return true;
                }
                break;
            case hx.key.KEY_7:
                if (this._hiddenKeySeq === 1) {
                    this._hiddenKeySeq += 1;
                }
                break;
            default:
                this._hiddenKeySeq = 0;
                break;
        }

        return false;
    }
    _checkFusingStateMenu() {
        var bBoxFirstBootStatus: boolean = false;

        bBoxFirstBootStatus = hx.svc.SETTING_UTIL.getBoxFirstBootStatus();
        if (bBoxFirstBootStatus === true) {
            this._showHiddenMenu();
            hx.svc.SETTING_UTIL.setBoxFirstBootStatus(false);
        }
    }
    _showHiddenMenu() {
        if ((ENV.listCAS.IR === ENV.cas) || (ENV.listCAS.IRUC === ENV.cas)) {
            this._dispIRFusinInfo();
        } else {
            this._dispFusinInfo();
        }
    }
    _dispFusinInfo() {
        var stHiddenInfo: any;
        var ulSerialNumber: number, szLang: string, ulSecureBoot: number, ulJtag: number, ulOTPLock: number, ulSecureCW: number;
        var html: string;
        var buttonDlg: any;

        stHiddenInfo = hx.svc.SETTING_UTIL.getHiddenInfo();
        ulSerialNumber = stHiddenInfo.serialNumber;
        szLang = stHiddenInfo.language;
        ulSecureBoot = stHiddenInfo.scs;
        ulJtag = stHiddenInfo.jtag;
        ulOTPLock = stHiddenInfo.otp;
        ulSecureCW = stHiddenInfo.cwe;

        function convertSN(hex) {
            var str = '', i;
            for ( i = 1; i < hex.length; i += 2) {
                //str += String.fromCharCode(parseInt(hex.substr(i, 2), 16));
                str += hex[i];
            }
            return str;
        }

        html = '';
        html += '<table>';
        html += '<td>SN :&nbsp;</td><td class ="serial">  ' + convertSN(ulSerialNumber) + '</td><tr>';
        html += '</table>';
        html += '<table>';
        html += '<td>LANGUAGE</td><td class ="language"> :' + szLang + '</td><tr>';
        html += '</table>';
        html += '<table>';
        html += '<td>S-BOOT</td><td> : </td><td>' + ((ulSecureBoot === 1) ? 'O' : 'X') + '</td>';
        html += '<td>&nbsp;&nbsp;&nbsp;JTAG</td><td> : ' + ((ulJtag === 1) ? 'O' : 'X') + '  </td><tr>';
        html += '<td>S-CW</td><td> : </td><td>' + ((ulSecureCW === 1) ? 'O' : 'X') + '</td>';
        html += '<td>&nbsp;&nbsp;&nbsp;OTP</td><td> : ' + ((ulOTPLock === 1) ? 'O' : 'X') + '  </td><tr>';
        html += '</table>';

        buttonDlg = prism.createInfoDlg(null, {
            textHtml : html
        });
        buttonDlg.open();
    }
    _dispIRFusinInfo() {
        var szFusingInfo: any;
        var html: string = '';
        var buttonDlg: any;

        szFusingInfo = hx.svc.SETTING_UTIL.getIRFusinInfo();

        function convertSN(hex) {
            var str = '', i;
            for ( i = 1; i < hex.length; i += 2) {
                //str += String.fromCharCode(parseInt(hex.substr(i, 2), 16));
                str += hex[i];
            }
            return str;
        }

        var szLanguage = szFusingInfo.language;
        html += '<table>';
        html += '<td>SN :&nbsp;</td><td class ="serial">  ' + convertSN(szFusingInfo.serialNumber) + '</td><tr>';
        html += '</table>';
        html += '<table>';
        html += '<td>LANGUAGE</td><td class ="language"> :' + szFusingInfo.language + '</td><tr>';
        html += '</table>';
        html += '<table>';
        html += '<td>S-BOOT</td><td> : </td><td>' + (szFusingInfo.sboot) + '</td>';
        html += '<td>&nbsp;&nbsp;&nbsp;JTAG</td><td> : ' + (szFusingInfo.jtag) + '  </td><tr>';
        html += '<td>S-CW</td><td> : </td><td>' + (szFusingInfo.scw) + '</td>';
        html += '<td>&nbsp;&nbsp;&nbsp;OTP</td><td> : ' + (szFusingInfo.otp) + '  </td><tr>';
        html += '<td>IR-DEV</td><td> : </td><td>' + (szFusingInfo.ir_mancode) + ',' + (szFusingInfo.ir_hwcode) + ',' + (szFusingInfo.ir_variant) + '</td><tr>';
        html += '<td>IR-KEY</td><td> : </td><td>' + (szFusingInfo.ir_sysid) + ',' + (szFusingInfo.ir_keyver) + ',' + (szFusingInfo.ir_sigver) + '</td><tr>';
        html += '</table>';

        buttonDlg = prism.createInfoDlg(null, {
            textHtml : html
        });
        buttonDlg.open({
            dimm : true
        });
    }
    _fadeOutAni(aTargetPage) {
        hx.il.ani.action({
            targets : this.$article,
            cls : 'aniFadeOut'
        });

        hx.il.ani.action({
            targets : this.$title,
            cls : 'aniFadeOut',
            cb : () =>  {
                this.sendEventToEntity({
                    'alEvent' : 'alChangeInstallPage',
                    'target' : aTargetPage,
                    'me' : this
                });
            }
        });
    }
}
hx.al.CPgInstall02 = cPgInstallStart;

/********************      Static Variables     ********************/
if (hx.config.wzd.page02) {
    hx.al.CPgInstall02.items = hx.config.wzd.page02;
} else {
    hx.al.CPgInstall02.items = {
        itemStr : ['LOC_BIGLANGUAGE_ID', 'LOC_NETWORK_ID', 'LOC_BIGCHANNEL_ID'],
        stepType : ['LOC_SELECTION_ID', 'LOC_CONNECTION_ID', 'LOC_SEARCH_ID'],
        nextPage : 'CPgInstall03',
        prevPage : ''
    };
}

export = cPgInstallStart;