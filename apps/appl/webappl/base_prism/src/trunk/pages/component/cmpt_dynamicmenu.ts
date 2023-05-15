// file name : pages/component/cDynamicMenu.js
/*jslint sloppy:true, nomen:true, vars:true*/
///<reference path="../../def/sugar.d.ts" />
declare var hx: ifSugar;
import __KEYNAVIGATOR__ = require("pages/component/cmpt_keynavigator");

export class dynamicMenu extends __KEYNAVIGATOR__ {
    source = [];
    items = [];
    currentIndex = 0;
    DISPLAY_COUNT = 8;
    POSITION_MOVE_LIST_COUNT = 6;
    BTProfile = {
        PROFILE_UNKNOWN : 0,
        PROFILE_A2DP : 262144,
        PROFILE_AVRCP : 524288,
        PROFILE_A2DP_AVRCP : 786432,
        PROFILE_HID : 1048576,
        PROFILE_ALL : 536870911,
        PROFILE_AUTO : 2147483648
    };
    BTDevClass = {
        HID_KEYBOARD : 64,
        HID_MOUSE : 128,
        HID_COMBO : 192
    };
    devStatus = ["STATUS_NotPaired", "STATUS_Connencted", "STATUS_Paired", "STATUS_Unknown", "STATUS_PinRequested", "STATUS_ProfileStarted"];
    $: JQuery;
    $ul: JQuery;
    type: any;
    constructor() {
        super($('<div id="listView" style="height: 700px; width: 773px; position: absolute; overflow-x: hidden; overflow-y: hidden">'), 'focusable', '-fd');
        this.$ = this._$;
        this.$ul = $('<div style="position: absolute; height: 700px; width: 773px;" class="-list dep1 wifi active -f -fd" data="0">');
        this.$.append(this.$ul);
    }
    create(aParam : any) : any {
        var i : number, length : number, $item : any, attr : any;
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
        for ( i = 0; i < length; i += 1) {
            if (this.type === 'bluetooth') {
                $item = $('<div class="-list-item -f noInd" data="0" style="position: absolute; padding: 0px 0px 0px 0px; width: 773px; top: 0px">');
            } else if (this.type === 'wireless') {
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
    }
    bind(aData : any) : void {
        this.source = aData;
        if (this.currentIndex === -1) {
            this.currentIndex = this.source.length - 1;
        }
    }
    getItem(aIndex : number) : any {
        if ((0 > aIndex) || (this.items.length < aIndex)) {
            return undefined;
        }
        return this.items[aIndex];
    }
    getfocusedItem() : any {
        if (this.type === 'wireless') {
            if (this.$ul.children('.-fd').length) {
                return this.$ul.children('.-fd')[0];
            }
        } else if (this.type === 'bluetooth') {
            if (this.$ul.children('.-fd').children('b').length) {
                return this.$ul.children('.-fd').children('b')[0];
            }
        }
        return false;
    }
    getConnectedItem() : any {
        if (this.$.children('div').children('div').children('b').children('.conn').length) {
            return this.$.children('div').children('div').children('b').children('.conn');
        }
        return false;
    }
    setItemFocus(aIndex : any) : boolean {
        if (this.items[aIndex]) {
            this.$ul.children().removeClass('-fd');
            this.items[aIndex].addClass('-fd');
            return true;
        }
        return false;
    }
    draw() : void {
        var i : number, item : any, $li : any;
        var $ul : any = this.$ul;
        var currentIndex : number = this.currentIndex;
        var length : number = this.DISPLAY_COUNT;
        var start : number = currentIndex;
        var end : number = start + length;
        var displayedData : any;
        var items : any = this.items;
        var bhasFocus : boolean = false;
        var bhidden : boolean = false;

        if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
            displayedData = this.source.slice(start, end);
            if (displayedData.length < length) {
                displayedData = displayedData.concat(this.source.slice(0, length - displayedData.length));
            }
        } else {
            displayedData = this.source.slice(0, length);
        }

        for ( i = 0; i < length; i += 1) {
            if (this.type === 'wireless') {
                if (displayedData[i]) {
                    item = displayedData[i].apname;

                    bhidden = false;
                    if (displayedData[i].hidden !== undefined)
                        bhidden = displayedData[i].hidden;

                    var strength : string = '', lock : string = '', conn : string = '';
                    strength = "signal int" + displayedData[i].strength;

                    if (displayedData[i].secure === "true") {
                        lock = 'lock';
                    }

                    if (displayedData[i].isConnected === 1) {//con success
                        conn = 'conn';
                    }

                    $li = this.items[i];
                    $li.removeClass('focusable');
                    $li.text('');
                    $li.attr('index', this.source.indexOf(displayedData[i]));
                    $li.css('top', (70 * i) + 'px');

                    if (bhidden) {
                        $li.addClass('noInd').append($('<b>', {
                            'class' : 'wifihidden'
                        }).append($('<em>', {
                            'class' : conn
                        }))).append($('<em>').append($('<span>', {
                            'class' : strength
                        })).append($('<span>', {
                            'class' : lock
                        })));
                    } else {
                        $li.addClass('noInd').append($('<b>', {
                            'html' : item
                        }).append($('<em>', {
                            'class' : conn
                        }))).append($('<em>').append($('<span>', {
                            'class' : strength
                        })).append($('<span>', {
                            'class' : lock
                        })));
                    }
                }
            } else if (this.type === 'bluetooth') {
                if (displayedData[i]) {
                    item = displayedData[i].devName;

                    var status : string = '', statusStyle : string = '', devNum : any;
                    statusStyle = "position: absolute; right: 20px;";
                    // show dev status
                    switch(this.devStatus[displayedData[i].devStatus]) {
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
                            'id' : devNum,
                            'style' : "margin: 0px 0px 0px 65px;",
                            'html' : item
                        }).append($('<em>', {
                            'class' : ''
                        }))).append($('<em>', {
                            'html' : status,
                            'style' : statusStyle
                        }));
                    } else if (displayedData[i].devProfile === this.BTProfile.PROFILE_HID) {
                        if (displayedData[i].devClass === this.BTDevClass.HID_KEYBOARD) {
                            $li.addClass('noInd icoStorage bt keyboard').append($('<b>', {
                                'id' : devNum,
                                'style' : "margin: 0px 0px 0px 65px;",
                                'html' : item
                            }).append($('<em>', {
                                'class' : ''
                            }))).append($('<em>', {
                                'html' : status,
                                'style' : statusStyle
                            }));
                        } else if (displayedData[i].devClass === this.BTDevClass.HID_MOUSE) {
                            $li.addClass('noInd icoStorage bt mouse').append($('<b>', {
                                'id' : devNum,
                                'style' : "margin: 0px 0px 0px 65px;",
                                'html' : item
                            }).append($('<em>', {
                                'class' : ''
                            }))).append($('<em>', {
                                'html' : status,
                                'style' : statusStyle
                            }));
                        }
                    } else {
                        $li.addClass('noInd').append($('<b>', {
                            'id' : devNum,
                            'style' : "margin: 0px 0px 0px 20px;",
                            'html' : item
                        }).append($('<em>', {
                            'class' : ''
                        }))).append($('<em>', {
                            'html' : status,
                            'style' : statusStyle
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
                } else {
                    $li.addClass('focusable');
                }
            } else {
                if (((i > this.POSITION_MOVE_LIST_COUNT - 1)) && this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                    $li.removeClass('focusable');
                } else {
                    $li.addClass('focusable');
                }
            }
        }

        if (!bhasFocus) {
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                items[0].addClass('-fd');
            } else {
                items[this.currentIndex].addClass('-fd');
            }
        }

        this.build(this.upDownBuilder);
    }
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
    KEY_UP(aParam : {alKey: number} ) : boolean {
        if (this.$.children('div').children('div').filter('.-fd').children('b').length) {
            if (parseInt(this.$.children('div').children('div').filter('.-fd').children('b')[0].id, 10) === 0) {
                this.$.children('div').children('div.-fd').removeClass('-fd');
                this.sendEventToParent({
                    alEvent : 'alScrollup',
                    alKey : aParam.alKey,
                    alData : {
                        me : this
                    }
                });
                return false;
            }
        }

        var handled : any = this.doNavi(aParam);
        var focusedElList : any = this.$.children('div').children('div').filter('.-fd');
        var focusIndex : number = this.$.children('div').children('div').index(focusedElList[0]);

        if (!handled || (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT && focusIndex === 0)) {
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                this.currentIndex -= 1;
                this.currentIndex = this.currentIndex < 0 ? this.source.length - 1 : this.currentIndex;
                this.draw();
            } else if (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT) {
                if (!handled) {//not handled in doNavi
                    this.currentIndex = this.POSITION_MOVE_LIST_COUNT - 1;
                    if (this.$.children('div').children('div.-fd').length) {
                        this.$.children('div').children('div.-fd').removeClass('-fd');
                    }
                } else if (handled) {//handled in doNavi
                    this.currentIndex = 0;
                    if (this.$.children('div').children('div.-fd').length) {
                        this.$.children('div').children('div.-fd').removeClass('-fd');
                    }
                }
                this.items[this.currentIndex].addClass('-fd');
                this.draw();
            } else {
                if (this.$.children('div').children('div.-fd').length) {
                    this.$.children('div').children('div.-fd').removeClass('-fd');
                }
                this.currentIndex = this.DISPLAY_COUNT - 1;
                this.items[this.currentIndex].addClass('-fd');
            }
        }
        return handled;
    }
    KEY_DOWN(aParam : {alKey: number}) : boolean {
        if (this.$.children('div').children('div').filter('.-fd').children('b').length) {
            if (parseInt(this.$.children('div').children('div').filter('.-fd').children('b')[0].id, 10) === this.source.length - 1) {
                this.$.children('div').children('div.-fd').removeClass('-fd');
                this.sendEventToParent({
                    alEvent : 'alScrolldown',
                    alKey : aParam.alKey,
                    alData : {
                        me : this
                    }
                });
                return false;
            }
        }

        var handled : any = this.doNavi(aParam);
        var focusedElList : any = this.$.children('div').children('div').filter('.-fd');
        var focusIndex : number = this.$.children('div').children('div').index(focusedElList[0]);

        if (!handled || (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT && focusIndex === this.POSITION_MOVE_LIST_COUNT - 1)) {
            if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
                this.currentIndex += 1;
                this.currentIndex = this.currentIndex > this.source.length - 1 ? 0 : this.currentIndex;
                this.draw();
            } else if (this.DISPLAY_COUNT === this.POSITION_MOVE_LIST_COUNT) {
                if (!handled) {//not handled in doNavi
                    this.currentIndex = 0;
                    if (this.$.children('div').children('div.-fd').length) {
                        this.$.children('div').children('div.-fd').removeClass('-fd');
                    }
                } else if (handled) {//handled in doNavi
                    this.currentIndex = this.POSITION_MOVE_LIST_COUNT - 1;
                    if (this.$.children('div').children('div.-fd').length) {
                        this.$.children('div').children('div.-fd').removeClass('-fd');
                    }
                }
                this.items[this.currentIndex].addClass('-fd');
                this.draw();
            } else {
                if (this.$.children('div').children('div.-fd').length) {
                    this.$.children('div').children('div.-fd').removeClass('-fd');
                }
                this.currentIndex = 0;
                this.items[this.currentIndex].addClass('-fd');
            }
        }
        return handled;
    }
    KEY_CHANNEL_UP(aParam : any) : boolean {
        var i : number;

        if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
            if (this.source.length <= (this.DISPLAY_COUNT - 2)) {
                this.currentIndex = 0;
            } else {
                for ( i = 0; i < (this.DISPLAY_COUNT - 2); i += 1) {
                    this.currentIndex -= 1;
                    this.currentIndex = this.currentIndex < 0 ? this.source.length - 1 : this.currentIndex;
                }
            }
            this.draw();
        }
        return true;
    }
    KEY_CHANNEL_DOWN(aParam : any) : boolean {
        var i : number;
        if (this.DISPLAY_COUNT > this.POSITION_MOVE_LIST_COUNT) {
            if (this.source.length <= (this.DISPLAY_COUNT - 2)) {
                this.currentIndex = this.source.length - 1;
            } else {
                for ( i = 0; i < (this.DISPLAY_COUNT - 2); i += 1) {
                    this.currentIndex += 1;
                    this.currentIndex = this.currentIndex > this.source.length - 1 ? 0 : this.currentIndex;
                }
            }
            this.draw();
        }
        return true;
    }
    KEY_OK(aParam : any) : boolean  {
        var getFirstItem : any, focusedElList : any = this.$.children('div').children('div').filter('.-fd'), focusIndex : number = -1;
        if (focusedElList.length !== 0) {
            focusIndex = this.$.children('div').children('div').index(focusedElList[0]);
        }
        //getFirstItem = this.$.children('div').children('div')[0].innerHTML; // it has an issue this special characters(@#$%%^..)
        // change to &amp; or something.
        getFirstItem = $(this.$.children('div').children('div')[0]).text();
        this.sendEventToParent({
            alEvent : 'alClicked',
            alData : {
                me : this,
                index : focusIndex,
                getFirstItem : getFirstItem
            }
        });
        return true;
    }
    KEY_BACK(aParam : {alKey: number} ) : boolean {
        this.sendEventToParent({
            alEvent : 'alKey',
            alKey : aParam.alKey,
            alData : {
                me : this
            }
        });
        return true;
    }
    KEY_LEFT(aParam : {alKey: number}) : boolean {
        this.sendEventToParent({
            alEvent : 'alScrollLeft',
            alKey : aParam.alKey,
            alData : {
                me : this
            }
        });
        return false;
    }
}
