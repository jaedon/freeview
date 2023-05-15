///<reference path="../def/sugar.d.ts" />
// file name : fw/common/entry/alComposer.js
/*jslint sloppy:true, nomen:true, vars:true */
/**
 * composer, make component & compose component too
 *
 * @class hx.al.composer
 */

declare var hx: ifSugar;

module mComposer {
    export class cALComposer {
        _requestLoading = [];
        _loadingDone = [];
        _entity: any;
        _components: ifOBJKeyStringArray;
        _events: any;
        name: string;
        constructor(entity, components, events, cb) {
            this.name = "composer";
            this._entity = entity;
            this._components = components;
            this._events = events;
            this._LoadComponents(cb);
        }
        _LoadComponents(cb) {
            var factory = this._entity._factory;
            var repository = this._entity._repos;
            var comp = this._components;
            var keys = Object.keys(comp);
            var len = keys.length;
            var i = 0;
            var countComponent = 0;
            var fnCounter = (function(end, cb) {
                var count = 0;
                return function() {
                    if (++count === end) {
                        cb();
                    }
                }
            }(len, cb));
            for (var i = 0; i < len; i++) {
                factory.create(keys[i], function(obj, aKey) {
                    repository.add(aKey, obj);
                    fnCounter();
                })
            }
            return;
            var _fnLoadSequencally = () => {
                var _createDone = (obj, aKey) => {
                    repository.add(aKey, obj);
                    this._loadingDone.push(aKey);
                    countComponent--;
                    hx.log("info", "-- countComponent: " + countComponent);
                    _fnLoadSequencally();
                };
                if (i < len) {
                    this._requestLoading.push(keys[i]);
                    countComponent += comp[keys[i]].length;
                    hx.log("info", "++ countComponent: " + countComponent);
                    factory.create(keys[i++], _createDone);
                    return;
                }
                hx.log("info", "=== countComponent: " + countComponent);
                if (countComponent === 0) {
                    cb();
                }
            };
            _fnLoadSequencally();
        }
        /*  create component */
        createComponent(name, param) {
            var com;
            if ( typeof (name) === 'string') {
                var repository = this._entity._repos;
                com = repository.get(name);
                com = new com();
                if (!com) {
                    hx.log("error", "Not found this component in the repository - " + name);
                    return;
                }
            } else {
                com = name;
            }
            if (com.create) {
                com.create(param);
                com.setName(name);
            }

            if (com.destroy && com._hook_destory_fx === undefined) {
                com._hook_destroy_fx = com.destroy;
            }
            com._active_fx = true;
            com.destroy = function() {
                if (com._hook_destroy_fx && com._active_fx) {
                    com._hook_destroy_fx();
                    com._active_fx = false;
                }
            };
            com.__created__ = true;
            return com;
        }
        destroyComponent(com) {
            /**DEVCODE*/
            if (com === undefined || com === null) {
                return;
            }
            /*DEVCODE**/
            if (com.destroy) {
                com.destroy();
            }

            if (!!com._subObj) {
                var i;
                for ( i = 0; i < com._subObj.length; i += 1) {
                    this.destroyComponent(com._subObj[i]);
                }
            }
            com.focus = undefined;
            com._subObj = undefined;
            com._superObj = undefined;
            com.__created__ = undefined;
        }
        /* page related section */
        addPage(page) {
            if (!!page._composer) {
                return page;
            }
            /* compose page */
            page._composer = this;
            page._entity = this._entity;
            page._keyeventTable = {};

            var t = page._keyeventTable;
            t.alKey = !!page._on_alKey;
            t.alEndOfRepeatKey = !!page._on_alEndOfRepeatKey;
            t.alKeyt = [page._keyTable];
            t.alEndOfRepeatKeyt = [page._keyRTable];

            return page;
        }
        removePage(page) {
            if (!page._composer) {
                return;
            }
            if (!!page._subObj) {
                var i;
                for ( i = 0; i < page._subObj.length; i += 1) {
                    this.destroyComponent(page._subObj[i]);
                }
            }
            page._subObj = undefined;
        }
        /* EVENT handler section */
        doCommand(param) {
            if (!param.alPage) {
                throw "stop propagation hxCommand";
            }
            hx.log("appKey", "[COMPOSER] page : " + param.alPage.name + ": event : " + param.alEvent);
            var fObj = param.alPage.getLastFocusedElement();
            if (!fObj) {
                return false;
            }

            var ev = this._events[param.alEvent];
            if (!ev) {
                throw "event is not defined";
            }
            if (ev.fx) {
                ev.fx(param);
            }
            hx.log("ComposerEvent", "send hxCommnad - " + param.alEvent);
            return fObj.doCommand(param);
        }
    }
}
export = mComposer;