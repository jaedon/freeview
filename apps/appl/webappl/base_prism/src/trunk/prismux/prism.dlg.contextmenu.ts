/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.contextmenu.js
/*jslint sloppy:true, nomen:true, vars:true */

import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    export class CContextMenu extends __PRISM_CDIALOG__.CDialog {
        private _$contextMenuBg: JQuery;
        private $contextMenu: JQuery;
        public  _contextMenuInfo: any;
        private _contextMenu: __CONTROLS__.CLayeredGroupControl;
        private _checkList: any;
        private _depth: any;
        public  _onItemSelected: any;
        constructor(layeredGroupControl?: __CONTROLS__.CLayeredGroupControl) {
            super();
            var a = this;
            if (arguments.length) {
                a.init(layeredGroupControl);
            }
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl);
        }
        destroy() {
            var a = this;
            if (a._$contextMenuBg) {
                a._$contextMenuBg.remove();
                delete a._$contextMenuBg;
            }
            super.destroy.call(a);
            delete a._contextMenuInfo;
            if (a._checkList) {
                a._checkList = null;
            }
        }
        setContextMenuInfo(contextMenuInfo) {
            var a = this;
            a._contextMenuInfo = contextMenuInfo;
        }
        setDisplayDepth(depth) {
            var a = this;
            a._depth = depth;
        }
        doKeyOption() {
            var a = this;
            a.close();
            return true;
        }
        doKey(keyCode) {
            var a = this;
            var consumed = super.doKey(keyCode);
            switch(keyCode) {
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
        }
        doOpen(param?) {
            var a = this;
            var $contextMenuBg = $('<div>', {
                'class' : 'contextMenuBg newUi'
            });
            if (a._depth) {
                $($contextMenuBg.get(0)).css('z-index', a._depth);
            }
            if (param.drawParam && param.drawParam.$parent) {
                var $parent = param.drawParam.$parent;
                $contextMenuBg.appendTo($parent);
                a._$contextMenuBg = $contextMenuBg;
            } else {
                $contextMenuBg.appendTo(a._parent.$layer());
            }

            var contextMenu: any = new CLayeredGroupControl($contextMenuBg);
            contextMenu.setCommonTag({
                'optionMenuInd' : 'optionMenuInd',
                'contextMenu' : 'contextMenu newUi'
            });
            a.setChildControl([contextMenu]);
            contextMenu.createLayer({
                fnAfterCreate : function() {
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
                            setFocusRolling : true,
                            visibleAfterDraw: true
                        });
                    } else {
                        listControl.setDrawParam({
                            itemHeight : 50,
                            setFocusRolling : true
                        });
                    }
                    if (param && param.useSubBackground) {
                        $contextMenuBg.prepend($('<div>', {
                            'class' : 'contextMenuBg subBg'
                        }));
                    }
                    listControl.setDataDrawer(function(param) {
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
                        } else if (item.type === 'normal') {
                            $item.append('<span class="l">' + item.name + '</span>');
                        } else {
                            $item.append('<span class="b">' + '&nbsp;' + '</span>' + '<span class="l">' + item.name + '</span>');
                            if (item.decorator) {
                                item.decorator($item, param.index);
                            }
                            // param.$item.append('<span class="l">' + param.item.name + '</span>');
                            // param.$item.text(param.item.name);
                        }
                    });
                    listControl.onItemSelected = function(param) {
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
                            } else {
                                a._checkList[param.item.id] = true;
                                $checkbox.addClass('checked');
                            }
                        }
                        var cbParam:any = {};
                        if (param.item.id) {
                            cbParam.id = param.item.id;
                        }
                        if (a._checkList) {
                            cbParam.checkList = a._checkList;
                        }

                        cbParam.index = idx;

                        if (a._onItemSelected) {
                            a._onItemSelected(cbParam);
                        } else {
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
        }
        doShow() {
            var a = this;
            if (a._contextMenu && a._contextMenu._$self) {
                a._contextMenu._$self.removeClass('hide');
            }
        }
        doHide() {
            var a = this;
            if (a._contextMenu && a._contextMenu._$self) {
                a._contextMenu._$self.addClass('hide');
            }
        }
        contextMenu() {
            return this._contextMenu;
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createContextMenu = function(layeredGroupControl, contextMenuInfo?, callback?) {
        if (layeredGroupControl) {
            var contextMenu = new CContextMenu(layeredGroupControl);
            if (contextMenu) {
                contextMenu._contextMenuInfo = contextMenuInfo;
            }
            if (callback) {
                contextMenu._onItemSelected = callback;
            }
            return contextMenu;
        } else {
            var globalDlg = prism.createGlobalDialog('createContextMenu', contextMenuInfo, callback);
            return globalDlg;
        }
    };
}
export = mPrism;
