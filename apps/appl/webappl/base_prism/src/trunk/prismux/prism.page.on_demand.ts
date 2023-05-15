/**
 * Created by bglee on 2014-10-29.
 */
///<reference path="../def/sugar.d.ts"/>
///<reference path="../def/prism.d.ts"/>

import __CTRL_BASE__ = require('prismux/controls/ctrlbase');
import __BUTTON_PROMOTION__ = require("prismux/layout/prism.layout.button.promotion");
import __SLIDER__ = require("prismux/by_controls/prism.slider");
import __PAGE__ = require("prismux/by_controls/prism.page");
import __SCROLL_CONTROL__ = require("prismux/by_controls/prism.scroll");
import CArrowLayout = require("prismux/layout/prism.layout.arrow");
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");

declare var hx: ifSugar;

enum eModeListControl {
    NORMAL,
    MOVE,
    DELETE
}

interface ifAPIOperation {
    exit(): void;
    getNetworkAvailable(fnSuccess, fnFail): boolean;
    isNeedHelp(): boolean;
    setVisit(): void;
    deleteCookies(): void;
    isNeedTerms(): boolean;
}

class CPageOnDemand extends __PAGE__ {
    private _requestorAPI: ifPortalRequestorAPI = null;
    private _requestorOperation: ifAPIOperation = null;
    static ID = "on-demand";
    private _privacyPolicy: boolean = true;
    private _termsAndConditions: boolean = true;

