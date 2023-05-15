var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    ///<reference path="../../def/sugar.d.ts" />
    var table = (function (_super) {
        __extends(table, _super);
        function table() {
            _super.call(this);
        }
        table.prototype.create = function (aParam) {
            var i;
            $.extend(this, aParam);
            $.extend(this, {
                _$item: $('<dt>'),
                _$describe: $('<dd>'),
                _$describe2: $('<dd>'),
                _$dtItems: [],
                _$desItems: [],
                _$desItems2: [],
                _$scroll: $('<div>', {
                    'style': 'top:0px;height:0%',
                    'display': 'none'
                }),
                $: $('<dl>', this.attr)
            });
            this.count = this.items.length;
            for (i = 0; i < this.count; i += 1) {
                var item = this._$item.clone(), desc = this._$describe.clone(), itemStr = '', descStr = '', descStr2 = '';
                if (this.describe2) {
                    var desc2 = this._$describe2.clone();
                }
                if (this.langID === true) {
                    if (this.items) {
                        if (this.items[i]) {
                            if (this.items[i] !== "") {
                                itemStr = hx.l(this.items[i]);
                            }
                        }
                    }
                    if (this.describe) {
                        if (this.describe[i]) {
                            if (this.describe[i] !== "") {
                                descStr = hx.l(this.describe[i]);
                            }
                        }
                    }
                    if (this.describe2) {
                        if (this.describe2[i]) {
                            if (this.describe2[i] !== "") {
                                descStr2 = hx.l(this.describe2[i]);
                            }
                        }
                    }
                    if (this.describe2) {
                        this.$.append(item.html(itemStr)).append(desc.html(descStr)).append(desc2.html(descStr2));
                    }
                    else {
                        this.$.append(item.html(itemStr)).append(desc.html(descStr));
                    }
                }
                else {
                    if (this.describe2) {
                        this.$.append(item.html(this.items[i])).append(desc.html(this.describe[i])).append(desc2.html(this.describe2[i]));
                    }
                    else {
                        this.$.append(item.html(this.items[i])).append(desc.html(this.describe[i]));
                    }
                }
                this._$dtItems.push(item);
                this._$desItems.push(desc);
                if (this.describe2) {
                    this._$desItems2.push(desc2);
                }
            }
            return this;
        };
        table.prototype.destroy = function () {
            this.$.remove();
        };
        table.prototype.getDtItem = function (aIndex) {
            if (this._$dtItems.length < aIndex) {
                return undefined;
            }
            if (0 > aIndex) {
                return undefined;
            }
            return this._$dtItems[aIndex];
        };
        table.prototype.getItem = function (aIindex) {
            if (this._$desItems.length < aIindex) {
                return undefined;
            }
            if (0 > aIindex) {
                return undefined;
            }
            return this._$desItems[aIindex];
        };
        table.prototype.getItem2 = function (aIndex) {
            if (this._$desItems2.length < aIndex) {
                return undefined;
            }
            if (0 > aIndex) {
                return undefined;
            }
            return this._$desItems2[aIndex];
        };
        return table;
    })(__COMPONENT_KERNEL__);
    exports.table = table;
    var settingsList = (function (_super) {
        __extends(settingsList, _super);
        function settingsList() {
            _super.call(this);
            this._prismDialog = null; // to process key on Prism Dialog (eg, storage selector )
        }
        settingsList.prototype.create = function (aParam) {
            var _this = this;
            var i;
            $.extend(this, aParam);
            $.extend(this, {
                _$src: $('<li>', {
                    'class': 'focusable'
                }),
                _$items: [],
                $: $('<ul>', this.attr)
            });
            for (i = 0; i < this.count; i += 1) {
                var o = this._$src.clone();
                this._$items.push(o);
                this.$.append(o);
                this.fx(i, o);
            }
            this._keyTable = [];
            var n;
            for (n in aParam.keyTable) {
                if (aParam.keyTable.hasOwnProperty(n)) {
                    if (n !== undefined) {
                        var keyCode = aParam.keyTable[n];
                        switch (keyCode) {
                            case hx.key.KEY_UP:
                                this._keyTable[keyCode] = this._doUp;
                                break;
                            case hx.key.KEY_DOWN:
                                this._keyTable[keyCode] = this._doDown;
                                break;
                            case hx.key.KEY_OK:
                            case hx.key.KEY_RIGHT:
                                this._keyTable[keyCode] = this._doOk;
                                break;
                            case hx.key.KEY_LEFT:
                                this._keyTable[hx.key.KEY_LEFT] = this._doLeft;
                                break;
                            case hx.key.KEY_BACK:
                            case hx.key.KEY_MENU:
                                this._keyTable[keyCode] = this._doExit;
                                break;
                        }
                    }
                }
            }
            this.$.focus(function () {
                _this._doFocus();
            });
            return this;
        };
        /*
         hover : function() {
         if(this.$.children('.on').hasClass('his')) {
         this.$.children('.on').removeClass('his');
         } else {
         this.$.children('.on').addClass('his');
         }
    
         this.sendEventToParent({
         alEvent : 'alhover',
         alData : {
         me : this
         }
         });
         },
         */
        settingsList.prototype.destroy = function () {
            //this.$.remove();
        };
        settingsList.prototype.addItem = function (aParam) {
            var o = this._$src.clone();
            this._$items.push(o);
            this.$.append(o);
            o.html(aParam).addClass('noInd').append($('<em>')).append($('<span>').addClass('signal')).append($('<span>'));
            this.count += 1;
        };
        settingsList.prototype.removeAllItem = function () {
            var i;
            for (i = 0; i < this.count; i += 1) {
                this.getItem(i).remove();
            }
            for (i = 0; i < this.count; i += 1) {
                this._$items.pop();
            }
            this.count = 0;
        };
        settingsList.prototype.getListLength = function () {
            return this._$items.length;
        };
        settingsList.prototype.getItem = function (aIndex) {
            if (this._$items.length < aIndex) {
                return undefined;
            }
            if (0 > aIndex) {
                return undefined;
            }
            return this._$items[aIndex];
        };
        settingsList.prototype.setItemFocus = function (aIndex) {
            var _this = this;
            if (this._$items.length < aIndex) {
                return;
            }
            if (0 > aIndex) {
                return;
            }
            this.$.children().removeClass('on');
            if (this._$items[aIndex]) {
                this._$items[aIndex].addClass('on');
            }
            else {
                setTimeout(function () {
                    _this._$items[0].addClass('on');
                }, 1);
            }
        };
        settingsList.prototype.getFocusedItem = function () {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            return this._$items[i];
        };
        settingsList.prototype.setItemDisable = function (index, notvisible) {
            if (this._$items.length < index) {
                return;
            }
            if (0 > index) {
                return;
            }
            this._$items[index].addClass('dis');
            if (notvisible) {
                this._$items[index][0].style.display = 'none';
            }
        };
        settingsList.prototype.setItemEnable = function (aIndex) {
            if (this._$items.length < aIndex) {
                return;
            }
            if (0 > aIndex) {
                return;
            }
            this._$items[aIndex].removeClass('dis');
            this._$items[aIndex][0].style.display = 'block';
        };
        settingsList.prototype.setItemHistory = function (aIndex) {
            if (this._$items.length < aIndex) {
                return;
            }
            if (0 > aIndex) {
                return;
            }
            this._$items[aIndex].addClass('his');
        };
        settingsList.prototype.unsetItemHistory = function (aIndex) {
            if (this._$items.length < aIndex) {
                return;
            }
            if (0 > aIndex) {
                return;
            }
            this._$items[aIndex].removeClass('his');
        };
        settingsList.prototype._doFocus = function () {
            if (this.$.children('.on').length < 1) {
                if (this._$items.length < 1) {
                    hx.log("error", "Must to have one or more items");
                    return;
                }
                this._$items[0].addClass('on');
            }
        };
        settingsList.prototype.registerPrismDialog = function (hDlg) {
            this._prismDialog = hDlg;
        };
        settingsList.prototype.unregisterPrismDialog = function () {
            this._prismDialog = null;
        };
        settingsList.prototype._processKeyPrismDialog = function (alKey) {
            var consume = false, k = hx.key;
            if (this._prismDialog && (typeof this._prismDialog.doKey === 'function')) {
                switch (alKey) {
                    case k.KEY_POWER:
                    case k.KEY_ESCAPE:
                        this._prismDialog.close();
                        consume = false;
                        break;
                    case k.KEY_BACK:
                        this._prismDialog.close();
                        consume = true;
                        break;
                    default:
                        this._prismDialog.doKey(alKey);
                        consume = true;
                }
            }
            return consume;
        };
        /* KEY Event */
        settingsList.prototype._on_alKey = function (aParam) {
            var bConsumed = false;
            var fx = this._keyTable[aParam.alKey];
            bConsumed = this._processKeyPrismDialog(aParam.alKey);
            if (bConsumed) {
                return true;
            }
            if (fx) {
                fx.call(this, aParam);
                if (aParam.alData === undefined) {
                    aParam.alData = {};
                }
                aParam.alData.me = this;
                this.sendEventToParent(aParam);
                bConsumed = true;
            }
            return bConsumed;
        };
        settingsList.prototype._doUp = function (aParam) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length, j;
            if (this.editChList === true || this.editMailList === true || this.editsoftcellList === true) {
                aParam.alEvent = 'alScrollup';
                aParam.alData = {
                    index: i
                };
                if (i === 0) {
                    return false;
                }
                this.$.children().removeClass('on');
                if (this.$[0].querySelector('.mov')) {
                    this.$.children().eq(i - 1).addClass('mov');
                }
                this.$.children().eq(i -= 1).addClass('on');
                return false;
            }
            if (this.roll !== 'notroll' && this.roll !== 'notrollWithUp') {
                var tempCount = 0;
                for (j = 0; j < this.count; j += 1) {
                    if (!this.$.children().eq(j).hasClass('dis') && !this.$.children().eq(j).hasClass('blank')) {
                        tempCount += 1;
                    }
                }
                if (tempCount >= 2) {
                    this.$.children().removeClass('on');
                }
                else {
                    return false;
                }
            }
            else if (this.roll === 'notroll') {
                this.$.children().removeClass('on');
            }
            if (i === 0) {
                if (this.roll === 'notroll' || this.roll === 'notrollWithUp') {
                    aParam.alEvent = 'alScrollup';
                    return false;
                }
                i = this.count;
            }
            for (i; i >= 0; i -= 1) {
                if (this.$.children().eq(i - 1).hasClass('dis') || this.$.children().eq(i - 1).hasClass('blank')) {
                    if (i === 0) {
                        if (this.roll === 'notroll') {
                            aParam.alEvent = 'alScrollup';
                            return true;
                        }
                        j = 0;
                        for (j = (this.count - 1); j >= 0; j -= 1) {
                            if (this.$.children().eq(j).hasClass('dis') || this.$.children().eq(j).hasClass('blank')) {
                                continue;
                            }
                            else {
                                break;
                            }
                        }
                        i = j + 1;
                        break;
                    }
                }
                else {
                    break;
                }
            }
            this.$.children().eq(i -= 1).addClass('on');
            aParam.alEvent = 'alChangeFocus';
            aParam.alData = {
                index: i
            };
        };
        settingsList.prototype._doDown = function (aParam) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length, j = 0;
            var cnt = this.$.children('.focusable').length - 1;
            var tempCount = 0;
            if (this.editChList === true || this.editMailList === true || this.editsoftcellList === true) {
                aParam.alEvent = 'alScrolldown';
                aParam.alData = {
                    index: i
                };
                if (i >= cnt) {
                    return false;
                }
                this.$.children().removeClass('on');
                if (this.$[0].querySelector('.mov')) {
                    this.$.children().eq(i + 1).addClass('mov');
                }
                this.$.children().eq(i += 1).addClass('on');
                return false;
            }
            if (this.roll !== 'notroll' && this.roll !== 'notrollWithUp') {
                for (j = 0; j < this.count; j += 1) {
                    if (!this.$.children().eq(j).hasClass('dis') && !this.$.children().eq(j).hasClass('blank')) {
                        tempCount += 1;
                    }
                }
                if (this.count === 1 || tempCount >= 2) {
                    this.$.children().removeClass('on');
                }
                else {
                    return false;
                }
            }
            else if (this.roll === 'notroll' || this.roll === 'notrollWithUp') {
                this.$.children().removeClass('on');
            }
            if (i >= cnt) {
                if (this.roll === 'notroll' || this.roll === 'notrollWithUp') {
                    aParam.alEvent = 'alScrolldown';
                    return false;
                }
                i = -1;
            }
            for (i; i < this.count; i += 1) {
                if (this.$.children().eq(i + 1).hasClass('dis') || this.$.children().eq(i + 1).hasClass('blank')) {
                    if ((i + 1) >= cnt) {
                        if (this.roll === 'notroll' || this.roll === 'notrollWithUp') {
                            aParam.alEvent = 'alScrolldown';
                            aParam.alData = {
                                index: i
                            };
                            return true;
                        }
                        j = 0;
                        for (j = 0; j < this.count; j += 1) {
                            if (this.$.children().eq(j).hasClass('dis') || this.$.children().eq(j).hasClass('blank')) {
                                continue;
                            }
                            else {
                                break;
                            }
                        }
                        i = (j - 1);
                        break;
                    }
                }
                else {
                    break;
                }
            }
            this.$.children().eq(i += 1).addClass('on');
            aParam.alEvent = 'alChangeFocus';
            aParam.alData = {
                index: i
            };
        };
        settingsList.prototype._doLeft = function (aParam) {
            var o = this.$.children(".on");
            var i = o.prevAll('.focusable').length;
            aParam.alEvent = 'alScrollLeft';
            aParam.alData = {
                index: i
            };
            return true;
        };
        settingsList.prototype._doOk = function (aPparam) {
            var o = this.$.children(".on");
            this.$.children().removeClass('on');
            var i = o.prevAll('.focusable').length;
            aPparam.alEvent = 'alClicked';
            aPparam.alData = {
                index: i,
                page: (this.pages === undefined) ? '' : this.pages[i]
            };
        };
        settingsList.prototype._doExit = function (aParam) {
            var o = this.$.children(".on");
            //this.$.children().removeClass('on');
            var i = o.prevAll('.focusable').length;
            aParam.alEvent = 'alExit';
            aParam.alData = {
                index: i
            };
        };
        return settingsList;
    })(__COMPONENT_KERNEL__);
    exports.settingsList = settingsList;
    var menuList = (function (_super) {
        __extends(menuList, _super);
        function menuList() {
            _super.call(this);
        }
        menuList.prototype.create = function (aParam) {
            var _this = this;
            var n;
            $.extend(this, aParam, {
                $: $('<div>'),
                _$ul: $('<ul class="menu">'),
                _$li: $('<li class="">'),
                _keyTable: [],
                _menu_arr: [],
                _menu_parent_name: '',
                _menu_tot_arr: [],
                _level: 0,
                _level_tmp: 0,
                _viewCount: 9,
                _view_data: []
            });
            this.$.append(this._$ul);
            for (n in aParam.keyTable) {
                if (aParam.keyTable.hasOwnProperty(n)) {
                    if (n !== undefined) {
                        var keyCode = aParam.keyTable[n];
                        switch (keyCode) {
                            case hx.key.KEY_BACK:
                                this._keyTable[keyCode] = this._doBack;
                                break;
                            case hx.key.KEY_DOWN:
                                this._keyTable[keyCode] = this._doDown;
                                break;
                            case hx.key.KEY_UP:
                                this._keyTable[keyCode] = this._doUp;
                                break;
                            case hx.key.KEY_OK:
                                this._keyTable[keyCode] = this._doOk;
                                break;
                        }
                    }
                }
            }
            this.$.focus(function () {
                _this.setFocus(0);
            });
            return this;
        };
        menuList.prototype.getTitle = function (aTitle, aClass, aId) {
            var tmp = $('<p>');
            tmp.html(aTitle);
            if (aId) {
                tmp.attr('id', aId);
            }
            if (aClass) {
                tmp.attr('class', aClass);
            }
            return tmp;
        };
        menuList.prototype._on_alKey = function (aParam) {
            var fx = this._keyTable[aParam.alKey];
            if (fx) {
                fx.call(this, aParam);
            }
            return true;
        };
        menuList.prototype.setList = function (aArr) {
            var i, _viewCount = this._viewCount, _li;
            this._$ul.children().remove();
            this._view_data = [];
            if (aArr.length <= _viewCount) {
                for (i = 0; i < aArr.length; i += 1) {
                    _li = this._$li.clone();
                    this._$ul.append(_li.text(aArr[i].name));
                    this._view_data.push(aArr[i]);
                }
                this.$.attr('class', '');
                this._$ul.attr('class', 'menu');
            }
            else {
                for (i = 0; i <= _viewCount; i += 1) {
                    _li = this._$li.clone();
                    this._$ul.append(_li.text(aArr[i].name));
                    this._view_data.push(aArr[i]);
                }
                this.$.attr('class', 'menu rol');
                this._$ul.attr('class', '');
                this._$ul.attr('style', 'margin-top:30px;');
            }
            this._menu_arr = aArr;
            if (this._level === 0) {
                this._menu_tot_arr = aArr;
            }
            else {
                this.setFocus(0);
            }
        };
        menuList.prototype.setFocus = function (aIdx) {
            this._$ul.children('li').removeClass('on');
            this._$ul.children('li').eq(aIdx).addClass('on');
        };
        menuList.prototype.getFocus = function () {
            var result = this.$.find('.on').prevAll('.focusable').length;
            if (result === undefined) {
                result = 0;
            }
            return result;
        };
        menuList.prototype._doBack = function (aParam) {
            if (this._level === 0) {
                this._$ul.children().remove();
                if (aParam.alData === undefined) {
                    aParam.alData = {};
                }
                aParam.alEvent = 'alClicked';
                aParam.alData.me = this;
                aParam.alData.status = 'back';
                this.sendEventToParent(aParam);
            }
            else {
                this._go_back_menu(this._menu_tot_arr, this._menu_arr);
            }
        };
        menuList.prototype._go_back_menu = function (aMenu_tot_arr, aMenu_arr) {
            var child, item;
            this._level_tmp += 1;
            for (item in aMenu_tot_arr) {
                child = aMenu_tot_arr[item];
                if (this._level_tmp < this._level) {
                    this._go_back_menu(child.sub_menu, aMenu_arr);
                }
                else {
                    if (child.name === this._menu_parent_name && child.sub_menu === aMenu_arr) {
                        this.setList(aMenu_tot_arr);
                        this._level = this._level - 1;
                        break;
                    }
                }
            }
        };
        menuList.prototype._doDown = function (aParam) {
            var i;
            var _len = this._menu_arr.length;
            var _cnt = this.getFocus();
            var _viewCount = this._viewCount;
            var tmp, _target_index, _li, _next;
            if (_len <= _viewCount) {
                _cnt = _cnt + 1;
                if (_cnt >= _len) {
                    _cnt = 0;
                }
                this.setFocus(_cnt);
            }
            else {
                _cnt = _cnt + 1;
                if (_cnt < _viewCount) {
                    this.setFocus(_cnt);
                }
                else {
                    if (this._$ul.css('margin-top') === '30px') {
                        _target_index = this._menu_arr.indexOf(this._view_data[this._viewCount - 1]);
                        _next = this._menu_arr[_target_index + 1];
                        this._$ul.attr('style', 'margin-top:-20px;');
                        this.setFocus(_cnt);
                        _next = this._menu_arr[_target_index + 2];
                        if (_next !== undefined) {
                            _li = this._$li.clone();
                            this._$ul.append(_li.text(_next.name));
                            this._view_data.push(_next);
                        }
                    }
                    else {
                        _target_index = this._menu_arr.indexOf(this._view_data[this._viewCount]);
                        _next = this._menu_arr[_target_index + 1];
                        if (_next === undefined) {
                            this._view_data = [];
                            this.$.find('li').remove();
                            for (i = 0; i <= _viewCount; i += 1) {
                                tmp = this._menu_arr[i];
                                _li = this._$li.clone();
                                this._$ul.append(_li.text(tmp.name));
                                this._view_data.push(tmp);
                            }
                            this._$ul.attr('style', 'margin-top:30px;');
                            this.setFocus(0);
                        }
                        else {
                            this.setFocus(_cnt);
                            this.$.find('li').eq(0).remove();
                            this._view_data.splice(0, 1);
                            _next = this._menu_arr[_target_index + 2];
                            if (_next !== undefined) {
                                _li = this._$li.clone();
                                this._$ul.append(_li.text(_next.name));
                                this._view_data.push(_next);
                            }
                        }
                    }
                }
            }
        };
        menuList.prototype._doUp = function () {
            var i;
            var _len = this._menu_arr.length;
            var _cnt = this.getFocus();
            var _viewCount = this._viewCount;
            var tmp, _target_index, _li, _next;
            if (_len <= _viewCount) {
                _cnt = _cnt - 1;
                if (_cnt < 0) {
                    _cnt = (_len - 1);
                }
                this.setFocus(_cnt);
            }
            else {
                _cnt = _cnt - 1;
                if (_cnt > 0) {
                    this.setFocus(_cnt);
                }
                else {
                    if (this._$ul.css('margin-top') === '-20px') {
                        _target_index = this._menu_arr.indexOf(this._view_data[1]);
                        _next = this._menu_arr[_target_index - 1];
                        this.setFocus(_cnt);
                        _next = this._menu_arr[_target_index - 2];
                        if (_next !== undefined) {
                            _li = this._$li.clone();
                            this._$ul.prepend(_li.text(_next.name));
                            this._view_data.splice(0, 0, _next);
                            if (this._$ul.children().length > (_viewCount + 2)) {
                                this._$ul.children().last().remove();
                                this._view_data.splice(this._view_data.length - 1, 1);
                            }
                        }
                        else {
                            this._$ul.attr('style', 'margin-top:30px;');
                            if (this._view_data.length > (_viewCount + 1)) {
                                this._$ul.children().last().remove();
                                this._view_data.splice(this._view_data.length - 1, 1);
                            }
                        }
                    }
                    else {
                        this._view_data = [];
                        this.$.find('li').remove();
                        this._$ul.attr('style', 'margin-top:-20px;');
                        for (i = 0; i <= _viewCount; i += 1) {
                            tmp = this._menu_arr[this._menu_arr.length - 1 - i];
                            _li = this._$li.clone();
                            this._$ul.prepend(_li.text(tmp.name));
                            this._view_data.push(tmp);
                        }
                        this._view_data.reverse();
                        this.setFocus(_viewCount);
                    }
                }
            }
        };
        menuList.prototype._doOk = function (aParam) {
            var idx = this.getFocus();
            var target = this._menu_arr[idx];
            if (target.sub_menu) {
                this.show_sub_menu(target.sub_menu);
                this._menu_parent_name = target.name;
            }
            else if (target.call_back) {
                if (aParam.alData === undefined) {
                    aParam.alData = {};
                }
                aParam.alEvent = 'alClicked';
                aParam.alData.me = this;
                aParam.alData.status = 'ok';
                aParam.alData.call_back = target.call_back;
                aParam.alData.index = this.getFocus();
                this.sendEventToParent(aParam);
            }
        };
        menuList.prototype.show_sub_menu = function (aSub_menu) {
            this._level = this._level + 1;
            this.setList(aSub_menu);
        };
        return menuList;
    })(__COMPONENT_KERNEL__);
    exports.menuList = menuList;
    var hgroup = (function (_super) {
        __extends(hgroup, _super);
        function hgroup() {
            _super.call(this);
        }
        hgroup.prototype.create = function (aParam) {
            $.extend(this, aParam);
            $.extend(this, {
                _$head: $('<h1>', {
                    'html': 'TV'
                }),
                _$number: $('<p>', {
                    'html': this.selectItem + ' / ' + this.allItem
                }),
                $: $('<hgroup>')
            });
            this.$.append(this._$head).append(this._$number);
            return this;
        };
        hgroup.prototype.destroy = function () {
            this.$.remove();
        };
        return hgroup;
    })(__COMPONENT_KERNEL__);
    exports.hgroup = hgroup;
});
