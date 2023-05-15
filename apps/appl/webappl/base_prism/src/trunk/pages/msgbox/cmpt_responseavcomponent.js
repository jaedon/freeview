///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cResponse.js
/*jslint sloppy:true, nomen:true, vars:true */
var __extends = this.__extends || function (d, b) {
    for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p];
    function __() { this.constructor = d; }
    __.prototype = b.prototype;
    d.prototype = new __();
};
define(["require", "exports", "pages/component_kernel"], function (require, exports, __COMPONENT_KERNEL__) {
    var responseAVC = (function (_super) {
        __extends(responseAVC, _super);
        function responseAVC() {
            _super.call(this);
        }
        responseAVC.prototype.create = function (aParam) {
            hx.logadd('responseAVC');
            hx.log('responseAVC', 'create');
            var iconL = 'spl on';
            var iconR = 'spr on';
            var iconDolby = '';
            var icon = '';
            var iconHearing = '';
            if (aParam._msg_type === 'audio') {
                if (aParam.encoding === 'AC3') {
                    iconDolby = 'alTitleAC3';
                }
                else if (aParam.encoding === 'E-AC3') {
                    iconDolby = 'alTitleAC3P';
                }
                if (aParam.streoL === true) {
                    iconL = 'spl';
                }
                if (aParam.streoR === true) {
                    iconR = 'spr';
                }
                $.extend(this, aParam, {
                    _$audT: $('<span>', {
                        'class': iconDolby,
                        'text': aParam.text
                    }),
                    _$audL: $('<p>', {
                        'class': iconL,
                        'text': 'L'
                    }),
                    _$audR: $('<p>', {
                        'class': iconR,
                        'text': 'R'
                    })
                });
                this._create({
                    'tag': 'div',
                    'attr': { 'class': 'alTitleWrap' }
                });
                this.$.append(this._$audT).append(this._$audL).append(this._$audR);
            }
            else if (aParam._msg_type === 'audiodescription') {
                $.extend(this, aParam, {
                    _$audT: $('<span>', {
                        'class': null,
                        'text': aParam.text
                    })
                });
                this._create({
                    'tag': 'div',
                    'attr': { 'class': 'alTitleWrap' }
                });
                this.$.append(this._$audT);
            }
            else if (aParam._msg_type === 'subtitle') {
                // DVB: 201, EBU: 202
                if (aParam.subType === 201) {
                    icon = 'sTitleDVB';
                }
                else if (aParam.subType === 202) {
                    icon = 'sTitleEBU';
                }
                // Support Hard of Hearing
                if (aParam.hardofHearing === true) {
                    icon = 'sTitleHardofHearing';
                    iconHearing = 'subHardofHearing';
                }
                $.extend(this, aParam, {
                    _$subT: $('<span>', {
                        'class': icon,
                        'text': aParam.text
                    }),
                    _$subH: $('<span>', {
                        'class': iconHearing
                    })
                });
                this._create({
                    'tag': 'div',
                    'attr': { 'class': 'sTitleWrap' }
                });
                this.$.append(this._$subT).append(this._$subH);
            }
            else {
                this._create({
                    'tag': 'div',
                    'attr': { 'class': '' }
                });
            }
            return this;
        };
        responseAVC.prototype.destroy = function () {
            this.$.remove();
        };
        return responseAVC;
    })(__COMPONENT_KERNEL__);
    exports.responseAVC = responseAVC;
});
