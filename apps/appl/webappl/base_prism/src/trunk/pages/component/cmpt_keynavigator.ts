// file name : pages/component/cKeyNavigator.js
/*jslint sloppy:true, nomen:true, vars:true*/
///<reference path="../../def/sugar.d.ts" />
import __COMPONENT_KERNEL__ = require("pages/component_kernel");
declare var hx: ifSugar;

class cKeynavigator extends __COMPONENT_KERNEL__ {
    _$: JQuery;
    _$focusable: any;
    _focusable: string;
    _focused: string;
    _keyTable: number[];
    _lastKey: number;
    _children: any[];
    _naviTable: number[];
    _numTable: number[];
    constructor($?, aFocusable?, aRocused?) {
        super();
        var key, keyCode;
        this._$ = $;
        this._$focusable = [];
        this._focusable = aFocusable;
        this._focused = aRocused;
        this._keyTable = [];
        this._lastKey = null;
        this._children = [];

        this._naviTable = [hx.key.KEY_UP, hx.key.KEY_DOWN, hx.key.KEY_LEFT, hx.key.KEY_RIGHT];

        this._numTable = [hx.key.KEY_0, hx.key.KEY_1, hx.key.KEY_2, hx.key.KEY_3, hx.key.KEY_4, hx.key.KEY_5, hx.key.KEY_6, hx.key.KEY_7, hx.key.KEY_8, hx.key.KEY_9];

        var num: number = 0;

        var bindKey = (obj, key) => {
            keyCode = hx.key[key];

            if (!obj[key]) {
                if (obj._naviTable.indexOf(keyCode) > -1) {
                    obj[key] = function(param) {
                        return this.doNavi(param);
                    };
                } else if (obj._numTable.indexOf(keyCode) > -1) {
                    obj._keyTable[keyCode] = this.KEY_NUM;
                } else {
                    obj[key] = function(param) {
                        return false;
                    };
                }
            }

            if (!obj._keyTable[keyCode]) {
                obj._keyTable[keyCode] = obj[key];
            }
        };

        for (key in hx.key) {
            if (typeof hx.key[key] === "number") {
                if (hx.key.hasOwnProperty(key)) {
                    bindKey(this, key);
                }
            }
        }
    }
    add(aChild) {
        $.extend(aChild, {
            sendEventToParent : (param) => {
                this._on_ChildNotify(param);
            }
        });
        this._children.push(aChild);
    }
    _on_ChildNotify(aParam) {
        // TODO implement
        throw 'child of cKeynavigator must implement this function';
    }
    remove(aChild) {
        var i, length;
        var children = this._children;

        for ( i = 0, length = children.length; i < length; i += 1) {

            if (aChild === children[i]) {
                this._children.splice(i, 1);
            }
        }
    }
    clear() {
        this._children = [];
    }
    destroy() {
        this._keyTable = [];
        this._naviTable = [];
        this._numTable = [];
    }
    get(aName?: string): any {
        var i, length;
        var children = this._children, child = null;

        for ( i = 0, length = children.length; i < length; i += 1) {
            if (aName === children[i].name) {
                child = children[i];
            }
        }

        return child;
    }
    build(aBuilder, aRolling?: boolean) {
        var focusable = this._focusable;

        this._$focusable = this._$.find('.' + focusable).filter((index) => {
            return $(this, this._$[0]).parentsUntil(this._$, '.' + focusable).length === 0;
        });

        aBuilder(this._$focusable, aRolling);
    }
    _on_alKey(aParam): boolean {
        return this.doKey(aParam);
    }
    KEY_NUM(aParam): boolean {
        return false;
    }
    hasClassForDom(aEl, aClss): boolean {
        return aEl.classList.contains(aClss);
        //return aEl.className && new RegExp("(^|\\s)" + aClss + "(\\s|$)").test(aEl.className);
    }
    doNavi(aParam): boolean {
        var i: number, item, next, $item: JQuery, $next: JQuery;
        var focused: string = this._focused;
        var $focusable: JQuery = this._$focusable;
        var length: number, child, children = this._children;

        this._lastKey = aParam.alKey;

        if ($focusable) {
            if (this._naviTable.indexOf(aParam.alKey) > -1) {
                for ( i = 0, length = $focusable.length; i < length; i += 1) {
                    item = $focusable[i];
                    if (this.hasClassForDom(item, focused)) {
                        next = item[aParam.alKey];

                        if (next || next === 0) {
                            $next = $($focusable[next]);
                            $item = $(item);
                            //$item.removeClass(focused);
                            //$next.addClass(focused);
                            item.classList.remove(focused);
                            $focusable[next].classList.add(focused);
                            this.onFocusChanged($item, $next);
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }
    doKey(aParam): boolean {
        var func, child, i: number, length: number;
        var handled: boolean = false;

        func = this._keyTable[aParam.alKey];

        for ( i = 0, length = this._children.length; i < length; i += 1) {
            child = this._children[i];

            if (child.$.hasClass(this._focused)) {
                handled = child.doKey.call(child, aParam);
            }
        }

        if (!handled) {
            if (func) {
                handled = func.call(this, aParam);
            }
        }

        return handled;
    }
    moveToEnd(aKey: number) {
        var focused: string = this._focused, current, $old: JQuery, $focused: JQuery, $focusable: JQuery;
        $focused = this.$.find('.' + focused);
        $focusable = this._$focusable;
        current = $focused ? $focused[0] : null;

        $old = $(current);

        var doKey = (key) => {
            if (current) {
                var next = current[key];
                if (next || next === 0) {
                    current = $focusable[next];
                    doKey(key);
                } else {
                    $focused.removeClass(focused);
                    $(current).addClass(focused);
                    this.onFocusChanged($old, $(current));
                }
            }
        }

        doKey(aKey);
    }
    getLastKey() {
        return this._lastKey;
    }
    initKey() {
        this._lastKey = null;
    }
    onFocusChanged($old, $new) {
    }
    leftRightBuilder($map: JQuery) {
        var i: number, length: number, left: number = hx.key.KEY_LEFT, right: number = hx.key.KEY_RIGHT;

        for ( i = 0, length = $map.length; i < length - 1; i += 1) {
            var current = $map[i];
            var next = $map[i + 1];
            current[right] = i + 1;
            next[left] = i;
        }
    }
    upDownBuilder($map: JQuery, rolling: boolean) {
        var i: number, length: number, up: number = hx.key.KEY_UP, down: number = hx.key.KEY_DOWN;

        for ( i = 0, length = $map.length; i < length - 1; i += 1) {
            var current = $map[i];
            var next = $map[i + 1];
            current[down] = i + 1;
            next[up] = i;
        }

        if (rolling) {
            $map[0][up] = $map.length - 1;
            $map[$map.length - 1][down] = 0;
        }
    }
    positionBuilder($map: JQuery, rolling: boolean) {
        var offset, left, right, top, bottom, i, j, d, o, t, x, y, minLeft, minRight, minTop, minBottom, minDistanceTop, minDistanceBottom, indexLeft, indexRight, indexTop, indexBottom, item, temp;
        var positions = [], MAX = 999999, length = $map.length;
        var height = 999999, height2 = 999999;
        var currStyle;

        var position = function($item) {
            offset = $item.getBoundingClientRect();
            left = offset.left;
            right = offset.right;
            top = offset.top;
            bottom = offset.bottom;
            x = left + (right - left) / 2;
            y = top + (bottom - top) / 2;

            return {
                top : top,
                bottom: bottom,
                left : left,
                right : right,
                x : x,
                y : y
            };
        };


        for ( i = 0; i < length; i += 1) {
            positions.push(position($map[i]));
        }

        for ( i = 0; i < length; i += 1) {
            o = positions[i];
            minLeft = MAX;
            minRight = MAX;
            minTop = MAX;
            minBottom = MAX;
            minDistanceTop = MAX;
            minDistanceBottom = MAX;
            item = $map[i];

            for ( j = 0; j < length; j += 1) {
                t = positions[j];

                // left
                if (o.y === t.y && o.x > t.x) {
                    temp = o.x - t.x;
                    if (minLeft > temp) {
                        minLeft = temp;
                        indexLeft = j;
                    }
                }

                // right
                if (o.y === t.y && o.x < t.x) {
                    temp = t.x - o.x;
                    if (minRight > temp) {
                        minRight = temp;
                        indexRight = j;
                    }
                }

                // top
                if (((o.left >= t.left && o.left < t.right) || (t.left >= o.left && t.left < o.right)) && o.y > t.y) {
                    temp = o.y - t.y;
                    if (minTop > temp) {
                        minTop = temp;
                        indexTop = j;
                    }
                }

                // bottom
                if (((o.left >= t.left && o.left < t.right) || (t.left >= o.left && t.left < o.right)) && o.y < t.y) {
                    temp = t.y - o.y;
                    if (minBottom > temp) {
                        minBottom = temp;
                        indexBottom = j;
                    }
                }

                temp = t.y - o.y;
                if (temp < height && temp > 0) {
                    height2 = height;
                    height = temp;
                }

            }

            if (height2 === MAX) {
                height2 = height;
            }

            item[hx.key.KEY_LEFT] = null;
            item[hx.key.KEY_RIGHT] = null;
            item[hx.key.KEY_UP] = null;
            item[hx.key.KEY_DOWN] = null;

            if (minLeft !== MAX) {
                item[hx.key.KEY_LEFT] = indexLeft;
            }

            if (minRight !== MAX) {
                item[hx.key.KEY_RIGHT] = indexRight;
            }

            if (minTop !== MAX && minTop < height2 * 2 - 1) {
                item[hx.key.KEY_UP] = indexTop;
            }

            if (minBottom !== MAX && minBottom < height2 * 2 - 1) {
                item[hx.key.KEY_DOWN] = indexBottom;
            }
        }
    }
    distanceBuilder($map: JQuery, rolling: boolean) {
        var offset, left, right, top, bottom, i, j, x, y, length, positions = [], MAX = 999999, d, o, t, minLeft, minRight, minTop, minBottom, indexLeft, indexRight, indexTop, indexBottom, item;

        var position = function($item) {
            offset = $item.offset();
            left = offset.left;
            right = left + $item.width();
            top = offset.top;
            bottom = top + $item.height();
            x = left + (right - left) / 2;
            y = top + (bottom - top) / 2;

            return {
                left : left,
                right : right,
                top : top,
                bottom : bottom,
                x : x,
                y : y
            };
        };

        var distance = function(p1, p2) {
            return Math.sqrt(Math.pow(p1.x - p2.x, 2) + Math.pow(p1.y - p2.y, 2));
        };

        for ( i = 0, length = $map.length; i < length; i += 1) {
            positions.push(position($($map[i])));
        }

        for ( i = 0; i < length; i += 1) {
            o = positions[i];
            minLeft = MAX;
            minRight = MAX;
            minTop = MAX;
            minBottom = MAX;
            item = $map[i];

            for ( j = 0; j < length; j += 1) {
                t = positions[j];
                d = distance(o, t);

                // left
                if (((o.y >= t.top && o.y <= t.bottom) || (t.y >= o.top && t.y <= o.bottom)) && o.x > t.x) {
                    if (minLeft > d) {
                        minLeft = d;
                        indexLeft = j;
                    }
                }

                // right
                if (((o.y >= t.top && o.y <= t.bottom) || (t.y >= o.top && t.y <= o.bottom)) && o.x < t.x) {
                    if (minRight > d) {
                        minRight = d;
                        indexRight = j;
                    }
                }

                // top
                if (((o.x >= t.left && o.x <= t.right) || (t.x >= o.left && t.x <= o.right)) && o.y > t.y) {
                    if (minTop > d) {
                        minTop = d;
                        indexTop = j;
                    }
                }

                // bottom
                if (((o.x >= t.left && o.x <= t.right) || (t.x >= o.left && t.x <= o.right)) && o.y < t.y) {
                    if (minBottom > d) {
                        minBottom = d;
                        indexBottom = j;
                    }
                }
            }

            item[hx.key.KEY_LEFT] = null;
            item[hx.key.KEY_RIGHT] = null;
            item[hx.key.KEY_UP] = null;
            item[hx.key.KEY_DOWN] = null;

            if (minLeft !== MAX) {
                item[hx.key.KEY_LEFT] = indexLeft;
            }

            if (minRight !== MAX) {
                item[hx.key.KEY_RIGHT] = indexRight;
            }

            if (minTop !== MAX) {
                item[hx.key.KEY_UP] = indexTop;
            }

            if (minBottom !== MAX) {
                item[hx.key.KEY_DOWN] = indexBottom;
            }
        }
    }
    doDraw(aParam1?: any, aParam2?: any, aParam3?: any) { // TODO Fix param
        throw "do implement"
    }
}
export = cKeynavigator;