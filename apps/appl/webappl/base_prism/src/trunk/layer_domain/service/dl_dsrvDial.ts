/// <reference path="../../def/sugar.d.ts" />
// file name : fw/service/HMX/dservice/dl_dsrvDial.js
/*jslint nomen:true, vars:true */
//    DIAL: ['layer_domain/service/dl_dsrvDial.js'],

declare var hx: ifSugar;
import __KERNEL__ = require("./dl_kernel");

/**************************************************
 * Definitions
 **************************************************/
var EAPP_LAUNCHED = 0;
var EAPP_STOPPED = 1;
var EAPP_DELETED = 2;
var ESERVER_STARTED = 3;
var ESERVER_STOPPED = 4;
var EDIAL_STATUS = {
    STOP : 0,
    RUNNING : 1
};
hx.logadd('dsrvDial');

var fnLog = function(log) {
    hx.log('dsrvDial', '#####' + log);
};
class cDLDial extends __KERNEL__ {
    mDial = this.getPluginInstance('Dial');
    mDialServer = this.mDial.server;
    constructor(aServiceName: string) {
        super(aServiceName);
        this.defineEvents(['DialStatusEvent', 'ReqLaunch', 'ReqStop']);

        this.mDialServer.addEventListener('DialStatusEvent', (e) => {
            this.onDialStatusEvent(e.status);
        });
        this.mDialServer.addEventListener('ReqLaunch', (e) => {
            this.onReqLaunch(e.name, e.url);
        });
        this.mDialServer.addEventListener('ReqStop', (e) => {
            this.onReqStop(e.name);
        });
    }
    start(): void {
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog('[fnStart]');
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        if (this.isStarted()) {
            this.registApplication('YouTube');
            if (hx.svc.APP_MGR.isAppOpened({'name':'YouTube'})) {
                this.updateApplicationState('YouTube', EDIAL_STATUS.RUNNING);
            }
            this.registApplication('Netflix');
            if (hx.svc.NETFLIX.isRunning()) {
                this.updateApplicationState('Netflix', EDIAL_STATUS.RUNNING);
            }
        }
        else {
            //this.mDialServer.start();
            // DIAL is controlled by MW
        }
    }
    stop(): void {
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog('[fnStop]');
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        this.mDialServer.stop();
    }
    isStarted(): boolean {
        var started = this.mDialServer.started;
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('[fnIsStarted] - started = {0}', started));
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        return started;
    }
    registApplication(aName): void {
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('[fnRegistApplication] - name = {0}', aName));
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        this.mDialServer.registApplication(aName);
    }
    unregistApplication(aName): void {
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('[fnUnregistApplication] - name = {0}', aName));
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        this.mDialServer.unregistApplication(aName);
    }
    updateApplicationState(aName, aState): void {
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('[fnUpdateApplicationState] - name = {0}', aName));
        fnLog(hx.util.format('[fnUpdateApplicationState] - state = {0}', aState));
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        this.mDialServer.updateApplicationState(aName, aState);
    }

    onDialStatusEvent(status): void {
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('[onDialStatusEvent] - status = {0}', status));
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        switch(status) {
            case ESERVER_STARTED:
                this.registApplication('YouTube');
                if (hx.svc.APP_MGR.isAppOpened({'name':'YouTube'})) {
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
    }
    onReqLaunch(name, url?): void {
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('[onReqLaunch] - name = {0}', name));
        fnLog(hx.util.format('[onReqLaunch] - url = {0}', url));
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        this.fireEvent('ReqLaunch', {
            name : name,
            url : url
        });
    }
    onReqStop(name): void {
        fnLog('##################################################');
        fnLog('--------------------------------------------------');
        fnLog(hx.util.format('[onReqStop] - name = {0}', name));
        fnLog('--------------------------------------------------');
        fnLog('##################################################');
        this.fireEvent('ReqStop', {
            name : name
        });
    }
}
export = cDLDial;

