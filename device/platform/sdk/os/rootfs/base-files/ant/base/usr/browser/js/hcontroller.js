/**
 * @file hcontroller/hcontroller.js
 *
 * Root application for supporting HBBTV broadcast applications.
 *
 * Copyright (C) ANT Software Limited 2005-2009 - Company Confidential - All Rights Reserved
 *
 * $Header: //depot/app/galio/branch/venturi/src/controllers/ctrl_generic/resources/hbbtv/hcontroller/js/hcontroller.js#10 $
 */

/**
 * @fileOverview
 *
 * Manages the launching of HBBTV applications and their interaction with ANT
 * Galio Applications.
 */

var key_sets = {};
key_sets[ KeyboardEvent.VK_RED ] = keyset.RED;
key_sets[ KeyboardEvent.VK_GREEN ] = keyset.GREEN;
key_sets[ KeyboardEvent.VK_YELLOW ] = keyset.YELLOW;
key_sets[ KeyboardEvent.VK_BLUE ] = keyset.BLUE;
key_sets[ KeyboardEvent.VK_UP ] = keyset.NAVIGATION;
key_sets[ KeyboardEvent.VK_DOWN ] = keyset.NAVIGATION;
key_sets[ KeyboardEvent.VK_LEFT ] = keyset.NAVIGATION;
key_sets[ KeyboardEvent.VK_RIGHT ] = keyset.NAVIGATION;
key_sets[ KeyboardEvent.VK_ENTER ] = keyset.NAVIGATION;
key_sets[ KeyboardEvent.VK_BACK ] = keyset.NAVIGATION;
key_sets[ KeyboardEvent.VK_PLAY ] = keyset.VCR;
key_sets[ KeyboardEvent.VK_PAUSE ] = keyset.VCR;
key_sets[ KeyboardEvent.VK_STOP ] = keyset.VCR;
key_sets[ KeyboardEvent.VK_NEXT ] = keyset.VCR;
key_sets[ KeyboardEvent.VK_PREV ] = keyset.VCR;
key_sets[ KeyboardEvent.VK_FAST_FWD ] = keyset.VCR;
key_sets[ KeyboardEvent.VK_REWIND ] = keyset.VCR;
key_sets[ KeyboardEvent.VK_PLAY_PAUSE ] = keyset.VCR;
key_sets[ KeyboardEvent.VK_PAGE_UP ] = keyset.SCROLL;
key_sets[ KeyboardEvent.VK_PAGE_DOWN ] = keyset.SCROLL;
key_sets[ KeyboardEvent.VK_INFO ] = keyset.INFO;

var layers_key_PORTAL = 0x4000009C;
var layers_key_TV = 0x4000009D;

var mom_notifications = {};
mom_notifications[ "keydown" ] = MOMLocalSystem.NOTIFICATION_KEY_DOWN;
mom_notifications[ "keypress" ] = MOMLocalSystem.NOTIFICATION_KEY_PRESS;
mom_notifications[ "keyup" ] = MOMLocalSystem.NOTIFICATION_KEY_UP;
mom_notifications[ "portal open" ] = MOMLocalSystem.NOTIFICATION_PORTAL_OPEN;
mom_notifications[ "portal close" ] = MOMLocalSystem.NOTIFICATION_PORTAL_CLOSE;

var root_document = document.ownerWafer.ownerDocument;

/**
 *
 * @private
 * @return undefined
 */
function iterate_own_properties( obj, fn )
{
    for( prop in obj )
    {
        if( obj.hasOwnProperty( prop ) )
        {
            fn( prop );
        }
        else
        {
            /*  we've reached Object.prototype properties so we're
                not interested in anything else to come.
            */
            break;
        }
    }
}

function array_find( array, val )
{
    var rv = -1;
    var idx = 0;
    while( rv == -1 && idx < array.length )
    {
        if( array[idx] == val )
        {
            rv = idx;
        }
        idx++;
    }
    return rv;
}

