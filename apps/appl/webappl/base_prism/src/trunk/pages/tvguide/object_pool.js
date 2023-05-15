define(["require", "exports"], function (require, exports) {
    /**
     * Created by bglee on 2015-02-17.
     */
    ///<reference path="../../def/provider.d.ts" />
    ///<reference path="../../def/dateformat.d.ts" />
    var CObjectPool = (function () {
        function CObjectPool(aConstructor) {
            this._pool = [];
            this._countAlloc = 0;
            this._countFree = 0;
            this._factory = aConstructor;
        }
        CObjectPool.prototype.alloc = function () {
            var ret = null;
            if (this._pool.length === 0) {
                ret = new this._factory();
                console.debug("[pool] allocated from new");
            }
            else {
                ret = this._pool.shift();
                console.debug("[pool] allocated from pool");
            }
            this._countAlloc++;
            console.debug("[pool] alloc count: " + this._countAlloc);
            return ret;
        };
        CObjectPool.prototype.free = function (aItem) {
            this._pool.push(aItem);
            this._countFree++;
            console.debug("[pool] free count: " + this._countFree + " pool.length: " + this._pool.length);
            return this;
        };
        CObjectPool.prototype.destroy = function () {
        };
        return CObjectPool;
    })();
    exports.CObjectPool = CObjectPool;
    var CEPGCell = (function () {
        function CEPGCell(engine, element, title, thumb, text0, text2) {
            this._doUpdate = function (aProperties, aMeterials) {
            };
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
        CEPGCell.prototype.setUpdateEngine = function (aFn) {
            if (aFn) {
                this._doUpdate = aFn;
            }
        };
        CEPGCell.prototype._getMethods = function () {
            var _this = this;
            return {
                setTitle: function (text) {
                    _this._elStrong.textContent = text;
                },
                setThumbnail: function (url, urlDefault) {
                    var elImg = _this._elImg;
                    if (urlDefault) {
                        elImg.style.display = 'none';
                        elImg.onerror = function () {
                            this.src = urlDefault;
                        };
                        elImg.onload = function () {
                            this.style.display = 'block';
                        };
                    }
                    elImg.src = url || '';
                    if (!url) {
                        _this._elEm.classList.add('icoRec');
                    }
                },
                setTextOne: function (text) {
                    _this._elTextOne.textContent = text;
                },
                setTextTwo: function (text) {
                    _this._elTextTwo.textContent = text;
                },
                setProgress: function (persent) {
                    _this._elSpan.style.width = persent + '%';
                },
                setIcon: function (icons) {
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
                            _this._elSpan.classList.add(classNames[key]);
                        }
                    }
                }
            };
        };
        CEPGCell.prototype.getProperties = function () {
            return {
                title: this._elStrong.textContent,
                thumb: this._elImg.src,
                thumbDefault: null,
                data0: this._elTextOne.textContent,
                data1: this._elTextTwo.textContent,
                progress: parseInt(this._elSpan.style.width)
            };
        };
        CEPGCell.prototype.update = function (aMaterials) {
            this._doUpdate(this._getMethods(), aMaterials);
        };
        CEPGCell.prototype.setClassNameToLi = function (className) {
            this._elLi.className = className;
        };
        CEPGCell.prototype.setClassNameToFigure = function (className) {
            this._elFigure.className = className;
        };
        CEPGCell.prototype.addClassToLi = function (className) {
            this._elLi.classList.add(className);
        };
        //addClassToFigure(className: string): void {
        //    this._elFigure.classList.add(className);
        //}
        CEPGCell.prototype.removeClassToLi = function (className) {
            this._elLi.classList.remove(className);
        };
        CEPGCell.prototype.removeClassToFigure = function (className) {
            this._elFigure.classList.remove(className);
        };
        //getFigure(): HTMLElement {
        //    return this._elFigure;
        //}
        CEPGCell.prototype.getLi = function () {
            return this._elLi;
        };
        return CEPGCell;
    })();
    exports.CEPGCell = CEPGCell;
    var CCellHTML = (function () {
        function CCellHTML() {
            this._elRoot = document.createDocumentFragment();
            this._elSpanIconReccording = document.createElement('span');
            this._elFigure = document.createElement('figure');
            this._elDivCAS = document.createElement('div');
            this._elStrongTitle = document.createElement('strong');
            this._elEm = document.createElement('em');
            this._elImgThumbnail = document.createElement('img');
            this._nodeText01 = document.createTextNode('');
            this._elSpanProgressBarFrame = document.createElement('span');
            this._elSpanProgressBar = document.createElement('span');
            this._nodeText02 = document.createTextNode('');
            this._nodeChildren = [];
            this._image = new Image();
            this._elStrongTitle.classList.add('title');
            this._elEm.classList.add('time');
            this._elSpanProgressBarFrame.classList.add('progress');
            this._image.onload = function () {
                this.style.display = 'block';
            };
            this._elRoot.addEventListener('DOMNodeRemoved', function () {
            }, false);
            this._elRoot.addEventListener('DOMNodeRemovedFromDocument', function () {
            }, false);
        }
        CCellHTML.prototype._getClassScheduleIcon = function (programme) {
            return this._doGetClassScheduleIcon(programme);
        };
        CCellHTML.prototype._getClassThumbnail = function (programme) {
            return this._doGetClassThumbnail(programme);
        };
        CCellHTML.prototype._doWipe = function () {
            this._elStrongTitle.textContent = '';
            this._nodeText01.textContent = '';
            this._nodeText02.textContent = '';
        };
        CCellHTML.prototype._doGetClassScheduleIcon = function (programme) {
            var ret;
            if (programme.isOnAir) {
                if (this.reqToCheckRecInProgress(programme)) {
                    ret = 'rec';
                }
            }
            else {
                if (programme.hasSchedule()) {
                    ret = 'rec2';
                }
                else if (programme.hasReminder()) {
                    ret = 'remi';
                }
                else if (programme.hasSeriesSchedule()) {
                    ret = 'seri';
                }
            }
            return ret || '';
        };
        CCellHTML.prototype._doGetClassThumbnail = function (programme) {
            return 'thum icoRec';
        };
        CCellHTML.prototype._doUpdate = function (programme, hasThumbnail) {
        };
        CCellHTML.prototype.update = function (programme, aHasThumbnail) {
            var name = programme.getName();
            var startTime = programme.getStartTime() * 1000;
            var endTime = programme.getEndTime() * 1000;
            var children = this._nodeChildren;
            var nodesFigure = [];
            var nodesEm = [];
            var hasPluginObject = !!programme.getPluginObject();
            children.length = 0;
            if (aHasThumbnail) {
                this._elFigure.className = this._getClassThumbnail(programme);
                nodesFigure.push(this._elFigure);
                if (!programme.backwardView) {
                    if (hasPluginObject) {
                        this._elSpanIconReccording.className = this._getClassScheduleIcon(programme);
                        nodesFigure.push(this._elSpanIconReccording);
                    }
                }
                if (programme.isCAS) {
                    this._elDivCAS.className = programme.isCAS;
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
                }
                else {
                    this._image.onerror = null;
                    this._image.src = programme.getDefaultThumbnail();
                }
                this._elImgThumbnail.src = this._image.src;
                nodesFigure.push(this._elImgThumbnail);
                children.push(nodesFigure);
            }
            else {
                this._elEm.classList.add('icoRec');
            }
            children.push(this._elStrongTitle);
            children.push(nodesEm);
            this._elStrongTitle.textContent = name;
            //var objFromDoUpdated = _doUpdate(programme, aHasThumbnail);
            nodesEm.push(this._elEm);
            if (hasPluginObject) {
                if (startTime && endTime) {
                    nodesEm.push(this._nodeText01);
                    this._nodeText01.textContent = dateFormat(startTime, 'HH:MM');
                    if (programme.isOnAir) {
                        this._nodeText02.textContent = dateFormat(endTime, 'HH:MM');
                        this._elSpanProgressBar.style.width = ((Date.now() - programme.startTime) / programme.duration * 100) + 'px';
                        nodesEm.push([this._elSpanProgressBarFrame, this._elSpanProgressBar]);
                        nodesEm.push(this._nodeText02);
                    }
                    else {
                        this._nodeText01.textContent += ' - ' + dateFormat(endTime, 'HH:MM');
                    }
                    if (aHasThumbnail) {
                        var dispDate = '';
                        if (programme.isOnAir) {
                            if (programme.nextView || programme.backwardView) {
                                dispDate = ' / ' + dateFormat(startTime, 'ddd.dd.mm');
                            }
                        }
                        this._nodeText01.textContent += dispDate;
                    }
                }
            }
            else {
                if (!isNaN(startTime)) {
                    nodesEm.push(this._nodeText01);
                    this._nodeText01.textContent = dateFormat(startTime, 'ddd.dd.mm');
                }
            }
            this._nodeChildren = children;
            return this;
        };
        CCellHTML.prototype.disassemble = function (aChildren) {
            if (aChildren === void 0) { aChildren = this._nodeChildren; }
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
        };
        CCellHTML.prototype.assemble = function (aElRoot, aChildren) {
            if (aElRoot === void 0) { aElRoot = this._elRoot; }
            if (aChildren === void 0) { aChildren = this._nodeChildren; }
            for (var i = 0, subtree; i < aChildren.length; i++) {
                if (aChildren[i] instanceof Array) {
                    subtree = aChildren[i];
                    aElRoot.appendChild(arguments.callee(subtree[0], subtree.slice(1)));
                    continue;
                }
                aElRoot.appendChild(aChildren[i]);
            }
            return aElRoot;
        };
        CCellHTML.prototype.wipe = function (disassembl) {
            if (disassembl) {
                this.disassemble();
            }
            this._doWipe();
            this._nodeChildren = [];
            return this;
        };
        CCellHTML.prototype.reqToCheckRecInProgress = function (aSchedule) {
            //TODO:
            //if (this.recInProgress(aSchedule))
            return false;
        };
        return CCellHTML;
    })();
    exports.CCellHTML = CCellHTML;
});
