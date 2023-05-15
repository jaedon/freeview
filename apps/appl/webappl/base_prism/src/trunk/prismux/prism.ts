/// <reference path="../def/sugar.d.ts" />
// file name : prismux/prism.js
/*jslint sloppy:true, nomen:true, vars:true */
import __NETFLIX__ = require("layer_domain/service/dl_dsrvNetflix");

declare var hx: any;

interface Window {
    tr: (aString: string) => string;
}

module mPrism {
    var fnFindReceiverInfo = function(a, eventName, receiver) {
        var receivers = a._eventReceivers[eventName], i, len = receivers.length, receiverInfo;
        for ( i = 0; i < len; i++) {
            receiverInfo = receivers[i];
            if (receiverInfo.receiver == receiver) {
                return receiverInfo;
            }
        }
        return null;
    };
    export var getTimeText = function(date) {
        return date.format("ddd HH:MM");
    };
    export class CEventSource {
        _eventReceivers: any = {};
        _registerEvent(eventList) {
            var a = this, i, eventName;
            var eventReceivers = a._eventReceivers || {};
            for ( i = 0; i < eventList.length; i++) {
                eventName = eventList[i];
                if (eventReceivers[eventName]) {
                    console.error('EVENT ALREADY DEFINED');
                    throw "EVENT ALREADY DEFINED";
                }
                eventReceivers[eventName] = [];
            }
            a._eventReceivers = eventReceivers;
        }
        destroy() {
            var a = this;
            a._eventReceivers = null;
        }
        addEventHandler(eventName, receiver, handler) {
            var a = this, receiverInfo = fnFindReceiverInfo(a, eventName, receiver);
            if (receiverInfo) {
                receiverInfo.handlers.push(handler);
            } else {
                a._eventReceivers[eventName].push({
                    receiver : receiver,
                    handlers : [handler]
                });
            }
        }
        removeEventHandler(receiver, eventName?) {
            var a = this, receivers = a._eventReceivers[eventName], removeReceiver = function(receivers, receiver) {
                var i, len = receivers.length, receiverInfo, index = -1;
                for ( i = 0; i < len; i++) {
                    receiverInfo = receivers[i];
                    if (receiverInfo.receiver == receiver) {
                        delete receiverInfo.receiver;
                        delete receiverInfo.handlers;
                        index = i;
                    }
                }
                if (index != -1) {
                    receivers.splice(index, 1);
                    return true;
                }
                return false;
            }, key;
            if (receivers) {
                removeReceiver(receivers, receiver);
            } else {
                for (key in a._eventReceivers) {
                    receivers = a._eventReceivers[key];
                    removeReceiver(receivers, receiver);
                }
            }
        }
        _fireEvent(aEventName: string, ...aArgument: any[]) {
            var a = this, args = $.makeArray(arguments), eventName = args.shift(), receivers = a._eventReceivers[eventName], len = receivers.length, receiverInfo, i, j, len2, receiver, handlers;
            for ( i = 0; i < len; i++) {
                receiverInfo = receivers[i];
                receiver = receiverInfo.receiver;
                handlers = receiverInfo.handlers;
                len2 = handlers.length;
                for ( j = 0; j < len2; j++) {
                    handlers[j].apply(receiver, args);
                }
            }
            if (a['on' + eventName]) {
                a['on' + eventName].apply(a, args);
            }
        }
    }

