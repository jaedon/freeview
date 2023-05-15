/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvDial.js
/*jslint nomen:true, vars:true */
//    DIAL: ['layer_domain/service/dl_dsrvDial.js'],
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "./dl_kernel"], function (require, exports, __KERNEL__) {
    /**************************************************
     * Definitions
     **************************************************/
    var EAPP_LAUNCHED = 0;
    var EAPP_STOPPED = 1;
    var EAPP_DELETED = 2;
    var ESERVER_STARTED = 3;
    var ESERVER_STOPPED = 4;
    var EDIAL_STATUS = {
        STOP: 0,
        RUNNING: 1
    };
    hx.logadd('dsrvDial');
    var fnLog = function (log) {
        hx.log('dsrvDial', '#####' + log);
    };
    var cDLDial = (function (_super) {
        __extends(cDLDial, _super);
        function cDLDial(aServiceName) {
            var _this = this;
            _super.call(this, aServiceName);
            this.mDial = this.getPluginInstance('Dial');
            this.mDialServer = this.mDial.server;
            this.defineEvents(['DialStatusEvent', 'ReqLaunch', 'ReqStop']);
            this.mDialServer.addEventListener('DialStatusEvent', function (e) {
                _this.onDialStatusEvent(e.status);
            });
            this.mDialServer.addEventListener('ReqLaunch', function (e) {
                _this.onReqLaunch(e.name, e.url);
            });
            this.mDialServer.addEventListener('ReqStop', function (e) {
                _this.onReqStop(e.name);
            });
        }
        cDLDial.prototype.start = function () {
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog('[fnStart]');
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            if (this.isStarted()) {
                this.registApplication('YouTube');
                if (hx.svc.APP_MGR.isAppOpened({ 'name': 'YouTube' })) {
                    this.updateApplicationState('YouTube', EDIAL_STATUS.RUNNING);
                }
                this.registApplication('Netflix');
                if (hx.svc.NETFLIX.isRunning()) {
                    this.updateApplicationState('Netflix', EDIAL_STATUS.RUNNING);
                }
            }
            else {
            }
        };
        cDLDial.prototype.stop = function () {
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog('[fnStop]');
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            this.mDialServer.stop();
        };
        cDLDial.prototype.isStarted = function () {
            var started = this.mDialServer.started;
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog(hx.util.format('[fnIsStarted] - started = {0}', started));
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            return started;
        };
        cDLDial.prototype.registApplication = function (aName) {
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog(hx.util.format('[fnRegistApplication] - name = {0}', aName));
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            this.mDialServer.registApplication(aName);
        };
        cDLDial.prototype.unregistApplication = function (aName) {
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog(hx.util.format('[fnUnregistApplication] - name = {0}', aName));
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            this.mDialServer.unregistApplication(aName);
        };
        cDLDial.prototype.updateApplicationState = function (aName, aState) {
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog(hx.util.format('[fnUpdateApplicationState] - name = {0}', aName));
            fnLog(hx.util.format('[fnUpdateApplicationState] - state = {0}', aState));
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            this.mDialServer.updateApplicationState(aName, aState);
        };
        cDLDial.prototype.onDialStatusEvent = function (status) {
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog(hx.util.format('[onDialStatusEvent] - status = {0}', status));
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            switch (status) {
                case ESERVER_STARTED:
                    this.registApplication('YouTube');
                    if (hx.svc.APP_MGR.isAppOpened({ 'name': 'YouTube' })) {
                        this.updateApplicationState('YouTube', EDIAL_STATUS.RUNNING);
                    }
                    this.registApplication('Netflix');
                    if (hx.svc.NETFLIX.isRunning()) {
                        this.updateApplicationState('Netflix', EDIAL_STATUS.RUNNING);
                    }
                    break;
                case ESERVER_STOPPED:
                    this.unregistApplication('YouTube');
                    this.unregistApplication('Netflix');
                    break;
                default:
                    break;
            }
        };
        cDLDial.prototype.onReqLaunch = function (name, url) {
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog(hx.util.format('[onReqLaunch] - name = {0}', name));
            fnLog(hx.util.format('[onReqLaunch] - url = {0}', url));
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            this.fireEvent('ReqLaunch', {
                name: name,
                url: url
            });
        };
        cDLDial.prototype.onReqStop = function (name) {
            fnLog('##################################################');
            fnLog('--------------------------------------------------');
            fnLog(hx.util.format('[onReqStop] - name = {0}', name));
            fnLog('--------------------------------------------------');
            fnLog('##################################################');
            this.fireEvent('ReqStop', {
                name: name
            });
        };
        return cDLDial;
    })(__KERNEL__);
    return cDLDial;
});
