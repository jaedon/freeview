/// <reference path="../def/builtinapp.d.ts" />
// file name : builtin_apps/home_menu/home_menu_new.js
/*jslint sloppy:true, nomen:true, vars:true */
/*
 * Prism UX glue
 */
(function ($) {
    $.extend({
        imgPreload: function (options) {
            var loadcounter = 0, i, settings = $.extend({
                images: null,
                onLoad: null,
                onLoadAll: null
            }, options);
            for (i = 0; i < settings.images.length; i += 1) {
                var img = $('<img/>').addClass('preloading').css('display', 'none').attr('src', settings.images[i]).load(onloadEvent);
            }
            function onloadEvent() {
                loadcounter += 1;
                if (settings.onLoad !== null) {
                    settings.onLoad($(this));
                }
                if (loadcounter === settings.images.length) {
                    if (settings.onLoadAll !== null) {
                        settings.onLoadAll();
                    }
                }
            }
        }
    });
}(jQuery));
var layeredControl = new CLayeredGroupControl($('body'));
var hubDlg = null;
hubDlg = prism.createHubDlg(layeredControl);
var fnHide = function (cb) {
    hubDlg.hide(function () {
        notify('notifyHidden');
        if (cb) {
            cb();
        }
    });
};
var fnShow = function (cb) {
    hubDlg.show(function () {
        notify('notifyShown');
        if (cb) {
            cb();
        }
    });
};
var fnRequestStandby = function () {
    fnHide();
    notify('notifyRequestStandby');
};
var fnLaunchNetflix = function () {
    fnHide();
    notify('notifyLaunchNetflix');
};
hubDlg.onHomeMenuSelected = function (item, path) {
    fnHide(function (x) {
        return function () {
            notify('notifyHomeMenuSelected', x, path);
        };
    }(item));
};
hubDlg.onHomeItemSelected = function (item, type) {
    fnHide();
    notify('notifyHomeItemSelected', item, type);
};
hubDlg.onStatisticsKeyDown = function (keycode) {
    notify('notifyStatisticsKeyDown', keycode);
};
hubDlg.onRequestUserAppList = function () {
    notify('notifyRequestUserAppList');
};
hubDlg.open();
/*
 * Key handler
 */
$('body').keydown(function (e) {
    hubDlg.onStatisticsKeyDown(e.which);
    if (hubDlg && layeredControl.doKey(e.which)) {
    }
    else if ((e.which == hx.key.KEY_BACK) || (e.which == hx.key.KEY_ESCAPE) || (e.which == hx.key.KEY_MENU)) {
        fnHide();
    }
    else if (e.which == hx.key.KEY_POWER) {
        fnRequestStandby();
    }
    else if (e.which == hx.key.KEY_GUIDE) {
        hubDlg.onHomeMenuSelected("tvguide");
    }
    else if (e.which == hx.key.KEY_PLAYER) {
        hubDlg.onHomeMenuSelected("recordings");
    }
    else if (e.which == hx.key.KEY_ON_DEMAND) {
        hubDlg.onHomeMenuSelected("toppicks", "/ondemand");
    }
    else if (e.which == hx.key.KEY_NETFLIX) {
        fnLaunchNetflix();
    }
    else {
        console.log(e.which);
        if (hx.emulMode) {
            log(e.which);
            log('prevent');
        }
        else {
            notify('notifyUnHandledKeyDown', e.which);
        }
    }
});
/*
 * APIs
 */
defineAPI("show", function () {
    fnShow();
});
defineAPI("hide", function () {
    fnHide();
});
defineAPI("setData", function (param) {
    hubDlg.setData(param);
});
defineAPI("setLocalizationText", function (param) {
    window.tr = function (locStr) {
        var ret = param[locStr];
        return ret ? ret : locStr;
    };
});
defineAPI("setLanguage", function (param) {
    hubDlg.setLanguage(param);
});
defineAPI("setHomeData", function (appList, type) {
    hubDlg.setHomeData(appList, type);
});
defineAPI("setHideAppList", function (hideList) {
    hubDlg.setHideAppData(hideList);
});
defineAPI("setTermsAndConditions", function (isAccept) {
    if (isAccept) {
        hubDlg._$self.addClass('termsAndConditions');
    }
    else {
        hubDlg._$self.removeClass('termsAndConditions');
    }
});
defineNotifyAPI("notifyShown");
defineNotifyAPI("notifyHidden");
defineNotifyAPI("notifyHomeMenuSelected");
defineNotifyAPI("notifyHomeItemSelected");
defineNotifyAPI("notifyStatisticsKeyDown");
defineNotifyAPI("notifyRequestUserAppList");
defineNotifyAPI("notifyRequestStandby");
defineNotifyAPI("notifyLaunchNetflix");
