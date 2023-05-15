/// <reference path="def/builtinapp.d.ts" />
/// <reference path="../def/require.d.ts" />
/// <reference path="../def/sugar.d.ts" />
/// <reference path="../def/config.d.ts" />

/**
 * Created by bglee@humaxdigital.com on 2014. 3. 25..
 */

declare var __REQUEST_INFO__: ifRequestBuiltinAppsInfo;
declare var notifyScriptLoaded: () => void;
import __ENV__ = require("../env/env");

var log = function(msg) {
    if (window.debug) {
        window.debug(__REQUEST_INFO__.ansi + " BUILT_IN_APP : "
            + "\033[0m| "
            + msg);
    }
    console.log(msg);
};

function main(aENV: ifProductConfiguration) {
    log("__BUILTIN_APPS_MAIN__");
    var prefixForRequireJS = "";
    function _fnLoad(aDoesNotUse: (...aModules: any[]) => void) {
        log("[__builtin_apps_main__] : script ext libs = " + scripts.join(", "));
        require(scripts.splice(0, 1), scripts.length ? _fnLoad: callback);
    }
    function fnLoadExtLibs(aCallback: (...aModules: any[]) => void) {
        //TODO: check to need sync
        scripts = aENV.getExtLibs(prefixForRequireJS);
        log("[__builtin_apps_main__] : script ext libs = " + scripts.join(', '));
        require(scripts, aCallback);
    }
    function fnLoadUtilities(aCallback: (...aModules: any[]) => void) {
        scripts = aENV.getUtility();
        console.log("[main] : scripts = " + scripts.join(", "));
        require(scripts, aCallback);
    }
    function fnLoadKeyTable(aCallback: (...aModules: any[]) => void) {
        scripts = aENV.getKeyTable();
        log("[__builtin_apps_main__] : scripts = " + scripts.join(", "));
        require(scripts, aCallback);
    }
    function fnLoadPrismUIFramework(aCallback: (...aModules: any[]) => void) {
        scripts = aENV.getPrismUIFramework();
        log("[__builtin_apps_main__] : scripts = " + scripts.join(", "));
        require(scripts, aCallback);
    }
    function fnEach(arr, fn) {
        var i, len;
        for ( i = 0, len = arr.length; i < len ; i += 1 ) {
            fn(arr[i]);
        }
    }
    var scripts: string[] = [];
    var callback: (...aModules: any[]) => void = null;

    fnLoadExtLibs(() => {
        fnLoadKeyTable((__KEY_TABLE__, __PRODUCT_KEY_TABLE__) => {
            $.extend(__KEY_TABLE__, __PRODUCT_KEY_TABLE__);
            var hx: any = {
                key: __KEY_TABLE__,
                emulMode: aENV.isEmul(),
                logadd: function() {},
                logremove: function() {},
                config: aENV.getConfig(),
            };
            window.hx = hx;
            fnLoadUtilities((__UTILS__, __UI_COMPONENT__, __LANGUAGE_TRANSLATOR__) => {
                hx.il = __UTILS__;
                hx.ui = __UI_COMPONENT__;
                hx.l = new __LANGUAGE_TRANSLATOR__(aENV.getLanguageTable());
                fnLoadPrismUIFramework(() => {
                    scripts = __REQUEST_INFO__.deps;
                    log("[__builtin_apps_main__] : scripts = " + scripts.join(", "));
                    callback = () => {
                        log("[__builtin_apps_main__] framework loaded");
                        require(scripts, () => {
                            log("[__builtin_apps_main__] deps loaded");
                            scripts = __REQUEST_INFO__.url;
                            log("[__builtin_apps_main__] : scripts = " + scripts.join(", "));

                            require(scripts, () => {
                                log("[__builtin_apps_main__] " + "ALL loaded");
                                log("[__builtin_apps_main__] " + __REQUEST_INFO__.url.join(', ') + " loaded");
                                (function(){
                                    if ( !!window.fnGetPreReceivedMessages) {
                                        fnEach(window.fnGetPreReceivedMessages(), window._mainMessageHandler);
                                    }
                                }());
                            });
                        });
                    };
                    _fnLoad(callback);
                });
            });
        });
    });
}

__ENV__.onload(function(aENV: ifProductConfiguration) {
    main(aENV);
});
