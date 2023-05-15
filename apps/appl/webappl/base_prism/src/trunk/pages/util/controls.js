var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports"], function (require, exports) {
    /// <reference path="../../def/sugar.d.ts" />
    /// <reference path="../../def/controls.d.ts" />
    /** controls */
    var mControl;
    (function (mControl) {
        var CONTROLS_CONFIG = {
            KEY_DOWN: hx.key.KEY_DOWN,
            KEY_UP: hx.key.KEY_UP,
            KEY_LEFT: hx.key.KEY_LEFT,
            KEY_RIGHT: hx.key.KEY_RIGHT,
            KEY_BACK: hx.key.KEY_BACK,
            KEY_PAGEUP: hx.key.KEY_CHANNEL_UP,
            KEY_PAGEDOWN: hx.key.KEY_CHANNEL_DOWN,
            KEY_REWIND: hx.key.KEY_REWIND,
            KEY_FAST_FWD: hx.key.KEY_FAST_FWD,
            KEY_PLAY_SPEED_DOWN: hx.key.KEY_PLAY_SPEED_DOWN,
            KEY_PLAY_SPEED_UP: hx.key.KEY_PLAY_SPEED_UP,
            KEY_PLAY: hx.key.KEY_PLAY,
            KEY_PAUSE: hx.key.KEY_PAUSE,
            KEY_ESCAPE: hx.key.KEY_ESCAPE,
            KEY_STOP: hx.key.KEY_STOP,
            CUSTOM_KEY_HANDLERS: {},
            LOG: function (msg) {
                //hx.log('debug', msg);
            }
        };
        CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS[hx.key.KEY_RECORD] = 'doKeyRec';
        CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS[hx.key.KEY_OPTION] = 'doKeyOption';
        CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS[hx.key.KEY_PLAY] = 'doKeyPlay';
        CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS[hx.key.KEY_PAUSE] = 'doKeyPause';
        CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS[hx.key.KEY_ESCAPE] = 'doKeyEscape';
        CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS[hx.key.KEY_STOP] = 'doKeyStop';
        CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS[hx.key.KEY_PLAY_SPEED_UP] = 'doKeyPlaySpeedUp';
        CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS[hx.key.KEY_PLAY_SPEED_DOWN] = 'doKeyPlaySpeedDown';
        //
        var cssPrefix = '-';
        var cssFocusable = cssPrefix + 'f';
        var cssFocused = cssPrefix + 'fd';
        var cssActiveFocused = cssPrefix + 'afd';
        var cssActiveFocusedLeaf = cssPrefix + 'afd-leaf';
        var KKeyUp = 38;
        var KKeyDown = 40;
        var KKeyLeft = 37;
        var KKeyRight = 39;
        var KKeyOk = 13;
        var KKeyPageUp = 33;
        var KKeyPageDown = 34;
        var KKeyBack = 8;
        var KKeyEscape = 27;
        var KKeyRewind = -1;
        var KKeyFastFwd = -1;
        var KKeyPlaySpeedDown = -1;
        var KKeyPlaySpeedUp = -1;
        var KKeyPause = 19;
        var keyHandlers = {};
        var fnLog = function (msg) {
            console.log(msg);
        };
        if (CONTROLS_CONFIG) {
            if (CONTROLS_CONFIG.KEY_BACK !== undefined) {
                KKeyBack = CONTROLS_CONFIG.KEY_BACK;
            }
            if (CONTROLS_CONFIG.KEY_PAGEUP !== undefined) {
                KKeyPageUp = CONTROLS_CONFIG.KEY_PAGEUP;
            }
            if (CONTROLS_CONFIG.KEY_PAGEDOWN !== undefined) {
                KKeyPageDown = CONTROLS_CONFIG.KEY_PAGEDOWN;
            }
            if (CONTROLS_CONFIG.KEY_REWIND !== undefined) {
                KKeyRewind = CONTROLS_CONFIG.KEY_REWIND;
            }
            if (CONTROLS_CONFIG.KEY_FAST_FWD !== undefined) {
                KKeyFastFwd = CONTROLS_CONFIG.KEY_FAST_FWD;
            }
            if (CONTROLS_CONFIG.KEY_PLAY_SPEED_DOWN !== undefined) {
                KKeyPlaySpeedDown = CONTROLS_CONFIG.KEY_PLAY_SPEED_DOWN;
            }
            if (CONTROLS_CONFIG.KEY_PLAY_SPEED_UP !== undefined) {
                KKeyPlaySpeedUp = CONTROLS_CONFIG.KEY_PLAY_SPEED_UP;
            }
            if (CONTROLS_CONFIG.KEY_PAUSE !== undefined) {
                KKeyPause = CONTROLS_CONFIG.KEY_PLAY_PAUSE;
            }
            if (CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS) {
                var keyCode, customHandlers = CONTROLS_CONFIG.CUSTOM_KEY_HANDLERS;
                for (keyCode in customHandlers) {
                    if (customHandlers.hasOwnProperty(keyCode)) {
                        keyHandlers[keyCode] = customHandlers[keyCode];
                    }
                }
            }
            if (CONTROLS_CONFIG.LOG) {
                fnLog = CONTROLS_CONFIG.LOG;
            }
        }
        keyHandlers[KKeyUp] = 'doKeyUp';
        keyHandlers[KKeyDown] = 'doKeyDown';
        keyHandlers[KKeyLeft] = 'doKeyLeft';
        keyHandlers[KKeyRight] = 'doKeyRight';
        keyHandlers[KKeyPageUp] = 'doKeyPageUp';
        keyHandlers[KKeyPageDown] = 'doKeyPageDown';
        keyHandlers[KKeyRewind] = 'doKeyRewind';
        keyHandlers[KKeyFastFwd] = 'doKeyFastFwd';
        keyHandlers[KKeyPlaySpeedDown] = 'doKeyPlaySpeedDown';
        keyHandlers[KKeyPlaySpeedUp] = 'doKeyPlaySpeedUp';
        keyHandlers[KKeyOk] = 'doKeyOk';
        keyHandlers[KKeyBack] = 'doKeyBack';
        for (var key in hx.key) {
            var value = hx.key[key];
            if (typeof value === "number") {
                if (!keyHandlers[value]) {
                    var split = key.split("_");
                    var part;
                    split[0] = "doKey";
                    for (var i = 1; i < split.length; i++) {
                        part = split[i];
                        if (part.length > 1) {
                            part = part[0] + part.substr(1).toLowerCase();
                        }
                        split[i] = part;
                    }
                    keyHandlers[value] = split.join("");
                }
            }
        }
        var cssPropTransition = 'transition';
        var cssEvntTransitionEnd = 'transitionend';
        var cssTransitionDuration = 'transition-duration';
        if ($.browser.webkit) {
            cssPropTransition = '-webkit-transition';
            cssEvntTransitionEnd = 'webkitTransitionEnd';
            cssTransitionDuration = '-webkit-transition-duration';
        }
        else if ($.browser.opera) {
            cssPropTransition = '-o-transition';
            cssEvntTransitionEnd = 'oTransitionEnd otransitionend';
        }
        else if ($.browser.msie) {
            cssPropTransition = '-ms-transition';
            cssEvntTransitionEnd = 'MSTransitionEnd';
        }
        var cssTransitionParamPos = 'top .1s linear, left .1s linear';
        var cssTransitionParamOpa = 'opacity .3s linear';
        var cssTransitionParamScale = 'height .3s linear, width .3s linear';
        $.fn.absoluteClone = function (param) {
            var target = this;
            var clone = $('<div></div>');
            clone.css({
                'position': 'absolute',
                'left': parseInt(target.position().left, 10) + 'px',
                'top': parseInt(target.position().top, 10) + 'px',
                'width': parseInt(target.outerWidth(), 10) + 'px',
                'height': parseInt(target.outerHeight(), 10) + 'px'
            });
            if (param) {
                if (param.id) {
                    clone.attr('id', param.id);
                }
            }
            if (param && param.prepend) {
                target.parent().prepend(clone);
            }
            else {
                target.parent().append(clone);
            }
            return clone;
        };
        $.fn.afterTransition = function (handler) {
            var target = this;
            target.bind(cssEvntTransitionEnd, function (e) {
                if (e.srcElement === this) {
                    setTimeout(function () {
                        handler(e);
                    }, 1);
                }
            });
            return target;
        };
        $.fn.afterTransitionOneShot = function (handler) {
            var target = this, duration = parseFloat(target.css(cssTransitionDuration)) * 1000;
            setTimeout(function () {
                handler();
            }, duration);
            return target;
        };
        var leftToRightBuilder = function ($focusable, aControl) {
            var map = [], startIndex = 0;
            $focusable.each(function (index) {
                var $el = $(this);
                var item = {
                    $: $el
                };
                if (index) {
                    if ($el.hasClass(cssFocused)) {
                        startIndex = index;
                    }
                    var prevIndex = index - 1;
                    var prevItem = map[index - 1];
                    prevItem.r = index;
                    item.l = prevIndex;
                }
                map.push(item);
            });
            return {
                map: map,
                startIndex: startIndex
            };
        };
        var rightToLeftBuilder = function ($focusable, aControl) {
            var map = [], startIndex = 0;
            $focusable.each(function (index) {
                var $el = $(this);
                var item = {
                    $: $el
                };
                if (index) {
                    if ($el.hasClass(cssFocused)) {
                        startIndex = index;
                    }
                    var prevIndex = index - 1;
                    var prevItem = map[index - 1];
                    prevItem.l = index;
                    item.r = prevIndex;
                }
                map.push(item);
            });
            return {
                map: map,
                startIndex: startIndex
            };
        };
        var topToDownBuilder = function ($focusable, aControl) {
            var map = [], startIndex = 0;
            $focusable.each(function (index) {
                var $el = $(this);
                var item = {
                    $: $el
                };
                if (index) {
                    if ($el.hasClass(cssFocused)) {
                        startIndex = index;
                    }
                    var prevIndex = index - 1;
                    var prevItem = map[index - 1];
                    prevItem.d = index;
                    item.u = prevIndex;
                }
                map.push(item);
            });
            return {
                map: map,
                startIndex: startIndex
            };
        };
        var gridBuilder = function ($focusable, aControl) {
            var map = [];
            var posY = -1;
            var startIndex = 0;
            var rowCount = 0;
            var colCount = 0;
            $focusable.each(function (index) {
                var $focusable = $(this);
                if ($focusable.hasClass(cssFocused)) {
                    startIndex = index;
                }
                var pos = $focusable.position();
                if (index === 0) {
                    map.push({
                        $: $focusable
                    });
                    posY = pos.top;
                    return;
                }
                var item = {
                    $: $focusable
                };
                var top = Number($focusable.css('top').replace('px', ''));
                if (posY == top) {
                    var prevIndex = index - 1;
                    var prevItem = map[prevIndex];
                    prevItem.r = index;
                    item.l = prevIndex;
                }
                else {
                    posY = top;
                    if (!colCount) {
                        colCount = index;
                    }
                    rowCount += 1;
                }
                if (rowCount) {
                    var aboveIndex = index - colCount;
                    var aboveItem = map[aboveIndex];
                    aboveItem.d = index;
                    item.u = aboveIndex;
                }
                map.push(item);
            });
            return {
                map: map,
                startIndex: startIndex
            };
        };
        var horizontalGridBuilder = function ($focusable, aControl) {
            var map = [];
            var posX = -1;
            var startIndex = 0;
            var rowCount = 0;
            var colCount = 0;
            var drawParam = aControl ? aControl.getDrawParam() : null;
            $focusable.each(function (index) {
                var $focusable = $(this);
                if ($focusable.hasClass(cssFocused)) {
                    startIndex = index;
                }
                var pos = $focusable.position();
                if (index === 0) {
                    map.push({
                        $: $focusable
                    });
                    posX = pos.left;
                    return;
                }
                var item = {
                    $: $focusable
                };
                var left = Number($focusable.css('left').replace('px', ''));
                if (posX == left) {
                    var aboveIndex = index - 1;
                    var aboveItem = map[aboveIndex];
                    aboveItem.d = index;
                    item.u = aboveIndex;
                }
                else {
                    posX = left;
                    if (!rowCount) {
                        rowCount = index;
                    }
                    colCount += 1;
                }
                if (colCount) {
                    var leftIndex = index - rowCount;
                    var leftItem = map[leftIndex];
                    leftItem.r = index;
                    item.l = leftIndex;
                }
                map.push(item);
            });
            return {
                map: map,
                startIndex: startIndex
            };
        };
        var changeFocus = function (a, $old, $new, leaf) {
            var cssActive = leaf ? cssActiveFocusedLeaf : cssActiveFocused;
            $old.removeClass(cssFocused + ' ' + cssActive);
            $new.addClass(cssFocused + ' ' + cssActive);
            // TODO: find better way to report;
            if (a._onChildFocusChanged) {
                a._onChildFocusChanged($old.attr('data'), $new.attr('data'));
            }
            else {
                /*
                 if (a.onFocusChanged) {
                 a.onFocusChanged($old, $new);
                 }
                 */
                a._fireEvent('FocusChanged', $old, $new);
            }
        };
        var marqueeOn = function ($item, selector) {
            if ($item && $item.find('marquee').length === 0) {
                var $marquee = $('<marquee>');
                $marquee.append($item.find(selector));
                $item.append($marquee);
            }
        };
        var marqueeOff = function ($item, selector) {
            if ($item) {
                var $marquee = $item.find('marquee');
                $item.append($marquee.find(selector));
                $marquee.remove();
            }
        };
        var fnFindReceiverInfo = function (a, eventName, receiver) {
            var receivers, i, receiverInfo, len;
            if (a._eventReceivers) {
                receivers = a._eventReceivers[eventName];
                len = receivers.length;
            }
            if (receivers) {
                for (i = 0; i < len; i++) {
                    receiverInfo = receivers[i];
                    if (receiverInfo.receiver == receiver) {
                        return receiverInfo;
                    }
                }
            }
            return null;
        };
        var CEventSource = (function () {
            function CEventSource() {
            }
            CEventSource.prototype._registerEvent = function (eventList) {
                var a = this, i, eventName;
                var eventReceivers = a._eventReceivers || {};
                for (i = 0; i < eventList.length; i += 1) {
                    eventName = eventList[i];
                    if (eventReceivers[eventName]) {
                        console.error('EVENT ALREADY DEFINED');
                        throw "EVENT ALREADY DEFINED";
                    }
                    eventReceivers[eventName] = [];
                }
                a._eventReceivers = eventReceivers;
            };
            CEventSource.prototype.destroy = function () {
                var a = this;
                a._eventReceivers = null;
            };
            CEventSource.prototype.addEventHandler = function (eventName, receiver, handler) {
                if (arguments.length === 2) {
                    throw 'DEPRECATED RECEIVER REQUIRED FOR REMOVAL';
                }
                var a = this, receiverInfo = fnFindReceiverInfo(a, eventName, receiver);
                if (receiverInfo) {
                    receiverInfo.handlers.push(handler);
                }
                else {
                    if (a._eventReceivers) {
                        a._eventReceivers[eventName].push({
                            receiver: receiver,
                            handlers: [handler]
                        });
                    }
                }
            };
            CEventSource.prototype.removeEventHandler = function (receiver, eventName) {
                var a = this;
                if (a._eventReceivers) {
                    var receivers = a._eventReceivers[eventName];
                    var removeReceiver = function (receivers, receiver) {
                        var i, len = receivers.length, receiverInfo, index = -1;
                        for (i = 0; i < len; i += 1) {
                            receiverInfo = receivers[i];
                            if (receiverInfo.receiver === receiver) {
                                delete receiverInfo.receiver;
                                delete receiverInfo.handlers;
                                index = i;
                            }
                        }
                        if (index !== -1) {
                            receivers.splice(index, 1);
                            return true;
                        }
                        return false;
                    }, key;
                    if (receivers) {
                        removeReceiver(receivers, receiver);
                    }
                    else {
                        for (key in a._eventReceivers) {
                            receivers = a._eventReceivers[key];
                            removeReceiver(receivers, receiver);
                        }
                    }
                }
            };
            CEventSource.prototype._fireEvent = function () {
                var aArgs = [];
                for (var _i = 0; _i < arguments.length; _i++) {
                    aArgs[_i - 0] = arguments[_i];
                }
                var a = this, args = $.makeArray(arguments), eventName = args.shift(), receivers = a._eventReceivers[eventName], len = receivers.length, receiverInfo, i, j, len2, receiver, handlers;
                for (i = 0; i < len; i += 1) {
                    receiverInfo = receivers[i];
                    if (receiverInfo) {
                        receiver = receiverInfo.receiver;
                        handlers = receiverInfo.handlers;
                        len2 = handlers.length;
                        for (j = 0; j < len2; j += 1) {
                            handlers[j].apply(receiver, args);
                        }
                    }
                }
                if (a['on' + eventName]) {
                    a['on' + eventName].apply(a, args);
                }
            };
            return CEventSource;
        })();
        mControl.CEventSource = CEventSource;
        var keyHandlersNaviMap = {};
        keyHandlersNaviMap[KKeyUp] = 'u';
        keyHandlersNaviMap[KKeyDown] = 'd';
        keyHandlersNaviMap[KKeyLeft] = 'l';
        keyHandlersNaviMap[KKeyRight] = 'r';
        keyHandlersNaviMap[KKeyPageUp] = 'pu';
        keyHandlersNaviMap[KKeyPageDown] = 'pd';
        var doNavi = function (a, direction) {
            if (a._keyMap && direction) {
                if (a._currentIndex !== -1) {
                    var nextIndex, nextItem, tempIndex, tempItem, currentItem;
                    var isPageMove = direction.indexOf('p') === 0;
                    var drawParam = a.getDrawParam();
                    direction = direction.replace('p', '');
                    currentItem = tempItem = a._keyMap[a._currentIndex];
                    if (isPageMove && drawParam.scrollPageSizeByPageUpDownKey) {
                        var cssActive = !a._controls ? cssActiveFocusedLeaf : cssActiveFocused;
                        if (currentItem) {
                            currentItem.$.removeClass(cssActive);
                        }
                        return;
                    }
                    nextIndex = tempIndex = currentItem[direction];
                    if (nextIndex !== undefined) {
                        if (direction && isPageMove) {
                            while (tempIndex !== undefined) {
                                nextIndex = tempIndex;
                                tempItem = a._keyMap[tempIndex];
                                tempIndex = tempItem[direction];
                            }
                        }
                        var $old = currentItem.$;
                        nextItem = a._keyMap[nextIndex];
                        var $new = nextItem.$;
                        a._currentIndex = nextIndex;
                        changeFocus(a, $old, $new, !a._controls);
                        return true;
                    }
                }
            }
            return false;
        };
        var CControl = (function (_super) {
            __extends(CControl, _super);
            function CControl() {
                _super.call(this);
                this.CONST = {
                    'cssPropTransition': cssPropTransition,
                    'cssEvntTransitionEnd': cssEvntTransitionEnd
                };
                this._currentIndex = -1;
                this._drawParam = {};
                this._volitileParam = [];
                this._keyMapBuilder = null;
                this.onItemSelected = null;
            }
            CControl.prototype.doShow = function (aParam) {
            };
            CControl.prototype.init = function ($el) {
                var a = this;
                a._$self = $el || $('<div>');
                a._$self.css('position', 'absolute');
                a._registerEvent(['FocusChanged', 'FocusGained', 'FocusLost', 'ActiveFocusedGained', 'ActiveFocusedLost']);
            };
            CControl.prototype.$self = function () {
                return this._$self;
            };
            CControl.prototype.doDraw = function (aParam) {
                throw 'inherit';
                return $(this);
            };
            CControl.prototype.addVolitileParam = function (paramNameList) {
                var a = this;
                paramNameList = paramNameList instanceof Array ? paramNameList : [paramNameList];
                a._volitileParam = a._volitileParam.concat(paramNameList);
            };
            CControl.prototype.destroy = function () {
                var a = this;
                a._$self.remove();
                a._keyMap = null;
                CEventSource.prototype.destroy.call(a);
            };
            CControl.prototype.doNavi = function (naviHandler) {
                var a = this;
                return doNavi(a, naviHandler);
            };
            CControl.prototype.doKey = function (keyCode) {
                var a = this;
                if (a._transitioning) {
                    return true;
                }
                var handler = keyHandlers[keyCode];
                var debugHandler = handler + "Debug";
                var handled = false;
                if (a[debugHandler]) {
                }
                if (!a['_onChildFocusChanged'] && handler && a[handler]) {
                    handled = a[handler](keyCode);
                }
                else {
                    handled = doNavi(a, keyHandlersNaviMap[keyCode]);
                }
                if (!handled && 48 <= keyCode && keyCode <= 57 && a['doKeyNumber']) {
                    handled = a['doKeyNumber'](keyCode - 48);
                }
                return handled;
            };
            CControl.prototype.doKeyOk = function (aKeyCode) {
                var ret = false;
                if (this.onItemSelected) {
                    var $item = this.getFocusedItem();
                    if ($item) {
                        var index = parseInt($item.attr('data'), 10);
                        this.onItemSelected(index, $item);
                        ret = true;
                    }
                }
                return ret;
            };
            CControl.prototype.isFocusable = function () {
                return true;
            };
            CControl.prototype.hasFocus = function () {
                var a = this;
                return a._$self.hasClass(cssFocused);
            };
            CControl.prototype.setFocus = function (focus) {
                var a = this;
                if (focus) {
                    a._$self.addClass(cssFocused);
                }
                else {
                    a._$self.removeClass(cssFocused);
                }
            };
            CControl.prototype.setFocusedItem = function (index, force) {
                var a = this;
                if (index !== a._currentIndex || force) {
                    if (a._keyMap) {
                        var $old = a._keyMap[a._currentIndex].$;
                        var $new;
                        if (a._keyMap[index]) {
                            $new = a._keyMap[index].$;
                            a._currentIndex = index;
                        }
                        if ($old && $new) {
                            changeFocus(a, $old, $new, true);
                        }
                    }
                }
            };
            CControl.prototype.getFocusedItem = function () {
                var a = this, $ret = null;
                if (a._keyMap && a._keyMap[a._currentIndex]) {
                    $ret = a._keyMap[a._currentIndex].$;
                }
                return $ret;
            };
            CControl.prototype.$focusedItem = function () {
                var a = this, $ret = null;
                var currentItem = null;
                if (a._keyMap) {
                    currentItem = a._keyMap[a._currentIndex];
                    if (currentItem) {
                        $ret = currentItem.$;
                    }
                }
                return $ret;
            };
            CControl.prototype.setFocusable = function (focusable) {
                var a = this;
                if (focusable) {
                    a._$self.addClass(cssFocusable);
                }
                else {
                    a._$self.removeClass(cssFocusable);
                }
            };
            CControl.prototype.setActiveFocus = function (activeFocus) {
                var a = this;
                if (a._keyMap) {
                    if (activeFocus) {
                        a._$self.addClass(cssActiveFocused);
                        a._keyMap[a._currentIndex].$.addClass(cssFocused + ' ' + cssActiveFocusedLeaf);
                        a._fireEvent('ActiveFocusedGained', a._keyMap[a._currentIndex].$);
                    }
                    else {
                        a._keyMap[a._currentIndex].$.removeClass(cssFocused + ' ' + cssActiveFocusedLeaf);
                        a._$self.removeClass(cssActiveFocused);
                        a._fireEvent('ActiveFocusedLost', a._keyMap[a._currentIndex].$);
                    }
                }
            };
            CControl.prototype.getActiveFocusInfo = function () {
                var a = this;
                var $item = a.$focusedItem();
                if ($item && $item.hasClass(cssActiveFocusedLeaf)) {
                    return {
                        control: a,
                        '$item': $item
                    };
                }
                return null;
            };
            CControl.prototype.changeFocus = function ($item) {
                var a = this, i;
                if (a._keyMap) {
                    for (i = 0; i < a._keyMap.length; i += 1) {
                        var item = a._keyMap[i];
                        if (item.$.get(0) === $item.get(0)) {
                            changeFocus(a, a._keyMap[a._currentIndex].$, item.$, !a._controls);
                            a._currentIndex = i;
                            return true;
                        }
                    }
                }
                return false;
            };
            CControl.prototype.hasNaviMap = function () {
                var a = this;
                return a._keyMap ? true : false;
            };
            CControl.prototype.draw = function (param) {
                var a = this, executeAfterDraw = [], i, drawPram = a.getDrawParam();
                param = param || {};
                if (param.visibleAfterDraw || drawPram.visibleAfterDraw) {
                    a._$self.css('visibility', 'hidden');
                    executeAfterDraw.push(function () {
                        a._$self.css('visibility', '');
                    });
                    delete param.visibleAfterDraw;
                }
                if (drawPram.align === 'center') {
                    a._$self.css('visibility', 'hidden');
                    executeAfterDraw.push(function () {
                        var $parent = a._$self.parent(), parentWidth = $parent.width(), parentHeight = $parent.height(), width = a._$self.width(), height = a._$self.height();
                        if (width < parentWidth && height < parentHeight) {
                            a._$self.css({
                                'top': Math.floor((parentHeight - height) / 2) + 'px',
                                'left': Math.floor((parentWidth - width) / 2) + 'px',
                                'visibility': ''
                            });
                        }
                    });
                }
                else if (drawPram.align === 'hcenter') {
                    a._$self.css('visibility', 'hidden');
                    executeAfterDraw.push(function () {
                        var $parent = a._$self.parent(), parentWidth = $parent.width(), width = a._$self.width();
                        if (width <= parentWidth) {
                            a._$self.css({
                                'left': Math.floor((parentWidth - width) / 2) + 'px',
                                'visibility': ''
                            });
                        }
                    });
                }
                if (drawPram.$parent) {
                    a._$self.appendTo(drawPram.$parent);
                }
                if (param.setActiveFocus || param.bReDraw) {
                    executeAfterDraw.push(function () {
                        a.setActiveFocus(true);
                    });
                    delete param.setActiveFocus;
                }
                if (param.after) {
                    executeAfterDraw.push(param.after);
                }
                if (!a._$self.parent().length) {
                    return;
                }
                var ret = a.doDraw(param);
                var keyMapInfo = null;
                if (ret.length) {
                    keyMapInfo = a._keyMapBuilder(ret, a);
                    a._keyMap = keyMapInfo.map;
                    a._currentIndex = keyMapInfo.startIndex;
                }
                else {
                    a._keyMap = null;
                    a._currentIndex = -1;
                }
                for (i = 0; i < executeAfterDraw.length; i += 1) {
                    executeAfterDraw[i]();
                }
                delete a._drawParam.applyOnce;
            };
            CControl.prototype.setId = function (idName) {
                var a = this;
                a._$self.attr('id', idName);
            };
            CControl.prototype.addClass = function (classNames) {
                this._$self.addClass(classNames);
                return this;
            };
            CControl.prototype.removeClass = function (classNames) {
                this._$self.removeClass(classNames);
                return this;
            };
            CControl.prototype.isShown = function () {
                var a = this;
                return a._shown;
            };
            CControl.prototype.show = function (param) {
                var a = this;
                if (!a._shown) {
                    if (a.doShow) {
                        a.doShow(param);
                    }
                    a._shown = true;
                }
            };
            CControl.prototype.hide = function (param) {
                var a = this;
                if (a._shown) {
                    if (a.doHide) {
                        a.doHide(param);
                    }
                    a._shown = false;
                }
            };
            CControl.prototype.doHide = function (param) {
            };
            CControl.prototype.setTransition = function (transition) {
                var a = this;
                a._transitioning = transition;
            };
            CControl.prototype.isTransitioning = function () {
                var a = this;
                return a._transitioning ? true : false;
            };
            CControl.prototype.setDrawParam = function (drawParam, value, volitile) {
                var a = this, key;
                if (arguments.length > 1) {
                    if (volitile || a._volitileParam.indexOf(drawParam) !== -1) {
                        a._drawParam.applyOnce = a._drawParam.applyOnce || {};
                        a._drawParam.applyOnce[drawParam] = value;
                    }
                    else {
                        a._drawParam[drawParam] = value;
                    }
                }
                else {
                    for (key in drawParam) {
                        if (drawParam.hasOwnProperty(key)) {
                            if (a._volitileParam.indexOf(key) !== -1) {
                                a._drawParam.applyOnce = a._drawParam.applyOnce || {};
                                a._drawParam.applyOnce[key] = drawParam[key];
                            }
                            else {
                                a._drawParam[key] = drawParam[key];
                            }
                        }
                    }
                }
            };
            CControl.prototype.getDrawParam = function (paramName) {
                var a = this;
                var ret = null, key;
                if (a._drawParam) {
                    ret = {};
                    for (key in a._drawParam) {
                        if (a._drawParam.hasOwnProperty(key)) {
                            ret[key] = a._drawParam[key];
                        }
                    }
                    if (ret.applyOnce) {
                        ret.applyOnce = null;
                        delete ret.applyOnce;
                        if (a._drawParam.applyOnce) {
                            for (key in a._drawParam.applyOnce) {
                                if (a._drawParam.applyOnce.hasOwnProperty(key)) {
                                    ret[key] = a._drawParam.applyOnce[key];
                                }
                            }
                        }
                    }
                    if (paramName) {
                        ret = ret[paramName];
                    }
                }
                return ret;
            };
            CControl.prototype.getMargins = function () {
                var a = this, margins = a._drawParam.margins || [], ret = {
                    t: 0,
                    r: 0,
                    b: 0,
                    l: 0
                }, i;
                if (margins.length) {
                    ret.t = margins[0];
                    ret.r = margins[1] || ret.t;
                    ret.b = margins[2] || ret.t;
                    ret.l = margins[3] || ret.r;
                }
                return ret;
            };
            CControl.prototype.getWidth = function () {
                var a = this;
                return a._$self.outerWidth();
            };
            CControl.prototype.getHeight = function () {
                var a = this;
                return a._$self.outerHeight();
            };
            CControl.prototype.getTop = function () {
                var a = this;
                return a._$self.position().top;
            };
            CControl.prototype.getLeft = function () {
                var a = this;
                return a._$self.position().left;
            };
            CControl.prototype.setActiveIndex = function (index) {
                var a = this;
                a._currentIndex = index;
            };
            return CControl;
        })(CEventSource);
        mControl.CControl = CControl;
        var CGroupControl = (function (_super) {
            __extends(CGroupControl, _super);
            function CGroupControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CGroupControl.prototype.init = function ($el) {
                _super.prototype.init.call(this, $el);
                this._keyMapBuilder = leftToRightBuilder;
            };
            CGroupControl.prototype.destroy = function () {
                var a = this, i;
                var len = a._controls ? a._controls.length : 0;
                for (i = 0; i < len; i += 1) {
                    var c = a._controls[i];
                    c.destroy();
                }
                CControl.prototype.destroy.call(a);
            };
            CGroupControl.prototype.destroyChildControl = function () {
                var a = this, i, c, len = a._controls ? a._controls.length : 0;
                for (i = 0; i < len; i += 1) {
                    c = a._controls[i];
                    c.destroy();
                }
                a._controls = [];
            };
            CGroupControl.prototype.setChildControl = function (controls, focusIndex, RemovePrevFocused) {
                var a = this, i, c, $c, len = controls.length;
                focusIndex = focusIndex || 0;
                if (len) {
                    for (i = 0; i < len; i += 1) {
                        c = controls[i];
                        $c = c._$self;
                        if (RemovePrevFocused) {
                            $c.removeClass(cssFocused);
                        }
                        $c.addClass(cssFocusable);
                        $c.attr('data', i);
                        if ($c.parent().length === 0) {
                            if (a._$container) {
                                a._$container.append($c);
                            }
                            else {
                                a._$self.append($c);
                            }
                        }
                    }
                    controls[focusIndex]._$self.addClass(cssFocused);
                }
                a._controls = controls;
            };
            CGroupControl.prototype._insertChildControl = function ($target, index, control, setFocus) {
                var a = this, controls = a._controls, len = controls.length + 1, i, prevFocusIndex;
                controls.splice(index, 0, control);
                if (len) {
                    for (i = 0; i < len; i += 1) {
                        var c = controls[i];
                        var $c = c._$self;
                        $c.addClass(cssFocusable);
                        $c.attr('data', i);
                        if ($c.parent().length === 0) {
                            if (a._$container) {
                                a._$container.append($c);
                            }
                            else {
                                $target.append($c);
                            }
                        }
                        if ($c.hasClass(cssFocused)) {
                            prevFocusIndex = i;
                        }
                    }
                    if (setFocus) {
                        controls[prevFocusIndex]._$self.removeClass(cssFocused);
                        controls[prevFocusIndex].setActiveFocus(false);
                        controls[index]._$self.addClass(cssFocused);
                    }
                }
                a._controls = controls;
            };
            CGroupControl.prototype.insertChildControl = function (index, control, setFocus) {
                var a = this;
                a._insertChildControl(a._$self, index, control, setFocus);
            };
            CGroupControl.prototype.removeChildControl = function (control) {
                var a = this, controls = a._controls, i, len = controls ? controls.length : 0, c, $c;
                for (i = 0; i < len; i += 1) {
                    c = controls[i];
                    if (c === control) {
                        controls.splice(i, 1);
                        for (i = 0, len = controls.length; i < len; i += 1) {
                            c = controls[i];
                            $c = c._$self;
                            $c.attr('data', i);
                        }
                        return true;
                    }
                }
                return false;
            };
            CGroupControl.prototype._onChildFocusChanged = function (oldIndex, newIndex) {
                var a = this;
                var oldChild = a._controls[oldIndex];
                var newChild = a._controls[newIndex];
                oldChild.setActiveFocus(false);
                newChild.setActiveFocus(true);
                oldChild._fireEvent('FocusLost');
                newChild._fireEvent('FocusGained');
                a._fireEvent('FocusChanged', oldChild, newChild);
            };
            CGroupControl.prototype.initActiveFocus = function () {
                var a = this;
                var child = a.getFocusedChild();
                child.setActiveFocus(true);
            };
            CGroupControl.prototype.setActiveFocus = function (activeFocus) {
                var a = this;
                if (a._keyMap) {
                    var child = a.getFocusedChild();
                    if (activeFocus) {
                        a._$self.addClass(cssActiveFocused);
                        child.setActiveFocus(true);
                    }
                    else {
                        child.setActiveFocus(false);
                        a._$self.removeClass(cssActiveFocused);
                    }
                }
            };
            CGroupControl.prototype.getFocusedChild = function () {
                var a = this;
                if (a._keyMap) {
                    return a._controls[a._keyMap[a._currentIndex].$.attr('data')];
                }
                return null;
            };
            CGroupControl.prototype.getFocusedChildIndex = function () {
                var a = this;
                if (a._keyMap) {
                    return parseInt(a._keyMap[a._currentIndex].$.attr('data'), 10);
                }
                return -1;
            };
            CGroupControl.prototype.getActiveFocusInfo = function () {
                var a = this, i;
                for (i = 0; i < a._controls.length; i += 1) {
                    var c = a._controls[i];
                    var activeFocusInfo = c.getActiveFocusInfo();
                    if (activeFocusInfo) {
                        return activeFocusInfo;
                    }
                }
                return null;
            };
            CGroupControl.prototype.setFocusedChld = function (control) {
                var a = this, i, $item = control._$self;
                for (i = 0; i < a._keyMap.length; i += 1) {
                    var item = a._keyMap[i];
                    if (item.$.get(0) === $item.get(0)) {
                        changeFocus(a, a._keyMap[a._currentIndex].$, item.$);
                        a._currentIndex = i;
                        return true;
                    }
                }
                return false;
            };
            CGroupControl.prototype.doKey = function (keyCode) {
                var a = this, i, handled;
                var controls = a._controls ? a._controls.slice(0) : null;
                if (a._transitioning) {
                    hx.log('debug', 'a._transitioning');
                    return true;
                }
                var handler = keyHandlers[keyCode];
                if (handler && a[handler]) {
                    if (a[handler](keyCode)) {
                        return true;
                    }
                }
                if (controls) {
                    for (i = 0; i < controls.length; i += 1) {
                        var c = controls[i];
                        if (c.hasFocus() && c.doKey(keyCode)) {
                            return true;
                        }
                    }
                }
                handled = CControl.prototype.doKey.call(a, keyCode);
                if (!handled) {
                    handler += 'IfNotHandled';
                    if (handler && a[handler]) {
                        handled = a[handler](keyCode);
                    }
                }
                return handled;
            };
            CGroupControl.prototype.isFocusable = function () {
                var a = this, i;
                var len = a._controls ? a._controls.length : 0;
                for (i = 0; i < len; i += 1) {
                    var c = a._controls[i];
                    if (c.isFocusable()) {
                        return true;
                    }
                }
                return false;
            };
            CGroupControl.prototype.doDraw = function (param) {
                var a = this, i;
                var len = a._controls ? a._controls.length : 0;
                var focusableChild = [];
                for (i = 0; i < len; i += 1) {
                    var c = a._controls[i];
                    c.draw(param);
                    if (c.isFocusable()) {
                        focusableChild.push(c._$self);
                    }
                }
                return $(focusableChild);
            };
            return CGroupControl;
        })(CControl);
        mControl.CGroupControl = CGroupControl;
        var CHorizontalGroupControl = (function (_super) {
            __extends(CHorizontalGroupControl, _super);
            function CHorizontalGroupControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CHorizontalGroupControl.prototype.init = function ($el) {
                _super.prototype.init.call(this, $el);
                this._keyMapBuilder = leftToRightBuilder;
            };
            CHorizontalGroupControl.prototype.doDraw = function (param) {
                var a = this, i = 0, c, $c, pos, len = a._controls ? a._controls.length : 0, focusableChild = [], maxHeight = 0, drawParam = a.getDrawParam(), margins = a.getMargins(), left = margins.l, padding = drawParam.padding || 0, height, width, top;
                for (i = 0; i < len; i += 1) {
                    c = a._controls[i];
                    c.draw(param);
                    $c = c._$self;
                    pos = $c.position();
                    if (!$c.position().left) {
                        $c.css('left', left + 'px');
                    }
                    if (margins.t) {
                        $c.css('top', margins.t + 'px');
                    }
                    width = $c.outerWidth();
                    left += (width + padding);
                    height = $c.height();
                    if (c.isFocusable()) {
                        focusableChild.push($c);
                    }
                    maxHeight = maxHeight < height ? height : maxHeight;
                }
                if (len) {
                    left -= padding;
                }
                width = a._$self.width();
                if (width === 0) {
                    width = left + margins.r;
                    a._$self.css('width', width + 'px');
                }
                height = a._$self.height();
                if (height === 0) {
                    a._$self.css('height', maxHeight + 'px');
                    height = maxHeight;
                }
                if (drawParam.childAlign === 'center') {
                    left = Math.floor((width - left) / 2);
                    for (i = 0; i < len; i += 1) {
                        $c = a._controls[i]._$self;
                        $c.css('left', left + 'px');
                        top = Math.floor((height - $c.height()) / 2);
                        $c.css('top', top + 'px');
                        left += ($c.outerWidth() + padding);
                    }
                }
                else if (drawParam.childAlign === 'vcenter') {
                    for (i = 0; i < len; i += 1) {
                        $c = a._controls[i]._$self;
                        top = Math.floor((height - $c.height()) / 2);
                        $c.css('top', top + 'px');
                    }
                }
                return $(focusableChild);
            };
            return CHorizontalGroupControl;
        })(CGroupControl);
        mControl.CHorizontalGroupControl = CHorizontalGroupControl;
        var CVerticalGroupControl = (function (_super) {
            __extends(CVerticalGroupControl, _super);
            function CVerticalGroupControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CVerticalGroupControl.prototype.init = function ($el) {
                _super.prototype.init.call(this, $el);
                this._keyMapBuilder = topToDownBuilder;
            };
            CVerticalGroupControl.prototype.doDraw = function (param) {
                var a = this, i = 0, c, $c, pos, len = a._controls ? a._controls.length : 0, focusableChild = [], maxWidth = 0, drawParam = a.getDrawParam(), margins = a.getMargins(), top = margins.t, padding = drawParam.padding || 0, height, width, left;
                for (i = 0; i < len; i += 1) {
                    c = a._controls[i];
                    c.draw(param);
                    $c = c._$self;
                    pos = $c.position();
                    if (!pos.top) {
                        $c.css('top', top + 'px');
                    }
                    if (margins.l) {
                        $c.css('left', margins.l + 'px');
                    }
                    height = $c.outerHeight();
                    top += (height + padding);
                    width = $c.width();
                    if (c.isFocusable()) {
                        focusableChild.push($c);
                    }
                    maxWidth = maxWidth < width ? width : maxWidth;
                }
                if (len) {
                    top -= padding;
                }
                width = a._$self.width();
                if (width === 0) {
                    a._$self.css('width', maxWidth + 'px');
                    width = maxWidth;
                }
                height = a._$self.height();
                if (height === 0) {
                    height = top + margins.b;
                    a._$self.css('height', height + 'px');
                }
                if (drawParam.childAlign === 'center') {
                    top = Math.floor((height - top) / 2);
                    for (i = 0; i < len; i += 1) {
                        $c = a._controls[i]._$self;
                        $c.css('top', top + 'px');
                        left = Math.floor((width - $c.width()) / 2);
                        $c.css('left', left + 'px');
                        top += ($c.outerHeight() + padding);
                    }
                }
                else if (drawParam.childAlign === 'hcenter') {
                    for (i = 0; i < len; i += 1) {
                        $c = a._controls[i]._$self;
                        left = Math.floor((width - $c.width()) / 2);
                        $c.css('left', left + 'px');
                    }
                }
                return $(focusableChild);
            };
            CVerticalGroupControl.prototype.doKey = function (keyCode) {
                console.log("c vertical right");
                var a = this;
                var drawParam = a.getDrawParam();
                var handled = CGroupControl.prototype.doKey.call(a, keyCode);
                var fnSetFocusedItem = function (parent, keyCode) {
                    var i, length, control, controls;
                    controls = parent._controls;
                    if (controls) {
                        length = controls.length;
                        if (keyCode === KKeyDown) {
                            for (i = 0; i < length; i += 1) {
                                control = controls[i];
                                if (control._keyMap) {
                                    control.setFocusedItem(0);
                                    fnSetFocusedItem(control, keyCode);
                                    break;
                                }
                            }
                        }
                        else if (keyCode === KKeyUp) {
                            for (i = length - 1; i >= 0; i -= 1) {
                                control = controls[i];
                                if (control._keyMap) {
                                    control.setFocusedItem(control._keyMap.length - 1);
                                    fnSetFocusedItem(control, keyCode);
                                    break;
                                }
                            }
                        }
                    }
                };
                var fnGetFocusedChild = function (control) {
                    var focusedChild = control;
                    if (control.getFocusedChild) {
                        focusedChild = control.getFocusedChild();
                        return fnGetFocusedChild(focusedChild);
                    }
                    return focusedChild;
                };
                if (drawParam.setFocusRolling && !handled) {
                    if (keyCode === KKeyDown) {
                        a.setFocusedItem(0);
                    }
                    else if (keyCode === KKeyUp) {
                        var child = fnGetFocusedChild(a);
                        a.setFocusedItem(a._keyMap.length - 1);
                        if (a._keyMap.length > 1) {
                            child._currentIndex = child._keyMap.length - 1;
                        }
                    }
                    fnSetFocusedItem(a, keyCode);
                }
                return handled;
            };
            CVerticalGroupControl.prototype.setCommonTag = function (commonTagInfo) {
                var a = this, key;
                for (key in commonTagInfo) {
                    if (commonTagInfo.hasOwnProperty(key)) {
                        var className = commonTagInfo[key];
                        a['$' + key] = (function () {
                            var localClassName = className;
                            return function () {
                                var tag = a._$self ? a._$self.children('.' + localClassName) : null;
                                if (tag && tag.length === 0) {
                                    tag = $('<div>', {
                                        'class': localClassName
                                    });
                                    a._$self.append(tag);
                                }
                                return tag;
                            };
                        }());
                    }
                }
            };
            return CVerticalGroupControl;
        })(CGroupControl);
        mControl.CVerticalGroupControl = CVerticalGroupControl;
        function getIntersectedRect(a, b) {
            var x0 = Math.max(a.left, b.left);
            var x1 = Math.min(a.right, b.right);
            if (x0 <= x1) {
                var y0 = Math.max(a.top, b.top);
                var y1 = Math.min(a.bottom, b.bottom);
                if (y0 <= y1) {
                    return {
                        left: x0,
                        top: y0,
                        right: x1,
                        bottom: y1
                    };
                }
            }
            return null;
        }
        function getIntersectedRect3(a, b, container, right) {
            var x0 = Math.min(a.left, b.left);
            var x1 = Math.min(a.right, b.right);
            if (right === 0) {
                x0 = x0 + container.left;
                x1 = x1 + container.left;
            }
            if (x0 <= x1) {
                var y0 = Math.max(a.top, b.top);
                var y1 = Math.min(a.bottom, b.bottom);
                if (y0 <= y1) {
                    return {
                        left: x0,
                        top: y0,
                        right: x1,
                        bottom: y1
                    };
                }
            }
            return null;
        }
        var tryMove = function (a, keyCode, child, basePos) {
            var increment = child.getNextAvail(keyCode);
            if (increment.top) {
                return moveTop(a, keyCode, (basePos || parseInt(-a._$container.position().top + "", 10)) + increment.top);
            }
            if (increment.left) {
                return moveLeft(a, keyCode, (basePos || parseInt(-a._$container.position().left + "", 10)) + increment.left);
            }
            if (increment.right) {
                var right = a._$container.css('right');
                return moveRight(a, keyCode, (basePos || (-parseInt(right, 10))) + (increment.right));
            }
        };
        var handleIncrement = function (a, keyCode) {
            var index = a.getFocusedChildIndex();
            var handled = false;
            if (index !== -1) {
                var child = a._controls[index];
                var drawParam = child.getDrawParam();
                var basePos = 0;
                var pagePos = 0;
                var itemHeight = child._drawParam.itemHeight;
                var selfHeight = a._$self.height();
                var containerHeight = a._$container.height();
                var itemCount = parseInt((selfHeight / itemHeight) + "", 10);
                var increment;
                handled = tryMove(a, keyCode, child, basePos);
                // TODO: Find proper way for moving across child.
                if (!handled) {
                    increment = child.getNextAvail(keyCode);
                    switch (keyCode) {
                        case KKeyUp:
                            while (index) {
                                index -= 1;
                                child = a._controls[index];
                                if (!child.isFocusable()) {
                                    basePos = child._$self.position().top;
                                    pagePos = Math.max(pagePos, 0);
                                    continue;
                                }
                                handled = tryMove(a, keyCode, child, basePos);
                            }
                            break;
                        case KKeyDown:
                            index += 1;
                            if (index < a._controls.length) {
                                handled = tryMove(a, keyCode, child, basePos);
                                if (!handled) {
                                    if (itemHeight !== undefined) {
                                        if (containerHeight > selfHeight) {
                                            pagePos = -a._$container.position().top + itemHeight;
                                            if (-pagePos !== a._$container.position().top) {
                                                moveTop(a, keyCode, pagePos);
                                                handled = true;
                                            }
                                        }
                                    }
                                }
                            }
                            break;
                        case KKeyLeft:
                            break;
                        case KKeyRight:
                            break;
                        case KKeyPageUp:
                            if (itemHeight !== undefined) {
                                var inViewItemHeight = (itemCount - 1) * itemHeight;
                                var outViewItemHeight = itemCount * itemHeight;
                                pagePos = -a._$container.position().top - (drawParam.scrollPageSizeByPageUpDownKey ? outViewItemHeight : inViewItemHeight);
                                pagePos = Math.max(pagePos, 0);
                                if (drawParam.scrollPageSizeByPageUpDownKey || (pagePos !== a._$container.position().top)) {
                                    moveTop(a, keyCode, pagePos);
                                    handled = true;
                                }
                            }
                            break;
                        case KKeyPageDown:
                            if (itemHeight !== undefined) {
                                if (containerHeight > selfHeight) {
                                    var inViewItemHeight = (itemCount - 1) * itemHeight;
                                    var outViewItemHeight = itemCount * itemHeight;
                                    pagePos = -a._$container.position().top + (drawParam.scrollPageSizeByPageUpDownKey ? outViewItemHeight : inViewItemHeight);
                                    pagePos = Math.min(pagePos, (containerHeight - outViewItemHeight));
                                    if (drawParam.scrollPageSizeByPageUpDownKey || (-pagePos !== a._$container.position().top)) {
                                        moveTop(a, keyCode, pagePos);
                                        handled = true;
                                    }
                                }
                                else {
                                    if (drawParam.scrollPageSizeByPageUpDownKey) {
                                        moveTop(a, keyCode, 0);
                                        handled = true;
                                    }
                                }
                            }
                            break;
                    }
                }
            }
            return handled;
        };
        var moveTop = function (a, keyCode, top, withoutAnim) {
            var ret = true;
            var activeFocusInfo = a.getActiveFocusInfo();
            var focusedChild = a.getFocusedChild();
            var drawParam = focusedChild ? focusedChild.getDrawParam() : {};
            a.draw({
                'top': top,
                setActiveFocus: activeFocusInfo ? true : false,
                bReDraw: (drawParam.scrollPageSizeByPageUpDownKey && (keyCode == KKeyPageUp || keyCode == KKeyPageDown)) ? true : false
            });
            if (keyCode && keyCode !== KKeyPageUp && keyCode !== KKeyPageDown) {
                ret = CGroupControl.prototype.doKey.call(a, keyCode);
            }
            if (withoutAnim) {
                a._$container.css(cssPropTransition, '');
                a._$container.css('top', -top + 'px');
                setTimeout(function () {
                    a.setTransition(false);
                    a._$container.css(cssPropTransition, a._containerTransitionParam);
                }, 1);
            }
            else {
                a.setTransition(true);
                a._$container.css('top', -top + 'px').afterTransitionOneShot(function () {
                    fnLog('transition finished');
                    if (a._saftDraw) {
                        a.draw();
                        delete a._saftDraw;
                    }
                    a.setTransition(false);
                });
            }
            return ret;
        };
        var moveLeft = function (a, keyCode, left, withoutAnim) {
            var ret = true;
            var activeFocusInfo = a.getActiveFocusInfo();
            a.draw({
                'left': left,
                setActiveFocus: activeFocusInfo ? true : false
            });
            if (keyCode) {
                ret = CGroupControl.prototype.doKey.call(a, keyCode);
            }
            if (withoutAnim) {
                a._$container.css(cssPropTransition, '');
                a._$container.css('left', -left + 'px');
                setTimeout(function () {
                    a.setTransition(false);
                    a._$container.css(cssPropTransition, a._containerTransitionParam);
                }, 1);
            }
            else {
                a.setTransition(true);
                a._$container.css('left', -left + 'px').afterTransitionOneShot(function () {
                    fnLog('transition finished');
                    if (a._saftDraw) {
                        a.draw();
                        delete a._saftDraw;
                    }
                    a.setTransition(false);
                });
            }
            return ret;
        };
        var moveRight = function (a, keyCode, right, withoutAnim) {
            var ret = true;
            var activeFocusInfo = a.getActiveFocusInfo();
            a.draw({
                'right': -right,
                setActiveFocus: activeFocusInfo ? true : false
            });
            if (keyCode) {
                ret = CGroupControl.prototype.doKey.call(a, keyCode);
            }
            if (withoutAnim) {
                a._$container.css(cssPropTransition, '');
                a._$container.css('right', -right + 'px');
                setTimeout(function () {
                    a.setTransition(false);
                    a._$container.css(cssPropTransition, a._containerTransitionParam);
                }, 1);
            }
            else {
                a.setTransition(true);
                a._$container.css('right', -right + 'px').afterTransitionOneShot(function () {
                    fnLog('transition finished');
                    if (a._saftDraw) {
                        a.draw();
                        delete a._saftDraw;
                    }
                    a.setTransition(false);
                });
            }
            return ret;
        };
        var CViewGroupControl = (function (_super) {
            __extends(CViewGroupControl, _super);
            function CViewGroupControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CViewGroupControl.prototype.init = function ($el) {
                var a = this, 
                /*
                 cssValue = {
                 'position' : 'absolute',
                 'top' : '0px',
                 'left' : '0px'
                 };
                 */
                // added
                cssValue = {
                    'position': 'absolute',
                    'top': '0px',
                    'right': '0px',
                    'left': '0px',
                };
                _super.prototype.init.call(a, $el);
                a._$self.addClass(cssPrefix + 'group-view');
                a._$self.css('overflow', 'hidden');
                //a._containerTransitionParam = 'translate3D(0, 0, 0) top .3s linear, left .3s linear';
                a._containerTransitionParam = 'top .3s linear, left .3s linear';
                cssValue[cssPropTransition] = a._containerTransitionParam;
                a._$container = $('<div>', {
                    'class': cssPrefix + 'group-container'
                }).css(cssValue);
                a._$self.append(a._$container);
                a._keyMapBuilder = topToDownBuilder;
            };
            CViewGroupControl.prototype.setChildControl = function (controls, focusIndex) {
                var a = this, i;
                _super.prototype.setChildControl.call(a, controls, focusIndex);
                for (i = 0; i < controls.length; i += 1) {
                    var c = controls[i];
                    if (c instanceof CDataControl) {
                        c._setParentViewGroupControl(a);
                    }
                    if (c._onDpItemChanged) {
                        var defaultChangedHandler = c._onDpItemChanged;
                        c._onDpItemChanged = (function (c, defaultChangedHandler) {
                            return function (keys) {
                                defaultChangedHandler.call(c, keys);
                                var position = a._$container.position();
                                a.draw({
                                    'top': -position.top,
                                    'left': -position.left
                                });
                            };
                        }(c, defaultChangedHandler));
                    }
                }
            };
            CViewGroupControl.prototype.updateViewParam = function (param) {
                var a = this, i, len, height, width, containerPos, drawParam, paramTop, paramLeft, paramRight, drawRect = {};
                len = a._controls ? a._controls.length : 0;
                height = Math.round(a._$self.height());
                width = Math.round(a._$self.width());
                containerPos = a._$container.position();
                drawParam = a.getDrawParam();
                paramTop = param.hasOwnProperty('top') ? param.top : (drawParam.hasOwnProperty('top') ? drawParam.top : -containerPos.top);
                paramLeft = param.hasOwnProperty('left') ? param.left : (drawParam.hasOwnProperty('left') ? drawParam.left : -containerPos.left);
                paramRight = param.hasOwnProperty('right') ? param.right : (drawParam.hasOwnProperty('right') ? drawParam.right : -containerPos.right);
                if (!paramRight) {
                    drawRect = {
                        top: paramTop,
                        bottom: paramTop + height,
                        left: paramLeft,
                        right: paramLeft + width
                    };
                }
                else {
                    drawRect = {
                        top: paramTop,
                        bottom: paramTop + height,
                        left: paramRight,
                        right: paramRight + width
                    };
                }
                a.setTransition(false);
                fnLog('CView.updateViewParam d: t:' + drawRect.top + ' l:' + drawRect.left + ' r:' + drawRect.right + ' b:' + drawRect.bottom);
                var topChild = 0;
                var leftChild = 0;
                for (i = 0; i < len; i += 1) {
                    var c = a._controls[i];
                    var childRect = {
                        top: topChild,
                        bottom: topChild + c.getHeight(),
                        left: leftChild,
                        right: leftChild + c.getWidth()
                    };
                    fnLog('CView.updateViewParam c: t:' + childRect.top + ' l:' + childRect.left + ' r:' + childRect.right + ' b:' + childRect.bottom);
                    var rectToDraw;
                    if (!paramRight && (paramLeft > 0)) {
                        rectToDraw = getIntersectedRect(drawRect, childRect);
                    }
                    else {
                        rectToDraw = getIntersectedRect3(drawRect, childRect, containerPos, paramRight);
                    }
                    if (rectToDraw) {
                        fnLog('CView.updateViewParam i: t:' + rectToDraw.top + ' l:' + rectToDraw.left + ' r:' + rectToDraw.right + ' b:' + rectToDraw.bottom);
                        var clientParam = {
                            'top': rectToDraw.top - topChild,
                            'left': rectToDraw.left - leftChild,
                            'bottom': rectToDraw.bottom - topChild,
                            'right': rectToDraw.right - leftChild
                        };
                        if (paramRight === 0) {
                            paramLeft = clientParam.left;
                        }
                        c.draw(clientParam);
                        drawRect.top = topChild;
                        drawRect.left = leftChild;
                    }
                    else {
                        c.draw({
                            'top': 0,
                            'left': 0,
                            'bottom': 0,
                            'right': 0
                        });
                    }
                }
            };
            CViewGroupControl.prototype.doDraw = function (param) {
                var a = this, i, len, $self, height, width, containerPos, drawParam, paramTop, paramLeft, paramRight, paramCheckedItems, withoutAnim, showScroll, drawRect = {};
                len = a._controls ? a._controls.length : 0;
                $self = a._$self;
                height = Math.round(a._$self.height());
                width = Math.round(a._$self.width());
                containerPos = a._$container.position();
                drawParam = a.getDrawParam();
                paramTop = param.hasOwnProperty('top') ? param.top : (drawParam.hasOwnProperty('top') ? drawParam.top : -containerPos.top);
                paramLeft = param.hasOwnProperty('left') ? param.left : (drawParam.hasOwnProperty('left') ? drawParam.left : -containerPos.left);
                paramRight = param.hasOwnProperty('right') ? param.right : (drawParam.hasOwnProperty('right') ? drawParam.right : -containerPos.right);
                //paramCheckedItems = param.hasOwnProperty('checkedItems') ? param.checkedItems : 0;
                withoutAnim = drawParam.withoutAnim;
                showScroll = drawParam.showScroll;
                if (!paramRight) {
                    drawRect = {
                        top: paramTop,
                        bottom: paramTop + height,
                        left: paramLeft,
                        right: paramLeft + width
                    };
                }
                else {
                    drawRect = {
                        top: paramTop,
                        bottom: paramTop + height,
                        left: paramRight,
                        right: paramRight + width
                    };
                }
                if (drawParam.showHidden) {
                    $self.css('overflow', 'visible');
                }
                a.setTransition(false);
                fnLog('CView.doDraw =============================');
                fnLog('CView.doDraw d: t:' + drawRect.top + ' l:' + drawRect.left + ' r:' + drawRect.right + ' b:' + drawRect.bottom);
                var topChild = 0;
                var leftChild = 0;
                var focusableItems = [];
                for (i = 0; i < len; i += 1) {
                    var c = a._controls[i];
                    var childRect = {
                        top: topChild,
                        bottom: topChild + c.getHeight(),
                        left: leftChild,
                        right: leftChild + c.getWidth()
                    };
                    fnLog('CView.doDraw c: t:' + childRect.top + ' l:' + childRect.left + ' r:' + childRect.right + ' b:' + childRect.bottom);
                    var rectToDraw;
                    if (!paramRight && (paramLeft > 0)) {
                        rectToDraw = getIntersectedRect(drawRect, childRect);
                    }
                    else {
                        rectToDraw = getIntersectedRect3(drawRect, childRect, containerPos, paramRight);
                    }
                    if (rectToDraw) {
                        fnLog('CView.doDraw i: t:' + rectToDraw.top + ' l:' + rectToDraw.left + ' r:' + rectToDraw.right + ' b:' + rectToDraw.bottom);
                        var clientParam = {
                            'top': rectToDraw.top - topChild,
                            'left': rectToDraw.left - leftChild,
                            'bottom': rectToDraw.bottom - topChild,
                            'right': rectToDraw.right - leftChild,
                            'bReDraw': param.bReDraw
                        };
                        if (paramRight === 0) {
                            paramLeft = clientParam.left;
                        }
                        c.draw(clientParam);
                        drawRect.top = topChild;
                        drawRect.left = leftChild;
                    }
                    else {
                        c.draw({
                            'top': 0,
                            'left': 0,
                            'bottom': 0,
                            'right': 0
                        });
                    }
                    var $el = c._$self;
                    $el.css({
                        'top': childRect.top + 'px'
                    });
                    if (c.hasNaviMap()) {
                        c.setFocusable(true);
                        focusableItems.push($el);
                    }
                    else {
                        c.setFocusable(false);
                    }
                    topChild = childRect.bottom;
                }
                if (containerPos.top === 0) {
                    a._$container.css({
                        'top': '0px',
                        'width': width + 'px',
                        // 'height' : top + 'px'
                        'height': topChild
                    });
                }
                if (this._saftDraw) {
                    a._$container.css({
                        'width': width + 'px',
                        'height': topChild
                    });
                }
                if (containerPos.top !== paramTop || containerPos.left !== paramLeft || a._$container.css('right') !== paramRight) {
                    if (withoutAnim) {
                        a._$container.css(cssPropTransition, '');
                    }
                    if (!paramRight) {
                        a._$container.css({
                            'top': -paramTop + 'px',
                            'left': -paramLeft + 'px'
                        });
                    }
                    else {
                        a._$container.css({
                            'top': -paramTop + 'px',
                            'left': -paramRight + 'px'
                        });
                    }
                    if (withoutAnim) {
                        setTimeout(function () {
                            a.setTransition(false);
                            a._$container.css(cssPropTransition, a._containerTransitionParam);
                        }, 1);
                    }
                }
                if (showScroll && height < topChild) {
                    var $scroll = a._$scroll;
                    if (!$scroll) {
                        $scroll = $('<div>', {
                            'class': cssPrefix + 'scroll'
                        });
                        $scroll.css({
                            'position': 'absolute',
                            'top': '0px',
                            'right': '0px',
                            'height': height + 'px'
                        });
                        a._$self.append($scroll);
                        a._$scroll = $scroll;
                    }
                    var $scrollTrack = a._$scrollTrack;
                    if (!$scrollTrack) {
                        $scrollTrack = $('<div>', {
                            'class': cssPrefix + 'scroll-track'
                        });
                        var cssValue = {
                            'position': 'absolute',
                            'width': 'inherit'
                        };
                        cssValue[cssPropTransition] = cssTransitionParamPos;
                        $scrollTrack.css(cssValue);
                        a._$scrollTrack = $scrollTrack;
                        $scroll.append($scrollTrack);
                    }
                    $scrollTrack.css({
                        'top': Math.floor((paramTop / topChild) * height) + 'px',
                        'height': Math.floor((height / topChild) * height) + 'px'
                    });
                }
                else {
                    if (a._$scrollTrack) {
                        a._$scrollTrack.remove();
                        a._$scrollTrack = null;
                    }
                    if (a._$scroll) {
                        a._$scroll.remove();
                        a._$scroll = null;
                    }
                }
                if (drawParam.after) {
                    drawParam.after();
                }
                return $(focusableItems);
            };
            CViewGroupControl.prototype.safeDraw = function () {
                var a = this;
                if (a.isTransitioning()) {
                    a._saftDraw = true;
                }
                else {
                    a.draw();
                }
            };
            CViewGroupControl.prototype.isTransitioning = function () {
                var a = this, i, len, controls = a._controls;
                for (i = 0, len = controls.length; i < len; i += 1) {
                    if (controls[i].isTransitioning()) {
                        return true;
                    }
                }
                return a._transitioning;
            };
            CViewGroupControl.prototype.doKey = function (keyCode) {
                var a = this, i;
                if (a.isTransitioning()) {
                    return true;
                }
                var handled = _super.prototype.doKey.call(a, keyCode);
                if (handled && a._drawParam.anchorPositionLeft !== undefined) {
                    // TODO: apply real position value, now we only support 0
                    if (keyCode === KKeyLeft || keyCode === KKeyRight) {
                        for (i = 0; i < a._controls.length; i += 1) {
                            var c = a._controls[i];
                            if (c.hasNaviMap()) {
                                var itemWidth = c.getDrawParam('itemWidth');
                                if (itemWidth) {
                                    var left = -a._$container.position().left;
                                    left += keyCode === KKeyRight ? itemWidth : -itemWidth;
                                    moveLeft(a, null, left);
                                }
                            }
                        }
                    }
                    return true;
                }
                if (!handled) {
                    handled = handleIncrement(a, keyCode);
                }
                return handled;
            };
            CViewGroupControl.prototype.setLeft = function (left, withoutAnim) {
                moveLeft(this, null, left, withoutAnim);
            };
            CViewGroupControl.prototype.setTop = function (left, withoutAnim) {
                moveTop(this, null, left, withoutAnim);
            };
            CViewGroupControl.prototype.getClientRect = function () {
                var a = this, containerPos = a._$container.position();
                return {
                    top: -containerPos.top,
                    left: -containerPos.left
                };
            };
            CViewGroupControl.prototype.enableTop = function () {
                this._enabledTop = true;
            };
            CViewGroupControl.prototype.doScroll = function (keyCode) {
                handleIncrement(this, keyCode);
            };
            CViewGroupControl.prototype._onChildFocusChanged = function (oldIndex, newIndex) {
                var a = this;
                var oldChild = a._controls[oldIndex];
                var newChild = a._controls[newIndex];
                oldChild.setActiveFocus(false);
                if (newChild instanceof CListControl && oldIndex > newIndex) {
                    newChild.setFocusedItemByKey(newChild._dataProvider.getCount() - 1);
                }
                newChild.setActiveFocus(true);
                oldChild._fireEvent('FocusLost');
                newChild._fireEvent('FocusGained');
                a._fireEvent('FocusChanged', oldChild, newChild);
            };
            return CViewGroupControl;
        })(CGroupControl);
        mControl.CViewGroupControl = CViewGroupControl;
        var CDrawnItems = (function () {
            function CDrawnItems() {
                this.items = {};
                this.onTransition = false;
                this.queueAdd = [];
                this.queueRemove = [];
            }
            CDrawnItems.prototype.doRemove = function (a, fnTransitionEnd) {
                var _this = this;
                this.onTransition = true;
                var queueRemove = this.queueRemove;
                var keys = queueRemove.shift();
                var items = this.items;
                if (!keys) {
                    this.onTransition = false;
                    return false;
                }
                var keysToDelete = keys.slice(0).sort(), key = keysToDelete.shift(), max = parseInt(Object.keys(items).pop(), 10), i, prevPos = null, prevPosTemp, prevKey, prevFsd = false, prevAfd = false, $item, $activeFocusedItem = null, transitionTimer = null, animated = false;
                if (items[key] === undefined) {
                    key = parseInt(Object.keys(items).shift(), 10);
                }
                for (i = key; i <= max; i += 1) {
                    $item = items[i];
                    delete items[i];
                    prevPosTemp = prevPos;
                    prevPos = $item.position();
                    if (i === key) {
                        prevFsd = $item.hasClass(cssFocused);
                        prevAfd = $item.hasClass(cssActiveFocusedLeaf);
                        $item.remove();
                        prevKey = key;
                        key = keysToDelete.shift();
                        if (items[key] === undefined) {
                            key = prevKey;
                        }
                    }
                    else {
                        if (prevPosTemp === null) {
                            $item.css({
                                top: 0,
                                left: 0
                            }).attr('data', 0);
                        }
                        else {
                            $item.css({
                                top: prevPosTemp.top,
                                left: prevPosTemp.left
                            }).attr('data', prevKey);
                        }
                        if (prevFsd) {
                            $item.addClass(cssFocused);
                            prevFsd = false;
                        }
                        if (prevAfd) {
                            $item.addClass(cssActiveFocusedLeaf);
                            $activeFocusedItem = $item;
                            prevAfd = false;
                        }
                        items[prevKey] = $item;
                        prevKey += 1;
                        clearTimeout(transitionTimer);
                        this.onTransition = true;
                        transitionTimer = setTimeout(function () {
                            $item.afterTransitionOneShot(function () {
                                if (!_this.doRemove(a, fnTransitionEnd)) {
                                    a.onTransition = false;
                                    fnTransitionEnd($activeFocusedItem);
                                }
                            });
                        }, 1);
                        animated = true;
                    }
                }
                if (prevFsd || prevAfd) {
                    $item = items[keys[0] - 1];
                    if ($item) {
                        if (prevFsd) {
                            $item.addClass(cssFocused);
                            prevFsd = false;
                        }
                        if (prevAfd) {
                            $item.addClass(cssActiveFocusedLeaf);
                            $activeFocusedItem = $item;
                            prevAfd = false;
                        }
                    }
                }
                if (animated) {
                    this.onTransition = true;
                }
                else {
                    fnTransitionEnd();
                }
                return true;
            };
            CDrawnItems.prototype.doRecRemove = function (a, fnTransitionEnd) {
                var _this = this;
                var queueRemove = this.queueRemove;
                var keys = queueRemove.shift();
                if (!keys) {
                    return false;
                }
                var items = this.items;
                var keysToDelete = keys.slice(0).sort(), key = keysToDelete.shift(), max = parseInt(Object.keys(items).pop(), 10), i, prevPos = null, prevPosTemp, prevKey, prevFsd = false, prevAfd = false, $item, $activeFocusedItem = null, transitionTimer = null, animated = false;
                if (items[key] === undefined) {
                    key = parseInt(Object.keys(items).shift(), 10);
                }
                for (i = key; i <= max; i += 1) {
                    $item = items[i];
                    delete items[i];
                    prevPosTemp = prevPos;
                    prevPos = $item.position();
                    if (i === key) {
                        prevFsd = $item.hasClass(cssFocused);
                        prevAfd = $item.hasClass(cssActiveFocusedLeaf);
                        $item.remove();
                        prevKey = key;
                        key = keysToDelete.shift();
                        if (items[key] === undefined) {
                            key = prevKey;
                        }
                    }
                    else {
                        if (prevPosTemp === null) {
                            $item.css({
                                top: 0,
                                left: 0
                            }).attr('data', 0);
                        }
                        else {
                            $item.css({
                                top: prevPosTemp.top,
                                left: prevPosTemp.left
                            }).attr('data', prevKey);
                        }
                        if (prevFsd) {
                            $item.addClass(cssFocused);
                            prevFsd = false;
                        }
                        if (prevAfd) {
                            $item.addClass(cssActiveFocusedLeaf);
                            $activeFocusedItem = $item;
                            prevAfd = false;
                        }
                        items[prevKey] = $item;
                        prevKey += 1;
                        clearTimeout(transitionTimer);
                        transitionTimer = setTimeout(function () {
                            $item.afterTransitionOneShot(function () {
                                if (!_this.doRecRemove(a, fnTransitionEnd)) {
                                    _this.onTransition = false;
                                    fnTransitionEnd($activeFocusedItem);
                                }
                            });
                        }, 1);
                        animated = true;
                    }
                }
                if (prevFsd || prevAfd) {
                    $item = items[keys[0] - 1];
                    if ($item) {
                        if (prevFsd) {
                            $item.addClass(cssFocused);
                            prevFsd = false;
                        }
                        if (prevAfd) {
                            $item.addClass(cssActiveFocusedLeaf);
                            $activeFocusedItem = $item;
                            prevAfd = false;
                        }
                    }
                }
                if (animated) {
                    this.onTransition = true;
                }
                else {
                    fnTransitionEnd();
                }
                return true;
            };
            CDrawnItems.prototype.doAdd = function (a, fnTransitionEnd) {
                var _this = this;
                var queueAdd = this.queueAdd;
                var addParam = queueAdd.shift();
                if (!addParam) {
                    return false;
                }
                var items = this.items;
                var keysToAdd = [addParam.key], key = keysToAdd.shift(), max = parseInt(Object.keys(items).pop(), 10), i, $item, pos, $itemLastMove, transitionTimer = null, $prevItem, animated = false;
                for (i = key; i <= max; i += 1) {
                    $item = items[i];
                    if ($item) {
                        delete items[i];
                        if (i === key) {
                            $prevItem = $item;
                            key = keysToAdd.shift();
                        }
                        else {
                            if (!$prevItem) {
                                $prevItem = $item;
                                key = keysToAdd.shift();
                            }
                            else {
                                pos = $item.position();
                                $prevItem.css({
                                    top: pos.top,
                                    left: pos.left
                                }).attr('data', i);
                                items[i] = $prevItem;
                                $itemLastMove = $prevItem;
                                clearTimeout(transitionTimer);
                                transitionTimer = setTimeout(function () {
                                    $itemLastMove.afterTransitionOneShot(function () {
                                        if (!_this.doAdd(a, fnTransitionEnd)) {
                                            _this.onTransition = false;
                                            fnTransitionEnd();
                                        }
                                    });
                                }, 1);
                                $prevItem = $item;
                                animated = true;
                            }
                        }
                    }
                }
                if ($item) {
                    $item.remove();
                }
                if (animated) {
                    this.onTransition = true;
                }
                else {
                    fnTransitionEnd();
                }
                return true;
            };
            CDrawnItems.prototype.destroy = function () {
                this.removeAll();
            };
            CDrawnItems.prototype.getCount = function () {
                var keys = Object.keys(this.items);
                return keys.length;
            };
            CDrawnItems.prototype.getItemsByArray = function () {
                var ret = [];
                var items = this.items;
                var keys = Object.keys(items);
                var i;
                for (i = 0; i < keys.length; i += 1) {
                    ret.push(items[keys[i]]);
                }
                return ret;
            };
            CDrawnItems.prototype.getItem = function (key) {
                return this.items[key];
            };
            CDrawnItems.prototype.setItem = function (key, item) {
                this.items[key] = item;
            };
            CDrawnItems.prototype.pickItem = function (key) {
                var ret = null;
                var items = this.items;
                //XXX
                if (items.hasOwnProperty(key)) {
                    ret = items[key];
                    delete items[key];
                }
                return ret;
            };
            CDrawnItems.prototype.remove = function (key) {
                var i, keys = key.length ? key : [key];
                var items = this.items;
                for (i = 0; i < keys.length; i += 1) {
                    var k = keys[i];
                    if (items.hasOwnProperty(k)) {
                        items[k].remove();
                        delete items[k];
                    }
                }
            };
            CDrawnItems.prototype.removeAll = function () {
                var key;
                var items = this.items;
                for (key in items) {
                    if (items.hasOwnProperty(key)) {
                        var $item = items[key];
                        $item.remove();
                    }
                }
                this.items = {};
            };
            CDrawnItems.prototype.each = function (fn) {
                var items = this.items;
                $.each(items, fn);
            };
            CDrawnItems.prototype.setAnimation = function (enable) {
                var a = this;
                if (enable) {
                    this.each(function (index, $item) {
                        $item.css(cssPropTransition, cssTransitionParamPos);
                    });
                }
                else {
                    this._drawnItems.each(function (index, $item) {
                        $item.css(cssPropTransition, '');
                    });
                }
            };
            CDrawnItems.prototype._move = function (from, to, fnTransitionEnd) {
                var a = this, moveForward = from < to, $startItem = this.getItem(from), i, $item, prevKey = null, prevPos = null, prevPosTemp;
                //
                if (moveForward) {
                    for (i = from; i <= to; i += 1) {
                        $item = this.getItem(i);
                        prevPosTemp = prevPos;
                        prevPos = {
                            top: $item.css('top'),
                            left: $item.css('left')
                        };
                        if (prevPosTemp) {
                            $item.css({
                                top: prevPosTemp.top,
                                left: prevPosTemp.left
                            }).attr('data', prevKey);
                            this.setItem(prevKey, $item);
                        }
                        prevKey = i;
                    }
                }
                else {
                    for (i = from; i >= to; i -= 1) {
                        $item = this.getItem(i);
                        prevPosTemp = prevPos;
                        prevPos = {
                            top: $item.css('top'),
                            left: $item.css('left')
                        };
                        if (prevPosTemp) {
                            $item.css({
                                top: prevPosTemp.top,
                                left: prevPosTemp.left
                            }).attr('data', prevKey);
                            this.setItem(prevKey, $item);
                        }
                        prevKey = i;
                    }
                }
                if (prevPos) {
                    $startItem.css({
                        top: prevPos.top,
                        left: prevPos.left
                    }).attr('data', prevKey);
                    this.setItem(prevKey, $startItem);
                    if (fnTransitionEnd) {
                        $startItem.one(cssEvntTransitionEnd, function () {
                            fnTransitionEnd();
                        });
                    }
                }
            };
            CDrawnItems.prototype._remove = function (keys, fnTransitionEnd) {
                var _this = this;
                var a = this;
                var queueRemove = this.queueRemove;
                //
                this.setAnimation(true);
                queueRemove.push(keys);
                var fnRemove = function () {
                    if (a.onTransition) {
                        setTimeout(function () {
                            fnRemove();
                        }, 100);
                    }
                    else {
                        _this.doRemove(a, fnTransitionEnd);
                    }
                };
                fnRemove();
            };
            CDrawnItems.prototype._recRemove = function (keys, fnTransitionEnd) {
                var a = this;
                var queueRemove = this.queueRemove;
                //
                this.setAnimation(true);
                queueRemove.push(keys);
                if (!this.onTransition) {
                    this.doRecRemove(a, fnTransitionEnd);
                }
            };
            CDrawnItems.prototype._add = function (key, item, fnTransitionEnd) {
                var queueAdd = this.queueAdd;
                this.setAnimation(true);
                queueAdd.push({
                    key: key,
                    item: item
                });
                if (!this.onTransition) {
                    this.doAdd(this, fnTransitionEnd);
                }
            };
            return CDrawnItems;
        })();
        mControl.CDrawnItems = CDrawnItems;
        //CArrayDataProvider
        //Provides data for CListControl or CGridControl
        var processLinearDataInsertUpward = function (a, data, key, newItem, silent, array, onlyDraw) {
            var keys = Object.keys(data), i, newItems = array ? newItem.slice(0) : [newItem], len = keys.length + newItems.length, prev = [], step = 0, item, changedKeys = [], insertedKeys = [];
            for (i = key; i < len; i += 1) {
                changedKeys.push(i);
                switch (step) {
                    case 0:
                        if (i === key) {
                            item = newItems.shift();
                            if (data[i]) {
                                prev.push(data[i]);
                            }
                            data[i] = item;
                            if (newItems.length === 0) {
                                step = 2;
                            }
                            else {
                                step = 1;
                            }
                        }
                        break;
                    case 1:
                        item = newItems.shift();
                        if (data[i]) {
                            prev.push(data[i]);
                        }
                        data[i] = item;
                        if (newItems.length === 0) {
                            step += 1;
                        }
                        break;
                    case 2:
                        if (data[i]) {
                            prev.push(data[i]);
                        }
                        data[i] = prev.shift();
                        break;
                }
            }
            if (prev.length) {
                console.error('Linear data insertion error');
                throw 'Linear data insertion error';
            }
            a._count = len;
            if (!silent) {
                a._fireEvent('ItemInserted', key, newItem, onlyDraw);
            }
        };
        var processLinearDataRemove = function (a, data, key, silent) {
            var keys = Object.keys(data), i, updatedKeys = [], keysToDelete = typeof key === 'Array' ? key.sort() : [key], deletedKeys = keysToDelete.slice(0), deletedItems = [], keyToDelete = keysToDelete.shift(), len = keys.length, moveCount = 0;
            for (i = keyToDelete; i < len; i += 1) {
                if (i === keyToDelete) {
                    moveCount += 1;
                    deletedItems.push(data[i]);
                    keysToDelete = keysToDelete.shift();
                }
                else if (moveCount) {
                    data[i - moveCount] = data[i];
                }
                if (len - i <= moveCount) {
                    delete data[i];
                }
            }
            a._count = len - moveCount;
            if (!silent) {
                a._fireEvent('ItemRemoved', deletedKeys, deletedItems);
            }
        };
        var processRecordingRemove = function (a, data, key, silent) {
            var keys = Object.keys(data), i, updatedKeys = [], keysToDelete = typeof key === 'Array' ? key.sort() : [key], deletedKeys = keysToDelete.slice(0), deletedItems = [], keyToDelete = keysToDelete.shift(), len = keys.length, moveCount = 0;
            for (i = keyToDelete; i < len; i += 1) {
                if (i === keyToDelete) {
                    moveCount += 1;
                    deletedItems.push(data[i]);
                    keysToDelete = keysToDelete.shift();
                }
                else if (moveCount) {
                    data[i - moveCount] = data[i];
                }
                if (len - i <= moveCount) {
                    delete data[i];
                }
            }
            a._count = len - moveCount;
            if (!silent) {
                a._fireEvent('RecItemRemoved', deletedKeys, deletedItems);
            }
        };
        var CDataProvider = (function (_super) {
            __extends(CDataProvider, _super);
            function CDataProvider(array) {
                _super.call(this);
                this._data = {};
                if (arguments.length) {
                    this.init(array);
                }
            }
            CDataProvider.prototype.init = function (array) {
                var a = this;
                a._registerEvent(['ItemInserted', 'ItemChanged', 'ItemMoved', 'ItemRemoved', 'ItemRefreshed', 'RecItemRemoved']);
                a._count = 0;
                CDataProvider.prototype._handleDataInsertUpward = processLinearDataInsertUpward;
                CDataProvider.prototype._handleDataRemove = processLinearDataRemove;
                CDataProvider.prototype._handleRecordingDataRemove = processRecordingRemove;
                if (array) {
                    a.setArray(array);
                }
            };
            CDataProvider.prototype.setArray = function (array) {
                var a = this, i;
                var len = array.length;
                a._data = {};
                for (i = 0; i < len; i += 1) {
                    a._data[i] = array[i];
                }
                a._count = len;
                a._fireEvent('ItemRefreshed');
            };
            CDataProvider.prototype.destroy = function () {
                var a = this;
                delete a._data;
                delete a._count;
                _super.prototype.destroy.call(a);
            };
            CDataProvider.prototype.getItem = function (key) {
                var a = this;
                if (a._data && a._data[key]) {
                    return a._data[key];
                }
                else {
                    return null;
                }
            };
            CDataProvider.prototype.getItemIndex = function (item, matcher) {
                var a = this, dataArray = a.getDataArray(), i, len;
                for (i = 0, len = dataArray.length; i < len; i += 1) {
                    if (matcher(dataArray[i])) {
                        return i;
                    }
                }
                return -1;
            };
            CDataProvider.prototype.getCount = function () {
                var a = this;
                return a._count;
            };
            CDataProvider.prototype.insertItem = function (key, item) {
                var a = this;
                if (a._handleDataInsertUpward) {
                    a._handleDataInsertUpward(a, a._data, key, item);
                }
                else {
                    a._data[key] = item;
                    a._fireEvent('ItemChanged', key);
                }
            };
            CDataProvider.prototype.insertItems = function (key, item, onlyDraw) {
                if (onlyDraw === void 0) { onlyDraw = false; }
                var a = this;
                if (a._handleDataInsertUpward) {
                    a._handleDataInsertUpward(a, a._data, key, item, false, true, onlyDraw);
                }
                else {
                    a._data[key] = item;
                    a._fireEvent('ItemChanged', key);
                }
            };
            CDataProvider.prototype.appendItem = function (item) {
                var a = this;
                var key = a._count;
                a._count += 1;
                a.insertItem(key, item);
            };
            CDataProvider.prototype.appendItems = function (item) {
                var a = this;
                var key = a._count;
                a._count += item.length;
                a.insertItems(key, item);
            };
            CDataProvider.prototype.removeItem = function (key) {
                var a = this;
                if (a._handleDataRemove) {
                    a._handleDataRemove(a, a._data, key);
                }
                else {
                    a._data[key] = null;
                    delete a._data[key];
                    a._fireEvent('ItemChanged', key);
                }
            };
            CDataProvider.prototype.removeRecordingItem = function (key) {
                var a = this;
                if (a._handleRecordingDataRemove) {
                    a._handleRecordingDataRemove(a, a._data, key);
                }
                else {
                    a._data[key] = null;
                    delete a._data[key];
                    a._fireEvent('ItemChanged', key);
                }
            };
            CDataProvider.prototype.prependItem = function (item) {
                var a = this;
                a.insertItem(0, item);
            };
            CDataProvider.prototype.updateItem = function (key, item) {
                var a = this;
                if (item) {
                    a._data[key] = item;
                }
                a._fireEvent('ItemChanged', [key]);
            };
            CDataProvider.prototype.move = function (from, to) {
                var a = this, i, startItem;
                //
                startItem = a._data[from];
                a._handleDataRemove(a, a._data, from, true);
                a._handleDataInsertUpward(a, a._data, to, startItem, true);
                a._fireEvent('ItemMoved', from, to);
            };
            CDataProvider.prototype.getDataArray = function () {
                var a = this, data = a._data, keys = [], i, ret = [];
                if (data) {
                    keys = Object.keys(data);
                }
                for (i = 0; i < keys.length; i += 1) {
                    ret.push(data[keys[i]]);
                }
                return ret;
            };
            return CDataProvider;
        })(CEventSource);
        mControl.CDataProvider = CDataProvider;
        var fnItemChanged = function (a, keys) {
            var dp = a._dataProvider, i, key, focusedItemKey = -1, $item, focusedItemInfo = a.getFocusedItem();
            if (focusedItemInfo && focusedItemInfo.$item && focusedItemInfo.$item.hasClass(cssActiveFocusedLeaf)) {
                focusedItemKey = focusedItemInfo.itemKey;
            }
            for (i = 0; i < keys.length; i += 1) {
                key = keys[i];
                $item = a._drawnItems.getItem(key);
                if ($item) {
                    a._drawer({
                        index: key,
                        item: dp.getItem(key),
                        $item: $item
                    });
                    if (focusedItemKey === key) {
                        if (a.onFocusChanged) {
                            a.onFocusChanged(null, $item);
                        }
                    }
                }
            }
        };
        var clearItemMoving = function ($item) {
            $item.removeClass(cssPrefix + 'movable-up');
            $item.removeClass(cssPrefix + 'movable-down');
            $item.removeClass(cssPrefix + 'movable-left');
            $item.removeClass(cssPrefix + 'movable-right');
        };
        var CDataControl = (function (_super) {
            __extends(CDataControl, _super);
            function CDataControl() {
                _super.call(this);
                this.onItemSelected = null;
                this.onFocusChanged = function ($old, $new) {
                    var a = this;
                    var selector = a._marqueeSelector;
                    if (selector) {
                        marqueeOff($old, selector);
                        marqueeOn($new, selector);
                    }
                };
            }
            CDataControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a._selectedIndex = 0;
                a._drawnItems = new CDrawnItems();
                a._registerEvent(['MovingModeCanceled']);
            };
            CDataControl.prototype.destroy = function () {
                var a = this;
                delete a._drawer;
                if (a._dataProvider) {
                    a._dataProvider.destroy();
                    a._dataProvider.removeEventHandler(a);
                    a._dataProvider = null;
                }
                if (a._drawnItems) {
                    a._drawnItems.destroy();
                    a._drawnItems = null;
                }
                _super.prototype.destroy.call(a);
            };
            CDataControl.prototype.getItemPos = function (key) {
                throw 'must override';
            };
            CDataControl.prototype._setParentViewGroupControl = function (viewGroupControl) {
                var a = this;
                a._viewGroupControl = viewGroupControl;
            };
            CDataControl.prototype.getDataProvider = function () {
                return this._dataProvider;
            };
            CDataControl.prototype.setDataProvider = function (dataProvider, onlyData) {
                var a = this;
                if (!onlyData) {
                    if (a._drawnItems) {
                        a._drawnItems.removeAll();
                    }
                }
                if (a._dataProvider) {
                    a._dataProvider.destroy();
                    a._dataProvider = null;
                }
                a._dataProvider = dataProvider;
                a._dataProvider.addEventHandler('ItemChanged', a, function (keys) {
                    fnItemChanged(a, keys);
                });
                a._dataProvider.addEventHandler('ItemInserted', a, function (key, item, onlyDraw) {
                    if (onlyDraw) {
                        if (a._viewGroupControl) {
                            a._viewGroupControl.safeDraw();
                        }
                    }
                    else {
                        a.setTransition(true);
                        a._drawnItems._add(key, item, function () {
                            a.setTransition(false);
                            if (a._viewGroupControl) {
                                a._viewGroupControl.safeDraw();
                            }
                        });
                    }
                });
                a._dataProvider.addEventHandler('ItemMoved', a, function (from, to) {
                    if (a._ignoreDrawnItemMove) {
                        delete a._ignoreDrawnItemMove;
                        if (a._movingCanceling) {
                            delete a._movingCanceling;
                            a._viewGroupControl.setTop(a._movingModeStartViewPos.top);
                            a.setFocusedItemByKey(a._movingModeStartKey);
                            a._finalizeMovingModeLeft();
                        }
                    }
                    else {
                        a.setTransition(true);
                        a._drawnItems._move(from, to, function () {
                            // check item moved due to cancellation
                            if (a._movingCanceling) {
                                delete a._movingCanceling;
                                a._finalizeMovingModeLeft();
                            }
                            a.setTransition(false);
                        });
                    }
                });
                a._dataProvider.addEventHandler('ItemRemoved', a, function (keys) {
                    a.setTransition(true);
                    a._drawnItems._remove(keys, function ($newFocusedItem) {
                        a.setTransition(false);
                        if ($newFocusedItem) {
                            // rebuild key navigation map for changed orders
                            var drawnItems = a._drawnItems;
                            var keyMapInfo = a._keyMapBuilder($(drawnItems.getItemsByArray()));
                            a._keyMap = keyMapInfo.map;
                            a._currentIndex = keyMapInfo.startIndex;
                            a.changeFocus($newFocusedItem);
                        }
                        if (a._drawnItems.getCount() === 0) {
                            var drawParam = a.getDrawParam();
                            if (drawParam.align && drawParam.align === 'horizontal') {
                                if (a._viewGroupControl) {
                                    a._viewGroupControl.doScroll(KKeyLeft);
                                }
                            }
                            else {
                                if (a._viewGroupControl) {
                                    a._viewGroupControl.doScroll(KKeyUp);
                                }
                            }
                        }
                        a.redraw();
                    });
                });
                a._dataProvider.addEventHandler('ItemRefreshed', a, function () {
                    a.clearDrawnItems();
                });
                a._dataProvider.addEventHandler('RecItemRemoved', a, function (keys) {
                    a.setTransition(true);
                    a._drawnItems._recRemove(keys, function ($newFocusedItem) {
                        a.setTransition(false);
                        if ($newFocusedItem) {
                            // rebuild key navigation map for changed orders
                            var drawnItems = a._drawnItems;
                            var keyMapInfo = a._keyMapBuilder($(drawnItems.getItemsByArray()));
                            a._keyMap = keyMapInfo.map;
                            a._currentIndex = keyMapInfo.startIndex;
                            a.changeFocus($newFocusedItem);
                        }
                        if (a._drawnItems.getCount() === 0) {
                            var drawParam = a.getDrawParam();
                            if (drawParam.align && drawParam.align === 'horizontal') {
                                a._viewGroupControl.doScroll(KKeyLeft);
                            }
                            else {
                                a._viewGroupControl.doScroll(KKeyUp);
                            }
                        }
                        a.redraw();
                    });
                });
            };
            CDataControl.prototype.setDataDrawer = function (drawer) {
                var a = this;
                a._drawer = drawer;
            };
            CDataControl.prototype.getCurrentIndex = function () {
                var a = this;
                return a._currentIndex;
            };
            CDataControl.prototype.getHeight = function () {
                var a = this, drawParam = a.getDrawParam();
                //
                var headHeight = drawParam.head ? drawParam.head.height || 0 : 0;
                var itemHeight = drawParam.itemHeight || 0;
                var totalCount = a._dataProvider ? a._dataProvider.getCount() : 0;
                if (drawParam.align === 'horizontal') {
                    return itemHeight;
                }
                return headHeight + itemHeight * totalCount;
            };
            CDataControl.prototype.getWidth = function () {
                var a = this, drawParam = a.getDrawParam();
                //
                var headWidth = drawParam.head ? drawParam.head.width || 0 : 0;
                var itemWidth = drawParam.itemWidth || 0;
                var totalCount = a._dataProvider ? a._dataProvider.getCount() : 0;
                return headWidth + itemWidth * totalCount;
            };
            CDataControl.prototype.getNextAvail = function (keyCode) {
                var a = this;
                var ret = {};
                if (a._contentAvail) {
                    if (a._keyMapBuilder === rightToLeftBuilder) {
                        switch (keyCode) {
                            case "up":
                            case KKeyUp:
                                if (a._contentAvail.up) {
                                    ret.top = -a._contentAvail.up;
                                }
                                break;
                            case "down":
                            case KKeyDown:
                                if (a._contentAvail.down) {
                                    ret.top = a._contentAvail.down;
                                }
                                break;
                            case "left":
                            case KKeyLeft:
                                if (a._contentAvail.left) {
                                    ret.right = a._contentAvail.left;
                                }
                                break;
                            case "right":
                            case KKeyRight:
                                if (a._contentAvail.right) {
                                    ret.right = -a._contentAvail.right;
                                }
                                break;
                        }
                    }
                    else {
                        switch (keyCode) {
                            case "up":
                            case KKeyUp:
                                if (a._contentAvail.up) {
                                    ret.top = -a._contentAvail.up;
                                }
                                break;
                            case "down":
                            case KKeyDown:
                                if (a._contentAvail.down) {
                                    ret.top = a._contentAvail.down;
                                }
                                break;
                            case "left":
                            case KKeyLeft:
                                if (a._contentAvail.left) {
                                    ret.left = -a._contentAvail.left;
                                }
                                break;
                            case "right":
                            case KKeyRight:
                                if (a._contentAvail.right) {
                                    ret.left = a._contentAvail.right;
                                }
                                break;
                        }
                    }
                }
                return ret;
            };
            CDataControl.prototype.doKeyOk = function (aKeyCode) {
                var ret = false;
                if (this.onItemSelected && this._keyMap) {
                    var $focusedItem = this._keyMap[this._currentIndex].$;
                    if ($focusedItem) {
                        var itemKey = parseInt($focusedItem.attr('data'), 10);
                        var item = this._dataProvider.getItem(itemKey);
                        this._selectedIndex = itemKey;
                        this.onItemSelected({
                            $item: $focusedItem,
                            itemKey: itemKey,
                            item: item
                        });
                        ret = true;
                    }
                }
                return ret;
            };
            CDataControl.prototype.getSelectedIndex = function () {
                return this._selectedIndex;
            };
            CDataControl.prototype.getFocusedItem = function () {
                var a = this;
                var $item, itemKey, item;
                $item = _super.prototype.getFocusedItem.call(a);
                if ($item) {
                    itemKey = parseInt($item.attr('data'), 10);
                    item = a._dataProvider.getItem(itemKey);
                    return {
                        $item: $item,
                        itemKey: itemKey,
                        item: item
                    };
                }
                return null;
            };
            CDataControl.prototype.setFocusedItemByKey = function (key) {
                var a = this;
                var i, length, control, controls, result = false;
                var $item = a._getDrawnItem(key);
                if ($item) {
                    a.changeFocus($item);
                }
                else {
                    if (a._viewGroupControl) {
                    }
                    else {
                        a._requestedFocusedItemKey = key;
                    }
                }
            };
            CDataControl.prototype.draw = function (param) {
                var a = this;
                var i, length, pos, control, controls, controlTop = 0;
                if (a._dataProvider) {
                    a._prevParam = param;
                    _super.prototype.draw.call(a, param);
                    if (a._requestedFocusedItemKey !== undefined) {
                        var requestedFocusedItemKey = a._requestedFocusedItemKey;
                        delete a._requestedFocusedItemKey;
                        if (a._viewGroupControl) {
                            pos = a.getItemPos(requestedFocusedItemKey);
                            controls = a._viewGroupControl._controls;
                            if (controls) {
                                length = controls.length;
                                for (i = 0; i < length; i += 1) {
                                    control = controls[i];
                                    if (control === a) {
                                        break;
                                    }
                                    controlTop += control.getHeight();
                                }
                            }
                            if (pos.top !== undefined) {
                                var viewHeight = a._viewGroupControl._$self.height();
                                var targetTop = Math.floor((controlTop + pos.top) / viewHeight) * viewHeight;
                                var maxTop = Math.max(a.getHeight() - viewHeight, 0);
                                var overPos = Math.max(a.getHeight() + controlTop - viewHeight, 0);
                                if (a._viewGroupControl._enabledTop) {
                                    targetTop = Math.max(targetTop, maxTop);
                                    targetTop = Math.max(targetTop, controlTop);
                                    if (targetTop !== 0) {
                                        targetTop -= Math.min(targetTop, 50);
                                    }
                                }
                                else {
                                    if (controlTop === 0) {
                                        targetTop = a.getItemPos(requestedFocusedItemKey).top;
                                    }
                                    else {
                                        if (overPos && overPos > 0) {
                                            targetTop = a.getItemPos(Math.floor(overPos / a.getHeight()) + 1).top;
                                        }
                                        else {
                                            targetTop = Math.min(targetTop, maxTop);
                                            targetTop = Math.min(targetTop, controlTop);
                                        }
                                    }
                                }
                                setTimeout(function () {
                                    a._viewGroupControl.setTop(targetTop, true);
                                    a._drawnItems.each(function (i, item) {
                                        item.removeClass(cssFocused + ' ' + cssActiveFocused);
                                    });
                                    a.setFocusedItemByKey(requestedFocusedItemKey);
                                }, 1);
                            }
                        }
                        else {
                            a._drawnItems.each(function (i, item) {
                                item.removeClass(cssFocused + ' ' + cssActiveFocused);
                            });
                            a.setFocusedItemByKey(requestedFocusedItemKey);
                        }
                        a._selectedIndex = requestedFocusedItemKey;
                    }
                }
            };
            CDataControl.prototype.redraw = function () {
                var a = this;
                if (a._prevParam) {
                    var activeFocusInfo = a.getActiveFocusInfo();
                    if (activeFocusInfo) {
                        a._prevParam.setActiveFocus = true;
                    }
                    _super.prototype.draw.call(a, a._prevParam);
                }
            };
            CDataControl.prototype.clearDrawnItems = function () {
                var a = this;
                a._drawnItems.removeAll();
            };
            CDataControl.prototype.getDrawnItems = function () {
                var a = this;
                return a._drawnItems;
            };
            CDataControl.prototype._setDrawnItem = function (key, $item) {
                var a = this;
                a._drawnItems.setItem(key, $item);
            };
            CDataControl.prototype._getDrawnItem = function (key) {
                var a = this;
                return a._drawnItems.getItem(key);
            };
            CDataControl.prototype._pickDrawnItems = function () {
                var a = this;
                var ret = a._drawnItems;
                a._drawnItems = new CDrawnItems();
                return ret;
            };
            CDataControl.prototype.isMovingMode = function () {
                var a = this;
                return a._moving;
            };
            CDataControl.prototype.enterMovingMode = function () {
                var a = this, focusedItemInfo = a.getFocusedItem();
                a._movingModeStartKey = focusedItemInfo.itemKey;
                if (a._viewGroupControl) {
                    a._moving = true;
                    a._movingModeStartViewPos = a._viewGroupControl.getClientRect();
                }
                a._setItemMoving(focusedItemInfo.itemKey);
                a._drawnItems.each(function (index, $item) {
                    $item.css(cssPropTransition, cssTransitionParamPos);
                });
            };
            CDataControl.prototype.leaveMovingMode = function (cancel) {
                var a = this, focusedItemInfo = a.getFocusedItem(), finalize = true;
                if (!focusedItemInfo) {
                    return;
                }
                var focusedItemKey = focusedItemInfo.itemKey;
                var movingModeStartKey = a._movingModeStartKey;
                // process moving cancel
                if (cancel && focusedItemKey !== movingModeStartKey) {
                    var itemToMove = a._drawnItems.getItem(a._movingModeStartKey);
                    if (itemToMove) {
                        finalize = false;
                    }
                    else {
                        a._ignoreDrawnItemMove = true;
                    }
                    // flag for finalize after transition
                    a._movingCanceling = true;
                    // revert to the original positioin
                    a._move(focusedItemKey, movingModeStartKey);
                }
                else {
                    a._finalizeMovingModeLeft();
                }
            };
            CDataControl.prototype._finalizeMovingModeLeft = function () {
                var a = this, drawnItems = a._drawnItems, focusedItemInfo = a.getFocusedItem();
                // reset context
                delete a._movingModeStartKey;
                delete a._movingModeStartViewPos;
                a._moving = false;
                if (!focusedItemInfo) {
                    return;
                }
                // clear moving indicator CSS class
                clearItemMoving(focusedItemInfo.$item);
                // remove animation property for drawn items
                a._drawnItems.each(function (index, $item) {
                    $item.css(cssPropTransition, '');
                });
                // rebuild key navigation map for changed orders
                var keyMapInfo = a._keyMapBuilder($(drawnItems.getItemsByArray()));
                a._keyMap = keyMapInfo.map;
                var i = 0;
                if (keyMapInfo.map) {
                    for (i = 0; i < keyMapInfo.map.length; i += 1) {
                        var item = keyMapInfo.map[i];
                        if (item.$[0] === focusedItemInfo.$item[0]) {
                            a._currentIndex = i;
                        }
                    }
                }
                else {
                    a._currentIndex = keyMapInfo.startIndex;
                }
            };
            CDataControl.prototype._setItemMoving = function (key) {
                var a = this, $item = a._drawnItems.getItem(key), movableInfo = a._getMovableInfo(key);
                //
                clearItemMoving($item);
                if (movableInfo.up !== -1) {
                    $item.addClass(cssPrefix + 'movable-up');
                }
                if (movableInfo.down !== -1) {
                    $item.addClass(cssPrefix + 'movable-down');
                }
                if (movableInfo.left !== -1) {
                    $item.addClass(cssPrefix + 'movable-left');
                }
                if (movableInfo.right !== -1) {
                    $item.addClass(cssPrefix + 'movable-right');
                }
            };
            CDataControl.prototype._getMovableInfo = function (key) {
                return {
                    up: 0,
                    down: 0,
                    left: 0,
                    right: 0
                };
            };
            CDataControl.prototype.doKey = function (keyCode) {
                var a = this;
                if (a._moving) {
                    if (a.isTransitioning()) {
                        return true;
                    }
                    var focusedItemInfo = a.getFocusedItem();
                    if (focusedItemInfo) {
                        var movableInfo = a._getMovableInfo(focusedItemInfo.itemKey);
                        var target = -1;
                        var isNaviKey = true;
                        switch (keyCode) {
                            case KKeyDown:
                                target = movableInfo.down;
                                break;
                            case KKeyUp:
                                target = movableInfo.up;
                                break;
                            case KKeyLeft:
                                target = movableInfo.left;
                                break;
                            case KKeyRight:
                                target = movableInfo.right;
                                break;
                            case KKeyBack:
                                a.leaveMovingMode(true);
                                return true;
                            case KKeyPageUp:
                            case KKeyPageDown:
                                // TODO: Page up/down in moving mode
                                return true;
                            default:
                                isNaviKey = false;
                                break;
                        }
                    }
                    if (isNaviKey) {
                        var itemToMove = a._drawnItems.getItem(target);
                        if (itemToMove && itemToMove.hasClass(cssFocusable)) {
                            if (target !== -1) {
                                a._move(focusedItemInfo.itemKey, target);
                                focusedItemInfo = a.getFocusedItem();
                                a._setItemMoving(focusedItemInfo.itemKey);
                            }
                            // Implementation move and scroll functions.
                            // It shall be divided according to the key Event Type.
                            var maxNumbers = a._drawParam.maxColCount;
                            if (keyCode === KKeyRight && maxNumbers) {
                                if (itemToMove.find('move') && target > maxNumbers) {
                                    return false;
                                }
                            }
                            else if (keyCode === KKeyLeft && maxNumbers) {
                                if (itemToMove.find('move') && target < (focusedItemInfo.itemKey - maxNumbers)) {
                                    return false;
                                }
                            }
                            return true;
                        }
                    }
                    return false;
                }
                if ((keyCode == KKeyBack) && (this._$self.find('.unchecked').length > 0 || this._$self.find('.checked').length > 0))
                    return true;
                return _super.prototype.doKey.call(a, keyCode);
            };
            CDataControl.prototype._move = function (from, to) {
                var a = this, dp = a._dataProvider;
                dp.move(from, to);
            };
            CDataControl.prototype._setPreviousDrawItems = function (prevDrawnItems) {
                var a = this;
                a.clearPreviousDrawItems();
                a._prevDrawnItems = prevDrawnItems;
            };
            CDataControl.prototype.clearPreviousDrawItems = function () {
                var a = this;
                if (a._prevDrawnItems) {
                    a._prevDrawnItems.destroy();
                }
                a._prevDrawnItems = null;
            };
            CDataControl.prototype.marqueeOn = function (selector) {
                this._marqueeSelector = selector;
            };
            CDataControl.prototype.marqueeOff = function () {
                this._marqueeSelector = null;
            };
            CDataControl.prototype.onActiveFocusedGained = function () {
                var a = this;
                var selector = a._marqueeSelector;
                if (selector) {
                    marqueeOn(a.getFocusedItem().$item, selector);
                }
            };
            CDataControl.prototype.onActiveFocusedLost = function () {
                var a = this;
                var selector = a._marqueeSelector;
                if (selector) {
                    marqueeOff(a.getFocusedItem().$item, selector);
                }
            };
            return CDataControl;
        })(CControl);
        mControl.CDataControl = CDataControl;
        var doDrawForVertical = function (a, param) {
            var dp = a._dataProvider, itemCount = dp.getCount(), drawParam = a.getDrawParam(), itemHeight = drawParam.itemHeight, separatorHeight = drawParam.separatorHeight, separatorIndex = drawParam.separatorIndex, paramHeight = param.bottom - param.top, max = paramHeight ? Math.min(Math.ceil(param.bottom / itemHeight), itemCount) : itemCount, startIndex = paramHeight ? Math.max(Math.floor(param.top / itemHeight), 0) : 0, drawnItems = a._pickDrawnItems(), focusableItems = [], movePosition = drawParam.drawEffect === 'spreadOut', cssValueForMove = {
                'position': 'absolute',
                'top': '0px' //
            }, i, executeNext = [];
            if (drawParam.itemWidth) {
                a._$self.css('width', drawParam.itemWidth);
            }
            //
            cssValueForMove[cssPropTransition] = cssTransitionParamPos;
            if (separatorHeight) {
                var totalHeight = itemHeight * (itemCount - 1) + separatorHeight;
                a._$self.css('height', totalHeight);
            }
            else {
                a._$self.css('height', itemHeight * itemCount);
            }
            fnLog('CListControl.doDraw: t:' + param.top + ' b:' + param.bottom);
            a._contentAvail = {};
            if (param.hasOwnProperty('top') && param.hasOwnProperty('bottom') && ((param.bottom - param.top) === 0)) {
                drawnItems.removeAll();
                a._contentAvail.down = itemHeight;
                a._contentAvail.up = itemHeight;
                return $([]);
            }
            for (i = startIndex; i < max; i += 1) {
                var $listItem = drawnItems.pickItem(i);
                if (!$listItem) {
                    $listItem = $('<div>', {
                        'class': [cssPrefix + 'list-item', cssFocusable].join(' ')
                    }).attr('data', i);
                    var getTopPosition = function (i, itemHeight, separatorHeight, separatorIndex) {
                        if (separatorHeight) {
                            var revisedTop;
                            if (i <= separatorIndex) {
                                revisedTop = i * itemHeight;
                            }
                            else {
                                revisedTop = separatorHeight + (i - 1) * itemHeight;
                            }
                            return revisedTop;
                        }
                        else {
                            return i * itemHeight;
                        }
                    };
                    if (movePosition) {
                        $listItem.css(cssValueForMove);
                        executeNext.push((function ($listItem, left) {
                            return function () {
                                $listItem.css({
                                    'position': 'absolute',
                                    'top': left + 'px'
                                });
                            };
                        }($listItem, getTopPosition(i, itemHeight, separatorHeight, separatorIndex))));
                    }
                    else {
                        $listItem.css({
                            'position': 'absolute',
                            'top': getTopPosition(i, itemHeight, separatorHeight, separatorIndex) + 'px'
                        });
                    }
                    if (i === 0) {
                        $listItem.addClass(cssFocused);
                    }
                    a._drawer({
                        index: i,
                        item: dp.getItem(i),
                        $item: $listItem
                    });
                    a._$self.append($listItem);
                }
                a._setDrawnItem(i, $listItem);
                if ($listItem.hasClass(cssFocusable)) {
                    focusableItems.push($listItem);
                }
            }
            //a._$self.show();
            a._setPreviousDrawItems(drawnItems);
            var overflowDown = param.bottom % itemHeight;
            if (overflowDown) {
                fnLog('overflowDown=' + overflowDown);
                a._contentAvail.down = drawParam.scrollByItemSize ? itemHeight : itemHeight - overflowDown;
                if (!(drawParam.focusableItemLength && drawParam.focusableItemLength >= focusableItems.length)) {
                    focusableItems.pop();
                }
            }
            else if (max < itemCount) {
                a._contentAvail.down = itemHeight;
            }
            if (param.top - param.bottom) {
                var overflowTop = param.top % itemHeight;
                if (overflowTop) {
                    fnLog('overflowTop=' + overflowTop);
                    a._contentAvail.up = drawParam.scrollByItemSize ? itemHeight : overflowTop;
                    // TODO: Is this correct?
                    if (overflowTop !== itemHeight) {
                        focusableItems.shift();
                    }
                }
                else if (startIndex) {
                    a._contentAvail.up = itemHeight;
                }
            }
            else {
                a._contentAvail.up = itemHeight;
            }
            fnLog('CListControl.doDraw: count=' + focusableItems.length);
            setTimeout(function () {
                var i;
                for (i = 0; i < executeNext.length; i += 1) {
                    executeNext[i]();
                }
            }, 1);
            return $(focusableItems);
        };
        var doDrawForHorizontal = function (a, param) {
            var dp = a._dataProvider, itemCount = dp.getCount(), drawParam = a.getDrawParam(), itemWidth = drawParam.itemWidth, separatorWidth = drawParam.separatorWidth, separatorIndex = drawParam.separatorIndex, paramWidth = param.right - param.left, focusMax = paramWidth ? Math.min(Math.ceil(param.right / itemWidth), itemCount) : itemCount, drawMax = focusMax < itemCount ? focusMax + 1 : itemCount, focusStartIndex = paramWidth ? Math.max(Math.floor(param.left / itemWidth), 0) : 0, drawStartIndex = focusStartIndex > 0 ? focusStartIndex - 1 : 0, drawnItems = a._pickDrawnItems(), focusableItems = [], movePosition = drawParam.drawEffect === 'spreadOut', cssValueForMove = {
                'position': 'absolute',
                'left': '0px' //
            }, i, executeNext = [];
            if (drawParam.itemHeight) {
                a._$self.css('height', drawParam.itemHeight);
            }
            cssValueForMove[cssPropTransition] = cssTransitionParamPos;
            if (separatorWidth) {
                var totalWidth = itemWidth * (itemCount - 1) + separatorWidth;
                a._$self.css('width', totalWidth);
            }
            else {
                a._$self.css('width', itemWidth * itemCount);
            }
            fnLog('CListControl.doDraw: l:' + param.left + ' r:' + param.right);
            a._contentAvail = {};
            if (param.hasOwnProperty('left') && param.hasOwnProperty('right') && ((param.left - param.right) === 0)) {
                drawnItems.removeAll();
                a._contentAvail.left = itemWidth;
                a._contentAvail.right = itemWidth;
                return $([]);
            }
            for (i = drawStartIndex; i < drawMax; i += 1) {
                var $listItem = drawnItems.pickItem(i);
                if (!$listItem) {
                    $listItem = $('<div>', {
                        'class': cssPrefix + 'list-item'
                    }).attr('data', i);
                    var getLeftPosition = function (i, itemWidth, separatorWidth, separatorIndex) {
                        if (separatorWidth) {
                            var revisedLeft;
                            if (i <= separatorIndex) {
                                revisedLeft = i * itemWidth;
                            }
                            else {
                                revisedLeft = separatorWidth + (i - 1) * itemWidth;
                            }
                            return revisedLeft;
                        }
                        else {
                            return i * itemWidth;
                        }
                    };
                    if (movePosition) {
                        $listItem.css(cssValueForMove);
                        executeNext.push((function ($listItem, left) {
                            return function () {
                                $listItem.css({
                                    'position': 'absolute',
                                    'left': left + 'px'
                                });
                            };
                        }($listItem, getLeftPosition(i, itemWidth, separatorWidth, separatorIndex))));
                    }
                    else {
                        $listItem.css({
                            'position': 'absolute',
                            'left': getLeftPosition(i, itemWidth, separatorWidth, separatorIndex) + 'px'
                        });
                    }
                    if (i === 0) {
                        $listItem.addClass(cssFocused);
                    }
                    a._drawer({
                        index: i,
                        item: dp.getItem(i),
                        $item: $listItem
                    });
                    a._$self.append($listItem);
                }
                if (focusStartIndex <= i && i < focusMax) {
                    $listItem.addClass(cssFocusable);
                }
                else {
                    $listItem.removeClass(cssFocusable);
                }
                a._setDrawnItem(i, $listItem);
                if ($listItem.hasClass(cssFocusable)) {
                    focusableItems.push($listItem);
                }
            }
            a._setPreviousDrawItems(drawnItems);
            var overflowRight = param.right % itemWidth;
            if (overflowRight) {
                fnLog('overflowRight=' + overflowRight);
                a._contentAvail.right = drawParam.scrollByItemSize ? itemWidth : itemWidth - overflowRight;
                focusableItems.pop();
            }
            else if (focusMax < itemCount) {
                a._contentAvail.right = itemWidth;
            }
            if (param.right - param.left) {
                var overflowLeft = param.left % itemWidth;
                if (overflowLeft) {
                    a._contentAvail.left = drawParam.scrollByItemSize ? itemWidth : overflowLeft;
                    // TODO: Is this correct?
                    if (overflowLeft !== itemWidth) {
                        focusableItems.shift();
                    }
                }
                else if (focusStartIndex) {
                    a._contentAvail.left = itemWidth;
                }
            }
            else {
                a._contentAvail.left = itemWidth;
            }
            fnLog('CListControl.doDraw: count:' + focusableItems.length);
            setTimeout(function () {
                var i;
                for (i = 0; i < executeNext.length; i += 1) {
                    executeNext[i]();
                }
            }, 1);
            return $(focusableItems);
        };
        var CVariableListControl = (function (_super) {
            __extends(CVariableListControl, _super);
            function CVariableListControl($el) {
                _super.call(this);
                this.doDrawForHorizontal = function (a, param) {
                    var dp = a._dataProvider, itemCount = dp.getCount(), drawParam = a.getDrawParam(), itemWidth = drawParam.itemWidth, paramWidth = param.right - param.left, max = paramWidth ? Math.min(Math.ceil(param.right / itemWidth), itemCount) : itemCount, startIndex = paramWidth ? Math.max(Math.floor(param.left / itemWidth), 0) : 0, drawnItems = a._pickDrawnItems(), focusableItems = [], movePosition = drawParam.drawEffect === 'spreadOut', cssValueForMove = {
                        'position': 'absolute',
                        'left': '0px' //
                    }, i, executeNext = [];
                    if (drawParam.itemHeight) {
                        a._$self.css('height', drawParam.itemHeight);
                    }
                    cssValueForMove[cssPropTransition] = cssTransitionParamPos;
                    a._$self.css('width', itemWidth * itemCount);
                    fnLog('CListControl.doDraw: l:' + param.left + ' r:' + param.right);
                    a._contentAvail = {};
                    // added
                    var variablePixel = 0;
                    if (param.hasOwnProperty('left') && param.hasOwnProperty('right') && ((param.left - param.right) === 0)) {
                        drawnItems.removeAll();
                        a._contentAvail.left = itemWidth;
                        a._contentAvail.right = itemWidth;
                        return $([]);
                    }
                    for (i = startIndex; i < max; i += 1) {
                        var dpItem = dp.getItem(i);
                        var $listItem = drawnItems.pickItem(i);
                        if (dpItem.getWidth() && dpItem.getWidth() !== itemWidth) {
                            variablePixel = dpItem.getWidth();
                        }
                        if (!$listItem) {
                            $listItem = $('<div>', {
                                'class': [cssPrefix + 'list-item', cssFocusable].join(' ')
                            }).attr('data', i);
                            if (movePosition) {
                                $listItem.css(cssValueForMove);
                                executeNext.push((function ($listItem, left) {
                                    return function () {
                                        $listItem.css({
                                            'position': 'absolute',
                                            'left': left + 'px'
                                        });
                                    };
                                }($listItem, i * itemWidth)));
                            }
                            else {
                                if (i == startIndex) {
                                    $listItem.css({
                                        'position': 'absolute',
                                        'left': (i * variablePixel) + 'px'
                                    });
                                }
                                else {
                                    $listItem.css({
                                        'position': 'absolute',
                                        'left': variablePixel + ((i - 1) * itemWidth) + 'px',
                                    });
                                }
                            }
                            if (i === 0) {
                                $listItem.addClass(cssFocused);
                            }
                            a._drawer({
                                index: i,
                                item: dp.getItem(i),
                                $item: $listItem
                            });
                            a._$self.append($listItem);
                        }
                        a._setDrawnItem(i, $listItem);
                        focusableItems.push($listItem);
                    }
                    a._setPreviousDrawItems(drawnItems);
                    var overflowRight = param.right % itemWidth;
                    if (overflowRight) {
                        fnLog('overflowRight=' + overflowRight);
                        a._contentAvail.right = drawParam.scrollByItemSize ? itemWidth : itemWidth - overflowRight;
                        focusableItems.pop();
                    }
                    else if (max < itemCount) {
                        a._contentAvail.right = itemWidth;
                    }
                    if (param.right - param.left) {
                        var overflowLeft = param.left % itemWidth;
                        if (overflowLeft) {
                            a._contentAvail.left = drawParam.scrollByItemSize ? itemWidth : overflowLeft;
                            // TODO: Is this correct?
                            if (overflowLeft !== itemWidth) {
                                focusableItems.shift();
                            }
                        }
                        else if (startIndex) {
                            a._contentAvail.left = itemWidth;
                        }
                    }
                    else {
                        a._contentAvail.left = itemWidth;
                    }
                    fnLog('CListControl.doDraw: count:' + focusableItems.length);
                    setTimeout(function () {
                        var i;
                        for (i = 0; i < executeNext.length; i += 1) {
                            executeNext[i]();
                        }
                    }, 1);
                    return $(focusableItems);
                };
                this.doDrawForReverseHorizontal = function (a, param) {
                    var absLeft = Math.abs(param.left);
                    var dp = a._dataProvider, itemCount = dp.getCount(), drawParam = a.getDrawParam(), itemWidth = drawParam.itemWidth, paramWidth = param.right - param.left, startIndex = paramWidth ? Math.floor(absLeft / itemWidth) : 0, max = paramWidth ? Math.ceil(((absLeft + param.right) + (startIndex * itemWidth)) / itemWidth) : itemCount, drawnItems = a._pickDrawnItems(), focusableItems = [], movePosition = drawParam.drawEffect === 'spreadOut', cssValueForMove = {
                        'position': 'absolute',
                        'left': '0px' //
                    }, i, executeNext = [];
                    cssValueForMove[cssPropTransition] = cssTransitionParamPos;
                    //a._$self.css('width', itemWidth * itemCount);
                    fnLog('CListControl.doDraw: l:' + param.left + ' r:' + param.right);
                    a._contentAvail = {};
                    if (param.hasOwnProperty('left') && param.hasOwnProperty('right') && ((param.left - param.right) === 0)) {
                        drawnItems.removeAll();
                        a._contentAvail.left = itemWidth;
                        a._contentAvail.right = itemWidth;
                        return $([]);
                    }
                    for (i = startIndex; i < max; i += 1) {
                        var $listItem = drawnItems.pickItem(i);
                        if (!$listItem) {
                            $listItem = $('<div>', {
                                'class': [cssPrefix + 'list-item', cssFocusable].join(' ')
                            }).attr('data', i);
                            if (movePosition) {
                                $listItem.css(cssValueForMove);
                                executeNext.push((function ($listItem, left) {
                                    return function () {
                                        $listItem.css({
                                            'position': 'absolute',
                                            'right': left + 'px'
                                        });
                                    };
                                }($listItem, i * itemWidth)));
                            }
                            else {
                                $listItem.css({
                                    'position': 'absolute',
                                    'right': i * itemWidth + 'px'
                                });
                            }
                            if (i === 0) {
                                $listItem.addClass(cssFocused);
                            }
                            a._drawer({
                                index: i,
                                item: dp.getItem(i),
                                $item: $listItem
                            });
                            a._$self.append($listItem);
                        }
                        a._setDrawnItem(i, $listItem);
                        focusableItems.push($listItem);
                    }
                    a._setPreviousDrawItems(drawnItems);
                    var overflowLeft = param.left % itemWidth;
                    if (overflowLeft) {
                        fnLog('overflowLeft=' + overflowLeft);
                        a._contentAvail.left = drawParam.scrollByItemSize ? itemWidth : itemWidth - overflowLeft;
                        focusableItems.pop();
                    }
                    else if (max < itemCount) {
                        a._contentAvail.left = itemWidth;
                    }
                    if (param.right - param.left) {
                        var overflowRight = param.left % itemWidth;
                        if (overflowRight) {
                            a._contentAvail.right = drawParam.scrollByItemSize ? itemWidth : overflowRight;
                            // TODO: Is this correct?
                            if (overflowRight !== itemWidth) {
                                focusableItems.shift();
                            }
                        }
                        else if (startIndex) {
                            a._contentAvail.right = itemWidth;
                        }
                    }
                    else {
                        a._contentAvail.right = itemWidth;
                    }
                    fnLog('CListControl.doDraw: count:' + focusableItems.length);
                    setTimeout(function () {
                        var i;
                        for (i = 0; i < executeNext.length; i += 1) {
                            executeNext[i]();
                        }
                    }, 1);
                    return $(focusableItems);
                };
                if (arguments.length) {
                    this.init($el);
                }
            }
            CVariableListControl.prototype.init = function ($el) {
                var a = this;
                CListControl.prototype.init.call(a, $el);
            };
            CVariableListControl.prototype.doDraw = function (param) {
                var a = this, drawParam = a.getDrawParam();
                //
                if (drawParam.align === 'horizontal') {
                    if (drawParam.direction === 'reverse') {
                        a._keyMapBuilder = rightToLeftBuilder;
                        return a.doDrawForReverseHorizontal(a, param);
                    }
                    else {
                        a._keyMapBuilder = leftToRightBuilder;
                        return a.doDrawForHorizontal(a, param);
                    }
                }
                a._keyMapBuilder = topToDownBuilder;
                return doDrawForVertical(a, param);
            };
            CVariableListControl.prototype.getItemPos = function (key) {
                var a = this;
                CListControl.prototype.getItemPos.call(a, key);
            };
            CVariableListControl.prototype.doKey = function (keyCode) {
                var a = this;
                var drawParam = a.getDrawParam();
                if (a._moving) {
                    CDataControl.prototype.doKey.call(a, keyCode);
                    return true;
                }
                else {
                    var handled = CControl.prototype.doKey.call(a, keyCode);
                    if (drawParam.setFocusRolling && !handled) {
                        if (keyCode === KKeyDown) {
                            a.setFocusedItem(0);
                        }
                        else if (keyCode === KKeyUp) {
                            a.setFocusedItem(a._keyMap.length - 1);
                        }
                    }
                    return handled;
                }
            };
            CVariableListControl.prototype._getMovableInfo = function (key) {
                var a = this;
                // CListControl.prototype._getMovableInfo.call(a, key);
                var a = this, dp = a._dataProvider, len = dp.getCount(), drawParam = a.getDrawParam(), maxColCount = drawParam.maxColCount, index = key + 1, modulo = index % maxColCount, row = Math.ceil(index / maxColCount), maxRow = Math.ceil(len / maxColCount), ret = {
                    up: 0,
                    down: 0,
                    left: key - 1,
                    right: key + 1 //
                };
                return ret;
            };
            return CVariableListControl;
        })(CDataControl);
        mControl.CVariableListControl = CVariableListControl;
        var CListControl = (function (_super) {
            __extends(CListControl, _super);
            function CListControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CListControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a._$self.addClass(cssPrefix + 'list');
            };
            CListControl.prototype.doDraw = function (param) {
                var a = this, drawParam = a.getDrawParam();
                //
                if (drawParam.align === 'horizontal') {
                    a._keyMapBuilder = leftToRightBuilder;
                    return doDrawForHorizontal(a, param);
                }
                else {
                    a._keyMapBuilder = topToDownBuilder;
                    return doDrawForVertical(a, param);
                }
            };
            CListControl.prototype.getItemPos = function (key) {
                var a = this, drawParam = a._drawParam, retObj = { 'top': 0, 'left': 0 }, count = a._dataProvider.getCount();
                if (!drawParam) {
                    return retObj;
                }
                if (drawParam.doNotScrollFirstMaxCount) {
                    if (drawParam.doNotScrollFirstMaxCount <= key) {
                        key = key - drawParam.doNotScrollFirstMaxCount + 1;
                    }
                    else {
                        return retObj;
                    }
                }
                if (drawParam.align === 'horizontal') {
                    retObj.left = drawParam.itemWidth + parseInt(key, 10);
                }
                else {
                    retObj.top = drawParam.itemHeight * parseInt(key, 10);
                }
                return retObj;
            };
            CListControl.prototype.doKey = function (keyCode) {
                var a = this;
                var drawParam = a.getDrawParam();
                if (a._moving) {
                    var handled = CDataControl.prototype.doKey.call(a, keyCode);
                    return handled;
                }
                else {
                    var handled = CControl.prototype.doKey.call(a, keyCode);
                    if (drawParam.setFocusRolling && !handled) {
                        if (drawParam.align === 'horizontal') {
                            if (keyCode === KKeyRight) {
                                a.setFocusedItem(0);
                            }
                            else if (keyCode === KKeyLeft) {
                                a.setFocusedItem(a._keyMap.length - 1);
                            }
                        }
                        else {
                            if (keyCode === KKeyDown) {
                                a.setFocusedItem(0);
                            }
                            else if (keyCode === KKeyUp) {
                                a.setFocusedItem(a._keyMap.length - 1);
                            }
                        }
                    }
                    return handled;
                }
            };
            CListControl.prototype._getMovableInfo = function (key) {
                var a = this, dp = a._dataProvider, len = dp.getCount(), drawParam = a.getDrawParam(), maxColCount = drawParam.maxColCount, index = key + 1, modulo = index % maxColCount, row = Math.ceil(index / maxColCount), maxRow = Math.ceil(len / maxColCount), ret = {
                    up: 0,
                    down: 0,
                    left: key - 1,
                    right: key + 1 //
                };
                return ret;
            };
            return CListControl;
        })(CDataControl);
        mControl.CListControl = CListControl;
        var CGridControl = (function (_super) {
            __extends(CGridControl, _super);
            function CGridControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CGridControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a._$self.addClass(cssPrefix + 'grid');
                a._contentAvail = {};
                a._keyMapBuilder = gridBuilder;
            };
            CGridControl.prototype.getHeight = function () {
                var a = this, drawParam = a.getDrawParam();
                var headHeight = drawParam.head ? drawParam.head.height : 0;
                return headHeight + drawParam.itemHeight * Math.ceil(a._dataProvider.getCount() / drawParam.maxColCount);
            };
            CGridControl.prototype.doDraw = function (param) {
                var a = this, dp = a._dataProvider, len = dp.getCount(), drawnItems = a._pickDrawnItems(), drawParam = a.getDrawParam(), headInfo = drawParam.head, headHeight = headInfo ? headInfo.height : 0, itemWidth = drawParam.itemWidth, itemHeight = drawParam.itemHeight, maxColCount = drawParam.maxColCount, paramTop = param.top || 0, paramLeft = param.left || 0, paramRight = param.hasOwnProperty('right') ? param.right : itemWidth * maxColCount, paramBottom = param.hasOwnProperty('bottom') ? param.bottom : itemHeight * Math.ceil(len / maxColCount), startIndex = 0, i;
                startIndex = (Math.floor(paramTop / itemHeight) * maxColCount);
                a._contentAvail = {};
                var focusableItems = [];
                var focusableLength = 0;
                fnLog('CGridControl.doDraw: t:' + paramTop + ' l:' + paramLeft + ' r:' + paramRight + ' b:' + paramBottom);
                fnLog('CGridControl.doDraw: startIndex:' + startIndex);
                if (headInfo) {
                    if (headInfo.html) {
                        a._$self.html(headInfo.html);
                    }
                    else if (headInfo.text) {
                        var $head = a._$head;
                        if (!$head) {
                            $head = $('<div>', {
                                'class': cssPrefix + 'grid-head'
                            });
                            a._$self.append($head);
                            a._$head = $head;
                        }
                        $head.text(headInfo.text);
                    }
                }
                var hasFocusedItem = false;
                for (i = startIndex; i < len; i += 1) {
                    var colCount = i % maxColCount;
                    var rowCount = Math.floor(i / maxColCount);
                    if (paramBottom <= headHeight + rowCount * itemHeight) {
                        if (!a._contentAvail.down) {
                            a._contentAvail.down = itemHeight;
                        }
                        break;
                    }
                    // Handle already drawn item
                    var $gridItem = drawnItems.pickItem(i);
                    if ($gridItem) {
                        if ($gridItem.hasClass(cssFocused)) {
                            hasFocusedItem = true;
                            if (param.bReDraw) {
                                hasFocusedItem = false; // Re-Draw!!
                                $gridItem.removeClass(cssFocused + ' ' + cssActiveFocused);
                            }
                        }
                    }
                    else {
                        $gridItem = $('<div>', {
                            'class': [cssPrefix + 'grid-item'].join(' ')
                        }).css({
                            'position': 'absolute',
                            'top': rowCount * itemHeight + headHeight,
                            'left': colCount * itemWidth
                        }).attr('data', i);
                        if (a.isMovingMode()) {
                            $gridItem.css(cssPropTransition, cssTransitionParamPos);
                        }
                        a._drawer({
                            index: i,
                            item: dp.getItem(i),
                            $item: $gridItem
                        });
                        a._$self.append($gridItem);
                    }
                    a._setDrawnItem(i, $gridItem);
                    if (headHeight + rowCount * itemHeight + itemHeight <= paramBottom) {
                        $gridItem.addClass(cssFocusable);
                        focusableItems.push($gridItem);
                    }
                    else {
                        $gridItem.removeClass(cssFocusable);
                        var overflowDown = (paramBottom - headHeight) % itemHeight;
                        if (overflowDown) {
                            if (drawParam.scrollByItemSize) {
                                a._contentAvail.down = itemHeight;
                            }
                            else {
                                fnLog('overflowDown=' + overflowDown);
                                a._contentAvail.down = itemHeight - overflowDown;
                            }
                        }
                    }
                }
                if (paramBottom - paramTop) {
                    var overflowUp = paramTop % itemHeight;
                    if (overflowUp) {
                        fnLog('overflowUp=' + overflowUp);
                        a._contentAvail.up = overflowUp;
                        // TODO: Is this correct?
                        if (overflowUp !== headHeight) {
                            focusableItems.splice(0, maxColCount);
                        }
                    }
                    else if (startIndex) {
                        //if (param.checkedItems !== 0) {
                        //    a._contentAvail.up = (Math.floor((param.checkedItems - (len - startIndex)) / 3) + 1) * itemHeight;
                        //} else {
                        a._contentAvail.up = itemHeight;
                    }
                }
                else {
                    //if (param.checkedItems !== undefined) {
                    //    a._contentAvail.up = (Math.floor((param.checkedItems - (len - startIndex)) / 3) + 1) * itemHeight;
                    //} else {
                    a._contentAvail.up = itemHeight;
                }
                a._setPreviousDrawItems(drawnItems);
                a._$self.css({
                    width: itemWidth * maxColCount,
                    height: itemHeight * Math.ceil(len / maxColCount)
                });
                focusableLength = focusableItems.length;
                fnLog('CGridControl.doDraw: count:' + focusableLength);
                if (!hasFocusedItem && focusableLength) {
                    var newIndex = a.getCurrentIndex();
                    if (param.bReDraw) {
                        newIndex = (newIndex > -1) ? newIndex : 0;
                        if (newIndex >= focusableLength) {
                            newIndex = focusableLength - 1;
                        }
                    }
                    else {
                        newIndex = 0;
                    }
                    focusableItems[newIndex].addClass(cssFocused);
                }
                return $(focusableItems);
            };
            CGridControl.prototype._getMovableInfo = function (key) {
                var a = this, dp = a._dataProvider, len = dp.getCount(), drawParam = a.getDrawParam(), maxColCount = drawParam.maxColCount, index = key + 1, modulo = index % maxColCount, row = Math.ceil(index / maxColCount), maxRow = Math.ceil(len / maxColCount), ret = {
                    up: key - maxColCount,
                    down: key + maxColCount,
                    left: key - 1,
                    right: key + 1 //
                };
                //
                if (row === maxRow) {
                    ret.down = -1;
                }
                if (row === 1) {
                    ret.up = -1;
                }
                if (modulo === 0) {
                    ret.right = -1;
                }
                if (modulo === 1) {
                    ret.left = -1;
                }
                return ret;
            };
            return CGridControl;
        })(CDataControl);
        mControl.CGridControl = CGridControl;
        var CHorizontalGridControl = (function (_super) {
            __extends(CHorizontalGridControl, _super);
            function CHorizontalGridControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CHorizontalGridControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a._$self.addClass(cssPrefix + 'grid');
                a._contentAvail = {};
                a._keyMapBuilder = horizontalGridBuilder;
            };
            CHorizontalGridControl.prototype.getHeight = function () {
                var a = this, drawParam = a.getDrawParam();
                var headHeight = drawParam.head ? drawParam.head.height : 0;
                return headHeight + drawParam.itemHeight * drawParam.maxRowCount;
            };
            CHorizontalGridControl.prototype.getWidth = function () {
                var a = this, drawParam = a.getDrawParam();
                var headWidth = drawParam.head ? drawParam.head.width : 0;
                return headWidth + drawParam.itemWidth * Math.ceil(a._dataProvider.getCount() / drawParam.maxRowCount);
            };
            CHorizontalGridControl.prototype.updateContentAvail = function (param) {
                var a = this, dp = a._dataProvider, len = dp.getCount(), drawParam = a.getDrawParam(), prevParam = a._prevParam, maxRowCount = drawParam.maxRowCount, itemWidth = drawParam.itemWidth, paramRight = param.hasOwnProperty('right') ? param.right : itemWidth * Math.ceil(len / maxRowCount);
                // It should be updated by adding items to data provider
                // To be able to do scroll without needless draw
                if ((prevParam && prevParam.right) < paramRight) {
                    if (drawParam.scrollByViewSize) {
                        var unitWidth = a._viewGroupControl ? a._viewGroupControl._$self.width() : itemWidth;
                        a._contentAvail.right = unitWidth;
                        a._contentAvail.left = unitWidth;
                    }
                    else if (drawParam.scrollByItemSize) {
                        a._contentAvail.right = itemWidth;
                        a._contentAvail.left = itemWidth;
                    }
                    else {
                    }
                }
            };
            CHorizontalGridControl.prototype.doDraw = function (param) {
                var a = this, dp = a._dataProvider, len = dp.getCount(), drawnItems = a._pickDrawnItems(), drawParam = a.getDrawParam(), headInfo = drawParam.head, headHeight = headInfo ? headInfo.height : 0, headWidth = headInfo ? headInfo.width : 0, itemWidth = drawParam.itemWidth, itemHeight = drawParam.itemHeight, maxRowCount = drawParam.maxRowCount, gridAreaWidth = drawParam.gridAreaWidth, paramTop = param.top || 0, paramRight = param.hasOwnProperty('right') ? param.right : itemWidth * Math.ceil(len / maxRowCount), paramLeft = param.left || 0, paramBottom = param.hasOwnProperty('bottom') ? param.bottom : itemHeight * maxRowCount, i, startIndex = 0;
                var startIndex = (Math.floor(paramLeft / itemWidth) * maxRowCount) || 0;
                if (drawParam.startIndex !== undefined) {
                    startIndex = drawParam.startIndex;
                }
                a._contentAvail = {};
                var focusableItems = [];
                fnLog('CHorizontalGridControl.doDraw: top:' + paramTop + ' left:' + paramLeft + ' right:' + paramRight + ' bottom:' + paramBottom);
                fnLog('CHorizontalGridControl.doDraw: startIndex:' + startIndex);
                if (headInfo) {
                    if (headInfo.html) {
                        a._$self.html(headInfo.html);
                    }
                    else if (headInfo.text) {
                        var $head = a._$head;
                        if (!$head) {
                            $head = $('<div>', {
                                'class': cssPrefix + 'grid-head'
                            });
                            a._$self.append($head);
                            a._$head = $head;
                        }
                        $head.text(headInfo.text);
                    }
                }
                var hasFocusedItem = false;
                for (i = startIndex; i < len; i += 1) {
                    // dynamic col & row count
                    var colCount = Math.floor(i / maxRowCount);
                    var rowCount = Math.floor(i % maxRowCount);
                    if (paramRight <= headWidth + colCount * itemWidth) {
                        if (!a._contentAvail.right) {
                            a._contentAvail.right = itemWidth;
                        }
                        break;
                    }
                    var $gridItem = drawnItems.pickItem(i);
                    if ($gridItem) {
                        if ($gridItem.hasClass(cssFocused)) {
                            hasFocusedItem = true;
                        }
                    }
                    else {
                        $gridItem = $('<div>', {
                            'class': [cssPrefix + 'grid-item'].join(' ')
                        }).css({
                            'position': 'absolute',
                            'top': rowCount * itemHeight + headHeight,
                            'left': colCount * itemWidth
                        }).attr('data', i);
                        if (a.isMovingMode()) {
                            $gridItem.css(cssPropTransition, cssTransitionParamPos);
                        }
                        a._drawer({
                            index: i,
                            item: dp.getItem(i),
                            $item: $gridItem
                        });
                        a._$self.append($gridItem);
                    }
                    a._setDrawnItem(i, $gridItem);
                    if (headWidth + colCount * itemWidth + (itemWidth) <= paramRight) {
                        $gridItem.addClass(cssFocusable);
                        focusableItems.push($gridItem);
                    }
                    else {
                        $gridItem.removeClass(cssFocusable);
                        var overflowRight = (paramRight - headWidth) % itemWidth;
                        if (overflowRight) {
                            if (drawParam.scrollByItemSize) {
                                a._contentAvail.right = itemWidth;
                            }
                            else {
                                fnLog('overflowRight=' + overflowRight);
                                a._contentAvail.right = itemWidth - overflowRight;
                            }
                        }
                    }
                }
                if (paramRight - paramLeft) {
                    var overflowLeft = paramLeft % itemWidth;
                    if (overflowLeft) {
                        a._contentAvail.left = overflowLeft;
                        if (overflowLeft !== headWidth) {
                            focusableItems.splice(0, maxRowCount);
                        }
                    }
                    else if (startIndex) {
                        a._contentAvail.left = itemWidth;
                    }
                }
                else {
                    a._contentAvail.left = itemWidth;
                }
                if (drawParam.scrollByViewSize) {
                    var unitWidth = a._viewGroupControl ? a._viewGroupControl._$self.width() : itemWidth;
                    if (a._contentAvail.right) {
                        a._contentAvail.right = unitWidth;
                    }
                    if (a._contentAvail.left) {
                        a._contentAvail.left = unitWidth;
                    }
                }
                a._setPreviousDrawItems(drawnItems);
                a._$self.css({
                    width: itemWidth * Math.ceil(len / maxRowCount),
                    height: itemHeight * maxRowCount
                });
                fnLog('CGridControl.doDraw: count:' + focusableItems.length);
                if (!hasFocusedItem && focusableItems.length) {
                    if (drawParam.scrollByViewSize) {
                        var targetItem = focusableItems[a._currentIndex] || focusableItems[0];
                        if (targetItem) {
                            targetItem.addClass(cssFocused);
                        }
                    }
                    else {
                        focusableItems[0].addClass(cssFocused);
                    }
                }
                return $(focusableItems);
            };
            CHorizontalGridControl.prototype._getMovableInfo = function (key) {
                var a = this, dp = a._dataProvider, len = dp.getCount(), drawParam = a.getDrawParam(), maxRowCount = drawParam.maxRowCount, index = key + 1, modulo = index % maxRowCount, row = Math.ceil(index / maxRowCount), maxRow = Math.ceil(len / maxRowCount), ret = {
                    up: key - maxRowCount,
                    down: key + maxRowCount,
                    left: key - 1,
                    right: key + 1 //
                };
                //
                if (row === maxRow) {
                    ret.down = -1;
                }
                if (row === 1) {
                    ret.up = -1;
                }
                if (modulo === 0) {
                    ret.right = -1;
                }
                if (modulo === 1) {
                    ret.left = -1;
                }
                return ret;
            };
            return CHorizontalGridControl;
        })(CDataControl);
        mControl.CHorizontalGridControl = CHorizontalGridControl;
        var CRollDataProvider = (function () {
            function CRollDataProvider(array) {
                if (array) {
                    this.setArray(array);
                }
            }
            CRollDataProvider.prototype.destroy = function () {
                var a = this;
                a._array = [];
                delete a._array;
            };
            CRollDataProvider.prototype.getArray = function () {
                var a = this;
                return a._array;
            };
            CRollDataProvider.prototype.setArray = function (array) {
                var a = this;
                a._array = array;
                a._idxCur = 0;
                a._idxLast = array.length - 1;
            };
            CRollDataProvider.prototype.appendArray = function (array) {
                var a = this;
                a._array = a._array.concat(array);
                a._idxLast = array.length - 1;
            };
            CRollDataProvider.prototype.cur = function () {
                var a = this;
                return a._idxCur;
            };
            CRollDataProvider.prototype.curItem = function () {
                var a = this;
                return a._array[a._idxCur];
            };
            CRollDataProvider.prototype.setCur = function (index) {
                var a = this;
                a._idxCur = a.indexing(index);
            };
            CRollDataProvider.prototype.inc = function (count) {
                var a = this;
                count = typeof count !== 'undefined' ? count : 1;
                a._idxCur = a.indexing(a._idxCur + count);
            };
            CRollDataProvider.prototype.dec = function (count) {
                var a = this;
                count = typeof count !== 'undefined' ? count : 1;
                a._idxCur = a.indexing(a._idxCur - count);
            };
            CRollDataProvider.prototype.indexing = function (index) {
                var a = this;
                return index >= 0 ? index % a._array.length : a._array.length + index % a._array.length;
            };
            CRollDataProvider.prototype.at = function (index) {
                var a = this;
                return index >= 0 ? a._array[index % a._array.length] : a._array[a._array.length + index % a._array.length];
            };
            CRollDataProvider.prototype.get = function (count, offset) {
                var a = this;
                var CResult = function () {
                };
                CResult.prototype = new Array();
                CResult.prototype.firstAvailIndex = function () {
                    var i, a = this;
                    for (i = 0; i < a.length; i += 1) {
                        if (a[i]) {
                            return i;
                        }
                    }
                    return -1;
                };
                CResult.prototype.lastAvailIndex = function () {
                    var i, a = this;
                    for (i = a.length - 1; i >= 0; i -= 1) {
                        if (a[i]) {
                            return i;
                        }
                    }
                    return -1;
                };
                var ret = new CResult();
                offset = typeof offset !== 'undefined' ? offset : 0;
                var len = a._array.length;
                var i;
                if (count > len) {
                    var halfCount = Math.floor(len / 2);
                    var centering = halfCount <= offset && halfCount <= count - offset - 1;
                    var index;
                    for (i = 0; i < count; i += 1) {
                        ret.push(null);
                    }
                    var start = offset - halfCount;
                    if (centering) {
                        for (i = 0; i < len; i += 1) {
                            index = a.indexing(a._idxCur - halfCount + i);
                            ret[i + start] = {
                                index: index,
                                data: a._array[index]
                            };
                        }
                    }
                    else if (len - 1 <= offset) {
                        start = offset - len + 1;
                        for (i = offset; i >= start; i -= 1) {
                            index = a.indexing(a._idxCur - i + offset);
                            ret[i] = {
                                index: index,
                                data: a._array[index]
                            };
                        }
                    }
                    else if (len <= count - offset) {
                        start = offset;
                        for (i = 0; i < len; i += 1) {
                            index = a.indexing(a._idxCur + i);
                            ret[i + start] = {
                                index: index,
                                data: a._array[index]
                            };
                        }
                    }
                    else {
                        for (i = 0; i < len; i += 1) {
                            index = a.indexing(a._idxCur - offset + i);
                            ret[i] = {
                                index: index,
                                data: a._array[index]
                            };
                        }
                    }
                }
                else {
                    for (i = 0; i < count; i += 1) {
                        index = a.indexing(a._idxCur - offset + i);
                        ret.push({
                            index: index,
                            data: a._array[index]
                        });
                    }
                }
                return ret;
            };
            CRollDataProvider.prototype.del = function (index) {
                var a = this;
                var compromised = a.indexing(index);
                if (isNaN(compromised)) {
                    return;
                }
                a._array.splice(compromised, 1);
            };
            CRollDataProvider.prototype.each = function (fn, offset) {
                var a = this;
                var index = offset ? a.indexing(a._idxCur + offset) : a._idxCur;
                var need = false;
                do {
                    need = fn(index, a.at(index));
                    index = a.indexing(index + 1);
                } while (need && index !== a._idxCur);
            };
            CRollDataProvider.prototype.eachReverse = function (fn) {
                var a = this;
                var index = a._idxCur;
                var need = false;
                do {
                    need = fn(index, a.at(index));
                    index = a.indexing(index - 1);
                } while (need && index !== a._idxCur);
            };
            CRollDataProvider.prototype.eachArray = function (fn) {
                var a = this;
                var index = a._idxCur;
                var need = false;
                do {
                    if (index < a._array.length) {
                        need = fn(index, a.at(index));
                    }
                    else {
                        need = fn(-1, undefined);
                    }
                    index += 1;
                } while (need);
            };
            CRollDataProvider.prototype.length = function () {
                var a = this;
                var ret = 0;
                if (a._array) {
                    ret = a._array.length;
                }
                return ret;
            };
            CRollDataProvider.prototype.clone = function () {
                var a = this;
                var ret = new CRollDataProvider(a._array);
                ret.setCur(a._idxCur);
                return ret;
            };
            return CRollDataProvider;
        })();
        var fnDeliverFocusChanged = function (a) {
            if (a.onFocusChanged) {
                a.onFocusChanged({
                    '$anchor': a._$anchor,
                    'item': a._rollDataProvider.curItem()
                });
            }
        };
        var drawItem = function (a, $item, item, index) {
            if (a._dataDrawer) {
                a._dataDrawer({
                    '$item': $item,
                    'item': item,
                    'key': index
                });
            }
            else {
                $item.text(item);
            }
        };
        var createItem = function (a, item, top, className) {
            var drawParam = a.getDrawParam();
            var align = drawParam.align === undefined ? 'vertical' : drawParam.align;
            var animation = drawParam.animation === undefined ? true : drawParam.animation;
            var animationInterval = drawParam.animationInterval || 0.3;
            var classNames = className ? [cssPrefix + 'carousel-item', className] : [cssPrefix + 'carousel-item'];
            var css = {
                'position': 'absolute'
            }, $item = $('<div>').addClass(classNames.join(' '));
            if (align === 'horizontal') {
                if (animation) {
                    css[cssPropTransition] = 'left ' + animationInterval + 's linear';
                }
                css.left = top + 'px';
            }
            else {
                if (animation) {
                    css[cssPropTransition] = 'top ' + animationInterval + 's linear';
                }
                css.top = top + 'px';
            }
            if (item) {
                $item.attr('data', item.index);
                drawItem(a, $item, item.data, item.index);
            }
            $item.css(css);
            return $item;
        };
        var moveItemsLeftward = function ($items, itemWidth, anchorIndex, indexOffset, anchorWidth) {
            var nextLeft = 0;
            $items.each(function (i) {
                var $item = $(this), itemIndex = i + indexOffset, prevDiff = itemIndex - 1 - anchorIndex, prevDist = Math.abs(prevDiff), prevDirection = prevDiff < 0 ? '-l' : '-r', prevDistClassName = cssPrefix + 'carousel-dist' + prevDist, diff = itemIndex - anchorIndex, dist = Math.abs(diff), direction = diff < 0 ? '-l' : '-r', distClassName = cssPrefix + 'carousel-dist' + dist;
                //
                $item.css('left', nextLeft + 'px').removeClass(prevDistClassName).addClass(distClassName);
                if (diff === 0) {
                    $item.removeClass(prevDirection);
                }
                else {
                    $item.removeClass(prevDirection).addClass(direction);
                }
                nextLeft += (anchorWidth && itemIndex === anchorIndex) ? anchorWidth : itemWidth;
            });
        };
        var moveItemsRightward = function ($items, itemWidth, anchorIndex, indexOffset, anchorWidth) {
            var nextLeft = -itemWidth;
            $items.each(function (i) {
                var $item = $(this), itemIndex = i + indexOffset, prevDiff = itemIndex + 1 - anchorIndex, prevDist = Math.abs(prevDiff), prevDirection = prevDiff < 0 ? '-l' : '-r', prevDistClassName = cssPrefix + 'carousel-dist' + prevDist, diff = itemIndex - anchorIndex, dist = Math.abs(diff), direction = diff < 0 ? '-l' : '-r', distClassName = cssPrefix + 'carousel-dist' + dist;
                //
                $item.css('left', nextLeft + 'px').removeClass(prevDistClassName).addClass(distClassName);
                if (diff === 0) {
                    $item.removeClass(prevDirection);
                }
                else {
                    $item.removeClass(prevDirection).addClass(direction);
                }
                nextLeft += (anchorWidth && itemIndex === anchorIndex) ? anchorWidth : itemWidth;
            });
        };
        var moveItemsDownward = function ($items, itemHeight, anchorIndex, indexOffset, anchorHeight) {
            var nextTop = 0;
            $items.each(function (i) {
                var $item = $(this), itemIndex = i + indexOffset, prevDiff = itemIndex - 1 - anchorIndex, prevDist = Math.abs(prevDiff), prevDirection = prevDiff < 0 ? '-u' : '-d', prevDistClassName = cssPrefix + 'carousel-dist' + prevDist, diff = itemIndex - anchorIndex, dist = Math.abs(diff), direction = diff < 0 ? '-u' : '-d', distClassName = cssPrefix + 'carousel-dist' + dist;
                //
                $item.css('top', nextTop + 'px').removeClass(prevDistClassName).addClass(distClassName);
                if (diff === 0) {
                    $item.removeClass(prevDirection);
                }
                else {
                    $item.removeClass(prevDirection).addClass(direction);
                }
                nextTop += (anchorHeight && itemIndex === anchorIndex) ? anchorHeight : itemHeight;
            });
        };
        var moveItemsUpward = function ($items, itemHeight, anchorIndex, indexOffset, anchorHeight) {
            var nextTop = -itemHeight;
            $items.each(function (i) {
                var $item = $(this), itemIndex = i + indexOffset, prevDiff = itemIndex + 1 - anchorIndex, prevDist = Math.abs(prevDiff), prevDirection = prevDiff < 0 ? '-u' : '-d', prevDistClassName = cssPrefix + 'carousel-dist' + prevDist, diff = itemIndex - anchorIndex, dist = Math.abs(diff), direction = diff < 0 ? '-u' : '-d', distClassName = cssPrefix + 'carousel-dist' + dist;
                //
                $item.css('top', nextTop + 'px').removeClass(prevDistClassName).addClass(distClassName);
                if (diff === 0) {
                    $item.removeClass(prevDirection);
                }
                else {
                    $item.removeClass(prevDirection).addClass(direction);
                }
                nextTop += (anchorHeight && itemIndex === anchorIndex) ? anchorHeight : itemHeight;
            });
        };
        var handleTransitionEnd = function (a) {
            a._onTransition = false;
            if (a.fnSafeUpdate) {
                a.fnSafeUpdate();
                a.fnSafeUpdate = null;
                a._keyQueue = [];
            }
            else {
                if (a._keyQueue.length) {
                    a[a._keyQueue.shift()]();
                }
                else {
                    fnDeliverFocusChanged(a);
                }
            }
        };
        var doTransitionAndAfter = function (a, $target, fnAfter) {
            $target.afterTransitionOneShot(function () {
                fnAfter();
                handleTransitionEnd(a);
            });
        };
        var CCarouselControl = (function (_super) {
            __extends(CCarouselControl, _super);
            function CCarouselControl($el) {
                _super.call(this);
                this._rollDataProvider = null;
                this.init($el);
                this._keyMapBuilder = leftToRightBuilder;
            }
            CCarouselControl.prototype.init = function ($el) {
                var a = this;
                CControl.prototype.init.call(a, $el);
                a.addVolitileParam('startIndex');
                a._$self.addClass(cssPrefix + 'carousel');
                a._onTransition = false;
                a._css = {};
                a._keyQueue = [];
            };
            CCarouselControl.prototype.setDataDrawer = function (drawer) {
                var a = this;
                a._dataDrawer = drawer;
            };
            CCarouselControl.prototype.setAnchorDrawer = function (anchorDrawer) {
                var a = this;
                a._anchorDrawer = anchorDrawer;
            };
            CCarouselControl.prototype.setActiveFocus = function (activeFocus) {
                var a = this;
                if (activeFocus) {
                    a._$self.addClass(cssActiveFocused);
                }
                else {
                    a._$self.removeClass(cssActiveFocused);
                }
            };
            CCarouselControl.prototype.setData = function (menuItems) {
                var a = this;
                if (a._rollDataProvider) {
                    if (a._rollDataProvider.destroy) {
                        a._rollDataProvider.destroy();
                    }
                    a._rollDataProvider = null;
                }
                a._rollDataProvider = new CRollDataProvider(menuItems);
                a._dataChanged = true;
            };
            CCarouselControl.prototype.getViewCount = function () {
                var a = this;
                return a._viewCount;
            };
            CCarouselControl.prototype.getCurrentIndex = function () {
                var a = this;
                return a._rollDataProvider.cur();
            };
            CCarouselControl.prototype.getCurrentItem = function () {
                var a = this;
                return a._rollDataProvider.curItem();
            };
            CCarouselControl.prototype.setCurrentIndex = function (index) {
                var a = this;
                if (index < 0 || a._rollDataProvider.length() <= index) {
                    throw "CCarouselControl OUT OF BOUNDS";
                }
                a._rollDataProvider.setCur(index);
                fnDeliverFocusChanged(a);
            };
            CCarouselControl.prototype.getCurrentViewingItemInfos = function ($el) {
                var a = this, drawParam = a._drawParam, viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, items = a._rollDataProvider.get(viewCount, anchorIndex), ret = [], i, item, $self = a._$self, $item;
                for (i = 0; i < items.length; i += 1) {
                    item = items[i];
                    if (item) {
                        $item = $self.find('.' + cssPrefix + 'carousel-item' + '[data="' + item.index + '"]');
                        ret.push({
                            item: item.data,
                            itemKey: item.index,
                            $item: $item.length ? $item : null
                        });
                    }
                    else {
                        ret.push(null);
                    }
                }
                return ret;
            };
            CCarouselControl.prototype.jqCurrent = function () {
                var a = this;
                return a._jq.children('.-roll-menu-dist0');
            };
            CCarouselControl.prototype.safeUpdate = function (fnUpdate) {
                var a = this;
                if (a._onTransition) {
                    a.fnSafeUpdate = fnUpdate;
                }
                else {
                    fnUpdate();
                }
            };
            CCarouselControl.prototype.doDraw = function (param) {
                var a = this, drawParam = a.getDrawParam();
                var align = drawParam.align === undefined ? 'vertical' : drawParam.align;
                a._onTransition = false;
                if (align === 'horizontal') {
                    a._keyMapBuilder = leftToRightBuilder;
                    return a.doDrawHorizontal(param);
                }
                a._keyMapBuilder = topToDownBuilder;
                return a.doDrawVertical(param);
            };
            CCarouselControl.prototype.doDrawHorizontal = function (param) {
                var a = this, menuLen = a._rollDataProvider.length(), height = a._$self.height(), drawParam = a.getDrawParam(), viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, itemWidth = drawParam.itemWidth, anchorWidth = drawParam.anchorWidth || itemWidth, anchorLeft = anchorIndex * itemWidth, startPosition = drawParam.drawEffect === 'spreadOut' ? anchorLeft : 0, items, nextLeft = 0, drawInfos = [], drawnItems = [], i;
                //
                a._$self.empty();
                if (drawParam.startIndex !== undefined) {
                    a._rollDataProvider.setCur(drawParam.startIndex);
                }
                items = a._rollDataProvider.get(viewCount, anchorIndex);
                if (drawParam.transparentAnchor) {
                    a._upperBoundWidth = anchorIndex * itemWidth;
                    a._$upperBound = $('<div>').css({
                        'position': 'absolute',
                        'height': height + 'px',
                        'width': a._upperBoundWidth + 'px',
                        'overflow': 'hidden'
                    });
                    a._lowerBoundLeft = anchorIndex * itemWidth + anchorWidth;
                    a._lowerBoundWidth = (viewCount - anchorIndex - 1) * itemWidth;
                    a._$lowerBound = $('<div>').css({
                        'position': 'absolute',
                        'left': a._lowerBoundLeft + 'px',
                        'height': height + 'px',
                        'width': a._lowerBoundWidth + 'px',
                        'overflow': 'hidden'
                    });
                    a._$self.append(a._$upperBound);
                    a._$self.append(a._$lowerBound);
                }
                for (i = 0; i < items.length; i += 1) {
                    var $parent = null;
                    var itemLeft = nextLeft;
                    var itemLeftAnim = startPosition;
                    if (drawParam.transparentAnchor) {
                        if (i < anchorIndex) {
                            $parent = a._$upperBound;
                            itemLeftAnim = a._lowerBoundWidth;
                        }
                        else if (anchorIndex < i) {
                            itemLeft = nextLeft - a._lowerBoundLeft;
                            itemLeftAnim = -itemWidth;
                            $parent = a._$lowerBound;
                        }
                    }
                    else {
                        $parent = a._$self;
                    }
                    drawInfos.push({
                        'left': itemLeft,
                        '$parent': $parent,
                        'leftAnim': itemLeftAnim
                    });
                    nextLeft += i === anchorIndex ? anchorWidth : itemWidth;
                }
                for (i = 0; i < drawInfos.length; i += 1) {
                    var item = items[i];
                    var diff = i - anchorIndex;
                    var dist = Math.abs(diff);
                    var distClassName = cssPrefix + 'carousel-dist' + dist;
                    var drawInfo = drawInfos[i];
                    var $item = null;
                    if (drawInfo.$parent) {
                        if (drawParam.drawEffect === 'spreadOut') {
                            $item = createItem(a, item, drawInfo.leftAnim, distClassName);
                        }
                        else {
                            $item = createItem(a, item, drawInfo.left, distClassName);
                        }
                        if (diff !== 0) {
                            if (diff < 0) {
                                $item.addClass('-l');
                            }
                            else {
                                $item.addClass('-r');
                            }
                        }
                        drawInfo.$parent.append($item);
                    }
                    drawnItems.push($item);
                }
                if (drawParam.drawEffect === 'spreadOut') {
                    setTimeout(function () {
                        for (i = 0; i < drawInfos.length; i += 1) {
                            if (drawnItems[i]) {
                                drawnItems[i].css('left', drawInfos[i].left + 'px');
                            }
                        }
                    }, 1);
                }
                var $anchor = $('<div>', {
                    'class': cssPrefix + 'carousel-anchor'
                });
                $anchor.css({
                    'position': 'absolute',
                    'left': anchorLeft + 'px',
                    'width': anchorWidth + 'px',
                    'height': height + 'px'
                });
                if (a._anchorDrawer) {
                    a._anchorDrawer({
                        '$anchor': $anchor,
                        'currentItem': items[anchorIndex]
                    });
                }
                a._$self.append($anchor);
                a._width = anchorWidth + (itemWidth * (viewCount - 1));
                a._$anchor = $anchor;
                a._$self.css('width', a._width + 'px');
                if (a._dataChanged) {
                    a._dataChanged = false;
                    fnDeliverFocusChanged(a);
                }
                return [];
            };
            CCarouselControl.prototype.doDrawVertical = function (param) {
                var a = this, menuLen = a._rollDataProvider.length(), width = a._$self.width(), drawParam = a.getDrawParam(), viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, itemHeight = drawParam.itemHeight, anchorHeight = drawParam.anchorHeight || itemHeight, anchorTop = anchorIndex * itemHeight, startPosition = drawParam.drawEffect === 'spreadOut' ? anchorTop : 0, items, nextTop = 0, drawInfos = [], drawnItems = [], i;
                //
                a._$self.empty();
                if (drawParam.startIndex !== undefined) {
                    a._rollDataProvider.setCur(drawParam.startIndex);
                }
                items = a._rollDataProvider.get(viewCount, anchorIndex);
                if (drawParam.transparentAnchor) {
                    a._upperBoundHeight = anchorIndex * itemHeight;
                    a._$upperBound = $('<div>').css({
                        'position': 'absolute',
                        'width': width + 'px',
                        'height': a._upperBoundHeight + 'px',
                        'overflow': 'hidden'
                    });
                    a._lowerBoundTop = anchorIndex * itemHeight + anchorHeight;
                    a._lowerBoundHeight = (viewCount - anchorIndex - 1) * itemHeight;
                    a._$lowerBound = $('<div>').css({
                        'position': 'absolute',
                        'top': a._lowerBoundTop + 'px',
                        'width': width + 'px',
                        'height': a._lowerBoundHeight + 'px',
                        'overflow': 'hidden'
                    });
                    a._$self.append(a._$upperBound);
                    a._$self.append(a._$lowerBound);
                }
                for (i = 0; i < items.length; i += 1) {
                    var $parent = null;
                    var itemTop = nextTop;
                    var itemTopAnim = startPosition;
                    if (drawParam.transparentAnchor) {
                        if (i < anchorIndex) {
                            $parent = a._$upperBound;
                            itemTopAnim = a._upperBoundHeight;
                        }
                        else if (anchorIndex < i) {
                            itemTop = nextTop - a._lowerBoundTop;
                            itemTopAnim = -itemHeight;
                            $parent = a._$lowerBound;
                        }
                    }
                    else {
                        $parent = a._$self;
                    }
                    drawInfos.push({
                        'top': itemTop,
                        '$parent': $parent,
                        'topAnim': itemTopAnim
                    });
                    nextTop += i === anchorIndex ? anchorHeight : itemHeight;
                }
                for (i = 0; i < drawInfos.length; i += 1) {
                    var item = items[i];
                    var diff = i - anchorIndex;
                    var dist = Math.abs(diff);
                    var distClassName = cssPrefix + 'carousel-dist' + dist;
                    var drawInfo = drawInfos[i];
                    var $item = null;
                    if (drawInfo.$parent) {
                        if (drawParam.drawEffect === 'spreadOut') {
                            $item = createItem(a, item, drawInfo.topAnim, distClassName);
                        }
                        else {
                            $item = createItem(a, item, drawInfo.top, distClassName);
                        }
                        if (diff !== 0) {
                            if (diff < 0) {
                                $item.addClass('-u');
                            }
                            else {
                                $item.addClass('-d');
                            }
                        }
                        drawInfo.$parent.append($item);
                    }
                    drawnItems.push($item);
                }
                if (drawParam.drawEffect === 'spreadOut') {
                    setTimeout(function () {
                        for (i = 0; i < drawInfos.length; i += 1) {
                            if (drawnItems[i]) {
                                drawnItems[i].css('top', drawInfos[i].top + 'px');
                            }
                        }
                    }, 1);
                }
                var $anchor = $('<div>', {
                    'class': cssPrefix + 'carousel-anchor'
                });
                $anchor.css({
                    'position': 'absolute',
                    'top': anchorTop + 'px',
                    'height': anchorHeight + 'px',
                    'width': width + 'px'
                });
                if (a._anchorDrawer) {
                    a._anchorDrawer({
                        '$anchor': $anchor,
                        'currentItem': items[anchorIndex]
                    });
                }
                a._$self.append($anchor);
                a._height = anchorHeight + (itemHeight * (viewCount - 1));
                a._$anchor = $anchor;
                a._$self.css('height', a._height + 'px');
                if (a._dataChanged) {
                    a._dataChanged = false;
                    fnDeliverFocusChanged(a);
                }
                return [];
            };
            CCarouselControl.prototype._update = function (down) {
                var a = this, menuLen = a._rollDataProvider.length(), drawParam = a.getDrawParam(), anchorIndex = drawParam.anchorIndex, align = drawParam.align === undefined ? 'vertical' : drawParam.align, css = {
                    'position': 'absolute'
                }, nextTop = 0, prev = down ? 1 : -1, $prevItem = null, $currItem = null, itemOffset, anchorOffset, fnMoveItemUpper, fnMoveItemlower;
                //
                if (align === 'vertical') {
                    itemOffset = drawParam.itemHeight;
                    anchorOffset = drawParam.anchorHeight;
                    fnMoveItemUpper = moveItemsUpward;
                    fnMoveItemlower = moveItemsDownward;
                }
                else {
                    itemOffset = drawParam.itemWidth;
                    anchorOffset = drawParam.anchorWidth;
                    fnMoveItemUpper = moveItemsRightward;
                    fnMoveItemlower = moveItemsLeftward;
                }
                if (a.onFocusStartToChange) {
                    a.onFocusStartToChange({
                        '$anchor': a._$anchor,
                        'item': a._rollDataProvider.curItem()
                    });
                }
                if (drawParam.transparentAnchor) {
                    var $upperItems = a._$upperBound.children('.' + cssPrefix + 'carousel-item');
                    var $lowerItems = a._$lowerBound.children('.' + cssPrefix + 'carousel-item');
                    if (down) {
                        fnMoveItemUpper($upperItems, itemOffset, anchorIndex, -1);
                        fnMoveItemUpper($lowerItems, itemOffset, anchorIndex, anchorIndex);
                        if ($lowerItems.length) {
                            $upperItems.first().remove();
                            $lowerItems.first().remove();
                        }
                        else {
                            $upperItems.first().remove();
                        }
                    }
                    else {
                        fnMoveItemlower($upperItems, itemOffset, anchorIndex, 0);
                        fnMoveItemlower($lowerItems, itemOffset, anchorIndex, anchorIndex + 1);
                        if ($lowerItems.length) {
                            $upperItems.last().remove();
                            $lowerItems.last().remove();
                        }
                        else {
                            $upperItems.last().remove();
                        }
                    }
                }
                else {
                    var $items = a._$self.children('.' + cssPrefix + 'carousel-item');
                    $items.css('opacity', '0');
                    if (down) {
                        fnMoveItemUpper($items, itemOffset, anchorIndex, -1, anchorOffset);
                        $items.first().remove();
                    }
                    else {
                        fnMoveItemlower($items, itemOffset, anchorIndex, 0, anchorOffset);
                        $items.last().remove();
                    }
                    $items.css('opacity', '1');
                }
                fnDeliverFocusChanged(a);
                /* DEPRECATED
                 if (a.onRollMenuAboutToChange) {
                 a.onRollMenuAboutToChange(a._rollDataProvider.cur(), $prevItem, $currItem);
                 }
                 */
            };
            CCarouselControl.prototype._animate = function (down) {
                var a = this;
                a._onTransition = true;
                //setTimeout(function() {
                var menuLen = a._rollDataProvider.length(), drawParam = a.getDrawParam(), align = drawParam.align === undefined ? 'vertical' : drawParam.align, anchorIndex = drawParam.anchorIndex, css = {
                    'position': 'absolute'
                }, nextTop = 0, prev = down ? 1 : -1, $prevItem = null, $currItem = null, itemOffset, anchorOffset, fnMoveItemUpper, fnMoveItemlower;
                //
                if (align === 'vertical') {
                    itemOffset = drawParam.itemHeight;
                    anchorOffset = drawParam.anchorHeight;
                    fnMoveItemUpper = moveItemsUpward;
                    fnMoveItemlower = moveItemsDownward;
                }
                else {
                    itemOffset = drawParam.itemWidth;
                    anchorOffset = drawParam.anchorWidth;
                    fnMoveItemUpper = moveItemsRightward;
                    fnMoveItemlower = moveItemsLeftward;
                }
                if (a.onFocusStartToChange) {
                    a.onFocusStartToChange({
                        '$anchor': a._$anchor,
                        'item': a._rollDataProvider.curItem()
                    });
                }
                if (drawParam.transparentAnchor) {
                    var $upperItems = a._$upperBound.children('.' + cssPrefix + 'carousel-item');
                    var $lowerItems = a._$lowerBound.children('.' + cssPrefix + 'carousel-item');
                    if (down) {
                        $upperItems.last().css('left');
                        $lowerItems.last().css('left');
                        fnMoveItemUpper($upperItems, itemOffset, anchorIndex, -1);
                        fnMoveItemUpper($lowerItems, itemOffset, anchorIndex, anchorIndex);
                        if ($lowerItems.length) {
                            doTransitionAndAfter(a, $lowerItems.last(), function () {
                                $upperItems.first().remove();
                                $lowerItems.first().remove();
                            });
                        }
                        else {
                            doTransitionAndAfter(a, $upperItems.last(), function () {
                                $upperItems.first().remove();
                            });
                        }
                    }
                    else {
                        $upperItems.first().css('left');
                        $lowerItems.first().css('left');
                        fnMoveItemlower($upperItems, itemOffset, anchorIndex, 0);
                        fnMoveItemlower($lowerItems, itemOffset, anchorIndex, anchorIndex + 1);
                        if ($lowerItems.length) {
                            doTransitionAndAfter(a, $lowerItems.last(), function () {
                                $upperItems.last().remove();
                                $lowerItems.last().remove();
                            });
                        }
                        else {
                            doTransitionAndAfter(a, $upperItems.last(), function () {
                                $upperItems.last().remove();
                            });
                        }
                    }
                }
                else {
                    var $items = a._$self.children('.' + cssPrefix + 'carousel-item');
                    if (down) {
                        $items.last().css('left');
                        fnMoveItemUpper($items, itemOffset, anchorIndex, -1, anchorOffset);
                        doTransitionAndAfter(a, $items.last(), function () {
                            $items.first().remove();
                        });
                    }
                    else {
                        $items.first().css('left');
                        fnMoveItemlower($items, itemOffset, anchorIndex, 0, anchorOffset);
                        doTransitionAndAfter(a, $items.last(), function () {
                            $items.last().remove();
                        });
                    }
                }
                /* DEPRECATED
                 if (a.onRollMenuAboutToChange) {
                 a.onRollMenuAboutToChange(a._rollDataProvider.cur(), $prevItem, $currItem);
                 }
                 */
                //    }, 1);
            };
            CCarouselControl.prototype.doKeyLeft = function (aKeyCode) {
                var a = this, dataLen = a._rollDataProvider.length(), drawParam = a.getDrawParam(), viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, itemWidth = drawParam.itemWidth, anchorWidth = drawParam.anchorWidth, animation = drawParam.animation === undefined ? true : drawParam.animation, align = drawParam.align === undefined ? 'vertical' : drawParam.align, maxKeyQueueCount = drawParam.maxKeyQueueCount;
                //
                if (align === 'vertical') {
                    return false;
                }
                if (dataLen <= 1) {
                    return true;
                }
                if (!a._onTransition) {
                    a._rollDataProvider.dec();
                    var items = a._rollDataProvider.get(viewCount, anchorIndex);
                    if (drawParam.transparentAnchor) {
                        var $uppperItems = a._$upperBound.children('.' + cssPrefix + 'carousel-item');
                        var $lowerItems = a._$lowerBound.children('.' + cssPrefix + 'carousel-item');
                        if (dataLen < viewCount) {
                            var $newUpper = createItem(a, null, -itemWidth);
                            var $upperAll = $newUpper.add($uppperItems);
                            $uppperItems.first().before($newUpper);
                            $upperAll.each(function (i) {
                                var $item = $(this), item = items[i];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                            var $newLower = createItem(a, null, -itemWidth);
                            var $lowerAll = $newLower.add($lowerItems);
                            $lowerItems.first().before($newLower);
                            $lowerAll.each(function (i) {
                                var $item = $(this), item = items[i + anchorIndex + 1];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                        }
                        else {
                            $uppperItems.first().before(createItem(a, items[0], -itemWidth));
                            $lowerItems.first().before(createItem(a, items[anchorIndex + 1], -itemWidth));
                        }
                    }
                    else {
                        var $items = a._$self.children('.' + cssPrefix + 'carousel-item');
                        var $newItem = createItem(a, items[0], -itemWidth, '-l');
                        $items.first().before($newItem);
                        $items = $newItem.add($items);
                        if (dataLen < viewCount) {
                            $items.each(function (i) {
                                var $item = $(this), item = items[i];
                                if (!item) {
                                    $item.empty();
                                }
                                if ($item.children().length === 0 && item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                        }
                    }
                    if (animation) {
                        a._animate(false);
                    }
                    else {
                        a._update(false);
                    }
                }
                else {
                    if (maxKeyQueueCount === undefined) {
                        a._keyQueue.push('doKeyRight');
                    }
                    else if (a._keyQueue.length < maxKeyQueueCount) {
                        a._keyQueue.push('doKeyRight');
                    }
                }
                return true;
            };
            CCarouselControl.prototype.doKeyRight = function (aKeyCode) {
                var a = this, dataLen = a._rollDataProvider.length(), drawParam = a.getDrawParam(), viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, itemWidth = drawParam.itemWidth, anchorWidth = drawParam.anchorWidth, animation = drawParam.animation === undefined ? true : drawParam.animation, maxKeyQueueCount = drawParam.maxKeyQueueCount, align = drawParam.align === undefined ? 'vertical' : drawParam.align;
                if (align === 'vertical') {
                    return false;
                }
                if (dataLen <= 1) {
                    return true;
                }
                if (!a._onTransition) {
                    a._rollDataProvider.inc();
                    var items = a._rollDataProvider.get(viewCount, anchorIndex);
                    if (drawParam.transparentAnchor) {
                        var $uppperItems = a._$upperBound.children('.' + cssPrefix + 'carousel-item');
                        var $lowerItems = a._$lowerBound.children('.' + cssPrefix + 'carousel-item');
                        if (dataLen < viewCount) {
                            var $newUpper = createItem(a, null, a._upperBoundWidth);
                            var $upperAll = $uppperItems.add($newUpper);
                            $uppperItems.last().after($newUpper);
                            $upperAll.each(function (i) {
                                var $item = $(this), item = items[i - 1];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                            var $newLower = createItem(a, null, a._lowerBoundWidth);
                            var $lowerAll = $lowerItems.add($newLower);
                            $lowerItems.last().after($newLower);
                            $lowerAll.each(function (i) {
                                var $item = $(this), item = items[i + anchorIndex];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                        }
                        else {
                            $uppperItems.last().after(createItem(a, items[anchorIndex - 1], a._upperBoundWidth));
                            $lowerItems.last().after(createItem(a, items[items.length - 1], a._lowerBoundWidth));
                        }
                    }
                    else {
                        var $items = a._$self.children('.' + cssPrefix + 'carousel-item');
                        var $newItem = createItem(a, items[items.length - 1], a._width, '-r');
                        $items.last().after($newItem);
                        $items = $items.add($newItem);
                        if (dataLen < viewCount) {
                            $items.each(function (i) {
                                var $item = $(this), item = items[i - 1];
                                if (!item) {
                                    $item.empty();
                                }
                                if ($item.children().length === 0 && item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                        }
                    }
                    if (animation) {
                        a._animate(true);
                    }
                    else {
                        a._update(true);
                    }
                }
                else {
                    if (maxKeyQueueCount === undefined) {
                        a._keyQueue.push('doKeyLeft');
                    }
                    else if (a._keyQueue.length < maxKeyQueueCount) {
                        a._keyQueue.push('doKeyLeft');
                    }
                }
                return true;
            };
            CCarouselControl.prototype.doKeyUp = function (aKeyCode) {
                var a = this, dataLen = a._rollDataProvider.length(), drawParam = a.getDrawParam(), viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, itemHeight = drawParam.itemHeight, anchorHeight = drawParam.anchorHeight, animation = drawParam.animation === undefined ? true : drawParam.animation, maxKeyQueueCount = drawParam.maxKeyQueueCount, align = drawParam.align === undefined ? 'vertical' : drawParam.align;
                if (align === 'horizontal') {
                    return false;
                }
                if (dataLen <= 1) {
                    return true;
                }
                if (!a._onTransition) {
                    a._rollDataProvider.dec();
                    var items = a._rollDataProvider.get(viewCount, anchorIndex);
                    if (drawParam.transparentAnchor) {
                        var $uppperItems = a._$upperBound.children('.' + cssPrefix + 'carousel-item');
                        var $lowerItems = a._$lowerBound.children('.' + cssPrefix + 'carousel-item');
                        var $newUpper, $newLower;
                        if (dataLen < viewCount) {
                            $newUpper = createItem(a, null, -itemHeight);
                            var $upperAll = $newUpper.add($uppperItems);
                            $uppperItems.first().before($newUpper);
                            $upperAll.each(function (i) {
                                var $item = $(this), item = items[i];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                            $newLower = createItem(a, null, -itemHeight);
                            var $lowerAll = $newLower.add($lowerItems);
                            $lowerItems.first().before($newLower);
                            $lowerAll.each(function (i) {
                                var $item = $(this), item = items[i + anchorIndex + 1];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                        }
                        else {
                            $newUpper = createItem(a, items[0], -itemHeight);
                            $newLower = createItem(a, items[anchorIndex + 1], -itemHeight);
                            $uppperItems.first().before($newUpper);
                            $lowerItems.first().before($newLower);
                        }
                    }
                    else {
                        var $items = a._$self.children('.' + cssPrefix + 'carousel-item');
                        var $newItem = createItem(a, items[0], -itemHeight, '-u');
                        $items.first().before($newItem);
                        $items = $newItem.add($items);
                        if (dataLen < viewCount) {
                            $items.each(function (i) {
                                var $item = $(this), item = items[i];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                        }
                    }
                    if (animation) {
                        a._animate(false);
                    }
                    else {
                        a._update(false);
                    }
                }
                else {
                    if (maxKeyQueueCount === undefined) {
                        a._keyQueue.push('doKeyUp');
                    }
                    else if (a._keyQueue.length < maxKeyQueueCount) {
                        a._keyQueue.push('doKeyUp');
                    }
                }
                return true;
            };
            CCarouselControl.prototype.doKeyDown = function (aKeyCode) {
                var a = this, dataLen = a._rollDataProvider.length(), drawParam = a.getDrawParam(), viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, itemHeight = drawParam.itemHeight, anchorHeight = drawParam.anchorHeight, animation = drawParam.animation === undefined ? true : drawParam.animation, maxKeyQueueCount = drawParam.maxKeyQueueCount, align = drawParam.align === undefined ? 'vertical' : drawParam.align;
                if (align === 'horizontal') {
                    return false;
                }
                if (dataLen <= 1) {
                    return true;
                }
                if (!a._onTransition) {
                    a._rollDataProvider.inc();
                    var items = a._rollDataProvider.get(viewCount, anchorIndex);
                    if (drawParam.transparentAnchor) {
                        var $uppperItems = a._$upperBound.children('.' + cssPrefix + 'carousel-item');
                        var $lowerItems = a._$lowerBound.children('.' + cssPrefix + 'carousel-item');
                        var $newUpper, $newLower;
                        if (dataLen < viewCount) {
                            $newUpper = createItem(a, null, a._upperBoundHeight);
                            var $upperAll = $uppperItems.add($newUpper);
                            $uppperItems.last().after($newUpper);
                            $upperAll.each(function (i) {
                                var $item = $(this), item = items[i - 1];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                            $newLower = createItem(a, null, a._lowerBoundHeight);
                            var $lowerAll = $lowerItems.add($newLower);
                            $lowerItems.last().after($newLower);
                            $lowerAll.each(function (i) {
                                var $item = $(this), item = items[i + anchorIndex];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data, item.index);
                                }
                            });
                        }
                        else {
                            $newUpper = createItem(a, items[anchorIndex - 1], a._upperBoundHeight);
                            $newLower = createItem(a, items[items.length - 1], a._lowerBoundHeight);
                            $uppperItems.last().after($newUpper);
                            $lowerItems.last().after($newLower);
                        }
                    }
                    else {
                        var $items = a._$self.children('.' + cssPrefix + 'carousel-item');
                        var $newItem = createItem(a, items[items.length - 1], a._height, '-d');
                        $items.last().after($newItem);
                        $items = $items.add($newItem);
                        if (dataLen < viewCount) {
                            $items.each(function (i) {
                                var $item = $(this), item = items[i - 1];
                                $item.empty();
                                if (item) {
                                    drawItem(a, $item, item.data);
                                }
                            });
                        }
                    }
                    if (animation) {
                        a._animate(true);
                    }
                    else {
                        a._update(true);
                    }
                }
                else {
                    if (maxKeyQueueCount === undefined) {
                        a._keyQueue.push('doKeyDown');
                    }
                    else if (a._keyQueue.length < maxKeyQueueCount) {
                        a._keyQueue.push('doKeyDown');
                    }
                }
                return true;
            };
            CCarouselControl.prototype.doKeyPageUp = function (aKeyCode) {
                var a = this, dataLen = a._rollDataProvider.length(), drawParam = a.getDrawParam(), viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, itemHeight = drawParam.itemHeight, anchorHeight = drawParam.anchorHeight;
                //
                if (dataLen <= 1) {
                    return true;
                }
                if (!a._onTransition) {
                    a._rollDataProvider.inc(-viewCount);
                    a.draw();
                    fnDeliverFocusChanged(a);
                }
                return true;
            };
            CCarouselControl.prototype.doKeyPageDown = function (aKeyCode) {
                var a = this, dataLen = a._rollDataProvider.length(), drawParam = a.getDrawParam(), viewCount = drawParam.viewCount, anchorIndex = drawParam.anchorIndex, itemHeight = drawParam.itemHeight, anchorHeight = drawParam.anchorHeight;
                //
                if (dataLen <= 1) {
                    return true;
                }
                if (!a._onTransition) {
                    a._rollDataProvider.inc(viewCount);
                    a.draw();
                    fnDeliverFocusChanged(a);
                }
                return true;
            };
            CCarouselControl.prototype.doKeyOk = function (aKeyCode) {
                var a = this;
                if (!a._onTransition) {
                    if (this.onItemSelected) {
                        this.onItemSelected({
                            '$anchor': a._$anchor,
                            'item': a._rollDataProvider.curItem(),
                            'key': a._rollDataProvider.cur()
                        });
                    }
                }
                return true;
            };
            return CCarouselControl;
        })(CControl);
        mControl.CCarouselControl = CCarouselControl;
        var CLayoutControl = (function (_super) {
            __extends(CLayoutControl, _super);
            function CLayoutControl() {
                _super.call(this);
            }
            CLayoutControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
            };
            CLayoutControl.prototype.setItemDrawers = function (drawers) {
                var a = this;
                a._itemDrawers = drawers;
            };
            CLayoutControl.prototype.isFocusable = function () {
                var a = this;
                return a._keyMap && a._keyMap.length;
            };
            return CLayoutControl;
        })(CControl);
        mControl.CLayoutControl = CLayoutControl;
        var CVerticalLayoutControl = (function (_super) {
            __extends(CVerticalLayoutControl, _super);
            function CVerticalLayoutControl($el) {
                var _this = this;
                _super.call(this);
                this.onFocusChanged = function ($old, $new) {
                    var a = _this;
                    var selector = a._marqueeSelector;
                    if (selector) {
                        marqueeOff($old, selector);
                        marqueeOn($new, selector);
                    }
                };
                if (arguments.length) {
                    this.init($el);
                }
            }
            CVerticalLayoutControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a._keyMapBuilder = topToDownBuilder;
            };
            CVerticalLayoutControl.prototype.doKey = function (keyCode) {
                var a = this, handled = _super.prototype.doKey.call(a, keyCode);
                if (a._drawParam.setFocusRolling && !handled) {
                    if (keyCode === KKeyDown) {
                        a.setFocusedItem(0);
                    }
                    else if (keyCode === KKeyUp) {
                        a.setFocusedItem(a._keyMap.length - 1);
                    }
                }
                return handled;
            };
            CVerticalLayoutControl.prototype.doDraw = function () {
                var a = this, $self = a._$self, top = 0, bottom = 0, focusableItems = [], focusableItemsBottom = [], drawParam = a.getDrawParam(), padding = drawParam.padding || 0, alignCenter = (drawParam.childAlign && drawParam.childAlign === 'center'), i, j, $item, drawer, spanning = false, maxWidth = 0, left = 0;
                //
                $self.empty();
                for (i = 0; i < a._itemDrawers.length; i += 1) {
                    drawer = a._itemDrawers[i];
                    if (drawer) {
                        $item = $('<div>', {
                            'class': cssFocusable
                        }).attr('data', i);
                        $item.css({
                            'position': 'absolute',
                            'top': top + 'px'
                        });
                        $item.appendTo($self);
                        if (drawer(i, $item, a)) {
                            $item.addClass(cssFocused + ' ' + cssActiveFocused);
                        }
                        if ($item.hasClass(cssFocusable)) {
                            focusableItems.push($item);
                        }
                        top += ($item.outerHeight() + padding);
                        maxWidth = Math.max(maxWidth, $item.outerWidth());
                    }
                    else {
                        spanning = true;
                        break;
                    }
                }
                if (spanning) {
                    for (j = a._itemDrawers.length - 1; j > i; j -= 1) {
                        drawer = a._itemDrawers[j];
                        if (drawer) {
                            $item = $('<div>', {
                                'class': cssFocusable
                            }).attr('data', j);
                            $item.css({
                                'position': 'absolute',
                                'bottom': bottom + 'px'
                            });
                            $item.appendTo($self);
                            if (drawer(j, $item)) {
                                $item.addClass(cssFocused + ' ' + cssActiveFocused);
                            }
                            if ($item.hasClass(cssFocusable)) {
                                focusableItemsBottom.push($item);
                            }
                            bottom += ($item.outerHeight() + padding);
                        }
                        else {
                            break;
                        }
                    }
                }
                else if (alignCenter) {
                    top = Math.floor(($self.height() - (top - padding)) / 2);
                    left = Math.floor(($self.width() - maxWidth) / 2);
                    for (i = 0; i < focusableItems.length; i += 1) {
                        $item = focusableItems[i];
                        $item.css({
                            'top': top + 'px',
                            'left': left + 'px'
                        });
                        top += ($item.outerHeight() + padding);
                    }
                }
                if ($self.width() === 0 && maxWidth) {
                    $self.css('width', maxWidth + 'px');
                }
                if ($self.height() === 0) {
                    $self.css('height', top - padding + 'px');
                }
                focusableItems = focusableItems.concat(focusableItemsBottom.reverse());
                return $(focusableItems);
            };
            CVerticalLayoutControl.prototype.marqueeOn = function (selector) {
                this._marqueeSelector = selector;
            };
            CVerticalLayoutControl.prototype.marqueeOff = function () {
                this._marqueeSelector = null;
            };
            CVerticalLayoutControl.prototype.onActiveFocusedGained = function () {
                var a = this;
                var selector = a._marqueeSelector;
                if (selector) {
                    marqueeOn(a.getFocusedItem().$item, selector);
                }
            };
            CVerticalLayoutControl.prototype.onActiveFocusedLost = function () {
                var a = this;
                var selector = a._marqueeSelector;
                if (selector) {
                    marqueeOff(a.getFocusedItem().$item, selector);
                }
            };
            return CVerticalLayoutControl;
        })(CLayoutControl);
        mControl.CVerticalLayoutControl = CVerticalLayoutControl;
        var CHorizontalLayoutControl = (function (_super) {
            __extends(CHorizontalLayoutControl, _super);
            function CHorizontalLayoutControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CHorizontalLayoutControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a._keyMapBuilder = leftToRightBuilder;
            };
            CHorizontalLayoutControl.prototype.doKey = function (keyCode) {
                var a = this, handled = _super.prototype.doKey.call(a, keyCode);
                if (a._drawParam.setFocusRolling && !handled) {
                    if (keyCode === KKeyRight) {
                        a.setFocusedItem(0);
                        handled = true;
                    }
                    else if (keyCode === KKeyLeft) {
                        a.setFocusedItem(a._keyMap.length - 1);
                        handled = true;
                    }
                }
                return handled;
            };
            CHorizontalLayoutControl.prototype.doDraw = function () {
                var a = this, $self = a._$self, left = 0, right = 0, drawnItems = [], focusableItems = [], focusableItemsBottom = [], drawParam = a.getDrawParam(), padding = drawParam.padding || 0, alignCenter = (drawParam.childAlign && drawParam.childAlign === 'center'), i, j, $item, drawer, spanning = false, maxHeight = 0, top = 0;
                //
                $self.empty();
                for (i = 0; i < a._itemDrawers.length; i += 1) {
                    drawer = a._itemDrawers[i];
                    if (drawer) {
                        $item = $('<div>', {
                            'class': cssFocusable
                        }).attr('data', i);
                        $item.css({
                            'position': 'absolute',
                            'left': left + 'px'
                        });
                        $item.appendTo($self);
                        drawnItems.push($item);
                        if (drawer(i, $item)) {
                            $item.addClass(cssFocused + ' ' + cssActiveFocused);
                        }
                        if ($item.hasClass(cssFocusable)) {
                            focusableItems.push($item);
                        }
                        left += ($item.outerWidth() + padding);
                        maxHeight = Math.max(maxHeight, $item.outerHeight());
                    }
                    else {
                        spanning = true;
                        break;
                    }
                }
                if (spanning) {
                    for (j = a._itemDrawers.length - 1; j > i; j -= 1) {
                        drawer = a._itemDrawers[j];
                        if (drawer) {
                            $item = $('<div>', {
                                'class': cssFocusable
                            }).attr('data', j);
                            $item.css({
                                'position': 'absolute',
                                'right': right + 'px'
                            });
                            $item.appendTo($self);
                            if (drawer(j, $item)) {
                                $item.addClass(cssFocused + ' ' + cssActiveFocused);
                            }
                            if ($item.hasClass(cssFocusable)) {
                                focusableItemsBottom.push($item);
                            }
                            right += ($item.outerWidth() + padding);
                        }
                        else {
                            break;
                        }
                    }
                }
                else if (alignCenter) {
                    top = Math.floor(($self.height() - maxHeight) / 2);
                    left = Math.floor(($self.width() - (left - padding)) / 2);
                    for (i = 0; i < drawnItems.length; i += 1) {
                        $item = drawnItems[i];
                        $item.css({
                            'top': top + 'px',
                            'left': left + 'px'
                        });
                        left += ($item.outerWidth() + padding);
                    }
                }
                if ($self.width() === 0) {
                    $self.css('width', left - padding + 'px');
                }
                if ($self.height() === 0 && maxHeight) {
                    $self.css('height', maxHeight + 'px');
                }
                focusableItems = focusableItems.concat(focusableItemsBottom.reverse());
                return $(focusableItems);
            };
            return CHorizontalLayoutControl;
        })(CLayoutControl);
        mControl.CHorizontalLayoutControl = CHorizontalLayoutControl;
        var CViewControl = (function (_super) {
            __extends(CViewControl, _super);
            function CViewControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CViewControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a._$self.addClass(cssPrefix + 'ctrl-view');
                a._$self.css({
                    'overflow': 'hidden'
                });
                a._keyMapBuilder = topToDownBuilder;
            };
            CViewControl.prototype.doDraw = function ($el) {
                var a = this, $item = a.doDrawItem(), cssValue = {
                    'position': 'absolute',
                    'top': '0px' //
                };
                //
                if (a._$item) {
                    a._$item.remove();
                    delete a._$item;
                }
                cssValue[cssPropTransition] = cssTransitionParamPos;
                $item.css(cssValue);
                a._$self.append($item);
                if ($item.width() === 0) {
                    $item.css('width', 'inherit');
                }
                var width = a._$self.width();
                var height = a._$self.height();
                var itemWidth = $item.width();
                var itemHeight = $item.height();
                var drawParam = a.getDrawParam();
                console.log('height=' + height + ', itemHeight=' + itemHeight);
                if (height < itemHeight) {
                    var $scrollRight = a._$scrollRight;
                    if (!$scrollRight) {
                        $scrollRight = $('<div>', {
                            'class': cssPrefix + 'scroll'
                        });
                        $scrollRight.css({
                            'position': 'absolute',
                            'top': '0px',
                            'right': '0px',
                            'height': height + 'px'
                        });
                        if (drawParam.scrollClass) {
                            $scrollRight.addClass(drawParam.scrollClass);
                        }
                        a._$self.append($scrollRight);
                        a._$scrollRight = $scrollRight;
                    }
                    var $scrollRightTrack = a._$scrollRightTrack;
                    if (!$scrollRightTrack) {
                        $scrollRightTrack = $('<div>', {
                            'class': cssPrefix + 'scroll-track'
                        });
                        cssValue = {
                            'position': 'absolute',
                            'width': 'inherit'
                        };
                        cssValue[cssPropTransition] = cssTransitionParamPos;
                        $scrollRightTrack.css(cssValue);
                        if (drawParam.scrollClass) {
                            $scrollRightTrack.addClass(drawParam.scrollClass);
                        }
                        a._$scrollRightTrack = $scrollRightTrack;
                        $scrollRight.append($scrollRightTrack);
                        a._$scrollRightTrack._customHeight = $scrollRightTrack.height();
                        a._$scrollRightTrack._defaultHeight = Math.floor((height / itemHeight) * height);
                    }
                    if (a._$scrollRightTrack._customHeight) {
                        $scrollRightTrack.css({
                            'top': '0px'
                        });
                    }
                    else {
                        $scrollRightTrack.css({
                            'top': '0px',
                            'height': a._$scrollRightTrack._defaultHeight + 'px'
                        });
                    }
                }
                else {
                    if (a._$scrollRight) {
                        a._$scrollRight.remove();
                        delete a._$scrollRightTrack;
                        delete a._$scrollRight;
                    }
                }
                a._$item = $item;
                return $([]);
            };
            CViewControl.prototype.isFocusable = function () {
                var a = this;
                return a._$scrollRight ? true : false;
            };
            CViewControl.prototype.doKey = function (keyCode) {
                var a = this, height = a._$self.height(), itemHeight = a._$item.height(), minTop = height - itemHeight, itemTop = parseInt(a._$item.css('top'), 10), scrollUnit = a._drawParam.scrollUnit || height, itemTopTarget = itemTop;
                //
                if (a.isTransitioning()) {
                    return true;
                }
                switch (keyCode) {
                    case KKeyUp:
                        itemTopTarget = itemTop + scrollUnit;
                        if (0 < itemTopTarget) {
                            itemTopTarget = 0;
                        }
                        break;
                    case KKeyDown:
                        itemTopTarget = itemTop - scrollUnit;
                        if (itemTopTarget < minTop) {
                            itemTopTarget = minTop;
                        }
                        break;
                    case KKeyPageUp:
                        itemTopTarget = itemTop + height;
                        if (0 < itemTopTarget) {
                            itemTopTarget = 0;
                        }
                        break;
                    case KKeyPageDown:
                        itemTopTarget = itemTop - height;
                        if (itemTopTarget < minTop) {
                            itemTopTarget = minTop;
                        }
                        break;
                }
                if (itemTopTarget !== itemTop && a._$scrollRightTrack) {
                    if (a._$scrollRightTrack._customHeight) {
                        a._$scrollRightTrack.css('top', Math.floor((itemTopTarget / minTop) * (height - a._$scrollRightTrack._customHeight)) + 'px');
                    }
                    else {
                        a._$scrollRightTrack.css('top', Math.floor((-itemTopTarget / itemHeight) * height) + 'px');
                    }
                    a.setTransition(true);
                    a._$item.css('top', itemTopTarget + 'px').one(cssEvntTransitionEnd, function () {
                        a.setTransition(false);
                    });
                    return true;
                }
                return false;
            };
            return CViewControl;
        })(CControl);
        mControl.CViewControl = CViewControl;
        var CLayeredGroupControl = (function (_super) {
            __extends(CLayeredGroupControl, _super);
            function CLayeredGroupControl($el) {
                _super.call(this);
                if (arguments.length) {
                    this.init($el);
                }
            }
            CLayeredGroupControl.prototype.init = function ($el) {
                var a = this;
                _super.prototype.init.call(a, $el);
                a._layerStack = [];
            };
            CLayeredGroupControl.prototype.destroy = function () {
                var a = this, i, layerInfo, controls, j;
                for (i = 0; i < a._layerStack.length; i += 1) {
                    layerInfo = a._layerStack[i];
                    controls = layerInfo.controls;
                    if (controls) {
                        for (j = 0; j < controls.length; j += 1) {
                            controls[j].destroy();
                        }
                    }
                    layerInfo.$layer.remove();
                }
                _super.prototype.destroy.call(a);
                a._shown = false;
            };
            CLayeredGroupControl.prototype.setCommonTag = function (commonTagInfo) {
                var a = this, key;
                for (key in commonTagInfo) {
                    if (commonTagInfo.hasOwnProperty(key)) {
                        var className = commonTagInfo[key];
                        a['$' + key] = (function () {
                            var localClassName = className;
                            return function () {
                                var tag = a._$layer ? a._$layer.children('.' + localClassName) : null;
                                if (tag && tag.length === 0) {
                                    tag = $('<div>', {
                                        'class': localClassName
                                    });
                                    a._$layer.append(tag);
                                }
                                return tag;
                            };
                        }());
                    }
                }
            };
            CLayeredGroupControl.prototype.setContextualProperties = function (contextualProperties) {
                var a = this;
                a._contextualProperties = contextualProperties;
            };
            CLayeredGroupControl.prototype.stackSize = function () {
                var a = this;
                return a._layerStack.length;
            };
            CLayeredGroupControl.prototype.$layer = function () {
                var a = this;
                return a._$layer;
            };
            CLayeredGroupControl.prototype.createLayer = function (param) {
                var a = this, cssValue = {
                    'position': 'absolute',
                    'width': 'inherit',
                    'height': 'inherit' //
                }, $prevLayer = a._$layer, executeNext = [], i, len;
                //
                var stackIndex = 0;
                if (a._$layer) {
                    var contextualProperties = null;
                    if (a._contextualProperties) {
                        contextualProperties = {};
                        for (i = 0, len = a._contextualProperties.length; i < len; i += 1) {
                            var property = a._contextualProperties[i];
                            contextualProperties[property] = a[property];
                            delete a[property];
                        }
                    }
                    stackIndex = a._layerStack.push({
                        createParam: a._createParam,
                        $layer: a._$layer.addClass(cssPrefix + 'stacked'),
                        keyMap: a._keyMap,
                        currentIndex: a._currentIndex,
                        controls: a._controls,
                        contextualProperties: contextualProperties
                    });
                    a._createParam = null;
                    a._$layer = null;
                    a._keyMap = null;
                    a._currentIndex = -1;
                    a._controls = [];
                }
                var $layer = $('<div>');
                if (param.addClass) {
                    $layer.addClass(param.addClass);
                }
                if (param.zindex) {
                    $layer.css('z-index', param.zindex);
                }
                if (param.transition) {
                    var transitionParam = param.transition;
                    if (transitionParam.fn) {
                        executeNext.push(transitionParam.fn({
                            $prevLayer: $prevLayer,
                            $layer: $layer.css(cssPropTransition, cssTransitionParamOpa + ',' + cssTransitionParamPos)
                        }));
                    }
                    else {
                        switch (transitionParam.prevLayer) {
                            case 'fadeOut':
                                if ($prevLayer) {
                                    $prevLayer.css(cssPropTransition, cssTransitionParamOpa);
                                    executeNext.push(function () {
                                        $prevLayer.css('opacity', '0');
                                    });
                                }
                                break;
                            case 'moveLeft':
                                $prevLayer.css(cssPropTransition, cssTransitionParamPos);
                                executeNext.push(function () {
                                    $prevLayer.css('left', -$prevLayer.width() + 'px');
                                });
                                break;
                            case 'moveDown':
                                $prevLayer.css(cssPropTransition, cssTransitionParamPos);
                                executeNext.push(function () {
                                    $prevLayer.css('top', a._$self.height() + 'px');
                                });
                                break;
                        }
                        switch (transitionParam.newLayer) {
                            case 'fadeIn':
                                cssValue.opacity = '0';
                                cssValue[cssPropTransition] = cssTransitionParamOpa;
                                executeNext.push(function () {
                                    $layer.css('opacity', '1');
                                });
                                break;
                            case 'moveLeft':
                                cssValue[cssPropTransition] = cssTransitionParamPos;
                                cssValue.left = a._$self.width() + 'px';
                                executeNext.push(function () {
                                    $layer.css('left', '0px');
                                });
                                break;
                            case 'moveUp':
                                cssValue[cssPropTransition] = cssTransitionParamPos;
                                cssValue.top = a._$self.height() + 'px';
                                executeNext.push(function () {
                                    $layer.css('top', '0px');
                                });
                                break;
                            case 'moveDown':
                                cssValue[cssPropTransition] = cssTransitionParamPos;
                                cssValue.top = '-' + a._$self.height() + 'px';
                                executeNext.push(function () {
                                    $layer.css('top', '0px');
                                });
                                break;
                        }
                    }
                    if (transitionParam.fnAfterTransition) {
                        hx.log('warn', 'setTransition true');
                        a.setTransition(true);
                        $layer.afterTransitionOneShot(function () {
                            hx.log('warn', 'setTransition false');
                            a.setTransition(false);
                            transitionParam.fnAfterTransition();
                        }, true);
                    }
                }
                $layer.css(cssValue);
                $layer.appendTo(a._$self);
                a._$layer = $layer;
                if (executeNext.length) {
                    setTimeout(function () {
                        for (i = 0; i < executeNext.length; i += 1) {
                            executeNext[i]();
                        }
                    }, 1);
                }
                if (param.fnAfterCreate) {
                    param.fnAfterCreate.call(a);
                }
                a._createParam = param;
                a._shown = true;
                return stackIndex;
            };
            CLayeredGroupControl.prototype.removeLayer = function (targetControl) {
                var a = this, i, j, c, executeNext = [], $prevLayer, len, aLayer, aControl;
                if (targetControl) {
                    for (i = 0; i < a._layerStack.length; i += 1) {
                        aLayer = a._layerStack[i];
                        if (!aLayer.controls) {
                            continue;
                        }
                        if (aLayer.controls.indexOf(targetControl) > -1) {
                            for (j = 0; j < aLayer.controls.length; j += 1) {
                                aControl = aLayer.controls[j];
                                aControl.destroy();
                            }
                            a._layerStack.splice(i, 1);
                            aLayer.$layer.remove();
                            aLayer.$layer = null;
                            aLayer = null;
                            return;
                        }
                    }
                }
                if (a._createParam && a._createParam.transition) {
                    var transitionParam = a._createParam.transition;
                    switch (transitionParam.prevLayer) {
                        case 'fadeOut':
                            executeNext.push(function () {
                                if ($prevLayer) {
                                    $prevLayer.css('opacity', '1');
                                }
                            });
                            break;
                        case 'moveLeft':
                            executeNext.push(function () {
                                $prevLayer.css('left', '0px');
                            });
                            break;
                        case 'moveDown':
                            executeNext.push(function () {
                                $prevLayer.css('top', '0px');
                            });
                            break;
                    }
                }
                for (i = 0; i < a._controls.length; i += 1) {
                    c = a._controls[i];
                    c.destroy();
                }
                a._controls = [];
                a._currentIndex = -1;
                a._keyMap = null;
                a._$layer.remove();
                a._$layer = null;
                var layerInfo = a._layerStack.pop();
                if (layerInfo) {
                    a._controls = layerInfo.controls;
                    a._currentIndex = layerInfo.currentIndex;
                    a._keyMap = layerInfo.keyMap;
                    a._$layer = layerInfo.$layer.removeClass(cssPrefix + 'stacked');
                    a._createParam = layerInfo.createParam;
                    $prevLayer = layerInfo.$layer;
                    if (a._contextualProperties && layerInfo.contextualProperties) {
                        for (i = 0, len = a._contextualProperties.length; i < len; i += 1) {
                            var property = a._contextualProperties[i];
                            a[property] = layerInfo.contextualProperties[property];
                            delete layerInfo.contextualProperties[property];
                        }
                    }
                }
                for (i = 0; i < executeNext.length; i += 1) {
                    executeNext[i]();
                }
            };
            CLayeredGroupControl.prototype.setChildControl = function (controls, focusIndex, keyMap) {
                var a = this, i, len = controls.length;
                focusIndex = focusIndex || 0;
                if (len) {
                    for (i = 0; i < len; i += 1) {
                        var c = controls[i];
                        var $c = c._$self;
                        $c.addClass(cssFocusable);
                        $c.attr('data', i);
                        if ($c.parent().length === 0) {
                            if (a._$container) {
                                a._$container.append($c);
                            }
                            else {
                                a._$layer.append($c);
                            }
                        }
                    }
                    controls[focusIndex]._$self.addClass(cssFocused);
                }
                a._controls = controls;
                if (keyMap === 'keyMapTopDown') {
                    a._keyMapBuilder = topToDownBuilder;
                }
                else if (keyMap === 'keyMapLeftRight') {
                    a._keyMapBuilder = leftToRightBuilder;
                }
            };
            CLayeredGroupControl.prototype.doKey = function (keyCode) {
                var a = this, handled = false;
                var i, length;
                handled = _super.prototype.doKey.call(a, keyCode);
                if (!handled && a._layerStack.length && keyCode === KKeyBack) {
                    if (a._controls.length) {
                        length = a._controls.length;
                        for (i = 0; i < length; i += 1) {
                            if (!a._controls[i].isShown()) {
                                return false;
                            }
                        }
                    }
                    a.removeLayer();
                    return true;
                }
                return handled;
            };
            CLayeredGroupControl.prototype.insertChildControl = function (index, control, setFocus) {
                var a = this;
                a._insertChildControl(a._$layer, index, control, setFocus);
            };
            return CLayeredGroupControl;
        })(CGroupControl);
        mControl.CLayeredGroupControl = CLayeredGroupControl;
        window['CONTROLS_CONFIG'] = CONTROLS_CONFIG;
        if (!window['tr']) {
            window['tr'] = function (text) {
                var ret = hx.l(text);
                if (ret) {
                    return ret;
                }
                hx.log('error', '"' + text + '" is not localized');
                return text;
            };
        }
        window['CGroupControl'] = CGroupControl;
        window['CVerticalGroupControl'] = CVerticalGroupControl;
        window['CHorizontalGroupControl'] = CHorizontalGroupControl;
        window['CViewGroupControl'] = CViewGroupControl;
        window['CDataProvider'] = CDataProvider;
        window['CListControl'] = CListControl;
        window['CVariableListControls'] = CVariableListControl;
        window['CGridControl'] = CGridControl;
        window['CHorizontalGridControl'] = CHorizontalGridControl;
        window['CCarouselControl'] = CCarouselControl;
        window['CVerticalLayoutControl'] = CVerticalLayoutControl;
        window['CHorizontalLayoutControl'] = CHorizontalLayoutControl;
        window['CViewControl'] = CViewControl;
        window['CLayeredGroupControl'] = CLayeredGroupControl;
    })(mControl || (mControl = {}));
    return mControl;
});
