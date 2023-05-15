/// <reference path="../../def/sugar.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 7. 26..
 */
define(["require", "exports"], function (require, exports) {
    var CAppRpcUtil = (function () {
        function CAppRpcUtil(appInfo) {
            var _this = this;
            this._notifyList = {};
            this._callback = {};
            this._appInfo = appInfo;
            this._window = appInfo.app.window;
            this.notifyHidden(function () {
                hx.svc.APP_MGR.hideApplication({
                    appInfo: appInfo
                });
            });
            this._notify('notifyCallback', function (fname, index, args) {
                _this._callback[fname][index].apply(_this, args);
            });
        }
        CAppRpcUtil.prototype._call = function () {
            var aArguments = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                aArguments[_i - 0] = arguments[_i];
            }
            var i, len, arg;
            var args = $.makeArray(arguments);
            var fname = args.shift();
            for (i = 0, len = args.length; i < len; i += 1) {
                arg = args[i];
                if (typeof arg === 'function') {
                    this._callback[fname] = this._callback[fname] || [];
                    this._callback[fname].push(arg);
                    args[i] = '__callback__.' + fname + '.' + (this._callback[fname].length - 1);
                }
            }
            var msg = JSON.stringify({
                type: 'call',
                fname: fname,
                args: args
            });
            this._window.postMessage('iwc.' + msg, this._appInfo.param.url);
            //hx.log("debug", "main app => " + this._appInfo.param.url);
            //hx.log("debug", "[_call_] message: " + msg);
        };
        CAppRpcUtil.prototype._notify = function (fname, fnCallback) {
            if (!this._notifyList[fname]) {
                this._notifyList[fname] = [];
            }
            this._notifyList[fname].push(fnCallback);
            var msg = JSON.stringify({
                type: 'reqNotify',
                fname: fname
            });
            this._window.postMessage('iwc.' + msg, this._appInfo.param.url);
            //hx.log("debug", "main app => " + this._appInfo.param.url);
            //hx.log("debug", "[_reqNotify_] message: " + msg);
        };
        CAppRpcUtil.prototype.show = function () {
            var _this = this;
            setTimeout(function () {
                hx.svc.APP_MGR.showApplication({
                    appInfo: _this._appInfo
                });
                _this._call('show');
            }, 1);
        };
        CAppRpcUtil.prototype.hide = function () {
            this._call('hide');
        };
        CAppRpcUtil.prototype.notifyShown = function (fnShown) {
            this._notify('notifyShown', fnShown);
        };
        CAppRpcUtil.prototype.notifyHidden = function (fnHidden) {
            this._notify('notifyHidden', fnHidden);
        };
        return CAppRpcUtil;
    })();
    return CAppRpcUtil;
});
