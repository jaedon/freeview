/**
 * Created by deptno on 2014. 8. 11..
 */
//define(["require", "exports", "text!product_name", "env/config"], function(require, exports, __PRODUCT_NAME__, __CONFIGURATOR__) {
require(["text!product_name", "env/config"], function(__PRODUCT_NAME__, __CONFIGURATOR__) {
    var __config__;
    require([
            "env/conf_" + __PRODUCT_NAME__.toLowerCase(),
            "text!product/" + __PRODUCT_NAME__.toLowerCase() + "/data.json"
    ], function(__OWN_CONFIG__, __OWN_DATA__) {
        __config__ = new __CONFIGURATOR__(__PRODUCT_NAME__, __PRODUCT_NAME__, __OWN_DATA__);
        console.log("loaded");
        var fnGetServiceNameByPath = function(aPath) {
            var services = __config__.getService();
            var ret = null;
            for (var key in services) {
                if (services[key] === aPath) {
                    ret = key;
                    break;
                }
            }
            return ret;
        };
        var fnGetPageNameByPath = function(aPath) {
            var services = __config__.getPages();
            var ret = null;
            for (var key in services) {
                if (services[key].indexOf(aPath) !== -1) {
                    ret = key;
                    break;
                }
            }
            return ret;
        };
        var map = {
            "logger": function(aLogger) {
                for (var key in aLogger) {
                    hx[key] = aLogger[key];
                }
                console.log("info", "[map_load_after] set hx");
            },
            "keytable": function(__KEY_TABLE__, __PRODUCT_KEY_TABLE__, __EMUL_KEY_TABLE__) {
                $.extend(__KEY_TABLE__, __PRODUCT_KEY_TABLE__, __EMUL_KEY_TABLE__);
                hx.key = __KEY_TABLE__;
            },
            "service": function(aConstructor) {
                hx.svc.addService()
            }
        };
        var reloadCSS = function () {
            console.log("ReloadCSS");
            var elements = document.getElementsByTagName("link");

            for (var i = 0; i < elements.length; i++) {
                if (elements[i].rel == "stylesheet") {
                    var href = elements[i].getAttribute("data-href")

                    if (href == null) {
                        href = elements[i].href;
                        elements[i].setAttribute("data-href", href);
                    }

                    /*
                     if (window.__BL_OVERRIDE_CACHE) {
                     var link = document.createElement("link");
                     link.href = href;
                     link.rel = "stylesheet";
                     document.head.appendChild(link);

                     document.head.removeChild(elements[i]);

                     continue;
                     }
                     */
                    elements[i].href = href + ((href.indexOf("?") == -1) ? "?" : "&") + "c=" + (new Date).getTime();
                }
            }
        };
        function getExtension(aFilepath) {
            return aFilepath.substring(aFilepath.lastIndexOf(".") + 1).toLowerCase();
        }
        window.__reloadFile = function(aFilePath) {
            var suffix = aFilePath.lastIndexOf(".js") !== -1 ? "?" : ".js?";
            var filesToLoad = [aFilePath + suffix + new Date().getTime()];
            var tree = {
                css: {
                    fx: function() {
                        reloadCSS(aFilePath);
                    }
                },
                prismux: {
                    fx: function() {
                        var ext = getExtension(aFilePath);
                        if (ext === "css") {
                            reloadCSS(aFilePath);
                        } else if (ext === "js") {
                            console.log(filesToLoad.join() + " reload");
                            require(filesToLoad);
                        }
                    }
                },
                env: {
                    keytable: {
                        fx: function() {

                        }
                    }
                },
                layer_domain: {
                    fx: null,
                    service: {
                        fx: function() {
                            /* COMMENT
                             * service reload failed
                             * there are no event reset functions;
                             * * bglee@humaxdigital.com, 16:12, 2014. 8. 12.
                             */
                            //require(filesToLoad, function(aConstructor) {
                            //hx.svc.addService(fnGetServiceNameByPath(aFilePath), aConstructor, true);
                            //})
                        }
                    }
                },
                pages: {
                    fx: function() {
                        var pageName = fnGetPageNameByPath(aFilePath);
                        var alEntity = hx.al.entity;
                        var page = alEntity._repos.get(pageName);
                        if (page) {
                            page.destroy();
                        }
                        alEntity._repos.remove(pageName);
                        if (alEntity.getCenterPageName() === pageName) {
                            alEntity._pageMgr.change("CPgLiveController");
                        }
                    }
                },
                model: {
                    fx: function() {
                        console.log("model changed");
                    }
                }
            };
            var splited = aFilePath.replace("\\","/").split("/");
            var way = tree;
            while (splited.length) {
                var next = way[splited.shift()];
                if (!next) {
                    break;
                }
                way = next;
            }
            var fx = way.fx;
            if (!fx) {
//                throw "make fx";
            }
            fx();
        };
    });
});

