// scope_io.cpp : Windows NT api i/o functions
//
#include "stdafx.h"
#include "scope_nt.h"

HANDLE      hCom;               // global COM pot handle
HANDLE      hRWThread   [2];    // global R/W threads (0 for read, 1 for write)
HANDLE      hREvent, hWEvent;   // global R/W events

BYTE        readBuffer  [MAX_READ_BUFFER]   = {0};
BYTE        writeBuffer [MAX_WRITE_BUFFER]  = {0};

DWORD       dwBytesRead     = 0;
DWORD       dwBytesWrite    = 0;


/* PREDEFINE */
DWORD WINAPI read_thread    (LPVOID );
DWORD WINAPI write_thread   (LPVOID );

int scope_get_avail_com_port(SERIAL_INFO *portInfo, int *portCount)
{
    HKEY    hKey;
    DWORD   retCode;

    TCHAR   subKeyName[MAX_KEY_LENGTH];
    TCHAR   subKeyData[MAX_VALUE_NAME];

    DWORD   subKeyNameSize, subkeyDataSize;

    retCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey);
    if (retCode == ERROR_SUCCESS)
    {
        for (DWORD subKeyIndex = 0, retCode = ERROR_SUCCESS; ; subKeyIndex++) 
        {
            subKeyNameSize = MAX_KEY_LENGTH;
            subkeyDataSize = MAX_VALUE_NAME;

            retCode = RegEnumValue(hKey, subKeyIndex, 
                subKeyName,
                &subKeyNameSize,
                NULL,
                NULL,
                (LPBYTE)subKeyData,
                &subkeyDataSize);

            if (retCode == ERROR_SUCCESS)
            {
                // copy registry value into our multi-byte var
                WideCharToMultiByte(CP_OEMCP, 0, subKeyName, -1, 
                    portInfo[subKeyIndex].deviceName, MAX_KEY_LENGTH, NULL, NULL);

                WideCharToMultiByte(CP_OEMCP, 0, subKeyData, -1, 
                    portInfo[subKeyIndex].portName, MAX_VALUE_NAME, NULL, NULL);

                (*portCount) ++;
            }
            else
            {
                break; // exit for
            }
        }
        RegCloseKey(hKey);
    }

    return 1;
}

int scope_open_com_port(char *portNumber)
{
    DCB     dcb;

    TCHAR szPortNumber[MAX_PATH];

    MultiByteToWideChar(CP_OEMCP, 0, portNumber, -1, szPortNumber, MAX_PATH);

    hCom = CreateFile(szPortNumber,  
        GENERIC_READ | GENERIC_WRITE, 
        0,          // exclusive access 
        NULL,       // default security attributes 
        OPEN_EXISTING,
        NULL,
        NULL);
    if (hCom == INVALID_HANDLE_VALUE)
        return 0;

// configure comm state:
    if (!GetCommState(hCom, &dcb)) 
        return 0;
    if (!BuildCommDCB(COM_CONFIG, &dcb)) 
        return 0;
    if (!SetCommState(hCom, &dcb)) 
        return 0;

    if (!SetCommMask(hCom, EV_RXCHAR))
        return 0;

// configure timeouts:
    COMMTIMEOUTS commTimeouts;
    if (!GetCommTimeouts(hCom, &commTimeouts))
        return 0;

// ensure read is completed periodically:
    commTimeouts.ReadIntervalTimeout        = 0;    // interval time-outs not used
    commTimeouts.ReadTotalTimeoutConstant   = READ_CONST;
    commTimeouts.ReadTotalTimeoutMultiplier = READ_MULTI;
    
//  commTimeouts.WriteTotalTimeoutConstant      = 1;
//  commTimeouts.WriteTotalTimeoutMultiplier    = 0;

    if (!SetCommTimeouts(hCom, &commTimeouts))
        return 0;

// clear err to ensure serial r/w operations successful:
    DWORD commErr;
    if (!ClearCommError(hCom, &commErr, NULL))
        return 0;

    return 1;
}

int scope_close_com_port()
{
    CloseHandle(hCom);

    CloseHandle(hRWThread[0]);  // R thread
    CloseHandle(hRWThread[1]);  // W thread
    return 1;
}

int scope_read_port(BYTE *buffer)
{
    return 1;
}

