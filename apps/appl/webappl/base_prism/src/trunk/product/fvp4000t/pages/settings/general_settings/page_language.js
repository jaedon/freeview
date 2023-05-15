var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "../../../../../pages/settings/general_settings/page_language"], function (require, exports, __SUPER__) {
    var CPgLanguage = (function (_super) {
        __extends(CPgLanguage, _super);
        function CPgLanguage() {
            var _this = this;
            _super.call(this);
            this._handlerPreferredLanguage = function (aIndex) {
                var subtitle = hx.svc.SETTING_UTIL.getLanguage().LangSubTtl;
                var len = _this._handlers.length;
                if (subtitle) {
                    if (subtitle === "off") {
                        len--;
                    }
                }
                for (var i = len - 1; i > 0; i--) {
                    _this._handlers[i](aIndex);
                }
            };
            this._handlers.unshift(this._handlerPreferredLanguage);
        }
        CPgLanguage.prototype.doGetSubListFocusedIndex = function (aIndex, aArray) {
            var ret;
            var valueSetted = hx.svc.SETTING_UTIL.getLanguage();
            var getBaseValue = [
                function () {
                    return valueSetted.LangMenu;
                },
                function () {
                    return valueSetted.LangMenu;
                },
                function () {
                    return valueSetted.LangAudio;
                },
                function () {
                    return valueSetted.LangSubTtl;
                }
            ][aIndex];
            var iso639 = getBaseValue();
            if (aIndex !== 3) {
                ret = this._conv_iso639_to_idx(iso639);
            }
            else {
                ret = this._conv_iso639_to_subttlidx(iso639);
            }
            return ret;
        };
        CPgLanguage.prototype.doGetEntryStringID = function (aIndex) {
            var base = [
                this._menuLang,
                this._menuLang,
                this._audioLang,
                this._subLang
            ];
            return this._conv_iso639_to_DispText(base[aIndex]);
        };
        CPgLanguage.prototype.doSetSubListFocusedIndex = function (aIndexMain, aIndexSub) {
        };
        return CPgLanguage;
    })(__SUPER__);
    return CPgLanguage;
});
