///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cResponse.js
/*jslint sloppy:true, nomen:true, vars:true */

/*
 * message component
 * @param
 */
declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class responseAVC extends __COMPONENT_KERNEL__ {
    _$audT: any;
    _$audR: any;
    _$audL: any;
    _$subT: any;
    _$subH: any;
    _msg_type: string;

    constructor() {
        super();
    }
    create(aParam) {
        hx.logadd('responseAVC');
        hx.log('responseAVC', 'create');
        var iconL: string = 'spl on';
        var iconR: string = 'spr on';
        var iconDolby: string = '';
        var icon: string = '';
        var iconHearing: string = '';
        if (aParam._msg_type === 'audio') {
            if (aParam.encoding === 'AC3') {
                iconDolby = 'alTitleAC3';
            } else if (aParam.encoding === 'E-AC3') {
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
                'attr': {'class': 'alTitleWrap'}
            });
            this.$.append(this._$audT).append(this._$audL).append(this._$audR);
        } else if (aParam._msg_type === 'audiodescription') {
            $.extend(this, aParam, {
                _$audT: $('<span>', {
                    'class': null,//iconDolby,
                    'text': aParam.text
                })
            });
            this._create({
                'tag': 'div',
                'attr': {'class': 'alTitleWrap'}
            });
            this.$.append(this._$audT);
        } else if (aParam._msg_type === 'subtitle') {
            // DVB: 201, EBU: 202
            if (aParam.subType === 201) {
                icon = 'sTitleDVB';
            } else if (aParam.subType === 202) {
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
                'attr': {'class': 'sTitleWrap'}
            });
            this.$.append(this._$subT).append(this._$subH);
        } else {
            this._create({
                'tag': 'div',
                'attr': {'class': ''}
            });
        }
        return this;
    }

    destroy() {
        this.$.remove();
    }
}
