/**
 * Created by bglee@humaxdigital.com on 2014. 7. 25..
 */
/// <reference path="../../def/jquery.d.ts" />
/// <reference path="../../def/sugar.d.ts" />
define(["require", "exports"], function (require, exports) {
    var __logger;
    /*
     sizeof.js
    
     A function to calculate the approximate memory usage of objects
    
     Created by Stephen Morley - http://code.stephenmorley.org/ - and released under
     the terms of the CC0 1.0 Universal legal code:
    
     http://creativecommons.org/publicdomain/zero/1.0/legalcode
    
     */
    /* Returns the approximate memory usage, in bytes, of the specified object. The
     * parameter is:
     *
     * object - the object whose size should be determined
     */
    function sizeof(object) {
        // initialise the list of objects and size
        var objects = [object];
        var size = 0;
        for (var index = 0; index < objects.length; index++) {
            switch (typeof objects[index]) {
                case 'boolean':
                    size += 4;
                    break;
                case 'number':
                    size += 4;
                    break;
                case 'string':
                    size += 1 * objects[index].length;
                    break;
                case 'object':
                    // if the object is not an array, add the sizes of the keys
                    if (Object.prototype.toString.call(objects[index]) != '[object Array]') {
                        for (var key in objects[index])
                            size += 2 * key.length;
                    }
                    for (var key in objects[index]) {
                        // determine whether the value has already been processed
                        var processed = false;
                        for (var search = 0; search < objects.length; search++) {
                            if (objects[search] === objects[index][key]) {
                                processed = true;
                                break;
                            }
                        }
                        // queue the value to be processed if appropriate
                        if (!processed)
                            objects.push(objects[index][key]);
                    }
            }
        }
        // return the calculated size
        return size;
    }
    var CKeyCatcher = (function () {
        function CKeyCatcher(aExtFnLog, aFnAddHooker, aFnRemoveHooker) {
            var _this = this;
            this._queue = [];
            this._registeredKeyCodes = {};
            this._registeredKeyLong = {};
            this._registeredKeyCodesNumber = {};
            this._elBoard = document.createElement("div");
            this._textDiv = document.createElement("div");
            if (!aExtFnLog.log) {
                throw "argument0 must have \"log\" property";
            }
            __logger = aExtFnLog;
            if (aFnAddHooker && aFnRemoveHooker) {
                var elBoard = this._elBoard;
                var fnHooker = function (aKeyCode) {
                    var handled = false;
                    __logger.log("debug", "[CKeyCatcher] fnHookder : " + aKeyCode);
                    var num = aKeyCode - 48;
                    var len = Object.keys(_this._registeredKeyCodes).length;
                    if (num >= 0 && num < len) {
                        elBoard.children[num].style.color = "red";
                        for (var key in _this._registeredKeyCodes) {
                            if (num-- == 0) {
                                _this._registeredKeyCodes[key].fx();
                                setTimeout(function () {
                                    if (elBoard.parentNode) {
                                        elBoard.parentNode.removeChild(elBoard);
                                        while (elBoard.children[0]) {
                                            elBoard.removeChild(elBoard.children[0]);
                                        }
                                    }
                                }, 1000);
                                aFnRemoveHooker(fnHooker);
                                handled = true;
                                break;
                            }
                        }
                    }
                    if (!handled) {
                        for (var key in _this._registeredKeyCodesNumber) {
                            if (Number(key) === aKeyCode) {
                                _this._registeredKeyCodesNumber[key].fx();
                                handled = true;
                                break;
                            }
                        }
                    }
                    return handled;
                };
                var style = this._elBoard.style;
                style.position = "absolute";
                style.top = "30px";
                style.left = "240px";
                style.width = "1000px";
                style.height = "720px";
                style.border = "2px solid violet";
                style.opacity = ".7";
                style.background = "black";
                style.zIndex = "1000";
                var self = this;
                this.registerKeyCode([4, 5, 6, 3, 9, 7, 1], "<big><tt>Internal Fx</tt></big> - debugging info board show", function () {
                    if (elBoard.parentNode) {
                        if (elBoard.parentNode) {
                            elBoard.parentNode.removeChild(elBoard);
                            while (elBoard.children[0]) {
                                elBoard.removeChild(elBoard.children[0]);
                            }
                            aFnRemoveHooker(fnHooker);
                        }
                        return;
                    }
                    var count = 0;
                    var el;
                    for (var key in _this._registeredKeyCodes) {
                        el = document.createElement("div");
                        el.innerHTML = "[" + count++ + "] : " + _this._registeredKeyCodes[key].desc;
                        elBoard.appendChild(el);
                    }
                    for (var key in _this._registeredKeyCodesNumber) {
                        el = document.createElement("div");
                        el.innerHTML = "[" + count++ + "] : " + _this._registeredKeyCodesNumber[key].desc;
                        elBoard.appendChild(el);
                    }
                    document.body.appendChild(elBoard);
                    aFnAddHooker(fnHooker);
                    self.addTextDiv();
                    self.__reloadTextDiv();
                });
            }
        }
        CKeyCatcher.prototype.__reloadTextDiv = function () {
            var that = this;
            var elBoard = this._elBoard;
            this._textDiv.innerHTML = '';
            this.addTextDiv();
            setTimeout(function () {
                if (elBoard.parentNode) {
                    that.__reloadTextDiv();
                    __logger.log("debug", "__reloadTextDiv......");
                }
            }, 1000);
        };
        CKeyCatcher.prototype.addTextDiv = function () {
            var textDiv = this._textDiv;
            textDiv.id = 'textDiv';
            this._elBoard.appendChild(textDiv);
            var db = hx.svc.IP_EPG._cache_controller.getDb();
            var text = '';
            var index = 0;
            var metaKeys = Object.keys(db['meta']);
            var metaTotal = 0;
            //for(index =0;index<metaKeys.length;index++){
            //    metaTotal += sizeof(db['meta'][metaKeys[index]]);
            //}
            text += 'MDS Meta number : ' + metaKeys.length; // + ', Meta total byte : '+metaTotal;
            var imgKeys = Object.keys(db['image']);
            var imageTotal = 0;
            for (index = 0; index < imgKeys.length; index++) {
                //imageTotal += sizeof(db['image'][imgKeys[index]]);
                if (db['image'][imgKeys[index]] && db['image'][imgKeys[index]].data) {
                    imageTotal += db['image'][imgKeys[index]].data.length;
                }
            }
            text += ' / Image number : ' + imgKeys.length + ', Image total byte : ' + imageTotal;
            //Voice Assistance
            if (hx.config.voiceAssistant && hx.svc.VOICE_ASSISTANCE) {
                if (hx.svc.WEB_STORAGE.getAlexaDeviceEnable()) {
                    text += '</br>VOICE ASSISTANCE ENABLED Subscribed : ' + hx.svc.VOICE_ASSISTANCE.isSubscribed();
                    text += hx.svc.VOICE_ASSISTANCE.getStatus();
                }
                else {
                    text += '</br> VOICE ASSISTANT OPTION DISABLED';
                }
            }
            else {
                text += '</br> VOICE ASSISTANT DISABLED';
            }
            textDiv.innerHTML = text;
        };
        CKeyCatcher.prototype.registerKeyLong = function (aKeyCode, aDesc, aCallback) {
            this._registeredKeyLong[aKeyCode] = {
                fx: function () {
                    aCallback();
                },
                desc: aDesc
            };
            //this._registeredKeyCodes[key] = {
            //    fx: function() {
            //        aCallback();
            //    },
            //    desc: aDesc
            //};
            __logger.log("debug", "################################################################");
            __logger.log("debug", "### CKeyCatcher ################################################");
            //__logger.log("debug", "### " + key + " registered ###########################################");
            __logger.log("debug", "### description: " + "###############################################");
            __logger.log("debug", "### " + aDesc);
            __logger.log("debug", "################################################################");
        };
        CKeyCatcher.prototype.registerKeyCode = function (aNumbers, aDesc, aCallback) {
            var i;
            var len = 7;
            var key = "";
            if (aNumbers.length !== len) {
                throw "7 length required";
            }
            for (i = 0; i < len; i++) {
                if (typeof aNumbers[i] !== "number") {
                    throw "type must number";
                }
                if (aNumbers[i].toString().length !== 1) {
                    throw "array have one digit in each cell";
                }
                key += aNumbers[i].toString();
            }
            this._registeredKeyCodes[key] = {
                fx: function () {
                    aCallback();
                },
                desc: aDesc
            };
            __logger.log("debug", "################################################################");
            __logger.log("debug", "### CKeyCatcher ################################################");
            __logger.log("debug", "### " + key + " registered ###########################################");
            __logger.log("debug", "### description: " + "###############################################");
            __logger.log("debug", "### " + aDesc);
            __logger.log("debug", "################################################################");
        };
        CKeyCatcher.prototype.registerKeyCodeNumber = function (aNumbers, aDesc, aCallback) {
            var key = aNumbers;
            this._registeredKeyCodesNumber[key] = {
                fx: function () {
                    aCallback();
                },
                desc: aDesc
            };
            __logger.log("debug", "################################################################");
            __logger.log("debug", "### CKeyCatcher ################################################");
            __logger.log("debug", "### " + key + " registered ###########################################");
            __logger.log("debug", "### description: " + "###############################################");
            __logger.log("debug", "### " + aDesc);
            __logger.log("debug", "################################################################");
        };
        CKeyCatcher.prototype.input = function (aKeyCode, aRepeat) {
            var _this = this;
            if (aRepeat === void 0) { aRepeat = 0; }
            var ret = false;
            var queue = this._queue;
            var complete = 7;
            var item;
            var keyLong = aKeyCode;
            aKeyCode -= 48;
            if (this._registeredKeyLong[keyLong] && aRepeat > 5) {
                item = this._registeredKeyLong[keyLong];
                if (item) {
                    __logger.log("debug", keyLong + " runs!!");
                    __logger.log("debug", "description: " + item.desc);
                    item.fx();
                    queue.length = 0;
                    var fnBackup = this._registeredKeyLong[keyLong];
                    setTimeout((function (afnBackup) {
                        return function () {
                            _this._registeredKeyLong[keyLong] = afnBackup;
                        };
                    })(fnBackup), 2 * 1000);
                    this._registeredKeyLong[keyLong] = null;
                    ret = true;
                }
            }
            else if (aKeyCode >= 0 && aKeyCode <= 9) {
                if (queue.length === complete) {
                    queue.shift();
                }
                queue.push(aKeyCode);
                if (queue.length === complete) {
                    var key = queue.join("");
                    item = this._registeredKeyCodes[key];
                    if (item) {
                        __logger.log("debug", key + " runs!!");
                        __logger.log("debug", "description: " + item.desc);
                        item.fx();
                        queue.length = 0;
                        ret = true;
                    }
                }
            }
            else {
                queue.length = 0;
            }
            return ret;
        };
        return CKeyCatcher;
    })();
    return CKeyCatcher;
});
//# sourceMappingURL=key_catcher.js.map