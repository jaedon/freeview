///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cSimpleMenu.js
/*jslint sloppy:true, nomen:true, vars:true*/
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component/cmpt_keynavigator"], function (require, exports, __KEYNAVIGATOR__) {
    var simpleMenu = (function (_super) {
        __extends(simpleMenu, _super);
        function simpleMenu() {
            _super.call(this, $('<div>'), 'focusable', 'on');
            this.source = [];
            this.items = [];
            this.currentIndex = 0;
            this.DISPLAY_COUNT = 9;
            this.POSITION_MOVE_LIST_COUNT = 8;
            this.UPPER_TOP_PX = '-26px';
            this.DOWN_TOP_PX = '44px';
            this.DIM_POINT = 'dim30';
            this._itemCount = 0;
            this.$ = this._$;
            this.$ul = $('<ul>');
            this.$.append(this.$ul);
        }
        simpleMenu.prototype.create = function (param) {
            var i, length = this.DISPLAY_COUNT, $item, attr;
            this._isNotId = param._isNotId;
            for (attr in param.attr) {
                if (param.attr.hasOwnProperty(attr)) {
                    this.$.attr(attr, param.attr[attr]);
                }
            }
            if (param.displayCount) {
                if (param.displayCount < 9) {
                    this.DISPLAY_COUNT = param.displayCount;
                }
                this._itemCount = param.displayCount;
            }
            if (param.top) {
                this.DOWN_TOP_PX = param.top;
            }
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                this.$ul.css('top', this.UPPER_TOP_PX);
                this.currentIndex = param.currentindex - 1;
            }
            else {
                this.$ul.css('top', this.DOWN_TOP_PX);
                this.currentIndex = param.currentindex;
            }
            this.$.show();
            this.items = [];
            for (i = 0; i < length; i += 1) {
                $item = $('<li>');
                this.items.push($item);
                this.$ul.append($item);
            }
            /*
             this._keyTable = [];
             this._keyTable[hx.key.KEY_CHANNEL_DOWN] = true;
             this._keyTable[hx.key.KEY_CHANNEL_UP] = true;
             this._keyTable[hx.key.KEY_OK] = true;
             this._keyTable[hx.key.KEY_UP] = true;
             this._keyTable[hx.key.KEY_DOWN] = true;
             */
            return this;
        };
        simpleMenu.prototype.destroy = function () {
        };
        simpleMenu.prototype.bind = function (data) {
            this.source = data;
            if (this.currentIndex === -1) {
                this.currentIndex = this.source.length - 1;
            }
        };
        simpleMenu.prototype.getItem = function (index) {
            if ((0 > index) || (this.items.length < index)) {
                return undefined;
            }
            return this.items[index];
        };
        simpleMenu.prototype.setItemFocus = function (index) {
            if (this.items[index]) {
                this.$ul.children().removeClass('on');
                this.items[index].addClass('on');
                return true;
            }
            return false;
        };
        simpleMenu.prototype.draw = function () {
            var i, item, $li;
            var $ul = this.$ul;
            var currentIndex = this.currentIndex;
            var length = this.DISPLAY_COUNT;
            var start = currentIndex;
            var end = start + length;
            var displayedData;
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                displayedData = this.source.slice(start, end);
                if (displayedData.length < length) {
                    displayedData = displayedData.concat(this.source.slice(0, length - displayedData.length));
                }
            }
            else {
                displayedData = this.source.slice(0, length);
            }
            var items = this.items;
            var hasFocus = false;
            for (i = 0; i < length; i += 1) {
                item = displayedData[i];
                $li = this.items[i];
                $li.removeClass('focusable');
                $li.text('');
                $li.attr('index', this.source.indexOf(item));
                if ($li.hasClass('on')) {
                    hasFocus = true;
                }
                if (item) {
                    if (this._isNotId) {
                        $li.html(item);
                    }
                    else {
                        $li.html(hx.l(item));
                    }
                    if ((i === 0 || (i === length - 1)) && this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                        $li.addClass(this.DIM_POINT);
                    }
                    else {
                        $li.addClass('focusable');
                    }
                }
            }
            if (!hasFocus) {
                if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                    items[1].addClass('on');
                }
                else {
                    items[this.currentIndex].addClass('on');
                }
            }
            if (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT) {
                if (this.currentIndex === this.source.length - 1) {
                    $ul.css('top', this.UPPER_TOP_PX);
                    items[this.source.length - 1].removeClass(this.DIM_POINT);
                    items[0].addClass(this.DIM_POINT);
                }
                else {
                    $ul.css('top', this.DOWN_TOP_PX);
                    items[0].removeClass(this.DIM_POINT);
                    items[this.source.length - 1].addClass(this.DIM_POINT);
                }
            }
            this.build(this.upDownBuilder);
        };
        /*
         _on_alKey(param) {
         var this = this;
         var fx = this._keyTable[param.alKey];
         if (fx) {
         fx.call(this, param);
         if (param.alData === undefined) {
         param.alData = {};
         }
         param.alData.me = this;
         } else {
         this.sendEventToParent({
         alEvent : 'alKey',
         alKey : param.alKey,
         alData : {me : this}
         });
         return false;
         }
         }
         */
        simpleMenu.prototype.KEY_UP = function (param) {
            var handled = this.doNavi(param);
            var focusedElList = this.$.children('ul').children('li').filter('.on');
            var focusIndex = this.$.children('ul').children('li').index(focusedElList[0]);
            if (!handled || (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT && focusIndex === 0)) {
                if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                    this.currentIndex -= 1;
                    this.currentIndex = this.currentIndex < 0 ? this.source.length - 1 : this.currentIndex;
                    this.draw();
                }
                else if (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT) {
                    if (!handled) {
                        this.currentIndex = this.POSITION_MOVE_LIST_COUNT - 1;
                        this.items[0].removeClass('on');
                    }
                    else if (handled) {
                        this.currentIndex = 0;
                        this.items[1].removeClass('on');
                    }
                    this.items[this.currentIndex].addClass('on');
                    this.draw();
                }
                else {
                    this.items[0].removeClass('on');
                    this.currentIndex = this.DISPLAY_COUNT - 1;
                    this.items[this.currentIndex].addClass('on');
                }
            }
            return handled;
        };
        simpleMenu.prototype.KEY_DOWN = function (param) {
            var handled = this.doNavi(param);
            var focusedElList = this.$.children('ul').children('li').filter('.on');
            var focusIndex = this.$.children('ul').children('li').index(focusedElList[0]);
            if (!handled || (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT && focusIndex === this.POSITION_MOVE_LIST_COUNT - 1)) {
                if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                    this.currentIndex += 1;
                    this.currentIndex = this.currentIndex > this.source.length - 1 ? 0 : this.currentIndex;
                    this.draw();
                }
                else if (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT) {
                    if (!handled) {
                        this.currentIndex = 0;
                        this.items[this.DISPLAY_COUNT - 1].removeClass('on');
                    }
                    else if (handled) {
                        this.currentIndex = this.POSITION_MOVE_LIST_COUNT - 1;
                        this.items[6].removeClass('on');
                    }
                    this.items[this.currentIndex].addClass('on');
                    this.draw();
                }
                else {
                    this.items[this.DISPLAY_COUNT - 1].removeClass('on');
                    this.currentIndex = 0;
                    this.items[this.currentIndex].addClass('on');
                }
            }
            return handled;
        };
        simpleMenu.prototype.KEY_CHANNEL_UP = function (param) {
            var i;
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                if (this.source.length <= (this.DISPLAY_COUNT - 2)) {
                    this.currentIndex = 0;
                }
                else {
                    for (i = 0; i < (this.DISPLAY_COUNT - 2); i += 1) {
                        this.currentIndex -= 1;
                        this.currentIndex = this.currentIndex < 0 ? this.source.length - 1 : this.currentIndex;
                    }
                }
                this.draw();
            }
            return true;
        };
        simpleMenu.prototype.KEY_CHANNEL_DOWN = function (param) {
            var i;
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                if (this.source.length <= (this.DISPLAY_COUNT - 2)) {
                    this.currentIndex = this.source.length - 1;
                }
                else {
                    for (i = 0; i < (this.DISPLAY_COUNT - 2); i += 1) {
                        this.currentIndex += 1;
                        this.currentIndex = this.currentIndex > this.source.length - 1 ? 0 : this.currentIndex;
                    }
                }
                this.draw();
            }
            return true;
        };
        simpleMenu.prototype.KEY_OK = function (param) {
            var getFirstItem, focusedElList = this.$.children('ul').children('li').filter('.on'), focusIndex = -1;
            var o = this.$.find('.on');
            var result = o.html();
            var absoluteFocusedIndex;
            if (focusedElList.length !== 0) {
                focusIndex = this.$.children('ul').children('li').index(focusedElList[0]);
            }
            //getFirstItem = this.$.children('ul').children('li')[0].innerHTML; // it has an issue this special characters(@#$%%^..)
            // change to &amp; or something.
            getFirstItem = $(this.$.children('ul').children('li')[0]).text();
            absoluteFocusedIndex = focusIndex;
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                absoluteFocusedIndex += this.currentIndex;
                absoluteFocusedIndex %= this._itemCount;
            }
            this.sendEventToParent({
                alEvent: 'alClicked',
                alData: {
                    me: this,
                    index: focusIndex,
                    absoluteIndex: absoluteFocusedIndex,
                    getFirstItem: getFirstItem,
                    result: result
                }
            });
            return true;
        };
        simpleMenu.prototype.KEY_BACK = function (param) {
            this.sendEventToParent({
                alEvent: 'alKey',
                alKey: param.alKey,
                alData: {
                    me: this
                }
            });
            return false;
        };
        simpleMenu.prototype.KEY_LEFT = function (param) {
            this.sendEventToParent({
                alEvent: 'alScrollLeft',
                alKey: param.alKey,
                alData: {
                    me: this
                }
            });
            return false;
        };
        return simpleMenu;
    })(__KEYNAVIGATOR__);
    exports.simpleMenu = simpleMenu;
});
