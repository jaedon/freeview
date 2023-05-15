// file name : common.lib/CListControl.js
/* global CCircularArray */
/* global CtrlBase.CControl */
/* global $ */
/* global CKeyNaviMap */
/// <reference path="common.ts"/>
import CtrlBase = require("./ctrlbase");
module CtrlList {
    /*
     * @class CListModel
     */
    class CListModel extends CCircularArray {

        _modelItems;
        _modelItemsDataIndex;
        _rowCount: number;
        _dataRolling: boolean;

        constructor(rowCount) {
            super();
            var modelItems = [];
            var modelItemsDataIndex = [];
            var i;

            for (i = 0; i < rowCount; i += 1) {
                modelItems.push(null);
                modelItemsDataIndex.push(-1);
            }
            this._modelItems = modelItems;
            this._modelItemsDataIndex = modelItemsDataIndex;
            this._rowCount = rowCount;
            this._dataRolling = true;
        }
        destroy() {
            delete this._modelItems;
            super.destroy();
        }
        setData(data, startIndex?) {
            
            this.setArray(data);
            if (startIndex !== undefined) {
                this.setCur(startIndex);
            }
            this._updateModelItems();
        }
        appendData(data) {
            
            this.appendArray(data);
            this._updateModelItems();
        }
        removeData(index) {
            
            var length = this._array.length;
            var del = this.cur() + index;
            var ret = false;
            console.log('       : this.cur() : ' + this.cur());
            console.log('       : index : ' + index);
            console.log('       : del : ' + del);
            console.log('       : length : ' + length);
            if (del >= length) {
                del = del - length;
                ret = true;
            }
            this._array.splice(del, 1);
            if (ret) {
                var i;
                console.log('       : del2 : ' + del);
                for (i = 0; i <= del; i += 1) {
                    this._array.unshift(this._array.pop());
                }
                this.inc(del);
            }
            this._updateModelItems();
            return this._array.length;
        }
        setDataRolling(dataRolling) {
            
            this._dataRolling = dataRolling;
        }
        count() {
            var a = this,
                count = 0,
                i;

            for (i = 0; i < this._rowCount; i += 1) {
                if (this._modelItems[i]) {
                    count++;
                }
            }
            return count;
        }
        hasPage() {
            
            return this.count() < this.length();
        }
        getItems() {
            
            return this._modelItems;
        }
        getItem(index) {
            
            return this._modelItems[index];
        }
        up(count) {
            
            count = count ? count : 1;
            if (this.hasPage()) {
                if (this._dataRolling) {
                    this.dec(count);
                    this._updateModelItems();
                    return true;
                } else {
                    if (this.cur() !== 0) {
                        var cur = this.cur();
                        this.dec(count);
                        if (this.cur() >= cur) {
                            this.setCur(0);
                        }
                        this._updateModelItems();
                        return true;
                    }
                }
            }
            return false;
        }
        down(count) {
            
            count = count ? count : 1;
            if (this.hasPage()) {
                if (this._dataRolling) {
                    this.inc(count);
                    this._updateModelItems();
                    return true;
                } else {
                    if (this.cur() + this._rowCount < this.length()) {
                        this.inc(count);
                        this._updateModelItems();
                        return true;
                    }
                }
            }
            return false;
        }
        pageUp() {
            
            if (this.hasPage()) {
                if (this._dataRolling) {
                    this.dec(this._rowCount);
                    this._updateModelItems();
                    return true;
                } else {
                    var availCount = this.cur() - this._rowCount + this._rowCount;
                    availCount = availCount < 0 ? this._rowCount + availCount : availCount;
                    if (availCount) {
                        this.dec(availCount < this._rowCount ? availCount : this._rowCount);
                        this._updateModelItems();
                        return true;
                    }
                }
            }
            return false;
        }
        pageDown() {
            
            if (this.hasPage()) {
                if (this._dataRolling) {
                    this.inc(this._rowCount);
                    this._updateModelItems();
                    return true;
                } else {
                    var availCount = this.length() - this.cur() - this._rowCount;
                    if (availCount) {
                        this.inc(availCount < this._rowCount ? availCount : this._rowCount);
                        this._updateModelItems();
                        return true;
                    }
                }
            }
            return false;
        }

