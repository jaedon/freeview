/// <reference path="../def/builtinapp.d.ts" />
// file name : builtin_apps/home_menu/home_menu.js
/*jslint sloppy:true, nomen:true, vars:true */
define(["require", "exports", "../../lib/local/key_catcher"], function (require, exports, __KEY_CATCHER__) {
    prism.CDialog.showMsgbox = function (finished, param) {
        notify('notifyCreateGlobalDlg');
        finished();
    };
    var layeredControl = new CLayeredGroupControl($('body'));
    layeredControl.createLayer({});
    var dlg = null;
    var stopPropagation = function (e) {
        if (e.stopPropagation) {
            e.stopPropagation();
        }
        if (!e.cancelBubble) {
            e.cancelBubble = true;
        }
    };
    var __hook = (function () {
        var keyCatcher = new __KEY_CATCHER__(window);
        function fnHook(aKeyCode, aRepeat) {
            return keyCatcher.input(aKeyCode, aRepeat);
        }
        keyCatcher.registerKeyCode([3, 6, 9, 0, 0, 1, 2], "exit child key grap", function () {
            log("exit!");
            log("exit!");
            log("exit!");
            if (dlg) {
                log("dlg fire event Close");
                dlg.fireEvent("Close");
            }
            else {
                log("dlg does not exist");
            }
        });
        return fnHook;
    })();
    document.body.addEventListener('keydown', function (e) {
        log(e.which);
        notify('notifyStatisticsKeyDown', e.which);
        if (!__hook(e.which)) {
            if (dlg && e.which == hx.key.KEY_ESCAPE) {
                if (!dlg.passiveClose) {
                    notify('notifyClosing');
                    while (layeredControl.stackSize()) {
                        layeredControl.removeLayer();
                    }
                }
                stopPropagation(e);
            }
            else if (dlg && e.which == hx.key.KEY_BACK) {
                if (!dlg.passiveClose) {
                    var handled = layeredControl.doKey(e.which);
                    if (!handled) {
                        notify('notifyClosing');
                        dlg.close();
                    }
                }
                stopPropagation(e);
            }
            else if (layeredControl && layeredControl.doKey(e.which)) {
                stopPropagation(e);
            }
            else {
                log(e.which);
            }
        }
        if (e.preventDefault) {
            e.preventDefault();
        }
    });
    defineAPI("create", function () {
        var args = $.makeArray(arguments);
        var createFuncName = args.shift();
        args.unshift(layeredControl);
        dlg = prism[createFuncName].apply(window, args);
        dlg.addEventHandler('Close', this, function (param, index) {
            notify('notifyClosed', param, index);
            if (layeredControl._layerStack.length <= 1) {
                notify('notifyHidden');
            }
        });
        dlg.addEventHandler('Closing', this, function (param, index) {
            notify('notifyClosing', param, index);
        });
    });
    defineAPI("show", function (param) {
    });
    defineAPI("open", function (param) {
        param = param || {};
        if (dlg) {
            dlg.passiveClose = param.passiveClose;
            dlg.open(param);
        }
    });
    defineAPI("close", function () {
        log('close invoked');
        if (dlg) {
            dlg.close();
        }
    });
    defineAPI("setData", function (param) {
        param = param || [];
        if (dlg) {
            dlg.setData(param);
        }
    });
    defineAPI("doKey", function (param) {
        if (param.fn && dlg) {
            dlg.doKey = param.fn;
        }
    });
    defineAPI("hide", function () {
        notify('notifyHidden');
    });
    defineAPI("setLocalizationText", function (param) {
        tr = function (locStr) {
            var ret = param[locStr];
            return ret ? ret : locStr;
        };
    });
    defineAPI("setStatusItem", function (param) {
        if (dlg) {
            if (param.type === 'signal') {
                dlg._$self.find('.statusItem .statusBar').eq(0).css('width', param.strength + '%');
                dlg._$self.find('.statusItem .statusBar').eq(1).css('width', param.quality + '%');
            }
            else if (param.type === 'inputnumber') {
                dlg.handleSettingItemSelection(dlg, param);
            }
        }
    });
    defineNotifyAPI("notifyHidden");
    defineNotifyAPI("notifyClosed");
    defineNotifyAPI("notifyClosing");
    defineNotifyAPI("notifyButtonSelected");
    defineNotifyAPI("notifyItemSelected");
    defineNotifyAPI("notifyCreateGlobalDlg");
    defineNotifyAPI("notifyStatisticsKeyDown");
});
