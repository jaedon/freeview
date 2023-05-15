/// <reference path="../../def/jquery.d.ts"/>
/// <reference path="../../def/require.d.ts"/>
/// <reference path="common.ts"/>
/// <reference path='ctrlbase.ts'/>

import CtrlBase = require("./ctrlbase");
module CtrlLib {
    console.log("CtrlLib Start");
    export class CFsatMediaListControl extends CtrlBase.CMediaListControl {
        static FN_DRAWER_MEDIA: CtrlBase.FDataDrawer = (() => {
            var _fnCreateDiv = (aClass: string, aInnertText?): HTMLElement => {
                var elDiv = document.createElement('div');
                elDiv.classList.add(aClass);
                if (aInnertText !== undefined) {
                    elDiv.innerText = aInnertText;
                }
                return elDiv;
            };
            var _fnGetContentType = (aContentType: number): string => {
                /* to test */
                var contentType = [
                    '-folder',
                    '-folder-plus',
                    '-video',
                    '-photo',
                    '-music'
                ];
                return contentType[aContentType];
            };
            var _fnGetDuration = function (aDuration) {
                if (aDuration > 0 && aDuration < 60) {
                    return aDuration + ' sec';
                }

                aDuration = Math.floor(aDuration / 60) + ((aDuration % 60) >= 30 ? 1 : 0);
                var hrs = Math.floor(aDuration / 60);
                var mins = aDuration % 60;
                var hrTxt = (hrs === 0 ? "" : hrs + (hrs == 1 ? " hr " : " hrs "));
                var minTxt = (mins === 0 ? "" : mins + (mins == 1 ? " min " : " mins "));
                return hrTxt + minTxt;
            };
            var FN_DRAWER_MEDIA: CtrlBase.FDataDrawer = (aKey: any, aItem: any, aEl: HTMLElement): CtrlBase.TFocusInfo => {
                var elFrame = _fnCreateDiv('-frame');
                var elMediaType = _fnCreateDiv('-media-type');
                var elTitle = _fnCreateDiv('-title', aItem.title);
                var elDuration = _fnCreateDiv('-duration', _fnGetDuration(aItem.duration));
                elMediaType.classList.add(_fnGetContentType(aItem.contentType));
                elFrame.appendChild(elMediaType);
                elFrame.appendChild(elTitle);
                elFrame.appendChild(elDuration);
                aEl.appendChild(elFrame);
                return CtrlBase.TFocusInfo.KFocusAble;
            };
            return FN_DRAWER_MEDIA;
        })();
        static KClassFsatMediaList = '-fsat-media-list';
        static KClassLoading = '-loading';
        static KClassEmpty = '-empty';
        static KClassHide = '-hide';
        static KEventEmpty = 'Empty';

        private _elEmpty: HTMLElement;
        constructor(aElement: HTMLElement, aDrawer: CtrlBase.FDataDrawer, aData: any[] = [], aDataRoll: boolean = false) {
            super(aElement);
            this.registerSignal([CFsatMediaListControl.KEventEmpty]);
            this._element.classList.add(CFsatMediaListControl.KClassFsatMediaList);
            this.setItemWidth(764);
            this.setItemHeight(50);
            this.setOrientation(CtrlBase.TParamOrientation.EVertical);
            this.setOwnedDataProvider(this.doGetDataProvider(aData, aDataRoll));
            this.setScrollScheme(CtrlBase.TParamScrollScheme.EByFixedFocusRemains);
            this.setDataRolling(aDataRoll);
            this.setDataDrawer(aDrawer || CFsatMediaListControl.FN_DRAWER_MEDIA);

            var elEmpty = document.createElement('div');
            elEmpty.classList.add(CFsatMediaListControl.KClassEmpty);
            elEmpty.classList.add(CFsatMediaListControl.KClassLoading);
            this._elEmpty = elEmpty;
            this._elEmpty.innerText = 'Loading...';
            this._container.appendChild(elEmpty);
            this.draw();
        }
        setListData(aData: any[], aDataRoll?: boolean) {
            this.setDataRolling(aDataRoll);
            this.setOwnedDataProvider(this.doGetDataProvider(aData, aDataRoll));
        }
        doGetDataProvider(aData: any[], aDataRolling: boolean): any {
            return new CtrlBase.CMediaListDataProvider(aData, aDataRolling);
        }
        checkEmpty(aByUpdate?: boolean) {
            /* COMMENT
             * replaced this._keyMap to this.getItemLength();
             * * bglee@humaxdigital.com, 오후 6:28, 2013. 10. 30.
             */
            var count = this.getItemLength();
            if (count) {
                this._elEmpty.classList.add(CFsatMediaListControl.KClassHide);
            } else {
                this._elEmpty.classList.remove(CFsatMediaListControl.KClassHide);
                this._elEmpty.classList.remove(CFsatMediaListControl.KClassLoading);
                if (aByUpdate) {
                    this._elEmpty.innerText = 'No items';
                    this._emitEmpty();
                }
            }
        }
        draw(aRect?: CtrlBase.TRect) {
            super.draw(aRect);
            this.checkEmpty();
        }
        update(aFirstCalled: boolean = true) {
            super.update();
            this.checkEmpty(true);
        }
        connectEmpty(aHolder: any, aSlotName: string, aSlot: () => void) {
            this.connect("Empty", aHolder, aSlotName);
        }
        private _emitEmpty() {
            this.emit.call(this, "Empty");
        }
    }
    export class CFsatMediaShareListControl extends CFsatMediaListControl {
        doGetDataProvider(aData: any[], aDataRolling: boolean) {
            return new CtrlBase.CMediaMediaShareDataProvider(aData, aDataRolling);
        }
    }
    export interface THelpString {
        element: HTMLElement;
        keyString: string;
    }
    export class CHelpString {
        static KClassHelpString = '-help-string';
        static KClassDisable = '-disable';

