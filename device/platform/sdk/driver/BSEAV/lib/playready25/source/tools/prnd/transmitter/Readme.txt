# @@@+++@@@@******************************************************************
# **
# ** Microsoft (r) PlayReady (r)
# ** Copyright (c) Microsoft Corporation. All rights reserved.
# **
# @@@+++@@@@******************************************************************

How to setup PRNDTransmitter?

i)   Create a local folder e.g. c:\Transmitter

ii)  Copy the following files to c:\transmitter
    a)  %PK_INSTALL%\Samples\bgroupcert.dat
    b)  %PK_INSTALL%\Samples\zgpriv.dat
    c)  %PK_INSTALL%\Samples\revpackage.xml
    d)  %PK_INSTALL%\Samples\PlayReady_UPnP_TransmitterServiceDescription.xml (IF you are using the binary included in the MSI and want UPnP enabled).
    e)  %PK_INSTALL%\Source\Tools\prnd\PRNDTransmitter.xml (Modify this as below to pick up your content).

iii) Launch PRNDTransmitter.exe

------------------------
Can I stream Clear content using test transmitter?

Yes, don't send the session ID as part of HTTP request. So that, transmitter interprets request as clear content request and stream it.

--------------------------------
How can I choose which file to stream?

i) Edit PRNDTransmitter.xml in the local folder.

There are two ways to specify content:
    a)  VRoots
        VRoots operate as webserver vroots do. Streaming requests will look under each path for files requested.
        
        <VRoots>
            <VRoot>\\bar-server\Media\Video</VRoot>
            <VRoot>\\foo-server\Media\Video2</VRoot>
        </VRoots>
        
    b)  Channels
        Channels allow specifying a set of files as one stream or reading from an HTTP stream, encrypting it on the fly and retransmitting it.
        
        <Channels>
            <Channel Name="StitchedFile" Type="TSFile">
                <TSFile>\\bar-server\http\media\TS\stitch0.ts</TSFile>
                <TSFile>\\bar-server\http\media\TS\stitch1.ts</TSFile>
            </Channel>
            <Channel Name="LocalFile" Type="TSFile">
                <TSFile>C:\SavedFile.ts</TSFile>
            </Channel>
            <Channel Name="StreamingChannel1" Type="Http">http://bar-server/TSStreaming/1</Channel>
        </Channels>


ii) From the receiver application, make an HTTP streaming request, as usual.

e.g. if you want to stream a file such as \\bar-server\Media\Video\MyVideo.ts

your HTTP request should be http://xx.xxx.xxx.xxx:15000/MyVideo.ts

---------------------------------------------

How can I troubleshoot, if there are any issues?

Look at transmitter.log in the local folder.

----------------------------------


