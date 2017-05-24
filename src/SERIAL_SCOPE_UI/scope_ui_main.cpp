// SERIAL_SCOPE_UI.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "scope_ui_main.h"
#include "scope_ui_command.h"

#define WINDOW_MIN_WIDTH    720
#define WINDOW_MIN_HEIGHT   480

// Global Variables:
HINSTANCE hInst;                                // current instance
TCHAR szTitle       [MAX_LOADSTRING];           // The title bar text
TCHAR szWindowClass [MAX_LOADSTRING];           // the main window class name

HWND hWnd;
HWND hWndRebar;
HWND hWndToolbar; 
HWND hWndStatusbar;
HWND hWndRichedit;

HANDLE hWndConOut;      // console output screen buffer

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);

LRESULT CALLBACK    WndProc     (HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    dlgAbout    (HWND, UINT, WPARAM, LPARAM);


HWND create_scope_rebar     (HWND , HWND );
HWND create_scope_toolbar   (HWND );
HWND create_scope_statusbar (HWND );
HWND create_scope_richedit  (HWND );

// void scope_print(TCHAR *infoMessage) // output general debug info
// {
//  TCHAR *outputMessage = new TCHAR[_tcslen(infoMessage) + 1];     // add the "\n"
// 
//  _tcscpy(outputMessage, infoMessage);
//  _tcscat(outputMessage, _T("\n"));
// 
//  WriteConsole(hWndConOut, outputMessage, _tcslen(outputMessage), NULL, NULL);
//  return ;
// }

void scope_print(const TCHAR * szFormat, ...)
{
    TCHAR szBuffer[MAX_GENERAL_LENGTH];

    va_list pArgs;
    va_start(pArgs, szFormat);
    _vstprintf(szBuffer, szFormat, pArgs);
    va_end(pArgs);

    WriteConsole(hWndConOut, szBuffer, _tcslen(szBuffer), NULL, NULL);
    return ;
}

void scope_print_error(int errNumber, TCHAR *errMessage)    // output error info
{
    TCHAR outputMessage[MAX_GENERAL_LENGTH];

    _stprintf(outputMessage, _T("ERROR %X: %s\n"), errNumber, errMessage);

    WriteConsole(hWndConOut, outputMessage, _tcslen(outputMessage), NULL, NULL);
    return ;
}

void scope_prompt_error(TCHAR *errMessage)  // used to pop up urgent error
{
    MessageBox(hWnd, errMessage, szTitle, MB_ICONEXCLAMATION);
    return ;
}

void scope_prompt_error(int errNumber, TCHAR *errMessage)   // prompt err with err number
{
    TCHAR szErrMessage[MAX_GENERAL_LENGTH];

    _stprintf(szErrMessage, _T("ERROR %X: %s\n"), errNumber, errMessage);

    MessageBox(hWnd, szErrMessage, szTitle, MB_ICONERROR);
    return ;
}

void statusbar_update(UINT uID)
{
    TCHAR szStat[MAX_LOADSTRING];
    LoadString(hInst, uID, szStat, MAX_LOADSTRING);
    SendMessage(hWndStatusbar, SB_SETTEXT, 0, (LPARAM)szStat);
}

void create_console()
{
    if (AllocConsole())
    {   // success
        hWndConOut = GetStdHandle(STD_OUTPUT_HANDLE);
    }
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    MSG msg;
    HACCEL hAccelTable;

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_SERIAL_SCOPE_UI, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERIAL_SCOPE_UI));

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = NULL; //CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERIAL_SCOPE_UI));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL; //MAKEINTRESOURCE(IDC_SERIAL_SCOPE_UI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindowEx(WS_EX_APPWINDOW, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    // Initialize common controls:
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC  = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_CREATE:
        {
            // create TOOLBAR:
            hWndToolbar = create_scope_toolbar(hWnd);

            // create REBAR:
            if (hWndToolbar)
            {
                hWndRebar = create_scope_rebar(hWnd, hWndToolbar);
            }

            // create STATUSBAR:
            hWndStatusbar = create_scope_statusbar(hWnd);

            // create RICHEDIT:
            hWndRichedit = create_scope_richedit(hWnd);


            refresh_tools();

            // set "Ready" to STATUSBAR:
            statusbar_update(IDS_STAT_READY);

            SendMessage(hWndStatusbar, SB_SETTEXT, 1, (LPARAM)_T("Ln 1"));
            SendMessage(hWndStatusbar, SB_SETTEXT, 2, (LPARAM)_T("Col 1"));

            // Set Version info to STATUSBAR:
            SendMessage(hWndStatusbar, SB_SETTEXT, 3 | SBT_NOBORDERS, (LPARAM)_T("AIO-Play Version 0.1 (Build 15563) Alpha"));  //(LPARAM)szTitle

            // create console window for debugging:
            create_console();

            //ShowWindow(hWndRebar, SW_HIDE);

            break;
        }
    case WM_COMMAND:
        {
            wmId    = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            
            if ((HWND)lParam == hWndRichedit)
            {
                switch (wmEvent)
                {
                case EN_CHANGE:
                    refresh_caret_pos();
                    break;
                }
            }
            else
            {   // Parse the menu selections:
                switch (wmId)
                {
                case IDM_NEW:
                    cmd_new();
                    break;
                case IDM_OPEN:
                    cmd_open();
                    break;
                case IDM_SAVE:
                    cmd_save();
                    break;
                case IDM_SAVEAS:
                    cmd_saveas();
                    break;
                case IDM_UNDO:
                    cmd_undo();
                    break;
                case IDM_REDO:
                    cmd_redo();
                    break;
                case IDM_CUT:
                    cmd_cut();
                    break;
                case IDM_COPY:
                    cmd_copy();
                    break;
                case IDM_PASTE:
                    cmd_paste();
                    break;
                case IDM_DEL:
                    cmd_del();
                    break;
                case IDM_RUN:
                    cmd_run();
                    break;
                case IDM_PAUSE:
                    cmd_pause();
                    break;
                case IDM_STOP:
                    cmd_stop();
                    break;
                case IDM_STEP:
                    cmd_step();
                    break;
                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, dlgAbout);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
                }
                break;
            }
        }
    case WM_GETMINMAXINFO:
        {
//              ((LPMINMAXINFO)lParam)->ptMinTrackSize.x = WINDOW_MIN_WIDTH;
//              ((LPMINMAXINFO)lParam)->ptMinTrackSize.y = WINDOW_MIN_HEIGHT;
//          LPMINMAXINFO pMinMaxNfo = (LPMINMAXINFO)lParam;
//          pMinMaxNfo->ptMinTrackSize.x = WINDOW_MIN_WIDTH;
//          pMinMaxNfo->ptMinTrackSize.y = WINDOW_MIN_HEIGHT;
            break;
        }
    case WM_NOTIFY:
        {

            TCHAR outer[MAX_PATH];
            _stprintf(outer, _T("%d"), wParam);
            OutputDebugString(outer);

            if ((HWND)wParam == hWndRichedit)
            {
                switch (HIWORD(lParam))
                {
                case EN_MSGFILTER:
                    MSGFILTER msgFilter;
                    /*SendMessage(hWnd, )*/
                    return 0;
                    break;
                }

                break;
            }
            switch (((LPNMHDR)lParam)->code)
            {   // Tooltip
            case TTN_GETDISPINFO: 
                { 
                    LPTOOLTIPTEXT lpTooltip = (LPTOOLTIPTEXT)lParam; 

                    // Set the instance of the module that contains the resource:
                    lpTooltip->hinst = hInst; 

                    UINT_PTR idButton = lpTooltip->hdr.idFrom; 
                    switch (idButton)
                    { 
                    case IDM_NEW:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_NEW);
                        break;
                    case IDM_OPEN:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_OPEN);
                        break;
                    case IDM_SAVE:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_SAVE);
                        break;
                    case IDM_SAVEAS:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_SAVEAS);
                        break;
                    case IDM_UNDO:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_UNDO);
                        break;
                    case IDM_RUN:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_RUN);
                        break;
                    case IDM_PAUSE:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_PAUSE);
                        break;
                    case IDM_STOP:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_STOP);
                        break;
                    case IDM_ABOUT:
                        lpTooltip->lpszText = MAKEINTRESOURCE(IDS_TIP_ABOUT);
                        break;
                        //case IDM_NEW
                    } 
                    break; 
                }
                break;
            }
        }
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;
    case WM_SIZE:
        {
#define PARTS_1_WIDTH       0x40
#define PARTS_2_WIDTH       0x40
#define PARTS_3_WIDTH       0xFF

            SendMessage(hWndRebar       , WM_SIZE, 0, 0);
            SendMessage(hWndStatusbar   , WM_SIZE, 0, 0);

            // Calculate the size of RICHEDIT:
            RECT rWindow, rRebar, rStatus, rRichedit;
            GetClientRect(hWnd          , &rWindow);    // Retrieve client area
            GetClientRect(hWndRebar     , &rRebar);
            GetClientRect(hWndStatusbar , &rStatus);

            rRichedit.left      = 0;
            rRichedit.top       = rRebar.bottom;
            rRichedit.right     = rWindow.right;
            rRichedit.bottom    = rWindow.bottom - rStatus.bottom;

            MoveWindow(hWndRichedit, 
                (int)rRichedit.left,
                (int)rRichedit.top,
                (int)(rRichedit.right - rRichedit.left),
                (int)(rRichedit.bottom - rRichedit.top),
                FALSE);

            int partsWidth[STATUSBAR_PARTS] = {
                rWindow.right - rWindow.left - PARTS_3_WIDTH - PARTS_2_WIDTH - PARTS_1_WIDTH,
                rWindow.right - rWindow.left - PARTS_3_WIDTH - PARTS_2_WIDTH,
                rWindow.right - rWindow.left - PARTS_3_WIDTH,
                -1 };

            SendMessage(hWndStatusbar, SB_SETPARTS, (WPARAM)STATUSBAR_PARTS, (LPARAM)partsWidth);

            break;
        }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box:
