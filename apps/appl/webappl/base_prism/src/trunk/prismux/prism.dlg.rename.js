var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var CRenameDlg = (function (_super) {
            __extends(CRenameDlg, _super);
            function CRenameDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CRenameDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('renameDlg');
            };
            CRenameDlg.prototype.setTitleHtml = function (html) {
                var a = this;
                if (!a._$title) {
                    var $title = $('<div>', {
                        'class': 'text'
                    });
                    a._$title = $title;
                }
                a._$title.html(html);
            };
            CRenameDlg.prototype.setLabelHtml = function (html) {
                var a = this;
                if (!a._$label) {
                    var $label = $('<div>', {
                        'class': 'label'
                    });
                    a._$label = $label;
                }
                a._$label.html(html);
            };
            CRenameDlg.prototype.setButtonInfoList = function (buttonInfoList) {
                var a = this;
                a._buttonInfoList = buttonInfoList;
            };
            CRenameDlg.prototype.doOpen = function (param) {
                var a = this;
                var i, length;
                var initialFocus = 1;
                var childControl = [];
                var buttonDrawers = [];
                var titleDrawers = [
                    function (index, $item) {
                        $item.append(a._$title);
                        $item.removeClass('-f');
                    }
                ];
                var inputDrawers = [
                    function (index, $item) {
                        $item.append(a._$label);
                        $item.removeClass('-f');
                    },
                    function (index, $item) {
                        var $input = $('<input>', {
                            'type': 'text'
                        });
                        a._$input = $input;
                        $item.append($input);
                    }
                ];
                var buttonDrawer = function (index, $item) {
                    $item.addClass('btn').html(a._buttonInfoList[index].text);
                };
                var titleLayout = new CVerticalLayoutControl(null);
                titleLayout.setDrawParam({
                    padding: 15
                });
                titleLayout.onItemSelected = function (index, $item) {
                    var $input = $item.find('input');
                    $input.focus();
                    $input.focus();
                };
                titleLayout.setItemDrawers(titleDrawers);
                childControl.push(titleLayout);
                var inputLayout = new CHorizontalLayoutControl(null);
                inputLayout.setDrawParam({
                    padding: 15,
                    align: 'hcenter'
                });
                inputLayout.setItemDrawers(inputDrawers);
                inputLayout.onItemSelected = function (index, $item) {
                    var $input = $item.find('input');
                    $item.append($input);
                    $input.focus();
                    $input.focus();
                };
                childControl.push(inputLayout);
                var buttonLayout = new CHorizontalLayoutControl(null);
                buttonLayout.addClass('buttonArea');
                buttonLayout.setDrawParam({
                    padding: 15,
                    align: 'hcenter'
                });
                if (a._buttonInfoList) {
                    length = a._buttonInfoList.length;
                    for (i = 0; i < length; i += 1) {
                        buttonDrawers.push(buttonDrawer);
                    }
                }
                buttonLayout.setItemDrawers(buttonDrawers);
                buttonLayout.onItemSelected = function (index, $item) {
                    var fn = null;
                    if (a._buttonInfoList) {
                        fn = a._buttonInfoList[index].fn;
                    }
                    if (fn) {
                        setTimeout(function () {
                            fn(a._$input.val());
                        }, 1);
                    }
                };
                childControl.push(buttonLayout);
                a.setChildControl(childControl, initialFocus);
                a.setDrawParam({
                    align: 'center',
                    childAlign: 'hcenter',
                    padding: 33,
                    margins: [33, 50, 30]
                });
            };
            return CRenameDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CRenameDlg = CRenameDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.createStorageRenameDlg = function (layeredGroupControl) {
            var renameDlg = new CRenameDlg(layeredGroupControl);
            renameDlg.setTitleHtml(tr('LOC_ENTER_STORAGE_NAME_ID'));
            renameDlg.setLabelHtml(tr('LOC_STORAGE_NAME_ID'));
            return renameDlg;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
