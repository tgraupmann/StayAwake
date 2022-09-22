/*
 *   Stay Awake - prevents Windows from starting the screen saver.
 *   Copyright (C) 2012  Micha³ Leœniewski
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "common.h"

/* Popup menu identifiers */
#define IDPM_QUIT              1
#define IDPM_TOGGLE            2
#define IDPM_ABOUT             4
#define IDPM_SETTINGS          5

HWND hMainWindow;
HMENU hTrayPopup;

void popupBalloon(TCHAR *title, TCHAR *message) {
    NOTIFYICONDATA nid;
    ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
    nid.cbSize              =   sizeof(NOTIFYICONDATA);
    nid.hWnd                =   hMainWindow;
    nid.uID                 =   0;
    nid.uFlags              =   NIF_INFO;
    lstrcpy(nid.szInfoTitle, title);
    lstrcpy(nid.szInfo, message);
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

void popupBalloonModeChange() {
    if (settings.state)
        popupBalloon(
            TEXT("StayAwake enabled"),
            TEXT("StayAwake is now preventing Windows form starting the screensaver and going to sleep or hibernation.")
        );
    else
        popupBalloon(
            TEXT("StayAwake disabled"),
            TEXT("StayAwake will now allow starting the screensaver and entering sleep or hibernation mode.")
        );
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_TIMER:
            break;
            
        case WM_COMMAND:
            switch (wParam) {
                case IDPM_QUIT:
                    DestroyWindow(hwnd);
                    break;
                case IDPM_ABOUT:
                    openAboutDlg();
                    break;
                case IDPM_SETTINGS:
                    openSettingsDlg();
                    break;
                case IDPM_TOGGLE:
                    enableStayAwake(!settings.state);
                    if (settings.balloons) popupBalloonModeChange();
                    break;
            }
            break;
            
        case WM_DESTROY: {
                NOTIFYICONDATA nid;
                ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
                nid.cbSize              =   sizeof(NOTIFYICONDATA);
                nid.hWnd                =   hwnd;
                nid.uID                 =   0;
                Shell_NotifyIcon(NIM_DELETE, &nid);
                PostQuitMessage(0);
            }
            break;
            
        case WM_CREATE: {
                NOTIFYICONDATA nid;
                hMainWindow = hwnd;
                
                enableHotkeys();
                
                hTrayPopup = CreatePopupMenu();
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_TOGGLE, TEXT("Stay Awake"));
                AppendMenu(hTrayPopup, MF_SEPARATOR, 0, NULL);
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_SETTINGS, TEXT("Settings"));
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_ABOUT, TEXT("About"));
                AppendMenu(hTrayPopup, MF_ENABLED | MF_STRING, IDPM_QUIT, TEXT("Quit"));
                
                ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
                nid.cbSize = sizeof(NOTIFYICONDATA);
                nid.uVersion = NOTIFYICON_VERSION_4;
                Shell_NotifyIcon(NIM_SETVERSION, &nid);
                
                ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
                nid.cbSize              =   sizeof(NOTIFYICONDATA);
                nid.hWnd                =   hwnd;
                nid.uID                 =   0;
                nid.uFlags              =   NIF_ICON | NIF_MESSAGE;
                nid.uCallbackMessage    =   WM_USER;
                nid.hIcon               =   LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAY_OFF));
                
                /* Check and correct the autostart entry if necessary. */
                autostartUpdate();
                
                /* According to WinApi documentation, Bill thinks that displaying an icon
                   in the tray menu is not a very important task. For this reason, the
                   system may sometimes refuse this kind of operation. Not because there
                   is an error, but because the system is just busy doing other important
                   stuff. This usually only occurs during system startup. Again, according
                   to the WinApi documentation, the best thing to do in such cases is
                   waiting some time and trying again. */
                while (!Shell_NotifyIcon(NIM_ADD, &nid)) {
                    /* Let's give Windows a whole second to let it finish its current job. */
                    Sleep(1000);
                }
                
                if (settings.state) {
                    settings.state = FALSE; /* this is necessary to fool enableStayAwake */
                    
                    /* Enable stay awake (this will also change the tray icon). */
                    enableStayAwake(TRUE);
                    
                    /* Display startup balloon */
                    if (settings.balloonsStartup)
                        popupBalloonModeChange();
                }
                
            }
            break;
            
        case WM_HOTKEY: {
                EndMenu();
                if (wParam == STAYAWAKE_HOTKEY_ID) {
                    enableStayAwake(!settings.state);
                    if (settings.balloons) popupBalloonModeChange();
                } else
                    return DefWindowProc(hwnd, msg, wParam, lParam);
            }
            break;
        case WM_USER:
            if (lParam == WM_RBUTTONUP) {
                POINT p;
                
                CheckMenuItem(
                    hTrayPopup,
                    IDPM_TOGGLE,
                    MF_BYCOMMAND | (settings.state ? MF_CHECKED : MF_UNCHECKED)
                );
                
                /*
                    The following code displays the tray menu. These magic calls
                    around TrackPopupMenuEx are really necessary. We first need to
                    bring the application window to the foreground (this is weird,
                    the window isn't visible anyway) and after the menu is displayed
                    we additionally need to post WM_NULL (the only message that
                    must be ignored by any window) to our message window. Without
                    this, the tray menu will not disappear if the user clicks
                    somewhere outside it.
                
                    Bill, why?
                */
                GetCursorPos(&p);
                SetForegroundWindow(hwnd);
                TrackPopupMenuEx(hTrayPopup, TPM_LEFTALIGN, p.x, p.y, hwnd, NULL);
                PostMessage(hwnd, WM_NULL, 0, 0);
            } else if (lParam == WM_LBUTTONDBLCLK) {
                enableStayAwake(!settings.state);
                if (settings.balloons) popupBalloonModeChange();
            }
            break;
            
        case WM_STAYAWAKE_NEWINSTANCE:
            popupBalloon(
                TEXT("StayAwake is already running"),
                TEXT("You can access StayAwake using the memory tray icon."));
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void createMainWinow() {
    WNDCLASSEX wc;
    
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = STAYAWAKE_WIN_CLASS;
    
    checkPtr((void *) RegisterClassEx(&wc), TEXT("Couldn't register window class."));
    
    checkPtr(CreateWindowEx(
                 WS_EX_CLIENTEDGE,
                 STAYAWAKE_WIN_CLASS,
                 STAYAWAKE_WIN_NAME,
                 WS_OVERLAPPEDWINDOW,
                 CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
                 HWND_MESSAGE, NULL, hInstance, NULL),
             TEXT("Couldn't create window."));
}