var controller = applicationManager.getOwnerApplication( document );
controller.hbbtv_visible = true;
controller.show_hide = false;
controller.app_zorder = [];
controller.app_reshow = [];
controller.shown_hdlr = undefined;
controller.portal_started_hdlr = undefined;
controller.portal_stopped_hdlr = undefined;
controller.portal_app = undefined;
controller.current_package_list = null;
controller.portal_selected = false;

controller.start_app = function( uri )
{
    debug.log( "HBBTV |  starting app " + uri );
    app = this.createApplication( uri, true );
}

controller.restart_app = function( uri, app )
{
    this.terminate_app( uri, app );
    this.start_app( uri, app );
}

controller.terminate_app = function( uri, app )
{
    debug.log( "HBBTV |  killing app " + uri );
    this.terminating[ uri ] = app;
    app.destroyApplication();
}

controller.nothing = function( uri, app )
{
    /* nothing to do */
}

/* find the package associated with this application. */
function find_pkg( packagelist, uri )
{
    var found_pkg = null;
    
    if(uri)
    {
        var index = 0;
        for( var index = 0; index < packagelist.length; index++ )
        {
            var pkg = packagelist[index];
            
            if(pkg.uri == app.uri)
            {
                found_pkg = pkg;
                break;            
            }
        }
    }
    
    return found_pkg;
}


controller.list_children = function( app, actions )
{
    var children = app.children;
    var default_action = (this.portal_selected)? this.nothing : this.terminate_app;
    
    for( var idx = 0; idx < children.length; ++idx )
    {
        var child = children[ idx ];
        debug.log( "HBBTV |  got running app " + child.uri );
        
        var action = default_action;
        
        if( this.current_package_list != null )
        {
            var pkg = find_pkg( this.current_package_list, child.uri );
            if( pkg != null && pkg.serviceBound )
            {
                child.serviceBound = pkg.serviceBound;
                action = this.terminate_app;
            }
        }
        
        actions[ child.uri ] = {    action: action,
                                    app:    child };
        this.list_children( child, actions );
    }
}

controller.terminate_children = function( app, ignore )
{
    var children = app.children;
    for( var idx = 0; idx < children.length; ++idx )
    {
        var child = children[ idx ];
        if( child != ignore )
        {
            debug.log( "HBBTV |  terminate app " + child.uri );
            this.terminate_children( child );
            this.terminate_app( child.uri, child );
        }
    }
}

controller.terminate_all_but_autostart = function( start, autostart_uri )
{
    var killed_something = false;
    var children = start.children;
    for( var idx = 0; idx < children.length; ++idx )
    {
        var child = children[ idx ];

        killed_something =  killed_something ||
                            this.terminate_all_but_autostart(
                                                    child,
                                                    autostart_uri );

        if( autostart_uri == undefined ||
            child.uri != autostart_uri )
        {
            debug.log( "HBBTV | terminate_all killing " + child.uri );
            this.terminate_app( child.uri, child );
            killed_something = true;
        }
    }

    return killed_something;
}

controller.find_app = function( search_start, uri )
{
    var app = undefined;
    var children = search_start.children;
    var idx = 0;
    while( idx < children.length && app == undefined )
    {
        var child = children[ idx ];
        if( child.uri == uri )
        {
            app = child;
        }
        else
        {
            app = this.find_app( child, uri );
        }
        idx++;
    }

    return app;
}

controller.key_required = function( keyCode )
{
    var oipf_config = document.getElementById( 'oipf_config' );
    var key_set = key_sets[ keyCode ];

    if( key_set == undefined )
    {
        if( keyCode >= KeyboardEvent.VK_0 && keyCode <= KeyboardEvent.VK_9 )
        {
            key_set = keyset.NUMERIC;
        }
        else if(    ( keyCode >= 'A' && keyCode <= 'Z' ) ||
                    ( keyCode >= 'a' && keyCode <= 'z' ) )
        {
            key_set = keyset.ALPHA;
        }
    }

    return( key_set != undefined &&
            (oipf_config.keyset.value & key_set) == key_set );
}

