/*
  FILE : stub_pi.c
  PURPOSE: This file is a stub for linking tests.
*/
#include "va_pi.h"

INT VA_PI_ParseCatDescriptors ( DWORD dwAcsId )
{
  return 0;
}

INT VA_PI_ParsePmtProgramDescriptors( DWORD dwAcsId )
{
  return 0;
}

INT VA_PI_GetProgramInformation( DWORD dwAcsId, tVA_PI_ProgramInformation *pProgramInfo )
{
  return 0;
}

INT VA_PI_NotifyStreamEvent(DWORD dwStbStreamHandler, tVA_PI_StreamEvent eStreamEvent)
{
  return 0;
}

INT VA_PI_NotifyStreamRecordPolicy (DWORD dwStbStreamHandle, tVA_PI_StreamRecordPolicy eStreamRecordPolicy)
{
  return 0;
}

INT VA_PI_ParsePmtStreamDescriptors( DWORD dwStbStreamHandle )
{
  return 0;
}

INT VA_PI_GetStreamInformation( DWORD dwStbStreamHandle, tVA_PI_StreamInformation *pStreamInfo)
{
  return 0;
}

/* End of File */