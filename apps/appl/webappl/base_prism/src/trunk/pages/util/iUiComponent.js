/// <reference path="../../def/sugar.d.ts" />
// file name : /pages/util/iUiComponent.js
/*jslint sloppy:true, nomen:true, vars:true */
/**
 * UIComponent API's
 */
/*
if (!hx) {
    var hx = {};
}

hx.ui = {};
*/
define(["require", "exports"], function (require, exports) {
    var mUIComponent;
    (function (mUIComponent) {
        mUIComponent.uiComponent = {
            el: null,
            onChangeRow: null,
            _activeIndex: null,
            _count: null,
            template: {
                toEl: function (tplStr) {
                    var el = this.el;
                    if ((el === null) || (el === undefined)) {
                        el = document.createElement('div');
                    }
                    el.innerHTML = tplStr;
                    return el.firstChild;
                }
            },
            listable: {
                _nativeFocus: true,
                _$listEl: null,
                /**
                 * @private
                 * @type {jQuery}
                 */
                _$items: null,
                _datas: null,
                /**
                 * item active index
                 * @private
                 * @type {Number}
                 */
                _activeIndex: -1,
                /**
                 * row active index
                 * @private
                 * @type {Number}
                 */
                _activeRowIndex: 0,
                /**
                 * @private
                 * @type {Number}
                 */
                _selectedIndex: -1,
                /**
                 * @private
                 * @type {Number}
                 */
                _currentPageIndex: -1,
                /**
                 * @public
                 * @type {Number}
                 */
                _pageRowCount: 4,
                _columnCount: 1,
                /**
                 * @private
                 * @type {Number}
                 */
                _count: 0,
                _listSelector: 'ul',
                onItemActive: function () {
                },
                getListEl: function () {
                    if (this._$listEl === null) {
                        //this._$listEl = this.$.children(this._listSelector);
                        this._$listEl = $(this._listSelector, this.$);
                    }
                    return this._$listEl;
                },
                /**
                 * @private
                 * @return {Number} pageCount
                 */
                _getPageCount: function () {
                    return Math.ceil(this._count / (this._pageRowCount * this._columnCount));
                },
                /**
                 * @private
                 * @return {Number} pageCount
                 */
                _getRowCount: function () {
                    return Math.ceil(this._count / (this._columnCount));
                },
                /**
                 * @public
                 * @return {Object}
                 *           - album {String}
                 *           - artist {String}
                 *           - deviceIndex {Number}
                 *           - duration {Number}
                 *           - fileType {Number}
                 *           - fullPath {String}
                 *           - pictureDate {String}
                 *           - playPosition {Number}
                 *           - resolution {String}
                 *           - thumbnailUrl {String}
                 *           - title {String}
                 *           - watched {Number}
                 */
                getActiveItemData: function () {
                    return this._datas[this._activeIndex];
                },
                /**
                 * @public
                 * @param {Number} index
                 */
                setItemActive: function (index) {
                    var elCnt = this._count;
                    var oldInex = this._activeIndex;
                    if (0 > index || (elCnt - 1) < index || elCnt === 0) {
                        return;
                    }
                    /*
                     if (this._nativeFocus) {
                     this._$items[index].focus();
                     } else {
                     if (oldInex > -1) {
                     this._$items[oldInex].removeClass('on');
                     }
                     this._$items[index].addClass('on');
                     }*/
                    if (oldInex > -1) {
                        this._$items[oldInex].removeClass('on');
                    }
                    this._$items[index].addClass('on');
                    this._$items[index].removeClass('his');
                    this._activeIndex = index;
                    var itemData = this._datas[this._activeIndex];
                    this.onItemActive(index, itemData);
                    var activeRowIndex = Math.floor(index / this._columnCount);
                    if (this._activeRowIndex !== activeRowIndex) {
                        var direction = (this._activeRowIndex < activeRowIndex ? 'down' : 'up');
                        var isJump = Math.abs(this._activeRowIndex - activeRowIndex) > 1;
                        if (isJump) {
                            direction += 'jump';
                        }
                        this._activeRowIndex = activeRowIndex;
                        if (this.onChangeRow) {
                            this.onChangeRow(activeRowIndex, direction);
                        }
                    }
                    // + '\n _currentPageIndex : ' + this._currentPageIndex);
                },
                clearFocus: function () {
                    if (this._selectedIndex > -1) {
                        this._$items[this._selectedIndex].removeClass('his');
                        this._selectedIndex = -1;
                    }
                    if (this._activeIndex > -1) {
                        this._$items[this._activeIndex].removeClass('on');
                        this._activeIndex = -1;
                    }
                },
                /**
                 * select item
                 * @public
                 * @param {Number} selectIndex
                 */
                setItemSelected: function (index) {
                    var selectedIndex = this._selectedIndex;
                    if (selectedIndex > -1) {
                        this._$items[selectedIndex].removeClass('his');
                    }
                    if (index > -1) {
                        this._$items[index].addClass('his');
                    }
                    this._selectedIndex = index;
                },
                /**
                 * @private
                 */
                _clear: function () {
                    this.getListEl()[0].innerHTML = '';
                    this._$items = [];
                    this._datas = [];
                    this._activeIndex = 0;
                    this._count = 0;
                },
                /**
                 * navigation up
                 * @private
                 * @param {Number} offsetPosition Count
                 */
                _nav_up: function () {
                    var that = this;
                    var activeIndex = that._count - 1;
                    if (that._activeIndex > 0) {
                        activeIndex = that._activeIndex - 1;
                    }
                    this.setItemActive(activeIndex);
                },
                /**
                 * navigation down
                 * @private
                 */
                _nav_down: function () {
                    var that = this;
                    var activeIndex = 0;
                    if (that._count - 1 > that._activeIndex) {
                        activeIndex = that._activeIndex + 1;
                    }
                    this.setItemActive(activeIndex);
                },
                /**
                 * @public
                 * @event
                 */
                onAdd: function () {
                },
                /**
                 * @private
                 */
                _resetViewPos: function (rowIndex, direction) {
                    var i, len, start;
                    this._viewPos = [];
                    if (direction === 'down') {
                        start = (rowIndex - (this._pageRowCount - 1));
                        len = rowIndex;
                    }
                    else if (direction === 'up') {
                        start = rowIndex;
                        len = (rowIndex + (this._pageRowCount - 1));
                    }
                    else if (direction === 'jump') {
                        start = rowIndex;
                        len = (rowIndex + (this._pageRowCount - 1));
                    }
                    for (i = start; i <= len; i += 1) {
                        this._viewPos.push(i);
                    }
                },
                /**
                 * @interface
                 */
                /**
                 * item add
                 * @private
                 * @param {String} HTMLTemplate
                 */
                _addItem: function (template, argObject) {
                    var $itemEl = $(template);
                    this._$items.push($itemEl);
                    this.getListEl()[0].appendChild($itemEl[0]);
                    if (this._datas === null) {
                        this._datas = [];
                    }
                    this._datas.push(argObject);
                    this._count += 1;
                    this.onAdd();
                },
                /**
                 * @private
                 * @param {String} HtmlTemplate
                 * @param {Object} Object
                 * @param {Element} Element or jQuery Element
                 * @param {String Enum} insertPosition "before||after"
                 */
                _inertItem: function (template, argObject, targetEl, insertPosition) {
                    var $itemEl = $(template);
                    var index = targetEl.index();
                    if (insertPosition === 'before') {
                        $itemEl.insertBefore(targetEl);
                        this._$items.splice(index, 0, $itemEl);
                        this._datas.splice(index, 0, argObject);
                    }
                    else if (insertPosition === 'after') {
                        if (index === targetEl[0].parentNode.childNodes.length - 1) {
                            this._addItem(template, argObject);
                            return;
                        }
                        //target2Parent.insertBefore(div.childNodes[0],target2Ref)
                        targetEl[0].parentNode.insertBefore($itemEl[0], targetEl[0].nextSibling);
                        //$itemEl.inserAfter(targetEl); jQuery buggy..
                        this._$items.splice(index + 1, 0, $itemEl);
                        this._datas.splice(targetEl.index() + 1, 0, argObject);
                    }
                    this._count += 1;
                    this.onAdd();
                },
                /**
                 * @public
                 * @event
                 */
                onRemove: function () {
                },
                /**
                 * @interface
                 */
                //remove
                /**
                 * @public
                 * @method
                 * @param {Number} index
                 * @example
                 *      this.remove(0);
                 */
                _removeItem: function (index, autoFocus) {
                    var removeAfterFocus = (autoFocus === false ? false : true);
                    if (index < 0 || this._count - 1 < index || this._count === 0) {
                        return;
                    }
                    this._$items[index].remove();
                    this._$items.splice(index, 1);
                    this._datas.splice(index, 1);
                    this._count -= 1;
                    if (removeAfterFocus && this._count > 0 && this._count > index) {
                        this.setItemActive(index);
                    }
                    this.onRemove();
                }
            },
            visibility: {
                _isActive: false,
                /**
                 * show after event
                 * @public
                 * @event
                 */
                onShow: function () {
                },
                /**
                 * show after event
                 * @private
                 * @event
                 _onShow : function() {
             },
                 */
                /**
                 * hide after event
                 * @public
                 * @event
                 */
                onHide: function () {
                },
                /**
                 * hide after event
                 * @private
                 * @event
    
                 _onHide : function() {
             },
                 */
                /**
                 * @public
                 * @method
                 */
                show: function () {
                    if (this._onShow !== null) {
                        this._onShow.apply(this, arguments || []);
                    }
                    this.$[0].style.display = 'block';
                    this._isActive = true;
                    this.onShow();
                },
                /**
                 * @public
                 * @method
                 */
                hide: function () {
                    this.$[0].style.display = 'none';
                    this._isActive = false;
                    if (this._onHide !== null) {
                        this._onHide.apply(this, arguments || []);
                    }
                    this.onHide();
                }
            }
        };
        var $overay = $('<article class="overlay" style="z-index:1"></article>');
        mUIComponent.overlay = {
            hide: function () {
                if ($overay) {
                    $overay.remove();
                    $overay[0].className = 'overlay';
                }
            },
            left: function ($target) {
                if ($target.length) {
                    $target.append($overay);
                    $overay[0].className = 'overlay optDim-left';
                }
            },
            right: function ($target) {
                if ($target.length) {
                    $target.append($overay);
                    $overay[0].className = 'overlay optDim-right';
                }
            },
            bottomLeft: function ($target) {
                if ($target.length) {
                    $target.append($overay);
                    $overay[0].className = 'overlay optTitleDim-left';
                }
            },
            bottomRight: function ($target) {
                if ($target.length) {
                    $target.append($overay);
                    $overay[0].className = 'overlay optTitleDim-right';
                }
            }
        };
    })(mUIComponent || (mUIComponent = {}));
    return mUIComponent;
});
