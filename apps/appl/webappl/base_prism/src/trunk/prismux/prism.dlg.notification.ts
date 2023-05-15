/// <reference path="../def/prism.d.ts" />
/// <reference path="../def/sugar.d.ts" />
// file name : prismux/prism.dlg.notification.js
/*jslint sloppy:true, nomen:true, vars:true */

import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');

declare var hx: ifSugar;

module mPrism {
    export class CNotificationDlg extends __PRISM_CDIALOG__.CDialog {
        private _icon: boolean;
        private _textHtml: string;

        constructor(layeredGroupControl) {
            super(layeredGroupControl);
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl);
            a.addClass('notificationDlg');
        }
        setIcon() {
            var a = this;
            a._icon = true;
        }
        setTextHtml(textHtml) {
            var a = this;
            a._textHtml = textHtml;
        }
        doOpen(param?) {
            var a = this;
            var horizontalLayout = new CHorizontalLayoutControl(null);
            var drawers = [];
            if (a._icon) {
                drawers.push(function(index, $item) {
                    $item.css({
                        'width:' : '60px',
                        'height' : '1px'
                    });
                    $item.append($('<div>', {
                        'class' : 'icon'
                    }));
                });
            }
            drawers.push(function(index, $item) {
                $item.addClass('text');
                $item.html(a._textHtml);
            });
            horizontalLayout.setItemDrawers(drawers);
            horizontalLayout.setDrawParam({
                padding : 25
            });
            a.setChildControl([horizontalLayout]);
            a.setDrawParam({
                $parent : a._parent.$layer(),
                align : 'hcenter',
                childAlign : 'hcenter',
                padding : 40,
                margins : [50, 120, 33, 95],
                transition : {
                    newLayer : 'moveDown'
                }
            });
        }
        doKey(aKeyCode?) {
            if (aKeyCode === hx.key.KEY_ESCAPE || aKeyCode === hx.key.KEY_BACK) {
                return false;
            }
            return true;
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.CNotificationDlg = CNotificationDlg;
    prism.createNotificationDlg = function(layeredGroupControl, htmlText, buttonDlgInfo) {
        if (layeredGroupControl) {
            var notifDlg = new CNotificationDlg(layeredGroupControl);
            notifDlg.addClass('notification');
            if (htmlText) {
                notifDlg.setTextHtml(htmlText);
            }
            notifDlg.setTimeout(3 * 1000);
            return notifDlg;
        } else {
            var globalDlg = prism.createGlobalDialog('createNotificationDlg', htmlText, buttonDlgInfo);
            return globalDlg;
        }
    };
    prism.createWaitingNotificationDlg = function(layeredGroupControl) {
        var buttonDlg = new CNotificationDlg(layeredGroupControl);
        buttonDlg.setIcon();
        buttonDlg.addClass('waiting');

        if($('#tvg')){
            buttonDlg.addClass('onTvg');
        }

        return buttonDlg;
    };
}
export = mPrism;
