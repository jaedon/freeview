/// <reference path="../def/prism.d.ts" />
// file name : prismux/prism.page.main.js
/*jslint nomen:true, vars:true */
import __CONTROLS__ = require('pages/util/controls');
import __PRISM_CPAGE__ = require('./prism.page');

module mPrism {
    export class CMainPage extends __PRISM_CPAGE__.CPage {
        constructor($el) {
            super($el);
        }
        init($el) {
            var a = this;
            super.init.call(a, $el);
            a.setId('main');
        }
        draw(param?) {
            var a = this;
            if (!a.$layer()) {
                a.createLayer({
                    fnAfterCreate: () => {
                        var childControl = [];
                        a.setChildControl(childControl);
                        super.draw.call(a, {
                            setActiveFocus: true
                        });
                    }
                });
            } else {
                super.draw.call(a, param);
            }
        }
    }

    if (!(<any>window).prism) {
        (<any>window).prism = {}
    }
    prism.createMainPage = function ($parent) {
        var page;
        var $page = $('<div>', {
            'class': 'prism-page'
        });
        $parent.append($page);
        return new CMainPage($page);
    };
}
export = mPrism;