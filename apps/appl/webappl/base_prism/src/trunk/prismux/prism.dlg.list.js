var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'pages/util/controls', 'prismux/prism.dlg'], function (require, exports, __CONTROLS__, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var CListDlg = (function (_super) {
            __extends(CListDlg, _super);
            function CListDlg(layeredGroupControl) {
                _super.call(this);
                var a = this;
                if (arguments.length) {
                    a.init(layeredGroupControl);
                }
            }
            CListDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('listDlg');
            };
            CListDlg.prototype.setTitleHtml = function (html) {
                var a = this;
                if (!a._$title) {
                    var $title = $('<div>', {
                        'class': 'text'
                    });
                    a._$title = $title;
                }
                a._$title.html(html);
            };
            CListDlg.prototype.setOnItemSelected = function (fn) {
                var a = this;
                a._onItemSelected = fn;
            };
            CListDlg.prototype.doOpen = function (param) {
                var a = this;
                var initialFocus = 1;
                var childControl = [];
                var openParam = param;
                var titleDrawers = [
                    function (index, $item) {
                        $item.append(a._$title);
                        $item.removeClass('-f');
                    }
                ];
                a._curGroupIndex = openParam.curGroupIndex;
                var titleLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                titleLayout.setDrawParam({
                    padding: 15
                });
                titleLayout.setItemDrawers(titleDrawers);
                if (openParam.openClass) {
                    titleLayout.addClass(openParam.openClass);
                }
                childControl.push(titleLayout);
                var listViewControl = new __CONTROLS__.CViewGroupControl(null);
                listViewControl.addClass('dlgMenuBg');
                if (openParam.openClass) {
                    listViewControl.addClass(openParam.openClass);
                }
                var listControl = null;
                if (openParam.openClass === 'grid') {
                    listControl = new __CONTROLS__.CGridControl(null);
                }
                else {
                    listControl = new __CONTROLS__.CListControl(null);
                }
                listControl.addClass('dlgMenu');
                if (openParam.openClass) {
                    listControl.addClass(openParam.openClass);
                }
                listControl.setDataProvider(new __CONTROLS__.CDataProvider(a._groupItems));
                listControl.setDrawParam({
                    maxColCount: 2,
                    itemWidth: openParam.itemWidth || 471,
                    itemHeight: openParam.itemHeight || 53,
                    margin: [5, 10, 5, 10]
                });
                listControl.setDataDrawer(function (param) {
                    param.$item.addClass('dlgMenuItem');
                    if (openParam.openClass) {
                        param.$item.addClass(openParam.openClass);
                    }
                    else if (a._groupItems.length <= a._listDlgVisibleMaxLength) {
                        param.$item.addClass('noScroll');
                    }
                    param.$item.append('<span class="b">' + '&nbsp;' + '</span>' + '<span class="l">' + param.item.name + '</span>');
                    if (param.index === a._curGroupIndex) {
                        var $span = param.$item.find('span');
                        $span.addClass('selected');
                        a._$preSelectedItem = $span;
                    }
                });
                listControl.onItemSelected = function (param) {
                    var cbParam = { 'index': null, 'id': null };
                    var $span = param.$item.find('span');
                    cbParam.index = a._curGroupIndex = param.itemKey;
                    if (!($span.hasClass('selected'))) {
                        $span.addClass('selected');
                        if (a._$preSelectedItem) {
                            a._$preSelectedItem.removeClass('selected');
                        }
                        a._$preSelectedItem = $span;
                    }
                    if (param.item.id) {
                        cbParam.id = param.item.id;
                    }
                    if (a._onItemSelected) {
                        a._onItemSelected(cbParam);
                    }
                    a.close();
                };
                listViewControl.setChildControl([listControl]);
                listViewControl.setDrawParam({
                    margin: [5, 10, 5, 10],
                    align: 'left',
                    scrollByViewSize: true,
                    showScroll: true,
                });
                childControl.push(listViewControl);
                a.setChildControl(childControl, initialFocus);
                if (openParam.openClass === 'grid') {
                    a.setDrawParam({
                        align: 'hcenter',
                        childAlign: 'vcenter',
                        padding: 15,
                        margins: [20, 30, 15]
                    });
                }
                else {
                    a.setDrawParam({
                        align: 'hcenter',
                        childAlign: 'vcenter',
                        padding: 33,
                        margins: [34, 50, 30]
                    });
                }
            };
            CListDlg.prototype.doKey = function (keyCode) {
                var a = this;
                var consumed = _super.prototype.doKey.call(a, keyCode);
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
            CListDlg.prototype.doKeyOption = function () {
                return true;
            };
            CListDlg.prototype.doKeyBack = function () {
                this.close();
                return true;
            };
            return CListDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CListDlg = CListDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.createListDlg = function (layeredGroupControl, listDlgInfo, callback) {
            if (layeredGroupControl) {
                var listDlg = new CListDlg(layeredGroupControl);
                listDlg.setTitleHtml('Select a group to list the channels thereof.');
                listDlg._curGroupIndex = 0;
                //initial value
                listDlg._listDlgVisibleMaxLength = 5;
                if (listDlgInfo) {
                    if (listDlgInfo.textHtml) {
                        listDlg.setTitleHtml(listDlgInfo.textHtml);
                    }
                    if (listDlgInfo.groupItems) {
                        listDlg._groupItems = listDlgInfo.groupItems;
                    }
                    if (listDlgInfo.cls) {
                        listDlg.addClass(listDlgInfo.cls);
                    }
                    if (listDlgInfo.timeOutValue) {
                        listDlg.setTimeout(listDlgInfo.timeOutValue);
                    }
                }
                if (callback) {
                    listDlg._onItemSelected = callback;
                }
                else {
                    var itemSelectedHandler = function (param) {
                        notify('notifyItemSelected', {
                            type: "ItemSelected",
                            param: param
                        });
                    };
                    listDlg.setOnItemSelected(itemSelectedHandler);
                }
                return listDlg;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createListDlg', listDlgInfo, callback);
                return globalDlg;
            }
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