controller.hide_current = function()
{
    while( this.app_zorder.length > 0 )
    {
        var app = this.app_zorder.shift();
        if( app )
        {
            debug.log( "HBBTV |  hiding " + app.uri );
            app.hide();
            app.deactivate();
            controller.app_reshow.push( app );
        }
    }
}

controller.reshow_current = function()
{
    while( this.app_reshow.length > 0 )
    {
        var app = this.app_reshow.shift();
        debug.log( "HBBTV |  reshowing " + app.uri );
        app.show();
        app.activate();
    }
}

controller.update_running_apps = function( service_selected )
{
    debug.log( "HBBTV | update_running_apps" );

    /*  three stages:
        1.  Go through the running apps and make a list, with an
            action for each of 'kill';
        2.  Go through the new package list and update the action
            for existing items and add any new apps;
        3.  Go over the list and take the appropriate action for
            each item.
    */
    var actions = {};
    var autostart_app = null;
    var autostart_uri = undefined;

    this.list_children( this, actions );

    this.current_package_list = mom.packageManager.createFilteredList(
                            MOMPackageDescriptor.TYPE_APPLICATION,
                            true,           /* signalled */
                            undefined,      /* stored */
                            undefined,      /* built in */
                            undefined );    /* display in menu */

    for( var idx = 0; idx < this.current_package_list.length; ++idx )
    {
        var app = this.current_package_list[ idx ];
        var uri = app.uri;
        var found_running = false;

        debug.log( "HBBTV |  examining app " + uri );

        if( uri in actions )
        {
            found_running = true;
        }
        else
        {
            /* uri is the signalled uri of the app being checked
             * actions[] contains the uris of what's running including queries
             * uri2 is the current.ai url without query
             * compare uri2 against actions[] stripped of uri and if matches 
             * set uri to uri2 + query from actions[]
             */
            var uri2 = "dvb://current.ait/" + app.organisationID + "." + app.packageID;
            debug.log( "HBBTV |  examining app " + uri2 );
            for( action_uri in actions )
            {
                var action_uri_split = action_uri.split("?");
                if ( uri2 == action_uri_split[0] )
                {
                    uri = uri2;
                    if ( action_uri_split[1] )
                    {
                        uri += '?' + action_uri_split[1];
                    }
                    found_running = true;
                }
            }
        }

        if ( found_running )
        {
            debug.log( "HBBTV |  found running app " + uri );

            /*  packages that are already running and are still
                valid take priority over everything else.
            */
            if( autostart_app != null &&
                actions[ autostart_uri ].app == undefined )
            {
                debug.log(  "HBBTV |  abandoning autostart of " +
                            autostart_uri +
                            " in favour of already running app " +
                            uri );
                actions[ autostart_uri ].action = this.nothing;
            }

            if( service_selected )
            {
                var running_app = controller.find_app( controller, uri );
                if(running_app && running_app.serviceBound)
                {
                    if(app.autostart)
                    {
                        /* this application was service bound to the last channel 
                           and it is now an autostart app so just re-start it. */
                        actions[ uri ].action = this.restart_app;
                    }/* else leave application to terminate */                
                }
                else
                {
                    actions[ uri ].action = this.nothing;
                }
            }
            else
            {
                actions[ uri ].action = this.nothing;
            }
            
            autostart_app = app;
            autostart_uri = uri;
        }
        else if( app.autostart )
        {
            /*  if we already have a lower priority autostart app
                that wasn't running in the previous service, then
                we shouldn't start it after all.
            */
            if( autostart_app != null &&
                app.priority > autostart_app.priority &&
                actions[ autostart_uri ].app == undefined )
            {
                debug.log(  "HBBTV |  abandoning autostart of " +
                            autostart_uri +
                            " in favour of higher priority app " +
                            uri );
                actions[ autostart_uri ].action = this.nothing;
                autostart_app = null;
                autostart_uri = undefined;
            }

            if( autostart_app == null )
            {
                debug.log( "HBBTV |  considering autostarting " + uri );
                actions[ uri ] = { action: this.start_app };
                autostart_app = app;
                autostart_uri = uri;
            }
        }
    }

    /* if no apps running then disable all keys so they all get 
     * passed to the native app */
    if ( ! autostart_app )
    {
        document.getElementById( 'oipf_config' ).keyset.value = 0;
    }

    iterate_own_properties( actions,
                            function( item )
                            {
                                action = actions[ item ];
                                action.action.call( controller,
                                                    item,
                                                    action.app );
                            } );
}

