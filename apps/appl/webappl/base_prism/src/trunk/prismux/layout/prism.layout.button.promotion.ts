/**
 * Created by bglee@humaxdigital.com on 2014. 8. 8..
 * Button Promotion class
 * ex: green button, hd watch, etc...
 */
/// <reference path="../../def/sugar.d.ts" />
/// <reference path="../../def/layout.d.ts" />

declare var hx;

export class CButtonPromotion implements ifCButtonPromotion {
    private _element: HTMLElement;
    private _isBlocked: boolean = false;
    private _data;

    static CLASS_SHOW = "-show";

    constructor(aElement: HTMLElement) {
        this._element = aElement;
    }
    showButton(aForceShowing?): boolean {
        if (aForceShowing || !this._isBlocked) {
            this._element.classList.add(CButtonPromotion.CLASS_SHOW);
            this._isBlocked = false;
        }
        return this._doShowButton();
    }
    hideButton(aKeepHiding?) {
        this._element.classList.remove(CButtonPromotion.CLASS_SHOW);
        this._isBlocked = aKeepHiding;
        this._doHideButton();
    }
    isShowing(): boolean {
        return this._element.classList.contains(CButtonPromotion.CLASS_SHOW);
    }
    getElement(): HTMLElement {
        return this._element;
    }
    protected _doShowButton(): boolean {
        //TODO "do implement at inherited class";
        return false;
    }
    protected _doHideButton() {
        //TODO "do implement at inherited class";
    }
    setData(aData: any = null) {
        this._data = aData;
    }
    getData(): any {
        return this._data;
    }
}
if (!ENV.isModeRelease) {
    var __fnDebugger = function(aElementID: string) {
        var el = document.createElement("div");
        el.id = aElementID;
        el.classList.add("-debug");
        document.getElementById("DIA").appendChild(el);
        return el;
    };
}
export class CGreenButtonPromotion extends CButtonPromotion {
    static ID_GREEN_BUTTON = "green-button";
    static CLASS_GREEN_BUTTON = "-green-button";
    static CLASS_TEXT = "-text";
    static CLASS_IMAGE_ICON = "-image-icon";
    private _elDefault: HTMLDivElement;
    _elImage: HTMLImageElement;
    private _debugger: HTMLDivElement;

