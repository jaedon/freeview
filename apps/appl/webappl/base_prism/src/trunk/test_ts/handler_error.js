/**
 * Created by deptno on 2/14/15.
 */
define(['require', 'exports'], function(r, e) {
    function handler(ex) {
        if (handler._instance) {
            return handler._instance;
        }
        function parse(exception) {
            var ret = {
                type: "",
                message: "",
                stack: "",
                filename: "",
                lineno: -1,
                colno: -1,
                typestamp: -1
            };
            if (exception instanceof ErrorEvent) {
                for (var key in ret) {
                    ret[key] = exception[key];
                }
            } else if(exception instanceof Error) {
                ret.message = exception.message;
                ret.stack = exception.stack;
            } else {
                ret.message = exception;
            }
        }
        window.addEventListener('error', function(exception) {
            this._stack.push(exception);
            var type = parse(exception);
            for (var i = 0; i < this._receivers.length; i++) {
                this._receivers[i](exception);
            }
        });
    }
    handler._instance = null;
    this._receivers = [];
    this._stack = [];
    handler.prototype.get = function() {
        return this._stack.shift();
    };
    handler.prototype.length = function() {
        return this._stack.length;
    };
    handler.prototype.receivers = function() {
        return this._receivers.length;
    };
    handler.prototype.flush = function() {
        var ret = _.clone(this._stack, true);
        this._stack = [];
        return ret;
    };
    handler.prototype.subscribe = function(fn) {
    };
    handler.prototype.subscribe = function(fn) {
    };

    return handler;
});
