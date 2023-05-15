/// <reference path="../def/prism.d.ts" />
/// <reference path="../def/sugar.d.ts" />
// file name : prismux/prism.dlg.notification.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'prismux/prism.dlg'], function (require, exports, __PRISM_CDIALOG__) {
    var mPrism;
    (function (mPrism) {
        var CNotificationDlg = (function (_super) {
            __extends(CNotificationDlg, _super);
            function CNotificationDlg(layeredGroupControl) {
                _super.call(this, layeredGroupControl);
            }
            CNotificationDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('notificationDlg');
            };
            CNotificationDlg.prototype.setIcon = function () {
                var a = this;
                a._icon = true;
            };
            CNotificationDlg.prototype.setTextHtml = function (textHtml) {
                var a = this;
                a._textHtml = textHtml;
            };
            CNotificationDlg.prototype.doOpen = function (param) {
                var a = this;
                var horizontalLayout = new CHorizontalLayoutControl(null);
                var drawers = [];
                if (a._icon) {
                    drawers.push(function (index, $item) {
                        $item.css({
                            'width:': '60px',
                            'height': '1px'
                        });
                        $item.append($('<div>', {
                            'class': 'icon'
                        }));
                    });
                }
                drawers.push(function (index, $item) {
                    $item.addClass('text');
                    $item.html(a._textHtml);
                });
                horizontalLayout.setItemDrawers(drawers);
                horizontalLayout.setDrawParam({
                    padding: 25
                });
                a.setChildControl([horizontalLayout]);
                a.setDrawParam({
                    $parent: a._parent.$layer(),
                    align: 'hcenter',
                    childAlign: 'hcenter',
                    padding: 40,
                    margins: [50, 120, 33, 95],
                    transition: {
                        newLayer: 'moveDown'
                    }
                });
            };
            CNotificationDlg.prototype.doKey = function (aKeyCode) {
                if (aKeyCode === hx.key.KEY_ESCAPE || aKeyCode === hx.key.KEY_BACK) {
                    return false;
                }
                return true;
            };
            return CNotificationDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CNotificationDlg = CNotificationDlg;
        if (!window.prism) {
            window.prism = {};
        }
        prism.CNotificationDlg = CNotificationDlg;
        prism.createNotificationDlg = function (layeredGroupControl, htmlText, buttonDlgInfo) {
            if (layeredGroupControl) {
                var notifDlg = new CNotificationDlg(layeredGroupControl);
                notifDlg.addClass('notification');
                if (htmlText) {
                    notifDlg.setTextHtml(htmlText);
                }
                notifDlg.setTimeout(3 * 1000);
                return notifDlg;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createNotificationDlg', htmlText, buttonDlgInfo);
                return globalDlg;
            }
        };
        prism.createWaitingNotificationDlg = function (layeredGroupControl) {
            var buttonDlg = new CNotificationDlg(layeredGroupControl);
            buttonDlg.setIcon();
            buttonDlg.addClass('waiting');
            if ($('#tvg')) {
                buttonDlg.addClass('onTvg');
            }
            return buttonDlg;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
