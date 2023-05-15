' @@@+++@@@@******************************************************************
' **
' ** Microsoft (r) PlayReady (r)
' ** Copyright (c) Microsoft Corporation. All rights reserved.
' **
' @@@+++@@@@******************************************************************

' this script runs tests for the WMDRM Porting Kits
' settings can  be configured in testconfig.config or in your own config file passed with -config:somefile
' see runpkTests.vbs -? for more on commandline options.

option explicit

''''''''''''''''''''''''' Set Defaults '''''''''''''''''''''''''''''''''''''''''''''''''''''
' Path Expansion supported for defaults:
' $PKPath       Expands to the runtime value for the PK Path
' $TestPath     Expands to the runtime value for the test path
' $ResultPath   Expands to the runtime value for the result path
' $DATE         Expands to the current date
' $BRANCH       Expands the branch name
' $BUILD        Expands the build number
' $FLAVOR       Expands the flavor (chk, fre, perf, ... )
' $ARCH         Expands the architecture (x86, ARM, ... )
'
' Expansion is done once, just after launch and is not recursive.
' The value for PKPath set in the defaults or on the commandline may not contain TestPath or ResultPath
' for expansion. The TestPath cannot contain the ResultPath.

' This is the Porting Kit location
const constDefaultPKPath = "C:\PlayReady\$BRANCH_$BUILD\"

const constDefaultDebugger = "C:\debuggers\windbg.exe"

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
const constDebugSymbols = "srv*"

'=========================No customization after this point========================

dim g_strTestPath, g_strResultPath, g_strBinPath, g_strSamplesPath, g_strPKPath, g_fParallel
dim g_strBranch, g_strFlavor, g_strBuildNo, g_strArch, g_strCopyDep, g_strTempPath, g_strToolsPath, g_strDebugSymbols


dim sobjWShell, sobjWShellEnvironment, sobjWShellNetwork, sobjFS
dim TestAreaFile, AreaToRun, g_strCaseTypes, g_nTestCaseToRun, g_fDebugTest, g_fCodeCoverage


const constFlavorFlag = "-flav:"
const constBuildNumberFlag = "-buildNo:"
const constArchFlag = "-arch:"
const constDebugTest = "-debug"
const constCopyDep = "-copydep:"
const constDontRunParallel = "-singlethread"

Set sobjWShell = WScript.CreateObject("WScript.Shell")
Set sobjWShellEnvironment = sobjWShell.Environment("Process")
Set sobjWShellNetwork = WScript.CreateObject("WScript.Network")
Set sobjFS = CreateObject("Scripting.FileSystemObject")

Const TemporaryFolder = 2
g_strTempPath = sobjFS.GetSpecialFolder(TemporaryFolder)


g_fParallel = true
g_strCaseTypes = ""
g_fCodeCoverage = false


ParseCommandLineParameter AreaToRun

' This is the number of test areas to run at the same time
Dim nTestModuleConcurrency
Dim objCPU

nTestModuleConcurrency = 0

' Default the number of concurrent test areas to the number of logical processors
On Error Resume Next
For each objCPU in GetObject("winmgmts:{impersonationLevel=impersonate}\\.\root\cimv2").InstancesOf("Win32_Processor")
	' This property is not supported pre-Vista, so pre-Vista nTestModuleConcurrency will simply end up
	' as the count of CPU objects, post-Vista it will be the count of logical processors
	nTestModuleConcurrency = nTestModuleConcurrency + objCPU.NumberOfLogicalProcessors

	If Err.Number <> 0 Then
		nTestModuleConcurrency = nTestModuleConcurrency + 1
		Err.Clear
	End If
Next
If Err.Number <> 0 Or nTestModuleConcurrency = 0 or not g_fParallel Then
	nTestModuleConcurrency = 1
	Err.Clear
End If
On Error Goto 0


CheckDefault g_strBranch , constDefaultBranch
CheckDefault g_strBuildNo , constDefaultBuildNo
CheckDefault g_strFlavor , constDefaultFlavor
CheckDefault g_strArch , constDefaultArch
CheckDefault g_strDebugSymbols, constDebugSymbols

CheckPath g_strPKPath,             constDefaultPKPath 
CheckPath g_strTestPath,           constDefaultTestPath 
CheckPath g_strResultPath,         constDefaultResultPath 
CheckPath g_strSamplesPath,        constDefaultSamplesPath 
CheckPath g_strToolsPath,          constDefaultToolsPath 
CheckPath g_strBinPath,            constDefaultBinPath
CheckPath g_strCopyDep,            ""

'
' main program
'

'Create results Folder
CreateFolders  "" ,  g_strResultPath, True

'copy the stylesheet
ForceCopy ExpandDefaults(constXSLTSource), g_strResultPath & "result-transform.xsl"
ForceCopy ExpandDefaults(constCSSSource), g_strResultPath & "result-style.css"

Dim strLine
Dim TestAreaData
Dim i, j
ReDim oExecStatus(nTestModuleConcurrency-1)
ReDim oExecParams(nTestModuleConcurrency-1,3)

For i = 0 To UBound(oExecStatus)
    oExecStatus(i) = 0
Next

i = 0

Set TestAreaFile = sobjFS.OpenTextFile( ExpandDefaults(constTestAreas) )
do until TestAreaFile.AtEndOfStream
    strLine = Trim(TestAreaFile.ReadLine)
    if strLine <> ""  and left(strLine, 1) <> "#" then
        TestAreaData = Split(strLine)
        
        ' Check if we are to run a specific area or all areas
        If RunThisArea(TestAreaData(0), AreaToRun) Then
        	If g_strCopyDep <> "" Then
				CopyDependentFiles TestAreaData(0), g_strCopyDep
        	Else
	            Do
	                If VarType(oExecStatus(i)) = 9 Then
	                    If oExecStatus(i).Status <> 0 Then
	                        FinishExecJob i, oExecParams, oExecStatus
	                    Else
	                        Wscript.Sleep 50
	                    End If
	                End If
	                
	                If VarType(oExecStatus(i)) = 2 Then
	                    oExecParams(i,0) = TestAreaData(0)
	                    oExecParams(i,1) = TestAreaData(1)
	                    oExecParams(i,2) = Now

	                    Wscript.Echo i & "> " & oExecParams(i,2) & " - Running area " & TestAreaData(0)
	                    Set oExecStatus(i) = RunTest(TestAreaData(0), TestAreaData(1), i)
	                    
	                    i = (i + 1) Mod (UBound(oExecStatus) + 1)
	                    Exit Do
	                End If
	                
	                i = (i + 1) Mod (UBound(oExecStatus) + 1)
	            Loop While True
	        End If
        End If
    end if
loop
TestAreaFile.close

Dim fAllTestsDone
fAllTestsDone = False
Do
    i = (i + 1) Mod (UBound(oExecStatus) + 1)

    If VarType(oExecStatus(i)) = 9 Then
        If oExecStatus(i).Status <> 0 Then
            FinishExecJob i, oExecParams, oExecStatus
        Else
            Wscript.Sleep 50
        End If
    End If
    
    For j = 0 To UBound(oExecStatus)
        If VarType(oExecStatus(j)) = 9 Then
            Exit For
        ElseIf j = UBound(oExecStatus) Then
            fAllTestsDone = True
        End If
    Next
Loop Until fAllTestsDone

Sub FinishExecJob(i, ByRef oExecParams, ByRef oExecStatus)
    Dim oStdOutFile
    Dim strTestFilesPath
    
    Wscript.Echo i & "> " & Now & " - Area " & oExecParams(i,0) & " finished, " & DateDiff("s", oExecParams(i,2), Now) & " second(s) elapsed"

    ' Get the test files path
    strTestFilesPath = g_strTempPath & "\runPKtest" & i
    
    SaveTestResult oExecParams(i,0), oExecParams(i,1), strTestFilesPath
    
    oExecStatus(i) = 0
End Sub

'
' End of main program
'

' Parse commandline parameters
' -bvt                  run bvt
' -func                 run functional
' -drt                  run drt
' -perf                 run perf
' -TestRoot:            specify test path on command line
' -SamplesRoot:         specify samples path on command line
' -ToolsRoot:           specify tools path on command line
' -BinRoot:             specify bin path on command line
' -resultroot:          specify where to aggregate the results.
' -pkroot:              specify the root of the PK installation.
' -?                    help
' -branch:              ovverride default branch name
' areaname:             only run this area
' -copydep:				copy test case files to specified destination only, don't run tests
sub ParseCommandLineParameter(byRef AreaToRun)
    dim i
    g_fDebugTest = false
    AreaToRun = Null
    for i = 0 to wscript.arguments.length - 1
        if lcase(left(wscript.arguments(i), 2)) = "-?" then
            wscript.echo "runPKTests.vbs executes the test case scripts defined in the testareas file" _
                & vbNewLine & "options:" _
                & vbNewLine & vbtab & " -?" & vbTab & "this help" _
                & vbNewLine & vbtab & " -bvt" & vbTab & "run BVT test cases" _
                & vbNewLine & vbtab & " -func" & vbTab & "run functional test cases" _
                & vbNewLine & vbtab & " -drt" & vbTab & "run DRT test cases" _
                & vbNewLine & vbtab & " -perf" & vbTab & "run performance test cases" _
                & vbNewLine & vbtab & " -cov" & vbTab & "run test cases from code coverage build, and get code coverage results" _
                & vbNewLine & vbtab & " -pkroot:somepath" & vbTab & "set the root directory of the pk install" _
                & vbNewLine & vbtab & " -TestRoot:somepath" & vbTab & "set the directory for the test sources" _
                & vbNewLine & vbtab & " -ResultRoot:somepath" & vbTab & "set the location to send the result logs" _
                & vbNewLine & vbtab & " -SamplesRoot:somepath" & vbTab & "set the directory for sample files" _
                & vbNewLine & vbtab & " -ToolsRoot:somepath" & vbTab & "set the directory for pregenerated tools" _
                & vbNewLine & vbtab & " -BinRoot:somepath" & vbTab & "set the directory of the binaries" _
                & vbNewLine & vbtab & " -branch:name" & vbTab & "override the default PK branch name" _
                & vbNewLine & vbtab & " -buildNo:somestring" & vbTab & "override the default build number" _
                & vbNewLine & vbtab & " -arch:name" & vbTab & "override the default architecture name" _
                & vbNewLine & vbtab & " -flav:name" & vbTab & "override the default flavor name" _
                & vbNewLine & vbtab & " -temptestfolder:somepath" & vbTab & "override the default TEMP folder path" _
                & vbNewLine & vbtab & " -copydep:somepath" & vbTab & "copy test case files to specified destination only, don't run tests" _
                & vbNewLine & vbtab & " " & constDontRunParallel & vbTab & "don't run multiple test areas concurrently" _	
                & vbNewLine & vbtab & " areaname" & vbTab & "only run this area" _
                & vbNewLine & vbNewLine & "The file testconfig.config is a VBScript file" _
                & "that defines default constants. If you have any customization you can make it in that file or" _
                & " create a copy of that file and customize the copy and pass it in with the -config: flag" _
                & vbNewLine & vbNewLine
            wscript.quit
        elseif lcase(left(wscript.arguments(i), 8)) = "-pkroot:" then
            g_strPKPath = right(wscript.arguments(i), len(wscript.arguments(i)) - 8)
        elseif lcase(left(wscript.arguments(i), 10)) = "-testroot:" then
            g_strTestPath = right(wscript.arguments(i), len(wscript.arguments(i)) - 10)
        elseif lcase(left(wscript.arguments(i), 13)) = "-samplesroot:" then
            g_strSamplesPath = right(wscript.arguments(i), len(wscript.arguments(i)) - 13)
        elseif lcase(left(wscript.arguments(i), 11)) = "-toolsroot:" then
            g_strToolsPath = right(wscript.arguments(i), len(wscript.arguments(i)) - 11)
        elseif lcase(left(wscript.arguments(i), 9)) = "-binroot:" then
            g_strBinPath = right(wscript.arguments(i), len(wscript.arguments(i)) - 9)
        elseif lcase(left(wscript.arguments(i), 12)) = "-resultroot:" then 
            g_strResultPath= right(wscript.arguments(i), len(wscript.arguments(i)) - 12)
        elseif lcase(left(wscript.arguments(i), 16)) = "-temptestfolder:" then 
            g_strTempPath= right(wscript.arguments(i), len(wscript.arguments(i)) - 16)
        elseif lcase(left(wscript.arguments(i), 8 ) ) = "-branch:" then
            g_strBranch = right(wscript.arguments(i), len(wscript.arguments(i)) - 8 )
        elseif StrComp(left(wscript.arguments(i), len(constFlavorFlag )), constFlavorFlag , 1) = 0 then 
            g_strFlavor = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constFlavorFlag ))
        elseif StrComp(left(wscript.arguments(i), len(constArchFlag)), constArchFlag, 1) = 0 then 
            g_strArch = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constArchFlag))
        elseif StrComp(left(wscript.arguments(i), len(constBuildNumberFlag)), constBuildNumberFlag, 1) = 0 then 
            g_strBuildNo = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constBuildNumberFlag))
        elseif StrComp(left(wscript.arguments(i), len(constCopyDep)), constCopyDep, 1) = 0 then 
            g_strCopyDep = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constCopyDep))
        elseif StrComp(left(wscript.arguments(i), len(constDontRunParallel)), constDontRunParallel, 1) = 0 then 
            g_fParallel = false
        elseif StrComp(left(wscript.arguments(i), len(constDebugTest)), constDebugTest, 1) = 0 then 
            g_fDebugTest = true
'            g_strDebugSymbols = Right(wscript.arguments(i), len(wscript.arguments(i)) - len(constDebugTest))
        elseif left(wscript.arguments(i), 4) = "-bvt" then 
            g_strCaseTypes = g_strCaseTypes & "-bvt "
        elseif left(wscript.arguments(i), 5) = "-func" then 
            g_strCaseTypes = g_strCaseTypes & "-func "
       elseif left(wscript.arguments(i), 4) = "-drt" then 
            g_strCaseTypes = g_strCaseTypes & "-drt "
        elseif left(wscript.arguments(i), 5) = "-perf" then 
            g_strCaseTypes = g_strCaseTypes & "-perf "
        elseif left(wscript.arguments(i), 4) = "-cov" then 
            g_fCodeCoverage = true
        elseif IsTestCaseString(wscript.arguments(i))  then
        	   g_nTestCaseToRun = wscript.arguments(i)
            DebugPrint( g_nTestCaseToRun )
        else
            AreaToRun = wscript.arguments(i)
        end if          
    next
end sub


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
    If f_strPath <> "" Then
		If Right(f_strPath, 1) <> "\" Then
			f_strPath = f_strPath & "\"
		End If
		f_strPath = RegExReplace(f_strPath, "(.+)(\\\\)", "$1\")
	End If
end sub

sub CheckDefault( byref f_strIn, f_strDefault)
    if f_strIn = "" then
        f_strIn = f_strDefault
    end if
    f_strIn = ExpandDefaults( f_strIn )
end sub

' Check if we are to run a specific area or all areas.
' If there is a command line parameter, then only run the specified area. If not, then run all areas.
function RunThisArea(strArea, AreaToRun)
    dim fRun
    fRun = true
    if not isNULL(AreaToRun) then
        if StrComp(AreaToRun, strArea, 1) <> 0 then fRun = false
    end if
    RunThisArea = fRun
end function

function RunTest(strArea, strExe, nInstance)
    DebugPrint "Testing " & strArea & " exe: " & strExe & " " & g_strCaseTypes
    
    Dim strCmdline
    Dim strDrmPath
    Dim strTempPath
    Dim strTestFilesPath
    Dim strOldCurrentDirectory
    
    ' Get the instance specific working path for the module
    strTempPath = g_strTempPath & "\RunPkTests"

    ' Set up the DRM path
    strDrmPath = strTempPath & "\runPKtest" & nInstance & "DRM"
    ' Ensure the DRM path is created, and emptied
    CreateFolders strTempPath, "\runPKtest" & nInstance & "DRM\", True     
    CreateFolders strTempPath, "\runPKtest" & nInstance & "DRM\receiver\", True

    ' Set up the test files path
    strTestFilesPath = strTempPath & "\runPKtest" & nInstance
    ' Ensure the test files path is created, and emptied
    CreateFolders strTempPath, "\runPKtest" & nInstance & "\", True

    ' Copy test dependent files (test case files etc.)
    CopyDependentFiles strArea, strTestFilesPath

    strCmdline = "cmd /c " 

    if g_fDebugTest then
        strCmdline = strCmdline & constDefaultDebugger & " -o -y " & g_strDebugSymbols
    end if

    strCmdline = strCmdline & " " & g_strBinPath & strExe & " " & g_nTestCaseToRun & " -q -drmpath " & Chr(34) & strDrmPath & Chr(34) & " " & g_strCaseTypes & " > " & g_strBinPath & strExe & "_stdout.log"

    wscript.echo strCmdline

 	 strOldCurrentDirectory = sobjWShell.CurrentDirectory
	 sobjWShell.CurrentDirectory = strTestFilesPath
    if sobjFS.FileExists(g_strBinPath & strExe) then
        DebugPrint nInstance & "> Executing " & strCmdLine
        Set RunTest = sobjWShell.Exec(strCmdline)
    else
        Set RunTest = sobjWShell.Exec("cmd /c exit 0")
    end if
    sobjWShell.CurrentDirectory = strOldCurrentDirectory
end function

sub SaveTestResult(strArea, strModuleExeName, sTestFilesPath)
    dim  strModuleName, strSrc, strTar
    Dim  strCmdline
    Dim  objShell

	'If we are running the code coverage tests, we need to call covercmd.exe for each test area here
	if g_fCodeCoverage then
		strCmdline = "covercmd.exe /Save /As PK." & strArea
		Set objShell = CreateObject("wscript.shell")
		objShell.exec (strCmdLine)
	end if
       
    'the framework will create a XML log file with module name
    strModuleName=Left(strModuleExeName, Len(strModuleExeName)-4)

    strSrc = g_strBinPath & strModuleName & ".xml"
    strTar = g_strResultPath & strArea & ".xml"
    DebugPrint "Copying result from " & strSrc & " to " & strTar
    ForceCopy strSrc, strTar

    strSrc = g_strBinPath & strModuleName & ".htm"
    strTar = g_strResultPath & strModuleName & ".htm"
    DebugPrint "Copying result log from " & strSrc & " to " & strTar
    ForceCopy strSrc, strTar
    
    strSrc = sTestFilesPath & "\Perf.dat"
    strTar = g_strResultPath & strArea & "_perf.dat"
    DebugPrint "Copying result from " & strSrc & " to " & strTar
    ForceCopy strSrc, strTar

    strSrc = sTestFilesPath & "\PerfAgg.dat"
    strTar = g_strResultPath & strArea & "_perfagg.dat"
    DebugPrint "Copying result from " & strSrc & " to " & strTar
    ForceCopy strSrc, strTar

    strSrc = sTestFilesPath & "\PerfUserData.dat"
    strTar = g_strResultPath & strArea & "_perfuserdata.dat"
    DebugPrint "Copying result from " & strSrc & " to " & strTar
    ForceCopy strSrc, strTar
end sub

function EvalPathMacro(sLine)
    if left(sLine, 1) = "$" then
        dim nBPos, szMacro, szMacroEval
        nBPos = InStr(sLine, "\")
        szMacro = Left(sLine, nBPos - 1)
        
        ' Only support $(_DRM_REFCODE_TEST_FILES), $(_DRM_OEM_FILES), $(_DRM_TOOLS_FILES)
        if strcomp(szMacro, "$(_DRM_REFCODE_TEST_FILES)") = 0 then 
            szMacroEval = g_strTestPath & "common\"
        elseif strcomp(szMacro, "$(_DRM_OEM_FILES)") = 0 then 
            szMacroEval = g_strSamplesPath
        elseif strcomp(szMacro, "$(_DRM_TOOLS_FILES)") = 0 then 
            szMacroEval = g_strToolsPath
        else
            szMacroEval = szMacro
        end if
        
        DebugPrint "EvalPathMacro:" & szMacro & " evaluated to " & szMacroEval
        EvalPathMacro = szMacroEval & Mid(sLine, nBPos + 1)
    else
        EvalPathMacro = sLine   
    end if
end function

sub CopyDependentFiles(strArea, strDest)
    DebugPrint "Copying dependent files for " & strArea & " to temp folder"

    if sobjFS.FileExists(g_strTestPath & strArea & "\filedep.txt") then
    
	    dim fDep, sLine
	    Set fDep = sobjFS.OpenTextFile(g_strTestPath & strArea & "\filedep.txt")
	    do until fDep.AtEndOfStream
	        sLine = Trim(fDep.ReadLine)
	        if sLine <> "" then
	            dim posBS

	            ' If the file name has a macro, evaluate it first
	            if left(sLine, 1) = "$" then sLine = EvalPathMacro(sLine)

	            posBS = InstrRev(sLine, "\")

	            if left(sLine, 1) = "\" or InStr(sLine, ":") > 0 then 'This is a network or an absolute path
	                ForceCopy sLine, strDest & "\" & Mid(sLine, posBS + 1)
	            else
	                if posBS <> 0 then
	                    CreateFolders strDest & "\", Left(sLine, posBS - 1), False
	                end if
	                ForceCopy g_strTestPath & strArea & "\" & sLine, strDest & "\" & sLine
	            end if
	        end if
	    loop
	    fDep.close
	end if
end sub
 
' DeleteIfExist
Sub DeleteIfExist(strFile)
    DebugPrint "Trying to delete the file " & strFile
'   If sobjFS.FileExists(strFile) Then
        sobjFS.DeleteFile strFile, True
'   End If
    On Error Resume Next
End Sub ' DeleteIfExist

' ForceCopy
Sub ForceCopy(strSrc, strDest)
    'DebugPrint "ForceCopy " & strSrc & " to " & strDest
    Dim fFile

    If sobjFS.FileExists(strDest) Then
        sobjFS.DeleteFile strDest, True
    End If

    If Not sobjFS.FileExists(strSrc) Then
        DebugPrint "ForceCopy: Source File Does Not Exist: " & strSrc
        Exit Sub
    End If

    sobjFS.CopyFile strSrc, strDest, True

    Set fFile = sobjFS.GetFile(strDest)
    fFile.Attributes = 0
End Sub ' ForceCopy

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

sub CreateFolder(szFolder)
    if not sobjFS.FolderExists(szFolder) then sobjFS.CreateFolder szFolder
end sub

sub CreateFolders(szRoot, szPath, fClearContents)
    dim i

    If fClearContents And sobjFS.FolderExists(szRoot & szPath) Then
        On Error Resume Next
        Dim szDeletePath

        If Right(szRoot & szPath, 1) = "\" Then
            szDeletePath = Left(szRoot & szPath, Len(szRoot & szPath) - 1)
        Else
            szDeletePath = szRoot & szPath
        End If

        sobjFS.DeleteFolder szDeletePath, True
        If Err.Number <> 0 Then
            Wscript.Echo "Error deleting path '" & szDeletePath & "' due to error number " & Err.Number & ": " & Err.Description & ". Check that the directory is not in use."
        End If
        On Error Goto 0
    End If
    
    i = 0
    do 
        i = instr(i + 1, szPath, "\")
        if i <> 0 then CreateFolder szRoot & left(szPath, i - 1)
    loop until i = 0
    CreateFolder szRoot & szPath
end sub

sub DebugPrint(s)
    'wscript.echo s
end sub

function RegExReplace( f_strSrc, f_strPattern, f_strReplace )
    dim regex
    set regex = new RegExp
    regex.Pattern = f_strPattern
    regex.IgnoreCase = True

    RegExReplace = regex.Replace(f_strSrc, f_strReplace)
end function

function IsTestCaseString( f_strSrc )
   dim regex
   set regex = new RegExp
   regex.Pattern = "^\d+(-\d+)?"

   IsTestCaseString = regex.Test( f_strSrc )
end function


