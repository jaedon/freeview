/// <reference path="../../def/sugar.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 5. 11..
 */
//    CLOCK: ['layer_domain/service/dl_clock.js']
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    var cDLClock = (function (_super) {
        __extends(cDLClock, _super);
        function cDLClock(aServiceName) {
            _super.call(this, aServiceName);
            this.defineEvents(['SecondChanged', 'MinuteChanged', 'HourChanged', 'DayChanged']);
            //this.startClock();
        }
        cDLClock.prototype.startClock = function () {
            var _this = this;
            if (this.clockTimer) {
                return;
            }
            var now = new Date();
            var date = now;
            var sec = now.getSeconds();
            var min = now.getMinutes();
            var hour = now.getHours();
            var day = now.getDate();
            this.clockTimer = setInterval(function () {
                var now = new Date();
                if (30000 < Math.abs(now.getTime() - date.getTime()) || Math.abs(now.getTimezoneOffset() - date.getTimezoneOffset())) {
                    sec = -1;
                    min = -1;
                    hour = -1;
                    day = -1;
                }
                date = now;
                if (sec != now.getSeconds()) {
                    if (min != now.getMinutes()) {
                        if (hour != now.getHours()) {
                            if (day != now.getDate()) {
                                day = now.getDate();
                                _this.fireEvent('DayChanged', now);
                            }
                            hour = now.getHours();
                            _this.fireEvent('HourChanged', now);
                        }
                        min = now.getMinutes();
                        _this.fireEvent('MinuteChanged', now);
                    }
                    sec = now.getSeconds();
                    _this.fireEvent('SecondChanged', now);
                }
            }, 1000);
        };
        cDLClock.prototype.stopClock = function () {
            if (this.clockTimer) {
                clearInterval(this.clockTimer);
                this.clockTimer = null;
            }
        };
        return cDLClock;
    })(__KERNEL__);
    return cDLClock;
});
