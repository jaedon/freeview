/**
 * Created by hsseo on 14. 5. 12.
 */

module TDATA
{
    export interface TStartInfoLive {
        liveType : number;
        masterSvcUid : number;
        suppleSvcUid : number;
        suppSvcType : number;
        majorCHNum : number;
    }

    export interface TStartInfoPvrPb {
        contentId : number;
        startTime : number;
    }

    export interface TStartInfoTsrPb {
        masterSvcUid : number;
        suppleSvcUid : number;
        suppSvcType : number;
        startTime : number;
    }

    export interface TStartInfoMediaPlay {
        videoCodec : number;
        audioCodec : number;
        videoPid : number;
        audioPid : number;
        startTime : number;
        url : string;
    }

    export interface TService {
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

    export interface TNetworkInfo {
        uid: number;
        typeOf: string;
        version: number;
        onid: number;
        name: string;
        deliType: number;
    }

    export interface TTransponderInfo {
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

    export interface TProviderInfo {
        uid: number;
        typeOf: string;
        version: number;
        name: string;
    }

    export interface TGroupInfo {
        uid: number;
        typeOf: string;
        version: number;
        id: number;
        name: string;
    }

    export interface TBouquetInfo {
        uid: number;
        typeOf: string;
        version: number;
        id: number;
        name: string;
    }

    export interface TChannelLogoInfo {
        svcuid: number;
        ucServerLogoUrl: string;
        ucLocalLogoUrl: string;
    }

    export interface TSubtitleTrack {
        pid: number;
        componentTag: number;
        type: number;
        typeValue: number;
        componentExtTag: number;
        langCode: string;
    }

    export interface TAntennaInfo {
        uid : number;
        satType : string;
        satName : string;
        antType : number;
        extInfo : ArrayBuffer;
    }

    export interface TEvent {
        type : number;
        refCount : number;
        uid : number;
        svcuid : number;
        onid : number;
        tsid : number;
        svcid : number;
        eventid : number;
        startTime : number;
        duration : number;
        runStatus : number;
        freeCaMode : number;
        language : number;
        name : string;
        text : string;
        parentalRating : number;
    }

    export interface TRecListData {
        contentId : number;
        url : string;
        name : string;
        thumbnailPath : string;
        recStartTime : number;
        recDuration : number;
        recOrgDuration : number;
        type : number;
        recStatus : number;
        recEndReason : number;
    }

    export interface TSchedule {
        slot : number;
        rsvType : number;
        svcUid : number;
        onid : number;
        tsid : number;
        svcid : number;
        rsvReady : number;
        startTime : number;
        duration : number;
        repeat : number;
        repeatDays : number;
        status : number;
        exceptConflict : number;
        epgType : number;
        scheduleSpec : number;
        removed : number;
        remevedReason : number;
        detailInfo : number;
    }

    export interface TTunerParamSat {
        deliveryType : number;
        tunerGroupId : number;
        rfInputId : number;
        antennaType : number;
        frequency : number;
        symbolrate : number;
        feccoderate : number;
        polarization : number;
        transSpec : number;
        pskMod : number;
        pilot : number;
        rolloff : number;
        satType : string;
        antuid : number;
    }

    export interface TTunerParamTer {
        deliveryType : number;
        tunerGroupId : number;
        frequency : number;
        bandWidth : number;
        constellation : number;
        hierachy : number;
        codeRate : number;
        transMode : number;
        guardInterval : number;
        stream : number;
        offset : number;
        system : number;
    }

    export interface TTunerParamCab {
        deliveryType : number;
        tunerGroupId : number;
        frequency : number;
        symbolRate : number;
        constellation : number;
        spectrum : number;
        tsId : number;
        netId : number;
    }

    export interface TCarouselItem {
        carouselId : number;
    }

    export interface TBTDeviceinfo {
        devId : number;
        devName : string;
        status : number;
        devClass : string;
        supportedProfie : number;
        address : string;
    }

    export interface TNetDevinfo {
        type : number;
        isConnected : number;
        isNetActive : number;
        isRunning : number;
        macAddr : string;
        devName : string;
    }

    export interface TFTPServerInfo {
        isStarted : number;
        clientCount : number;
        serverName : string;
        port : number;
    }

    export interface TFTPShared {
        name : string;
        path : string;
        state : number;
        isWritable : number;
        isMounted : number;
    }

    export interface TCASMMIEvent {
        eventType : number;
        session : number;
        sessionType : number;
        module : number;
        slot : number;
        encodeUiEvent : string;
    }

    export interface TDMSInfo {
        title : string;
        UDN : string;
        IP : string;
        deviceHandle : number;
        macAddress : string;
        totalStroageSize : string;
        isSupportSrs : number;
        sortCapNum : number;
        searchCapNum : number;
        dlnaCapNum : number;
        JLabsCapNum : number;

    }

