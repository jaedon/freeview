///<reference path="../../def/sugar.d.ts" />
// file name : pages/msgbox/cmpt_watchinHDButton.js
/* jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel", "../../def/message.def"], function (require, exports, __COMPONENT_KERNEL__, __MSG_DEF__) {
    (function (direction) {
        direction[direction["NONE"] = 0] = "NONE";
        direction[direction["PREV"] = 1] = "PREV";
        direction[direction["NEXT"] = 2] = "NEXT";
        direction[direction["ALL"] = 3] = "ALL";
    })(exports.direction || (exports.direction = {}));
    var direction = exports.direction;
    var watchinHDButton = (function (_super) {
        __extends(watchinHDButton, _super);
        function watchinHDButton() {
            _super.call(this);
            this.$ = null;
            this._$watchinHDBtn = null;
            this._elMinipop = null;
            this._elMiniMessage = null;
            this._elText = null;
            this._updownKeyManager = null;
            this._callback_fn = null;
            //private _programmes: any = null;
            this._linkage = null;
        }
        watchinHDButton.prototype.create = function (aParam) {
            this.$ = $('<section>');
            this._$watchinHDBtn = $('<section>');
            this._$watchinHDBtn[0].id = 'trailer';
            this._$watchinHDBtn.hide();
            this.$[0].appendChild(this._$watchinHDBtn[0]);
            //this._programmes = aParam.programmes;
            this._callback_fn = aParam.callback_fn;
            this._linkage = aParam.linkage;
            this._showWatchinHDBtn();
            return this;
        };
        watchinHDButton.prototype.resetTimeout = function () {
            this.__send_timer_refresh();
        };
        watchinHDButton.prototype.destroy = function () {
            this.$.remove();
            this.$ = null;
            this._callback_fn = null;
            this._$watchinHDBtn = null;
            this._elMinipop = null;
            this._elMiniMessage = null;
            this._elText = null;
            this._updownKeyManager = null;
            //this._programmes = null;
            this._linkage = null;
        };
        watchinHDButton.prototype.show = function () {
            this.$.show();
        };
        watchinHDButton.prototype.hide = function () {
            this.$.hide();
        };
        // EVENT HANDLER
        watchinHDButton.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var kc = aParam.alKey;
            var curFocusEl = null;
            if (this._$watchinHDBtn[0].style.display === "none") {
            }
            else {
                switch (kc) {
                    case hx.key.KEY_OK:
                        this.__send_event(0 /* GOTO_THE_HD_CHANNEL */);
                        bConsumed = true;
                        break;
                }
            }
            this.__send_timer_refresh(aParam);
            return bConsumed;
        };
        watchinHDButton.prototype._showWatchinHDBtn = function () {
            this._$watchinHDBtn.hide();
            if (!this._elMinipop) {
                this._elMinipop = document.createElement('section');
                this._elMinipop.id = 'minipop';
                this._elMinipop.className = 'dimTrailer';
                this._$watchinHDBtn[0].appendChild(this._elMinipop);
            }
            if (!this._elMiniMessage) {
                this._elMiniMessage = document.createElement('section');
                this._elMiniMessage.className = 'minimessage';
                this._elMinipop.appendChild(this._elMiniMessage);
            }
            if (!this._elText) {
                this._elText = document.createElement('p');
                this._elText.className = 'txt';
                this._elText.innerHTML = "<em class='OK'></em>" + hx.l('LOC_WATCH_IN_HD_ID');
                this._elMiniMessage.appendChild(this._elText);
            }
            this._$watchinHDBtn.show();
        };
        watchinHDButton.prototype.__send_timer_refresh = function (aParam) {
            var index;
            aParam = (aParam) || {};
            aParam.alEvent = 'alMessageToPage';
            if (aParam.alData === undefined) {
                aParam.alData = {};
            }
            aParam.alData.me = this;
            this.sendEventToParent(aParam);
        };
        watchinHDButton.prototype.__send_event = function (aInput) {
            var aParam = {
                alEvent: null,
                alData: {
                    me: null,
                    callback_fn: null,
                    input_title: null,
                    linkage: null
                }
            };
            aParam.alEvent = 'alClicked';
            aParam.alData.me = this;
            aParam.alData.callback_fn = this._callback_fn;
            aParam.alData.input_title = aInput;
            aParam.alData.linkage = this._linkage;
            this.sendEventToParent(aParam);
        };
        return watchinHDButton;
    })(__COMPONENT_KERNEL__);
    exports.watchinHDButton = watchinHDButton;
});
