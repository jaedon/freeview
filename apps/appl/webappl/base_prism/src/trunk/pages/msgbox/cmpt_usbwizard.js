var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    ///<reference path="../../def/sugar.d.ts" />
    var mComponent;
    (function (mComponent) {
        var usbWizard = (function (_super) {
            __extends(usbWizard, _super);
            function usbWizard() {
                _super.call(this);
                hx.logadd('sh');
            }
            usbWizard.prototype.create = function (aParam) {
                $.extend(this, aParam, {
                    _callback_fn: aParam.callback_fn,
                    _last_focus: 0
                });
                this._create({
                    'tag': 'div',
                    'attr': {
                        'class': 'unlo usbwizard',
                        'id': 'unlo',
                        'style': 'top:208px'
                    }
                });
                this.$.html('<p class="title usb">' + hx.l('LOC_USB_DETACHED_ID') + '</p>' + '<p class="button vi "><em>' + hx.l('LOC_STR_VIDEO_01_ID') + '</em></p>' + '<p class="button ph"><em>' + hx.l('LOC_STR_PHOTO_01_ID') + '</em></p>' + '<p class="button mu"><em>' + hx.l('LOC_STR_MUSIC_01_ID') + '</em></p>');
                //that.$.focus(function() {
                this.setFocus();
                //});
                return this;
            };
            usbWizard.prototype.destroy = function () {
                this.$.remove();
            };
            usbWizard.prototype.setFocus = function (aIndex) {
                hx.log('sh', 'setFocus index = ' + aIndex);
                aIndex = (aIndex === undefined ? this._last_focus : aIndex);
                this.$.find('.button').eq(aIndex).addClass('on');
                this._last_focus = aIndex;
            };
            usbWizard.prototype.removeFocus = function (aIndex) {
                hx.log('sh', 'removeFocus index = ' + aIndex);
                aIndex = (aIndex === undefined ? this._last_focus : aIndex);
                this.$.find('.button').eq(aIndex).removeClass('on');
            };
            usbWizard.prototype._on_alKey = function (aParam) {
                var result = false;
                var kc = aParam.alKey;
                switch (kc) {
                    case hx.key.KEY_RIGHT:
                        //var tmp = that.$.find('.button').index('.on');
                        var tmp = this._last_focus;
                        hx.log('sh', 'KEY_RIGHT tmp = ' + tmp);
                        this.removeFocus(tmp);
                        tmp += 1;
                        if (tmp >= 3) {
                            tmp = 0;
                        }
                        this.setFocus(tmp);
                        result = true;
                        break;
                    case hx.key.KEY_LEFT:
                        //var tmp = that.$.find('.button').index('.on');
                        var tmp = this._last_focus;
                        hx.log('sh', 'KEY_LEFT tmp = ' + tmp);
                        this.removeFocus(tmp);
                        tmp -= 1;
                        if (tmp < 0) {
                            tmp = 2;
                        }
                        this.setFocus(tmp);
                        result = true;
                        break;
                    case hx.key.KEY_OK:
                        this.__send_event(aParam);
                        result = true;
                        break;
                    case hx.key.KEY_UP:
                    case hx.key.KEY_DOWN:
                    case hx.key.KEY_RED:
                    case hx.key.KEY_GREEN:
                    case hx.key.KEY_BLUE:
                    case hx.key.KEY_YELLOW:
                        result = true;
                        break;
                    case hx.key.KEY_BACK:
                    case hx.key.KEY_POWER:
                    case hx.key.KEY_ESCAPE:
                        return false;
                        break;
                }
                this.__send_timer_refresh(aParam);
                return result;
            };
            usbWizard.prototype.__send_timer_refresh = function (aParam) {
                //that._last_focus = that.$.find('.button').index('.on');
                hx.log('sh', '__send_timer_refresh that._last_focus = ' + this._last_focus);
                aParam = (aParam) || {};
                aParam.alEvent = 'alMessageToPage';
                if (aParam.alData === undefined) {
                    aParam.alData = {};
                }
                aParam.alData.me = this;
                this.sendEventToParent(aParam);
            };
            usbWizard.prototype.__send_event = function (aParam) {
                var _title;
                //var tmp = that.$.find('.button').index('.on');
                var tmp = this._last_focus;
                hx.log('sh', '__send_event tmp = ' + tmp);
                aParam = (aParam) || {};
                aParam.alEvent = 'alClicked';
                if (aParam.alData === undefined) {
                    aParam.alData = {};
                }
                switch (tmp) {
                    case 0:
                        //video
                        _title = 'video';
                        break;
                    case 1:
                        //photo
                        _title = 'photo';
                        break;
                    case 2:
                        //music
                        _title = 'music';
                        break;
                    case 3:
                        //auto copy
                        _title = 'auto copy';
                        break;
                    default:
                        break;
                }
                aParam.alData.me = this;
                aParam.alData.index = tmp;
                aParam.alData.input_title = _title;
                aParam.alData.callback_fn = this._callback_fn;
                this.sendEventToParent(aParam);
            };
            return usbWizard;
        })(__COMPONENT_KERNEL__);
        mComponent.usbWizard = usbWizard;
    })(mComponent || (mComponent = {}));
    return mComponent;
});