    constructor() {
        super(document.createElement("div"));
        $(document).on("load","img .test" , function(){});

        var elContainer = this.getElement();
        var elText = document.createElement("div");
        var elIcon = document.createElement("div");
        var elImageIcon = document.createElement("img");



        elContainer.setAttribute("id", CGreenButtonPromotion.ID_GREEN_BUTTON);
        elText.setAttribute("class", CGreenButtonPromotion.CLASS_TEXT);
        elIcon.setAttribute("class", CGreenButtonPromotion.CLASS_GREEN_BUTTON);
        elImageIcon.setAttribute("class", CGreenButtonPromotion.CLASS_IMAGE_ICON);
        elText.appendChild(elIcon);
        elContainer.appendChild(elText);
        elContainer.appendChild(elImageIcon);
        document.getElementById("DIA").appendChild(elContainer);

        elImageIcon.onerror = () => {
            this.hideButton();
        };
        elImageIcon.alt = "";
        this._elDefault = elText;
        this._elImage = elImageIcon;
    }
    setDebugModeToggle() {
        if (this._debugger) {
            this._debugger.parentElement.removeChild(this._debugger);
            delete this._debugger;
        } else {
            this._debugger = __fnDebugger("debugger-green-button");
            this.showButton();
        }
    }
    setData(aRctInfoCollection: RctLinkInfoCollection = null) {
        super.setData(aRctInfoCollection);
    }
    getData(): RctLinkInfoCollection {
        return super.getData();
    }
    getRctInfoCount(): number {
        var rctInfoCollection = this.getData();
        var ret = 0;
        if (rctInfoCollection) {
            var item;
            for (var i = 0, len = rctInfoCollection.totalLinkCnt; i < len; i++) {
                item = rctInfoCollection.item(i);
                if (item) {
                    ret++;
                } else {
                    break;
                }
            }
        }
        return ret;
    }
    showButton(aForceShowing?: boolean): boolean {
        var ret = false;
        if (this.getData()) {
            ret = super.showButton(aForceShowing);
        }
        return ret;
    }
    _doShowButton(): boolean {
        var showDefaultIcon = (aPosition?: string[]) => {
            if (aPosition) {
                this._elDefault.style.left = aPosition[0] + "px";
                this._elDefault.style.top = aPosition[1] + "px";
            } else {
                this._elDefault.style.left = "";
                this._elDefault.style.top = "";
            }
            this._elDefault.classList.add(CButtonPromotion.CLASS_SHOW);
            this._elImage.classList.remove(CButtonPromotion.CLASS_SHOW);
        };

        var uri = null;
        var position: string = null;
        var rctInfoCollection = this.getData();
        var rctInfo = rctInfoCollection.item(0);
        var imageInfo = rctInfoCollection.imageIconInfo;
        var showDescriptorIcon = (aFxIfFail) => {
            var positions;
            var scailingWidth = 1;
            var scailingHeight = 1;
            var mwIcon;
            var workaround = false;
            var osdScreenWidth=1280;
            var osdScreenHeight=720;
            var rctIconWidth=167;
            var rctIconHeight=29;
            var positionFlag;
            var mwIconTypes = {
                "/png": "file://tmp/RctImageIcon.png",
                "/jpg": "file://tmp/RctImageIcon.jpg",
                "/jpeg": "file://tmp/RctImageIcon.jpg"
            };
            var timeoutTimer = -1;
            var compensation = [
                [720, 576],
                [1280, 720],
                [1920, 1080]
            ][imageInfo.coordinateSystem];
            uri = rctInfoCollection.imageIconInfo.iconUrl;
            if (!uri) {
                mwIcon = mwIconTypes[imageInfo.iconType];
            } else if (uri.indexOf("dvb:/") !== -1) {
                mwIcon = "file://tmp/RctImageIcon.dvb";
                workaround = true;
            }
            if (mwIcon) {
                uri = mwIcon;
            }
            positionFlag = imageInfo.positionFlag;
            if (positionFlag) {
                position = imageInfo.horizontalOrigin + "x" + imageInfo.verticalOrigin;
            }
            if (compensation instanceof Array) {
                scailingWidth = (osdScreenWidth / compensation[0]);
                scailingHeight = (osdScreenHeight / compensation[1]);
            }
            if (position) {
                positions = position.split("x");
                hx.log("debug", "position info: " + positions);
                this._elImage.style.left = ((positions[0] || 0) * scailingWidth) + "px";
                this._elImage.style.top = ((positions[1] || 0) * scailingHeight) + "px";
            }
            if (uri) {
                uri += "?" + Date.now();
                this._elImage.onload = () => {
                    hx.log("warning", "[onload] width: " + this._elImage.naturalWidth);
                    hx.log("warning", "[onload] height: " + this._elImage.naturalHeight);
                    clearTimeout(timeoutTimer);
                    if (positionFlag)
                    {
                        rctIconWidth = this._elImage.naturalWidth * scailingWidth;
                        rctIconHeight = this._elImage.naturalHeight * scailingHeight;
                    }
                    this._elImage.style.width = rctIconWidth + "px";
                    this._elImage.style.height = rctIconHeight + "px";
                };
                if (workaround) {
                    this._elImage.onerror = (function(){
                        var opportunities = 2;
                        return function() {
                            hx.log("warning", "[onerror]");
                            if (opportunities-- > 0) {
                                setTimeout(() => {
                                    hx.log("warning", "[onerror]: retry opportunities: " + opportunities);
                                    this.src = uri;
                                }, 1 * 500);
                            } else {
                                aFxIfFail(positions);
                            }
                        }
                    })();
                } else {
                    this._elImage.onerror = function() {
                        hx.log("warning", "[onerror]");
                        clearTimeout(timeoutTimer);
                        aFxIfFail(positions);
                    };
                }
                if (!workaround) {
                    timeoutTimer = setTimeout(() => {
                        aFxIfFail(positions);
                    }, 5000);
                }
                this._elImage.src = "";
                this._elImage.src = uri;


                hx.log("info", "Uri: " + uri);
            } else {
                aFxIfFail(positions);
            }
            this._elDefault.classList.remove(CButtonPromotion.CLASS_SHOW);
            this._elImage.classList.add(CButtonPromotion.CLASS_SHOW);
        };
        if (rctInfo.isDefaultIcon) {
            if (rctInfoCollection.imageIconInfo.iconId > 0) { // 1 N
                showDescriptorIcon((aPosition) => {
                    showDefaultIcon(aPosition);
                });
            } else { // 1 0
                showDefaultIcon();
            }
        } else {
            if (rctInfoCollection.imageIconInfo.iconId > 0) { // 0 N
                showDescriptorIcon(() => {
                    this.hideButton()
                });
            } else { // 0 0
                this.hideButton();
            }
        }
        if (!ENV.isModeRelease && this._debugger) {
            var debugInfo = [
                "URI: " + uri,
                "positionFlag: " + imageInfo.positionFlag,
                "position: " + position,
                "isDefaultIcon in RctInfo: " + (rctInfo.isDefaultIcon ? 1 : 0),
                "iconId in RctInfoCollection: " + rctInfoCollection.imageIconInfo.iconId,
                "coordinatingSystem: " + imageInfo.coordinateSystem,
                "width: " + this._elImage.style.width,
                "height: " + this._elImage.style.height
            ];
            this._debugger.innerHTML = debugInfo.join("<br>");
        }
        return this._elImage.classList.contains(CButtonPromotion.CLASS_SHOW) || this._elDefault.classList.contains(CButtonPromotion.CLASS_SHOW);
    }
    _doHideButton() {
        this._elImage.style.left = "";
        this._elImage.style.top = "";
    }
    isShowing(): boolean {
        var ret = this.getElement().classList.contains(CButtonPromotion.CLASS_SHOW);
        return ret;
    }
}
export class CWatchInHDPromotion extends CButtonPromotion {
    constructor(aElement: HTMLElement) {
        super(aElement);
    }
}