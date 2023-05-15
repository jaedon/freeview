///<reference path="../def/sugar.d.ts" />
/// <reference path="../def/prism.d.ts" />

import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CDIALOG__ = require('prismux/prism.dlg');
import __DEF__ = require("def/oipf.def");
declare var hx;

module mPrism {
    var mOpenCount = 0;
    export class CButtonDlg extends __PRISM_CDIALOG__.CDialog {
        _buttonLayoutType: string;
        _buttonInfoList: any[];
        _textHtml: string;
        _textDescription: string;
        _regions: string;
        _layout: any;
        static isOpened() {
            if (mOpenCount < 0) throw "check init & destroy routine";
            return mOpenCount ? true: false;
        }
        constructor(layeredGroupControl: __CONTROLS__.CLayeredGroupControl) {
            super();
            var a = this;
            a.init(layeredGroupControl);
            a._buttonLayoutType = 'horizontal';
        }
        init(layeredGroupControl) {
            var a = this;
            super.init.call(a, layeredGroupControl);
            a.addClass('buttonDlg');
            a._buttonInfoList = [{
                text : tr('LOC_OK_ID')
            }];
            mOpenCount += 1;
        }
        destroy() {
            var a = this;
            mOpenCount -= 1;
            a._buttonInfoList = null;
            super.destroy.call(a);
        }
        setTextHtml(textHtml) {
            var a = this;
            a._textHtml = textHtml;
        }
        setButtonInfoList(buttonInfoList) {
            var a = this;
            a._buttonInfoList = buttonInfoList;
        }
        setButtonLayoutType(buttonLayoutType) {
            var a = this;
            a._buttonLayoutType = buttonLayoutType;
        }
        doOpen(param) {
            var a = this;
            var verticalLayout = new CVerticalLayoutControl(null);
            verticalLayout.setItemDrawers([
                function(index, $item) {
                    $item.addClass("-title");
                    $item.removeClass('-f');
                    $item.html(a._textHtml);
                    return false;
                }]);
            verticalLayout.addClass('text');

            var buttonLayout, buttonDrawer;
            if (a._buttonLayoutType == 'vertical') {
                buttonLayout = new CVerticalLayoutControl(null);
                buttonDrawer = function(index, $item) {
                    $item.addClass('btn long').html(a._buttonInfoList[index].text);
                };
            } else {
                buttonLayout = new CHorizontalLayoutControl(null);
                buttonLayout.setDrawParam("childAlign", "hcenter");
                buttonDrawer = function(index, $item) {
                    $item.addClass('btn').html(a._buttonInfoList[index].text);
                };
            }
            var fnSetDrawer = function(drawer) {
                var drawers = [];
                if (a._buttonInfoList) {
                    var i;
                    for ( i = 0; i < a._buttonInfoList.length; i += 1) {
                        drawers.push(drawer);
                    }
                }
                buttonLayout.setItemDrawers(drawers);
            };
            fnSetDrawer(buttonDrawer);
            buttonLayout.setDrawParam({
                padding : 15
            });
            buttonLayout.addClass("-btn-ctrl");
            buttonLayout.onItemSelected = function(index, $item) {
                var fn = null;

                var fnAsync = null;
                if (a._buttonInfoList) {
                    fn = a._buttonInfoList[index].fn;
                    fnAsync = a._buttonInfoList[index].fnAsync;
                }
                if (fn) {
                    setTimeout(function() {
                        var nextInfo = fn(index, $item);
                        if (nextInfo) {
                            a._regions = nextInfo.title;
                            a.setButtonInfoList(nextInfo.data);
                            fnSetDrawer(buttonDrawer);
                            a.draw();
                        } else {
                            a.close('internal');
                        }
                    }, 1);
                } else if (fnAsync) {
                    fnAsync(index, function(nextInfo) {
                        if (nextInfo) {
                            a._regions = nextInfo.title;
                            a.setButtonInfoList(nextInfo.data);
                            fnSetDrawer(buttonDrawer);
                            a.draw();
                            a.setActiveFocus(true);
                        }
                    })
                } else {
                    a.close('internal');
                }
            };
            a.setChildControl([verticalLayout, buttonLayout], 1);
            this.doOpenCustomProcess();
            param = param ? param : {};
            a.setDrawParam({
                $parent : a._parent.$layer(),
                align : this.doGetAlign(),
                childAlign : this.doGetChildAlign(),
                padding : param.padding || 33,
                margins : param.margins || [33, 50, 60]
            });
        }
        doOpenCustomProcess(): void {
        }
        doGetAlign(): string {
            return "hcenter";
        }
        doGetChildAlign(): string {
            return "hcenter";
        }
    }

