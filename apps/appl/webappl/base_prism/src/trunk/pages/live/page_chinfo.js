var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel", "def/oipf.def", 'pages/util/provider'], function (require, exports, __PAGE_KERNEL__, __DEF__, __PROVIDER__) {
    var cCPgChInfo = (function (_super) {
        __extends(cCPgChInfo, _super);
        function cCPgChInfo() {
            _super.call(this);
            this._channel = null;
            this._tuneInfo = null;
            this._event = '';
            this._st = 0;
            this._et = 0;
            this._percentage = 0;
            this._timeOutID = -1;
            this._OSDtimeOut = 3; // seconds
            this._getEPGTimer = -1;
        }
        cCPgChInfo.prototype.init = function () {
            hx.logadd('CPgChInfo');
        };
        cCPgChInfo.prototype.create = function () {
            var _this = this;
            this._bActivate = false;
            $.extend(this, {
                _$section: $('#ltv'),
                $_dim: $('<div>', {
                    'class': 'dimCHange'
                }),
                $_chChangeWrap: $('<div>', {
                    'class': 'chChange'
                }),
                $_match: $('<dl>', {
                    'class': 'match'
                }),
                $_num: $('<span>', {
                    'text': ''
                }),
                $_svc: $('<dt>', {
                    'text': ''
                }),
                $_evt: $('<dd>', {
                    'text': ''
                }),
                $_progress: $('<dl>', {
                    'class': 'progress'
                }),
                $_st: $('<dd>', {
                    'class': 'time srt',
                    'text': ''
                }),
                $_et: $('<dd>', {
                    'class': 'time end',
                    'text': ''
                }),
                $_grp: $('<dd>', {
                    'class': 'graph'
                }),
                $_grp2: $('<span>', {
                    'class': 'live'
                })
            });
            hx.al.compose(this);
            hx.svc.AV_CTRL.addEventCb('evtSetChannel', this, function (aParam) {
                _this._on_evtSetChannel(aParam);
            });
            hx.svc.AV_CTRL.addEventCb('ProgrammesChanged', this, function (aParam) {
                _this._on_ProgrammesChanged(aParam);
            });
            if (hx.config.netflix) {
                hx.svc.NETFLIX.addEventCb('HideChInfo', this, function (aParam) {
                    this._on_HideChInfo(aParam);
                });
            }
            this._OSDtimeOut = hx.svc.SETTING_UTIL.getInfoDisplayTimeout();
        };
        cCPgChInfo.prototype.destroy = function () {
            hx.svc.AV_CTRL.clearEventCb(this);
            hx.svc.NETFLIX.clearEventCb(this);
            this._hide();
        };
        cCPgChInfo.prototype.isActive = function () {
            return this._bActivate;
        };
        cCPgChInfo.prototype._show = function () {
            if (hx.config.netflix) {
                if (hx.svc.NETFLIX.isRunning()) {
                    return;
                }
            }
            var ulnum = 0;
            var cname = '', ChInfo;
            if (this._OSDtimeOut === 0) {
                return;
            }
            if (this._channel !== null) {
                ChInfo = this._channel;
                ulnum = ChInfo.majorChannel;
                cname = ChInfo.name;
            }
            // time out
            this._updateTimeOut();
            // update data
            this._update();
            this._$section.append(this.$_dim.append(this.$_chChangeWrap.append(this.$_match.append(this.$_num).append(this.$_svc).append(this.$_evt).append(this.$_progress.append(this.$_st).append(this.$_et).append(this.$_grp.append(this.$_grp2))))));
            this._bActivate = true;
        };
        cCPgChInfo.prototype._hide = function () {
            clearTimeout(this._timeOutID);
            clearTimeout(this._getEPGTimer);
            if (this.$_dim !== undefined) {
                this.$_dim.remove();
            }
            this._bActivate = false;
        };
        cCPgChInfo.prototype._update = function () {
            if (hx.config.netflix) {
                if (hx.svc.NETFLIX.isRunning()) {
                    return;
                }
            }
            var ulnum = 0;
            var cname = '';
            if (this._channel !== null) {
                ulnum = this._channel.majorChannel;
                cname = this._channel.name;
            }
            this.$_num.text(hx.il.makeDigitNumber(ulnum));
            this.$_svc.text(cname);
            this.$_evt.text(this._event);
            this.$_st.text(dateFormat(new Date(this._st * 1000), "HH:MM"));
            this.$_et.text(dateFormat(new Date(this._et * 1000), "HH:MM"));
            this.$_grp2.width(this._percentage + '%');
            if (this._event === '') {
                this.$_progress[0].style.display = 'none';
            }
            else {
                this.$_progress[0].style.display = 'block';
            }
        };
        cCPgChInfo.prototype._updateTimeOut = function () {
            var _this = this;
            var ultimeout = this._OSDtimeOut;
            if (ultimeout === undefined) {
                ultimeout = 5;
            }
            ultimeout += 1;
            // additional time
            // kill timer
            if (this._timeOutID !== -1) {
                clearTimeout(this._timeOutID);
                this._timeOutID = -1;
            }
            // set timer
            this._timeOutID = setTimeout(function () {
                _this._hide();
            }, ultimeout * 1000);
        };
        cCPgChInfo.prototype._on_alKey = function (aParam) {
            var ulkeyCode = aParam.alKey;
            var k = hx.key;
            var bConsumed = false;
            switch (ulkeyCode) {
                case k.KEY_0:
                case k.KEY_1:
                case k.KEY_2:
                case k.KEY_3:
                case k.KEY_4:
                case k.KEY_5:
                case k.KEY_6:
                case k.KEY_7:
                case k.KEY_8:
                case k.KEY_9:
                case k.KEY_SCREEN_MODE_NEXT:
                    // bypassing numeric key for DCN
                    if (this._bActivate) {
                        this._hide();
                        bConsumed = false;
                    }
                    break;
                case k.KEY_GUIDE:
                case k.KEY_ON_DEMAND:
                case k.KEY_ESCAPE:
                case k.KEY_PLAYER:
                case k.KEY_OK:
                    if (this._bActivate) {
                        this._hide();
                        bConsumed = false;
                    }
                    break;
                default:
                    break;
            }
            return bConsumed;
        };
        cCPgChInfo.prototype._callEpg = function (aChannel, aTuneInfo) {
            // clear event
            this._event = '';
            this._st = 0;
            this._et = 0;
            this._percentage = 0;
            var result = hx.svc.EPG.getLiveProgrammes({
                'channel': aChannel
            });
            var evtUpdated = false;
            if (this.getCenterPageName() !== 'CPgLiveController') {
                return;
            }
            if (result) {
                var prog = result[0][0];
                if (prog) {
                    this._event = prog.name;
                    this._st = prog.getStartTime();
                    this._et = prog.getEndTime();
                    this._percentage = prog.getPercentage();
                    evtUpdated = true;
                }
            }
            if (aTuneInfo ? !(aTuneInfo & 1 /* TUNEMODE_NO_IPLATE */) : true) {
                if (evtUpdated === true) {
                    if (this._bActivate === true) {
                        this._update();
                    }
                    else {
                        this._show();
                    }
                }
            }
        };
        cCPgChInfo.prototype._updateEpg = function (aEvent) {
            if (!aEvent) {
                return;
            }
            this._event = aEvent.getProgrammeName();
            this._st = aEvent.getStartTime();
            this._et = aEvent.getEndTime();
            this._percentage = aEvent.getPercentage();
            if (this._tuneInfo ? !(this._tuneInfo & 1 /* TUNEMODE_NO_IPLATE */) : true) {
                this._update();
            }
        };
        cCPgChInfo.prototype._on_evtSetChannel = function (aParam) {
            var _this = this;
            var ulgetEPGtime = (this._OSDtimeOut > 4) ? 3000 : 0;
            if (!aParam || aParam === null) {
                return;
            }
            if (this.getCenterPageName() !== 'CPgLiveController') {
                clearTimeout(this._getEPGTimer);
                return;
            }
            this._channel = aParam.channel;
            this._tuneInfo = aParam.tuneInfo;
            this._event = '';
            this._st = 0;
            this._et = 0;
            this._percentage = 0;
            clearTimeout(this._getEPGTimer);
            this._getEPGTimer = setTimeout(function () {
                if (_this.getCenterPageName() !== 'CPgLiveController') {
                    return;
                }
                //this._callEpg(this._channel, aParam.tuneInfo);
            }, ulgetEPGtime);
            if (aParam.tuneInfo ? !(aParam.tuneInfo & 1 /* TUNEMODE_NO_IPLATE */) : true) {
                if (this._bActivate === true) {
                    this._update();
                    this._updateTimeOut();
                }
                else {
                    this._show();
                }
            }
        };
        cCPgChInfo.prototype._on_ProgrammesChanged = function (aParam) {
            if (this.getCenterPageName() !== 'CPgLiveController') {
                return;
            }
            if (this._bActivate) {
                if (aParam && aParam instanceof __PROVIDER__.cPdeProgrammeItem) {
                    this._updateEpg(new __PROVIDER__.CProgrammeProvider({ oipfData: aParam.getSourceItem() }));
                }
                else {
                    this._callEpg(this._channel, this._tuneInfo);
                }
            }
        };
        cCPgChInfo.prototype._on_alMessageToPage = function (aParam) {
            if (aParam.query === 'update') {
                var o = hx.svc.WEB_STORAGE.getLastService();
                if (o.ccid !== null && o.group !== null) {
                    var o2 = hx.svc.CH_LIST.getChannel({
                        ccid: o.ccid,
                        group: o.group
                    });
                    if (o2 && o2.channel) {
                        this._on_evtSetChannel({
                            channel: o2.channel
                        });
                    }
                }
            }
        };
        cCPgChInfo.prototype._on_HideChInfo = function () {
            var that = this;
            hx.log("info", "[CPgChInfo][_on_HideChInfo] hide!!");
            if (that._bActivate) {
                that._hide();
            }
        };
        return cCPgChInfo;
    })(__PAGE_KERNEL__);
    hx.al.CPgChInfo = cCPgChInfo;
    return cCPgChInfo;
});
