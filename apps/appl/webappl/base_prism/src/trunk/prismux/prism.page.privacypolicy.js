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
define(["require", "exports", 'prismux/controls/ctrlbase', "prismux/by_controls/prism.slider", "prismux/by_controls/prism.page", "prismux/by_controls/prism.scroll"], function (require, exports, __CTRL_BASE__, __SLIDER__, __PAGE__, __SCROLL_CONTROL__) {
    var CPagePrivacyPolicy = (function (_super) {
        __extends(CPagePrivacyPolicy, _super);
        function CPagePrivacyPolicy($aParent, btnInfo) {
            var _this = this;
            _super.call(this, $("<div>", {
                "id": CPagePrivacyPolicy.ID,
                "class": "show"
            }));
            this._requestorAPI = null;
            this._requestorOperation = null;
            this._privacyPolicyAccepted = true;
            this._termsAndConditionsAccepted = true;
            var $self = this.$self();
            this.reqCreateLayer('main', function () {
                var APIOperation = _this._requestorOperation;
                APIOperation.getNetworkAvailable(function () {
                    //TODO: network available
                    var APIPortal = _this._requestorAPI;
                    APIPortal.getPrivacyPolicy(function (aHTML, aAccepted) {
                        var privacyPolicyHtml = aHTML;
                        _this._privacyPolicyAccepted = aAccepted;
                        APIPortal.getTermsAndConditions(function (aHTML, aAccepted) {
                            var termsAndConditionsHtml = aHTML;
                            _this._termsAndConditionsAccepted = aAccepted;
                            var modelInfos = [];
                            for (var i = 0; i < btnInfo.btnName.length; i++) {
                                modelInfos.push({
                                    text: btnInfo.btnName[i],
                                    fn: btnInfo.action[i]
                                });
                            }
                            var ctrlGroup = new CScrollGroupControl(_this.getLayoutElement('-ctrl-scroll-group'), [privacyPolicyHtml, termsAndConditionsHtml], [{
                                text: hx.l('LOC_AGREE_PRIVACY_POLICY_ID'),
                                checked: _this._privacyPolicyAccepted,
                                fn: function (checked) {
                                    _this._privacyPolicyAccepted = checked;
                                }
                            }, {
                                text: hx.l('LOC_AGREE_TERMS_AND_CONDITIONS_ID'),
                                checked: _this._termsAndConditionsAccepted,
                                fn: function (checked) {
                                    _this._termsAndConditionsAccepted = checked;
                                }
                            }], modelInfos, btnInfo.focusedIndex);
                            _this.addControl(ctrlGroup);
                        }, function (aHTML) {
                            //TODO: getTermsAndConditions fail
                            APIOperation.goNextPage();
                        });
                    }, function (aHTML) {
                        //TODO: getPrivacyPolicy fail
                        APIOperation.goNextPage();
                    });
                }, function () {
                    //TODO: network not available
                    hx.msg('response', {
                        text: hx.l('LOC_CONNECTION_FAILED_ID'),
                        auto_close: true,
                        close_time: 3000
                    });
                    APIOperation.goNextPage();
                });
            });
            $aParent.append($self);
        }
        CPagePrivacyPolicy.prototype.getCheckedValues = function () {
            return [this._privacyPolicyAccepted, this._termsAndConditionsAccepted];
        };
        CPagePrivacyPolicy.prototype.setDataRequestor = function (aRequestor) {
            this._requestorAPI = aRequestor;
            return this;
        };
        CPagePrivacyPolicy.prototype.setOperationRequester = function (aRequestor) {
            this._requestorOperation = aRequestor;
            return this;
        };
        CPagePrivacyPolicy.prototype.reqCreateLayer = function (aName, aFnAfterTransition) {
            this.createLayer({
                addClass: "-layer-" + aName.toLocaleLowerCase(),
                transition: {
                    newLayer: "fadeIn",
                    fnAfterTransition: aFnAfterTransition
                }
            });
            return this;
        };
        CPagePrivacyPolicy.prototype._doKeyBackLatent = function () {
            var handled = false;
            if (this.getLayerCount()) {
                this.removeLayer();
                handled = false;
            }
            return handled;
        };
        CPagePrivacyPolicy.ID = "privacy-policy-terms-and-conditions";
        return CPagePrivacyPolicy;
    })(__PAGE__);
    var CScrollGroupControl = (function (_super) {
        __extends(CScrollGroupControl, _super);
        function CScrollGroupControl(element, contents, checkboxInfos, modelInfo, focusedIndex) {
            _super.call(this, element);
            this._keyFlooding = ['Back', 'Escape', 'Power'];
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
            ctrlPrivacyPolicyGroup.setKeyBuilder(1 /* EVertical */);
            var elTCGroup = $('<div>');
            elTCGroup.addClass('-ctrl-terms-and-conditions');
            elTCGroup.append($('<div>').addClass('-title').text(hx.l('LOC_TERMS_AND_CONDITIONS_ID')));
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
            if (!checkboxInfos[0].checked) {
                ctrlPPCheckBox._children[0].doSelect();
            }
            if (!checkboxInfos[1].checked) {
                ctrlTCCheckBox._children[0].doSelect();
            }
            this.draw();
            ctrlPrivacyPolicy.setHTML(contents[0]);
            ctrlTermsAndConditions.setHTML(contents[1]);
            ctrlButtons.setFocusedElementByIndex(focusedIndex);
            if (modelInfo.length === 2) {
                ctrlButtons.$self().css('left', '312px');
            }
            else {
                ctrlButtons.$self().css('left', '445px');
            }
            ctrlGroup.setFocusedChild(ctrlPrivacyPolicyGroup);
            ctrlPrivacyPolicyGroup.setFocusedChild(ctrlPPCheckBox);
            ctrlTermsAndConditionsGroup.setFocusedChild(ctrlTCCheckBox);
            this.setFocusedControl(ctrlButtons);
        }
        CScrollGroupControl.prototype.doKey = function (aKeyStr, aArgs) {
            _super.prototype.doKey.call(this, aKeyStr, aArgs);
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
        };
        CScrollGroupControl.prototype.onMovableStateBefore = function (movable) {
            //movable ? this._arrows.left.show() : this._arrows.left.hide();
        };
        CScrollGroupControl.prototype.onMovableStateAfter = function (movable) {
            //movable ? this._arrows.right.show() : this._arrows.right.hide();
        };
        return CScrollGroupControl;
    })(__CTRL_BASE__.CGroupControl);
    return CPagePrivacyPolicy;
});