    export function convert_dmsinfo(aDBusData:any):TDMSInfo {
        var ret:TDMSInfo = {
            title : aDBusData[0],
            UDN : aDBusData[1],
            IP : aDBusData[2],
            deviceHandle : aDBusData[3],
            macAddress : aDBusData[4],
            totalStroageSize : aDBusData[5],
            isSupportSrs : aDBusData[6],
            sortCapNum : aDBusData[7],
            searchCapNum : aDBusData[8],
            dlnaCapNum : aDBusData[9],
            JLabsCapNum : aDBusData[10]
        };
        return ret;
    }

    export function convert_casmmievent(aDBusData:any):TCASMMIEvent {
        var ret:TCASMMIEvent = {
            eventType : aDBusData[0],
            session : aDBusData[1],
            sessionType : aDBusData[2],
            module : aDBusData[3],
            slot : aDBusData[4],
            encodeUiEvent : aDBusData[5]
        };
        return ret;
    }

    export function convert_ftpshared(aDBusData:any):TFTPShared {
        var ret:TFTPShared = {
            name : aDBusData[0],
            path : aDBusData[1],
            state : aDBusData[2],
            isWritable : aDBusData[3],
            isMounted : aDBusData[4]
        };
        return ret;
    }

    export function convert_ftpserverinfo(aDBusData:any):TFTPServerInfo {
        var ret:TFTPServerInfo = {
            isStarted : aDBusData[0],
            clientCount : aDBusData[1],
            serverName : aDBusData[2],
            port : aDBusData[3]
        };
        return ret;
    }

    export function convert_netdevinfo(aDBusData:any):TNetDevinfo {
        var ret:TNetDevinfo = {
            type : aDBusData[0],
            isConnected : aDBusData[1],
            isNetActive : aDBusData[2],
            isRunning : aDBusData[3],
            macAddr : aDBusData[4],
            devName : aDBusData[5]
        };
        return ret;
    }

    export function convert_btdeviceinfo(aDBusData:any):TBTDeviceinfo {
        var ret:TBTDeviceinfo = {
            devId : aDBusData[0],
            devName : aDBusData[1],
            status : aDBusData[2],
            devClass : aDBusData[3],
            supportedProfie : aDBusData[4],
            address : aDBusData[5]
        };
        return ret;
    }

    export function convert_carouselitem(aDBusData:any):TCarouselItem {
        var ret:TCarouselItem = {
            carouselId : aDBusData[0]
        };
        return ret;
    }

    export function convert_tunerparam_cab(aDBusData:any):TTunerParamCab {
        var ret:TTunerParamCab = {
            deliveryType : aDBusData[0],
            tunerGroupId : aDBusData[1],
            frequency : aDBusData[2],
            symbolRate : aDBusData[3],
            constellation : aDBusData[4],
            spectrum : aDBusData[5],
            tsId : aDBusData[6],
            netId : aDBusData[7]
        };
        return ret;
    }

    export function convert_tunerparam_ter(aDBusData:any):TTunerParamTer {
        var ret:TTunerParamTer = {
            deliveryType : aDBusData[0],
            tunerGroupId : aDBusData[1],
            frequency : aDBusData[2],
            bandWidth : aDBusData[3],
            constellation : aDBusData[4],
            hierachy : aDBusData[5],
            codeRate : aDBusData[6],
            transMode : aDBusData[7],
            guardInterval : aDBusData[8],
            stream : aDBusData[9],
            offset : aDBusData[10],
            system : aDBusData[11]
        };
        return ret;
    }

    export function convert_tunerparam_sat(aDBusData:any):TTunerParamSat {
        var ret:TTunerParamSat = {
            deliveryType : aDBusData[0],
            tunerGroupId : aDBusData[1],
            rfInputId : aDBusData[2],
            antennaType : aDBusData[3],
            frequency : aDBusData[4],
            symbolrate : aDBusData[5],
            feccoderate : aDBusData[6],
            polarization : aDBusData[7],
            transSpec : aDBusData[8],
            pskMod : aDBusData[9],
            pilot : aDBusData[10],
            rolloff : aDBusData[11],
            satType : aDBusData[12],
            antuid : aDBusData[13]
        };
        return ret;
    }

    export function convert_schedule(aDBusData:any):TSchedule {
        var ret:TSchedule = {
            slot : aDBusData[0][0],
            rsvType : aDBusData[0][1],
            svcUid : aDBusData[0][2],
            onid : aDBusData[0][3][0],
            tsid : aDBusData[0][3][1],
            svcid : aDBusData[0][3][2],
            rsvReady : aDBusData[0][4],
            startTime : aDBusData[0][5],
            duration : aDBusData[0][6],
            repeat : aDBusData[0][7],
            repeatDays : aDBusData[0][8],
            status : aDBusData[0][9],
            exceptConflict : aDBusData[0][10],
            epgType : aDBusData[0][11],
            scheduleSpec : aDBusData[1][0],
            removed : aDBusData[2][0],
            remevedReason : aDBusData[3][0],
            detailInfo : aDBusData[3][1]
        };
        return ret;
    }

