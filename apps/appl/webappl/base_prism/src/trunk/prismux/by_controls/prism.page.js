/**
 * Created by bglee on 2015-03-09.
 */
/// <reference path="../../def/by_controls.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/controls/ctrlbase', "prismux/layout/prism.layout.button.promotion"], function (require, exports, __CTRL_BASE__, __BUTTON_PROMOTION__) {
    var CPage = (function (_super) {
        __extends(CPage, _super);
        function CPage($aParent) {
            _super.call(this, $aParent);
            this._buttonOption = null;
        }
        CPage.prototype.setOptionMenu = function (aTitle, aContext) {
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
        CPage.prototype.setActiveOptionMenu = function (aActive) {
            aActive ? this._buttonOption.showButton() : this._buttonOption.hideButton();
            return this;
        };
        CPage.prototype._doKeyOptionLatent = function () {
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
        return CPage;
    })(__CTRL_BASE__.CLayeredGroupControl);
    return CPage;
});
