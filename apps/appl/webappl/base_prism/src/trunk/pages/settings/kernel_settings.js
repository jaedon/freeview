///<reference path="../../def/sugar.d.ts" />
// file name : pages/settings/settings.js
/*jslint nomen: true, undef: true, sloppy: true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/page_kernel"], function (require, exports, __PAGE_KERNEL__) {
    var CPageSettings = (function (_super) {
        __extends(CPageSettings, _super);
        function CPageSettings(aEvents) {
            _super.call(this, aEvents);
            this._dataCutted = null;
            this._productData = ENV.productData.settings;
        }
        CPageSettings.prototype.create = function (aEntity, aPageUrl) {
            hx.log('error', 'Kernel_settings create');
            hx.svc.CLOCK.startClock();
            this.showDateTime();
            this.createBackground();
        };
        CPageSettings.prototype.destroy = function () {
            _super.prototype.destroy.call(this); //removeEventCB on hx.svc.CLOCK
            hx.svc.CLOCK.stopClock();
            this.removeBackground();
        };
        CPageSettings.prototype.showDateTime = function () {
            var _dateTime = $('.dateTime');
            if (_dateTime.length > 0) {
                //addEventListener            
                var tagChildren = _dateTime.children();
                if (tagChildren.length > 2) {
                    hx.svc.CLOCK.addEventCb('MinuteChanged', this, function (aNow) {
                        tagChildren[1].innerHTML = dateFormat(aNow, "dd.mm.yyyy");
                        tagChildren[2].innerHTML = dateFormat(aNow, "ddd HH:MM").toUpperCase();
                    });
                }
                return null;
            }
            else {
                return _super.prototype.showDateTime.call(this);
            }
        };
        CPageSettings.prototype.createBackground = function () {
            if (!$('.prism-page.setting').length) {
                var $bg = $('<div>', {
                    'class': 'prism-page setting'
                });
                $('#wrap').append($bg);
                hx.il.updatePageBackground();
                $bg.addClass('show');
            }
        };
        CPageSettings.prototype.removeBackground = function () {
            var centerPage = this.getCenterPageName();
            switch (centerPage) {
                case "CPgLiveController":
                case "CPgRecordings":
                case "CPgGuide":
                case "CPgMediaCenter":
                case "CPgVideo":
                case "CPgMusic":
                case "CPgPhoto":
                case "CPgOnDemand":
                case "CPgTopPicks":
                case "CPgStandby":
                    $('.prism-page.setting').remove();
                    break;
                default:
                    break;
            }
        };
        Object.defineProperty(CPageSettings.prototype, "productData", {
            get: function () {
                return this._productData;
            },
            enumerable: true,
            configurable: true
        });
        CPageSettings.prototype.setDataCutted = function (aData) {
            this._dataCutted = aData;
        };
        CPageSettings.prototype.getHandlerSelectionMenu = function () {
            var context = this._dataCutted;
            var items = context.items;
            var titles = [];
            var i;
            var len = items.length;
            for (i = 0; i < len; i++) {
                titles[i] = items[i].title;
            }
            return {
                getTitle: function () {
                    return context.title;
                },
                getItemTitles: function () {
                    return titles;
                },
                getItemOptions: function (aItemTitle) {
                    var index = titles.indexOf(aItemTitle);
                    if (index === -1) {
                        throw "can't find title name";
                    }
                    return items[index].items;
                }
            };
        };
        return CPageSettings;
    })(__PAGE_KERNEL__);
    return CPageSettings;
});