    export var getDateText = function(date) {
        return date.format("dd.mm.yyyy");
    };
    export class cClock extends CEventSource {
        private clockTimer;
        constructor() {
            super();
            this._registerEvent(['SecondChanged', 'MinuteChanged', 'HourChanged', 'DayChanged', 'UpdateTimeText', 'UpdateDateText']);
            //this.startClock();
        }
        startClock() {
            var now = new Date();
            var date = now;
            var sec = now.getSeconds();
            var min = now.getMinutes();
            var hour = now.getHours();
            var day = now.getDate();
            this.clockTimer = setInterval(() => {
                var now = new Date();
                if (30000 < Math.abs(now.getTime() - date.getTime()) || Math.abs(now.getTimezoneOffset() - date.getTimezoneOffset())) {
                    sec = -1;
                    min = -1;
                    hour = -1;
                    day = -1;
                }
                date = now;
                if (sec != now.getSeconds()) {
                    if (min != now.getMinutes()) {
                        if (hour != now.getHours()) {
                            if (day != now.getDate()) {
                                day = now.getDate();
                                this._fireEvent('DayChanged', now);
                                this._fireEvent('UpdateDateText', getDateText(now));
                            }
                            hour = now.getHours();
                            this._fireEvent('HourChanged', now);
                        }
                        min = now.getMinutes();
                        this._fireEvent('MinuteChanged', now);
                        this._fireEvent('UpdateTimeText', getTimeText(now));
                    }
                    sec = now.getSeconds();
                    this._fireEvent('SecondChanged', now);
                }
            }, 1000);
        }
        stopClock() {
            clearInterval(this.clockTimer);
        }
    }
    export var clock = new cClock();
    export var drawers = {
        /*
         * profile {
         *     ProfileName: 'Name',
         *     PhotoUrl: 'URL'
         * }
         */
        profile : function(param) {
            var $item = param.$item, item = param.item;
            $item.addClass('profileItem');
            var $picture = $('<div>', {
                'class' : 'profilePicture'
            });
            var $name = $('<div>', {
                'class' : 'profileName'
            });
            if (item) {
                $picture.css('background-image', 'url(' + item.PhotoUrl + ')');
                $name.text(item.ProfileName);
            }
            $item.append($picture).append($name);
        },
        storage : function(param) {
            var $item = param.$item, item = param.item;
            var $picture = $('<div>', {
                'class' : 'storage-selection'
            });
            var $name = $('<div>', {
                'class' : 'storage-label'
            });
            if (item) {
                $picture.addClass('-'+item.type);
                $name.text(item.name);
            }
            $item.append($picture).append($name);
        },
        storage2 : function(param) {
            var $item = param.$item, item = param.item;
            var $picture = $('<div>', {
                'class' : 'storage-selection'
            });
            var $name = $('<div>', {
                'class' : 'storage-label'
            });
            if (item) {
                $picture.addClass('-' + item.getStorageCssClass());
                $name.text(item.getName());
            }
            $item.append($picture).append($name);
        },
        usbWizard : function(param) {
            var $item = param.$item, item = param.item;
            var $picture = $('<div>', {
                'class' : 'storage-selection'
            });
            var $name = $('<div>', {
                'class' : 'storage-label'
            });
            if (item) {
                $picture.addClass(item.cssClass);
                $name.text(item.name);
            }
            $item.append($picture).append($name);
        }
    };

    var mGlobalDialogApi = null;
    var mOnGoingDialogOpenParam = [];
    var mOnGoingDialogRemovedIdx = -1;
    var mOnGoingDialogOpenCount = 0;
    var mDialogOpenParamAfterClosed = [];

    export var drawStorageGraph = function(used, available, xPos, yPos, radiusValue, canvasId) {
        var that = this, rightblock, usednum = Math.round(used), availnum = Math.round(available);

        var x = xPos, y = yPos, radius = radiusValue;
        var canvas;
        var ctx;

        if ( canvasId instanceof HTMLCanvasElement) {
            canvas = canvasId;
        } else {
            canvas = document.getElementById(canvasId);
        }

        ctx = canvas.getContext("2d");
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        var myColor = ["#FFFF00", "#414141"];
        var myData = [((usednum / availnum) * 100), (100 - (usednum / availnum) * 100)];

        function getTotal() {
            var j;
            var myTotal = 0;
            for ( j = 0; j < myData.length; j += 1) {
                myTotal += ( typeof myData[j] == 'number') ? myData[j] : 0;
            }
            return myTotal;
        }

        function plotData() {
            var lastend = Math.PI * 1.5, myTotal = getTotal(), i;

            for ( i = 0; i < myData.length; i += 1) {
                ctx.fillStyle = myColor[i];
                ctx.beginPath();
                ctx.moveTo(x, y);
                ctx.shadowColor = "#000000";
                //ctx.shadowBlur = 10;
                ctx.arc(x, y, radius, lastend, lastend + (Math.PI * 2 * (myData[i] / myTotal)), false);
                radius = radius - 8;
                ctx.lineTo(x, y);
                ctx.fill();
                lastend += Math.PI * 2 * (myData[i] / myTotal);
            }
        }

        plotData();

        myColor = ["#000000"];
        myData = [100];
        radius = 20;
        plotData();
    };

