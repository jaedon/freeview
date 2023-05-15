///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cOta.js
/*jslint vars:true */
/*jslint nomen:true */

declare var hx: ifSugar;
import __COMPONENT_KERNEL__ = require("pages/component_kernel");

export class OtaNoti extends __COMPONENT_KERNEL__ {
    aniAction = hx.il.ani.action2;
    isShow: boolean = false;
    _step: number = 0;
    _sceneIndex: number = 0;
    _type: string = 'otanoti';
    dissolveInterval = null;
    $step: any;
    $wrap_title: any;
    $pmoItem: any;
    callback: any;
    parent: any;
    $pmo: any;
    $pmoDesp: any;
    $arrowL: any;
    $arrowR: any;
    section: any;
    _leftArr: any;
    _rightArr: any;
    _stepList: any;
    _nativeFocus: boolean;
    aniStyleMap: any = {
        fadeIn: {
            start: {
                'opacity': 0
            },
            end: {
                'opacity': 1
            }
        },
        fadeOut: {
            start: {
                'opacity': 1
            },
            end: {
                'opacity': 0
            }
        },
        step: {
            fadeIn: {
                start: {
                    'opacity': 0,
                    '-o-transform': 'scale(0.9,0.9)'
                },
                end: {
                    'opacity': 1,
                    '-o-transform': 'scale(1.0,1.0)'
                }
            },
            fadeOut: {
                start: {
                    'opacity': 1,
                    '-o-transform': 'scale(0.9,0.9)'
                },
                end: {
                    'opacity': 0,
                    '-o-transform': 'scale(1.0,1.0)'
                }
            }
        },
        pmoItem: {
            nextOut: {
                start: {
                    'opacity': 1,
                    'left': 0
                },
                end: {
                    'opacity': 0,
                    'left': '-30px'
                }
            },
            nextIn: {
                start: {
                    'opacity': 0,
                    'left': '70px'
                },
                end: {
                    'opacity': 1,
                    'left': 0
                }
            },
            prevOut: {
                start: {
                    'opacity': 1,
                    'left': 0
                },
                end: {
                    'opacity': 0,
                    'left': '30px'
                }
            },
            prevIn: {
                start: {
                    'opacity': 0,
                    'left': '-70px'
                },
                end: {
                    'opacity': 1,
                    'left': 0
                }
            }
        }
    };

    constructor() {
        super();
    }

    _changeScene(that: any): void {
        var stepLists: any = that.section.querySelectorAll('ul');
        var scenesLists: any = stepLists[that._step].querySelectorAll('li');
        if (scenesLists.length <= 1) {
            clearInterval(that.dissolveInterval);
            that.dissolveInterval = null;
            return;
        }
        var $currentDesp: any = $(scenesLists[that._sceneIndex]);
        that._sceneIndex += 1;
        if (that._sceneIndex === scenesLists.length) {
            that._sceneIndex = 0;
        }
        var $newDesp: any = $(scenesLists[that._sceneIndex]);
        $newDesp.show();
        $currentDesp.ani({
            style: that.aniStyleMap.fadeOut,
            trans: {
                dur: 350,
                delay: 100
            }
        });
        $newDesp.ani({
            style: that.aniStyleMap.fadeIn,
            trans: {
                dur: 320
            }
        });
    }

    _getItemData(): any {
        var updatedList: any, datas: any = [];
        updatedList = hx.svc.SETTING_UTIL.getUpdatedList();
        datas.push(updatedList.updatedList);
        return datas;
    }

    execAni() {
        var that = this;
        var $Noti: any = this.$, $step: any = this.$step, $wrap_title: any = this.$wrap_title, $pmoItem: any = this.$pmoItem; //
        var $arrowR: any = this.$arrowR, //
            aniInfo: string = this.isShow ? 'fadeOut' : 'fadeIn', fadeIn: any = this.aniStyleMap.fadeIn;
        $Noti.ani({
            style: fadeIn,
            trans: {
                dur: 300
            }
        });
        $step.ani({
            style: this.aniStyleMap.step.fadeIn,
            trans: {
                dur: 320,
                delay: 300
            }
        });
        $wrap_title.ani({
            style: fadeIn,
            trans: {
                dur: 420,
                delay: 300
            }
        });
        $arrowR.ani({
            style: fadeIn,
            trans: {
                dur: 420,
                delay: 300
            }
        });
        $pmoItem.ani({
            style: fadeIn,
            trans: {
                dur: 420,
                delay: 300
            },
            cb: function () {
                if (that.dissolveInterval !== null) {
                    clearInterval(that.dissolveInterval);
                    that.dissolveInterval = null;
                }
                that.dissolveInterval = setInterval(that._changeScene, 7000, that);
            }
        });
        this.isShow = !this.isShow;
    }

    init() {
    }

