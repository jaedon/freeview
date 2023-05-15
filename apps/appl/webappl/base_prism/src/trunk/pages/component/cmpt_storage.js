///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cStorage.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var storageInfo = (function (_super) {
        __extends(storageInfo, _super);
        function storageInfo() {
            _super.call(this);
        }
        storageInfo.prototype.init = function () {
            // TODO:
        };
        storageInfo.prototype.create = function (aParam) {
            $.extend(this, aParam, {
                $: $('<dl>', {
                    'class': ''
                }),
                _$title: $('<dt>', {
                    'text': hx.l('LOC_TOTAL_STORAGE_ID')
                }),
                _$totalStorage: $('<dd>', {
                    'text': aParam.totalStorage
                }),
                _$title3: $('<dt>', {
                    'text': hx.l('LOC_USER_USAGE_ID')
                }),
                _$userUsage: $('<dd>', {
                    'text': aParam.userUsage
                }),
                _$title4: $('<dt>', {
                    'class': 'foot',
                    'text': hx.l('LOC_AVAILABLE_ID')
                }),
                _$available: $('<dd>', {
                    'class': 'foot',
                    'text': aParam.available
                })
            });
            this.$.append(this._$title).append(this._$totalStorage).append(this._$title3).append(this._$userUsage).append(this._$title4).append(this._$available);
            if (aParam.systemUsage !== 'external') {
                $.extend(this, {
                    _$title2: $('<dt>', {
                        'text': hx.l('LOC_SYSTEM_USAGE_ID')
                    }),
                    _$systemUsage: $('<dd>', {
                        'text': aParam.systemUsage
                    })
                });
                this._$totalStorage.after(this._$title2.after(this._$systemUsage));
            }
            return this;
        };
        storageInfo.prototype.destroy = function () {
            this.$.remove();
        };
        return storageInfo;
    })(__COMPONENT_KERNEL__);
    exports.storageInfo = storageInfo;
    var storageGraph = (function (_super) {
        __extends(storageGraph, _super);
        function storageGraph() {
            _super.call(this);
        }
        storageGraph.prototype.init = function () {
            var ini;
        };
        storageGraph.prototype.create = function (aParam) {
            $.extend(this, aParam, {
                $: $('<aside>'),
                _$figure: $('<figure>'),
                _$canvas: $('<canvas>', {
                    'id': 'divStorageDlgRight',
                    'style': 'width:350px; height:180px; top:70px;'
                }),
                _$used: $('<p>'),
                _$available: $('<p>')
            });
            this._$used.html('<em>' + aParam.used + '</em>' + hx.l('LOC_USED_ID'));
            this._$available.html('<em>' + aParam.available + '</em>' + hx.l('LOC_SMALLAVAILABLE_ID'));
            this.$.append(this._$figure.append(this._$canvas)).append(this._$used).append(this._$available);
            return this;
        };
        storageGraph.prototype.destroy = function () {
            this.$.remove();
        };
        return storageGraph;
    })(__COMPONENT_KERNEL__);
    exports.storageGraph = storageGraph;
    var strPincode = (function (_super) {
        __extends(strPincode, _super);
        function strPincode() {
            _super.call(this);
        }
        strPincode.prototype.create = function (aParam) {
            $.extend(this, aParam);
            this.$ = $('<div>', {
                'class': 'pas',
                'style': 'top:270px;'
            });
            this._$txt = $('<p>');
            this._$txt.html(hx.l('LOC_ENTER_PASSWORD_ID'));
            this._$pin = $('<p>', {
                'class': 'wrap_pw'
            });
            if (this.baseText === undefined) {
                this.baseText = "-";
            }
            if (this.count === undefined) {
                this.count = 1;
            }
            this._$items = [];
            var n;
            for (n = 0; n < this.count; n += 1) {
                this._$items.push($('<em>').text(this.baseText));
                this._$items[n].addClass('button');
                if (n === 0) {
                    this._$items[n].addClass('on');
                }
                this._$pin.append(this._$items[n]);
            }
            this.$.append(this._$txt).append(this._$pin);
            return this;
        };
        strPincode.prototype.destroy = function () {
            this.$.empty();
        };
        strPincode.prototype.getNum = function () {
            var i;
            for (i = 0; i < this.count; i += 1) {
                if (i === 0) {
                    var Num = this._$items[i].text();
                }
                else {
                    Num += this._$items[i].text();
                }
            }
            return Num;
        };
        strPincode.prototype.position = function (aIndex) {
            if (aIndex === undefined) {
                var on = this._$pin.children('.on');
                if (on.length === 0) {
                    return undefined;
                }
                return on.prevAll().length;
            }
            if (aIndex >= 0 && aIndex < this.count) {
                this._$pin.children().removeClass('on');
                this._$items[aIndex].addClass('on');
            }
        };
        strPincode.prototype.clear = function () {
            this._$pin.children().removeClass('on');
        };
        //PRIVATE
        strPincode.prototype._on_alKey = function (aParam) {
            if (aParam.alKey >= hx.key.KEY_0 && aParam.alKey <= hx.key.KEY_9) {
                var num = aParam.alKey - hx.key.KEY_0;
                return this._move(aParam, function (index, obj) {
                    obj.text(num);
                    return index + 1;
                });
            }
            else if (aParam.alKey === hx.key.KEY_LEFT) {
                return this._move(aParam, function (index) {
                    return index - 1;
                });
            }
            else if (aParam.alKey === hx.key.KEY_RIGHT) {
                return this._move(aParam, function (index) {
                    return index + 1;
                });
            }
            else if (aParam.alKey === hx.key.KEY_OK) {
                aParam.alEvent = 'alClicked';
            }
            else if (aParam.alKey === hx.key.KEY_BACK) {
                aParam.alEvent = 'alExit';
            }
            return false;
        };
        strPincode.prototype._move = function (aParam, cb) {
            var on = this._$pin.children('.on');
            if (on.length === 0) {
                return true;
            }
            var index = cb(on.prevAll().length, on);
            this.position(index);
            return false;
        };
        return strPincode;
    })(__COMPONENT_KERNEL__);
    exports.strPincode = strPincode;
});
