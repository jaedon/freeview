define(["require", "exports", "../def/config.def"], function (require, exports, __DEF__) {
    var mENV;
    (function (mENV) {
        function log(aMessage) {
            var ENV = "[ENV] : ";
            console.log(ENV + aMessage);
        }
        //XXX: _cache is export variable but does not used
        var _loaded = false;
        var callback;
        var configurator;
        require.onError = function (aErr) {
            throw aErr;
        };
        require(["text!product_name"], function (aProductName) {
            log('PRODUCT NAME : ' + aProductName);
            aProductName = aProductName.trim();
            require([
                "./config",
                './conf_' + aProductName.toLowerCase(),
                "text!product/" + aProductName.toLowerCase() + "/data.json"
            ], function (aProductConfigurator, aProductConfig, aProductData) {
                log('PRODUCT NAME : ' + aProductName);
                configurator = new aProductConfigurator(aProductName, aProductConfig, aProductData);
                if (callback) {
                    callback(configurator);
                }
                _loaded = true;
                window.ENV = new cEnvInterface(configurator.getConfig(), configurator.getUnclassifiedData(), aProductData, configurator.getModeDistribution());
            });
        });
        function onload(aCallback) {
            callback = aCallback;
            if (_loaded) {
                aCallback(configurator);
            }
        }
        mENV.onload = onload;
        var cProductDataInterface = (function () {
            function cProductDataInterface(aProductData) {
                this._productData = JSON.parse(aProductData);
            }
            Object.defineProperty(cProductDataInterface.prototype, "settings", {
                get: function () {
                    return this._productData.settings;
                },
                enumerable: true,
                configurable: true
            });
            return cProductDataInterface;
        })();
        mENV.cProductDataInterface = cProductDataInterface;
        var cEnvInterface = (function () {
            function cEnvInterface(aConfig, aProductConfig, aProductData, aModeDistribution) {
                this._config = aConfig;
                this._productConfig = aProductConfig;
                this._productData = new cProductDataInterface(aProductData);
                this._modeDistribution = aModeDistribution;
            }
            Object.defineProperty(cEnvInterface.prototype, "productData", {
                get: function () {
                    return this._productData;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "op", {
                get: function () {
                    return this._config.op;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "model", {
                get: function () {
                    return this._config.model;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "cas", {
                get: function () {
                    return this._config.useCAS;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "casOTA", {
                get: function () {
                    return this._config.useCasOTA;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "listOP", {
                get: function () {
                    return __DEF__.eOP;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "listModel", {
                get: function () {
                    return __DEF__.eProductName;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "listCAS", {
                get: function () {
                    return __DEF__.eCAS;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "listCasOTA", {
                get: function () {
                    return __DEF__.eCASOTA;
                },
                enumerable: true,
                configurable: true
            });
            Object.defineProperty(cEnvInterface.prototype, "isModeRelease", {
                get: function () {
                    return __DEF__.eModeDistribution[1 /* RELEASE */] === this._modeDistribution;
                },
                enumerable: true,
                configurable: true
            });
            return cEnvInterface;
        })();
        mENV.cEnvInterface = cEnvInterface;
    })(mENV || (mENV = {}));
    return mENV;
});
