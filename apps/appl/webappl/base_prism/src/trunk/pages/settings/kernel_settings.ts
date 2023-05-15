///<reference path="../../def/sugar.d.ts" />
// file name : pages/settings/settings.js
/*jslint nomen: true, undef: true, sloppy: true */

/* settings main menu initializer */
/* COMMENT
 * if you need to use product oriented data, extends this and this.productData
 * * bglee@humaxdigital.com, 19:32, 2014. 5. 19.
 */

declare var hx: ifSugar;

import __PAGE_KERNEL__ = require("pages/page_kernel");
class CPageSettings extends __PAGE_KERNEL__ {
    private _productData: ifDataSettingCategory;
    private _dataCutted: any = null;

    constructor(aEvents?) {
        super(aEvents);
        this._productData = ENV.productData.settings;
    }

    create(aEntity?, aPageUrl?){
        hx.log('error','Kernel_settings create');
        hx.svc.CLOCK.startClock();
        this.showDateTime();
        this.createBackground();
    }

    destroy(){
        super.destroy();//removeEventCB on hx.svc.CLOCK
        hx.svc.CLOCK.stopClock();
        this.removeBackground();
    }

    showDateTime(): JQuery {
        var _dateTime = $('.dateTime');
        if(_dateTime.length>0){
            //addEventListener            
            var tagChildren = _dateTime.children();
            if(tagChildren.length >2){
                hx.svc.CLOCK.addEventCb('MinuteChanged', this, (aNow) => {                   
                    tagChildren[1].innerHTML = dateFormat(aNow, "dd.mm.yyyy");
                    tagChildren[2].innerHTML = dateFormat(aNow, "ddd HH:MM").toUpperCase();
                });
            }
            return null;
        }else{
            return super.showDateTime();
        }
    }

    createBackground() {
        if (!$('.prism-page.setting').length) {
            var $bg = $('<div>', {
                'class' : 'prism-page setting'
            });
            $('#wrap').append($bg);
            hx.il.updatePageBackground();
            $bg.addClass('show');
        }
    }

    removeBackground() {
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
    }

    get productData(): ifDataSettingCategory {
        return this._productData;
    }
    setDataCutted(aData: any) {
        this._dataCutted = aData;
    }
    getHandlerSelectionMenu(): ifDataHandlerSelectionMenu {
        var context = <ifDataSettingSelectionMenu<any>>this._dataCutted;
        var items: ifDataSettingSelectionMenu<string>[] = context.items;
        var titles: string[] = [];
        var i;
        var len = items.length;
        for (i = 0; i < len; i++) {
            titles[i] = items[i].title;
        }
        return {
            getTitle: function() {
                return context.title;
            },
            getItemTitles: function() {
                return titles;
            },
            getItemOptions: function(aItemTitle: string) {
                var index = titles.indexOf(aItemTitle);
                if (index === -1) {
                    throw "can't find title name";
                }
                return items[index].items;
            }
        }
    }
}
export = CPageSettings;