        _updateModelItems() {
            var a = this,
                count = 0,
                i;
            
            for (i = 0; i < this._rowCount; i += 1) {
                this._modelItems[i] = null;
            }
            if (this._dataRolling) {
                this.each((i, item) => {
                    this._modelItemsDataIndex[count] = i;
                    this._modelItems[count++] = item;
                    return count < this._rowCount;
                });
            } else {
                this.eachArray((i, item) => {
                    this._modelItemsDataIndex[count] = i;
                    this._modelItems[count++] = item;
                    return count < this._rowCount;
                });
            }
        }
        getDataIndexList() {
            
            return this._modelItemsDataIndex;
        }
        getDataIndex(modelIndex) {
            
            return this._modelItemsDataIndex[modelIndex];
        }
        getModelItemIndexFromDataIndex(dataIndex) {
            var a = this,
                ret = -1,
                i;
            
            for (i = 0; i < this._modelItemsDataIndex.length; i += 1) {
                if (dataIndex == this._modelItemsDataIndex[i]) {
                    ret = i;
                    break;
                }
            }
            return ret;
        }
    }
    /*
     * @class CListControl
     */

    export class CListControl extends CtrlBase.CControl {

        _$list;
        _$listItems;
        _$noItemMsg;
        //_$helpPrompt;
        _$paginationPrompt;
        _$pageUp;
        _$pageDown;
        _$indexingText;
        _model;
        _rowCount;
        onFocusGained;
        onFocusLost;
        onListItemSelected;
        onListItemFocusChanged;
        _dataRolling;
        _showPagination;
        _showHelpString;
        _showEmpty;
        _animation;
//        _animating;
        _upDownUnit;
        _itemDrawer;
        _itemActiveChecker;
        _prevDrawn;

        private _customPaginationSeparator: string;
        private _customPaginationUnit: string;