    var drawStorageGraphRect = function(used, available, xPos, yPos, canvasId) {
        var usednum = Math.round(used), availnum = Math.round(available);

        var x = xPos, y = yPos;
        var canvas;
        var ctx;

        if ( canvasId instanceof HTMLCanvasElement) {
            canvas = canvasId;
        } else {
            canvas = document.getElementById(canvasId);
        }

        ctx = canvas.getContext("2d");
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        var myColor = ["#FFFF00", "#414141"];
        var myData = [((usednum / availnum) * 100) * 2, (100 - (usednum / availnum) * 100) * 2];

        function plotData() {
            ctx.fillStyle = myColor[0];
            ctx.shadowBlur = 0;
            ctx.fillRect(x, y + 1, myData[0], 25);

            ctx.fillStyle = "#FFFFFF";
            ctx.fillRect(x, y, 200, 1);
            ctx.fillRect(x, y+26, 200, 1);
        }

        plotData();
    };

    export var drawStorageGraphCircleType2 = function(used, available, xPos, yPos, radiusValueOfRing, radiusValue, canvasId) {
        var that = this, usednum = Math.round(used), availnum = Math.round(available);

        var x = xPos, y = yPos, radius = radiusValue;
        var canvas;
        var ctx;

        if ( canvasId instanceof HTMLCanvasElement) {
            canvas = canvasId;
        } else {
            canvas = document.getElementById(canvasId);
        }

        ctx = canvas.getContext("2d");
        ctx.clearRect(0, 0, canvas.width, canvas.height);

        function drawRing() {
            var startloc = Math.PI * 1.5;

            ctx.beginPath();
            ctx.arc(x, y, radiusValueOfRing, startloc, startloc + (Math.PI * 2), false);
            ctx.lineWidth = 10;
            ctx.globalAlpha = 0.5;
            ctx.strokeStyle = 'white';
            ctx.stroke();
        }

        function drawUsedCircle(percent, color) {
            var startloc = Math.PI * 1.5;

            ctx.fillStyle = color;
            ctx.beginPath();
            ctx.moveTo(x, y);
            ctx.shadowColor = "#000000";
            ctx.globalAlpha = 1;
            ctx.shadowBlur = 20;
            ctx.arc(x, y, radius, startloc, startloc + (Math.PI * 2 * (percent / 100)), false);
            ctx.lineTo(x, y);
            ctx.fill();
        }

        drawRing();
        drawUsedCircle(((usednum / availnum) * 100), "#FFFF00");
    };

    export var makeChannelNumberString = function(chNum) {
        var strNum = '' + chNum;
        var strLength = strNum.length;
        var tmp_idx;
        for ( tmp_idx = 0; tmp_idx < (4 - strLength); tmp_idx += 1) {
            strNum = '0' + strNum;
        }
        return strNum;
    };
    export var processRequest = function(self, layeredControl, callFunc, param, handler, asyncWaitMessage) {
        var waitingDlg = null;
        var finishedCalled = false;
        if (self[callFunc]) {
            var responseHandler = function(response) {
                if (waitingDlg) {
                    waitingDlg.close();
                }
                handler(response);
                finishedCalled = true;
            };
            var response = self[callFunc](responseHandler, param);
            if (response) {
                responseHandler(response);
            } else {
                if (!finishedCalled) {
                    waitingDlg = (<any>window).prism['createWaitingNotificationDlg'](layeredControl || self);
                    if (asyncWaitMessage) {
                        waitingDlg.setTextHtml(asyncWaitMessage);
                    } else {
                        waitingDlg.setTextHtml((<any>window).tr('LOC_PLEASE_WAIT_ID'));
                    }
                    //TODO onclose
                    waitingDlg.open();
                }
            }
        }
    };

    export var focusInterrupt = function(layer, bOn, userCSS) {
        var css = userCSS || '.-afd-leaf';
        var cssInterrupted = '-interrupted';
        if (!layer) {
            return;
        }
        if (bOn) {
            layer.find(css).addClass(cssInterrupted);
        } else {
            layer.find(css).removeClass(cssInterrupted);
        }
    };
    export var log = function(msg) {
        console.log(msg);
    };
    if (window.debug) {
        hx.logadd('PrismUX');
        log = function(msg) {
            hx.log('PrismUX', msg);
        };
    }

