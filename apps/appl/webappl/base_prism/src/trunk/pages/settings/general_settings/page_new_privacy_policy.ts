//file name : pages/settings/general_settings/privacy_policy.js
/*jslint sloppy:true, nomen:true, vars:true */
///<reference path="../../../def/sugar.d.ts"/>
import __KERNEL_SETTINGS__ = require("pages/settings/kernel_settings");
import __PAGE_PRIVACY_POLICY__ = require("prismux/prism.page.privacypolicy");
declare var hx: ifSugar;
class cPgPrivacyPolicy extends __KERNEL_SETTINGS__ {
    _page: __PAGE_PRIVACY_POLICY__;
    _$article : any;
    aniConfig = {
        fadeIn : {
            start : {
                'opacity' : 0
            },
            end : {
                'opacity' : 1
            }
        },
        article : {
            changeOut : {
                start : {
                    'opacity' : 1,
                    'left' : 0
                },
                end : {
                    'opacity' : 0,
                    'left' : '20px'
                }
            }
        }
    };
    _isChangePageInSetting: boolean = false;
    constructor() {
        super();
    }
    create() {
        super.create();
        $.extend(this, {
            _$article : $('<article>', {
                'class' : 'cont',
            })
        });
        this._$article.css('text-align', 'center');
        var $title = $('#set .title');
        if ($title[0]) {
            (<HTMLElement>$title[0].children[0]).innerHTML = hx.l('LOC_PRIVACY_POLICY_TERMS_AND_CONDITIONS_ID'); // TODO check
        }
        var fnExit = (aPP, aTC) => {
            hx.svc.TV_PORTAL.acceptPrivacyTerms({
                privacy : aPP,
                terms : aTC,
                fn : () => {},
                fnError : () => {}
            });
            this._gotoSettingPage();
        };
        var btnInfo = {
            focusedIndex: 0,
            btnName : [hx.l('LOC_DONE_ID')],
            action : [() => {
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

        $('#set').append(this._$article);

        hx.al.compose(this);

        $title.ani({
            style : this.aniConfig.fadeIn,
            trans : {
                dur : 100,
                delay : 300
            }
        });
    }
    destroy() {
        if (this._page) {
            this._page.destroy();
            this._page = null;
        }
        if (this._$article) {
            this._$article.remove();
            this._$article = null;
        }
        super.destroy();
        if (!this._isChangePageInSetting) {
            $('#set').remove();
        } else {
            this._isChangePageInSetting = false;
        }
    }
    _on_alKey(aParam): boolean {
        var bConsumed: boolean = false;
        var keyCode = aParam.alKey;
        bConsumed = this._page.doKey(keyCode);
        if (!bConsumed) {
            switch (aParam.alKey) {
                case hx.key.KEY_MENU:
                case hx.key.KEY_GUIDE:
                case hx.key.KEY_ON_DEMAND:
                    bConsumed = true;
                    break;
                case hx.key.KEY_ESCAPE:
                    bConsumed = true;
                    hx.svc.SETTING_UTIL.setFrontPanelString({
                        text : ''
                    });
                    this.sendEventToEntity({
                        alEvent : 'alChangePage',
                        target : 'CPgLiveController',
                        me : this
                    });
                    break;
                case hx.key.KEY_BACK:
                    this._gotoSettingPage();
                    bConsumed = true;
                    break;
            }
        }
        return bConsumed;
    }
    _gotoSettingPage() {
        this._isChangePageInSetting = true;
        hx.al.decompose(this);
        this._$article.ani({
            style : this.aniConfig.article.changeOut,
            trans : {
                dur : 200,
                delay : 100
            },
            cb : () => {
                this.sendEventToEntity({
                    alEvent : 'alChangePage',
                    target : 'CPgSettings',
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
                this._gotoSettingPage();
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
hx.al.CPgPrivacyPolicy = cPgPrivacyPolicy;
export = cPgPrivacyPolicy;
