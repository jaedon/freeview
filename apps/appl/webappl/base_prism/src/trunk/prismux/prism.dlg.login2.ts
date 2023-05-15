/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.dlg.login2.js
/*jslint sloppy:true, nomen:true, vars:true */

import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

module mPrism {
    export class CLoginDlg extends __PRISM_CDIALOG__.CDialog {
        _$title: JQuery;
        _$inputUser: JQuery;
        _$inputPassword: JQuery;
        _buttonInfoList: any[];

        constructor(layeredGroupControl) {
            super(layeredGroupControl);
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl);
            a.addClass('loginDlg');
        }
        setTitleHtml(titleHtml) {
            var a = this;
            if (!a._$title) {
                var $title = $('<div>', {
                    'class' : 'text'
                });
                a._$title = $title;
            }
            a._$title.html(titleHtml);
        }
        setButtonInfoList(buttonInfoList) {
            var a = this;
            a._buttonInfoList = buttonInfoList;
        }
        doOpen(param?) {
            var a = this;
            var i, length;
            var initialFocus = 0;
            var childControl = [];
            var drawers = [];
            var buttonDrawer = function(index, $item) {
                $item.addClass('btn').html(a._buttonInfoList[index].text);
            };
            var verticalDrawers = [
                function(index, $item) {
                    $item.append(a._$title);
                    $item.removeClass('-f');
                },
                function(index, $item) {
                    var $row = $('<div>', {
                        'class' : 'row'
                    });
                    var $label = $('<div>', {
                        'class' : 'label'
                    });
                    $label.text(tr('LOC_STR_USER_ID'));
                    var $input = $('<input>', {
                        'type' : 'text'
                    });
                    a._$inputUser = $input;
                    $row.append($label).append($input).appendTo($item);
                },
                function(index, $item) {
                    var $row = $('<div>', {
                        'class' : 'row'
                    });
                    var $label = $('<div>', {
                        'class' : 'label'
                    });
                    $label.text(tr('LOC_STR_PASSWORD_ID'));
                    var $input = $('<input>', {
                        'type' : 'password'
                    });
                    a._$inputPassword = $input;
                    $row.append($label).append($input).appendTo($item);
                }
            ];
            var verticalLayout = new CVerticalLayoutControl(null);
            verticalLayout.setDrawParam({
                padding : 15
            });
            verticalLayout.onItemSelected = function(index, $item) {
                var $input = $item.find('input');
                $input.focus();
                $input.focus();
            };
            verticalLayout.setItemDrawers(verticalDrawers);
            childControl.push(verticalLayout);
            if (a._buttonInfoList) {
                length = a._buttonInfoList.length;
                for ( i = 0; i < length; i += 1) {
                    drawers.push(buttonDrawer);
                }
            }
            var horizontalLayout = new CHorizontalLayoutControl(null);
            horizontalLayout.addClass('buttonArea');
            horizontalLayout.setDrawParam({
                padding : 15,
                align : 'hcenter'
            });
            horizontalLayout.setItemDrawers(drawers);
            horizontalLayout.onItemSelected = function(index, $item) {
                var fn = null;
                if (a._buttonInfoList) {
                    fn = a._buttonInfoList[index].fn;
                }
                if (fn) {
                    setTimeout(function() {
                        fn(a._$inputUser.val(), a._$inputPassword.val());
                    }, 1);
                }
            };
            childControl.push(horizontalLayout);
            a.setChildControl(childControl, initialFocus);
            a.setDrawParam({
                align : 'center',
                childAlign : 'hcenter',
                padding : 33,
                margins : [33, 50, 30]
            });
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createLoginDlg = function(layeredGroupControl) {
        var loginDlg = new CLoginDlg(layeredGroupControl);
        return loginDlg;
    };
}
export = mPrism;