controller.toggle_teletext_app = function()
{
    /*  find the first teletext app, the currently running teletext
        app and the next teletext app after the currently running
        one. Any or all of these may not exist.
    */
    var new_package_list = mom.packageManager.createFilteredList(
                            MOMPackageDescriptor.TYPE_APPLICATION,
                            true,           /* signalled */
                            undefined,      /* stored */
                            undefined,      /* built in */
                            undefined );    /* display in menu */
    var cur_text_app = undefined;
    var first_text_app = undefined;
    var next_text_app = undefined;

    for( var idx = 0; idx < new_package_list.length; ++idx )
    {
        var app = new_package_list[ idx ];
        debug.log(  "HBBTV |  examining app " + app.uri +
                    ": usage type = " + app.usageType );

        if( app.usageType ==
            MOMPackageDescriptor.USAGE_TYPE_TEXT_APPLICATION )
        {
            debug.log( "HBBTV |  got teletext app " + app.uri );

            if( cur_text_app == undefined )
            {
                cur_text_app = this.find_app( controller, app.uri );

                if( cur_text_app == undefined )
                {
                    if( first_text_app == undefined )
                    {
                        debug.log( "HBBTV |  first teletext app " + app.uri );
                        first_text_app = app;
                    }
                }
                else
                {
                    debug.log( "HBBTV |  cur teletext app " + cur_text_app.uri );
                }
            }
            else
            {
                if( next_text_app == undefined )
                {
                    debug.log( "HBBTV |  next teletext app " + app.uri );
                    next_text_app = app;
                }
            }
        }
    }

    if( cur_text_app != undefined )
    {
        /* if there is another teletext app, then use terminate_app
            to suppress autostart, otherwise just kill the app and
            let autostart decide what to do next.
        */
        if( next_text_app != undefined )
        {
            this.terminate_app( cur_text_app.uri, cur_text_app );
        }
        else
        {
            cur_text_app.destroyApplication();
        }
    }
    else
    {
        next_text_app = first_text_app;
    }

    if( next_text_app != undefined )
    {
        /*  launch it; we can't wait to see if it starts successfully
            before killing other apps, as we do for the portal,
            because it might be a native app and so we may never see
            a running event.
        */
        debug.log( "HBBTV |  launching teletext app " + next_text_app.uri );
        var text_app = this.createApplication( next_text_app.uri, true );
        controller.terminate_children( controller, text_app );
    }
}

controller.launch_portal = function()
{
    /* read the config to find the portal app */
    var portal_config = document.ownerWafer.apis.config.get(
                                        "mom.hbbtv.portal" )
    var portal_uri = portal_config.value;
    debug.log( "HBBTV |  launching portal " + portal_uri );

    /*  launch it; when it starts successfully, kill all
        other running apps
    */
    if ( portal_uri )
    {
        this.portal_app = this.createApplication( portal_uri, true );

        applicationManager.addEventListener(
            "ApplicationEvent",
            controller_portal_launch_handler,
            false );
    }

    function controller_portal_launch_handler( event )
    {
        if( event.code == ApplicationEvent.RUNNING &&
            event.app == controller.portal_app )
        {
            debug.log( "HBBTV | portal running " + controller.portal_app.uri );

            applicationManager.removeEventListener(
                                    "ApplicationEvent",
                                    controller_portal_launch_handler,
                                    false );
            controller.portal_app.show();
            controller.portal_app.activate();

            controller.terminate_children( controller, controller.portal_app );

            if( controller.portal_started_hdlr != undefined )
            {
                controller.portal_started_hdlr();
            }
        }
    }
}

