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


Sub DoWork

	Dim objMetering
	Dim strMeterChallenge
	Dim objArgs
	Dim objFSO
	Dim objFile

	Set objArgs = WScript.Arguments

	if objArgs.Length <> 2 then
		WScript.Echo "Usage:  genmeterresponse.vbs meterchallengefile meterresponsefile"
		exit sub
	end if
	    
	Set objFSO = CreateObject("Scripting.FileSystemObject")
	Set objFile = objFSO.OpenTextFile(objArgs(0),1, 0, -1)
	strMeterChallenge = objFile.ReadAll
	objFile.Close



	Set objMetering = CreateObject("wmrmobjs.wmrmMetering")


	objMetering.MeteringCertificate = "<METERCERT version=""1.0""><DATA><MID>pIKx8Wsy5aYRQN8LLIIcTQ==</MID><PUBLICKEY>nm0yQ6CXLcQIBf*Zc*VKGL!SziEdTT!ehGxDotUazwFtE7X40IlqAQ==</PUBLICKEY><URL>http://www.bad.contoso.com</URL></DATA><CERTIFICATECHAIN><CERTIFICATE>AAEAADgAAAB17bfg!cqDLB2clHGU7P6zkrOCVUddX95UYkqpjqvoIr4ezqw2zgwfrvKR1dq!EzdGD8ND2Ihkn0OPEoWZZKCwgidp7Y5SHR!NFJJazdPWfBQEBB4AAAABAAAAAQAAAAE=</CERTIFICATE></CERTIFICATECHAIN><SIGNATURE><HASHALGORITHM type=""SHA""></HASHALGORITHM><SIGNALGORITHM type=""MSDRM""></SIGNALGORITHM><VALUE>dvVMtj5p9o7I5IK2XIYwkQoF1Rli4cgpS1UkCjBx8KIr7SrPE9!0Vg==</VALUE></SIGNATURE></METERCERT>"
	objMetering.Challenge = strMeterChallenge
	objMetering.ServerPrivateKey =  "lVvvpn!q4bMJi!BosMpAPZ9IERg="

	WScript.Echo objMetering.GetMeteringDataAsXml() 

	Set objFile = objFSO.OpenTextFile(objArgs(1),2,-1, -1)                    
	objFile.Write objMetering.GetMeteringResponse()

	objFile.Close

End Sub

'Main program 

DoWork
