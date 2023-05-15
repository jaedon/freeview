var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component/cmpt_keynavigator"], function (require, exports, __KEYNAVIGATOR__) {
    ///<reference path="../../def/sugar.d.ts" />
    var cGuideGuickNavi = (function (_super) {
        __extends(cGuideGuickNavi, _super);
        function cGuideGuickNavi(param) {
            _super.call(this, $('<article>', {
                'id': 'qNav'
            }), 'focusable', 'on');
            this._UNIXDAY = 86400000;
            //_MAX_NAVI_LENGTH:number = 21;
            this._NAVI_LENGTH = 8;
            this._WEEKDAY = [];
            this._dayOffset = 0;
            this._$new = null;
            this.isTransitioning = false;
            this._readyKeyProcess = false;
            this._currentViewState = 'hide';
            this._focusColIndex = 0;
            this._usePast = false;
            this.$ = this._$;
            this.$days = $('<ul>');
            if (param.animation) {
                this.animation = param.animation;
            }
            this.create();
            this._WEEKDAY = [hx.l('LOC_SUNDAY_ID'), hx.l('LOC_MONDAY_ID'), hx.l('LOC_TUESDAY_ID'), hx.l('LOC_WEDNESDAY_ID'), hx.l('LOC_THURSDAY_ID'), hx.l('LOC_FRIDAY_ID'), hx.l('LOC_SATURDAY_ID')];
        }
        cGuideGuickNavi.prototype.create = function () {
            var $days = this.$days, $li;
            var i, length = this._NAVI_LENGTH;
            for (i = 0; i < length; i += 1) {
                $li = $('<li>');
                if (i === 0) {
                    this._$new = $li;
                }
                $li.attr('data', i);
                $days.append($li);
            }
            this.$.append($('<div>', {
                'class': 'gDate'
            }).append($days));
        };
        cGuideGuickNavi.prototype.draw = function () {
            var $day, $days;
            var i, str = [], length = this._NAVI_LENGTH;
            var now = new Date(), date = new Date();
            var dayOffset = this._dayOffset;
            this.$.removeClass('rolL rolR');
            $days = this.$days.children();
            $days.removeClass('focusable');
            if (dayOffset !== 0) {
                this.$.addClass('rolL');
            }
            if ((7 + dayOffset) < length) {
                this.$.addClass('rolR');
            }
            for (i = 0; i < length; i += 1) {
                $day = $days.eq(i);
                if (this._usePast === true) {
                    date.setTime(now.getTime() - (length - i - 1) * this._UNIXDAY);
                }
                else {
                    date.setTime(now.getTime() + (i * this._UNIXDAY));
                }
                $day.html(hx.util.format('<span>{0}</span><em>{1}</em>', date.format('dd.mm'), this._WEEKDAY[date.getDay()]));
                $day[0][hx.key.KEY_LEFT] = null;
                $day[0][hx.key.KEY_RIGHT] = null;
                if (i >= dayOffset && i < 7 + dayOffset) {
                    $day.addClass('focusable');
                }
            }
            $days.eq(this._focusColIndex).addClass('on');
            this.build(this.leftRightBuilder);
        };
        cGuideGuickNavi.prototype.destroy = function () {
            this.isTransitioning = false;
            this._readyKeyProcess = false;
            this._dayOffset = 0;
            this._focusColIndex = 0;
            this.$days.css('left', '0px');
            this.$.remove();
        };
        cGuideGuickNavi.prototype.config = function (aMode) {
            var bUsePast = false;
            if (aMode === "backward") {
                bUsePast = true;
            }
            if (this._usePast !== bUsePast) {
                if (bUsePast === true) {
                    this._usePast = true;
                    //this._NAVI_LENGTH = 21;
                    this._NAVI_LENGTH = 8;
                }
                else {
                    this._usePast = false;
                    //this._NAVI_LENGTH = 14;
                    this._NAVI_LENGTH = 8;
                }
            }
        };
        cGuideGuickNavi.prototype.show = function (aTimelineStartTime) {
            var i, length = this._NAVI_LENGTH, now = new Date(), anchorDate = dateFormat(aTimelineStartTime * 1000, 'dd.mm'), left, tmpDate;
            this._startHours = dateFormat(aTimelineStartTime * 1000, 'H');
            this._startMinutes = dateFormat(aTimelineStartTime * 1000, 'M');
            for (i = 0; i < length; i += 1) {
                if (this._usePast === true) {
                    tmpDate = dateFormat(now.getTime() - (length - i - 1) * this._UNIXDAY, 'dd.mm');
                }
                else {
                    tmpDate = dateFormat(now.getTime() + i * this._UNIXDAY, 'dd.mm');
                }
                if (anchorDate === tmpDate) {
                    if (i >= 7) {
                        this._dayOffset = i - 6;
                        left = (-165 * this._dayOffset) + 'px';
                        this.$days.css('left', left);
                    }
                    this._focusColIndex = i;
                    this._$new = this.$days.children().eq(i);
                    break;
                }
            }
            this._currentViewState = 'show';
            this.$.addClass("active onFocus on");
            this.draw();
        };
        cGuideGuickNavi.prototype.hide = function (aParam) {
            this._currentViewState = 'hide';
            this._readyKeyProcess = false;
            this._dayOffset = 0;
            this._focusColIndex = 0;
            this.$days.css('left', '0px');
            this._$new.removeClass('on');
            this.$.removeClass("active onFocus on");
            this.sendEventToParent(aParam);
        };
        cGuideGuickNavi.prototype.getVisible = function () {
            return !!(this._currentViewState === 'show');
        };
        cGuideGuickNavi.prototype.onFocusChanged = function ($old, $new) {
            this._$new = $new;
        };
        cGuideGuickNavi.prototype.KEY_LEFT = function (aParam) {
            var _this = this;
            var left;
            if (this.animation.running) {
                return true;
            }
            if (!this._readyKeyProcess && aParam.alRepeat > 0) {
                return true;
            }
            this._readyKeyProcess = true;
            if (!this.isTransitioning) {
                if (!this.doNavi(aParam)) {
                    if (this._dayOffset !== 0) {
                        this.isTransitioning = true;
                        this._dayOffset -= 1;
                        left = (-165 * this._dayOffset) + 'px';
                        this._focusColIndex = this._dayOffset;
                        this.$days.anim({
                            'left': left
                        }, function () {
                            _this.draw();
                            _this.doNavi(aParam);
                            _this.isTransitioning = false;
                        });
                    }
                }
            }
            return true;
        };
        cGuideGuickNavi.prototype.KEY_RIGHT = function (aParam) {
            var _this = this;
            var left, maxDayOffset = this._NAVI_LENGTH - 7;
            if (this.animation.running) {
                return true;
            }
            if (!this._readyKeyProcess && aParam.alRepeat > 0) {
                return true;
            }
            this._readyKeyProcess = true;
            if (!this.isTransitioning) {
                if (!this.doNavi(aParam)) {
                    if (this._dayOffset < maxDayOffset) {
                        this.isTransitioning = true;
                        this._dayOffset += 1;
                        this._focusColIndex = this._dayOffset + 6;
                        left = (-165 * this._dayOffset) + 'px';
                        this.$days.anim({
                            'left': left
                        }, function () {
                            _this.draw();
                            _this.doNavi(aParam);
                            _this.isTransitioning = false;
                        });
                    }
                }
            }
            return true;
        };
        cGuideGuickNavi.prototype.KEY_UP = function (param) {
            return true;
        };
        cGuideGuickNavi.prototype.KEY_DOWN = function (param) {
            return true;
        };
        cGuideGuickNavi.prototype.KEY_CHANNEL_UP = function (param) {
            return true;
        };
        cGuideGuickNavi.prototype.KEY_CHANNEL_DOWN = function (param) {
            return true;
        };
        cGuideGuickNavi.prototype.KEY_OPTION = function (param) {
            return true;
        };
        cGuideGuickNavi.prototype.KEY_NUM = function (num) {
            return true;
        };
        cGuideGuickNavi.prototype.KEY_OK = function (param) {
            var date = new Date();
            var $new = this._$new;
            var index = Number($new.attr('data'));
            if (index === 0 && this._usePast !== true) {
                date.setMinutes(date.getMinutes() < 30 ? 0 : 30);
                date.setSeconds(0, 0);
            }
            else if (index === this._NAVI_LENGTH - 1 && this._usePast === true) {
                date.setHours(date.getHours() - 2);
                date.setMinutes(date.getMinutes() < 30 ? 0 : 30);
                date.setSeconds(0, 0);
            }
            else {
                if (this._usePast !== true) {
                    date.setTime(date.getTime() + index * 86400000);
                }
                else {
                    date.setTime(date.getTime() - (this._NAVI_LENGTH - index - 1) * 86400000);
                }
                date.setHours(this._startHours);
                date.setMinutes(this._startMinutes >= 30 ? 30 : 0);
                date.setSeconds(0, 0);
            }
            this.hide({
                'action': 'quickNaviClicked',
                'data': date
            });
            return true;
        };
        cGuideGuickNavi.prototype.KEY_BACK = function (param) {
            this.hide({
                'action': 'quickNaviClicked'
            });
            return true;
        };
        cGuideGuickNavi.prototype.KEY_ESCAPE = function (param) {
            this.hide({
                'action': 'quickNaviClicked'
            });
            return true;
        };
        cGuideGuickNavi.prototype.getForwardLimitTime = function () {
            return new Date(new Date(dateFormat('isoDate')).valueOf() + this._UNIXDAY * (this._NAVI_LENGTH - 1)).toSeconds();
        };
        cGuideGuickNavi.prototype.getBackwardLimitTime = function () {
            return new Date(new Date(dateFormat('isoDate')).valueOf() - this._UNIXDAY * (this._NAVI_LENGTH - 1)).toSeconds();
        };
        return cGuideGuickNavi;
    })(__KEYNAVIGATOR__);
    return cGuideGuickNavi;
});
