/**
 * Created by bglee@humaxdigital.com on 2014. 3. 16..
 */
/// <reference path="oipf.humax.d.ts" />

interface ifApplicationInfo {
    name: string;
    param: any;
    app: Application;
    wd?: WidgetDescriptor;
}
interface ifGlobalKeyCaptureInfo {
    fn : Function;
    keyList : number[];
}

interface ifFreeviewLinkage {
    channel: string;
    event: string;
    simul: string
}

interface ifSystemInfo {
    modelName : string;
    softwareVersion : string;
    loaderVersion : string;
    ciplusVersion : string;
    system_id : string;
    update_date : string;
    micom_version : string;
    serialNumber : string;
    chipId : string;
    wiredMacAddress : string;
    wirelessMacAddress : string;
    bluetoothMacAddress : string;
    esn : string;
}

