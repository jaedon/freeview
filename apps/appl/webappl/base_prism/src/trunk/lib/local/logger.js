define(["require", "exports"], function (require, exports) {
    /// <reference path="../../def/sugar.d.ts" />
    var mLogger;
    (function (mLogger) {
        var COLOR = {
            reset: '\033[22;39m',
            info: '\033[01;34m',
            debug: '\033[01;32m',
            warn: '\033[22;33m',
            error: '\033[22;31m',
            time: '\033[01;37m',
            timeEnd: '\033[01;37m',
            normal: '\033[01;36m'
        };
        var cLogger = (function () {
            function cLogger() {
                this._defaultClass = ['global'];
                this._consoleDirect = {
                    info: 'info',
                    debug: 'debug',
                    warning: 'warn',
                    warn: 'warn',
                    error: 'error',
                    start: 'time',
                    end: 'timeEnd'
                };
                this._consoleClass = ['info', 'debug', 'warning', 'warn', 'error', 'start', 'end'];
                this.clear();
            }
            cLogger.prototype.add = function (aKeyword) {
                if (this._class.indexOf(aKeyword) < 0) {
                    this._class.push(aKeyword);
                }
            };
            cLogger.prototype.remove = function (aKeyword) {
                /**DEVCODE*/
                if (this._defaultClass.indexOf(aKeyword) > -1) {
                    alert("[" + aKeyword + "] is a fixed keyword");
                    return;
                } /*DEVCODE**/
                var ar = this._class;
                var len = ar.length;
                if (len > 1) {
                    var index = ar.indexOf(aKeyword);
                    if (index > -1) {
                        ar.splice(index, 1);
                    }
                }
            };
            cLogger.prototype.clear = function () {
                this._class = this._defaultClass.concat(this._consoleClass);
            };
            return cLogger;
        })();
        var logger = new cLogger();
        function log(aType, aLog) {
            if (!logger._class || logger._class.indexOf(aType) < 0) {
                return;
            }
            var con = logger._consoleDirect[aType];
            if (con) {
                console[con](aLog);
                if ((!mLogger.emulMode || hx.config.emulOnBox) && window.debug) {
                    var time = dateFormat(Date.now(), "MM:ss");
                    if (con === 'time' || con === 'timeEnd') {
                        window.debug(COLOR[con] + time + ": " + aType + ":\t" + aLog + ":\t" + new Date().valueOf() + COLOR.reset);
                    }
                    else {
                        window.debug(COLOR[con] + time + ": " + aType + ":\t" + aLog + COLOR.reset);
                    }
                }
                return;
            }
            var c = aType.split(','), i;
            for (i = 0; i < c.length; i += 1) {
                if (logger._class.indexOf(c[i]) > -1) {
                    console.log(":" + aType + ":\t" + aLog);
                    if ((!mLogger.emulMode || hx.config.emulOnBox) && window.debug) {
                        window.debug(COLOR.normal + dateFormat(Date.now(), "MM:ss") + aType + " :\t" + aLog + COLOR.reset);
                    }
                    break;
                }
            }
        }
        mLogger.log = log;
        log.prototype = {
            exist: function (aType) {
                return (logger._class && logger._class.indexOf(aType) > -1);
            },
            add: function (aType) {
                logger.add(aType);
            },
            remove: function (aType) {
                logger.remove(aType);
            }
        };
        mLogger.emulMode;
        mLogger.logadd = log.prototype.add;
        mLogger.logremove = log.prototype.remove;
    })(mLogger || (mLogger = {}));
    return mLogger;
});