    create(aParam) {
        this._nativeFocus = false;
        this.dissolveInterval = null;
        $.extend(this, aParam);
        this.section = this.getTemplateEl({
            'step': this._step,
            'type': this._type
        });
        this.$ = $(this.section);
        this._leftArr = this.getOtherTemplateEl({
            'id': 'arL',
            'cls': 'arrow arL'
        });
        this._rightArr = this.getOtherTemplateEl({
            'id': 'arR',
            'cls': 'arrow arR'
        });
        this._stepList = this.getStepListTemplateEl();
        var article: any = this.section.querySelector('#step');
        article.appendChild(this._leftArr);
        article.appendChild(this._rightArr);
        article.appendChild(this._stepList);
        this.$wrap_title = this.$.find('.wrap_title');
        this.$step = this.$.find('.step');
        this.$pmo = this.$.find('.wrap_pmo');
        this.$pmoDesp = this.$.find('.pmoDesp');
        this.$pmoItem = $(this._stepList);
        this.$arrowR = $(this._rightArr);
        this.$arrowL = $(this._leftArr);
        this.execAni();
    }

    _moveStep(action) {
        var datas: any = [];
        var uidataObj: any;
        datas = this._getItemData();
        uidataObj = JSON.parse(datas);
        var uidataListItems: any = uidataObj.uidata.listitem;
        var currentStep: number = this._step;
        var isPrev: boolean = (action === 'prev');
        var nextFlag: number = (action === 'prev') ? -1 : 1;
        var nextStep: number = this._step + nextFlag;
        var isFirst: boolean = (isPrev && currentStep === 1);
        var isLast: boolean = (!isPrev && currentStep === uidataListItems.length - 2);
        var toBeShowArrowL: boolean = (!isPrev && currentStep === 0);
        var toBeShowArrowR: boolean = (isPrev && currentStep === uidataListItems.length - 1);
        var toChangetxt: boolean = true;
        var changeInfo: any = {
            action: action,
            isPrev: isPrev,
            nextFlag: nextFlag,
            isFirst: isFirst,
            isLast: isLast,
            toBeShowArrowL: toBeShowArrowL,
            toBeShowArrowR: toBeShowArrowR,
            toChangetxt: toChangetxt
        };
        this.changeStepOut(changeInfo);
    }

    changeStepOut(aArg: any): boolean {
        var stepLists: any = this.section.querySelectorAll('ul');
        var fadeOut: any = this.aniStyleMap.fadeOut;
        var $pmoItem: any = $(this.$pmo.find('.pmoItem').get(this._step));
        if (aArg.isLast) {
            this.$arrowR.ani({
                style: fadeOut,
                trans: {
                    dur: 300
                }
            });
        }
        if (aArg.isFirst) {
            this.$arrowL.ani({
                style: fadeOut,
                trans: {
                    dur: 300
                }
            });
        }
        if (aArg.toChangetxt) {
            this.$wrap_title.ani({
                style: this.aniStyleMap.fadeOut,
                trans: {
                    dur: 100
                }
            });
        }
        clearInterval(this.dissolveInterval);
        this.dissolveInterval = null;
        $pmoItem.ani({
            style: this.aniStyleMap.pmoItem[aArg.action + 'Out'],
            trans: {
                dur: 300
            },
            cb: this.changeStepIn({
                changeInfo: aArg
            })
        });
        return false;
    }

    changeStepIn(aArg: any): void {
        var htmlTxt: string, txt1: string, txt2: string, txt3: string, txt4: string, txt5: string;
        var changeInfo: any = aArg.changeInfo, that = this;
        this._step += changeInfo.nextFlag;
        txt3 = '<p class="tit_txt01">';
        txt4 = hx.l('LOC_UPDATED_LIST_CAP_ID');
        txt5 = '</em></span>';
        htmlTxt = txt3 + txt4 + txt5;
        this.$wrap_title.html(htmlTxt);
        var $pmoItem: any = $(this.$pmo.find('.pmoItem').get(this._step));
        var fadeIn: any = this.aniStyleMap.fadeIn;
        if (!changeInfo.isFirst && changeInfo.toBeShowArrowL) {
            this.$arrowL.ani({
                style: fadeIn,
                trans: {
                    dur: 300,
                    delay: 300
                }
            });
        } else if (!changeInfo.isLast && changeInfo.toBeShowArrowR) {
            this.$arrowR.ani({
                style: fadeIn,
                trans: {
                    dur: 300,
                    delay: 300
                }
            });
        }
        if (changeInfo.toChangetxt) {
            this.$wrap_title.ani({
                style: fadeIn,
                trans: {
                    dur: 200,
                    delay: 300
                }
            });
        }
        $pmoItem.ani({
            style: this.aniStyleMap.pmoItem[changeInfo.action + 'In'],
            trans: {
                dur: 420
            },
            cb: function () {
                if (that.dissolveInterval !== null) {
                    clearInterval(that.dissolveInterval);
                    that.dissolveInterval = null;
                }
                var stepLists = that.section.querySelectorAll('ul');
                var scenesLists = stepLists[that._step].querySelectorAll('li');
                var i;
                for (i = 0; i < scenesLists.length; i += 1) {
                    if (scenesLists[i].style.opacity === '1') {
                        that._sceneIndex = i;
                        break;
                    }
                }
                that.dissolveInterval = setInterval(that._changeScene, 7100, that);
            }
        });
    }

