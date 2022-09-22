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

static BOOLEAN dialogVisible = FALSE;

static INT_PTR CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {
        case WM_COMMAND:
        
            switch (LOWORD(wParam)) {
                case IDOK:
                    EndDialog(hwndDlg, IDOK);
                    return TRUE;
                case IDC_DONATE:
                    ShellExecute(NULL, NULL, TEXT("http://sourceforge.net/donate/?user_id=3740414"), NULL, NULL, SW_SHOW);
                    return TRUE;
                case IDC_WEBSITE:
                    ShellExecute(NULL, NULL, TEXT("http://mlesniew.wordpress.com/stayawake"), NULL, NULL, SW_SHOW);
                    return TRUE;
                default:
                    return FALSE;
            }
        case WM_INITDIALOG:
            dialogVisible = TRUE;
            return TRUE;
        case WM_DESTROY:
            dialogVisible = FALSE;
            return TRUE;
    }
    
    if (message == WM_COMMAND && LOWORD(wParam) == IDOK) {
        EndDialog(hwndDlg, IDOK);
        return TRUE;
    } else
        return FALSE;
}

void openAboutDlg() {
    if (!dialogVisible)
        DialogBox(hInstance,
                  MAKEINTRESOURCE(IDD_ABOUT),
                  hMainWindow,
                  DlgProc);
}