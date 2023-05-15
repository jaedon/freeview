/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.button.js
/*jslint sloppy:true, nomen:true, vars:true */
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    export class CViewDlg extends __PRISM_CDIALOG__.CDialog {
        private _titleHtml: string;
        private _listInfoDrawer: Function;
        private _dataProvider: any;
        private _controlType: any;
        private _dataControlDrawParam: any;
        private _dataDrawer: Function
        private _view: any;
        private _dataControl: __CONTROLS__.CDataControl;
        private _$listInfoArea: JQuery;
        private _page: any;
        private _buttonInfo: any;

        onDataItemSelected: (aParam) => void;
        constructor(layeredGroupControl) {
            super(layeredGroupControl)
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl)
            a.addClass('viewDlg');
        }
        destroy() {
            var a = this;
            a._buttonInfoList = null;
            super.destroy.call(a);
        }
        setTitleHtml(titleHtml) {
            var a = this;
            a._titleHtml = titleHtml;
        }
        setButtonInfo(buttonInfo) {
            var a = this;
            a._buttonInfo = buttonInfo;
        }
        setListInfoDrawer(listInfoDrawer) {
            var a = this;
            a._listInfoDrawer = listInfoDrawer;
        }
        setDataProvider(dataProvider) {
            var a = this;
            a._dataProvider = dataProvider;
        }
        setDataControlDrawParam(controlType, dataControlDrawParam) {
            var a = this;
            a._controlType = controlType;
            a._dataControlDrawParam = dataControlDrawParam;
        }
        setDataDrawer(dataDrawer) {
            var a = this;
            a._dataDrawer = dataDrawer;
        }
        getView() {
            var a = this;
            return a._view;
        }
        getDataControl() {
            var a = this;
            return a._dataControl;
        }
        setOptionMenuTarget(page) {
            var a = this;
            return (a._page = page);
        }
        updateListInfo() {
            var a = this;
            if (a._listInfoDrawer) {
                a._listInfoDrawer(a._$listInfoArea);
            }
        }
        doOpen(param?) {
            var a = this;
            a._parent.$layer().addClass('viewDlgLayer');
            var titleAreaDrawers = [
                function (index, $item) {
                    $item.addClass('viewDlgTitle');
                    $item.removeClass('-f');
                    $item.html(a._titleHtml);
//                    return false;
                }];
            if (a._listInfoDrawer) {
                titleAreaDrawers.push(function (index, $item) {
                    $item.addClass('viewDlgInfo');
                    $item.removeClass('-f');
                    a._$listInfoArea = $item;
                    a._listInfoDrawer($item);
                });
            }
            titleAreaDrawers.push(null);
            if (a._buttonInfo) {
                titleAreaDrawers.push(function (index, $item) {
                    $item.addClass('prism-button');
                    $item.html(a._buttonInfo[0].text);
                });
            }
            var horizontalLayout = new CHorizontalLayoutControl(null);
            horizontalLayout.addClass('viewDlgTitleArea');
            horizontalLayout.setItemDrawers(titleAreaDrawers);
            horizontalLayout.onItemSelected = function (index, $item) {
                a._buttonInfo[index - 3].fn();
            };
            var dataControl = null;
            if (a._controlType == 'grid') {
                dataControl = new CGridControl(null);
                if (a._dataControlDrawParam) {
                    dataControl.setDrawParam(a._dataControlDrawParam);
                } else {
                    var focusableItemLength = null;
                    if (param.focusableItemLength) {
                        focusableItemLength = param.focusableItemLength;
                    }
                    if (param.drawParam) {
                        param.drawParam.focusableItemLength = focusableItemLength;
                        dataControl.setDrawParam(param.drawParam);
                    } else {
                        dataControl.setDrawParam({
                            maxColCount: 3,
                            itemWidth: 295,
                            itemHeight: 232,
                            scrollByItemSize: true,
                            focusableItemLength : focusableItemLength
                        });
                    }
                }
            } else {
                dataControl = new CListControl(null);
                if (a._dataControlDrawParam) {
                    dataControl.setDrawParam(a._dataControlDrawParam);
                } else {
                    dataControl.setDrawParam({
                        itemHeight: 126,
                        scrollByItemSize: true
                    });
                }
                dataControl.addEventHandler('FocusChanged', dataControl, function ($old, $new) {
                    if (a._listInfoDrawer) {
                        a._listInfoDrawer(a._$listInfoArea);
                    }
                });
            }
            dataControl.setDataProvider(a._dataProvider);
            dataControl.setDataDrawer(a._dataDrawer);
            dataControl.onItemSelected = function (param) {
                a.onDataItemSelected(param);
            };
            if (a._page) {
                a._page.addOptionMenuTarget(dataControl);
            }
            var view = new CViewGroupControl(null);
            view.addClass('contentsView');
            view.setDrawParam({
                showScroll: true
            });
            view.setChildControl([dataControl]);
            a._view = view;
            a.setChildControl([horizontalLayout, view], a._dataProvider.getCount() ? 1 : 0);
            a.setDrawParam({
                align: 'hcenter',
                childAlign: 'hcenter',
                padding: 10
                // margins : [33, 50, 60]
            });
            a._dataControl = dataControl;
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.CViewDlg = CViewDlg;
    prism.createViewDlg = function (layeredGroupControl) {
        var viewDlg = new CViewDlg(layeredGroupControl);
        return viewDlg;
    };
}
export = mPrism;
