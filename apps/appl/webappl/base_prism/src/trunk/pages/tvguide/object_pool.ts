/**
 * Created by bglee on 2015-02-17.
 */
///<reference path="../../def/provider.d.ts" />
///<reference path="../../def/dateformat.d.ts" />
export class CObjectPool<T> {
    private _pool: T[] = [];
    private _factory;
    private _countAlloc = 0;
    private _countFree = 0;
    constructor(aConstructor) {
        this._factory = aConstructor;
    }
    alloc(): T {
        var ret: T = null;
        if (this._pool.length === 0) {
            ret = new this._factory();
            console.debug("[pool] allocated from new");
        } else {
            ret = this._pool.shift();
            console.debug("[pool] allocated from pool");
        }
        this._countAlloc++;
        console.debug("[pool] alloc count: " + this._countAlloc);
        return ret;
    }
    free(aItem: T) {
        this._pool.push(aItem);
        this._countFree++;
        console.debug("[pool] free count: " + this._countFree + " pool.length: " + this._pool.length);
        return this;
    }
    destroy() {
    }
}
export interface ifEPGCell {
    title: string;
    thumb: string;
    thumbDefault: string;
    data0: any;
    data1: any;
    progress: number;
    icon?: ifEPGCellIcon;
}
export interface ifEPGCellMethod {
    setTitle: (text: string) => void;
    setThumbnail: (url: string, defaultUrl: string) => void;
    setTextOne: (text: string) => void;
    setTextTwo: (text: string) => void;
    setProgress: (persent: number) => void;
    setIcon: (icon: ifEPGCellIcon) => void;
}
export interface ifEPGCellEngine {
    (aMethods: any, aMaterials: ifEPGCell): void;
}
export class CEPGCell {
    private _elLi: HTMLElement;
    private _elFigure: HTMLElement;
    private _elStrong: HTMLElement;
    private _elImg: HTMLImageElement;
    private _elEm: HTMLElement;
    private _elSpan: HTMLSpanElement;
    private _elTextOne: Node;
    private _elTextTwo: Node;
    private _doUpdate = (aProperties: any, aMeterials: ifEPGCell) => {};

