/// <reference path="def/builtinapp.d.ts" />
/// <reference path="../def/config.d.ts" />
// file name : builtin_apps/builtin_common.js
/*jslint sloppy:true, nomen:true, vars:true */

// Parse URL
declare var __REQUEST_INFO__: ifRequestBuiltinAppsInfo;
( function() {
    var mAppParam = ( function parseParams(urlParams) {
        var params = /^\?appName=?([\w]+)?\&parentUrl=?([\w:.\/]+)/.exec(urlParams);
        return {
            appName : params[1],
            parentUrl : params[2]
        };
    }(location.search) );

    window.log  = function(msg) {
        if (arguments.length === 2) {
            msg = arguments[1];
        }
        ( window.debug || function(){} )(" APP : " + mAppParam.appName + ":" + msg);
        console.log("APP [" + mAppParam.appName + "]: " + msg);
    };

    var notifyListInstance = (function(){
        var list = {};
        function each(ar, fn ) {
            var i, len;
            for ( i = 0, len = ar.length; i < len; i += 1 ) {
                fn(ar[i]);
            }
        }
        return {
            fnDefine: function(notifyName){
                log("Notify DEF: " + notifyName);
                list[notifyName] = [];
            },
            fnAppend: function(param, source){
                log("Notify APPEND: " + param.type + ": [" + param.fname + "]");
                var fname = param.fname;
                if (list[fname] && list[fname].indexOf(source) === -1) {
                    list[fname].push(source);
                }
            },
            fnNotify : function(notifyName){
                log("Notify NOTIFY: " + notifyName);
                var args = $.makeArray(arguments);
                args.shift();
                each(list[notifyName] || [], function(source){
                    source.postMessage("iwc." + JSON.stringify({
                            type: "notify",
                            fname: notifyName,
                            args: args,
                            appName : mAppParam.appName
                        }) , "*");
                });
            }
        };
    }());
    window.defineNotifyAPI = notifyListInstance.fnDefine;
    window.notify = notifyListInstance.fnNotify;

    var callListInstance = (function(){
        var list = {};
        return {
            fnDefine : function(apiName, handler) {
                log("Call DEF: " + apiName);
                list[apiName] = handler;
            },
            fnAppend: function(param) {
                log("Call APPEND: " + param.type + ": [" + param.fname + "]");
                var i, len, arg, args = param.args;
                for (i = 0, len = args.length; i < len; i++) {
                    arg = args[i];
                    if (typeof arg === "string" && arg.indexOf("__callback__") === 0) {
                        var callbackInfo = arg.split(".");
                        args[i] = (function (fname, index) {
                            return function () {
                                notify("notifyCallback", fname, index, $.makeArray(arguments));
                            };
                        }(callbackInfo[1], callbackInfo[2]));
                    }
                }
                if (list[param.fname]) {
                    list[param.fname].apply(this, args);
                }
            }
        };
    }());
    window.defineAPI = callListInstance.fnDefine;

    var fnMessageHandler = (function(fnNotify, fnCall){
        var fns = {
            reqNotify : fnNotify,
            call : fnCall
        };
        function messageHandler(messageEvent) {
            var param  = JSON.parse(/^iwc.([\w\W\"]+)/.exec(messageEvent.data)[1]) || {};
            (fns[param.type] || function(){})(param, messageEvent.source);
            log(param.type + ": [" + param.fname + "]");
        }
        return messageHandler;
    }(notifyListInstance.fnAppend, callListInstance.fnAppend));
    window._mainMessageHandler = fnMessageHandler;
    window.addEventListener("message", fnMessageHandler);

    window.getRoot = function() {
        return mAppParam.parentUrl.slice(0, mAppParam.parentUrl.lastIndexOf("/"));
    };
    (<any>window).tr = function(msg) {
        return msg;
    };

    defineNotifyAPI("notifyCallback");
}());