    prism.CButtonDlg = CButtonDlg;
    prism.createButtonDlg = function(layeredGroupControl, buttonDlgInfo, callback) {
        if (layeredGroupControl) {
            var buttonDlg = new CButtonDlg(layeredGroupControl);
            if (buttonDlgInfo) {
                if (buttonDlgInfo.textHtml) {
                    buttonDlg.setTextHtml(buttonDlgInfo.textHtml);
                }
                if (buttonDlgInfo.buttonList) {
                    var i, len, btnStr;
                    var buttonInfoList = [];
                    for (i=0, len=buttonDlgInfo.buttonList.length; i<len; i++) {
                        btnStr = buttonDlgInfo.buttonList[i];
                        buttonInfoList.push({
                            text: btnStr,
                            fn: (function(callback, i) {
                                return function() {
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
        } else {
            var globalDlg = prism.createGlobalDialog('createButtonDlg', buttonDlgInfo, callback);
            return globalDlg;
        }
    };
    prism.createReminderDlg = function(layeredGroupControl, reminderInfo, callback) {
        if (layeredGroupControl) {
            var buttonDlg = new CButtonDlg(layeredGroupControl);
            //buttonDlg.addClass('reminder');
            var textHtml = reminderInfo.title;
            textHtml += '<div class="text box">' //
                + '<dt class = "textW">' + reminderInfo.eventName + '</dt>' //
                + '<dt class = "textY">' //
                + '<span style="float:left;">' + reminderInfo.channelNo + ' ' + reminderInfo.channelName + '</span>' //
                + '<span style="float:right">' //
                + (new Date(reminderInfo.startTime)).format('HH:MM') + ' - ' //
                + (new Date(reminderInfo.endTime)).format('HH:MM') + '</span>' //
                + '</dt>' //
                + '</div>';
            buttonDlg.setTextHtml(textHtml);
            buttonDlg.setTimeout(20 * 1000);
            buttonDlg.setButtonInfoList([{
                text : tr('LOC_YES_ID'),
                fn: function() {
                    if (callback) {
                        callback(true);
                    }
                }
            }, {
                text : tr('LOC_NO_ID'),
                fn: function() {
                    if (callback) {
                        callback(false);
                    }
                }
            }]);
            return buttonDlg;
        } else {
            var globalDlg = prism.createGlobalDialog('createReminderDlg', reminderInfo, callback);
            return globalDlg;
        }
    };
    prism.createConfirmDlg = function(layeredGroupControl, confirm, callback) {
        if (layeredGroupControl) {
            var buttonDlg = new CButtonDlg(layeredGroupControl);
            //buttonDlg.addClass('reminder');
            var textHtml = confirm.title;
            textHtml += '<div class="text box wLine">' //
                + '<dt class = "textW">' //
                + '<span style="float:left;">' + confirm.label + '</span>' //
                + '<span style="float:right">' //
                + confirm.value
                + '</dt>' //
                + '</div>';
            buttonDlg.setTextHtml(textHtml);
            //buttonDlg.setTimeout(20 * 1000);
            buttonDlg.setButtonInfoList([{
                text : tr('LOC_OK_ID'),
                fn: function() {
                    if (callback) {
                        callback(true);
                    }
                }
            }]);
            return buttonDlg;
        } else {
            var globalDlg = prism.createGlobalDialog('createConfirmDlg', confirm, callback);
            return globalDlg;
        }
    };
    prism.createInfoDlg = function(layeredGroupControl, info, callback) {
        if (layeredGroupControl) {
            var buttonDlg = new CButtonDlg(layeredGroupControl);
            buttonDlg.addClass('info');
            buttonDlg.setTextHtml(info.textHtml);
            buttonDlg.setButtonInfoList([]);
            //buttonDlg.setTimeout(20 * 1000);
            return buttonDlg;
        } else {
            var globalDlg = prism.createGlobalDialog('createInfoDlg', info, callback);
            return globalDlg;
        }
    };

    class CRCTButtonDlg extends CButtonDlg {
        private _rctInfo: RctLinkInfoCollection;
        private _rctIndex = 0;
        private _$arrowLeft = $("<div>").attr("class", "-arrow -left");
        private _$arrowRight = $("<div>").attr("class", "-arrow -right");
        //private _$recSingle = $("<div>").attr("class", "-schedule -single");
        //private _$recSeries = $("<div>").attr("class", "-schedule -series");
        private _textDesc = "No Information";
        private onCheckSchedule: (aRctInfo: RctLinkInfo) => void;
        static CLASS = {
            SHOW: "-show",
            SINGLE: "-single",
            SERIES: "-series"
        };
        constructor(aLayeredGroupControl: __CONTROLS__.CLayeredGroupControl, aRctInfo: RctLinkInfoCollection, aCallback: (aRctInfo: RctLinkInfo, aIsReminder: boolean) => void, aFnClosed: () => void) {
            super(aLayeredGroupControl);
            var rctItem = aRctInfo.item(0);


            this._rctInfo = aRctInfo;
            this.setButtonInfoList([{
                text: __DEF__.eTermID[rctItem.termId],
                fn: (aIndex) => {
                    aCallback(this._rctInfo.item(this._rctIndex), !!aIndex);
                }
            //}, {
            //    text: 'SET REMINDER',
            //    fn: (aIndex) => {
            //        aCallback(this._rctInfo.item(this._rctIndex), !!aIndex);
            //    }
            }]);
            this._buttonLayoutType = 'vertical';
            this.addClass('-rct-info');
            this.setTextHtml(rctItem.eventName);
            this.setDescription(rctItem.promotionalText);
            if (aRctInfo.length > 1) {
                this.showRightArrow();
            }
            this._$self
                .append(this._$arrowLeft)
                .append(this._$arrowRight);

            this.open({
                onClosed: aFnClosed
            });
        }
        setOnCheckSchedule(onCheckSchedule: (aRctInfo: RctLinkInfo) => void): void {
            this.onCheckSchedule = onCheckSchedule;
        }
        doCheckSchedule(): void {
            this.onCheckSchedule(this._rctInfo.item(this._rctIndex));
        }
        doGetChildAlign(): string {
            return "";
        }
        setDescription(aDesc: string) {
            this._textDesc = aDesc || this._textDesc;
        }
        showLeftArrow(): void {
            this._$arrowLeft.addClass(CRCTButtonDlg.CLASS.SHOW);
        }
        hideLeftArrow(): void {
            this._$arrowLeft.removeClass(CRCTButtonDlg.CLASS.SHOW);
        }
        showRightArrow(): void {
            this._$arrowRight.addClass(CRCTButtonDlg.CLASS.SHOW);
        }
        hideRightArrow(): void {
            this._$arrowRight.removeClass(CRCTButtonDlg.CLASS.SHOW);
        }
        showRecSingle(): void {
            this._controls[0]._$self.addClass(CRCTButtonDlg.CLASS.SINGLE);
        }
        hideRecSingle(): void {
            this._controls[0]._$self.removeClass(CRCTButtonDlg.CLASS.SINGLE);
        }
        showRecSeries(): void {
            this._controls[0]._$self.addClass(CRCTButtonDlg.CLASS.SERIES);
        }
        hideRecSeries(): void {
            this._controls[0]._$self.removeClass(CRCTButtonDlg.CLASS.SERIES);
        }
        setTextHtml(aText: string) {
            super.setTextHtml("<span class=\"-schedule\"></span>" + aText);
        }
        doKeyLeft() {
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
        }
        doKeyRight() {
            console.log("right")
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
        }
        doKeyGreen() {
            return true;
        }
        doKeyUpIfNotHandled() {
            return true;
        }
        doKeyDownIfNotHandled() {
            return true;
        }
        doKeyBack() {
            this.close();
            return true;
        }
        doKeyEscape() {
            this.close();
            return false;
        }
        doKeyRecord() {
            return true;
        }
        doOpenCustomProcess() {
            var descLayout = new __CONTROLS__.CVerticalLayoutControl(null);
            descLayout.setItemDrawers([
                (index, $item) => {
                    $item.addClass('-desc');
                    $item.removeClass('-f');
                    $item.html(this._textDesc);
                    return false;
                }
            ]);
            descLayout.addClass('text');
            descLayout._$self.css("top", "190px");
            this.insertChildControl(1, descLayout, false);
        }
        doGetAlign() {
            return "";
        }
    }
    class CTRDConflictButtonDlg extends CButtonDlg {
        constructor(aLayeredGroupControl: __CONTROLS__.CLayeredGroupControl) {
            super(aLayeredGroupControl);
            this.addClass('-trd');
        }
        doOpenCustomProcess() {
            var descLayout = new __CONTROLS__.CVerticalLayoutControl(null);
            descLayout.setItemDrawers([
                function(index, $item) {
                    $item.addClass('text');
                    $item.removeClass('-f');
                    $item.html(tr('LOC_SELECT_REGIONS_ID'));
                    return false;
                }]);
            var regionLayout = new __CONTROLS__.CVerticalLayoutControl(null);
            regionLayout.setItemDrawers([
                (index, $item) => {
                    $item.addClass('text');
                    $item.removeClass('-f');
                    $item.html(this._regions);
                    return false;
                }]);
            this.insertChildControl(1, regionLayout, false);
            this.insertChildControl(1, descLayout, false);
        }
        doGetAlign() {
            return "";
        }
        doKeyEscape() {
            this.destroy();
            return true;
        }
        doKeyBack() {
            this.destroy();
            return true;
        }
    }
    class CNCDButtonDlg extends CButtonDlg {
        private _textDesc: string;
        private _textExpectedTime: string;
        constructor(aLayeredGroupControl: __CONTROLS__.CLayeredGroupControl, aHightHeight: boolean) {
            super(aLayeredGroupControl);
            this.addClass('-ncd');
            if (aHightHeight) {
                this.addClass('-high-height');
            }
        }
        setDescription(aDesc: string) {
            this._textDesc = aDesc || "";
        }
        setExpectedTime(aExpectedTime: string) {
            this._textExpectedTime = aExpectedTime;
        }
        doOpenCustomProcess() {
            if (!(typeof this._textDesc === "string")) {
                throw "setDescription(string[]) call first";
            }
            if (this._textExpectedTime) {
                var timeLayout = new __CONTROLS__.CVerticalLayoutControl(null);
                timeLayout.setItemDrawers([
                    (index, $item) => {
                        $item.addClass('text');
                        $item.addClass('-date');
                        $item.removeClass('-f');
                        $item.html("Expected Date: " + this._textExpectedTime);
                        return false;
                    }
                ]);
                this.insertChildControl(1, timeLayout, false);
            }
            var descLayout = new __CONTROLS__.CVerticalLayoutControl(null);
            descLayout.setItemDrawers([
                (index, $item) => {
                    $item.addClass('text');
                    $item.addClass('-desc');
                    $item.removeClass('-f');
                    $item.html(this._textDesc);
                    return false;
                }
            ]);
            this.insertChildControl(1, descLayout, false);
        }
        doGetAlign() {
            return "hcenter";
        }
        doKeyEscape() {
            this.close();
            //TODO: LATER
            return true;
        }
        doKeyBack() {
            this.close();
            return true;
        }
        doKeyUp() {
            return true;
        }
        doKeyDown() {
            return true;
        }
        doKey(aKey) {
            super.doKey(aKey);
            return true;
        }
    }
    class CHDAlternativeButtonDlg extends CButtonDlg {
        private _textDesc: string;
        constructor(aLayeredGroupControl: __CONTROLS__.CLayeredGroupControl) {
            super(aLayeredGroupControl);
            this.addClass('-hd-alter');
        }
        setDescription(aDesc: string) {
            this._textDesc = aDesc || "";
        }
        doOpenCustomProcess() {
            if (!(typeof this._textDesc === "string")) {
                throw "setDescription(string[]) call first";
            }
            var descLayout = new __CONTROLS__.CVerticalLayoutControl(null);
            descLayout.setItemDrawers([
                (index, $item) => {
                    $item.addClass('text');
                    $item.addClass('-desc');
                    $item.removeClass('-f');
                    $item.html(this._textDesc);
                    return false;
                }
            ]);
            this.insertChildControl(1, descLayout, false);
        }
        doGetAlign() {
            return "hcenter";
        }
        doKeyEscape() {
            this.close();
            return true;
        }
        doKeyBack() {
            this.close();
            return true;
        }
        doKeyRec() {
            return true;
        }
        doKeyUpIfNotHandled() {
            return true;
        }
        doKeyDownIfNotHandled() {
            return true;
        }
        doKeyLeftIfNotHandled() {
            return true;
        }
        doKeyRightIfNotHandled() {
            return true;
        }
        doKeyPageUpIfNotHandled() {
            return true;
        }
        doKeyPageDownIfNotHandled() {
            return true;
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createTRDConflictButtonDlg = function(aLayeredGroupControl, aTRDConflictInfo: ifPrismDlgButtonInfoAsync[], aCallback?) {
        var dlg;
        if (aLayeredGroupControl) {
            dlg = new CTRDConflictButtonDlg(aLayeredGroupControl);
            dlg.setTextHtml(tr('LOC_MULTIPLE_REGIONS_DETECTED_ID'));
            dlg.setButtonLayoutType('vertical');
            dlg.setButtonInfoList(aTRDConflictInfo);
            $("#DIA").append(aLayeredGroupControl._$self);
        } else {
            dlg = prism.createGlobalDialog('createButtonDlg', aTRDConflictInfo, aCallback);
        }
        return dlg;
    };
    prism.createNCDButtonDlg = function(
        aLayeredGroupControl: __CONTROLS__.CLayeredGroupControl, aNCDInfo, aCallback?) {
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
            } else {
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
        } else {
            dlg = prism.createGlobalDialog('createNCDButtonDlg', aNCDInfo, aCallback);
        }
        return dlg;
    };
    prism.createHDAlternativeButtonDlg = function(aLayeredGroupControl: __CONTROLS__.CLayeredGroupControl, aButtonInfos: ifPrismDlgButtonInfo[]) {
        var layeredGroupControl = aLayeredGroupControl;
        if (!layeredGroupControl) {
            layeredGroupControl = new __CONTROLS__.CLayeredGroupControl($("#wrap"));
        }
        var dlg = new CHDAlternativeButtonDlg(layeredGroupControl);
        // REPLACE_STRING
        dlg.setTextHtml(tr('LOC_HD_ALTERNATIVE_ID') );
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
    prism.createExtendedEventLinkageDescriptorButtonDlg = function(aLayeredGroupControl: __CONTROLS__.CLayeredGroupControl, aButtonInfos: ifPrismDlgButtonInfo[]) {
        var layeredGroupControl = aLayeredGroupControl;
        if (!layeredGroupControl) {
            layeredGroupControl = new __CONTROLS__.CLayeredGroupControl($("#wrap"));
        }
        var dlg = new CHDAlternativeButtonDlg(layeredGroupControl);
        // REPLACE_STRING

        dlg.setTextHtml(tr('LOC_CHANNEL_ALTERNATIVE_ID') );
        dlg.setDescription(tr('LOC_SAME_PROGRAMMES_ID') );
        dlg.setButtonLayoutType('vertical');
        if (aButtonInfos[0].text) {
            //aButtonInfos[0].text = "<div class=\"-hd\"/>" + aButtonInfos[0].text;
        }
        if (aButtonInfos[1].text) {
            //aButtonInfos[1].text = "<div class=\"-sd\"/>" + aButtonInfos[1].text;
        }
        dlg.setButtonInfoList(aButtonInfos);
        /* COMMENT
         * if not prism page(CLayeredGroupControl) need to append #DIA
         * * bglee@humaxdigital.com, 20:32, 2014. 7. 29.
         */
        //$("#DIA").append(aLayeredGroupControl._$self);

        return dlg;
    };
    prism.createRCTButtonDlg = function(aLayeredGroupControl: __CONTROLS__.CLayeredGroupControl, aRctInfo: RctLinkInfoCollection, aCallback: (aRctInfo: RctLinkInfo, aIsReminder: boolean) => void, aFnClosed: () => void) {
        var layeredGroupControl = aLayeredGroupControl;
        if (!layeredGroupControl) {
            layeredGroupControl = new __CONTROLS__.CLayeredGroupControl($("#wrap"));
        }
        var rctInfoDlg = new CRCTButtonDlg(layeredGroupControl, aRctInfo, aCallback, aFnClosed);
        return rctInfoDlg;
    };
}
export = mPrism;
