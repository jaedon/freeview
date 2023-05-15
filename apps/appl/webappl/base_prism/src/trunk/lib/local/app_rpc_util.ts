/// <reference path="../../def/sugar.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 7. 26..
 */

declare var hx: ifSugar;

class CAppRpcUtil {
    private _appInfo;
    private _window;
    private _notifyList = {};
    private _callback = {};

    constructor(appInfo) {
        this._appInfo = appInfo;
        this._window = appInfo.app.window;
        this.notifyHidden(function() {
            hx.svc.APP_MGR.hideApplication({
                appInfo : appInfo
            });
        });
        this._notify('notifyCallback', (fname, index, args) => {
            this._callback[fname][index].apply(this, args);
        });
    }
    _call(...aArguments): void {
        var i, len, arg;
        var args = $.makeArray(arguments);
        var fname = args.shift();
        for (i=0, len=args.length; i<len; i+=1) {
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
    }
    _notify(fname, fnCallback): void {
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
    }
    show(): void {
        setTimeout(() => {
            hx.svc.APP_MGR.showApplication({
                appInfo : this._appInfo
            });
            this._call('show');
        }, 1);
    }
    hide(): void {
        this._call('hide');
    }
    notifyShown(fnShown): void {
        this._notify('notifyShown', fnShown);
    }
    notifyHidden(fnHidden): void {
        this._notify('notifyHidden', fnHidden);
    }
}
export = CAppRpcUtil