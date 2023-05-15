/**
 * Created by bglee on 2015-01-26.
 */
/// <reference path="../../def/sugar.d.ts"/>
/// <reference path="../../def/layout.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/controls/ctrlbase', "prismux/layout/prism.layout.button.promotion"], function (require, exports, __CTRL_BASE__, __BUTTON_PROMOTION__) {
    function log(aMesasge) {
        //hx.log("warning", "^[CButtonPromotionControl]");
        //hx.log("debug", aMesasge);
        //hx.log("warning", "$[CButtonPromotionControl]");
    }
    var CButtonPromotionControl = (function (_super) {
        __extends(CButtonPromotionControl, _super);
        function CButtonPromotionControl($aParent, aTitle, aCBSelected) {
            _super.call(this, $("<div>", {
                id: CButtonPromotionControl.ID
            }));
            this._dataVolatile = null; // {ccid,linkage}
            this._timer = null;
            this._button = null;
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
        CButtonPromotionControl.prototype.doSelect = function ($aFocusedElement) {
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
        };
        CButtonPromotionControl.prototype.showButton = function (aForceShowing) {
            var _this = this;
            var ret = false;
            ret = this._button.showButton(aForceShowing);
            clearTimeout(this._timer);
            this._timer = setTimeout(function () {
                log("[showButton] : " + "call hide by tiemr");
                _this.hideButton();
            }, 10 * 1000);
            log("[showButton] : " + ret);
            return ret;
        };
        CButtonPromotionControl.prototype.hideButton = function (aKeepHiding) {
            log("[hideButton] : " + aKeepHiding);
            clearTimeout(this._timer);
            this._button.hideButton(aKeepHiding);
            this._button.setData();
        };
        CButtonPromotionControl.prototype.isShowing = function () {
            log("[isShowing] : ");
            return this._button.isShowing();
        };
        CButtonPromotionControl.prototype.getElement = function () {
            log("[getElement] : ");
            return this._button.getElement();
        };
        CButtonPromotionControl.prototype.setData = function (aData) {
            if (aData === void 0) { aData = null; }
            this._button.setData(aData);
            if (!aData) {
                this.hideButton();
            }
            log("[setData] : " + aData);
        };
        CButtonPromotionControl.prototype.getData = function () {
            log("[getData] : ");
            return this._button.getData();
        };
        CButtonPromotionControl.prototype.setVolatileData = function (aData) {
            if (aData === void 0) { aData = null; }
            log("[setVolatileData] : " + aData);
            this._dataVolatile = aData;
        };
        CButtonPromotionControl.prototype.getVolatileData = function () {
            log("[getVolatileData]");
            return this._dataVolatile;
        };
        CButtonPromotionControl.prototype.setFromVolatileData = function () {
            var ret = false;
            this.setData(this._dataVolatile);
            if (this._dataVolatile) {
                this._dataVolatile = null;
                ret = true;
            }
            log("[setFromVolatileData] : " + ret);
            return ret;
        };
        CButtonPromotionControl.prototype._doKeyBack = function () {
            var ret = false;
            if (this.isShowing()) {
                this.hideButton();
                ret = true;
            }
            return ret;
        };
        CButtonPromotionControl.prototype._doKeyEscape = function () {
            if (this.isShowing()) {
                this.hideButton();
            }
            return false;
        };
        CButtonPromotionControl.ID = "button-watch-in-hd";
        CButtonPromotionControl.CLASS = "-button-watch-in-hd";
        return CButtonPromotionControl;
    })(__CTRL_BASE__.CControl);
    exports.CButtonPromotionControl = CButtonPromotionControl;
});
