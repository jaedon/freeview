///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cHelp.js
/*jslint sloppy:true, nomen:true, vars:true*/

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class HelpView extends __COMPONENT_KERNEL__ {
    aniAction = hx.il.ani.action2;
    isShow : boolean = false;
    _step : number = 0;
    _sceneIndex : number = 0;
    _type : string = '';
    dissolveInterval : any  = null;
    _contents : any  = hx.config.help;
    parent : any;
    $arrowL : any;
    $arrowR : any;
    _close : any;
    _leftArr : any;
    _rightArr : any;
    _stepList : any;
    _indicate : any;
    $step : any;
    $tit : any;
    $indicate : any;
    $close : any;
    section;
    $pmo : any;
    $pmoDesp : any;
    $pmoItem : any;
    _nativeFocus : boolean;
    callback : any;

    //animation info
    aniStyleMap : any = {
        fadeIn : {
            start : {
                'opacity' : 0
            },
            end : {
                'opacity' : 1
            }
        },
        fadeOut : {
            start : {
                'opacity' : 1
            },
            end : {
                'opacity' : 0
            }
        },
        step : {
            fadeIn : {
                start : {
                    'opacity' : 0,
                    '-o-transform' : 'scale(0.9,0.9)'
                },
                end : {
                    'opacity' : 1,
                    '-o-transform' : 'scale(1.0,1.0)'
                }
            },
            fadeOut : {
                start : {
                    'opacity' : 1,
                    '-o-transform' : 'scale(0.9,0.9)'
                },
                end : {
                    'opacity' : 0,
                    '-o-transform' : 'scale(1.0,1.0)'
                }
            }
        },
        pmoItem : {
            nextOut : {
                start : {
                    'opacity' : 1,
                    'left' : 0
                },
                end : {
                    'opacity' : 0,
                    'left' : '-30px'
                }
            },
            nextIn : {
                start : {
                    'opacity' : 0,
                    'left' : '70px'
                },
                end : {
                    'opacity' : 1,
                    'left' : 0
                }
            },
            prevOut : {
                start : {
                    'opacity' : 1,
                    'left' : 0
                },
                end : {
                    'opacity' : 0,
                    'left' : '30px'
                }
            },
            prevIn : {
                start : {
                    'opacity' : 0,
                    'left' : '-70px'
                },
                end : {
                    'opacity' : 1,
                    'left' : 0
                }
            }
        }
    };
    private _cbFnEnd: Function;

    constructor() {
        super();
    }
    init () {

    }
    _changeScene (aParam) : void {
        var stepLists : any = aParam.section.querySelectorAll('ul');
        var scenesLists : any = stepLists[aParam._step].querySelectorAll('li');
        if (scenesLists.length <= 1) {
            clearInterval(aParam.dissolveInterval);
            aParam.dissolveInterval = null;
            return;
        }

        var $currentDesp : any = $(scenesLists[aParam._sceneIndex]);
        aParam._sceneIndex += 1;
        if (aParam._sceneIndex === scenesLists.length) {
            aParam._sceneIndex = 0;
        }
        var $newDesp : any = $(scenesLists[aParam._sceneIndex]);
        $newDesp.show();
        $currentDesp.ani({
            style : aParam.aniStyleMap.fadeOut,
            trans : {
                dur : 350,
                delay : 100
            }
        });
        $newDesp.ani({
            style : aParam.aniStyleMap.fadeIn,
            trans : {
                dur : 320
            }
        });
    }
    execAni () : void {
        var $hep = this.$, $step = this.$step, $tit = this.$tit, $pmoItem = this.$pmoItem, //
            $indicate = this.$indicate, $arrowR = this.$arrowR, //
            aniInfo : string = this.isShow ? 'fadeOut' : 'fadeIn', that = this, fadeIn : any = this.aniStyleMap.fadeIn;

        $hep.ani({
            style : fadeIn,
            trans : {
                dur : 300
            }
        });
        $step.ani({
            style : this.aniStyleMap.step.fadeIn,
            trans : {
                dur : 320,
                delay : 300
            }
        });
        $tit.ani({
            style : fadeIn,
            trans : {
                dur : 420,
                delay : 300
            }
        });

        $indicate.ani({
            style : fadeIn,
            trans : {
                dur : 420,
                delay : 760
            }
        });
        $arrowR.ani({
            style : fadeIn,
            trans : {
                dur : 420,
                delay : 300
            }
        });
        $pmoItem.ani({
            style : fadeIn,
            trans : {
                dur : 420,
                delay : 300
            },
            cb : function() {
                if (that.dissolveInterval !== null) {
                    clearInterval(that.dissolveInterval);
                    that.dissolveInterval = null;
                }
                that.dissolveInterval = setInterval(that._changeScene, 7000, that);
            }
        });
        this.isShow = !this.isShow;
    }

    create (aParam : any) : void  {
        this._nativeFocus = false;
        this.dissolveInterval = null;
        //variable
        $.extend(this, aParam);

        //this._type = 'TVG';
        this.section = this.getTemplateEl({
            'step' : this._step,
            'type' : this._type
        });

        this.$ = $(this.section);
        // close button
        this._close = this.getOtherTemplateEl({
            'id' : 'close',
            'cls' : 'close'
        });

        // arrow button
        this._leftArr = this.getOtherTemplateEl({
            'id' : 'arL',
            'cls' : 'arrow arL'
        });
        this._rightArr = this.getOtherTemplateEl({
            'id' : 'arR',
            'cls' : 'arrow arR'
        });

        this._stepList = this.getStepListTemplateEl(this._contents[this._type].step);

        // indicate
        this._indicate = this.getIndecateTemplateEl({
            'len' : this._contents[this._type].step.length
        });
        this._indicate.querySelectorAll('p')[0].classList.add('on');

        var article : any  = this.section.querySelector('#step');
        article.appendChild(this._close);
        article.appendChild(this._leftArr);
        article.appendChild(this._rightArr);
        article.appendChild(this._stepList);
        article.appendChild(this._indicate);

        this.$tit = this.$.find('.tit');
        this.$step = this.$.find('.step');
        this.$pmo = this.$.find('.wrap_pmo');
        this.$pmoDesp = this.$.find('.pmoDesp');
        this.$pmoItem = $(this._stepList);
        this.$arrowR = $(this._rightArr);
        this.$arrowL = $(this._leftArr);
        this.$indicate = $(this._indicate).find('p');
        this.$close = $(this._close);

        this.execAni();
    }

    /**
     * @private
     * @event
     * @param {String}

     */

    _moveStep (aAction : string) {
        var currentStep : number = this._step;
        var isPrev : boolean = (aAction === 'prev');
        var nextFlag : number = (aAction === 'prev') ? -1 : 1;
        var nextStep : number = this._step + nextFlag;
        var isFirst : boolean = (isPrev && currentStep === 1);
        var isLast : boolean = (!isPrev && currentStep === this._contents[this._type].step.length - 2);
        var toBeShowArrowL : boolean = (!isPrev && currentStep === 0);
        var toBeShowArrowR : boolean = (isPrev && currentStep === this._contents[this._type].step.length - 1);
        var toChangetxt : boolean = (this._contents[this._type].step[currentStep][0].txtnum !== this._contents[this._type].step[nextStep][0].txtnum) ? true : false;
        var changeInfo = {
            action : aAction,
            isPrev : isPrev,
            nextFlag : nextFlag,
            isFirst : isFirst,
            isLast : isLast,
            toBeShowArrowL : toBeShowArrowL,
            toBeShowArrowR : toBeShowArrowR,
            toChangetxt : toChangetxt
        };
        this.changeStepOut(changeInfo);
    }

    changeStepOut (aArg: {isLast: boolean; isFirst: boolean; toChangetxt: boolean; action: string}) {
        // contentsSteps = contents.step;
        var that = this;
        var stepLists : any = that.section.querySelectorAll('ul');
        var stepTitle : any = document.querySelector('#stepTitle');
        var fadeOut : any = this.aniStyleMap.fadeOut;
        var $pmoItem : any = $(this.$pmo.find('.pmoItem').get(this._step));
        var $tit = this.$tit;

        if (aArg.isLast) {
            this.$arrowR.ani({
                style : fadeOut,
                trans : {
                    dur : 300
                }
            });
        } else if (this.$close.css('opacity') === '1') {
            this.$close.ani({
                style : fadeOut,
                trans : {
                    dur : 100
                }
            });
        }
        if (aArg.isFirst) {
            this.$arrowL.ani({
                style : fadeOut,
                trans : {
                    dur : 300
                }
            });
        }

        //tit animation
        if (aArg.toChangetxt) {
            this.$tit.ani({
                style : this.aniStyleMap.fadeOut,
                trans : {
                    dur : 100
                }
            });
        }

        clearInterval(that.dissolveInterval);
        that.dissolveInterval = null;

        $pmoItem.ani({
            style : this.aniStyleMap.pmoItem[aArg.action + 'Out'],
            trans : {
                dur : 300
            },
            cb : this.changeStepIn({
                changeInfo : aArg
            })
        });
        return false;
    }
    changeStepIn (aParam : {changeInfo: any}) : void {
        var htmlTxt : string, txt1 : string, txt2 : string, txt3 : string, txt4 : string, txt5 : string;
        var changeInfo : any = aParam.changeInfo, contents = this._contents[this._type], that = this;

        this.$indicate[this._step].classList.remove('on');
        this._step += changeInfo.nextFlag;

        this.$indicate[this._step].classList.add('on');

        txt1 = '<span><h1 id="stepTitle">0';
        txt2 = (contents.step[this._step][0].txtnum + 1);
        txt3 = '</h1></span><span><em class="txt">';
        txt4 = this._conv_String(contents.step[this._step][0].txt);
        txt5 = '</em></span>';
        htmlTxt = txt1 + txt2 + txt3 + txt4 + txt5;
        this.$tit.html(htmlTxt);

        var $pmoItem : any = $(this.$pmo.find('.pmoItem').get(this._step));

        var fadeIn : any = this.aniStyleMap.fadeIn;

        if (changeInfo.isLast) {
            this.$close.ani({
                style : fadeIn,
                trans : {
                    dur : 200,
                    delay : 300
                }
            });
        }
        if (!changeInfo.isFirst && changeInfo.toBeShowArrowL) {
            this.$arrowL.ani({
                style : fadeIn,
                trans : {
                    dur : 300,
                    delay : 300
                }
            });
        } else if (!changeInfo.isLast && changeInfo.toBeShowArrowR) {
            this.$arrowR.ani({
                style : fadeIn,
                trans : {
                    dur : 300,
                    delay : 300
                }
            });
        }
        if (changeInfo.toChangetxt) {
            this.$tit.ani({
                style : fadeIn,
                trans : {
                    dur : 200,
                    delay : 300
                }
            });
        }

        $pmoItem.ani({
            style : this.aniStyleMap.pmoItem[changeInfo.action + 'In'],
            trans : {
                dur : 420
            },
            cb : function() {
                if (that.dissolveInterval !== null) {
                    clearInterval(that.dissolveInterval);
                    that.dissolveInterval = null;
                }
                var stepLists : any = that.section.querySelectorAll('ul');
                var scenesLists : any = stepLists[that._step].querySelectorAll('li');
                var i : number;
                for ( i = 0; i < scenesLists.length; i += 1) {
                    if (scenesLists[i].style.opacity === '1') {
                        that._sceneIndex = i;
                        break;
                    }
                }
                that.dissolveInterval = setInterval(that._changeScene, 7100, that);
            }
        });
    }
    _doRight () : void {
        if (this._step < this._contents[this._type].step.length - 1) {
            this._moveStep('next');
        }

    }
    _doLeft () : void {
        if (this._step > 0) {
            this._moveStep('prev');
        }
    }
    destroy () {
        var helpType : string = this._type, dataObj : any = {};

        clearInterval(this.dissolveInterval);
        dataObj[helpType] = true;
        hx.svc.WEB_STORAGE.setHelp(dataObj);
        if (this.callback) {
            this.callback();
        }
        this.$.remove();
        this.$ = null;
        $('#wrap').find('#hep').remove();

        // send to live
        if (!!helpType && helpType === 'ltv') {
            var param : any = {};
            param.alEvent = 'alClicked';
            param.alData = {};
            param.alData.me = this;
            param.alData.msg = 'destroyed';
            this.sendEventToParent(param);
        }
        if (!!helpType && helpType === 'home') {
            this.parent.sendEventToEntity({
                alEvent : 'alMessageToPage',
                target : 'CPgHubProxy?show=toggle'
            });
        }
        if (this._cbFnEnd) {
            this._cbFnEnd();
        }
        this.getParent()._subObj = this.getParent()._subObj.filter((obj) => {
            return obj !== this;
        });
    }
    _on_alKey (aParam : {alKey: number}) {
        var bConsumed : boolean = false, k : any = hx.key, kc : number = aParam.alKey;

        hx.log("info", "Help - HelpView(Comp) - _on_alKey(" + aParam.alKey + ")");

        switch(kc) {
            case k.KEY_LEFT:
                this._doLeft();
                bConsumed = true;
                break;
            case k.KEY_RIGHT:
                this._doRight();
                bConsumed = true;
                break;
            case k.KEY_ESCAPE:
                this._doBack();
                bConsumed = true;
                break;
            case k.KEY_BACK:
                if (this._step === 0) {
                    this._doBack();
                } else {
                    this._doLeft();
                }
                bConsumed = true;
                break;
            case k.KEY_OK:
                var stepLists = this.section.querySelectorAll('ul');
                if (this._step === (stepLists.length - 1)) {
                    this._doBack();
                }
                bConsumed = true;
                break;
            case k.KEY_UP:
            case k.KEY_DOWN:
            case k.KEY_CHANNEL_UP:
            case k.KEY_CHANNEL_DOWN:
            case k.KEY_PLAY:
            case k.KEY_OPTION:
            case k.KEY_SEARCH:
            case k.KEY_MENU:
            case k.KEY_GUIDE:
            case k.KEY_PLAYER:
            case k.KEY_ON_DEMAND:
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
            case k.KEY_RECORD:
            case k.KEY_PAUSE:
            case k.KEY_STOP:
            case k.KEY_FAST_FWD:
            case k.KEY_REWIND:
                bConsumed = true;
                break;
        }
        if (hx.config.netflix) {
            switch(kc) {
                case k.KEY_NETFLIX:
                    bConsumed = true;
                    break;
            }
        }
        return bConsumed;
    }
    _doBack () : boolean {
        this.parent.on();
        this.destroy();
        return true;
    }

    _conv_String (aTextArray : any) : string {
        var i : number, len : number, retString : string = '';

        len = aTextArray.length;
        for ( i = 0; i < len; i += 1) {
            if ( typeof (aTextArray[i]) === 'object') {
                var formatObj : any = aTextArray[i];
                // FIXME : It is just work around... :-).....
                if (formatObj.param.length === 1) {
                    retString += hx.util.format(hx.l(formatObj.stringFormat), formatObj.param[0]);
                } else if (formatObj.param.length === 2) {
                    retString += hx.util.format(hx.l(formatObj.stringFormat), formatObj.param[0], formatObj.param[1]);
                } else if (formatObj.param.length === 3) {
                    retString += hx.util.format(hx.l(formatObj.stringFormat), formatObj.param[0], formatObj.param[1], formatObj.param[2]);
                }
                //retString += hx.util.format(hx.l(formatObj.stringFormat));
            } else if ( typeof (aTextArray[i]) === 'string') {
                if (aTextArray[i].substr(0, 4) !== 'LOC_') {
                    retString += aTextArray[i];
                } else {
                    retString += hx.l(aTextArray[i]);
                }
            }
        }
        return retString;
    }

    getTemplateEl (aParam : {type: any; step: number}) {
        var txt;
        var tpl1 = '<section id="hep" style="opacity: 0;"><article class="step first" id="step" style="opacity: 0;"><div class="tit" style="opacity: 0;"><span><h1 id="stepTitle">0';
        var tpl2 = (aParam.step + 1);
        var tpl3 = '</h1></span><span><em class="txt">';
        //var tpl4 = txt;
        var tpl5 = '</em></span></div><p class="line"> </p></article></section>';
        var div = document.createElement('div');

        txt = this._conv_String(this._contents[aParam.type].txt[0]);

        div.innerHTML = (tpl1 + tpl2 + tpl3 + txt + tpl5);

        return div.firstChild;
    }

    getStepListTemplateEl (aParam : any) : any {
        var tpl : string = '<aside class="wrap_pmo"><div class="pmoList" id="pmoList" style="left: -0px;" >';
        var i : number, len : number;
        // list
        len = aParam.length;
        for ( i = 0; i < len; i += 1) {
            var listObj = aParam[i], j, listLen;
            tpl += '<ul class="pmoItem" style="opacity:' + (i === 0 ? 1 : 0) + '">';
            for ( j = 0, listLen = listObj.length; j < listLen; j += 1) {
                tpl += '<li class="pmoDesp on" style="opacity:' + (j === 0 ? 1 : 0) + '">';
                tpl += '<figure><img src="' + listObj[j].scene + '"></figure>';
                tpl += '<p>' + this._conv_String(listObj[j].caption) + '</p>';
                tpl += '</li>';
            }
            tpl += '</ul>';
        }

        tpl += '</div></aside>';
        var div = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }
    getIndecateTemplateEl(aArgObject : {len: number}) {
        var tpl : string = '<nav class="indicate" style="opacity: 1;">';
        var i : number, len : number;

        len = aArgObject.len;
        for ( i = 0; i < len; i += 1) {
            tpl += '<p class="button"></p>';
        }
        var div = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }

    getOtherTemplateEl (aArgObject : {id: string; cls: string} ) : any {
        var tpl : string = '<p id="' + aArgObject.id + '" class="' + aArgObject.cls + '" style="opacity: 0;"></p>';
        var div : any = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }

}
