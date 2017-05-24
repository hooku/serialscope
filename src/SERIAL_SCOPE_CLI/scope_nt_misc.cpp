// scope_nt.cpp : Windows NT api call functions
//
#include "stdafx.h"
#include "scope_nt.h"
// #include <AtlConv.h>
// using namespace ATL;

void scope_print_error()
{
    _tprintf(_T("ERROR %d\n"), GetLastError());
    return ;
}

void scope_print_error(const char *errMessage)
{   
    TCHAR szErrMessage[MAX_GENERIC_LENGTH];
    MultiByteToWideChar(CP_OEMCP, 0, errMessage, -1, szErrMessage, MAX_GENERIC_LENGTH);

    _tprintf(_T("ERROR: %s\n"), szErrMessage);
    return ;
}

void scope_print_error(int errNumber, const char *errMessage)
{
    TCHAR szErrMessage[MAX_GENERIC_LENGTH];
    MultiByteToWideChar(CP_OEMCP, 0, errMessage, -1, szErrMessage, MAX_GENERIC_LENGTH);

    _tprintf(_T("ERROR %X: %s\n"), errNumber, szErrMessage);
    return ;
}

void *scope_create_event(bool bManualReset, bool bInitialState)
{
    return CreateEvent(NULL, bManualReset, bInitialState, NULL);    // always no name
}

bool scope_set_event(void *hEvent)
{
    return (bool)SetEvent(hEvent);
}

bool scope_reset_event(void *hEvent)
{
    return (bool)ResetEvent(hEvent);
}