        constructor($self: JQuery, rowCount: number) {
            super($self);
            var $self = this.$self().addClass("-list");
            var $noItemMsg = $('<span class="-no-item">').text('No item');
            $self.append($noItemMsg);
            $noItemMsg.hide();
            var i;
            for (i = 0; i < rowCount; i += 1) {
                var $listItem = $('<div>', { 'class': '-list-item' }).attr('data', i);
                $self.append($listItem);
            }
            var $pagenationPrompt = $('<div>', { 'class': '-list-pagenation-prompt' });
            var $pageUp = $('<div>', { 'class': '-pagenation-page-up' });
            var $pageDown = $('<div>', { 'class': '-pagenation-page-down' });

            //$pageUp.append($('<img>', { src: "image/button/btn_bottom_key_pgup.png" }));
            //$pageUp.append($('<span>').text('Page Up'));
            //$pageDown.append($('<img>', { src: "image/button/btn_bottom_key_pgdn.png" }));
            //$pageDown.append($('<span>').css('margin-right', '42px').text('Page down'));

            $pagenationPrompt.append($pageUp);
            $pagenationPrompt.append($pageDown);

            //var $helpPrompt = $('<div>', { 'class': '-list-pagenation-prompt' });

            var $indexingText = $('<p>').css({ 'float': 'right' });
            $pagenationPrompt.append($indexingText);
            $pagenationPrompt.hide();
            $self.append($pagenationPrompt);
            //$self.append($helpPrompt);
            $self.append($self);
            this._$list = $self;
            this._$listItems = this._$list.children('.-list-item');
            this._$noItemMsg = $noItemMsg;
            //this._$helpPrompt = $helpPrompt;
            this._$paginationPrompt = $pagenationPrompt;
            this._$pageUp = $pageUp;
            this._$pageDown = $pageDown;
            this._$indexingText = $indexingText;
            this._model = null;
            this._rowCount = rowCount;
            this.onFocusGained = null;
            this.onFocusLost = null;
            this.onListItemSelected = null;
            this.onListItemFocusChanged = null;
            this._dataRolling = true;
            this._showPagination = true;
            this._showHelpString = true;
            this._showEmpty = false;
            this._animation = true;
            //this._animating = false;
            this._upDownUnit = this._rowCount - 1;
            this.registerSignal(["ListItemSelected"]);
        }
        destroy() {
            if (this._model) {
                this._model.destroy();
                this._model = null;
            }
            super.destroy();
        }
        _doFocusGained() {
            if (this.onFocusGained) {
                this.onFocusGained();
                this._fireItemFocusChangedEvent();
            }
            super._doFocusGained();
        }
        _doFocusLost() {
            if (this.onFocusLost) {
                this.onFocusLost();
            }
            super._doFocusLost();
        }
        _doFocusChanged(aElOld: HTMLElement, aElNew: HTMLElement) {
            this._fireItemFocusChangedEvent(aElOld, aElNew);
            super._doFocusChanged(aElOld, aElNew);
        }
        setUpDownUnit(aUpDownUnit: number) {
            this._upDownUnit = aUpDownUnit;
        }
        setNoItemMsg(text) {
            this._$noItemMsg.html(text);
        }
        setCustomPaginationSeparator(aSeperator: string) {
            this._customPaginationSeparator = aSeperator;
        }
        setCustomPaginationUnit(aUnit: string) {
            this._customPaginationUnit = aUnit;
        }
        setDrawer(itemDrawer) {
            this._itemDrawer = itemDrawer;
        }
        setData(data, startIndex?) {
            
            this._$noItemMsg.hide();
            if (this._model === null) {
                var model = new CListModel(this._rowCount);
                model.setDataRolling(this._dataRolling);
                model.setData(data, startIndex);
                this.setModel(model);
            } else {
                this._model.setData(data);
                this._drawAndRefocus();
            }
            this._fireItemFocusChangedEvent();
        }
        appendData(data) {
            
            this._model.appendData(data);
            this._drawPagination();
        }
        removeData(index) {
            var a = this,
                rolled = this._model.removeData(index);
            this._drawAndRefocus();
            return rolled;
        }
        setDataRolling(dataRolling) {
            
            this._dataRolling = dataRolling;
            if (this._model) {
                this._model.setDataRolling(dataRolling);
            }
        }
        updateData(dataIndex?) {
            
            if (typeof dataIndex == 'undefined') {
                var oldFocusItem = this.getFocusedItem();
                this._model._updateModelItems();
                this._draw();
                var newFocusItem = this.getFocusedItem();
                if (oldFocusItem.listItem != newFocusItem.listItem) {
                    this._fireItemFocusChangedEvent();
                }
            } else {
                var modelIndex = this._model.getModelItemIndexFromDataIndex(dataIndex);
                if (modelIndex != -1) {
                    this._model._updateModelItems();
                    // console.log('CListControl.updateData: ' + dataIndex + '->' + modelIndex);
                    this._draw(modelIndex);
                }
            }
        }
        updateItem(modelIndex) {
            this._draw(modelIndex);
        }
        getSeletedItem() {
            var $focused = this._$list.find('.-fd');
            var index = parseInt($focused.attr('data'), 10);
            return this._model.getItem(index);
        }
        setModel(model) {
            this._model = model;
            this._drawAndRefocus();
        }
        setItemActiveChecker(itemActiveChecker) {
            this._itemActiveChecker = itemActiveChecker;
        }
        showPagination(show) {
            this._showPagination = show;
        }
        showHelpString(show) {
            this._showHelpString = show;
            return this;
        }
        showEmpty(show) {
            this._showEmpty = show;
        }
        enableAnimation(enable) {
            this._animation = enable;
            //this._setTransitioning(enable);
        }
        count() {
            return this._model ? this._model.count() : 0;
        }
        _fireItemFocusChangedEvent(aOld?: HTMLElement, aNew?: HTMLElement) {
            var $focused = $(this.getFocusedElement());
            if ($focused.length) {
                var index = parseInt($focused.attr('data'), 10);
                var item = this._model.getItem(index);
                var length = this._model.length();
                if (this._showPagination && this._model.hasPage()) {
                    this._updateIndexingText(index);
                }
                if (this.onListItemFocusChanged) {
                    this.onListItemFocusChanged(index, $focused, item);
                }
            }
        }
        _fireItemSelectedEvent() {
            var $focused = $(this.getFocusedElement());
            var index = parseInt($focused.attr('data'), 10);
            var item = this._model.getItem(index);
            if (this.onListItemSelected) {
                this.onListItemSelected(index, $focused, item);
            }
            this._emitListItemSelected(index, $focused, item);
        }
        _drawPagination() {
            
            if (this._showPagination) {
                if (this._model.count() === 0) {
                    this._$noItemMsg.show();
                } else {
                    this._$noItemMsg.hide();
                }
                if (this._model.hasPage()) {
                    this._updateIndexingText();
                    this._$paginationPrompt.show();
                } else {
                    this._$paginationPrompt.hide();
                }
            }
        }
        _drawAndRefocus() {
            var activeFocused = this.isActiveFocused();
            this._draw();
            if (this._showPagination) {
                if (this._model.count() === 0) {
                    this._$noItemMsg.show();
                } else {
                    this._$noItemMsg.hide();
                }
                if (this._model.hasPage()) {
                    this._$paginationPrompt.show();
                } else {
                    this._$paginationPrompt.hide();
                }
            }
            this._reFocus(activeFocused);
        }
        jqPaginationPrompt() {
            
            return this._$paginationPrompt;
        }
        _reFocus(aActiveFocused: boolean) {
            console.log('CListControl._reFocus');
            var $listItems = this._$listItems.removeClass('-f -fd -afd-leaf');
            var modelItems = this._model.getItems();
            var i;
            for (i = 0; i < modelItems.length; i += 1) {
                var item = modelItems[i];
                var $listItem = $listItems.eq(i);
                if (item) {
                    if (this._itemActiveChecker) {
                        if (this._itemActiveChecker(item)) {
                            $listItem.addClass('-f');
                        }
                    } else {
                        $listItem.addClass('-f');
                        if (i === 0) {
                            $listItem.addClass('-fd');
                            if (aActiveFocused) {
                                $listItem.addClass('-afd-leaf');
                            }
                        }
                    }
                }
            }
            this.buildNaviMap(CtrlBase.KBuilderTopDown);
        }
        _draw(index?) {
            var activeFocused = this.isActiveFocused();
            var $listItems = this._$listItems;
            var modelItems = this._model.getItems();
            var itemUpdate = typeof index != 'undefined';
            if (!itemUpdate) {
                $listItems.css('visibility', 'hidden');
                $listItems.empty();
            } else {
                $listItems.eq(index).css('visibility', 'hidden');
                $listItems.eq(index).empty();
            }

            //this._$helpPrompt.hide();

            var drawn = [];
            var drawItem = (index, $listItem, item) => {
                if (item) {
                    this._itemDrawer(index, $listItem, item);
                    $listItem.css('visibility', '');
                    return true;
                }
                return false;
            };
            var modelItemLen = modelItems.length;
            var i;
            if (itemUpdate) {
                drawItem(index, $listItems.eq(index), modelItems[index]);
            } else {
                for (i = 0; i < modelItemLen; i += 1) {
                    drawn.push(drawItem(i, $listItems.eq(i), modelItems[i]));
                }
            }
            if (this._showEmpty) {
                $listItems.css('visibility', '');
            }
            if (this._prevDrawn) {
                var refocus = false;
                for (i = 0; i < drawn.length; i += 1) {
                    if (this._prevDrawn[i] != drawn[i]) {
                        refocus = true;
                    }
                }
                if (refocus) {
                    $listItems.show();
                    this._reFocus(activeFocused);
                }
            }
            this._prevDrawn = drawn;
            if (this._showPagination) {
                if (this._model.hasPage()) {
                    if (this._dataRolling) {
                        if (this._showHelpString) {
                            this._$pageUp.addClass("-show");
                            this._$pageDown.addClass("-show");
                        } else {
                            this._$pageUp.removeClass("-show");
                            this._$pageDown.removeClass("-show");
                        }
                    } else {
                        var first = this._model.getDataIndex(0) + 1;
                        var last = this._model.getDataIndex(this._rowCount - 1) + 1;
                        var length = this._model.length();
                        this._updateIndexingText();
                        
                        if (this._showHelpString) {
                            this._$pageUp.addClass("-show");
                            this._$pageDown.addClass("-show");
                            
                            if (this._rowCount > first) {
                                this._$pageUp.removeClass("-show");
                            }
                            
                            if (length - this._rowCount < first) {
                                this._$pageDown.removeClass("-show");
                            }
                        } else {
                            this._$pageUp.removeClass("-show");
                            this._$pageDown.removeClass("-show");
                        }
                    }
                } else {
                    this._$paginationPrompt.hide();
                }
                if (this._model.count() === 0) {
                    this._$noItemMsg.show();
                } else {
                    this._$noItemMsg.hide();
                }
            }
            //this._$helpPrompt.show();
        }
        getPaginationInfo() {
            var ret = {
                ableToPageUp: false,
                ableToPageDown: false
            };
            if (this._model.hasPage()) {
                var first = this._model.getDataIndex(0) + 1;
                var length = this._model.length();
                if (this._dataRolling) {
                    ret.ableToPageUp = true;
                    ret.ableToPageDown = true;
                } else {
                    if (this._rowCount > first) {
                        ret.ableToPageUp = true;
                    }
                    if (length - this._rowCount < first) {
                        ret.ableToPageDown = true;
                    }
                }
            }
            return ret;
        }
        _updateIndexingText(index?) {
            if (index === undefined) {
                var $focused = this._$list.find('.-fd');
                if ($focused.length) {
                    index = parseInt($focused.attr('data'), 10);
                } else {
                    return;
                }
            }
            var length = this._model.length();
            var first = this._model.getDataIndex(0) + 1;
            var last = this._model.getDataIndex(this._rowCount - 1) + 1;
            if (last === 0 && first > length - this._rowCount) {
                last = this._model.length();
            }
            var separator = this._customPaginationSeparator || '/';
            var unit = this._customPaginationUnit || '';
            var html = '<span>' + first + '</span><span>-' + last + separator + length + unit + '</span>';
            this._$indexingText.html(html);
        }
        doSetFocus(focus) {
            
            if (focus) {
                if (this.onFocusGained) {
                    this.onFocusGained();
                }
                this._fireItemFocusChangedEvent();
            } else {
                if (this.onFocusLost) {
                    this.onFocusLost();
                }
            }
        }
        setFocusIndex(aIndex: number) {
            /*
            var $focused = this._$list.find('.-fd');
            var $focusable = this._$list.find('.-f');
            var baseIndex = parseInt($focused.attr('data'), 10);

            if (aIndex < 0 || aIndex >= this._rowCount || baseIndex == aIndex || $focusable.length == 1) {
                return false;
            }

            $focused.removeClass('-fd');
            $($focusable[aIndex]).addClass('-fd');
            */
            this.setFocusedElementByIndex(aIndex);
            this._fireItemFocusChangedEvent();
            return true;
        }
        getFocusedItem() {
            var $focused = this._$list.find('.-fd');
            var index = parseInt($focused.attr('data'), 10);
            var listItem = this._model.getItem(index);
            var dataIndex = this._model.getDataIndex(index);
            return {
                index: index,
                $listItem: $focused,
                listItem: listItem,
                dataIndex: dataIndex
            };
        }
        getItem(index) {
            var $listItem = this._$listItems.eq(index);
            var listItem = this._model.getItem(index);
            var dataIndex = this._model.getDataIndex(index);
            return {
                index: index,
                $listItem: $listItem,
                listItem: listItem,
                dataIndex: dataIndex
            };
        }
        _animBar(topStart, topEnd, cb) {
            var $listItems = this._$listItems.hide().detach();
            var $animBar = $('<div>', { 'class': '-list-anim-bar' })
                .css('top', topStart + 'px')
                .appendTo(this._$list);
            this._setTransitioning(true);
            this._$paginationPrompt.hide();
            $animBar.anim('top', topEnd + 'px', () => {
                this._setTransitioning(false);
                this._$paginationPrompt.show();
                $listItems.prependTo(this._$list);
                cb();
                $listItems.fadeIn();
                $animBar.remove();
            });
        }
        _doKeyUpLatent(): boolean {
            if (this._model.up(this._upDownUnit)) {
                var apply = () => {
                    this._draw();
                    if (this._upDownUnit != 1) {
                        //this.setFocusIndex(this._rowCount - 1);
                        this.setFocusedElementByIndex(this._rowCount - 1);
                    }
                    this._fireItemFocusChangedEvent();
                };
                if (this._animation) {
                    var topStart = this.getItem(0).$listItem.position().top;
                    var topEnd = this.getItem(this._rowCount - 1).$listItem.position().top;
                    this._animBar(topStart, topEnd, apply);
                } else {
                    apply();
                }
                return true;
            }
            return false;
        }
        _doKeyDownLatent() {
            if (this._model.down(this._upDownUnit)) {
                var apply = () => {
                    this._draw();
                    if (this._upDownUnit != 1) {
                        //this.setFocusIndex(0);
                        this.setFocusedElementByIndex(0);
                    }
                    this._fireItemFocusChangedEvent();
                };
                if (this._animation) {
                    var topStart = this.getItem(this._rowCount - 1).$listItem.position().top;
                    var topEnd = this.getItem(0).$listItem.position().top;
                    this._animBar(topStart, topEnd, apply);
                } else {
                    apply();
                }
                return true;
            }
            return false;
        }
        _doKeyPageUp() {
            if (this.isTransitioning()) {
                return true;
            }
            if (this._model.pageUp()) {
                var apply = () => {
                    this._draw();
                    this._fireItemFocusChangedEvent();
                    this._emitFocusChanged(null, this.getFocusedElement());
                };
                if (this._animation) {
                    var topStart = this.getItem(0).$listItem.position().top;
                    var $last = this.getItem(this._rowCount - 1).$listItem;
                    var topEnd = $last.position().top + $last.height();
                    this._animBar(topStart, topEnd, apply);
                } else {
                    apply();
                }
            }
            return true;
        }
        _doKeyPageDown() {
            if (this.isTransitioning()) {
                return true;
            }
            if (this._model.pageDown()) {
                var apply = () => {
                    this._draw();
                    this._fireItemFocusChangedEvent();
                    this._emitFocusChanged(null, this.getFocusedElement());
                };
                if (this._animation) {
                    var $last = this.getItem(this._rowCount - 1).$listItem;
                    var topStart = $last.position().top + $last.height();
                    var topEnd = this.getItem(0).$listItem.position().top;
                    this._animBar(topStart, topEnd, apply);
                } else {
                    apply();
                }
            }
            return true;
        }
        _doKeyOk() {
            this._fireItemSelectedEvent();
            return true;
        }
        rollDataPositionCalculator(pos) {
            
            return this._model.indexing(pos);
        }
        //ItemSelected
        connectListItemSelected(aHolder: any, aSlotName: string, aSlot: (aIndex: number, $item: JQuery, aItem: any) => void) {
            this.connect("ListItemSelected", aHolder, aSlotName);
        }
        private _emitListItemSelected(aIndex: number, $item: JQuery, item: any) {
            this.emit.call(this, "ListItemSelected", aIndex, $item, item);
        }
    }
    /*
     * @class CStringListControl
     */

