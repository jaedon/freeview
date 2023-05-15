///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cSimpleList.js
/*jslint sloppy:true, nomen:true, vars:true*/
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component/cmpt_keynavigator"], function (require, exports, __KEYNAVIGATOR__) {
    var simpleList = (function (_super) {
        __extends(simpleList, _super);
        function simpleList($, focusable, focused) {
            _super.call(this, $, focusable, focused);
            this.source = null;
            this.currentIndex = 0;
            this.dispalyedData = [];
            this.displayedCount = 0;
            this.DISPLAY_COUNT = 5;
        }
        simpleList.prototype.bind = function (source, displayCount) {
            if (source) {
                this.source = source;
            }
            if (displayCount) {
                this.DISPLAY_COUNT = displayCount;
            }
        };
        simpleList.prototype.get = function (aName) {
            var start;
            var end;
            var data = [];
            if (this.source) {
                start = this.currentIndex;
                end = start + this.DISPLAY_COUNT;
                data = this.source.slice(start, end);
            }
            return data;
        };
        simpleList.prototype.draw = function () {
            var data = this.get();
            this.dispalyedData = data;
            this.displayedCount = data.length;
            if (this.doDraw) {
                this.doDraw(data);
            }
            this.build(this.upDownBuilder);
        };
        simpleList.prototype.KEY_UP = function (param) {
            var handled = this.doNavi(param);
            if (!handled) {
                this.currentIndex -= 1;
                this.currentIndex = Math.max(0, this.currentIndex);
                this.draw();
            }
            if (this.currentIndex > 0) {
                handled = true;
            }
            return handled;
        };
        simpleList.prototype.KEY_DOWN = function (param) {
            var handled = this.doNavi(param);
            if (!handled && this.displayedCount === this.DISPLAY_COUNT && this.dispalyedData.length !== this.DISPLAY_COUNT) {
                this.currentIndex += 1;
                this.currentIndex = Math.min(this.source.length - 1, this.currentIndex);
                this.draw();
            }
            return handled;
        };
        return simpleList;
    })(__KEYNAVIGATOR__);
    exports.simpleList = simpleList;
});
