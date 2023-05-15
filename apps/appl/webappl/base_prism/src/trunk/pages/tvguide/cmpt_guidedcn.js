var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component/cmpt_keynavigator"], function (require, exports, __KEYNAVIGATOR__) {
    ///<reference path="../../def/sugar.d.ts" />
    var cGuideDcn = (function (_super) {
        __extends(cGuideDcn, _super);
        function cGuideDcn() {
            _super.call(this);
            this.isActive = false;
            this.cmptDcn = null;
            this.$ = $('#ltv');
            this._$dim = $('<div/>');
            this._timerId = null;
        }
        cGuideDcn.prototype.show = function (num, group) {
            var p = {};
            this.isActive = true;
            this._create(num, group);
        };
        cGuideDcn.prototype.clearDcn = function () {
            this._destroy();
            this.isActive = false;
        };
        cGuideDcn.prototype.getFocusedChannelInfo = function () {
            var param = {};
            param.alData = this.cmptDcn.getFocusedChannelInfo();
            return param;
        };
        cGuideDcn.prototype._create = function (aNum, aGroup) {
            var _this = this;
            this.cmptDcn = hx.al.component('dcn', {
                'tag': 'div',
                'attr': {
                    'class': 'chChange'
                },
                'startnum': aNum,
                'group': aGroup,
                'frontPanelText': '-TV GUIDE-'
            });
            this.$.prepend(this._$dim.append(this.cmptDcn.$));
            this._$dim.addClass('dimCHange');
            this._timerId = setTimeout(function () {
                _this.clearDcn();
            }, 5000);
        };
        cGuideDcn.prototype._destroy = function () {
            hx.al.decomponent(this.cmptDcn);
            if (this._timerId !== null) {
                clearTimeout(this._timerId);
                this._timerId = null;
            }
            this.$.removeClass('aniActive');
            this._$dim.remove();
            this.$.addClass('aniOut');
        };
        return cGuideDcn;
    })(__KEYNAVIGATOR__);
    return cGuideDcn;
});
