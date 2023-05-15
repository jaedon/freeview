'*@@@+++@@@@******************************************************************
'
' Microsoft (r) PlayReady (r)
' Copyright (c) Microsoft Corporation. All rights reserved.
'
' IMPORTANT:  THESE SAMPLES ARE PROVIDED FOR HIGH-LEVEL REFERENCE ONLY
'             AND SHOULD NOT BE USED IN PRODUCTION ENVIRONMENTS.
'
'*@@@---@@@@******************************************************************
Option explicit 

const RightTypeCounted = 1
const RightTypeExpire  = 2
const RightTypeRelativeExpire = 3


'********************************************************'
''''''''''''''SET THESE VALUES''''''''''''''''''''''''''''

const ContentPubKey = "jsmKgeWLEXdzM4XqL1J1uacu4TjPDTBo1CrK!U!1elg6V2gYb0tXNw==" 'NOTE:  You must place your content PUBKEY here
const KeySeed      = "pLtgG421X5SdG2wolkcNr7SB1Ot6A0A2uDQUZF7E" 'NOTE:  You should add you keyseed here

Dim g_RightsType
   
' Choose which right to issue
'g_RightsType = RightTypeExpire
g_RightsType = RightTypeCounted
'g_RightsType = RightTypeRelativeExpire

''''''''''''''END SET VALUES'''''''''''''''''''''''''''''''
'*********************************************************'

Function CreateRightsString( RightsType )
    '"""""""""""""""""""""""""""""""
    'Rights object to generate rights
    '"""""""""""""""""""""""""""""""
    Dim objRights
    Set objRights = CreateObject("wmrmobjs.wmrmRights")
    Dim SetV2Rights
               
    With objRights
        .MinimumAppSecurity = 150
        .AllowPlayOnPC = true
        .AllowBackupRestore = true
        
	select case RightsType
        case RightTypeCounted .Playcount = 5
        case RightTypeExpire 
		.ExpirationDate = "#20041125Z#"
		.DisableOnClockRollback = true
        case RightTypeRelativeExpire .ExpirationAfterFirstUse = 72
        end select
        
        CreateRightsString = .GetAllRights
    End with 
    Set objRights = Nothing
End Function


'To get a license, client provides information to verify it has the right to one
Function CreateLicense( Kid, KEY, ClientInfo, PubKey, RightsType )
       
    '"""""""""""""""""""""""""""""""""
    'Generate a license with the License object
    '"""""""""""""""""""""""""""""""""
    Dim objLicense
    Set objLicense = CreateObject("wmrmobjs.wmrmlicgen")
    Dim varLicense

    
    With objLicense
        .KeyID = Kid ' from Key object 
        .SetKey "", KEY ' from Key object
        .Rights = CreateRightsString( RightsType )
        .ClientInfo = ClientInfo ' from challenge object
        .BindToPubKey = PubKey ' This is required now.        
        CreateLicense = .GetLicenseToDeliver() 
    End with
    Set objLicense = Nothing    
End Function
   
Sub DoWork   
    Dim objChallenge
    Dim objHeader
    Dim objKey
    Dim objLicense
    Dim objResponse
    Dim strLicenseRequested
    Dim varHeader
    Dim objArgs
    Dim objFSO
    Dim objFile

    Set objArgs = WScript.Arguments
    
    if objArgs.Length <> 2 then
        WScript.Echo "Usage:  genresponse.vbs challengefile responsefile"
        exit sub
    end if
        
    Set objFSO = CreateObject("Scripting.FileSystemObject")
    Set objFile = objFSO.OpenTextFile(objArgs(0),1, 0)
    strLicenseRequested = objFile.ReadAll
    objFile.Close
    
    
    Set objChallenge = CreateObject("wmrmobjs.wmrmchallenge")
    Set objHeader = CreateObject("wmrmobjs.wmrmheader")
    Set objKey = CreateObject("wmrmobjs.wmrmkeys")
    set objResponse = CreateObject("wmrmobjs.wmrmresponse")
    
    'strLicenseRequested = Right(strLicenseRequested, Len( strLicenseRequested ) - 10 )
            
    'Set the challenge
    objChallenge.Challenge = strLicenseRequested

    'Put the header into the Header object to get the key ID information
    objHeader.Header = objChallenge.Header

    'Verify the header with the packaging server's public key
    'You can get the public key by e-mail or from a database, but
    'this sample uses an Application variable to collect this information
    
    if ( 0 = objHeader.Verify(ContentPubKey) ) then 'Public key information from a database
        WScript.Echo "Header verification failed"
        Exit Sub
    End If

      
    'Put the license key seed and key ID into the Key object
    objKey.KeyID = objHeader.KeyID
    objKey.Seed = KeySeed 

    Call objResponse.AddLicense("2.0.0.0", CreateLicense( objHeader.KeyID, objKey.GenerateKey, objChallenge.ClientInfo, ContentPubKey, g_RightsType ) )

    Set objFile = objFSO.OpenTextFile(objArgs(1),2,-1)                    
    objFile.Write objResponse.GetLicenseResponse()
    objFile.Close

'Clear objects

Set objChallenge = Nothing
Set objHeader = Nothing
Set objKey = Nothing
Set objResponse= Nothing
End Sub

' Main code
DoWork
