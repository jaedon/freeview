///<reference path="../../def/sugar.d.ts" />
// file name : pages/live/dcn.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var cCPgDcn = (function (_super) {
        __extends(cCPgDcn, _super);
        function cCPgDcn() {
            _super.call(this);
            this._timerId = -1;
            this._bActive = false;
        }
        cCPgDcn.prototype.create = function () {
            this._bActive = false;
            this._timerId = -1;
            $.extend(this, {
                _$section: $('#ltv'),
                _$dim: $('<div>')
            });
            this.$wrap = $('#wrap');
            hx.al.compose(this);
            if (hx.config.netflix) {
                hx.svc.NETFLIX.addEventCb('HideChInfo', this, function (aParam) {
                    this._on_HideChInfo(aParam);
                });
            }
        };
        cCPgDcn.prototype.destroy = function () {
            this.__clear_dcn();
            this._dcn = undefined;
        };
        cCPgDcn.prototype.isActive = function () {
            return this._bActive;
        };
        //visible () : boolean {
        //    return this._bActive;
        //}
        // EVENT HANDLER
        cCPgDcn.prototype._on_alUpdate = function (aParam) {
            var _this = this;
            var data = aParam.alData;
            if (data) {
                if (data.result) {
                    switch (data.result) {
                        case 'setChannelFail':
                            if (hx.config.useTVStreaming === false) {
                                hx.msg('response', {
                                    text: hx.l('LOC_CANNOTCHANGEWHILEREC_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            else {
                                hx.msg('response', {
                                    text: hx.l('LOC_CANTCHANGECHANNELRECSTREAM_ID'),
                                    auto_close: true,
                                    close_time: 3000
                                });
                            }
                            break;
                        case 'setChannelSuccess':
                            break;
                    }
                    this.__clear_dcn();
                }
                else {
                    // update timer
                    if (this._bActive && (this._timerId !== -1)) {
                        clearTimeout(this._timerId);
                        this._timerId = setTimeout(function () {
                            _this.__do_execute(_this._dcn.getFocusedChannelId());
                        }, 3000);
                    }
                }
            }
        };
        cCPgDcn.prototype._on_alClicked = function (aParam) {
            if (aParam.alData && aParam.alData.index !== -1) {
                this.__do_execute(aParam.alData.ccid);
            }
            return true;
        };
        cCPgDcn.prototype._on_alKey = function (aParam) {
            var bConsumed = false, k = hx.key, kc = aParam.alKey;
            if (this._bActive) {
                switch (kc) {
                    case k.KEY_BACK:
                    case k.KEY_ESCAPE:
                        this.__clear_dcn();
                        bConsumed = true;
                        break;
                    case k.KEY_UP:
                    case k.KEY_DOWN:
                        bConsumed = true;
                        break;
                    case k.KEY_CHANNEL_UP:
                    case k.KEY_CHANNEL_DOWN:
                    case k.KEY_MENU:
                        this.__clear_dcn();
                        bConsumed = true;
                        break;
                }
            }
            return bConsumed;
        };
        cCPgDcn.prototype._on_alExit = function (aParam) {
            this.__clear_dcn();
            return true;
        };
        cCPgDcn.prototype._on_alMessageToPage = function (aData) {
            if (aData && aData.query) {
                this['__query_' + aData.query](aData.param, aData.data.group);
            }
        };
        // PRIVATE FUNCTION FOR CONTENT MAKING
        cCPgDcn.prototype.__query_active = function (aStartNum, aGroup) {
            var _this = this;
            if (this._bActive === false) {
                this.__make_dcn(aStartNum, aGroup);
                this._bActive = true;
                this.sendEventToEntity({
                    alEvent: 'alCheckPageVisible'
                });
                this._timerId = setTimeout(function () {
                    _this.__do_execute(_this._dcn.getFocusedChannelId());
                    //that.__clear_dcn();
                }, 3000);
            }
        };
        cCPgDcn.prototype.__make_dcn = function (aNum, aGroup) {
            this._dcn = hx.al.component('dcn', {
                'tag': 'div',
                'attr': {
                    'class': 'chChange'
                },
                'startnum': aNum,
                'group': aGroup
            });
            // append components
            this.append(this._dcn);
            // append elements
            this._$section.prepend(this._$dim.append(this._dcn.$));
            this._dcn.on();
            this._$dim.addClass('dimCHange');
        };
        cCPgDcn.prototype.__clear_dcn = function () {
            if (this._bActive === true) {
                hx.al.decomponent(this._dcn);
                if (this._timerId !== -1) {
                    clearTimeout(this._timerId);
                    this._timerId = -1;
                }
                this._bActive = false;
                this.sendEventToEntity({
                    alEvent: 'alCheckPageVisible'
                });
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgInfoHub?dcn=clear'
                });
            }
            this._$dim.remove();
        };
        cCPgDcn.prototype.__do_execute = function (aCcid) {
            if (this.getPageVisible('CPgInfoHub') && (this.$wrap.find('#channelList').length !== 0 || this.$wrap.find('#iPlate').length !== 0)) {
                this.sendEventToEntity({
                    alEvent: 'alMessageToPage',
                    target: 'CPgInfoHub?dcn=' + aCcid
                });
            }
            var bSet = this.__set_channel();
            if (!bSet) {
                this.__clear_dcn();
            }
            return true;
        };
        cCPgDcn.prototype.__set_channel = function () {
            return this._dcn.setChannel();
        };
        cCPgDcn.prototype._on_HideChInfo = function () {
            var that = this;
            hx.log("info", "[CPgDcn][_on_HideChInfo] clear & disable DCN");
            if (that._bActive) {
                that.__clear_dcn();
            }
        };
        return cCPgDcn;
    })(__PAGE_KERNEL__);
    hx.al.CPgDcn = cCPgDcn;
    return cCPgDcn;
});
