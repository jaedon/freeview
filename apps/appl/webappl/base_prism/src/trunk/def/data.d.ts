/**
 * Created by bglee@humaxdigital.com on 2014. 5. 19..
 * product oriented data
 */
interface ifDataSettingSelectionMenu<T> {
    title: string;
    items: T[];
    itemId?: any;
    itemStr?: any;
}
interface ifDataSettingMainMenu {
    parental_control?: ifDataSettingSelectionMenu<any>;
    language?: ifDataSettingSelectionMenu<any>;
    video?: ifDataSettingSelectionMenu<any>;
    audio?: ifDataSettingSelectionMenu<any>;
    on_screen_display?: ifDataSettingSelectionMenu<any>;
}
interface ifSettingNetworkMenu {
    live_tv_streaming?: ifDataSettingSelectionMenu<any>;
}

interface ifSettingPvrMenu {
    recording_option?: ifDataSettingSelectionMenu<any>;
}

interface ifSettingSystemMenu {
    system_information?: ifDataSettingSelectionMenu<any>;
}

interface ifDataSettingCategory {
    general_settings?: ifDataSettingMainMenu;
    network_settings?: ifSettingNetworkMenu;
    pvr_settings?: ifSettingPvrMenu;
    system_settings?: ifSettingSystemMenu;
    channel_settings?: ifDataSettingMainMenu;
}
interface ifDataProductData {
    settings?: ifDataSettingCategory;
}

interface ifDataProductDataInterface {
    settings: ifDataSettingCategory;
}

interface ifDataHandlerSelectionMenu {
    getTitle(): string;
    getItemTitles(): string[];
    getItemOptions(aItemTitle: string): string[];
}
