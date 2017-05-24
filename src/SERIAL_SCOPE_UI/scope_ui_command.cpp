// scope_ui_command.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "scope_ui_main.h"
#include "scope_ui_command.h"

#include "scope_ui_xml_parser.h"

// forward declarations of functions included in this code module:
void refresh_undo           ();
void open_save_box          (TCHAR *, BOOL );

// highlight syntax for the XML script
void highlight_xml()
{

    return ;
}


void refresh_tools()
{
    refresh_undo();
    return ;
}

void refresh_undo()
{
    DWORD fUndoState;
    if (SendMessage(hWndRichedit, EM_CANUNDO, 0, 0))
    {   // Can undo
        fUndoState = TBSTATE_ENABLED;
    }
    else
    {
        fUndoState = TBSTATE_INDETERMINATE;
    }

    SendMessage(hWndToolbar, TB_SETSTATE, IDM_UNDO, fUndoState);
    return ;
}

void refresh_caret_pos()
{
    CHARRANGE chRange;
    DWORD lineIndex     = SendMessage(hWndRichedit, EM_LINEINDEX    , (WPARAM)-1, 0 );
    DWORD lineFromChar  = SendMessage(hWndRichedit, EM_LINEFROMCHAR , (WPARAM)-1, 0 );
    DWORD caretFromChar;
    SendMessage(hWndRichedit, EM_GETSEL, NULL , (LPARAM)&caretFromChar);

    TCHAR szStat[MAX_LOADSTRING];

    _stprintf(szStat, _T("Ln %d"), lineFromChar + 1);
    SendMessage(hWndStatusbar, SB_SETTEXT, 1, (LPARAM)szStat);

    _stprintf(szStat, _T("Col %d"), caretFromChar - lineIndex + 1);
    SendMessage(hWndStatusbar, SB_SETTEXT, 2, (LPARAM)szStat);

//  TCHAR outer[MAX_PATH];
//  _stprintf(outer, _T("%d"), caretFromChar - lineIndex);
//  OutputDebugString(outer);
    return ;
}

void cmd_new()
{
    scope_print(_T("COMMAND_NEW\n"));

    TCHAR szPrompt[MAX_LOADSTRING];
    LoadString(hInst, IDS_PROMPT_NEW, szPrompt, MAX_LOADSTRING);

    switch (MessageBox(hWnd, szPrompt, szTitle, MB_ICONEXCLAMATION | MB_YESNOCANCEL))
    {
    case IDYES:
        cmd_save();
        break;
    case IDNO:
        break;
    }

    return ;
}

void cmd_open()
{
    scope_print(_T("COMMAND_OPEN\n"));

    TCHAR szFile[MAX_PATH] = { 0 };
    open_save_box(szFile, FALSE);

    scope_print(szFile);

    gui_xml_load_file(szFile);
    return ;
}

void cmd_save()
{
    scope_print(_T("COMMAND_SAVE\n"));

    if (TRUE)
    {
        cmd_saveas();
        statusbar_update(IDS_STAT_SAVED);
    }

    return ;
}

void cmd_saveas()
{
    scope_print(_T("COMMAND_SAVEAS\n"));

    TCHAR szFile[MAX_PATH] = { 0 };
    open_save_box(szFile, TRUE);

    scope_print(szFile);
    return ;
}

void cmd_cut()
{
    scope_print(_T("COMMAND_CUT\n"));

    SendMessage(hWndRichedit, WM_CUT, 0, 0);
    return ;
}

void cmd_copy()
{

    scope_print(_T("COMMAND_COPY\n"));

    SendMessage(hWndRichedit, WM_COPY, 0, 0);
    return ;
}

void cmd_paste()
{
    scope_print(_T("PASTE_COMMAND\n"));

    SendMessage(hWndRichedit, WM_PASTE, 0, 0);
    return ;
}

void cmd_del()
{
    scope_print(_T("COMMAND_DEL\n"));

    SendMessage(hWndRichedit, WM_CLEAR, 0, 0);
    return ;
}

