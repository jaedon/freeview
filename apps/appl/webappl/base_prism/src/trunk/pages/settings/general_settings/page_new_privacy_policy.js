var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/settings/kernel_settings", "prismux/prism.page.privacypolicy"], function (require, exports, __KERNEL_SETTINGS__, __PAGE_PRIVACY_POLICY__) {
    var cPgPrivacyPolicy = (function (_super) {
        __extends(cPgPrivacyPolicy, _super);
        function cPgPrivacyPolicy() {
            var _this = this;
            _super.call(this);
            this.aniConfig = {
                fadeIn: {
                    start: {
                        'opacity': 0
                    },
                    end: {
                        'opacity': 1
                    }
                },
                article: {
                    changeOut: {
                        start: {
                            'opacity': 1,
                            'left': 0
                        },
                        end: {
                            'opacity': 0,
                            'left': '20px'
                        }
                    }
                }
            };
            this._isChangePageInSetting = false;
            this.requestorOperation = (function () {
                return {
                    exit: function (target) {
                        _this.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: target || 'CPgLiveController',
                            me: _this
                        });
                    },
                    getNetworkAvailable: function (aFnSuccess, aFnFail) {
                        if (hx.svc.SETTING_UTIL.getInternetAvailable()) {
                            aFnSuccess();
                        }
                        else {
                            aFnFail();
                        }
                    },
                    goNextPage: function () {
                        _this._gotoSettingPage();
                    }
                };
            })();
            this.requestorPortalApi = {
                isLogged: function () {
                    return hx.svc.TV_PORTAL.isLogged();
                },
                login: function (aCallbackSuccess, aCallbackFail) {
                    hx.svc.TV_PORTAL.login({
                        fn: function (aData) {
                            aCallbackSuccess(aData);
                        },
                        fnError: function (aData) {
                            aCallbackFail(aData);
                        }
                    });
                },
                getActiveProfile: function (aCallbackSuccess, aCallbackFail) {
                    hx.svc.TV_PORTAL.getActiveProfile({
                        fn: function (aData) {
                            aCallbackSuccess(aData);
                        },
                        fnError: function (aData) {
                            aCallbackFail(aData);
                        }
                    });
                },
                getPrivacyPolicy: function (aCBSuccess, aCBFail) {
                    hx.svc.TV_PORTAL.getPrivacyPolicyText(function (aText) {
                        if (aCBSuccess) {
                            aCBSuccess(aText, hx.svc.TV_PORTAL.isPrivacyPolicyAccepted());
                        }
                    }, aCBFail);
                },
                getTermsAndConditions: function (aCBSuccess, aCBFail) {
                    hx.svc.TV_PORTAL.getTermsAndConditionsText(function (aText) {
                        if (aCBSuccess) {
                            aCBSuccess(aText, hx.svc.TV_PORTAL.isTermsAndConditionsAccepted());
                        }
                    }, aCBFail);
                }
            };
        }
        cPgPrivacyPolicy.prototype.create = function () {
            var _this = this;
            _super.prototype.create.call(this);
            $.extend(this, {
                _$article: $('<article>', {
                    'class': 'cont',
                })
            });
            this._$article.css('text-align', 'center');
            var $title = $('#set .title');
            if ($title[0]) {
                $title[0].children[0].innerHTML = hx.l('LOC_PRIVACY_POLICY_TERMS_AND_CONDITIONS_ID'); // TODO check
            }
            var fnExit = function (aPP, aTC) {
                hx.svc.TV_PORTAL.acceptPrivacyTerms({
                    privacy: aPP,
                    terms: aTC,
                    fn: function () {
                    },
                    fnError: function () {
                    }
                });
                _this._gotoSettingPage();
            };
            var btnInfo = {
                focusedIndex: 0,
                btnName: [hx.l('LOC_DONE_ID')],
                action: [function () {
                    var checked = _this._page.getCheckedValues();
                    if (!checked[0]) {
                        hx.msg('confirm', {
                            text: "Some of the apps or internet based features on your Humax product may not be accessible or working correctly unless you accept the new Privacy Policy.",
                            auto_close: false,
                            dia_class: 'osd',
                            btn_title_arr: ["OK"],
                            callback_fn: function (param) {
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
                style: this.aniConfig.fadeIn,
                trans: {
                    dur: 100,
                    delay: 300
                }
            });
        };
        cPgPrivacyPolicy.prototype.destroy = function () {
            if (this._page) {
                this._page.destroy();
                this._page = null;
            }
            if (this._$article) {
                this._$article.remove();
                this._$article = null;
            }
            _super.prototype.destroy.call(this);
            if (!this._isChangePageInSetting) {
                $('#set').remove();
            }
            else {
                this._isChangePageInSetting = false;
            }
        };
        cPgPrivacyPolicy.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
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
                            text: ''
                        });
                        this.sendEventToEntity({
                            alEvent: 'alChangePage',
                            target: 'CPgLiveController',
                            me: this
                        });
                        break;
                    case hx.key.KEY_BACK:
                        this._gotoSettingPage();
                        bConsumed = true;
                        break;
                }
            }
            return bConsumed;
        };
        cPgPrivacyPolicy.prototype._gotoSettingPage = function () {
            var _this = this;
            this._isChangePageInSetting = true;
            hx.al.decompose(this);
            this._$article.ani({
                style: this.aniConfig.article.changeOut,
                trans: {
                    dur: 200,
                    delay: 100
                },
                cb: function () {
                    _this.sendEventToEntity({
                        alEvent: 'alChangePage',
                        target: 'CPgSettings',
                        me: _this
                    });
                }
            });
        };
        return cPgPrivacyPolicy;
    })(__KERNEL_SETTINGS__);
    hx.al.CPgPrivacyPolicy = cPgPrivacyPolicy;
    return cPgPrivacyPolicy;
});
