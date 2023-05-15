define(["require", "exports"], function (require, exports) {
    ///<reference path="../def/sugar.d.ts" />
    var _EVENT_ = {
        alKey: {},
        alForwardKey: {},
        alNextKey: {},
        alNextpage: {},
        alClicked: {},
        alScrollup: {},
        alScrolldown: {},
        alScrollRight: {},
        alScrollLeft: {},
        alChange: {},
        alUpdate: {},
        alChangeFocus: {},
        alExit: {},
        alBack: {},
        alChangeItem: {},
        alNumberOverflow: {},
        alNumberUnderflow: {},
        algetIpAddr: {},
        alEndOfRepeatKey: {},
        alMessageToPage: {},
        alPagedown: {},
        alPageup: {},
        alNumber: {},
        alMostTopPage: {},
        alResetOrdering: {},
        alChangeInstallPage: {},
        alChangeServiceGroup: {},
        //for grab
        alCheckPageVisible: {},
        alPagesVisible: {}
    };
    var cALComponentKernel = (function () {
        function cALComponentKernel(events) {
            this._events = events || _EVENT_;
        }
        cALComponentKernel.prototype.destroy = function () {
        };
        cALComponentKernel.prototype._currentChild = function () {
        };
        cALComponentKernel.prototype._doOnLatent = function () {
            var page = this.getPage();
            if (page && page.visible) {
                hx.svc.SETTING_UTIL.doingKeyGrab(page.visible());
            }
        };
        cALComponentKernel.prototype._doOffLatent = function () {
            var page = this.getPage();
            if (page && page.visible) {
                hx.svc.SETTING_UTIL.doingKeyGrab(page.visible());
            }
        };
        cALComponentKernel.prototype.print = function (num) {
            var branch = "+";
            var i;
            for (i = 1; i < num; i += 1) {
                branch += "--";
            }
            hx.log("warning", branch + "[COMPONENT:" + this.name + "]" + (this.focus ? '-ON' : ''));
            if (this._subObj !== undefined) {
                var n;
                for (n = 0; n < this._subObj.length; n += 1) {
                    this._subObj[n].print(num + 1);
                }
            }
        };
        cALComponentKernel.prototype.setName = function (aName) {
            if (!this.name) {
                this.name = aName;
            }
        };
        cALComponentKernel.prototype.getName = function () {
            return this.name;
        };
        /* PRIVATE */
        cALComponentKernel.prototype.append = function (obj) {
            if (!this._subObj) {
                this._subObj = [];
            }
            hx.log("composer", "APPEND:" + this.name + "-" + obj.name);
            obj._superObj = this;
            this._subObj.push(obj);
            return this;
        };
        cALComponentKernel.prototype.pickout = function () {
            if (!this._superObj) {
                return;
            }
            var parent = this._superObj;
            var slibling = parent._subObj;
            var i;
            for (i = 0; i < slibling.length; i += 1) {
                if (slibling[i] === this) {
                    if (this.destroy) {
                        this.destroy();
                    }
                    slibling.splice(i, 1);
                    break;
                }
            }
        };
        cALComponentKernel.prototype.isLeaf = function () {
            return !this._subObj;
        };
        cALComponentKernel.prototype.on = function (findLeaf) {
            hx.log('onComp', 'ON: ' + this.name);
            var pg;
            var page = this.getPage();
            if (!findLeaf || this.isLeaf()) {
                page._off();
                if (this.doFocus === undefined) {
                    if (this.$ !== undefined) {
                        hx.log("warning", "DEPRECATE: jquery.focus function");
                        this.$.focus();
                    }
                    else {
                        if (!this.isPage) {
                            hx.log("error", "not found '$ object' or 'doFocus() function' - [name:" + this.name + "]");
                        }
                    }
                }
                else {
                    this.doFocus();
                }
                pg = this._on();
            }
            else {
                var subobj = this._subObj[0];
                if (this._currentChild !== undefined) {
                    subobj = this._currentChild();
                }
                subobj.on();
                pg = this;
                while (pg._superObj !== undefined) {
                    pg = pg._superObj;
                }
            }
            //BACKUP for refocus process
            if (pg._superObj === undefined && pg._entity !== undefined) {
                pg._entity.pageFocused(pg);
            }
            this._doOnLatent();
        };
        cALComponentKernel.prototype._on = function () {
            var obj = this;
            if (obj._superObj === undefined) {
                if (!this.isPage) {
                    hx.log("error", "alComposer :: _superObj undefined :: _on function - [name:" + this.name + "]");
                }
                return obj;
            }
            var keTable = [false, false];
            var kMap = [], rMap = [];
            function _kChk(obj, keTable, kMap, rMap) {
                keTable[0] = keTable[0] || !!obj._on_alKey;
                keTable[1] = keTable[1] || !!obj._on_alEndOfRepeatKey;
                kMap.push(obj._keyTable);
                rMap.push(obj._keyRTable);
            }
            do {
                obj.focus = true;
                _kChk(obj, keTable, kMap, rMap);
                obj = obj._superObj;
            } while (!!obj._superObj);
            _kChk(obj, keTable, kMap, rMap);
            var o = obj._keyeventTable;
            if (o) {
                o.alKey = keTable[0];
                o.alEndOfRepeatKey = keTable[1];
                o.alKeyt = kMap;
                o.alEndOfRepeatKeyt = rMap;
            }
            else {
                hx.log("error", "Its not a page.");
            }
            return obj;
        };
        cALComponentKernel.prototype.off = function () {
            this._off();
            var sup = this._superObj;
            while (!!sup && sup.focus) {
                sup.focus = false;
                sup = sup._superObj;
            }
            this._doOffLatent();
        };
        cALComponentKernel.prototype._off = function () {
            if (!!this._subObj) {
                var ch = this._subObj;
                var i;
                for (i = 0; i < ch.length; i += 1) {
                    ch[i]._off();
                }
            }
            this.focus = false;
            if (this._childOff) {
                this._childOff();
            }
        };
        cALComponentKernel.prototype.getParent = function () {
            return this._superObj;
        };
        cALComponentKernel.prototype.getPage = function () {
            var obj = this;
            while (!!obj._superObj) {
                obj = obj._superObj;
            }
            return obj;
        };
        cALComponentKernel.prototype.doCommand = function (p) {
            /**DEVCODE*/
            hx.log("ComponentEventCatch", "-------------------------------------------");
            hx.log("ComponentEventCatch", "[ THIS: " + this.name + " ] - [ EVENT: " + p.alEvent + " ]");
            /*DEVCODE**/
            var consumed = false;
            /* CALL EVENT HANDLER !!!!! */
            var fx = this['_on_' + p.alEvent];
            if (fx && (this.__created__ === true)) {
                if (p.alEvent === 'alKey' || p.alEvent === 'alEndOfRepeatKey') {
                    //                try {
                    if (p.alPage) {
                        console.info('page throw debugging : ' + p.alPage.getName());
                    }
                    else {
                        console.info('not page throw debugging : ');
                    }
                    consumed = !!fx.call(this, p);
                }
                else {
                    consumed = !!fx.call(this, p);
                }
            }
            else {
                hx.log("ComponentEventCatch", "[ CREATED : " + this.__created__ + " ]");
            }
            hx.log("ComponentEventCatch", "[ FROM: " + p.alStarter + " ] - [ EVENT: " + p.alEvent + " ]");
            if (p.alStarter !== 'alEntity') {
                return;
            }
            if (consumed) {
                return consumed;
            }
            hx.log("ComponentEventCatch", "[ consumed: " + consumed + " ] - [ cut: " + (!!p.alCutEvent) + " ]");
            if (this._superObj) {
                /**DEVCODE*/
                hx.log("ComponentEventCatch", "[ Propergation Key ]");
                /*DEVCODE**/
                consumed = this._runEvent(this._superObj, p);
            }
            return consumed;
        };
        cALComponentKernel.prototype.sendEventToParent = function (param) {
            hx.log("ComponentEventCatch", "{sendEventToParent}");
            if (!this._superObj) {
                hx.log("warning", "!this._superObj");
                return;
            }
            var p = $.extend({}, param);
            p.alStarter = this.name;
            this._sendEvent(this._superObj, p);
        };
        cALComponentKernel.prototype._sendEvent = function (obj, param) {
            if (!obj) {
                hx.log("error", "Do not send to not defined object - return");
                return;
            }
            /**DEVCODE*/
            if (!this._events[param.alEvent]) {
                hx.log("error", "Not defined event " + param.alEvent + " -return");
                return;
            }
            /*DEVCODE**/
            if (param.alData === undefined) {
                param.alData = {};
            }
            param.alData.me = this;
            hx.log("ComponentEventCatch", "SEND [ TO : " + obj.name + " ] - [event : " + param.alEvent + " ]");
            obj.doCommand(param);
        };
        cALComponentKernel.prototype._runEvent = function (obj, param) {
            if (!obj) {
                hx.log("error", "Do not run to not defined object - return");
                return;
            }
            /**DEVCODE*/
            if (!this._events[param.alEvent]) {
                hx.log("error", "Not defined event " + param.alEvent + " -return");
                return;
            }
            /*DEVCODE**/
            if (param.alData === undefined) {
                param.alData = {};
            }
            param.alData.me = this;
            hx.log("ComponentEventCatch", "RUN-[ TO : " + obj.name + " ] - [event : " + param.alEvent + " ]");
            if (obj == hx.al.entity) {
                throw 'c8';
            }
            else {
                return obj.doCommand(param);
            }
        };
        /* COMMENT
         * from hx.cmpt.frame
         * * bglee@humaxdigital.com, 13:58, 2014. 5. 17.
         */
        cALComponentKernel.prototype._create = function (param, aGroup) {
            $.extend(this, param);
            if (this.tag === undefined) {
                this.tag = 'div';
            }
            this.tag = '<' + this.tag + '>';
            this.$ = $(this.tag, this.attr);
        };
        cALComponentKernel.prototype._destroy = function () {
            this.$.empty();
        };
        return cALComponentKernel;
    })();
    return cALComponentKernel;
});
