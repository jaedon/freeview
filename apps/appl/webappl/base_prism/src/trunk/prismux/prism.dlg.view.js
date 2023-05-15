var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var CViewDlg = (function (_super) {
            __extends(CViewDlg, _super);
            function CViewDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CViewDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('viewDlg');
            };
            CViewDlg.prototype.destroy = function () {
                var a = this;
                a._buttonInfoList = null;
                _super.prototype.destroy.call(a);
            };
            CViewDlg.prototype.setTitleHtml = function (titleHtml) {
                var a = this;
                a._titleHtml = titleHtml;
            };
            CViewDlg.prototype.setButtonInfo = function (buttonInfo) {
                var a = this;
                a._buttonInfo = buttonInfo;
            };
            CViewDlg.prototype.setListInfoDrawer = function (listInfoDrawer) {
                var a = this;
                a._listInfoDrawer = listInfoDrawer;
            };
            CViewDlg.prototype.setDataProvider = function (dataProvider) {
                var a = this;
                a._dataProvider = dataProvider;
            };
            CViewDlg.prototype.setDataControlDrawParam = function (controlType, dataControlDrawParam) {
                var a = this;
                a._controlType = controlType;
                a._dataControlDrawParam = dataControlDrawParam;
            };
            CViewDlg.prototype.setDataDrawer = function (dataDrawer) {
                var a = this;
                a._dataDrawer = dataDrawer;
            };
            CViewDlg.prototype.getView = function () {
                var a = this;
                return a._view;
            };
            CViewDlg.prototype.getDataControl = function () {
                var a = this;
                return a._dataControl;
            };
            CViewDlg.prototype.setOptionMenuTarget = function (page) {
                var a = this;
                return (a._page = page);
            };
            CViewDlg.prototype.updateListInfo = function () {
                var a = this;
                if (a._listInfoDrawer) {
                    a._listInfoDrawer(a._$listInfoArea);
                }
            };
            CViewDlg.prototype.doOpen = function (param) {
                var a = this;
                a._parent.$layer().addClass('viewDlgLayer');
                var titleAreaDrawers = [
                    function (index, $item) {
                        $item.addClass('viewDlgTitle');
                        $item.removeClass('-f');
                        $item.html(a._titleHtml);
                        //                    return false;
                    }
                ];
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
                    }
                    else {
                        var focusableItemLength = null;
                        if (param.focusableItemLength) {
                            focusableItemLength = param.focusableItemLength;
                        }
                        if (param.drawParam) {
                            param.drawParam.focusableItemLength = focusableItemLength;
                            dataControl.setDrawParam(param.drawParam);
                        }
                        else {
                            dataControl.setDrawParam({
                                maxColCount: 3,
                                itemWidth: 295,
                                itemHeight: 232,
                                scrollByItemSize: true,
                                focusableItemLength: focusableItemLength
                            });
                        }
                    }
                }
                else {
                    dataControl = new CListControl(null);
                    if (a._dataControlDrawParam) {
                        dataControl.setDrawParam(a._dataControlDrawParam);
                    }
                    else {
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
                });
                a._dataControl = dataControl;
            };
            return CViewDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CViewDlg = CViewDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.CViewDlg = CViewDlg;
        prism.createViewDlg = function (layeredGroupControl) {
            var viewDlg = new CViewDlg(layeredGroupControl);
            return viewDlg;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
