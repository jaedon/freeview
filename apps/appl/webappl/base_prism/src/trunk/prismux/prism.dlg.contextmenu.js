/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.contextmenu.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var CContextMenu = (function (_super) {
            __extends(CContextMenu, _super);
            function CContextMenu(layeredGroupControl) {
                _super.call(this);
                var a = this;
                if (arguments.length) {
                    a.init(layeredGroupControl);
                }
            }
            CContextMenu.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
            };
            CContextMenu.prototype.destroy = function () {
                var a = this;
                if (a._$contextMenuBg) {
                    a._$contextMenuBg.remove();
                    delete a._$contextMenuBg;
                }
                _super.prototype.destroy.call(a);
                delete a._contextMenuInfo;
                if (a._checkList) {
                    a._checkList = null;
                }
            };
            CContextMenu.prototype.setContextMenuInfo = function (contextMenuInfo) {
                var a = this;
                a._contextMenuInfo = contextMenuInfo;
            };
            CContextMenu.prototype.setDisplayDepth = function (depth) {
                var a = this;
                a._depth = depth;
            };
            CContextMenu.prototype.doKeyOption = function () {
                var a = this;
                a.close();
                return true;
            };
            CContextMenu.prototype.doKey = function (keyCode) {
                var a = this;
                var consumed = _super.prototype.doKey.call(this, keyCode);
                switch (keyCode) {
                    case CONTROLS_CONFIG.KEY_UP:
                    case CONTROLS_CONFIG.KEY_DOWN:
                    case CONTROLS_CONFIG.KEY_LEFT:
                    case CONTROLS_CONFIG.KEY_RIGHT:
                    case CONTROLS_CONFIG.KEY_PAGEUP:
                    case CONTROLS_CONFIG.KEY_PAGEDOWN:
                        consumed = true;
                        break;
                }
                return consumed;
            };
            CContextMenu.prototype.doOpen = function (param) {
                var a = this;
                var $contextMenuBg = $('<div>', {
                    'class': 'contextMenuBg newUi'
                });
                if (a._depth) {
                    $($contextMenuBg.get(0)).css('z-index', a._depth);
                }
                if (param.drawParam && param.drawParam.$parent) {
                    var $parent = param.drawParam.$parent;
                    $contextMenuBg.appendTo($parent);
                    a._$contextMenuBg = $contextMenuBg;
                }
                else {
                    $contextMenuBg.appendTo(a._parent.$layer());
                }
                var contextMenu = new CLayeredGroupControl($contextMenuBg);
                contextMenu.setCommonTag({
                    'optionMenuInd': 'optionMenuInd',
                    'contextMenu': 'contextMenu newUi'
                });
                a.setChildControl([contextMenu]);
                contextMenu.createLayer({
                    fnAfterCreate: function () {
                        if (a._contextMenuInfo.text) {
                            contextMenu.$optionMenuInd().text(a._contextMenuInfo.text);
                        }
                        if (a._contextMenuInfo.cssClass) {
                            contextMenu.addClass(a._contextMenuInfo.cssClass);
                        }
                        var listControl = new CListControl(contextMenu.$contextMenu());
                        listControl.setDataProvider(new CDataProvider(a._contextMenuInfo.menuItems));
                        if (param && param.itemHeight) {
                            listControl.setDrawParam({
                                itemHeight: param.itemHeight,
                                setFocusRolling: true,
                                visibleAfterDraw: true
                            });
                        }
                        else {
                            listControl.setDrawParam({
                                itemHeight: 50,
                                setFocusRolling: true
                            });
                        }
                        if (param && param.useSubBackground) {
                            $contextMenuBg.prepend($('<div>', {
                                'class': 'contextMenuBg subBg'
                            }));
                        }
                        listControl.setDataDrawer(function (param) {
                            var item = param.item;
                            var $item = param.$item;
                            $item.addClass('contextMenuItem newUi');
                            if (item.cssClass) {
                                $item.addClass(param.item.cssClass);
                                if ((param.index + 1) === a._contextMenuInfo.menuItems.length) {
                                    $item.addClass('last');
                                }
                            }
                            if (item.type === 'checkbox') {
                                $item.append('<span class="checkbox">' + '&nbsp;' + '</span>');
                                $item.addClass('indent');
                            }
                            if (item.type === 'button') {
                                $item.append('<p class="button">' + param.item.name + '</p>');
                            }
                            else if (item.type === 'normal') {
                                $item.append('<span class="l">' + item.name + '</span>');
                            }
                            else {
                                $item.append('<span class="b">' + '&nbsp;' + '</span>' + '<span class="l">' + item.name + '</span>');
                                if (item.decorator) {
                                    item.decorator($item, param.index);
                                }
                            }
                        });
                        listControl.onItemSelected = function (param) {
                            var self = listControl;
                            var fn = a._contextMenuInfo.menuItems[param.itemKey].fn;
                            var idx = param.itemKey;
                            var noclose = param.item.noclose;
                            var redraw = param.item.redraw;
                            var $checkbox;
                            if (!noclose) {
                                a.close(param);
                            }
                            if (param.item.type === 'checkbox') {
                                if (!a._checkList) {
                                    a._checkList = {};
                                }
                                $checkbox = param.$item.find('.checkbox');
                                if ($checkbox.hasClass('checked')) {
                                    a._checkList[param.item.id] = false;
                                    $checkbox.removeClass('checked');
                                }
                                else {
                                    a._checkList[param.item.id] = true;
                                    $checkbox.addClass('checked');
                                }
                            }
                            var cbParam = {};
                            if (param.item.id) {
                                cbParam.id = param.item.id;
                            }
                            if (a._checkList) {
                                cbParam.checkList = a._checkList;
                            }
                            cbParam.index = idx;
                            if (a._onItemSelected) {
                                a._onItemSelected(cbParam);
                            }
                            else {
                                fn(cbParam);
                            }
                            if (redraw && a._contextMenuInfo) {
                                var index = self.getCurrentIndex();
                                self.setDataProvider(new CDataProvider(a._contextMenuInfo.menuItems));
                                self.redraw();
                                self.setFocusedItem(index);
                            }
                        };
                        contextMenu.setChildControl([listControl]);
                    }
                });
                a._contextMenu = contextMenu;
            };
            CContextMenu.prototype.doShow = function () {
                var a = this;
                if (a._contextMenu && a._contextMenu._$self) {
                    a._contextMenu._$self.removeClass('hide');
                }
            };
            CContextMenu.prototype.doHide = function () {
                var a = this;
                if (a._contextMenu && a._contextMenu._$self) {
                    a._contextMenu._$self.addClass('hide');
                }
            };
            CContextMenu.prototype.contextMenu = function () {
                return this._contextMenu;
            };
            return CContextMenu;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CContextMenu = CContextMenu;
        if (!window.prism) {
            window.prism = {};
        }
        prism.createContextMenu = function (layeredGroupControl, contextMenuInfo, callback) {
            if (layeredGroupControl) {
                var contextMenu = new CContextMenu(layeredGroupControl);
                if (contextMenu) {
                    contextMenu._contextMenuInfo = contextMenuInfo;
                }
                if (callback) {
                    contextMenu._onItemSelected = callback;
                }
                return contextMenu;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createContextMenu', contextMenuInfo, callback);
                return globalDlg;
            }
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
