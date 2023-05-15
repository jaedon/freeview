///<reference path="../../def/sugar.d.ts" />
// file name : pages/component/cSimpleList.js
/*jslint sloppy:true, nomen:true, vars:true*/

import __KEYNAVIGATOR__ = require("pages/component/cmpt_keynavigator");
declare var hx: ifSugar;

export class simpleList extends __KEYNAVIGATOR__ {
    source = null;
    currentIndex = 0;
    dispalyedData = [];
    displayedCount = 0;
    DISPLAY_COUNT = 5;

    constructor($, focusable, focused) {
        super($, focusable, focused);
    }

    bind(source, displayCount) {
        if (source) {
            this.source = source;
        }
        if (displayCount) {
            this.DISPLAY_COUNT = displayCount;
        }
    }

    get(aName?: string): any {
        var start;
        var end;
        var data = [];
        if (this.source) {
            start = this.currentIndex;
            end = start + this.DISPLAY_COUNT;
            data = this.source.slice(start, end);
        }
        return data;
    }

    draw() {
        var data = this.get();
        this.dispalyedData = data;
        this.displayedCount = data.length;
        if (this.doDraw) {
            this.doDraw(data);
        }
        this.build(this.upDownBuilder);
    }

    KEY_UP(param) {
        var handled = this.doNavi(param);
        if (!handled) {
            this.currentIndex -= 1;
            this.currentIndex = Math.max(0, this.currentIndex);
            this.draw();
        }
        if (this.currentIndex > 0) {
            handled = true;
        }
        return handled;
    }

    KEY_DOWN(param) {
        var handled = this.doNavi(param);
        if (!handled && this.displayedCount === this.DISPLAY_COUNT && this.dispalyedData.length !== this.DISPLAY_COUNT) {
            this.currentIndex += 1;
            this.currentIndex = Math.min(this.source.length - 1, this.currentIndex);
            this.draw();
        }
        return handled;
    }
}
