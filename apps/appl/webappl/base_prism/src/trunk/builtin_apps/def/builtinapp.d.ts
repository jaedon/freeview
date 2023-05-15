/// <reference path="../../def/jquery.d.ts" />
/// <reference path="../../def/controls.d.ts" />
/// <reference path="../../def/prism.d.ts" />
/**
 * Created by bglee@humaxdigital.com on 2014. 4. 5..
 */

interface Window {
    log(aMessage: string): void;
    defineAPI(aAPIName: string, aFnHandler: Function): void;
    defineNotifyAPI(aNotifyName: string): void;
    notify(...Args: any[]): void;
    getRoot(): string;
    debug(aString: string): void;
    _mainMessageHandler(messageEvent: any): void;
    fnGetPreReceivedMessages(): any;
}

declare var log: (aMessage: any) => void;
declare var defineAPI: (aAPIName: string, aFnHandler: Function) => void;
declare var defineNotifyAPI: (aNotifyName: string) => void;
declare var notify: (...Args: any[]) => void;
declare var getRoot:() => string;
//declare var tr: (aString: string) => string;
//declare var debug: (aString: string) => void;

interface JQueryStatic {
    imgPreload(aParam: any): void;
}

declare var hx: any;