    export function setGlobalDialogApi(appRpcUtil) {
        var globalDlgApi = {
            create: function() {
                log("global dialog: create");
                var a = this;
                var args = $.makeArray(arguments);
                args.unshift('create');
                a._call.apply(a, args);
            },
            open: function(param) {
                log("global dialog: open");
                var a = this;
                a.show();
                a._call('open', param);
            },
            close: function() {
                log("global dialog: close");
                var a = this;
                a._call('close');
            },
            setData: function(param) {
                log("global dialog: setData");
                var a = this;
                a._call('setData', param);
            },
            setStatusItem: function(param) {
                log("global dialog: setStatusItem");
                var a = this;
                a._call('setStatusItem', param);
            },
            notifyClosed: function(fn) {
                log("global dialog: notifyClosed");
                var a = this;
                a._notify('notifyClosed', fn);
            },
            notifyClosing: function(fn) {
                log("global dialog: notifyClosing");
                var a = this;
                a._notify('notifyClosing', fn);
            },
            notifyButtonSelected: function(fn) {
                log("global dialog: notifyButtonSelected");
                var a = this;
                a._notify('notifyButtonSelected', fn);
            },
            notifyItemSelected: function(fn) {
                log("global dialog: notifyItemSelected");
                var a = this;
                a._notify('notifyItemSelected', fn);
            },
            setLocalizationText: function(locStrList) {
                log("global dialog: setLocalizationText");
                var a = this;
                a._call('setLocalizationText', locStrList);
            },
            notifyCreateGlobalDlg : function(fn) {
                log("global dialog: notifyCreateGlobalDlg ");
                var a = this;
                a._notify('notifyCreateGlobalDlg', fn);
            },
            notifyStatisticsKeyDown : function(fn) {
                log("global dialog: notifyStatisticsKeyDown ");
                var a = this;
                a._notify('notifyStatisticsKeyDown', fn);
            }
        };
        mGlobalDialogApi = $.extend(appRpcUtil, globalDlgApi);
        mGlobalDialogApi.notifyClosed(function(param, index) {
            index = index - 1;
            if (mOnGoingDialogRemovedIdx >= 0 && index > mOnGoingDialogRemovedIdx) {
                index = index - 1;
            }
            var lastIdx = mOnGoingDialogOpenParam.length - 1;
            if (lastIdx >= 0 && index > lastIdx) {
                index = lastIdx;
            }
            mOnGoingDialogOpenCount--;
            if (mOnGoingDialogOpenParam[index]) {
                if (mOnGoingDialogOpenParam[index].onClosed) {
                    mOnGoingDialogOpenParam[index].onClosed(param);
                }
                mOnGoingDialogOpenParam.splice(index, 1);
                mOnGoingDialogRemovedIdx = index;
            }
            if (mOnGoingDialogOpenCount <= 0) {
                mOnGoingDialogOpenParam = [];
                mOnGoingDialogOpenCount = 0;
                mOnGoingDialogRemovedIdx = -1;
            }
        });
        mGlobalDialogApi.notifyClosing(function(param, index) {
            index = index - 1;
            if (mOnGoingDialogRemovedIdx >= 0 && index > mOnGoingDialogRemovedIdx) {
                index = index - 1;
            }
            var lastIdx = mOnGoingDialogOpenParam.length - 1;
            if (lastIdx >= 0 && index > lastIdx) {
                index = lastIdx;
            }
            if (mOnGoingDialogOpenParam[index]) {
                if (mOnGoingDialogOpenParam[index].onClosing) {
                    mOnGoingDialogOpenParam[index].onClosing(param);
                }
            }
        });
        mGlobalDialogApi.notifyButtonSelected(function(param, index) {
            index = index - 1;
            if (mDialogOpenParamAfterClosed[index] && param.type === 'OK') {
                if (mDialogOpenParamAfterClosed[index].onOk) {
                    mDialogOpenParamAfterClosed[index].onOk(param);
                }
                mDialogOpenParamAfterClosed.splice(index, 1);
            }
        });
        mGlobalDialogApi.notifyItemSelected(function(param, index) {
            index = index - 1;
            if (mOnGoingDialogRemovedIdx >= 0 && index > mOnGoingDialogRemovedIdx) {
                index = index - 1;
            }
            var lastIdx = mOnGoingDialogOpenParam.length - 1;
            if (lastIdx >= 0 && index > lastIdx) {
                index = lastIdx;
            }
            if (mOnGoingDialogOpenParam[index] && param.type === 'ItemSelected') {
                if (mOnGoingDialogOpenParam[index].onItemSelected) {
                    mOnGoingDialogOpenParam[index].onItemSelected(param);
                }
            }
        });
        mGlobalDialogApi.notifyCreateGlobalDlg(function(param) {
            hx.svc.SETTING_UTIL.showMsgbox();
        });
        mGlobalDialogApi.notifyStatisticsKeyDown(function(keycode) {
            if (hx.svc.STATISTICS) {
                hx.svc.STATISTICS.statisticsLog({
                    "CODE" : "KI", //KEY_INPUT
                    "TRS" : "I",
                    "KEY" : keycode
                });
            }

        });
    }
    export function createGlobalDialog() {
        var args = $.makeArray(arguments);
        if (hx.config.netflix) {
            var bValidOnNetflix = false;
            args.filter((element, index, array) => {
                if (typeof array[index] === "object") {
                    bValidOnNetflix = array[index].validOnNetflix || false;
                }
            });
            if (!bValidOnNetflix && hx.svc.NETFLIX.isRunning()) {
                hx.log("info","[createGlobalDialog] .... return due to run Netflix !");
                return;
            }
        }
        mGlobalDialogApi.setLocalizationText({
            LOC_YES_ID : hx.l('LOC_YES_ID'),
            LOC_NO_ID : hx.l('LOC_NO_ID'),
            LOC_OK_ID : hx.l('LOC_OK_ID'),
            LOC_CANCEL_ID : hx.l('LOC_CANCEL_ID'),
            LOC_TVAPPS_NETWORK_ERROR_ID : hx.l('LOC_TVAPPS_NETWORK_ERROR_ID'),
            LOC_TVAPPS_NETWORK_ERROR_TEXT_ID : hx.l('LOC_TVAPPS_NETWORK_ERROR_TEXT_ID'),
            LOC_EXPLORE_THE_VARIETY_OF_TV_APPS_ID : hx.l('LOC_EXPLORE_THE_VARIETY_OF_TV_APPS_ID'),
            LOC_CREATE_AN_ACCOUNT_FIRST_ID : hx.l('LOC_CREATE_AN_ACCOUNT_FIRST_ID'),
            LOC_HELP_TV_APPS_STEP_3_TITLE_1_ID : hx.l('LOC_HELP_TV_APPS_STEP_3_TITLE_1_ID'),
            LOC_CATCH_UP_FREEVIEW_ID : hx.l('LOC_CATCH_UP_FREEVIEW_ID'),
            LOC_HELP_TV_APPS_STEP_2_SCENE_1_TXT_1_ID : hx.l('LOC_HELP_TV_APPS_STEP_2_SCENE_1_TXT_1_ID'),
            LOC_HELP_TV_APPS_STEP_2_SCENE_2_TXT_1_ID : hx.l('LOC_HELP_TV_APPS_STEP_2_SCENE_2_TXT_1_ID'),
            LOC_HELP_TV_APPS_STEP_3_SCENE_1_TXT_1_ID : hx.l('LOC_HELP_TV_APPS_STEP_3_SCENE_1_TXT_1_ID'),
            LOC_HELP_TV_APPS_STEP_3_SCENE_2_TXT_1_ID : hx.l('LOC_HELP_TV_APPS_STEP_3_SCENE_2_TXT_1_ID'),
        });
        mGlobalDialogApi.create.apply(mGlobalDialogApi, args)
        return {
            open: function(param) {
                mOnGoingDialogOpenCount++;
                mDialogOpenParamAfterClosed.push(param || {});
                mOnGoingDialogOpenParam.push(param || {});
                mGlobalDialogApi.open(param);
                if (hx.config.netflix) {
                    if (hx.svc.NETFLIX.isRunning()) {
                        hx.svc.NETFLIX.notifyOverlayState(__NETFLIX__.OVERLAY_POPUP_STATE.OPEN_WITH_KEY);
                    }
                }
            },
            close: function() {
                mGlobalDialogApi.close();
            },
            setStatusItem : function(param) {
                mGlobalDialogApi.setStatusItem(param);
            },
            setData: function(param) {
                mGlobalDialogApi.setData(param);
            }
        };
    }
    export function isGlobalDialogOpened() {
        return (mOnGoingDialogOpenParam.length > 0) ? true: false;
    }

    if ((<any>window).tr === undefined) {
        (<any>window).tr = function(text) {
            return text;
        };
    }
    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    (<any>window).prism.getTimeText = getTimeText;
    (<any>window).prism.getDateText = getDateText;
    (<any>window).prism.clock = clock;
    (<any>window).prism.drawers = drawers;
    (<any>window).prism.drawStorageGraphRect = drawStorageGraphRect;
    (<any>window).prism.drawStorageGraphCircleType2 = drawStorageGraphCircleType2;
    (<any>window).prism.makeChannelNumberString = makeChannelNumberString;
    (<any>window).prism.processRequest = processRequest;
    (<any>window).prism.focusInterrupt = focusInterrupt;
    (<any>window).prism.log = log;
    (<any>window).prism.setGlobalDialogApi = setGlobalDialogApi;
    (<any>window).prism.createGlobalDialog = createGlobalDialog;
    (<any>window).prism.isGlobalDialogOpened = isGlobalDialogOpened;
}