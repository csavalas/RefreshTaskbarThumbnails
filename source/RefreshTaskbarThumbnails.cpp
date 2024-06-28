#define WINVER 0x0500
#include <windows.h>
#include <winuser.h>
#include <string>
#include <bits/stdc++.h>
#include <tchar.h>
#include <dwmapi.h>
#include <chrono>

using namespace std;

// Baseline Variables
const int max_window_count = 500;
HWND windows_minimized[max_window_count];
int windows_minimized_count = 0;
HWND windows[max_window_count];
int windows_count = 0;
int iterations = 3;
int waittime = 500;
int showui = 1;
vector<string> args;

// UI Variables
const WORD ID_txtEdit = 4;
typedef struct {int width, height;} dimensions;
dimensions mainWindow = {300, 150};
dimensions txtEdit = {150, 20};
HWND mainwindowHandle = NULL;
HWND txtEditHandle = NULL;
HWND progressHandle = NULL;

// Baseline Functions
static BOOL CALLBACK organizeWindows(HWND hwnd, LPARAM lParam);
void force_to_top(HWND hwnd);
vector<string> parseArgs(LPSTR lpCmdLine);
int getArg(string arg, int default_value);
void print_usage();

// UI Functions
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void initializeUI(HINSTANCE hInstance);

// Main Entry Point
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    // Gather information on open/minimized windows
    EnumWindows(organizeWindows, (LPARAM) nullptr);
    //Parse/act upon command line arguments
    args = parseArgs(lpCmdLine);
    if (!getArg("help", -1)) {
        print_usage();
    }
    iterations = getArg("iterations", iterations);
    waittime = getArg("waittime", waittime);
    if (iterations < 1 || waittime < 1) print_usage();
    showui = getArg("showui", showui);
    if (showui) initializeUI(hInstance);
    // Main loop
    for (int i = 0; i < iterations; i++) {
        using namespace chrono;
        steady_clock::time_point begin = steady_clock::now();
        SetWindowText(
            txtEditHandle,
            (
                wstring(L"Stage ")
                + to_wstring(i + 1)
                + wstring(L" of ")
                + to_wstring(iterations)
                + wstring(L"...")
            ).c_str()
        );
        // Show minimized windows  
        for (int j = 0; j < windows_minimized_count; j++) {
            ShowWindow(windows_minimized[j], SW_RESTORE);
            SendMessage(progressHandle, PBM_STEPIT, 0, 0);
        }
        steady_clock::time_point end = steady_clock::now();        
        Sleep(max(0, (int) (waittime - duration_cast<milliseconds>(end - begin).count())));
        // Minimize minimized windows, restoration order is reversed to preserve Z-order
        for (int j = windows_minimized_count - 1; j >= 0; j--) {
            force_to_top(windows_minimized[j]);
            ShowWindow(windows_minimized[j], SW_MINIMIZE);
            SendMessage(progressHandle, PBM_STEPIT, 0, 0);

        }
    }
    SetWindowText(txtEditHandle, L"Finalizing...");
    // Reset window animation setting to default
    for (int i = 0; i < windows_minimized_count; i++) {
        BOOL attrib = FALSE;
        DwmSetWindowAttribute(windows_minimized[i], DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));
    }
    // Re-activate active windows, restoration order is reversed to preserve Z-order
    for (int i = windows_count - 1; i >= 0; i--) {
        ShowWindow(windows[i], SW_MINIMIZE);
        ShowWindow(windows[i], SW_RESTORE);
        force_to_top(windows[i]);
        // Reset window animation setting to default  
        BOOL attrib = FALSE;
        DwmSetWindowAttribute(windows[i], DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));
    }
    DestroyWindow(mainwindowHandle);    
    return 0;
}

// Organize windows into active/minimized
static BOOL CALLBACK organizeWindows(HWND hwnd, LPARAM lParam) {
    // Gather window title
    const DWORD TITLE_SIZE = 1024;
    WCHAR windowTitle[TITLE_SIZE];
    GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);
    int length = ::GetWindowTextLength(hwnd);
    wstring title(&windowTitle[0]);
    // Only handle non-hidden windows 
    if (
        windows_minimized_count < max_window_count
        && IsWindowVisible(hwnd)
        && length
        && title != L"Program Manager"
    ) {
        // Disable window animation, must be re-enabled later
        BOOL attrib = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_TRANSITIONS_FORCEDISABLED, &attrib, sizeof(attrib));
        // Window is minimized           
        if (IsIconic(hwnd)) {
            windows_minimized[windows_minimized_count] = hwnd;
            windows_minimized_count++;
        // Window is active
        } else {
            windows[windows_count] = hwnd;
            windows_count++;            
        }
    }
    return TRUE;
}

