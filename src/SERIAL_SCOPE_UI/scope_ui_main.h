#pragma once

#include "resource.h"

#define MAX_GENERAL_LENGTH  1024    // 1K

#define MAX_LOADSTRING      100
#define XML_SCRIPT_FILTER   _T("XML Script (*.xml)\0*.xml\0All Files (*.*)\0*.*\0")

#define STATUSBAR_PARTS     4

extern TCHAR szTitle        [MAX_LOADSTRING];

extern HINSTANCE    hInst;
extern HWND         hWnd;
extern HWND         hWndRebar;
extern HWND         hWndToolbar; 
extern HWND         hWndStatusbar;
extern HWND         hWndRichedit;

void scope_print                (const TCHAR *, ...);
void scope_print_error      (int , TCHAR *);
void scope_prompt_error     (TCHAR *);
void scope_prompt_error     (int , TCHAR *);
void statusbar_update       (UINT uID);
