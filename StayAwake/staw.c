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

HINSTANCE hInstance;
SettingsT settings;

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow) {
                   
    HWND window = FindWindow(STAYAWAKE_WIN_CLASS, STAYAWAKE_WIN_NAME);
    
    if (window) {
        /*
           Another Stay Awake hInstance is running, so we quit immediately.
           Before we do that, we send a message to the running hInstance
           to make it display a notification.
        */
        PostMessage(window, WM_STAYAWAKE_NEWINSTANCE, 0, 0);
    } else {
        MSG msg;
        hInstance = hInst;
        
        loadSettings();
        createMainWinow();
        
        while (GetMessage(&msg, NULL, 0, 0) != 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        saveSettings();
    }
    return 0;
}