    export function convert_reclistdata(aDBusData:any):TRecListData {
        var ret:TRecListData = {
            contentId : aDBusData[0],
            url : aDBusData[1],
            name : aDBusData[2],
            thumbnailPath : aDBusData[3],
            recStartTime : aDBusData[4],
            recDuration : aDBusData[5],
            recOrgDuration : aDBusData[6],
            type : aDBusData[7],
            recStatus : aDBusData[8],
            recEndReason : aDBusData[9]
        };
        return ret;
    }

    export function convert_event(aDBusData:any):TEvent {
        var ret:TEvent = {
            type : aDBusData[0][0],
            refCount : aDBusData[0][1],
            uid : aDBusData[0][2],
            svcuid : aDBusData[0][3],
            onid : aDBusData[0][4],
            tsid : aDBusData[0][5],
            svcid : aDBusData[0][6],
            eventid : aDBusData[0][7],
            startTime : aDBusData[0][8],
            duration : aDBusData[0][9],
            runStatus : aDBusData[0][10],
            freeCaMode : aDBusData[0][11],
            language : aDBusData[1][0],
            name : aDBusData[2][1],
            text : aDBusData[3][2],
            parentalRating : aDBusData[4][3]
        };
        return ret;
    }
    export function convert_antennainfo(aDBusData:any):TAntennaInfo {
        var ret:TAntennaInfo = {
            uid: aDBusData[0],
            satType: aDBusData[1],
            satName: aDBusData[2],
            antType: aDBusData[3],
            extInfo: aDBusData[4]
        };
        return ret;
    }

    export function convert_service(aDBusData:any):TService {
        var ret:TService = {
            uid: aDBusData[0],
            tsuid: aDBusData[1],
            prvuid: aDBusData[2],
            antuid: aDBusData[3],

            svcid: aDBusData[4],
            tsid: aDBusData[5],
            onid: aDBusData[6],

            lcn: aDBusData[7],

            svcType: aDBusData[8],
            deliType: aDBusData[9],
            casType: aDBusData[10],

            name: aDBusData[11],
            satType: aDBusData[12]
        };
        return ret;
    }

    export function convert_network(aDBusData:any):TNetworkInfo {
        var ret:TNetworkInfo = {
            uid: aDBusData[0],
            typeOf: aDBusData[1],
            version: aDBusData[2],
            onid: aDBusData[3],
            name: aDBusData[4],
            deliType: aDBusData[5]
        };
        return ret;
    }

    export function convert_transponder(aDBusData:any):TTransponderInfo {
        var ret:TTransponderInfo = {
            uid: aDBusData[0],
            typeOf: aDBusData[1],
            version: aDBusData[2],
            netuid: aDBusData[3],
            tsid: aDBusData[4],
            onid: aDBusData[5],
            tunerid: aDBusData[6],
            deliType: aDBusData[7],
            eDeliType: aDBusData[8]
        };
        return ret;
    }

    export function convert_provider(aDBusData:any):TProviderInfo {
        var ret:TProviderInfo = {
            uid: aDBusData[0],
            typeOf: aDBusData[1],
            version: aDBusData[2],
            name: aDBusData[3]
        };
        return ret;
    }

    export function convert_group(aDBusData:any):TGroupInfo {
        var ret:TGroupInfo = {
            uid: aDBusData[0],
            typeOf: aDBusData[1],
            version: aDBusData[2],
            id: aDBusData[3],
            name: aDBusData[4]
        };
        return ret;
    }

    export function convert_bouquet(aDBusData:any):TBouquetInfo {
        var ret:TBouquetInfo = {
            uid: aDBusData[0],
            typeOf: aDBusData[1],
            version: aDBusData[2],
            id: aDBusData[3],
            name: aDBusData[4]
        };
        return ret;
    }

    export function convert_logo(aDBusData:any):TChannelLogoInfo {
        var ret:TChannelLogoInfo = {
            svcuid: aDBusData[0],
            ucServerLogoUrl: aDBusData[1],
            ucLocalLogoUrl: aDBusData[2]
        };
        return ret;
    }

    export function convert_subtitletrack(aDbusData:any):TSubtitleTrack {
        var ret:TSubtitleTrack = {
            pid: aDbusData[0],
            componentTag: aDbusData[1],
            type: aDbusData[2],
            typeValue: aDbusData[3],
            componentExtTag: aDbusData[4],
            langCode: aDbusData[7]
        };
        return ret;
    }

    /*
     export function compare_service(aA: TService, aB: TService): boolean {
     if (aA.uid != aB.uid) {
     return false;
     }
     return true;
     }
     */
}

export = TDATA;