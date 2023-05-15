declare module TDATA {
    interface TStartInfoLive {
        liveType: number;
        masterSvcUid: number;
        suppleSvcUid: number;
        suppSvcType: number;
        majorCHNum: number;
    }
    interface TStartInfoPvrPb {
        contentId: number;
        startTime: number;
    }
    interface TStartInfoTsrPb {
        masterSvcUid: number;
        suppleSvcUid: number;
        suppSvcType: number;
        startTime: number;
    }
    interface TStartInfoMediaPlay {
        videoCodec: number;
        audioCodec: number;
        videoPid: number;
        audioPid: number;
        startTime: number;
        url: string;
    }
    interface TService {
        uid: number;
        tsuid: number;
        prvuid: number;
        antuid: number;
        svcid: number;
        tsid: number;
        onid: number;
        lcn: number;
        svcType: number;
        deliType: number;
        casType: number;
        name: string;
        satType: string;
    }
    interface TNetworkInfo {
        uid: number;
        typeOf: string;
        version: number;
        onid: number;
        name: string;
        deliType: number;
    }
    interface TTransponderInfo {
        uid: number;
        typeOf: string;
        version: number;
        netuid: number;
        tsid: number;
        onid: number;
        tunerid: number;
        deliType: number;
        eDeliType: number;
    }
    interface TProviderInfo {
        uid: number;
        typeOf: string;
        version: number;
        name: string;
    }
    interface TGroupInfo {
        uid: number;
        typeOf: string;
        version: number;
        id: number;
        name: string;
    }
    interface TBouquetInfo {
        uid: number;
        typeOf: string;
        version: number;
        id: number;
        name: string;
    }
    interface TChannelLogoInfo {
        svcuid: number;
        ucServerLogoUrl: string;
        ucLocalLogoUrl: string;
    }
    interface TSubtitleTrack {
        pid: number;
        componentTag: number;
        type: number;
        typeValue: number;
        componentExtTag: number;
        langCode: string;
    }
    interface TAntennaInfo {
        uid: number;
        satType: string;
        satName: string;
        antType: number;
        extInfo: ArrayBuffer;
    }
    interface TEvent {
        type: number;
        refCount: number;
        uid: number;
        svcuid: number;
        onid: number;
        tsid: number;
        svcid: number;
        eventid: number;
        startTime: number;
        duration: number;
        runStatus: number;
        freeCaMode: number;
        language: number;
        name: string;
        text: string;
        parentalRating: number;
    }
    interface TRecListData {
        contentId: number;
        url: string;
        name: string;
        thumbnailPath: string;
        recStartTime: number;
        recDuration: number;
        recOrgDuration: number;
        type: number;
        recStatus: number;
        recEndReason: number;
    }
    interface TSchedule {
        slot: number;
        rsvType: number;
        svcUid: number;
        onid: number;
        tsid: number;
        svcid: number;
        rsvReady: number;
        startTime: number;
        duration: number;
        repeat: number;
        repeatDays: number;
        status: number;
        exceptConflict: number;
        epgType: number;
        scheduleSpec: number;
        removed: number;
        remevedReason: number;
        detailInfo: number;
    }
    interface TTunerParamSat {
        deliveryType: number;
        tunerGroupId: number;
        rfInputId: number;
        antennaType: number;
        frequency: number;
        symbolrate: number;
        feccoderate: number;
        polarization: number;
        transSpec: number;
        pskMod: number;
        pilot: number;
        rolloff: number;
        satType: string;
        antuid: number;
    }
    interface TTunerParamTer {
        deliveryType: number;
        tunerGroupId: number;
        frequency: number;
        bandWidth: number;
        constellation: number;
        hierachy: number;
        codeRate: number;
        transMode: number;
        guardInterval: number;
        stream: number;
        offset: number;
        system: number;
    }
    interface TTunerParamCab {
        deliveryType: number;
        tunerGroupId: number;
        frequency: number;
        symbolRate: number;
        constellation: number;
        spectrum: number;
        tsId: number;
        netId: number;
    }
    interface TCarouselItem {
        carouselId: number;
    }
    interface TBTDeviceinfo {
        devId: number;
        devName: string;
        status: number;
        devClass: string;
        supportedProfie: number;
        address: string;
    }
    interface TNetDevinfo {
        type: number;
        isConnected: number;
        isNetActive: number;
        isRunning: number;
        macAddr: string;
        devName: string;
    }
    interface TFTPServerInfo {
        isStarted: number;
        clientCount: number;
        serverName: string;
        port: number;
    }
    interface TFTPShared {
        name: string;
        path: string;
        state: number;
        isWritable: number;
        isMounted: number;
    }
    interface TCASMMIEvent {
        eventType: number;
        session: number;
        sessionType: number;
        module: number;
        slot: number;
        encodeUiEvent: string;
    }
    interface TDMSInfo {
        title: string;
        UDN: string;
        IP: string;
        deviceHandle: number;
        macAddress: string;
        totalStroageSize: string;
        isSupportSrs: number;
        sortCapNum: number;
        searchCapNum: number;
        dlnaCapNum: number;
        JLabsCapNum: number;
    }
    function convert_dmsinfo(aDBusData: any): TDMSInfo;
    function convert_casmmievent(aDBusData: any): TCASMMIEvent;
    function convert_ftpshared(aDBusData: any): TFTPShared;
    function convert_ftpserverinfo(aDBusData: any): TFTPServerInfo;
    function convert_netdevinfo(aDBusData: any): TNetDevinfo;
    function convert_btdeviceinfo(aDBusData: any): TBTDeviceinfo;
    function convert_carouselitem(aDBusData: any): TCarouselItem;
    function convert_tunerparam_cab(aDBusData: any): TTunerParamCab;
    function convert_tunerparam_ter(aDBusData: any): TTunerParamTer;
    function convert_tunerparam_sat(aDBusData: any): TTunerParamSat;
    function convert_schedule(aDBusData: any): TSchedule;
    function convert_reclistdata(aDBusData: any): TRecListData;
    function convert_event(aDBusData: any): TEvent;
    function convert_antennainfo(aDBusData: any): TAntennaInfo;
    function convert_service(aDBusData: any): TService;
    function convert_network(aDBusData: any): TNetworkInfo;
    function convert_transponder(aDBusData: any): TTransponderInfo;
    function convert_provider(aDBusData: any): TProviderInfo;
    function convert_group(aDBusData: any): TGroupInfo;
    function convert_bouquet(aDBusData: any): TBouquetInfo;
    function convert_logo(aDBusData: any): TChannelLogoInfo;
    function convert_subtitletrack(aDbusData: any): TSubtitleTrack;
}
export = TDATA;