INT_PTR CALLBACK dlgAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    case WM_CTLCOLORBTN:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORSTATIC:
        return (INT_PTR)GetStockObject(WHITE_BRUSH);
        break;
    case WM_NOTIFY:
        switch (((LPNMHDR)lParam)->code)
        {
        case NM_CLICK:
        case NM_RETURN:
            ShellExecute(NULL, _T("open"), _T("http://www.alps.com"), NULL, NULL, SW_SHOW);
        }
        break;
    }
    return (INT_PTR)FALSE;
}

HWND create_scope_rebar(HWND hWndParent, HWND hwndToolbar)
{
#define NUMBUTTONS 9

    // Check for Toolbar:
    if ((hwndToolbar == NULL))
    {
        return NULL;
    }

    // Create the rebar:
    HWND hwndRebar = CreateWindowEx(0, REBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
        RBS_BANDBORDERS | RBS_VARHEIGHT | CCS_NODIVIDER | CCS_TOP,
        0, 0, 0, 0, hWndParent, NULL, hInst, NULL);

    if(!hwndRebar)
    {
        return NULL;
    }

    // Initialize band info used by both bands:
    REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };
    rbBand.cbSize   = sizeof(REBARBANDINFO);
    rbBand.fMask    = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE;
    rbBand.fStyle   = RBBS_GRIPPERALWAYS | RBBS_USECHEVRON | RBBS_CHILDEDGE;  

    // Get the height of the toolbar:
    DWORD dwBtnSize = (DWORD)SendMessage(hwndToolbar, TB_GETBUTTONSIZE, 0, 0);

    // Set values unique to the band with the toolbar:
    rbBand.lpText       = TEXT("TOOLBRA");
    rbBand.hwndChild    = hwndToolbar;
    rbBand.cx           = 0;
    rbBand.cyChild      = HIWORD(dwBtnSize);
    rbBand.cxMinChild   = NUMBUTTONS * LOWORD(dwBtnSize);
    rbBand.cyMinChild   = HIWORD(dwBtnSize);

    rbBand.cxIdeal      = NUMBUTTONS * LOWORD(dwBtnSize);

    // Add the band that has the toolbar:
    if( !SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand))
    {
        return NULL;
    }

    return hwndRebar;
}

