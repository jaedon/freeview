/**
 * Created by bglee on 2015-01-26.
 */
/// <reference path="../../def/sugar.d.ts"/>
/// <reference path="../../def/layout.d.ts" />

import __CTRL_BASE__ = require('prismux/controls/ctrlbase');
import __BUTTON_PROMOTION__ = require("prismux/layout/prism.layout.button.promotion");

declare var hx: ifSugar;
function log(aMesasge: string) {
    //hx.log("warning", "^[CButtonPromotionControl]");
    //hx.log("debug", aMesasge);
    //hx.log("warning", "$[CButtonPromotionControl]");
}

export class CButtonPromotionControl extends __CTRL_BASE__.CControl implements ifCButtonPromotion{
    private _fnCB: (aData) => void;
    private _dataVolatile: any = null;// {ccid,linkage}

    private _timer = null;

    protected _button: __BUTTON_PROMOTION__.CButtonPromotion = null;
    static ID = "button-watch-in-hd";
    static CLASS = "-button-watch-in-hd";

    constructor($aParent: JQuery, aTitle: string, aCBSelected: (aData) => void) {
        super($("<div>", {
            id: CButtonPromotionControl.ID
        }));
        var doc = document;
        var elOptionButton = this._element;
        var optionButton = new __BUTTON_PROMOTION__.CButtonPromotion(elOptionButton);
        //elOptionButton.classList.add(CButtonPromotionControl.CLASS);
        //optionButton.setData(aContext);
        var elOK = doc.createElement("div");
        elOK.innerText = aTitle;
        this._button = optionButton;
        this._fnCB = aCBSelected;
        this.$self().append(elOK);
        $aParent.append(this.$self());
    }
    doSelect($aFocusedElement): boolean {
        var button = this._button;
        var data = button.getData();
        var ret = false;
        if (button.isShowing() && data) {
            this._fnCB(data);
            this.hideButton();
            ret = true;
        }
        log("[doSelect] : " + ret);
        return ret;
    }
    showButton(aForceShowing?): boolean {
        var ret = false;
        ret = this._button.showButton(aForceShowing);
        clearTimeout(this._timer);
        this._timer = setTimeout(() => {
            log("[showButton] : " + "call hide by tiemr");
            this.hideButton();
        }, 10 * 1000);
        log("[showButton] : " + ret);
        return ret;
    }
    hideButton(aKeepHiding?: boolean): void {
        log("[hideButton] : " + aKeepHiding);
        clearTimeout(this._timer);
        this._button.hideButton(aKeepHiding);
        this._button.setData();
    }
    isShowing(): boolean {
        log("[isShowing] : ");
        return this._button.isShowing();
    }
    getElement(): HTMLElement {
        log("[getElement] : ");
        return this._button.getElement();
    }
    setData(aData: any = null): void {
        this._button.setData(aData);
        if (!aData) {
            this.hideButton();
        }
        log("[setData] : " + aData);
    }
    getData(): any {
        log("[getData] : ");
        return this._button.getData();
    }
    setVolatileData(aData: any = null) {
        log("[setVolatileData] : " + aData);
        this._dataVolatile = aData;
    }

    getVolatileData() {
        log("[getVolatileData]");
        return this._dataVolatile;
    }

    setFromVolatileData(): boolean {
        var ret = false;
        this.setData(this._dataVolatile);
        if (this._dataVolatile) {
            this._dataVolatile = null;
            ret = true;
        }
        log("[setFromVolatileData] : " + ret);
        return ret;
    }
    _doKeyBack() {
        var ret = false;
        if (this.isShowing()) {
            this.hideButton();
            ret = true;
        }
        return ret;
    }
    _doKeyEscape() {
        if (this.isShowing()) {
            this.hideButton();
        }
        return false;
    }
}
