/// <reference path="../../def/sugar.d.ts" />
/// <reference path="../../def/require.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 3. 28..
 */
//    KEY_EVENT_BINDER: ['layer_domain/service/dl_key_event_binder.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel", 'lib/local/key_catcher'], function (require, exports, __KERNEL__, __KEY_CATCHER__) {
    var cDLKeyEventBinder = (function (_super) {
        __extends(cDLKeyEventBinder, _super);
        function cDLKeyEventBinder(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this._appMgr = null;
            this._countRepeat = 0;
            this._userListener = null;
            this._fnHookers = [];
            this._listener = function (aEvent) {
                var keyString = hx.key[aEvent.which];
                hx.log("info", "[cDLKeyEventBinder] : [" + aEvent.which + "] " + hx.key[aEvent.which] + (keyString ? "" : " : Unsupported key inputted"));
                if (aEvent.metaKey || aEvent.ctrlKey) {
                    /* COMMENT
                     * for debug env
                     * * bglee@humaxdigital.com, 10:25, 2014. 4. 5.
                     */
                    hx.log("warn", "[cDLKeyEventBinder] : meta key + [" + hx.key[aEvent.which] + "] will be skipped");
                    /* TODO
                     * if clauses will be replaced by meta key event hooker
                     * bglee@humaxdigital.com, 10:58, 2014. 8. 20.
                     */
                    var handled = false;
                    if (aEvent.which === 65) {
                        var t = new window['Collection']();
                        t.push({
                            termId: 2,
                            eventName: "EastEnders",
                            eventText: "EastEndersis a British television soap opera, first broadcast in the United Kingdom on BBC One on 19 February 1985 and continuing to today.<br>" + "EastEnders storylines examine the domestic and professional lives of the people who live and work in the fictional London Borough of Walford in the East End of London. The series primarily centres around the EastEndersis a British television soap opera, first broadcast in the United Kingdom on BBC One on 19 February 1985 and continuing to today. EastEnders storylines examine the domestic and professional lives of the people who live and work in the fictional London Borough of \
                    Walford in the East End of London. The series primarily centres around the East End of London. The series primarily centres around the EastEndersis"
                        });
                        t.push({
                            termId: 5,
                            eventName: "bglee # 05",
                            eventText: new Date().toString()
                        });
                        hx.svc.AV_CTRL.fireEvent("RctInfoChanged", 3, t);
                        handled = true;
                    }
                    else if (aEvent.which === 66) {
                        var url = "snippet/event_rct.js?";
                        require([url + new Date().getTime()], function () {
                            console.log('loaded');
                        });
                    }
                    if (handled) {
                        aEvent.preventDefault();
                    }
                    return;
                }
                aEvent.preventDefault();
                _this._deliver(aEvent);
            };
            this._listenerKeyUp = function (aEvent) {
                if (aEvent.metaKey) {
                    return;
                }
                aEvent.preventDefault();
                _this._countRepeat = 0;
                _this._target.onKey(aEvent.which, _this._countRepeat, aEvent.type);
            };
            if (!ENV.isModeRelease) {
                var keyCatcher = new __KEY_CATCHER__(hx, function (aFx) {
                    _this.reqAddHooker(aFx);
                }, function (aFx) {
                    _this.reqRemoveHooker(aFx);
                });
                //keyCatcher.registerKeyCode([1, 7, 9, 3, 0, 0, 1], "user aging", () => {
                //    var e = window.crossBrowser_initKeyboardEvent("keydown", {
                //        which: 27,
                //        keyCode: 27
                //    });
                //    document.dispatchEvent(e);
                //    require(["text!test_ts/useraging"], (aText: string) => {
                //        var testCases = aText.match(/[^\r\n]+/g);
                //        var step = 0;
                //        var test = () => {
                //            var testCase = testCases[step];
                //            var data = testCase.split(',');
                //            var e = window.crossBrowser_initKeyboardEvent("keydown", {
                //                which: hx.key[data[0]],
                //                keyCode: hx.key[data[0]]
                //            });
                //            document.dispatchEvent(e);
                //            var time: number = Number(data[1].trim()) * 1000;
                //            step++;
                //            if (step < testCase.length) {
                //                setTimeout(test, time);
                //            }
                //        }
                //        test();
                //    });
                //});
                keyCatcher.registerKeyCode([2, 5, 8, 0, 0, 0, 2], "reload web page", function () {
                    hx.svc.SETTING_UTIL.doingKeyGrab(false);
                    var mainApp = hx.stbObject.applicationManager.getOwnerApplication(document);
                    var childApps = hx.stbObject.applicationManager.getChildApplications(mainApp);
                    for (var i = 0; i < childApps.length; i++) {
                        if (childApps[i]) {
                            childApps[i].destroyApplication();
                        }
                    }
                    document.location.reload();
                });
                //keyCatcher.registerKeyCode([5, 9, 4, 5, 0, 1, 2], "<strike>web for test</strike>", () => {
                //    require(["test_ts/main"], (aMain) => {
                //        new aMain();
                //    });
                //});
                keyCatcher.registerKeyCode([5, 9, 4, 5, 3, 4, 5], "reset MDS Cache && authenticate", function () {
                    var category = [
                        'meta',
                        'image',
                        'moved',
                        'auth'
                    ];
                    var db = {};
                    category.forEach(function (name) {
                        db[name] = {};
                    });
                    hx.log('debug', 'db\'s category: ' + Object.keys(db));
                    hx.svc.IP_EPG._cache_controller.removeAll();
                    hx.svc.IP_EPG._cache_controller.setDb(db);
                    hx.svc.IP_EPG._adapter._engine.setBaseUrl(null);
                    hx.svc.IP_EPG.getAuthToken(-1); //reset All cached data on HTTP and reset Auth
                });
                //keyCatcher.registerKeyCode([5, 9, 4, 5, 4, 5, 6], "set toggle wired device enabled property", () => {
                //    hx.svc.SETTING_UTIL["mNetworkManager"].updateDevWired(function(dev) {dev.enabled = !dev.enabled; })
                //});
                //keyCatcher.registerKeyCode([5, 9, 4, 5, 5, 6, 7], "set toggle to show GreenButton(RCT) debug board",
                // () => {
                //    hx.log('debug', 'get getAuthToken');
                //    hx.svc.IP_EPG.getAuthToken();
                //});
                keyCatcher.registerKeyCode([5, 9, 4, 5, 6, 7, 8], "tvportal device reset", function () {
                    hx.svc.TV_PORTAL.deviceReset({
                        fn: function () {
                            hx.log("debug", "device reset OK!");
                        },
                        fnError: function () {
                            hx.log("debug", "device reset fail!");
                        }
                    });
                });
                keyCatcher.registerKeyCode([5, 9, 4, 5, 1, 2, 3], "tvportal disable", function () {
                    hx.svc.TV_PORTAL.disableHumaxServerForTest();
                });
                keyCatcher.registerKeyCode([5, 9, 4, 5, 1, 2, 4], "tvportal enable", function () {
                    hx.svc.TV_PORTAL.enableHumaxServerForTest();
                });
                keyCatcher.registerKeyCode([5, 9, 4, 5, 1, 2, 5], "network disconnect", function () {
                    hx.svc.SETTING_UTIL.disconnectNetworkForTest();
                });
                keyCatcher.registerKeyCode([5, 9, 4, 5, 1, 2, 6], "network connect", function () {
                    hx.svc.SETTING_UTIL.connectNetworkForTest();
                });
                keyCatcher.registerKeyCode([5, 9, 4, 5, 7, 8, 9], "keygrab = true", (function () {
                    hx.svc.SETTING_UTIL.doingKeyGrab(true);
                }));
                keyCatcher.registerKeyCode([5, 9, 4, 5, 8, 9, 0], "keygrab = false", (function () {
                    hx.svc.SETTING_UTIL.doingKeyGrab(false);
                }));
                keyCatcher.registerKeyCode([5, 9, 4, 5, 9, 0, 1], "web log toggle", (function () {
                    var logger = function () {
                    };
                    var backup;
                    return function () {
                        if (!backup) {
                            backup = hx.log;
                            hx.log = logger;
                        }
                        else {
                            hx.log = backup;
                            backup = null;
                        }
                    };
                })());
                keyCatcher.registerKeyCodeNumber(hx.key.KEY_BLUE, "Blue Key - Voice Assistant", function () {
                    var dsrvVA = hx.svc.VOICE_ASSISTANCE;
                    if (hx.config.voiceAssistant) {
                        hx.config.voiceAssistant = false;
                        hx.svc.WEB_STORAGE.setAlexaDeviceEnable(false);
                        //make disable
                        if (dsrvVA) {
                            dsrvVA.removeMQTTClient();
                        }
                    }
                    else {
                        hx.config.voiceAssistant = true;
                        hx.svc.WEB_STORAGE.setAlexaDeviceEnable(true);
                        if (dsrvVA) {
                            var options = dsrvVA.getMQTTOptions();
                            var systemInfo = hx.svc.SETTING_UTIL.getSystemInfo();
                            var lanMacAddress = systemInfo.wiredMacAddress;
                            options.userId = lanMacAddress;
                            dsrvVA.createMQTTClient(options);
                            dsrvVA.connectMQTTClient(options);
                        }
                    }
                });
                /*
                keyCatcher.registerKeyLong(hx.key.KEY_BLUE, "debugging board", function() {
                    keyCatcher.input(hx.key.KEY_4);
                    keyCatcher.input(hx.key.KEY_5);
                    keyCatcher.input(hx.key.KEY_6);
                    keyCatcher.input(hx.key.KEY_3);
                    keyCatcher.input(hx.key.KEY_3);
                });
                keyCatcher.registerKeyLong(hx.key.KEY_YELLOW, "yellow debugging board", (function() {
                        var id = 'dev-yellow-board';
                        var elHelperNetwork: HTMLDivElement = <HTMLDivElement>document.getElementById(id);
                        if (!elHelperNetwork) {
                            elHelperNetwork = document.createElement('div');
                            elHelperNetwork.setAttribute('id', id);
                            var style = elHelperNetwork.style;
                            style.position = "absolute";
                            style.left = "100px";
                            style.top = "60px";
                            style.padding = "15px";
                            style.borderRadius = "8px";
                            style.fontSize = "18px";
                            style.backgroundColor = "rgba(0,0,0,.4)";
                            style.width = "1080px";
                            style.height = "500px";
                            style.textAlign = "center";
                            style.zIndex = "1000";
                            style.display = "block";
                            style.zIndex = "100";
                        }
                        (<any>window).d = (<any>window).d || {};
                        (<any>window).d.network = (<any>window).d.network || {
                            onReqOnAccessStatusUpdated: [],
                            onReqOnLinkStatusChanged: []
                        };
                        var interval = 0;
                        var color = ["Salmon", "SandyBrown", "SeaGreen", "DeepPink"];
                        return function() {
                            document.body.appendChild(elHelperNetwork);
                            clearTimeout(interval);
                            interval = setInterval(function() {
                                var network = (<any>window).d.network;
                                for (var key in network) {
                                    if (network[key].length) {
                                        var element = document.createElement('div');
                                        var wired = (<any>hx.svc.SETTING_UTIL).mNetworkManager.getDevWired();
                                        var wireless = (<any>hx.svc.SETTING_UTIL).mNetworkManager.getDevWireless();
                                        var config = (<any>hx.svc.SETTING_UTIL).mWifiConfig.getConnectedAPInfo();
                                        var info = "";
                                        if (wired.enabled) {
                                            info += " wired.enabled: " + wired.enabled;
                                        }
                                        info += " wl.connected: " + wireless.connected;
                                        if (wireless.enabled) {
                                            info += " wl.enabled: " + wireless.enabled;
                                        }
                                        info += " apInfo: " + (config ? config.isConnected : 'disconnected');
                                        element.style.color = color[0];
                                        element.innerText = dateFormat(Date.now(), null) + ': ' + network[key].pop() + ': ' + info;
                                        $(elHelperNetwork).prepend(element);
                                        color.push(color.shift());
                                    }
                                }
                            }, 1 * 1000);
                        }
                    }
                )());
                */
                this.reqAddHooker(function (aKeyCode, aRepeat) {
                    return keyCatcher.input(aKeyCode, aRepeat);
                });
            }
            if (hx.emulMode) {
                this._appMgr = this.getPluginInstance("ApplicationManager");
                this._deliver = this._deliverInEmulation;
            }
            else {
                this._deliver = this._deliverInSTB;
            }
        }
        cDLKeyEventBinder.prototype._hooking = function (aKeyCode, aRepeat) {
            var handled = false;
            for (var i = this._fnHookers.length - 1; i >= 0 && !handled; i--) {
                handled = this._fnHookers[i](aKeyCode, aRepeat);
            }
            return handled;
        };
        cDLKeyEventBinder.prototype._deliverInSTB = function (aEvent) {
            if (!this._hooking(aEvent.which, this._countRepeat)) {
                this._target.onKey(aEvent.which, this._countRepeat++, aEvent.type);
            }
        };
        cDLKeyEventBinder.prototype._deliverInEmulation = function (aEvent) {
            if (!this._hooking(aEvent.which)) {
                var appMgr = this._appMgr;
                var keyString = hx.key[aEvent.which] || aEvent.which;
                var keyCode = hx.key[keyString];
                var topMost = appMgr._topMostApp;
                hx.log("info", "[cDLKeyEventBinder] : emulation keycode : " + hx.key[keyCode]);
                this._countRepeat = aEvent.repeat ? this._countRepeat + 1 : 0;
                if (topMost && topMost._url) {
                    var elChildApps = document.getElementsByTagName('iframe');
                    for (var i = 0, len = elChildApps.length; i < len; i++) {
                        if (elChildApps[i].src === topMost._url) {
                            var e = window.crossBrowser_initKeyboardEvent("keydown", {
                                which: keyCode,
                                keyCode: keyCode
                            });
                            try {
                                if (!elChildApps[i].contentDocument.body.dispatchEvent(e)) {
                                }
                            }
                            catch (ex) {
                                alert("keyCode: " + hx.key[keyCode] + "[" + keyCode + "] - " + ex.toString());
                            }
                            break;
                        }
                    }
                }
                else {
                    this._target.onKey(aEvent.which, this._countRepeat, aEvent.type);
                }
            }
        };
        cDLKeyEventBinder.prototype._deliver = function (aEvent) {
            throw 'overrided';
        };
        cDLKeyEventBinder.prototype.bind = function (aElement, aTarget, aUserListener) {
            if (aUserListener === void 0) { aUserListener = null; }
            if (!aTarget.onKey) {
                throw 'target must have "onKey" property';
            }
            this._element = aElement;
            this._target = aTarget;
            if (aUserListener) {
                this._userListener = aUserListener;
            }
            aElement.addEventListener("keydown", this._userListener || this._listener);
            aElement.addEventListener("keyup", this._listenerKeyUp);
        };
        cDLKeyEventBinder.prototype.reqAddHooker = function (aFnHooker) {
            this._fnHookers.push(aFnHooker);
            hx.log("info", "[cDLKeyEventBinder] : hooker added");
            hx.log("info", "[cDLKeyEventBinder] : " + aFnHooker);
        };
        cDLKeyEventBinder.prototype.reqRemoveHooker = function (aFnHooker) {
            var index = this._fnHookers.lastIndexOf(aFnHooker);
            if (index !== -1) {
                this._fnHookers.splice(index, 1);
                hx.log("info", "[cDLKeyEventBinder] : hooker removed");
                hx.log("info", "[cDLKeyEventBinder] : " + aFnHooker);
            }
        };
        cDLKeyEventBinder.prototype.reqDeliverMainApp = function (aKeyCode) {
            var e = window.crossBrowser_initKeyboardEvent("keydown", {
                which: aKeyCode,
                keyCode: aKeyCode
            });
            this._deliver(e);
        };
        cDLKeyEventBinder.prototype.unBind = function () {
            this._element.removeEventListener("keydown", this._userListener || this._listener);
            this._userListener = null;
        };
        cDLKeyEventBinder.prototype.destroy = function () {
            this.unBind();
        };
        return cDLKeyEventBinder;
    })(__KERNEL__);
    return cDLKeyEventBinder;
});
//# sourceMappingURL=dl_key_event_binder.js.map