HWND create_scope_toolbar(HWND hWndParent)
{
// TOOLBAR:
//  NEW | OPEN | SAVE | SAVEAS | UNDO | RUN | BREAK | STOP | ABOUT

#define TOOLBAR_BUTTONS     12

    // Create the toolbar:
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, 
        WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_TRANSPARENT | WS_CLIPCHILDREN |
        WS_CLIPSIBLINGS | CCS_NODIVIDER | CCS_NORESIZE | CCS_TOP | WS_VISIBLE,
        0, 0, 0, 0, hWndParent,
        NULL, hInst, NULL); // (HMENU) ID_TOOLBAR
    if (hWndToolbar == NULL)
    {
        return NULL;
    }

    // Create the imagelist:
    HIMAGELIST hImageList = ImageList_Create(64, 64, ILC_COLOR32, TOOLBAR_BUTTONS, 0);

    // Set the image list.
    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImageList);   // (WPARAM)0 = Only one imagelist

    // Load the text from a resource:
    // In the string table, the text for all buttons is a single entry that 
    // appears as "~New~Open~Save~~". The separator character is arbitrary, 
    // but it must appear as the first character of the string. The message 
    // returns the index of the first item, and the items are numbered 
    // consecutively.
    int iToolbra = SendMessage(hWndToolbar, TB_ADDSTRING, (WPARAM)hInst, (LPARAM)IDS_TOOLBRA); 

    TBBUTTON tbButtons[TOOLBAR_BUTTONS] = {
        { 0             , IDM_NEW       , TBSTATE_ENABLED       , BTNS_BUTTON   , {0}, 0, iToolbra          },
        { 1             , IDM_OPEN      , TBSTATE_ENABLED       , BTNS_BUTTON   , {0}, 0, iToolbra + 1      },
        { 2             , IDM_SAVE      , TBSTATE_ENABLED       , BTNS_BUTTON   , {0}, 0, iToolbra + 2      },
        { 3             , IDM_SAVEAS    , TBSTATE_ENABLED       , BTNS_BUTTON   , {0}, 0, iToolbra + 3      },
        { I_IMAGENONE   , 0             , TBSTATE_ENABLED       , BTNS_SEP      , {0}, 0, (INT_PTR)_T("")   },
        { 4             , IDM_UNDO      , TBSTATE_ENABLED       , BTNS_BUTTON   , {0}, 0, iToolbra + 4      },
        { I_IMAGENONE   , 0             , TBSTATE_ENABLED       , BTNS_SEP      , {0}, 0, (INT_PTR)_T("")   },
        { 5             , IDM_RUN       , TBSTATE_ENABLED       , BTNS_BUTTON   , {0}, 0, iToolbra + 5      },
        { 6             , IDM_PAUSE     , TBSTATE_INDETERMINATE , BTNS_BUTTON   , {0}, 0, iToolbra + 6      },
        { 7             , IDM_STOP      , TBSTATE_INDETERMINATE , BTNS_BUTTON   , {0}, 0, iToolbra + 7      },
        { I_IMAGENONE   , 0             , TBSTATE_ENABLED       , BTNS_SEP      , {0}, 0, (INT_PTR)_T("")   },
        { 8             , IDM_ABOUT     , TBSTATE_ENABLED       , BTNS_BUTTON   , {0}, 0, iToolbra + 8      }
    };

    // Send the TB_BUTTONSTRUCTSIZE message, which is required for backward compatibility:
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0); 

    // Load the icon resources, and add the icons into image list
    HANDLE hIcoNew      = LoadImage(hInst, MAKEINTRESOURCE(IDI_NEW  ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    HANDLE hIcoOpen     = LoadImage(hInst, MAKEINTRESOURCE(IDI_OPEN ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    HANDLE hIcoSave     = LoadImage(hInst, MAKEINTRESOURCE(IDI_SAVE ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    HANDLE hIcoSaveAs   = LoadImage(hInst, MAKEINTRESOURCE(IDI_SAAS ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    HANDLE hIcoUndo     = LoadImage(hInst, MAKEINTRESOURCE(IDI_UNDO ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    HANDLE hIcoRun      = LoadImage(hInst, MAKEINTRESOURCE(IDI_RUN  ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    HANDLE hIcoPause    = LoadImage(hInst, MAKEINTRESOURCE(IDI_PAUS ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    HANDLE hIcoStop     = LoadImage(hInst, MAKEINTRESOURCE(IDI_STOP ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
    HANDLE hIcoHelp     = LoadImage(hInst, MAKEINTRESOURCE(IDI_HELP ), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

    ImageList_AddIcon(hImageList, (HICON)hIcoNew    );
    ImageList_AddIcon(hImageList, (HICON)hIcoOpen   );
    ImageList_AddIcon(hImageList, (HICON)hIcoSave   );
    ImageList_AddIcon(hImageList, (HICON)hIcoSaveAs );
    ImageList_AddIcon(hImageList, (HICON)hIcoUndo   );
    ImageList_AddIcon(hImageList, (HICON)hIcoRun    );
    ImageList_AddIcon(hImageList, (HICON)hIcoPause  );
    ImageList_AddIcon(hImageList, (HICON)hIcoStop   );
    ImageList_AddIcon(hImageList, (HICON)hIcoHelp   );

    // Add buttons
    SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)TOOLBAR_BUTTONS, (LPARAM) (LPTBBUTTON) &tbButtons);

    // Tell the toolbar to resize itself
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
    return hWndToolbar; 
}


HWND create_scope_statusbar(HWND hWndParent) 
{
    HWND hWndStatusbar = CreateWindowEx(0, STATUSCLASSNAME, (LPCTSTR) NULL,
        SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, 
        hWndParent, NULL, hInst, NULL);
    
    int partsWidth[STATUSBAR_PARTS] = { 0, 0, 0, 0 };
 
    // Tell the status bar to create the window parts
    SendMessage(hWndStatusbar, SB_SETPARTS, (WPARAM)STATUSBAR_PARTS, (LPARAM)partsWidth);

    return hWndStatusbar; 
}

HWND create_scope_richedit(HWND hWndParent)
{
    LoadLibrary(_T("msftedit.dll"));

    HWND hWndRichedit= CreateWindowEx(0, MSFTEDIT_CLASS, NULL,
        ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL | ES_WANTRETURN |
        WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 0, 0, 0,
        hWndParent, NULL, hInst, NULL);

    // set the event mask to make parent window receive notification message:
    SendMessage(hWndRichedit, EM_SETEVENTMASK, 0, ENM_CHANGE | EN_SELCHANGE);

    // set the default char format:
    CHARFORMAT chFormat = { 0 };
    chFormat.cbSize     = sizeof(CHARFORMAT);
    chFormat.dwMask     = CFM_BOLD | CFM_FACE | CFM_SIZE;
    chFormat.dwEffects  = !CFE_BOLD;
    chFormat.yHeight    = 200;
    _tcscpy(chFormat.szFaceName, _T("Inconsolata"));

    SendMessage(hWndRichedit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&chFormat);

    return hWndRichedit;
}
