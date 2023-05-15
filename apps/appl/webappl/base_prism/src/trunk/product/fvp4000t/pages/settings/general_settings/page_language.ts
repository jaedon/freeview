/**
 * Created by bglee@humaxdigital.com on 2014. 5. 20..
 * Scenario 16.1.2
 */
import __SUPER__ = require("../../../../../pages/settings/general_settings/page_language");
declare var hx: ifSugar;

class CPgLanguage extends __SUPER__ {
    private _handlerPreferredLanguage = (aIndex: number) => {
        var subtitle = hx.svc.SETTING_UTIL.getLanguage().LangSubTtl;
        var len = this._handlers.length;
        if (subtitle) {
            if (subtitle === "off") {
                len--;
            }
        }
        for (var i = len - 1; i > 0; i--) {
            this._handlers[i](aIndex);
        }
    };
    constructor() {
        super();
        this._handlers.unshift(this._handlerPreferredLanguage);
    }
    doGetSubListFocusedIndex(aIndex: number, aArray: any[]): number {
        var ret: number;
        var valueSetted = hx.svc.SETTING_UTIL.getLanguage();
        var getBaseValue = [
            () => {
                return valueSetted.LangMenu;
            },
            () => {
                return valueSetted.LangMenu;
            },
            () => {
                return valueSetted.LangAudio;
            },
            () => {
                return valueSetted.LangSubTtl;
            }
        ][aIndex];
        var iso639: string = getBaseValue();
        if (aIndex !== 3) {
            ret = this._conv_iso639_to_idx(iso639);
        } else {
            ret = this._conv_iso639_to_subttlidx(iso639);
        }
        return ret;
    }
    doGetEntryStringID(aIndex: number): string {
        var base = [
            this._menuLang,
            this._menuLang,
            this._audioLang,
            this._subLang
        ];
        return this._conv_iso639_to_DispText(base[aIndex]);
    }
    doSetSubListFocusedIndex(aIndexMain: number, aIndexSub: number) {
    }
}
export = CPgLanguage;