controller.get_autostart_uri = function()
{
    var new_package_list = mom.packageManager.createFilteredList(
                            MOMPackageDescriptor.TYPE_APPLICATION,
                            true,           /* signalled */
                            undefined,      /* stored */
                            undefined,      /* built in */
                            undefined );    /* display in menu */

    var autostart_app = null;
    var autostart_uri = undefined;

    for( var idx = 0; idx < new_package_list.length; ++idx )
    {
        var app = new_package_list[ idx ];

        if( app.autostart )
        {
            if( autostart_app == null ||
                app.priority > autostart_app.priority )
            {
                autostart_app = app;
                autostart_uri = app.uri;
            }
        }
    }

    return autostart_uri;
}

function controller_package_manager_event_handler( event )
{
    switch( event.code )
    {
        case MOMPackageEvent.LIST_UPDATED:
        {
            debug.log( "HBBTV | list updated" );
            controller.update_running_apps( false );
            break;
        }

        case MOMPackageEvent.SERVICE_SELECTED:
        {
            debug.log( "HBBTV | service selected" );
            controller.update_running_apps( true );
            break;
        }

        case MOMPackageEvent.TERMINATE:
        {
            var uri = event.pkg.uri;
            debug.log( "HBBTV | got terminate event for " + uri );
            app = controller.find_app( controller, uri );
            if( app != undefined )
            {
                controller.terminate_app( uri, app );

                /*  Unless this was a native app terminating,
                    we don't need to call update_running_apps() here:
                    the package manager will send a LIST_UPDATED or
                    SERVICE_SELECTED event when it has finished
                    processing the updated package list.
                */
                if( event.reason == MOMPackageEvent.REASON_NATIVE_APP )
                {
                    controller.update_running_apps( false );
                }
            }
            else
            {
                debug.log( "HBBTV |  but it is not running!" );
            }
            break;
        }
    }
}

function controller_application_shown( event )
{
    debug.log( "HBBTV | z-order | controller_application_shown" );
    var idx = array_find( controller.app_zorder, event.target );
    if( idx >= 0 )
    {
        debug.log( "HBBTV | z-order |   promoting " + event.target.uri );
        delete controller.app_zorder[ idx ];
    }
    else
    {
        debug.log( "HBBTV | z-order |   pushing " + event.target.uri );
    }
    controller.app_zorder.push( event.target );
    if( controller.shown_hdlr != undefined )
    {
        controller.shown_hdlr();
    }
}

function controller_application_hidden( event )
{
    debug.log( "HBBTV | z-order | controller_application_hidden" );
    var idx = array_find( controller.app_zorder, event.target );
    if( idx >= 0 )
    {
        debug.log( "HBBTV | z-order |   removing " + event.target.uri );
        delete controller.app_zorder[ idx ];
    }
}

