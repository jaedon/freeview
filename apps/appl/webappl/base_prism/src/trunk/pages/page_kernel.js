///<reference path="../def/sugar.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 4. 4..
 */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var CALPageKernel = (function (_super) {
        __extends(CALPageKernel, _super);
        function CALPageKernel(aEvents) {
            _super.call(this, aEvents);
            this.isPage = true;
            this._visible = false;
            this._pagesRelated = [];
            this._stackKeyHookingInfos = [];
            this._on_alKey_toRecevery = null;
        }
        CALPageKernel.prototype.setVisible = function (aVisible) {
            this._visible = aVisible;
        };
        CALPageKernel.prototype.setRelatedPages = function (aPages) {
            this._pagesRelated = aPages;
        };
        CALPageKernel.prototype.setHomeMenuOpen = function (bOpened) {
            CALPageKernel.__isOpenedHomeMenu = bOpened;
        };
        CALPageKernel.prototype.setSmartSearchOpened = function (bOpened) {
            CALPageKernel.__isOpenedSmartSearch = bOpened;
        };
        CALPageKernel.prototype.visible = function () {
            var ret = this._visible || CALPageKernel.__isOpenedHomeMenu || CALPageKernel.__isOpenedSmartSearch;
            if (!ret) {
                if (this._entity && this._entity.getCenterPageName() !== "CPgLiveController") {
                    ret = true;
                }
            }
            if (!ret) {
                var pages = this._pagesRelated;
                for (var i = 0; i < pages.length; i++) {
                    if (this.getPageVisible(pages[i])) {
                        ret = true;
                        break;
                    }
                }
            }
            if (!ret) {
                if (this._subObj) {
                    var focused = this.focusedChild(this._subObj);
                    if (typeof focused === "object") {
                        ret = true;
                    }
                }
            }
            if (!ret) {
                ret = this._getCountKeyHookingStack() > 0;
            }
            return ret;
        };
        CALPageKernel.prototype.destroy = function () {
            hx.svc.CLOCK.clearEventCb(this);
        };
        CALPageKernel.prototype.print = function () {
            hx.log("warning", "[PAGE:" + this.name + "]");
            if (this._subObj !== undefined) {
                var n;
                for (n = 0; n < this._subObj.length; n += 1) {
                    this._subObj[n].print(1);
                }
            }
        };
        CALPageKernel.prototype.sendEventToEntity = function (param) {
            hx.log("ComponentEventCatch", "{sendEventToEntity}-" + param.alEvent);
            var p = $.extend({}, param);
            p.alStarter = this.name;
            this._entity.byComposer(p);
        };
        CALPageKernel.prototype.focusedChild = function (arr) {
            if (arr === undefined) {
                return;
            }
            var n;
            for (n = 0; n < arr.length; n += 1) {
                if (!!arr[n].focus && !arr[n]._focusException) {
                    return arr[n];
                }
            }
        };
        CALPageKernel.prototype.getLastFocusedElement = function () {
            var obj = this, next = this.focusedChild(obj._subObj);
            while (next) {
                obj = next;
                next = this.focusedChild(obj._subObj);
            }
            return obj;
        };
        CALPageKernel.prototype.getCenterPageName = function () {
            return this._entity.getCenterPageName();
        };
        CALPageKernel.prototype.getPageVisible = function (name) {
            return this._entity.getPageVisible(name);
        };
        CALPageKernel.prototype.addHistory = function (str) {
            var a = this._entity.history();
            return a.add(str);
        };
        CALPageKernel.prototype.popHistory = function () {
            var a = this._entity.history();
            return a.pop();
        };
        CALPageKernel.prototype.addMark = function (str, mark) {
            var a = this._entity.history();
            return a.addwithMark(str, mark);
        };
        CALPageKernel.prototype.popMark = function (mark) {
            var a = this._entity.history();
            return a.popwithMark(mark);
        };
        CALPageKernel.prototype.byEntity = function (aParam) {
            this.doCommand(aParam);
        };
        // for settings & guide
        CALPageKernel.prototype.showDateTime = function () {
            var elDate = document.createElement('dd');
            var elTime = document.createElement('dd');
            var $dateTime = $('<dl>', {
                'class': 'dateTime'
            }).append($('<dt>')).append($(elDate)).append($(elTime));
            var now = new Date();
            elDate.innerHTML = dateFormat(now, "dd.mm.yyyy");
            elTime.innerHTML = dateFormat(now, "ddd HH:MM").toUpperCase();
            hx.svc.CLOCK.addEventCb('MinuteChanged', this, function (aNow) {
                elDate.innerHTML = dateFormat(aNow, "dd.mm.yyyy");
                elTime.innerHTML = dateFormat(aNow, "ddd HH:MM").toUpperCase();
            });
            return $dateTime;
        };
        CALPageKernel.prototype._on_alKey = function (aParam) {
            return false;
        };
        CALPageKernel.prototype._getCountKeyHookingStack = function () {
            var ret = 0;
            var stackKeyHookingInfos = this._stackKeyHookingInfos;
            for (var i = 0, len = stackKeyHookingInfos.length; i < len; i++) {
                if (!stackKeyHookingInfos[i].countException) {
                    ret++;
                }
            }
            return ret;
        };
        CALPageKernel.prototype.setKeyHookingControl = function (aControl, aForceReturnTrue, aReqCountException) {
            var _this = this;
            if (aForceReturnTrue === void 0) { aForceReturnTrue = false; }
            if (aReqCountException === void 0) { aReqCountException = false; }
            var _setKeyHandlelr = function (aKeyHandler) {
                _this._on_alKey = aKeyHandler;
            };
            var stackInfos = this._stackKeyHookingInfos;
            var controls = _.pluck(stackInfos, "control");
            var position = controls.indexOf(aControl);
            var current;
            if (!this._on_alKey_toRecevery) {
                this._on_alKey_toRecevery = this._on_alKey;
            }
            if (position !== -1) {
                stackInfos.splice(position, 1);
                if (stackInfos.length > 0) {
                    current = _.where(stackInfos, { "current": true })[0];
                    if (!current) {
                        current = stackInfos[stackInfos.length - 1];
                        current.current = true;
                        _setKeyHandlelr(current.keyHandler);
                    }
                }
                else {
                    _setKeyHandlelr(this._on_alKey_toRecevery);
                    delete this._on_alKey_toRecevery;
                }
            }
            else {
                stackInfos = stackInfos.map(function reOrder(stackInfo, index) {
                    stackInfo.current = false;
                    return stackInfo;
                });
                current = {
                    control: aControl,
                    keyHandler: function (aKeyCode) {
                        var handled;
                        if (aControl && aControl.doKey) {
                            handled = aControl.doKey(aKeyCode.alKey);
                        }
                        if (!handled || !aControl.doKey) {
                            handled = _this._on_alKey_toRecevery.call(_this, aKeyCode);
                        }
                        return handled || aForceReturnTrue;
                    },
                    current: true,
                    countException: aReqCountException
                };
                stackInfos.push(current);
                _setKeyHandlelr(current.keyHandler);
                this._stackKeyHookingInfos = stackInfos;
            }
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        };
        CALPageKernel.prototype.closeAllHookingControls = function () {
            this._stackKeyHookingInfos.forEach(function (stackInfo) {
                if (stackInfo.control && stackInfo.control.close) {
                    stackInfo.control.close();
                }
            });
        };
        CALPageKernel.prototype._doOnLatent = function () {
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        };
        CALPageKernel.prototype._doOffLatent = function () {
            hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
        };
        CALPageKernel.__isOpenedHomeMenu = false;
        CALPageKernel.__isOpenedSmartSearch = false;
        return CALPageKernel;
    })(__COMPONENT_KERNEL__);
    return CALPageKernel;
});