void cmd_undo()
{
    scope_print(_T("COMMAND_UNDO\n"));

    SendMessage(hWndRichedit, EM_UNDO, 0, 0);
    refresh_undo();
    return ;
}

void cmd_redo()
{
    scope_print(_T("COMMAND_REDO\n"));
    
    SendMessage(hWndRichedit, EM_REDO, 0, 0);
    return ;
}

void cmd_run()
{
    scope_print(_T("COMMAND_RUN\n"));

    //gui_xml_load_file(_T("Script\\AIO_Script.xml"));

    //SendMessage(hWndStatusbar, SB_SETTEXT, 1 | SBT_NOBORDERS, (LPARAM)_T("asdfgh"));
    return ;
}

void cmd_pause()
{
    scope_print(_T("COMMAND_PAUSE\n"));

    return ;
}

void cmd_stop()
{
    scope_print(_T("COMMAND_STOP\n"));

    return ;
}

void cmd_step()
{
    scope_print(_T("COMMAND_STEP\n"));

    return ;
}

void open_save_box(TCHAR *path, BOOL isSave)
{
    OPENFILENAME    ofn                 = { 0 };    // common dialog box structure
    TCHAR           szFile[MAX_PATH]    = { 0 };    // buffer for file name

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize     = sizeof(ofn);
    ofn.hwndOwner       = hWnd;
    ofn.lpstrFile       = szFile;

    ofn.nMaxFile        = MAX_PATH;
    ofn.lpstrFilter     = XML_SCRIPT_FILTER;
    ofn.nFilterIndex    = 1;
    ofn.lpstrFileTitle  = NULL;
    ofn.nMaxFileTitle   = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags           = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    // Display the Open dialog box. 

    if (isSave)
    {
        if (GetSaveFileName(&ofn))
        {
            _tcscpy(path, ofn.lpstrFile);
        }
    }
    else
    {
        if (GetOpenFileName(&ofn))
        {
            _tcscpy(path, ofn.lpstrFile);
        }
    }
    return ;
}

DWORD CALLBACK richedit_stream_in_callback(DWORD_PTR dwCookie, LPBYTE lpBuff, LONG cb, PLONG pcb)
{ // We might do some XML syntax highlight here?
    HANDLE hFile = (HANDLE)dwCookie;
    if (ReadFile(hFile, lpBuff, cb, (DWORD *)pcb, NULL)) 
    {
        return 0;
    }
    return -1;
}

int gui_xml_load_file(TCHAR *szFile)
{ // once read, close immediately
    BOOL fSuccess = FALSE;
    HANDLE hFile = CreateFile(szFile, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
        FILE_FLAG_SEQUENTIAL_SCAN /* optimize for caching */, NULL);
    if (hFile != INVALID_HANDLE_VALUE) 
    {
        EDITSTREAM es       = { 0 };
        es.pfnCallback      = richedit_stream_in_callback;
        es.dwCookie         = (DWORD_PTR)hFile;
        if (SendMessage(hWndRichedit, EM_STREAMIN, SF_TEXT, (LPARAM)&es) && es.dwError == 0) 
        {
            fSuccess = TRUE;
        }
        CloseHandle(hFile);
    }
    return fSuccess;
}

DWORD CALLBACK richedit_stream_out_callback(DWORD_PTR dwCookie, LPBYTE lpBuff, LONG cb, PLONG pcb)
{
    HANDLE hFile = (HANDLE)dwCookie;
    if (ReadFile(hFile, lpBuff, cb, (DWORD *)pcb, NULL)) 
    {
        return 0;
    }
    return -1;
}

int gui_xml_parse()
{
    EDITSTREAM es       = { 0 };
    es.pfnCallback      = richedit_stream_out_callback;
//  es.dwCookie         = (DWORD_PTR)hFile;
    SendMessage(hWndRichedit, EM_STREAMOUT, SF_TEXT, (LPARAM)&es);
    return 1;
}
