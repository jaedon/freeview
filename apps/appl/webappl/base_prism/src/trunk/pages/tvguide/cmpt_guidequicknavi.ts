//file name :  pages/smartsearch/cGuideQuickNavi.js
/*jslint sloppy:true, nomen:true, vars:true*/
///<reference path="../../def/sugar.d.ts" />
declare var hx;
import __KEYNAVIGATOR__ = require("pages/component/cmpt_keynavigator");

class cGuideGuickNavi extends __KEYNAVIGATOR__ {
    _UNIXDAY:number = 86400000;
    //_MAX_NAVI_LENGTH:number = 21;
    _NAVI_LENGTH:number = 8;
    _WEEKDAY = [];
    _dayOffset:number = 0;
    _$new:JQuery = null;
    isTransitioning:boolean = false;
    _readyKeyProcess:boolean = false;
    _currentViewState:string = 'hide';
    _focusColIndex:number = 0;
    _usePast:boolean = false;
    $: JQuery;
    $days: JQuery;
    animation: any;
    _startHours: any;
    _startMinutes: any;
    constructor(param) {
        super($('<article>', {
            'id': 'qNav'
        }), 'focusable', 'on');
        this.$ = this._$;
        this.$days = $('<ul>');
        if (param.animation) {
            this.animation = param.animation;
        }
        this.create();

        this._WEEKDAY = [hx.l('LOC_SUNDAY_ID'), hx.l('LOC_MONDAY_ID'), hx.l('LOC_TUESDAY_ID'), hx.l('LOC_WEDNESDAY_ID'), hx.l('LOC_THURSDAY_ID'), hx.l('LOC_FRIDAY_ID'), hx.l('LOC_SATURDAY_ID')];
    }
    create() {
        var $days: JQuery = this.$days, $li: JQuery;
        var i: number, length: number = this._NAVI_LENGTH;

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
    }
    draw() {
        var $day: JQuery, $days: JQuery;
        var i: number, str = [], length: number = this._NAVI_LENGTH;
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
    }
    destroy() {
        this.isTransitioning = false;
        this._readyKeyProcess = false;
        this._dayOffset = 0;
        this._focusColIndex = 0;
        this.$days.css('left', '0px');
        this.$.remove();
    }
    config(aMode: string) {
        var bUsePast: boolean = false;

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
    }
    show(aTimelineStartTime) {
        var i: number, length: number = this._NAVI_LENGTH, now = new Date(), anchorDate = dateFormat(aTimelineStartTime * 1000, 'dd.mm'), left, tmpDate;
        this._startHours = dateFormat(aTimelineStartTime * 1000, 'H');
        this._startMinutes = dateFormat(aTimelineStartTime * 1000, 'M');
        for (i = 0; i < length; i += 1) {

            if (this._usePast === true) {
                tmpDate = dateFormat(now.getTime() - (length - i - 1) * this._UNIXDAY, 'dd.mm');
            } else {
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
    }
    hide(aParam) {
        this._currentViewState = 'hide';
        this._readyKeyProcess = false;
        this._dayOffset = 0;
        this._focusColIndex = 0;
        this.$days.css('left', '0px');
        this._$new.removeClass('on');
        this.$.removeClass("active onFocus on");
        this.sendEventToParent(aParam);
    }
    getVisible() {
        return !!(this._currentViewState === 'show');
    }
    onFocusChanged($old, $new) {
        this._$new = $new;
    }
    KEY_LEFT(aParam):boolean {
        var left: string;

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
                    }, () => {
                        this.draw();
                        this.doNavi(aParam);
                        this.isTransitioning = false;
                    });
                }
            }
        }
        return true;
    }
    KEY_RIGHT(aParam):boolean {
        var left: string, maxDayOffset: number = this._NAVI_LENGTH - 7;

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
                    }, () => {
                        this.draw();
                        this.doNavi(aParam);
                        this.isTransitioning = false;
                    });
                }
            }
        }
        return true;
    }
    KEY_UP(param):boolean {
        return true;
    }
    KEY_DOWN(param):boolean {
        return true;
    }
    KEY_CHANNEL_UP(param):boolean {
        return true;
    }
    KEY_CHANNEL_DOWN(param):boolean {
        return true;
    }
    KEY_OPTION(param):boolean {
        return true;
    }
    KEY_NUM(num):boolean {
        return true;
    }
    KEY_OK(param):boolean {
        var date = new Date();
        var $new: JQuery = this._$new;
        var index: number = Number($new.attr('data'));

        if (index === 0 && this._usePast !== true) {
            date.setMinutes(date.getMinutes() < 30 ? 0 : 30);
            date.setSeconds(0, 0);
        } else if (index === this._NAVI_LENGTH - 1 && this._usePast === true) {
            date.setHours(date.getHours() - 2);
            date.setMinutes(date.getMinutes() < 30 ? 0 : 30);
            date.setSeconds(0, 0);
        } else {
            if (this._usePast !== true) {
                date.setTime(date.getTime() + index * 86400000);
            } else {
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
    }
    KEY_BACK(param):boolean {
        this.hide({
            'action': 'quickNaviClicked'
        });

        return true;
    }
    KEY_ESCAPE(param):boolean {
        this.hide({
            'action': 'quickNaviClicked'
        });
        return true;
    }
    getForwardLimitTime() {
        return new Date(new Date(dateFormat('isoDate')).valueOf() + this._UNIXDAY * (this._NAVI_LENGTH - 1)).toSeconds();
    }
    getBackwardLimitTime() {
        return new Date(new Date(dateFormat('isoDate')).valueOf() - this._UNIXDAY * (this._NAVI_LENGTH - 1)).toSeconds();
    }
}
export = cGuideGuickNavi;