function controller_application_manager_event_handler( event )
{
    if( event.code == ApplicationEvent.TERMINATED &&
        event.app != controller )
    {
        debug.log( "HBBTV | got terminated event for " + event.app.uri );

        if( event.app.uri in controller.terminating )
        {
            delete controller.terminating[ event.app.uri ];
        }
        else
        {
            if( !document.ownerWafer.apis.browser.shuttingDown )
            {
                controller.update_running_apps( false );
            }
        }

        event.app.removeEventListener(  "ApplicationShown",
                                        controller_application_shown,
                                        false );
        event.app.removeEventListener(  "ApplicationHidden",
                                        controller_application_hidden,
                                        false );

        if( event.app == controller.portal_app )
        {
            debug.log( "HBBTV | portal terminated " + event.app.uri );

            if( controller.portal_stopped_hdlr != undefined )
            {
                controller.portal_stopped_hdlr();
            }

            controller.portal_selected = false;
            mom.localSystem.notifyEvent(
                MOMLocalSystem.NOTIFICATION_PORTAL_CLOSE,
                event.keyCode );

            controller.portal_app = undefined;
        }
    }
    else if(    event.code == ApplicationEvent.LOADING &&
                event.app != controller )
    {
        debug.log( "HBBTV | got loading event for " + event.app.uri );

        /*  add event listeners so that we can track the z-order of
            applications.
        */
        event.app.addEventListener( "ApplicationShown",
                                    controller_application_shown,
                                    false );
        event.app.addEventListener( "ApplicationHidden",
                                    controller_application_hidden,
                                    false );
    }
    else if(    event.code == ApplicationEvent.RUNNING &&
                event.app != controller )
    {
        debug.log( "HBBTV | got running event for " + event.app.uri );

        if( event.app.visible )
        {
            if( controller.hbbtv_visible )
            {
                /*  we may have missed the app making itself visible,
                    so make sure it is in the list.
                */
                debug.log( "HBBTV | z-order | app already visible " + event.app.uri );
                controller.app_zorder.push( event.app );
                if( controller.shown_hdlr != undefined )
                {
                    controller.shown_hdlr();
                }
            }
            else
            {
                /*  HBBTV apps should not be visible, so add this to the
                    list of active applications and hide it
                */
                debug.log( "HBBTV | z-order | hiding newly visible app " + event.app.uri );
                event.app.hide();
                event.app.deactivate();
                controller.app_reshow.push( event.app );
            }
        }
    }
}


var previously_required = false;
function controller_key_event_handler( event )
{
    var required = controller.key_required( event.keyCode );
    if(required != previously_required)
    {
        if(required)
        {
            /* autostart app is visible */
            if( controller.portal_started_hdlr != undefined )
            {
                controller.portal_started_hdlr();
            }
        }else
        {
            /* autostart app is quiescent */
            if( controller.portal_stopped_hdlr != undefined )
            {
                controller.portal_stopped_hdlr();
            }
        }
        previously_required = required;
    }

    if( !required )
    {
        mom.localSystem.notifyEvent(
                                mom_notifications[ event.type ],
                                event.keyCode );
    }
}

function controller_key_capture_handler( event )
{
    var rv = true;

    if( event.type == "keydown" )
    {
        switch( event.keyCode )
        {
            case KeyboardEvent.VK_TELETEXT:
            {
                debug.log( "HBBTV | got teletext button" );
                controller.toggle_teletext_app();
                event.stopPropagation = true;
                event.preventDefault = true;
                rv = false;
                break;
            }

            case KeyboardEvent.ANT_VK_BROWSER_HOME:
            case KeyboardEvent.VK_HOME:
            case layers_key_PORTAL:
            {
                debug.log( "HBBTV | got portal button" );
                controller.portal_selected = true;
                controller.launch_portal();
                event.stopPropagation = true;
                event.preventDefault = true;
                rv = false;
                mom.localSystem.notifyEvent(
                                MOMLocalSystem.NOTIFICATION_PORTAL_OPEN,
                                event.keyCode );
                break;
            }

            case layers_key_TV:
            {
                debug.log( "HBBTV | got TV button" );
                var autostart_uri = controller.get_autostart_uri();
                if( controller.terminate_all_but_autostart( controller,
                                                            autostart_uri ) )
                {
                    controller.update_running_apps( true );
                }
                else
                {
                    mom.localSystem.notifyEvent(
                                MOMLocalSystem.NOTIFICATION_KEY_DOWN,
                                event.keyCode );
                }
                event.stopPropagation = true;
                event.preventDefault = true;
                rv = false;
                break;
            }
        }
    }
    else if( event.keyCode == layers_key_TV )
    {
        /* always pass on layers_key_TV notifications */
        mom.localSystem.notifyEvent(    mom_notifications[ event.type ],
                                        event.keyCode );
        event.stopPropagation = true;
        event.preventDefault = true;
        rv = false;
    }

    return rv;
}

