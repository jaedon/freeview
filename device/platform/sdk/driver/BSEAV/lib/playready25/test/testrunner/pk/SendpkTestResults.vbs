' @@@+++@@@@******************************************************************
' **
' ** Microsoft (r) PlayReady (r)
' ** Copyright (c) Microsoft Corporation. All rights reserved.
' **
' @@@+++@@@@******************************************************************

' this script gathers the results of tests run by runPKTests.vbs
' settings can  be configured in testconfig.config or in your own config file passed with -config:somefile
' see SendpkTestResults.vbs -? for more on commandline options.

option explicit

''''''''''''''''''''''''' Set Defaults '''''''''''''''''''''''''''''''''''''''''''''''''''''

' Path Expansion supported for defaults:
' $PKPath      Expands to the runtime value for the PK Path
' $TestPath    Expands to the runtime value for the test path
' $ResultPath  Expands to the runtime value for the result path
' $DATE        Expands to the current date
' $BRANCH      Expands the branch name
' $BUILD       Expands the build number
' $FLAVOR      Expands the flavor (chk, fre, perf, ... )
' $ARCH        Expands the architecture (x86, ARM, ... )
'
' Expansion is done once, just after launch and is not recursive.
' The value for PKPath set in the defaults or on the commandline may not contain TestPath or ResultPath
' for expansion. The test path cannot contain the ResultPath.

' This is the Porting Kit location
const constDefaultPKPath = "C:\PlayReady\$BRANCH_$BUILD\"

const constDefaultTestPath = "$PKPath\test\"
const constDefaultSamplesPath = "$PKPath\Samples\"
const constDefaultToolsPath = "$PKPath\Tools\"
const constDefaultResultPath = "$TestPath\TestResults\$DATE\"


'Change "bind" to "bin" if you want to test free bits
const constDefaultBinPath =  "$TestPath\bind\i386\"

const constTestAreas = "$TestPath\TestRunner\pk\PKTestAreas.txt"

'style information
const constXSLTSource = "$TestPath\TestRunner\common\result-transform.xsl"
const constCSSSource = "$TestPath\TestRunner\common\result-style.css"

'Default Branch Name
const constDefaultBranch = "Device_PK"
const constDefaultBuildNo = "2.5.0"
const constDefaultFlavor = "chk"
const constDefaultArch = "x86"


const constTestPlatform = ".pk." ' PC is ".pc.", CEPC is ".ce."


' Email Config
' Need to replace with actual email address
Const EMAILTO   = "some_address@domain.com"
Const EMAILFROM = "some_address@domain.com"
const constMailServer = "yourserver.yourdomain.com"

'what goes after the branch for the subject line/title
const EMAILTITLE = "$BRANCH $BUILD $ARCH $FLAVOR Full Test Result: $DATE"



'=========================No customization after this point========================

dim g_strResultPath, g_strResultURL, g_strTestPath, g_strPKPath, g_strBranch, g_strFlavor, g_strBuildNo, g_strArch
dim g_strConfigFile, g_strEmailTo, g_strEmailFrom, g_strEmailServer, g_strRoot
dim msgBody, msgSubject, fnTestResult, fPass, fEmail, strReport, g_fResultError

dim g_cTotalCases, g_cTotalPasses, g_cTotalFail, g_cTotalMissed, g_cTotalDisabled
dim g_cBVTCases, g_cBVTPasses, g_cBVTFail, g_cBVTMissed, g_cBVTDisabled

'==== Strings to hold the list of disabled and not run tests and files  ===='
dim g_strDisabledTests, fnDisabledTests
dim g_strNotRunTests, fnNotRunTests

const constBVTEndsHere = "BVT ends here"
const constFuncEndsHere = "FUNC ends here"
const constDRTEndsHere = "DRT ends here"
const constEmailFlag = "-email"
const constTestRootFlag = "-testroot:"
const constResultRootFlag = "-resultroot:"
const constResultUrlFlag = "-resulturl:"
const constBranchFlag = "-branch:"
const constPKRootFlag = "-pkroot:"
const constEmailToFlag = "-to:"
const constEmailFromFlag = "-from:"
const constEmailServerFlag = "-mailserver:"
const constHelpFlag = "-?"
const constFlavorFlag = "-flav:"
const constBuildNumberFlag = "-buildNo:"
const constArchFlag = "-arch:"


const constRunAll = 31
const constRunBVT = 1
const constRunFunc = 2
const constRunDRT = 4
const constRunPerf = 8
const constRunNone = 0
const constRunFuzz = 16

' Check cmdline parameter. 
' If there is "-email" then  send email.
' If there is "-branch:" then we change the name of the branch in the headings
' If there is "-flav:"	then we set the name of the Flavor in the headings
' IF there is "-buildNo:" then we set the build number in the headings
' If there is "-arch:" then wee set the architecture in the headings
' If there is "-pkroot:" then we redirect the default root path for the porting kits
' If there is "-testroot:" then we pick up that path
' If there is "-resultroot:" then we pick up that path as the result location
' If there is "-resulturl:" then we pick up that path as the result URL for the email
' If there is "-to:"        then we override the EMAILTO setting from the config file
' If there is "-from:"    then wee override the EMAIL From setting from the config file

fEmail = false
ParseCommandLineParameter fEmail

CheckDefault g_strBranch , constDefaultBranch
CheckDefault g_strBuildNo , constDefaultBuildNo
CheckDefault g_strFlavor , constDefaultFlavor
CheckDefault g_strArch , constDefaultArch

' This is the source code location
CheckPath g_strPKPath, constDefaultPKPath
CheckPath g_strTestPath, constDefaultTestPath
CheckPath g_strResultPath, constDefaultResultPath
CheckPath g_strResultURL, g_strResultPath

CheckDefault g_strEmailFrom, EMAILFROM
CheckDefault g_strEmailTo, EMAILTO
CheckDefault g_strEmailServer, constMailServer

g_cBVTCases = 0
g_cBVTPasses = 0
g_cBVTFail = 0
g_cBVTMissed = 0
g_cBVTDisabled = 0
g_cTotalCases = 0
g_cTotalPasses = 0
g_cTotalFail = 0
g_cTotalMissed = 0
g_cTotalDisabled = 0
g_fResultError = false

g_strDisabledTests = ""
g_strNotRunTests = ""

dim sobjFS
Set sobjFS = CreateObject("Scripting.FileSystemObject")

'
' Main Program
'
msgSubject = ExpandDefaults(EMAILTITLE)
fnTestResult = "\TestResultSummary.htm"
fnDisabledTests = "\disabled_tests.txt"
fnNotRunTests = "\notrun_tests.txt"
    
BuildReport g_strResultPath, msgSubject, fPass, msgBody, strReport

If g_cBVTFail > 0 or g_cBVTMissed > 0 or g_fResultError Then
    msgSubject = msgSubject & " (FAIL)"
ElseIf fPass then
    msgSubject = msgSubject & " (PASS)"
else
    msgSubject = msgSubject & " (WARNING)"
end if

if fEmail then 
    SendEmail g_strEmailFrom, g_strEmailTo, msgSubject, msgBody
end if

' Save a copy to the BVT result folder
SaveFile g_strResultPath & fnTestResult, strReport
SaveFile g_strResultPath & fnDisabledTests, g_strDisabledTests
SaveFile g_strResultPath & fnNotRunTests, g_strNotRunTests

'
' Main Program ends
'
If g_cBVTFail > 0 then
    wscript.quit(6)
elseif g_fResultError Then
    wscript.quit(7)
ElseIf g_cTotalFail > 0 Then
    wscript.quit(4)
elseif g_cBVTMissed > 0 Then
    wscript.quit(5)
ElseIf g_cTotalMissed > 0 Then
    wscript.quit(3)
ElseIf g_cTotalPasses < 1 Then
    wscript.quit(2)
ElseIf fPass = False Then
    wscript.quit(1)
End If

wscript.quit(0)

' Parse commandline parameters
' -bvt only run bvt
' -func only run functionals
' -email    send email
' -testroot: path for test directory
' -pkroot:   path for pk root directory
' -resultroot:  path for results
' -resulturl:  path for results
' -branch:  the name of the branch
' -config:   overrides the default config file
' -to:         overides the default email to address
' -from:     overides the default email from address
sub ParseCommandLineParameter(byRef fEmail)
    dim i
    for i = 0 to wscript.arguments.length - 1
        if StrComp(wscript.arguments(i), constHelpFlag, 1) = 0 then
            wscript.echo "SendPKTestResults.vbs gathers the test results into a summary and optionally emails the summary" _
                & vbNewLine & "options:" _
                & vbNewLine & vbtab & " -?" & vbTab & "this help" _
                & vbNewLine & vbtab & " -email" & vbTab & "email results" _
                & vbNewLine & vbtab & " -pkroot:somepath" & vbTab & "set the root directory of the pk install" _
                & vbNewLine & vbtab & " -TestRoot:somepath" & vbTab & "set the directory for the test sources" _
                & vbNewLine & vbtab & " -ResultRoot:somepath" & vbTab & "set the location to send the result logs" _
                & vbNewLine & vbtab & " -ResultURL:somepath" & vbTab & "set the URL for the result logs" _
                & vbNewLine & vbtab & " -branch:name" & vbTab & "override the default PK branch name" _
                & vbNewLine & vbtab & " -buildNo:somestring" & vbTab & "override the default build number" _
                & vbNewLine & vbtab & " -arch:name" & vbTab & "override the default architecture name" _
                & vbNewLine & vbtab & " -flav:name" & vbTab & "override the default flavor name" _
                & vbNewLine & vbtab & " -to:somename" & vbTab & "overides the default email to address" _
                & vbNewLine & vbtab & " -from:somename" & vbTab & "overides the default email from address" _
                & vbNewLine & vbtab & " -mailserver:myserver" & vbTab & "overides the email sending server" _
                & vbNewLine & vbNewLine & "The file testconfig.config is a VBScript file" _
                & "that defines default constants. If you have any customization you can make it in that file or" _
                & " create a copy of that file and customize the copy and pass it in with the -config: flag" _
                & vbNewLine & vbNewLine
            wscript.quit
        elseif StrComp(wscript.arguments(i), constEmailFlag, 1) = 0 then 
            fEmail = true
            DebugPrint "Command Line Arg Email = true "    
        elseif StrComp(left(wscript.arguments(i), len(constTestRootFlag)), constTestRootFlag, 1) = 0 then 
            g_strTestPath = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constTestRootFlag))
            DebugPrint "Command Line Arg TestRoot = " & g_strTestPath
        elseif StrComp(left(wscript.arguments(i), len(constResultRootFlag)), constResultRootFlag, 1) = 0 then 
            g_strResultPath = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constResultRootFlag))
            DebugPrint "Command Line Arg ResultRoot = " & g_strResultPath
        elseif StrComp(left(wscript.arguments(i), len(constResultURLFlag)), constResultURLFlag, 1) = 0 then 
            g_strResultURL = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constResultURLFlag))
            DebugPrint "Command Line Arg ResultURL = " & g_strResultURL
        elseif StrComp(left(wscript.arguments(i), len(constBranchFlag)), constBranchFlag, 1) = 0 then 
            g_strBranch = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constBranchFlag))
            DebugPrint "Command Line Arg branch = " & g_strBranch
        elseif StrComp(left(wscript.arguments(i), len(constFlavorFlag )), constFlavorFlag , 1) = 0 then 
            g_strFlavor = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constFlavorFlag ))
            DebugPrint "Command Line Arg flavor = " & g_strFlavor
        elseif StrComp(left(wscript.arguments(i), len(constArchFlag)), constArchFlag, 1) = 0 then 
            g_strArch = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constArchFlag))
            DebugPrint "Command Line Arg arch = " & g_strArch
        elseif StrComp(left(wscript.arguments(i), len(constBuildNumberFlag)), constBuildNumberFlag, 1) = 0 then 
            g_strBuildNo = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constBuildNumberFlag))
            DebugPrint "Command Line Arg branch = " & g_strBuildNo
        elseif StrComp(left(wscript.arguments(i), len(constPKRootFlag)), constPKRootFlag, 1) = 0 then 
            g_strPKPath= Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constPKRootFlag))
            DebugPrint "Command Line Arg pkPath = " & g_strPKPath
        elseif StrComp(left(wscript.arguments(i), len(constEmailFromFlag)), constEmailFromFlag, 1) = 0 then 
            g_strEmailFrom= Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constEmailFromFlag))
            DebugPrint "Command Line Arg mailfrom = " & g_strEmailFrom
        elseif StrComp(left(wscript.arguments(i), len(constEmailServerFlag)), constEmailServerFlag, 1) = 0 then 
            g_strEmailServer = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constEmailServerFlag))
            DebugPrint "Command Line Arg mailserver = " & g_strEmailServer
        elseif StrComp(left(wscript.arguments(i), len(constEmailToFlag)), constEmailToFlag, 1) = 0 then 
            g_strEmailTo= Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constEmailToFlag))
            DebugPrint "Command Line Arg mailto = " & g_strEmailTo
        end if
    next
end sub

' return today's date in YYYY-MM-DD format
Function StrToday()
    Dim dtmToday, strMonth, strDay

    dtmToday = Date

    strMonth = Month(dtmToday)
    If Len(strMonth) = 1 Then strMonth = "0" & strMonth

    strDay = Day(dtmToday)
    If Len(strDay) = 1 Then strDay = "0" & strDay

    StrToday = Year(dtmToday) & "-" & strMonth & "-" & strDay
End Function ' StrToday

'  Replaces the following flags in paths:
'   $PKPath        : value of g_strPKPath
'   $TestPath      : value of g_strTestPath
'   $ResultPath    : value of g_strResultPath
'   $DATE          : current Date string
'   $BRANCH        : name of the branch
'   $BUILD         : Expands the build number
'   $FLAVOR        : Expands the flavor (chk, fre, perf, ... )
'   $ARCH          : Expands the architecture (x86, ARM, ... )
function ExpandDefaults(  f_strPath)
    dim strResult
    strResult = replace( f_strPath, "$ResultPath", g_strResultPath )
    strResult = replace( strResult, "$TestPath", g_strTestPath )
    strResult = replace( strResult, "$PKPath", g_strPKPath )
    strResult = replace( strResult, "$DATE", StrToday() )
    strResult = replace( strResult, "$BRANCH", g_strBranch )
    strResult = replace( strResult, "$BUILD", g_strBuildNo )
    strResult = replace( strResult, "$FLAVOR", g_strFlavor )
    strResult = replace( strResult, "$ARCH", g_strArch )
    ExpandDefaults = strResult
end function

' checks to see if f_strPath is empty, if yes expands f_strDefault and set f_strPath to it
' also checks and ensures that there is a trailing "\"
sub CheckPath( byref f_strPath, f_strDefault )
    CheckDefault  f_strPath, f_strDefault 
    If Right(f_strPath, 1) <> "\" Then
        f_strPath = f_strPath & "\"
    End If
    f_strPath = RegExReplace(f_strPath, "(.+)(\\\\)", "$1\")
end sub

sub CheckDefault( byref f_strIn, f_strDefault)
    if f_strIn = "" then
        f_strIn = f_strDefault
    end if
    f_strIn = ExpandDefaults( f_strIn )
end sub

' mail it
sub SendEmail(sFrom, sTo, sTitle, sBody)
    dim cdoMessage, cdoConfig, try_count

    try_count = 0
    
    DebugPrint "Sending mail from = " & sFrom & " to = " & sTo
    set cdoMessage = CreateObject("cdo.message")
    set cdoConfig  = CreateObject("cdo.Configuration")
    cdoMessage.Configuration = cdoConfig
    
    cdoConfig.Fields("http://schemas.microsoft.com/cdo/configuration/sendusing") = 2 'cdoSendUsingPort
    cdoConfig.Fields("http://schemas.microsoft.com/cdo/configuration/smtpserver") = g_strEmailServer
    cdoConfig.Fields("http://schemas.microsoft.com/cdo/configuration/smtpconnectiontimeout") = 60
    cdoConfig.Fields("http://schemas.microsoft.com/cdo/configuration/smtpauthenticate") = 2
    cdoConfig.Fields.Update
      
    with cdoMessage
        .from = sFrom
        .to = sTo
        .subject = sTitle
        .htmlbody = sBody   ' send html mail	
        if InStr( sTitle, "(PASS)" ) <> 0 then
            .Fields("urn:schemas:mailheader:X-Priority") = 0 
            .Fields("urn:schemas:httpmail:importance") = 1
        else
            .Fields("urn:schemas:mailheader:X-Priority") = 1 
            .Fields("urn:schemas:httpmail:importance") = 2
        end if
        .Fields.Update
    end with

    on error resume next
    do until try_count > 5
        cdoMessage.send
        if Err.Number <> 0 then
            wscript.echo "Caught error:" & Err.Number & vbnewline & Err.Description & vbnewline & " retrying"
            try_count = try_count + 1
            Err.Clear
        else
            exit do
        end if
    loop
    on error goto 0

    set cdoMessage = nothing
    set cdoConfig  = nothing
    
end sub

sub SaveFile(strFileName, strFileContent)
    dim sTextStream
    set sTextStream = sobjFS.CreateTextFile(strFileName, true)
    sTextStream.write strFileContent
end sub

sub BuildReport(sResultFolder, msgSubject, byRef fPass, byref strMessageBody, byref strFileText)
'	On Error Resume Next
	dim sobjDictionaryTestAreaData, folderMachine
    
    'Contains test area name and test case count
    set sobjDictionaryTestAreaData = CreateObject("Scripting.Dictionary")

    fPass = true
    GetTestAreaDictionary sobjDictionaryTestAreaData, ExpandDefaults(constTestAreas)
    
    ' Collect all test results for all test machines
    DebugPrint "Reading results folder = " & sResultFolder
    
    if not AddResultsToDictionary(sobjFS.GetFolder(sResultFolder), sobjDictionaryTestAreaData) then
        fPass = false
    end if

    
    ' Build the message body in html
    if fEmail = true then
        strMessageBody = BuildHTML( true, sobjDictionaryTestAreaData, msgSubject)
    end if
    strFileText = BuildHTML( false, sobjDictionaryTestAreaData, msgSubject)

    If Err.Number <> 0 Then
    	fPass = false
    	strMessageBody = "<h3>Error #" & Err.Number & " in result processing, this could indicate an incomplete XML file due to a test module crash</h3><br>" & Err.Description & "<hr>" & strMessageBody
    	strFileText = "<h3>Error #" & Err.Number & " in result processing, this could indicate an incomplete XML file due to a test module crash</h3><br>" & Err.Description & "<hr>" & strFileText
    	Err.Clear
    End If
    on error goto 0
end sub

function IsTestResultFile(strResultFile, byRef strArea)
    dim fGood, sFileName
    fGood = false
    
    'file should have .xml extension
    if strcomp(Right(strResultFile, 4), ".xml", 1) = 0 then
        sFileName = left(strResultFile, len(strResultFile) - 4)
        strArea = sFileName
        fGood = true
    end if

    IsTestResultFile = fGood
end function

function AddResultsToDictionary( folder, byRef sobjDictionaryTestAreaData )
    dim sobjResultFiles, sResultFile, objTestArea, fError, fAreaPass

    fAreaPass = true
    set sobjResultFiles = folder.Files
    
    for each sResultFile in sobjResultFiles
        dim strArea
        if IsTestResultFile(sResultFile.Name, strArea) then

            set objTestArea = sobjDictionaryTestAreaData.Item(strArea)
            objTestArea.ResultFile = sResultFile.Name

            DebugPrint vbTab & "TotalCases    = " & objTestArea.ExpectedCount( constRunAll )
            DebugPrint vbTab & "TotalPasses   = " & objTestArea.PassedCount( constRunAll )
            DebugPrint vbTab & "TotalFails    = " & objTestArea.FailedCount( constRunAll )
            DebugPrint vbTab & "TotalMissed   = " & objTestArea.MissedCount( constRunAll )
            DebugPrint vbTab & "TotalDisabled = " & objTestArea.DisabledCount( constRunAll )

            g_cTotalCases = g_cTotalCases + objTestArea.ExpectedCount( constRunAll )
            g_cTotalPasses = g_cTotalPasses + objTestArea.PassedCount( constRunAll )
            g_cTotalFail = g_cTotalFail + objTestArea.FailedCount( constRunAll )
            g_cTotalMissed= g_cTotalMissed + objTestArea.MissedCount( constRunAll )
            g_cTotalDisabled= g_cTotalDisabled + objTestArea.DisabledCount( constRunAll )
            g_cBVTCases = g_cBVTCases + objTestArea.ExpectedCount( constRunDRT + constRunBVT )
            g_cBVTPasses = g_cBVTPasses + objTestArea.PassedCount( constRunDRT + constRunBVT )
            g_cBVTFail = g_cBVTFail + objTestArea.FailedCount( constRunDRT + constRunBVT )
            g_cBVTMissed= g_cBVTMissed + objTestArea.MissedCount( constRunDRT + constRunBVT )
            g_cBVTDisabled = g_cBVTDisabled + objTestArea.DisabledCount( constRunDRT + constRunBVT )
            if objTestArea.ParseError then
                g_fResultError = true
            end if
            if not objTestArea.AreaPassed then
                fAreaPass = false
            end if
        end if
    next
    AddResultsToDictionary = fAreaPass
end function

' read the testareas.txt file for all expected test areas and calculate test case counts
sub GetTestAreaDictionary(byRef sobjTestAreaData, szFileName)
    dim fTestAreas
    
    DebugPrint "Opening file " & szFileName
    Set fTestAreas = sobjFS.OpenTextFile(szFileName)
    
    sobjTestAreaData.RemoveAll
    do until fTestAreas.AtEndOfStream
        dim strLine 
        strLine = Trim(fTestAreas.ReadLine)
        if strLine <> ""  and left(strLine, 1) <> "#" then
            dim TestAreaData, objTestArea
            TestAreaData = Split(strLine)
            set objTestArea = new TestAreaResults
            objTestArea.AreaName = TestAreaData(0)
            sobjTestAreaData.Add TestAreaData(0), objTestArea
        end if
    loop
    fTestAreas.close
end sub

function GetStyleInformation()
    dim strCss, CSSFile
    set CSSFile = sobjFS.OpenTextFile( ExpandDefaults(constCSSSource), 1 )
    strCss = "<style>" & vbNewLine _
           & CSSFile.ReadAll _
           & "</style>" & vbNewLine
    CSSFile.Close
    GetStyleInformation = strCss
end function

function BuildHTML(f_fEmail, sobjDictionaryTestAreaData, msgSubject)
    dim strHTML,  sTestDetail, i, j, objTestArea
        
    strHTML = _
        "<html>" & vbNewLine & vbTab _
            & "<head>" & vbNewLine & vbTab _
                & "<title>" & msgSubject & "</title>" & vbNewLine & vbTab _
                & GetStyleInformation() & vbTab _
            & "</head>" & vbNewLine _ 
            & "<body>"

    'page heading
    strHTML = strHTML & vbNewLine & "<h1>" & msgSubject & "</h1>" _
        & "<table cellspacing=""0"">" _
        & vbNewLine & vbTab & "<tr class=""header"">" _
        & vbNewLine & vbTab & vbTab & "<td></td>" _
        & vbNewLine & vbTab & vbTab & "<td>BVT Results</td>" _
        & vbNewLine & vbTab & vbTab & "<td>Total Results</td>" _
        & vbNewLine & vbTab & "</tr>" _
        & vbNewLine & vbTab & "<tr>" _
        & vbNewLine & "<td class=""total_missed"">Missed</td>" _
        & vbNewLine & "<td class=""total_missed"">" & g_cBVTMissed & "</td>" _
        & vbNewLine & "<td class=""total_missed"">" & g_cTotalMissed & "</td>" _
        & vbNewLine & vbTab & "</tr>" _
        & vbNewLine & vbTab & "<tr>" _
        & vbNewLine & "<td class=""total_failed"">Failed</td>" _
        & vbNewLine & "<td class=""total_failed"">" & g_cBVTFail & "</td>" _
        & vbNewLine & "<td class=""total_failed"">" & g_cTotalFail & "</td>" _
        & vbNewLine & vbTab & "</tr>" _
        & vbNewLine & vbTab & "<tr>" _
        & vbNewLine & "<td class=""total_passed"">Passed</td>" _
        & vbNewLine & "<td class=""total_passed"">" & g_cBVTPasses & "</td>" _
        & vbNewLine & "<td class=""total_passed"">" & g_cTotalPasses & "</td>" _
        & vbNewLine & vbTab & "</tr>" _
        & vbNewLine & vbTab & "<tr>" _
        & vbNewLine & "<td class=""total_cases"">Expected</td>" _
        & vbNewLine & "<td class=""total_cases"">" & g_cBVTCases & "</td>" _
        & vbNewLine & "<td class=""total_cases"">" & g_cTotalCases & "</td>" _
        & vbNewLine & vbTab & "</tr>" _
        & vbNewLine & vbTab & "<tr>" _
        & vbNewLine & "<td class=""total_disabled"">Disabled</td>" _
        & vbNewLine & "<td class=""total_disabled"">" & g_cBVTDisabled & "</td>" _
        & vbNewLine & "<td class=""total_disabled"">" & g_cTotalDisabled & "</td>" _
        & vbNewLine & vbTab & "</tr>" _
        & vbNewLine & "</table>" _
        & vbNewLine & "<h2>Breakdown</h2>" & vbNewLine

    ' table header, again nice HTML means pretty verbose VBScript
    strHTML = strHTML &  vbNewLine & "<table cellspacing=""0"">"
    strHTML = strHTML &  vbNewLine & vbTab & "<tr class=""header"">"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>Test Area</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>Not Run</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>Expected</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>BVT Failures</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>BVT Passes</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>BVT Pass Rate</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>Other Failures</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>Other Passes</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>Other Pass Rate</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>Total Pass Rate</td>"
    strHTML = strHTML &  vbNewLine & vbTab & vbTab & "<td>Total Disabled Count</td>"
    strHTML = strHTML &  vbNewLine & vbTab & "</tr>"
    
    'table rows
    for each objTestArea in sobjDictionaryTestAreaData.Items
        if f_fEmail then
            sTestDetail = objTestArea.GetCellString( FALSE )
        else
            sTestDetail = objTestArea.GetCellString( TRUE )
        end if
            
        ' If no test result for this cell, then display N/A
        if sTestDetail = "" then 
            sTestDetail = "<tr class=""na"">" _
            & vbNewLine & vbTab & "<td class=""area"">" & objTestArea.AreaName & "</td>" _
            & vbNewLine & vbTab & "<td colspan=""10"">N/A</td>" _
            & vbNewLine & "</tr>"
           end if
        strHTML = strHTML & vbNewLine & sTestDetail 
    next
    
    strHTML = strHTML & vbNewLine & "</table>"
    
    strHTML = strHTML & "<br />" & vbNewLine _
        & "Archived results are in <a href=""" & g_strResultURL & """>" & g_strResultURL & "</a>"
    strHTML = strHTML & vbNewLine & "</body>" & vbNewLine & "</html>"
    
    BuildHTML = strHTML
end function

sub DebugPrint(s)
    wscript.echo s
end sub

function RegExReplace( f_strSrc, f_strPattern, f_strReplace )
    dim regex
    set regex = new RegExp
    regex.Pattern = f_strPattern
    regex.IgnoreCase = True

    RegExReplace = regex.Replace(f_strSrc, f_strReplace)
end function

Class TestCaseCounts
    public Passed
    Public Failed
    Public Disabled
    Public Expected
    public property get Missed
        Missed = Expected - ( Passed + Failed )
    end property
    public property get PassRate
        if Expected = 0 then 
            PassRate = 100
        else
            PassRate = int( Passed / Expected * 100 )
        end if
    end property
    public property get RunRate
        if Expected + Disabled = 0 then 
            RunRate = 100
        else
            RunRate = int( (Passed + Failed) / ( Expected + Disabled ) * 100 )
        end if
    end property

    private sub class_initialize
        Passed = 0
        Failed = 0
        Disabled = 0
        Expected = 0
    end sub
end class

'Lovely little class to encapsulate the test result information for each area.
Class TestAreaResults
    ''''''''''''''''''''''''''''''
    '' Internal Variables
    ''''''''''''''''''''''''''''''
    private m_cBVT
    private m_cDRT
    private m_cFunc
    private m_cPerf
    private m_cFuzz
    private m_fError
    private m_strResultFile
    private m_strTestArea
    private m_rgScriptFiles()
    private m_rgPlatformExtensions()
    private m_fCountWhat
    private m_fInited
    private m_fResultsParsed

    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    '' Properties
    ''
    '' for the most part we want to do more things
    '' when data is set, like clearing m_fInited, so we don't 
    '' make data members public and we use properties
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    public property let ResultFile( byval strResultFile )
        m_fInited = false
        m_strResultFile = strResultFile
    end property

    public property get ResultFile
        ResultFile = m_strResultFile
    end property


    public property let AreaName( byval strArea )
        m_fInited = false
        m_strTestArea = strArea
    end property

    public property get AreaName
        AreaName = m_strTestArea
    end property

    public property get CountWhat
        CountWhat = m_fCountWhat
    end property
    
    public property get ParseError
        InitIfNeeded
        ParseError = m_fError
    end property
    
    public property get AreaPassed
        InitIfNeeded
        if FailedCount( constRunAll ) > 0 or m_fError = true then 
            AreaPassed = false
        else
            AreaPassed = true
        end if
    end property

    public property get MissedCount( getType )
        InitIfNeeded     
        MissedCount = 0
        if ( getType and constRunBVT ) = constRunBVT then
            MissedCount = MissedCount + m_cBVT.Missed
        end if
        if ( getType and constRunDRT ) = constRunDRT then
            MissedCount = MissedCount + m_cDRT.Missed
        end if
        if ( getType and constRunFunc ) = constRunFunc then
            MissedCount = MissedCount + m_cFunc.Missed
        end if
        if ( getType and constRunPerf ) = constRunPerf then
            MissedCount = MissedCount + m_cPerf.Missed
        end if
        if( getType and constRunFuzz ) = constRunFuzz then
            MissedCount = MissedCount + m_cFuzz.Missed
        end if 
    end property

    public property get FailedCount( getType )
        InitIfNeeded        
        FailedCount = 0
        if ( getType and constRunBVT ) = constRunBVT then
            FailedCount = FailedCount + m_cBVT.Failed
        end if
        if ( getType and constRunDRT ) = constRunDRT then
            FailedCount = FailedCount + m_cDRT.Failed
        end if
        if ( getType and constRunFunc ) = constRunFunc then
            FailedCount = FailedCount + m_cFunc.Failed
        end if
        if ( getType and constRunPerf ) = constRunPerf then
            FailedCount = FailedCount + m_cPerf.Failed
        end if
        if( getType and constRunFuzz ) = constRunFuzz then
            FailedCount = FailedCount + m_cFuzz.Failed
        end if 
    end property

    public property get PassedCount( getType )
        InitIfNeeded
        PassedCount = 0
        if ( getType and constRunBVT ) = constRunBVT then
            PassedCount = PassedCount + m_cBVT.Passed
        end if
        if ( getType and constRunDRT ) = constRunDRT then
            PassedCount = PassedCount + m_cDRT.Passed
        end if
        if ( getType and constRunFunc ) = constRunFunc then
            PassedCount = PassedCount + m_cFunc.Passed
        end if
        if ( getType and constRunPerf ) = constRunPerf then
            PassedCount = PassedCount + m_cPerf.Passed
        end if
        if( getType and constRunFuzz ) = constRunFuzz then
            PassedCount = PassedCount + m_cFuzz.Passed
        end if 
    end property

    public property get DisabledCount( getType )
        InitIfNeeded
        DisabledCount = 0
        if ( getType and constRunBVT ) = constRunBVT then
            DisabledCount = DisabledCount + m_cBVT.Disabled
        end if
        if ( getType and constRunDRT ) = constRunDRT then
            DisabledCount = DisabledCount + m_cDRT.Disabled
        end if
        if ( getType and constRunFunc ) = constRunFunc then
            DisabledCount = DisabledCount + m_cFunc.Disabled
        end if
        if ( getType and constRunPerf ) = constRunPerf then
            DisabledCount = DisabledCount + m_cPerf.Disabled
        end if
        if( getType and constRunFuzz ) = constRunFuzz then
            DisabledCount = DisabledCount + m_cFuzz.Disabled
        end if 
    end property

    public property get ExpectedCount( getType )
        InitIfNeeded
        ExpectedCount = 0
        if ( getType and constRunBVT ) = constRunBVT then
            ExpectedCount = ExpectedCount + m_cBVT.Expected - m_cBVT.Missed
        end if
        if ( getType and constRunDRT ) = constRunDRT then
            ExpectedCount = ExpectedCount + m_cDRT.Expected - m_cDRT.Missed
        end if
        if ( getType and constRunFunc ) = constRunFunc then
            ExpectedCount = ExpectedCount + m_cFunc.Expected - m_cFunc.Missed
        end if
        if ( getType and constRunPerf ) = constRunPerf then
            ExpectedCount = ExpectedCount + m_cPerf.Expected - m_cPerf.Missed
        end if
        if( getType and constRunFuzz ) = constRunFuzz then
            ExpectedCount = ExpectedCount + m_cFuzz.Expected
        end if 
    end property

    public property get PassPercentage( getType )
        InitIfNeeded
        if ExpectedCount(getType) = 0 then 
            PassPercentage = 100
        else
            PassPercentage = int( PassedCount(getType) / ExpectedCount(getType) * 100 )
        end if
    end property
    

    public property get RunPercentage( getType )
        InitIfNeeded
        if ExpectedCount(getType) + DisabledCount(getType) = 0 then 
            PassPercentage = 100
        else
            PassPercentage = int( (PassedCount(getType) + FailedCount( getType )) / (ExpectedCount(getType) + DisabledCount(getType)) * 100 )
        end if
    end property

    'Class Constructor
    private sub class_initialize
        m_fInited = false
        m_fResultsParsed = false
        set m_cBVT = new TestCaseCounts
        set m_cDRT = new TestCaseCounts
        set m_cFunc = new TestCaseCounts
        set m_cPerf = new TestCaseCounts
        set m_cFuzz = new TestCaseCounts
        
        m_fError = false
        m_strResultFile = Empty
        m_strTestArea = Empty
        m_fCountWhat = constRunNone
    end sub

    private sub InitIfNeeded
        if not m_fInited then
            ScanResultFile
            if m_fResultsParsed then
                ScanTestScripts
            end if
            m_fInited = true
        end if
    end sub

    private sub ScanResultFile
        on error resume next
        if m_strResultFile <> Empty then
            m_fResultsParsed = FALSE
            ParseResultXML
            If Err.Number <> 0 Then
                DebugPrint "Failed to Parse Result File: " & g_strResultPath & m_strResultFile
                m_fError = true
    	        Err.Clear
    	    else
    	        m_fResultsParsed = true
            End If
        end if
        on error goto 0
    end sub

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    '' Parse the result file
    ''
    '' The XML looks like
    ''      <?xml version="1.0" encoding="iso-8859-1"?>
    ''      <?xml-stylesheet type="text/xsl" href="result-transform.xsl"?>
    ''
    ''      <LOGSUMMARY stdout_log="testarealog.htm">
    ''          <testinfo>
    ''              <scriptfilebases>
	''                  <file>testscript1</file>
	''                  <file>testscript2</file>
	''                  ...
    ''              </scriptfilebases>
    ''              <platforms>
	''                  <platform>.tc.</platform>
	''                  <platform>.pc.</platform>
	''                  ...
    ''              </platforms>
    ''              <runwhat DRT="YES|NO" BVT="YES|NO" FUNC="YES|NO" PERF="YES|NO" />
    ''          </testinfo>
    ''          <LOGRESULTS CaseNo="Case#" Type="BVT|FUNC|DRT|PERF" Result="PASS" Ticks="#####">
    ''              <Title>Some Passing Test Case</Title>
    ''          </LOGRESULTS>
    ''          <LOGRESULTS CaseNo="Case#" Type="BVT|FUNC|DRT|PERF" Result="FAIL" Ticks="#####">
    ''              <Title>Some Failing Test Case</Title>
    ''              <Trace>What Failed</Trace>
    ''          </LOGRESULTS>
    ''          <LOGRESULTS CaseNo="Case#" Type="BVT|FUNC|DRT|PERF" Result="SKIP" Ticks="#####">
    ''              <Title>Some Skipped Test Case</Title>
    ''          </LOGRESULTS>
    ''          ...
    ''     </LOGSUMMARY>
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    private sub ParseResultXML
        dim domDoc, domRoot, domNodeList, domNode, i
        DebugPrint "Parsing Result File: " & g_strResultPath & m_strResultFile

        m_fCountWhat = constRunNone

        set domDoc = CreateObject("Msxml2.DOMDocument")
        domDoc.async = false
        domDoc.load "file:" & g_strResultPath & m_strResultFile
        set domRoot = domDoc.documentElement

        DebugPrint "Successfully Loaded Result File into DOM: " & g_strResultPath & m_strResultFile

        with domRoot

            set domNodeList = .selectNodes("/LOGSUMMARY/testinfo/scriptfilebases/file")
            ReDim m_rgScriptFiles(domNodeList.length)
            for i = 0 to domNodeList.length - 1
                set domNode = domNodeList.nextNode
                m_rgScriptFiles(i) = domNode.Text
            next

            set domNodeList = .selectNodes("/LOGSUMMARY/testinfo/platforms/platform")
            ReDim m_rgPlatformExtensions(domNodeList.length)
            for i = 0 to domNodeList.length - 1
                set domNode = domNodeList.nextNode
                m_rgPlatformExtensions(i) = domNode.Text
            next

            'Get what to count from the test info
            set domNode = .selectSingleNode("/LOGSUMMARY/testinfo/runwhat")
            if domNode.getAttribute("DRT") = "YES" then
                m_fCountWhat = m_fCountWhat + constRunDRT
            end if
            if domNode.getAttribute("BVT") = "YES" then
                m_fCountWhat = m_fCountWhat + constRunBVT
            end if
            if domNode.getAttribute("FUNC") = "YES" then
                m_fCountWhat = m_fCountWhat + constRunFunc
            end if
            if domNode.getAttribute("PERF") = "YES" then
                m_fCountWhat = m_fCountWhat + constRunPerf
            end if
            if domNode.getAttribute("FUZZ") = "YES" then
                m_fCountWhat = m_fCountWhat + constRunFuzz
            end if

            'count each type
            'BVT
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""PASS""][@Type=""BVT""]")
            m_cBVT.Passed = domNodeList.length
            set domNodeList = nothing
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""FAIL""][@Type=""BVT""]")
            m_cBVT.Failed = domNodeList.length
            set domNodeList = nothing
            
            'DRT
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""PASS""][@Type=""DRT""]")
            m_cDRT.Passed = domNodeList.length
            set domNodeList = nothing
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""FAIL""][@Type=""DRT""]")
            m_cDRT.Failed = domNodeList.length
            set domNodeList = nothing
            
            'FUNC
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""PASS""][@Type=""FUNC""]")
            m_cFunc.Passed = domNodeList.length
            set domNodeList = nothing
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""FAIL""][@Type=""FUNC""]")
            m_cFunc.Failed = domNodeList.length
            set domNodeList = nothing
            
            'PERF
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""PASS""][@Type=""PERF""]")
            m_cPerf.Passed = domNodeList.length
            set domNodeList = nothing
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""FAIL""][@Type=""PERF""]")
            m_cPerf.Failed = domNodeList.length
            set domNodeList = nothing
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""PASS""][@Type=""FUZZ""]")
            m_cFuzz.Passed = domNodeList.length
            set domNodeList = nothing
            set domNodeList = .selectNodes("/LOGSUMMARY/LOGRESULTS[@Result=""FAIL""][@Type=""FUZZ""]")
            m_cFuzz.Failed = domNodeList.length
            set domNodeList = nothing
        end with
        set domRoot = nothing
        set domDoc = nothing
        set domNode = Nothing
    end sub

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''
    '' Check the name of the file to see if it matches 
    '' an expected test script and see if it has the correct
    '' platform extension
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''
    public function IsTestScriptFile( strFile )
        dim strScriptFile, strExtension, strFileBase, rgParts, fPlatformGood
        DebugPrint "testing file for script file: " & strFile
        rgParts = Split(strFile, ".")
        strFileBase = rgParts(0)
        fPlatformGood = false
        IsTestScriptFile = false

        if strcomp(right(strFile, 4), ".txt", 1) = 0 then
            for each strExtension in m_rgPlatformExtensions
                if strExtension = Empty then
                    exit for
                end if
                DebugPrint "checking extension: " & strExtension
                if InStr(strFile, strExtension ) > 0 then
                    fPlatformGood = true
                    exit for
                end if
            next
            if not fPlatformGood then
                exit function
            end if
            for each strScriptFile in m_rgScriptFiles
                if strScriptFile = Empty then
                    exit for
                end if
                DebugPrint "checking base: " & strScriptFile
                if strcomp( strFileBase, strScriptFile, vbTextCompare ) = 0 then
                    IsTestScriptFile = true
                    Exit Function
                end if
            Next
        end if
    end function

    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    '' Scans the test script files to count the expected number
    '' of test cases based on the type of tests to run
    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    private sub ScanTestScripts
        dim files, file, folderPath, defaultRunType
        folderPath = g_strTestPath & m_strTestArea
        set files = sobjFS.GetFolder(folderPath).files
        for each file in files

            if IsTestScriptFile(file.name) then
                defaultRunType = constRunBVT
                dim inf, FirstChar, line, SecondChar, fDisabled, comment
                
                DebugPrint "Opening file " & file.path
                set inf = sobjFS.OpenTextFile(file.path, 1)

                Do until inf.AtEndOfStream
                    fDisabled = false
                    SecondChar = 0
                    line = ltrim(inf.readline)
                    comment = Mid(line,2)
                    if line <> "" then
                        FirstChar = Asc(line)
                        if comment <> "" then
                            SecondChar = Asc( comment )
                        end if
                        
                        if FirstChar = asc("-") then
                            ' If we see the BVT end marker here
                            if Instr(line, constBVTEndsHere) > 0 then
                                defaultRunType = constRunFunc
                            elseif Instr(line, constFuncEndsHere) > 0 then
                               defaultRunType = constRunDRT
                            elseif Instr(line, constDRTEndsHere) > 0 then
                                defaultRunType = constRunPerf
                             end if
                        end if
                        
                        if FirstChar = asc("'") and SecondChar > 47 and SecondChar < 58 and Instr(line, ":") > 0 then
                            fDisabled = true
                        end if
                        if ( FirstChar > 47 and FirstChar < 58 and Instr(line, ":") > 0 ) or fDisabled then
                            dim strCurrentTag, strTestCaseID, fCounted
                            dim currentType
                            dim nOffset1, nOffset2
                            dim nOffset3, strCurrentTag2

                            strCurrentTag = ""
                            strTestCaseID = ""
                            nOffset1 = Instr(line, ":")
                            strTestCaseID = left( line, nOffset1 - 1 )
                            fCounted = false
                            
                            nOffset2 = Instr(nOffset1+1, line, ":")
                            if nOffset2 <> 0 then
                                strCurrentTag = mid(line, nOffset1 + 1, nOffset2 - nOffset1 - 1)
                                nOffset3  = Instr(nOffset2+1, line, ":")
                                if nOffset3 <> 0  then
                                    strCurrentTag2 = mid(line, nOffset2 + 1, nOffset3 - nOffset2 - 1)
                                    if strCurrentTag2 = "DISABLED" then
                                        fDisabled = true
                                    end if
                                end if
                            end if

                            currentType = GetTestCaseType( strCurrentTag )
                            if currentType = constRunNone then 
                                currentType = defaultRunType
                            end if

                            select case currentType
                                case constRunBVT 
                                    if ( m_fCountWhat and constRunBVT ) = constRunBVT then
                                        if fDisabled then
                                            m_cBVT.Disabled = m_cBVT.Disabled + 1
                                        else
                                            m_cBVT.Expected = m_cBVT.Expected + 1
                                        end if
                                        fCounted = True
                                    end if
                                case constRunDRT 
                                    if ( m_fCountWhat and constRunDRT ) = constRunDRT then
                                        if fDisabled then
                                            m_cDRT.Disabled = m_cDRT.Disabled + 1
                                        else
                                            m_cDRT.Expected = m_cDRT.Expected + 1
                                        end if
                                        fCounted = True
                                    end if
                                case constRunFunc 
                                    if ( m_fCountWhat and constRunFunc ) = constRunFunc then
                                        if fDisabled then
                                            m_cFunc.Disabled = m_cFunc.Disabled + 1
                                        else
                                            m_cFunc.Expected = m_cFunc.Expected + 1
                                        end if
                                        fCounted = True
                                    end if
                                case constRunPerf 
                                    if ( m_fCountWhat and constRunPerf ) = constRunPerf then
                                        if fDisabled then
                                            m_cPerf.Disabled = m_cPerf.Disabled + 1
                                        else
                                            m_cPerf.Expected = m_cPerf.Expected + 1
                                        end if
                                        fCounted = True
                                    end if
                                case constRunFuzz 
                                    if ( m_fCountWhat and constRunFuzz ) = constRunFuzz then
                                        if fDisabled then
                                            m_cFuzz.Disabled = m_cFuzz.Disabled + 1
                                        else
                                            m_cFuzz.Expected = m_cFuzz.Expected + 1
                                        end if
                                        fCounted = True
                                    end if
                            end select
                            if fDisabled and fCounted then
                                ' strTestCaseID will have a leading apostrophe, need to strip it off
                                g_strDisabledTests = g_strDisabledTests & m_strTestArea & ":" & Mid(strTestCaseID,2) & vbNewLine
                            elseif ( not WasTestCaseRun( strTestCaseID ) ) And fCounted then
                                g_strNotRunTests = g_strNotRunTests & vbNewLine & m_strTestArea & ":" & strTestCaseID & vbNewLine
                            End if
                        end if
                    end if
                loop
                set inf = nothing
            end if
        next
        set files = nothing
        DebugPrint "Test case count for " & m_strTestArea & " :"
        DebugPrint vbTab & "BVT  = " & m_cBVT.Expected
        DebugPrint vbTab & "DRT  = " & m_cDRT.Expected
        DebugPrint vbTab & "Func = " & m_cFunc.Expected
        DebugPrint vbTab & "Perf = " & m_cPerf.Expected
        DebugPrint vbTab & "Fuzz = " & m_cFuzz.Expected
    end sub

    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    '' Looks for a node in the result file with the CaseNo = f_strTestCaseID    ''
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    Private function WasTestCaseRun( f_strTestCaseID )
        dim domDoc, domRoot, domNode
    
        set domDoc = CreateObject("Msxml2.DOMDocument")
        domDoc.async = false
        domDoc.load "file:" & g_strResultPath & m_strResultFile
        set domRoot = domDoc.documentElement
    
        set domNode = domRoot.selectSingleNode( "/LOGSUMMARY/LOGRESULTS[@CaseNo=""" & f_strTestCaseID & """]" )
    
        if domNode Is Nothing Then
            WasTestCaseRun = False
        else
            WasTestCaseRun = True
        end if
        Set domDoc  = Nothing
        Set domRoot = Nothing
        Set domNode = Nothing
    End function

    private function GetTestCaseType( strTag )
        select case strTag
            case "BVT" 
                GetTestCaseType = constRunBVT
            case "DRT"
                GetTestCaseType = constRunDRT
            case "PERF"
                GetTestCaseType = constRunPerf
            case "FUNC"
                GetTestCaseType = constRunFunc
            case "FUZZ"
                GetTestCaseType = constRunFuzz
            case else
                GetTestCaseType = constRunNone
        end select
    end function

    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    '' Makes a table cell string for inclusion in a larger result summary table
    ''
    '' Formatted as:
    ''  <tr class="SUCCESS-CLASS">
	''      <td class="area"><a href="testarea.xml"> TestArea</a></td>
	''      <td>Missed Count</td>
	''      <td>Expected Count</td>
	''      <td>BVT Failed Count</td>
	''      <td>BVT Passed Count</td>
	''      <td>BVT PassPercentage</td>
	''      <td>Other Failed Count</td>
	''      <td>Other Passed Count</td>
	''      <td>Other PassPercentage</td>
	''      <td>Total PassPercentage</td>
    ''  </tr>
    ''
    ''  SUCCESS-CLASS is one of the following CSS classes:
    ''      "pass"      -- if all tests pass
    ''      "some-fail" -- if more that 50% of the tests pass
    ''      "fail"      -- if fewer than 50% of the tests pass, ANY BVT or DRT failed, or a parse error occured
    ''
    ''  If there was an error parsing the html the following string is generated:
    ''      <td colspan=""10""> ERROR reading XML file </td>
    ''
    ''  Returns:    A string containing the generated HTML 
    ''
    '''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    public function GetCellString(fRelativePath)
        dim strRet, strResultFilePath
        InitIfNeeded

        if not m_fResultsParsed then
            GetCellString = ""
            exit function
        end if
        
        if fRelativePath then
            strResultFilePath = m_strResultFile
        else
            strResultFilePath = g_strResultURL & m_strResultFile
        end if
        
        if PassPercentage( constRunFunc + constRunPerf + constRunFuzz ) < 50 or FailedCount( constRunBVT + constRunDRT ) > 0 or m_fError then
            strRet = "<tr class=""fail"">"
        elseif PassPercentage( constRunAll ) < 100 then
            strRet = "<tr class=""some-fail"">"
        else
            strRet = "<tr class=""pass"">"
        end if
        
        strRet = strRet & vbNewLine & vbTab & "<td class=""area""><a href=""" & strResultFilePath & """> " & m_strTestArea & "</a></td>" 
        if m_fError = false then    
            strRet = strRet & vbNewLine & vbTab & "<td>" & cstr(MissedCount( constRunAll )) & "</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(ExpectedCount( constRunAll )) & "</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(FailedCount( constRunBVT + constRunDRT ) ) & "</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(PassedCount( constRunBVT + constRunDRT )) & "</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(PassPercentage( constRunBVT + constRunDRT )) & "%</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(FailedCount( constRunFunc + constRunPerf + constRunFuzz) ) & "</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(PassedCount( constRunFunc + constRunPerf + constRunFuzz )) & "</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(PassPercentage( constRunFunc + constRunPerf + constRunFuzz )) & "%</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(PassPercentage( constRunAll )) & "%</td>" _
                & vbNewLine & vbTab & "<td>" & cstr(DisabledCount( constRunAll )) & "</td>"
        else
            strRet = strRet & vbNewLine & vbTab & "<td colspan=""10""> ERROR reading XML file </td>" 
        end if
        strRet = strRet & vbNewLine & "</tr>"

        GetCellString = strRet
    end function

end class


