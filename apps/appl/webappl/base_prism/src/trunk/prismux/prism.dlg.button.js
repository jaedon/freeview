///<reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", 'pages/util/controls', 'prismux/prism.dlg', "def/oipf.def"], function (require, exports, __CONTROLS__, __PRISM_CDIALOG__, __DEF__) {
    var mPrism;
    (function (mPrism) {
        var mOpenCount = 0;
        var CButtonDlg = (function (_super) {
            __extends(CButtonDlg, _super);
            function CButtonDlg(layeredGroupControl) {
                _super.call(this);
                var a = this;
                a.init(layeredGroupControl);
                a._buttonLayoutType = 'horizontal';
            }
            CButtonDlg.isOpened = function () {
                if (mOpenCount < 0)
                    throw "check init & destroy routine";
                return mOpenCount ? true : false;
            };
            CButtonDlg.prototype.init = function (layeredGroupControl) {
                var a = this;
                _super.prototype.init.call(a, layeredGroupControl);
                a.addClass('buttonDlg');
                a._buttonInfoList = [{
                    text: tr('LOC_OK_ID')
                }];
                mOpenCount += 1;
            };
            CButtonDlg.prototype.destroy = function () {
                var a = this;
                mOpenCount -= 1;
                a._buttonInfoList = null;
                _super.prototype.destroy.call(a);
            };
            CButtonDlg.prototype.setTextHtml = function (textHtml) {
                var a = this;
                a._textHtml = textHtml;
            };
            CButtonDlg.prototype.setButtonInfoList = function (buttonInfoList) {
                var a = this;
                a._buttonInfoList = buttonInfoList;
            };
            CButtonDlg.prototype.setButtonLayoutType = function (buttonLayoutType) {
                var a = this;
                a._buttonLayoutType = buttonLayoutType;
            };
            CButtonDlg.prototype.doOpen = function (param) {
                var a = this;
                var verticalLayout = new CVerticalLayoutControl(null);
                verticalLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass("-title");
                        $item.removeClass('-f');
                        $item.html(a._textHtml);
                        return false;
                    }
                ]);
                verticalLayout.addClass('text');
                var buttonLayout, buttonDrawer;
                if (a._buttonLayoutType == 'vertical') {
                    buttonLayout = new CVerticalLayoutControl(null);
                    buttonDrawer = function (index, $item) {
                        $item.addClass('btn long').html(a._buttonInfoList[index].text);
                    };
                }
                else {
                    buttonLayout = new CHorizontalLayoutControl(null);
                    buttonLayout.setDrawParam("childAlign", "hcenter");
                    buttonDrawer = function (index, $item) {
                        $item.addClass('btn').html(a._buttonInfoList[index].text);
                    };
                }
                var fnSetDrawer = function (drawer) {
                    var drawers = [];
                    if (a._buttonInfoList) {
                        var i;
                        for (i = 0; i < a._buttonInfoList.length; i += 1) {
                            drawers.push(drawer);
                        }
                    }
                    buttonLayout.setItemDrawers(drawers);
                };
                fnSetDrawer(buttonDrawer);
                buttonLayout.setDrawParam({
                    padding: 15
                });
                buttonLayout.addClass("-btn-ctrl");
                buttonLayout.onItemSelected = function (index, $item) {
                    var fn = null;
                    var fnAsync = null;
                    if (a._buttonInfoList) {
                        fn = a._buttonInfoList[index].fn;
                        fnAsync = a._buttonInfoList[index].fnAsync;
                    }
                    if (fn) {
                        setTimeout(function () {
                            var nextInfo = fn(index, $item);
                            if (nextInfo) {
                                a._regions = nextInfo.title;
                                a.setButtonInfoList(nextInfo.data);
                                fnSetDrawer(buttonDrawer);
                                a.draw();
                            }
                            else {
                                a.close('internal');
                            }
                        }, 1);
                    }
                    else if (fnAsync) {
                        fnAsync(index, function (nextInfo) {
                            if (nextInfo) {
                                a._regions = nextInfo.title;
                                a.setButtonInfoList(nextInfo.data);
                                fnSetDrawer(buttonDrawer);
                                a.draw();
                                a.setActiveFocus(true);
                            }
                        });
                    }
                    else {
                        a.close('internal');
                    }
                };
                a.setChildControl([verticalLayout, buttonLayout], 1);
                this.doOpenCustomProcess();
                param = param ? param : {};
                a.setDrawParam({
                    $parent: a._parent.$layer(),
                    align: this.doGetAlign(),
                    childAlign: this.doGetChildAlign(),
                    padding: param.padding || 33,
                    margins: param.margins || [33, 50, 60]
                });
            };
            CButtonDlg.prototype.doOpenCustomProcess = function () {
            };
            CButtonDlg.prototype.doGetAlign = function () {
                return "hcenter";
            };
            CButtonDlg.prototype.doGetChildAlign = function () {
                return "hcenter";
            };
            return CButtonDlg;
        })(__PRISM_CDIALOG__.CDialog);
        mPrism.CButtonDlg = CButtonDlg;
        prism.CButtonDlg = CButtonDlg;
        prism.createButtonDlg = function (layeredGroupControl, buttonDlgInfo, callback) {
            if (layeredGroupControl) {
                var buttonDlg = new CButtonDlg(layeredGroupControl);
                if (buttonDlgInfo) {
                    if (buttonDlgInfo.textHtml) {
                        buttonDlg.setTextHtml(buttonDlgInfo.textHtml);
                    }
                    if (buttonDlgInfo.buttonList) {
                        var i, len, btnStr;
                        var buttonInfoList = [];
                        for (i = 0, len = buttonDlgInfo.buttonList.length; i < len; i++) {
                            btnStr = buttonDlgInfo.buttonList[i];
                            buttonInfoList.push({
                                text: btnStr,
                                fn: (function (callback, i) {
                                    return function () {
                                        if (callback) {
                                            callback(i);
                                        }
                                    };
                                }(callback, i))
                            });
                        }
                        buttonDlg.setButtonInfoList(buttonInfoList);
                    }
                    if (buttonDlgInfo.timeOutValue) {
                        buttonDlg.setTimeout(buttonDlgInfo.timeOutValue);
                    }
                    if (buttonDlgInfo.layoutType) {
                        buttonDlg.setButtonLayoutType(buttonDlgInfo.layoutType);
                    }
                }
                return buttonDlg;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createButtonDlg', buttonDlgInfo, callback);
                return globalDlg;
            }
        };
        prism.createReminderDlg = function (layeredGroupControl, reminderInfo, callback) {
            if (layeredGroupControl) {
                var buttonDlg = new CButtonDlg(layeredGroupControl);
                //buttonDlg.addClass('reminder');
                var textHtml = reminderInfo.title;
                textHtml += '<div class="text box">' + '<dt class = "textW">' + reminderInfo.eventName + '</dt>' + '<dt class = "textY">' + '<span style="float:left;">' + reminderInfo.channelNo + ' ' + reminderInfo.channelName + '</span>' + '<span style="float:right">' + (new Date(reminderInfo.startTime)).format('HH:MM') + ' - ' + (new Date(reminderInfo.endTime)).format('HH:MM') + '</span>' + '</dt>' + '</div>';
                buttonDlg.setTextHtml(textHtml);
                buttonDlg.setTimeout(20 * 1000);
                buttonDlg.setButtonInfoList([{
                    text: tr('LOC_YES_ID'),
                    fn: function () {
                        if (callback) {
                            callback(true);
                        }
                    }
                }, {
                    text: tr('LOC_NO_ID'),
                    fn: function () {
                        if (callback) {
                            callback(false);
                        }
                    }
                }]);
                return buttonDlg;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createReminderDlg', reminderInfo, callback);
                return globalDlg;
            }
        };
        prism.createConfirmDlg = function (layeredGroupControl, confirm, callback) {
            if (layeredGroupControl) {
                var buttonDlg = new CButtonDlg(layeredGroupControl);
                //buttonDlg.addClass('reminder');
                var textHtml = confirm.title;
                textHtml += '<div class="text box wLine">' + '<dt class = "textW">' + '<span style="float:left;">' + confirm.label + '</span>' + '<span style="float:right">' + confirm.value + '</dt>' + '</div>';
                buttonDlg.setTextHtml(textHtml);
                //buttonDlg.setTimeout(20 * 1000);
                buttonDlg.setButtonInfoList([{
                    text: tr('LOC_OK_ID'),
                    fn: function () {
                        if (callback) {
                            callback(true);
                        }
                    }
                }]);
                return buttonDlg;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createConfirmDlg', confirm, callback);
                return globalDlg;
            }
        };
        prism.createInfoDlg = function (layeredGroupControl, info, callback) {
            if (layeredGroupControl) {
                var buttonDlg = new CButtonDlg(layeredGroupControl);
                buttonDlg.addClass('info');
                buttonDlg.setTextHtml(info.textHtml);
                buttonDlg.setButtonInfoList([]);
                //buttonDlg.setTimeout(20 * 1000);
                return buttonDlg;
            }
            else {
                var globalDlg = prism.createGlobalDialog('createInfoDlg', info, callback);
                return globalDlg;
            }
        };
        var CRCTButtonDlg = (function (_super) {
            __extends(CRCTButtonDlg, _super);
            function CRCTButtonDlg(aLayeredGroupControl, aRctInfo, aCallback, aFnClosed) {
                var _this = this;
                _super.call(this, aLayeredGroupControl);
                this._rctIndex = 0;
                this._$arrowLeft = $("<div>").attr("class", "-arrow -left");
                this._$arrowRight = $("<div>").attr("class", "-arrow -right");
                //private _$recSingle = $("<div>").attr("class", "-schedule -single");
                //private _$recSeries = $("<div>").attr("class", "-schedule -series");
                this._textDesc = "No Information";
                var rctItem = aRctInfo.item(0);
                this._rctInfo = aRctInfo;
                this.setButtonInfoList([{
                    text: __DEF__.eTermID[rctItem.termId],
                    fn: function (aIndex) {
                        aCallback(_this._rctInfo.item(_this._rctIndex), !!aIndex);
                    }
                }]);
                this._buttonLayoutType = 'vertical';
                this.addClass('-rct-info');
                this.setTextHtml(rctItem.eventName);
                this.setDescription(rctItem.promotionalText);
                if (aRctInfo.length > 1) {
                    this.showRightArrow();
                }
                this._$self.append(this._$arrowLeft).append(this._$arrowRight);
                this.open({
                    onClosed: aFnClosed
                });
            }
            CRCTButtonDlg.prototype.setOnCheckSchedule = function (onCheckSchedule) {
                this.onCheckSchedule = onCheckSchedule;
            };
            CRCTButtonDlg.prototype.doCheckSchedule = function () {
                this.onCheckSchedule(this._rctInfo.item(this._rctIndex));
            };
            CRCTButtonDlg.prototype.doGetChildAlign = function () {
                return "";
            };
            CRCTButtonDlg.prototype.setDescription = function (aDesc) {
                this._textDesc = aDesc || this._textDesc;
            };
            CRCTButtonDlg.prototype.showLeftArrow = function () {
                this._$arrowLeft.addClass(CRCTButtonDlg.CLASS.SHOW);
            };
            CRCTButtonDlg.prototype.hideLeftArrow = function () {
                this._$arrowLeft.removeClass(CRCTButtonDlg.CLASS.SHOW);
            };
            CRCTButtonDlg.prototype.showRightArrow = function () {
                this._$arrowRight.addClass(CRCTButtonDlg.CLASS.SHOW);
            };
            CRCTButtonDlg.prototype.hideRightArrow = function () {
                this._$arrowRight.removeClass(CRCTButtonDlg.CLASS.SHOW);
            };
            CRCTButtonDlg.prototype.showRecSingle = function () {
                this._controls[0]._$self.addClass(CRCTButtonDlg.CLASS.SINGLE);
            };
            CRCTButtonDlg.prototype.hideRecSingle = function () {
                this._controls[0]._$self.removeClass(CRCTButtonDlg.CLASS.SINGLE);
            };
            CRCTButtonDlg.prototype.showRecSeries = function () {
                this._controls[0]._$self.addClass(CRCTButtonDlg.CLASS.SERIES);
            };
            CRCTButtonDlg.prototype.hideRecSeries = function () {
                this._controls[0]._$self.removeClass(CRCTButtonDlg.CLASS.SERIES);
            };
            CRCTButtonDlg.prototype.setTextHtml = function (aText) {
                _super.prototype.setTextHtml.call(this, "<span class=\"-schedule\"></span>" + aText);
            };
            CRCTButtonDlg.prototype.doKeyLeft = function () {
                if (this._rctIndex !== 0) {
                    var rctItem = this._rctInfo.item(--this._rctIndex);
                    /* TODO
                     * info update
                     * bglee@humaxdigital.com, 23:48, 2014. 8. 2.
                     */
                    if (this._buttonInfoList[0]) {
                        this._buttonInfoList[0].text = __DEF__.eTermID[rctItem.termId];
                    }
                    this.setTextHtml(rctItem.eventName);
                    this.setDescription(rctItem.promotionalText);
                    this.doCheckSchedule();
                    this.draw({
                        setActiveFocus: true,
                        visibleAfterDraw: true
                    });
                    this.showRightArrow();
                }
                if (this._rctIndex === 0) {
                    this.hideLeftArrow();
                }
                return true;
            };
            CRCTButtonDlg.prototype.doKeyRight = function () {
                console.log("right");
                if (this._rctIndex < this._rctInfo.length - 1) {
                    var rctItem = this._rctInfo.item(++this._rctIndex);
                    this._buttonInfoList[0].text = __DEF__.eTermID[rctItem.termId];
                    /* TODO
                     * info update
                     * bglee@humaxdigital.com, 23:48, 2014. 8. 2.
                     */
                    this.setTextHtml(rctItem.eventName);
                    this.setDescription(rctItem.promotionalText);
                    this.doCheckSchedule();
                    this.draw({
                        setActiveFocus: true,
                        visibleAfterDraw: true
                    });
                    this.showLeftArrow();
                }
                if (this._rctIndex === this._rctInfo.length - 1) {
                    this.hideRightArrow();
                }
                return true;
            };
            CRCTButtonDlg.prototype.doKeyGreen = function () {
                return true;
            };
            CRCTButtonDlg.prototype.doKeyUpIfNotHandled = function () {
                return true;
            };
            CRCTButtonDlg.prototype.doKeyDownIfNotHandled = function () {
                return true;
            };
            CRCTButtonDlg.prototype.doKeyBack = function () {
                this.close();
                return true;
            };
            CRCTButtonDlg.prototype.doKeyEscape = function () {
                this.close();
                return false;
            };
            CRCTButtonDlg.prototype.doKeyRecord = function () {
                return true;
            };
            CRCTButtonDlg.prototype.doOpenCustomProcess = function () {
                var _this = this;
                var descLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                descLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('-desc');
                        $item.removeClass('-f');
                        $item.html(_this._textDesc);
                        return false;
                    }
                ]);
                descLayout.addClass('text');
                descLayout._$self.css("top", "190px");
                this.insertChildControl(1, descLayout, false);
            };
            CRCTButtonDlg.prototype.doGetAlign = function () {
                return "";
            };
            CRCTButtonDlg.CLASS = {
                SHOW: "-show",
                SINGLE: "-single",
                SERIES: "-series"
            };
            return CRCTButtonDlg;
        })(CButtonDlg);
        var CTRDConflictButtonDlg = (function (_super) {
            __extends(CTRDConflictButtonDlg, _super);
            function CTRDConflictButtonDlg(aLayeredGroupControl) {
                _super.call(this, aLayeredGroupControl);
                this.addClass('-trd');
            }
            CTRDConflictButtonDlg.prototype.doOpenCustomProcess = function () {
                var _this = this;
                var descLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                descLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.removeClass('-f');
                        $item.html(tr('LOC_SELECT_REGIONS_ID'));
                        return false;
                    }
                ]);
                var regionLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                regionLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.removeClass('-f');
                        $item.html(_this._regions);
                        return false;
                    }
                ]);
                this.insertChildControl(1, regionLayout, false);
                this.insertChildControl(1, descLayout, false);
            };
            CTRDConflictButtonDlg.prototype.doGetAlign = function () {
                return "";
            };
            CTRDConflictButtonDlg.prototype.doKeyEscape = function () {
                this.destroy();
                return true;
            };
            CTRDConflictButtonDlg.prototype.doKeyBack = function () {
                this.destroy();
                return true;
            };
            return CTRDConflictButtonDlg;
        })(CButtonDlg);
        var CNCDButtonDlg = (function (_super) {
            __extends(CNCDButtonDlg, _super);
            function CNCDButtonDlg(aLayeredGroupControl, aHightHeight) {
                _super.call(this, aLayeredGroupControl);
                this.addClass('-ncd');
                if (aHightHeight) {
                    this.addClass('-high-height');
                }
            }
            CNCDButtonDlg.prototype.setDescription = function (aDesc) {
                this._textDesc = aDesc || "";
            };
            CNCDButtonDlg.prototype.setExpectedTime = function (aExpectedTime) {
                this._textExpectedTime = aExpectedTime;
            };
            CNCDButtonDlg.prototype.doOpenCustomProcess = function () {
                var _this = this;
                if (!(typeof this._textDesc === "string")) {
                    throw "setDescription(string[]) call first";
                }
                if (this._textExpectedTime) {
                    var timeLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                    timeLayout.setItemDrawers([
                        function (index, $item) {
                            $item.addClass('text');
                            $item.addClass('-date');
                            $item.removeClass('-f');
                            $item.html("Expected Date: " + _this._textExpectedTime);
                            return false;
                        }
                    ]);
                    this.insertChildControl(1, timeLayout, false);
                }
                var descLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                descLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.addClass('-desc');
                        $item.removeClass('-f');
                        $item.html(_this._textDesc);
                        return false;
                    }
                ]);
                this.insertChildControl(1, descLayout, false);
            };
            CNCDButtonDlg.prototype.doGetAlign = function () {
                return "hcenter";
            };
            CNCDButtonDlg.prototype.doKeyEscape = function () {
                this.close();
                //TODO: LATER
                return true;
            };
            CNCDButtonDlg.prototype.doKeyBack = function () {
                this.close();
                return true;
            };
            CNCDButtonDlg.prototype.doKeyUp = function () {
                return true;
            };
            CNCDButtonDlg.prototype.doKeyDown = function () {
                return true;
            };
            CNCDButtonDlg.prototype.doKey = function (aKey) {
                _super.prototype.doKey.call(this, aKey);
                return true;
            };
            return CNCDButtonDlg;
        })(CButtonDlg);
        var CHDAlternativeButtonDlg = (function (_super) {
            __extends(CHDAlternativeButtonDlg, _super);
            function CHDAlternativeButtonDlg(aLayeredGroupControl) {
                _super.call(this, aLayeredGroupControl);
                this.addClass('-hd-alter');
            }
            CHDAlternativeButtonDlg.prototype.setDescription = function (aDesc) {
                this._textDesc = aDesc || "";
            };
            CHDAlternativeButtonDlg.prototype.doOpenCustomProcess = function () {
                var _this = this;
                if (!(typeof this._textDesc === "string")) {
                    throw "setDescription(string[]) call first";
                }
                var descLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                descLayout.setItemDrawers([
                    function (index, $item) {
                        $item.addClass('text');
                        $item.addClass('-desc');
                        $item.removeClass('-f');
                        $item.html(_this._textDesc);
                        return false;
                    }
                ]);
                this.insertChildControl(1, descLayout, false);
            };
            CHDAlternativeButtonDlg.prototype.doGetAlign = function () {
                return "hcenter";
            };
            CHDAlternativeButtonDlg.prototype.doKeyEscape = function () {
                this.close();
                return true;
            };
            CHDAlternativeButtonDlg.prototype.doKeyBack = function () {
                this.close();
                return true;
            };
            CHDAlternativeButtonDlg.prototype.doKeyRec = function () {
                return true;
            };
            CHDAlternativeButtonDlg.prototype.doKeyUpIfNotHandled = function () {
                return true;
            };
            CHDAlternativeButtonDlg.prototype.doKeyDownIfNotHandled = function () {
                return true;
            };
            CHDAlternativeButtonDlg.prototype.doKeyLeftIfNotHandled = function () {
                return true;
            };
            CHDAlternativeButtonDlg.prototype.doKeyRightIfNotHandled = function () {
                return true;
            };
            CHDAlternativeButtonDlg.prototype.doKeyPageUpIfNotHandled = function () {
                return true;
            };
            CHDAlternativeButtonDlg.prototype.doKeyPageDownIfNotHandled = function () {
                return true;
            };
            return CHDAlternativeButtonDlg;
        })(CButtonDlg);
        if (!window.prism) {
            window.prism = {};
        }
        prism.createTRDConflictButtonDlg = function (aLayeredGroupControl, aTRDConflictInfo, aCallback) {
            var dlg;
            if (aLayeredGroupControl) {
                dlg = new CTRDConflictButtonDlg(aLayeredGroupControl);
                dlg.setTextHtml(tr('LOC_MULTIPLE_REGIONS_DETECTED_ID'));
                dlg.setButtonLayoutType('vertical');
                dlg.setButtonInfoList(aTRDConflictInfo);
                $("#DIA").append(aLayeredGroupControl._$self);
            }
            else {
                dlg = prism.createGlobalDialog('createButtonDlg', aTRDConflictInfo, aCallback);
            }
            return dlg;
        };
        prism.createNCDButtonDlg = function (aLayeredGroupControl, aNCDInfo, aCallback) {
            var dlg;
            if (aLayeredGroupControl) {
                dlg = new CNCDButtonDlg(aLayeredGroupControl, aNCDInfo.isValid);
                dlg.setTextHtml("NETWORK IS CHANGED");
                dlg.setDescription(aNCDInfo.aMessage);
                if (!aNCDInfo.isValid) {
                    dlg.setButtonInfoList([{
                        text: "NOW",
                        fn: function () {
                            if (aCallback) {
                                aCallback("NOW");
                            }
                        }
                    }, {
                        text: "LATER",
                        fn: function () {
                            if (aCallback) {
                                aCallback("LATER");
                            }
                        }
                    }]);
                }
                else {
                    dlg.setExpectedTime(aNCDInfo.aExpectedTime);
                    dlg.setButtonInfoList([{
                        text: "OK",
                        fn: function () {
                            if (aCallback) {
                                aCallback("OK");
                            }
                        }
                    }]);
                }
            }
            else {
                dlg = prism.createGlobalDialog('createNCDButtonDlg', aNCDInfo, aCallback);
            }
            return dlg;
        };
        prism.createHDAlternativeButtonDlg = function (aLayeredGroupControl, aButtonInfos) {
            var layeredGroupControl = aLayeredGroupControl;
            if (!layeredGroupControl) {
                layeredGroupControl = new __CONTROLS__.CLayeredGroupControl($("#wrap"));
            }
            var dlg = new CHDAlternativeButtonDlg(layeredGroupControl);
            // REPLACE_STRING
            dlg.setTextHtml(tr('LOC_HD_ALTERNATIVE_ID'));
            dlg.setDescription('The same programme is available in HD.<br>Select a programme for recording.');
            dlg.setButtonLayoutType('vertical');
            if (aButtonInfos[0].text) {
                aButtonInfos[0].text = "<div class=\"-hd\"/>" + aButtonInfos[0].text;
            }
            if (aButtonInfos[1].text) {
                aButtonInfos[1].text = "<div class=\"-sd\"/>" + aButtonInfos[1].text;
            }
            dlg.setButtonInfoList(aButtonInfos);
            /* COMMENT
             * if not prism page(CLayeredGroupControl) need to append #DIA
             * * bglee@humaxdigital.com, 20:32, 2014. 7. 29.
             */
            //$("#DIA").append(aLayeredGroupControl._$self);
            return dlg;
        };
        prism.createExtendedEventLinkageDescriptorButtonDlg = function (aLayeredGroupControl, aButtonInfos) {
            var layeredGroupControl = aLayeredGroupControl;
            if (!layeredGroupControl) {
                layeredGroupControl = new __CONTROLS__.CLayeredGroupControl($("#wrap"));
            }
            var dlg = new CHDAlternativeButtonDlg(layeredGroupControl);
            // REPLACE_STRING
            dlg.setTextHtml(tr('LOC_CHANNEL_ALTERNATIVE_ID'));
            dlg.setDescription(tr('LOC_SAME_PROGRAMMES_ID'));
            dlg.setButtonLayoutType('vertical');
            if (aButtonInfos[0].text) {
            }
            if (aButtonInfos[1].text) {
            }
            dlg.setButtonInfoList(aButtonInfos);
            /* COMMENT
             * if not prism page(CLayeredGroupControl) need to append #DIA
             * * bglee@humaxdigital.com, 20:32, 2014. 7. 29.
             */
            //$("#DIA").append(aLayeredGroupControl._$self);
            return dlg;
        };
        prism.createRCTButtonDlg = function (aLayeredGroupControl, aRctInfo, aCallback, aFnClosed) {
            var layeredGroupControl = aLayeredGroupControl;
            if (!layeredGroupControl) {
                layeredGroupControl = new __CONTROLS__.CLayeredGroupControl($("#wrap"));
            }
            var rctInfoDlg = new CRCTButtonDlg(layeredGroupControl, aRctInfo, aCallback, aFnClosed);
            return rctInfoDlg;
        };
    })(mPrism || (mPrism = {}));
    return mPrism;
});
