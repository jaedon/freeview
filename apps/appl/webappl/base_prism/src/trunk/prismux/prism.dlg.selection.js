var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var mOpenCount = 0;
        var CSelectionDlg = (function (_super) {
            __extends(CSelectionDlg, _super);
            function CSelectionDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CSelectionDlg.isOpened = function () {
                if (mOpenCount < 0)
                    throw "check init & destroy routine";
                return mOpenCount ? true : false;
            };
            CSelectionDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('selectionDlg');
                mOpenCount += 1;
            };
            CSelectionDlg.prototype.destroy = function () {
                var a = this;
                _super.prototype.destroy.call(a);
                mOpenCount -= 1;
            };
            CSelectionDlg.prototype.setSelectionItems = function (selectionItems) {
                var a = this;
                a._selectionItems = selectionItems;
            };
            CSelectionDlg.prototype.setSelectionIndex = function (selectionIndex) {
                var a = this;
                a._selectionIndex = selectionIndex;
            };
            CSelectionDlg.prototype.setSelectionItemWidth = function (selectionItemWidth) {
                var a = this;
                a._selectionItemWidth = selectionItemWidth;
            };
            CSelectionDlg.prototype.setSelectionItemDrawer = function (selectionItemDrawer) {
                var a = this;
                a._selectionItemDrawer = selectionItemDrawer;
            };
            CSelectionDlg.prototype.setTextHtml = function (textHtml) {
                var a = this;
                a._textHtml = textHtml;
            };
            CSelectionDlg.prototype.doOpen = function (param) {
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
                    }
                    else {
                        $arrowLeft.removeClass('show');
                    }
                    if (list.getNextAvail('right').left !== undefined) {
                        $arrowRight.addClass('show');
                    }
                    else {
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
                        }
                    ]);
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
            };
            return CSelectionDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CSelectionDlg = CSelectionDlg;
        if (!window.prism) {
            window.prism = {};
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
            }
            else {
                selectionDlg = prism.createGlobalDialog('createUsbWizardSelectionDlg', info, callback);
            }
            return selectionDlg;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