function controller_register_shown_handler( hdlr )
{
    debug.log( "HBBTV | shown handler registered" );
    controller.shown_hdlr = hdlr;
}

function controller_register_portal_notification_handlers(
                                                    started_hdlr,
                                                    stopped_hdlr )
{
    debug.log( "HBBTV | portal notification handlers registered" );
    controller.portal_started_hdlr = started_hdlr;
    controller.portal_stopped_hdlr = stopped_hdlr;
}

function controller_reshow_current()
{
    debug.log( "HBBTV | reshow hbbtv apps" );
    controller.hbbtv_visible = true;
    controller.reshow_current();
}

function controller_hide_current()
{
    debug.log( "HBBTV | hide hbbtv apps" );
    controller.hbbtv_visible = false;
    controller.app_reshow = [];
    controller.hide_current();
}


function controller_launch_portal()
{
    debug.log( "HBBTV | got launch_portal() request" );
    controller.launch_portal();
}

function initialiseHController()
{
    debug.log( "HBBTV | initialising controller" );

    controller.terminating = {};

    mom.packageManager.addEventListener(
                                "MOM_Package",
                                controller_package_manager_event_handler,
                                false );
    applicationManager.addEventListener(
                                "ApplicationEvent",
                                controller_application_manager_event_handler,
                                false );
    document.addEventListener(  "keydown",
                                controller_key_event_handler,
                                false );
    document.addEventListener(  "keypress",
                                controller_key_event_handler,
                                false );
    document.addEventListener(  "keyup",
                                controller_key_event_handler,
                                false );

    root_document.addEventListener( "keydown",
                                    controller_key_capture_handler,
                                    true );
    root_document.addEventListener( "keypress",
                                    controller_key_capture_handler,
                                    true );
    root_document.addEventListener( "keyup",
                                    controller_key_capture_handler,
                                    true );

    controller.update_running_apps( false );

    /* set up integration API */
    document.ownerWafer.apis.register_shown_handler = controller_register_shown_handler;
    document.ownerWafer.apis.register_portal_notification_handlers =
                                controller_register_portal_notification_handlers;
    document.ownerWafer.apis.reshow_current = controller_reshow_current;
    document.ownerWafer.apis.hide_current = controller_hide_current;
    document.ownerWafer.apis.launch_portal = controller_launch_portal;

    /*  Read the config to see if we have a terminal specific app to
        launch
    */
    var config = document.ownerWafer.apis.config;
    var terminal_config = config.get( "mom.hbbtv.terminal_app" );
    var terminal_uri = terminal_config.value;
    if( terminal_uri.length > 0 )
    {
        debug.log( "HBBTV |  launching terminal specific app " + terminal_uri );

        /* launch terminal specific app */
        var terminal_app = document.ownerWafer.createWafer( terminal_uri );
        document.ownerWafer.appendChild( terminal_app );
    }
}

function finaliseHController()
{
    debug.log( "HBBTV | finalising controller" );

    mom.packageManager.removeEventListener(
                                    "MOM_Package",
                                    controller_package_manager_event_handler,
                                    false );
    applicationManager.removeEventListener(
                                    "ApplicationEvent",
                                    controller_application_manager_event_handler,
                                    false );
    document.removeEventListener(   "keydown",
                                    controller_key_event_handler,
                                    false );
    document.removeEventListener(   "keypress",
                                    controller_key_event_handler,
                                    false );
    document.removeEventListener(   "keyup",
                                    controller_key_event_handler,
                                    false );

    root_document.removeEventListener(  "keydown",
                                        controller_key_capture_handler,
                                        true );
    root_document.removeEventListener(  "keypress",
                                        controller_key_capture_handler,
                                        true );
    root_document.removeEventListener(  "keyup",
                                        controller_key_capture_handler,
                                        true );
}
