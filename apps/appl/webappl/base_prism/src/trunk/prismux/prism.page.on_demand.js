/**
 * Created by bglee on 2014-10-29.
 */
///<reference path="../def/sugar.d.ts"/>
///<reference path="../def/prism.d.ts"/>
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/controls/ctrlbase', "prismux/layout/prism.layout.button.promotion", "prismux/by_controls/prism.slider", "prismux/by_controls/prism.page", "prismux/by_controls/prism.scroll", "layer_domain/service/dl_dsrvNetflix"], function (require, exports, __CTRL_BASE__, __BUTTON_PROMOTION__, __SLIDER__, __PAGE__, __SCROLL_CONTROL__, __NETFLIX__) {
    var eModeListControl;
    (function (eModeListControl) {
        eModeListControl[eModeListControl["NORMAL"] = 0] = "NORMAL";
        eModeListControl[eModeListControl["MOVE"] = 1] = "MOVE";
        eModeListControl[eModeListControl["DELETE"] = 2] = "DELETE";
    })(eModeListControl || (eModeListControl = {}));
    var CPageOnDemand = (function (_super) {
        __extends(CPageOnDemand, _super);
        function CPageOnDemand($aParent) {
            var _this = this;
            _super.call(this, $("<div>", {
                "id": CPageOnDemand.ID,
                "class": "prism-page show"
            }));
            this._requestorAPI = null;
            this._requestorOperation = null;
            this._privacyPolicy = true;
            this._termsAndConditions = true;
            var $self = this.$self();
            this.reqCreateLayer('main', function () {
                var APIOperation = _this._requestorOperation;
                APIOperation.getNetworkAvailable(function () {
                    var doc = document;
                    var elTop = _this.getLayoutElement("-top");
                    var elLogo = _this.getLayoutElement("-logo");
                    var elImgFVC = doc.createElement("img");
                    var ctrlGroup = new __CTRL_BASE__.CGroupControl(_this.getLayoutElement("-ctrl-group"));
                    var ctrlProfile = new CProfileListControl(doc.createElement("div"));
                    var ctrlOnDemand = new COnDemandListControl(doc.createElement("div"), '-on-demand-player -player');
                    var elBanner = _this.getLayoutElement('-banner-humax-apps');
                    var ctrlHumax = new COnDemandListControl(doc.createElement("div"), '-humax-app-player -player');
                    var ctrlTvApps = new CTvAppsListControl(doc.createElement("div"));
                    var APIPortal = _this._requestorAPI;
                    var __fnMakeBorder = function (aClassName) {
                        var el = doc.createElement("div");
                        if (aClassName) {
                            el.classList.add(aClassName);
                        }
                        return function (aText) {
                            el.innerHTML = aText;
                            return el;
                        };
                    };
                    var setGridItemTitle = __fnMakeBorder();
                    var setListItemTitle = __fnMakeBorder();
                    var setListItemBorderToMove = __fnMakeBorder(CTvAppsListControl.CLASS_MODE.MOVE);
                    var createInternetConnectionDlg = function () {
                        _this.reqCreateLayer("modal", function () {
                            _this.createModalDialog("ctrl-popup", [{
                                title: hx.l("LOC_TVAPPS_NETWORK_ERROR_ID"),
                                image: "images/614_Help_Promotion_TA01.png",
                                desc: hx.l("LOC_CONNECT_TO_INTERNET_DESC_ID")
                            }]);
                        });
                    };
                    var showPPTCDialog = function (agreeCallback, disagreeCallback) {
                        var currentPP = _this._privacyPolicy;
                        var currentTC = _this._termsAndConditions;
                        _this._privacyPolicy = true;
                        _this._termsAndConditions = true;
                        APIPortal.getPrivacyPolicy(function (aHTML) {
                            var privacyPolicy = aHTML;
                            APIPortal.getTermsAndConditions(function (aHTML) {
                                var termsAndConditions = aHTML;
                                _this.reqCreateLayer("modal-no-back", function () {
                                    var modal = new CSliderWithButtonModalDialog(_this.getLayoutElement("-ctrl-modal"), [hx.l('LOC_PRIVACY_POLICY_ID'), hx.l('LOC_TERMS_AND_CONDITIONS_ID')], [privacyPolicy, termsAndConditions], [{
                                        text: hx.l('LOC_AGREE_PRIVACY_POLICY_ID'),
                                        fn: function (checked) {
                                            _this._privacyPolicy = checked;
                                        }
                                    }, {
                                        text: hx.l('LOC_AGREE_TERMS_AND_CONDITIONS_ID'),
                                        fn: function (checked) {
                                            _this._termsAndConditions = checked;
                                        }
                                    }], [{
                                        text: hx.l('LOC_DONE_ID'),
                                        fn: function () {
                                            APIPortal.setAcceptPrivacyTerms(_this._privacyPolicy, _this._termsAndConditions, function (aSuccess) {
                                                if (_this.getLayerCount()) {
                                                    _this.removeLayer();
                                                }
                                                if (_this._privacyPolicy) {
                                                    agreeCallback();
                                                }
                                                else {
                                                    disagreeCallback();
                                                }
                                                if (!_this._termsAndConditions) {
                                                    if (currentTC !== _this._termsAndConditions) {
                                                        elTop.removeChild(elLogo);
                                                        elLogo.removeChild(elImgFVC);
                                                        elImgFVC.classList.remove("-logo-fvc");
                                                        ctrlOnDemand.setListData([]);
                                                        ctrlGroup.draw();
                                                    }
                                                }
                                                else {
                                                    if (currentTC !== _this._termsAndConditions) {
                                                        elImgFVC.classList.add("-logo-fvc");
                                                        elLogo.appendChild(elImgFVC);
                                                        elTop.appendChild(elLogo);
                                                        APIPortal.getModelPlayers(function (aModelPlayers) {
                                                            var modelOnDemand = [];
                                                            aModelPlayers.forEach(function (app) {
                                                                if (app.PlayerType === '/promoted_players' || app.PlayerType === '/other_players') {
                                                                    modelOnDemand.push(app);
                                                                }
                                                            });
                                                            ctrlOnDemand.setListData(modelOnDemand);
                                                            ctrlGroup.draw();
                                                        });
                                                    }
                                                }
                                            });
                                            return true;
                                        }
                                    }], {
                                        doExitLayer: function () {
                                            _this._privacyPolicy = currentPP;
                                            _this._termsAndConditions = currentTC;
                                            if (_this.getLayerCount()) {
                                                _this.removeLayer();
                                                return true;
                                            }
                                            return false;
                                        }
                                    });
                                    _this.addControl(modal);
                                });
                            }, function (aHTML) {
                                hx.log("debug", "fail to get termsAndConditions");
                                //APIOperation.exit();
                            });
                        }, function (aHTML) {
                            //TODO: expected behavior: notify & exit
                            hx.log("debug", "fail to get privacyPolicy");
                            APIOperation.exit();
                            //throw aHTML;
                        });
                    };
                    var showDisagreeConfirmMsg = function () {
                        var confirmMsg = hx.l('LOC_CANNOT_USE_SOME_SERVICE_IN_PORTAL_MSG_ID');
                        _this.reqCreateLayer("modal", function () {
                            var confirmModal = new __SLIDER__.CModalButtonControl(_this.getLayoutElement("-modal-confirm"), confirmMsg, [{
                                text: hx.l('LOC_OK_ID'),
                                fn: function () {
                                    return true;
                                }
                            }]);
                            confirmModal.onSelected = function () {
                                _this.removeLayer();
                            };
                            confirmModal.addClass('-three-line');
                            _this.addControl(confirmModal);
                        }, false);
                    };
                    var holder = {
                        profileItemSelected: function (aKey, aItem, aEl) {
                            //XXX: just considered info
                            _this.reqCreateLayer("info", function () {
                                var ctrlOndemandInfo = new COnDemandInfo(_this.getLayoutElement("-ondemand-info"), _this._requestorAPI);
                                var optionMenuProvider = [
                                    [{
                                        title: hx.l("LOC_TITLEDELETE_ID"),
                                        fn: function () {
                                            _this.reqCreateLayer("modal", function () {
                                                var modal = new __SLIDER__.CModalButtonControl(_this.getLayoutElement("-modal-selective"), hx.l('LOC_DO_YOU_AGREE_DELETE_COOKIES_ID'), [{
                                                    text: hx.l('LOC_YES_ID'),
                                                    fn: function () {
                                                        return true;
                                                    }
                                                }, {
                                                    text: hx.l('LOC_NO_ID'),
                                                    fn: function () {
                                                        return false;
                                                    }
                                                }]);
                                                modal.onSelected = function (isDelete) {
                                                    _this.removeLayer();
                                                    if (isDelete === true) {
                                                        _this._requestorOperation.deleteCookies();
                                                        setTimeout(function () {
                                                            _this.reqCreateLayer("modal", function () {
                                                                var secondModal = new __SLIDER__.CModalButtonControl(_this.getLayoutElement("-modal-confirm"), hx.l('LOC_COOKIES_HAVE_BEEN_DELETED_ID'), [{
                                                                    text: hx.l('LOC_OK_ID'),
                                                                    fn: function () {
                                                                        return true;
                                                                    }
                                                                }]);
                                                                secondModal.onSelected = function () {
                                                                    _this.removeLayer();
                                                                };
                                                                _this.addControl(secondModal);
                                                            });
                                                        }, 300);
                                                    }
                                                };
                                                _this.addControl(modal);
                                            });
                                        }
                                    }]
                                ];
                                ctrlOndemandInfo.setOptionMenu(hx.l("LOC_TITLEDELETE_ID"), optionMenuProvider[0]);
                                ctrlOndemandInfo.setOptionMenuProvider(optionMenuProvider);
                                ctrlOndemandInfo.setActiveOptionMenu(true);
                                _this.addControl(ctrlOndemandInfo);
                            });
                        },
                        listItemSelected: function (aKey, aItem, aEl) {
                            if (hx.config.netflix && aItem.name === 'Netflix') {
                                hx.il.launchApp("Netflix", aItem.appid, undefined, _this, __NETFLIX__.START_TYPE.DEDICATED_ON_SCREEN_ICON);
                                return;
                            }
                            if (!_this._privacyPolicy) {
                                //TODO: Pop up check dialog
                                showPPTCDialog(function () {
                                    if (!APIPortal.launchApp(aItem, aKey === 0)) {
                                        createInternetConnectionDlg();
                                    }
                                }, function () {
                                    setTimeout(function () {
                                        showDisagreeConfirmMsg();
                                    }, 300);
                                });
                            }
                            else {
                                if (!APIPortal.launchApp(aItem, aKey === 0)) {
                                    createInternetConnectionDlg();
                                }
                            }
                        },
                        playerItemSelected: function (aKey, aItem, aEl) {
                            if (hx.config.netflix && aItem.name === 'Netflix') {
                                hx.il.launchApp("Netflix", aItem.appid, undefined, _this, __NETFLIX__.START_TYPE.DEDICATED_ON_SCREEN_ICON);
                                return;
                            }
                            var isPromoted = aItem.PlayerType ? ((aItem.PlayerType.indexOf('/promoted_players') !== -1) || (aItem.PlayerType.indexOf('/other_players') !== -1)) : false;
                            if (isPromoted) {
                                if (!APIPortal.launchApp(aItem, false, true)) {
                                    createInternetConnectionDlg();
                                }
                            }
                            else {
                                if (!_this._privacyPolicy) {
                                    //TODO: Pop up check dialog
                                    showPPTCDialog(function () {
                                        if (!APIPortal.launchApp(aItem, false, false)) {
                                            createInternetConnectionDlg();
                                        }
                                    }, function () {
                                        setTimeout(function () {
                                            showDisagreeConfirmMsg();
                                        }, 300);
                                    });
                                }
                                else {
                                    if (!APIPortal.launchApp(aItem, false, false)) {
                                        createInternetConnectionDlg();
                                    }
                                }
                            }
                            //if (!APIPortal.launchApp(aItem, false, (aItem.PlayerName ? (aItem.PlayerName.indexOf('promoted') !== -1) : false))) {
                            //    createInternetConnectionDlg();
                            //}
                        },
                        //gridItemSelected: (aKey:any, aItem:ifPortalModelApps, aEl:HTMLElement) => {
                        //    if (!APIPortal.launchApp(aItem)) {
                        //        createInternetConnectionDlg();
                        //    }
                        //},
                        //gridFocusChanged: (aKey:any, aItem:ifPortalModelApps, aEl:HTMLElement) => {
                        //    aEl.appendChild(setGridItemTitle(aItem.name));
                        //},
                        listFocusChanged: function (aKey, aItem, aEl) {
                            aEl.appendChild(setListItemTitle(aItem.name));
                            aEl.appendChild(setListItemBorderToMove(""));
                        },
                        listDataItemOrderChanged: (function () {
                            var movedList = null;
                            return function (aPosPrev, aPosNew, aDataProvider) {
                                if (!movedList) {
                                    movedList = _.cloneDeep(aDataProvider._listData);
                                }
                                else if (movedList.length !== aDataProvider.getLength()) {
                                    throw "you may need to reload";
                                }
                                if (aPosPrev > aPosNew) {
                                    var item = movedList.splice(aPosPrev, 1);
                                    if (item && item[0]) {
                                        movedList.splice(aPosNew, 0, item[0]);
                                    }
                                    else {
                                        throw "something wrong";
                                    }
                                }
                                else {
                                    var item = movedList.splice(aPosPrev, 1);
                                    if (item && item[0]) {
                                        movedList.splice(aPosNew, 0, item[0]);
                                    }
                                    else {
                                        throw "something wrong";
                                    }
                                }
                                APIPortal.setTvAppsArrangement(_.pluck(movedList, "appid"));
                            };
                        })(),
                        groupFocusChanged: function (aOld, aNew) {
                            if (!aNew.classList.contains('-profile')) {
                                ctrlGroup.getFocusedChild().initFocusedElement();
                            }
                            /* author: bglee@humaxdigital.com
                             * date: 2015-02-09 오전 5:07
                             * comments:
                             - disabled option menu(move)
                             */
                            //this.setActiveOptionMenu(aNew === ctrlTvApps.$self()[0]);
                        }
                    };
                    elBanner.innerText = hx.l('LOC_HUMAX_APPS_ID');
                    ctrlProfile.connectDataItemSelected(holder, "profileItemSelected", holder.profileItemSelected);
                    //ctrlOnDemand.connectFocusedDataItemChanged(holder, "gridFocusChanged", holder.gridFocusChanged);
                    //ctrlOnDemand.connectDataItemSelected(holder, "gridItemSelected", holder.gridItemSelected);
                    //ctrlOnDemand.connectFocusedDataItemChanged(holder, "gridFocusChanged", holder.gridFocusChanged);
                    ctrlOnDemand.connectDataItemSelected(holder, "playerItemSelected", holder.playerItemSelected);
                    //ctrlHumax.connectFocusedDataItemChanged(holder, "gridFocusChanged", holder.gridFocusChanged);
                    ctrlHumax.connectDataItemSelected(holder, "playerItemSelected", holder.playerItemSelected);
                    ctrlTvApps.connectFocusedDataItemChanged(holder, "listFocusChanged", holder.listFocusChanged);
                    ctrlTvApps.connectDataItemSelected(holder, "listItemSelected", holder.listItemSelected);
                    ctrlTvApps.connectDataItemOrderChanged(holder, "listDataItemOrderChanged", holder.listDataItemOrderChanged);
                    ctrlGroup.connectFocusChanged(holder, "groupFocusChanged", holder.groupFocusChanged);
                    ctrlGroup.setKeyBuilder(1 /* EVertical */);
                    //ctrlGroup.setOwnedChildControls([ctrlProfile, ctrlOnDemand, ctrlTvApps]);
                    ctrlGroup.setOwnedChildControls([ctrlProfile, ctrlOnDemand, ctrlHumax, ctrlTvApps]);
                    _this.addControl(ctrlGroup);
                    var fnLoad = function (aLoginData) {
                        var fnLoginAfter = function () {
                            ctrlProfile.setListData([
                                aLoginData
                            ]);
                            APIPortal.getModelPlayers(function (aModelPlayers) {
                                var modelOnDemand = [];
                                var modelHumax = [];
                                aModelPlayers.forEach(function (app) {
                                    if (app.PlayerType === '/promoted_players' || app.PlayerType === '/other_players') {
                                        modelOnDemand.push(app);
                                    }
                                    else {
                                        modelHumax.push(app);
                                    }
                                });
                                ctrlOnDemand.setListData(modelOnDemand);
                                ctrlHumax.setListData(modelHumax);
                                ctrlGroup.draw();
                                elBanner.classList.add('-show');
                                if (aModelPlayers.length > 0) {
                                    ctrlGroup.setFocusedControl(ctrlOnDemand);
                                    if (modelOnDemand.length) {
                                        ctrlOnDemand.initFocusedElement();
                                    }
                                    else {
                                        ctrlHumax.initFocusedElement();
                                    }
                                }
                                hx.log("debug", "players draw");
                            });
                            APIPortal.getModelApps(function (aModelApps, aIsUpdate, aIndex) {
                                if (!aIsUpdate) {
                                    ctrlTvApps.setListData(aModelApps);
                                }
                                else {
                                    ctrlTvApps.insertItems(aIndex, aModelApps);
                                }
                                _this.setOptionMenu("LOC_MOVE_01_ID", [{
                                    title: hx.l("LOC_MOVE_01_ID"),
                                    fn: function () {
                                        ctrlTvApps.setModeMove();
                                    }
                                }]);
                                ctrlGroup.draw();
                                //ctrlTvApps.initFocusedElement();
                                hx.log("debug", "tv-apps draw");
                            });
                        };
                        hx.log('error', '########## AcceptPrivacy : ' + aLoginData.AcceptPrivacy + ', AcceptTerms : ' + aLoginData.AcceptTerms);
                        if ((aLoginData.AcceptPrivacy == false) || (aLoginData.AcceptTerms == false)) {
                            //if(APIOperation.isNeedTerms()) {
                            if (true) {
                                APIPortal.getPrivacyPolicy(function (aHTML) {
                                    var privacyPolicy = aHTML;
                                    APIPortal.getTermsAndConditions(function (aHTML) {
                                        var termsAndConditions = aHTML;
                                        _this.reqCreateLayer("modal-no-back", function () {
                                            var modal = new CSliderWithButtonModalDialog(_this.getLayoutElement("-ctrl-modal"), [hx.l('LOC_PRIVACY_POLICY_ID'), hx.l('LOC_TERMS_AND_CONDITIONS_ID')], [privacyPolicy, termsAndConditions], [{
                                                text: hx.l('LOC_AGREE_PRIVACY_POLICY_ID'),
                                                fn: function (checked) {
                                                    _this._privacyPolicy = checked;
                                                }
                                            }, {
                                                text: hx.l('LOC_AGREE_TERMS_AND_CONDITIONS_ID'),
                                                fn: function (checked) {
                                                    _this._termsAndConditions = checked;
                                                }
                                            }], [{
                                                text: hx.l('LOC_DONE_ID'),
                                                fn: function () {
                                                    APIPortal.setAcceptPrivacyTerms(_this._privacyPolicy, _this._termsAndConditions, function (aSuccess) {
                                                        //if (this._privacyPolicy) {
                                                        if (_this.getLayerCount()) {
                                                            _this.removeLayer();
                                                        }
                                                        fnLoginAfter();
                                                        //} else {
                                                        //    APIOperation.exit();
                                                        //}
                                                        if (_this._termsAndConditions) {
                                                            elImgFVC.classList.add("-logo-fvc");
                                                            elLogo.appendChild(elImgFVC);
                                                            elTop.appendChild(elLogo);
                                                        }
                                                    });
                                                    return true;
                                                }
                                            }], {
                                                doExitLayer: function () {
                                                    APIOperation.exit();
                                                    return false;
                                                }
                                            });
                                            _this.addControl(modal);
                                        });
                                    }, function (aHTML) {
                                        hx.log("debug", "fail to get termsAndConditions");
                                        APIOperation.exit();
                                    });
                                }, function (aHTML) {
                                    //TODO: expected behavior: notify & exit
                                    hx.log("debug", "fail to get privacyPolicy");
                                    APIOperation.exit();
                                    //throw aHTML;
                                });
                            }
                            else {
                                if (!aLoginData.AcceptTerms) {
                                    _this.reqCreateLayer("modal", function () {
                                        var modal = new __SLIDER__.CModalButtonControl(_this.getLayoutElement("-modal-selective"), hx.l('LOC_ONDEMAND_AGREE_TERMS_AND_CONDITIONS_ID'), [{
                                            text: hx.l('LOC_AGREE_ID'),
                                            fn: function () {
                                                return true;
                                            }
                                        }, {
                                            text: hx.l('LOC_DISAGREE_ID'),
                                            fn: function () {
                                                return false;
                                            }
                                        }]);
                                        modal.onSelected = function (isAccept) {
                                            _this._termsAndConditions = isAccept;
                                            if (_this._termsAndConditions) {
                                                elImgFVC.classList.add("-logo-fvc");
                                                elLogo.appendChild(elImgFVC);
                                                elTop.appendChild(elLogo);
                                            }
                                            APIPortal.setAcceptTerms(isAccept, function (aSuccess) {
                                                _this.removeLayer();
                                                if (!aLoginData.AcceptPrivacy) {
                                                    setTimeout(function () {
                                                        _this.reqCreateLayer("modal", function () {
                                                            var secondModal = new __SLIDER__.CModalButtonControl(_this.getLayoutElement("-modal-selective"), hx.l('LOC_ONDEMAND_AGREE_PRIVACY_POLICY_ID'), [{
                                                                text: hx.l('LOC_AGREE_ID'),
                                                                fn: function () {
                                                                    return true;
                                                                }
                                                            }, {
                                                                text: hx.l('LOC_DISAGREE_ID'),
                                                                fn: function () {
                                                                    return false;
                                                                }
                                                            }]);
                                                            secondModal.onSelected = function (isAccept) {
                                                                _this._privacyPolicy = isAccept;
                                                                _this.removeLayer();
                                                                APIPortal.setAcceptPrivacy(_this._privacyPolicy, function (aSuccess) {
                                                                    hx.log("debug", "setAcceptPrivacy true return: " + aSuccess);
                                                                    if (_this._privacyPolicy) {
                                                                        fnLoginAfter();
                                                                    }
                                                                    else {
                                                                        APIOperation.exit();
                                                                    }
                                                                });
                                                            };
                                                            _this.addControl(secondModal);
                                                        });
                                                    }, 300);
                                                }
                                                else {
                                                    fnLoginAfter();
                                                }
                                            });
                                            if (_this._termsAndConditions) {
                                                elImgFVC.classList.add("-logo-fvc");
                                                elLogo.appendChild(elImgFVC);
                                                elTop.appendChild(elLogo);
                                            }
                                        };
                                        _this.addControl(modal);
                                    });
                                }
                                else if (!aLoginData.AcceptPrivacy) {
                                    _this.reqCreateLayer("modal", function () {
                                        var secondModal = new __SLIDER__.CModalButtonControl(_this.getLayoutElement("-modal-selective"), hx.l('LOC_ONDEMAND_AGREE_PRIVACY_POLICY_ID'), [{
                                            text: hx.l('LOC_AGREE_ID'),
                                            fn: function () {
                                                return true;
                                            }
                                        }, {
                                            text: hx.l('LOC_DISAGREE_ID'),
                                            fn: function () {
                                                return false;
                                            }
                                        }]);
                                        secondModal.onSelected = function (isAccept) {
                                            _this._privacyPolicy = isAccept;
                                            _this.removeLayer();
                                            APIPortal.setAcceptPrivacy(_this._privacyPolicy, function (aSuccess) {
                                                hx.log("debug", "setAcceptPrivacy true return: " + aSuccess);
                                                if (_this._privacyPolicy) {
                                                    fnLoginAfter();
                                                }
                                                else {
                                                    APIOperation.exit();
                                                }
                                            });
                                            if (_this._termsAndConditions) {
                                                elImgFVC.classList.add("-logo-fvc");
                                                elLogo.appendChild(elImgFVC);
                                                elTop.appendChild(elLogo);
                                            }
                                        };
                                        _this.addControl(secondModal);
                                    });
                                }
                            }
                        }
                        else {
                            elImgFVC.classList.add("-logo-fvc");
                            elLogo.appendChild(elImgFVC);
                            elTop.appendChild(elLogo);
                            fnLoginAfter();
                        }
                    };
                    var fnLoadDefault = function () {
                        ctrlProfile.setListData([{}]);
                        ctrlOnDemand.setListData([]);
                        ctrlHumax.setListData([]);
                        APIPortal.getModelApps(function (aModelApps, aIsUpdate, aIndex) {
                            if (!aIsUpdate) {
                                ctrlTvApps.setListData(aModelApps);
                            }
                            else {
                                ctrlTvApps.insertItems(aIndex, aModelApps);
                            }
                            ctrlGroup.draw();
                            hx.log("debug", "tv-apps draw");
                        });
                        ctrlGroup.draw();
                        elBanner.classList.add('-show');
                        throw "login fail";
                    };
                    var fnStartOnDemand = function () {
                        hx.log('error', 'fnStartOnDemand isLogged : ' + APIPortal.isLogged());
                        APIPortal.login(function (aData) {
                            fnLoad(aData);
                        }, function (aData) {
                            fnLoad(aData);
                            //fnLoadDefault();
                        });
                    };
                    if (APIOperation.isNeedHelp()) {
                        _this.reqCreateLayer("modal", function () {
                            _this.createModalDialog("ctrl-help", [{
                                title: hx.l("LOC_EXPERIENCE_FREE_ID"),
                                image: "images/614_Help_Promotion_TA01.png",
                                desc: hx.l("LOC_HELP_ONDEMAND_STEP_1_SCENE_1_TXT_ID")
                            }, {
                                title: hx.l("LOC_EXPLORE_THE_VARIETY_OF_TV_APPS_ID"),
                                image: "images/614_Help_Promotion_TA02.png",
                                desc: hx.l("LOC_ON_DEMAND_HELP_02_DESC_ID")
                            }], APIOperation.setVisit, fnStartOnDemand);
                        });
                    }
                    else {
                        fnStartOnDemand();
                    }
                }, function () {
                    var modal = new __SLIDER__.CModalDialogSildeControl(_this.getLayoutElement("-ctrl-modal"), "ctrl-popup", [{
                        title: hx.l("LOC_TVAPPS_NETWORK_ERROR_ID"),
                        image: "images/614_Help_Promotion_TA01.png",
                        desc: hx.l("LOC_CONNECT_TO_INTERNET_DESC_ID")
                    }], 30);
                    modal.connectWhenReqClosed(APIOperation, "exit", APIOperation.exit);
                    _this.addControl(modal);
                    modal.draw();
                });
            });
            $aParent.append($self);
        }
        CPageOnDemand.prototype.setDataRequestor = function (aRequestor) {
            this._requestorAPI = aRequestor;
            return this;
        };
        CPageOnDemand.prototype.setOperationRequester = function (aRequestor) {
            this._requestorOperation = aRequestor;
            return this;
        };
        CPageOnDemand.prototype.reqCreateLayer = function (aName, aFnAfterTransition, isDim) {
            if (isDim === void 0) { isDim = true; }
            var dim = isDim ? "" : " -no-dim";
            this.createLayer({
                addClass: "-layer-" + aName.toLocaleLowerCase() + dim,
                transition: {
                    newLayer: "fadeIn",
                    fnAfterTransition: aFnAfterTransition
                }
            });
            return this;
        };
        CPageOnDemand.prototype.createModalDialog = function (id, slideInfo, fnAfter, fnCloseCallback) {
            var _this = this;
            var modal;
            if (fnCloseCallback) {
                modal = new CSliderDialogWithExit(this.getLayoutElement("-ctrl-modal"), id, slideInfo, 0, {
                    doBackLayer: function () {
                        if (_this.getLayerCount()) {
                            _this.removeLayer();
                            if (fnCloseCallback) {
                                setTimeout(function () {
                                    fnCloseCallback();
                                }, 300);
                            }
                        }
                    }
                });
            }
            else {
                modal = new CSliderDialogWithExit(this.getLayoutElement("-ctrl-modal"), id, slideInfo, 0);
            }
            var holder = {
                reqClose: function () {
                    if (_this.getLayerCount()) {
                        _this.removeLayer();
                        if (fnCloseCallback) {
                            setTimeout(function () {
                                fnCloseCallback();
                            }, 300);
                        }
                    }
                }
            };
            modal.connectWhenReqClosed(holder, "reqClose", holder.reqClose);
            this.addControl(modal);
            modal.draw();
            if (fnAfter) {
                fnAfter();
            }
            return this;
        };
        CPageOnDemand.prototype._doKeyBackLatent = function () {
            var handled = false;
            if (this.getLayerCount()) {
                this.removeLayer();
                handled = true;
            }
            return handled;
        };
        CPageOnDemand.ID = "on-demand";
        return CPageOnDemand;
    })(__PAGE__);
    var CProfileListControl = (function (_super) {
        __extends(CProfileListControl, _super);
        function CProfileListControl(aElement) {
            _super.call(this, aElement);
            this.addClass("-profile");
            this.setClassToGroupViewElement("-user-info");
            this.setItemWidth(60);
            this.setItemHeight(76);
            this.setOrientation(2 /* EHorizontal */);
            this.setDataDrawer(function (aKey, aItem, aEl) {
                var elBorder = document.createElement("div");
                var elName = document.createElement("div");
                elBorder.appendChild(elName);
                aEl.appendChild(elBorder);
                aEl.classList.add("-user-info");
                return 2 /* KFocusAble */;
            });
            this._doKeyRightLatent = function () {
                return true;
            };
            this._doKeyLeftLatent = function () {
                return true;
            };
        }
        return CProfileListControl;
    })(__CTRL_BASE__.CListControl);
    var COnDemandListControl = (function (_super) {
        __extends(COnDemandListControl, _super);
        function COnDemandListControl(aElement, className) {
            _super.call(this, aElement);
            var TFocusInfo = __CTRL_BASE__.TFocusInfo;
            this.setClassToGroupViewElement(className);
            this.setItemWidth(270);
            this.setItemHeight(146);
            this.setOrientation(2 /* EHorizontal */);
            this.setScrollScheme(5 /* EByFixed */, 270);
            this.setDataDrawer(function (aKey, aItem, aEl) {
                var ret = 2 /* KFocusAble */;
                aEl.classList.add('-unload');
                if (aItem.focusable) {
                    if (aItem.icons.length > 0) {
                        var icons = aItem.icons;
                        var index = -1;
                        for (var i = 0; i < icons.length; i++) {
                            if (icons[i].ImageType === "STBLAUNCHER_NORMAL") {
                                index = i;
                                break;
                            }
                            else if (icons[i].ImageType === "HMS_FOCUS") {
                                index = i;
                            }
                        }
                        if (index !== -1) {
                            var elImage = document.createElement("img");
                            elImage.onload = function () {
                                this.style.opacity = 1;
                            };
                            if (aItem.PlayerType === '/promoted_players' || aItem.PlayerType === '/other_players') {
                                hx.svc.IP_EPG.getThumbnail(aItem.icons[index].Url, 260).done(function (url) {
                                    elImage.setAttribute("src", url);
                                    aEl.classList.remove('-unload');
                                });
                            }
                            else {
                                elImage.setAttribute("src", aItem.icons[index].Url);
                                aEl.classList.remove('-unload');
                            }
                            aEl.appendChild(elImage);
                        }
                    }
                }
                else {
                    ret = 1 /* KFocusNone */;
                }
                if (parseInt(aEl.style.top) > 0) {
                    aEl.style.marginTop = "8px";
                }
                return ret;
            });
            this._dataControl._doKeyRightLatent = function () {
                var item = this.getOwnedDataProvider().getItem(this.getFocusedElementIndex() + 1);
                return !(item && item.focusable);
            };
        }
        COnDemandListControl.prototype.initFocusedElement = function () {
            var dataControl = this._dataControl;
            var focusedElementIndex = dataControl.getFocusedElementIndex();
            if (focusedElementIndex !== -1) {
                dataControl.setFocusedElementByIndex(focusedElementIndex, true);
            }
            return this;
        };
        COnDemandListControl.prototype.setListData = function (aData, aDataRolling) {
            for (var i = aData.length; i < 5; i++) {
                aData[i] = {};
            }
            _super.prototype.setListData.call(this, aData, aDataRolling);
        };
        return COnDemandListControl;
    })(__CTRL_BASE__.CListControl);
    var CTvAppsListControl = (function (_super) {
        __extends(CTvAppsListControl, _super);
        function CTvAppsListControl(aElement) {
            _super.call(this, aElement);
            this._mode = 0 /* NORMAL */;
            var TFocusInfo = __CTRL_BASE__.TFocusInfo;
            this.setClassToGroupViewElement("-tv-apps");
            this.setItemWidth(114);
            this.setItemHeight(104);
            this.setOrientation(2 /* EHorizontal */);
            this.setScrollScheme(5 /* EByFixed */, 114);
            this.setDataDrawer(function (aKey, aItem, aEl) {
                if (aItem.icons.length && aItem.icons[0].Url) {
                    aEl.style.backgroundImage = "url(" + aItem.icons[3].Url + ")";
                }
                return 2 /* KFocusAble */;
            });
        }
        CTvAppsListControl.prototype.initFocusedElement = function () {
            var dataControl = this._dataControl;
            var focusedElementIndex = dataControl.getFocusedElementIndex();
            if (focusedElementIndex !== -1) {
                dataControl.setFocusedElementByIndex(focusedElementIndex, true);
            }
            return this;
        };
        //XXX: MOVE
        CTvAppsListControl.prototype._setMode = function (aMode) {
            if (this._mode !== aMode) {
                this._dataControl.setModeMove(aMode === 1 /* MOVE */);
            }
            this._mode = aMode;
            var classes = Object.keys(CTvAppsListControl.CLASS_MODE);
            var el = this._element;
            var className;
            var mode = null;
            for (var i = 0; i < classes.length; i++) {
                className = CTvAppsListControl.CLASS_MODE[classes[i]];
                if (classes[i] === eModeListControl[aMode]) {
                    mode = className;
                }
                else {
                    if (className) {
                        el.classList.remove(className);
                    }
                }
            }
            if (mode) {
                el.classList.add(mode);
            }
        };
        CTvAppsListControl.prototype.setModeMove = function () {
            if (this._element.classList.contains(CTvAppsListControl.CLASS_MODE[eModeListControl[1 /* MOVE */]])) {
                this._setMode(0 /* NORMAL */);
            }
            else {
                this._setMode(1 /* MOVE */);
            }
            return this;
        };
        CTvAppsListControl.CLASS_MODE = {
            NORMAL: "",
            MOVE: "-mode-move",
            DELETE: "-mode-delete"
        };
        return CTvAppsListControl;
    })(__CTRL_BASE__.CListControl);
    var COnDemandInfo = (function (_super) {
        __extends(COnDemandInfo, _super);
        function COnDemandInfo(aElement, aAPI) {
            _super.call(this, aElement);
            this._buttonOption = null;
            var elTitle = document.createElement("div");
            var elMenu = document.createElement("div");
            var ctrlList = new __CTRL_BASE__.CListControl(elMenu);
            var ctrlScroll = new __SCROLL_CONTROL__($("<div>"));
            elTitle.classList.add(COnDemandInfo.CLASS.TITLE);
            elTitle.innerText = hx.l("LOC_INFO_ID");
            ctrlList.setClassToGroupViewElement(COnDemandInfo.CLASS.MENU);
            this.$self().prepend(elTitle);
            ctrlList.setListData([
                COnDemandInfo.MENU.DELETE_COOKIES
            ]);
            ctrlList.setItemWidth(200);
            ctrlList.setItemHeight(60);
            ctrlList.setOrientation(1 /* EVertical */);
            ctrlList.setScrollScheme(1 /* EByItem */);
            ctrlList._doKeyUpLatent = function () {
                return true;
            };
            ctrlList._doKeyDownLatent = function () {
                return true;
            };
            ctrlList.setDataDrawer(function (aKey, aItem, aEl) {
                var elText = document.createElement('div');
                elText.innerHTML = aItem;
                aEl.appendChild(elText);
                return 2 /* KFocusAble */;
            });
            this.setOwnedChildControls([ctrlList, ctrlScroll]);
            this.setKeyBuilder(2 /* EHorizontal */);
            this.draw();
            ctrlScroll.setHTML('A cookie is a small amount of data placed on your Product. A cookie is installed automatically when you use the Product and Services, but you can delete cookies by pressing the <+> button.');
        }
        COnDemandInfo.prototype.setOptionMenuProvider = function (optionMenuProvider) {
            this.optionMenuProvider = optionMenuProvider;
        };
        COnDemandInfo.prototype.setOptionMenu = function (aTitle, aContext) {
            if (aContext.length === 1) {
                var elOptionButton = document.createElement("div");
                var optionButton = new __BUTTON_PROMOTION__.CButtonPromotion(elOptionButton);
                elOptionButton.classList.add("-button-option-menu");
                elOptionButton.innerText = aTitle;
                optionButton.setData(aContext);
                this._buttonOption = optionButton;
                this._element.appendChild(elOptionButton);
            }
            else {
            }
            return this;
        };
        //changeOptionMenuProvider(aTitle: string, aContext: ifOptionMenuContext[]) {
        //    if (this._buttonOption !== null) {
        //        var elOptionButton = this._element.getElementsByClassName("-button-option-menu");
        //        (<any>elOptionButton[0]).innerText = aTitle;
        //        this._buttonOption.setData(aContext);
        //    }
        //}
        COnDemandInfo.prototype.setActiveOptionMenu = function (aActive) {
            aActive ? this._buttonOption.showButton() : this._buttonOption.hideButton();
            return this;
        };
        COnDemandInfo.prototype._doKeyOptionLatent = function () {
            //TODO: start here
            var handled = false;
            var buttonOption = this._buttonOption;
            if (buttonOption.isShowing()) {
                var data = buttonOption.getData();
                if (data) {
                    if (data.length === 1) {
                        data[0].fn();
                    }
                    handled = true;
                }
            }
            return handled;
        };
        COnDemandInfo.CLASS = {
            TITLE: "-title",
            MENU: "-menu",
            VIEW: "-view",
            TEXT: "-text",
            SCROLLBAR: "-scroll-bar"
        };
        COnDemandInfo.MENU = {
            DELETE_COOKIES: hx.l('LOC_DELETE_COOKIES_ID')
        };
        return COnDemandInfo;
    })(__CTRL_BASE__.CGroupControl);
    var CSliderDialogWithExit = (function (_super) {
        __extends(CSliderDialogWithExit, _super);
        function CSliderDialogWithExit(aParent, aID, aModel, autoClose, layerFunc) {
            if (autoClose === void 0) { autoClose = 0; }
            _super.call(this, aParent, aID, aModel, autoClose);
            this.modalCtrlFunc = layerFunc;
        }
        CSliderDialogWithExit.prototype.doKey = function (aKeyStr, aArgs) {
            var ret = _super.prototype.doKey.call(this, aKeyStr, aArgs);
            //close this layer
            if (aKeyStr === "Back") {
                if (this.modalCtrlFunc) {
                    this.modalCtrlFunc.doBackLayer();
                }
                return true;
            }
            return ret;
        };
        return CSliderDialogWithExit;
    })(__SLIDER__.CModalDialogSildeControl);
    var CSliderWithButtonModalDialog = (function (_super) {
        __extends(CSliderWithButtonModalDialog, _super);
        function CSliderWithButtonModalDialog(element, titles, contents, checkboxInfos, modelInfo, layerFunc) {
            _super.call(this, element);
            this.setId('dlg-privacy-policy');
            var ctrlGroup = new __CTRL_BASE__.CGroupControl(document.createElement('div'));
            ctrlGroup.setId('ctrl-scroll-group');
            var elPPGroup = $('<div>');
            elPPGroup.addClass('-ctrl-privacy-policy');
            elPPGroup.append($('<div>').addClass('-title').text(titles[0]));
            var ctrlPrivacyPolicyGroup = new __CTRL_BASE__.CGroupControl(elPPGroup[0]);
            var ctrlPrivacyPolicy = new __SCROLL_CONTROL__($("<div>"));
            var ctrlPPCheckBox = new __SLIDER__.CCheckBoxGroupControl(document.createElement('div'), checkboxInfos[0]);
            ctrlPrivacyPolicyGroup.setOwnedChildControls([ctrlPrivacyPolicy, ctrlPPCheckBox]);
            ctrlPrivacyPolicyGroup.setKeyBuilder(1 /* EVertical */);
            var elTCGroup = $('<div>');
            elTCGroup.addClass('-ctrl-terms-and-conditions');
            elTCGroup.append($('<div>').addClass('-title').text(titles[1]));
            var ctrlTermsAndConditionsGroup = new __CTRL_BASE__.CGroupControl(elTCGroup[0]);
            var ctrlTermsAndConditions = new __SCROLL_CONTROL__($("<div>"));
            var ctrlTCCheckBox = new __SLIDER__.CCheckBoxGroupControl(document.createElement('div'), checkboxInfos[1]);
            ctrlTermsAndConditionsGroup.setOwnedChildControls([ctrlTermsAndConditions, ctrlTCCheckBox]);
            ctrlTermsAndConditionsGroup.setKeyBuilder(1 /* EVertical */);
            ctrlGroup.setOwnedChildControls([ctrlPrivacyPolicyGroup, ctrlTermsAndConditionsGroup]);
            ctrlGroup.setKeyBuilder(2 /* EHorizontal */);
            var ctrlButtons = new __SLIDER__.CButtonGroupControl(document.createElement('div'), null, modelInfo);
            this.setOwnedChildControls([ctrlGroup, ctrlButtons]);
            this.setKeyBuilder(1 /* EVertical */);
            ctrlPPCheckBox.onSelected = function (value) {
                console.log('ctrlCheckBox return value: ' + !!value);
            };
            ctrlTCCheckBox.onSelected = function (value) {
                console.log('ctrlCheckBox return value: ' + !!value);
            };
            ctrlButtons.onSelected = function (value) {
                console.log('ctrlButton return value: ' + !!value);
            };
            this.draw();
            ctrlPrivacyPolicy.setHTML(contents[0]);
            ctrlTermsAndConditions.setHTML(contents[1]);
            ctrlButtons.setFocusedElementByIndex(0);
            ctrlGroup.setFocusedChild(ctrlPrivacyPolicyGroup);
            ctrlPrivacyPolicyGroup.setFocusedChild(ctrlPPCheckBox);
            ctrlTermsAndConditionsGroup.setFocusedChild(ctrlTCCheckBox);
            this.setFocusedControl(ctrlButtons);
            this.modalCtrlFunc = layerFunc;
        }
        CSliderWithButtonModalDialog.prototype.doKey = function (aKeyStr, aArgs) {
            var ret = _super.prototype.doKey.call(this, aKeyStr, aArgs);
            if (aKeyStr === "Back") {
                var rootFocusCtrl = this.getFocusedChild();
                if (rootFocusCtrl instanceof __CTRL_BASE__.CGroupControl) {
                    var termsGroup = rootFocusCtrl.getFocusedChild();
                    var focusCtrl = termsGroup.getFocusedChild();
                    if (focusCtrl instanceof __SCROLL_CONTROL__) {
                        termsGroup._keyMap.changeFocus(1);
                        return true;
                    }
                }
            }
            //close this layer
            if (aKeyStr === "Escape" || aKeyStr === "Back") {
                if (this.modalCtrlFunc) {
                    ret = this.modalCtrlFunc.doExitLayer();
                }
            }
            return ret;
        };
        CSliderWithButtonModalDialog.prototype.onMovableStateBefore = function (movable) {
            //movable ? this._arrows.left.show() : this._arrows.left.hide();
        };
        CSliderWithButtonModalDialog.prototype.onMovableStateAfter = function (movable) {
            //movable ? this._arrows.right.show() : this._arrows.right.hide();
        };
        return CSliderWithButtonModalDialog;
    })(__SLIDER__.CModalDialogControl);
    return CPageOnDemand;
});
/* author: bglee@humaxdigital.com
 * date: 2015-05-05 오후 9:30
 * comments:
    - COnDemandGridControl deprecated
 */
