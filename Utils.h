#pragma once
#include "GameConfig.h"

// Global offset for centering game content in the console
inline int g_OffsetX = 0;
inline int g_OffsetY = 0;

inline void GotoXY(int x, int y) {
    COORD coord;
    coord.X = (SHORT)(x + g_OffsetX);
    coord.Y = (SHORT)(y + g_OffsetY);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// === Console window lock: subclass to enforce MINIMUM size only ===
// Cho phep keo to hon kich thuoc game, NHUNG khong cho keo nho hon.
inline WNDPROC g_OrigConsoleWndProc = nullptr;
inline int g_LockedW = 0;   // chieu rong pixel TOI THIEU (= kich thuoc game)
inline int g_LockedH = 0;   // chieu cao pixel TOI THIEU

inline LRESULT CALLBACK LockedConsoleProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SIZING: {
        // Chi chan khi nguoi choi dang keo NHO HON kich thuoc toi thieu
        RECT* pRect = (RECT*)lParam;
        int width  = pRect->right  - pRect->left;
        int height = pRect->bottom - pRect->top;

        // Xu ly chieu ngang
        if (width < g_LockedW) {
            // Neu keo canh trai -> dich left, neu keo canh phai -> dich right
            if (wParam == WMSZ_LEFT || wParam == WMSZ_TOPLEFT || wParam == WMSZ_BOTTOMLEFT) {
                pRect->left = pRect->right - g_LockedW;
            } else {
                pRect->right = pRect->left + g_LockedW;
            }
        }

        // Xu ly chieu doc
        if (height < g_LockedH) {
            if (wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT) {
                pRect->top = pRect->bottom - g_LockedH;
            } else {
                pRect->bottom = pRect->top + g_LockedH;
            }
        }
        return TRUE;
    }
    case WM_GETMINMAXINFO: {
        // Chi khoa MIN size = kich thuoc game
        // MAX size de rat lon de cho phep keo to
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = g_LockedW;
        mmi->ptMinTrackSize.y = g_LockedH;
        mmi->ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN);
        mmi->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN);
        return 0;
    }
    case WM_WINDOWPOSCHANGING: {
        // Chi ep kich thuoc neu dang co gang nho hon min
        WINDOWPOS* wp = (WINDOWPOS*)lParam;
        if (!(wp->flags & SWP_NOSIZE)) {
            if (wp->cx < g_LockedW) wp->cx = g_LockedW;
            if (wp->cy < g_LockedH) wp->cy = g_LockedH;
        }
        break;
    }
    }
    return CallWindowProcA(g_OrigConsoleWndProc, hWnd, msg, wParam, lParam);
}

inline void FixConsoleWindow() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hIn  = GetStdHandle(STD_INPUT_HANDLE);
    HWND hWnd = GetConsoleWindow();

    SetConsoleTitleA("Crossing Road Game");

    // Detect Windows Terminal via WT_SESSION environment variable
    char* wtSession = nullptr;
    size_t wtLen = 0;
    _dupenv_s(&wtSession, &wtLen, "WT_SESSION");
    bool isWT = (wtSession != nullptr);
    free(wtSession);
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    // ===== STEP 1: Set large font (works in legacy console, ignored by WT) =====
    // Use tight width ratio (0.48) to maximize font size for Consolas
    int maxFontByW = (int)(screenW / (SCREEN_WIDTH * 0.48));
    int maxFontByH = (screenH - 40) / SCREEN_HEIGHT;
    int fontSize = (maxFontByW < maxFontByH) ? maxFontByW : maxFontByH;
    if (fontSize < 16) fontSize = 16;
    if (fontSize > 72) fontSize = 72;

    CONSOLE_FONT_INFOEX cfi = {};
    cfi.cbSize = sizeof(cfi);
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = (SHORT)fontSize;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(hOut, FALSE, &cfi);

    // ===== STEP 2: Maximize the window FIRST =====
    HWND hTop = GetAncestor(hWnd, GA_ROOT);
    if (!hTop) hTop = hWnd;

    if (!isWT) {
        // Legacy console: try true fullscreen mode first
        COORD newSz;
        if (!SetConsoleDisplayMode(hOut, CONSOLE_FULLSCREEN_MODE, &newSz)) {
            // Fullscreen not supported, just maximize
            ShowWindow(hWnd, SW_MAXIMIZE);
        }
    } else {
        // Windows Terminal: maximize the WT window, then zoom in
        ShowWindow(hTop, SW_MAXIMIZE);
        SetForegroundWindow(hTop);
        Sleep(400);  // Wait for WT to finish maximize animation

        // Zoom in aggressively until visible columns match game grid exactly
        // WT ignores SetCurrentConsoleFontEx, so we simulate keyboard zoom
        for (int i = 0; i < 50; i++) {
            CONSOLE_SCREEN_BUFFER_INFO csbi;
            GetConsoleScreenBufferInfo(hOut, &csbi);
            int visCols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int visRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

            // Stop when visible area fits the game grid tightly (plus border)
            if (visCols <= SCREEN_WIDTH + 2 || visRows <= SCREEN_HEIGHT + 2) break;

            // Send Ctrl+= (zoom in one step)
            keybd_event(VK_CONTROL, 0, 0, 0);
            keybd_event(VK_OEM_PLUS, 0, 0, 0);
            keybd_event(VK_OEM_PLUS, 0, KEYEVENTF_KEYUP, 0);
            keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
            Sleep(80);
        }
    }

    Sleep(200);  // Let window settle after maximize/zoom

    // ===== STEP 3: Calculate centering offset based on visible area =====
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);
    int visCols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int visRows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // Compute offset to center the game grid within the visible area
    // Guarantee at least 1 cell of offset for the border
    g_OffsetX = (visCols >= SCREEN_WIDTH + 2)  ? (visCols - SCREEN_WIDTH)  / 2 : 1;
    g_OffsetY = (visRows >= SCREEN_HEIGHT + 2) ? (visRows - SCREEN_HEIGHT) / 2 : 1;

    // Set buffer to match the full visible area (ensure minimum size for border)
    int finalCols = (visCols > SCREEN_WIDTH + 2) ? visCols : SCREEN_WIDTH + 2;
    int finalRows = (visRows > SCREEN_HEIGHT + 2) ? visRows : SCREEN_HEIGHT + 2;

    COORD bufSz = { (SHORT)finalCols, (SHORT)finalRows };
    SetConsoleScreenBufferSize(hOut, bufSz);

    SMALL_RECT winSz = { 0, 0, (SHORT)(finalCols - 1), (SHORT)(finalRows - 1) };
    SetConsoleWindowInfo(hOut, TRUE, &winSz);

    // ===== STEP 4: Window style cleanup =====
    LONG_PTR style = GetWindowLongPtrA(hWnd, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX | WS_VSCROLL | WS_HSCROLL);
    SetWindowLongPtrA(hWnd, GWL_STYLE, style);

    HMENU hMenu = GetSystemMenu(hWnd, FALSE);
    if (hMenu) {
        DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
    }

    // ===== STEP 5: Center the console window on screen =====
    RECT wr;
    GetWindowRect(hWnd, &wr);
    int wW = wr.right - wr.left;
    int wH = wr.bottom - wr.top;
    int posX = (screenW - wW) / 2;
    int posY = (screenH - wH) / 2 - 20;
    if (posX < 0) posX = 0;
    if (posY < 0) posY = 0;
    SetWindowPos(hWnd, NULL, posX, posY, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    // ===== STEP 6: Disable Quick Edit mode =====
    DWORD consoleMode;
    GetConsoleMode(hIn, &consoleMode);
    consoleMode &= ~ENABLE_QUICK_EDIT_MODE;
    consoleMode |= ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(hIn, consoleMode);

    // ===== STEP 7: Subclass for minimum size lock =====
    GetWindowRect(hWnd, &wr);
    g_LockedW = wr.right - wr.left;
    g_LockedH = wr.bottom - wr.top;
    g_OrigConsoleWndProc = (WNDPROC)SetWindowLongPtrA(hWnd, GWLP_WNDPROC, (LONG_PTR)LockedConsoleProc);
}

