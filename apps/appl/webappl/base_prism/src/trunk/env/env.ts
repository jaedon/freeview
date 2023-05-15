/// <reference path="../def/require.d.ts" />
/// <reference path="../def/config.d.ts" />
/// <reference path="../def/data.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 3. 18..
 */
import __DEF__ = require("../def/config.def");
module mENV {
    function log(aMessage: string) {
        var ENV = "[ENV] : ";
        console.log(ENV + aMessage);
    }
    //XXX: _cache is export variable but does not used
    var _loaded: boolean = false;
    var callback: (aENV: ifProductConfiguration) => void;
    var configurator: ifProductConfiguration;
    require.onError = (aErr: RequireError) => {
        throw aErr;
    };
    require(["text!product_name"], (aProductName: string) => {
        log('PRODUCT NAME : ' + aProductName);
        aProductName = aProductName.trim();
        require([
            "./config",
            './conf_' + aProductName.toLowerCase(),
            "text!product/" + aProductName.toLowerCase() + "/data.json"
        ], (aProductConfigurator: any, aProductConfig: ifProductConfiguration, aProductData) => {
            log('PRODUCT NAME : ' + aProductName);
            configurator = new aProductConfigurator(aProductName, aProductConfig, aProductData);
            if (callback) {
                callback(configurator);
            }
            _loaded = true;
            window.ENV = new cEnvInterface(configurator.getConfig(), configurator.getUnclassifiedData(), aProductData, configurator.getModeDistribution());
        });
    });
    export function onload(aCallback: (aENV: ifProductConfiguration) => void) {
        callback = aCallback;
        if (_loaded) {
            aCallback(configurator);
        }
    }

    export class cProductDataInterface implements ifDataProductDataInterface {
        private _productData: ifDataProductData;
        constructor(aProductData) {
             this._productData = JSON.parse(aProductData)
        }
        get settings(): ifDataSettingCategory {
            return this._productData.settings;
        }
    }
    export class cEnvInterface implements ifENV {
        private _config: ifCONFIG;
        private _productConfig: any;
        private _productData: cProductDataInterface;
        private _modeDistribution: string;

        constructor(aConfig: ifCONFIG, aProductConfig: any, aProductData: any, aModeDistribution: string) {
            this._config = aConfig;
            this._productConfig = aProductConfig;
            this._productData = new cProductDataInterface(aProductData);
            this._modeDistribution = aModeDistribution;
        }
        get productData(): cProductDataInterface {
            return this._productData;
        }
        get op(): number {
            return this._config.op;
        }
        get model(): number {
            return this._config.model;
        }
        get cas(): number {
            return this._config.useCAS;
        }
        get casOTA(): number {
            return this._config.useCasOTA;
        }
        get listOP(): ifENVOP {
            return __DEF__.eOP;
        }
        get listModel(): ifENVProductName {
            return __DEF__.eProductName;
        }
        get listCAS(): ifENVCAS {
            return __DEF__.eCAS;
        }
        get listCasOTA(): ifENVCASOTA {
            return __DEF__.eCASOTA;
        }
        get isModeRelease() {
            return __DEF__.eModeDistribution[__DEF__.eModeDistribution.RELEASE] === this._modeDistribution;
        }
    }
    /* COMMENT
     * make window.ENV
     * * bglee@humaxdigital.com, 14:22, 2014. 5. 9.
     */
}
export = mENV;
