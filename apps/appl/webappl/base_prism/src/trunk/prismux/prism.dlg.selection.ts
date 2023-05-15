/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.selection.js
/*jslint sloppy:true, nomen:true, vars:true */
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    var mOpenCount = 0;
    export class CSelectionDlg extends __PRISM_CDIALOG__.CDialog {
        private _selectionItems: any[];
        private _selectionIndex: number;
        private _selectionItemWidth: number;
        private _selectionItemDrawer: Function;
        private _textHtml: string;

        onSelectionChanged: (aParam) => void;
        static isOpened() {
            if (mOpenCount < 0) throw "check init & destroy routine";
            return mOpenCount ? true : false;
        }

        constructor(layeredGroupControl) {
            super(layeredGroupControl);
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl);
            a.addClass('selectionDlg');
            mOpenCount += 1;
        }
        destroy() {
            var a = this;
            super.destroy.call(a);
            mOpenCount -= 1;
        }
        setSelectionItems(selectionItems) {
            var a = this;
            a._selectionItems = selectionItems;
        }
        setSelectionIndex(selectionIndex) {
            var a = this;
            a._selectionIndex = selectionIndex;
        }
        setSelectionItemWidth(selectionItemWidth) {
            var a = this;
            a._selectionItemWidth = selectionItemWidth;
        }
        setSelectionItemDrawer(selectionItemDrawer) {
            var a = this;
            a._selectionItemDrawer = selectionItemDrawer;
        }
        setTextHtml(textHtml) {
            var a = this;
            a._textHtml = textHtml;
        }
        doOpen(param?) {
            var a = this;
            var $arrowLeft = $('<div>', {
                'class': 'arrow-left'
            });
            var $arrowRight = $('<div>', {
                'class': 'arrow-right'
            });
            var updateArrow = function () {
                if (list.getNextAvail('left').left !== undefined) {
                    $arrowLeft.addClass('show');
                } else {
                    $arrowLeft.removeClass('show');
                }
                if (list.getNextAvail('right').left !== undefined) {
                    $arrowRight.addClass('show');
                } else {
                    $arrowRight.removeClass('show');
                }
            };
            if (a._selectionItems && a._selectionItemDrawer) {
                var list = new CListControl(null);
                list.setDataProvider(new CDataProvider(a._selectionItems));
                list.setDataDrawer(a._selectionItemDrawer);
                list.setDrawParam({
                    itemWidth: a._selectionItemWidth,
                    align: 'horizontal'
                });
                if (!a._textHtml) {
                    list.addEventHandler('FocusChanged', a, function ($old, $new) {
                        updateArrow();
                    });
                }
                list.onItemSelected = function (param) {
                    a.onSelectionChanged(param);
                };
            }
            var view = null;
            if (a._textHtml) {
                var verticalLayout = new CVerticalLayoutControl(null);
                verticalLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.removeClass('-f');
                        $item.html(a._textHtml);
                        return false;
                    }]);
                view = new CVerticalGroupControl(null);
                view.addClass('selectionDlg-view');
                view.setChildControl([verticalLayout, list], 1);
            }
            else {
                view = new CViewGroupControl(null);
                view.addClass('selectionDlg-view');
                var width = 0;
                if (a._selectionItems) {
                    width = Math.min(a._selectionItemWidth * a._selectionItems.length, a._selectionItemWidth * 4);
                }
                view._$self.css('width', width + 'px');
                view.setChildControl([list]);
                setTimeout(function () {
                    updateArrow();
                }, 1);
            }
            a.setChildControl([view]);
            a.setDrawParam({
                $parent: a._parent.$layer(),
                align: 'center',
                childAlign: 'center'
            });
            a._$self.append($arrowLeft).append($arrowRight);
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.CSelectionDlg = CSelectionDlg;
    prism.createProfileSelectionDlg = function (layeredGroupControl) {
        var selectionDlg = new CSelectionDlg(layeredGroupControl);
        selectionDlg.setSelectionItemWidth(184);
        selectionDlg.setSelectionItemDrawer(prism.drawers.profile);
        return selectionDlg;
    };
    prism.createStorageSelectionDlg = function (layeredGroupControl) {
        var selectionDlg = new CSelectionDlg(layeredGroupControl);
        selectionDlg.setId('storageSelectionDlg');
        selectionDlg.setSelectionItemWidth(174);
        selectionDlg.setSelectionItemDrawer(prism.drawers.storage);
        return selectionDlg;
    };
    prism.createStorageSelectionDlg2 = function (layeredGroupControl) {
        var selectionDlg = new CSelectionDlg(layeredGroupControl);
        selectionDlg.setId('storageSelectionDlg');
        selectionDlg.setSelectionItemWidth(174);
        selectionDlg.setSelectionItemDrawer(prism.drawers.storage2);
        return selectionDlg;
    };
    prism.createUsbWizardSelectionDlg = function (layeredGroupControl, info, callback) {
        var selectionDlg;
        if (layeredGroupControl) {
            selectionDlg = new CSelectionDlg(layeredGroupControl);
            selectionDlg.addClass('usbWizard');
            selectionDlg.setId('storageSelectionDlg');
            selectionDlg.setSelectionIndex(0);
            selectionDlg.setSelectionItemWidth(174);
            selectionDlg.setSelectionItemDrawer(prism.drawers.usbWizard);
            selectionDlg.setTextHtml(info ? info.title : '');
            selectionDlg.setTimeout(30 * 1000);
            selectionDlg.setSelectionItems([
                {
                    name: info ? info.video : '',
                    cssClass: '-video',
                    pageName: 'CPgVideo'
                },
                {
                    name: info ? info.music : '',
                    cssClass: '-music',
                    pageName: 'CPgMusic'
                },
                {
                    name: info ? info.photo : '',
                    cssClass: '-photo',
                    pageName: 'CPgPhoto'
                }
            ]);
            selectionDlg.setDrawParam({
                align: 'hcenter'
            });
            selectionDlg.onSelectionChanged = function (param) {
                var item = param.item;
                var pageName = item.pageName;
                callback(pageName);
            };
        } else {
            selectionDlg = prism.createGlobalDialog('createUsbWizardSelectionDlg', info, callback);
        }
        return selectionDlg;
    };
}
export = mPrism;
