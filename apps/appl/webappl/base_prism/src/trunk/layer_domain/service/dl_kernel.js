define(["require", "exports"], function (require, exports) {
    /// <reference path="../../def/sugar.d.ts" />
    var fnFindReceiverInfo = function (a, eventName, receiver) {
        var receivers = a._eventReceivers[eventName], i, receiverInfo, len;
        if (receivers && receivers.length) {
            len = receivers.length;
        }
        else {
            len = 0;
        }
        for (i = 0; i < len; i += 1) {
            receiverInfo = receivers[i];
            if (receiverInfo.receiver === receiver) {
                return receiverInfo;
            }
        }
        return null;
    };
    var CEventSource = (function () {
        function CEventSource() {
        }
        CEventSource.prototype._registerEvent = function (eventList) {
            var a = this, i, eventName;
            var eventReceivers = a._eventReceivers || {};
            for (i = 0; i < eventList.length; i += 1) {
                eventName = eventList[i];
                if (eventReceivers[eventName]) {
                    console.error('EVENT ALREADY DEFINED');
                    throw "EVENT ALREADY DEFINED";
                }
                eventReceivers[eventName] = [];
            }
            a._eventReceivers = eventReceivers;
        };
        CEventSource.prototype.destroy = function () {
            var a = this;
            a._eventReceivers = null;
        };
        CEventSource.prototype.addEventHandler = function (eventName, receiver, handler) {
            if (arguments.length === 2) {
                throw 'DEPRECATED RECEIVER REQUIRED FOR REMOVAL';
            }
            var a = this, receiverInfo = fnFindReceiverInfo(a, eventName, receiver);
            if (receiverInfo) {
                receiverInfo.handlers.push(handler);
            }
            else {
                a._eventReceivers[eventName].push({
                    receiver: receiver,
                    handlers: [handler]
                });
            }
        };
        CEventSource.prototype.removeEventHandler = function (receiver, eventName) {
            var a = this;
            if (a._eventReceivers) {
                var receivers = a._eventReceivers[eventName];
                var removeReceiver = function (receivers, receiver) {
                    var i, len = receivers.length, receiverInfo, index = -1;
                    for (i = 0; i < len; i += 1) {
                        receiverInfo = receivers[i];
                        if (receiverInfo.receiver === receiver) {
                            delete receiverInfo.receiver;
                            delete receiverInfo.handlers;
                            index = i;
                        }
                    }
                    receiverInfo = undefined;
                    if (index !== -1) {
                        receivers.splice(index, 1);
                        return true;
                    }
                    return false;
                }, key;
                if (receivers) {
                    removeReceiver(receivers, receiver);
                }
                else {
                    for (key in a._eventReceivers) {
                        receivers = a._eventReceivers[key];
                        removeReceiver(receivers, receiver);
                    }
                }
            }
        };
        CEventSource.prototype._fireEvent = function () {
            var a = this, args = $.makeArray(arguments), eventName = args.shift(), receivers = a._eventReceivers[eventName], len, receiverInfo, i, j, len2, receiver, handlers;
            if (receivers) {
                len = receivers.length;
            }
            else {
                len = 0;
            }
            for (i = 0; i < len; i += 1) {
                receiverInfo = receivers[i];
                receiver = receiverInfo.receiver;
                handlers = receiverInfo.handlers;
                len2 = handlers.length;
                for (j = 0; j < len2; j += 1) {
                    handlers[j].apply(receiver, args);
                }
            }
            if (a['on' + eventName]) {
                a['on' + eventName].apply(a, args);
            }
            args = undefined;
            receiver = undefined;
            handlers = undefined;
            receiverInfo = undefined;
            receivers = undefined;
        };
        return CEventSource;
    })();
    function createPlugIn(pluginName) {
        var plugInCreators = {
            'ApplicationManager': function () {
                return hx.stbObject.applicationManager;
            },
            'VideoBroadcast': function () {
                return hx.stbObject.videoBroadcast;
            },
            'AVController': function () {
                return hx.stbObject.avController;
            },
            'RecordingScheduler': function () {
                return hx.stbObject.recordingScheduler;
            },
            'Configuration': function () {
                return hx.stbObject.configuration.configuration;
            },
            'LocalSystem': function () {
                return hx.stbObject.configuration.localSystem;
            },
            'StorageUtil': function () {
                return hx.stbObject.storageUtil;
            },
            'Samba': function () {
                return hx.stbObject.hSamba;
            },
            'SambaServer': function () {
                return hx.stbObject.hSamba.server;
            },
            'Ftp': function () {
                return hx.stbObject.hFtp;
            },
            'FtpServer': function () {
                return hx.stbObject.hFtp.server;
            },
            'Woon': function () {
                return hx.stbObject.hWoon;
            },
            'BluetoothManager': function () {
                return hx.stbObject.hBluetoothManager;
            },
            'ContentManager': function () {
                return hx.stbObject.hContentManager;
            },
            'DLNA.DMP': function () {
                return hx.stbObject.hDlna.dmp;
            },
            'DLNA.DMR': function () {
                return hx.stbObject.hDlna.dmr;
            },
            'DLNA.DMS': function () {
                return hx.stbObject.hDlna.dms;
            },
            'Dial': function () {
                return hx.stbObject.hDial;
            },
            'AirPlay': function () {
                return hx.stbObject.hAirPlay;
            },
            'Satip': function () {
                return hx.stbObject.hSatip;
            },
            'ParentalControlManager': function () {
                return hx.stbObject.parentalControlManager;
            },
            'ChannelConfig': function () {
                return hx.stbObject.channelConfig;
            },
            'OperatorFeature': function () {
                return hx.stbObject.operatorFeature;
            },
            'Netflix': function () {
                if (hx.config.netflix === true) {
                    return new Netflix();
                }
            },
            'AlexaHelper': function () {
                return hx.stbObject.hAlexaHelper;
            }
        };
        return plugInCreators[pluginName]();
    }
    var fnLog = function (msg) {
        hx.log('info', msg);
    };
    var fnLogError = function (msg) {
    };
    var mApiGroups = {};
    var mPluginPool = {};
    var mInterAPIGroupRequestList = [];
    var fnCheckReadyAndFire = function (aApiGroupRequest) {
        var requiredApiGroups = aApiGroupRequest.requiredApiGroups, i, max, apiGroup, args = [];
        for (i = 0, max = requiredApiGroups.length; i < max; i += 1) {
            apiGroup = mApiGroups[requiredApiGroups[i]];
            if (!apiGroup) {
                return false;
            }
            args.push(apiGroup);
        }
        aApiGroupRequest.callback.apply(mApiGroups[aApiGroupRequest.targetApiGroupName], args);
        return true;
    };
    var fnProcessAPIGroupRequests = function () {
        var newList = [], i, max, req;
        for (i = 0, max = mInterAPIGroupRequestList.length; i < max; i += 1) {
            req = mInterAPIGroupRequestList[i];
            if (!fnCheckReadyAndFire(req)) {
                newList.push(req);
            }
        }
        mInterAPIGroupRequestList = newList;
    };
    var cDServiceKernel = (function () {
        function cDServiceKernel(aServiceName) {
            this._eventHandlers = [];
            this._groupName = aServiceName;
            mApiGroups[aServiceName] = this;
            fnProcessAPIGroupRequests();
        }
        cDServiceKernel.prototype.addEventListeners = function (params) {
            var i, objList, events = params.events, objTarget = params.objTarget, priority = params.priority;
            for (i = 0; i < events.length; i += 1) {
                this.addEventListener({
                    event: events[i],
                    objTarget: objTarget,
                    priority: priority
                });
            }
        };
        cDServiceKernel.prototype.addEventListener = function (params) {
            var i, objList, event = params.event, objTarget = params.objTarget, priority = params.priority;
            if (!this._eventHandlers[event]) {
                this._eventHandlers[event] = [];
            }
            priority = priority || 0;
            objList = this._eventHandlers[event];
            for (i = 0; i < objList.length; i += 1) {
                var o = objList[i].o;
                if (o === objTarget) {
                    return;
                }
            }
            this._eventHandlers[event].push({
                p: priority,
                o: objTarget
            });
        };
        cDServiceKernel.prototype.removeEventListener = function (objTarget) {
            var i, eventName, objList;
            for (eventName in this._eventHandlers) {
                if (this._eventHandlers.hasOwnProperty(eventName)) {
                    objList = this._eventHandlers[eventName];
                    for (i = 0; i < objList.length; i += 1) {
                        var o = objList[i].o;
                        if (o === objTarget) {
                            objList.splice(i, 1);
                            break;
                        }
                    }
                }
            }
        };
        cDServiceKernel.prototype._fireEvent = function () {
            var aArgs = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                aArgs[_i - 0] = arguments[_i];
            }
            var args = $.makeArray(arguments);
            var eventName = args.shift();
            var i, t, f, eventTarget = this._eventHandlers[eventName];
            if (eventTarget) {
                eventTarget.sort(function (a, b) {
                    return a.p - b.p;
                });
                for (i = 0; i < eventTarget.length; i += 1) {
                    try {
                        t = eventTarget[i];
                        f = t.o['_on_' + eventName];
                        if (f) {
                            f.apply(t.o, args);
                        }
                    }
                    catch (e) {
                        hx.log('dsvc', 'hx.dl.factory.dService_kernel.fireEvent [' + typeof this + '][' + 'on' + eventName + '] EXCEPT: ' + e);
                    }
                }
            }
            args = undefined;
            eventTarget = undefined;
        };
        cDServiceKernel.prototype.getPluginInstance = function (pluginName) {
            if (!mPluginPool[pluginName]) {
                mPluginPool[pluginName] = createPlugIn(pluginName);
            }
            return mPluginPool[pluginName];
        };
        cDServiceKernel.prototype.defineEvents = function (eventList) {
            this._eventSource = new CEventSource();
            this._eventSource._registerEvent(eventList);
        };
        cDServiceKernel.prototype.addEventCb = function (eventName, receiver, handler) {
            this._eventSource.addEventHandler(eventName, receiver, handler);
        };
        cDServiceKernel.prototype.clearEventCb = function (receiver, eventName) {
            this._eventSource.removeEventHandler(receiver, eventName);
        };
        cDServiceKernel.prototype.fireEvent = function () {
            var aArgs = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                aArgs[_i - 0] = arguments[_i];
            }
            this._eventSource._fireEvent.apply(this._eventSource, $.makeArray(arguments));
        };
        cDServiceKernel.prototype.fireEvent_Legacy = function () {
            var aArgs = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                aArgs[_i - 0] = arguments[_i];
            }
            this._fireEvent.apply(this, $.makeArray(arguments));
        };
        cDServiceKernel.prototype.defineAPIs = function (apis) {
            throw "define public function in class instead of [definesAPIs] function";
            var a = this;
            var apiName;
            for (apiName in apis) {
                this[apiName] = (function (apiName, func) {
                    return function (param) {
                        fnLog(a._groupName + '.' + apiName);
                        return func(param);
                    };
                }(apiName, apis[apiName]));
            }
        };
        cDServiceKernel.prototype.saveData = function (key, param) {
            localStorage.setItem(this._groupName + '.' + key, JSON.stringify(param));
        };
        cDServiceKernel.prototype.readData = function (key) {
            var a = this, ret = localStorage.getItem(a._groupName + '.' + key);
            if (ret) {
                ret = JSON.parse(ret);
            }
            return ret;
        };
        cDServiceKernel.prototype.require = function () {
            var aArgs = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                aArgs[_i - 0] = arguments[_i];
            }
            var args = $.makeArray(arguments);
            var req = {
                targetApiGroupName: this._groupName,
                callback: args.pop(),
                requiredApiGroups: args
            };
            if (!fnCheckReadyAndFire(req)) {
                mInterAPIGroupRequestList.push(req);
            }
        };
        return cDServiceKernel;
    })();
    return cDServiceKernel;
});