int scope_write_port(BYTE *buffer, DWORD packetLen)
{
// ***TEMP
    //DWORD PKG_LEN = strlen((char *)buffer);
    //WriteFile(hCom, buffer, packetLen, &dwBytesWrite, NULL);
    return 1;
}

int scope_create_rw_threads()
{
// Create read event:
    hREvent = CreateEvent(NULL, FALSE, FALSE, NULL);
// Create write event:
    hWEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

// Create read thread:
    hRWThread[0] = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size
        read_thread,            // thread function name
        NULL,                   // argument to thread function
        0,                      // use default creation flags
        NULL);                  // returns the thread identifier

// Create write thread:
    hRWThread[1] = CreateThread(NULL, 0, write_thread, NULL, 0, NULL);

    return 1;
}

DWORD WINAPI read_thread(LPVOID lpParam)
{
#define WAIT_HANDLES        1

    BOOL        fThreadExit     = FALSE;    // exit read thread before exit the app

    BOOL        fOverlapped     = FALSE;
    OVERLAPPED  readOverlapped  = {0};

    HANDLE      hWAIT[WAIT_HANDLES];
    DWORD       dwWaitObjectID;

    readOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (readOverlapped.hEvent == NULL)
        return 0;

    hWAIT[0] = readOverlapped.hEvent;
    hWAIT[1] = hAppExitEvent;

    while (!fThreadExit)
    {
        // if no read is outstanding, then issue another one
        // ...
        if (!fOverlapped)
        {
            if (!ReadFile(hCom, readBuffer, MAX_READ_BUFFER, &dwBytesRead, &readOverlapped))
            {
                if (GetLastError() != ERROR_IO_PENDING) // IO_PENDING: read in progress, we'll be notified when read done
                {
                    return 0;               // read failed
                }
                else
                {
                    fOverlapped = TRUE;     // *read has been queued and will complete in the future
                }
            }
            else
            {   
                fOverlapped = FALSE;        // *read has completed immediately
                if (dwBytesRead != MAX_READ_BUFFER)
                {
                    OutputDebugString(_T("Read done immediately\n"));
                }

                if (dwBytesRead)
                {
                    _tprintf(_T("Got a packet: %02X %02X\n"), readBuffer[0], readBuffer[1]);
                    SetEvent(hREvent);      // tell there is a write happen
                }
            }
        }

        //
        // wait for pending operations to complete
        //
        if (fOverlapped)
        {
            dwWaitObjectID = WaitForMultipleObjects(WAIT_HANDLES, hWAIT, FALSE, COM_CHECK_TIMEOUT);
            switch(dwWaitObjectID)
            {
            case WAIT_OBJECT_0:
                if (!GetOverlappedResult(hCom, &readOverlapped, &dwBytesRead, FALSE))
                {
                    if (GetLastError() == ERROR_OPERATION_ABORTED)
                        OutputDebugString(_T("Read aborted\n"));
                    else
                        OutputDebugString(_T("GetOverlappedResult (in Reader)\n"));
                }
                else
                {   // read completed successfully
                    if (dwBytesRead != MAX_READ_BUFFER)
                        OutputDebugString(_T("Read timed out overlapped\n"));

                    if (dwBytesRead)
                        _tprintf(_T("Got a packet: %02X %02X...\n"), readBuffer[0], readBuffer[1]);
                }

                fOverlapped = FALSE;
                break;
            case WAIT_OBJECT_0 + 1:
                fThreadExit = TRUE;
                break;
            case WAIT_TIMEOUT:
                // timeouts are not reported because they happen too often
                OutputDebugString(_T("read timeout checked\n"));
                break;                       
            default:
                // err
                break;
            }
        }
    }
    // close event handles:
    CloseHandle(readOverlapped.hEvent);

    _tprintf(_T("\n"));
    return 1;
}

DWORD WINAPI write_thread(LPVOID lpParam)
{
    BOOL fThreadExit        = FALSE;    // exit read thread before exit the app

    while (!fThreadExit)
    {
        WaitForSingleObject(hWEvent, INFINITE);
        //WriteFile(hCom, writeBuffer, MAX_WRITE_BUFFER, &dwBytesWrite, );
    }

    return 1;
}