inline void HideCursor() {
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize   = 1;
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

inline void ShowCursor() {
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize   = 1;
    ci.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

inline void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (WORD)color);
}

inline void ResetColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
}

inline void ClearCell(int x, int y) {
    GotoXY(x, y);
    std::cout << ' ';
}

inline char getBackgroundChar(int x, int y) {
    if (y == 0) return ' ';  // HUD row - don't overwrite
    if (y == FINISH_Y) return '=';
    int lanes[] = { LANE_BIRD_Y - 1,  LANE_BIRD_Y + 1,
                    LANE_DINO_Y - 1,  LANE_DINO_Y + 1,
                    LANE_CAR_Y - 1,   LANE_CAR_Y + 1,
                    LANE_TRUCK_Y - 1, LANE_TRUCK_Y + 1 };
    for (int lane : lanes)
        if (y == lane) return '-';
    return ' ';
}

inline void RestoreCell(int x, int y) {
    char bg = getBackgroundChar(x, y);
    GotoXY(x, y);
    if (bg == '=') {
        SetColor(10); std::cout << '='; ResetColor();
    } else if (bg == '-') {
        SetColor(8); std::cout << '-'; ResetColor();
    } else {
        std::cout << ' ';
    }
}

inline void DrawBox(int x, int y, int w, int h) {
    SetColor(15);
    GotoXY(x, y);
    std::cout << (char)201;
    for (int i = 1; i < w - 1; i++) std::cout << (char)205;
    std::cout << (char)187;
    for (int j = 1; j < h - 1; j++) {
        GotoXY(x, y + j);
        std::cout << (char)186;
        for (int i = 1; i < w - 1; i++) std::cout << ' ';
        std::cout << (char)186;
    }
    GotoXY(x, y + h - 1);
    std::cout << (char)200;
    for (int i = 1; i < w - 1; i++) std::cout << (char)205;
    std::cout << (char)188;
    ResetColor();
}

inline void ClearArea(int x, int y, int w, int h) {
    for (int j = 0; j < h; j++) {
        GotoXY(x, y + j);
        for (int i = 0; i < w; i++) std::cout << ' ';
    }
}

inline void EnsureDir(const std::string& path) {
    _mkdir(path.c_str());
}

inline int CenterX(int textLen) {
    return (SCREEN_WIDTH - textLen) / 2;
}

// Draw a white double-line border around the entire game area (80x30)
// Border sits just OUTSIDE the game grid using the guaranteed offset space
inline void DrawScreenBorder() {
    SetColor(15);
    int x0 = -1;
    int y0 = -1;
    int x1 = SCREEN_WIDTH;
    int y1 = SCREEN_HEIGHT;

    // Top-left corner
    GotoXY(x0, y0); std::cout << (char)201;
    // Top border
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        GotoXY(x, y0); std::cout << (char)205;
    }
    // Top-right corner
    GotoXY(x1, y0); std::cout << (char)187;

    // Left and right borders
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        GotoXY(x0, y); std::cout << (char)186;
        GotoXY(x1, y); std::cout << (char)186;
    }

    // Bottom-left corner
    GotoXY(x0, y1); std::cout << (char)200;
    // Bottom border
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        GotoXY(x, y1); std::cout << (char)205;
    }
    // Bottom-right corner
    GotoXY(x1, y1); std::cout << (char)188;

    ResetColor();
}