        private _elRoot: HTMLElement;
        private _ifHelp: THelpString[];
        private _listControl: CtrlBase.CAbsCommonDataOperationControl;

        constructor(aElRoot: HTMLElement, aListControl: CtrlBase.CListControl, aArrayKeySet: string[], aCustomStringSet?: string[]) {
            aElRoot.classList.add(CHelpString.KClassHelpString);
            aListControl.connectFocusedDataItemChanged(this, "_slFocusedDataItemChanged", this._slFocusedDataItemChanged);
            aListControl.connectTransitioningChanged(this, "_slTransitioningChanged", this._slTransitioningChanged);
            this._elRoot = aElRoot;
            this._ifHelp = this._makeIfHelpString(aArrayKeySet, aCustomStringSet);
            this._listControl = aListControl;
            this.update();
        }
        destroy() {
            this._listControl.disconnect(this);
        }
        addClass(aClass: string) {
            this._elRoot.classList.add(aClass);
        }
        removeClass(aClass: string) {
            this._elRoot.classList.remove(aClass);
        }
        _slTransitioningChanged(aTransitioning: boolean) {
            if (!aTransitioning) {
                setTimeout(() => {
                    this.update();
                }, 100);
            }
        }
        _slFocusedDataItemChanged(aKey: any, aItem: any, aEl: HTMLElement) {
            this.update();
        }
        _makeIfHelpString(aKeyString: string[], aCustomStringSet: string[] = []): THelpString[] {
            var fnCreateElementWithClass = (aClass: string, aIndex: number, aString: string): HTMLElement => {
                var el = document.createElement('div');
                el.classList.add('-item');
                el.classList.add('-index0' + aIndex);
                el.classList.add(aClass);
                el.innerText = aString;
                return el;
            };
            var i, len;
            var elRoot = this._elRoot;
            var ret: THelpString[] = [];
            var keyString;
            var element;
            for (i = 0, len = aKeyString.length; i < len; i++) {
                keyString = aKeyString[i];
                element = fnCreateElementWithClass(keyString, i, aCustomStringSet[i]);
                ret.push({
                    element: element,
                    keyString: keyString
                });
                elRoot.appendChild(element);
            }
            element = fnCreateElementWithClass('-navi', i, '');
            ret.push({
                element: element,
                keyString: ''
            });
            elRoot.appendChild(element);
            return ret;
        }
        update() {
            var i, len, item, key, items = this._ifHelp;
            for (i = 0, len = items.length; i < len; i++) {
                item = items[i];
                key = item.keyString;
                if (key) {
                    if (this._isPossible(item.keyString)) {
                        item.element.classList.remove(CHelpString.KClassDisable);
                    } else {
                        item.element.classList.add(CHelpString.KClassDisable);
                    }
                }
                else {
                    this._updateNaviInfo(item.element);
                }
            }
        }
        _isPossible(aKeyString: string): boolean {
            return this._listControl.isMovable(aKeyString);
        }
        _updateNaviInfo(aEl: HTMLElement) {
            var list = this._listControl;
            var onePage = list.getCountOnPage();
            var count = list.getItemLength();
            var focusedItem = list.getFocusedElement();
            var scheme = list.getVerticalScrollScheme();
            var text = '';
            if (focusedItem) {
                if (scheme === CtrlBase.TParamScrollScheme.EByFixedFocusRemains) {
                    var pos = focusedItem.attributes['data'];
                    var index = list.getFocusedElementIndex();
                    var countNextView = onePage - 1;
                    var pageLast = Math.floor(count / countNextView);
                    var quotient = Math.floor(pos / countNextView);
                    var reminder = pos % countNextView;
                    if (reminder === 0 && index !== 0) {
                        quotient = quotient - 1;
                    }
                    var start = quotient * countNextView;
                    var end = pageLast === quotient ? count : Math.min(count, start + onePage);
                    text = (start + 1) + '-'  + end;
                } else {
                    throw 'do implement other scheme algorithm';
                }
                text += ' of ' + count;
            }
            aEl.innerHTML = text;
        }
    }
    console.log("CtrlLib End");
}
