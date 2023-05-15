/**
 * Created by bglee on 2014-10-30.
 */
interface ifUIFocusableControl {
    focusable?: boolean
}
interface ifPortalIcon {
    ImageType: string;
    Size: string;
    Url: string;
}
interface ifPortalCatecory {
    CategoryID: number;
    CategoryName: string;
    Lang: string;
}
interface ifPortalModelApps extends ifUIFocusableControl{
    appid: string;
    version: string;
    versioncode: number;
    name: string;
    downloadurl: string;
    contenturl: string;
    smartsearchurl: string;
    package: string;
    icons: ifPortalIcon[];
    UID: string;
    InstallUID: string;
    description: string;
    categorys: ifPortalCatecory[];
    PlayerType: string;
}
interface ifPortalRunApp {
    Status: boolean;
    Url: string;
    MessageType: string;
    title: string;
    appType: number;
    widgetUrl: string;
    Message: string[];
}

interface ifPortalRequestorAPI {
    isLogged(): any;
    login(aCallbackSuccess, aCallbackFail): void;
    getActiveProfile(aCallbackSuccess, aCallbackFail): void;
    getPrivacyPolicy(aCBSuccess: (aHTML: string, aAccepted: boolean) => void, aCBFail: (aHTML: string) => void): void;
    getTermsAndConditions(aCBSuccess: (aHTML: string, aAccepted: boolean) => void, aCBFail: (aHTML: string) => void): void;
    getModelPlayers(aCallback: (aModelApps: ifPortalModelApps[]) => void): void;
    getModelApps(aCallback: (aModelApps: ifPortalModelApps[], aIsUpdate: boolean, aIndex: number) => void): void;
    launchApp(app: any, aIsFromUSB?: boolean, aIsFVC?: boolean): void;
    setTvAppsArrangement(aAppIDList: string[]): void;
    setAcceptPrivacyTerms: (aPrivacyAgree: boolean, aTermsAgree: boolean, aCallback: (aSuccess: boolean) => void) => void;
    setAcceptPrivacy: (aAgree: boolean, aCallback: (aSuccess: boolean) => void) => void;
    setAcceptTerms: (aAgree: boolean, aCallback: (aSuccess: boolean) => void) => void;
}

interface ifPortalLogin {
    AcceptPrivacy: boolean;
    AcceptTerms: boolean;
    AuthTicket: string;
    CreateDate: string;
    DID: string;
    DeviceName: string;
    IsManager: boolean;
    PhotoUrl: string;
    ProfileName: string;
    UID: string;
}
interface ifPortalAcceptPrivacy {
    status: boolean;
    className: string;
    title : string;
    message: string[];
}