/*
class COnDemandGridControl extends __CTRL_BASE__.CGridControl {
    constructor(aElement: HTMLElement) {
        super(aElement);
        var TFocusInfo = __CTRL_BASE__.TFocusInfo;
        var elGridTitle = document.createElement("div");
        elGridTitle.classList.add("-title");
        elGridTitle.innerHTML = "ON DEMAND";
        this.$self().append(elGridTitle);
        this.setClassToGroupViewElement("-on-demand-player");
        this.setMaxRowCount(2);
        this.setItemWidth(286);
        this.setItemHeight(145);
        this.setOrientation(__CTRL_BASE__.TParamOrientation.EHorizontal);
        this.setDataDrawer((aKey: any, aItem: ifPortalModelApps, aEl: HTMLElement): __CTRL_BASE__.TFocusInfo => {
            var ret = TFocusInfo.KFocusAble;
            if (aItem.focusable) {
                if (aItem.icons.length > 0) {
                    var icons = aItem.icons;
                    var index = -1;
                    for (var i = 0; i < icons.length; i++) {
                        if (icons[i].ImageType === "STBLAUNCHER_NORMAL") {
                            index = i;
                            break;
                        } else if (icons[i].ImageType === "HMS_FOCUS") {
                            index = i;
                        }
                    }
                    if (index !== -1) {
                        var elImage = document.createElement("img");
                        elImage.onload = function() {
                            this.style.opacity = 1;
                        };
                        elImage.setAttribute("src", aItem.icons[index].Url);
                        aEl.appendChild(elImage);
                    }
                }
            } else {
                ret = TFocusInfo.KFocusNone;
            }
            if (parseInt(aEl.style.top) > 0) {
                aEl.style.marginTop = "8px";
            }
            return ret;
        });
        this._doKeyRightLatent = function() { return true; };
        this._doKeyLeftLatent = function() { return true; };
    }
    _doDraw(aRect: __CTRL_BASE__.TRect, aDrawParam: { [key: string]: any; }) {
        var countRow = this._dataControl.getMaxRowCount();
        this._dataControl.getOwnedDataProvider().sort((aApps: ifPortalModelApps[]): ifPortalModelApps[] => {
            var countFVC = 0;
            var countOther = 1;
            var ret: ifPortalModelApps[] = <ifPortalModelApps[]>[];
            var app;
            var countBaseScreen = 10;
            var i;
            var fxIsSorted = function() {
                var isSorted = aApps.length > 0;
                for (i = 0; i < aApps.length; i++) {
                    app = aApps[i];
                    if (app.PlayerName !== undefined) {
                        if (app.PlayerName !== "FVC" && i % countRow === 0) {
                            isSorted = false;
                            break;
                        }
                    }
                }
                return isSorted && aApps.length >= countBaseScreen;
            };
            if (fxIsSorted()) {
                ret = aApps;
            } else {
                for (i = 0; i < aApps.length; i++) {
                    app = aApps[i];
                    if (app.PlayerName === "FVC") {
                        ret[countFVC] = app;
                        if (!ret[countFVC + 1]) {
                            ret[countFVC + 1] = <ifPortalModelApps>{};
                        }
                        countFVC += 2;
                    } else {
                        ret[countOther] = app;
                        if (!ret[countOther - 1]) {
                            ret[countOther - 1] = <ifPortalModelApps>{};
                        }
                        countOther += 2;
                    }
                }
                if (ret.length < countBaseScreen) {
                    for (i = 0; i < countBaseScreen; i++) {
                        if (!ret[i]) {
                            ret[i] = <ifPortalModelApps>{};
                        }
                    }
                }
            }
            return ret;
        });
        return super._doDraw(aRect, aDrawParam);
    }
    initFocusedElement() {
        var dataControl = this._dataControl;
        var focusedElementIndex = dataControl.getFocusedElementIndex();
        if (focusedElementIndex !== -1) {
            dataControl.setFocusedElementByIndex(focusedElementIndex, true);
        }
        return this;
    }
}
*/
