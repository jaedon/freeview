var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/tvguide/cmpt_guideview", "pages/util/provider"], function (require, exports, __VIEW__, __PROVIDER__) {
    ///<reference path="../../def/sugar.d.ts" />
    var cGuideChannelList = (function (_super) {
        __extends(cGuideChannelList, _super);
        function cGuideChannelList(aParam) {
            _super.call(this, $('<article>', {
                'id': 'chList',
                'name': 'guideChannelList'
            }), 'focusable', 'on', aParam);
            this._strCasID = ['CAS_FTA', 'CAS_NAGRA', 'CAS_IRDETO', 'CAS_NDS', 'CAS_VIACCESS', 'CAS_CONAX', 'CAS_MEDIAGUARD', 'CAS_CRYPTOWORKS'];
            this._sUPSIDE = 'dummy-up';
            this._sDOWNSIDE = 'dummy-down';
            this.casIDs = [];
            this.channels = [];
            this.dummyUpChannels = [];
            this.dummyDownChannels = [];
            this.channelGroup = {};
            this.isChangeable = [];
            this.liveChannelInfo = {};
            this.currentRowIndex = 0;
            this._focusClass = 'on';
            this._textAniValueOffset = 7;
            this._textAniIntervalOffset = 45;
            this.$ = this._$;
            var box;
            //hx.logadd('guideChannelList');
            hx.log('guideChannelList', hx.util.format('===== [{0}][init] ===== ', this.name));
            //        this._setPrismAndPrismLiteMode();
            box = document.createElement('ul');
            box.className = 'root';
            this._ulBox = box;
            if (aParam.animation) {
                this.animation = aParam.animation;
            }
            this.create();
        }
        cGuideChannelList.prototype.create = function () {
            var i, length, top, li, ulBox = this._ulBox, $channelList = this.$[0], defaultRowIndex = this.defaultRowIndex;
            hx.log('guideChannelList', hx.util.format('===== [{0}][create] ===== ', this.name));
            if (this.ANIMATION_EFFECT) {
                for (i = -(this.ROW_COUNT), length = (this.ROW_COUNT * 2); i < length; i += 1) {
                    top = i * this.ROW_HEIGHT;
                    li = document.createElement('li');
                    li.style.top = top + "px";
                    li.style.height = this.ROW_HEIGHT + "px";
                    if (defaultRowIndex === i) {
                        li.className = this._focusClass;
                    }
                    ulBox.appendChild(li);
                    if (i >= 0 && i < this.ROW_COUNT) {
                        this.rowList.push({
                            'element': li,
                            'top': top
                        });
                        this.isChangeable[i] = true;
                    }
                    else if (i < 0) {
                        li.className = this._sUPSIDE;
                    }
                    else if (i >= this.ROW_COUNT) {
                        li.className = this._sDOWNSIDE;
                    }
                }
            }
            else {
                for (i = 0, length = this.ROW_COUNT; i < length; i += 1) {
                    top = i * this.ROW_HEIGHT;
                    li = document.createElement('li');
                    li.style.top = top + "px";
                    li.style.height = this.ROW_HEIGHT + "px";
                    if (defaultRowIndex === i) {
                        li.className = this._focusClass;
                    }
                    ulBox.appendChild(li);
                    this.rowList.push({
                        'element': li,
                        'top': top
                    });
                    this.isChangeable[i] = true;
                }
            }
            $channelList.appendChild(ulBox);
            this.getCurrentChannel();
        };
        cGuideChannelList.prototype.destroy = function () {
            this.casIDs = [];
            this.channels = [];
            this.dummyUpChannels = [];
            this.dummyDownChannels = [];
            this.rowList = [];
            this.isChangeable = [];
            this.liveChannelInfo = {};
            this.channelGroup = {};
        };
        cGuideChannelList.prototype.bindRowList = function (rows) {
            var i, length, li;
            hx.log('guideChannelList', hx.util.format('===== [{0}][bindRowList] ===== ', this.name));
            length = this.rowList.length;
            for (i = 0; i < length; i += 1) {
                li = rows[i];
                this.rowList[i] = {
                    'element': li,
                    'top': li.offsetTop
                };
            }
        };
        cGuideChannelList.prototype.activateRow = function (index) {
            hx.log('warning', hx.util.format('===== [{0}][activateRow] ===== ', index));
            this.rowList[index].element.setAttribute('class', this._focusClass);
            this.currentRowIndex = index;
        };
        cGuideChannelList.prototype.deactivateRow = function (index) {
            hx.log('warning', hx.util.format('===== [{0}][deactivateRow] ===== ', index));
            this.rowList[this.currentRowIndex].element.setAttribute('class', '');
            this.rowList[index].element.setAttribute('class', '');
        };
        cGuideChannelList.prototype.getChannels = function (param) {
            var channels = [], count = param.count;
            var i, len, pluginChannels, provider_channels = [];
            hx.log('guideChannelList', hx.util.format('===== [{0}][getChannels] ===== ', this.name));
            pluginChannels = hx.svc.CH_LIST.getChannels(param);
            len = pluginChannels.length;
            if (len) {
                for (i = 0; i < len; i += 1) {
                    provider_channels.push(new __PROVIDER__.CChannelProvider(pluginChannels[i]));
                }
                channels = this.srvDataToUi(provider_channels, count, this.EDATA_TYPE.ECHANNEL);
                // update channel-list
                this.channels = channels;
                this.draw(param);
                this._getCasId();
                this.checkTunes(this.channels);
                if (this.ANIMATION_EFFECT) {
                    this.getDummyChannels(this.channels[0].ccid, param.group);
                }
            }
            hx.log('guideChannelList', hx.util.format('pluginChannels length : {0} uiChannel length : {1}', len, channels.length));
        };
        cGuideChannelList.prototype.getDummyChannels = function (ccid, group) {
            hx.log('guideChannelList', hx.util.format('===== [{0}][getDummyChannels] ===== ', this.name));
            this.dummyUpChannels = hx.svc.CH_LIST.getChannels({
                ccid: ccid,
                group: group,
                offset: 0,
                count: 1
            });
            this.dummyDownChannels = hx.svc.CH_LIST.getChannels({
                ccid: ccid,
                group: group,
                offset: this.ROW_COUNT - 1,
                count: 1
            });
            this.dummyDraw();
        };
        cGuideChannelList.prototype.draw = function (param) {
            var _this = this;
            var i, length, channel, majorChannel, name, logoURL, imgObj, channels = this.channels;
            hx.log('guideChannelList', hx.util.format('===== [{0}][draw] ===== ', this.name));
            for (i = 0, length = channels.length; i < length; i += 1) {
                channel = channels[i];
                majorChannel = channel.majorChannel ? hx.il.makeDigitNumber(channel.majorChannel) : '';
                name = channel.name || '';
                var element = this.rowList[i].element;
                var elFigure = element.querySelector('figure');
                var fnGetImage = function (aElement, serviceId) {
                    if (_this._isWaitingScroll() || (param && param.keyRepeatCount)) {
                        hx.log('warning', '[draw] Do not request Channel Logo during Scrolling');
                        return;
                    }
                    hx.svc.IP_EPG.getChannelLogo(serviceId).then(function (url) {
                        if (_this._isWaitingScroll()) {
                            hx.log('warning', '[getChannelLogo] Do not update Channel Logo during Scrolling');
                            return;
                        }
                        if (url == null) {
                            hx.log('warning', '[getChannelLogo] url is invalid');
                            return;
                        }
                        if (!_this._isValidServiceIdToUpdate(serviceId)) {
                            hx.log('warning', '[getChannelLogo] it already passed');
                            return;
                        }
                        aElement.lastChild.innerHTML = "<img>";
                        var elImg = aElement.querySelector('img');
                        elImg.setAttribute('src', url);
                        elImg.style.display = 'block';
                    }, function () {
                        if (_this._isWaitingScroll()) {
                            hx.log('warning', '[getChannelLogo] Do not update Channel Logo during Scrolling');
                            return;
                        }
                        var elImg = aElement.querySelector('img');
                        if (elImg) {
                        }
                    });
                };
                if (!elFigure) {
                    var $num = $('<figure>', { 'class': 'num', 'text': majorChannel });
                    var $logo = $('<figure>', { 'class': 'logo' });
                    var tNodeName = document.createTextNode(name);
                    var elImg = document.createElement('img');
                    $logo.append(tNodeName);
                    $logo.append(elImg);
                    element.appendChild($num[0]);
                    element.appendChild($logo[0]);
                    fnGetImage(element, channel.getServiceId());
                }
                else if (majorChannel !== element.firstChild.textContent || (param && param.updateFlag)) {
                    element.firstChild.textContent = majorChannel;
                    element.lastChild.textContent = name;
                    fnGetImage(element, channel.getServiceId());
                }
            }
        };
        cGuideChannelList.prototype.clear = function () {
            var i, count;
            for (i = 0, count = this.ROW_COUNT; i < count; i += 1) {
                this.rowList[i].element.innerHTML = '';
            }
            this.deactivateRow(this.currentRowIndex);
            this.activateRow(this.defaultRowIndex);
            this.currentRowIndex = this.defaultRowIndex;
        };
        cGuideChannelList.prototype.dummyDraw = function (direction) {
            var uEle = $('#tvg #chList .dummy-up'), dEle = $('#tvg #chList .dummy-down'), i, length, u, d;
            if (uEle.length === 0 || dEle.length === 0) {
                return;
            }
            for (i = 0, length = this.dummyUpChannels.length; i < length; i += 1) {
                u = this.dummyUpChannels[i];
                d = this.dummyDownChannels[i];
                uEle[i].innerHTML = u.majorChannel ? "<figure class='num'>" + hx.il.makeDigitNumber(u.majorChannel) + "</figure>" + "<figure class='logo'></figure>" : "";
                dEle[i].innerHTML = d.majorChannel ? "<figure class='num'>" + hx.il.makeDigitNumber(d.majorChannel) + "</figure>" + "<figure class='logo'></figure>" : "";
            }
        };
        cGuideChannelList.prototype.doScroll = function (param) {
            var _this = this;
            var direction, movingCount, offset, chParam;
            var top = 0;
            var $container = $(this._ulBox);
            hx.log("guideChannelList", hx.util.format('+++++++ ####### doScroll[Start] this.animation.running = {0} ####### +++++++', this.animation.running));
            function getTop($li, where) {
                var top = 0;
                var offset = 0;
                if ($li) {
                    if (where === "first") {
                        offset = 0;
                    }
                    else if (where === "last") {
                        if ($li.length > 0) {
                            offset = $li.length - 1;
                        }
                    }
                    else if (where === "this") {
                        offset = 0;
                    }
                    top = parseInt($li[offset].style.top);
                }
                return top;
            }
            var before = function () {
                hx.log("guideChannelList", '+++++++ ####### doScroll before() ####### +++++++');
                if (_this.ANIMATION_EFFECT) {
                    //top = $container.position().top + (direction === 'up' ? -offset : offset);
                    top = $container[0].offsetTop;
                    top = top + (direction === 'up' ? -offset : offset);
                }
            };
            var after = function () {
                hx.log("guideChannelList", '+++++++ ####### doScroll after() ####### +++++++');
                if (_this.ANIMATION_EFFECT) {
                    var $liC, $liU, $liD, $liM;
                    var start = 0;
                    var $li = $container.find('li');
                    if (direction === 'up') {
                        $liC = $li.slice(0, movingCount);
                        top = getTop($li, "last");
                        start = top + _this.ROW_HEIGHT;
                        $liC.detach();
                        $liC.each(function (i, li) {
                            li.style.top = start + i * _this.ROW_HEIGHT + 'px';
                        });
                        $container.append($liC);
                    }
                    else {
                        $liC = $li.slice(-movingCount);
                        top = getTop($li, "first");
                        start = top + (-movingCount * _this.ROW_HEIGHT);
                        $liC.detach();
                        $liC.each(function (i, li) {
                            li.style.top = start + i * _this.ROW_HEIGHT + 'px';
                        });
                        $container.prepend($liC);
                    }
                    $li = $container.find('li');
                    $liU = $li.slice(0, _this.ROW_COUNT);
                    $liM = $li.slice(_this.ROW_COUNT, _this.ROW_COUNT * 2);
                    $liD = $li.slice(_this.ROW_COUNT * 2, _this.ROW_COUNT * 3);
                    $liU.attr('class', _this._sUPSIDE);
                    $liM.attr('class', '');
                    $liD.attr('class', _this._sDOWNSIDE);
                    _this.bindRowList($liM);
                }
                if (_this.onScrollStarted) {
                    _this.onScrollStarted(chParam);
                }
                if (_this.onScrollStopped) {
                    _this.onScrollStopped();
                }
            };
            var anim = function () {
                if (_this.animation.running) {
                    return;
                }
                _this.animation.running = true;
                before();
                _this.animation.state = 'scrolling';
                $container.addClass(direction);
                $container.anim({
                    'top': top + 'px'
                }, function () {
                    _this.animation.running = false;
                    _this.animation.state = 'idle';
                    $container.removeClass(direction);
                    after();
                });
            };
            if (this.ANIMATION_EFFECT) {
                if (this.animation.use) {
                    if (!this.animation.running) {
                        direction = param.direction;
                        movingCount = param.movingCount;
                        this.onScrollStarted = param.onScrollStarted;
                        this.onScrollStopped = param.onScrollStopped;
                        chParam = param.chParam;
                        offset = this.ROW_HEIGHT * movingCount;
                        anim();
                    }
                }
                else {
                    before();
                    $container.css('top', top + 'px');
                    after();
                }
            }
            else {
                direction = param.direction;
                movingCount = param.movingCount;
                this.onScrollStarted = param.onScrollStarted;
                this.onScrollStopped = param.onScrollStopped;
                chParam = param.chParam;
                offset = this.ROW_HEIGHT * movingCount;
                before();
                after();
            }
            hx.log("guideChannelList", hx.util.format('+++++++ ####### doScroll[End] this.animation.running = {0} ####### +++++++', this.animation.running));
        };
        cGuideChannelList.prototype.checkTunes = function (channels) {
            var i, length, channel;
            hx.log('guideChannelList', hx.util.format('===== [{0}][checkTunes] ===== ', this.name));
            for (i = 0, length = channels.length; i < length; i += 1) {
                channel = channels[i];
                this.isChangeable[i] = (this.isEmpty(channel.ccid)) ? false : hx.svc.CH_LIST.checkChannelChangable({
                    'channel': channel.pluginObject
                });
                hx.log('guideChannelList', hx.util.format('===== [{0}] {1} - isChangeable {2} ===== ', this.name, i, this.isChangeable[i]));
            }
        };
        cGuideChannelList.prototype.getCurrentChannel = function () {
            var chinfo = hx.svc.CH_LIST.getCurrentChannel();
            hx.log('guideChannelList', hx.util.format('===== [{0}][getCurrentChannel] ===== ', this.name));
            this.liveChannelInfo = chinfo;
            this.liveChannelInfo.chCnt = hx.svc.CH_LIST.getChannelCount({
                group: chinfo.group
            }).chCnt;
            this.channelGroup = chinfo.group;
        };
        cGuideChannelList.prototype.updateLastService = function () {
            var channel, group;
            hx.log('guideChannelList', hx.util.format('===== [{0}][updateLastService] ===== ', this.name));
            channel = this.channels[this.currentRowIndex];
            group = this.channelGroup;
            hx.svc.WEB_STORAGE.updateLastService({
                'ccid': channel.ccid,
                'group': group
            });
        };
        cGuideChannelList.prototype.hasTuned = function () {
            var liveInfo = this.liveChannelInfo;
            hx.log('guideChannelList', hx.util.format('===== [{0}][hasTuned] ===== ', this.name));
            this.getCurrentChannel();
            return ((liveInfo.channel.ccid === this.channels[this.currentRowIndex].ccid) && liveInfo.group === this.channelGroup);
        };
        cGuideChannelList.prototype.changeChannel = function (param) {
            var programme, isChannelChangeable, state;
            hx.log('guideChannelList', hx.util.format('===== [{0}][changeChannel] ===== ', this.name));
            isChannelChangeable = this.isChangeable[this.currentRowIndex];
            if (!isChannelChangeable) {
                state = 'clickedDisabledChannel';
            }
            else {
                var bMedGoing = hx.svc.PLAYBACK.isMpbOnGoing();
                if (bMedGoing) {
                    this.updateLastService();
                    state = 'updateLastService';
                    hx.svc.BRIDGE.requestStopPlayback();
                }
                else {
                    this.tuneChannel();
                    state = 'changeChannel';
                }
            }
            return state;
        };
        cGuideChannelList.prototype.tuneChannel = function () {
            var channel, group;
            hx.log('guideChannelList', hx.util.format('===== [{0}][tuneChannel] ===== ', this.name));
            channel = this.channels[this.currentRowIndex];
            group = this.channelGroup;
            hx.svc.AV_CTRL.channelChange({
                'channel': channel.pluginObject,
                'group': group
            });
        };
        cGuideChannelList.prototype.changeChannelGroup = function (group) {
            var count, offset, ccid, channelInfo, p;
            hx.log('guideChannelList', hx.util.format('===== [{0}][changeChannelGroup] ===== ', this.name));
            count = hx.svc.CH_LIST.getChannelCount({
                'group': group
            }).chCnt;
            hx.log('guideChannelList', hx.util.format('group : {0}, count : {1}', group, count));
            if (count < 1) {
                hx.log('guideChannelList', 'I am sorry. But It can not be accepted. Good-bye.');
                return null;
            }
            this.clear();
            /* get current live channel information */
            ccid = this.channels[this.defaultRowIndex].ccid;
            channelInfo = hx.svc.CH_LIST.getChannel({
                'ccid': ccid,
                'group': group
            });
            if (!channelInfo.result) {
                ccid = null;
            }
            this.channelGroup = group;
            p = {
                ccid: ccid,
                group: group,
                offset: 0,
                count: this.ROW_COUNT
            };
            return p;
        };
        cGuideChannelList.prototype._getChannelCount = function (group) {
            hx.log('guideChannelList', hx.util.format('===== [{0}][_getChannelCount] ===== ', this.name));
            if (this.isEmpty(group)) {
                return -1;
            }
            return hx.svc.CH_LIST.getChannelCount({
                group: group
            }).chCnt;
        };
        cGuideChannelList.prototype._getCasId = function () {
            var n, i, l, casArr, length, channel, casIDs;
            hx.log('guideChannelList', hx.util.format('===== [{0}][_getCasId] ===== ', this.name));
            for (n = 0, l = this.channels.length; n < l; n += 1) {
                channel = this.channels[n];
                casArr = [];
                casIDs = [];
                if (channel.ccid) {
                    casIDs = channel.pluginObject.casIDs;
                    if (!!casIDs) {
                        length = casIDs.length;
                        for (i = 0; i < length; i += 1) {
                            casArr.push(casIDs[i]);
                        }
                    }
                }
                if (!this.isEmpty(casArr)) {
                    this.casIDs[n] = 'cas ';
                    if (casArr.length > 1) {
                        this.casIDs[n] += 'multi';
                    }
                    else {
                        for (i = 0, length = casArr.length; i < length; i += 1) {
                            if (casArr[i] === this._strCasID.indexOf('CAS_NAGRA')) {
                                this.casIDs[n] += 'na ';
                            }
                            else if (casArr[i] === this._strCasID.indexOf('CAS_IRDETO')) {
                                this.casIDs[n] += 'ir ';
                            }
                            else if (casArr[i] === this._strCasID.indexOf('CAS_NDS')) {
                                this.casIDs[n] += 'nd ';
                            }
                            else if (casArr[i] === this._strCasID.indexOf('CAS_VIACCESS')) {
                                this.casIDs[n] += 'va ';
                            }
                            else if (casArr[i] === this._strCasID.indexOf('CAS_CONAX')) {
                                this.casIDs[n] += 'cx ';
                            }
                            else if (casArr[i] === this._strCasID.indexOf('CAS_MEDIAGUARD')) {
                                this.casIDs[n] += 'mg ';
                            }
                            else if (casArr[i] === this._strCasID.indexOf('CAS_CRYPTOWORKS')) {
                                this.casIDs[n] += 'cr ';
                            }
                        }
                    }
                }
                else {
                    this.casIDs[n] = '';
                }
                hx.log('guideChannelList', hx.util.format('ccid : {0}, casId : {1}, attr : {2}', casIDs, channel.ccid, casArr));
            }
        };
        cGuideChannelList.prototype._isValidServiceIdToUpdate = function (aServiceId) {
            var i = 0, len = this.channels.length;
            for (i = 0; i < len; i += 1) {
                if (aServiceId === this.channels[i].getServiceId()) {
                    return true;
                }
            }
            return false;
        };
        return cGuideChannelList;
    })(__VIEW__);
    return cGuideChannelList;
});
