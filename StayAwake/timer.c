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

UINT_PTR timerID;
BOOL ticktack = FALSE;

VOID CALLBACK timerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    if (settings.state) {
        INPUT input;
        input.type = INPUT_MOUSE;
        if (settings.safemode) {
            ticktack = !ticktack;
            input.mi.dx = ticktack ? -2 : 2;
        } else {
            input.mi.dx = 0;
        }
        input.mi.dy = input.mi.dx;
        input.mi.dwFlags = MOUSEEVENTF_MOVE;
        input.mi.mouseData = 0;
        input.mi.time = 0;
        input.mi.dwExtraInfo = 0;
        SendInput(1, &input, sizeof(input));
    }
}

void enableStayAwake(BOOL state) {
    if (settings.state != state)  {
        NOTIFYICONDATA nid;
        ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
        nid.cbSize              =   sizeof(NOTIFYICONDATA);
        nid.hWnd                =   hMainWindow;
        nid.uID                 =   0;
        nid.uFlags              =   NIF_ICON ;
        nid.hIcon               =   LoadIcon(hInstance, MAKEINTRESOURCE(state ? IDI_TRAY_ON : IDI_TRAY_OFF));
        Shell_NotifyIcon(NIM_MODIFY, &nid);
        
        if (state)
            SetTimer(NULL, timerID, 5000, timerProc);
        else
            KillTimer(NULL, timerID);
            
        settings.state = state;
        saveSettings();
    }
}