    export class CStringListControl extends CListControl {
        constructor($self, rowCount) {
            super($self, rowCount);
            
            this._itemDrawer = function (index, $listItem, string) {
                $listItem.append($('<span>').html(string));
            };
        }
    }

    /*
     * @class CAnchorListControl
     */
    export class CAnchorListControl extends CListControl {

        _anchorIndex;
        _animation;
        _upDownUnit;

        constructor($self, rowCount) {
            super($self, rowCount);
            
            this._anchorIndex = 0;
            this._animation = false;
            this._upDownUnit = 1;
            this.setDataRolling(false);
        }

        _reFocus() {
            var a = this,
                $listItems = this._$listItems.removeClass('-f -fd'),
                modelItems = this._model.getItems(),
                i;

            for (i = 0; i < modelItems.length; i += 1) {
                var item = modelItems[i];
                var $listItem = $listItems.eq(i);
                if (item) {
                    if (this._anchorIndex == i) {
                        $listItem.addClass('-f -fd');
                    }
                }
            }
            this.buildNaviMap(CtrlBase.KBuilderTopDown);
        }

        setAnchorIndex(index) {
            
            if (0 <= index && index < this._rowCount) {
                this._anchorIndex = index;
            } else {
                throw 'CAnchorList index is out of range.';
            }
        }

        setAnchorListData(data, startIndex, roll?) {
            
            var roll = roll || false;
            if (this._model === null) {
                var model = new CListModel(this._rowCount);
                var dataCopy = [];
                var i;

                if (!roll) {
                    for (i = 0; i < this._anchorIndex; i += 1) {
                        dataCopy.push(null);
                    }
                }
                for (i = 0; i < data.length; i += 1) {
                    dataCopy.push(data[i]);
                }
                if (!roll) {
                    for (i = 1; i < (this._rowCount - this._anchorIndex); i += 1) {
                        dataCopy.push(null);
                    }
                }
                if (startIndex) {
                    model.setData(dataCopy, startIndex);
                } else {
                    model.setData(dataCopy);
                }
                this.setModel(model);
            } else {
                this._model.setData(data);
                this._drawAndRefocus();
            }
            this.setDataRolling(roll);
        }

    }
}
export = CtrlList;