    constructor($aParent: JQuery) {
        super($("<div>", {
            "id": CPageOnDemand.ID,
            "class": "prism-page show"
        }));
        var $self = this.$self();
        this.reqCreateLayer('main', () => {
            var APIOperation = this._requestorOperation;
            APIOperation.getNetworkAvailable(
                () => {
                    var doc = document;
                    var elTop = this.getLayoutElement("-top");
                    var elLogo = this.getLayoutElement("-logo");
                    var elImgFVC = doc.createElement("img");
                    var ctrlGroup = new __CTRL_BASE__.CGroupControl(this.getLayoutElement("-ctrl-group"));
                    var ctrlProfile = new CProfileListControl(doc.createElement("div"));
                    var ctrlOnDemand = new COnDemandListControl(doc.createElement("div"), '-on-demand-player -player');
                    var elBanner = this.getLayoutElement('-banner-humax-apps');
                    var ctrlHumax = new COnDemandListControl(doc.createElement("div"), '-humax-app-player -player');
                    var ctrlTvApps = new CTvAppsListControl(doc.createElement("div"));
                    var APIPortal = this._requestorAPI;
                    var __fnMakeBorder = function (aClassName?) {
                        var el = doc.createElement("div");
                        if (aClassName) {
                            el.classList.add(aClassName);
                        }
                        return function (aText:string) {
                            el.innerHTML = aText;
                            return el;
                        };
                    };
                    var setGridItemTitle = __fnMakeBorder();
                    var setListItemTitle = __fnMakeBorder();
                    var setListItemBorderToMove = __fnMakeBorder(CTvAppsListControl.CLASS_MODE.MOVE);

                    var createInternetConnectionDlg = () => {
                        this.reqCreateLayer("modal", () => {
                            this.createModalDialog("ctrl-popup", [{
                                title: hx.l("LOC_TVAPPS_NETWORK_ERROR_ID"),
                                image: "images/614_Help_Promotion_TA01.png",
                                desc: hx.l("LOC_CONNECT_TO_INTERNET_DESC_ID")
                            }]);
                        });
                    };
                    var showPPTCDialog = (agreeCallback, disagreeCallback) => {
                        var currentPP = this._privacyPolicy;
                        var currentTC = this._termsAndConditions;
                        this._privacyPolicy = true;
                        this._termsAndConditions = true;
                        APIPortal.getPrivacyPolicy(
                            (aHTML:string) => {
                                var privacyPolicy = aHTML;
                                APIPortal.getTermsAndConditions((aHTML:string) => {
                                    var termsAndConditions = aHTML;
                                    this.reqCreateLayer("modal-no-back", () => {
                                        var modal = new CSliderWithButtonModalDialog(
                                            this.getLayoutElement("-ctrl-modal"),
                                            [hx.l('LOC_PRIVACY_POLICY_ID'), hx.l('LOC_TERMS_AND_CONDITIONS_ID')],
                                            [privacyPolicy, termsAndConditions],
                                            [{
                                                text: hx.l('LOC_AGREE_PRIVACY_POLICY_ID'),
                                                fn: (checked:boolean) => {
                                                    this._privacyPolicy = checked;
                                                }
                                            }, {
                                                text: hx.l('LOC_AGREE_TERMS_AND_CONDITIONS_ID'),
                                                fn: (checked:boolean) => {
                                                    this._termsAndConditions = checked;
                                                }
                                            }],
                                            [{
                                                text: hx.l('LOC_DONE_ID'),
                                                fn: () => {
                                                    APIPortal.setAcceptPrivacyTerms(this._privacyPolicy, this._termsAndConditions, (aSuccess: boolean) => {
                                                        if (this.getLayerCount()) {
                                                            this.removeLayer();
                                                        }
                                                        if (this._privacyPolicy) {
                                                            agreeCallback();
                                                        } else {
                                                            disagreeCallback();
                                                        }
                                                        if (!this._termsAndConditions) {
                                                            if (currentTC !== this._termsAndConditions) {
                                                                elTop.removeChild(elLogo);
                                                                elLogo.removeChild(elImgFVC);
                                                                elImgFVC.classList.remove("-logo-fvc");

                                                                ctrlOnDemand.setListData([]);
                                                                ctrlGroup.draw();
                                                            }
                                                        } else {
                                                            if (currentTC !== this._termsAndConditions) {
                                                                elImgFVC.classList.add("-logo-fvc");
                                                                elLogo.appendChild(elImgFVC);
                                                                elTop.appendChild(elLogo);

                                                                APIPortal.getModelPlayers((aModelPlayers:ifPortalModelApps[]) => {
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
                                            }],
                                            {
                                                doExitLayer : (): boolean=>{
                                                    this._privacyPolicy = currentPP;
                                                    this._termsAndConditions = currentTC;
                                                    if (this.getLayerCount()) {
                                                        this.removeLayer();
                                                        return true;
                                                    }
                                                    return false;
                                                }
                                            }
                                        );
                                        this.addControl(modal);
                                    });
                                }, (aHTML:string) => {
                                    hx.log("debug", "fail to get termsAndConditions");
                                    //APIOperation.exit();
                                });
                            },
                            (aHTML:string) => {
                                //TODO: expected behavior: notify & exit
                                hx.log("debug", "fail to get privacyPolicy");
                                APIOperation.exit();
                                //throw aHTML;
                            }
                        );
                    };
                    var showDisagreeConfirmMsg = () => {
                        var confirmMsg = hx.l('LOC_CANNOT_USE_SOME_SERVICE_IN_PORTAL_MSG_ID');
                        this.reqCreateLayer("modal", () => {
                            var confirmModal = new __SLIDER__.CModalButtonControl(
                                this.getLayoutElement("-modal-confirm"),
                                confirmMsg, [{
                                    text: hx.l('LOC_OK_ID'),
                                    fn: function() { return true; }
                                }]
                            )
                            confirmModal.onSelected = () => {
                                this.removeLayer();
                            }
                            confirmModal.addClass('-three-line');
                            this.addControl(confirmModal);
                        }, false);
                    }

                    var holder = {
                        profileItemSelected: (aKey:any, aItem:ifPortalModelApps, aEl:HTMLElement) => {
                            //XXX: just considered info
                            this.reqCreateLayer("info", () => {
                                var ctrlOndemandInfo = new COnDemandInfo(this.getLayoutElement("-ondemand-info"), this._requestorAPI);
                                var optionMenuProvider: ifOptionMenuContext[][] = [
                                [{
                                    title: hx.l("LOC_TITLEDELETE_ID"),
                                    fn: () => {
                                        this.reqCreateLayer("modal", () => {
                                            var modal = new __SLIDER__.CModalButtonControl(
                                                this.getLayoutElement("-modal-selective"),
                                                hx.l('LOC_DO_YOU_AGREE_DELETE_COOKIES_ID'), [{
                                                    text: hx.l('LOC_YES_ID'),
                                                    fn: function() { return true; }
                                                }, {
                                                    text: hx.l('LOC_NO_ID'),
                                                    fn: function() { return false; }
                                                }]);
                                            modal.onSelected = (isDelete: boolean) => {
                                                this.removeLayer();
                                                if (isDelete === true) {
                                                    this._requestorOperation.deleteCookies();
                                                    setTimeout(() => {
                                                        this.reqCreateLayer("modal", () => {
                                                            var secondModal = new __SLIDER__.CModalButtonControl(
                                                                this.getLayoutElement("-modal-confirm"),
                                                                hx.l('LOC_COOKIES_HAVE_BEEN_DELETED_ID'), [{
                                                                    text: hx.l('LOC_OK_ID'),
                                                                    fn: function() { return true; }
                                                                }]
                                                            )
                                                            secondModal.onSelected = () => {
                                                                this.removeLayer();
                                                            }
                                                            this.addControl(secondModal);
                                                        });
                                                    }, 300);
                                                }
                                            };
                                            this.addControl(modal);
                                        });
                                    }
                                }
                                ]];
                                ctrlOndemandInfo.setOptionMenu(hx.l("LOC_TITLEDELETE_ID"), optionMenuProvider[0]);
                                ctrlOndemandInfo.setOptionMenuProvider(optionMenuProvider);
                                ctrlOndemandInfo.setActiveOptionMenu(true);
                                this.addControl(ctrlOndemandInfo);
                            });
                        },
                        listItemSelected: (aKey:any, aItem:ifPortalModelApps, aEl:HTMLElement) => {
                            if(hx.config.netflix && aItem.name === 'Netflix'){
                                hx.il.launchApp("Netflix", aItem.appid, undefined, this, __NETFLIX__.START_TYPE.DEDICATED_ON_SCREEN_ICON);
                                return;
                            }
                            if (!this._privacyPolicy) {
                                //TODO: Pop up check dialog
                                showPPTCDialog(() => {
                                    if (!APIPortal.launchApp(aItem, aKey === 0)) {
                                        createInternetConnectionDlg();
                                    }
                                }, () => {
                                    setTimeout(() => {
                                        showDisagreeConfirmMsg();
                                    }, 300);
                                });
                            } else {
                                if (!APIPortal.launchApp(aItem, aKey === 0)) {
                                    createInternetConnectionDlg();
                                }
                            }
                        },
                        playerItemSelected: (aKey:any, aItem:ifPortalModelApps, aEl:HTMLElement) => {
                            if(hx.config.netflix && aItem.name === 'Netflix'){
                                hx.il.launchApp("Netflix", aItem.appid, undefined, this, __NETFLIX__.START_TYPE.DEDICATED_ON_SCREEN_ICON);
                                return;
                            }
                            var isPromoted = aItem.PlayerType ? ((aItem.PlayerType.indexOf('/promoted_players') !== -1) || (aItem.PlayerType.indexOf('/other_players') !== -1)) : false;
                            if (isPromoted) { // FVC Player
                                if (!APIPortal.launchApp(aItem, false, true)) {
                                    createInternetConnectionDlg();
                                }
                            } else { // HUMAX Player
                                if (!this._privacyPolicy) {
                                    //TODO: Pop up check dialog
                                    showPPTCDialog(() => {
                                        if (!APIPortal.launchApp(aItem, false, false)) {
                                            createInternetConnectionDlg();
                                        }
                                    }, () => {
                                        setTimeout(() => {
                                            showDisagreeConfirmMsg();
                                        }, 300);
                                    });
                                } else {
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
                        listFocusChanged: (aKey:any, aItem:ifPortalModelApps, aEl:HTMLElement) => {
                            aEl.appendChild(setListItemTitle(aItem.name));
                            aEl.appendChild(setListItemBorderToMove(""));
                        },
                        listDataItemOrderChanged: (() => {
                            var movedList = null;
                            return (aPosPrev:number, aPosNew:number, aDataProvider:__CTRL_BASE__.CListDataProvider) => {
                                if (!movedList) {
                                    movedList = _.cloneDeep(aDataProvider._listData);
                                } else if (movedList.length !== aDataProvider.getLength()) {
                                    throw "you may need to reload";
                                }
                                if (aPosPrev > aPosNew) {
                                    var item = movedList.splice(aPosPrev, 1);
                                    if (item && item[0]) {
                                        movedList.splice(aPosNew, 0, item[0]);
                                    } else {
                                        throw "something wrong";
                                    }
                                } else {
                                    var item = movedList.splice(aPosPrev, 1);
                                    if (item && item[0]) {
                                        movedList.splice(aPosNew, 0, item[0]);
                                    } else {
                                        throw "something wrong";
                                    }
                                }
                                APIPortal.setTvAppsArrangement(_.pluck(movedList, "appid"));
                            }
                        })(),
                        groupFocusChanged: (aOld: HTMLElement, aNew: HTMLElement) => {
                            if (!aNew.classList.contains('-profile')) {
                                (<COnDemandListControl>ctrlGroup.getFocusedChild()).initFocusedElement();
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
                    ctrlGroup.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EVertical);
                    //ctrlGroup.setOwnedChildControls([ctrlProfile, ctrlOnDemand, ctrlTvApps]);
                    ctrlGroup.setOwnedChildControls([ctrlProfile, ctrlOnDemand, ctrlHumax, ctrlTvApps]);
                    this.addControl(ctrlGroup);

                    var fnLoad = (aLoginData) => {
                        var fnLoginAfter = () => {
                            ctrlProfile.setListData([
                                aLoginData
                            ]);
                            APIPortal.getModelPlayers((aModelPlayers:ifPortalModelApps[]) => {
                                var modelOnDemand = [];
                                var modelHumax = [];
                                aModelPlayers.forEach(function (app) {
                                    if (app.PlayerType === '/promoted_players' || app.PlayerType === '/other_players') {
                                        modelOnDemand.push(app);
                                    } else {
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
                                    } else {
                                        ctrlHumax.initFocusedElement();
                                    }
                                }
                                hx.log("debug", "players draw");
                            });
                            APIPortal.getModelApps((aModelApps:ifPortalModelApps[], aIsUpdate:boolean, aIndex:number) => {
                                if (!aIsUpdate) {
                                    ctrlTvApps.setListData(aModelApps);
                                } else {
                                    ctrlTvApps.insertItems(aIndex, aModelApps);
                                }
                                this.setOptionMenu("LOC_MOVE_01_ID", [{
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
                        hx.log('error','########## AcceptPrivacy : '+aLoginData.AcceptPrivacy + ', AcceptTerms : '+aLoginData.AcceptTerms);
                        if ((aLoginData.AcceptPrivacy==false) || (aLoginData.AcceptTerms==false)) {
                            //if(APIOperation.isNeedTerms()) {
                            if(true) { // 전문을 항상 출력하도록 변경
                                APIPortal.getPrivacyPolicy(
                                    (aHTML:string) => {
                                        var privacyPolicy = aHTML;
                                        APIPortal.getTermsAndConditions((aHTML:string) => {
                                            var termsAndConditions = aHTML;
                                            this.reqCreateLayer("modal-no-back", () => {
                                                var modal = new CSliderWithButtonModalDialog(
                                                    this.getLayoutElement("-ctrl-modal"),
                                                    [hx.l('LOC_PRIVACY_POLICY_ID'), hx.l('LOC_TERMS_AND_CONDITIONS_ID')],
                                                    [privacyPolicy, termsAndConditions],
                                                    [{
                                                        text: hx.l('LOC_AGREE_PRIVACY_POLICY_ID'),
                                                        fn: (checked:boolean) => {
                                                            this._privacyPolicy = checked;
                                                        }
                                                    }, {
                                                        text: hx.l('LOC_AGREE_TERMS_AND_CONDITIONS_ID'),
                                                        fn: (checked:boolean) => {
                                                            this._termsAndConditions = checked;
                                                        }
                                                    }],
                                                    [{
                                                        text: hx.l('LOC_DONE_ID'),
                                                        fn: () => {
                                                            APIPortal.setAcceptPrivacyTerms(this._privacyPolicy, this._termsAndConditions, (aSuccess: boolean) => {
                                                                //if (this._privacyPolicy) {
                                                                if (this.getLayerCount()) {
                                                                    this.removeLayer();
                                                                }
                                                                fnLoginAfter();
                                                                //} else {
                                                                //    APIOperation.exit();
                                                                //}
                                                                if (this._termsAndConditions) {
                                                                    elImgFVC.classList.add("-logo-fvc");
                                                                    elLogo.appendChild(elImgFVC);
                                                                    elTop.appendChild(elLogo);
                                                                }
                                                            });
                                                            return true;
                                                        }
                                                    }],
                                                    {
                                                        doExitLayer : (): boolean =>{
                                                            APIOperation.exit();
                                                            return false;
                                                        }
                                                    }
                                                );
                                                this.addControl(modal);
                                            });
                                        }, (aHTML:string) => {
                                            hx.log("debug", "fail to get termsAndConditions");
                                            APIOperation.exit();
                                        });
                                    },
                                    (aHTML:string) => {
                                        //TODO: expected behavior: notify & exit
                                        hx.log("debug", "fail to get privacyPolicy");
                                        APIOperation.exit();
                                        //throw aHTML;
                                    }
                                );
                            } else {
                                if (!aLoginData.AcceptTerms) {
                                    this.reqCreateLayer("modal", () => {
                                        var modal = new __SLIDER__.CModalButtonControl(
                                            this.getLayoutElement("-modal-selective"),
                                            hx.l('LOC_ONDEMAND_AGREE_TERMS_AND_CONDITIONS_ID'), [{
                                                text: hx.l('LOC_AGREE_ID'),
                                                fn: function() { return true; }
                                            }, {
                                                text: hx.l('LOC_DISAGREE_ID'),
                                                fn: function() { return false; }
                                            }]);
                                        modal.onSelected = (isAccept: boolean) => {
                                            this._termsAndConditions = isAccept;
                                            if (this._termsAndConditions) {
                                                elImgFVC.classList.add("-logo-fvc");
                                                elLogo.appendChild(elImgFVC);
                                                elTop.appendChild(elLogo);
                                            }
                                            APIPortal.setAcceptTerms(isAccept, (aSuccess: boolean) => {
                                                this.removeLayer();
                                                if (!aLoginData.AcceptPrivacy) {
                                                    setTimeout(() => {
                                                        this.reqCreateLayer("modal", () => {
                                                            var secondModal = new __SLIDER__.CModalButtonControl(
                                                                this.getLayoutElement("-modal-selective"),
                                                                hx.l('LOC_ONDEMAND_AGREE_PRIVACY_POLICY_ID'), [{
                                                                    text: hx.l('LOC_AGREE_ID'),
                                                                    fn: function() { return true; }
                                                                }, {
                                                                    text: hx.l('LOC_DISAGREE_ID'),
                                                                    fn: function() { return false; }
                                                                }]
                                                            );
                                                            secondModal.onSelected = (isAccept: boolean) => {
                                                                this._privacyPolicy = isAccept;
                                                                this.removeLayer();
                                                                APIPortal.setAcceptPrivacy(this._privacyPolicy, (aSuccess:boolean) => {
                                                                    hx.log("debug", "setAcceptPrivacy true return: " + aSuccess);
                                                                    if (this._privacyPolicy) {
                                                                        fnLoginAfter();
                                                                    } else {
                                                                        APIOperation.exit();
                                                                    }
                                                                });
                                                            }
                                                            this.addControl(secondModal);
                                                        });
                                                    }, 300);
                                                } else {
                                                    fnLoginAfter();
                                                }
                                            });
                                            if (this._termsAndConditions) {
                                                elImgFVC.classList.add("-logo-fvc");
                                                elLogo.appendChild(elImgFVC);
                                                elTop.appendChild(elLogo);
                                            }
                                        };
                                        this.addControl(modal);
                                    });
                                } else if (!aLoginData.AcceptPrivacy) {
                                    this.reqCreateLayer("modal", () => {
                                        var secondModal = new __SLIDER__.CModalButtonControl(
                                            this.getLayoutElement("-modal-selective"),
                                            hx.l('LOC_ONDEMAND_AGREE_PRIVACY_POLICY_ID'), [{
                                                text: hx.l('LOC_AGREE_ID'),
                                                fn: function() { return true; }
                                            }, {
                                                text: hx.l('LOC_DISAGREE_ID'),
                                                fn: function() { return false; }
                                            }]
                                        );
                                        secondModal.onSelected = (isAccept: boolean) => {
                                            this._privacyPolicy = isAccept;
                                            this.removeLayer();
                                            APIPortal.setAcceptPrivacy(this._privacyPolicy, (aSuccess:boolean) => {
                                                hx.log("debug", "setAcceptPrivacy true return: " + aSuccess);
                                                if (this._privacyPolicy) {
                                                    fnLoginAfter();
                                                } else {
                                                    APIOperation.exit();
                                                }
                                            });
                                            if (this._termsAndConditions) {
                                                elImgFVC.classList.add("-logo-fvc");
                                                elLogo.appendChild(elImgFVC);
                                                elTop.appendChild(elLogo);
                                            }
                                        }
                                        this.addControl(secondModal);
                                    });
                                }
                            }
                        } else{
                            elImgFVC.classList.add("-logo-fvc");
                            elLogo.appendChild(elImgFVC);
                            elTop.appendChild(elLogo);
                            fnLoginAfter();
                        }
                    }
                    var fnLoadDefault = () => {
                        ctrlProfile.setListData([{}]);
                        ctrlOnDemand.setListData([]);
                        ctrlHumax.setListData([]);
                        APIPortal.getModelApps((aModelApps:ifPortalModelApps[], aIsUpdate:boolean, aIndex:number) => {
                            if (!aIsUpdate) {
                                ctrlTvApps.setListData(aModelApps);
                            } else {
                                ctrlTvApps.insertItems(aIndex, aModelApps);
                            }
                            ctrlGroup.draw();
                            hx.log("debug", "tv-apps draw");
                        });
                        ctrlGroup.draw();
                        elBanner.classList.add('-show');
                        throw "login fail";
                    };
                    var fnStartOnDemand = () => {
                        hx.log('error','fnStartOnDemand isLogged : '+APIPortal.isLogged());
                        APIPortal.login((aData) => {
                            fnLoad(aData);
                        }, (aData) => {
                            fnLoad(aData);
                            //fnLoadDefault();
                        });
                    }


                    if (APIOperation.isNeedHelp()) {
                        this.reqCreateLayer("modal", () => {
                            this.createModalDialog("ctrl-help", [{
                                    title: hx.l("LOC_EXPERIENCE_FREE_ID"),
                                    image: "images/614_Help_Promotion_TA01.png",
                                    desc: hx.l("LOC_HELP_ONDEMAND_STEP_1_SCENE_1_TXT_ID")
                                }, {
                                    title: hx.l("LOC_EXPLORE_THE_VARIETY_OF_TV_APPS_ID"),
                                    image: "images/614_Help_Promotion_TA02.png",
                                    desc: hx.l("LOC_ON_DEMAND_HELP_02_DESC_ID")
                                }],
                                APIOperation.setVisit,
                                fnStartOnDemand
                            );
                        });
                    } else {
                        fnStartOnDemand();
                    }
                }, () => {
                    var modal = new __SLIDER__.CModalDialogSildeControl(this.getLayoutElement("-ctrl-modal"), "ctrl-popup", [{
                        title: hx.l("LOC_TVAPPS_NETWORK_ERROR_ID"),
                        image: "images/614_Help_Promotion_TA01.png",
                        desc: hx.l("LOC_CONNECT_TO_INTERNET_DESC_ID")
                    }], 30);
                    modal.connectWhenReqClosed(APIOperation, "exit", APIOperation.exit);
                    this.addControl(modal);
                    modal.draw();
                }
            );
        });
        $aParent.append($self);
    }
    setDataRequestor(aRequestor: ifPortalRequestorAPI) {
        this._requestorAPI = aRequestor;
        return this;
    }
    setOperationRequester(aRequestor) {
        this._requestorOperation = aRequestor;
        return this;
    }
    reqCreateLayer(aName: string, aFnAfterTransition, isDim = true) {
        var dim = isDim ? "" : " -no-dim"
        this.createLayer({
            addClass: "-layer-"+aName.toLocaleLowerCase()+dim,
            transition: {
                newLayer: "fadeIn",
                fnAfterTransition: aFnAfterTransition
            }
        });
        return this;
    }

    createModalDialog(id: string, slideInfo: __SLIDER__.ifSliderModel[], fnAfter?, fnCloseCallback?) {
        var modal;
        if(fnCloseCallback){
            modal = new CSliderDialogWithExit(this.getLayoutElement("-ctrl-modal"), id, slideInfo,0,{
                doBackLayer : ()=>{
                    if (this.getLayerCount()) {
                        this.removeLayer();
                        if (fnCloseCallback) {
                            setTimeout(() => {
                                fnCloseCallback();
                            }, 300);

                        }
                    }
                }
            });
        }else{
            modal = new CSliderDialogWithExit(this.getLayoutElement("-ctrl-modal"), id, slideInfo,0);
        }

        var holder = {
            reqClose: ()=> {
                if (this.getLayerCount()) {
                    this.removeLayer();
                    if (fnCloseCallback) {
                        setTimeout(() => {
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
    }
    _doKeyBackLatent() {
        var handled = false;
        if (this.getLayerCount()) {
            this.removeLayer();
            handled = true;
        }
        return handled;
    }
}
class CProfileListControl extends __CTRL_BASE__.CListControl {
    constructor(aElement: HTMLElement) {
        super(aElement);
        this.addClass("-profile");
        this.setClassToGroupViewElement("-user-info");
        this.setItemWidth(60);
        this.setItemHeight(76);
        this.setOrientation(__CTRL_BASE__.TParamOrientation.EHorizontal);
        this.setDataDrawer((aKey: any, aItem: ifPortalLogin, aEl: HTMLElement): __CTRL_BASE__.TFocusInfo => {
            var elBorder = document.createElement("div");
            var elName = document.createElement("div");
            elBorder.appendChild(elName);
            aEl.appendChild(elBorder);
            aEl.classList.add("-user-info");
            return __CTRL_BASE__.TFocusInfo.KFocusAble;
        });
        this._doKeyRightLatent = function() { return true; };
        this._doKeyLeftLatent = function() { return true; };
    }
}
class COnDemandListControl extends __CTRL_BASE__.CListControl {
    constructor(aElement: HTMLElement, className: string) {
        super(aElement);
        var TFocusInfo = __CTRL_BASE__.TFocusInfo;
        this.setClassToGroupViewElement(className);
        this.setItemWidth(270);
        this.setItemHeight(146);
        this.setOrientation(__CTRL_BASE__.TParamOrientation.EHorizontal);
        this.setScrollScheme(__CTRL_BASE__.TParamScrollScheme.EByFixed, 270);
        this.setDataDrawer((aKey: any, aItem: ifPortalModelApps, aEl: HTMLElement): __CTRL_BASE__.TFocusInfo => {
            var ret = TFocusInfo.KFocusAble;
            aEl.classList.add('-unload');
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
                        if (aItem.PlayerType === '/promoted_players' || aItem.PlayerType === '/other_players') {
                            hx.svc.IP_EPG.getThumbnail(aItem.icons[index].Url, 260).done((url) => {
                                elImage.setAttribute("src", url);
                                aEl.classList.remove('-unload');
                            });
                        } else {
                            elImage.setAttribute("src", aItem.icons[index].Url);
                            aEl.classList.remove('-unload');
                        }

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
        (<any>this._dataControl)._doKeyRightLatent = function() {
            var item: ifPortalModelApps = this.getOwnedDataProvider().getItem(this.getFocusedElementIndex() + 1);
            return !(item && item.focusable);
        }
    }
    initFocusedElement() {
        var dataControl = this._dataControl;
        var focusedElementIndex = dataControl.getFocusedElementIndex();
        if (focusedElementIndex !== -1) {
            dataControl.setFocusedElementByIndex(focusedElementIndex, true);
        }
        return this;
    }
    setListData(aData: any[], aDataRolling?: boolean) {
        for (var i = aData.length; i < 5; i++) {
            aData[i] = {};
        }
        super.setListData(aData, aDataRolling);
    }
}
class CTvAppsListControl extends __CTRL_BASE__.CListControl {
    private _mode: eModeListControl = eModeListControl.NORMAL;

    static CLASS_MODE = {
        NORMAL: "",
        MOVE: "-mode-move",
        DELETE: "-mode-delete"
    };
    constructor(aElement: HTMLElement) {
        super(aElement);
        var TFocusInfo = __CTRL_BASE__.TFocusInfo;
        this.setClassToGroupViewElement("-tv-apps");
        this.setItemWidth(114);
        this.setItemHeight(104);
        this.setOrientation(__CTRL_BASE__.TParamOrientation.EHorizontal);
        this.setScrollScheme(__CTRL_BASE__.TParamScrollScheme.EByFixed, 114);
        this.setDataDrawer((aKey: any, aItem: ifPortalModelApps, aEl: HTMLElement): __CTRL_BASE__.TFocusInfo => {
            if (aItem.icons.length && aItem.icons[0].Url) {
                aEl.style.backgroundImage = "url(" + aItem.icons[3].Url + ")";
            }
            return TFocusInfo.KFocusAble;
        });
    }
    initFocusedElement() {
        var dataControl = this._dataControl;
        var focusedElementIndex = dataControl.getFocusedElementIndex();
        if (focusedElementIndex !== -1) {
            dataControl.setFocusedElementByIndex(focusedElementIndex, true);
        }
        return this;
    }
    //XXX: MOVE
    private _setMode(aMode: eModeListControl) {
        if (this._mode !== aMode) {
            this._dataControl.setModeMove(aMode === eModeListControl.MOVE);
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
            } else {
                if (className) {
                    el.classList.remove(className);
                }
            }
        }
        if (mode) {
            el.classList.add(mode);
        }
    }
    setModeMove() {
        if (this._element.classList.contains(CTvAppsListControl.CLASS_MODE[eModeListControl[eModeListControl.MOVE]])) {
            this._setMode(eModeListControl.NORMAL);
        } else {
            this._setMode(eModeListControl.MOVE);
        }
        return this;
    }
}
class COnDemandInfo extends __CTRL_BASE__.CGroupControl {
    protected _buttonOption: __BUTTON_PROMOTION__.CButtonPromotion = null;
    private optionMenuProvider: any;
    static CLASS = {
        TITLE: "-title",
        MENU: "-menu",
        VIEW: "-view",
        TEXT: "-text",
        SCROLLBAR: "-scroll-bar"
    };
    static MENU = {
        DELETE_COOKIES : hx.l('LOC_DELETE_COOKIES_ID')
    }
    constructor(aElement: HTMLElement, aAPI) {
        super(aElement);
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
        ctrlList.setOrientation(__CTRL_BASE__.TParamOrientation.EVertical);
        ctrlList.setScrollScheme(__CTRL_BASE__.TParamScrollScheme.EByItem);
        ctrlList._doKeyUpLatent = function() { return true; };
        ctrlList._doKeyDownLatent = function() { return true; };
        ctrlList.setDataDrawer((aKey: any, aItem: string, aEl: HTMLElement): __CTRL_BASE__.TFocusInfo => {
            var elText = document.createElement('div');
            elText.innerHTML = aItem;
            aEl.appendChild(elText);
            return __CTRL_BASE__.TFocusInfo.KFocusAble;
        });

        this.setOwnedChildControls([ctrlList, ctrlScroll]);
        this.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EHorizontal);

        this.draw();
        ctrlScroll.setHTML('A cookie is a small amount of data placed on your Product. A cookie is installed automatically when you use the Product and Services, but you can delete cookies by pressing the <+> button.');
    }
    setOptionMenuProvider(optionMenuProvider: ifOptionMenuContext[][]) {
        this.optionMenuProvider = optionMenuProvider;
    }
    setOptionMenu(aTitle: string, aContext: ifOptionMenuContext[]) {
        if (aContext.length === 1) {
            var elOptionButton = document.createElement("div");
            var optionButton = new __BUTTON_PROMOTION__.CButtonPromotion(elOptionButton);
            elOptionButton.classList.add("-button-option-menu");
            elOptionButton.innerText = aTitle;
            optionButton.setData(aContext);
            this._buttonOption = optionButton;
            this._element.appendChild(elOptionButton);
        } else {
            //TODO: need context menu
        }
        return this;
    }
    //changeOptionMenuProvider(aTitle: string, aContext: ifOptionMenuContext[]) {
    //    if (this._buttonOption !== null) {
    //        var elOptionButton = this._element.getElementsByClassName("-button-option-menu");
    //        (<any>elOptionButton[0]).innerText = aTitle;
    //        this._buttonOption.setData(aContext);
    //    }
    //}
    setActiveOptionMenu(aActive: boolean) {
        aActive ? this._buttonOption.showButton() : this._buttonOption.hideButton();
        return this;
    }
    _doKeyOptionLatent() {
        //TODO: start here
        var handled = false;
        var buttonOption = this._buttonOption;
        if (buttonOption.isShowing()) {
            var data: ifOptionMenuContext[] = buttonOption.getData();
            if (data) {
                if (data.length === 1) {
                    data[0].fn();
                }
                handled = true;
            }
        }
        return handled;
    }
}

interface ifDialogLayerHandle {
    doExitLayer? : () => boolean;
    doBackLayer? : () => void;
}

class CSliderDialogWithExit extends __SLIDER__.CModalDialogSildeControl{
    private modalCtrlFunc : ifDialogLayerHandle;
    constructor(aParent: HTMLElement, aID: string, aModel: __SLIDER__.ifSliderModel[], autoClose = 0,  layerFunc? : ifDialogLayerHandle) {
        super(aParent,aID,aModel,autoClose);
        this.modalCtrlFunc = layerFunc;
    }
    doKey(aKeyStr: string, aArgs?: any) {
        var ret = super.doKey(aKeyStr, aArgs);
        //close this layer
        if(aKeyStr==="Back"){
            if(this.modalCtrlFunc){
                this.modalCtrlFunc.doBackLayer();
            }
            return true;
        }
        return ret;
    }

}

class CSliderWithButtonModalDialog extends __SLIDER__.CModalDialogControl {
    private modalCtrlFunc : ifDialogLayerHandle;
    constructor(element: HTMLElement, titles:string[], contents: string[], checkboxInfos: __SLIDER__.ifDialogCheckBoxInfo[], modelInfo: __SLIDER__.ifDialogSelectiveButtonInfo[], layerFunc? : ifDialogLayerHandle) {
        super(element);
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
        ctrlPrivacyPolicyGroup.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EVertical);

        var elTCGroup = $('<div>');
        elTCGroup.addClass('-ctrl-terms-and-conditions');
        elTCGroup.append($('<div>').addClass('-title').text(titles[1]));
        var ctrlTermsAndConditionsGroup = new __CTRL_BASE__.CGroupControl(elTCGroup[0]);
        var ctrlTermsAndConditions = new __SCROLL_CONTROL__($("<div>"));
        var ctrlTCCheckBox = new __SLIDER__.CCheckBoxGroupControl(document.createElement('div'), checkboxInfos[1]);
        ctrlTermsAndConditionsGroup.setOwnedChildControls([ctrlTermsAndConditions, ctrlTCCheckBox]);
        ctrlTermsAndConditionsGroup.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EVertical);

        ctrlGroup.setOwnedChildControls([ctrlPrivacyPolicyGroup, ctrlTermsAndConditionsGroup]);
        ctrlGroup.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EHorizontal);

        var ctrlButtons = new __SLIDER__.CButtonGroupControl(document.createElement('div'), null, modelInfo);

        this.setOwnedChildControls([ctrlGroup, ctrlButtons]);
        this.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EVertical);

        ctrlPPCheckBox.onSelected = function(value) {
            console.log('ctrlCheckBox return value: ' + !!value);
        };
        ctrlTCCheckBox.onSelected = function(value) {
            console.log('ctrlCheckBox return value: ' + !!value);
        };
        ctrlButtons.onSelected = function(value) {
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
    doKey(aKeyStr: string, aArgs?: any) {
        var ret = super.doKey(aKeyStr, aArgs);
        if (aKeyStr === "Back") {
            var rootFocusCtrl = this.getFocusedChild()
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
        if(aKeyStr==="Escape"||aKeyStr==="Back"){
            if(this.modalCtrlFunc){
                ret = this.modalCtrlFunc.doExitLayer();
            }
        }
        return ret;
    }
    onMovableStateBefore(movable) {
        //movable ? this._arrows.left.show() : this._arrows.left.hide();
    }
    onMovableStateAfter(movable) {
        //movable ? this._arrows.right.show() : this._arrows.right.hide();
    }
}

export = CPageOnDemand;
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

