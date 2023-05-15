/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.rename.js
/*jslint sloppy:true, nomen:true, vars:true */
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    export class CRenameDlg extends __PRISM_CDIALOG__.CDialog {
        _$title: JQuery;
        _$label: JQuery;
        _$input: JQuery;
        _buttonInfoList: any[];

        constructor(layeredGroupControl) {
            super(layeredGroupControl);
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl);
            a.addClass('renameDlg');
        }
        setTitleHtml(html) {
            var a = this;
            if (!a._$title) {
                var $title = $('<div>', {
                    'class': 'text'
                });
                a._$title = $title;
            }
            a._$title.html(html);
        }
        setLabelHtml(html) {
            var a = this;
            if (!a._$label) {
                var $label = $('<div>', {
                    'class': 'label'
                });
                a._$label = $label;
            }
            a._$label.html(html);
        }
        setButtonInfoList(buttonInfoList) {
            var a = this;
            a._buttonInfoList = buttonInfoList;
        }
        doOpen(param?) {
            var a = this;
            var i, length;
            var initialFocus = 1;
            var childControl = [];
            var buttonDrawers = [];
            var titleDrawers = [
                function (index, $item) {
                    $item.append(a._$title);
                    $item.removeClass('-f');
                }];
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
                }];
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
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createStorageRenameDlg = function (layeredGroupControl) {
        var renameDlg = new CRenameDlg(layeredGroupControl);
        renameDlg.setTitleHtml(tr('LOC_ENTER_STORAGE_NAME_ID'));
        renameDlg.setLabelHtml(tr('LOC_STORAGE_NAME_ID'));
        return renameDlg;
    };
}
export = mPrism;