// Fixes Strangeness where restoring does not always bring to top
void force_to_top(HWND hwnd) {
    DWORD dwExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowPos(
        hwnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE
    );
    // Remove HWND_TOPMOST if not initially present        
    if (!(dwExStyle & WS_EX_TOPMOST))
    {
        SetWindowPos(
            hwnd,
            HWND_NOTOPMOST,
            0, 0, 0, 0,
            SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE
        );
    }
}

// Parse arguments into an array, delimited by the space character
vector<string> parseArgs(LPSTR lpCmdLine) {
    vector<string> args;
    string str = lpCmdLine;
    int start, end;
    start = end = 0;
    char dl = ' ';
    while ((start = str.find_first_not_of(dl, end)) != string::npos) {
        end = str.find(dl, start);
        args.push_back(str.substr(start, end - start));
    }
    return args;
}

// Get value of specified argument or return provided default
int getArg(string arg, int default_value) {
    for (int i = 0; i < args.size(); i++) {
        if (args[i] == "--" + arg)
            if (default_value == -1)
                return 0;
            else if (i + 1 < args.size())
                return stoi(args[i + 1]);
    }
    return default_value;
}

// Show usage information to console
void print_usage() {
    AttachConsole(ATTACH_PARENT_PROCESS);
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    HWND Handle = GetConsoleWindow();
    freopen("CON", "w", stdout);
    printf("Usage: RefreshTaskbarThumbnails.exe --iterations (some number) --waittime (some number in ms) --showui (1 or 0) --help");
    exit(1);
}

// UI Callback Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
        case WM_CREATE:
            mainwindowHandle = hwnd;
            {
                RECT rect;
                GetWindowRect(hwnd, &rect);
                int xSize = rect.right - rect.left;
                int ySize = rect.bottom - rect.top;
                int x = (GetSystemMetrics(SM_CXSCREEN) - xSize) / 2;
                int y = (GetSystemMetrics(SM_CYSCREEN) - ySize) / 2;
                SetWindowPos(hwnd,0,x,y,0,0,SWP_NOZORDER|SWP_NOSIZE);
            }
            txtEditHandle = CreateWindow(
                WC_STATIC,
                TEXT("Initializing..."),
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                mainWindow.width * .5 - txtEdit.width * .5,
                mainWindow.height * .25,
                txtEdit.width,
                txtEdit.height,
                hwnd,
                (HMENU)ID_txtEdit,
                NULL,
                NULL
            );
            progressHandle = CreateWindow(
                PROGRESS_CLASS,
                TEXT(""),
                WS_CHILD | WS_VISIBLE | WS_BORDER | PBS_SMOOTH,
                mainWindow.width * .5 - txtEdit.width * .5,
                mainWindow.height * .5,
                txtEdit.width,
                txtEdit.height,
                hwnd,
                (HMENU)ID_txtEdit,
                NULL,
                NULL
            );
            SendMessage(progressHandle,(UINT)PBM_SETBKCOLOR,0,RGB(30,30,30));
            SendMessage(progressHandle,(UINT) PBM_SETBARCOLOR,0,(LPARAM)RGB(0,170,130));            
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Initialize UI Elements
void initializeUI(HINSTANCE hInstance) {
    MSG msg;
    WNDCLASS mainWindowClass = { 0 };
    mainWindowClass.lpszClassName = TEXT("JRH.MainWindow");
    mainWindowClass.hInstance = hInstance;
    mainWindowClass.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    mainWindowClass.lpfnWndProc = WndProc;
    mainWindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    mainWindowClass.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(0, 0, 0));
    RegisterClass(&mainWindowClass);
    CreateWindowEx(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        mainWindowClass.lpszClassName,
        L"Refresh Taskbar Thumbnails",
        WS_VISIBLE,
        0,
        0,
        mainWindow.width,
        mainWindow.height,
        NULL,
        0,
        hInstance,
        NULL
    );
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (msg.message == WM_TIMER) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    SendMessage(progressHandle, PBM_SETRANGE, 0, MAKELPARAM(0, windows_minimized_count * iterations * 2));
    SendMessage(progressHandle, PBM_SETSTEP, (WPARAM) 1, 0);
}