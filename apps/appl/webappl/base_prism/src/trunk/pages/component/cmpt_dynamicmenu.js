var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component/cmpt_keynavigator"], function (require, exports, __KEYNAVIGATOR__) {
    ///<reference path="../../def/sugar.d.ts" />
    var dynamicMenu = (function (_super) {
        __extends(dynamicMenu, _super);
        function dynamicMenu() {
            _super.call(this, $('<div id="listView" style="height: 700px; width: 773px; position: absolute; overflow-x: hidden; overflow-y: hidden">'), 'focusable', '-fd');
            this.source = [];
            this.items = [];
            this.currentIndex = 0;
            this.DISPLAY_COUNT = 8;
            this.POSITION_MOVE_LIST_COUNT = 6;
            this.BTProfile = {
                PROFILE_UNKNOWN: 0,
                PROFILE_A2DP: 262144,
                PROFILE_AVRCP: 524288,
                PROFILE_A2DP_AVRCP: 786432,
                PROFILE_HID: 1048576,
                PROFILE_ALL: 536870911,
                PROFILE_AUTO: 2147483648
            };
            this.BTDevClass = {
                HID_KEYBOARD: 64,
                HID_MOUSE: 128,
                HID_COMBO: 192
            };
            this.devStatus = ["STATUS_NotPaired", "STATUS_Connencted", "STATUS_Paired", "STATUS_Unknown", "STATUS_PinRequested", "STATUS_ProfileStarted"];
            this.$ = this._$;
            this.$ul = $('<div style="position: absolute; height: 700px; width: 773px;" class="-list dep1 wifi active -f -fd" data="0">');
            this.$.append(this.$ul);
        }
        dynamicMenu.prototype.create = function (aParam) {
            var i, length, $item, attr;
            this.type = aParam.type;
            for (attr in aParam.attr) {
                if (aParam.attr.hasOwnProperty(attr)) {
                    this.$.attr(attr, aParam.attr[attr]);
                }
            }
            if (aParam.showCount) {
                this.DISPLAY_COUNT = aParam.showCount;
            }
            if (aParam.focusCount) {
                this.POSITION_MOVE_LIST_COUNT = aParam.focusCount;
            }
            if (aParam.displayCount && aParam.displayCount < this.DISPLAY_COUNT) {
                this.DISPLAY_COUNT = aParam.displayCount;
            }
            length = this.DISPLAY_COUNT;
            this.currentIndex = aParam.currentindex;
            this.$.show();
            this.items = [];
            for (i = 0; i < length; i += 1) {
                if (this.type === 'bluetooth') {
                    $item = $('<div class="-list-item -f noInd" data="0" style="position: absolute; padding: 0px 0px 0px 0px; width: 773px; top: 0px">');
                }
                else if (this.type === 'wireless') {
                    $item = $('<div class="-list-item -f noInd" data="0" style="position: absolute; width: 773px; top: 0px">');
                }
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
        dynamicMenu.prototype.bind = function (aData) {
            this.source = aData;
            if (this.currentIndex === -1) {
                this.currentIndex = this.source.length - 1;
            }
        };
        dynamicMenu.prototype.getItem = function (aIndex) {
            if ((0 > aIndex) || (this.items.length < aIndex)) {
                return undefined;
            }
            return this.items[aIndex];
        };
        dynamicMenu.prototype.getfocusedItem = function () {
            if (this.type === 'wireless') {
                if (this.$ul.children('.-fd').length) {
                    return this.$ul.children('.-fd')[0];
                }
            }
            else if (this.type === 'bluetooth') {
                if (this.$ul.children('.-fd').children('b').length) {
                    return this.$ul.children('.-fd').children('b')[0];
                }
            }
            return false;
        };
        dynamicMenu.prototype.getConnectedItem = function () {
            if (this.$.children('div').children('div').children('b').children('.conn').length) {
                return this.$.children('div').children('div').children('b').children('.conn');
            }
            return false;
        };
        dynamicMenu.prototype.setItemFocus = function (aIndex) {
            if (this.items[aIndex]) {
                this.$ul.children().removeClass('-fd');
                this.items[aIndex].addClass('-fd');
                return true;
            }
            return false;
        };
        dynamicMenu.prototype.draw = function () {
            var i, item, $li;
            var $ul = this.$ul;
            var currentIndex = this.currentIndex;
            var length = this.DISPLAY_COUNT;
            var start = currentIndex;
            var end = start + length;
            var displayedData;
            var items = this.items;
            var bhasFocus = false;
            var bhidden = false;
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                displayedData = this.source.slice(start, end);
                if (displayedData.length < length) {
                    displayedData = displayedData.concat(this.source.slice(0, length - displayedData.length));
                }
            }
            else {
                displayedData = this.source.slice(0, length);
            }
            for (i = 0; i < length; i += 1) {
                if (this.type === 'wireless') {
                    if (displayedData[i]) {
                        item = displayedData[i].apname;
                        bhidden = false;
                        if (displayedData[i].hidden !== undefined)
                            bhidden = displayedData[i].hidden;
                        var strength = '', lock = '', conn = '';
                        strength = "signal int" + displayedData[i].strength;
                        if (displayedData[i].secure === "true") {
                            lock = 'lock';
                        }
                        if (displayedData[i].isConnected === 1) {
                            conn = 'conn';
                        }
                        $li = this.items[i];
                        $li.removeClass('focusable');
                        $li.text('');
                        $li.attr('index', this.source.indexOf(displayedData[i]));
                        $li.css('top', (70 * i) + 'px');
                        if (bhidden) {
                            $li.addClass('noInd').append($('<b>', {
                                'class': 'wifihidden'
                            }).append($('<em>', {
                                'class': conn
                            }))).append($('<em>').append($('<span>', {
                                'class': strength
                            })).append($('<span>', {
                                'class': lock
                            })));
                        }
                        else {
                            $li.addClass('noInd').append($('<b>', {
                                'html': item
                            }).append($('<em>', {
                                'class': conn
                            }))).append($('<em>').append($('<span>', {
                                'class': strength
                            })).append($('<span>', {
                                'class': lock
                            })));
                        }
                    }
                }
                else if (this.type === 'bluetooth') {
                    if (displayedData[i]) {
                        item = displayedData[i].devName;
                        var status = '', statusStyle = '', devNum;
                        statusStyle = "position: absolute; right: 20px;";
                        switch (this.devStatus[displayedData[i].devStatus]) {
                            case "STATUS_NotPaired":
                            case "STATUS_Unknown":
                            case "STATUS_PinRequested":
                                //Pin is requested on Paring
                                status = hx.l('LOC_NOT_PAIRED_ID');
                                break;
                            case "STATUS_Paired":
                                status = hx.l('LOC_NOT_CONNECTED_ID');
                                break;
                            case "STATUS_Connencted":
                            case "STATUS_ProfileStarted":
                                status = hx.l('LOC_CONNECTED_ID');
                                statusStyle = "position: absolute; right: 20px; color: #ffff00;";
                                break;
                        }
                        devNum = displayedData[i].devNum;
                        $li = this.items[i];
                        $li.removeClass('focusable');
                        $li.text('');
                        $li.attr('index', this.source.indexOf(displayedData[i]));
                        $li.css('top', (70 * i) + 'px');
                        if (displayedData[i].devProfile === this.BTProfile.PROFILE_A2DP || displayedData[i].devProfile === this.BTProfile.PROFILE_A2DP_AVRCP) {
                            $li.addClass('noInd icoStorage bt head').append($('<b>', {
                                'id': devNum,
                                'style': "margin: 0px 0px 0px 65px;",
                                'html': item
                            }).append($('<em>', {
                                'class': ''
                            }))).append($('<em>', {
                                'html': status,
                                'style': statusStyle
                            }));
                        }
                        else if (displayedData[i].devProfile === this.BTProfile.PROFILE_HID) {
                            if (displayedData[i].devClass === this.BTDevClass.HID_KEYBOARD) {
                                $li.addClass('noInd icoStorage bt keyboard').append($('<b>', {
                                    'id': devNum,
                                    'style': "margin: 0px 0px 0px 65px;",
                                    'html': item
                                }).append($('<em>', {
                                    'class': ''
                                }))).append($('<em>', {
                                    'html': status,
                                    'style': statusStyle
                                }));
                            }
                            else if (displayedData[i].devClass === this.BTDevClass.HID_MOUSE) {
                                $li.addClass('noInd icoStorage bt mouse').append($('<b>', {
                                    'id': devNum,
                                    'style': "margin: 0px 0px 0px 65px;",
                                    'html': item
                                }).append($('<em>', {
                                    'class': ''
                                }))).append($('<em>', {
                                    'html': status,
                                    'style': statusStyle
                                }));
                            }
                        }
                        else {
                            $li.addClass('noInd').append($('<b>', {
                                'id': devNum,
                                'style': "margin: 0px 0px 0px 20px;",
                                'html': item
                            }).append($('<em>', {
                                'class': ''
                            }))).append($('<em>', {
                                'html': status,
                                'style': statusStyle
                            }));
                        }
                    }
                }
                if ($li.hasClass('-fd')) {
                    bhasFocus = true;
                }
                if (this.DISPLAY_COUNT >= this.POSITION_MOVE_LIST_COUNT + 2) {
                    if (((i === this.DISPLAY_COUNT - 2) || (i === this.DISPLAY_COUNT - 1)) && this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                        $li.removeClass('focusable');
                    }
                    else {
                        $li.addClass('focusable');
                    }
                }
                else {
                    if (((i > this.POSITION_MOVE_LIST_COUNT - 1)) && this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                        $li.removeClass('focusable');
                    }
                    else {
                        $li.addClass('focusable');
                    }
                }
            }
            if (!bhasFocus) {
                if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                    items[0].addClass('-fd');
                }
                else {
                    items[this.currentIndex].addClass('-fd');
                }
            }
            this.build(this.upDownBuilder);
        };
        /*
         _on_alKey : function(param) {
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
         },
         */
        dynamicMenu.prototype.KEY_UP = function (aParam) {
            if (this.$.children('div').children('div').filter('.-fd').children('b').length) {
                if (parseInt(this.$.children('div').children('div').filter('.-fd').children('b')[0].id, 10) === 0) {
                    this.$.children('div').children('div.-fd').removeClass('-fd');
                    this.sendEventToParent({
                        alEvent: 'alScrollup',
                        alKey: aParam.alKey,
                        alData: {
                            me: this
                        }
                    });
                    return false;
                }
            }
            var handled = this.doNavi(aParam);
            var focusedElList = this.$.children('div').children('div').filter('.-fd');
            var focusIndex = this.$.children('div').children('div').index(focusedElList[0]);
            if (!handled || (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT && focusIndex === 0)) {
                if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                    this.currentIndex -= 1;
                    this.currentIndex = this.currentIndex < 0 ? this.source.length - 1 : this.currentIndex;
                    this.draw();
                }
                else if (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT) {
                    if (!handled) {
                        this.currentIndex = this.POSITION_MOVE_LIST_COUNT - 1;
                        if (this.$.children('div').children('div.-fd').length) {
                            this.$.children('div').children('div.-fd').removeClass('-fd');
                        }
                    }
                    else if (handled) {
                        this.currentIndex = 0;
                        if (this.$.children('div').children('div.-fd').length) {
                            this.$.children('div').children('div.-fd').removeClass('-fd');
                        }
                    }
                    this.items[this.currentIndex].addClass('-fd');
                    this.draw();
                }
                else {
                    if (this.$.children('div').children('div.-fd').length) {
                        this.$.children('div').children('div.-fd').removeClass('-fd');
                    }
                    this.currentIndex = this.DISPLAY_COUNT - 1;
                    this.items[this.currentIndex].addClass('-fd');
                }
            }
            return handled;
        };
        dynamicMenu.prototype.KEY_DOWN = function (aParam) {
            if (this.$.children('div').children('div').filter('.-fd').children('b').length) {
                if (parseInt(this.$.children('div').children('div').filter('.-fd').children('b')[0].id, 10) === this.source.length - 1) {
                    this.$.children('div').children('div.-fd').removeClass('-fd');
                    this.sendEventToParent({
                        alEvent: 'alScrolldown',
                        alKey: aParam.alKey,
                        alData: {
                            me: this
                        }
                    });
                    return false;
                }
            }
            var handled = this.doNavi(aParam);
            var focusedElList = this.$.children('div').children('div').filter('.-fd');
            var focusIndex = this.$.children('div').children('div').index(focusedElList[0]);
            if (!handled || (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT && focusIndex === this.POSITION_MOVE_LIST_COUNT - 1)) {
                if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                    this.currentIndex += 1;
                    this.currentIndex = this.currentIndex > this.source.length - 1 ? 0 : this.currentIndex;
                    this.draw();
                }
                else if (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT) {
                    if (!handled) {
                        this.currentIndex = 0;
                        if (this.$.children('div').children('div.-fd').length) {
                            this.$.children('div').children('div.-fd').removeClass('-fd');
                        }
                    }
                    else if (handled) {
                        this.currentIndex = this.POSITION_MOVE_LIST_COUNT - 1;
                        if (this.$.children('div').children('div.-fd').length) {
                            this.$.children('div').children('div.-fd').removeClass('-fd');
                        }
                    }
                    this.items[this.currentIndex].addClass('-fd');
                    this.draw();
                }
                else {
                    if (this.$.children('div').children('div.-fd').length) {
                        this.$.children('div').children('div.-fd').removeClass('-fd');
                    }
                    this.currentIndex = 0;
                    this.items[this.currentIndex].addClass('-fd');
                }
            }
            return handled;
        };
        dynamicMenu.prototype.KEY_CHANNEL_UP = function (aParam) {
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
        dynamicMenu.prototype.KEY_CHANNEL_DOWN = function (aParam) {
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
        dynamicMenu.prototype.KEY_OK = function (aParam) {
            var getFirstItem, focusedElList = this.$.children('div').children('div').filter('.-fd'), focusIndex = -1;
            if (focusedElList.length !== 0) {
                focusIndex = this.$.children('div').children('div').index(focusedElList[0]);
            }
            //getFirstItem = this.$.children('div').children('div')[0].innerHTML; // it has an issue this special characters(@#$%%^..)
            // change to &amp; or something.
            getFirstItem = $(this.$.children('div').children('div')[0]).text();
            this.sendEventToParent({
                alEvent: 'alClicked',
                alData: {
                    me: this,
                    index: focusIndex,
                    getFirstItem: getFirstItem
                }
            });
            return true;
        };
        dynamicMenu.prototype.KEY_BACK = function (aParam) {
            this.sendEventToParent({
                alEvent: 'alKey',
                alKey: aParam.alKey,
                alData: {
                    me: this
                }
            });
            return true;
        };
        dynamicMenu.prototype.KEY_LEFT = function (aParam) {
            this.sendEventToParent({
                alEvent: 'alScrollLeft',
                alKey: aParam.alKey,
                alData: {
                    me: this
                }
            });
            return false;
        };
        return dynamicMenu;
    })(__KEYNAVIGATOR__);
    exports.dynamicMenu = dynamicMenu;
});
