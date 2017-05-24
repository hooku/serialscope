// SERIAL_SCOPE.cpp : Defines the entry point for the console application
//
// SERIAL_SCOPE procedure:
//      1.Config -> 2.Create R/W Threads

#include "stdafx.h"

#ifdef WIN32
#include "scope_nt.h"
#else
#include "scope_linux.h"
#endif

#include "scope_xml_parser.h"
#include "scope_conf.h"

HANDLE hAppExitEvent;       // for GUI to trigger the exit event

int _tmain(int argc, _TCHAR* argv[])
{
    _tprintf(_T("SERIAL_SCOPE\n"));

    COLORREF a = RGB(236, 233, 216);

    hAppExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (hAppExitEvent == NULL)
    {
        scope_print_error();
        goto APP_END;
    }

// let's load the XML file first:
    if (!cli_xml_load_file("Script\\AIO_Script.xml"))
    {
        scope_print_error();
    }

// parse through the sections and validate the XML file:
    SERIAL_INFO xmlPortInfo = { 0 };
    if (!cli_xml_parse_sect_config(&xmlPortInfo))
    { // config section
        scope_print_error();
    }

    if (!cli_xml_parse_sect_event())
    { // event section
        scope_print_error();
    }

    if (!cli_xml_parse_sect_script())
    { // script section
        scope_print_error();
    }

    SERIAL_INFO osPortInfo[MAX_PORT]    = { 0 };
    int         osPortCount             = 0;

// retrieve the OS serial port info:
    if (!scope_get_avail_com_port(osPortInfo, &osPortCount))
    {
        scope_print_error();
        goto APP_END;
    }

// validate the existence of serial port:
    bool portValid = false;
    for (int portIndex = 0; portIndex < osPortCount; portIndex ++)
    {
        if (!_stricmp(xmlPortInfo.portName, osPortInfo[portIndex].portName))
        { // Got it
            portValid = true;
            break;
        }
    }
    if (!portValid)
    {
        scope_print_error();
    }

//// ask which serial port to use:
//  for (int portIndex = 0; portIndex < portCount; portIndex ++)
//  {
//      _tprintf(_T("%d.%s: %s\n"), portIndex + 1, portInfo[portIndex].portName, portInfo[portIndex].deviceName);
//  }
//
//  _tprintf(_T("\n"));
//
//  _tprintf(_T("Which port to use?\n[1-%d]:"), portCount);
//
//  int portSelectedIndex = 2;
//  _tscanf(_T("%d"), &portSelectedIndex);
//  if (portSelectedIndex <= 0 || portSelectedIndex > portCount)
//  {
//      _tprintf(_T("INPUT ERR\n"));
//      goto APP_END;
//  }
//
//  portSelectedIndex --;
//
//  _tprintf(_T("\n"));
//
// initialize the serial port:



    printf("Open %s...", xmlPortInfo.portName);
    if (scope_open_com_port(xmlPortInfo.portName))
    {
        printf("Success\n", xmlPortInfo.portName);
    }
    else
    {
        scope_print_error();
        goto APP_END;
    }

    printf("Create R/W Threads...");
    if (scope_create_rw_threads())
    {
        printf("Success\n");
    }
    else
    {
        scope_print_error();
        goto APP_END;
    }


APP_END:

    Sleep(2000);
    _tprintf(_T("Exiting...\n"));
    Sleep(5000);

    return 0;
}
