/**
 * Created by bglee on 2015-03-09.
 */
/// <reference path="../../def/by_controls.d.ts" />

import __CTRL_BASE__ = require('prismux/controls/ctrlbase');
import __BUTTON_PROMOTION__ = require("prismux/layout/prism.layout.button.promotion");

class CPage extends __CTRL_BASE__.CLayeredGroupControl {
    protected _buttonOption: __BUTTON_PROMOTION__.CButtonPromotion = null;
    constructor($aParent: JQuery) {
        super($aParent);
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
export = CPage;