    constructor(engine: ifEPGCellEngine, element: HTMLElement, title: string, thumb: string, text0: Date, text2: Date) {
        var doc = document;
        var elLi = doc.createElement('li');
        var elFigure = doc.createElement('figure');
        var elStrong = doc.createElement('strong');
        var elImg = doc.createElement('img');
        var elEm = doc.createElement('me');
        var elTextOne = doc.createTextNode('');
        var elSpan = doc.createElement('span');
        var elTextTwo = doc.createTextNode('');

        this._elTextOne = elTextOne;
        this._elTextTwo = elTextTwo;
        this._elSpan = elSpan;
        this._elEm = elEm;
        this._elImg = elImg;
        this._elStrong = elStrong;
        this._elFigure = elFigure;
        this._elLi = elLi;

        //elLi.classList.add('pInfo');
        //elLi.classList.add('focusable');
        elFigure.classList.add('thum');
        elFigure.classList.add('icoRec');
        elStrong.classList.add('title');
        elEm.classList.add('time');

        if (engine) {
            this.setUpdateEngine(engine);
        }
        this.update(Array.prototype.slice.call(arguments, 1));

        elLi.appendChild(elFigure);
            elFigure.appendChild(elImg);
        elLi.appendChild(elStrong);
        elLi.appendChild(elEm);
            elEm.appendChild(elTextOne);
            elEm.appendChild(elSpan);
            elEm.appendChild(elTextTwo);
    }
    setUpdateEngine(aFn: ifEPGCellEngine) {
        if (aFn) {
            this._doUpdate = aFn;
        }
    }
    private _getMethods(): ifEPGCellMethod {
        return {
            setTitle: (text: string) => {
                this._elStrong.textContent = text;
            },
            setThumbnail: (url: string, urlDefault: string) => {
                var elImg = this._elImg;
                if (urlDefault) {
                    elImg.style.display = 'none';
                    elImg.onerror = function() {
                        this.src = urlDefault;
                    };
                    elImg.onload = function() {
                        this.style.display = 'block';
                    };
                }
                elImg.src = url || '';
                if (!url) {
                    this._elEm.classList.add('icoRec');
                }
            },
            setTextOne: (text: string) => {
                this._elTextOne.textContent = text;
            },
            setTextTwo: (text: string) => {
                this._elTextTwo.textContent = text;
            },
            setProgress: (persent: number) => {
                this._elSpan.style.width = persent + '%';
            },
            setIcon: (icons: ifEPGCellIcon) => {
                var classNames = {
                    play: 'play',
                    recording: 'rec',
                    reservation_single: 'rec2',
                    reservation_series: 'seri',
                    reminder: 'remi',
                    live_streaming: 'lis'
                };
                for (var key in icons) {
                    if (icons[key]) {
                        this._elSpan.classList.add(classNames[key]);
                    }
                }
            }
        };
    }
    getProperties(): ifEPGCell {
        return {
            title: this._elStrong.textContent,
            thumb: this._elImg.src,
            thumbDefault: null,
            data0: this._elTextOne.textContent,
            data1: this._elTextTwo.textContent,
            progress: parseInt(this._elSpan.style.width)
        }
    }
    update(aMaterials: ifEPGCell): void {
        this._doUpdate(this._getMethods(), aMaterials);
    }
    setClassNameToLi(className: string): void {
        this._elLi.className = className;
    }
    setClassNameToFigure(className: string): void {
        this._elFigure.className = className;
    }
    addClassToLi(className: string): void {
        this._elLi.classList.add(className);
    }
    //addClassToFigure(className: string): void {
    //    this._elFigure.classList.add(className);
    //}
    removeClassToLi(className: string): void {
        this._elLi.classList.remove(className);
    }
    removeClassToFigure(className: string): void {
        this._elFigure.classList.remove(className);
    }
    //getFigure(): HTMLElement {
    //    return this._elFigure;
    //}
    getLi(): HTMLElement {
        return this._elLi;
    }
}
export interface ifEPGCellIcon {
    play?: boolean;
    recording?: boolean;
    reservation_single?: boolean;
    reservation_series?: boolean;
    reminder?: boolean;
    live_streaming?: boolean;
}
export class CCellHTML {
    private _elRoot = document.createDocumentFragment();
    private _elSpanIconReccording = document.createElement('span');
    private _elFigure = document.createElement('figure');
    private _elDivCAS = document.createElement('div');
    private _elStrongTitle = document.createElement('strong');
    private _elEm = document.createElement('em');
    private _elImgThumbnail = document.createElement('img');
    private _nodeText01 = document.createTextNode('');
    private _elSpanProgressBarFrame = document.createElement('span');
    private _elSpanProgressBar = document.createElement('span');
    private _nodeText02 = document.createTextNode('');
    private _nodeChildren: Node[] = [];
    private _image: HTMLImageElement = new Image();

    constructor() {
        this._elStrongTitle.classList.add('title');
        this._elEm.classList.add('time');
        this._elSpanProgressBarFrame.classList.add('progress');
        this._image.onload = function() {
            this.style.display = 'block';
        };
        this._elRoot.addEventListener('DOMNodeRemoved', function() {
        }, false);
        this._elRoot.addEventListener('DOMNodeRemovedFromDocument', function() {
        }, false);
    }
    private _getClassScheduleIcon(programme: ifProgrammeProvider) {
        return this._doGetClassScheduleIcon(programme);
    }
    private _getClassThumbnail(programme: ifProgrammeProvider) {
        return this._doGetClassThumbnail(programme);
    }

