///<reference path="../def/sugar.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 4. 4..
 */

import __COMPONENT_KERNEL__ = require("./component_kernel");
import __CONTROLS__ = require("pages/util/controls");
declare var hx: ifSugar;
class CALPageKernel extends __COMPONENT_KERNEL__ {
    static __isOpenedHomeMenu = false;
    static __isOpenedSmartSearch = false;
    isPage: boolean = true;
    protected _visible = false;
    protected _pagesRelated: string[] = [];
    constructor(aEvents?) {
        super(aEvents);
    }
    protected setVisible(aVisible: boolean) {
        this._visible = aVisible;
    }
    setRelatedPages(aPages: string[]) {
        this._pagesRelated = aPages;
    }
    setHomeMenuOpen(bOpened: boolean) {
        CALPageKernel.__isOpenedHomeMenu = bOpened;
    }
    setSmartSearchOpened(bOpened: boolean) {
        CALPageKernel.__isOpenedSmartSearch = bOpened;
    }
    visible() {
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
            ret = this._getCountKeyHookingStack() > 0
        }
        return ret;
    }
    destroy() {
        hx.svc.CLOCK.clearEventCb(this);
    }
    print() {
        hx.log("warning", "[PAGE:" + this.name + "]");
        if (this._subObj !== undefined) {
            var n;
            for ( n = 0; n < this._subObj.length; n += 1) {
                this._subObj[n].print(1);
            }
        }
    }
    sendEventToEntity(param) {
        hx.log("ComponentEventCatch", "{sendEventToEntity}-" + param.alEvent);

        var p = $.extend({}, param);
        p.alStarter = this.name;
        this._entity.byComposer(p);
    }
    focusedChild(arr) {
        if (arr === undefined) {
            return;
        }
        var n;
        for ( n = 0; n < arr.length; n += 1) {
            if (!!arr[n].focus && !arr[n]._focusException) {
                return arr[n];
            }
        }
    }
    getLastFocusedElement() {
        var obj = this, next = this.focusedChild(obj._subObj);

        while (next) {
            obj = next;
            next = this.focusedChild(obj._subObj);
        }
        return obj;
    }
    getCenterPageName() {
        return this._entity.getCenterPageName();
    }
    getPageVisible(name) {
        return this._entity.getPageVisible(name);
    }
    addHistory(str) {
        var a = this._entity.history();
        return a.add(str);
    }
    popHistory() {
        var a = this._entity.history();
        return a.pop();
    }
    addMark(str, mark) {
        var a = this._entity.history();
        return a.addwithMark(str, mark);
    }
    popMark(mark) {
        var a = this._entity.history();
        return a.popwithMark(mark);
    }
    byEntity(aParam) {
        this.doCommand(aParam);
    }
    // for settings & guide
    showDateTime(): JQuery {
        var elDate = document.createElement('dd');
        var elTime = document.createElement('dd');
        var $dateTime = $('<dl>', {
            'class' : 'dateTime'
        }).append($('<dt>'))    // title
            .append($(elDate)).append($(elTime));
        var now = new Date();
        elDate.innerHTML = dateFormat(now, "dd.mm.yyyy");
        elTime.innerHTML = dateFormat(now, "ddd HH:MM").toUpperCase();
        hx.svc.CLOCK.addEventCb('MinuteChanged', this, (aNow) => {
            elDate.innerHTML = dateFormat(aNow, "dd.mm.yyyy");
            elTime.innerHTML = dateFormat(aNow, "ddd HH:MM").toUpperCase();
        });
        return $dateTime;
    }
    _on_alKey(aParam: any): boolean {
        return false;
    }
    private _stackKeyHookingInfos: ifKeyHookingStackInfo[] = [];
    private _on_alKey_toRecevery: (aParam: any) => boolean = null;
    private _getCountKeyHookingStack() {
        var ret = 0;
        var stackKeyHookingInfos = this._stackKeyHookingInfos;
        for (var i = 0, len = stackKeyHookingInfos.length; i < len; i++) {
            if (!stackKeyHookingInfos[i].countException) {
                ret++;
            }
        }
        return ret;
    }
    setKeyHookingControl(aControl: __CONTROLS__.CControl, aForceReturnTrue = false, aReqCountException = false) {
        var _setKeyHandlelr = (aKeyHandler: (aKeyParam) => boolean) => {
            this._on_alKey = aKeyHandler;
        };
        var stackInfos: ifKeyHookingStackInfo[] = this._stackKeyHookingInfos;
        var controls = _.pluck(stackInfos, "control");
        var position = controls.indexOf(aControl);
        var current: ifKeyHookingStackInfo;
        if (!this._on_alKey_toRecevery) {
            this._on_alKey_toRecevery = this._on_alKey;
        }
        if (position !== -1) {
            stackInfos.splice(position, 1);
            if (stackInfos.length > 0) {
                current = _.where(stackInfos, { "current": true } )[0];
                if (!current) {
                    current = stackInfos[stackInfos.length - 1];
                    current.current = true;
                    _setKeyHandlelr(current.keyHandler);
                }
            } else {
                _setKeyHandlelr(this._on_alKey_toRecevery);
                delete this._on_alKey_toRecevery;
            }
        } else {
            stackInfos = <ifKeyHookingStackInfo[]>stackInfos.map(function reOrder(stackInfo, index) {
                stackInfo.current = false;
                return stackInfo;
            });
            current = {
                control: aControl,
                keyHandler: (aKeyCode): boolean => {
                    var handled;
                    if(aControl && aControl.doKey){
                        handled = aControl.doKey(aKeyCode.alKey);
                    }
                    if (!handled || !aControl.doKey) {
                        handled = this._on_alKey_toRecevery.call(this, aKeyCode);
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
    }
    closeAllHookingControls() {
        this._stackKeyHookingInfos.forEach(function(stackInfo) {
            if (stackInfo.control && (<any>stackInfo.control).close) {
                (<any>stackInfo.control).close();
            }
        });
    }
    _doOnLatent() {
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
    }
    _doOffLatent() {
        hx.svc.SETTING_UTIL.doingKeyGrab(this.visible());
    }
}

interface ifKeyHookingStackInfo {
    control: __CONTROLS__.CControl;
    keyHandler: (aKeyCode) => boolean;
    current: boolean;
    countException: boolean;
}

export = CALPageKernel;