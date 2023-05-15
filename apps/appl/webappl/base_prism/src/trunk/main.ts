/// <reference path="def/require.d.ts" />
/// <reference path="def/sugar.d.ts" />
/// <reference path="def/config.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 3. 21..
 */

console.time("BOOT");
console.time("ENV");
import __ENV__ = require("env/env");

//TODO: consider to use require function that user can see the files name
function main(aENV: ifProductConfiguration) {
    console.log("__MAIN__");
    console.log("[main] : env loaded !");

    var scripts: string[] = [];
    var callback: (...aModules: any[]) => void = null;
    function _fnLoad(aDoesNotUse: (...aModules: any[]) => void) {
        require(scripts.splice(0, 1), scripts.length ? _fnLoad: callback);
    }
    function fnLoadExtLibs(aCallback: (...aModules: any[]) => void) {
        //TODO: check to need sync
        var scriptExtLibs: string[] = aENV.getExtLibs();
        console.log("[main] : script ext libs = " + scriptExtLibs.join(", "));
        require(scriptExtLibs, aCallback);
    }
    function fnLoadEmulator(aCallback: (...aModules: any[]) => void) {
        function _fnLoadEmulationData(aCallback: (...aModules: any[]) => void) {
            scripts = aENV.getEmulationData();
            callback = aCallback;
            console.log("[main] : scripts = " + scripts.join(", "));
            require(scripts, aCallback);
        }

        scripts = aENV.getEmulator();
        callback = () => {
            _fnLoadEmulationData(aCallback);
        };
        console.log("[main] : scripts = " + scripts.join(", "));

        _fnLoad(aCallback);
    }
    function fnMakeOIPFObject(): any {
        //XXX: HMXConnect.js
        var ret: any = {};

        console.log("[fnMakeOIPF] make OIPF object");
        if (aENV.getConfig().useHMXVideoBroadcast) {
            ret.videoBroadcast = window.oipfObjectFactory.createHumaxVideoBroadcastObject();
        } else {
            ret.videoBroadcast = window.oipfObjectFactory.createVideoBroadcastObject();
        }
        ret.channelConfig = ret.videoBroadcast.getChannelConfig();
        ret.configuration = window.oipfObjectFactory.createHumaxConfigurationObject();
        ret.avController = window.oipfObjectFactory.createHumaxVideoOnDemandObject();
        ret.applicationManager = window.oipfObjectFactory.createApplicationManagerObject();
        ret.parentalControlManager = window.oipfObjectFactory.createParentalControlManagerObject();
        ret.searchManager = window.oipfObjectFactory.createSearchManagerObject();
        ret.recordingScheduler = window.oipfObjectFactory.createRecordingSchedulerObject();
        ret.casUtil = window.oipfObjectFactory.createHumaxCasUtil();
        ret.storageUtil = window.oipfObjectFactory.createHumaxStorageUtilObject();
        ret.hDlna = window.oipfObjectFactory.createDlnaManagerObject();
        ret.hSamba = window.oipfObjectFactory.createSambaManagerObject();
        ret.hFtp = window.oipfObjectFactory.createFtpManagerObject();
        ret.hContentManager = window.oipfObjectFactory.createContentManagerObject();
        ret.hWoon = window.oipfObjectFactory.createWoonManagerObject();
        ret.hBluetoothManager = window.oipfObjectFactory.createBluetoothManagerObject();
        ret.hDial = window.oipfObjectFactory.createDialManagerObject();
        ret.hSatip = window.oipfObjectFactory.createSatipManagerObject();
        //ret.hAirPlay = window.oipfObjectFactory.createAirplayManagerObject();
        ret.operatorFeature = window.oipfObjectFactory.createHumaxOperatorFeatureObject();
        ret.hAlexaHelper = window.oipfObjectFactory.createAlexaHelperManagerObject();
        console.log("[fnMakeOIPF] make OIPF object");

        return ret;
    }
    function fnLoadPrism(aCallback: (...aModules: any[]) => void) {
        scripts = aENV.getPrismUX();
        callback = aCallback;
        console.log("[main] : scripts = " + scripts.join(", "));
//        require(scripts, aCallback);
        _fnLoad(callback);
    }
    function fnLoadUtilities(aCallback: (...aModules: any[]) => void) {
        scripts = aENV.getUtility();
        console.log("[main] : scripts = " + scripts.join(", "));
        require(scripts, aCallback);
    }
    function fnLoadKeyTable(aCallback: (...aModules: any[]) => void) {
        scripts = aENV.getKeyTable();
        console.log("[main] : scripts = " + scripts.join(", "));
        require(scripts, aCallback);
    }
    function fnLoadLogger(aCallback: (...aModules: any[]) => void) {
//            XXX: name will be name changed to hx
        scripts = ["lib/local/logger"];
        callback = aCallback;
        console.log("[main] : scripts = " + scripts.join(", "));
        _fnLoad(callback);
    }
    function fnLoadPageUtilities(aCallback: (...aModules: any[]) => void) {
        scripts = aENV.getPageUtility();
        callback = aCallback;
        console.log("[main] : scripts = " + scripts.join(", "));
        require(scripts, aCallback);
    }
    function fnLoadDL(aCallback: (...aModules: any[]) => void) {
        scripts = ["layer_domain/dlEntity"];
        callback = aCallback;
        console.log("[main] : scripts = " + scripts.join(", "));
        _fnLoad(callback);
    }
    function fnLoadAL(aCallback: (...aModules: any[]) => void) {
        scripts = ["layer_application/alEntity"];
        callback = aCallback;
        console.log("[main] : scripts = " + scripts.join(", "));
        _fnLoad(callback);
    }
    function _fnLoadRCU() {
        var elEmulConsole: HTMLDivElement = document.createElement('div')
        elEmulConsole.setAttribute("id", "divEmulConsole");
        $(elEmulConsole).load(aENV.getRCU());
        document.body.appendChild(elEmulConsole);
    }
    function _fnLoadDevUtil() {
        if (!ENV.isModeRelease) {
            var serverUrl = [
                "http://localhost:3400/client.js",
                "http://192.168.0.2:3400/client.js",
                "http://10.0.120.59:3400/client.js",
                "http://10.0.13.145:3400/client.js"
            ];
            var requirejs = window["requirejs"];
            var defaultErrorHandler = requirejs.onError;
            var fnRequire = function(aModules: string[]) {
                console.warn(aModules.join());
                require(aModules, function (_1, _2) {
                    console.warn("comeplete");
                    window["rjs"].setUpdateHandler(function (filename) {
                        if (window["__reloadFile"]) {
                            window["__reloadFile"](filename);
                        }
                    });
                    requirejs.onError = defaultErrorHandler;
                });
            };
            requirejs.onError = function(error: RequireError) {
                requirejs.undef(error.requireModules[0]);
                var url = serverUrl.shift();
                if (url) {
                    console.log(url);
                    fnRequire([url]);
                } else {
                    requirejs.onError = defaultErrorHandler;
                    console.error(error);
                    window["debug"](error.toString());
                }
            };
            require(["snippet/map_load_after.js"], function() {
                fnRequire([serverUrl.shift()]);
            });
        }
    }
    console.time("EXT_LIB");
    fnLoadExtLibs(() => {
        if (!ENV.isModeRelease) {
            (<any>window).Q = arguments[5];
        } else {
            (<any>window).Q = arguments[4];
        }
        console.timeEnd("EXT_LIB");
        if (aENV.isEmul()) {
            console.time("EMUL");
            fnLoadEmulator(() => {
                console.timeEnd("EMUL");
                console.time("KEYTABLE");
                fnLoadKeyTable((__KEY_TABLE__, __PRODUCT_KEY_TABLE__, __EMUL_KEY_TABLE__) => {
                    $.extend(__KEY_TABLE__, __PRODUCT_KEY_TABLE__, __EMUL_KEY_TABLE__);
                    console.timeEnd("KEYTABLE");
                    console.time("HX");
                    fnLoadLogger((__HX__) => {
                        window.hx = __HX__;
                        //XXX: config for for backward compatibility
                        __HX__.config = aENV.getConfig();
                        $.extend(__HX__.config, aENV.getUnclassifiedData());
//                        __HX__.config.settings = aENV.getUnclassifiedData().settings;
                        __HX__.emulMode = aENV.isEmul();
                        __HX__.key = __KEY_TABLE__;
                        __HX__.stbObject = fnMakeOIPFObject();
                        console.timeEnd("HX");
                        console.time("RCU");
                        _fnLoadRCU();
                        console.timeEnd("RCU");
                        console.time("UTIL");
                        fnLoadUtilities((__UTILS__, __UI_COMPONENT__, __LANGUAGE_TRANSLATOR__) => {
                            __HX__.il = __UTILS__;
                            __HX__.ui = __UI_COMPONENT__;
                            __HX__.l = new __LANGUAGE_TRANSLATOR__(aENV.getLanguageTable());
                            __HX__.util = {
                                format: function() {
                                    if (arguments.length > 1) {
                                        var text = arguments[0], token, tokenCount = arguments.length - 2;
                                        for ( token = 0; token <= tokenCount; token += 1) {
                                            text = text.replace(new RegExp("\\{" + token + "\\}", "gi"), arguments[token + 1]);
                                        }
                                    }
                                    return text;
                                }
                            };
                            console.timeEnd("UTIL");
                            console.time("PRISM");
                            fnLoadPrism(() => {
                                console.timeEnd("PRISM");
                                console.time("LAYER_DOMAIN");
                                fnLoadDL((__DL__) => {
                                    __HX__.dl = __DL__;
                                    __DL__.entity = new __DL__.cDLEntity(aENV.getService(), _fnLoadAfterDirectService);
                                    console.timeEnd("LAYER_DOMAIN");
                                });
                            });
                            function _fnLoadAfterDirectService() {
                                console.time("LAYER_APPLICATION");
                                fnLoadAL((__AL__) => {
                                    __AL__.entity = new __AL__.cALEntity(aENV.getPages(), aENV.getPageLayout(), aENV.getPageEntry(), aENV.getComponents(), aENV.getEvents());
                                    __HX__.al = __AL__;
                                    console.timeEnd("LAYER_APPLICATION");
                                    console.timeEnd("BOOT");
//                                        _fnLoadDevUtil();
                                });
                            }
                        });
                    });
                });
            });
        } else {
            fnLoadKeyTable((__KEY_TABLE__, __PRODUCT_KEY_TABLE__) => {
                $.extend(__KEY_TABLE__, __PRODUCT_KEY_TABLE__);
                fnLoadLogger((__HX__) => {
                    window.hx = __HX__;
                    //XXX: config for for backward compatibility
                    __HX__.config = aENV.getConfig();
                    $.extend(__HX__.config, aENV.getUnclassifiedData());
//                        __HX__.config.settings = aENV.getUnclassifiedData().settings;
                    __HX__.emulMode = aENV.isEmul();
                    __HX__.key = __KEY_TABLE__;
                    __HX__.stbObject = fnMakeOIPFObject();
                    //XXX: page_proxyhomenu, page_hotkey depend on help2
//                        __HX__.config.help2 = aENV.getUnclassifiedData().help2;
                    fnLoadUtilities((__UTILS__, __UI_COMPONENT__, __LANGUAGE_TRANSLATOR__) => {
                        __HX__.il = __UTILS__;
                        __HX__.ui = __UI_COMPONENT__;
                        __HX__.l = new __LANGUAGE_TRANSLATOR__(aENV.getLanguageTable());
                        __HX__.util = {
                            format: function() {
                                if (arguments.length > 1) {
                                    var text = arguments[0], token, tokenCount = arguments.length - 2;
                                    for ( token = 0; token <= tokenCount; token += 1) {
                                        text = text.replace(new RegExp("\\{" + token + "\\}", "gi"), arguments[token + 1]);
                                    }
                                }
                                return text;
                            }
                        };
                        //TODO: entity init
                        fnLoadPrism(() => {
                            fnLoadDL((__DL__) => {
                                __HX__.dl = __DL__;
                                __DL__.entity = new __DL__.cDLEntity(aENV.getService(), _fnLoadAfterDirectService);
                            });
                        });
                        function _fnLoadAfterDirectService() {
                            fnLoadAL((__AL__) => {
                                __AL__.entity = new __AL__.cALEntity(aENV.getPages(), aENV.getPageLayout(), aENV.getPageEntry(), aENV.getComponents(), aENV.getEvents());
                                __HX__.al = __AL__;
                                console.timeEnd("BOOT");
                                __HX__.log("debug", "BOOT");
                                console.time("DEV");
                                __HX__.log("debug", "BOOT");
//                                    _fnLoadDevUtil();
                            });
                        }
                    });
                });
            });
        }
    });
}
document.addEventListener("DOMContentLoaded", start, false);
if (document.readyState === "complete") {
    start();
}
function start() {
    __ENV__.onload(function(aENV: ifProductConfiguration) {
        console.timeEnd("ENV");
        main(aENV);
    });
}