    protected _doWipe() {
        this._elStrongTitle.textContent = '';
        this._nodeText01.textContent = '';
        this._nodeText02.textContent = '';
    }
    protected _doGetClassScheduleIcon(programme: ifProgrammeProvider) {
        var ret: string;
        if ((<any>programme).isOnAir) {
            if (this.reqToCheckRecInProgress(programme)) {
                ret = 'rec';
            }
        } else {
            if (programme.hasSchedule()) {
                ret = 'rec2';
            } else if (programme.hasReminder()) {
                ret = 'remi';
            } else if (programme.hasSeriesSchedule()) {
                ret = 'seri';
            }
        }
        return ret || '';
    }
    protected _doGetClassThumbnail(programme) {
        return 'thum icoRec';
    }
    protected _doUpdate(programme: ifProgrammeProvider, hasThumbnail: boolean) {

    }
    update(programme: ifProgrammeProvider, aHasThumbnail: boolean): CCellHTML {
        var name = programme.getName();
        var startTime = programme.getStartTime() * 1000;
        var endTime = programme.getEndTime() * 1000;
        var children: Node[] = this._nodeChildren;
        var nodesFigure = [];
        var nodesEm = [];
        var hasPluginObject = !!programme.getPluginObject();

        children.length = 0;
        if (aHasThumbnail) {
            this._elFigure.className = this._getClassThumbnail(programme);
            nodesFigure.push(this._elFigure);
            if (!(<any>programme).backwardView) {
                if (hasPluginObject) {
                    this._elSpanIconReccording.className = this._getClassScheduleIcon(programme);
                    nodesFigure.push(this._elSpanIconReccording);
                }
            }

            if ((<any>programme).isCAS) {
                this._elDivCAS.className = (<any>programme).isCAS;
                nodesFigure.push(this._elDivCAS);
            }

            this._image.style.display = 'none';
            if (hasPluginObject) {
                if (programme.getThumbnailUrlType() === 'https') {
                    this._image.onerror = function () {
                        this.src = programme.getDefaultThumbnail();
                        this._elImgThumbnail.src = this.src;
                    };
                }
                this._image.style.display = 'none';
                this._image.src = programme.getThumbnailURL();
            } else {
                this._image.onerror = null;
                this._image.src = programme.getDefaultThumbnail();
            }
            this._elImgThumbnail.src = this._image.src;
            nodesFigure.push(this._elImgThumbnail);
            children.push(<Node><any>nodesFigure);
        } else {
            this._elEm.classList.add('icoRec');
        }
        children.push(<Node><any>this._elStrongTitle);
        children.push(<Node><any>nodesEm);
        this._elStrongTitle.textContent = name;

        //var objFromDoUpdated = _doUpdate(programme, aHasThumbnail);
        nodesEm.push(this._elEm);
        if (hasPluginObject) {
            if (startTime && endTime) {
                nodesEm.push(this._nodeText01);
                this._nodeText01.textContent = dateFormat(startTime, 'HH:MM');
                if ((<any>programme).isOnAir) {
                    this._nodeText02.textContent = dateFormat(endTime, 'HH:MM');
                    this._elSpanProgressBar.style.width = ((Date.now() - programme.startTime) / programme.duration * 100) + 'px';
                    nodesEm.push([this._elSpanProgressBarFrame, this._elSpanProgressBar]);
                    nodesEm.push(this._nodeText02);
                } else {
                    this._nodeText01.textContent += ' - ' + dateFormat(endTime, 'HH:MM');
                }
                if (aHasThumbnail) {
                    var dispDate = '';
                    if ((<any>programme).isOnAir) {
                        if ((<any>programme).nextView || (<any>programme).backwardView) {
                            dispDate = ' / ' + dateFormat(startTime, 'ddd.dd.mm')
                        }
                    }
                    this._nodeText01.textContent += dispDate;
                }
            }
        } else {
            if (!isNaN(startTime)) {
                nodesEm.push(this._nodeText01);
                this._nodeText01.textContent = dateFormat(startTime, 'ddd.dd.mm');
            }
        }
        this._nodeChildren = children;
        return this;
    }
    disassemble(aChildren: any[] = this._nodeChildren): CCellHTML {
        for (var i = 0, child; i < aChildren.length; i++) {
            child = aChildren[i];
            if (child instanceof Array) {
                arguments.callee(child);
                continue;
            }
            if (child instanceof HTMLElement && child.parentElement) {
                child.parentElement.removeChild(child);
            }
        }
        return this;
    }
    assemble(aElRoot: Node = this._elRoot, aChildren: Node[] = this._nodeChildren): Node {
        for (var i = 0, subtree: Node[]; i < aChildren.length; i++) {
            if (aChildren[i] instanceof Array) {
                subtree = <HTMLElement[]><any>aChildren[i];
                aElRoot.appendChild(arguments.callee(subtree[0], subtree.slice(1)));
                continue;
            }
            aElRoot.appendChild(aChildren[i]);
        }
        return aElRoot;
    }
    wipe(disassembl?: boolean): CCellHTML {
        if (disassembl) {
            this.disassemble();
        }
        this._doWipe();
        this._nodeChildren = [];
        return this;
    }
    reqToCheckRecInProgress(aSchedule): boolean {
        //TODO:
        //if (this.recInProgress(aSchedule))
        return false;
    }
}
