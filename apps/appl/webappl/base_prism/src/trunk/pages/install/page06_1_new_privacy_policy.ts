/// <reference path="../../def/sugar.d.ts" />
// file name : pages/install/page06_1_privacy_policy.js
/*jslint sloppy:true, nomen:true, vars:true */
/* DESIGN : design_result/02_WIZ/WIZ_302.html (success) */
/* DESIGN : design_result/02_WIZ/WIZ_303.html (fail) */

import __PAGE_KERNEL__ = require("pages/page_kernel");
import __PAGE_PRIVACY_POLICY__ = require("prismux/prism.page.privacypolicy");
declare var hx: any;
class cCPgInstall06_1 extends __PAGE_KERNEL__ {
    _page: __PAGE_PRIVACY_POLICY__;
    _containerEl : HTMLElement;
    _promoImgEl : HTMLElement;
    _stepEl : HTMLElement;
    _graphicInfoEl : HTMLElement;
    _show : any;
    _isGoWizardPage : boolean;
    _timeset : any;
    _$article : any;
    _$hArea : any;
    _$title : any;

    constructor() {
        super();
    }

    create (aParam, aQuery) {
        hx.logadd('CPgInstall06_1');
        var i : number, _result : string

        $.extend(this, {
            _containerEl : document.querySelector('#wiz'),
            _$article : $('<article>', {
                'class' : 'cont'
            }),
            _show : aQuery.data.show, //success, fail

            _$hArea : $('<h1>', {
                'class' : 'hArea'
            }),
            _$title : $('<span>', {
                'class' : 'tit',
                'html' : hx.l('LOC_PRIVACY_POLICY_TERMS_AND_CONDITIONS_ID')
            }),
            _stepEl : document.createElement('span')
        });
        var fnExit = (aPP, aTC) => {
            hx.svc.TV_PORTAL.acceptPrivacyTerms({
                privacy : aPP,
                terms : aTC,
                fn : () => {},
                fnError : () => {}
            });
            this._go_next_page();
        };
        var btnInfo = {
            focusedIndex: 1,
            btnName : [hx.l('LOC_PREVIOUS_ID'), hx.l('LOC_NEXT_ID')],
            action : [() => {
                this._go_pre_page();
                return true;
            }, () => {
                var checked = this._page.getCheckedValues();
                if (!checked[0]) {
                    hx.msg('confirm', {
                        text : "Some of the apps or internet based features on your Humax product may not be accessible or working correctly unless you accept the new Privacy Policy.",
                        auto_close : false,
                        dia_class : 'osd',
                        btn_title_arr : ["OK"],
                        callback_fn : function(param) {
                            if (param === "OK") {
                                fnExit(checked[0], checked[1]);
                            }
                        }
                    });
                    return;
                }
                fnExit(checked[0], checked[1]);
                return true;
            }]
        };
        var page = new __PAGE_PRIVACY_POLICY__(this._$article, btnInfo);
        page.setDataRequestor(this.requestorPortalApi);
        page.setOperationRequester(this.requestorOperation);
        this._page = page;
        //////////////////set Element attribute
        if (aQuery.query === 'prev') {
            this._$article.css('left', '-50px');
        } else {
            this._$article.css('left', '50px');
        }
        this._stepEl.classList.add(hx.al.CPgInstall06_1.items.step?hx.al.CPgInstall06_1.items.step:'step2');
        /////////////////append To DOM
        if (!this._containerEl) {
            this._containerEl = document.createElement('section');
            this._containerEl.id = 'wiz';
            document.querySelector('#wrap').appendChild(this._containerEl);
        }
        this._$hArea[0].appendChild(this._$title[0]);
        this._$hArea[0].appendChild(this._stepEl);
        this._containerEl.style.display = 'none';
        this._containerEl.appendChild(this._$hArea[0]);
        this._containerEl.appendChild(this._$article[0]);
        this._containerEl.style.display = 'block';

        /////////////Do Initial Script
        hx.al.compose(this);

        this._isGoWizardPage = false;
        hx.il.ani.action({
            targets : this._$hArea,
            cls : 'aniFadeIn'
        });
        hx.il.ani.action({
            targets : [this._$article],
            cls : 'aniFadeIn',
            style : 'left:0'
        });

    }
    destroy () : any {
        clearInterval(this._timeset);
        if (this._containerEl) {
            if (!this._isGoWizardPage) {
                this._containerEl.parentElement.removeChild(this._containerEl);
            } else {
                this._containerEl.style.display = 'none';
                this._containerEl.removeChild(this._$hArea[0]);
                this._containerEl.removeChild(this._$article[0]);
                this._containerEl.style.display = 'block';
            }
            this._containerEl = undefined;
        }
        this._$article = undefined;
        this._promoImgEl = undefined;
        this._show = undefined;
        this._$hArea = undefined;
        this._$title = undefined;
        this._stepEl = undefined;
        this._graphicInfoEl = undefined;
        this._page.destroy();
        // fling do not have to work when Install Wizard
        hx.svc.BRIDGE.enablePlayback(true);
    }
    _on_alKey (aParam) : boolean {
        var bConsumed : boolean = false;
        var keyCode = aParam.alKey;
        bConsumed = this._page.doKey(keyCode);
        if(!bConsumed) {
            switch( aParam.alKey ) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_BACK:
                    this._go_pre_page();
                    bConsumed = true;
                    break;
            }
        }
        return bConsumed;
    }
    _go_pre_page () : void {
        this._isGoWizardPage = true;
        hx.al.decompose(this);
        if (this._show === 'success')
            this._fadeOutAni(hx.al.CPgInstall06_1.items.prevPage + '?prev,show=success', true);
        else
            this._fadeOutAni(hx.al.CPgInstall06_1.items.prevPage + '?prev,show=fail', true);
    }
    _go_next_page () : void {
        this._isGoWizardPage = true;
        hx.al.decompose(this);
        this._fadeOutAni(hx.al.CPgInstall06_1.items.nextPage, false);
    }
    _fadeOutAni (aTargetPage, aIsGoPrev : boolean) : void {
        var left : string;

        if (aIsGoPrev) {
            left = '20px';
        } else {
            left = '-20px';
        }

        hx.il.ani.action({
            targets : this._$article,
            cls : 'aniFadeOut',
            style : 'left:' + left
        });

        hx.il.ani.action({
            targets : this._$title,
            cls : 'aniFadeOut',
            cb : ()=> {
                this.sendEventToEntity({
                    alEvent : 'alChangeInstallPage',
                    target : aTargetPage,
                    me : this
                });
            }
        });
    }
    private requestorOperation = (() => {
        return {
            exit: (target?: string) => {
                this.sendEventToEntity({
                    alEvent: 'alChangePage',
                    target: target || 'CPgLiveController',
                    me: this
                });
            },
            getNetworkAvailable: function(aFnSuccess, aFnFail) {
                if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                    aFnSuccess();
                } else {
                    aFnFail();
                }
            },
            goNextPage: () => {
                this._go_next_page();
            }
        }
    })();

    private requestorPortalApi : any = {
        isLogged: () => {
            return hx.svc.TV_PORTAL.isLogged();
        },
        login: (aCallbackSuccess, aCallbackFail) => {
            hx.svc.TV_PORTAL.login({
                fn: (aData) => {
                    aCallbackSuccess(aData);
                },
                fnError: (aData) => {
                    aCallbackFail(aData);
                }
            });
        },
        getActiveProfile: (aCallbackSuccess, aCallbackFail) => {
            hx.svc.TV_PORTAL.getActiveProfile({
                fn: (aData) => {
                    aCallbackSuccess(aData);
                },
                fnError: (aData) => {
                    aCallbackFail(aData);
                }
            });
        },
        getPrivacyPolicy: (aCBSuccess: (aHTML: string, aAccepted: boolean) => void, aCBFail: (aHTML: string) => void) => {
            hx.svc.TV_PORTAL.getPrivacyPolicyText((aText) => {
                if (aCBSuccess) {
                    aCBSuccess(aText, hx.svc.TV_PORTAL.isPrivacyPolicyAccepted());
                }
            }, aCBFail);
        },
        getTermsAndConditions: (aCBSuccess: (aHTML: string, aAccepted: boolean) => void, aCBFail: (aHTML: string) => void) => {
            hx.svc.TV_PORTAL.getTermsAndConditionsText((aText) => {
                if (aCBSuccess) {
                    aCBSuccess(aText, hx.svc.TV_PORTAL.isTermsAndConditionsAccepted());
                }
            }, aCBFail);
        }
    }
}
hx.al.CPgInstall06_1 = cCPgInstall06_1;

/********************      Static Variables     ********************/
if (hx.config.wzd.page06) {
    hx.al.CPgInstall06_1.items = hx.config.wzd.page06_1;
} else {
    hx.al.CPgInstall06_1.items = {
        itemStr : [''],
        nextPage : 'CPgInstall06_2',
        prevPage : 'CPgInstall06',
        step : 'step2',
    };
}
export = cCPgInstall06_1;