    _doRight(): void {
        var datas: any = [];
        var uidataObj: any;
        datas = this._getItemData();
        uidataObj = JSON.parse(datas);
        var uidataListItems = uidataObj.uidata.listitem;
        if (this._step < uidataListItems.length - 1) {
            this._moveStep('next');
        }
    }

    _doLeft(): void {
        if (this._step > 0) {
            this._moveStep('prev');
        }
    }

    destroy(): void {
        var Type: any = this._type, dataObj: any = {};
        clearInterval(this.dissolveInterval);
        dataObj[Type] = "true";
        hx.svc.WEB_STORAGE.setOtaNoti(dataObj);
        if (this.callback) {
            this.callback();
        }
        this.$.remove();
        this.$ = null;
        if (!!Type && Type === 'otanoti') {
            var param: any = {};
            param.alEvent = 'alClicked';
            param.alData = {};
            param.alData.me = this;
            param.alData.msg = 'destroyed';
            this.sendEventToParent(param);
        }
    }

    _on_alKey(aParam: {alKey: number}): boolean {
        var bConsumed: boolean = false, k: any = hx.key, kc: number = aParam.alKey;
        switch (kc) {
            case k.KEY_LEFT:
                this._doLeft();
                bConsumed = true;
                break;
            case k.KEY_RIGHT:
                this._doRight();
                bConsumed = true;
                break;
            case k.KEY_OK:
            case k.KEY_ESCAPE:
                this._doBack();
                bConsumed = true;
                break;
            case k.KEY_BACK:
                if (this._step === 0) {
                    this._doBack();
                    bConsumed = true;
                } else {
                    this._moveStep('prev');
                }
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

    _doBack(): boolean {
        this.parent.on();
        this.destroy();
        return true;
    }

    getTemplateEl(aArgObject: any): Node {
        var txt: string;
        var tpl1: string = '<section id="Noti" style="opacity: 0;"><article class="step first" id="step"><hgroup class="wrap_title" style="opacity: 0;">';
        var tpl2: string = '<p class="tit_txt01">';
        var tpl3: string = '</p></hgroup></article></section>';
        var div: HTMLElement = document.createElement('div');
        txt = hx.l('LOC_UPDATED_LIST_CAP_ID');
        div.innerHTML = (tpl1 + tpl2 + txt + tpl3);
        return div.firstChild;
    }

    getStepListTemplateEl(): Node {
        var datas: any = [];
        var uidataObj: any;
        datas = this._getItemData();
        uidataObj = JSON.parse(datas);
        var stringdata = uidataObj.uidata.listitem[0][1].string;
        var uidataListItems = uidataObj.uidata.listitem;
        var i: number, j: number, len: number, listLen: number;

        var tpl: string = '<aside class="wrap_pmo"><div class="item">';
        if (ENV.listModel.IR4000HD === ENV.model) {
            var newimg: string = 'product/ir4000hd/image/Ota_Noti01.png';
            var defaultimg: string = 'product/ir4000hd/image/Ota_Noti01.png';
        } else {
            var newimg: string = 'images/Noti01.png';
            var defaultimg: string = 'images/Noti02.png';
        }

        len = uidataListItems.length;
        for (i = 0; i < len; i += 1) {
            tpl += '<ul class="pmoItem" style="opacity:' + (i === 0 ? 1 : 0) + '">';
            for (j = 1, listLen = uidataListItems[i].length; j < listLen; j += 1) {
                tpl += '<li class="pmoDesp on" style="opacity:' + (j === 1 ? 1 : 0) + '">';
                if (uidataListItems[i][0].type[0] === "defaultimage") {
                    tpl += '<div class="promImg"><img src="' + defaultimg + '"></div>';
                } else if (uidataListItems[i][0].type[0] === "newimage") {
                    tpl += '<div class="promImg"><img src="' + newimg + '"></div>';
                }
                tpl += '<div class="info01 info_txt02">';
                tpl += '<span class="info_txt01">' + uidataListItems[i][1].string[0] + '</span>';
                tpl += '</div>';
                tpl += '<div class="button">' + 'CLOSE' + '</div>';
                tpl += '</li>';
            }
            tpl += '</ul>';
        }
        tpl += '</div></aside>';
        var div: HTMLElement = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }

    getOtherTemplateEl(aArgObject): Node {
        var tpl = '<div id="' + aArgObject.id + '" class="' + aArgObject.cls + '" style="opacity: 0;"></div>';
        var div: HTMLElement = document.createElement('div');
        div.innerHTML = tpl;
        return div.firstChild;
    }
}
