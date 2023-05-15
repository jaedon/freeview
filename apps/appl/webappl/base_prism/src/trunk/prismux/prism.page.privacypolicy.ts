/**
 * Created by bglee on 2014-10-29.
 */
///<reference path="../def/sugar.d.ts"/>
///<reference path="../def/prism.d.ts"/>

import __CTRL_BASE__ = require('prismux/controls/ctrlbase');
import __SLIDER__ = require("prismux/by_controls/prism.slider");
import __PAGE__ = require("prismux/by_controls/prism.page");
import __SCROLL_CONTROL__ = require("prismux/by_controls/prism.scroll");

declare var hx: ifSugar;

interface ifAPIOperation {
    exit(): void;
    getNetworkAvailable(fnSuccess, fnFail): boolean;
    goNextPage(): void;
}
interface ifButtonInfo {
    focusedIndex;
    btnName;
    action;
}

class CPagePrivacyPolicy extends __PAGE__ {
    private _requestorAPI: ifPortalRequestorAPI = null;
    private _requestorOperation: ifAPIOperation = null;
    static ID = "privacy-policy-terms-and-conditions";
    private _privacyPolicyAccepted: boolean = true;
    private _termsAndConditionsAccepted: boolean = true;

    constructor($aParent: JQuery, btnInfo: ifButtonInfo) {
        super($("<div>", {
            "id": CPagePrivacyPolicy.ID,
            "class": "show"
        }));
        var $self = this.$self();
        this.reqCreateLayer('main', () => {
            var APIOperation = this._requestorOperation;
            APIOperation.getNetworkAvailable(
                () => {
                    //TODO: network available
                    var APIPortal = this._requestorAPI;
                    APIPortal.getPrivacyPolicy((aHTML: string, aAccepted: boolean) => {
                        var privacyPolicyHtml = aHTML;
                        this._privacyPolicyAccepted = aAccepted;
                        APIPortal.getTermsAndConditions((aHTML: string, aAccepted: boolean) => {
                            var termsAndConditionsHtml = aHTML;
                            this._termsAndConditionsAccepted = aAccepted;
                            var modelInfos = [];
                            for(var i = 0; i < btnInfo.btnName.length ; i++) {
                                modelInfos.push(
                                    {
                                        text: btnInfo.btnName[i],
                                        fn: btnInfo.action[i]
                                    }
                                );
                            }
                            var ctrlGroup = new CScrollGroupControl(
                                this.getLayoutElement('-ctrl-scroll-group'),
                                [privacyPolicyHtml, termsAndConditionsHtml],
                                [{
                                    text: hx.l('LOC_AGREE_PRIVACY_POLICY_ID'),
                                    checked : this._privacyPolicyAccepted,
                                    fn: (checked: boolean) => {
                                        this._privacyPolicyAccepted = checked;
                                    }
                                }, {
                                    text: hx.l('LOC_AGREE_TERMS_AND_CONDITIONS_ID'),
                                    checked : this._termsAndConditionsAccepted,
                                    fn: (checked: boolean) => {
                                        this._termsAndConditionsAccepted = checked;
                                    }
                                }],
                                modelInfos,
                                btnInfo.focusedIndex
                            );
                            this.addControl(ctrlGroup);
                        }, (aHTML: string) => {
                            //TODO: getTermsAndConditions fail
                            APIOperation.goNextPage();
                        });
                    }, (aHTML: string) => {
                        //TODO: getPrivacyPolicy fail
                        APIOperation.goNextPage();
                    });
                },
                () => {
                    //TODO: network not available
                    hx.msg('response', {
                        text:  hx.l('LOC_CONNECTION_FAILED_ID'),
                        auto_close: true,
                        close_time: 3000
                    });
                    APIOperation.goNextPage();
                }
            );
        });
        $aParent.append($self);
    }
    getCheckedValues() : any {
        return [this._privacyPolicyAccepted, this._termsAndConditionsAccepted];
    }
    setDataRequestor(aRequestor: ifPortalRequestorAPI) {
        this._requestorAPI = aRequestor;
        return this;
    }
    setOperationRequester(aRequestor) {
        this._requestorOperation = aRequestor;
        return this;
    }
    reqCreateLayer(aName: string, aFnAfterTransition) {
        this.createLayer({
            addClass: "-layer-"+aName.toLocaleLowerCase(),
            transition: {
                newLayer: "fadeIn",
                fnAfterTransition: aFnAfterTransition
            }
        });
        return this;
    }
    _doKeyBackLatent() {
        var handled = false;
        if (this.getLayerCount()) {
            this.removeLayer();
            handled = false;
        }
        return handled;
    }
}

class CScrollGroupControl extends __CTRL_BASE__.CGroupControl {
    private _keyFlooding = ['Back', 'Escape', 'Power'];
    constructor(element: HTMLElement, contents, checkboxInfos, modelInfo, focusedIndex) {
        super(element);
        //this.setId('ctrl-scroll-group');

        var ctrlGroup = new __CTRL_BASE__.CGroupControl(document.createElement('div'));
        ctrlGroup.setId('ctrl-scroll-group');
        var elPPGroup = $('<div>');
        elPPGroup.addClass('-ctrl-privacy-policy');
        elPPGroup.append($('<div>').addClass('-title').text(hx.l('LOC_PRIVACY_POLICY_ID')));
        var ctrlPrivacyPolicyGroup = new __CTRL_BASE__.CGroupControl(elPPGroup[0]);
        var ctrlPrivacyPolicy = new __SCROLL_CONTROL__($("<div>"));
        var ctrlPPCheckBox = new __SLIDER__.CCheckBoxGroupControl(document.createElement('div'), checkboxInfos[0]);
        ctrlPrivacyPolicyGroup.setOwnedChildControls([ctrlPrivacyPolicy, ctrlPPCheckBox]);
        ctrlPrivacyPolicyGroup.setKeyBuilder(__CTRL_BASE__.TParamOrientation.EVertical);

        var elTCGroup = $('<div>');
        elTCGroup.addClass('-ctrl-terms-and-conditions');
        elTCGroup.append($('<div>').addClass('-title').text(hx.l('LOC_TERMS_AND_CONDITIONS_ID')));
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
        if(!checkboxInfos[0].checked){
            ctrlPPCheckBox._children[0].doSelect();
        }
        if(!checkboxInfos[1].checked){
            ctrlTCCheckBox._children[0].doSelect();
        }
        this.draw();
        ctrlPrivacyPolicy.setHTML(contents[0]);
        ctrlTermsAndConditions.setHTML(contents[1]);

        ctrlButtons.setFocusedElementByIndex(focusedIndex);
        if (modelInfo.length === 2) {
            ctrlButtons.$self().css('left', '312px');
        } else {
            ctrlButtons.$self().css('left', '445px');
        }

        ctrlGroup.setFocusedChild(ctrlPrivacyPolicyGroup);
        ctrlPrivacyPolicyGroup.setFocusedChild(ctrlPPCheckBox);
        ctrlTermsAndConditionsGroup.setFocusedChild(ctrlTCCheckBox);
        this.setFocusedControl(ctrlButtons);
    }
    doKey(aKeyStr: string, aArgs?: any) {
        super.doKey(aKeyStr, aArgs);
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
        return this._keyFlooding.indexOf(aKeyStr) === -1;
    }
    onMovableStateBefore(movable) {
        //movable ? this._arrows.left.show() : this._arrows.left.hide();
    }
    onMovableStateAfter(movable) {
        //movable ? this._arrows.right.show() : this._arrows.right.hide();
    }
}

export = CPagePrivacyPolicy;